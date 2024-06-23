/*******************************************************************************
    File:		CKingJson.cpp

    Contains:	the king json class implement code

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#include <stdio.h>
#include <emmintrin.h>
#include <nmmintrin.h>
#include "CKingJson.h"

CKingJson::CKingJson(void) {
}

CKingJson::~CKingJson(void) {
    releaseAll();
}

PKSONNODE CKingJson::ParseData(const char* pData, int nSize, int nFlag) {
    if (pData == NULL || nSize <= 1)
        return NULL;
    releaseAll();
    m_nTextNum = nSize + 1;
    m_pTextMem = new char[m_nTextNum];
    memcpy(m_pTextMem, pData, nSize);
    m_pTextMem[nSize] = 0;
    char* pBuff = m_pTextMem;
    if (((*(int*)m_pTextMem) & 0X00BFBBEF) == 0X00BFBBEF) {
        pBuff += 3; nSize -= 3;
    }
    m_nSetSize = ((nSize / 1048576) + 1) * 2048;
    m_nTextAlloc = nSize;
    int nErr = parseJson(pBuff, nSize);
    if (nErr <= 0)
        return NULL;
    return &m_ksonRoot;
}

PKSONNODE CKingJson::OpenFileA(const char* pFile, int nFlag) {
    FILE* hFile = fopen(pFile, "rb");
    if (hFile == NULL) return NULL;
    return openFile(hFile, nFlag);
}

PKSONNODE CKingJson::OpenFileW(const wchar_t* pFile, int nFlag) {
#ifdef _WIN32
    FILE* hFile = _wfopen(pFile, L"rb");
    if (hFile == NULL) return NULL;
    return openFile(hFile, nFlag);
#else
    return NULL;
#endif // _WIN32
}

PKSONNODE CKingJson::FindNode(PKSONNODE pNode, const char* pName, bool bChild) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    return findNode(pNode, pName, bChild);
}

PKSONITEM CKingJson::FindItem(PKSONNODE pNode, const char* pName, bool bChild) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    return findItem(pNode, pName, true, bChild);
}

PKSONITEM CKingJson::FindItem(PKSONNODE pNode, const char* pData) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    return findItem(pNode, pData, false, false);
}

PKSONITEM CKingJson::GetItem(PKSONNODE pNode, int nIndex) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONITEM pItem = pNode->pHead;
    while (pItem != NULL) {
        if (nIndex-- <= 0) return pItem;
        pItem = pItem->pNext;
    }
    return NULL;
}

PKSONNODE CKingJson::GetList(PKSONNODE pNode, int nIndex) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONNODE pHead = GetHeadNode(pNode);
    while (pHead != NULL) {
        if (nIndex-- <= 0) return pHead;
        pHead = pHead->pNext;
    }
    return NULL;
}

const char* CKingJson::GetValue(PKSONITEM pItem) {
    if (pItem == NULL || pItem->pData == NULL)
        return NULL;
    if (!(pItem->nFlag & KSON_TEXT_UTF8)) {
        convertTxt(pItem->pData);
        pItem->nFlag |= KSON_TEXT_UTF8;
    }
    return pItem->pData;
}

const char* CKingJson::GetValue(PKSONNODE pNode, const char* pName, bool bChild) {
    return GetValue(pNode, pName, bChild, NULL);
}

const char* CKingJson::GetValue(PKSONNODE pNode, const char* pName, bool bChild, const char* pDefault) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONITEM pItem = findItem(pNode, pName, true, bChild);
    if (pItem != NULL && pItem->pData != NULL && !(pItem->nFlag & KSON_TEXT_UTF8)) {
        convertTxt(pItem->pData);
        pItem->nFlag |= KSON_TEXT_UTF8;
    }
    return pItem != NULL ? (pItem->pData == NULL ? pDefault : pItem->pData) : pDefault;
}

int CKingJson::GetValueInt(PKSONNODE pNode, const char* pName, bool bChild, int nDefault) {
    const char* pValue = GetValue(pNode, pName, bChild, NULL);
    return pValue == NULL ? nDefault : atoi(pValue);
}

double CKingJson::GetValueDbl(PKSONNODE pNode, const char* pName, bool bChild, double dDefault) {
    const char* pValue = GetValue(pNode, pName, bChild, NULL);
    return pValue == NULL ? dDefault : atof(pValue);
}

long long CKingJson::GetValueLng(PKSONNODE pNode, const char* pName, bool bChild, long long lDefault) {
    const char* pValue = GetValue(pNode, pName, bChild, NULL);
    return pValue == NULL ? lDefault : atoll(pValue);
}

bool CKingJson::IsValueTrue(PKSONNODE pNode, const char* pName, bool bChild, bool bTure) {
    const char* pValue = GetValue(pNode, pName, bChild, NULL);
    return pValue == NULL ? bTure : !strcmp(pValue, "true");
}

bool CKingJson::IsValueNull(PKSONNODE pNode, const char* pName, bool bChild, bool bNull) {
    const char* pValue = GetValue(pNode, pName, bChild, NULL);
    return pValue == NULL ? bNull : !strcmp(pValue, "null");
}

int CKingJson::GetNodeNum(PKSONNODE pNode) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    int nNodeSize = 0;
    PKSONNODE pHead = GetHeadNode(pNode);
    while (pHead != NULL) {
        nNodeSize++; pHead = pHead->pNext;
    }
    return nNodeSize;
}

int CKingJson::GetItemNum(PKSONNODE pNode) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    int nItemSize = 0;
    PKSONITEM pItem = pNode->pHead;
    while (pItem != NULL) {
        nItemSize++; pItem = pItem->pNext;
    }
    return nItemSize;
}

const char* CKingJson::GetItemTxt(PKSONNODE pNode, int nIndex, const char* pDefault) {
    PKSONITEM pItem = GetItem(pNode, nIndex);
    return pItem == NULL ? pDefault : pItem->pData;
}

int CKingJson::GetItemInt(PKSONNODE pNode, int nIndex, int nDefault) {
    PKSONITEM pItem = GetItem(pNode, nIndex);
    return pItem != NULL ? (pItem->pData != NULL ? atoi(pItem->pData) : nDefault) : nDefault;
}

double CKingJson::GetItemDbl(PKSONNODE pNode, int nIndex, double dDefault) {
    PKSONITEM pItem = GetItem(pNode, nIndex);
    return pItem != NULL ? (pItem->pData != NULL ? atof(pItem->pData) : dDefault) : dDefault;
}

long long CKingJson::GetItemLng(PKSONNODE pNode, int nIndex, long long lDefault) {
    PKSONITEM pItem = GetItem(pNode, nIndex);
    return pItem != NULL ? (pItem->pData != NULL ? atoll(pItem->pData) : lDefault) : lDefault;
}

PKSONNODE CKingJson::AddNode(PKSONNODE pNode, const char* pName, const char* pJson, int nSize) {
    if (pJson == NULL || nSize <= 1)
        return NULL;
    char* pTextMem = new char[nSize + 1];
    memcpy(pTextMem, pJson, nSize);
    pTextMem[nSize] = 0;
    PKSONNODE pNewNode = parseNewJson(pNode, pTextMem, nSize);
    delete[]pTextMem;
    return pNewNode;
}

PKSONNODE CKingJson::AddNode(PKSONNODE pNode, const char* pName, bool bList) {
    return addNewNode(pNode, (char*)pName, strlen(pName), bList ? KSON_NODE_LIST : 0);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, const char* pValue) {
    return addNewItem(pNode, pName, strlen(pName), pValue, strlen(pValue), KSON_DATA_TEXT, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, int nValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%d", nValue);
    return addNewItem(pNode, pName, strlen(pName), szValue, strlen(szValue), 0, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, double dValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%f", dValue);
    return addNewItem(pNode, pName, strlen(pName), szValue, strlen(szValue), 0, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, long long lValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%lld", lValue);
    return addNewItem(pNode, pName, strlen(pName), szValue, strlen(szValue), 0, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, const char* pValue, bool bText) {
    if (pValue == NULL) return NULL;
    return addNewItem(pNode, NULL, 0, pValue, strlen(pValue), bText ? KSON_DATA_TEXT : 0, nIndex);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, int nValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%d", nValue);
    return addNewItem(pNode, NULL, 0, szValue, strlen(szValue), 0, nIndex);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, double dValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%f", dValue);
    return addNewItem(pNode, NULL, 0, szValue, strlen(szValue), 0, nIndex);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, long long lValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%lld", lValue);
    return addNewItem(pNode, NULL, 0, szValue, strlen(szValue), 0, nIndex);
}

int CKingJson::DelNode(PKSONNODE pNode, const char* pName) {
    PKSONNODE pFind = findNode(pNode, pName, false);
    return deleteNode(pFind);
}

int CKingJson::DelItem(PKSONNODE pNode, const char* pText, bool bName) {
    PKSONITEM pItem = findItem(pNode, pText, bName, false);
    return deleteItem(pNode, pItem);
}

int CKingJson::DelItem(PKSONNODE pNode, int nIndex) {
    PKSONITEM pItem = GetItem(pNode, nIndex);
    return deleteItem(pNode, pItem);
}

int CKingJson::DelNode(PKSONNODE pNode) {
    if (pNode == NULL)
        return releaseAll();
    else
        return deleteNode(pNode);
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, const char* pName, const char* pValue) {
    PKSONITEM pFind = findItem(pNode, pName, true, false);
    modifyItem(pFind, pValue, false);
    return pFind;
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, const char* pName, int nValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%d", nValue);
    return ModItem(pNode, pName, szValue);
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, const char* pName, double dValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%f", dValue);
    return ModItem(pNode, pName, szValue);
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, const char* pName, long long lValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%lld", lValue);
    return ModItem(pNode, pName, szValue);
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, int nIndex, const char* pValue) {
    PKSONITEM pFind = GetItem(pNode, nIndex);
    modifyItem(pFind, pValue, false);
    return pFind;
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, int nIndex, int nValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%d", nValue);
    return ModItem(pNode, nIndex, szValue);
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, int nIndex, double dValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%f", dValue);
    return ModItem(pNode, nIndex, szValue);
}

PKSONITEM CKingJson::ModItem(PKSONNODE pNode, int nIndex, long long lValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%lld", lValue);
    return ModItem(pNode, nIndex, szValue);
}

int CKingJson::ModItem(PKSONITEM pItem, const char* pNewName) {
    return modifyItem(pItem, pNewName, true);
}

int CKingJson::SortItem(PKSONNODE pNode, bool bName, bool bASC) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    int nIndex = 0;
    int nItemNum = 0;
    PKSONITEM pHead = GetHeadItem(pNode);
    while (pHead != NULL) {
        nItemNum++; pHead = pHead->pNext;
    }
    if (nItemNum <= 1)
        return 0;
    PKSONITEM* ppItem = new PKSONITEM[nItemNum];
    pHead = GetHeadItem(pNode);
    while (pHead != NULL) {
        ppItem[nIndex++] = pHead;
        pHead = pHead->pNext;
    }
    if (bName)
        qsort(ppItem, nItemNum, sizeof(PKSONITEM), ksonCompItemName);
    else
        qsort(ppItem, nItemNum, sizeof(PKSONITEM), ksonCompItemData);
    if (!bASC) {
        PKSONITEM pTemp = NULL;
        for (int i = 0; i < nItemNum / 2; i++) {
            pTemp = ppItem[i];
            ppItem[i] = ppItem[nItemNum - i - 1];
            ppItem[nItemNum - i - 1] = pTemp;
        }
    }
    pNode->pHead = pNode->pItem = ppItem[0];
    ppItem[nItemNum - 1]->pNext = NULL;
    for (int i = 0; i < nItemNum - 1; i++)
        ppItem[i]->pNext = ppItem[i + 1];
    delete[]ppItem;
    return 1;
}

int CKingJson::SortNode(PKSONNODE pNode, bool bASC) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    int nNodeNum = 0;
    PKSONNODE pHead = GetHeadNode(pNode);
    while (pHead != NULL) {
        nNodeNum++;
        pHead = pHead->pNext;
    }
    if (nNodeNum <= 1)
        return 0;
    PKSONNODE* ppNode = new PKSONNODE[nNodeNum];
    pHead = GetHeadNode(pNode);
    int nIndex = 0;
    while (pHead != NULL) {
        ppNode[nIndex++] = pHead;
        pHead = pHead->pNext;
    }
    qsort(ppNode, nNodeNum, sizeof(PKSONNODE), ksonCompNodeName);
    if (!bASC) {
        PKSONNODE pTemp = NULL;
        for (int i = 0; i < nNodeNum / 2; i++) {
            pTemp = ppNode[i];
            ppNode[i] = ppNode[nNodeNum - i - 1];
            ppNode[nNodeNum - i - 1] = pTemp;
        }
    }
    pNode->pNode = ppNode[0];
    ppNode[0]->pPrev = NULL;
    ppNode[0]->pNext = ppNode[1];
    ppNode[nNodeNum - 1]->pPrev = ppNode[nNodeNum - 2];
    ppNode[nNodeNum - 1]->pNext = NULL;
    for (int i = 1; i < nNodeNum - 1; i++) {
        ppNode[i]->pPrev = ppNode[i - 1];
        ppNode[i]->pNext = ppNode[i + 1];
    }
    delete[]ppNode;
    return 1;
}

PKSONITEM CKingJson::GetHeadItem(PKSONNODE pNode) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    return pNode->pHead;
}

PKSONNODE CKingJson::GetHeadNode(PKSONNODE pNode) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONNODE pChild = pNode->pNode;
    while (pChild != NULL && pChild->pPrev != NULL)
        pChild = pChild->pPrev;
    pNode->pNode = pChild;
    return pChild;
}

PKSONITEM CKingJson::GetTailItem(PKSONNODE pNode) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONITEM pItem = pNode->pItem;
    while (pItem != NULL && pItem->pNext != NULL)
        pItem = pItem->pNext;
    return pItem;
}

PKSONNODE CKingJson::GetTailNode(PKSONNODE pNode) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONNODE pChild = pNode->pNode;
    while (pChild != NULL && pChild->pNext != NULL)
        pChild = pChild->pNext;
    return pChild;
}

const char* CKingJson::FormatText(PKSONNODE pNode, int* pSize) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    m_pTxtNode = pNode;
    if (m_pTextFmt != NULL) delete[]m_pTextFmt;
    int nTextSize = m_nTextAlloc > 0 ? m_nTextAlloc : getTextSize(&m_ksonRoot);
    if (nTextSize <= 0)
        return NULL;
    m_pTextFmt = new char[nTextSize];
    memset(m_pTextFmt, 0, nTextSize);
    m_pTextPos = m_pTextFmt;
    if (pNode != &m_ksonRoot)
        *m_pTextPos++ = (pNode->nFlag & KSON_NODE_LIST) ? '[' : '{';
    formatText(pNode);
    if (pNode != &m_ksonRoot)
        *m_pTextPos++ = (pNode->nFlag & KSON_NODE_LIST) ? ']' : '}';
    if (pSize != NULL)
        *pSize = (int)(m_pTextPos - m_pTextFmt);
    return m_pTextFmt;
}

int CKingJson::SaveToFile(PKSONNODE pNode, const char* pFile) {
    FILE* hFile = fopen(pFile, "wb");
    if (hFile == NULL)
        return -1;
    int nJsonSize = 0;
    const char* pFmtText = FormatText(pNode, &nJsonSize);
    if (pFmtText == NULL)
        return -1;
    int nUtf8 = 0X00BFBBEF;
    size_t nWrite = fwrite(&nUtf8, 1, 3, hFile);
    if (nWrite > 0)
        fwrite(pFmtText, 1, nJsonSize, hFile);
    fclose(hFile);
    return 1;
}

PKSONNODE CKingJson::openFile(FILE* hFile, int nFlag) {
    fseek(hFile, 0, SEEK_END);
    int nSize = ftell(hFile);
    fseek(hFile, 0, SEEK_SET);
    if (m_pTextMem != NULL)
        delete[]m_pTextMem;
    m_nTextNum = nSize + 1;
    m_pTextMem = new char[m_nTextNum];
    size_t nRead = fread(m_pTextMem, 1, nSize, hFile);
    fclose(hFile);
    if (nRead != nSize)
        return NULL;
    m_pTextMem[nSize] = 0;
    return ParseData((const char*)m_pTextMem, nSize, nFlag);
}

PKSONNODE CKingJson::findNode(PKSONNODE pNode, const char* pName, bool bChild) {
    if (pName == NULL) return NULL;
    PKSONNODE pChild = GetHeadNode(pNode);
    while (pChild != NULL) {
        if (pChild->pName != NULL && strcmp(pChild->pName, pName) == 0)
            return pChild;
        pChild = pChild->pNext;
    }
    if (bChild) {
        PKSONNODE pFind = NULL;
        pChild = GetHeadNode(pNode);
        while (pChild != NULL) {
            pFind = findNode(pChild, pName, bChild);
            if (pFind != NULL)
                return pFind;
            pChild = pChild->pNext;
        }
    }
    return NULL;
}

PKSONITEM CKingJson::findItem(PKSONNODE pNode, const char* pText, bool bName, bool bChild) {
    if (pText == NULL) return NULL;
    PKSONITEM pItem = GetHeadItem(pNode);
    while (pItem != NULL) {
        if (bName) {
            if (pItem->pName != NULL && strcmp(pItem->pName, pText) == 0)
                return pItem;
        }
        else {
            if (pItem->pData != NULL && strcmp(pItem->pData, pText) == 0)
                return pItem;
        }
        pItem = pItem->pNext;
    }
    if (bChild) {
        PKSONNODE pChild = GetHeadNode(pNode);
        while (pChild != NULL) {
            pItem = findItem(pChild, pText, bName, bChild);
            if (pItem != NULL)
                return pItem;
            pChild = pChild->pNext;
        }
    }
    return NULL;
}

int CKingJson::deleteNode(PKSONNODE pNode) {
    if (pNode == NULL || pNode == &m_ksonRoot)
        return -1;
    if (pNode->pPrev != NULL)
        pNode->pPrev->pNext = pNode->pNext;
    if (pNode->pNext != NULL)
        pNode->pNext->pPrev = pNode->pPrev;
    if (pNode->pPrnt->pNode == pNode) {
        if (pNode->pPrev != NULL)
            pNode->pPrnt->pNode = pNode->pPrev;
        else if (pNode->pNext != NULL)
            pNode->pPrnt->pNode = pNode->pNext;
        else
            pNode->pPrnt->pNode = NULL;
    }
    releaseNode(pNode);
    return 1;
}

PKSONNODE CKingJson::parseNewJson(PKSONNODE pNode, char* pData, int nSize) {
    int   nDeep = 0;
    int   nName = 0;
    char* pName = NULL;
    char* pLast = pData + nSize;
    char* pText = pData;
    char* pNext = pData;
    while (pText < pLast) {
        if (*pText <= 0X20)
            pText = skipFormat(pText, pLast);
        if (*pText == '{' || *pText == '[') {
            if (pName == NULL)
                pNode = addNewNode(pNode, "ksonNode", 8, *pText++ == '[' ? KSON_NODE_LIST : 0);
            else
                pNode = addNewNode(pNode, pName, nName, *pText++ == '[' ? KSON_NODE_LIST : 0);
            if (pNode == NULL) return NULL;
        }
        else if (*pText == '}' || *pText == ']') {
            *pText++ = 0; if (*pText == ',') pText++;
            pNode = pNode->pPrnt;
        }
        else if (*pText == '"') {
            pNext = searchName(++pText, pLast);
            nName = (int)(pNext - pText); *pNext++ = 0;
            if (pNode->nFlag & KSON_NODE_LIST) {
                addNewItem(pNode, NULL, 0, pText, nName, KSON_DATA_TEXT, KSON_NUM_MAX);
                pText = (*pNext == ',') ? pNext + 1 : pNext;
            }
            else {
                pName = pText; pText = pNext + 1;
                while (*pText <= 0X20) pText++;
                if (*pText == '"') {
                    pNext = searchText(++pText, pLast);
                    addNewItem(pNode, pName, nName, pText, (int)(pNext - pText), KSON_DATA_TEXT, KSON_NUM_MAX);
                    *pNext++ = 0; pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
            }
        }
        else {
            pNext = searchData(pText, pLast);
            addNewItem(pNode, pName, nName, pText, (int)(pNext - pText), 0, KSON_NUM_MAX);
            if (*pNext == ',' || *pNext <= ' ')
                *pNext++ = 0;
            pText = pNext;
        }
    }
    return GetTailNode(pNode);
}

int CKingJson::deleteItem(PKSONNODE pNode, PKSONITEM pItem) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    if (pItem == NULL) return -1;
    PKSONITEM pHead = GetHeadItem(pNode);
    if (pHead == NULL || pHead->pNext == NULL) {
        if (pHead != pItem)
            return -1;
        if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ((pItem->nFlag & 0X0FFF0000) >> 16); delete[]pItem->pName;
        }
        if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= (pItem->nFlag & 0XFFFF); delete[]pItem->pData;
        }
        pNode->pHead = pNode->pItem = NULL;
        return 1;
    }
    PKSONITEM pPrev = NULL;
    while (pHead != NULL) {
        if (pHead == pItem) {
            if (pPrev != NULL)
                pPrev->pNext = pItem->pNext;
            if (pNode->pHead == pItem)
                pNode->pHead = pItem->pNext;
            if (pNode->pItem == pItem)
                pNode->pItem = pItem->pNext != NULL ? pItem->pNext : pPrev;
            if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
                m_nTextAlloc -= ((pItem->nFlag & 0X0FFF0000) >> 16); delete[]pItem->pName;
            }
            if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
                m_nTextAlloc -= (pItem->nFlag & 0XFFFF); delete[]pItem->pData;
            }
            return 1;
        }
        pPrev = pHead; pHead = pHead->pNext;
    }
    return -1;
}

PKSONNODE CKingJson::addNewNode(PKSONNODE pNode, const char* pName, int nNameLen, int nType) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    if ((m_nNodeCount % m_nSetSize) == 0)
        if (addNodeSet() < 0) return NULL;
    if (pName != NULL) {
        m_pSetNode->pName = new char[nNameLen + 1];
        m_pSetNode->pName[nNameLen] = 0;
        memcpy(m_pSetNode->pName, pName, nNameLen);
        m_nTextAlloc += (nNameLen + 4);
    }
    m_pSetNode->pPrnt = pNode;
    m_pSetNode->nFlag = nType | (nNameLen << 16);
    PKSONNODE pTaiNode = GetTailNode(pNode);
    m_pSetNode->pPrev = pTaiNode;
    if (pTaiNode == NULL)
        pNode->pNode = m_pSetNode;
    else
        pTaiNode->pNext = m_pSetNode;
    m_nNodeCount++;
    return m_pSetNode++;
}

PKSONITEM CKingJson::addNewItem(PKSONNODE pNode, const char* pName, int nNameLen, const char* pData, int nDataLen, int nType, int nIndex) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    if ((m_nItemCount % m_nSetSize) == 0)
        if (addItemSet() < 0) return NULL;
    m_pSetItem->nFlag = nType + (nNameLen << 16) + nDataLen;
    if (pName != NULL && ((pNode->nFlag & KSON_NODE_LIST) == 0)) {
        m_pSetItem->pName = new char[nNameLen + 1];
        m_pSetItem->pName[nNameLen] = 0;
        memcpy(m_pSetItem->pName, pName, nNameLen);
        m_nTextAlloc += (nNameLen + 4);
    }
    if (pData != NULL) {
        m_pSetItem->pData = new char[nDataLen + 1];
        m_pSetItem->pData[nDataLen] = 0;
        memcpy(m_pSetItem->pData, pData, nDataLen);
        m_nTextAlloc += (nDataLen + 4);
    }
    PKSONITEM pPosItem = GetHeadItem(pNode);
    if (pPosItem == NULL) {
        pNode->pHead = m_pSetItem;
    }
    else if (nIndex <= 0) {
        m_pSetItem->pNext = pNode->pHead;
        pNode->pHead = m_pSetItem;
    }
    else if (nIndex == KSON_NUM_MAX) {
        pPosItem = GetTailItem(pNode);
        pPosItem->pNext = m_pSetItem;
    }
    else {
        while (pPosItem != NULL) {
            if (--nIndex == 0) {
                m_pSetItem->pNext = pPosItem->pNext;
                pPosItem->pNext = m_pSetItem;
                break;
            }
            pPosItem = pPosItem->pNext;
        }
        if (pPosItem == NULL) {
            pPosItem = GetTailItem(pNode);
            pPosItem->pNext = m_pSetItem;
        }
    }
    pNode->pItem = m_pSetItem;
    m_nItemCount++;
    return m_pSetItem++;
}

int CKingJson::modifyItem(PKSONITEM pItem, const char* pText, bool bName) {
    if (pItem == NULL)
        return -1;
    if (bName) {
        if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ((pItem->nFlag & 0X0FFF0000) >> 16); delete[]pItem->pName;
        }
        int nSize = (int)strlen(pText);
        pItem->pName = new char[nSize + 1];
        pItem->pName[nSize] = 0;
        memcpy(pItem->pName, pText, nSize);
        pItem->nFlag = (pItem->nFlag & 0XF000FFFF) + (nSize << 16);
        m_nTextAlloc += (nSize + 4);
    }
    else {
        if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= (pItem->nFlag & 0XFFFF); delete[]pItem->pData;
        }
        int nSize = (int)strlen(pText);
        pItem->pData = new char[nSize + 1];
        pItem->pData[nSize] = 0;
        memcpy(pItem->pData, pText, nSize);
        pItem->nFlag = (pItem->nFlag & 0XFFFF0000) + nSize;
        m_nTextAlloc += (nSize + 4);
    }
    return 1;
}

int CKingJson::parseJson(char* pData, int nSize) {
    int         nDeep = 0;
    int         nName = 0;
    PKSONNODE   pNode = &m_ksonRoot;
    char* pName = NULL;
    char* pLast = pData + nSize;
    char* pText = pData;
    char* pNext = pData;
    pText = skipFormat(pText, pLast);
    if (*pText != '{' && *pText != '[') return -1;
    m_ksonRoot.nFlag = *pText++ == '[' ? KSON_NODE_LIST : 0;
    while (pText < pLast) {
        if (*pText <= 0X20) 
            pText = skipFormat(pText, pLast);
        if (*pText == '{' || *pText == '[') {
            if (nDeep++ > KSON_MAX_DEEPS) return -1;
            pNode = appendNode(pNode, pName, nName, *pText++ == '[' ? KSON_NODE_LIST : 0);
            if (pNode == NULL) return -1;
        }
        else if (*pText == '}' || *pText == ']') {
            if (pNode->pPrnt == NULL) break;
            *pText++ = 0; if (*pText == ',') pText++;
             pNode = pNode->pPrnt; nDeep--;
        }
        else if (*pText == '"') {
            pNext = searchName(++pText, pLast);
            nName = (int)(pNext - pText); *pNext++ = 0;
            if (pNode->nFlag & KSON_NODE_LIST) {
                appendItem(pNode, NULL, 0, pText, nName, KSON_DATA_TEXT);
                pText = (*pNext == ',') ? pNext + 1 : pNext;
            }
            else {
                pName = pText; pText = pNext + 1;
                while (*pText <= 0X20) pText++;
                if (*pText == '"') {
                    pNext = searchText(++pText, pLast);
                    appendItem(pNode, pName, nName, pText, (int)(pNext - pText), KSON_DATA_TEXT);
                    *pNext++ = 0; pText = (*pNext == ',') ? pNext + 1 : pNext;
                }
            }
        }
        else {
            pNext = searchData(pText, pLast);
            appendItem(pNode, pName, nName, pText, (int)(pNext - pText), 0);
            if (*pNext == ',' || *pNext <= ' ')
                *pNext++ = 0;
            pText = pNext;
        }
    }
    return (int)(pText - pData);
}

PKSONITEM CKingJson::appendItem(PKSONNODE pNode, char* pName, int nNameLen, char* pData, int nDataLen, int nType) {
    if ((m_nItemCount % m_nSetSize) == 0)
        if (addItemSet() < 0) return NULL;
    m_pSetItem->pName = (pNode->nFlag & KSON_NODE_LIST) ? NULL : pName;
    m_pSetItem->nFlag = (pNode->nFlag & KSON_NODE_LIST) ? (nType | nDataLen) : (nType | (nNameLen << 16) | nDataLen);
    m_pSetItem->pData = pData;
    if (pNode->pHead == NULL)
        pNode->pHead = m_pSetItem;
    if (pNode->pItem != NULL)
        pNode->pItem->pNext = m_pSetItem;
    pNode->pItem = m_pSetItem;
    m_nItemCount++;
    return m_pSetItem++;
}

PKSONNODE CKingJson::appendNode(PKSONNODE pNode, char* pName, int nNameLen, int nType) {
    if ((m_nNodeCount % m_nSetSize) == 0)
        if (addNodeSet() < 0) return NULL;
    m_pSetNode->pPrnt = pNode;
    m_pSetNode->pName = (pNode->nFlag & KSON_NODE_LIST) ? NULL : pName;
    m_pSetNode->nFlag = (pNode->nFlag & KSON_NODE_LIST) ? nType : (nType | (nNameLen << 16));
    if (pNode->pNode != NULL)
        pNode->pNode->pNext = m_pSetNode;
    m_pSetNode->pPrev = pNode->pNode;
    pNode->pNode = m_pSetNode;
    m_nNodeCount++;
    return m_pSetNode++;
}

char* CKingJson::skipFormat(char* pText, char* pLast) {
    uint32_t uMask;
    __m128i  vData;
    __m128i  vFind = _mm_set1_epi8(0X20);
    while (pText < pLast) {
        vData = _mm_loadu_si128((__m128i*)pText);
        uMask = _mm_movemask_epi8(_mm_cmpgt_epi8(vData, vFind));
        if (uMask)
            return (char *)(pText + forwardPos(uMask));
        pText = pText + KSON_VECTOR_SIZE;
    }
    return pLast;
}

char* CKingJson::searchName(char* pText, char* pLast) {
    __m128i  vTxt1;
    __m128i  vFind = _mm_set1_epi8((uint8_t)'\"');
    uint32_t uMask;
    while (pText < pLast) {
        vTxt1 = _mm_loadu_si128((__m128i*)pText);
        uMask = _mm_movemask_epi8(_mm_cmpeq_epi8(vFind, vTxt1));
        if (uMask) {
            pText = pText + forwardPos(uMask);
            if (*(pText - 1) == '\\') {
                pText++; continue;
            }
            return pText;
        }
        pText = pText + KSON_VECTOR_SIZE;
    }
    return pLast;
}

char* CKingJson::searchText(char* pText, char* pLast) {
    uint32_t uMask;
    __m128i  vTxt1, vTxt2, vEqa1, vEqa2, vTtor;
    __m128i  vFind = _mm_set1_epi8((uint8_t)'\"');
    while (pText < pLast) {
        vTxt1 = _mm_loadu_si128((__m128i*)pText);
        vTxt2 = _mm_loadu_si128((__m128i*)(pText + KSON_VECTOR_SIZE));
        vEqa1 = _mm_cmpeq_epi8(vFind, vTxt1);
        vEqa2 = _mm_cmpeq_epi8(vFind, vTxt2);
        vTtor = _mm_or_si128(vEqa1, vEqa2);
        if (_mm_movemask_epi8(vTtor)) {
            uMask = _mm_movemask_epi8(vEqa1);
            if (!uMask) {
                pText += KSON_VECTOR_SIZE;
                uMask = _mm_movemask_epi8(vEqa2);
            }
            pText = pText + forwardPos(uMask);
            if (*(pText - 1) == '\\') {
                pText++; continue;
            }
            return pText;
        }
        pText = pText + KSON_VECTOR_SIZE;
    }
    return pLast;
}

char* CKingJson::searchData(char* pText, char* pLast) {
    uint32_t vMask;
    __m128i  vData; 
    __m128i  vEqa1, vEqa2, vEqa3, vEqa4;
    __m128i  vEnd1 = _mm_set1_epi8((uint8_t)']');
    __m128i  vEnd2 = _mm_set1_epi8((uint8_t)'}');
    __m128i  vEnd3 = _mm_set1_epi8((uint8_t)',');
    __m128i  vEnd4 = _mm_set1_epi8((uint8_t)' ');
    while (pText < pLast) {
        vData = _mm_loadu_si128((__m128i*)pText);
        vEqa1 = _mm_cmpeq_epi8(vEnd1, vData);
        vEqa2 = _mm_cmpeq_epi8(vEnd2, vData);
        vEqa3 = _mm_cmpeq_epi8(vEnd3, vData);
        vEqa4 = _mm_cmplt_epi8(vData, vEnd4);
        vMask = _mm_movemask_epi8(_mm_or_si128(_mm_or_si128(vEqa1, vEqa2), _mm_or_si128(vEqa3, vEqa4)));
        if (vMask)
            return pText + forwardPos(vMask);
        pText = pText + KSON_VECTOR_SIZE;
    }
    return pLast;
}

uint32_t CKingJson::forwardPos(uint32_t mask) {
#ifdef _WIN32
    unsigned long uPos = 0;
    _BitScanForward(&uPos, mask);
    return uPos;
#else
    return __builtin_ctz(mask);
#endif
}

int CKingJson::convertTxt(char* pData) {
    char* pText = pData;
    char* pNext = pData;
    uint32_t    uWord = 0;
    uint32_t    uNext = 0;
    uint8_t     uMark = 0;
    uint8_t     uPost = 0;
    int         nUtf8 = 0;
    while (*pNext != 0) {
        if (*pNext == '\\' && *(pNext + 1) == 'u') {
            uWord = convertNum(pNext + 2); pNext += 6;
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
    *pText = 0;
    return 1;
}

uint32_t CKingJson::convertNum(const char* pText) {
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

int CKingJson::formatText(PKSONNODE pNode) {
    char* pTextTmp = NULL;
    char* pTextPos = m_pTextPos;
    if (pNode->pName != NULL) {
        *pTextPos++ = '\"';
        memcpy(pTextPos, pNode->pName, (pNode->nFlag & 0X0FFF0000) >> 16);
        pTextPos += (pNode->nFlag & 0X0FFF0000) >> 16;
        *pTextPos++ = '\"'; *pTextPos++ = ':';
    }
    *pTextPos++ = (pNode->nFlag & KSON_NODE_LIST) ? '[' : '{';

    PKSONITEM pItem = pNode->pHead;
    while (pItem != NULL) {
        if (pItem->pName != NULL) {
            *pTextPos++ = '\"';
            memcpy(pTextPos, pItem->pName, (pItem->nFlag & 0X0FFF0000) >> 16);
            pTextPos += (pItem->nFlag & 0X0FFF0000) >> 16;
            *pTextPos++ = '\"'; *pTextPos++ = ':';
        }
        if (pItem->pData != NULL) {
            if (pItem->nFlag & KSON_DATA_TEXT)
                *pTextPos++ = '\"';
            memcpy(pTextPos, pItem->pData, pItem->nFlag & 0XFFFF);
            pTextPos += (pItem->nFlag & 0XFFFF);
            if (pItem->nFlag & KSON_DATA_TEXT)
                *pTextPos++ = '\"';
        }
        if (pItem->pNext != NULL || pNode->pNode != NULL)
            *pTextPos++ = ',';
        pItem = pItem->pNext;
    }
    m_pTextPos = pTextPos;
    PKSONNODE pChild = GetHeadNode(pNode);
    while (pChild != NULL) {
        formatText(pChild);
        pChild = pChild->pNext;
    }
    *m_pTextPos++ = (pNode->nFlag & KSON_NODE_LIST) ? ']' : '}';
    if (pNode != m_pTxtNode && pNode->pNext != NULL)
        *m_pTextPos++ = ',';
    return 0;
}

int CKingJson::getTextSize(PKSONNODE pNode) {
    int nTextSize = 4;
    if (pNode->pName != NULL)
        nTextSize += (((pNode->nFlag & 0X0FFF0000) >> 16) + 4);
    PKSONITEM pItem = GetHeadItem(pNode);
    while (pItem != NULL) {
        if (pItem->pName != NULL)
            nTextSize += (((pItem->nFlag & 0X0FFF0000) >> 16) + 4);
        nTextSize += ((pItem->nFlag & 0XFFFF) + 4);
        pItem = pItem->pNext;
    }
    PKSONNODE pChild = GetHeadNode(pNode);
    while (pChild != NULL) {
        nTextSize += getTextSize(pChild);
        pChild = pChild->pNext;
    }
    return nTextSize;
}

int CKingJson::releaseNode(PKSONNODE pNode) {
    if (pNode == NULL) return -1;
    PKSONITEM pItem = pNode->pHead;
    while (pItem != NULL) {
        if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ((pItem->nFlag & 0X0FFF0000) >> 16); delete[]pItem->pName;
        }
        if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= (pItem->nFlag & 0XFFFF); delete[]pItem->pData;
        }
        pItem = pItem->pNext;
    }
    pNode->pHead = NULL; pNode->pItem = NULL;
    if (pNode->pName != NULL && (pNode->pName < m_pTextMem || pNode->pName > m_pTextMem + m_nTextNum)) {
        m_nTextAlloc -= ((pNode->nFlag & 0X0FFF0000) >> 16); delete[]pNode->pName; pNode->pName = NULL;
    }
    PKSONNODE pHead = GetHeadNode(pNode);
    while (pHead != NULL) {
        releaseNode(pHead);
        pHead = pHead->pNext;
    }
    pNode->pNode = NULL; pNode->pPrev = NULL; pNode->pNext = NULL;
    return 0;
}

int CKingJson::addItemSet(void) {
    PITEMSET pItemSet = new itemSet();
    pItemSet->pBuff = (char*)malloc(sizeof(ksonItem) * m_nSetSize);
    if (pItemSet->pBuff == NULL)
        return -1;
    memset(pItemSet->pBuff, 0, sizeof(ksonItem) * m_nSetSize);
    if (m_pHeadItemSet == NULL)
        m_pHeadItemSet = pItemSet;
    if (m_pWorkItemSet != NULL)
        m_pWorkItemSet->pNext = pItemSet;
    m_pWorkItemSet = pItemSet;
    m_pSetItem = (PKSONITEM)pItemSet->pBuff;
    return 1;
}

int CKingJson::addNodeSet(void) {
    PNODESET pNodeSet = new nodeSet();
    pNodeSet->pBuff = (char*)malloc(sizeof(ksonNode) * m_nSetSize);
    if (pNodeSet->pBuff == NULL)
        return -1;
    memset(pNodeSet->pBuff, 0, sizeof(ksonNode) * m_nSetSize);
    if (m_pHeadNodeSet == NULL)
        m_pHeadNodeSet = pNodeSet;
    if (m_pWorkNodeSet != NULL)
        m_pWorkNodeSet->pNext = pNodeSet;
    m_pWorkNodeSet = pNodeSet;
    m_pSetNode = (PKSONNODE)pNodeSet->pBuff;
    return 1;
}

int CKingJson::relaseSets(void) {
    PITEMSET pSetItem = m_pHeadItemSet;
    PITEMSET pSetNext = NULL;
    while (pSetItem != NULL) {
        free(pSetItem->pBuff);
        pSetNext = pSetItem->pNext;
        delete pSetItem;
        pSetItem = pSetNext;
    }
    PNODESET pNodeSet = m_pHeadNodeSet;
    PNODESET pNodeNXt = NULL;
    while (pNodeSet != NULL) {
        free(pNodeSet->pBuff);
        pNodeNXt = pNodeSet->pNext;
        delete pNodeSet;
        pNodeSet = pNodeNXt;
    }
    m_pHeadItemSet = NULL;
    m_pWorkItemSet = NULL;
    m_pHeadNodeSet = NULL;
    m_pWorkNodeSet = NULL;
    m_nNodeCount = 0;
    m_nItemCount = 0;
    return 0;
}

int CKingJson::releaseAll(void) {
    releaseNode(&m_ksonRoot);
    relaseSets();
    if (m_pTextMem != NULL)
        delete[]m_pTextMem;
    m_pTextMem = NULL;
    if (m_pTextFmt != NULL)
        delete[]m_pTextFmt;
    m_pTextFmt = NULL;
    return 0;
}

int CKingJson::ksonCompNodeName(const void* arg1, const void* arg2) {
    PKSONNODE pNode1 = *(PKSONNODE*)arg1;
    PKSONNODE pNode2 = *(PKSONNODE*)arg2;
    if (pNode1->pName == NULL || pNode2->pName == NULL)
        return 1;
    return strcmp(pNode1->pName, pNode2->pName);
}

int CKingJson::ksonCompItemName(const void* arg1, const void* arg2) {
    PKSONITEM pItem1 = *(PKSONITEM*)arg1;
    PKSONITEM pItem2 = *(PKSONITEM*)arg2;
    if (pItem1->pName == NULL || pItem2->pName == NULL)
        return 1;
    return strcmp(pItem1->pName, pItem2->pName);
}

int CKingJson::ksonCompItemData(const void* arg1, const void* arg2) {
    PKSONITEM pItem1 = *(PKSONITEM*)arg1;
    PKSONITEM pItem2 = *(PKSONITEM*)arg2;
    if (pItem1->pData == NULL || pItem2->pData == NULL)
        return 1;
    return strcmp(pItem1->pData, pItem2->pData);
}
