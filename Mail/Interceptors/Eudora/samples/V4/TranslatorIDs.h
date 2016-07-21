/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: TranslatorIDs.h 27776 2004-09-23 09:16:08Z wjb $
____________________________________________________________________________*/
#ifndef Included_TranslatorIDs_h	/* [ */
#define Included_TranslatorIDs_h

typedef enum TranslatorID
{
	kInvalidTranslatorID = 0,
	
	// note: ids must start at 1 and be sequential
	kFirstTranslatorID = 1,

	kDecryptTranslatorID = kFirstTranslatorID,
	kVerifyTranslatorID,
#if SENDOK
	kEncryptTranslatorID ,
	kSignTranslatorID,
	kEncryptAndSignTranslatorID,
#endif // SENDOK	
	kFirstManualTranslatorID,
#if SENDOK	
	kManualEncryptTranslatorID = kFirstManualTranslatorID,
	kManualSignTranslatorID,
	kManualEncryptSignTranslatorID, 
	kManualDecryptVerifyTranslatorID,
#else
	kManualDecryptVerifyTranslatorID = kFirstManualTranslatorID,
#endif // SENDOK
	kLastTranslatorIDPlusOne,
	
	kPGPNumTranslators	= kLastTranslatorIDPlusOne - kFirstTranslatorID
} TranslatorID;


typedef enum SpecialID
{
	kInvalidSpecialID = 0,
	
	// note: ids must start at 1 and be sequential
	kFirstSpecialID = 1,

	kSpecialLaunchKeysID = kFirstSpecialID,

	kLastSpecialIDPlusOne,
	
	kPGPNumSpecials	= kLastSpecialIDPlusOne - kFirstSpecialID
} TranslatorID;


#endif /* ] Included_TranslatorIDs_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/