//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1113
// Application Name           : Kaspersky Anti-Spam API
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
		136, 
		"Kaspersky Anti-Spam for Astaro Security Linux", 
		"", 
		0 
	}
};



_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003,  8,  4 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1113,                                             // Application Id
	"Kaspersky Anti-Spam API",                        // Application Name
	"",                                               // Application version
	1,                                                // Release
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  1, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  0, 0 }                                         // Legacy applications compatibility info
};



#endif


