#include <stdlib.h>
#include <stdio.h>
#include "CrC_User.h"

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀HTONL*/
#if !defined(__APPLE__)
DWORD  HTONL(DWORD X)
/* Вспомогательная процедура для вычисления хэш-функции */
{
#if BYTE_ORDER == LITTLE_ENDIAN
#else
return (X & 0xFF000000) >> 24 ^
       (X & 0x00FF0000) >>  8 ^
       (X & 0x0000FF00) <<  8 ^
       (X & 0x000000FF) << 24 ;
#endif
}
#endif

void   HTON(void *X, int w)
/* Вспомогательная процедура для вычисления хэш-функции */
{
#if BYTE_ORDER == LITTLE_ENDIAN
#else
int i;

for(i=0;i<w;i++)
   ((DWORD *)X)[i] = HTONL(((DWORD *)X)[i]);
#endif
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄HTONL*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀P_P*/
void  CrypC_P_P(BYTE *W, BYTE *K)
/* Вспомогательная процедура для вычисления хэш-функции */
{
int i,k;
HTON(W,LNQ);
for(i=0;i<4;i++) for(k=0;k<8;k++) K[i+4*k]=W[8*i+k];
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄P_P*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀A_A*/
void  CrypC_A_A(BYTE *W, BYTE *K)
/* Вспомогательная процедура для вычисления хэш-функции */
{
int  i;
BYTE TMP[8];

memcpy(TMP ,W   ,8);
memcpy(K   ,W+ 8,8);
memcpy(K+ 8,W+16,8);
memcpy(K+16,W+24,8);
for(i=0;i<8;i++) K[24+i]=TMP[i]^K[i];
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄A_A*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀R_R*/
void  CrypC_R_R(WORD *S)
/* Вспомогательная процедура для вычисления хэш-функции */
{
WORD RG;

HTON(S,LNQ);
RG=S[0]^S[1]^S[2]^S[3]^S[12]^S[15];
S[ 0]=S[ 1]; S[ 1]=S[ 2]; S[ 2]=S[ 3]; S[ 3]=S[ 4];
S[ 4]=S[ 5]; S[ 5]=S[ 6]; S[ 6]=S[ 7]; S[ 7]=S[ 8];
S[ 8]=S[ 9]; S[ 9]=S[10]; S[10]=S[11]; S[11]=S[12];
S[12]=S[13]; S[13]=S[14]; S[14]=S[15]; S[15]=RG;
HTON(S,LNQ);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄R_R*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀HASH*/
void CrypC__Hash(DWORD *K_, BYTE *H,BYTE *M)
/*                        Шаговая хэш-функция                       */
/* Входные параметры:                                               */
/* - Указатель K_ на массив модифицированных подстановок ГОСТ 28147.*/
/* - Начальное состояние хэш-вектора - поле H длины LNQ*4 байта.    */
/* - Блок данных для хэширования - поле M длины LNQ*4 байта.        */
/* Выход:                                                           */
/* - Модифицированние состояние хэш-вектора в поле H.               */
{
BYTE U[LNQ*4],W[LNQ*4],V[LNQ*4],S[LNQ*4],Key[LNK*4];
int  i;

//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)H)[I]);printf(" = H.\n");}
HTON(M,LNQ);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)M)[I]);printf(" = M.\n");}
for(i=0;i<LNQ*4;i++) W[i]=H[i]^M[i];
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)W)[I]);printf(" = W.\n");}
CrypC_P_P(W,Key);
HTON(Key,LNQ);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)Key)[I]);printf(" = Key.\n");}
CrypC__GOSTE_(K_, (DWORD *)Key,(DWORD *)&H[0],(DWORD *)&S[0]);

//         {int I;for(I=0;I<2;I++)printf("%08lX",((DWORD *)S)[I]);printf(" = S0.\n");}
CrypC_A_A(H,U);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)U)[I]);printf(" = U.\n");}
CrypC_A_A(M,V);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)V)[I]);printf(" = V.\n");}
CrypC_A_A(V,V);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)V)[I]);printf(" = V.\n");}
for(i=0;i<LNQ*4;i++) W[i]=U[i]^V[i];
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)W)[I]);printf(" = W.\n");}
CrypC_P_P(W,Key);
HTON(Key,LNQ);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)Key)[I]);printf(" = Key.\n");}
CrypC__GOSTE_(K_, (DWORD *)Key,(DWORD *)&H[8],(DWORD *)&S[8]);
//         {int I;for(I=0;I<2;I++)printf("%08lX",((DWORD *)(S+8))[I]);printf(" = S8.\n");}

CrypC_A_A(U,U);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)U)[I]);printf(" = U.\n");}
((DWORD *)U)[0] ^= 0xFF00FF00;
((DWORD *)U)[1] ^= 0xFF00FF00;
((DWORD *)U)[2] ^= 0x00FF00FF;
((DWORD *)U)[3] ^= 0x00FF00FF;
((DWORD *)U)[4] ^= 0x00FFFF00;
((DWORD *)U)[5] ^= 0xFF0000FF;
((DWORD *)U)[6] ^= 0x000000FF;
((DWORD *)U)[7] ^= 0xFF00FFFF;
//U[31]=~U[31]; U[29]=~U[29]; U[28]=~U[28]; U[24]=~U[24];
//U[23]=~U[23]; U[20]=~U[20]; U[18]=~U[18]; U[17]=~U[17];
//U[14]=~U[14]; U[12]=~U[12]; U[10]=~U[10]; U[ 8]=~U[ 8];
//U[ 7]=~U[ 7]; U[ 5]=~U[ 5]; U[ 3]=~U[ 3]; U[ 1]=~U[ 1];
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)U)[I]);printf(" = U.\n");}
CrypC_A_A(V,V);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)V)[I]);printf(" = V.\n");}
CrypC_A_A(V,V);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)V)[I]);printf(" = V.\n");}
for(i=0;i<LNQ*4;i++) W[i]=U[i]^V[i];
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)W)[I]);printf(" = W.\n");}
CrypC_P_P(W,Key);
HTON(Key,LNQ);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)Key)[I]);printf(" = Key.\n");}
CrypC__GOSTE_(K_, (DWORD *)Key,(DWORD *)&H[16],(DWORD *)&S[16]);

//         {int I;for(I=0;I<2;I++)printf("%08lX",((DWORD *)(S+16))[I]);printf(" = S16.\n");}
CrypC_A_A(U,U);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)U)[I]);printf(" = U.\n");}
CrypC_A_A(V,V);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)V)[I]);printf(" = V.\n");}
CrypC_A_A(V,V);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)V)[I]);printf(" = V.\n");}
for(i=0;i<LNQ*4;i++) W[i]=U[i]^V[i];
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)W)[I]);printf(" = W.\n");}
CrypC_P_P(W,Key);
HTON(Key,LNQ);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)Key)[I]);printf(" = Key.\n");}
CrypC__GOSTE_(K_, (DWORD *)Key,(DWORD *)&H[24],(DWORD *)&S[24]);

//         {int I;for(I=0;I<2;I++)printf("%08lX",((DWORD *)(S+24))[I]);printf(" = S24.\n");}
for(i=0;i<12  ;i++)             CrypC_R_R((WORD *)S);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)S)[I]);printf(" = S.\n");}
for(i=0;i<LNQ*4;i++) S[i]^=M[i];CrypC_R_R((WORD *)S);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)S)[I]);printf(" = S.\n");}
for(i=0;i<LNQ*4;i++) S[i]^=H[i];
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)S)[I]);printf(" = S.\n");}
for(i=0;i<61  ;i++)             CrypC_R_R((WORD *)S);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)S)[I]);printf(" = S.\n");}
memcpy(H,S,LNQ*4);
//         {int I;for(I=0;I<LNQ;I++)printf("%08lX",((DWORD *)H)[I]);printf(" = H.\n");}
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄HASH*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ROUNDC*/
int CrypC_LroundC(DWORD *C, DWORD *B, DWORD *P, int L)
/* Вспомогательная процедура для вычисления цифровой подписи */
/*                C = (B * 2**(32*L))(mod P)                 */
{
int  i, N2;
DWORD PP[LNP+1],CC[LNP+1];

//                                     {int I;for(I=0;I<L;I++)printf("%08lX",B[I]);printf(" = B.\n");}
//                                     {int I;for(I=0;I<L;I++)printf("%08lX",P[I]);printf(" = P.\n");}
N2 = L*32;
CC[0] = PP[0] = 0;
memcpy(CC+1,B,4*L);
memcpy(PP+1,P,4*L);
while(CrypC_Lcmp_LC(L+1,CC,PP) >= 0)           /* C (mod P). */
   CrypC_Lsub_LC(L+1,CC,PP);
//                                     {int I;for(I=0;I<L+1;I++)printf("%08lX",CC[I]);printf(" = BmodP.\n");}

for(i=0;i<N2;i++)
   {
   CrypC_Lshl_LC(L+1,CC);                      /* C *= 2; */
//                                     {int I;for(I=0;I<L+1;I++)printf("%08lX",CC[I]);printf(" = CCshl.\n");}
   while(CrypC_Lcmp_LC(L+1,CC,PP) >= 0)        /* C (mod P). */
      CrypC_Lsub_LC(L+1,CC,PP);
//                                     {int I;for(I=0;I<L+1;I++)printf("%08lX",CC[I]);printf(" = CCshlmodP.\n");}
   }
memcpy(C,CC+1,4*L);
//                                     {int I;for(I=0;I<L;I++)printf("%08lX",C[I]);printf(" = C.\n");}
return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ROUNDC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀CMP_LC*/
int CrypC_Lcmp_LC(int L, DWORD *Left, DWORD *Rigt)
/* Сравнение двух длинных беззнаковых чисел */
/* длины L 32-разрядных слов.               */
/* Возвращает <=> 0.                        */
{
int i;

for(i=0;i<L;i++)
   {
   if(Left[i] < Rigt[i]) return(-1);
   if(Left[i] > Rigt[i]) return( 1);
   }
return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄CMP_LC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀SUB_LC*/
int CrypC_Lsub_LC(int L, DWORD *Left, DWORD *Rigt)
/* Вычитание двух длинных беззнаковых чисел */
/* длины L 32-разрядных слов:               */
/* Left -= Rigt (mod 2**(32*L))             */
/* Возвращает <=> 0.                        */
{
int   i;
QWORD j;
union UQW T;

j = 0ULL;
for(i=L-1;i>=0;i--)
   {
   T.L = ((QWORD)Left[i] - j) - (QWORD)Rigt[i];
#if BYTE_ORDER == LITTLE_ENDIAN
   Left[i] = T.S[0];
   if(T.S[1]) j = 1L;                    /* Borrow. */
#else
   Left[i] = T.S[1];
   if(T.S[0]) j = 1L;                    /* Borrow. */
#endif
   else       j = 0L;
   }
return((int)j);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄SUB_LC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ADD_LC*/
int CrypC_Ladd_LC(int L, DWORD *Left, DWORD *Rigt)
/* Сложение двух длинных беззнаковых чисел      */
/* длины L 32-разрядных слов:                   */
/* Left += Rigt (mod 2**(32*L))                 */
/* Возвращает перенос за пределы рабочего поля. */
{
int    i;
QWORD  j;
union UQW T;


j = 0ULL;
for(i=L-1;i>=0;i--)
   {
   T.L = (QWORD)Left[i] + (QWORD)Rigt[i] + j;
#if BYTE_ORDER == LITTLE_ENDIAN
   Left[i] = T.S[0];
   j = T.S[1];                           /* Carry. */
#else
   Left[i] = T.S[1];
   j = T.S[0];                           /* Carry. */
#endif
   }
return((int)j);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ADD_LC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ADD_LBC*/
int CrypC__add_LbC(int L, BYTE *Left, BYTE *Rigt)
/* Сложение двух длинных беззнаковых чисел      */
/* длины L 8-разрядных слов:                    */
/* Left += Rigt (mod 2**(8*L))                  */
/* Возвращает перенос за пределы рабочего поля. */
{
int   i;
WORD  j;
union USW T;

HTON(Left,L/4);
HTON(Rigt,L/4);

j = 0;
for(i=0;i<L;i++)
   {
   T.S = (WORD)Left[i] + (WORD)Rigt[i] + j;
#if BYTE_ORDER == LITTLE_ENDIAN
   Left[i]  =  T.B[0];
   j   = (WORD)T.B[1];                   /* Carry. */
#else
   Left[i]  =  T.B[1];
   j   = (WORD)T.B[0];                   /* Carry. */
#endif
   }

HTON(Left,L/4);
return(j);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ADD_LBC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀ADD_PC*/
void CrypC_Ladd_PC(int L, DWORD *Left, DWORD *Rigt, DWORD *P)
/* Сложение двух длинных беззнаковых чисел      */
/* длины L 32-разрядных слов по модулю P той же */
/* длины:                                       */
/* Left += Rigt (mod P).                        */
{
int   j;
DWORD PP[LNP+1],LL[LNP+1],RR[LNP+1];

LL[0] = RR[0] = PP[0] = 0;
memcpy(LL+1,Left,4*L);
memcpy(RR+1,Rigt,4*L);
memcpy(PP+1,P   ,4*L);

j = CrypC_Ladd_LC(L+1, LL, RR);
Comp:
if(j) goto SubP;
j = CrypC_Lcmp_LC(L+1, LL, PP);
if(j<0)
  {
  memcpy(Left,LL+1,4*L);
  return;
  }
SubP:
j = CrypC_Lsub_LC(L+1, LL, PP);
goto Comp;
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ADD_PC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀SUB_PC*/
void CrypC_Lsub_PC(int L, DWORD *Left, DWORD *Rigt, DWORD *P)
/* Вычитание двух длинных беззнаковых чисел     */
/* длины L 32-разрядных слов по модулю P той же */
/* длины:                                       */
/* Left -= Rigt (mod P).                        */
{
DWORD PP[LNP+1],LL[LNP+1],RR[LNP+1];

LL[0] = RR[0] = PP[0] = 0;
memcpy(LL+1,Left,4*L);
memcpy(RR+1,Rigt,4*L);
memcpy(PP+1,P   ,4*L);

while( 1 )
 { if( CrypC_Lcmp_LC(L+1, LL, RR) >= 0 ) break;
   CrypC_Ladd_LC(L+1, LL, PP);
 }
CrypC_Lsub_LC(L+1, LL, RR);
memcpy(Left,LL+1,4*L);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄SUB_PC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀SHL_LC*/
int CrypC_Lshl_LC(int L, DWORD *F)
/* Сдвиг влево на 1 разряд длинного беззнакового числа */
/* длины L 32-разрядных слов.                          */
/* Возвращает значение разряда, сдвинутого за пределы  */
/* рабочего поля.                                      */
{
 int  i;
QWORD j;
union UQW T;

j = 0ULL;
for(i=L-1;i>=0;i--)
   {
   T.L = (((QWORD)F[i]) << 1) ^ j;
#if BYTE_ORDER == LITTLE_ENDIAN
   F[i] = T.S[0];
   j = T.S[1];                           /* Перенос. */
#else
   F[i] = T.S[1];
   j = T.S[0];                           /* Перенос. */
#endif
   }
return( (int)j);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄SHL_LC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀SHR_LC*/
int CrypC_Lshr_LC(int L, DWORD *F)
/* Сдвиг вправо на 1 разряд длинного беззнакового числа */
/* длины L 32-разрядных слов.                           */
/* Возвращает значение разряда, сдвинутого за пределы   */
/* рабочего поля.                                       */
{
int   i;
QWORD j;
union UQW T;

j = 0L;
for(i=0;i<L;i++)
   {
   T.L = (((QWORD)F[i]) << 31) ^ (j << 63);
#if BYTE_ORDER == LITTLE_ENDIAN
   F[i] = T.S[1];
   j = T.S[0]>>31;                       /* Перенос. */
#else
   F[i] = T.S[0];
   j = T.S[1]>>31;                       /* Перенос. */
#endif
   }
return((int)j);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄SHR_LC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀DIV_LC*/
void CrypC_Ldiv_LC (int    L,
                    DWORD *Dividend,              /* Делимое.  */
                    DWORD *Divisor,               /* Делитель. */
                    DWORD *Quotient,              /* Частное.  */
                    DWORD *Rest)                  /* Остаток.  */
/* Деление с остатком длинного беззнакового числа длины L 32-разрядных слов */
/* на длинное беззнаковое число длины L 32-разрядных слов.                  */
{
int   i, LL;
DWORD Mask[LNP];
DWORD Temp[LNP];

LL = 4*L;
if(CrypC_Lcmp_LC(L,Dividend,Divisor) < 0)      /* if Dividend < Divisor */
  {
  memset(Quotient,0,LL);
  memcpy(Rest,Dividend,LL);
  return;
  }
memset(Mask    ,0,LL);
Mask[L-1] = 1;
memset(Quotient,0,LL);                       /* Заполнить поле результата нулями.     */
memset(Rest    ,0,LL);                       /* Заполнить поле результата нулями.     */
memcpy(Temp,Divisor ,LL);                    /* Для вычисения Divisor << i.           */
memcpy(Rest,Dividend,LL);                    /* Для вычисления Остатка.               */
while(CrypC_Lcmp_LC(L,Temp,Dividend) < 0)    /* Пока Divisor*(2**i) < Dividend        */
     {
       CrypC_Lshl_LC(L,Mask);                /* i = выдвинутый бит.                   */
     i=CrypC_Lshl_LC(L,Temp);
     if(i) break;                            /* Переполнение.                         */
     }
if(i || CrypC_Lcmp_LC(L,Temp,Dividend) > 0)  /* Надо: max Divisor*(2**i) <= Dividend. */
  {
  CrypC_Lshr_LC(L,Mask);
  CrypC_Lshr_LC(L,Temp);
  if(i) Temp[0] |= 0x80000000;               /* Вернуть выдвигавшийся за пределы бит. */
  }
while(CrypC_Lcmp_LC(L,Temp,Divisor) >= 0)    /* Пока Divisor*(2**i) >= Divisor        */
     {
     if(CrypC_Lcmp_LC(L,Temp,Rest) <= 0)     /* Если Divisor*(2**i) <= Rest           */
       {
       CrypC_Ladd_LC(L,Quotient,Mask);       /* Прибавить соотв. бит к Quotient.      */
       CrypC_Lsub_LC(L,Rest,Temp);           /* Rest -= Divisor*(2**i).               */
       }
     CrypC_Lshr_LC(L,Mask);
     CrypC_Lshr_LC(L,Temp);
     }
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DIV_LC*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/
