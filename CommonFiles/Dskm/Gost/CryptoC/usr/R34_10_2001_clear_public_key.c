#include <stdio.h>
#include "CrC_User.h"

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_10_2001_clear_public_key*/
int CrypC_LR34_10_2001_clear_public_key(struct LR34_10_2001 *Context)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*                  éÁ®·‚™† Æ‚™‡Î‚Æ£Æ ™´ÓÁ† ¢ ™Æ≠‚•™·‚•                       */
/*                  ¢Î‡†°Æ‚™®/Ø‡Æ¢•‡™® Ê®‰‡Æ¢Æ© ØÆ§Ø®·®                       */
/*                            Ééëí ê3410-2001                                 */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* ÇÎÂÆ§:                                                                     */
/* - éÁ®È•≠≠Î© °„‰•‡ Æ‚™‡Î‚Æ£Æ ™´ÓÁ† ¢ ™Æ≠‚•™·‚•.                             */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                - ≠Æ‡¨†´Ï≠Æ• ß†¢•‡Ë•≠®•.                           */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOTREADY   - ™Æ≠‚•™·‚ ≠• ®≠®Ê®†´®ß®‡Æ¢†≠.                     */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
  int i,Ret;

if(!Context)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark     & GOST_XXXXX_Marker_Mask ==
    R34_10_2001_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_10_2001_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(!(Context->IsReady & R34_10_2001_CONTEXT))
  Ret = RET_CONTEXTNOTREADY;
else
  {
  for(i=0;i<LPTSIZE;i++) ((char *)&(Context->PublicKey))[i] = 0x00;
  Context->IsReady &= ~R34_10_2001_PUBLICKEY;
  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_10_2001_clear_public_key*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
