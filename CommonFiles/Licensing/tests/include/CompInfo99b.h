//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Component Id               : 99
// Component Name             : Updater - KeepUp2Date
// Component Version          : 
// Version type               : Beta
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __COMPONENT_INFO_H__
#define __COMPONENT_INFO_H__

#include "appinfo_struct.h"


static _app_component_info_t APP_COMPONENTS_LIST[] =
{
	{ 1101,  30 },		// Kaspersky Anti-Virus 5 Personal, Ver="5",  (Full functional)
	{ 1106,  30 },		// Kaspersky Anti-Virus 5 for Windows FileServers, Ver="",  (Full functional)
	{ 1107,  30 } 		// Kaspersky Anti-Virus 5 for Windows Workstations, Ver="",  (Full functional)
};

static _product_info_t PRODUCTS_LIST[] =
{
	{
		131, 
		"Kaspersky Anti-Virus 5 Personal", 
		"", 
		0 
	},
	{
		135, 
		"Kaspersky Anti-Virus 5 Business Optimal", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		105, 
		"Kaspersky Anti-Virus Personal"
	}
};

struct _app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003,  7, 17 },                                 // Date create
	aitApplicationInfo,                               // application info type (app or component)
	99,                                               // Application Id
	"Kaspersky Anti-Virus 5 for Windows Workstations",// Application Name
	"",                                               // Application version
	0,                                                // 0 - beta, not 0 - release (app only)
	{  0, 0 },                                        // Components list (app only)
	{  3, APP_COMPONENTS_LIST },                      // Applications list (component only)
	{  2, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


