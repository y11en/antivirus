#include "CrC_User.h"
#include <stdio.h>

void HTON(void *, int);
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀R34_11_1994_final*/
int CrypC_R34_11_1994_final(struct R34_11_1994 *Context, void *Digest)
/*────────────────────────────────────────────────────────────────────────────*/
/*           Завершение выработки имитовектора сообщения и выдача его         */
/*                           вызывающей программе                             */
/* Входные параметры:                                                         */
/* - Указатель на буфер контекста: *Context (структура).                      */
/* - Указатель на буфер для передачи хэш-векотра *Digest.                     */
/* Выход:                                                                     */
/* - Буфер передачи хэш-вектора содержит хэш-вектор сообщения.                */
/* Коды возврата:                                                             */
/* - RET_OK                                                                   */
/* - RET_INCORRECTCONTEXT  - передан инициализированный контекст др. режима.  */
/* - RET_CONTEXTNOTREADY                                                      */
/* - RET_INVALIDPARAMETER  - нулевой указатель на обязательный параметр.      */
/*────────────────────────────────────────────────────────────────────────────*/
{
   int          Ret;
  DWORD         TotalLength;
   BYTE         A[LNQ*4],SUMVEC[LNQ*4];

if(!Context || !Digest)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark     & GOST_XXXXX_Marker_Mask ==
    R34_11_1994_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_11_1994_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(Context->IsReady != 1)
  Ret = RET_CONTEXTNOTREADY;
else
  {  
  memcpy(Digest,Context->Digest,LNQ*4);       /* Вывести имитовектор в буфер. */
  memcpy(SUMVEC,Context->SumVec,LNQ*4);       /* Текущая сумма блоков.        */
  TotalLength = Context->MsgLength;           /* Текущая длина сообщения.     */

  if(Context->BlockOffs)                      /* Есть накопленные данные?     */
    {
    if(Context->BlockOffs < LNQ*4)            /* Накоплено меньше  блока?     */
      memset(Context->Feed+Context->BlockOffs,0,(LNQ*4)-(Context->BlockOffs));
                                              /* Хэширование неполного блока: */
//printf("Hash Partial:\n");
    CrypC__Hash(Context->PE->K_, Digest, Context->Feed); /* (в выходном буфере).*/
    CrypC__add_LbC(LNQ*4,SUMVEC,Context->Feed);
    TotalLength += Context->BlockOffs;        /* Длина с неполным блоком.     */
    }
  else                                        /* Нет накопленных данных.      */
    if(!TotalLength)                          /* Вообще не было данных.       */
      {                                       /* Хэширование нулевого блока:  */
                                              /* SUMVEC точно = 0.            */
      HTON(SUMVEC, LNQ);                      /* Net to Host Byteorder.       */
      CrypC__Hash(Context->PE->K_, Digest, SUMVEC); /* (в выходном буфере).   */
      }

  TotalLength <<= 3;                          /* Длина сообщения в битах.     */
  memset(A,0,LNQ*4);
  memcpy(A,&TotalLength,4);                   /* Блок, содержащий длину.      */
  HTON(A, LNQ);                               /* Net to Host Byteorder.       */
//printf("Hash Length:\n");
  CrypC__Hash(Context->PE->K_, Digest, A);    /* Хэширование длины сообщения. */
  HTON(SUMVEC, LNQ);                          /* Net to Host Byteorder.       */
//printf("Hash Sum:\n");
  CrypC__Hash(Context->PE->K_, Digest, SUMVEC);/* Хэширование суммы блоков.   */
    
  HTON(Digest, LNQ);                           /* Host to Net Byteorder.      */
//CrypC_HostToNet(Digest, LNQ);                /* Host to Net Byteorder.      */
  Ret = RET_OK;
  }
return(Ret);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄R34_11_1994_final*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
