/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: Translators.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_Translators_h	/* [ */
#define Included_Translators_h

#include "PGPPlugTypes.h"

PluginError CanPerformTranslation(
	short trans_id, 
	emsMIMEtypeP in_mime, 
	long context
	);

PluginError PerformTranslation(
	short trans_id, 
	char* in_file, 
	char* out_file, 
	char** addresses, 
	emsMIMEtypeP  in_mime,
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

