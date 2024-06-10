// 2024-06-07		kacha video			Create file

#ifndef __CBangJson_H__
#define __CBangJson_H__
#include <string>
#include <vector>

#define BSON_TYPE_TEXT          0X0001
#define BSON_TYPE_BOOL          0X0002
#define BSON_TYPE_NULL          0X0004

#define BSON_TYPE_NODE          0X0010
#define BSON_TYPE_LIST          0X0020
#define BSON_TYPE_ITEM          0X0040

#define BSON_TYPE_NAME          0X1000
#define BSON_TYPE_DATA          0X2000

#define BSON_TYPE_UTF8          0X4000

#define BSON_MAX_DEEPS          2000

typedef struct bsonNode {
    std::string             strName;
    std::string             strData;
    std::vector<bsonNode*>  lstNode;
    bsonNode*               pParent = NULL;
    uint32_t                uFlag   = BSON_TYPE_TEXT | BSON_TYPE_NODE;
} *PBSONNODE;

class CBangJson {
public:
    CBangJson(void);
    virtual ~CBangJson(void);

    virtual PBSONNODE   Parser(const char* pData, int nSize, int nFlag);
    virtual PBSONNODE   Search(PBSONNODE pNode, const char* pName, bool bChild);
    virtual const char* Format(PBSONNODE pNode, int* pSize);
    virtual int         Delete(PBSONNODE pNode);

protected:
    virtual int         parserData(PBSONNODE pNode, char* pData);
    virtual PBSONNODE   createNode(PBSONNODE pNode, char* pName, int nNameLen, char * pData, int nDataLen, int nType);
    virtual int         convertTxt(std::string& strText, char* pData, int nSize);
    virtual uint32_t    convertNum(const char* pText);
    virtual PBSONNODE   searchNode(PBSONNODE pNode, const char* pName, bool bChild);
    virtual int         formatJson(PBSONNODE pNode);
    virtual int         deleteNode(PBSONNODE pNode);

protected:
    PBSONNODE       m_pRoot = NULL;
    std::string     m_sText;
    int             m_nDeep = 0;
};

#endif // __CBangJson_H__
