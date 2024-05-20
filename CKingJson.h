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
    std::string             strName;
    std::string             strData;
    std::vector<kingJson*>  lstItem;
    std::vector<kingJson*>  lstNode;
    std::vector<kingJson*>  lstList;
    kingJson *              pParent;
    bool                    bString;
} *PKINGJSON;

class CKingJson {
public:
    CKingJson(void);
    virtual ~CKingJson(void);

    virtual int         ParseFile(const char* pFile, int nFlag);
    virtual int         ParseData(const char* pData, int nSize, int nFlag);

    virtual PKINGJSON   GetRoot(void) { return &m_kjsRoot; }
    virtual PKINGJSON   FindNode(PKINGJSON pNode, const char* pName, bool bChild);
    virtual PKINGJSON   FindItem(PKINGJSON pNode, const char* pName, bool bChild);
    virtual PKINGJSON   GetItem(PKINGJSON pNode, int nIndex);

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
    virtual PKINGJSON   AddItem(PKINGJSON pNode, int nIndex, const char* pValue, bool bString = true);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, int nIndex, int nValue);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, int nIndex, double dValue);
    virtual PKINGJSON   AddItem(PKINGJSON pNode, int nIndex, long long lValue);

    virtual int         DelNode(PKINGJSON pNode, const char* pName, bool bList);
    virtual int         DelItem(PKINGJSON pNode, const char* pName);
    virtual int         DelItem(PKINGJSON pNode, int nIndex);
    virtual int         DelNode(PKINGJSON pNode);

    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, const char* pValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, int nValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, double dValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, const char* pName, long long lValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, int nIndex, const char* pValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, int nIndex, int nValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, int nIndex, double dValue);
    virtual PKINGJSON   ModItem(PKINGJSON pNode, int nIndex, long long lValue);

    virtual const char* FormatText(void);
    virtual int         SaveToFile(const char * pFile);

protected:
    virtual PKINGJSON   findNode(PKINGJSON pNode, const char* pName, bool bChild);
    virtual PKINGJSON   findItem(PKINGJSON pNode, const char* pName, bool bChild);
    virtual int         delNode(PKINGJSON pNode, std::vector<kingJson*> * pList);

    virtual int         parseJson(PKINGJSON pNode, char* pData);
    virtual int         parseText(char * pText, char** ppNext);
    virtual int         parseData(char* pText, char** ppNext);
    virtual PKINGJSON   apendNode(PKINGJSON pNode, const char* pName, bool bList);
    virtual PKINGJSON   apendItem(PKINGJSON pNode, const char* pName, const char* pData, bool bString);

    virtual int         formatText(PKINGJSON pNode, bool bList, bool bLast);
    virtual int         removeChar(const char * pData, char * pText, int nSize);
    virtual bool        isNodeList(PKINGJSON pNode);
    virtual int         relaseNode(PKINGJSON pNode);

protected:
    kingJson        m_kjsRoot;
    std::string     m_strText;
};

#endif // __CKingJson_H__
