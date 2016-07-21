#include "CrC_User.h"
#include <stdio.h>

/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹DEFINES*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂIEEEﬂP1363ﬂVERSIONﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂCURVEMULT*/
CrypC_LCuAdd(struct LR34_10_2001 *Context, struct LPoint *B, struct LPoint *C)
/* ÇÎÁ®·´Ô•‚ ·„¨¨„ §¢„Â ‡†ß´®Á≠ÎÂ ‚ÆÁ•™ B ® C ≠† ™‡®¢Æ©, ÆØ®·†≠≠Æ© ™Æ≠‚•™·‚Æ¨.*/
/* èÆ¨•È†•‚ ‡•ß„´Ï‚†‚ ·´Æ¶•≠®Ô ¢ ØÆ´• Ø†‡†¨•‚‡† B.                            */
/* íÆÁ™® - ¢ Ø‡Æ•™‚®¢≠ÎÂ ™ÆÆ‡§®≠†‚†Â, ‚.•., ™†¶§†Ô •·‚Ï ‚‡® ¢•™‚Æ‡† (X,Y,Z)   */
/* ØÆ LCuNP ·´Æ¢.                                                             */
{
DWORD ZF[LCuNP],OF[LCuNP];
DWORD T1[LCuNP],T2[LCuNP],T3[LCuNP],T4[LCuNP];
DWORD T5[LCuNP],T6[LCuNP],T7[LCuNP],T0[LCuNP];
int   ZZ1;
#define BYTES       (Context->P_Size)
#define WORDS       (BYTES/4)
#define Add(x,y)    CrypC_Ladd_PC(WORDS,x,y,Context->P_)
#define Sub(x,y)    CrypC_Lsub_PC(WORDS,x,y,Context->P_)
#define Squa(x)     CrypC_Lpower2C(x,Context->P_,WORDS)
#define Mult(x,y)   CrypC_LmultC(x,y,Context->P_,WORDS)
#define HF          (Context->Half)
#define X0          (B->X)
#define X1          (C->X)
#define X2          (B->X)
#define Y0          (B->Y)
#define Y1          (C->Y)
#define Y2          (B->Y)
#define Z0          (B->Z)
#define Z1          (C->Z)
#define Z2          (B->Z)

memset(ZF,0,BYTES);
memset(OF,0,BYTES);
OF[WORDS-1] = 1;

memcpy(T1,X0,BYTES);                              // T1 = X0.
memcpy(T2,Y0,BYTES);                              // T2 = Y0.
memcpy(T3,Z0,BYTES);                              // T3 = Z0.
memcpy(T4,X1,BYTES);                              // T4 = X1.
memcpy(T5,Y1,BYTES);                              // T5 = Y1.
memcpy(T0,Z1,BYTES);
Mult  (T0,OF);                                    // Real Z1.
ZZ1 = CrypC_Lcmp_LC(WORDS,T0,OF);
if(ZZ1)                                           // If (Z1 != 1)
  {
  memcpy(T6,Z1,BYTES);                            // T6 = Z1.
  memcpy(T7,T6,BYTES);                            // T7 =...
  Squa  (T7   );                                  // ...= T6^2.
  Mult  (T1,T7);                                  // T1 = T1*T7.
  Mult  (T7,T6);                                  // T7 = T6*T7.
  Mult  (T2,T7);                                  // T2 = T2*T7.
  }
memcpy(T7,T3,BYTES);                              // T7 =...
Squa  (T7   );                                    // ...= T3^2.
Mult  (T4,T7);                                    // T4 = T4*T7.
Mult  (T7,T3);                                    // T7 = T3*T7.
Mult  (T5,T7);                                    // T5 = T5*T7.
memcpy(T0,T1,BYTES);                              // T1.
Sub   (T0,T4);                                    // T1-T4.
memcpy(T4,T0,BYTES);                              // T4 = T1-T4.
memcpy(T0,T2,BYTES);                              // T2.
Sub   (T0,T5);                                    // T2-T5.
memcpy(T5,T0,BYTES);                              // T5 = T2-T5.
if(!CrypC_Lcmp_LC(WORDS,T4,ZF))                   // If (T4 == 0)
  {
  if(!CrypC_Lcmp_LC(WORDS,T5,ZF))                 // If (T5 == 0)
    {
    CrypC_LCuDub(Context, B);
    return(0);                                    // ret(2B);
    }
  else
    {
    memset(B,0,LPTSIZE);
    X2[WORDS-1] = Y2[WORDS-1] = 1;
    CrypC_LIntToMont(Context,B);
    return(0);                                    // ret(1,1,0);
    }
  }
Add   (T1,T1);                                    // 2*T1.
Sub   (T1,T4);                                    // T1 = 2*T1-T4.
Add   (T2,T2);                                    // 2*T2.
Sub   (T2,T5);                                    // T2 = 2*T2-T5.
if(ZZ1)                                           // If (Z1 != 1)
  Mult(T3,T6);                                    // T3 = T3*T6.
Mult  (T3,T4);                                    // T3 = T3*T4   = Z2.
memcpy(T7,T4,BYTES);                              // T7 =...
Squa  (T7   );                                    // ...= T4^2.
Mult  (T4,T7);                                    // T4 = T4*T7.
Mult  (T7,T1);                                    // T7 = T1*T7.
memcpy(T1,T5,BYTES);                              // T1 =...
Squa  (T1   );                                    // ...= T5^2.
Sub   (T1,T7);                                    // T1 = T1-T7   = X2.
Sub   (T7,T1);                                    // T7 =...
Sub   (T7,T1);                                    // ...= T7-2*T1.
Mult  (T5,T7);                                    // T5 = T5*T7.
Mult  (T4,T2);                                    // T4 = T2*T4.
memcpy(T2,T5,BYTES);                              // T2 =...
Sub   (T2,T4);                                    // ...= T5-T4.
Mult  (T2,HF);                                    // T2 = T2/2.   = Y2.
memcpy(X2,T1,BYTES);                              // X2 = T1.
memcpy(Y2,T2,BYTES);                              // Y2 = T2.
memcpy(Z2,T3,BYTES);                              // Z2 = T3.
return(0);
#undef WORDS
#undef BYTES
#undef Add
#undef Sub
#undef Squa
#undef Mult
#undef HF
#undef X0
#undef X1
#undef X2
#undef Y0
#undef Y1
#undef Y2
#undef Z0
#undef Z1
#undef Z2
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹IEEE‹P1363‹VERSION‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹CURVEMULT*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂIEEEﬂP1363ﬂVERSIONﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂCURVESQUA*/
CrypC_LCuDub(struct LR34_10_2001 *Context, struct LPoint *B)
/* ÇÎÁ®·´Ô•‚ ‡•ß„´Ï‚†‚ „§¢Æ•≠®Ô ‚ÆÁ™® B ≠† ™‡®¢Æ©, ÆØ®·†≠≠Æ© ™Æ≠‚•™·‚Æ¨.      */
/* èÆ¨•È†•‚ ‡•ß„´Ï‚†‚ ·´Æ¶•≠®Ô ¢ ØÆ´• Ø†‡†¨•‚‡† B.                            */
/* íÆÁ™® - ¢ Ø‡Æ•™‚®¢≠ÎÂ ™ÆÆ‡§®≠†‚†Â, ‚.•., ™†¶§†Ô •·‚Ï ‚‡® ¢•™‚Æ‡† (X,Y,Z)   */
/* ØÆ CuNP ·´Æ¢.                                                              */
{
DWORD ZF[LCuNP],OF[LCuNP];
DWORD T1[LCuNP],T2[LCuNP],T3[LCuNP],T4[LCuNP],T5[LCuNP],T0[LCuNP];
#define BYTES       (Context->P_Size)
#define WORDS       (BYTES/4)
#define Add(x,y)    CrypC_Ladd_PC(WORDS,x,y,Context->P_)
#define Sub(x,y)    CrypC_Lsub_PC(WORDS,x,y,Context->P_)
#define Squa(x)     CrypC_Lpower2C(x,Context->P_,WORDS)
#define Mult(x,y)   CrypC_LmultC(x,y,Context->P_,WORDS)
#define X1          (B->X)
#define Y1          (B->Y)
#define Z1          (B->Z)
#define X2          (B->X)
#define Y2          (B->Y)
#define Z2          (B->Z)

memset(ZF,0,BYTES);
memset(OF,0,BYTES);
OF[WORDS-1] = 1;

memcpy(T1,X1,BYTES);                              // T1 = X1.
memcpy(T2,Y1,BYTES);                              // T2 = Y1.
memcpy(T3,Z1,BYTES);                              // T3 = Z1.
if(!CrypC_Lcmp_LC(WORDS,T2,ZF) ||                 // If (T2 == 0)
   !CrypC_Lcmp_LC(WORDS,T3,ZF)   )                // or (T3 == 0)
  {                                               // return(1,1,0).
  memset(B,0,LPTSIZE);
  X2[WORDS-1] = Y2[WORDS-1] = 1;
  CrypC_LIntToMont(Context,B);
  return(0);
  }
memcpy(T4,Context->a_,BYTES);                     // T4 = a.
memcpy(T5,T3,BYTES);                              // T5 =...
Squa  (T5   );                                    // ...= T3^2.
Squa  (T5   );                                    // T5 = T5^2.
Mult  (T5,T4);                                    // T5 = T4*T5.
memcpy(T4,T1,BYTES);                              // T4 =...
Squa  (T4   );                                    // ...= T1^2.
memcpy(T0,T4,BYTES);                              // T4.
Add   (T4,T0);                                    // T4 =...
Add   (T4,T0);                                    // ...= 3*T4.
Add   (T4,T5);                                    // T4 = T4+T5.
Mult  (T3,T2);                                    // T3 = T2*T3.
Add   (T3,T3);                                    // T3 = 2*T3.
Squa  (T2   );                                    // T2 = T2^2.
memcpy(T5,T1,BYTES);                              // T5 =...
Mult  (T5,T2);                                    // ...= T1*T2.
Add   (T5,T5);                                    // T5 =...
Add   (T5,T5);                                    // ...= 4*T5.
memcpy(T1,T4,BYTES);                              // T1 =...
Squa  (T1   );                                    // ...= T4^2.
Sub   (T1,T5);                                    // T1 =...
Sub   (T1,T5);                                    // ...= T1-2*T5.
Squa  (T2   );                                    // T2 = T2^2.
Add   (T2,T2);                                    // T2 =...
Add   (T2,T2);                                    // ...=...
Add   (T2,T2);                                    // ...= 8*T2.
Sub   (T5,T1);                                    // T5 = T5-T1.
Mult  (T5,T4);                                    // T5 = T4*T5.
memcpy(Y2,T5,BYTES);                              // T2 =...
Sub   (Y2,T2);                                    // ...= T5-T2; Y2 = T2.
memcpy(X2,T1,BYTES);                              // X2 = T1.
memcpy(Z2,T3,BYTES);                              // Z2 = T3.
return(0);
#undef WORDS
#undef BYTES
#undef Add
#undef Sub
#undef Squa
#undef Mult
#undef X1
#undef Y1
#undef Z1
#undef X2
#undef Y2
#undef Z2
}
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹IEEE‹P1363‹VERSION‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹CURVESQUA*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂPOWERC0256*/
void CrypC_LCuPower(struct LPoint       *Y      ,
                           DWORD        *M      ,
                    struct LPoint       *Out    ,
                    struct LR34_10_2001 *Context)
/* ë™†´Ô‡≠Æ• „¨≠Æ¶•≠®• ‚ÆÁ™® Y ™‡®¢Æ©, ÆØ®·†≠≠Æ© ¢ Context, ≠† Ê•´Æ• M: */
/* Out = (Y * M).                                                       */
/* ÇÂÆ§≠Î• Ø†‡†¨•‚‡Î:                                                   */
/* - ì™†ß†‚•´Ï *Y ≠† °„‰•‡, ·Æ§•‡¶†È®© Ø‡Æ•™‚®¢≠Æ• Ø‡•§·‚†¢´•≠®• ‚ÆÁ™®. */
/* - ì™†ß†‚•´Ï *M ≠† °„‰•‡, ·Æ§•‡¶†È®© Ê•´Î© ¨≠Æ¶®‚•´Ï (CuNQ ·´Æ¢).     */
/* - ì™†ß†‚•´Ï *Out ≠† °„‰•‡ §´Ô ™‡†‚≠Æ© ‚ÆÁ™®.                         */
/* ÇÎÂÆ§:                                                               */
/* - ê•ß„´Ï‚†‚ „¨≠Æ¶•≠®Ô ≠† ·™†´Ô‡ M ¢ ØÆ´• Out.                        */
{
#define P_Bytes (Context->P_Size)
#define P_Words (P_Bytes/4)
#define Q_Bytes (Context->Q_Size)
#define Q_Words (Q_Bytes/4)
       int    i,n;
       DWORD  mask;                             /* ê†°ÆÁ®• ØÆ´Ô. */
struct LPoint w;
//                       struct LPoint ww;
//memset(&w,0,LPTSIZE);                         /* X = (1,1,0) = CurveZero: */
//w.X[WORDS-1] = w.Y[WORDS-1] = 1;
//memcpy(&ww,Y,LPTSIZE); CrypC_LMontToInt(Context,&ww);
//{int I;for(I=0;I<8;I++)printf("%08lX",ww.X[I]);printf(" = Y.X\n");}
//{int I;for(I=0;I<8;I++)printf("%08lX",ww.Y[I]);printf(" = Y.Y\n");}
//{int I;for(I=0;I<8;I++)printf("%08lX",ww.Z[I]);printf(" = Y.Z\n");}
//{int I;for(I=0;I<8;I++)printf("%08lX",M[I]);printf(" = M\n");}

mask = 0x80000000l;
n    = (4*8*P_Words)-1;
i=0;
while(!(M[i>>5]&mask) && i <= n)                /* èÆ®·™ Ø•‡¢Æ£Æ °®‚†=1 ¢ M */
   {
   i++;
   mask = (mask>>1) ^ (mask<<31);               /* ë§¢®£ ¨†·™® ¢Ø‡†¢Æ ≠† °®‚. */
   }
                                                // printf("Start with i=%d\n",i);
if(i<n)
  {
  memcpy(&w,Y,LPTSIZE);                         /* X  = (Y) Ø•‡¢Î© ‡†ß. */
  i++;
  mask = (mask>>1) ^ (mask<<31);                /* ë§¢®£ ¨†·™® ¢Ø‡†¢Æ ≠† °®‚. */

  for(;i<=n;i++)
     {
     CrypC_LCuDub  (Context,&w);                /* X *= 2. */
//memcpy(&ww,&w,LPTSIZE); CrypC_LMontToInt(Context,&ww);
//{int I;printf("X%03ds=",n-i);for(I=0;I<8;I++)printf("%08lX",ww.X[I]);printf("\n");}
//{int I;printf("Y%03ds=",n-i);for(I=0;I<8;I++)printf("%08lX",ww.Y[I]);printf("\n");}
//{int I;printf("Z%03ds=",n-i);for(I=0;I<8;I++)printf("%08lX",ww.Z[I]);printf("\n");}
     if(M[i>>5] & mask)                        /* Å®‚ ≠Æ¨•‡ i ¢ M. */
       {
       CrypC_LCuAdd(Context,&w,Y);             /* Ö·´® °®‚==1, X += (Y). */
//memcpy(&ww,&w,LPTSIZE); CrypC_LMontToInt(Context,&ww);
//{int I;printf("X%03dm=",n-i);for(I=0;I<8;I++)printf("%08lX",ww.X[I]);printf("\n");}
//{int I;printf("Y%03dm=",n-i);for(I=0;I<8;I++)printf("%08lX",ww.Y[I]);printf("\n");}
//{int I;printf("Z%03dm=",n-i);for(I=0;I<8;I++)printf("%08lX",ww.Z[I]);printf("\n");}
       }
     mask = (mask>>1) ^ (mask<<31);             /* ë§¢®£ ¨†·™® ¢Ø‡†¢Æ ≠† °®‚. */
     }
  }
memcpy(Out,&w,LPTSIZE);                         /* Out = X. */
}
#undef P_Words
#undef P_Bytes
#undef Q_Words
#undef Q_Bytes
/*‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹POWERC0256*/
/*ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂEOF*/
