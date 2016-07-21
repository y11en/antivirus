////////////////////////////////////////////////////////////////////
//
//  CheckUse.h
//  AVP key loading and preprocessing (old and new formats)
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __CHECKUSE_H
#define __CHECKUSE_H

#include "AVP_data.h"
#include "property/property.h"
#include "windows.h"

BOOL CheckSelfExpir(SYSTEMTIME& stExpir);

BOOL CheckKeyUseTime(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& stExpir, AVP_dword* PLPIfTrial);
BOOL CheckKeyUseTimeEx(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& stExpir, AVP_dword* PLPIfTrial, AVP_bool bRegister);

#endif //__CHECKUSE_H
