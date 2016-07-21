#ifndef __signchk_h__
#define __signchk_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h> 
#include <sign/sign.h>

int _sign_check_file(TCHAR* file);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __signchk_h__
 