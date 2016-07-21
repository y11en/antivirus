#include "CrC_User.h"
#include <stdio.h>

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_11_1994_update*/
int CrypC_R34_11_1994_update(struct R34_11_1994 *Context, void *Buff, LINT LenBuff)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*           åÆ§®‰®™†Ê®Ô ÂÌË-¢•™‚Æ‡† ·ÆÆ°È•≠®Ô ‰‡†£¨•≠‚Æ¨ ·ÆÆ°È•≠®Ô           */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ‚•™·‚† §´Ô ÂÌË®‡Æ¢†≠®Ô *Buff.                         */
/* - Ñ´®≠† §†≠≠ÎÂ ¢ °„‰•‡• *Buf ¢ °†©‚†Â LenBuff.                             */
/* ÇÎÂÆ§:                                                                     */
/* - åÆ§®‰®Ê®‡Æ¢†≠≠Æ• ·Æ·‚ÆÔ≠®• ™Æ≠‚•™·‚†.                                    */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                                                                   */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOTREADY                                                      */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
    int         Ret;
   LINT         i;

if(!Context || !Buff)
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
  for(i=0;i<LenBuff;i++)
     {
     if(Context->BlockOffs == LNQ*4)         /* ÉÆ‚Æ¢ °´Æ™ §´Ô ÂÌË®‡Æ¢†≠®Ô?   */
       {
//     CrypC_NetToHost(Context->Feed,LNQ);
       CrypC__Hash(Context->PE->K_, Context->Digest, Context->Feed);
//     CrypC_HostToNet(Context->Feed,LNQ);
       CrypC__add_LbC(LNQ*4,Context->SumVec,Context->Feed);
       Context->MsgLength += LNQ*4;
       Context->BlockOffs  =     0;          /* í•™. ·§¢®£ ¢ °´Æ™•.           */
       }
                                             /* ç†™ÆØ´•≠®• ‚•™·‚†:            */
     Context->Feed[Context->BlockOffs++] = ((BYTE *)Buff)[i];
     }
    
  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_11_1994_update*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
