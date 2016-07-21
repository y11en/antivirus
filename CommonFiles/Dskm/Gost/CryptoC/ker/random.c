#if defined (_WIN32)
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "CrC_User.h"

//char    Pass_=0;
//DWORD   X0_, Y0_, A0_=18000, B0_=30903;
//DWORD   X1_, Y1_, A1_=23163, B1_=19215;
//DWORD   X2_, Y2_, A2_=28959, B2_=24948;
//DWORD   X3_, Y3_, A3_=19074, B3_=21723;
//DWORD   OLZ_, F_;

DWORD   A0_=18000, B0_=30903;
DWORD   A1_=23163, B1_=19215;
DWORD   A2_=28959, B2_=24948;
DWORD   A3_=19074, B3_=21723;

/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄DEFINES*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀FORCERANDOM*/
void  CrypC_ForceRandom_(struct RND_State *State,
                         DWORD _X0, DWORD _Y0 ,
                         DWORD _X1, DWORD _Y1 ,
                         DWORD _X2, DWORD _Y2 ,
                         DWORD _X3, DWORD _Y3 ,
                         char  _Pass          )
/*  Установка заданного начального заполнения длины 256 бит */
/*                датчика случайных чисел                   */
/*           и сохранение текущего заполнения               */
/* Входные параметры:                                       */
/* - Указатель *State на контекст ГСЧ.                      */
/* - Необходимые состояния регистров Xi_, Yi_, i=0,...,3.   */
/* - Необходимое состояние индикатора Pass_.                */
/* Выход:                                                   */
/* - Текущие состояния регистров Xi_, Yi_, i=0,...,3 и инди-*/
/*   катора Pass_ сохраняются в контексте и заполняются     */
/*   содержимым соответствующих параметров процедуры.       */
{
State->x0_   = State->X0_  ;       /* Сохранить состояние.  */
State->x1_   = State->X1_  ;
State->x2_   = State->X2_  ;
State->x3_   = State->X3_  ;
State->y0_   = State->Y0_  ;
State->y1_   = State->Y1_  ;
State->y2_   = State->Y2_  ;
State->y3_   = State->Y3_  ;
State->pass_ = State->Pass_;

State->X0_   = _X0  ;              /* Установить состояние. */
State->X1_   = _X1  ;
State->X2_   = _X2  ;
State->X3_   = _X3  ;
State->Y0_   = _Y0  ;
State->Y1_   = _Y1  ;
State->Y2_   = _Y2  ;
State->Y3_   = _Y3  ;
State->Pass_ = _Pass;
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄RESTRRANDOM*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀RESTRRANDOM*/
void  CrypC_RestrRandom_(struct RND_State *State)
/*   Восстановление сохраненного заполнения длины 256 бит   */
/*                датчика случайных чисел                   */
/* Входные параметры:                                       */
/* - Указатель *State на контекст ГСЧ.                      */
/* Выход:                                                   */
/* - Восстанавливаютя ранее сохраненные в контексте состоя- */
/*   ния регистров Xi_, Yi_, i=0,...,3 и индикатора Pass_.  */
{
State->X0_   = State->x0_  ;     /* Восстановить состояние. */
State->X1_   = State->x1_  ;
State->X2_   = State->x2_  ;
State->X3_   = State->x3_  ;
State->Y0_   = State->y0_  ;
State->Y1_   = State->y1_  ;
State->Y2_   = State->y2_  ;
State->Y3_   = State->y3_  ;
State->Pass_ = State->pass_;
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄FORCERANDOM*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀STEP*/
void CrypC_Step_(struct RND_State *State)
{                                        /* Шаг рекуррентного преобразования. */
State->X0_     = (A0_*(State->X0_ & 65535)) + (State->X0_ >> 16);
State->Y0_     = (B0_*(State->Y0_ & 65535)) + (State->Y0_ >> 16);
State->X1_     = (A1_*(State->X1_ & 65535)) + (State->X1_ >> 16);
State->Y1_     = (B1_*(State->Y1_ & 65535)) + (State->Y1_ >> 16);
State->X2_     = (A2_*(State->X2_ & 65535)) + (State->X2_ >> 16);
State->Y2_     = (B2_*(State->Y2_ & 65535)) + (State->Y2_ >> 16);
State->X3_     = (A3_*(State->X3_ & 65535)) + (State->X3_ >> 16);
State->Y3_     = (B3_*(State->Y3_ & 65535)) + (State->Y3_ >> 16);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄STEP*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀TAKTOLZ*/
void CrypC_TaktOLZ_(struct RND_State *State)
{                                        /* Такт   работы линии задержки. */
State->OLZ_ = (State->OLZ_ >> 1) ^ ((
                                     State->OLZ_       ^   /* Бит 0: x**0 */
                                    (State->OLZ_ >> 1) ^   /* Бит 1: x**1 */
                                    (State->OLZ_ >> 2) ^   /* Бит 2: x**2 */
                                    (State->OLZ_ >> 3) ^   /* Бит 3: x**3 */
                                    (State->OLZ_ >> 5) ^   /* Бит 5: x**5 */
                                    (State->OLZ_ >> 7)     /* Бит 7: x**7 */
                                    ) << 31
                                   );
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄TAKTOLZ*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀INITRANDOM*/
void  CrypC_InitRandom_( struct RND_State *State )
/* Генерация случайного начального заполнения длины 256 бит */
/*                датчика случайных чисел                   */
{
int           i, n;
DWORD        TMP;
DWORD        X_[2], w;
DWORD        M64[2] = {0x7f3f1f0f,0x0703016a};
DWORD        P64[2] = {0xffffffff,0xffffffc5};

State->F_  = (rdtick() >> 2) & 15;                         /* Начальное состояние F.*/
State->X0_ = (rdtick() >> 2) ^ (rdtick() << 16);           /* Первичное заполнение. */
State->X1_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->X2_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->X3_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->Y0_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->Y1_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->Y2_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->Y3_ = (rdtick() >> 2) ^ (rdtick() << 16);
State->OLZ_= (DWORD)time(0);

for(i=0;i<64;i++)                         /* 64 такта ОЛЗ.               */
   CrypC_TaktOLZ_(State);
for(n=0;n<64;n++)                         /* Размазывание регистра 0.    */
   {
   for(i=0;i<16;i++)                                CrypC_Step_(State);
   while(((State->Y0_+State->Y1_)&15) != State->F_) CrypC_Step_(State);

   TMP = (DWORD)(((BYTE *)&(State->OLZ_))[3]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[2]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[1]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[0]) ;
   State->F_  = ((rdtick() >> 3) ^        /* Новое значение F: случайное */
                 (TMP      >> 4) ^        /* плюс сумма полубайтов ОЛЗ.  */
                  TMP          ) & 15;  
   for(i=0;i<64;i++)                      /* 64 такта ОЛЗ.               */
      CrypC_TaktOLZ_(State);

   if(rdtick()&4)                         /* Если случайный бит == 1:    */
     {
     X_[0] = State->X0_;                  /* Временный вектор из 4 слов. */
     X_[1] = State->Y0_;
     X_[0] ^= M64[0]; X_[1] ^= M64[1];    /* Прибавить маску.            */
     w = CrypC_Lshl_LC(2,M64);            /* Циклический сдвиг маски...  */
     M64[1] ^= w;                         /*                        ...  */
     CrypC_Ladd_PC(2, X_, LO_, P64);      /* X_ += 0 (mod 2**64 - 59).   */
     State->X0_ = X_[0];                  /* Новое содержимое регистра.  */
     State->Y0_ = X_[1];
     }
   }
for(n=0;n<64;n++)                         /* Размазывание регистра 1.    */
   {
   for(i=0;i<16;i++)                                CrypC_Step_(State);
   while(((State->Y0_+State->Y1_)&15) != State->F_) CrypC_Step_(State);
   
   TMP = (DWORD)(((BYTE *)&(State->OLZ_))[3]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[2]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[1]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[0]) ;
   State->F_  = ((rdtick() >> 3) ^       /* Новое значение F: случайное */
                 (TMP      >> 4) ^        /* плюс сумма полубайтов ОЛЗ.  */
                  TMP          ) & 15;  
   for(i=0;i<64;i++)                      /* 64 такта ОЛЗ.               */
      CrypC_TaktOLZ_(State);

   if(rdtick()&4)                         /* Если случайный бит == 1:    */
     {
     X_[0] = State->X1_;                  /* Временный вектор из 4 слов. */
     X_[1] = State->Y1_;
     X_[0] ^= M64[0]; X_[1] ^= M64[1];    /* Прибавить маску.            */
     w = CrypC_Lshl_LC(2,M64);            /* Циклический сдвиг маски...  */
     M64[1] ^= w;                         /*                        ...  */
     CrypC_Ladd_PC(2, X_, LO_, P64);      /* X_ += 0 (mod 2**64 - 59).   */
     State->X1_ = X_[0];                  /* Новое содержимое регистра.  */
     State->Y1_ = X_[1];
     }
   }
for(n=0;n<64;n++)                         /* Размазывание регистра 2.    */
   {
   for(i=0;i<16;i++)                                CrypC_Step_(State);
   while(((State->Y0_+State->Y1_)&15) != State->F_) CrypC_Step_(State);
   
   TMP = (DWORD)(((BYTE *)&(State->OLZ_))[3]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[2]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[1]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[0]) ;
   State->F_  = ((rdtick() >> 3) ^        /* Новое значение F: случайное */
                 (TMP      >> 4) ^        /* плюс сумма полубайтов ОЛЗ.  */
                  TMP          ) & 15;  
   for(i=0;i<64;i++)                      /* 64 такта ОЛЗ.               */
      CrypC_TaktOLZ_(State);

   if(rdtick()&4)                         /* Если случайный бит == 1:    */
     {
     X_[0] = State->X2_;                  /* Временный вектор из 4 слов. */
     X_[1] = State->Y2_;
     X_[0] ^= M64[0]; X_[1] ^= M64[1];    /* Прибавить маску.            */
     w = CrypC_Lshl_LC(2,M64);            /* Циклический сдвиг маски...  */
     M64[1] ^= w;                         /*                        ...  */
     CrypC_Ladd_PC(2, X_, LO_, P64);      /* X_ += 0 (mod 2**64 - 59).   */
     State->X2_ = X_[0];                  /* Новое содержимое регистра.  */
     State->Y2_ = X_[1];
     }
   }
for(n=0;n<64;n++)                         /* Размазывание регистра 3.    */
   {
   for(i=0;i<16;i++)                                CrypC_Step_(State);
   while(((State->Y0_+State->Y1_)&15) != State->F_) CrypC_Step_(State);
   
   TMP = (DWORD)(((BYTE *)&(State->OLZ_))[3]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[2]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[1]) ^
         (DWORD)(((BYTE *)&(State->OLZ_))[0]) ;
   State->F_  = ((rdtick() >> 3) ^        /* Новое значение F: случайное */
                 (TMP      >> 4) ^        /* плюс сумма полубайтов ОЛЗ.  */
                  TMP          ) & 15;  
   for(i=0;i<64;i++)                      /* 64 такта ОЛЗ.               */
      CrypC_TaktOLZ_(State);

   if(rdtick()&4)                         /* Если случайный бит == 1:    */
     {
     X_[0] = State->X3_;                  /* Временный вектор из 4 слов. */
     X_[1] = State->Y3_;
     X_[0] ^= M64[0]; X_[1] ^= M64[1];    /* Прибавить маску.            */
     w = CrypC_Lshl_LC(2,M64);            /* Циклический сдвиг маски...  */
     M64[1] ^= w;                         /*                        ...  */
     CrypC_Ladd_PC(2, X_, LO_, P64);      /* X_ += 0 (mod 2**64 - 59).   */
     State->X3_ = X_[0];                  /* Новое содержимое регистра.  */
     State->Y3_ = X_[1];
     }
   }
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄INITRANDOM*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀RANDOM*/
int  CrypC_FillByRandom_( struct RND_State *State, void *Buf, long LenInBytes )
/*     Генерация псевдослучайного вектора заданой длины     */
/* Входные параметры:                                       */
/* - Указатель *Buf на буфер для псевдослучайного вектора.  */
/* - Длина LenInBytes буфера Buf в байтах.                  */
/* Выход:                                                   */
/* - В поле *Buf помещается псевдослучайный вектор длины    */
/*   LenInBytes байт.                                       */
{
DWORD        *Ptr, Tmp;

Ptr = (DWORD *)Buf;
if(!State->Pass_)                           /* Начальное заполнение Xi,Yi. */
  {
  CrypC_InitRandom_( State );
  State->Pass_ = 1;
  }
while(LenInBytes>=4)
   {
   CrypC_Step_(State);
   *Ptr    = (DWORD)((State->X0_<<16)^(State->Y0_&65535));
   *Ptr   ^= (DWORD)((State->X1_<<16)^(State->Y1_&65535));
   *Ptr   ^= (DWORD)((State->X2_<<16)^(State->Y2_&65535));
   *Ptr++ ^= (DWORD)((State->X3_<<16)^(State->Y3_&65535));
   LenInBytes -= 4;
   }
if( LenInBytes )
   {
   CrypC_Step_(State);
   Tmp     = (DWORD)((State->X0_<<16)^(State->Y0_&65535));
   Tmp    ^= (DWORD)((State->X1_<<16)^(State->Y1_&65535));
   Tmp    ^= (DWORD)((State->X2_<<16)^(State->Y2_&65535));
   Tmp    ^= (DWORD)((State->X3_<<16)^(State->Y3_&65535));
   memcpy( Ptr, &Tmp, (WORD)LenInBytes );
   }
return(0);
}
/*▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄RANDOM*/
/*▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀EOF*/

#endif //_WIN32