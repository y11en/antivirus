//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1101
// Application Name           : Kaspersky Anti-Virus 5 Personal
// Application Version        : 5
// Version type               : Release
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"


static _component_info_t COMPONENTS_LIST[] =
{
	{  90,  30 },		// Anti-Virus Server (Full functional)
	{  91,  30 },		// Personal Business Logic (Full functional)
	{  92,  30 },		// KAV Main Module (Full functional)
	{  93,  30 },		// User Interface for Personal (Full functional)
	{  94,  30 },		// MailChecker (Full functional)
	{  95,  30 },		// OnAccessScaner (Full functional)
	{  96,  30 },		// OnDemandScaner (Full functional)
	{  97,  30 },		// Quarantine & Backup (Full functional)
	{  98,  30 },		// Script Checker (Full functional)
	{  99,  30 } 		// Updater - KeepUp2Date (Full functional)
};

static _product_info_t PRODUCTS_LIST[] =
{
	{
		7, 
		"Kaspersky Anti-Virus Personal", 
		"4", 
		0 
	},
	{
		91, 
		"Kaspersky Anti-Virus OEM Personal", 
		"", 
		0 
	},
	{
		131, 
		"Kaspersky Anti-Virus 5 Personal", 
		"5", 
		0 
	}
};


static _app_compat_info_t APP_COMPAT_LIST[] =
{
	{
		105, 
		"Kaspersky Anti-Virus Personal"
	}
};

_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2003, 11,  4 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1101,                                             // Application Id
	"Kaspersky Anti-Virus 5 Personal",                // Application Name
	"5",                                              // Application version
	1,                                                // Release
	{ 10, COMPONENTS_LIST },                          // Components list
	{  0, 0 },                                        // Applications list
	{  3, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  1, APP_COMPAT_LIST }                           // Legacy applications compatibility info
};



#endif


