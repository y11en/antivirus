#include <stdio.h>
#include "CrC_User.h"

#define P_      (Context->P_)
#define Q_      (Context->Q_)
#define QG_     (Context->QG_)
#define AP_     (Context->AP_)
#define PK_     (Context->PublicKey)
#define P_Bytes (Context->P_Size)
#define P_Words (Context->P_Size/4)
#define Q_Bytes (Context->Q_Size)
#define Q_Words (Context->Q_Size/4)
#define Sign_H  (SField)
#define Sign_R  (SField+Q_Words)
#define Sign_S  (SField+Q_Words+Q_Words)

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀CHECKC*/
int CrypC_LCuCheck(struct LR34_10_2001 *Context, DWORD *SField)
/*        Проверка цифровой подписи в одном буфере данных                         */
/* Входные параметры:                                                             */
/* - Буфер данных *SField, содержащий в трех последовательных                     */
/*   полях длины CuNQ 32-разрядных слов каждое компоненты, необ-                  */
/*   ходимые для проверки подписи, соответственно:                                */
/*   - хэш-вектор H,                                                              */
/*   - компонент ЭЦП r',                                                          */
/*   - компонент ЭЦП s .                                                          */
/* - Контекст с параметрами подписи.                                              */
/* Выход:                                                                         */
/* - Код возврата:                                                                */
/*   - RET_OK             - подпись проверена, результат проверки положительный.  */
/*   - RET_INCORRECTSIGN  - подпись проверена, результат проверки отрицательный.  */
/*   - RET_ZERO_R         - нулевое значение компонента ЭЦП r'.                   */
/*   - RET_ZERO_S         - нулевое значение компонента ЭЦП s .                   */
/*   - RET_BAD_R          - некорректное значение компонента ЭЦП r'.              */
/*   - RET_BAD_S          - некорректное значение компонента ЭЦП s .              */
{
       DWORD  B[LCuNP],A[LCuNP],Tmp1[LCuNP],Tmp2[LCuNP],Field[LCuNP];
struct LPoint Y1, Y2;
#ifdef DEBUG
{int I;for(I=0;I<LCuNQ;I++)printf("%08lX",Sign_H[I]);printf("=S_H\n");}
{int I;for(I=0;I<LCuNQ;I++)printf("%08lX",Sign_R[I]);printf("=S_R\n");}
{int I;for(I=0;I<LCuNQ;I++)printf("%08lX",Sign_S[I]);printf("=S_S\n");}
#endif
if (memcmp(Sign_R,LO_,Q_Bytes)==0)                 /* Сравниваю с O_, ибо там */
                                                   /*  2048, а здесь <= 1024. */ 
   return(RET_ZERO_R);
if (memcmp(Sign_S,LO_,Q_Bytes)==0)
   return(RET_ZERO_S);
if(CrypC_Lcmp_LC(Q_Words,Sign_H,Q_) > 0)
   CrypC_LCuModuloQQ(Sign_H,Context);              /* h'= h(mod Q) */
#ifdef DEBUG
{int I;for(I=0;I<LCuNQ;I++)printf("%08lX",Sign_H[I]);printf("=S_HmodQ\n");}
#endif
if(CrypC_Lcmp_LC(Q_Words,Sign_R,Q_) > 0)
  return(RET_BAD_R);
if(CrypC_Lcmp_LC(Q_Words,Sign_S,Q_) > 0)
  return(RET_BAD_S);
memset(Tmp1,0,Q_Bytes);                            /* Если h==0(mod Q)*/
if(!memcmp(Sign_H,Tmp1,Q_Bytes))
  Sign_H[Q_Words-1] = 1;                           /* то   h = 1;   */ 

memcpy(A,Q_,Q_Bytes);                              /* Q.            */
memset(B,0,Q_Bytes);B[Q_Words-1] = 2;              /* 2.            */
CrypC_Lsub_LC(Q_Words,A,B);                        /* Q-2.          */
memcpy(B,Sign_H,Q_Bytes);                          /* Q.            */
CrypC_LmultC(B, QG_, Q_, Q_Words);                 /* [H]Q.         */
CrypC_LpowerC(B,A,A,1,Q_,Q_Words);                 /* A  = (h**(-1)) mod Q */
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",A[I]);printf("=H**(-1)\n");}
#endif

memcpy(Field,Sign_S,Q_Bytes);                     
CrypC_LmultC(Field,A,Q_,Q_Words);                  /* z1 = s * h**(-1) */

memset(B,0,P_Bytes);
CrypC_LmultC(Field, QG_, Q_, Q_Words);             /* z1 to real.      */
while(CrypC_Lcmp_LC(Q_Words,Field,Q_) > 0)
      CrypC_Lsub_LC(Q_Words,Field,Q_);

memcpy(B+P_Words-Q_Words,Field,Q_Bytes);           /* Save Z1 in P-Wide B Field. */
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",B[I]);printf("=Z1\n");}
#endif

memcpy(Field,Q_,Q_Bytes);                          /* q   */
CrypC_Lsub_LC(Q_Words,Field,Sign_R);               /* q-r */
CrypC_LmultC(Field,A,Q_,Q_Words);                  /* z2 = (q-r) * h**(-1) */
                     
memset(A,0,P_Bytes);
CrypC_LmultC(Field, QG_, Q_, Q_Words);             /* z2 to real.      */
while(CrypC_Lcmp_LC(Q_Words,Field,Q_) > 0)
      CrypC_Lsub_LC(Q_Words,Field,Q_);
memcpy(A+P_Words-Q_Words,Field,Q_Bytes);           /* z2 to P-wide A Field.  */
#ifdef DEBUG
{int I;for(I=0;I<Q_Words;I++)printf("%08lX",A[I]);printf("=Z2\n");}
{int I;for(I=0;I<P_Words;I++)printf("%08lX",AP_.X[I]);printf("=AP.X\n");}
{int I;for(I=0;I<P_Words;I++)printf("%08lX",AP_.Y[I]);printf("=AP.Y\n");}
{int I;for(I=0;I<P_Words;I++)printf("%08lX",AP_.Z[I]);printf("=AP.Z\n");}
{int I;for(I=0;I<P_Words;I++)printf("%08lX",PK_.X[I]);printf("=PK.X\n");}
{int I;for(I=0;I<P_Words;I++)printf("%08lX",PK_.Y[I]);printf("=PK.Y\n");}
{int I;for(I=0;I<P_Words;I++)printf("%08lX",PK_.Z[I]);printf("=PK.Z\n");}
#endif
CrypC_LCuPower(&AP_, B, &Y1, Context);             /* y1 = (AP)*z1 */
CrypC_LCuPower(&PK_, A, &Y2, Context);             /* y2 = (PK)*z2 */
CrypC_LCuAdd(Context, &Y1, &Y2);                   /* (AP)*z1 + (PKey)*z2 */
#ifdef DEBUG
{int I;for(I=0;I<P_Words;I++)printf("%08lX",Y1.X[I]);printf("=C.X-PM\n");}
#endif
CrypC_LProjToAffi(Context, &Y1);
#ifdef DEBUG
{int I;for(I=0;I<P_Words;I++)printf("%08lX",Y1.X[I]);printf("=C.X-M\n");}
#endif
CrypC_LMontToInt(Context, &Y1);
#ifdef DEBUG
{int I;for(I=0;I<P_Words;I++)printf("%08lX",Y1.X[I]);printf("=C.X\n");}
#endif
CrypC_LCuModuloPQ    (Y1.X,Context);
#ifdef DEBUG
{int I;for(I=0;I<P_Words;I++)printf("%08lX",Y1.X[I]);printf("=C.XmQ\n");}
#endif
if(memcmp(Y1.X+P_Words-Q_Words,Sign_R,Q_Bytes))
   return(RET_INCORRECTSIGN);
return(RET_OK);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄CHECKC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
