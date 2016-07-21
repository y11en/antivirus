#include <stdio.h>
#include "CrC_User.h"

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀R34_10_2001_get_secret_key*/
int CrypC_LR34_10_2001_get_secret_key(struct LR34_10_2001 *Context, void *SecretKey, int *KeySize)
/*────────────────────────────────────────────────────────────────────────────*/
/*       Чтение секретного ключа из контекста обработки цифровой подписи      */
/*                            ГОСТ Р3410-2001                                 */
/* Входные параметры:                                                         */
/* - Указатель на буфер контекста: *Context (структура).                      */
/* - Указатель на буфер немаскированного секретного ключа: *SecretKey.        */
/* - Указатель на буфер размера ключа: *KeySize.                              */
/* Выход:                                                                     */
/* - Немаскированный Секретный ключ в буфере SecretKey, размер ключа в байтах */
/*   - в буфере KeySize.                                                      */
/*   Если SecretKey == 0, то возвращается только длина ключа.                 */
/* Коды возврата:                                                             */
/* - RET_OK                - нормальное завершение.                           */
/* - RET_INCORRECTCONTEXT  - передан инициализированный контекст др. режима.  */
/* - RET_CONTEXTNOREADY    - не инициализированы параметры подписи.           */
/* - RET_KEYNOTPRESENT     - не установлен секретный ключ.                    */
/* - RET_INVALIDPARAMETER  - нулевой указатель на обязательный параметр.      */
/*────────────────────────────────────────────────────────────────────────────*/
{
         int  i,Ret;

     if(! Context || !KeySize)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark     & GOST_XXXXX_Marker_Mask ==
    R34_10_2001_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_10_2001_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(!(Context->IsReady & R34_10_2001_CONTEXT))
  Ret = RET_CONTEXTNOTREADY;
else if(!(Context->IsReady & R34_10_2001_SECRETKEY))
  {
  *KeySize = Context->Q_Size;             /* Размер ключа в байтах.           */
  Ret = RET_KEYNOTPRESENT;
  }
else  
  {
  *KeySize = Context->Q_Size;             /* Размер ключа в байтах.           */
                                          /* Размаскирование секретного ключа */
                                          /*             в буфер SecretKey.   */
  if(SecretKey)
    {
    for(i=0;i<Context->Q_Size/4;i++)
       ((DWORD *)         SecretKey)[i                  ] =
       ((DWORD *)Context->SecretKey)[i                  ] +
       ((DWORD *)Context->SecretKey)[i+Context->Q_Size/4] ;
     
    CrypC_NetToHost(           SecretKey,
                     (Context->Q_Size/4));/* Net to Host Byteorder.           */
                                          /* Перемаскирование ключа.          */
    CrypC_ReMask_SecretKey_(Context->RS, Context->SecretKey);
    }
  Ret = RET_OK;
  }
return(Ret);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄R34_10_2001_get_secret_key*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
