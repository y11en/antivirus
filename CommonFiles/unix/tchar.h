#ifndef _TCHAR_H_
#define _TCHAR_H_

#define TCHAR char
#define LPTSTR char*
#define _TCHAR char
#define _T(val) val

#define _tcscat   strcat
#define _tcschr   strchr
#define _tcscpy   strcpy
#define _tcscspn  strcspn
#define _tcslen   strlen
#define _tcsncat  strncat
#define _tcsncpy  strncpy
#define _tcsrchr  strrchr
#define _tcsspn   strspn
#define _tcsstr   strstr
#define _tcstok   strtok
#define _tcscmp   strcmp
#define _tcsnccmp strncmp
#define _tcsncmp  strncmp
#define _tprintf  printf
#define _gettchar getchar
#define _tcsicmp stricmp
#define _tcsnicmp strnicmp

#endif // _TCHAR_H_
