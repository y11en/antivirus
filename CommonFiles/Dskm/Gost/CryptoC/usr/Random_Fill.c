#include "CrC_User.h"

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀Random_Fill*/
int     CrypC_Random_Fill( struct RND_State *State, void  *Field, int Bytes )
/*────────────────────────────────────────────────────────────────────────────*/
/*                 Заполнение буфера псевдослучайными данными.                */
/* Входные параметры:                                                         */
/* - Адрес буфера памяти Field.                                               */
/* - Размер буфера в байтах Bytes.                                            */
/* Выход:                                                                     */
/* - Буфер заполнен псевдослучайной последовательностью байт длины Bytes.     */
/* Коды возврата:                                                             */
/* - RET_OK                - нормальное завершение.                           */
/* - RET_INVALIDPARAMETER  - нулевой указатель на обязательный параметр.      */
/*────────────────────────────────────────────────────────────────────────────*/
{
  int    i,Ret,Words,Rest;
  DWORD  TMP;

if(!Field)
  Ret = RET_INVALIDPARAMETER;
else
  {
  Words = Bytes / 4;
  Rest  = Bytes % 4;

  CrypC_FillByRandom_(State,Field,Bytes-Rest);
  CrypC_HostToNet(Field,Words);
  if(Rest)
    {
    CrypC_FillByRandom_(State,&TMP,4);
    CrypC_HostToNet(&TMP,1);
    memcpy(((BYTE *)Field)+Bytes-Rest,&TMP,Rest);
    }

  Ret = RET_OK;
  }
return(Ret);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄Random_Fill*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
