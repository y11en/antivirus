//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1241
// Application Name           : Kaspersky Anti-Virus 7 Japanese Edition
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
		786, 		// KL6014 
		"Kaspersky Anti-Virus 6 Japanese Edition", 
		"", 
		0 
	},
	{
		873, 		// KL6016 
		"Kaspersky Anti-Virus 7 Japanese Edition", 
		"", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		1169, 
		"Kaspersky Anti-Virus 6"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2007,  4, 17 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1241,                                             // Application Id
	"Kaspersky Anti-Virus 7 Japanese Edition",        // Application Name
	"",                                               // Application version
	1,                                                // Release
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  2, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


