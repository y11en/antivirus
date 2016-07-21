/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: TranslatorIDs.h 9679 2002-08-06 20:11:24Z dallen $
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
	
	kEncryptTranslatorID ,
	kSignTranslatorID,
	kEncryptAndSignTranslatorID,
	
	kFirstManualTranslatorID,
	
	kManualEncryptSignTranslatorID = kFirstManualTranslatorID,
	kManualDecryptVerifyTranslatorID,
	kLastTranslatorIDPlusOne,
	
	kPGPNumTranslators	= kLastTranslatorIDPlusOne - kFirstTranslatorID
} TranslatorID;



#endif /* ] Included_TranslatorIDs_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/