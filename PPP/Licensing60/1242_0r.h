//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1242
// Application Name           : Kaspersky Internet Security 7 Japanese Edition
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
		778, 		// KL6237 
		"Kaspersky Internet Security for Justsystems", 
		"", 
		0 
	},
	{
		874, 		// KL6017 
		"Kaspersky Internet Security 7 Japanese Edition", 
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
	{ 2007,  4, 26 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1242,                                             // Application Id
	"Kaspersky Internet Security 7 Japanese Edition", // Application Name
	"",                                               // Application version
	1,                                                // Release
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  2, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


