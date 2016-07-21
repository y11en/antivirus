//=========================================================================
// Lat1->Unicode and Unicode->Lat1 recode table
//=========================================================================
#include "StdPch.h"
#include <_include/Cp2Uni.h>

//-------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------------------

extern const char tabUcs2Lat1_DefPg[256];
extern const char tabUcs2Lat1_00Pg[256];
extern const char tabUcs2Lat1_01Pg[256];
extern const char tabUcs2Lat1_02Pg[256];
extern const char tabUcs2Lat1_20Pg[256];
extern const char tabUcs2Lat1_21Pg[256];
//-------------------------------------------------------------------------

extern const char tabUcs2Win_04Pg[256];

//-------------------------------------------------------------------------


const LPCSTR tabUsc2Lat1_cyr[256] =
    {
        tabUcs2Lat1_00Pg, tabUcs2Lat1_01Pg, tabUcs2Lat1_02Pg, tabUcs2Lat1_DefPg, tabUcs2Win_04Pg /*tabUcs2Lat1_DefPg*/, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_20Pg, tabUcs2Lat1_21Pg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, 
        tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg, tabUcs2Lat1_DefPg
    };



//-------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//=========================================================================
