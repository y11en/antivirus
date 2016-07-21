#ifndef __UTIL_H_8021537_
#define __UTIL_H_8021537_

#ifdef __cplusplus
extern "C" {
#endif


bool FastUni2Ansi(const wchar_t* sUniSrc, char* sAnsiDst, DWORD dwNumOfChars, DWORD dwDstBufCharSize = 0);
bool FastAnsi2Uni(const char* sAnsiSrc, wchar_t* sUniDst, DWORD dwNumOfChars, DWORD dwDstBufCharSize = 0);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif __UTIL_H_8021537_
