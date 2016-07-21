//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1109
// Application Name           : Kaspersky Anti-Virus for MS ISA Server
// Application Version        : 
// Version type               : Beta
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
		148, 
		"Kaspersky Anti-Virus for MS ISA Server", 
		"", 
		0 
	},
	{
		150, 
		"Kaspersky Anti-Virus Corporate for Internet Gateway Optional", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		77, 
		"Kaspersky Anti-Virus Corporate Suite"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003, 11, 18 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1109,                                             // Application Id
	"Kaspersky Anti-Virus for MS ISA Server",         // Application Name
	"",                                               // Application version
	0,                                                // Beta
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  4, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


