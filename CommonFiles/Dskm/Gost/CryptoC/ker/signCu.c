#include <stdlib.h>
#include <stdio.h>
#include "CrC_User.h"

#define P_      (Context->P_)
#define Q_      (Context->Q_)
#define QG_     (Context->QG_)
#define AP_     (Context->AP_)
#define SK_     (Context->SecretKey)
#define P_Bytes (Context->P_Size)
#define P_Words (Context->P_Size/4)
#define Q_Bytes (Context->Q_Size)
#define Q_Words (Context->Q_Size/4)
#define Sign_H  (SField)
#define Sign_R  (SField+Q_Words)
#define Sign_S  (SField+Q_Words+Q_Words)

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀SIGN*/
int CrypC_LCuSign(struct LR34_10_2001 *Context, DWORD *SField)
/*        Выработка цифровой подписи в одном буфере данных          */
/* Входные параметры:                                               */
/* - Буфер данных *SField длины 3*CuNQ 32-разрядных слов, содержа-  */
/*   щий в первом подполе длины CuNQ 32-разрядных слов хэш-вектор H.*/
/* - Секретный ключ подписи, замаскировпанний процедурой            */
/*                      Mask_SecretKey_.                            */
/* Выход:                                                           */
/* - Код возврата:                                                  */
/*   - 0   - подпись выработана успешно.                            */
/* - Второе и третье подполя длины NQ 32-разрядных слов поля SField */
/*   заполнены, соответственно, компонентами ЭЦП r'и s.             */
{
        DWORD  FFFFF[2*LNP];
        DWORD  Field[2*LNP];
        DWORD  K[LNP],A[LNP],Tmp1[LNP],Tmp2[LNP];
struct  LPoint C;

  if(CrypC_Lcmp_LC(Q_Words,Sign_H,Q_) >= 0)
     CrypC_LCuModuloQQ(Sign_H,Context);              /* h'= h(mod Q) */
  memset(Tmp1,0,Q_Bytes);
  if(!CrypC_Lcmp_LC(Q_Words,Sign_H,Tmp1))            /* Если h==0(mod Q), */
    SField[Q_Words-1] = 1;                           /* то   h = 1;       */
Pod:
  memset(K,0,P_Bytes);

  CrypC_Gen_SecretKey_(Context->RS,K+P_Words-Q_Words,1);   /* Случайное K. */
  CrypC_NetToHost((K+P_Words-Q_Words),(Q_Words));          /* Network to Host Byteorder. */
#ifdef DEBUG
{//═════════════════════════════════════════════════════════════════════════════════════════════════════════════════
                                                                 /*CCC*/ extern DWORD TEST_K_HBO[LCuNQ];  /* Test K */
                                                                 /*CCC*/ memcpy(K,TEST_K_HBO,Q_Bytes);    /* Test K */
}//═════════════════════════════════════════════════════════════════════════════════════════════════════════════════
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",K[I]);printf("=KR\n");}
#endif

  CrypC_LCuModuloQQ(K+P_Words-Q_Words,Context);            /* K'= K(mod Q) */
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",K[I]);printf("=KM\n");}
#endif
  
  memset(Tmp1,0,P_Bytes);
  if (!memcmp(K,Tmp1,P_Bytes)) goto Pod;                   /* If K' == 0, gen again. */
  CrypC_LCuPower(&AP_, K, &C, Context);                    /* C = (AP)*K */
  CrypC_LProjToAffi(Context, &C);
  CrypC_LMontToInt (Context, &C);
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",C.X[I]);printf("=C.X\n");}
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",C.Y[I]);printf("=C.Y\n");}
#endif
  CrypC_LCuModuloPQ(C.X,Context);                          /* r = C.X(mod Q) */
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",C.X[I]);printf("=C.X(Q)\n");}
#endif
  memcpy(Sign_R,C.X+P_Words-Q_Words,Q_Bytes);
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",Sign_R[I]);printf("=S_R\n");}
#endif
  memset(Tmp1,0,Q_Bytes);
  if(!CrypC_Lcmp_LC(Q_Words,Sign_R,Tmp1)) goto Pod;        /* Если r == 0, gen again. */

  memcpy(Field,Sign_R,Q_Bytes);                     

#ifdef DEBUG
  {                                                        /* Unmask Key to print. */
  int i;
  for(i=0;i<Q_Words;i++)
     Sign_S[i] = SK_[i        ] +                          /* s = X     */
                 SK_[i+Q_Words] ;
  }
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",Sign_S[I]);printf("=Key\n");}
#endif
                                                           /* T = X * r': */
  CrypC_LmultCm(Field, SK_, Q_, Q_Words);                  /* SK_ маскирован! */

  memcpy(A,Field,Q_Bytes);
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",A[I]);printf("=X*R\n");}
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",K[I]);printf("=K\n");}
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",Sign_H[I]);printf("=H\n");}
#endif
  memcpy(Field,K+P_Words-Q_Words,Q_Bytes);
  CrypC_LmultC(Field,Sign_H,Q_,Q_Words);                   /* T = K * H */
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",Field[I]);printf("=K*H\n");}
#endif
  CrypC_LroundC(Sign_S,A    ,Q_,Q_Words);
  CrypC_LroundC(Field ,Field,Q_,Q_Words);
  CrypC_Ladd_PC(Q_Words,Sign_S,Field,Q_);                  /* s = X*r'+K*H */

  CrypC_LCuModuloQQ(Sign_S,Context);

  memset(Tmp1,0,Q_Bytes);
  if (!memcmp(Sign_S,Tmp1,Q_Bytes)) goto Pod;
  return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄SIGN*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
