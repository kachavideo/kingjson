/*******************************************************************************
    File:		CKingJson.cpp

    Contains:	the king json class implement code

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#include "CKingJson.h"

CKingJson::CKingJson(void) {
    m_kjsRoot.pParent = NULL;
}

CKingJson::~CKingJson(void) {
    relaseNode(&m_kjsRoot);
}

int CKingJson::ParseData(const char* pData, int nSize, int nFlag) {
    relaseNode(&m_kjsRoot);

    char* pText = new char[nSize];
    removeChar(pData, pText, nSize);
    nSize = strlen(pText);
    int nRC = parseJson(&m_kjsRoot, pText);
    if (nFlag == 1) {
        if (nSize != nRC)
            nRC = -1;
    }
    delete[]pText;
    return nRC;
}

int CKingJson::OpenFileA(const char* pFile, int nFlag) {
    std::ifstream inFile(pFile, std::ios_base::binary);
    if (!inFile.good())
        return -1;
    return openFile(&inFile, nFlag);
}

int CKingJson::OpenFileW(const wchar_t* pFile, int nFlag) {
    std::ifstream inFile(pFile, std::ios_base::binary);
    if (!inFile.good())
        return -1;
    return openFile(&inFile, nFlag);
}

PKINGJSON CKingJson::FindNode(PKINGJSON pNode, const char* pName, bool bChild) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    return findNode(pNode, pName, bChild);
}

PKINGJSON CKingJson::FindItem(PKINGJSON pNode, const char* pName, bool bChild) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    return findItem(pNode, pName, bChild);
}

PKINGJSON CKingJson::GetItem(PKINGJSON pNode, int nIndex) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    if (nIndex < 0 || nIndex >= pNode->lstItem.size())
        return NULL;
    return pNode->lstItem.at(nIndex);
}

const char* CKingJson::GetValue(PKINGJSON pNode, const char* pName) {
    return GetValue(pNode, pName, NULL);
}

const char* CKingJson::GetValue(PKINGJSON pNode, const char* pName, const char* pDefault) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    PKINGJSON pItem = findItem(pNode, pName, false);
    return pItem != NULL ? pItem->strData.c_str() : pDefault;
}

int CKingJson::GetValueInt(PKINGJSON pNode, const char* pName, int nDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? nDefault : atoi(pValue);
}

double CKingJson::GetValueDbl(PKINGJSON pNode, const char* pName, double dDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? dDefault : atof(pValue);
}

long long CKingJson::GetValueLng(PKINGJSON pNode, const char* pName, long long lDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? lDefault : atol(pValue);
}

int CKingJson::GetItemNum(PKINGJSON pNode) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    return pNode->lstItem.size();
}

const char* CKingJson::GetItemTxt(PKINGJSON pNode, int nIndex, const char* pDefault) {
    PKINGJSON pItem = GetItem(pNode, nIndex);
    return pItem == NULL ? pDefault : pItem->strData.c_str();
}

int CKingJson::GetItemInt(PKINGJSON pNode, int nIndex, int nDefault) {
    PKINGJSON pItem = GetItem(pNode, nIndex);
    return pItem == NULL ? nDefault : atoi(pItem->strData.c_str());
}

double CKingJson::GetItemDbl(PKINGJSON pNode, int nIndex, double dDefault) {
    PKINGJSON pItem = GetItem(pNode, nIndex);
    return pItem == NULL ? dDefault : atof(pItem->strData.c_str());
}

long long CKingJson::GetItemLng(PKINGJSON pNode, int nIndex, long long lDefault) {
    PKINGJSON pItem = GetItem(pNode, nIndex);
    return pItem == NULL ? lDefault : atol(pItem->strData.c_str());
}

PKINGJSON CKingJson::AddNode(PKINGJSON pNode, const char* pName, bool bList) {
    return apendNode(pNode, pName, bList);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, const char* pValue) {
    return apendItem(pNode, pName, pValue, true);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, int nValue) {
    return apendItem(pNode, pName, std::to_string(nValue).c_str(), false);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, double dValue) {
    return apendItem(pNode, pName, std::to_string(dValue).c_str(), false);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, long long lValue) {
    return apendItem(pNode, pName, std::to_string(lValue).c_str(), false);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, int nIndex, const char* pValue, bool bString) {
    if (pValue == NULL)
        return NULL;
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    PKINGJSON pNewItem = new kingJson();
    pNewItem->pParent = pNode;
    pNewItem->strData = pValue;
    pNewItem->bString = bString;
    for (auto iter = pNode->lstItem.begin(); iter != pNode->lstItem.end(); iter++) {
        if (nIndex-- <= 0) {
            pNode->lstItem.insert(iter, pNewItem);
            break;
        }
    }
    if (nIndex > 0) 
        pNode->lstItem.push_back(pNewItem);
    return pNewItem;
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, int nIndex, int nValue) {
    return AddItem(pNode, nIndex, std::to_string(nValue).c_str(), false);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, int nIndex, double dValue) {
    return AddItem(pNode, nIndex, std::to_string(dValue).c_str(), false);
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, int nIndex, long long lValue) {
    return AddItem(pNode, nIndex, std::to_string(lValue).c_str(), false);
}

int CKingJson::DelNode(PKINGJSON pNode, const char* pName, bool bList) {
    PKINGJSON pFind = findNode(pNode, pName, false);
    if (pFind == NULL)
        return -1;
    relaseNode(pFind);
    std::vector<kingJson*>* pList = bList ? &pNode->lstList : &pNode->lstNode;
    for (auto iter = pList->begin(); iter != pList->end(); iter++) {
        if ((*iter)== pFind) {
            pList->erase(iter);
            break;
        }
    }
    delete pFind;
    return 1;
}

int CKingJson::DelItem(PKINGJSON pNode, const char* pName) {
    if (pName == NULL)
        return -1;
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    std::vector<kingJson*>* pList = &pNode->lstItem;
    for (auto iter = pList->begin(); iter != pList->end(); iter++) {
        if ((*iter)->strName == pName) {
            pList->erase(iter);
            delete (*iter);
            return 1;
        }
    }
    return 0;
}

int CKingJson::DelItem(PKINGJSON pNode, int nIndex) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    if (pNode->lstItem.empty())
        return -1;
    for (auto iter = pNode->lstItem.begin(); iter != pNode->lstItem.end(); iter++) {
        if (nIndex-- <= 0) {
            pNode->lstItem.erase(iter);
            break;
        }
    }
    if (nIndex > 0)
        pNode->lstItem.pop_back();
    return 0;
}

int CKingJson::DelNode(PKINGJSON pNode) {
    if (pNode == NULL || pNode->pParent == NULL)
        return 0;
    relaseNode(pNode);
    delNode(pNode, &pNode->pParent->lstNode);
    delNode(pNode, &pNode->pParent->lstList);
    return 1;
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, const char* pName, const char* pValue) {
    PKINGJSON pFind = findItem(pNode, pName, false);
    if (pFind != NULL) 
        pFind->strData = pValue;
    return pFind;
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, const char* pName, int nValue) {
    return ModItem(pNode, pName, std::to_string(nValue).c_str());
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, const char* pName, double dValue) {
    return ModItem(pNode, pName, std::to_string(dValue).c_str());
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, const char* pName, long long lValue) {
    return ModItem(pNode, pName, std::to_string(lValue).c_str());
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, int nIndex, const char* pValue) {
    PKINGJSON pFind = GetItem(pNode, nIndex);
    if (pFind != NULL) 
        pFind->strData = pValue;
    return pFind;
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, int nIndex, int nValue) {
    return ModItem(pNode, nIndex, std::to_string(nValue).c_str());
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, int nIndex, double dValue) {
    return ModItem(pNode, nIndex, std::to_string(dValue).c_str());
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, int nIndex, long long lValue) {
    return ModItem(pNode, nIndex, std::to_string(lValue).c_str());
}

const char* CKingJson::FormatText(void) {
    m_strText = "";
    formatText(&m_kjsRoot, false, true);
    return m_strText.c_str();
}

int CKingJson::SaveToFile(const char* pFile) {
    std::ofstream outFile(pFile, std::ios_base::binary);
    if (!outFile.good())
        return -1;
    FormatText();
    int nUtf8 = 0X00BFBBEF;
    outFile.write((const char *)&nUtf8, 3);
    outFile.write(m_strText.c_str(), m_strText.length());
    return m_strText.length();;
}

int CKingJson::openFile(std::ifstream* pFileStream, int nFlag) {
    pFileStream->seekg(0, pFileStream->end);
    size_t lSize = pFileStream->tellg();
    pFileStream->seekg(0, pFileStream->beg);
    char* pData = new char[lSize + 1];
    pFileStream->read(pData, lSize);
    pFileStream->close();
    pData[lSize] = 0;
    char* pBuff = pData;
    if (pData[0] == (char)0XEF && pData[1] == (char)0XBB && pData[2] == (char)0XBF) {
        pBuff += 3;
        lSize -= 3;
    }
    int nErr = ParseData(pBuff, lSize, nFlag);
    delete[]pData;
    return nErr;
}

PKINGJSON CKingJson::findNode(PKINGJSON pNode, const char* pName, bool bChild) {
    if (pName == NULL)
        return NULL;
    for (auto& iter : pNode->lstNode) {
        if (iter->strName == pName)
            return iter;
    }
    for (auto& iter : pNode->lstList) {
        if (iter->strName == pName)
            return iter;
    }
    if (!bChild)
        return NULL;
    PKINGJSON pFind = NULL;
    for (auto& iter : pNode->lstNode) {
        pFind = findNode(iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    for (auto& iter : pNode->lstList) {
        pFind = findNode(iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    return NULL;
}

PKINGJSON CKingJson::findItem(PKINGJSON pNode, const char* pName, bool bChild) {
    if (pName == NULL)
        return NULL;
    for (auto& iter : pNode->lstItem) {
        if (iter->strName == pName)
            return iter;
    }
    if (!bChild)
        return NULL;
    PKINGJSON pFind = NULL;
    for (auto& iter : pNode->lstItem) {
        pFind = findItem(iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    return NULL;
}

int CKingJson::delNode(PKINGJSON pNode, std::vector<kingJson*> * pList) {
    int i = 0;
    for (auto iter = pList->begin(); iter != pList->end(); iter++) {
        if (pList->at(i++) == pNode) {
            pList->erase(iter);
            delete pNode;
            return 1;
        }
    }
    return -1;
}

int CKingJson::parseJson(PKINGJSON pNode, char* pData) {
    char*       pText = pData;
    char*       pNext = pData;
    char*       pName = NULL;
    int         nErr  = 0;
    if (*pText != '{' && *pText != '[')
        return -1;
    bool bList = *pText == '[' ? true : false;
    pText++;
    while (*pText != 0) {
        if (*pText == '\"') {
            nErr = parseText(pText, &pNext);
            if (nErr <= 0)
                return -1;
            pText++;
            if (bList) {
                apendItem(pNode, NULL, pText, true);
                pText = (*pNext == ',') ? pNext + 1 : pNext;
            }
            else {
                if (*pNext++ != ':')
                    return -1;
                pName = pText;
                pText = pNext;
                if (*pText == '\"') {
                    nErr = parseText(pText, &pNext);
                    if (nErr <= 0)
                        return -1;
                    apendItem(pNode, pName, ++pText, true);
                    pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
                else if (*pText == '{' || *pText == '[') {
                    PKINGJSON pNewNode = apendNode(pNode, pName, *pText == '[');
                    nErr = parseJson(pNewNode, pText);
                    if (nErr < 0)
                        return nErr;
                    pText += nErr;
                }
                else if (*pText == '}' || *pText == ']') {
                    return (pText - pData) + (*(pText + 1) == ',' ? 2 : 1);
                }
                else {
                    nErr = parseData(pText, &pNext);
                    if (nErr <= 0)
                        return -1;
                    char cChar = *pNext; *pNext = 0;
                    apendItem(pNode, pName, pText, false);
                    *pNext = cChar;
                    pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
            }
        }
        else if (*pText == '{' || *pText == '[') {
            PKINGJSON pNewNode = apendNode(pNode, pName, *pText == '[');
            nErr = parseJson(pNewNode, pText);
            if (nErr <= 0)
                return -1;
            pText += nErr;
        }
        else if (*pText == '}' || *pText == ']') {
            return (pText - pData) + (*(pText + 1) == ',' ? 2 : 1);
        }
        else {
            nErr = parseData(pText, &pNext);
            if (nErr <= 0)
                return -1;
            char cChar = *pNext; *pNext = 0;
            apendItem(pNode, pName, pText, false);
            *pNext = cChar;
            pText = (*pNext == ',') ? pNext + 1 : pNext;
        }
    }
    return 0;
}

int CKingJson::parseText(char* pText, char** ppNext) {
    *ppNext = NULL;
    char* pNext = ++pText;
    while (*pNext != 0) {
        if (*(pNext - 1) != '\\' && *pNext == '\"') {
            *pNext++ = 0; *ppNext = pNext;
            break;
        }
        pNext++;
    }
    return *ppNext == NULL ? 0 : pNext - pText;
}

int CKingJson::parseData(char* pText, char** ppNext) {
    *ppNext = NULL;
    char* pNext = pText;
    while (*pNext++ != 0) {
        if (*pNext == ',' || *pNext == '}' || *pNext == ']') {
            *ppNext = pNext;
            break;
        }
    }
    return *ppNext == NULL? 0 : pNext - pText;
}

PKINGJSON CKingJson::apendItem(PKINGJSON pNode, const char* pName, const char* pData, bool bString) {
    PKINGJSON pNewItem = new kingJson();
    pNewItem->pParent = pNode;
    if (pName != NULL) 
        pNewItem->strName = pName;
    if (isNodeList(pNode))
        pNewItem->strName = "";
    pNewItem->bString = bString;
    if (bString) {
        const char* pMark = strstr(pData, "\\\"");
        if (pMark == NULL) {
            pNewItem->strData = pData;
        }
        else {
            while (*pData != NULL) {
                if (*pData == '\\' && *(pData + 1) == '\"') {
                    pNewItem->strData += "\""; pData += 2;
                    continue;
                }
                pNewItem->strData += *pData++;
            }
        }
    }
    else {
        pNewItem->strData = pData;
    }
    pNode->lstItem.push_back(pNewItem);
    return pNewItem;
}

PKINGJSON CKingJson::apendNode(PKINGJSON pNode, const char* pName, bool bList) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    PKINGJSON pNewNode = new kingJson();
    pNewNode->pParent = pNode;
    pNewNode->bString = false;
    if (pName != NULL)
        pNewNode->strName = pName;
    if (!bList) {
        pNode->lstNode.push_back(pNewNode);
    }
    else {
        pNode->lstList.push_back(pNewNode);
    }
    return pNewNode;
}

int CKingJson::formatText(PKINGJSON pNode, bool bList, bool bLast) {
    int nLength = m_strText.length();
    int nNodeSize = pNode->lstList.size() + pNode->lstNode.size();
    if (pNode->strName.length() > 0) {
        m_strText += "\"";
        m_strText += pNode->strName;
        m_strText += "\":";
    }
    m_strText += bList ? "[" : "{";
    for (auto & iter : pNode->lstItem) {
        if (iter->strName.length() > 0) {
            m_strText += "\"";
            m_strText += iter->strName;
            m_strText += "\":";
        }
        if (iter->strData.length() > 0) {
            if (iter->bString)
                m_strText += "\"";
            if (strchr(iter->strData.c_str(), '\"') == NULL) {
                m_strText += iter->strData;
            }
            else {
                for (int i = 0; i < iter->strData.length(); i++) {
                    if (iter->strData.at(i) == '\"')
                        m_strText += "\\\"";
                    else
                        m_strText += iter->strData.at(i);
                }
            }
            if (iter->bString)
                m_strText += "\"";
        }
        else {
            m_strText += "\"\"";
        }
        if (nNodeSize > 0 || iter != pNode->lstItem.back())
            m_strText += ",";
    }
    for (auto & iter : pNode->lstNode) {
        if (iter == pNode->lstNode.back()) {
            if (pNode->lstList.size() > 0)
                formatText(iter, false, false);
            else
                formatText(iter, false, true);
        }
        else
            formatText(iter, false, false);
    }
    for (auto& iter : pNode->lstList) {
        if (iter == pNode->lstList.back())
            formatText(iter, true, true);
        else
            formatText(iter, true, false);
    }
    m_strText += bList ? "]" : "}";
    if (!bLast)
        m_strText += ",";
    return m_strText.length() - nLength;
}

int CKingJson::removeChar(const char* pData, char* pText, int nSize) {
    char* pDst = pText;
    char* pSrc = (char*)pData;
    if (nSize > 0)
       ((char*)pData)[nSize] = 0;
    while (*pSrc != 0) {
        if (*pSrc == '\"') {
            *pDst++ = *pSrc++;
            while (*pSrc != 0) {
                if (*pSrc == '\"' && *(pSrc - 1) != '\\') 
                    break;
                *pDst++ = *pSrc++;
            }
        }
        if (*pSrc == ' ' || *pSrc == '\r' || *pSrc == '\n' || *pSrc == '\t') {
            pSrc++;
            continue;
        }
        *pDst++ = *pSrc++;
    }
    *pDst = 0;
    return pDst - pText;
}

bool CKingJson::isNodeList(PKINGJSON pNode) {
    if (pNode->pParent == NULL)
        return false;
    for (auto& iter : pNode->pParent->lstList) {
        if (iter == pNode)
            return true;
    }
    return false;
}

int CKingJson::relaseNode(PKINGJSON pNode) {
    for (auto& item : pNode->lstItem)
        delete item;
    pNode->lstItem.clear();
    for (auto& iter : pNode->lstNode) {
        relaseNode(iter);
        delete iter;
    }
    pNode->lstNode.clear();
    for (auto& iter : pNode->lstList) {
        relaseNode(iter);
        delete iter;
    }
    pNode->lstList.clear();
    return 0;
}
