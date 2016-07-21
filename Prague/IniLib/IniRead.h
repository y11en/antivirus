#ifndef _INIREAD_H_
#define _INIREAD_H_

#if defined( __cplusplus )
extern "C" {
#endif

#include "IniLib.h"

typedef struct tagIniMm
{
	tDWORD        dummy;
} sIniMm;

sIniMm *     IniReadMmCreate();
tERROR       IniReadMmLoad(sIniSection** head, sIniMm * mm, hIO io, tDWORD nFlags DEF_VAL(0), tIniLoadFilter fltfunc DEF_VAL(NULL), tPTR fltctx DEF_VAL(NULL));
void         IniReadMmClear(sIniMm * mm);
void         IniReadMmDestroy(sIniMm * mm);

#if defined( __cplusplus )
} // extern "C"
#endif

#endif//_INIREAD_H_
