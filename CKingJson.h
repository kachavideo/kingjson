/*******************************************************************************
    File:		CKingJson.h

    Contains:	the king json class header file.

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#ifndef __CKingJson_H__
#define __CKingJson_H__
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define KSON_DATA_TEXT          0X10000000
#define KSON_NODE_LIST          0X20000000
#define KSON_TEXT_UTF8          0X40000000
#define KSON_NUM_MAX            0X7FFFFFFF
#define KSON_MAX_DEEPS          2048
#define KSON_VECTOR_SIZE        (sizeof(__m128i))

typedef struct ksonItem {
    char*     pName = NULL;
    char*     pData = NULL;
    int       nFlag = KSON_DATA_TEXT;
    ksonItem* pNext = NULL;
} *PKSONITEM;

typedef struct ksonNode {
    char*     pName = NULL;
    int       nFlag = 0;
    ksonItem* pHead = NULL;
    ksonItem* pItem = NULL;
    ksonNode* pNode = NULL;
    ksonNode* pPrev = NULL;
    ksonNode* pNext = NULL;
    ksonNode* pPrnt = NULL;
} *PKSONNODE;

class CKingJson {
public:
    CKingJson(void);
    virtual ~CKingJson(void);

    virtual PKSONNODE   ParseData(const char* pData, int nSize, int nFlag);
    virtual PKSONNODE   OpenFileA(const char* pFile, int nFlag);
    virtual PKSONNODE   OpenFileW(const wchar_t* pFile, int nFlag);

    virtual PKSONNODE   GetRoot(void) { return &m_ksonRoot; }
    virtual PKSONNODE   FindNode(PKSONNODE pNode, const char* pName, bool bChild);
    virtual PKSONITEM   FindItem(PKSONNODE pNode, const char* pName, bool bChild);
    virtual PKSONITEM   FindItem(PKSONNODE pNode, const char* pData);
    virtual PKSONITEM   GetItem(PKSONNODE pNode, int nIndex);
    virtual PKSONNODE   GetList(PKSONNODE pNode, int nIndex);

    virtual const char* GetValue(PKSONITEM pItem);
    virtual const char* GetValue(PKSONNODE pNode, const char* pName, bool bChild);
    virtual const char* GetValue(PKSONNODE pNode, const char* pName, bool bChild, const char* pDefault);
    virtual int         GetValueInt(PKSONNODE pNode, const char* pName, bool bChild, int nDefault);
    virtual double      GetValueDbl(PKSONNODE pNode, const char* pName, bool bChild, double dDefault);
    virtual long long   GetValueLng(PKSONNODE pNode, const char* pName, bool bChild, long long lDefault);
    virtual bool        IsValueTrue(PKSONNODE pNode, const char* pName, bool bChild, bool bTure);
    virtual bool        IsValueNull(PKSONNODE pNode, const char* pName, bool bChild, bool bNull);

    virtual int         GetNodeNum(PKSONNODE pNode);
    virtual int         GetItemNum(PKSONNODE pNode);
    virtual const char* GetItemTxt(PKSONNODE pNode, int nIndex, const char* pDefault);
    virtual int         GetItemInt(PKSONNODE pNode, int nIndex, int nDefault);
    virtual double      GetItemDbl(PKSONNODE pNode, int nIndex, double dDefault);
    virtual long long   GetItemLng(PKSONNODE pNode, int nIndex, long long lDefault);

    virtual PKSONNODE   AddNode(PKSONNODE pNode, const char* pName, const char* pJson, int nSize);
    virtual PKSONNODE   AddNode(PKSONNODE pNode, const char* pName, bool bList);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, const char* pName, const char* pValue);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, const char* pName, int nValue);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, const char* pName, double dValue);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, const char* pName, long long lValue);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, int nIndex, const char* pValue, bool bText = true);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, int nIndex, int nValue);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, int nIndex, double dValue);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, int nIndex, long long lValue);

    virtual int         DelNode(PKSONNODE pNode, const char* pName);
    virtual int         DelItem(PKSONNODE pNode, const char* pText, bool bName);
    virtual int         DelItem(PKSONNODE pNode, int nIndex);
    virtual int         DelNode(PKSONNODE pNode);

    virtual PKSONITEM   ModItem(PKSONNODE pNode, const char* pName, const char* pValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, const char* pName, int nValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, const char* pName, double dValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, const char* pName, long long lValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, int nIndex, const char* pValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, int nIndex, int nValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, int nIndex, double dValue);
    virtual PKSONITEM   ModItem(PKSONNODE pNode, int nIndex, long long lValue);
    virtual int         ModItem(PKSONITEM pItem, const char* pNewName);

    virtual int         SortItem(PKSONNODE pNode, bool bName, bool bASC);
    virtual int         SortNode(PKSONNODE pNode, bool bASC);

    virtual PKSONITEM   GetHeadItem(PKSONNODE pNode);
    virtual PKSONNODE   GetHeadNode(PKSONNODE pNode);
    virtual PKSONITEM   GetTailItem(PKSONNODE pNode);
    virtual PKSONNODE   GetTailNode(PKSONNODE pNode);

    virtual const char* FormatText(PKSONNODE pNode, int* pSize);
    virtual int         SaveToFile(PKSONNODE pNode, const char* pFile);

protected:
    PKSONNODE   openFile(FILE* hFile, int nFlag);
    PKSONNODE   findNode(PKSONNODE pNode, const char* pName, bool bChild);
    PKSONITEM   findItem(PKSONNODE pNode, const char* pText, bool bName, bool bChild);
    int         deleteNode(PKSONNODE pNode);
    int         deleteItem(PKSONNODE pNode, PKSONITEM pItem);
    PKSONNODE   parseNewJson(PKSONNODE pNode, char* pData, int nSize);
    PKSONNODE   addNewNode(PKSONNODE pNode, const char* pName, int nNameLen, int nType);
    PKSONITEM   addNewItem(PKSONNODE pNode, const char* pName, int nNameLen, const char* pData, int nDataLen, int nType, int nIndex);
    int         modifyItem(PKSONITEM pItem, const char* pText, bool bName);

    int                 parseJson(char* pData, int nSize);
    inline PKSONNODE    appendNode(PKSONNODE pNode, char* pName, int nNameLen, int nType);
    inline PKSONITEM    appendItem(PKSONNODE pNode, char* pName, int nNameLen, char* pData, int nDataLen, int nType);

    inline char*        skipFormat(char* pText, char* pLast);
    inline char*        searchName(char* pText, char* pLast);
    inline char*        searchText(char* pText, char* pLast);
    inline char*        searchData(char* pText, char* pLast);
    inline uint32_t     forwardPos(uint32_t mask);

    int         convertTxt(char* pData);
    uint32_t    convertNum(const char* pText);
    int         formatText(PKSONNODE pNode);
    int         getTextSize(PKSONNODE pNode);
    int         releaseNode(PKSONNODE pNode);
    int         addItemSet(void);
    int         addNodeSet(void);
    int         relaseSets(void);
    int         releaseAll(void);

    static int  ksonCompNodeName(const void* arg1, const void* arg2);
    static int  ksonCompItemName(const void* arg1, const void* arg2);
    static int  ksonCompItemData(const void* arg1, const void* arg2);

protected:
    typedef struct itemSet {
        char*    pBuff = NULL;
        itemSet* pNext = NULL;
    } *PITEMSET;
    PKSONITEM    m_pSetItem = NULL;

    typedef struct nodeSet {
        char*    pBuff = NULL;
        nodeSet* pNext = NULL;
    } *PNODESET;
    PKSONNODE    m_pSetNode = NULL;
    int          m_nSetSize = 2048;

protected:
    ksonNode        m_ksonRoot;
    ksonNode*       m_pTxtNode = NULL;
    char*           m_pTextMem = NULL;
    int             m_nTextNum = 0;

    char*           m_pTextFmt = NULL;
    char*           m_pTextPos = NULL;

    int             m_nTextAlloc   = 0;
    int             m_nItemCount   = 0;
    PITEMSET        m_pHeadItemSet = NULL;
    PITEMSET        m_pWorkItemSet = NULL;

    int             m_nNodeCount   = 0;
    PNODESET        m_pHeadNodeSet = NULL;
    PNODESET        m_pWorkNodeSet = NULL;
};

#endif // __CKingJson_H__
