// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  25 July 2006,  18:02 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- plugin_wmihlpr.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_wmihlpr )
#define __public_plugin_wmihlpr
// AVP Prague stamp end

#define PRWMIH_ID_SIZE   16

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end


// AVP Prague stamp begin( Plugin identifier,  )
#define PID_WMIHLPR  ((tPID)(61022))
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SOBKO  ((tVID)(67))
// AVP Prague stamp end

enum enWmiComponent
{
	wmicAntiVirus = 1,
	wmicAntiHaker = 2,
	wmicAntiSpyWare = 3,
};

enum enWmiStatus
{
	wmisInstalled = 1,
	wmisUninstalled = 2,
	wmisEnabled = 0x100,
	wmisSignaturesUpToDate = 0x200,
};

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_wmihlpr
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&PrWmihInstall, PID_WMIHLPR, 0x35c6fe95l },
{ (tDATA)&PrWmihUpdateStatus, PID_WMIHLPR, 0x4b42b086l },
{ (tDATA)&PrWmihUninstall, PID_WMIHLPR, 0x36e4f47cl },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tERROR IMPEX_FUNC(PrWmihInstall)( const tBYTE* p_pguidID, const tCHAR* p_pszLegacyRegKey, const tWCHAR* p_pszCompanyName, const tWCHAR* p_pszProductVersion, const tWCHAR* p_pszDisplayName )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(PrWmihUpdateStatus)( const tBYTE* p_pguidID, tDWORD p_eComponent, tDWORD p_eStatus )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(PrWmihUninstall)( const tBYTE* p_pguidID )IMPEX_INIT;
#endif
// AVP Prague stamp end

