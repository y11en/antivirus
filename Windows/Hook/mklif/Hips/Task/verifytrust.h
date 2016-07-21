#ifndef _VERIFY_TRUST_C_
#define _VERIFY_TRUST_C_

#ifdef __cplusplus
extern "C" {
#endif
	
#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <softpub.h>
#include <Mscat.h>


typedef void* HVERIFY;

HVERIFY WINAPI VerifyTrustInit(BOOL bCacheContext);
void    WINAPI VerifyTrustDone(HVERIFY hVerify);

BOOL    WINAPI VTIsMicrosoftSigned(HVERIFY hVerify, const WCHAR* wcsFName, wchar_t ** l_ppOutCatFileName);
void    WINAPI VTDropCachedContext(HVERIFY hVerify);

#ifdef __cplusplus
} // extern "C"
#endif


#endif // _VERIFY_TRUST_C_