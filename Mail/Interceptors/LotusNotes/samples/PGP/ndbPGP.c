/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: ndbPGP.c 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Interface to the Lotus Notes "database-driver" API interface -- the only means 
by which an @Formula may call a custom DLL procedure, and the only way by which 
custom DLLs can be hooked to stay resident throughout the Notes session without 
use of a NOTES.INI reference (i.e. NSF Hooks or Extension Manager).

--- revision history ---------
3/14/03 Version 1.3 Paul Ryan
+ logic enhancement to improve error reporting to Notes client
+ logic enhancement to support Notes 6 client
+ documentation improvement

7/2/01 Version 1.2 Paul Ryan
+ logic enhancement in support of robust plug-in version handling
+ listing format adjustment, documentation adjustment

10/7/00 Version 1.1.3 Paul Ryan
+ replaced version-check call handler in us_PlugInCommand() with handler that 
  simply loads this "database driver"
+ documentation adjustment

9/16/00 Version 1.1.2 Paul Ryan
+ support for special-handling enhancements made with respect to PGP-encoded 
  attachments

8/9/00 Version 1.1.1 Paul Ryan
+ support for checkmark-toggle UI for plug-in's drop-down action-bar menu

3/20/00 Version 1.1 Paul Ryan
+ Module moved out of own shell DLL into the main plug-in DLL. Associated 
  adjustment.
+ moved PGP dis/engagement into this "database-driver" module
+ minor logic, documentation improvements

8/27/99 Version 1.0.1 Paul Ryan
+ pre-mail send logic adjustment

1/10/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "ndbPGP.h"


BOOL  ef_LoadedSessionLong;


/** xus_MailSendHookInitialize( ***

--- parameter & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history ------
3/14/03 PR: added initialization of fUpdateIfModified member
10/7/00 PR: removed logic to initialize this DLL's PGP connection due to 
	update in the plug-in subform's handling of this issue 
3/20/00	PR: moved PGP initialization into this event
12/10/98 PR: created		*/
//DOC!!
STATUS LNPUBLIC xus_MailSendHookInitialize( DBVEC * pt_drv)	{
    //fill in the subroutine vectors
    pt_drv->Init = xus_MailSendHookInitialize;
    pt_drv->Term = us_Terminating;
    pt_drv->Open = us_dmyOpen;
    pt_drv->Close = us_dmyClose;
	pt_drv->Function = us_PlugInCommand;

	pt_drv->fUpdateIfModified = FALSE;

	return eus_SUCCESS;
} //xus_MailSendHookInitialize(


/** us_Terminating( ***
The "database driver" (and therefore the DLL) is closing. Take this chance to 
do any clean-up needed.

--- parameter & return ----
pt_drv: address of information structure describing aspects of this 
	"database driver"
RETURN: eus_SUCCESS

--- revision history ------
10/7/00 PR: minor documentation adjustment

9/16/00 PR
+ added clean-up of allocated Windows GDI resources
+ completed standard documentation

3/20/00	PR: moved PGP disengagement into this event
12/10/98 PR: created			*/
static STATUS LNPUBLIC us_Terminating( DBVEC * pt_drv) {
	//Disengage PGP as necessary. Cannot do this in DllMain() because PGP 
	//	involves worker threads.
	e_ReleasePgpContext();

	//free dynamically, statically allocated Windows GDI resources
	e_FreeGdiResources();

    return eus_SUCCESS;
} //us_dmyTerminate(


static STATUS LNPUBLIC us_dmyOpen( DBVEC * pt_drv, 
									HDBDSESSION * ppv_dmySession)	{
    *ppv_dmySession = NULL;
    return eus_SUCCESS;
} //us_dmyOpen(


static STATUS LNPUBLIC us_dmyClose( DBVEC * pt_drv, 
									HDBDSESSION  pv_dmy)	{
    return eus_SUCCESS;
} //us_dmyClose(


/** us_PlugInCommand( ***
Clearinghouse procedure for all @DbCommand calls into this "database driver" 
DLL. No memory-block output created unless otherwise noted. Currently six tasks 
are supported:

+ PGP secure specified rich-text field
  - 7 arguments required, see eus_PreMailSend() documentation
  - RETURN: if error encountered, number "range" whose first element tells the 
		  code of the error that occurred

+ Load Plug-In DLL on a session-long basis (no arguments)

+ Replace MIME Part items with previously preserved rich-text stream
  - Arg1: numberlist whose first element represents the handle to the note 
	      currently being processed
  - Arg2: textlist whose first element represents the name of the MIME-Part 
          items and the replacement rich-text field

+ Test whether rich-text field contains PGP-encoded content
  - Arg1: numberlist whose first element represents the handle to the note 
	      currently being processed
  - Arg2: textlist whose first element represents the name of the rich-text 
          field
  - Arg3: Dummy argument to differentiate call. Ignored.
  - RETURN: if encoded content found, text list of plug-in state flags telling 
	      the type of content found

+ Undertake special treatment of the given PGP-encrypted attachment
  - Arg1: textlist whose first element represents the object name of the 
          PGP-encrypted attachment to be handled specially and whose second 
		  element represents the original filename of the PGP-encrypted 
		  attachment
  - Arg2: numberlist whose first element represents the handle to the note 
	      currently being processed and containing the target attachment

+ Return the text associated with the given error code
  - Arg1: numberlist whose first element tells the error code to look up
  - RETURN: text list whose first element gives the text associated with the 
		  error

--- parameters & return ----
pt_drv: Context information structure used by Notes to call this driver.
us_FUNC: Flags which type of database-driver @Formula command called this 
	procedure. Only @DbCommand is supported here.
us_ARGC: tells the number of arguments passed in the @DbCommand call
pul_ARGL: address of a one-to-one-correspondence array of the lengths of the 
	arguments passed in the @Formula call
ppv_ARGV: address of array of pointers to the arguments passed in the @Formula 
	call
ph_rslt: Output. Address of variable to receive the handle to the result set 
	determined by the procedure. If specified call has no result set, 
	NULLHANDLE is returned.
pul_lenRslt: Output. Address of variable to receive the length of the result 
	set determined by the procedure. If specified call has no result set, NULL 
	is returned.
RETURN: 
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if any input parameter is obviously invalid
	the Notes API error code if a Notes API error occurred
	!eus_SUCCESS if any PGP error occurred during its initialization routine as 
		part of "Load Plug-In DLL on a session-long basis"

--- revision history -------
3/14/03 PR
+ added means by which to test note for PGP ASCII-Armor content
+ added means by which the text associated with an error code may be retrieved
+ signature adjustment to indicate an unsigned return value
+ documentation adjustment

9/6/02 PR: minor exception-handling adjustment, minor documentation adjustment

7/2/01 PR
+ logic adjustment in support of relocation of PGP initialization to be part of 
  "Load Plug-In DLL on a session-long basis"
+ minor documentation correction, listing format adjustment

10/7/00 PR
+ replaced version-check call handler with handler that simply loads this 
  "database driver" so that this DLL will stay resident for the entire Notes 
  session, not just the message session
+ documentation adjustment

9/16/00 PR: added a call handler for when a PGP-encoded attachment's action 
	hotspot is invoked to provide special handling for the attachment
8/9/00 PR: added a call handler for support of a checkmark-toggle UI on the 
	drop-down action-bar menu

3/20/00 PR
+ improved error-return information
+ apart from specification of "PGP secure specified rich-text field" task 
  arguments, completed standard documentation

8/27/99 PR: logic adjustments to support enhancement of better emulation of 
	Lotus Notes' name-lookup behavior, including removal of recipient-count 
	parameter
12/10/98 PR: created		*/
static STATUS LNPUBLIC us_PlugInCommand( DBVEC * pt_drv, 
											HDBDSESSION  pv_dmy, 
											WORD  us_FUNC, 
											WORD  us_ARGC, 
											DWORD * pul_ARGL, 
											void * *const  ppv_ARGV, 
											HANDLE * ph_rslt, 
											DWORD * pul_lenRslt)	{
	const WORD  us_ARGS_PGP_SECURE_RTF = 7, us_ARGS_LOAD_DLL = 0, 
				us_ARGS_TWO = 2, us_ARGS_TEST_PGP_CTNT = 3, us_ARGS_ERR_TXT = 1;

	int  i_err;

	//if any of the important parameters are obviously invalid, return general 
	//	failure
	if (us_FUNC != DB_DBEXT)
		return ERR_DBD_FUNCTION;
	if (!( (us_ARGC == us_ARGS_PGP_SECURE_RTF || us_ARGC == us_ARGS_LOAD_DLL || 
										us_ARGC == us_ARGS_TWO || us_ARGC == 
										us_ARGS_TEST_PGP_CTNT || us_ARGC == 
										us_ARGS_ERR_TXT) && pul_ARGL && 
										ppv_ARGV && ph_rslt && pul_lenRslt))
		return eus_ERR_INVLD_ARG;

	*ph_rslt = NULLHANDLE;
	*pul_lenRslt = NULL;

	if (us_ARGS_PGP_SECURE_RTF == us_ARGC)
		return eus_PreMailSend( ppv_ARGV, ph_rslt, pul_lenRslt);
	else if (us_ARGS_ERR_TXT == us_ARGC)	{
		NUMBER  d;
		char  pc[ MAXPATH];
		DWORD  ul, ul_len;
		BYTE * puc;
		STATUS  us_err;

		//determine the text associated with the given error code, and set it 
		//	as the call's output
		if (!ef_GetNumberListEntry( ppv_ARGV[ 0], TRUE, 0, &d))
			return eus_ERR_INVLD_ARG;
		ul = OSLoadString( NULLHANDLE, ERR( (STATUS) d), pc, MAXPATH - 1);
		if (us_err = OSMemAlloc( NULL, ul_len = sizeof( WORD) + sizeof( LIST) + 
												sizeof( USHORT) + ul, ph_rslt))
			return us_err;
		puc = (BYTE *) OSLockObject( *ph_rslt);
		*((WORD *) puc)++ = TYPE_TEXT_LIST;
		(((LIST *) puc)++)->ListEntries = 1;
		*((USHORT *) puc)++ = (USHORT) ul;
		memcpy( puc, pc, ul);
		OSUnlockObject( *ph_rslt);
		*pul_lenRslt = ul_len;

		return eus_SUCCESS;
	}else if (us_ARGS_LOAD_DLL == us_ARGC)	{
		//if we've already done this work, short-circuit with success
		if (ef_LoadedSessionLong)
			return eus_SUCCESS;

		//Engage PGP. Cannot do this in DllMain() because PGP involves worker 
		//	threads.
		if (i_err = ei_InitializePgpContext())
			return !eus_SUCCESS;

		//note that this DLL has been successfully enabled for the duration of 
		//	the current Notes-client session
		ef_LoadedSessionLong = TRUE;

		return eus_SUCCESS;
	}else if (us_ARGS_TWO == us_ARGC)
		if (*(WORD *) ppv_ARGV[0] == TYPE_NUMBER_RANGE)
			return es_ReplaceMimeWithRtf( ppv_ARGV);
		else
			return es_SpecialDecryptAttachment( ppv_ARGV);
	else if (us_ARGS_TEST_PGP_CTNT == us_ARGC)
		return eus_TestNoteForPgpCtnt( ppv_ARGV, ph_rslt, pul_lenRslt);

	return !eus_SUCCESS;
} //us_PlugInCommand(


