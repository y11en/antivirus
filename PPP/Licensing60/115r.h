//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 115
// Application Name           : Kaspersky Anti-Virus for Windows File Servers
// Application Version        : 4
// Version type               : Release
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"

static _component_info_t COMPONENTS_LIST[] =
{
	{  17,  30 },		// Kaspersky Anti-Virus Monitor Win NT Workstation (Full functional)
	{  22,  30 },		// Kaspersky Anti-Virus Scanner Win NT Workstation (Full functional)
	{  23,  30 },		// Kaspersky Anti-Virus Control Center (Full functional)
	{  32,  30 },		// Kaspersky Anti-Virus Updater (Full functional)
	{  34,  30 },		// Kaspersky Anti-Virus for NT Server (Full functional)
	{  39,  30 },		// Kaspersky Anti-Virus NCC Server (Full functional)
	{  40,  30 },		// Kaspersky Anti-Virus NCC Console (Full functional)
	{  51,  30 } 		// Kaspersky Anti-Virus Script Checker (Full functional)
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
		27, 		// KL4211 
		"Kaspersky Anti-Virus Corporate Suite for Servers", 
		"", 
		0 
	},
	{
		49, 		// KL3201 
		"Kaspersky Anti-Virus for NT Server", 
		"", 
		0 
	},
	{
		204, 		// KL3211 
		"Antivirus BO file servers", 
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
		221, 		// KL2201 
		"Kaspersky Anti-Virus for NT Server SB", 
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
		476, 		// KL3522 
		"Kaspersky Anti-Virus BO KCP 2", 
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
		659, 		// KL2211 
		"Kaspersky Anti-Virus SB File Server Suite", 
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
	},
	{
		820, 		// KL4213 
		"Kaspersky Enterprise Anti-Virus for File Server", 
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
		1013, 
		"Kaspersky Anti-Virus Suite for Servers"
	},
	{
		1105, 
		"Kaspersky Anti-Virus Suite for WS and FS"
	},
	{
		1106, 
		"Kaspersky Anti-Virus 5 for Windows FileServers"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2007,  2,  7 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	115,                                              // Application Id
	"Kaspersky Anti-Virus for Windows File Servers",  // Application Name
	"4",                                              // Application version
	1,                                                // Release
	{  8, COMPONENTS_LIST },                          // Components list
	{  0, 0 },                                        // Applications list
	{ 24, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  4, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};

#endif