//--------------------------------------------------------------------
// Kaspersky lab application info file
// Copyright (C) 2003 Kaspersky Labs.
// All rights reserved.
//
// Application Id             : 1107
// Application Name           : Kaspersky Anti-Virus 5 for Windows Workstations
// Application Version        : 5
// Version type               : Beta
// App info structure version : 1
//--------------------------------------------------------------------

#ifndef __APPLICATION_INFO_H__
#define __APPLICATION_INFO_H__

#include "appinfo_struct.h"


static _component_info_t COMPONENTS_LIST[] =
{
	{  90,  30 },		// Anti-Virus Server (Full functional)
	{  92,  30 },		// KAV Main Module (Full functional)
	{  94,  30 },		// MailChecker (Full functional)
	{  95,  30 },		// OnAccessScaner (Full functional)
	{  96,  30 },		// OnDemandScaner (Full functional)
	{  97,  30 },		// Quarantine & Backup (Full functional)
	{  98,  30 },		// Script Checker (Full functional)
	{  99,  30 },		// Updater - KeepUp2Date (Full functional)
	{ 101,  30 },		// Workstation Business Logic (Windows) (Full functional)
	{ 102,  30 },		// User Interface for Workstation (Windows) (Full functional)
	{ 103,  30 } 		// Office Guard (Full functional)
};

static _product_info_t PRODUCTS_LIST[] =
{
	{
		176, 
		"Kaspersky Anti-Virus 5 for Windows Workstations", 
		"", 
		0 
	}
};



_app_info_t AppInfo = 
{
	"Kaspersky lab application info file",            // Description
	1,                                                // App info structure version
	{ 2004,  3, 24 },                                 // Date create
	aitApplicationInfo,                               // Application info type
	1107,                                             // Application Id
	"Kaspersky Anti-Virus 5 for Windows Workstations",// Application Name
	"5",                                              // Application version
	0,                                                // Beta
	{ 11, COMPONENTS_LIST },                          // Components list
	{  0, 0 },                                        // Applications list
	{  1, PRODUCTS_LIST },                            // Products list
	{  0, 0 },                                        // Public key list
	{  0, 0 }                                         // Legacy applications compatibility info
};



#endif


