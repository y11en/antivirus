//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1265
// Application Name           : Kaspersky Internet Security 8
// Application Version        : 
// Version type               : Release
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"



static _product_info_t PRODUCTS_LIST[] =
{
	{
		851, 		// KL1825 
		"Kaspersky Internet Security 7.0", 
		"", 
		0 
	},
	{
		923, 		// KL1808 
		"Kaspersky Internet Security 8.0", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		1230, 
		"Kaspersky Internet Security 7"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2007, 11, 20 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1265,                                             // Application Id
	"Kaspersky Internet Security 8",                  // Application Name
	"",                                               // Application version
	1,                                                // Release
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  2, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


