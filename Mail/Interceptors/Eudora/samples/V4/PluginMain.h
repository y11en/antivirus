/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: PluginMain.h 27776 2004-09-23 09:16:08Z wjb $
____________________________________________________________________________*/
#ifndef Included_PluginMain_h	/* [ */
#define Included_PluginMain_h

#include "emssdk/ems-win.h"
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
		( EMSF_ON_ARRIVAL | EMSF_ON_DISPLAY | EMSF_WHOLE_MESSAGE | 
			EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		IDI_PGP_DECRYPT
	},
	{
		"Verify PGP/MIME Message",
		EMST_SIGNATURE,
		kVerifyTranslatorID, 
		( EMSF_ON_ARRIVAL | EMSF_ON_DISPLAY | EMSF_WHOLE_MESSAGE | 
			EMSF_REQUIRES_MIME | EMSF_GENERATES_MIME ),
		IDI_PGP_DECRYPT
	},
#if SENDOK
	{
		"PGP Encrypt",
		EMST_PREPROCESS,
		kEncryptTranslatorID, 
		( EMSF_Q4_TRANSMISSION | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | 
			EMSF_GENERATES_MIME),
		IDI_PGP_ENCRYPT
	},
	{
		"PGP Sign",
		EMST_SIGNATURE,
		kSignTranslatorID, 
		( EMSF_Q4_TRANSMISSION | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | 
			EMSF_GENERATES_MIME),
		IDI_PGP_SIGN
	},
	{
		"PGP Encrypt & Sign",
		EMST_COALESCED,
		kEncryptAndSignTranslatorID, 
		( EMSF_Q4_TRANSMISSION | EMSF_WHOLE_MESSAGE | EMSF_REQUIRES_MIME | 
			EMSF_GENERATES_MIME ),
		0L
	},
	{
		"PGP Encrypt",
		EMST_LANGUAGE,
		kManualEncryptTranslatorID, 
		( EMSF_ON_REQUEST | EMSF_WHOLE_MESSAGE  ),
		IDI_PGP_ENCRYPT
	},
	{
		"PGP Sign",
		EMST_LANGUAGE,
		kManualSignTranslatorID, 
		( EMSF_ON_REQUEST | EMSF_WHOLE_MESSAGE  ),
		IDI_PGP_SIGN
	},
	{
		"PGP Encrypt & Sign",
		EMST_LANGUAGE,
		kManualEncryptSignTranslatorID,
		( EMSF_ON_REQUEST | EMSF_WHOLE_MESSAGE  ),
		IDI_PGP_ENCRYPT_SIGN
	},
#endif // SENDOK
	{
		"PGP Decrypt & Verify",
		EMST_LANGUAGE,
		kManualDecryptVerifyTranslatorID, 
		( EMSF_ON_REQUEST | EMSF_WHOLE_MESSAGE | EMSF_GENERATES_MIME |
			EMSF_TOOLBAR_PRESENCE),
		IDI_PGP_DECRYPT
	}
};

static TransInfoStruct gSpecialInfo[] = {

	{
		"Launch PGP Desktop",
		EMST_LANGUAGE,
		kSpecialLaunchKeysID,
		( EMSF_ON_REQUEST | EMSF_TOOLBAR_PRESENCE  ),
		IDI_PGP_KEYS
	}

};

static const long	kNumTrans = (sizeof(gTransInfo)/sizeof(*gTransInfo));

static const long	kNumSpecial = 
				(sizeof(gSpecialInfo)/sizeof(*gSpecialInfo));

// for future use we have 881 882 883 884 as well
static const short	kPluginID = 880; 

#endif /* ] Included_PluginMain_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/


