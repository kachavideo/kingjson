// 2024-06-07		kacha video			Create file

#include "CBangJson.h"

CBangJson::CBangJson(void) {}

CBangJson::~CBangJson(void) {
    if (m_pRoot != NULL)
        deleteNode(m_pRoot);
}

PBSONNODE CBangJson::Parser(const char* pData, int nSize, int nFlag) {
    if (pData == NULL || nSize <= 2) return NULL;
    if (m_pRoot != NULL) Delete(m_pRoot);
    m_pRoot = new bsonNode();
    char* pText = new char[nSize + 1];
    memcpy(pText, pData, nSize);
    pText[nSize] = 0;
    m_nDeep = 1;
    int nErr = parserData(m_pRoot, pText);
    delete[]pText;
    return (nErr > 0 && m_nDeep == 0) ? m_pRoot : NULL;
}

PBSONNODE CBangJson::Search(PBSONNODE pNode, const char* pName, bool bChild) {
    if (pNode == NULL || pName == NULL)
        return NULL;
    return searchNode(pNode, pName, bChild);
}

const char* CBangJson::Format(PBSONNODE pNode, int* pSize) {
    if (pNode == NULL) return NULL;
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

#define BANG_SKIP_FORMAT_CHAR while(*pText == ' ' || *pText == '\r' || *pText == '\n' || *pText == '\t') pText++
#define BANG_FIND_NEXTOF_TEXT pNext = ++pText; while((*pNext != '\"' && *pNext != 0) || *(pNext - 1) == '\\') { pNext++; } *pNext++ = 0;
#define BANG_FIND_NEXTOF_DATA pNext = pText+1; while (*pNext != ',' && *pNext != '}' && *pNext != ']' && *pNext != 0) { pNext++; }

int CBangJson::parserData(PBSONNODE pNode, char* pData) {
    char*   pText = pData;
    char*   pNext = pData;
    char*   pName = NULL;
    bool    bList = false;
    int     nErr = 0;
    if (m_nDeep > 2000)
        return -1;
    BANG_SKIP_FORMAT_CHAR;
    if (*pText != '{' && *pText != '[')
        return -1;
    bList = *pText++ == '[' ? true : false;
    while (*pText != 0) {
        BANG_SKIP_FORMAT_CHAR;
        if (*pText == '\"') {
            BANG_FIND_NEXTOF_TEXT;
            if (bList) {
                createNode(pNode, NULL, pText, 0, BSON_TYPE_ITEM | BSON_TYPE_TEXT);
                pText = (*pNext == ',') ? pNext + 1 : pNext;
            }
            else {
                if (*pNext++ != ':') return -1;
                pName = pText; pText = pNext;
                BANG_SKIP_FORMAT_CHAR;
                if (*pText == '\"') {
                    BANG_FIND_NEXTOF_TEXT;
                    createNode(pNode, pName, pText, 0, BSON_TYPE_ITEM | BSON_TYPE_TEXT);
                    pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
                else if (*pText == '{' || *pText == '[') {
                    BANG_SKIP_FORMAT_CHAR;
                    int nType = *pText == '{' ? BSON_TYPE_NODE : BSON_TYPE_LIST;
                    PBSONNODE pNewNode = createNode(pNode, pName, NULL, 0, nType);;
                    nErr = parserData(pNewNode, pText);
                    if (nErr < 0)
                        return nErr;
                    pText += nErr;
                }
                else if (*pText == '}' || *pText == ']') {
                    m_nDeep--; *pText = 0;
                    return (int)((pText - pData) + (*(pText + 1) == ',' ? 2 : 1));
                }
                else {
                    BANG_SKIP_FORMAT_CHAR;
                    BANG_FIND_NEXTOF_DATA;
                    createNode(pNode, pName, pText, (int)(pNext - pText), BSON_TYPE_ITEM);
                    pText = pNext;
                    if (*pNext == ',') {
                        *pNext = 0; pText = pNext + 1;
                    }
                }
            }
        }
        else if (*pText == '{' || *pText == '[') {
            BANG_SKIP_FORMAT_CHAR;
            int nType = *pText == '{' ? BSON_TYPE_NODE : BSON_TYPE_LIST;
            PBSONNODE pNewNode = createNode(pNode, pName, NULL, 0, nType);;
            nErr = parserData(pNewNode, pText);
            if (nErr <= 0)
                return -1;
            pText += nErr;
        }
        else if (*pText == '}' || *pText == ']') {
            m_nDeep--; *pText = 0;
            return (int)((pText - pData) + (*(pText + 1) == ',' ? 2 : 1));
        }
        else {
            BANG_SKIP_FORMAT_CHAR;
            BANG_FIND_NEXTOF_DATA;
            createNode(pNode, pName, pText, (int)(pNext - pText), BSON_TYPE_ITEM);
            pText = pNext;
            if (*pNext == ',') {
                *pNext = 0; pText = pNext + 1;
            }
        }
    }
    return (int)(pText - pData);
}

PBSONNODE CBangJson::createNode(PBSONNODE pNode, char* pName, char* pData, int nSize, uint32_t uType) {
    PBSONNODE pNewNode = new bsonNode();
    if (pName != NULL)
        pNewNode->strName = pName;
    if (pData != NULL)
        pNewNode->strData = nSize == 0 ? pData : std::string(pData, pData + nSize);
    pNewNode->pParent = pNode;
    pNewNode->uFlag = uType;
    if (uType & BSON_TYPE_NODE || uType & BSON_TYPE_LIST)
        m_nDeep++;
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
    if (!bChild)
        return NULL;
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