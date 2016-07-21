#include "CrC_User.h"
#include <string.h>
#include <stdio.h>
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀GENSEC*/
int     CrypC_Gen_SecretKey_(struct RND_State *State, void  *Sec, int Keys)
/*───────────────────────────────────────────────────────────────────────────*/
/*       Генерация Keys псевдослучайных двоичных ключей длины NK*4 байта     */
/* Входные параметры:                                                        */
/* - Поле ключа *Sec (Keys*NK*4 байта).                                      */
/* Выход:                                                                    */
/* - Keys псевдослучайных двоичных ключей в поле *Sec.                       */
/*───────────────────────────────────────────────────────────────────────────*/
{
  DWORD *M1, TMPKey[LNK];
  int    i;
extern   struct COM_State PERMDefault;

/* Вызов процедуры формирования  модифицированного массива перестановок       */
/* для криптосхемы ГОСТ 28147-89:                                             */
CrypC__InitGost( (BYTE *)PERMDefault.Perms, PERMDefault.K_ );
CrypC_FillByRandom_(State, TMPKey,     LNK*4);/* Случайный ключ.              */ 
CrypC_FillByRandom_(State, Sec   ,Keys*LNK*4);/* Начальное заполнение.        */ 
for(i=0;i<Keys*LNK/2;i++)                /* Перешифрование на случайном ключе.*/
   {
   M1=(DWORD *)((char *)Sec+(i*8));
   CrypC__GOSTE_(PERMDefault.K_,TMPKey,M1,M1);
   }
CrypC_HostToNet(Sec,(Keys*LNK));         /* Host to Network Byteorder.        */
return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄GENSEC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
