/*******************************************************************************
    File:		CKingJson.h

    Contains:	the king json class header file.

    Written by:	kacha video

    Change History (most recent first):
    2024-05-05		kacha video			Create file

*******************************************************************************/
#ifndef __CKingJson_H__
#define __CKingJson_H__
#include <string>
#include <vector>

typedef struct kingJson {
    kingJson *              pParent;
    std::string             strName;
    std::string             strData;
    std::vector<kingJson>   lstItem;
    std::vector<kingJson>   lstNode;
    std::vector<kingJson>   lstList;
} *PKINGJSON;

class CKingJson {
public:
    CKingJson(void);
    virtual ~CKingJson(void);

    virtual int         ParseFile(const char* pFile, int nFlag);
    virtual int         ParseData(const char* pData, int nSize);

    virtual PKINGJSON   GetRoot(void) { return &m_kjsRoot; }
    virtual PKINGJSON   FindNode(PKINGJSON pNode, const char* pName, bool bChild);
    virtual PKINGJSON   FindItem(PKINGJSON pNode, const char* pName, bool bChild);

    virtual const char* GetValue(PKINGJSON pNode, const char* pName);
    virtual const char* GetValue(PKINGJSON pNode, const char* pName, const char * pDefault);
    virtual int         GetValueInt(PKINGJSON pNode, const char* pName, int nDefault);
    virtual double      GetValueDbl(PKINGJSON pNode, const char* pName, double dDefault);
    virtual long long   GetValueLng(PKINGJSON pNode, const char* pName, long long lDefault);

    virtual int         GetItemNum(PKINGJSON pNode);
    virtual const char* GetItemTxt(PKINGJSON pNode, int nIndex, const char* pDefault);
    virtual int         GetItemInt(PKINGJSON pNode, int nIndex, int nDefault);
    virtual double      GetItemDbl(PKINGJSON pNode, int nIndex, double dDefault);
    virtual long long   GetItemLng(PKINGJSON pNode, int nIndex, long long lDefault);


    virtual PKINGJSON   AddNode(PKINGJSON pNode, const char* pName, bool bList);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, const char* pName, const char * pValue);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, const char* pName, int nValue);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, const char* pName, double dValue);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, const char* pName, long long lValue);

    virtual int         DelNode(PKINGJSON pNode, const char* pName, bool bList);
    virtual int         DelItem(PKINGJSON pNode, const char* pName);

    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, const char* pValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, int nValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, double dValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, long long lValue);

    virtual const char* FormatText(void);
    virtual int         SaveToFile(const char * pFile);

protected:
    PKINGJSON        findNode(PKINGJSON pNode, const char* pName, bool bChild);
    PKINGJSON        findItem(PKINGJSON pNode, const char* pName, bool bChild);

    virtual int     parseNode(PKINGJSON pNode, char* pData, int nSize, bool bList);
    virtual int     fillValue(std::string& strText, const char* pText);

    virtual int     formatText(PKINGJSON pNode, bool bList, bool bLast);

protected:
    kingJson        m_kjsRoot;
    std::string     m_strText;
};

#endif // __CKingJson_H__
