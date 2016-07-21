#ifndef __TSTRING_H
#define __TSTRING_H

#ifdef UNICODE
#include <tchar.h>
#define __T(x)      L ## x
#define _T(x)       __T(x)
typedef wchar_t tchar;
#else
#define __T(x)      x
#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)

typedef char tchar;
#endif

typedef tchar*       tstring;
typedef const tchar* tcstring;

//size_t  tcharlenb( tcstring string );
#define tcharlenb(string) (sizeof(tchar))
//tchar   tstrchar ( tcstring string );
#define tstrchar(string) ((string)[0])
size_t  tstrlenb ( tcstring string );
size_t  tstrlenbz( tcstring string );
size_t  tstrlen  ( tcstring string );
tstring tstrcpy  ( tstring dst, tcstring src );
tstring tstrncpy  ( tstring dst, tcstring src, size_t size );
tstring tstrcat  ( tstring dst, tcstring src );
int     tstrcmp  ( tcstring string1, tcstring string2 );
int     tstrncmp ( tcstring string1, tcstring string2, size_t count );
int     tstrcmpbegin( tcstring string1, tcstring strbegin );
bool    tstrcmpinc( tcstring* pstring1, tcstring strcmp );
tstring tstrupr  ( tstring string );
tcstring tstrchr  ( tcstring string, tchar ch );
tcstring tstrrchr ( tcstring string, tchar ch );
tcstring tstrstr  ( tcstring string1, tcstring string2 );
tstring tstrdup  ( tcstring string );
tcstring tstrgetlastchar( tcstring string );
tstring tstrappend( tstring string, tcstring append ); // this function will realloc string!!!
tstring tstralloc( size_t chars );
void    tstrfree ( tcstring string );

#define tstrdec(start, ptr) ((start)>=(ptr) ? NULL : (ptr)-1)
#define tstrinc(ptr)    ((ptr)+1)
#define tstrninc(ptr, n)    ((ptr)+(n))

int     tstricmp  ( tcstring string1, tcstring string2 );
int     tstrnicmp ( tcstring string1, tcstring string2, size_t count );

bool tstrMatchWithPattern(tcstring string, tcstring pattern_ucase, bool case_sensetivity);

char* tstrchrA( const char* string, char s );
size_t tstrlenA(const char* string);
int tstrnicmpA( const char* string1, const char* string2, size_t count );
char* tstrichrA( const char* string, char s );
const char* tstristrA( const char* string1, const char* string2 );

#endif // __TSTRING_H
