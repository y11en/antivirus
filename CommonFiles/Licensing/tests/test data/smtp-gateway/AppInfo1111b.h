//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1111
// Application Name           : Kaspersky SMTP-Gateway (Unix)
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
		135, 
		"Kaspersky Anti-Virus 5 Business Optimal", 
		"5", 
		0 
	}
};



_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003,  9, 19 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1111,                                             // Application Id
	"Kaspersky SMTP-Gateway (Unix)",                  // Application Name
	"",                                               // Application version
	0,                                                // Beta
	{  0, 0 },                                        // Components list
	{  0, 0 },                                        // Applications list
	{  1, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  0, 0 }                                         // Legacy applications compatibility info
};



#endif


