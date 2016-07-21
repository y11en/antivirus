#ifndef __BASE64P_H_
#define __BASE64P_H_

#if !defined(__cplusplus)
#if !defined(bool)
#define bool char
#define true 1
#define false 0
#endif
#endif 

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#if defined(__cplusplus)
extern "C"{
#endif

void InitBase64P(void);
bool DecodeBase64PackedLong(unsigned long* pnValue, unsigned char* pBuff, size_t nBuffSize, size_t* pnBytesRead);
bool EncodeBase64PackedLong(unsigned long nValue, unsigned char* pBuff, size_t nBuffSize, size_t* pnBytesWritten);
bool DecodeBase64(const unsigned char* pBase64, size_t nBase64Size, unsigned char* pBuff, size_t nBuffSize, size_t* pnBytes);
bool EncodeBase64(const unsigned char* pBuff, size_t nBuffSize, unsigned char* pBase64, size_t nBase64Size, size_t* pnBytes, bool bUsePadding);

#if defined(__cplusplus)
} // extern "C"
#endif


#endif // __BASE64P_H_



