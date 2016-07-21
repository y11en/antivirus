#include "sigdet.h"
#include "..\include\sign_struc.h"

#ifdef _M_X64
extern "C" __int64 __cdecl _time64(__int64 *)
{
    return 0;
}
#else
extern "C" int __cdecl time(int *)
{
    return 0;
}
#endif

/*
 * The global security cookie.  This name is known to the compiler.
 * Initialize to a garbage non-zero value just in case we have a buffer overrun
 * in any code that gets run before __security_init_cookie() has a chance to
 * initialize the cookie to the final value.
 */



#define OLD_SIGN_SIZE 64

// вы€сн€ет, подписан ли файл старой подписью. 
// pSignlessSize возвращает размер файла без старой подписи ( котора€ если есть, то добавлена в конец файла )
bool
__stdcall
CheckForSignOld( char* BasePtr, unsigned long TotalSize, unsigned long* pSignlessSize )
{
    if ( pSignlessSize )
        *pSignlessSize = TotalSize;

    if ( TotalSize > OLD_SIGN_SIZE )
    {
        char* ProbableSign = BasePtr + TotalSize - OLD_SIGN_SIZE; 
        
        if ( 
            ProbableSign[0] == 0x0D && 
            ProbableSign[1] == 0x0A &&
            ProbableSign[2] == 0x3B &&
            ProbableSign[3] == 0x20 &&
            ProbableSign[4] == 0x30 &&
            ProbableSign[5] == 0x58 &&
            ProbableSign[6] == 0x4C &&
            ProbableSign[7] == 0x53
            )
        {
            if ( pSignlessSize )
                *pSignlessSize -= OLD_SIGN_SIZE;

            return true;
        }
            
    }

    return false;
}

// вы€сн€ет, подписан ли файл новой подписью. 
// pSignlessSize возвращает размер файла без новой подписи ( котора€ если есть, то добавлена в конец файла )
bool
__stdcall
CheckForSignNew( char* BasePtr, unsigned long TotalSize, unsigned long* pSignlessSize )
{
    PSIGN_FOOTER footer = (PSIGN_FOOTER) ( BasePtr + TotalSize - sizeof ( SIGN_FOOTER ) );
    
    if ( footer->Tag != SIGN_FOOTER_SIGNATURE )
        return false;

    PSIGN_HEADER header = (PSIGN_HEADER) ( BasePtr + TotalSize - footer->TotalSize );

    if ( header->Tag != SIGN_HEADER_SIGNATURE )
        return false;

    if ( pSignlessSize )
        *pSignlessSize = TotalSize - footer->TotalSize;

    return true;
}

// получает указатели на соответствующие блоки в пам€ти дл€ CryptoC подписи.
// предполагаетс€, что нова€ подпись находитс€ в конце файла и за ней ничего дальше нету.
long
__stdcall
GetCryptoCPtrs( 
               char*            BasePtr, 
               unsigned long    TotalSize, 
               
               char**         pKey1,
               unsigned long* pKey1Size,

               char**         pKey2,
               unsigned long* pKey2Size,

               char**         pReg,
               unsigned long* pRegSize,
               
               unsigned long* pFileSize)
{    
    if (TotalSize < sizeof ( SIGN_FOOTER ))
        return 0;

    PSIGN_FOOTER footer = (PSIGN_FOOTER) ( BasePtr + TotalSize - sizeof ( SIGN_FOOTER ) );
    
    if ( footer->Tag != SIGN_FOOTER_SIGNATURE )
        return 0;

    if (footer->TotalSize > TotalSize ||
        footer->TotalSize < sizeof(SIGN_HEADER) + sizeof(SIGN_FOOTER))
        return 0;

    PSIGN_HEADER header = (PSIGN_HEADER) ( BasePtr + TotalSize - footer->TotalSize );

    if ( header->Tag != SIGN_HEADER_SIGNATURE )
        return 0;

    if ( header->TotalSize != footer->TotalSize )
        return 0;

    PSIGN_STRUCT Sign = (PSIGN_STRUCT) ( header + 1 );
    unsigned long BunchSizeLeft = header->TotalSize - (sizeof(SIGN_HEADER) + sizeof(SIGN_FOOTER));

    if (Sign->Size < sizeof(SIGN_STRUCT) ||
        Sign->Size > BunchSizeLeft)
        return 0;

    // ищем подпись CryptoC
    while ( Sign->AlgID != ALG_CRYPTOC )
    {
        BunchSizeLeft -= Sign->Size;
        Sign = (PSIGN_STRUCT) ( (char*)Sign + Sign->Size );

        if (Sign->Size < sizeof(SIGN_STRUCT) ||
            Sign->Size > BunchSizeLeft)
            return 0;
    }

    unsigned long SignSizeLeft = Sign->Size - sizeof(SIGN_STRUCT);
    if (SignSizeLeft < sizeof(CRYPTOC_SIGN))
        return 0;

    PCRYPTOC_SIGN CryptoCSign = (PCRYPTOC_SIGN) ( Sign + 1 );
    SignSizeLeft -= sizeof(CRYPTOC_SIGN);

    unsigned long offs = 0;

    if (SignSizeLeft < CryptoCSign->Key1Size)
        return 0;

    *pKey1Size = CryptoCSign->Key1Size;
    *pKey1 = CryptoCSign->Buffer + offs;    
    offs += CryptoCSign->Key1Size;
    SignSizeLeft -= CryptoCSign->Key1Size;

    if (SignSizeLeft < CryptoCSign->Key2Size)
        return 0;

    *pKey2Size = CryptoCSign->Key2Size;
    *pKey2 = CryptoCSign->Buffer + offs;
    offs += CryptoCSign->Key2Size;
    SignSizeLeft -= CryptoCSign->Key2Size;

    if (SignSizeLeft < CryptoCSign->RegSize)
        return 0;

    *pRegSize = CryptoCSign->RegSize;
    *pReg = CryptoCSign->Buffer + offs;    
    offs += CryptoCSign->RegSize;
    SignSizeLeft -= CryptoCSign->Key2Size;

    *pFileSize = TotalSize - header->TotalSize;

    return 1;
}