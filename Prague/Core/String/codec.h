#include <Prague/prague.h>

#if ( defined ( __cplusplus ) )
    #define EXT_C extern "C" 
#else
    #define EXT_C
#endif

typedef const tCHAR * LPCSTR;
typedef tCHAR * LPSTR;

typedef const tWCHAR * LPWCSTR;
//typedef tWCHAR * LPWSTR; confilicted with LPWSTR from commonfiles/kldtdefs.h in the unix-version

#if ( ! defined ( MAKELONG ) )
    #define MAKEWORD(a, b)      ((tWORD)(((tBYTE)(a)) | ((tWORD)((tBYTE)(b))) << 8))
    #define MAKELONG(a, b)      ((tLONG)(((tWORD)(a)) | ((tDWORD)((tWORD)(b))) << 16))
    #define LOWORD(l)           ((tWORD)(l))
    #define HIWORD(l)           ((tWORD)(((tDWORD)(l) >> 16) & 0xFFFF))
    #define LOBYTE(w)           ((tBYTE)(w))
    #define HIBYTE(w)           ((tBYTE)(((tWORD)(w) >> 8) & 0xFF))
#endif

EXT_C tERROR pr_call PrStringCodec  ( const tBYTE * pInBuf, tUINT InBufLen, tCODEPAGE InCoding, tBYTE * pOutBuf, tUINT OutBufLen, tCODEPAGE OutCoding, tUINT * pReqOutBufLen );
EXT_C tERROR pr_call PrStringChCase ( const tBYTE * pInBuf, tUINT InBufLen, tCODEPAGE InCoding, tBYTE * pOutBuf, tSTR_CASE case_code );
