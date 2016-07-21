//===========================================================================
#ifndef __Translit_Cp2Uni_H__
#define __Translit_Cp2Uni_H__
//===========================================================================
#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------------------
#include <_include/wintypes.h>


extern const wchar_16 tabLat12Usc[256];
extern const wchar_16 tabWin2Usc[256];
extern const wchar_16 tabKoi2Usc[256];
extern const wchar_16 tabIso2Usc[256];
extern const wchar_16 tabDos2Usc[256];
extern const wchar_16 tabMac2Usc[256];

extern const LPCSTR tabUsc2Win[256];
extern const LPCSTR tabUsc2Koi[256];
extern const LPCSTR tabUsc2Iso[256];
extern const LPCSTR tabUsc2Dos[256];
extern const LPCSTR tabUsc2Mac[256];
extern const LPCSTR tabUsc2Lat1[256];
extern const LPCSTR tabUsc2Lat1_cyr[256];


//-------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//===========================================================================
#endif // __Translit_Cp2Uni_H__
//===========================================================================
