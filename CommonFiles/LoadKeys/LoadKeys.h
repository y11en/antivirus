////////////////////////////////////////////////////////////////////
//
//  LoadKeys.h
//  AVP key loading and preprocessing (old and new formats)
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __LOADKEYS_H
#define __LOADKEYS_H

#include <property/property.h>
#include <AvpComID.h>
#include <loadkeys/avpkeyid.h>

#include "CheckUse.h"

#define  ID_SERIALSTR  7
#define  ID_FILENAME   8
#define PID_SERIALSTR    MAKE_AVP_PID(ID_SERIALSTR,AVP_APID_FIRST,avpt_str,0)
#define PID_FILENAME     MAKE_AVP_PID(ID_FILENAME,AVP_APID_FIRST,avpt_str,0)

BOOL LoadKeys(DWORD appID, const char* path, DWORD* level, BOOL* valid, CPtrArray* array);

// Return TRUE if at least one key founded for this appID
// appID - Application ID (see AVPCommonIDs.h)
// path - fullpathname to folder with \! or any file in this folder
// level - maximal level of functionality
// valid - at least one key is not expired
// array - array for keys DATATREE handles. can be NULL!

#endif//__LOADKEYS_H
