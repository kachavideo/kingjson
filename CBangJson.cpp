// 2024-06-07		kacha video			Create file

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

#define BANG_SKIP_FORMAT_CHAR while(*pText == ' ' || *pText == '\r' || *pText == '\n' || *pText == '\t') pText++
#define BANG_FIND_NEXTOF_TEXT pNext = ++pText; nType = 0; while((*pNext != '\"' && *pNext != 0) || *(pNext - 1) == '\\') { if (*(pNext - 1) == '\\' && *pNext == 'u') nType = BSON_TYPE_UTF8; pNext++; } *pNext++ = 0;
#define BANG_FIND_NEXTOF_DATA pNext = pText+1; nType = 0; while (*pNext != ',' && *pNext != '}' && *pNext != ']' && *pNext != 0) {if (*(pNext - 1) == '\\' && *pNext == 'u') nType = BSON_TYPE_UTF8; pNext++; }

int CBangJson::parserData(PBSONNODE pNode, char* pData) {
    char*   pText = pData;
    char*   pNext = pData;
    char*   pName = NULL;
    bool    bList = false;
    int     nType = 0;
    int     nSize = 0;
    int     nErr = 0;
    if (m_nDeep > BSON_MAX_DEEPS)
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
                createNode(pNode, NULL, 0, pText, (int)(pNext - pText - 1), BSON_TYPE_ITEM | BSON_TYPE_TEXT | nType);
                pText = (*pNext == ',') ? pNext + 1 : pNext;
            }
            else {
                if (*pNext++ != ':') return -1;
                pName = pText; pText = pNext; nSize = (int)(pText - pName - 2);
                BANG_SKIP_FORMAT_CHAR;
                if (*pText == '\"') {
                    BANG_FIND_NEXTOF_TEXT;
                    createNode(pNode, pName, nSize, pText, (int)(pNext-pText - 1), BSON_TYPE_ITEM | BSON_TYPE_TEXT | nType);
                    pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
                else if (*pText == '{' || *pText == '[') {
                    BANG_SKIP_FORMAT_CHAR;
                    nType |= (*pText == '{' ? BSON_TYPE_NODE : BSON_TYPE_LIST);
                    PBSONNODE pNewNode = createNode(pNode, pName, nSize, NULL, 0, nType | nType);;
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
                    createNode(pNode, pName, nSize, pText, (int)(pNext - pText), BSON_TYPE_ITEM | nType);
                    pText = pNext;
                    if (*pNext == ',') {
                        *pNext = 0; pText = pNext + 1;
                    }
                }
            }
        }
        else if (*pText == '{' || *pText == '[') {
            BANG_SKIP_FORMAT_CHAR;
            nType |= (*pText == '{' ? BSON_TYPE_NODE : BSON_TYPE_LIST);
            PBSONNODE pNewNode = createNode(pNode, pName, nSize, NULL, 0, nType);;
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
            createNode(pNode, pName, nSize, pText, (int)(pNext - pText), BSON_TYPE_ITEM | nType);
            pText = pNext;
            if (*pNext == ',') {
                *pNext = 0; pText = pNext + 1;
            }
        }
    }
    return (int)(pText - pData);
}

PBSONNODE CBangJson::createNode(PBSONNODE pNode, char* pName, int nNameLen, char* pData, int nDataLen, int nType) {
    PBSONNODE pNewNode = new bsonNode();
    if (pName != NULL && nNameLen > 0) {
        if (nType & BSON_TYPE_UTF8)
            convertTxt(pNewNode->strName, pName, nNameLen);
        else
            pNewNode->strName = std::string(pName, pName + nNameLen);
    }
    if (pData != NULL && nDataLen > 0) {
        if (nType & BSON_TYPE_UTF8)
            convertTxt(pNewNode->strData, pData, nDataLen);
        else
            pNewNode->strData = std::string(pData, pData + nDataLen);
    }
    pNewNode->pParent = pNode;
    pNewNode->uFlag = nType;
    if ((nType & BSON_TYPE_NODE) || (nType & BSON_TYPE_LIST))
        m_nDeep++;
    pNode->lstNode.push_back(pNewNode);
    return pNewNode;
}

int CBangJson::convertTxt(std::string& strText, char* pData, int nSize) {
    char*       pText = pData;
    char*       pNext = pData;
    uint32_t    uWord = 0;
    uint32_t    uNext = 0;
    uint8_t     uMark = 0;
    uint8_t     uPost = 0;
    int         nUtf8 = 0;
    while (pNext - pData < nSize) {
        if (*pNext == '\\' && *(pNext + 1) == 'u') {
            uWord = convertNum(pNext+2); pNext += 6;
            if ((uWord >= 0xDC00) && (uWord <= 0xDFFF)) {
                memcpy(pText, pNext, 6); pText += 6; 
                continue;
            }
            else if ((uWord >= 0xD800) && (uWord <= 0xDBFF)) {
                uNext = convertNum(pNext + 2); pNext += 6;
                if ((uWord >= 0xDC00) && (uWord <= 0xDFFF)) {
                    memcpy(pText, pNext, 6); pText += 6;
                    continue;
                }
                uWord = 0x10000 + (((uWord & 0x3FF) << 10) | (uNext & 0x3FF));
            }
            if (uWord < 0x80) {
                nUtf8 = 1;
            }
            else if (uWord < 0x800) {
                nUtf8 = 2;
                uMark = 0xC0; // 11000000 
            }
            else if (uWord < 0x10000) {
                nUtf8 = 3;
                uMark = 0xE0; // 11100000 
            }
            else if (uWord <= 0x10FFFF) {
                nUtf8 = 4;
                uMark = 0xF0; // 11110000 
            }
            for (uPost = (nUtf8 - 1); uPost > 0; uPost--) {
                pText[uPost] = (char)((uWord | 0x80) & 0xBF);
                uWord >>= 6;
            }
            if (nUtf8 > 1) 
                pText[0] = (char)((uWord | uMark) & 0xFF);
            else 
                pText[0] = (char)(uWord & 0x7F);
            pText += nUtf8;
        }
        else {
            *pText++ = *pNext++;
        }
    }
    strText = std::string(pData, pText);
    return 1;
}

uint32_t CBangJson::convertNum(const char* pText) {
    uint32_t uText = *(uint32_t*)pText;
    uint32_t uWord = 0;
    if ((uText & 0XFF000000) >= 0X30000000 && (uText & 0XFF000000) <= 0X39000000)
        uWord += (((uText & 0XFF000000) - 0X30000000) >> 24);
    else if ((uText & 0XFF000000) >= 0X41000000 && (uText & 0XFF000000) <= 0X6000000)
        uWord += (((uText & 0XFF000000) - 0X37000000) >> 24);
    else if ((uText & 0XFF000000) >= 0X61000000 && (uText & 0XFF000000) <= 0X66000000)
        uWord += (((uText & 0XFF000000) - 0X57000000) >> 24);
    if ((uText & 0XFF0000) >= 0X300000 && (uText & 0XFF0000) <= 0X390000)
        uWord += (((uText & 0XFF0000) - 0X300000) >> 12);
    else if ((uText & 0XFF0000) >= 0X410000 && (uText & 0XFF0000) <= 0X600000)
        uWord += (((uText & 0XFF0000) - 0X370000) >> 12);
    else if ((uText & 0XFF0000) >= 0X610000 && (uText & 0XFF0000) <= 0X660000)
        uWord += (((uText & 0XFF0000) - 0X570000) >> 12);
    if ((uText & 0XFF00) >= 0X3000 && (uText & 0XFF00) <= 0X3900)
        uWord += (((uText & 0XFF00) - 0X3000) >> 0);
    else if ((uText & 0XFF00) >= 0X4100 && (uText & 0XFF00) <= 0X6000)
        uWord += (((uText & 0XFF00) - 0X3700) >> 0);
    else if ((uText & 0XFF00) >= 0X6100 && (uText & 0XFF00) <= 0X6600)
        uWord += (((uText & 0XFF00) - 0X5700) >> 0);
    if ((uText & 0XFF) >= 0X30 && (uText & 0XFF) <= 0X39)
        uWord += (((uText & 0XFF) - 0X30) << 12);
    else if ((uText & 0XFF) >= 0X41 && (uText & 0XFF) <= 0X60)
        uWord += (((uText & 0XFF) - 0X37) << 12);
    else if ((uText & 0XFF) >= 0X61 && (uText & 0XFF) <= 0X66)
        uWord += (((uText & 0XFF) - 0X57) << 12);
    return uWord;
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