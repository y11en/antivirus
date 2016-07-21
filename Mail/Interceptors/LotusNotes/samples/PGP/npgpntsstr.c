/*____________________________________________________________________________
	Copyright (C) 2005 PGP Corporation
	All rights reserved.

	$Id: npgpntsstr.c 31157 2005-01-02 18:17:37Z wjb $
____________________________________________________________________________*/
#include "pgpPFLConfig.h"       /* or pgpConfig.h in the SDK */
#include "pgprc.h"
#include "resource.h"

PGPLocStringsContextRef g_CSStringContext=NULL;

PGPStrLocWChar *ModuleDependentCS( const char *string, const PGPChar *wide, const char *description )
{
	return MacroLookup(
		&g_CSStringContext,
		PGPTXT_MACHINE("nPGPnts"), 
		string, 
		wide,
		description );
}

const PGPStrLocWChar *GetPGPCSString(DWORD dwID)
{
	const PGPStrLocWChar *shared;

	shared=GetPGPCSSharedString(dwID);

	if(shared!=NULL)
		return shared;

	return NULL;
}
