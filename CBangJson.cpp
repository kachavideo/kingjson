// 2024-06-07		kacha video			Create file
#include "string.h"
#include "CBangJson.h"

CBangJson::CBangJson(void) {}

CBangJson::~CBangJson(void) {
    if (m_pRoot != NULL)
        deleteNode(m_pRoot);
}

PBSONNODE CBangJson::Parser(const char* pData, int nSize, int nFlag) {
    if (pData == NULL || nSize < 2) return NULL;
    if (m_pRoot != NULL) Delete(m_pRoot);
    m_pRoot = new bsonNode();
    char* pText = new char[nSize + 1];
    memcpy(pText, pData, nSize);
    pText[nSize] = 0;
    int nErr = parserData(m_pRoot, pText, nSize);
    delete[]pText;
    return nErr > 0 ? m_pRoot : NULL;
}

PBSONNODE CBangJson::Search(PBSONNODE pNode, const char* pName, bool bChild) {
    if (pNode == NULL || pName == NULL)
        return NULL;
    return searchNode(pNode, pName, bChild);
}

const char* CBangJson::Format(PBSONNODE pNode, int* pSize) {
    if (pNode == NULL) return NULL;
    m_sText = "";
    formatJson(pNode);
    if (pSize != NULL)
        *pSize = m_sText.length();
    return m_sText.c_str();
}

int CBangJson::Delete(PBSONNODE pNode) {
    if (pNode == NULL) return -1;
    if (pNode->pParent != NULL) {
        for (auto iter = pNode->pParent->lstNode.begin(); iter != pNode->pParent->lstNode.end(); iter++) {
            if (*iter == pNode) {
                pNode->pParent->lstNode.erase(iter);
                break;
            }
        }
    }
    deleteNode(pNode);
    return 1;
}

#define BANG_SKIP_FORMAT_CHAR while(*pText <= ' ' && *pText != 0) pText++
#define BANG_FIND_NEXTOF_TEXT pNext = ++pText; while((*pNext != '\"' && *pNext != 0) || *(pNext - 1) == '\\') pNext++;
#define BANG_FIND_NEXTOF_DATA pNext = pText+1; while (*pNext != ',' && *pNext != '}' && *pNext != ']' && *pNext != 0) pNext++;

int CBangJson::parserData(PBSONNODE pNode, char* pData, int nSize) {
    int   nDeep = 0;
    int   nName = 0;
    char* pName = NULL;
    char* pLast = pData + nSize;
    char* pText = pData;
    char* pNext = pData;
    BANG_SKIP_FORMAT_CHAR;
    if (*pText != '{' && *pText != '[') return -1;
    pNode->uFlag = *pText++ == '[' ? BSON_TYPE_LIST : BSON_TYPE_NODE;
    while (pText < pLast) {
        BANG_SKIP_FORMAT_CHAR;
        if (*pText == '{' || *pText == '[') {
            if (nDeep++ > BSON_MAX_DEEPS) return -1;
            pNode = createNode(pNode, pName, nName, NULL, 0, *pText++ == '[' ? BSON_TYPE_LIST : BSON_TYPE_NODE);
            if (pNode == NULL) return -1;
        }
        else if (*pText == '}' || *pText == ']') {
            if (pNode->pParent == NULL) break;
            *pText++ = 0; if (*pText == ',') pText++;
            pNode = pNode->pParent; nDeep--;
        }
        else if (*pText == '"') {
            BANG_FIND_NEXTOF_TEXT;
            nName = (int)(pNext - pText); *pNext++ = 0;
            if (pNode->uFlag & BSON_TYPE_LIST) {
                createNode(pNode, NULL, 0, pText, nName, BSON_TYPE_ITEM | BSON_TYPE_TEXT);
                pText = (*pNext == ',') ? pNext + 1 : pNext;
            }
            else {
                pName = pText; pText = pNext + 1;
                while (*pText <= 0X20) pText++;
                if (*pText == '"') {
                    BANG_FIND_NEXTOF_TEXT;
                    createNode(pNode, pName, nName, pText, (int)(pNext - pText), BSON_TYPE_ITEM | BSON_TYPE_TEXT);
                    *pNext++ = 0; pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
            }
        }
        else {
            BANG_FIND_NEXTOF_DATA;
            createNode(pNode, pName, nName, pText, (int)(pNext - pText), BSON_TYPE_ITEM | BSON_TYPE_DATA);
            if (*pNext == ',' || *pNext <= ' ')
                *pNext++ = 0;
            pText = pNext;
        }
    }
    return (int)(pText - pData);
}

PBSONNODE CBangJson::createNode(PBSONNODE pNode, char* pName, int nNameLen, char* pData, int nDataLen, int nType) {
    PBSONNODE pNewNode = new bsonNode();
    if (((pNode->uFlag & BSON_TYPE_LIST) == 0) && pName != NULL && nNameLen > 0)
         pNewNode->strName = std::string(pName, pName + nNameLen);
    if (pData != NULL && nDataLen > 0) 
        pNewNode->strData = std::string(pData, pData + nDataLen);
    pNewNode->pParent = pNode;
    pNewNode->uFlag = nType;
    pNode->lstNode.push_back(pNewNode);
    return pNewNode;
}

PBSONNODE CBangJson::searchNode(PBSONNODE pNode, const char* pName, bool bChild) {
    if (pNode->strName == pName)
        return pNode;
    for (auto& iter : pNode->lstNode) {
        if (iter->strName == pName)
            return iter;
    }
    if (!bChild) return NULL;
    PBSONNODE pFind = NULL;
    for (auto& iter : pNode->lstNode) {
        pFind = searchNode(iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    return NULL;
}

int CBangJson::formatJson(PBSONNODE pNode) {
    if (pNode->strName.length() > 0)
        m_sText += "\"" + pNode->strName + "\":";
    if (pNode->uFlag & BSON_TYPE_ITEM) {
        if (pNode->uFlag & BSON_TYPE_TEXT)
            m_sText += "\"" + pNode->strData + "\"";
        else
            m_sText += pNode->strData;
        if (pNode->pParent->lstNode.back() != pNode)
            m_sText += ",";
    }
    if (pNode->uFlag & BSON_TYPE_NODE || pNode->uFlag & BSON_TYPE_LIST) {
        m_sText += (pNode->uFlag & BSON_TYPE_NODE) ? "{" : "[";
        for (auto& iter : pNode->lstNode)
            formatJson(iter);
        m_sText += (pNode->uFlag & BSON_TYPE_NODE) ? "}" : "]";
        if (pNode->pParent != NULL && pNode->pParent->lstNode.back() != pNode)
            m_sText += ",";
    }
    return 1;
}

int CBangJson::deleteNode(PBSONNODE pNode) {
    for (auto & iter : pNode->lstNode) 
        deleteNode(iter);
    delete pNode;
    return 1;
}
