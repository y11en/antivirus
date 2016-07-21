#include <stdio.h>
#include "CrC_User.h"

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_10_2001_gen_secret_key*/
int CrypC_LR34_10_2001_gen_secret_key(struct LR34_10_2001 *Context)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*            ÇÎ‡†°Æ‚™† ® „·‚†≠Æ¢™† ·´„Á†©≠Æ£Æ ·•™‡•‚≠Æ£Æ ™´ÓÁ†               */
/*                  ¢ ™Æ≠‚•™·‚ Æ°‡†°Æ‚™® Ê®‰‡Æ¢Æ© ØÆ§Ø®·®                     */
/*                            Ééëí ê3410-2001                                 */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* ÇÎÂÆ§:                                                                     */
/* - äÆ≠‚•™·‚, £Æ‚Æ¢Î© ™ ¢ÎÁ®·´•≠®Ó Ê®‰‡Æ¢Æ© ØÆ§Ø®·®.                         */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                - ≠Æ‡¨†´Ï≠Æ• ß†¢•‡Ë•≠®•.                           */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOREADY    - ≠• ®≠®Ê®†´®ß®‡Æ¢†≠Î Ø†‡†¨•‚‡Î ØÆ§Ø®·®.           */
/* - RET_KEYALREADYPRESENT - „¶• „·‚†≠Æ¢´•≠ ·•™‡•‚≠Î© ™´ÓÁ.                   */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
         int  i,Ret;

     if(! Context)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark     & GOST_XXXXX_Marker_Mask ==
    R34_10_2001_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_10_2001_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(!(Context->IsReady &  R34_10_2001_CONTEXT                         ))
  Ret = RET_CONTEXTNOTREADY;
else if(  Context->IsReady & (R34_10_2001_SECRETKEY ^ R34_10_2001_PUBLICKEY))
  Ret = RET_KEYALREADYPRESENT;
else  
  {
  /* É•≠•‡†Ê®Ô ·´„Á†©≠Æ£Æ ™´ÓÁ†. */
  CrypC_Gen_SecretKey_(Context->RS, Context->SecretKey, 1);
  /* Network to Host Byteorder. */
  CrypC_NetToHost(Context->SecretKey,(Context->Q_Size/4));

  /* å†·™®‡Æ¢†≠®• ™´ÓÁ†. */
  CrypC_Mask_SecretKey_(Context->RS, Context->SecretKey);
    
  Context->IsReady |= R34_10_2001_SECRETKEY;
  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_10_2001_gen_secret_key*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
