// 2024-06-07		kacha video			Create file

#include "UTextFunc.h"

uint32_t bsonConvertNum(const char* pText) {
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

std::string bsonConvertTxt(const char* pData, int nSize) {
    const char* pText = pData;
    uint32_t    uWord = 0;
    uint32_t    uNext = 0;
    uint8_t     uMark = 0;
    uint8_t     uPost = 0;
    int         nUtf8 = 0;
    std::string strText;
    while (pText - pData < nSize) {
        if (*pText == '\\' && *(pText + 1) == 'u') {
            uWord = bsonConvertNum(pText +2); pText += 6;
            if ((uWord >= 0xDC00) && (uWord <= 0xDFFF)) {
                strText += std::string(pText, 6);
                pText += 6; 
                continue;
            }
            else if ((uWord >= 0xD800) && (uWord <= 0xDBFF)) {
                uNext = bsonConvertNum(pText + 2); 
                if ((uWord >= 0xDC00) && (uWord <= 0xDFFF)) {
                    strText += std::string(pText, 6);
                    pText += 6;
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
            if (nUtf8 > 1)
                strText += (char)((uWord | uMark) & 0xFF);
            else
                strText += (char)(uWord & 0x7F);
            for (uPost = 1;  uPost < nUtf8; uPost++) {
                strText += (char)((uWord | 0x80) & 0xBF);
                uWord >>= 6;
            }
            pText += nUtf8;
        }
        else {
            strText += std::string(pText++, 1);
        }
    }
    return strText;
}
