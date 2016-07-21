#include <stdio.h>
#include "CrC_User.h"

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀R34_10_2001_gen_mutual_key*/
int CrypC_LR34_10_2001_gen_mutual_key(struct LR34_10_2001 *Context,
                                       struct LPoint *PublicKey,
				       struct LPoint *MutualKey)
/*────────────────────────────────────────────────────────────────────────────*/
/*               Вычисление совместного ключа двух абонентов                  */
/*      с использованием скалярного умножения на эллиптической кривой,        */
/*            описанной в контексте обработки цифровой подписи                */
/*                            ГОСТ Р3410-2001                                 */
/* Входные параметры:                                                         */
/* - Указатель на буфер контекста: *Context (структура).                      */
/* - Указатель на буфер открытого ключа PublicKey удалённого абонента.        */
/* - Указатель на буфер совместного ключа MutualKey.                          */
/* Выход:                                                                     */
/* - Совместный ключ в роле MutualKey.                                        */
/* Коды возврата:                                                             */
/* - RET_OK                - нормальное завершение.                           */
/* - RET_INCORRECTCONTEXT  - передан инициализированный контекст др. режима.  */
/* - RET_CONTEXTNOREADY    - не инициализированы параметры подписи.           */
/* - RET_KEYNOTPRESENT     - не установлен секретный ключ.                    */
/* - RET_INVALIDPARAMETER  - нулевой указатель на обязательный параметр.      */
/*────────────────────────────────────────────────────────────────────────────*/
#define P_      (Context->P_)
#define SK_     (Context->SecretKey)
#define P_Bytes (Context->P_Size)
#define P_Words (Context->P_Size/4)
#define Q_Bytes (Context->Q_Size)
#define Q_Words (Context->Q_Size/4)
{
         int  i,Ret;
struct LPoint PK;

     if(! Context)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark      & GOST_XXXXX_Marker_Mask ==
    R34_10_2001_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_10_2001_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(!(Context->IsReady & R34_10_2001_CONTEXT))
  Ret = RET_CONTEXTNOTREADY;
else if(!(Context->IsReady & R34_10_2001_SECRETKEY))
  Ret = RET_KEYNOTPRESENT;
else  
  {
  /* Размаскирование секретного ключа.*/
  for(i=0;i<Q_Words;i++)
     ((DWORD *)SK_)[i        ] +=
     ((DWORD *)SK_)[i+Q_Words]  ;

  memcpy(&PK,PublicKey,LPTSIZE  );            /* Открытый ключ подписи.       */
  CrypC_NetToHost(PK.X,P_Words);              /* Net to Host Byteorder.       */
  CrypC_NetToHost(PK.Y,P_Words);              /* Net to Host Byteorder.       */
  CrypC_LAffiToProj(Context, &PK);
  CrypC_LIntToMont (Context, &PK);

  /* Вычисление совместного ключа. */
  CrypC_LCuPower(&PK, SK_, MutualKey, Context);             /* MK_ = (PK)*SK_ */

  /* Маскирование ключа. */
  CrypC_Mask_SecretKey_(Context->RS, SK_);

  CrypC_LProjToAffi(Context, MutualKey);
  CrypC_LMontToInt (Context, MutualKey);
                                              /* Host to Net Byteorder.       */
  CrypC_HostToNet(MutualKey->X, P_Words);
  CrypC_HostToNet(MutualKey->Y, P_Words);
    
  Ret = RET_OK;
  }
return(Ret);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄R34_10_2001_gen_mutual_key*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
