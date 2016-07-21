#include <Prague/prague.h>

#ifndef min
#define min(a,b) (a<b?a:b)
#endif

#ifndef max
#define max(a,b) (a>b?a:b)
#endif

#define cMAX_EXT_SIZE       64
#define cFORMAT_DATA_SIZE   0x200

tERROR pr_call AVLIB_IsExecutableExtensionBuff(tBYTE* pExt, tDWORD dwSize, tDWORD* pdwResult);
tERROR pr_call AVLIB_IsExecutableFormatBuff(tBYTE* pInData, tDWORD dwSize, tDWORD* pdwResult);
tERROR pr_call AVLIB_IsExecutableFormatBuffLite(tBYTE* pInData, tDWORD dwSize, tDWORD* pdwResult);
