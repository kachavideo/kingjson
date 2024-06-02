/*******************************************************************************
    File:		CKingJson.h

    Contains:	the king json class header file.

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#ifndef __CKingJson_H__
#define __CKingJson_H__
#include <stdlib.h>
#include <string.h>

#define KSON_TYPE_TEXT    0X0001
#define KSON_TYPE_LIST    0X0001

#define KSON_SET_COUNT    32768
#define KSON_NUM_MAX      0X7FFFFFFF

#define KSON_SKIP_FORMAT_CHAR while(*pText == ' ' || *pText == '\r' || *pText == '\n' || *pText == '\t')pText++
#define KSON_FIND_NEXTOF_TEXT pNext = ++pText; while(*pNext != '\"' || *(pNext - 1) == '\\') { pNext++; } *pNext++ = 0;
#define KSON_FIND_NEXTOF_DATA pNext = pText+1; while (*pNext != ',' && *pNext != '}' && *pNext != ']') { pNext++; }
#define KSON_FILL_FORMAT_TEXT *pTextPos++ = '\"'; while (*pTextTmp != 0) *pTextPos++ = *pTextTmp++; *pTextPos++ = '\"'; *pTextPos++ = ':';

typedef struct ksonItem {
    char *      pName = NULL;
    char *      pData = NULL;
    int         nFlag = KSON_TYPE_TEXT;
    ksonItem*   pNext = NULL;
} *PKSONITEM;

typedef struct ksonNode {
    char *      pName = NULL;
    int         nFlag = 0;
    ksonItem*   pHead = NULL;
    ksonItem*   pItem = NULL;
    ksonNode*   pNode = NULL;
    ksonNode*   pPrev = NULL;
    ksonNode*   pNext = NULL;
    ksonNode*   pPrnt = NULL;
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

    virtual const char* GetValue(PKSONNODE pNode, const char* pName);
    virtual const char* GetValue(PKSONNODE pNode, const char* pName, const char * pDefault);
    virtual int         GetValueInt(PKSONNODE pNode, const char* pName, int nDefault);
    virtual double      GetValueDbl(PKSONNODE pNode, const char* pName, double dDefault);
    virtual long long   GetValueLng(PKSONNODE pNode, const char* pName, long long lDefault);
    virtual bool        IsValueTrue(PKSONNODE pNode, const char* pName, bool bTure);
    virtual bool        IsValueNull(PKSONNODE pNode, const char* pName, bool bNull);

    virtual int         GetListNum(PKSONNODE pNode);
    virtual int         GetItemNum(PKSONNODE pNode);
    virtual const char* GetItemTxt(PKSONNODE pNode, int nIndex, const char* pDefault);
    virtual int         GetItemInt(PKSONNODE pNode, int nIndex, int nDefault);
    virtual double      GetItemDbl(PKSONNODE pNode, int nIndex, double dDefault);
    virtual long long   GetItemLng(PKSONNODE pNode, int nIndex, long long lDefault);

    virtual PKSONNODE   AddNode(PKSONNODE pNode, const char* pName, bool bList);
    virtual PKSONITEM   AddItem(PKSONNODE pNode, const char* pName, const char * pValue);
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
    virtual int         ModItem(PKSONITEM pItem, const char * pNewName);

    virtual int         SortItem(PKSONNODE pNode, bool bName, bool bASC);
    virtual int         SortNode(PKSONNODE pNode, bool bASC);

    virtual PKSONITEM   GetHeadItem(PKSONNODE pNode);
    virtual PKSONNODE   GetHeadNode(PKSONNODE pNode);
    virtual PKSONITEM   GetTailItem(PKSONNODE pNode);
    virtual PKSONNODE   GetTailNode(PKSONNODE pNode);

    virtual const char* FormatText(PKSONNODE pNode, int * pSize);
    virtual int         SaveToFile(PKSONNODE pNode, const char * pFile);

protected:
    virtual PKSONNODE   openFile(FILE * hFile, int nFlag);
    virtual PKSONNODE   findNode(PKSONNODE pNode, const char* pName, bool bChild);
    virtual PKSONITEM   findItem(PKSONNODE pNode, const char* pText, bool bName, bool bChild);
    virtual int         deleteNode(PKSONNODE pNode);
    virtual int         deleteItem(PKSONNODE pNode, PKSONITEM pItem);
    virtual PKSONNODE   addNewNode(PKSONNODE pNode, const char* pName, bool bList);
    virtual PKSONITEM   addNewItem(PKSONNODE pNode, const char* pName, const char* pData, bool bText, int nIndex);
    virtual int         modifyItem(PKSONITEM pItem, const char * pText, bool bName);

    virtual int         parseJson(PKSONNODE pNode, char* pData);
    virtual PKSONNODE   appendNode(PKSONNODE pNode, char* pName, bool bList);
    virtual PKSONITEM   appendItem(PKSONNODE pNode, char* pName, char* pData, bool bText);

    virtual int         formatText(PKSONNODE pNode);
    virtual int         getTextSize(PKSONNODE pNode);
    virtual int         relaseNode(PKSONNODE pNode);
    virtual int         addItemSet(void);
    virtual int         addNodeSet(void);
    virtual int         relaseSets(void);

    static int          ksonCompNodeName(const void* arg1, const void* arg2);
    static int          ksonCompItemName(const void* arg1, const void* arg2);
    static int          ksonCompItemData(const void* arg1, const void* arg2);
    static int          ksonStrLen(const char* pText);

protected:
    typedef struct itemSet {
        int         nSize = KSON_SET_COUNT;
        char*       pBuff = NULL;
        PKSONITEM*  pItem = NULL;
        itemSet*    pNext = NULL;
    } *PITEMSET;
    typedef struct nodeSet {
        int         nSize = KSON_SET_COUNT;
        char*       pBuff = NULL;
        PKSONNODE*  pNode = NULL;
        nodeSet*    pNext = NULL;
    } *PNODESET;

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
