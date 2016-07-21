//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 107
// Application Name           : Kaspersky Anti-Virus Personal Pro
// Application Version        : 
// Version type               : Release
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"


static _component_info_t COMPONENTS_LIST[] =
{
	{  16,  30 },		// Kaspersky Anti-Virus Monitor Win 95/98 (Full functional)
	{  17,  30 },		// Kaspersky Anti-Virus Monitor Win NT Workstation (Full functional)
	{  19,  30 },		// Kaspersky Anti-Virus Scanner Dos 32 (Full functional)
	{  21,  30 },		// Kaspersky Anti-Virus Scanner Win 95/98 (Full functional)
	{  22,  30 },		// Kaspersky Anti-Virus Scanner Win NT Workstation (Full functional)
	{  23,  20 },		// Kaspersky Anti-Virus Control Center (W/o network functionality)
	{  24,  30 },		// Kaspersky Inspector (Full functional)
	{  32,  30 },		// Kaspersky Anti-Virus Updater (Full functional)
	{  41,  30 },		// Kaspersky Office Guard (Full functional)
	{  42,  30 },		// Kaspersky Anti-Virus Office Monitor (Full functional)
	{  43,  30 },		// Kaspersky Anti-Virus Mail Checker (Full functional)
	{  51,  30 },		// Kaspersky Anti-Virus Script Checker (Full functional)
	{  52,  30 } 		// Kaspersky Anti-Virus Rescue Disk (Full functional)
};

static _product_info_t PRODUCTS_LIST[] =
{
	{
		8, 
		"Kaspersky Anti-Virus Personal Pro", 
		"4", 
		0 
	}
};



_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003,  7, 30 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	107,                                             // Application Id
	"Kaspersky Anti-Virus Personal Pro",              // Application Name
	"",                                               // Application version
	1,                                                // Release
	{ 13, COMPONENTS_LIST },                          // Components list
	{  0, 0 },                                        // Applications list
	{  1, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  0, 0 }                                         // Legacy applications compatibility info
};



#endif


