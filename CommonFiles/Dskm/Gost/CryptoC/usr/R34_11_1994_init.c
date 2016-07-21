#include <stdio.h>
#include "CrC_User.h"

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_11_1994_init*/
int CrypC_R34_11_1994_init(struct R34_11_1994 *Context, struct COM_State *PE, struct RND_State *State, void *Sync)
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*                        à≠®Ê®†´®ß†Ê®Ô ™Æ≠‚•™·‚†                             */
/*                         ¢Î‡†°Æ‚™® ÂÌË-¢•™‚Æ‡†                              */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ≠†Á†´Ï≠Æ£Æ ß≠†Á•≠®Ô ÂÌË-¢•™‚Æ‡† *Sync (NQ*2 °†©‚†).   */
/*             Ö·´® „™†ß†‚•´Ï ‡†¢•≠ 0, ‚Æ Ø‡®≠®¨†•‚·Ô ≠„´•¢Æ• ß†ØÆ´≠•≠®•.     */
/* ÇÎÂÆ§:                                                                     */
/* - äÆ≠‚•™·‚, £Æ‚Æ¢Î© ™ ¢Î‡†°Æ‚™• ÂÌË-¢•™‚Æ‡†.                               */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                                                                   */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOTCLEAR                                                      */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
	 int  i, Ret;

/* ÇÎßÆ¢ Ø‡ÆÊ•§„‡Î ‰Æ‡¨®‡Æ¢†≠®Ô  ¨Æ§®‰®Ê®‡Æ¢†≠≠Æ£Æ ¨†··®¢† Ø•‡•·‚†≠Æ¢Æ™       */
/* §´Ô ™‡®Ø‚Æ·Â•¨Î Ééëí 28147-89:                                             */
CrypC__InitGost( (BYTE *)PE->Perms, PE->K_ );

if(!Context || !State)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark     & GOST_XXXXX_Marker_Mask ==
    R34_11_1994_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_11_1994_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(Context->IsReady)
  Ret = RET_CONTEXTNOTCLEAR;
else
  {
  if(Context->Mark != R34_11_1994_Marker)
    CrypC_R34_11_1994_clear(Context);

  Context->RS = State;                   /* ì™†ß†‚•´Ï ≠† ™Æ≠‚•™·‚ Éëó.        */
  Context->PE = PE;                      /* ì™†ß†‚•´Ï ≠† ™Æ≠‚•™·‚ ØÆ§·‚†≠Æ¢Æ™.*/

  if(Sync)
    memcpy        (Context->Digest,Sync,LNQ*4);/* ç†Á†´Ï≠Æ• ·Æ·‚. ÂÌË-¢•™‚Æ‡†.*/
  else
    memset        (Context->Digest,   0,LNQ*4);/* ë®≠Â‡ÆØÆ·Î´™† ØÆ „¨Æ´Á†≠®Ó. */
  CrypC_NetToHost (Context->Digest     ,    8);/* Net to Host Byteorder.      */
  memset          (Context->SumVec,   0,LNQ*4);/* ç†Á†´Ï≠Æ• ·Æ·‚. Sum-¢•™‚Æ‡†.*/
    
  Context->MsgLength = 0;                   /* ëÁ•‚Á®™ ·„¨¨†‡≠Æ© §´®≠Î.       */
  Context->BlockOffs = 0;                   /* èÆß-Ô. ¢ °„‰•‡• ≠†™ÆØ-Ô °´Æ™†. */

  Context->IsReady = 1;
  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_11_1994_init*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
