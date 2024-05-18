/*******************************************************************************
    File:		CKingJson.cpp

    Contains:	the king json class implement code

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#include <fstream>
#include "CKingJson.h"

CKingJson::CKingJson(void) {
    m_kjsRoot.pParent = NULL;
}

CKingJson::~CKingJson(void) {
}

int CKingJson::ParseFile(const char* pFile, int nFlag) {
    std::ifstream inFile(pFile, std::ios_base::binary);
    if (!inFile.good())
        return -1;
    inFile.seekg(0, inFile.end);
    size_t lSize = inFile.tellg();
    inFile.seekg(0, inFile.beg);
    char* pData = new char[lSize + 1];
    inFile.read(pData, lSize);
    pData[lSize] = 0;
    char* pBuff = pData;
    if (pData[0] == (char)0XEF && pData[1] == (char)0XBB && pData[2] == (char)0XBF) {
        pBuff += 3;
        lSize -= 3;
    }
    int nErr = ParseData(pBuff, lSize);
    delete[]pData;
    return nErr;
}

int CKingJson::ParseData(const char* pData, int nSize) {
    m_kjsRoot.lstItem.clear();
    m_kjsRoot.lstNode.clear();
    m_kjsRoot.lstList.clear();

    char* pText = new char[nSize];
    memset(pText, 0, nSize);
    char* pDst  = pText;
    char* pSrc  = (char *)pData;
    while (pSrc - pData < nSize) {
        if (*pSrc == '\"') {
            *pDst++ = *pSrc++;
            while (pSrc - pData < nSize) {
                if (*pSrc == '\"' && *(pSrc - 1) != '\\') {
                    break;
                }
                *pDst++ = *pSrc++;
            }
        }
        if (*pSrc == ' ' || *pSrc == '\r' || *pSrc == '\n' || *pSrc == '\t') {
            pSrc++;
            continue;
        }
        *pDst++ = *pSrc++;
    }
    nSize = pDst - pText;
    int nRC = parseNode(&m_kjsRoot, pText, nSize, false);
    delete[]pText;
    return nRC;
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

const char* CKingJson::GetValue(PKINGJSON pNode, const char* pName) {
    return GetValue(pNode, pName, NULL);
}

const char* CKingJson::GetValue(PKINGJSON pNode, const char* pName, const char* pDefault) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    PKINGJSON pItem = findItem(pNode, pName, false);
    if (pItem != NULL)
        return pItem->strData.c_str();
    return pDefault;
}

int CKingJson::GetValueInt(PKINGJSON pNode, const char* pName, int nDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    if (pValue == NULL)
        return nDefault;
    return atoi(pValue);
}

double CKingJson::GetValueDbl(PKINGJSON pNode, const char* pName, double dDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    if (pValue == NULL)
        return dDefault;
    return atof(pValue);
}

long long CKingJson::GetValueLng(PKINGJSON pNode, const char* pName, long long lDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    if (pValue == NULL)
        return lDefault;
    return atol(pValue);
}

int CKingJson::GetItemNum(PKINGJSON pNode) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    return pNode->lstItem.size();
}

const char* CKingJson::GetItemTxt(PKINGJSON pNode, int nIndex, const char* pDefault) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    if (nIndex < 0 || nIndex >= pNode->lstItem.size())
        return pDefault;
    return pNode->lstItem.at(nIndex).strData.c_str();
}

int CKingJson::GetItemInt(PKINGJSON pNode, int nIndex, int nDefault) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    if (nIndex < 0 || nIndex >= pNode->lstItem.size())
        return nDefault;
    return atoi(pNode->lstItem.at(nIndex).strData.c_str());
}

double CKingJson::GetItemDbl(PKINGJSON pNode, int nIndex, double dDefault) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    if (nIndex < 0 || nIndex >= pNode->lstItem.size())
        return dDefault;
    return atof(pNode->lstItem.at(nIndex).strData.c_str());
}

long long CKingJson::GetItemLng(PKINGJSON pNode, int nIndex, long long lDefault) {
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    if (nIndex < 0 || nIndex >= pNode->lstItem.size())
        return lDefault;
    return atol(pNode->lstItem.at(nIndex).strData.c_str());
}

PKINGJSON CKingJson::AddNode(PKINGJSON pNode, const char* pName, bool bList) {
    if (pName == NULL)
        return NULL;
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    kingJson kjsNode;
    kjsNode.pParent = pNode;
    kjsNode.strName = pName;
    if (bList) {
        pNode->lstList.push_back(kjsNode);
        return &pNode->lstList.back();
    }
    else {
        pNode->lstNode.push_back(kjsNode);
        return &pNode->lstNode.back();
    }
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, const char* pValue) {
    if (pName == NULL || pValue == NULL)
        return NULL;
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    kingJson kjsItem;
    kjsItem.pParent = pNode;
    if (pName != NULL)
        kjsItem.strName = pName;
    if (pValue != NULL)
        kjsItem.strData = pValue;
    pNode->lstItem.push_back(kjsItem);
    return &pNode->lstItem.back();
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, int nValue) {
    return AddItem(pNode, pName, std::to_string(nValue).c_str());
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, double dValue) {
    return AddItem(pNode, pName, std::to_string(dValue).c_str());
}

PKINGJSON CKingJson::AddItem(PKINGJSON pNode, const char* pName, long long lValue) {
    return AddItem(pNode, pName, std::to_string(lValue).c_str());
}

int CKingJson::DelNode(PKINGJSON pNode, const char* pName, bool bList) {
    if (pName == NULL)
        return -1;
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    std::vector<kingJson>* pList = bList ? &pNode->lstList : &pNode->lstNode;
    for (auto iter = pList->begin(); iter != pList->end(); iter++) {
        if (iter->strName == pName) {
            pList->erase(iter);
            return 1;
        }
    }
    return 0;
}

int CKingJson::DelItem(PKINGJSON pNode, const char* pName) {
    if (pName == NULL)
        return -1;
    if (pNode == NULL)
        pNode = &m_kjsRoot;
    std::vector<kingJson>* pList = &pNode->lstItem;
    for (auto iter = pList->begin(); iter != pList->end(); iter++) {
        if (iter->strName == pName) {
            pList->erase(iter);
            return 1;
        }
    }
    return 0;
}

PKINGJSON CKingJson::ModItem(PKINGJSON pNode, const char* pName, const char* pValue) {
    PKINGJSON pFind = findItem(pNode, pName, false);
    if (pFind != NULL) {
        pFind->strData = pValue;
    }
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
    outFile.write(m_strText.c_str(), m_strText.length());
    return m_strText.length();;
}

PKINGJSON CKingJson::findNode(PKINGJSON pNode, const char* pName, bool bChild) {
    if (pName == NULL)
        return NULL;
    for (auto& iter : pNode->lstNode) {
        if (iter.strName == pName)
            return &iter;
    }
    for (auto& iter : pNode->lstList) {
        if (iter.strName == pName)
            return &iter;
    }
    if (!bChild)
        return NULL;
    PKINGJSON pFind = NULL;
    for (auto& iter : pNode->lstNode) {
        pFind = findNode(&iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    for (auto& iter : pNode->lstList) {
        pFind = findNode(&iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    return NULL;
}

PKINGJSON CKingJson::findItem(PKINGJSON pNode, const char* pName, bool bChild) {
    if (pName == NULL)
        return NULL;
    for (auto& iter : pNode->lstItem) {
        if (iter.strName == pName)
            return &iter;
    }
    if (!bChild)
        return NULL;

    PKINGJSON pFind = NULL;
    for (auto& iter : pNode->lstItem) {
        pFind = findNode(&iter, pName, true);
        if (pFind != NULL)
            return pFind;
    }
    return NULL;
}

int CKingJson::parseNode(PKINGJSON pNode, char* pData, int nSize, bool bList) {
    char* pText = pData;
    char* pNext = pData;
    if (*pText == '{' || *pText == '[') {
        bList = *pText++ == '[' ? true : false;
    }
    kingJson    kjsNode;
    kingJson    kjsItem;
    std::string strName;
    int nRest = nSize - (pText - pData);
    int nErr  = 0;
    while (pText - pData < nSize) {
        if (*pText == '}' || *pText == ']') {
            if (*(pText + 1) == ',')
                return pText - pData + 2;
            else
                return pText - pData + 1;
        }
        // get item name
        if (*pText != '\"')
            return -1;
        pText++;
        pNext = pText;
        while (pNext - pData < nSize) {
            if (*pNext == '\"' && *(pNext - 1) != '\\') {
                *pNext++ = 0;
                break;
            }
            pNext++;
        }
        if (*pNext != ':') {
            if (!bList)
                return -1;
            kjsItem.strName = "";
            fillValue(kjsItem.strData, pText);
            kjsItem.pParent = pNode;
            pNode->lstItem.push_back(kjsItem);
            if (*pNext == ',')
                pNext++;
            pText = pNext++;
            continue;
        }
        strName = pText;

        //get item data
        pText = pNext + 1;
        if (*pText == '{' || *pText == '[') {
            bList = *pText == '[' ? true : false;
            nRest = nSize - (pText - pData);
            kjsNode.pParent = pNode;
            kjsNode.strName = strName;
            if (bList) {
                pNode->lstList.push_back(kjsNode);
                nErr = parseNode(&pNode->lstList.back(), pText, nRest, bList);
            }
            else {
                pNode->lstNode.push_back(kjsNode);
                nErr = parseNode(&pNode->lstNode.back(), pText, nRest, bList);
            }
            if (nErr < 0)
                return nErr;
            pText += nErr;
        }
        else {
            pNext = pText + 1;
            if (*pText == '\"') {
                while (pNext - pData < nSize) {
                    if (*pNext == '\"' && *(pNext - 1) != '\\') {
                        pNext++;
                        break;
                    }
                    pNext++;
                }
            }
            if (*pNext == ',') {
                if (*pText == '\"') {
                    pText++;
                    *(pNext - 1) = 0;
                }
                else {
                    *pNext = 0;
                }
                kjsItem.pParent = pNode;
                kjsItem.strName = strName;
                fillValue(kjsItem.strData, pText);
                pNode->lstItem.push_back(kjsItem);
                pNext++;
                pText = pNext++;
            }
            else if (*pNext == '}' || *pNext == ']') {
                if (*pText == '\"') {
                    pText++;
                    *(pNext - 1) = 0;
                }
                else {
                    *pNext = 0;
                }
                kjsItem.pParent = pNode;
                kjsItem.strName = strName;
                fillValue(kjsItem.strData, pText);
                pNode->lstItem.push_back(kjsItem);
                pNext++;
                pText = pNext++;
                if (*pText == ',') {
                    pText++;
                    pNext++;
                }
                return pText - pData;
            }
        }
    }
    return pNext - pData;
}

int CKingJson::fillValue(std::string& strText, const char* pText) {
    const char * pMark = strstr(pText, "\\\"");
    if (pMark == NULL) {
        strText = pText;
    }
    else {
        strText = "";
        while (*pText != NULL) {
            if (*pText == '\\' && *(pText + 1) == '\"') {
                strText += "\"";
                pText += 2;
                continue;
            }
            strText += *pText++;
        }
    }
    return strText.length();
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
    for (auto iter = pNode->lstItem.begin(); iter != pNode->lstItem.end(); iter++) {
        if (iter->strName.length() > 0) {
            m_strText += "\"";
            m_strText += iter->strName;
            m_strText += "\":";
        }
        if (iter->strData.length() > 0) {
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
            m_strText += "\"";
        }
        else {
            m_strText += "\"\"";
        }
        if (nNodeSize > 0 || iter + 1 != pNode->lstItem.end())
            m_strText += ",";
    }
    for (int i = 0; i < pNode->lstNode.size(); i++) {
        if (i == pNode->lstNode.size() - 1)
            formatText(&pNode->lstNode.at(i), false, true);
        else
            formatText(&pNode->lstNode.at(i), false, false);
    }
    for (int i = 0; i < pNode->lstList.size(); i++) {
        if (i == pNode->lstList.size() - 1)
            formatText(&pNode->lstList.at(i), true, true);
        else
            formatText(&pNode->lstList.at(i), true, false);
    }
    m_strText += bList ? "]" : "}";
    if (!bLast)
        m_strText += ",";
    return m_strText.length() - nLength;
}
