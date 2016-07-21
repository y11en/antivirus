#include "CrC_User.h"
#include <stdio.h>

#define N   LNP
#if BYTE_ORDER == LITTLE_ENDIAN
#define LOW 0
#define HIG 1
#else
#define LOW 1
#define HIG 0
#endif

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EPSCOUNT*/
DWORD CrypC_LEPSCount(DWORD C)
{
 WORD e0,Tmp;
 WORD CH,CL;
DWORD e1,TMP;

CL = (WORD)(C & 0x0000FFFF);
CH = (WORD)(C >>  16      );

e0  = 0x0001;
Tmp = CL+1;

while(e0 && Tmp)
     {
     e0++;
     Tmp += CL;
     }

if(!e0) return(0l);

e1  = 0x00000000l + (DWORD)e0;
TMP = (e1 * C) + 1l;

while((e1&0xFFFF0000l) && TMP)
     {
     e1 += 0x00010000l;
     TMP = (e1 * C) + 1l;
     }

if(!e1) return(0l);
return(e1);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄EPSCOUNT*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀POWERC*/
int  CrypC_LpowerC(DWORD  *Y  ,
                   DWORD  *M  ,
                   DWORD  *Out,
                   int     Ind,
                   DWORD  *D  ,
                   int     L  )
/* Вычисление степени: Out = ((Y ** M) * 1**Ind) (mod D), Ind = 0 или 1 */
/* Входные параметры:                                                   */
/* - Указатель *Y на буфер, содержащий основание степени.               */
/* - Указатель *M на буфер, содержащий показатель степени.              */
/* - Указатель *Out на буфер для результата возведения.                 */
/* - Индикатор умножения результата на 1 перед выводом: целое 0 или 1.  */
/* - Указатель *D на буфер, содержащий порядок конечного поля.          */
/* - Размер L в 32-разрядных словах полей Y, M, D и Out.                */
/* Выход:                                                               */
/* - Результат возведения в степень M в поле Out.                       */
{
int   i,n;
DWORD  mask,w[2*LNP];                           /* Рабочие поля. */

//{int I;for(I=0;I<L;I++)printf("%08lX",Y[I]);printf(" = Y.\n");}
//{int I;for(I=0;I<L;I++)printf("%08lX",M[I]);printf(" = Exponent.\n");}
memset(w,0,4*L); w[L-1] = 1;                    /* 1 */

CrypC_LroundC(w,w,D,L);                         /* X = [1]D */

mask = 0x80000000;
n    = (32*L)-1;
i=0;
while(!(M[i>>5]&mask) && i <= n)                /* Поиск первого бита=1 в M */
   {
   i++;
   mask = (mask>>1) ^ (mask<<31);               /* Сдвиг маски вправо на бит. */
   }
memcpy(&w[L],Y,4*L);                            /* Второй операнд = Y. */
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w Beg.\n");}
if(i<n)
  {
  CrypC_LmultC(  (DWORD *)&w[0],                /* X *= (Y) первый раз. */
                 (DWORD *)&w[L],
                 (DWORD *)D    ,
                          L    );
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w1m:%d.\n",i);}
  CrypC_Lpower2C((DWORD *)&w[0],                /* X *=  X  первый раз. */
                 (DWORD *)D    ,
                          L    );
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w1p:%d.\n",i);}
  i++;
  mask = (mask>>1) ^ (mask<<31);                /* Сдвиг маски вправо на бит. */

  for(;i<n;i++)
     {
     if(M[i>>5] & mask)                         /* Бит номер i в M. */
       {
       CrypC_LmultC((DWORD *)&w[0],             /* Если бит==1, X *= (Y). */
                    (DWORD *)&w[L],
                    (DWORD *)D    ,
                             L    );
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w2m:%d.\n",i);}
       }
     CrypC_Lpower2C((DWORD *)&w[0],             /* X *= X. */
                    (DWORD *)D    ,
                             L    );
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w2p:%d.\n",i);}
     mask = (mask>>1) ^ (mask<<31);             /* Сдвиг маски вправо на бит. */
     }
  }
if(M[L-1] & mask)                               /* Последний бит M. */
  {
  CrypC_LmultC((DWORD *)&w[0],                  /* Если бит=1, X *= (Y). */
               (DWORD *)&w[L],
               (DWORD *)D    ,
                        L    );
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w3m:%d.\n",i);}
  }
if(Ind)                                         /* Умножить на 1 ? */
  {
  memset(&w[L],0,4*L); w[2*L-1] = 1;            /* Второй операнд = 1. */
  CrypC_LmultC((DWORD *)&w[0],                  /* X *= 1. */
               (DWORD *)&w[L],
               (DWORD *)D    ,
                        L    );
//{int I;for(I=0;I<L;I++)printf("%08lX",w[I]);printf(" = w4m.\n");}
  }
memcpy(Out,w,4*L);                              /* Out = X. */

return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄POWERC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀MULTC▀&▀POWER2C▀DEFINES*/
#define Eps  D[L]
#define I(n) (M-(n))
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄MULTC▄&▄POWER2C▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀MULTC*/
int CrypC_LmultC(DWORD  *B,                  /* Первый операнд.         */
                 DWORD  *C,                  /* Второй операнд.         */
                 DWORD  *D,                  /* Простое число.          */
                  int    L)                  /* Длина операндов.        */
/*                Умножение больших чисел по модулю D                   */
/* Входные параметры:                                                   */
/* - Указатель *B на буфер, содержащий первый операнд умножения.        */
/* - Указатель *C на буфер, содержащий второй операнд умножения.        */
/* - Указатель *D на буфер, содержащий порядок конечного поля.          */
/* - Размер L в 32-разрядных словах полей B, C, D.                      */
/* Выход:                                                               */
/* - Результат умножения B*C (mod D) замещает первый операнд в поле B.  */
{
 DWORD  X[3],A[N];                        /* Рабочие поля.       */
 union  UQW U,V,W;                        /* Рабочие переменные. */
 int    i,k,m,M;

//{int I;for(I=0;I<L;I++)printf("%08lX",B[I]);printf(" = B.\n");}
//{int I;for(I=0;I<L;I++)printf("%08lX",C[I]);printf(" = C.\n");}
//{int I;for(I=0;I<L+1;I++)printf("%08lX",D[I]);printf(" = P,Eps.\n");}
M = L - 1;
if(L>N) return(1);                        /* Неверная длина операндов. */
W.L    = (QWORD)B[I(0)] *                    /* */
         (QWORD)C[I(0)] ;                   /* */
X[0]   = W.S[LOW];                         /* */
X[1]   = W.S[HIG];                        /* X    = B(0)*C(0); */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
X[2]   =     0 ;
W.L    = (QWORD)X[0]    *                   /* */
         (QWORD)Eps     ;                  /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
A[I(0)]= W.S[LOW];                        /* A[0] = (X[0]*Eps) (mod r); */
U.S[LOW] = X[0];                                 /* */
U.S[HIG] = X[1];                                /* */
V.L    = (QWORD)A[I(0)] *                      /* */
         (QWORD)D[I(0)] ;                     /* */
//                                                printf("(U1,0) = %08lX %08lX\n",U.S[HIG],U.S[LOW]);
//                                                printf("(V1,0) = %08lX %08lX\n",V.S[HIG],V.S[LOW]);
W.L    = U.L + V.L;                          /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
X[0]   = W.S[LOW];                          /* */
X[1]   = W.S[HIG];                         /* */
if(W.L<U.L) X[2]++;                       /* X   += A[0]*D[0]; */
X[0]   = X[1];                              /* */
X[1]   = X[2];                             /* */
X[2]   =   0 ;                            /* X   /= r; */
for(k=1;k<L;k++)                          /* L-1 итераций цикла. */
   {
   for(i=0;i<k;i++)
      {
      U.S[LOW] = X[0];                           /* */
      U.S[HIG] = X[1];                          /* */
      V.L    = (QWORD)B[I(i)]   *              /* */
               (QWORD)C[I(k-i)] ;             /* */
      W.L    = U.L + V.L;                    /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*C[k-i]; */
      U.S[LOW] = X[0];                           /* */
      U.S[HIG] = X[1];                          /* */
      V.L    = (QWORD)A[I(i)]   *              /* */
               (QWORD)D[I(k-i)] ;             /* */
      W.L    = U.L + V.L;                    /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[k-i]; */
      }
   U.S[LOW] = X[0];                              /* */
   U.S[HIG] = X[1];                             /* */
   V.L    = (QWORD)B[I(k)] *                   /* */
            (QWORD)C[I(0)] ;                  /* */
   W.L    = U.L + V.L;                       /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
   X[0]   = W.S[LOW];                       /* */
   X[1]   = W.S[HIG];                      /* */
   if(W.L<U.L) X[2]++;                    /* X += B[k]*C[0]; */
   W.L    = (QWORD)X[0]    *                /* */
            (QWORD)Eps     ;               /* */
   A[I(k)]= W.S[LOW];                     /* A[k] = (X[0]*Eps) (mod r); */
   U.S[LOW] = X[0];                              /* */
   U.S[HIG] = X[1];                             /* */
   V.L    = (QWORD)A[I(k)] *                   /* */
            (QWORD)D[I(0)] ;                  /* */
   W.L    = U.L + V.L;                       /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
   X[0]   = W.S[LOW];                       /* */
   X[1]   = W.S[HIG];                      /* */
   if(W.L<U.L) X[2]++;                    /* X += A[k]*D[0]; */
   X[0]   = X[1];                           /* */
   X[1]   = X[2];                          /* */
   X[2]   =   0 ;                         /* X   /= r; */
   }
m = 2*L-1;
for(k=L;k<m;k++)                          /* Также L-1 итераций. */
   {
   for(i=k-L+1;i<L;i++)
      {
      U.S[LOW] = X[0];                           /* */
      U.S[HIG] = X[1];                          /* */
      V.L    = (QWORD)B[I(i)]   *              /* */
               (QWORD)C[I(k-i)] ;             /* */
      W.L    = U.L + V.L;                    /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*C[k-i]; */
      U.S[LOW] = X[0];                           /* */
      U.S[HIG] = X[1];                          /* */
      V.L    = (QWORD)A[I(i)]   *              /* */
               (QWORD)D[I(k-i)] ;             /* */
      W.L    = U.L + V.L;                    /* */
//                                                printf("(1,0) = %08lX %08lX\n",W.S[HIG],W.S[LOW]);
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[k-i]; */
      }
   B[I(k-L)] = X[0];                      /* B[k-L] = X[0]; */
   X[0]   = X[1];                           /* */
   X[1]   = X[2];                          /* */
   X[2]   =   0 ;                         /* X   /= r; */
   }
B[I(L-1)] = X[0];                         /* B[L-1] = X[0]; */

while(X[1])
     {
     CrypC_Lsub_LC(L,B,D);
     X[1]--;
     }
while(CrypC_Lcmp_LC(L,B,D) >= 0)
      CrypC_Lsub_LC(L,B,D);

return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄MULTC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀MULTCM*/
int CrypC_LmultCm(DWORD  *B,                 /* Первый операнд.         */
                  DWORD  *C,                 /* Второй операнд.         */
                  DWORD  *D,                 /* Простое число.          */
                   int    L)                 /* Длина операндов.        */
/*                Умножение больших чисел по модулю D                   */
/* Входные параметры:                                                   */
/* - Указатель *B на буфер, содержащий первый операнд умножения.        */
/* - Указатель *C на буфер, содержащий второй операнд умножения.        */
/*   Второй операнд замаскирован процедурой Mask_SecretKey_ и           */
/*   имеет двойную длину.                                               */
/* - Указатель *D на буфер, содержащий порядок конечного поля.          */
/* - Размер L в 32-разрядных словах полей B, C, D.                      */
/* Выход:                                                               */
/* - Результат умножения B*C (mod D) замещает первый операнд в поле B.  */
{
 DWORD  X[3],A[N];                        /* Рабочие поля.       */
 union  UQW U,V,W;                        /* Рабочие переменные. */
 int    i,k,m,M;
M = L - 1;
if(L>N) return(1);                        /* Неверная длина операндов. */
W.L    = (QWORD)  B[I(0)]    *                 /* */
         (QWORD)((C[I(0)  ]  +                /* */
                  C[I(0)+L]) &               /* */
                  0xffffffff);              /* */ /* Маска. */
X[0]   = W.S[LOW];                         /* */
X[1]   = W.S[HIG];                        /* X    = B(0)*C(0); */
X[2]   =     0 ;
W.L    = (QWORD)X[0]    *                   /* */
         (QWORD)Eps     ;                  /* */
A[I(0)]= W.S[LOW];                        /* A[0] = (X[0]*Eps) (mod r); */
U.S[LOW] = X[0];                                 /* */
U.S[HIG] = X[1];                                /* */
V.L    = (QWORD)A[I(0)] *                      /* */
         (QWORD)D[I(0)] ;                     /* */
W.L    = U.L + V.L;                          /* */
X[0]   = W.S[LOW];                          /* */
X[1]   = W.S[HIG];                         /* */
if(W.L<U.L) X[2]++;                       /* X   += A[0]*D[0]; */
X[0]   = X[1];                              /* */
X[1]   = X[2];                             /* */
X[2]   =   0 ;                            /* X   /= r; */
for(k=1;k<L;k++)                          /* L-1 итераций цикла. */
   {
   for(i=0;i<k;i++)
      {
      U.S[LOW] = X[0];                             /* */
      U.S[HIG] = X[1];                            /* */
      V.L    = (QWORD)  B[I(i)]     *            /* */
               (QWORD)((C[I(k-i)]   +           /* */
                        C[I(k-i)+L])&          /* */
                        0xffffffff) ;         /* */ /* Маска. */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*C[k-i]; */
      U.S[LOW] = X[0];                           /* */
      U.S[HIG] = X[1];                          /* */
      V.L    = (QWORD)A[I(i)]   *              /* */
               (QWORD)D[I(k-i)] ;             /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[k-i]; */
      }
   U.S[LOW] = X[0];                                /* */
   U.S[HIG] = X[1];                               /* */
   V.L    = (QWORD)  B[I(k)]    *                /* */
            (QWORD)((C[I(0)]    +               /* */
                     C[I(0)+L]) &              /* */
                     0xffffffff);             /* */ /* Маска. */
   W.L    = U.L + V.L;                       /* */
   X[0]   = W.S[LOW];                       /* */
   X[1]   = W.S[HIG];                      /* */
   if(W.L<U.L) X[2]++;                    /* X += B[k]*C[0]; */
   W.L    = (QWORD)X[0]    *                /* */
            (QWORD)Eps     ;               /* */
   A[I(k)]= W.S[LOW];                     /* A[k] = (X[0]*Eps) (mod r); */
   U.S[LOW] = X[0];                              /* */
   U.S[HIG] = X[1];                             /* */
   V.L    = (QWORD)A[I(k)] *                   /* */
            (QWORD)D[I(0)] ;                  /* */
   W.L    = U.L + V.L;                       /* */
   X[0]   = W.S[LOW];                       /* */
   X[1]   = W.S[HIG];                      /* */
   if(W.L<U.L) X[2]++;                    /* X += A[k]*D[0]; */
   X[0]   = X[1];                           /* */
   X[1]   = X[2];                          /* */
   X[2]   =   0 ;                         /* X   /= r; */
   }
m = 2*L-1;
for(k=L;k<m;k++)                          /* Также L-1 итераций. */
   {
   for(i=k-L+1;i<L;i++)
      {
      U.S[LOW] = X[0];                             /* */
      U.S[HIG] = X[1];                            /* */
      V.L    = (QWORD)  B[I(i)]     *            /* */
               (QWORD)((C[I(k-i)]   +           /* */
                        C[I(k-i)+L])&          /* */
                        0xffffffff) ;         /* */ /* Маска. */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*C[k-i]; */
      U.S[LOW] = X[0];                           /* */
      U.S[HIG] = X[1];                          /* */
      V.L    = (QWORD)A[I(i)]   *              /* */
               (QWORD)D[I(k-i)] ;             /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[k-i]; */
      }
   B[I(k-L)] = X[0];                      /* B[k-L] = X[0]; */
   X[0]   = X[1];                           /* */
   X[1]   = X[2];                          /* */
   X[2]   =   0 ;                         /* X   /= r; */
   }
B[I(L-1)] = X[0];                         /* B[L-1] = X[0]; */

while(X[1])
     {
     CrypC_Lsub_LC(L,B,D);
     X[1]--;
     }
while(CrypC_Lcmp_LC(L,B,D) >= 0)
      CrypC_Lsub_LC(L,B,D);

return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄MULTCM*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀POWER2C*/
int CrypC_Lpower2C(DWORD  *B,                 /* Операнд.               */
                   DWORD  *D,                 /* Простое число.         */
                    int    L)                 /* Длина операндов.       */
/*           Возведение в квадрат большого числа по модулю D            */
/* Входные параметры:                                                   */
/* - Указатель *B на буфер, содержащий операнд.                         */
/* - Указатель *D на буфер, содержащий порядок конечного поля.          */
/* - Размер L в 32-разрядных словах полей B, D.                         */
/* Выход:                                                               */
/* - Результат умножения B*B (mod D) замещает операнд в поле B.         */
{
DWORD       X[3],A[N];                    /* Рабочие поля.       */
union       UQW U,V,W;                    /* Рабочие переменные. */
int         i,k,m,M;
M = L - 1;
if(L>N) return(1);                        /* Неверная длина операндов. */
W.L    = (QWORD)B[I(0)] *                    /* */
         (QWORD)B[I(0)] ;                   /* */
X[0]   = W.S[LOW];                         /* */
X[1]   = W.S[HIG];                        /* X    = B(0)*B(0); */
X[2]   =     0 ;
W.L    = (QWORD)X[0]    *                   /* */
         (QWORD)Eps     ;                  /* */
A[I(0)]= W.S[LOW];                        /* A[0] = (X[0]*Eps) (mod r); */
U.S[LOW] = X[0];                                 /* */
U.S[HIG] = X[1];                                /* */
V.L    = (QWORD)A[I(0)] *                      /* */
         (QWORD)D[I(0)] ;                     /* */
W.L    = U.L + V.L;                          /* */
X[0]   = W.S[LOW];                          /* */
X[1]   = W.S[HIG];                         /* */
if(W.L<U.L) X[2]++;                       /* X   += A[0]*D[0]; */
X[0]   = X[1];                              /* */
X[1]   = X[2];                             /* */
X[2]   =   0 ;                            /* X   /= r; */
for(k=1;k<L;k++)                          /* L-1 итераций цикла. */
   {
   for(i=0;i<k;i++)
      {
      if(i>=k-i) break;
      V.L    = (QWORD)B[I(i)]   *                /* */
               (QWORD)B[I(k-i)] ;               /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*B[k-i]; */
      if(i)
        {
        U.S[LOW] = X[0];                       /* */
        U.S[HIG] = X[1];                      /* */
        W.L    = U.L + V.L;                  /* */
        X[0]   = W.S[LOW];                  /* */
        X[1]   = W.S[HIG];                 /* */
        if(W.L<U.L) X[2]++;               /* if(i) X += B[k-i]*B[i]; */
        }
      V.L    = (QWORD)A[I(i)]   *                /* */
               (QWORD)D[I(k-i)] ;               /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[k-i]; */
      if(i)
        {
        V.L    = (QWORD)A[I(k-i)] *              /* */
                 (QWORD)D[I(i)]   ;             /* */
        U.S[LOW] = X[0];                       /* */
        U.S[HIG] = X[1];                      /* */
        W.L    = U.L + V.L;                  /* */
        X[0]   = W.S[LOW];                  /* */
        X[1]   = W.S[HIG];                 /* */
        if(W.L<U.L) X[2]++;               /* X += A[k-i]*D[i]; */
        }
      }
   if(i==k-i)                             /* Нечетное K. */
      {
      V.L    = (QWORD)B[I(i)] *                  /* */
               (QWORD)B[I(i)] ;                 /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*B[i]; */
      V.L    = (QWORD)A[I(i)] *                  /* */
               (QWORD)D[I(i)] ;                 /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[i]; */
      }
   U.S[LOW] = X[0];                              /* */
   U.S[HIG] = X[1];                             /* */
   V.L    = (QWORD)B[I(k)] *                   /* */
            (QWORD)B[I(0)] ;                  /* */
   W.L    = U.L + V.L;                       /* */
   X[0]   = W.S[LOW];                       /* */
   X[1]   = W.S[HIG];                      /* */
   if(W.L<U.L) X[2]++;                    /* X += B[k]*B[0]; */
   W.L    = (QWORD)X[0]    *                /* */
            (QWORD)Eps     ;               /* */
   A[I(k)]= W.S[LOW];                     /* A[k] = (X[0]*Eps) (mod r); */
   U.S[LOW] = X[0];                              /* */
   U.S[HIG] = X[1];                             /* */
   V.L    = (QWORD)A[I(k)] *                   /* */
            (QWORD)D[I(0)] ;                  /* */
   W.L    = U.L + V.L;                       /* */
   X[0]   = W.S[LOW];                       /* */
   X[1]   = W.S[HIG];                      /* */
   if(W.L<U.L) X[2]++;                    /* X += A[k]*D[0]; */
   X[0]   = X[1];                           /* */
   X[1]   = X[2];                          /* */
   X[2]   =   0 ;                         /* X   /= r; */
   }
m = 2*L-1;
for(k=L;k<m;k++)                          /* Также L-1 итераций. */
   {
   for(i=k-L+1;i<L;i++)
      {
      if(i>=k-i) break;
      V.L    = (QWORD)B[I(i)]   *                /* */
               (QWORD)B[I(k-i)] ;               /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*B[k-i]; */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[k-i]*B[i]; */
      V.L    = (QWORD)A[I(i)]   *                /* */
               (QWORD)D[I(k-i)] ;               /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[k-i]; */
      V.L    = (QWORD)A[I(k-i)] *                /* */
               (QWORD)D[I(i)]   ;               /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[k-i]*D[i]; */
      }
   if(i==k-i)                             /* Нечетное K. */
      {
      V.L    = (QWORD)B[I(i)] *                  /* */
               (QWORD)B[I(i)] ;                 /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += B[i]*B[i]; */
      V.L    = (QWORD)A[I(i)] *                  /* */
               (QWORD)D[I(i)] ;                 /* */
      U.S[LOW] = X[0];                         /* */
      U.S[HIG] = X[1];                        /* */
      W.L    = U.L + V.L;                    /* */
      X[0]   = W.S[LOW];                    /* */
      X[1]   = W.S[HIG];                   /* */
      if(W.L<U.L) X[2]++;                 /* X += A[i]*D[i]; */
      }
   B[I(k-L)] = X[0];                      /* B[k-L] = X[0]; */
   X[0]   = X[1];                           /* */
   X[1]   = X[2];                          /* */
   X[2]   =   0 ;                         /* X   /= r; */
   }
B[I(L-1)] = X[0];                         /* B[L-1] = X[0]; */

while(X[1])
     {
     CrypC_Lsub_LC(L,B,D);
     X[1]--;
     }
while(CrypC_Lcmp_LC(L,B,D) >= 0)
      CrypC_Lsub_LC(L,B,D);

return(X[1]);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄POWER2C*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
