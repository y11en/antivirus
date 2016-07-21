#ifndef _CodePages_h_
#define _CodePages_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <_include/language.h>

int ConvertUTF(unsigned char * pData, int nLen/*=-1*/, int nDestCodePage);

int ConvertData(unsigned char* pData, int nLen/*=-1*/, 
                int nSourceCodePage,  int nDestCodePage );

int ConvertString( char *pData, int nLen/*=-1*/, 
                   int nSourceCodePage, int nDestCodePage );
/* All ConvertXXX() functions return destination length */

int CPstr2i(const char* pszCP);  /* short name to cpID */
const char* CPi2str(int n);      /* cpID to short name */
const char* CPi2strRFC(int n);   /* cpID to RFC name */
int CPname2i(const char* pszCP); /* any name to cpID */

const unsigned char* CP2CP_Rus(int nSourceCodePage, int nDestCodePage);
                                 /* Russian 8 bit codepages conversion */

unsigned char Unicode2cp(unsigned wc, int nDestCodePage);

unsigned CP2Unicode( wchar_t tab[256], unsigned char c);
#define CP2Unicode(tab,c) ((tab)[(unsigned char)(c)])

#ifdef __cplusplus
}
#endif

#endif
