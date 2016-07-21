#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#if !defined(_UNICODE)
__inline unsigned int __cdecl _wcsnextc(const wchar_t * _cpc) { return (unsigned int)*_cpc; }
__inline wchar_t * __cdecl _wcsninc(const wchar_t * _pc, size_t _sz) { return (wchar_t *)(_pc+_sz); }
__inline wchar_t * __cdecl _wcsdec(const wchar_t * _cpc1, const wchar_t * _cpc2) { return (wchar_t *)((_cpc1)>=(_cpc2) ? NULL : ((_cpc2)-1)); }
__inline wchar_t * __cdecl _wcsinc(const wchar_t * _pc) { return (wchar_t *)(_pc+1); }
_CRTIMP wchar_t*  __cdecl wcscat(wchar_t *, const wchar_t *);
#endif

// 7.41.  Напишите функцию match(строка,шаблон); для проверки соответствия строки  упро-
// щенному регулярному выражению в стиле Шелл. Метасимволы шаблона:
// 
//         * - любое число любых символов (0 и более);
//         ? - один любой символ.
//                       Усложнение:
//         [буквы]  - любая из перечисленных букв.
//         [!буквы] - любая из букв, кроме перечисленных.
//         [h-z]    - любая из букв от h до z включительно.
// 
// Указание: для проверки "остатка" строки используйте рекурсивный вызов этой  же  функ-
// ции.
//      Используя эту функцию, напишите программу,  которая  выделяет  из  файла  СЛОВА,
// удовлетворяющие  заданному шаблону (например, "[Ии]*о*т"). Имеется в виду, что каждую
// строку надо сначала разбить на слова, а потом проверить каждое слово.

//    #include <stdio.h>
//    #include <string.h>
//    #include <locale.h>

// ---
#define SWAB(a, b) { a^=b; b^=a; a^=b; }

// Проверить, что smy лежит между smax и smin
int syinsyW(unsigned smin, unsigned smy, unsigned smax)
     {
			if ( smin & 0xffffff00 ) SWAB(*((char *)&smin + 0), *((char *)&smin + 1));
			if ( smax & 0xffffff00 ) SWAB(*((char *)&smax + 0), *((char *)&smax + 1));
			if ( smy  & 0xffffff00 ) SWAB(*((char *)&smy  + 0), *((char *)&smy  + 1));

//      return (strcoll(left, middle) <= 0 && strcoll(middle, right) <= 0);
        return (wcscmp((wchar_t *)&smin, (wchar_t *)&smy) <= 0 && wcscmp((wchar_t *)&smy, (wchar_t *)&smax) <= 0);
     }

    #define  U(c,p) c=_wcsnextc(p) & 0xffff; p=_wcsinc(p);  // подавление расширения знака 
    #define  QUOT    L'\\'       // экранирующий символ         
    #ifndef  MATCH_ERR
//    # define MATCH_ERR printf("Нет ]\n")
    # define MATCH_ERR
    #endif

    //* s - сопоставляемая строка
    //* p - шаблон. Символ \ отменяет спецзначение метасимвола.

int matchW(const register wchar_t *s, const register wchar_t *p)
     {
      register int    scc; // текущий символ строки                 
      int     c, cc, lc;   // lc - предыдущий символ в [...] списке 
      int     ok, notflag;
			const wchar_t  *ss = s;

      for(;;)
       {
        U(scc,s);          // очередной символ строки  
				U(c,p);
        switch (c)
         { // очередной символ шаблона 
          case QUOT:          // a*\*b 
            U(c,p);
            if( c == 0 ) return(0); // ошибка: pattern\ 
            else goto def;
          case L'[':   // любой символ из списка 
            ok = notflag = 0;
            lc = 077777;      // достаточно большое число 
            if(_wcsnextc(p) == L'!')
             { notflag=1; p=_wcsinc(p); }

						U(cc,p);
            while (cc)
             {
              if(cc == L']')
               {    // конец перечисления 
                if (ok)
                      break;      // сопоставилось      
                return (0);     // не сопоставилось   
               }
              if(cc == L'-')
               {    // интервал символов  
                if (notflag)
                 {
                  // не из диапазона - OK 
									U(c,p);
                  if (!syinsyW (lc, scc, c))
                     ok++;
                  // из диапазона - неудача 
                  else return (0);
                 }
                else
                 {
                  // символ из диапазона - OK 
									U(c,p);
                  if (syinsyW (lc, scc, c))
                     ok++;
                 }
               }
              else
               {
                if (cc == QUOT)
                 {      // [\[\]] 
									U(cc,p);
                  if(!cc) return(0);// ошибка 
                 }
                if (notflag)
                 {
                  if (scc && scc != (lc = cc))
                    ok++;          // не входит в список 
                  else return (0);
                 }
                else
                 {
                  if (scc == (lc = cc)) // входит в список 
                    ok++;
                 }
               }
							U(cc,p);
						}
            if (cc == 0)
             {    // конец строки 
              MATCH_ERR;
              return (0);        // ошибка 
             }
            continue;
          case L'*':   // любое число любых символов 
            if(!*p)  return (1);
            for(s=_wcsdec(ss, s); s && _wcsnextc(s); s=_wcsinc(s))
              if(matchW(s, p))
                return (1);
            return (0);
          case 0:
            return (scc == 0);
          default: def:
            if(c != scc) return (0);
            continue;
          case L'?':   // один любой символ 
            if (scc == 0) return (0);
            continue;
         }
       }
     }


// Специальная версия функции для проверки файловых имен (Windows). 
// Не обрабатываются сочетания вида "[Ии]", "[!Ии]", "\Uu"
int matchWF(const register wchar_t *s, const register wchar_t *p)
	{
	register int  scc; // текущий символ строки                 
	int     c;  
	const wchar_t  *ss = s;
	
	for(;;)
		{
		  U(scc,s);          // очередной символ строки  
			U(c,p);
      switch (c)
				{ // очередной символ шаблона 
        case L'*':   // любое число любых символов 
          if(!*p)  return (1);
          for(s=_wcsdec(ss, s); s && _wcsnextc(s); s=_wcsinc(s))
            if(matchW(s, p))
              return (1);
						return (0);
        case 0:
          return (scc == 0);
        default: 
          if(c != scc) return (0);
          continue;
        case L'?':   // один любой символ 
          if (scc == 0) return (0);
          continue;
			}
		}
	}

// Обратите внимание на то, что в UNIX расширением  шаблонов  имен  файлов,  вроде  *.c,
// занимается  не  операционная система (как в MS DOS), а программа-интерпретатор команд
// пользователя (shell: /bin/sh, /bin/csh, /bin/ksh).   Это  позволяет  обрабатывать  (в
// принципе) разные стили шаблонов имен.
