//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1114
// Application Name           : Kaspersky Anti-Virus 5 for Novell NetWare
// Application Version        : 5
// Version type               : Release
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"



static _product_info_t PRODUCTS_LIST[] =
{
	{
		9, 
		"Kaspersky Anti-Virus Business Optimal", 
		"", 
		0 
	},
	{
		11, 
		"Kaspersky Anti-Virus Corporate Suite", 
		"", 
		0 
	},
	{
		27, 
		"Kaspersky Anti-Virus Corporate Suite for Servers", 
		"", 
		0 
	},
	{
		50, 
		"Kaspersky Anti-Virus for Novell NetWare", 
		"", 
		0 
	},
	{
		146, 
		"Kaspersky Anti-Virus 5 for Novell Netware", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		10, 
		"Kaspersky Anti-Virus for Novell NetWare"
	},
	{
		1013, 
		"Kaspersky Anti-Virus Corporate Suite for Servers"
	},
	{
		77, 
		"Kaspersky Anti-Virus Corporate Suite"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003, 11,  4 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1114,                                             // Application Id
	"Kaspersky Anti-Virus 5 for Novell NetWare",      // Application Name
	"5",                                              // Application version
	1,                                                // Release
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  5, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  3, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


