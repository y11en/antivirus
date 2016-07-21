#ifndef __KLSTD_TCHAR_H__
#define __KLSTD_TCHAR_H__

#include <wchar.h>

#ifdef _WIN32    
    #include <tchar.h>
#endif

#ifdef __unix__
  
typedef wchar_t     TCHAR;

#define __T(x)      L ## x

/* Formatted i/o */

#define _tprintf    wprintf
#define _ftprintf   fwprintf
#define _stprintf   swprintf
#define _sntprintf  _snwprintf
#define _vtprintf   vwprintf
#define _vftprintf  vfwprintf
#define _vstprintf  vswprintf
#define _vsntprintf _vsnwprintf
#define _tscanf     wscanf
#define _ftscanf    fwscanf
#define _stscanf    swscanf


/* Unformatted i/o */

#define _fgettc     fgetwc
#define _fgettchar  _fgetwchar
#define _fgetts     fgetws
#define _fputtc     fputwc
#define _fputtchar  _fputwchar
#define _fputts     fputws
#define _gettc      getwc
#define _gettchar   getwchar
#define _getts      _getws
#define _puttc      putwc
#define _puttchar   putwchar
#define _putts      _putws
#define _ungettc    ungetwc


/* String conversion functions */

#define _tcstod     wcstod
#define _tcstol     wcstol
#define _tcstoul    wcstoul

#define _itot       _itow
#define _ltot       _ltow
#define _ultot      _ultow
#define _ttoi       _wtoi
#define _ttol       _wtol

#define _ttoi64     _wtoi64
#define _i64tot     _i64tow
#define _ui64tot    _ui64tow

/* String functions */

#define _tcscat     wcscat
#define _tcschr     wcschr
#define _tcscpy     wcscpy
#define _tcscspn    wcscspn
#define _tcslen     wcslen
#define _tcsncat    wcsncat
#define _tcsncpy    wcsncpy
#define _tcspbrk    wcspbrk
#define _tcsrchr    wcsrchr
#define _tcsspn     wcsspn
#define _tcsstr     wcsstr
#define _tcstok     wcstok

#define _tcsdup     _wcsdup
#define _tcsnset    _wcsnset
#define _tcsrev     _wcsrev
#define _tcsset     _wcsset

#define _tcscmp     wcscmp
#define _tcsicmp    wcscasecmp
#define _tcsnccmp   wcsncmp
#define _tcsncmp    wcsncmp
#define _tcsncicmp  _wcsnicmp
#define _tcsnicmp   _wcsnicmp

#define _tcscoll    wcscoll
#define _tcsicoll   _wcsicoll
#define _tcsnccoll  _wcsncoll
#define _tcsncoll   _wcsncoll
#define _tcsncicoll _wcsnicoll
#define _tcsnicoll  _wcsnicoll


/* Execute functions */

#define _texecl     _wexecl
#define _texecle    _wexecle
#define _texeclp    _wexeclp
#define _texeclpe   _wexeclpe
#define _texecv     _wexecv
#define _texecve    _wexecve
#define _texecvp    _wexecvp
#define _texecvpe   _wexecvpe

#define _tspawnl    _wspawnl
#define _tspawnle   _wspawnle
#define _tspawnlp   _wspawnlp
#define _tspawnlpe  _wspawnlpe
#define _tspawnv    _wspawnv
#define _tspawnve   _wspawnve
#define _tspawnvp   _wspawnvp
#define _tspawnvp   _wspawnvp
#define _tspawnvpe  _wspawnvpe

#define _tsystem    _wsystem


/* Time functions */

#define _tasctime   _wasctime
#define _tctime     _wctime
#define _tstrdate   _wstrdate
#define _tstrtime   _wstrtime
#define _tutime     _wutime
#define _tcsftime   wcsftime


/* Directory functions */

#define _tchdir     _wchdir
#define _tgetcwd    _wgetcwd
#define _tgetdcwd   _wgetdcwd
#define _tmkdir     _wmkdir
#define _trmdir     _wrmdir


/* Environment/Path functions */

#define _tfullpath  _wfullpath
#define _tgetenv    _wgetenv
#define _tmakepath  _wmakepath
#define _tputenv    _wputenv
#define _tsearchenv _wsearchenv
#define _tsplitpath _wsplitpath


/* Stdio functions */

#define _tfdopen    _wfdopen
#define _tfsopen    _wfsopen
#define _tfopen     _wfopen
#define _tfreopen   _wfreopen
#define _tperror    _wperror
#define _tpopen     _wpopen
#define _ttempnam   _wtempnam
#define _ttmpnam    _wtmpnam



/* Redundant "logical-character" mappings */

#define _tcsclen    wcslen
#define _tcsnccat   wcsncat
#define _tcsnccpy   wcsncpy
#define _tcsncset   _wcsnset

#define _tcsdec     _wcsdec
#define _tcsinc     _wcsinc
#define _tcsnbcnt   _wcsncnt
#define _tcsnccnt   _wcsncnt
#define _tcsnextc   _wcsnextc
#define _tcsninc    _wcsninc
#define _tcsspnp    _wcsspnp

#define _tcslwr     _wcslwr
#define _tcsupr     _wcsupr
#define _tcsxfrm    wcsxfrm


/* ctype functions */

#define _istalnum   iswalnum
#define _istalpha   iswalpha
#define _istascii   iswascii
#define _istcntrl   iswcntrl
#define _istdigit   iswdigit
#define _istgraph   iswgraph
#define _istlower   iswlower
#define _istprint   iswprint
#define _istpunct   iswpunct
#define _istspace   iswspace
#define _istupper   iswupper
#define _istxdigit  iswxdigit

#define _totupper   towupper
#define _totlower   towlower

#define _istlegal(_c)   (1)
#define _istlead(_c)    (0)
#define _istleadbyte(_c)    (0)


/* Generic text macros to be used with string literals and character constants.
   Will also allow symbolic constants that resolve to same. */

#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)
#define TEXT(x)     __T(x)

#endif //!__unix

#endif //__KLSTD_TCHAR_H__
