#include <stdio.h>
#include "CrC_User.h"

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_10_2001_sign*/
int CrypC_LR34_10_2001_sign(struct LR34_10_2001 *Context, void *_Digest, void *_Signature)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*           ÇÎ‡†°Æ‚™† Ê®‰‡Æ¢Æ© ØÆ§Ø®·® ß†§†≠≠Æ£Æ ÂÌË-¢•™‚Æ‡†                 */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ÂÌË-¢•™‚Æ‡†: Digest (NQ*2 °†©‚).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ §´Ô Ê®‰‡Æ¢Æ© ØÆ§Ø®·®: Signature (4*NQ °†©‚).          */
/* ÇÎÂÆ§:                                                                     */
/* - Å„‰•‡ ØÆ§Ø®·® ·Æ§•‡¶®‚ ùñè Ø•‡•§†≠≠Æ£Æ ÂÌË-¢•™‚Æ‡†.                      */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                - ≠Æ‡¨†´Ï≠Æ• ß†¢•‡Ë•≠®•.                           */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOREADY    - ≠• ®≠®Ê®†´®ß®‡Æ¢†≠Î Ø†‡†¨•‚‡Î ØÆ§Ø®·®.           */
/* - RET_KEYNOTPRESENT     - ≠• „·‚†≠Æ¢´•≠ ·•™‡•‚≠Î© ™´ÓÁ.                    */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
#define Digest    ((BYTE *)_Digest)
#define Signature ((DWORD *)_Signature)
       DWORD SignBuff[LNQ+LNQ+LNQ];
         int Ret;

     if(! Context || !Digest || !Signature)
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
  Ret = RET_KEYNOTPRESENT;
else
  {
  memcpy(SignBuff, Digest, Context->Q_Size);
  CrypC_NetToHost(SignBuff,(Context->Q_Size/4));  /* Net to Host Byteorder.   */
  CrypC_LCuSign( Context, SignBuff );
  memcpy( Signature, SignBuff+Context->Q_Size/4, Context->Q_Size*2 );
  CrypC_HostToNet(Signature,(Context->Q_Size/2)); /* Host to Net Byteorder.   */
  
                                /* è•‡•¨†·™®‡Æ¢†≠®• ·•™‡•‚≠Æ£Æ ™´ÓÁ† ØÆ§Ø®·®: */
  CrypC_ReMask_SecretKey_(Context->RS, (BYTE *)Context->SecretKey);
    
  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_10_2001_sign*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
