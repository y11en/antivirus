//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1230
// Application Name           : Kaspersky Internet Security 7
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
		0, 		// 0 
		"Ultimate Product", 
		"", 
		0 
	},
	{
		496, 		// KL1821 
		"Kaspersky Internet Security 6.0", 
		"", 
		0 
	},
	{
		848, 		// KL1822 
		"Kaspersky Personal Network Security", 
		"", 
		0 
	},
	{
		851, 		// KL1825 
		"Kaspersky Internet Security 7.0", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		1165, 
		"Kaspersky Internet Security 6"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2007,  3, 26 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1230,                                             // Application Id
	"Kaspersky Internet Security 7",                  // Application Name
	"",                                               // Application version
	1,                                                // Release
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  4, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


