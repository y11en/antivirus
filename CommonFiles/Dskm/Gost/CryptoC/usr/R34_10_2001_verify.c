#include "CrC_User.h"
#include <stdio.h>

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_10_2001_verify*/
int CrypC_LR34_10_2001_verify(struct LR34_10_2001 *Context, void *_Digest, void *_Signature)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*            è‡Æ¢•‡™† Ê®‰‡Æ¢Æ© ØÆ§Ø®·® ß†§†≠≠Æ£Æ ÂÌË-¢•™‚Æ‡†                 */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ÂÌË-¢•™‚Æ‡†: Digest (NQ*2 °†©‚).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ Ê®‰‡Æ¢Æ© ØÆ§Ø®·®: Signature (4*NQ °†©‚).              */
/* ÇÎÂÆ§:                                                                     */
/* - äÆ§ ¢Æß¢‡†‚†.                                                            */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK             - ØÆ§Ø®·Ï Ø‡Æ¢•‡•≠†, ‡•ß„´Ï‚†‚ Ø‡Æ¢•‡™® ØÆ´Æ¶®‚•´Ï≠Î©.*/
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_INCORRECTSIGN  - ØÆ§Ø®·Ï Ø‡Æ¢•‡•≠†, ‡•ß„´Ï‚†‚ Ø‡Æ¢•‡™® Æ‚‡®Ê†‚•´Ï≠Î©.*/
/* - RET_ZERO_R         - ≠„´•¢Æ• ß≠†Á•≠®• ™Æ¨ØÆ≠•≠‚† ùñè r'.                 */
/* - RET_ZERO_S         - ≠„´•¢Æ• ß≠†Á•≠®• ™Æ¨ØÆ≠•≠‚† ùñè s .                 */
/* - RET_BAD_R          - ≠•™Æ‡‡•™‚≠Æ• ß≠†Á•≠®• ™Æ¨ØÆ≠•≠‚† ùñè r'.            */
/* - RET_BAD_S          - ≠•™Æ‡‡•™‚≠Æ• ß≠†Á•≠®• ™Æ¨ØÆ≠•≠‚† ùñè s .            */
/* - RET_CONTEXTNOTREADY   - ≠• ®≠®Ê®†´®ß®‡Æ¢†≠Î Ø†‡†¨•‚‡Î ØÆ§Ø®·®.           */
/* - RET_KEYNOTPRESENT     - ≠• „·‚†≠Æ¢´•≠ Æ‚™‡Î‚Î© ™´ÓÁ.                     */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
#define Digest    ((BYTE *)_Digest)
#define Signature ((DWORD *)_Signature)
DWORD SignBuff[LNQ+LNQ+LNQ];
int   Ret;

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
else if(!(Context->IsReady & R34_10_2001_PUBLICKEY))
  Ret = RET_KEYNOTPRESENT;
else
  {
  memcpy( SignBuff                  , Digest   , Context->Q_Size   );
  memcpy( SignBuff+Context->Q_Size/4, Signature, Context->Q_Size*2 );
  CrypC_NetToHost(SignBuff,((Context->Q_Size*3)/4));
  Ret = CrypC_LCuCheck(Context, SignBuff);
  }
return (Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_10_2001_verify*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
