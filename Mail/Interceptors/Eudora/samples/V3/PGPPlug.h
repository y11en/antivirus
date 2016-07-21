/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: PGPPlug.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_PGPPlug_h 
#define Included_PGPPlug_h 

#include "emssdk/ems-win.h"
#include "PGPPlugTypes.h"
#include "TranslatorIDs.h"
#include "resource.h"

#define InitPtrToNull( p )	{ if ( !( p ) )	*(p) = NULL; }

typedef struct _TransInfoStruct {
	char *description;
	long type;
	long subtype;
	unsigned long flags;
	unsigned long nIconID;
}TransInfoStruct;

static TransInfoStruct gTransInfo[] = {
	{
		"Decrypt PGP/MIME Message",
		EMST_PREPROCESS,
		kDecryptTranslatorID, 
		( EMSF_ON_ARRIVAL | EMSF_ON_DISPLAY | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		IDI_PGP_DECRYPT
	},
	{
		"Verify PGP/MIME Message",
		EMST_SIGNATURE,
		kVerifyTranslatorID, 
		( EMSF_ON_ARRIVAL | EMSF_ON_DISPLAY | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		IDI_PGP_DECRYPT
	},
	{
		"PGP plugin 1 (use toolbar)",
		EMST_PREPROCESS,
		kEncryptTranslatorID, 
		( EMSF_Q4_TRANSMISSION | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		IDI_PGP_ENCRYPT
	}
	,
	{
		"PGP plugin 2 (use toolbar)",
		EMST_SIGNATURE,
		kSignTranslatorID, 
		( EMSF_Q4_TRANSMISSION | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		IDI_PGP_SIGN
	},
	{
		"PGPmime Encrypt & Sign",
		EMST_COALESCED,
		kEncryptAndSignTranslatorID, 
		( EMSF_Q4_TRANSMISSION | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		0L
	}
};

static const long	kNumTrans	= (sizeof(gTransInfo)/sizeof(*gTransInfo));
static const short	kPGPPluginID= 880;	// for future use we have 
										// 881 882 883 884 as well

#endif /* Included_PGPPlug_h */

/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
