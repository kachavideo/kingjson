/*******************************************************************************
    File:		CKingJson.cpp

    Contains:	the king json class implement code

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "CKingJson.h"

CKingJson::CKingJson(void) {
}

CKingJson::~CKingJson(void) {
    relaseNode(&m_ksonRoot);
    relaseSets();
    if (m_pTextMem != NULL)
        delete[]m_pTextMem;
    if (m_pTextFmt != NULL)
        delete[]m_pTextFmt;
}

PKSONNODE CKingJson::ParseData(const char* pData, int nSize, int nFlag) {
    relaseNode(&m_ksonRoot);
    relaseSets();
    if (pData != m_pTextMem) {
        if (m_pTextMem != NULL)
            delete[]m_pTextMem;
        m_nTextNum = nSize + 1;
        m_pTextMem = new char[m_nTextNum];
        memcpy(m_pTextMem, pData, nSize);
        m_pTextMem[nSize] = 0;
    }
    char* pBuff = m_pTextMem;
    if (m_pTextMem[0] == (char)0XEF && m_pTextMem[1] == (char)0XBB && m_pTextMem[2] == (char)0XBF)
        pBuff += 3; nSize -= 3;
    if (pBuff[0] == '[')
        m_ksonRoot.nFlag = KSON_TYPE_LIST;
    int nRC = parseJson(&m_ksonRoot, pBuff);
    if (nRC <= 0 || (nFlag == 1 && (nSize - nRC) > 4)) 
        return NULL;
    m_nTextAlloc = nSize;
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
#endif // WIN32
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

const char* CKingJson::GetValue(PKSONNODE pNode, const char* pName) {
    return GetValue(pNode, pName, NULL);
}

const char* CKingJson::GetValue(PKSONNODE pNode, const char* pName, const char* pDefault) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    PKSONITEM pItem = findItem(pNode, pName, true, false);
    return pItem != NULL ? (pItem->pData == NULL ? pDefault : pItem->pData) : pDefault;
}

int CKingJson::GetValueInt(PKSONNODE pNode, const char* pName, int nDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? nDefault : atoi(pValue);
}

double CKingJson::GetValueDbl(PKSONNODE pNode, const char* pName, double dDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? dDefault : atof(pValue);
}

long long CKingJson::GetValueLng(PKSONNODE pNode, const char* pName, long long lDefault) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? lDefault : atoll(pValue);
}

bool CKingJson::IsValueTrue(PKSONNODE pNode, const char* pName, bool bTure) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? bTure : !strcmp(pValue, "true");
}

bool CKingJson::IsValueNull(PKSONNODE pNode, const char* pName, bool bNull) {
    const char* pValue = GetValue(pNode, pName, NULL);
    return pValue == NULL ? bNull : !strcmp(pValue, "null");
}

int CKingJson::GetListNum(PKSONNODE pNode) {
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

PKSONNODE CKingJson::AddNode(PKSONNODE pNode, const char* pName, bool bList) {
    return addNewNode(pNode, (char *)pName, bList);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, const char* pValue) {
    return addNewItem(pNode, pName, pValue, true, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, int nValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%d", nValue);
    return addNewItem(pNode, pName, szValue, false, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, double dValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%f", dValue);
    return addNewItem(pNode, pName, szValue, false, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, const char* pName, long long lValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%lld", lValue);
    return addNewItem(pNode, pName, szValue, false, KSON_NUM_MAX);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, const char* pValue, bool bText) {
    if (pValue == NULL) return NULL;
    return addNewItem(pNode, NULL, pValue, bText, nIndex);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, int nValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%d", nValue);
    return addNewItem(pNode, NULL, szValue, false, nIndex);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, double dValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%f", dValue);
    return addNewItem(pNode, NULL, szValue, false, nIndex);
}

PKSONITEM CKingJson::AddItem(PKSONNODE pNode, int nIndex, long long lValue) {
    char szValue[32]; snprintf(szValue, sizeof(szValue), "%lld", lValue);
    return addNewItem(pNode, NULL, szValue, false, nIndex);
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
        *m_pTextPos++ = (pNode->nFlag & KSON_TYPE_LIST) ? '[' : '{';
    formatText(pNode);
    if (pNode != &m_ksonRoot)
        *m_pTextPos++ = (pNode->nFlag & KSON_TYPE_LIST) ? ']' : '}';
    if (pSize != NULL)
        *pSize = (int)(m_pTextPos - m_pTextFmt);
    return m_pTextFmt;
}

int CKingJson::SaveToFile(PKSONNODE pNode, const char* pFile) {
    FILE* hFile = fopen(pFile, "wb");
    if (hFile == NULL)
        return -1;
    int nJsonSize = 0;
    const char * pFmtText = FormatText(pNode, &nJsonSize);
    if (pFmtText == NULL)
        return -1;
    int nUtf8 = 0X00BFBBEF;
    size_t nWrite = fwrite(&nUtf8, 1, 3, hFile);
    if (nWrite > 0)
        fwrite(pFmtText, 1, nJsonSize, hFile);
    fclose(hFile);
    return 1;
}

PKSONNODE CKingJson::openFile(FILE * hFile, int nFlag) {
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
    return ParseData((const char *)m_pTextMem, nSize, nFlag);
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
    relaseNode(pNode);
    return 1;
}

int CKingJson::deleteItem(PKSONNODE pNode, PKSONITEM pItem) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    if (pItem == NULL) return - 1;
    PKSONITEM pHead = GetHeadItem(pNode);
    if (pHead == NULL || pHead->pNext == NULL) {
        if (pHead != pItem)
            return -1;
        if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ksonStrLen(pItem->pName); delete[]pItem->pName;
        }
        if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ksonStrLen(pItem->pData); delete[]pItem->pData;
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
                m_nTextAlloc -= ksonStrLen(pItem->pName); delete[]pItem->pName;
            }
            if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
                m_nTextAlloc -= ksonStrLen(pItem->pData); delete[]pItem->pData;
            }
            return 1;
        }
        pPrev = pHead; pHead = pHead->pNext;
    }
    return -1;
}

PKSONNODE CKingJson::addNewNode(PKSONNODE pNode, const char* pName, bool bList) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    if ((m_nNodeCount % KSON_SET_COUNT) == 0)
        addNodeSet();
    PKSONNODE pNewNode = m_pWorkNodeSet->pNode[(m_nNodeCount++) % KSON_SET_COUNT];
    if (pName != NULL) {
        int nSize = (int)strlen(pName);
        pNewNode->pName = new char[nSize + 1]; 
        pNewNode->pName[nSize] = 0;
        memcpy(pNewNode->pName, pName, nSize);
        m_nTextAlloc += (nSize + 4);
    }
    pNewNode->pPrnt = pNode;
    pNewNode->nFlag = bList;
    PKSONNODE pTaiNode = GetTailNode(pNode);
    pNewNode->pPrev = pTaiNode;
    if (pTaiNode == NULL)
        pNode->pNode = pNewNode;
    else 
        pTaiNode->pNext = pNewNode;
    return pNewNode;
}

PKSONITEM CKingJson::addNewItem(PKSONNODE pNode, const char* pName, const char* pData, bool bText, int nIndex) {
    if (pNode == NULL) pNode = &m_ksonRoot;
    if ((m_nItemCount % KSON_SET_COUNT) == 0)
        addItemSet();
    PKSONITEM pNewItem = m_pWorkItemSet->pItem[(m_nItemCount++) % KSON_SET_COUNT];
    pNewItem->nFlag = bText;
    if (pName != NULL) {
        int nSize = (int)strlen(pName);
        pNewItem->pName = new char[nSize + 1];
        pNewItem->pName[nSize] = 0;
        memcpy(pNewItem->pName, pName, nSize);
        m_nTextAlloc += (nSize + 4);
    }
    if (pData != NULL) {
        int nSize = (int)strlen(pData);
        pNewItem->pData = new char[nSize + 1];
        pNewItem->pData[nSize] = 0;
        memcpy(pNewItem->pData, pData, nSize);
        m_nTextAlloc += (nSize + 4);
    }
    PKSONITEM pPosItem = GetHeadItem(pNode);
    if (pPosItem == NULL) {
        pNode->pHead = pNewItem;
    }
    else if (nIndex <= 0) {
        pNewItem->pNext = pNode->pHead;
        pNode->pHead = pNewItem;
    }
    else if (nIndex == KSON_NUM_MAX) {
        pPosItem = GetTailItem(pNode);
        pPosItem->pNext = pNewItem;
    }
    else {    
        while (pPosItem != NULL) {
            if (--nIndex == 0) {
                pNewItem->pNext = pPosItem->pNext;
                pPosItem->pNext = pNewItem;
                break;
            }
            pPosItem = pPosItem->pNext;
        }
        if (pPosItem == NULL) {
            pPosItem = GetTailItem(pNode);
            pPosItem->pNext = pNewItem;
        }
    }
    pNode->pItem = pNewItem;
    return pNewItem;
}

int CKingJson::modifyItem(PKSONITEM pItem, const char* pText, bool bName) {
    if (pItem == NULL)
        return -1;
    if (bName) {
        if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ksonStrLen(pItem->pName); delete[]pItem->pName;
        }
        int nSize = (int)strlen(pText);
        pItem->pName = new char[nSize + 1];
        pItem->pName[nSize] = 0;
        memcpy(pItem->pName, pText, nSize);
        m_nTextAlloc += (nSize + 4);
    }
    else {
        if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ksonStrLen(pItem->pData); delete[]pItem->pData;
        }
        int nSize = (int)strlen(pText);
        pItem->pData = new char[nSize + 1];
        pItem->pData[nSize] = 0;
        memcpy(pItem->pData, pText, nSize);
        m_nTextAlloc += (nSize + 4);
    }
    return 1;
}

int CKingJson::parseJson(PKSONNODE pNode, char* pData) {
    char*   pText = pData;
    char*   pNext = pData;
    char*   pName = NULL;
    bool    bList = false;
    int     nErr  = 0;
    KSON_SKIP_FORMAT_CHAR;
    if (*pText != '{' && *pText != '[')
        return -1;
    bList = *pText == '[' ? true : false;
    pText++;
    while (*pText != 0) {
        KSON_SKIP_FORMAT_CHAR;
        if (*pText == '\"') {
            KSON_FIND_NEXTOF_TEXT;
            if (bList) {
                if (appendItem(pNode, NULL, pText, true) == NULL)
                    return -1;
                pText = (*pNext == ',') ? pNext + 1 : pNext;
                KSON_SKIP_FORMAT_CHAR;
            }
            else {
                if (*pNext++ != ':')
                    return -1;
                pName = pText;
                pText = pNext;
                KSON_SKIP_FORMAT_CHAR;
                if (*pText == '\"') {
                    KSON_FIND_NEXTOF_TEXT;
                    if (appendItem(pNode, pName, pText, true) == NULL)
                        return -1;
                    pText = pNext;
                    if (*pNext == ',') {
                        *pNext = 0; pText = pNext + 1;
                    }
                    KSON_SKIP_FORMAT_CHAR;
                }
                else if (*pText == '{' || *pText == '[') {
                    PKSONNODE pNewNode = appendNode(pNode, pName, *pText == '[');
                    if (pNewNode == NULL)
                        return -1;
                    nErr = parseJson(pNewNode, pText);
                    if (nErr < 0)
                        return nErr;
                    pText += nErr;
                }
                else if (*pText == '}' || *pText == ']') {
                    *pText = 0;
                    KSON_SKIP_FORMAT_CHAR;
                    return (int)((pText - pData) + (*(pText + 1) == ',' ? 2 : 1));
                }
                else {
                    KSON_FIND_NEXTOF_DATA;
                    if (appendItem(pNode, pName, pText, false) == NULL)
                        return -1;
                    pText = pNext;
                    if (*pNext == ',') {
                        *pNext = 0; pText = pNext + 1;
                    }
                }
            }
        }
        else if (*pText == '{' || *pText == '[') {
            PKSONNODE pNewNode = appendNode(pNode, pName, *pText == '[');
            if (pNewNode == NULL)
                return -1;
            nErr = parseJson(pNewNode, pText);
            if (nErr <= 0)
                return -1;
            pText += nErr;
        }
        else if (*pText == '}' || *pText == ']') {
            *pText = 0;
            return (int)((pText - pData) + (*(pText + 1) == ',' ? 2 : 1));
        }
        else {
            KSON_FIND_NEXTOF_DATA;
            if (appendItem(pNode, pName, pText, false) == NULL)
                return -1;
            pText = pNext;
            if (*pNext == ',') {
                *pNext = 0; pText = pNext + 1;
            }
        }
    }
    return (int)(pText - pData);
}

PKSONITEM CKingJson::appendItem(PKSONNODE pNode, char* pName, char* pData, bool bText) {
    if ((m_nItemCount % KSON_SET_COUNT) == 0) 
        if (addItemSet() < 0) return NULL;
    PKSONITEM pNewItem = m_pWorkItemSet->pItem[(m_nItemCount++) % KSON_SET_COUNT];
    pNewItem->pName = pName;
    pNewItem->pData = pData;
    pNewItem->nFlag = bText;
    if (pNode->pHead == NULL)
        pNode->pHead = pNewItem;
    if (pNode->pItem != NULL) 
        pNode->pItem->pNext = pNewItem;
    pNode->pItem = pNewItem;
    return pNewItem;
}

PKSONNODE CKingJson::appendNode(PKSONNODE pNode, char* pName, bool bList) {
    if ((m_nNodeCount % KSON_SET_COUNT) == 0) 
        if (addNodeSet() < 0) return NULL;
    PKSONNODE pNewNode = m_pWorkNodeSet->pNode[(m_nNodeCount++) % KSON_SET_COUNT];
    pNewNode->pName = pName;
    pNewNode->pPrnt = pNode;
    pNewNode->nFlag = bList;
    if (pNode->pNode != NULL)
        pNode->pNode->pNext = pNewNode;
    pNewNode->pPrev = pNode->pNode;
    pNode->pNode = pNewNode;
    return pNewNode;
}

int CKingJson::formatText(PKSONNODE pNode) {
    char* pTextTmp = NULL;
    char* pTextPos = m_pTextPos;
    bool  bList    = pNode->nFlag & KSON_TYPE_LIST;
    if (pNode->pName != NULL) {
        pTextTmp = pNode->pName;
        KSON_FILL_FORMAT_TEXT
    }
    *pTextPos++ = bList ? '[' : '{';

    PKSONITEM pItem = pNode->pHead;
    while (pItem != NULL) {
        if (pItem->pName != NULL) {
            pTextTmp = pItem->pName;
            KSON_FILL_FORMAT_TEXT
        }
        if (pItem->pData != NULL) {
            if (pItem->nFlag & KSON_TYPE_TEXT) 
                *pTextPos++ = '\"';
            pTextTmp = pItem->pData;
            while (*pTextTmp != 0)
                *pTextPos++ = *pTextTmp++;
            if (pItem->nFlag & KSON_TYPE_TEXT) 
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
    *m_pTextPos++ = bList ? ']' : '}';
    if (pNode != m_pTxtNode && pNode->pNext != NULL)
        *m_pTextPos++ = ',';
    return 0;
}

int CKingJson::getTextSize(PKSONNODE pNode) {
    int nTextSize = 0;
    if (pNode->pName != NULL)
        nTextSize += (ksonStrLen(pNode->pName) + 4);
    PKSONITEM pItem = GetHeadItem(pNode);
    while (pItem != NULL) {
        if (pItem->pName != NULL)
            nTextSize += (ksonStrLen(pItem->pName) + 4);
        nTextSize += (ksonStrLen(pItem->pData) + 4);
        pItem = pItem->pNext;
    }
    PKSONNODE pChild = GetHeadNode(pNode);
    while (pChild != NULL) {
        nTextSize += getTextSize(pChild);
        pChild = pChild->pNext;
    }
    return nTextSize;
}

int CKingJson::relaseNode(PKSONNODE pNode) {
    PKSONITEM pItem = pNode->pHead;
    while (pItem != NULL) {
        if (pItem->pName != NULL && (pItem->pName < m_pTextMem || pItem->pName > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ksonStrLen(pItem->pName); delete[]pItem->pName; 
        }
        if (pItem->pData != NULL && (pItem->pData < m_pTextMem || pItem->pData > m_pTextMem + m_nTextNum)) {
            m_nTextAlloc -= ksonStrLen(pItem->pData); delete[]pItem->pData;
        }
        pItem = pItem->pNext;
    }
    pNode->pHead = NULL;

    if (pNode->pName != NULL && (pNode->pName < m_pTextMem || pNode->pName > m_pTextMem + m_nTextNum)) {
        m_nTextAlloc -= ksonStrLen(pNode->pName); delete[]pNode->pName;
    }
    PKSONNODE pHead = GetHeadNode(pNode);
    while (pHead != NULL) {
        relaseNode(pHead);
        pHead = pHead->pNext;
    }
    pNode->pNode = NULL;
    return 0;
}

int CKingJson::addItemSet(void) {
    PITEMSET pItemSet = new itemSet();
    pItemSet->pBuff = (char*)malloc(sizeof(ksonItem) * pItemSet->nSize);
    if (pItemSet->pBuff == NULL)
        return -1;
    memset(pItemSet->pBuff, 0, sizeof(ksonItem) * pItemSet->nSize);
    pItemSet->pItem = new PKSONITEM[pItemSet->nSize];
    for (int i = 0; i < pItemSet->nSize; i++)
        pItemSet->pItem[i] = (PKSONITEM)(pItemSet->pBuff + sizeof(ksonItem) * i);
    if (m_pHeadItemSet == NULL)
        m_pHeadItemSet = pItemSet;
    if (m_pWorkItemSet != NULL)
        m_pWorkItemSet->pNext = pItemSet;
    m_pWorkItemSet = pItemSet;
    return 1;
}

int CKingJson::addNodeSet(void) {
    PNODESET pNodeSet = new nodeSet();
    pNodeSet->pBuff = (char*)malloc(sizeof(ksonNode) * pNodeSet->nSize);
    if (pNodeSet->pBuff == NULL)
        return -1;
    memset(pNodeSet->pBuff, 0, sizeof(ksonNode) * pNodeSet->nSize);
    pNodeSet->pNode = new PKSONNODE[pNodeSet->nSize];
    for (int i = 0; i < pNodeSet->nSize; i++)
        pNodeSet->pNode[i] = (PKSONNODE)(pNodeSet->pBuff + sizeof(ksonNode) * i);
    if (m_pHeadNodeSet == NULL)
        m_pHeadNodeSet = pNodeSet;
    if (m_pWorkNodeSet != NULL)
        m_pWorkNodeSet->pNext = pNodeSet;
    m_pWorkNodeSet = pNodeSet;
    return 1;
}

int CKingJson::relaseSets(void) {
    PITEMSET pSetItem = m_pHeadItemSet;
    PITEMSET pSetNext = NULL;
    while (pSetItem != NULL) {
        delete[]pSetItem->pItem;
        free(pSetItem->pBuff);
        pSetNext = pSetItem->pNext;
        delete pSetItem;
        pSetItem = pSetNext;
    }
    PNODESET pNodeSet = m_pHeadNodeSet;
    PNODESET pNodeNXt = NULL;
    while (pNodeSet != NULL) {
        delete[]pNodeSet->pNode;
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

int CKingJson::ksonStrLen(const char* pText) {
    register int*   pTail = (int*)pText;
    register int    nText = 0;
    register int    nSize = 0;
    while (true) {
        nText = *pTail;
        if (!(nText & 0xff000000) || !(nText & 0xff0000) || !(nText & 0xff00) || !(nText & 0xff))
            break;
        nSize += 4; pTail++;
    }
    pText = (const char*)pTail;
    while (*pText++ != 0) nSize++;
    return nSize;
}
