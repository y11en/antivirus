/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: Translators.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_Translators_h	/* [ */
#define Included_Translators_h


long CanPerformTranslation(
	const long trans_id, 
	const emsMIMEtypeP in_mime, 
	const long context
	);

long PerformTranslation(
	const long trans_id, 
	const char* in_file, 
	const char* out_file, 
	emsHeaderDataP header,
	emsMIMEtypeP in_mime,
	emsMIMEtypeP* out_mime
	);


#endif /* ] Included_Translators_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/