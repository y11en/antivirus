#include "../../CommonFiles/property/Property.h"
#include "../../CommonFiles/AVPComID.h"
#include "../../CommonFiles/loadkeys/AVPKeyID.h"

#define FN_DISINFECT   0x00000001
#define FN_UNPACK      0x00000002
#define FN_EXTRACT     0x00000004
#define FN_MAIL        0x00000008
#define FN_CA          0x00000010
#define FN_UPDATES     0x00000020

#define FN_OPTIONS     0x00010000
#define FN_NETWORK     0x00020000

#define FN_FULL        0xFFFFFFFF
#define FN_MINIMAL     0x0000FFFF

#define  ID_SERIALSTR  7
#define  ID_FILENAME   8
#define PID_SERIALSTR    MAKE_AVP_PID(ID_SERIALSTR,AVP_APID_FIRST,avpt_str,0)
#define PID_FILENAME     MAKE_AVP_PID(ID_FILENAME,AVP_APID_FIRST,avpt_str,0)

DWORD LoadKeys(const char* path, CPtrArray* array, DWORD appID);
// path - fullpathname to folder with \! or any file in this folder
// array - array for keys DATATREE handles. can be NULL!
// appID - see AVPCommonIDs.h
