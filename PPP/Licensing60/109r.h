//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 109
// Application Name           : Kaspersky Anti-Virus for Windows Workstations
// Application Version        : 
// Version type               : Release
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"

static _component_info_t COMPONENTS_LIST[] =
{
	{  16,  30 },		// Kaspersky Anti-Virus Monitor Win 95/98 (Full functional)
	{  17,  30 },		// Kaspersky Anti-Virus Monitor Win NT Workstation (Full functional)
	{  19,  30 },		// Kaspersky Anti-Virus Scanner Dos 32 (Full functional)
	{  21,  30 },		// Kaspersky Anti-Virus Scanner Win 95/98 (Full functional)
	{  22,  30 },		// Kaspersky Anti-Virus Scanner Win NT Workstation (Full functional)
	{  23,  30 },		// Kaspersky Anti-Virus Control Center (Full functional)
	{  32,  30 },		// Kaspersky Anti-Virus Updater (Full functional)
	{  39,  30 },		// Kaspersky Anti-Virus NCC Server (Full functional)
	{  40,  30 },		// Kaspersky Anti-Virus NCC Console (Full functional)
	{  42,  30 },		// Kaspersky Anti-Virus Office Monitor (Full functional)
	{  43,  30 },		// Kaspersky Anti-Virus Mail Checker (Full functional)
	{  51,  30 },		// Kaspersky Anti-Virus Script Checker (Full functional)
	{  52,  30 } 		// Kaspersky Anti-Virus Rescue Disk (Full functional)
};

static _product_info_t PRODUCTS_LIST[] =
{
	{
		9, 		// 0 
		"Kaspersky Anti-Virus Business Optimal", 
		"", 
		0 
	},
	{
		11, 		// KL4521 
		"Kaspersky Anti-Virus Corporate Suite", 
		"", 
		0 
	},
	{
		26, 		// KL4111 
		"Kaspersky Anti-Virus Corporate Suite for Workstations", 
		"", 
		0 
	},
	{
		124, 		// KL3101 
		"Kaspersky Anti-Virus for Windows Workstation", 
		"", 
		0 
	},
	{
		203, 		// KL3111 
		"Antivirus BO workstation", 
		"", 
		0 
	},
	{
		208, 		// KL3511 
		"Antivirus BO workstation/file servers", 
		"", 
		0 
	},
	{
		209, 		// KL3512 
		"Antivirus BO workstation/file servers/mail servers", 
		"", 
		0 
	},
	{
		212, 		// KL3501 
		"Antivirus BO workstation/file servers for Windows MB", 
		"", 
		0 
	},
	{
		213, 		// KL3502 
		"Antivirus BO workstation/file/mail servers servers for Windows MB", 
		"", 
		0 
	},
	{
		217, 		// KL4511 
		"Antivirus Corporate workstation/file servers", 
		"", 
		0 
	},
	{
		218, 		// KL4512 
		"Antivirus Corporate workstation/file servers/mail servers", 
		"", 
		0 
	},
	{
		220, 		// KL2101 
		"Kaspersky Anti-Virus for Windows Workstation SB", 
		"", 
		0 
	},
	{
		223, 		// KL2501 
		"Antivirus BO workstation/file servers for Windows SB", 
		"", 
		0 
	},
	{
		224, 		// KL2502 
		"Antivirus BO workstation/file/mail servers servers for Windows SB", 
		"", 
		0 
	},
	{
		478, 		// KL3513 
		"Kaspersky Anti-Virus BO Suite 3", 
		"", 
		0 
	},
	{
		656, 		// KL2511 
		"Antivirus BO workstation/file servers Suite SB", 
		"", 
		0 
	},
	{
		657, 		// KL2512 
		"Antivirus BO workstation/file servers/mailserver Suite SB", 
		"", 
		0 
	},
	{
		658, 		// KL2111 
		"Kaspersky Anti-Virus SB Worstation Suite", 
		"", 
		0 
	},
	{
		661, 		// KL2513 
		"Kaspersky Anti-Virus BO for MS Small Business Server", 
		"", 
		0 
	},
	{
		807, 		// KL4851 
		"Kaspersky WorkSpace Security", 
		"", 
		0 
	},
	{
		808, 		// KL4853 
		"Kaspersky BusinessSpace Security", 
		"", 
		0 
	},
	{
		809, 		// KL4857 
		"Kaspersky EnterpriseSpace Security", 
		"", 
		0 
	},
	{
		810, 		// KL4859 
		"Kaspersky TotalSpace Security", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		77, 
		"Kaspersky Anti-Virus Total Suite"
	},
	{
		1012, 
		"Kaspersky Anti-Virus Suite for Workstations"
	},
	{
		1105, 
		"Kaspersky Anti-Virus Suite for WS and FS"
	},
	{
		1107, 
		"Kaspersky Anti-Virus 5 for Windows Workstations"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2007,  2,  7 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	109,                                              // Application Id
	"Kaspersky Anti-Virus for Windows Workstations",  // Application Name
	"",                                               // Application version
	1,                                                // Release
	{ 13, COMPONENTS_LIST },                          // Components list
	{  0, 0 },                                        // Applications list
	{ 23, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  4, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};

#endif


