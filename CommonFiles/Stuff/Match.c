#include <stdio.h>
#include <string.h>
#include <locale.h>
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

    // Проверить, что smy лежит между smax и smin
int syinsy(unsigned smin, unsigned smy, unsigned smax)
     {
      char left   [2];
      char right  [2];
      char middle [2];

      left  [0]   = smin;  left  [1]   = '\0';
      right [0]   = smax;  right [1]   = '\0';
      middle[0]   = smy;   middle[1]   = '\0';

//      return (strcoll(left, middle) <= 0 && strcoll(middle, right) <= 0);
        return (strcmp(left, middle) <= 0 && strcmp(middle, right) <= 0);
     }

    #define  U(c) ((c) & 0377)  // подавление расширения знака 
    #define  QUOT    '\\'       // экранирующий символ         
    #ifndef  MATCH_ERR
//    # define MATCH_ERR printf("Нет ]\n")
    # define MATCH_ERR
    #endif

    //* s - сопоставляемая строка
    //* p - шаблон. Символ \ отменяет спецзначение метасимвола.

int match(const register char *s, const register char *p)
     {
      register int    scc; // текущий символ строки                 
      int     c, cc, lc;   // lc - предыдущий символ в [...] списке 
      int     ok, notflag;

      for(;;)
       {
        scc = U(*s++);          // очередной символ строки  
        switch (c = U (*p++))
         { // очередной символ шаблона 
          case QUOT:          // a*\*b 
            c = U (*p++);
            if( c == 0 ) return(0); // ошибка: pattern\ 
            else goto def;
          case '[':   // любой символ из списка 
            ok = notflag = 0;
            lc = 077777;      // достаточно большое число 
            if(*p == '!')
             { notflag=1; p++; }

            while (cc = U (*p++))
             {
              if(cc == ']')
               {    // конец перечисления 
                if (ok)
                      break;      // сопоставилось      
                return (0);     // не сопоставилось   
               }
              if(cc == '-')
               {    // интервал символов  
                if (notflag)
                 {
                  // не из диапазона - OK 
                  if (!syinsy (lc, scc, U (*p++)))
                     ok++;
                  // из диапазона - неудача 
                  else return (0);
                 }
                else
                 {
                  // символ из диапазона - OK 
                  if (syinsy (lc, scc, U (*p++)))
                     ok++;
                 }
               }
              else
               {
                if (cc == QUOT)
                 {      // [\[\]] 
                  cc = U(*p++);
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
             }
            if (cc == 0)
             {    // конец строки 
              MATCH_ERR;
              return (0);        // ошибка 
             }
            continue;
          case '*':   // любое число любых символов 
            if(!*p)  return (1);
            for(s--; *s; s++)
                if(match (s, p))
                    return (1);
            return (0);
          case 0:
            return (scc == 0);
          default: def:
            if(c != scc) return (0);
            continue;
          case '?':   // один любой символ 
            if (scc == 0) return (0);
            continue;
         }
       }
     }


// Обратите внимание на то, что в UNIX расширением  шаблонов  имен  файлов,  вроде  *.c,
// занимается  не  операционная система (как в MS DOS), а программа-интерпретатор команд
// пользователя (shell: /bin/sh, /bin/csh, /bin/ksh).   Это  позволяет  обрабатывать  (в
// принципе) разные стили шаблонов имен.
