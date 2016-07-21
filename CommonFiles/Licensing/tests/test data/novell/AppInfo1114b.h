//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1114
// Application Name           : Kaspersky Anti-Virus 5 for Novell NetWare
// Application Version        : 5
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
		"Kaspersky Anti-Virus 5 Business Optimal", 
		"4", 
		0 
	}
};



_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003,  7, 17 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	10,                                               // Application Id
	"Kaspersky Anti-Virus 5 for Novell NetWare",      // Application Name
	"4",                                              // Application version
	1,                                                // Beta
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  1, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  0, 0 }                                         // Legacy applications compatibility info
};



#endif


