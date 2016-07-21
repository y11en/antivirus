////////////////////////////////////////////////////////////////////
//
//  CType2ID.cpp
//  Defines the CTYPE to ID convertion function 
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <AVPRegID.h>
#include <AVPComID.h>
#include <Stuff\CType2ID.h>

// ---
struct TableItem {
	DWORD m_dwCType;
	DWORD m_dwW95ID;
	DWORD m_dwWNTID;
};

// ---
static TableItem g_Table[] = {
	{ AVP_CTYPE_SCANNER,							AVP_APID_SCANNER_WIN_95,		AVP_APID_SCANNER_WIN_NTWS },
	{ AVP_CTYPE_MONITOR,							AVP_APID_MONITOR_WIN_95,		AVP_APID_MONITOR_WIN_NTWS },
	{ AVP_CTYPE_UPDATES,							AVP_APID_UPDATOR,						AVP_APID_UPDATOR },
	{ AVP_CTYPE_INSPECTOR,						AVP_APID_INSPECTOR,					AVP_APID_INSPECTOR },
	{ AVP_CTYPE_SERVER,								AVP_APID_NCC_SERVER,				AVP_APID_NCC_SERVER },
	{ AVP_CTYPE_NCC,									AVP_APID_NCC_CONSOLE,				AVP_APID_NCC_CONSOLE },
	{ AVP_CTYPE_FW,										AVP_APID_FIREWALL,					AVP_APID_FIREWALL },
	{ AVP_CTYPE_CC,										AVP_APID_CONTROLCENTER,			AVP_APID_CONTROLCENTER },
	{ AVP_CTYPE_EXCHANGE,							AVP_APID_EXCHANGE,					AVP_APID_EXCHANGE },
	{ AVP_CTYPE_OFFICE_GUARD,					AVP_APID_OFFICEGUARD,				AVP_APID_OFFICEGUARD },
	{ AVP_CTYPE_OFFICE_MONITOR,				AVP_APID_OFFICEMONITOR,			AVP_APID_OFFICEMONITOR },
	{ AVP_CTYPE_OFFICE_MAIL_CHECKER,	AVP_APID_OFFICEMAILCHECKER, AVP_APID_OFFICEMAILCHECKER },
	{ AVP_CTYPE_SCRIPT_CHECKER,				AVP_APID_SCRIPT_CHECKER,		AVP_APID_SCRIPT_CHECKER },
	{ AVP_CTYPE_RESCUE_DISK,					AVP_APID_RESCUE_DISK, 			AVP_APID_RESCUE_DISK },
	{ AVP_CTYPE_REPORT_VIEWER,				AVP_APID_REPORT_VIEWER, 		AVP_APID_REPORT_VIEWER },
	{ AVP_CTYPE_DT,										AVP_APID_DEPLOYMENT_TOOL, 	AVP_APID_DEPLOYMENT_TOOL },
	{ AVP_CTYPE_SCANNER_LITE,					AVP_APID_SCANNER_LITE,			AVP_APID_SCANNER_LITE },
	{ AVP_CTYPE_EXCHANGE2000_MMC_SNAPIN, AVP_APID_EXCHANGE2000_MMC_SNAPIN, AVP_APID_EXCHANGE2000_MMC_SNAPIN },
	{ AVP_CTYPE_CONTENT_FILTER,       AVP_APID_CONTENT_FILTER,    AVP_APID_CONTENT_FILTER },
};


static int g_nTableCount = sizeof(g_Table) / sizeof(g_Table[0]);

// Converts AVP Component CTYPE (see AVPRegID.h) to AVP Application ID (see AVPComID.h)
// dwCType			- value of standard AVP Component CType
// Return value - standard AVP Application ID
DWORD AvpCTypeToID( DWORD dwCType ) {
	for ( int i=0; i<g_nTableCount; i++ ) {
		if ( g_Table[i].m_dwCType == dwCType ) {
			OSVERSIONINFO OSInfo;
			OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if ( ::GetVersionEx(&OSInfo) )  {
				switch ( OSInfo.dwPlatformId) {
					case VER_PLATFORM_WIN32_WINDOWS :	// Windows 9x
						return g_Table[i].m_dwW95ID;
					case VER_PLATFORM_WIN32_NT :			// Windows NT
						return g_Table[i].m_dwWNTID;
				}
			}
			return g_Table[i].m_dwW95ID;
		}
	}
	return 0;
}

// Converts AVP Application ID (see AVPComID.h) tp AVP Component CTYPE (see AVPRegID.h) 
// dwAppID			- value of AVP Application ID
// Return value - standard standard AVP Component CType
DWORD AvpIDToCType( DWORD dwAppID ) {
	for ( int i=0; i<g_nTableCount; i++ ) {
		OSVERSIONINFO OSInfo;
		OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if ( ::GetVersionEx(&OSInfo) )  {
			switch ( OSInfo.dwPlatformId) {
				case VER_PLATFORM_WIN32_WINDOWS :	// Windows 9x
					if ( g_Table[i].m_dwW95ID == dwAppID ) 
						return g_Table[i].m_dwCType;
				case VER_PLATFORM_WIN32_NT :			// Windows NT
					if ( g_Table[i].m_dwWNTID == dwAppID )
						return g_Table[i].m_dwCType;
			}
		}
	}
	return 0;
}

