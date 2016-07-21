#include <stdio.h>
#include "CrC_User.h"

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂR34_10_2001_get_parms*/
int CrypC_LR34_10_2001_get_parms(struct   LR34_10_2001 *Context,
                                 void   *_R34_10_2001_P, int *P_Size,
                                 void   *_R34_10_2001_Q, int *Q_Size,
                                 void   *_R34_10_2001_a,
                                 void   *_R34_10_2001_b,
                          struct LPoint *_R34_10_2001_A
                                )
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
/*                  ó‚•≠®• Ø†‡†¨•‚‡Æ¢ Ê®‰‡Æ¢Æ© ØÆ§Ø®·®                        */
/*    ®ß ®≠®Ê®†´®ß®‡Æ¢†≠≠Æ£Æ ™Æ≠‚•™·‚† ¢Î‡†°Æ‚™®/Ø‡Æ¢•‡™® Ê®‰‡Æ¢Æ© ØÆ§Ø®·®    */
/*                            Ééëí ê3410-2001                                 */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                         */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ™Æ≠‚•™·‚†: *Context (·‚‡„™‚„‡†).                      */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ §´®≠Î *P_Size  Ø‡Æ·‚Æ£Æ ØÆ‡Ô§™† P: *R34_10_2001_P.    */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ §´®≠Î *Q_Size  Ø‡Æ·‚Æ£Æ ØÆ‡Ô§™† Q: *R34_10_2001_Q.    */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ §´®≠Î *P_Size  Ø†‡†¨•‚‡† a ™‡®¢Æ©: *R34_10_2001_a.    */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ §´®≠Î *P_Size  Ø†‡†¨•‚‡† b ™‡®¢Æ©: *R34_10_2001_b.    */
/* - ì™†ß†‚•´Ï ≠† °„‰•‡ ·‚‡„™‚„‡Î          ‚ÆÁ™® A ™‡®¢Æ©: *R34_10_2001_A.    */
/* ÇÎÂÆ§:                                                                     */
/* - èÆ´Ô R34_10_2001_P, R34_10_2001_Q, R34_10_2001_a, R34_10_2001_b,         */
/*   R34_10_2001_A ·Æ§•‡¶†‚ Ø†‡†¨•‚‡Î P, Q, a, b ® A, ·ÆÆ‚¢•‚·‚¢•≠≠Æ.         */
/*   èÆ´Ô *P_Size, *Q_Size ß†ØÆ´≠ÔÓ‚·Ô §´®≠†¨® P ® Q ¢ °†©‚†Â.                */
/* - Ö·´® R34_10_2001_P == 0, Ø†‡†¨•‚‡ P ≠• ™ÆØ®‡„•‚·Ô.                       */
/* - Ö·´® R34_10_2001_Q == 0, Ø†‡†¨•‚‡ Q ≠• ™ÆØ®‡„•‚·Ô.                       */
/* - Ö·´® R34_10_2001_a == 0, Ø†‡†¨•‚‡ a ≠• ™ÆØ®‡„•‚·Ô.                       */
/* - Ö·´® R34_10_2001_b == 0, Ø†‡†¨•‚‡ b ≠• ™ÆØ®‡„•‚·Ô.                       */
/* - Ö·´® R34_10_2001_A == 0, Ø†‡†¨•‚‡ A ≠• ™ÆØ®‡„•‚·Ô.                       */
/* äÆ§Î ¢Æß¢‡†‚†:                                                             */
/* - RET_OK                - ≠Æ‡¨†´Ï≠Æ• ß†¢•‡Ë•≠®•.                           */
/* - RET_INCORRECTCONTEXT  - Ø•‡•§†≠ ®≠®Ê®†´®ß®‡Æ¢†≠≠Î© ™Æ≠‚•™·‚ §‡. ‡•¶®¨†.  */
/* - RET_CONTEXTNOTREADY   - ™Æ≠‚•™·‚ ≠• ®≠®Ê®†´®ß®‡Æ¢†≠.                     */
/* - RET_INVALIDPARAMETER  - ≠„´•¢Æ© „™†ß†‚•´Ï ≠† Æ°Ôß†‚•´Ï≠Î© Ø†‡†¨•‚‡.      */
/*ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ*/
{
#define R34_10_2001_P ((DWORD *)_R34_10_2001_P)
#define R34_10_2001_Q ((DWORD *)_R34_10_2001_Q)
#define R34_10_2001_a ((DWORD *)_R34_10_2001_a)
#define R34_10_2001_b ((DWORD *)_R34_10_2001_b)
#define R34_10_2001_A ((struct LPoint *)_R34_10_2001_A)
         int  Ret;

if(! Context || !P_Size || !Q_Size)
  Ret = RET_INVALIDPARAMETER;
else
if((Context->Mark      & GOST_XXXXX_Marker_Mask ==
    R34_10_2001_Marker & GOST_XXXXX_Marker_Mask   ) &&
   (Context->Mark                                  !=
    R34_10_2001_Marker                            )   )
  Ret = RET_INCORRECTCONTEXT;
else if(!(Context->IsReady & R34_10_2001_CONTEXT))
  Ret = RET_CONTEXTNOTREADY;
else
  {
                                              /* ê†ß¨•‡Î.                     */
  *P_Size =                                    Context->P_Size    ;
  *Q_Size =                                    Context->Q_Size    ;
                                              /* è‡Æ·‚Æ• P.                   */
  if(R34_10_2001_P)
    {
    memcpy         (R34_10_2001_P,Context->P_, Context->P_Size   );
    CrypC_HostToNet(R34_10_2001_P,            (Context->P_Size/4));
    }
                                              /* è‡Æ·‚Æ• Q ® ‡†ß¨•‡.          */
  if(R34_10_2001_Q)
    {
    memcpy         (R34_10_2001_Q,Context->Q_, Context->Q_Size   );
    CrypC_HostToNet(R34_10_2001_Q,            (Context->Q_Size/4));
    }
                                              /* è†‡†¨.  a.                   */
  if(R34_10_2001_a)
    {
    memcpy         (R34_10_2001_a,Context->a_, Context->P_Size   );
    CrypC_LmultC((DWORD *)R34_10_2001_a,      /* a *= 1.                      */
                 (DWORD *)LO_+LNP-(Context->P_Size/4),
                 (DWORD *)Context->P_                ,
                          Context->P_Size/4          );
    CrypC_HostToNet(R34_10_2001_a,            (Context->P_Size/4));
    }
                                              /* è†‡†¨.  b.                   */
  if(R34_10_2001_b)
    {
    memcpy         (R34_10_2001_b,Context->b_, Context->P_Size   );
    CrypC_LmultC((DWORD *)R34_10_2001_b,      /* a *= 1.                      */
                 (DWORD *)LO_+LNP-(Context->P_Size/4),
                 (DWORD *)Context->P_                ,
                          Context->P_Size/4          );
    CrypC_HostToNet(R34_10_2001_b,            (Context->P_Size/4));
    }
                                              /* íÆÁ™† A.                     */
  if(_R34_10_2001_A)
    {
    memcpy         (R34_10_2001_A,&Context->A_,LPTSIZE           );
    CrypC_LProjToAffi(Context, R34_10_2001_A);
    CrypC_LMontToInt (Context, R34_10_2001_A);
    CrypC_HostToNet(R34_10_2001_A->X,         (Context->P_Size/4));
    CrypC_HostToNet(R34_10_2001_A->Y,         (Context->P_Size/4));
    }

  Ret = RET_OK;
  }
return(Ret);
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹R34_10_2001_get_parms*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
