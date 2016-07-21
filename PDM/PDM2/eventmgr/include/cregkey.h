#ifndef _CREGKEY_H__
#define _CREGKEY_H__

#include "types.h"

// Reserved Key Handles.
#define crHKEY_CLASSES_ROOT           (( tRegistryKey ) 0x80000000 )
#define crHKEY_CURRENT_USER           (( tRegistryKey ) 0x80000001 )
#define crHKEY_LOCAL_MACHINE          (( tRegistryKey ) 0x80000002 )
#define crHKEY_USERS                  (( tRegistryKey ) 0x80000003 )
#define crHKEY_PERFORMANCE_DATA       (( tRegistryKey ) 0x80000004 )
#define crHKEY_CURRENT_CONFIG         (( tRegistryKey ) 0x80000005 )
#define crHKEY_DYN_DATA               (( tRegistryKey ) 0x80000006 )
// Predefined Value Types.
#define crREG_NONE                    ( 0 )   // No value type
#define crREG_SZ                      ( 1 )   // Unicode nul terminated string
#define crREG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
#define crREG_BINARY                  ( 3 )   // Free form binary
#define crREG_DWORD                   ( 4 )   // 32-bit number
#define crREG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
#define crREG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
#define crREG_LINK                    ( 6 )   // Symbolic Link (unicode)
#define crREG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
#define crREG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define crREG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
#define crREG_RESOURCE_REQUIREMENTS_LIST ( 10 )	

const tcstring sRootKeyNames[] = {
	_T("HKEY_CLASSES_ROOT"),
	_T("HKEY_CURRENT_USER"),
	_T("HKEY_LOCAL_MACHINE"),
	_T("HKEY_USERS"),
	_T("HKEY_PERFORMANCE_DATA"),
	_T("HKEY_CURRENT_CONFIG"),
	_T("HKEY_DYN_DATA"),
};


#define IS_ROOT_KEY(key) (!!((key)&0x80000000))
#define GET_ROOT_KEY_INDEX(key) ((key)&(~(tRegistryKey)0x80000000))
#define GET_ROOT_KEY_NAME(key) (GET_ROOT_KEY_INDEX(key) < countof(sRootKeyNames) ? sRootKeyNames[GET_ROOT_KEY_INDEX(key)] : NULL)

class cRegKey
{
public:
	cRegKey(tRegistryKey root = 0, tcstring keyname = NULL);
	~cRegKey();

	cRegKey& operator =(const cRegKey& from);
	//cRegKey& operator =(tcstring keyname);

	bool     assign(tRegistryKey root, tcstring keyname);
	void     clear();

public:
	tcstring  m_fullname;      // unparsed key name in uppercase: \REGISTRY\USERS\S-1-5-21-1430328663-2098613005-1233803906-1116\SOFTWARE\MICROSOFT\WINDOWS\CURRENTVERSION\RUN
	tRegistryKey  m_root;      // parsed root: HKEY_USERS
	tcstring  m_orig_name;     // parsed key name with original case: S-1-5-21-1430328663-2098613005-1233803906-1116\Software\Microsoft\Windows\CurrentVersion\Run
	tcstring  m_open_name;     // parsed key name ready to open in uppercase: S-1-5-21-1430328663-2098613005-1233803906-1116\SOFTWARE\MICROSOFT\WINDOWS\CURRENTVERSION\RUN
	tcstring  m_name;          // parsed key name excluding user SID in uppercase: SOFTWARE\MICROSOFT\WINDOWS\CURRENTVERSION\RUN
	tcstring  m_controlset_subkey; // SERVICES\... <= HKLM\CURRENTCONTROLSET\SERVICES\...
};


#endif // _CREGKEY_H__
