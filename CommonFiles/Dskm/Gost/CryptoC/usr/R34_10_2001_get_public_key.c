#include <stdio.h>
#include "CrC_User.h"

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_10_2001_get_public_key*/
int CrypC_LR34_10_2001_get_public_key(struct LR34_10_2001 *Context, struct LPoint *PublicKey, int *KeySize)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*       ó‚•≠®• Æ‚™‡Î‚Æ£Æ ™´ÓÁ† ®ß ™Æ≠‚•™·‚† Æ°‡†°Æ‚™® Ê®‰‡Æ¢Æ© ØÆ§Ø®·®       */
/*                            Ééëí ê3410-2001                                 */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ·‚‡„™‚„‡Î Æ‚™‡Î‚Æ£Æ ™´ÓÁ†: *PublicKey.                */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ‡†ß¨•‡† ™Æ¨ØÆ≠•≠‚ ™´ÓÁ†: *KeySize.                    */
/* ÇÎÂÆ§:                                                                     */
/* - é‚™‡Î‚Î© ™´ÓÁ ¢ °„‰•‡• PublicKey, ‡†ß¨•‡ ™Æ¨ØÆ≠•≠‚ ™´ÓÁ† ¢ °†©‚†Â -      */
/*   ¢ °„‰•‡• KeySize.                                                        */
/*   Ö·´® PublicKey == 0, ‚Æ ¢Æß¢‡†È†•‚·Ô ‚Æ´Ï™Æ §´®≠† ™´ÓÁ†.                 */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                - ≠Æ‡¨†´Ï≠Æ• ß†¢•‡Ë•≠®•.                           */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOREADY    - ≠• ®≠®Ê®†´®ß®‡Æ¢†≠Î Ø†‡†¨•‚‡Î ØÆ§Ø®·®.           */
/* - RET_KEYNOTPRESENT     - ≠• „·‚†≠Æ¢´•≠ Æ‚™‡Î‚Î© ™´ÓÁ.                     */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
         int  Ret;

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
else if(!(Context->IsReady & R34_10_2001_PUBLICKEY))
  {
  *KeySize = Context->P_Size;                 /* ê†ß¨•‡ ™ÆÆ‡§®≠†‚Î ™´ÓÁ† ¢ °†©‚†Â. */
  Ret = RET_KEYNOTPRESENT;
  }
else  
  {
  *KeySize = Context->P_Size;                 /* ê†ß¨•‡ ™ÆÆ‡§®≠†‚Î ™´ÓÁ† ¢ °†©‚†Â. */
  if(PublicKey)
    {
    memcpy(          PublicKey,
           &Context->PublicKey,
                     LPTSIZE  );              /* é‚™‡Î‚Î© ™´ÓÁ ØÆ§Ø®·®.       */
    CrypC_LProjToAffi(Context, PublicKey);
    CrypC_LMontToInt (Context, PublicKey);
                                              /* Host to Net Byteorder.       */
    CrypC_HostToNet(PublicKey->X, (Context->P_Size/4));
    CrypC_HostToNet(PublicKey->Y, (Context->P_Size/4));
    }

  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_10_2001_get_public_key*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
