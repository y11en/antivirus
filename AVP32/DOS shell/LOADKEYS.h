#include "../../Common.Fil/property.h"
#include "../../Common.Fil/AvpComID.h"
#include "../../Common.Fil/AvpKeyIDs.h"
//#include "..\CommonFiles\property.h"
//#include "..\CommonFiles\AvpComID.h"
//#include "..\CommonFiles\AvpKeyID.h"

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

//DWORD LoadKeys(const char* path, CPtrArray* array, DWORD appID);
DWORD LoadKeys(DWORD appID, const char* path, DWORD* level, BOOL* valid, CPtrArray* array);
// Return TRUE if at least one key founded for this appID
// appID - Application ID (see AVPCommonIDs.h)
// path - fullpathname to folder with \! or any file in this folder
// level - maximal level of functionality
// valid - at least one key is not expired
// array - array for keys DATATREE handles. can be NULL!

