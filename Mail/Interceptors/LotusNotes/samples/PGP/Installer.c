/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: Installer.c 15637 2003-07-22 16:43:18Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Provides un/installation and administration services to the PGP Lotus Notes 
Plug-In.

--- suggested enhancements ----
3/16/00 PR: provide more flexibility at install time by implementing 
	suggestion specified in xus_InstallNotesComponents()
2/23/00 PR: see description of suggestion in us_PgpEnableEmailForm() 
	documentation
1/11/99 PR: decompose ACL stuff into an AclHandling module, maybe

--- revision history ----------
7/21/03 Version 1.2 Paul Ryan
+ extension to incorporate new DLL carrying plug-in subform design element
+ numerous minor clean-up & consistency improvements

3/14/03 Version 1.1.3.3 Paul Ryan
+ changes in support of improvements to globals-handling

7/2/01 Version 1.1.3.2 Paul Ryan
+ removed remmed-out code in us_PgpEnableEmailForm()

4/24/01 Version 1.1.3.1 Paul Ryan
+ removed remmed-out code in us_GetSourceEnvInfo()

8/9/00 Version 1.1.3 Paul Ryan
+ made xus_InstallNotesComponents() and xus_UninstallNotesComponents() adhere 
  to the __stdcall calling convention because in certain situations the 
  stack was not being cleaned up by the caller

3/16/00 Version 1.1.2 Paul Ryan
+ documentation adjustment
+ minor logic adjustment

2/23/00 Version 1.1.1 Paul Ryan
+ to prevent "corrupted note" error message in Notes 4.5.1 client, added 
  OPEN_EXPAND flag when opening notes to be signed
+ minor documentation adjustment

9/20/99 Version 1.1 Paul Ryan
+ integrated uninstall services
+ made install process more robust
+ decomposed database-handling services into own module incorporated here via 
  LibDbAndNote.h
+ documentation adjustment, logic enhancement

2/8/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "Installer.h"

#define mpc_SUBFORMNM_PGP  "s-PGP E-Mail Plug-In"
static char  mpc_APPNM[] = "Lotus Notes PGP Plugin", 
				mpc_SUBFORMNM_PGP_DLL[] = mpc_SUBFORMNM_PGP " DLL", 
				mpc_ABT_DOC_LIB[] = "sl-About Document Manipulation", 
				mpc_FORMNM_MEMO[] = "Memo", mpc_FORMNM_REPLY[] = "Reply", 
				mpc_FORMNM_REPLY_WH[] = "Reply With History", 
				mpc_FORMNM_DEFAULT[] = "(Default)", mpc_DELIMITER[] = "|\n", 
				mpc_FILENM_PGP_TEMPLATE_DEFAULT[] = "PGP Plugin Template.nsf";
static STATUS  mus_HANDLED_EXCEPTION = 0xFFFF;

static HANDLE  mh_mainWnd, mh_Instance;


/** DllMain( ***
Win32 entry point for loading and unloading this DLL.  We use the unload 
event to release all Extension Manager hooks and for freeing allocated memory.

--- parameters & return ---
h_DllInstance: the overall handle to this DLL's runtime information, settings
ul_reason: parameter tells why the function was called
pv_reserved: just that, reserved

--- revision history ------
9/20/99 PR: adjusted algorithm for hooking to an expected parent window
2/5/99 PR: created			*/
BOOL WINAPI DllMain( HINSTANCE  h_DllInstance, 
						DWORD  ul_reason, 
						LPVOID  pv_reserved)	{
	STATUS  us_err;

	switch (ul_reason)	{
		case DLL_PROCESS_ATTACH:
			//Set up debugging aids and the memory-leak detector in 
			//	particular.  Lines are preprocessed out in Release builds.
			_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG | 
														_CRTDBG_MODE_WNDW);
			_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | 
										_CrtSetDbgFlag( _CRTDBG_REPORT_FLAG));

			//if the DLL is being invoked by the InstallShield installer... 
			mh_Instance = h_DllInstance;
			if (!( mh_mainWnd = FindWindow( "InstallShield_Win", NULL)))
				mh_mainWnd = FindWindow( NULL, "PGPinstl"
									/*"c:\\Notes\\client\\PGPinstl.exe"*/);
			if (mh_mainWnd)	{
				char  pc_ModuleNm[ MAX_PATH];

				//initialize a Notes session
				if (!GetModuleFileName( h_DllInstance, pc_ModuleNm, MAX_PATH))
					return FALSE;
				if (us_err = NotesInitExtended( 1, (char * *) &pc_ModuleNm))
					return FALSE;
			//else it must be that the DLL is being invoked by the refresh 
			//	button in the plug-in template's About document
			}else	{
				const char  pc_CLASSNM_NOTES[] = "NOTES", 
							pc_CLASSNM_NOTES_FRAME[] = "NOTESframe";

				HWND  h;
				char pc[ sizeof( pc_CLASSNM_NOTES_FRAME) > 
											sizeof( pc_CLASSNM_NOTES) ? 
											sizeof( pc_CLASSNM_NOTES_FRAME) : 
											sizeof( pc_CLASSNM_NOTES)];

				//Store the handle of the main window associated with the rich-text 
				//	window currently being operated on by user. Loop is used because 
				//	the different releases of Notes have different window hierarchies.
				if (h = GetFocus())	{
					do
						if (GetClassName( h, pc, sizeof( pc)))
							if (strcmp( pc, pc_CLASSNM_NOTES) == ei_SAME || 
												strcmp( pc, 
												pc_CLASSNM_NOTES_FRAME) == 
												ei_SAME)	{
								mh_mainWnd = h;
								break;
							}
					while (h = GetParent( h));
					if (!mh_mainWnd)
						MessageBox( NULL, "The parent Lotus Notes "
												"window\ncould not be "
												"determined", mpc_APPNM, 
												MB_OK | MB_ICONEXCLAMATION);
				} //if (h = GetFocus())
			} //if (mh_mainWnd)
	} //switch (ul_reason)

	return TRUE;
} //DllMain(


/** xus_InstallNotesComponents( ***
Controlling procedure for selectively installing the Lotus Notes components of 
the PGP Lotus Notes Plug-In into a target Lotus Domino environment.

The tasks of the procedure vary substantially depending on which of the last 
four optional parameters are provided. Read parameter description carefully.

--- suggested enhancement ---
3/16/00 PR: honor input of existing plug-in template filename when refreshing 
	the template with the template contained in the installer (i.e. the 
	Create/Refresh wizard option that's indicated by input of the mail 
	template's filename), thus no longer forcing an organization to use our 
	default filename for the plug-in template

--- parameters & return ----
pc_SVRNM: address of the name of the Domino server on which the plug-in's 
	components are to be installed
pc_EXTFILENM_MAIL_TMPLT: Optional. Address of the extended filename of the 
	Notes Mail template whose e-mail forms will be copied to populate/refresh 
	the PGP Plugin Template database whose base filename is hardcoded in this 
	module. If NULL, procedure will perform no design refresh; the "install" is 
	limited to the PGP enabling of particular Notes Mail databases.
pc_EXTFILENM_PGP_TMPLT: Optional. Address of the extended filename of the 
	already installed PGP Plugin Template database. Parameter required if the 
	Notes Mail template parameter (pc_EXTFILENM_MAIL_TMPLT) is not provided and 
	specific Notes Mail databases are to be PGP enabled (i.e. either 
	pc_EXTFILENM_DB_LIST or pc_MailDbs was provided). Parameter ignored if the 
	Notes Mail template parameter is provided.
pc_EXTFILENM_DB_LIST: Optional. Address of a string indicating the extended 
	filename of a text file that contains the list of extended filenames of the 
	the Notes Mail databases to be PGP enabled. The filenames must be delimited 
	by a pipe character or linefeed character; whitespace surrounding each 
	filename is ignored. A pass of NULL or pointer to a null string indicates 
	that any list of Notes Mail databases to be enabled is provided via the 
	pc_MailDbs parameter.
pc_MailDbs: Optional Input & Undefined Output. Address of a string indicating 
	the extended filenames of the particular Notes Mail databases to PGP 
	enable. Each filename must be delimited by a pipe character or linefeed 
	character; whitespace surrounding each filename is ignored. String will be 
	tokenized and therefore changed in a way not worth defining (see 
	documentation on standard strtok() function if understanding is required). 
	Parameter is ignored if the pc_EXTFILENM_DB_LIST parameter is non-NULL. A 
	pass of NULL or pointer to a null string indicates that no explicitly named 
	Notes Mail database are to be enabled by the procedure.
RETURN:
	eus_SUCCESS if no error occured
	mus_EXCEPTION_HANDLED if any Notes API or module-defined error occurs
	undefined error code if some sort of subsystem error occurs (memory 
		depletion, file system access denied, etc.)

--- revision history -------
7/21/03 PR
+ extended to support new plug-in DLL subform element
+ minor signature change toward maintaining appropriate constness
+ minor documentation adjustment, token renaming, listing format adjustment

8/9/00 PR: changed to the __stdcall calling convention to force the function 
	to clean up the stack itself
3/16/00 PR: documentation adjustment

9/20/99 PR
+ logic adjustment in conjunction with recast of install routine
+ completed standard documentation

2/6/99 PR: created			*/
STATUS __stdcall xus_InstallNotesComponents( 
										const char  pc_SVRNM[], 
										const char  pc_EXTFILENM_MAIL_TMPLT[], 
										const char  pc_EXTFILENM_PGP_TMPLT[], 
										const char  pc_EXTFILENM_DB_LIST[], 
										char  pc_MailDbs[])	{
	const f_MAIL_TMPLT = pc_EXTFILENM_MAIL_TMPLT && *pc_EXTFILENM_MAIL_TMPLT, 
			f_DB_LIST = pc_EXTFILENM_DB_LIST && *pc_EXTFILENM_DB_LIST, 
			f_MAIL_DBS = pc_MailDbs && *pc_MailDbs;

	EnvInfo  t_env;
	BOOL  f_fileInput = FALSE, f_fail = FALSE;
	STATUS  us_err, us_error;

	if (!mh_mainWnd)	{
		MessageBox( NULL, "PGPinstl.dll failed to attach to an apporpriate\n"
												"application parent window", 
												mpc_APPNM, MB_ICONSTOP);
		return !eus_SUCCESS;
	}

	if (!( pc_SVRNM && (f_MAIL_TMPLT || f_DB_LIST || f_MAIL_DBS) && 
									(!f_MAIL_TMPLT ? 
									pc_EXTFILENM_PGP_TMPLT && 
									*pc_EXTFILENM_PGP_TMPLT : TRUE)))	{
		MessageBox( mh_mainWnd, "Incorrect parameters provided to\n"
												"xus_InstallNotesComponents", 
												mpc_APPNM, MB_ICONSTOP);
		return !eus_SUCCESS;
	}

	if (f_MAIL_TMPLT)	{
		if (us_err = us_InstallPlugInTemplate( pc_SVRNM, 
									pc_EXTFILENM_MAIL_TMPLT, 
									mpc_FILENM_PGP_TEMPLATE_DEFAULT, &t_env))
			goto errJump;
	}else if (f_DB_LIST || f_MAIL_DBS)	{
		memset( &t_env, NULL, sizeof( EnvInfo));
		if (us_err = us_GetSourceEnvInfo( pc_SVRNM, 
										pc_EXTFILENM_PGP_TMPLT, &t_env))
			goto errJump;
		if (us_err = NIFFindDesignNote( t_env.h_template, 
										mpc_SUBFORMNM_PGP, NOTE_CLASS_FORM, 
										&t_env.nid_PgpSbfrmMain))
			goto errJump;
		if (us_err = NIFFindDesignNote( t_env.h_template, 
										mpc_SUBFORMNM_PGP_DLL, NOTE_CLASS_FORM, 
										&t_env.nid_PgpSbfrmDll))
			goto errJump;
	} //if (f_MAIL_TMPLT)

	if (f_DB_LIST || f_MAIL_DBS)	{
		char * pc_mereErrLog;

		us_err = us_RefreshMailDbs( pc_SVRNM, f_DB_LIST ? 
										pc_EXTFILENM_DB_LIST : NULL, 
										pc_MailDbs, &t_env, &pc_mereErrLog);
		if (pc_mereErrLog)	{
			FILE * pfl;

			//let the user know about any "mere" errors
			if (f_fail = !(pfl = fopen( "PGPLog.txt", "wtc")))
				goto errJump;
			fprintf( pfl, pc_mereErrLog);
			fclose( pfl);	//ignore failure
			MessageBox( mh_mainWnd, f_DB_LIST ? "Some databases could not "
										"be processed. Refer to\nPGPLog.txt "
										"in the Notes Program directory "
										"for details." : pc_mereErrLog, 
										mpc_APPNM, MB_ICONEXCLAMATION);
			free( pc_mereErrLog);
		} //if (pc_mereErrLog)
	} //if (f_DB_LIST || f_MAIL_DBS)

errJump:
	if (t_env.h_template)
		if ((us_error = NSFDbClose( t_env.h_template)) && !us_err)
			us_err = us_error;

	if (us_err && us_err != mus_HANDLED_EXCEPTION)	{
		char  pc_msg[ 512] = "An unexpected error occurred, translated by "
															"Notes as\n\"";

		OSLoadString( mh_Instance, ERR( us_err), pc_msg + strlen( pc_msg), 
										(WORD) (512 - 2 - strlen( pc_msg)));
		sprintf( pc_msg + strlen( pc_msg), "\" (%hx)", us_err);
		MessageBox( mh_mainWnd, pc_msg, mpc_APPNM, MB_ICONSTOP);
		us_err = mus_HANDLED_EXCEPTION;
	} //if (us_err &&

	return us_err + f_fail;
} //xus_InstallNotesComponents(


/** xus_RefreshPgpTemplateForms( ***
Refreshes the e-mail forms in the PGP Plug-In template using the counterpart 
forms stored in the specified Notes Mail template.

--- parameters & return ----
pc_EXTFILENM_MAIL_TEMPLATE: Pointer to the extended filename of the Notes 
	Mail template on the given server. The path component of the filename is 
	used to construct the required directory location of the PGP Plugin 
	Template database (thus the location to check for that template's 
	presence).
pc_SERVERNM: pointer to the name of the server on which the Notes Mail 
	template resides
pc_FILENM_PGP_TEMPLATE: pointer to the filename of the PGP Plugin Template 
	database, shorn of any path information as the template must reside in 
	the same directory as the specified Notes Mail template
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if obviously invalid parameters were passed in the procedure 
		call
	mus_EXCEPTION_HANDLED if an anticipated error condition was handled via a 
		message-box prompt 
	the Notes API error code otherwise

--- revision history -------
3/16/00 PR
+ adjustment to support change in how subforms are inserted in R5
+ safety improvement

9/20/99 PR: created			*/
WORD xus_RefreshPgpTemplateForms( char  pc_SERVERNM[], 
									char  pc_EXTFILENM_MAIL_TEMPLATE[], 
									char  pc_FILENM_PGP_TEMPLATE[])	{
	DBHANDLE  h_MailTemplate, h_PgpTemplate = NULLHANDLE;
	NOTEID  nid;
	STATUS  us_err, us_error;

	if (!( pc_SERVERNM && pc_EXTFILENM_MAIL_TEMPLATE && 
													pc_FILENM_PGP_TEMPLATE))
		return !eus_SUCCESS;

	if (us_err = us_OpenMailTemplate( pc_EXTFILENM_MAIL_TEMPLATE, 
										pc_SERVERNM, &h_MailTemplate, NULL))
		return us_err;
	if (!h_MailTemplate)
		return mus_HANDLED_EXCEPTION;

	//open the PGP plug-in template that should be in the same directory as 
	//	the mail template
	if (us_err = us_TestPgpTemplatePlaced( pc_FILENM_PGP_TEMPLATE, 
									pc_SERVERNM, pc_EXTFILENM_MAIL_TEMPLATE, 
									&h_PgpTemplate, NULL, NULL))
		goto errJump;
	if (!h_PgpTemplate)	{
		us_err = mus_HANDLED_EXCEPTION;
		goto errJump;
	}

	//confirm that the plug-in subform is present, then PGP-enable each of 
	//	the significant e-mail forms, copying them from the mail template 
	if (us_err = NIFFindDesignNote( h_PgpTemplate, mpc_SUBFORMNM_PGP, 
													NOTE_CLASS_FORM, &nid))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_MEMO, TRUE, 
												h_MailTemplate, 
												h_PgpTemplate, FALSE, NULL))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_REPLY, TRUE, 
												h_MailTemplate, 
												h_PgpTemplate, FALSE, NULL))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_REPLY_WH, TRUE, 
												h_MailTemplate, 
												h_PgpTemplate, FALSE, NULL))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_DEFAULT, FALSE, 
												h_MailTemplate, 
												h_PgpTemplate, FALSE, NULL))
		goto errJump;

errJump:
	if (h_PgpTemplate)
		if ((us_error = NSFDbClose( h_PgpTemplate)) && !us_err)
			us_err = us_error;
	if ((us_error = NSFDbClose( h_MailTemplate)) && !us_err)
		us_err = us_error;

	return us_err;
} //xus_RefreshPgpTemplateForms(


/** xus_UninstallNotesComponents( ***
Controls the removal of the PGP Notes Plugin according to the instructions 
provided by the caller. A complete removal process is two-stage, and the 
caller may choose to suppress either of the stages: (1) remove the plug-in 
from a specified set of Notes Mail databases and (2) delete the PGP Plugin 
Template from the specified Domino server.

--- parameters & return ----
pc_SERVERNM: pointer to the name of the server on which all the Notes Mail 
	databases reside
pc_EXTFILENM_PGP_TEMPLATE: Optional. Pointer to the extended filename of the 
	PGP Plugin Template database. Provision of string indicates that caller 
	wishes to delete the template from the given server. A pass of NULL 
	indicates that the template should not be removed.
pc_EXTFILENM_MAIL_TEMPLATE: Optional. Pointer to the extended filename of the 
	Notes Mail template from which the specified PGP-enabled Notes Mail 
	databases inherit their design. Required only if plug-in components are 
	to be removed from particular Notes Mail databases.
pc_EXTFILENM_DB_LIST: Optional. Pointer to a string indicating the extended 
	filename of a text file that contains the list of extended filenames of 
	the the Notes Mail databases from which the PGP Plug-In should be 
	removed. The filenames must be delimited by a pipe character or linefeed 
	character; whitespace surrounding each filename is ignored. A pass of 
	NULL indicates that any list of Notes Mail databases to be enabled is 
	provided via the pc_MailDbs parameter.
pc_MailDbs: Optional Input & Undefined Output. Pointer to a string indicating 
	the extended filenames of the particular Notes Mail databases from which 
	the plug-in is to be removed. Each filename must be delimited by a pipe 
	character or linefeed character; whitespace surrounding each filename is 
	ignored. String will be tokenized and therefore changed in a way not 
	worth defining (see documentation on standard strtok() function if 
	understanding is required). Parameter is ignored if the 
	pc_EXTFILENM_DB_LIST parameter is non-NULL. A pass of NULL indicates that 
	plug-in components need not be removed from any Notes Mail databases.
RETURN:
	eus_SUCCESS if no error occured
	mus_EXCEPTION_HANDLED if any Notes API or module-defined error occurs
	undefined error code if some sort of subsystem error occurs (memory 
		depletion, file system access denied, etc.)

--- revision history -------
8/9/00 PR: changed to the __stdcall calling convention to force the function 
	to clean up the stack itself
9/20/99 PR: created			*/
STATUS __stdcall xus_UninstallNotesComponents( 
										char  pc_SERVERNM[], 
										char  pc_EXTFILENM_PGP_TEMPLATE[], 
										char  pc_EXTFILENM_MAIL_TEMPLATE[], 
										const char  pc_EXTFILENM_DB_LIST[], 
										char  pc_MailDbs[])	{
	const BOOL  f_PGP_TEMPLATE = pc_EXTFILENM_PGP_TEMPLATE && 
												*pc_EXTFILENM_PGP_TEMPLATE, 
				f_DB_LIST = pc_EXTFILENM_DB_LIST && *pc_EXTFILENM_DB_LIST, 
				f_MAIL_DBS = pc_MailDbs && *pc_MailDbs;

	char  pc_netFileNmPgpTemplate[ MAXPATH];
	STATUS  us_err;
	BOOL  f_failure = FALSE;

	if (!mh_mainWnd)	{
		MessageBox( NULL, "PGPinstl.dll failed to attach to an apporpriate\n"
												"application parent window", 
												mpc_APPNM, MB_ICONSTOP);
		return !eus_SUCCESS;
	}

	if (!( pc_SERVERNM && (f_PGP_TEMPLATE || (f_DB_LIST || f_MAIL_DBS) && 
											pc_EXTFILENM_MAIL_TEMPLATE && 
											*pc_EXTFILENM_MAIL_TEMPLATE)))	{
		MessageBox( mh_mainWnd, "Incorrect parameters provided to\n"
											"xus_UninstallNotesComponents", 
											mpc_APPNM, MB_ICONSTOP);
		return !eus_SUCCESS;
	}

	//if requested, uninstall the PGP Notes Plug-In from the specified Notes 
	//	Mail databases
	if (f_DB_LIST || f_MAIL_DBS)	{
		DBHANDLE  h;
		char * pc_mereErrLog, pc[ NSF_INFO_SIZE];
		STATUS  us_error;

		//if we are able to open and validate the specified Notes Mail 
		//	template...
		if (us_err = eus_OpenMailTemplate( pc_EXTFILENM_MAIL_TEMPLATE, 
														pc_SERVERNM, &h, pc))
			return us_err;
		if (!h)
			return mus_HANDLED_EXCEPTION;

		//carry out the uninstall on all the specified databases
		us_err = us_RollBackMailDbs( h, pc_SERVERNM, pc, 
												pc_EXTFILENM_MAIL_TEMPLATE, 
												f_DB_LIST ? 
												pc_EXTFILENM_DB_LIST : NULL, 
												pc_MailDbs, &pc_mereErrLog);
		if ((us_error = NSFDbClose( h)) && !us_err)
			us_err = us_error;

		//if any non-fatal errors occurred during the uninstall, let the user 
		//	know about them
		if (pc_mereErrLog)	{
			FILE * pfl;

			if (f_failure = !( pfl = fopen( "PGPLog.txt", "wtc")))
				goto errJump;
			fprintf( pfl, pc_mereErrLog);
			fclose( pfl);	//failure ignored
			MessageBox( mh_mainWnd, f_DB_LIST ? "Some databases could "
									"not be processed. Refer to\nPGPLog.txt "
									"in the Notes Program directory for "
									"details." : pc_mereErrLog, 
									mpc_APPNM, MB_ICONEXCLAMATION);
			free( pc_mereErrLog);
		} //if (pc_mereErrLog)

		if (us_err || f_failure)
			goto errJump;
	} //if (f_DB_LIST || f_MAIL_DBS)

	//if requested, delete PGP template from server
	if (f_PGP_TEMPLATE)	{
//should verify that it is the plug-in template??
		OSPathNetConstruct( NULL, pc_SERVERNM, pc_EXTFILENM_PGP_TEMPLATE, 
													pc_netFileNmPgpTemplate);
		us_err = NSFDbDelete( pc_netFileNmPgpTemplate);
	}

errJump:
	if (us_err)	{
		char  pc_msg[ 512] = "An unexpected error occurred, translated by "
															"Notes as\n\"";

		OSLoadString( mh_Instance, ERR( us_err), pc_msg + strlen( pc_msg), 
										(WORD) (512 - 2 - strlen( pc_msg)));
		sprintf( pc_msg + strlen( pc_msg), "\" (%hx)", us_err);
		MessageBox( mh_mainWnd, pc_msg, mpc_APPNM, MB_ICONSTOP);

		us_err = mus_HANDLED_EXCEPTION;
	}

	return us_err + f_failure;
} //xus_UninstallNotesComponents(


/** us_OpenMailTemplate( ***
Attempts to open the specified Notes Mail template on the given server and 
validate that it is in fact a mail template. If the specified databae cannot 
be opened or validated for anticipated reasons, the user is informed of the 
findings via a message box.

--- parameters & return ----
pc_FILENM: pointer to the extended filename of the Notes Mail template to be 
	opened
ph: Pointer to the variable in which to store the handle of the database if 
	successfully opened and validated. Guaranteed to be NULL if not 
	successfully opened and validated.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
7/21/03 PR: minor logic (and signature) adjustment to maintain appropriate 
	constness
9/20/99 PR: created			*/
static STATUS us_OpenMailTemplate( const char  pc_FILENM[], 
									const char  pc_SERVERNM[], 
									DBHANDLE *const  ph, 
									char *const  pc_class)	{
	char  pc[ 0x400];
	STATUS  us_err;

	_ASSERTE( pc_FILENM && *pc_FILENM && pc_SERVERNM && ph);

	if (!( us_err = eus_OpenMailTemplate( (char *) pc_FILENM, (char *) 
													pc_SERVERNM, ph, pc_class)))
		return eus_SUCCESS;

	//if an anticipated error occurred in opening the Notes Mail template, 
	//	inform the user accordingly
	switch (ERR( us_err))	{
		case ERR_NOEXIST:	{
			const char *const  ppc_PART[] = {"The template \'", "\' was "
													"not located\non the "
													"server \'", "\'"};
			sprintf( pc, "%s%s%s%s%s", ppc_PART[0], pc_FILENM, ppc_PART[1], 
													pc_SERVERNM, ppc_PART[2]);
			break;

		}case ERR_NOACCESS:	{
			const char *const  ppc_PART[] = {"You do not have access to "
													"the\ntemplate \'", 
													"\' on server \'", "\'"};
			sprintf( pc, "%s%s%s%s%s", ppc_PART[0], pc_FILENM, ppc_PART[1], 
													pc_SERVERNM, ppc_PART[2]);
			break;

		}case ERR_DIRECTORY:	{
			const char *const  ppc_PART[] = {"The name \'", "\' is a "
										"directory name,\nnot a template "
										"on the server \'", "\'"};
			sprintf( pc, "%s%s%s%s%s", ppc_PART[0], pc_FILENM, ppc_PART[1], 
													pc_SERVERNM, ppc_PART[2]);
			break;

		}case mi_ERR_NOT_MAIL_DB:	{
			const char *const  ppc_PART[] = {"The database \'", "\' does "
													"not seem to be a Notes "
													"Mail\ntemplate "};
			sprintf( pc, "%s%s%s", ppc_PART[0], pc_FILENM, ppc_PART[1]);
			break;

		}case mi_ERR_NOT_TEMPLATE:	{
			const char *const  ppc_PART[] = {"A Notes Mail template "
										"database with a template design "
										"name is needed.\nCurrently \'", 
										"\' on server \'", "\' does not "
										"have\na template design name."};
			sprintf( pc, "%s%s%s%s%s", ppc_PART[0], pc_FILENM, ppc_PART[1], 
													pc_SERVERNM, ppc_PART[2]);
			break;

		}default:
			return us_err;
	} //switch (ERR( us_err))
	MessageBox( mh_mainWnd, pc, mpc_APPNM, MB_ICONSTOP);

	return eus_SUCCESS;
} //us_OpenMailTemplate(


/** us_RollBackMailDbs( ***
Removes PGP Plug-In components from the specified list of Notes Mail database 
and in conjunction refreshes the database's e-mail forms (Memo, Reply, Reply 
With History, (Default)) from the Notes Mail template which was specified by 
the user in the uninstall wizard.

--- parameters & return ----
h_TEMPLATE: handle to the Notes Mail template to be used in refreshing 
	each Notes Mail database from which the plug-in is to be removed
pc_SERVERNM: pointer to the name of the server on which all the Notes Mail 
	databases reside
pc_CLASS: Pointer to the class name associated with the Notes Mail template. 
	Serves as a protection against refreshing any Notes Mail database from 
	a template which does not correspond to its design-class name.
pc_EXTFILENM_MAIL_TEMPLATE: Pointer to the extended filename of the Notes 
	Mail template whose handle was passed. Utilized only for setting 
	environment information appropriately.
pc_EXTFILENM_DB_LIST: Optional. Pointer to a string indicating the extended 
	filename of a text file that contains the list of extended filenames of 
	the the Notes Mail databases from which the PGP Plug-In should be 
	removed. The filenames must be delimited by a pipe character or linefeed 
	character; whitespace surrounding each filename is ignored. A pass of 
	NULL indicates that any list of Notes Mail databases to be enabled is 
	provided via the pc_MailDbs parameter.
pc_MailDbs: Optional Input & Undefined Output. Pointer to a string indicating 
	the extended filenames of the particular Notes Mail databases to PGP 
	enable. Each filename must be delimited by a pipe character or linefeed 
	character; whitespace surrounding each filename is ignored. String will 
	be tokenized and therefore changed in a way not worth defining (see 
	documentation on standard strtok() function if understanding is 
	required). Parameter is ignored if the pc_EXTFILENM_DB_LIST parameter is 
	non-NULL. A pass of NULL indicates that no explicitly named Notes Mail 
	database are to be enabled by the procedure.
ppc_mereErrLog: Input & Output. Pointer to the variable that points to a 
	running log of non-fatal errors being tracked by the codestream. If this 
	procedure determines a database to be invalid for removal of the PGP 
	Plug-In, a description of the cause will be appended to the log.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if obviously invalid parameters were passed in the procedure 
		call
	ERR_MEMORY if an insufficient-memory condition was encountered
	the Notes API error code otherwise

--- revision history -------
9/20/99 PR: created			*/
static STATUS us_RollBackMailDbs( DBHANDLE  h_TEMPLATE, 
									char  pc_SERVERNM[], 
									char  pc_CLASS[], 
									char  pc_EXTFILENM_MAIL_TEMPLATE[], 
									const char  pc_EXTFILENM_DB_LIST[], 
									char * pc_MailDbs, 
									char * *const  ppc_mereErrLog)	{
	char * pc_db, * pc_log = NULL, * pc;
	DBHANDLE  h_db = NULL;
	EnvInfo  t_env;
	STATUS  us_err, us_error;
	BOOL  f_failure = FALSE;

	_ASSERTE( h_TEMPLATE && pc_SERVERNM && pc_CLASS && 
											(pc_EXTFILENM_DB_LIST || 
											pc_MailDbs) && ppc_mereErrLog);

	*ppc_mereErrLog = NULL;

	//Set environment information for the uninstall. Includes obtaining the 
	//	NoteIDs associated with the e-mail forms in the mail template which 
	//	will replace the PGP-enabled forms in the specified mail databases
	memset( &t_env, NULL, sizeof( EnvInfo));
	t_env.h_template = h_TEMPLATE;
	strcpy( t_env.pc_DbClass, pc_CLASS);
	if (us_err = us_GetSourceEnvInfo( NULL, pc_EXTFILENM_MAIL_TEMPLATE, 
																	&t_env))
		return us_err;

	//if the user specified the special text file of mail-database paths, 
	//	retrieve that file's contents into the mail-database input string
	if (pc_EXTFILENM_DB_LIST)
		if (f_failure = !f_LoadTextFileEntire( pc_EXTFILENM_DB_LIST, 
																&pc_MailDbs))
			goto errJump;

	//strip any trailing whitespace from the mail-database input string
	pc = pc_MailDbs + strlen( pc_MailDbs) - 1;
	while (*pc == ' ' || *pc == '\t')
		*(pc--) = NULL;

	//if there are any mail databases to update...
	if (pc_db = strtok( pc_MailDbs, mpc_DELIMITER))
		//for each mail database to update...
		do	{
			//determine the extended file name of the database
			while (*pc_db == ' ' || *pc_db == '\t')
				pc_db++;
			if (!*pc_db)
				continue;
			pc = pc_db + strlen( pc_db) - 1;
			while (*pc == ' ' || *pc == '\t')
				*(pc--) = NULL;
			if (!*pc_db)
				continue;

			if (us_err = us_OpenAndValidateMailDb( pc_db, pc_SERVERNM, 
															&pc_log, &h_db))
				goto errJump;
			if (h_db)	{
				if (us_err = us_RollBackMailDb( h_db, &t_env, pc_db, &pc_log))
					goto errJump;
				if (us_err = NSFDbClose( h_db))
					goto errJump;
				h_db = NULL;
			}
		} while (pc_db = strtok( NULL, mpc_DELIMITER));

	*ppc_mereErrLog = pc_log;

errJump:
	if (pc_EXTFILENM_DB_LIST)
		free( pc_MailDbs);

	if (h_db)
		if ((us_error = NSFDbClose( h_db)) && !( us_err || f_failure))
			us_err = us_error;

	return us_err + f_failure;
} //us_RollBackMailDbs(


/** us_GetSourceEnvInfo( ***
Fills the provided special EnvInfo structure with information commonly used 
in the PGP Notes Plug-In un/install routines.

--- parameters & return ----
pc_SERVERNM: Optional. Pointer to the name of the Domino server on which the 
	template to be opened resides. NULL pointer indicates that 
	template database has already been opened and its handle has been 
	provided in the appropriate member of the structure pointed to by the 
	pt_env parameter.
pc_EXTFILENM_TEMPLATE: Optional. Pointer to the name of the Notes template 
	database to be pumped for information.
pt_env: Output & Optional Input. Pointer to the structure to fill with 
	determined information about the installation environment. If no template 
	filename was specified, procedure expects the handle to the template 
	database to be provided in the h_template member of the structure.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
7/21/03 PR: minor logic (and signature) adjustment to maintain appropriate 
	constness
4/24/01 PR: removed remmed-out code from listing
2/23/00 PR: logic adjustment to allow finding of crucial design elements 
	by alias as well as main title
9/20/99 PR: genericized code so install & uninstall could share
2/6/99 PR: created			*/
static STATUS us_GetSourceEnvInfo( const char  pc_SERVERNM[], 
									const char  pc_EXTFILENM_TEMPLATE[], 
									EnvInfo *const  pt_env)	{
	DBHANDLE  h;
	STATUS  us_err;

	_ASSERTE( pt_env && (pc_SERVERNM ? pc_EXTFILENM_TEMPLATE : 
														pt_env->h_template));

	if (pc_SERVERNM && pc_EXTFILENM_TEMPLATE)	{
		if (us_err = eus_OpenTemplate( (char *) pc_EXTFILENM_TEMPLATE, (char *) 
										pc_SERVERNM, &h, pt_env->pc_DbClass))
			return us_err;
		pt_env->h_template = h;
	}else
		h = pt_env->h_template;

	if (us_err = SECKFMGetUserName( pt_env->pc_UserNm))
		goto errJump;

	if (us_err = NIFFindDesignNote( h, mpc_FORMNM_MEMO, NOTE_CLASS_FORM, 
														&pt_env->nid_Memo))
		goto errJump;
	if (us_err = NIFFindDesignNote( h, mpc_FORMNM_REPLY, NOTE_CLASS_FORM, 
														&pt_env->nid_Reply))
		goto errJump;
	if (us_err = NIFFindDesignNote( h, mpc_FORMNM_REPLY_WH, NOTE_CLASS_FORM, 
														&pt_env->nid_ReplyWH))
		goto errJump;
	if (us_err = NIFFindDesignNote( h, mpc_FORMNM_DEFAULT, NOTE_CLASS_FORM, 
														&pt_env->nid_Default))
		if (ERR( us_err) != ERR_NOT_FOUND)
			goto errJump;
		else
			pt_env->nid_Default = NULL;

	if (pc_EXTFILENM_TEMPLATE)
		strcpy( pt_env->pc_extFileNmTemplate, pc_EXTFILENM_TEMPLATE);

	return eus_SUCCESS;

errJump:
	if (pc_EXTFILENM_TEMPLATE)
		NSFDbClose( h);

	return us_err;
} //us_GetSourceEnvInfo(


/** us_InstallPlugInTemplate( ***
Conducts the creation/refresh of the PGP Notes Plugin Template based upon the 
encountered current Domino environment. If the PGP template already exists on 
the server being installed to, it is not replaced by the procedure; rather its 
design elements are refreshed to the latest plug-in design as contained in the 
installation package.

--- parameters & return ----
pc_SVRNM: address of the name of the Domino server on which the plug-in's 
	components are to be installed
pc_EXTFILENM_MAIL_TMPLT: address of the extended filename of the Notes 
	Mail template whose e-mail forms will be copied to populate/refresh the 
	PGP Plugin Template database
pc_FILENM_PGP_TMPLT: address of the filename of the PGP Plugin Template 
	database, shorn of any path information as the template must reside in 
	the same directory as the specified Notes Mail template
pt_env: Output. Address of the information structure to fill with details 
	determined about the installation environment.
RETURN:
	eus_SUCCESS if no error occured
	mus_EXCEPTION_HANDLED if particular errors occurred in template handling 
		(in which case the user has been informed of their nature by a 
		message-box prompt)
	the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ logic extension to support new DLL carrying plug-in subform
+ minor logic (and signature) adjustment to maintain appropriate constness
+ listing format adjustment, token renaming, minor documentation adjustment

3/16/00 PR: safety improvement, at least

9/20/99 PR
+ logic adjustment in conjunction with recast of install routine
+ completed standard documentation

1/22/99: created			*/
static STATUS us_InstallPlugInTemplate( const char  pc_SVRNM[], 
										const char  pc_EXTFILENM_MAIL_TMPLT[], 
										const char  pc_FILENM_PGP_TMPLT[], 
										EnvInfo *const  pt_env)	{
	const char  pc_PRFX_EXTFILENM_SRC_PGP_TMPLT[] = "INSTALL COPY--";

	char  pc_netFileNmPgpTmplt[ MAXPATH], 
			pc_extFileNmSrcPgpTmplt[ MAXPATH];
	DBHANDLE  h_MailTmplt, h_PgpTmplt = NULLHANDLE;
	BOOL  f_freshInstall;
	NOTEID  nid_main, nid_dll;
	STATUS  us_err, us_error, us_errDbDelete = eus_SUCCESS;

	_ASSERTE( pc_SVRNM && pc_EXTFILENM_MAIL_TMPLT && pt_env);

	memset( pt_env, NULL, sizeof( EnvInfo));

	if (us_err = us_OpenMailTemplate( pc_EXTFILENM_MAIL_TMPLT, pc_SVRNM, 
															&h_MailTmplt, NULL))
		return us_err;
	if (!h_MailTmplt)
		return mus_HANDLED_EXCEPTION;

	//if no PGP plug-in template is found in the same directory as the mail 
	//	template...
	if (us_err = us_TestPgpTemplatePlaced( pc_FILENM_PGP_TMPLT, pc_SVRNM, 
									pc_EXTFILENM_MAIL_TMPLT, &h_PgpTmplt, 
									pt_env->pc_DbClass, pc_netFileNmPgpTmplt))
		goto errJump;
	strcat( strcpy( pc_extFileNmSrcPgpTmplt, pc_PRFX_EXTFILENM_SRC_PGP_TMPLT), 
														pc_FILENM_PGP_TMPLT);
	if (us_err = SECKFMGetUserName( pt_env->pc_UserNm))
		goto errJump;
	if (f_freshInstall = !h_PgpTmplt)	{
		 char * pc = strrchr( pc_netFileNmPgpTmplt, ec_PATH_SPECIFIER);

		//copy the template into that directory
		if (pc)
			pc_netFileNmPgpTmplt[ pc - pc_netFileNmPgpTmplt + 1] = NULL;
		else
			OSPathNetConstruct( NULL, (char *) pc_SVRNM, (char *) epc_NULL, 
														pc_netFileNmPgpTmplt);
		if (us_err = us_PlacePgpTemplate( pc_extFileNmSrcPgpTmplt, 
											strcat( pc_netFileNmPgpTmplt, 
											pc_FILENM_PGP_TMPLT), 
											pt_env->pc_UserNm, h_MailTmplt, 
											pc_EXTFILENM_MAIL_TMPLT, 
											&h_PgpTmplt, pt_env->pc_DbClass))
			goto errJump;

		//get the NoteID of the PGP subform design notes
		if (us_err = NIFFindDesignNote( h_PgpTmplt, mpc_SUBFORMNM_PGP, 
													NOTE_CLASS_FORM, &nid_main))
			goto errJump;
		if (us_err = NIFFindDesignNote( h_PgpTmplt, mpc_SUBFORMNM_PGP_DLL, 
													NOTE_CLASS_FORM, &nid_dll))
			goto errJump;
	//Else replace the design elements in the existing template with the 
	//	design elements in the template just copied in by the installation 
	//	process. This approach allows us to refresh the PGP installation 
	//	without disturbing the existing ACL or Replica ID.
	}else
		if (us_err = us_RefreshPgpTemplate( h_PgpTmplt, 
													pc_extFileNmSrcPgpTmplt, 
													pc_EXTFILENM_MAIL_TMPLT, 
													&nid_main, &nid_dll))
			goto errJump;

	//delete the local, source template database which carried in the latest 
	//	version of the PGP e-mail subform
#ifdef NDEBUG
	us_errDbDelete = NSFDbDelete( pc_extFileNmSrcPgpTmplt);
#endif

	//PGP-enable each of the significant e-mail forms, copying them from the 
	//	mail template 
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_MEMO, TRUE, h_MailTmplt, 
													h_PgpTmplt, f_freshInstall, 
													&pt_env->nid_Memo))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_REPLY, TRUE, h_MailTmplt, 
													h_PgpTmplt, f_freshInstall, 
													&pt_env->nid_Reply))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_REPLY_WH, TRUE, h_MailTmplt, 
													h_PgpTmplt, f_freshInstall, 
													&pt_env->nid_ReplyWH))
		goto errJump;
	if (us_err = us_PgpEnableEmailForm( mpc_FORMNM_DEFAULT, FALSE, h_MailTmplt, 
													h_PgpTmplt, f_freshInstall, 
													&pt_env->nid_Default))
		goto errJump;

	pt_env->h_template = h_PgpTmplt;
	pt_env->nid_PgpSbfrmMain = nid_main;
	pt_env->nid_PgpSbfrmDll = nid_dll;
	OSPathNetParse( pc_netFileNmPgpTmplt, NULL, NULL, 
												pt_env->pc_extFileNmTemplate);

errJump:
	if ((us_error = NSFDbClose( h_MailTmplt)) && !us_err)
		us_err = us_error;

	return us_err ? us_err : us_errDbDelete;
} //us_InstallPlugInTemplate(


/** us_PgpEnableEmailForm( ***

--- parameters & return ------

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- suggested enhancement ----
2/23/00 PR: Should handle error where PGP subform is already at the top of the 
	organization's mail template with user-friendly message box. It's quite 
	possible that some misguided administrator might decide to PGP enable the 
	mail template instead of sticking with the paradigm we've constructed.

--- revision history ---------
7/21/03 PR: listing format adjustment, minor documentation adjustment
7/2/01 PR: removed remmed-out code
3/16/00 PR: adjustment to support change in how subforms are inserted in R5

2/23/00 PR
+ to prevent "corrupted note" error message in Notes 4.5.1 client, added 
  OPEN_EXPAND flag when opening notes to be signed
+ logic adjustment to allow finding of crucial design elements by alias as 
  well as main title

9/20/99 PR: function signature change to promote efficiency
2/8/99 PR: created			*/
static STATUS us_PgpEnableEmailForm( char  pc_FORMNM[], 
										const BOOL  f_REQUIRED, 
										DBHANDLE  h_MAIL_TEMPLATE, 
										DBHANDLE  h_PGP_TEMPLATE, 
										const BOOL  f_FRESH_INSTALL, 
										NOTEID *const  pnid_form)	{
	NOTEID  nid_src, nid;
	NOTEHANDLE  h;
	STATUS  us_err;

	_ASSERTE( pc_FORMNM && *pc_FORMNM && h_MAIL_TEMPLATE && 
										h_PGP_TEMPLATE && h_MAIL_TEMPLATE != 
										h_PGP_TEMPLATE /*&& h_PGP_SUBFORM*/); 

	if (pnid_form)
		*pnid_form = NULL;

	//copy the form into the PGP design-element database, replacing an 
	//	existing copy if necessary
	if (us_err = NIFFindDesignNote( h_MAIL_TEMPLATE, pc_FORMNM, 
												NOTE_CLASS_FORM, &nid_src))
		return ERR( us_err) == ERR_NOT_FOUND && !f_REQUIRED ? 
													eus_SUCCESS : us_err;
	if (!f_FRESH_INSTALL)	{
		if (us_err = NIFFindDesignNote( h_PGP_TEMPLATE, pc_FORMNM, 
													NOTE_CLASS_FORM, &nid))	{
			if (ERR( us_err) != ERR_NOT_FOUND)
				return us_err;
		}else
			if (us_err = NSFNoteDelete( h_PGP_TEMPLATE, nid, NULL))
				return us_err;
	}
	if (us_err = NSFDbCopyNote( h_MAIL_TEMPLATE, NULL, NULL, nid_src, 
									h_PGP_TEMPLATE, NULL, NULL, &nid, NULL))
		return us_err;

	//Insert the PGP subform at the top of the form. Although at some point 
	//	later versions do not have the restriction, the Notes 4.5.1 client 
	//	will successfully verify design-note signatures only when the design 
	//	note was explicitly opened with the "expand" flag before being signed.
	if (us_err = NSFNoteOpen( h_PGP_TEMPLATE, nid, OPEN_EXPAND, &h))
		return us_err;
	if (us_err = us_InsertSubformTop( h, mpc_SUBFORMNM_PGP))
		goto errJump;

	//If this is the Reply With History form, remove any form aliases as a 
	//	work-around to a nettlesome Notes bug. When a Design Refresh is 
	//	invoked for a database and a particular form design element 
	//	inherits from a template different from the database's default 
	//	template, Notes searches for the form element by its *alias*, not by 
	//	its first name. The alias for the Reply With History form is "Reply", 
	//	so if no workaround were provided here, Notes would replace the 
	//	Reply With History form with a duplicate Reply form, thus breaking 
	//	the mail databae, effectively. So here we remove the alias, and we 
	//	rely on the smarts of the PGP E-Mail subform to reproduce the 
	//	form's alias functionality.
	if (strcmp( pc_FORMNM, mpc_FORMNM_REPLY_WH) == ei_SAME)	{
		const WORD  US = strlen( mpc_FORMNM_REPLY_WH);

		char  pc[ MAXBYTE];
		WORD  us;

		if (!( us = NSFItemGetTextListEntry( h, FIELD_TITLE, 0, pc, MAXBYTE)))
			goto errJump;
		if (us > MAXBYTE - US)	{
			us_err = !eus_SUCCESS;
			goto errJump;
		}

		if (us_err = eus_RemoveItem( h, FIELD_TITLE, ebid_NULLBLOCKID))
			goto errJump;
		if (us_err = NSFItemAppendTextList( h, FIELD_TITLE, pc, us, TRUE))
			goto errJump;
		if (us_err = NSFItemAppendTextList( h, FIELD_TITLE, 
											mpc_FORMNM_REPLY_WH, US, TRUE))
			goto errJump;
	} //if (strcmp( pc_FORMNM, 

	//sign and save the form
	if (us_err = NSFNoteSign( h))
		goto errJump;
	if (us_err = NSFNoteUpdate( h, NULL))
		goto errJump;

	if (pnid_form)
		*pnid_form = nid;

errJump:
	NSFNoteClose( h);

	return us_err;
} //us_PgpEnableEmailForm(


/** us_RefreshMailDbs( ***
PGP refreshes/enables the specified Notes Mail databases with the latest design 
already deposited in the PGP Plugin Template.

--- parameters & return ----
pc_SVRNM: address of the name of the server on which all the Notes Mail 
	databases reside
pc_EXTFILENM_DB_LIST: Optional. Address of the extended filename of the text 
	file holding the list of extended filenames of the particular Notes Mail 
	databases to be refreshed/enabled. Each filename must be delimited by a 
	pipe character or linefeed character; whitespace surrounding each filename 
	is ignored. A pass of NULL or pointer to a null string indicates that no 
	text file is involved.
pc_MailDbs: Optional Input & Undefined Output. Address of a string indicating 
	the extended filenames of the particular Notes Mail databases to be 
	refreshed/enabled. Each filename must be delimited by a pipe character or 
	linefeed character; whitespace surrounding each filename is ignored. String 
	may be tokenized and therefore changed in a way not worth defining here 
	(see documentation on standard strtok() function if understanding is 
	required). A pass of NULL or pointer to a null string indicates that no 
	_explicit_ list of database filenames is involved.
pt_ENV: address of the information structure describing the installation 
	environment, information about the Notes Mail template in particular
ppc_mereErrLog: Input & Output. Address of the variable that points to a 
	running log of non-fatal errors being tracked by the codestream. If this 
	procedure determines a database to be invalid for refresh/enablement of the 
	PGP Plug-In, a description of the cause will be appended to the log.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if obviously invalid parameters were passed in the procedure 
		call
	ERR_MEMORY if an insufficient-memory condition was encountered
	the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ minor logic (and signature) adjustment to maintain appropriate constness
+ listing format adjustment, minor documentation adjustment, token renaming

9/20/99 PR
+ adjustment to allow any text-file filename to be used to provide the list 
  of Notes Mail databases to be refreshed/enabled
+ completed standard documentation

2/6/99 PR: created			*/
static STATUS us_RefreshMailDbs( const char  pc_SVRNM[], 
									const char  pc_EXTFILENM_DB_LIST[], 
									char  pc_MailDbs[], 
									const EnvInfo *const  pt_ENV, 
									char * *const  ppc_mereErrLog)	{
	char  * pc_db, * pc;
	DBHANDLE  h_db = NULL;
	UINT  ui;
	STATUS  us_err, us_error = eus_SUCCESS;
	BOOL  f_fail = FALSE;

	_ASSERTE( pc_SVRNM && (pc_EXTFILENM_DB_LIST || pc_MailDbs) && pt_ENV && 
										pt_ENV->h_template && 
										pt_ENV->pc_extFileNmTemplate && 
										pt_ENV->pc_UserNm && ppc_mereErrLog);

	*ppc_mereErrLog = NULL;

	//if the PGP template does not have a design class specified, log the 
	//	condition and short-circuit without refreshing any databases
	if (!*pt_ENV->pc_DbClass)	{
		const char *const  ppc_PART[] = {"To PGP Enable a mail database, "
									"the PGP Plugin Template\ndatabase must "
									"have a template design name.\n"
									"Currently \'", "\'on server\n\'", "\' "
									"does not have a template design name."};

		if (!f_StartMereErrLog( ppc_mereErrLog))
			return ERR_MEMORY;
		if (!( pc = realloc( *ppc_mereErrLog, (ui = strlen( *ppc_mereErrLog)) + 
									strlen( ppc_PART[0]) + strlen( 
									ppc_PART[1]) + strlen( ppc_PART[2]) + 1)))
			return ERR_MEMORY;
		sprintf( (*ppc_mereErrLog = pc) + ui, "\n%s%s%s%s%s", ppc_PART[0], 
											pt_ENV->pc_extFileNmTemplate, 
											ppc_PART[1], pc_SVRNM, ppc_PART[2]);

		return eus_SUCCESS;
	} //if (!*pt_ENV->pc_DbClass)

	//if the user specified the special text file of mail-database paths, 
	//	retrieve that file's contents into the mail-database input string
	if (pc_EXTFILENM_DB_LIST)
		if (!f_LoadTextFileEntire( pc_EXTFILENM_DB_LIST, &pc_MailDbs))
			return !eus_SUCCESS;

	//strip any trailing whitespace from the mail-database input string
	pc = pc_MailDbs + strlen( pc_MailDbs) - 1;
	while (*pc == ' ' || *pc == '\t')
		*(pc--) = NULL;

	//if there are any mail databases to update...
	if (pc_db = strtok( pc_MailDbs, mpc_DELIMITER))
		//for each mail database to update...
		do	{
			//determine the extended file name of the database
			while (*pc_db == ' ' || *pc_db == '\t')
				pc_db++;
			if (!*pc_db)
				continue;
			pc = pc_db + strlen( pc_db) - 1;
			while (*pc == ' ' || *pc == '\t')
				*(pc--) = NULL;
			if (!*pc_db)
				continue;

			if (us_err = us_OpenAndValidateMailDb( pc_db, pc_SVRNM, 
														ppc_mereErrLog, &h_db))
				goto errJump;
			if (h_db)	{
				if (us_err = us_RefreshMailDb( h_db, pt_ENV))
					goto errJump;
				if (us_err = NSFDbClose( h_db))
					goto errJump;
				h_db = NULL;
			}
		} while (pc_db = strtok( NULL, mpc_DELIMITER));

errJump:
	if (pc_EXTFILENM_DB_LIST)
		free( pc_MailDbs);

	if (h_db)
		if ((us_error = NSFDbClose( h_db)) && !( us_err || f_fail))
			us_err = us_error;

	return us_err + f_fail;
} //us_RefreshMailDbs(


/** us_OpenAndValidateMailDb( ***
Attempts to open the named database and validates that the user has a minimum 
of designer access to the database. If the open fails for anticipated 
reasons, or the validation fails, the problem is noted and success is 
returned.

--- parameters & return ----
pc_FILENM: pointer to the extended filename of the Notes database to be 
	opened
pc_SERVERNM: pointer to the name of the Domino server on which the database 
	to be opened resides
ppc_mereErrLog: Input & Output. Pointer to the variable that points to a 
	running log of non-fatal errors being tracked by the codestream. If this 
	procedure determines the specified database to be invalid, a description 
	of the cause will be appended to the log.
ph: Pointer to the variable in which to store the handle of the database if 
	successfully opened and validated. Guaranteed to be NULL if not 
	successfully opened and validated.
RETURN:
	eus_SUCCESS if no error occured
	ERR_MEMORY if an insufficient-memory condition was encountered
	the Notes API error code otherwise

--- revision history -------
7/21/03 PR: minor logic (and signature) adjustment to maintain appropriate 
	constness
9/20/99 PR: created			*/
static STATUS us_OpenAndValidateMailDb( const char  pc_EXTFILENM[], 
										const char  pc_SERVERNM[], 
										char * *const  ppc_mereErrLog, 
										DBHANDLE *const  ph)	{
	static const char *const  ppc_DB_NO_EXIST[] = {"+ The database \'", "\' "
													"was not located\n   on "
													"the server \'", "\'"}, 
						*const  ppc_DB_IS_DIR[] = {"+ The name \'", "\' is "
										"a directory name,\n   not a "
										"database on the server \'", "\'"}, 
						*const  ppc_NOT_MAIL_DB[] = {"+ The database \'", 
												"\' does not seem to be a\n"
												"   Notes Mail database"}, 
						*const  ppc_NOT_DESIGNR[] = {"+ You do not have "
												"required Designer access "
												"to the\n   database \'", 
												"\' on server \'", "\'"};

	static UINT  ui_lenDbNoExist, ui_lenDbIsDir, ui_lenNotMailDb, 
					ui_lenNotDesigner;

	DBHANDLE  h_db;
	char  * pc, * pc_log;
	UINT  ui;
	WORD  us, us_dmy;
	STATUS  us_err;

	_ASSERTE( pc_EXTFILENM && *pc_EXTFILENM && pc_SERVERNM && 
														ppc_mereErrLog && ph);

	*ph = NULL;

	if (!ui_lenDbNoExist)	{
		ui_lenDbNoExist = strlen( ppc_DB_NO_EXIST[0]) + 
											strlen( ppc_DB_NO_EXIST[1]) + 
											strlen( ppc_DB_NO_EXIST[2]) + 1;
		ui_lenDbIsDir = strlen( ppc_DB_IS_DIR[0]) + 
												strlen( ppc_DB_IS_DIR[1]) + 
												strlen( ppc_DB_IS_DIR[2]) + 1;
		ui_lenNotMailDb = strlen( ppc_NOT_MAIL_DB[0]) + strlen( 
													ppc_NOT_MAIL_DB[1]) + 1;
		ui_lenNotDesigner = strlen( ppc_NOT_DESIGNR[0]) + 
											strlen( ppc_NOT_DESIGNR[1]) + 
											strlen( ppc_NOT_DESIGNR[2]) + 1;
	} //if (!ui_lenDbNoExist)


	//Try to open the target database. If unsuccessful, check whether the 
	//	error is fatal or not. If not fatal, log a description of the 
	//	problematic condition.
	pc_log = *ppc_mereErrLog;
	if (us_err = eus_OpenMailDb( (char *) pc_EXTFILENM, (char *) pc_SERVERNM, 
																	&h_db))	{
		//if one of our handled errors occurred, run common prep code
		switch (ERR( us_err))
			case ERR_NOEXIST:
			case ERR_NOACCESS:
			case ERR_DIRECTORY:
			case mi_ERR_NOT_MAIL_DB:
				if (!pc_log)
					if (!f_StartMereErrLog( &pc_log))
						return ERR_MEMORY;

		switch (ERR( us_err))	{
			//if the database doesn't exist, log the condition and skip back 
			//	to the caller
			case ERR_NOEXIST:
				if (!( pc = realloc( pc_log, (ui = strlen( pc_log)) + 
												ui_lenDbNoExist + 
												strlen( pc_EXTFILENM) + 
												strlen( pc_SERVERNM) + 1)))
					goto errJump;
				sprintf( (pc_log = pc) + ui, "\n%s%s%s%s%s", 
											ppc_DB_NO_EXIST[0], pc_EXTFILENM, 
											ppc_DB_NO_EXIST[1], pc_SERVERNM, 
											ppc_DB_NO_EXIST[2]);
				break;

			//if the current ID does not have access to the database, log the 
			//	condition and skip back to the caller
			case ERR_NOACCESS:
				if (!( pc = realloc( pc_log, (ui = strlen( pc_log)) + 
												ui_lenNotDesigner + strlen( 
												pc_EXTFILENM) + strlen( 
												pc_SERVERNM) + 1)))
					goto errJump;
				sprintf( (pc_log = pc) + ui, "\n%s%s%s%s%s", 
											ppc_NOT_DESIGNR[0], pc_EXTFILENM, 
											ppc_NOT_DESIGNR[1], pc_SERVERNM, 
											ppc_NOT_DESIGNR[2]);
				break;

			//if the "database" is really just a subdirectory path, log the 
			//	condition and skip back to the caller
			case ERR_DIRECTORY:
				if (!( pc = realloc( pc_log, (ui = strlen( pc_log)) + 
												ui_lenDbIsDir + 
												strlen( pc_EXTFILENM) + 
												strlen( pc_SERVERNM) + 1)))
					goto errJump;
				sprintf( (pc_log = pc) + ui, "\n%s%s%s%s%s", 
											ppc_DB_IS_DIR[0], pc_EXTFILENM, 
											ppc_DB_IS_DIR[1], pc_SERVERNM, 
											ppc_DB_IS_DIR[2]);
				break;

			//if the database doesn't seem to be a Notes Mail database, log 
			//	the condition and skip back to the caller
			case mi_ERR_NOT_MAIL_DB:
				if (!( pc = realloc( pc_log, (ui = strlen( pc_log)) + 
												ui_lenNotMailDb + 
												strlen( pc_EXTFILENM) + 1)))
					goto errJump;
				sprintf( (pc_log = pc) + ui, "\n%s%s%s", 
											ppc_NOT_MAIL_DB[0], pc_EXTFILENM, 
											ppc_NOT_MAIL_DB[1]);
				break;

			default:
				return us_err;
		} //switch (ERR( us_err))

		*ppc_mereErrLog = pc_log;
		return eus_SUCCESS;
	} //if (us_err = eus_OpenMailDb( pc_EXTFILENM, 

	//if the current user ID does not have designer access to the database...
	NSFDbAccessGet( h_db, &us, &us_dmy);
	if (us < ACL_LEVEL_DESIGNER)	{
		//log the condition
		if (!pc_log)
			if (!f_StartMereErrLog( &pc_log))
				goto errJump;
		if (!( pc = realloc( pc_log, (ui = strlen( pc_log)) + 
												ui_lenNotDesigner + 
												strlen( pc_EXTFILENM) + 
												strlen( pc_SERVERNM) + 1)))
			goto errJump;
		sprintf( (pc_log = pc) + ui, "\n%s%s%s%s%s", ppc_NOT_DESIGNR[0], 
											pc_EXTFILENM, ppc_NOT_DESIGNR[1], 
											pc_SERVERNM, ppc_NOT_DESIGNR[2]);
	//else we've met with success, so return the handle to the database to 
	//	be processed further by the caller
	}else
		*ph = h_db;

	*ppc_mereErrLog = pc_log;
	return eus_SUCCESS;

errJump:
	//we've run out of memory, so back out safely
	if (pc_log)	{
		free( pc_log);
		*ppc_mereErrLog = NULL;
	}
	if (h_db)
		NSFDbClose( h_db);

	return ERR_MEMORY;
} //us_OpenAndValidateMailDb(


/** us_RefreshMailDb( ***

--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ logic extension to support addition of DLL carrying plug-in subform
+ token renaming, listing format adjustment

9/20/99 PR: logic adjustment to conform to downstream changes
2/6/99 PR: created			*/
//DOC!!
static STATUS us_RefreshMailDb( const DBHANDLE  h_TRGT, 
								const EnvInfo *const  pt_ENV)	{
	HANDLE  h_src;
	const char * pc;
	STATUS  us_err;

	_ASSERTE( h_TRGT && pt_ENV && pt_ENV->h_template && pt_ENV->pc_DbClass && 
								pt_ENV->nid_PgpSbfrmMain && 
								pt_ENV->nid_PgpSbfrmDll && pt_ENV->nid_Memo && 
								pt_ENV->nid_Reply && pt_ENV->nid_ReplyWH);

	if (us_err = eus_ReplaceDesignNote( h_TRGT, NOTE_CLASS_FORM, 
											mpc_SUBFORMNM_PGP, 
											h_src = pt_ENV->h_template, 
											pt_ENV->nid_PgpSbfrmMain, (char *) 
											(pc = pt_ENV->pc_DbClass), NULL))
		return us_err;
	if (us_err = eus_ReplaceDesignNote( h_TRGT, NOTE_CLASS_FORM, 
									mpc_SUBFORMNM_PGP_DLL, h_src, 
									pt_ENV->nid_PgpSbfrmDll, (char *) pc, NULL))
		return us_err;
	if (us_err = eus_ReplaceDesignNote( h_TRGT, NOTE_CLASS_FORM, 
										mpc_FORMNM_MEMO, h_src, 
										pt_ENV->nid_Memo, (char *) pc, NULL))
		return us_err;
	if (us_err = eus_ReplaceDesignNote( h_TRGT, NOTE_CLASS_FORM, 
										mpc_FORMNM_REPLY, h_src, 
										pt_ENV->nid_Reply, (char *) pc, NULL))
		return us_err;
	if (us_err = eus_ReplaceDesignNote( h_TRGT, NOTE_CLASS_FORM, 
										mpc_FORMNM_REPLY_WH, h_src, 
										pt_ENV->nid_ReplyWH, (char *) pc, NULL))
		return us_err;
	if (pt_ENV->nid_Default)
		us_err = eus_ReplaceDesignNote( h_TRGT, NOTE_CLASS_FORM, 
										mpc_FORMNM_DEFAULT, h_src, 
										pt_ENV->nid_Default, (char *) pc, NULL);

	return us_err;
} //us_RefreshMailDb(


/** us_RollBackMailDb( ***
Removes PGP Plug-In components from the specified Notes Mail database and in 
conjunction refreshes the database's e-mail forms (Memo, Reply, Reply With 
History, (Default)) from the Notes Mail template which was specified by the 
user in the uninstall wizard.

--- parameters & return ----
H: handle to the Notes Mail database from which the PGP Plug-In should be 
	removed
pt_ENV: address of the information structure describing the installation 
	environment, information about the Notes Mail template in particular
pc_EXTFILENM: Pointer to the extended filename of the Notes Mail database being 
	processed. Only used in non-fatal error logging.
ppc_mereErrLog: Input & Output. Pointer to the non-fatal error log currently 
	underway. Log will be appended to if the design-class name of the mail 
	database being processed does not correspond to the class name of the mail 
	template database.
RETURN:
	eus_SUCCESS if no error occured
	!eus_SUCCESS if obviously invalid parameters were passed in the procedure 
		call
	ERR_MEMORY if an insufficient-memory condition was encountered
	the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ extended to support removal now also of the DLL carrying plug-in subform
+ listing format adjustment, minor documentation adjustment

9/20/99 PR: created			*/
static STATUS us_RollBackMailDb( const DBHANDLE  H, 
									const EnvInfo *const  pt_ENV, 
									const char  pc_EXTFILENM[], 
									char * *const  ppc_mereErrLog)	{
	char  pc_class[ NSF_INFO_SIZE];
	DBHANDLE  h_src;
	STATUS  us_err;

	_ASSERTE( H && pt_ENV && pt_ENV->h_template && pt_ENV->nid_Memo && 
									pt_ENV->nid_Reply && pt_ENV->nid_ReplyWH && 
									pc_EXTFILENM && ppc_mereErrLog);

	//if the design class of the mail database does not correspond to the class 
	//	name held by the mail template, log the non-fatal condition and 
	//	short-circuit
	if (us_err = eus_getDbDesignClass( H, pc_class))
		return us_err;
	if (stricmp( pc_class, pt_ENV->pc_DbClass) != ei_SAME)	{
		static const char *const  ppc_PART[] = {"+ The design-class \'", "\' "
									"for mail database\n   \'", "\' differs "
									"from the class name\n   \'", "\' of the "
									"Notes Mail template \'", "\'.\n   For "
									"safety, the PGP Plugin can be removed "
									"only if the\n   database's design-class "
									"name corresponds to the class\n   name "
									"of the mail template."};

		static UINT  ui_lenParts;

		char * pc;
		UINT  ui;

		if (!ui_lenParts)
			ui_lenParts = strlen( ppc_PART[0]) + strlen( ppc_PART[1]) + 
										strlen( ppc_PART[2]) + strlen( 
										ppc_PART[3]) + strlen( ppc_PART[4]) + 1;

		if (!*ppc_mereErrLog)
			if (!f_StartMereErrLog( ppc_mereErrLog))
				return ERR_MEMORY;
		if (!( pc = realloc( *ppc_mereErrLog, (ui = strlen( *ppc_mereErrLog)) + 
										ui_lenParts + strlen( pc_class) + 
										strlen( pc_EXTFILENM) + strlen( 
										pt_ENV->pc_DbClass) + strlen( 
										pt_ENV->pc_extFileNmTemplate))))	{
			free( *ppc_mereErrLog);
			*ppc_mereErrLog = NULL;
			return ERR_MEMORY;
		}
		sprintf( (*ppc_mereErrLog = pc) + ui, "\n%s%s%s%s%s%s%s%s%s", 
									ppc_PART[0], pc_class, ppc_PART[1], 
									pc_EXTFILENM, ppc_PART[2], 
									pt_ENV->pc_DbClass, ppc_PART[3], 
									pt_ENV->pc_extFileNmTemplate, ppc_PART[4]);

		return eus_SUCCESS;
	} //if (stricmp( pc_class, 

	//replace the Reply, Memo, Reply With History and, if needed, (Default) 
	//	forms in the mail database with their counterparts in the mail 
	//	template
	if (us_err = eus_ReplaceDesignNote( H, NOTE_CLASS_FORM, mpc_FORMNM_MEMO, 
												h_src = pt_ENV->h_template, 
												pt_ENV->nid_Memo, NULL, NULL))
		return us_err;
	if (us_err = eus_ReplaceDesignNote( H, NOTE_CLASS_FORM, mpc_FORMNM_REPLY, 
										h_src, pt_ENV->nid_Reply, NULL, NULL))
		return us_err;
	if (us_err = eus_ReplaceDesignNote( H, NOTE_CLASS_FORM, 
											mpc_FORMNM_REPLY_WH, h_src, 
											pt_ENV->nid_ReplyWH, NULL, NULL))
		return us_err;
	if (pt_ENV->nid_Default)
		us_err = eus_ReplaceDesignNote( H, NOTE_CLASS_FORM, 
											mpc_FORMNM_DEFAULT, h_src, 
											pt_ENV->nid_Default, NULL, NULL);

	//remove all PGP Plug-In subforms from the mail database
	if (us_err = eus_RemoveDesignNote( H, NOTE_CLASS_FORM, 
														mpc_SUBFORMNM_PGP_DLL))
		return us_err;
	return eus_RemoveDesignNote( H, NOTE_CLASS_FORM, mpc_SUBFORMNM_PGP);
} //us_RollBackMailDb(


/** f_LoadTextFileEntire( ***

--- parameters & return ----

--- revision history -------
7/21/03 PR: listing format adjustment, minor documentation adjustment
2/6/99 PR: created			*/
//DOC!!
static BOOL f_LoadTextFileEntire( const char  pc_EXTFILENM[], 
								   char * *const  ppc)	{
	FILE * pfl;
	int  i;
	char * pc = NULL;
	BOOL  f_failure;

	_ASSERTE( pc_EXTFILENM && ppc);

	*ppc = NULL;

	if (!( pfl = fopen( pc_EXTFILENM, "rt")))
		return FALSE;

	if (f_failure = (i = filelength( fileno( pfl))) < 0)
		goto errJump;

	if (i)	{
		if (f_failure = !( pc = malloc( i + 1)))
			goto errJump;

		i = fread( pc, 1, i, pfl);
		if (f_failure = ferror( pfl))
			goto errJump;
		pc[ i] = NULL;
	} //if (i)

errJump:
	if (fclose( pfl) == EOF)
		if (!f_failure)
			f_failure = TRUE;

	if (pc)
		if (!f_failure)
			*ppc = pc;
		else 
			free( pc);

	return !f_failure;
} //f_LoadTextFileEntire(


/** f_StartMereErrLog( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
1/24/99 PR: created			*/
//DOC!!
static __inline BOOL f_StartMereErrLog( char * *const  ppc_mereErrLog)	{
	const char  pc_MERE_ERR_LOG_START[] = "The following issues occurred "
												"with respect to the\nPGP "
												"Lotus Notes Plugin:";

	_ASSERTE( ppc_mereErrLog);

	*ppc_mereErrLog = NULL;

	if (!( *ppc_mereErrLog = malloc( sizeof( pc_MERE_ERR_LOG_START))))
		return FALSE;

	strcpy( *ppc_mereErrLog, pc_MERE_ERR_LOG_START);

	return TRUE;
} //f_StartMereErrLog(


/** us_InsertSubformTop( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/16/00 PR: adjusted to handle way R5 subform insertion must be done to avoid 
	duplicate actions showing in the user-interface
2/23/00 PR: minor token renaming
1/15/99 PR: created			*/
//DOC!!
static STATUS us_InsertSubformTop( NOTEHANDLE  h_FORM, 
									char  pc_SUBFORMNM[])	{
	const WORD  us_MAX_STYLE_ID = 0xFFFF;

	RtfContext  t_RtfContextForm/*, t_RtfContextSubform*/;
	BOOL  f_found/*, f_SubformRtfContextInitialized = FALSE*/;
	STATUS  us_err;

	_ASSERTE( h_FORM && pc_SUBFORMNM && *pc_SUBFORMNM);

	//if the subform is already included in the form, short-circuit with 
	//	failure
	if (us_err = eus_TestTextListItemHasEntry( h_FORM, SUBFORM_ITEM_NAME, 
													pc_SUBFORMNM, &f_found))
		return us_err;
	if (f_found)
		return !eus_SUCCESS;

	//initialize resources associated with the rich-text handling we're going 
	//	to do with the form
	if (us_err = eus_InitializeRtfContext( h_FORM, ITEM_NAME_TEMPLATE, 
													NULL, &t_RtfContextForm))
		return us_err;

	if (us_err = eus_InsertSubformTop( &t_RtfContextForm, pc_SUBFORMNM))
		goto errJump;

	//commit changes to the form's rich-text field to the form note
	if (us_err = eus_CommitChangedRtf( h_FORM, ITEM_NAME_TEMPLATE, 
														&t_RtfContextForm))
		goto errJump;

	//add the subform name to the form's subforms text-list field
	us_err = NSFItemAppendTextList( h_FORM, SUBFORM_ITEM_NAME, pc_SUBFORMNM, 
										(WORD) strlen( pc_SUBFORMNM), FALSE);

errJump:
	ef_FreeRtfContext( &t_RtfContextForm);

	return us_err;
} //us_InsertSubformTop(


/** us_RefreshPgpTemplate( ***
Replaces the design of the target Domino environment's PGP Plugin Template 
database with the design of the plug-in template residing in the installer.

--- parameters & return ----
h_PGP_TMPLT: handle to the PGP Plugin Template already existing in the target 
	Domino environment
pc_EXTFILENM_SRC_PGP_TMPLT: address of the extended filename of the PGP Plugin 
	Template copied by the installation process to the local system
pc_EXTFILENM_MAIL_TMPLT: Address of the extended filename of the Notes Mail 
	template on the given server. An item with this parameter's contents is 
	stored on the About Document of the destination PGP Plugin Template in 
	order to provide a default when the refresh button on the document is 
	pressed.
pnid_sbfrmMain: Output. Address of the NoteID structure in which to store the 
	NoteID associated with the PGP Plug-In subform in the production copy of 
	the PGP Plugin Template database.
pnid_sbfrmDll: Output. Address of the NoteID structure in which to store the 
	NoteID associated with the DLL carrying plug-in subform in the production 
	copy of the PGP Plugin Template database.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ logic extension to support addition of DLL carrying plug-in subform, 
  concomitant signature adjustment
+ minor signature adjustment to maintain appropriate constness
+ token renaming, listing format adjustment

9/20/99 PR
+ logic adjustment to utilize common code and to adhere to the recast of the 
  installation routine
+ completed standard documentation

2/6/99 PR: created			*/
static STATUS us_RefreshPgpTemplate( 
									const DBHANDLE  h_PGP_TMPLT, 
									const char  pc_EXTFILENM_SRC_PGP_TMPLT[], 
									const char  pc_EXTFILENM_MAIL_TMPLT[], 
									NOTEID *const  pnid_sbfrmMain, 
									NOTEID *const  pnid_sbfrmDll)	{
	DBHANDLE  h_srcTmplt;
	NOTEID  nid_sbfrmMain, nid_sbfrmDll, nid_AbtDoc, nid_AbtDocLib;
	STATUS  us_err, us_error;

	_ASSERTE( h_PGP_TMPLT && pc_EXTFILENM_SRC_PGP_TMPLT && pnid_sbfrmMain && 
																pnid_sbfrmDll);

	*pnid_sbfrmMain = *pnid_sbfrmDll = NULL;

	//get a handle on the local, source template the install process placed
	if (us_err = eus_OpenTemplate( (char *) pc_EXTFILENM_SRC_PGP_TMPLT, 
										(char *) epc_NULL, &h_srcTmplt, NULL))
		return us_err;

	//prepare the source template's PGP design elements for production use 
	//	(signing them, mainly)
	if (us_err = us_PrepPgpTemplateProductionDesign( h_srcTmplt, FALSE, 
											pc_EXTFILENM_MAIL_TMPLT, 
											&nid_sbfrmMain, &nid_sbfrmDll, 
											&nid_AbtDoc, &nid_AbtDocLib, NULL))
		goto errJump;

	//refresh the About Document, About-Document script library, Icon 
	//	document and PGP Plug-In subform design elements in the target 
	//	template with the elements in the source template brought in by the 
	//	installer
	if (us_err = eus_ReplaceDesignNote( h_PGP_TMPLT, NOTE_CLASS_INFO, NULL, 
											h_srcTmplt, nid_AbtDoc, NULL, NULL))
		goto errJump;
	if (us_err = eus_ReplaceDesignNote( h_PGP_TMPLT, NOTE_CLASS_ICON, NULL, 
										h_srcTmplt, NULL, (char *) TRUE, NULL))
		goto errJump;
	if (us_err = eus_ReplaceDesignNote( h_PGP_TMPLT, NOTE_CLASS_FILTER, 
												mpc_ABT_DOC_LIB, h_srcTmplt, 
												nid_AbtDocLib, NULL, NULL))
		goto errJump;
	if (us_err = eus_ReplaceDesignNote( h_PGP_TMPLT, NOTE_CLASS_FORM, 
										mpc_SUBFORMNM_PGP_DLL, h_srcTmplt, 
										nid_sbfrmDll, NULL, pnid_sbfrmDll))
		goto errJump;
	us_err = eus_ReplaceDesignNote( h_PGP_TMPLT, NOTE_CLASS_FORM, 
										mpc_SUBFORMNM_PGP, h_srcTmplt, 
										nid_sbfrmMain, NULL, pnid_sbfrmMain);

errJump:
	if (h_srcTmplt)
		if ((us_error = NSFDbClose( h_srcTmplt)) && !us_err)
			us_err = us_error;

	return us_err;
} //us_RefreshPgpTemplate(


/** us_PrepPgpTemplateProductionDesign( ***
Sets up the installer's copy of the PGP Plugin Template for use in the target 
production environment. Main task is to sign the design elements with the 
current user's Notes ID.

--- parameters & return ----
H: handle to the PGP Plugin Template database delivered by the installer
f_ICON_NOTE_TOO: flag telling whether or not the icon note should be signed 
	during this procedure (the only thing that happens to an icon note here)
pc_EXTFILENM_MAIL_TMPLT: Address of the extended filename of the Notes Mail 
	template on the given server. An item with this parameter's contents is 
	stored on the About Document of the PGP Plugin Template in order to provide 
	a default when the utility refresh button on the document is pressed.
pnid_sbfrmMain: Optional Output. Address of the NoteID variable in which to 
	store the NoteID of the main PGP Plug-In subform in the PGP Plugin Template 
	delivered by the installer. A pass of NULL suppresses this output.
pnid_sbfrmDll: Optional Output. Address of the NoteID variable in which to 
	store the NoteID of the DLL carrying plug-in subform in the PGP Plugin 
	Template delivered by the installer. A pass of NULL suppresses this output.
pnid_AbtDoc: Optional Output. Address of the NoteID variable in which to store 
	the NoteID of the About Document in the PGP Plugin Template delivered by 
	the installer. A pass of NULL suppresses this output.
pnid_AbtDocLib: Optional Output. Address of the NoteID variable in which to 
	store the NoteID of the About-Document script library in the PGP Plugin 
	Template delivered by the installer. A pass of NULL suppresses this output.
pnid_Icon: Optional Output. Address of the NoteID variable in which to store 
	the NoteID of the Icon document in the PGP Plugin Template delivered by the 
	installer. Parameter ignored if caller does not wish the icon note to be 
	signed. In any case, a pass of NULL suppresses the output.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ extended to support new plug-in DLL subform element, concomitant signature 
  change
+ minor logic (and signature) adjustment to maintain appropriate constness
+ minor documentation adjustment, token renaming, listing format adjustment

2/23/00 PR: to prevent "corrupted note" error message in Notes 4.5.1 client, 
	added OPEN_EXPAND flag when opening notes to be signed
9/20/99 PR: created			*/
static STATUS us_PrepPgpTemplateProductionDesign( 
										const DBHANDLE  H, 
										const BOOL  f_ICON_NOTE_TOO, 
										const char pc_EXTFILENM_MAIL_TMPLT[], 
										NOTEID *const  pnid_sbfrmMain, 
										NOTEID *const  pnid_sbfrmDll, 
										NOTEID *const  pnid_AbtDoc, 
										NOTEID *const  pnid_AbtDocLib, 
										NOTEID *const  pnid_Icon)	{
	char  pc_FLDNM_DEFAULT_MAIL_TEMPLATE[] = "t_DefaultMailTemplate";

	NOTEID  nid;
	NOTEHANDLE  h;
	STATUS  us_err;

	_ASSERTE( H && pc_EXTFILENM_MAIL_TMPLT);

	if (pnid_sbfrmMain)
		*pnid_sbfrmMain = NULL;
	if (pnid_sbfrmDll)
		*pnid_sbfrmDll = NULL;
	if (pnid_AbtDoc)
		*pnid_AbtDoc = NULL;
	if (pnid_AbtDocLib)
		*pnid_AbtDocLib = NULL;
	if (pnid_Icon)
		*pnid_Icon = NULL;

	//Open the About Document, add mail-template default field, then sign, 
	//	save & close. Although at some point later versions do not have the 
	//	restriction, the Notes 4.5.1 client will successfully verify 
	//	design-note signatures only when the note is explicitly opened with 
	//	the "expand" flag before being signed.
	if (us_err = NSFDbGetSpecialNoteID( H, SPECIAL_ID_NOTE | NOTE_CLASS_INFO, 
																		&nid))
		return us_err;
	if (us_err = NSFNoteOpen( H, nid, OPEN_EXPAND, &h))
		return us_err;
	if (us_err = NSFItemSetText( h, pc_FLDNM_DEFAULT_MAIL_TEMPLATE, 
									(char *) pc_EXTFILENM_MAIL_TMPLT, (WORD) 
									strlen( pc_EXTFILENM_MAIL_TMPLT)))
		goto errJump;
	if (us_err = NSFNoteSign( h))
		goto errJump;
	if (us_err = NSFNoteUpdate( h, NULL))
		goto errJump;
	NSFNoteClose( h);
	if (pnid_AbtDoc)
		*pnid_AbtDoc = nid;

	//if requested, open Icon document, then sign, save & close
	if (f_ICON_NOTE_TOO)	{
		if (us_err = NSFDbGetSpecialNoteID( H, SPECIAL_ID_NOTE | 
														NOTE_CLASS_ICON, &nid))
			return us_err;
		if (us_err = NSFNoteOpen( H, nid, OPEN_EXPAND, &h))
			return us_err;
		if (us_err = NSFNoteSign( h))
			goto errJump;
		if (us_err = NSFNoteUpdate( h, NULL))
			goto errJump;
		NSFNoteClose( h);
		if (pnid_Icon)
			*pnid_Icon = nid;
	} //if (f_ICON_NOTE_TOO)

	//open About-Document script library, then sign, save & close
	if (us_err = NIFFindDesignNote( H, mpc_ABT_DOC_LIB, NOTE_CLASS_FILTER, 
																		&nid))
		return us_err;
	if (us_err = NSFNoteOpen( H, nid, OPEN_EXPAND, &h))
		return us_err;
	if (us_err = NSFNoteSign( h))
		goto errJump;
	if (us_err = NSFNoteUpdate( h, NULL))
		goto errJump;
	NSFNoteClose( h);
	if (pnid_AbtDocLib)
		*pnid_AbtDocLib = nid;

	//open the DLL carrying plug-in subform, then sign, save & close
	if (us_err = NIFFindDesignNote( H, mpc_SUBFORMNM_PGP_DLL, NOTE_CLASS_FORM, 
																		&nid))
		return us_err;
	if (us_err = NSFNoteOpen( H, nid, OPEN_EXPAND, &h))
		return us_err;
	if (us_err = NSFNoteSign( h))
		goto errJump;
	if (us_err = NSFNoteUpdate( h, NULL))
		goto errJump;
	if (pnid_sbfrmDll)
		*pnid_sbfrmDll = nid;

	//open PGP Plug-In subform, then sign, save & close
	if (us_err = NIFFindDesignNote( H, mpc_SUBFORMNM_PGP, NOTE_CLASS_FORM, 
																		&nid))
		return us_err;
	if (us_err = NSFNoteOpen( H, nid, OPEN_EXPAND, &h))
		return us_err;
	if (us_err = NSFNoteSign( h))
		goto errJump;
	if (us_err = NSFNoteUpdate( h, NULL))
		goto errJump;
	if (pnid_sbfrmMain)
		*pnid_sbfrmMain = nid;

errJump:
	NSFNoteClose( h);

	return us_err;
} //us_PrepPgpTemplateProductionDesign(


/** us_PlacePgpTemplate( ***
Copies the PGP Plugin Template provided by the installation process to the 
required location on the Domino server, and sets its ACL appropriately.

--- parameters & return ----
pc_EXTFILENM_SRC_PGP_TEMPLATE: pointer to the extended filename of the 
	PGP Plugin Template copied by the installation process to the local 
	system
pc_NETFILENM_PGP_TEMPLATE: pointer to the Domino network filename to which 
	the PGP Plugin Template should be copied
pc_USERNM: the canonicalized name of the user who should be added to the ACL 
	of the PGP Plugin Template with Manager access
h_MAIL_TEMPLATE: handle to the Notes Mail template database whose ACL will 
	serve as the basis of the ACL in the PGP Plugin Template database
pc_EXTFILENM_MAIL_TEMPLATE: Pointer to the extended filename of the Notes 
	Mail template on the given server. An item with this parameter's contents 
	is stored on the About Document of the PGP Plugin Template in order to 
	provide a default when the utility refresh button on the document is 
	pressed.
ph: Output. Pointer to the variable in which to store the handle of the 
	final, production version of the PGP Plugin Template.
pc_class: Output. Pointer to the string buffer in which to store the class 
	name of the PGP Plugin Template.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
7/21/03 PR: minor logic (and signature) adjustment to maintain appropriate 
	constness

9/20/99 PR
+ logic adjustment to utilize common code and to adhere to the recast of the 
  installation routine
+ completed standard documentation

2/6/99 PR: created			*/
static STATUS us_PlacePgpTemplate( const char  pc_EXTFILENM_SRC_PGP_TEMPLATE[], 
									const char  pc_NETFILENM_PGP_TEMPLATE[], 
									const char  pc_USERNM[], 
									const DBHANDLE  h_MAIL_TEMPLATE, 
									const char  pc_EXTFILENM_MAIL_TEMPLATE[], 
									DBHANDLE *const  ph, 
									char *const  pc_class)	{
	DBHANDLE  h_PgpTemplate, h_PgpTemplateFinal = NULL;
	WORD  us, us_flags;
	HANDLE  h_Acl = NULL;
	BOOL  f_found;
	char  pc_DbInfo[ NSF_INFO_SIZE];
	TIMEDATE  td;
	STATUS  us_err;

	_ASSERTE( pc_EXTFILENM_SRC_PGP_TEMPLATE && pc_NETFILENM_PGP_TEMPLATE && 
											pc_USERNM && h_MAIL_TEMPLATE && 
											ph && pc_class);

	*ph = (DBHANDLE) (*pc_class = NULL);

	//open the local copy of the plug-in template (placed there by the 
	//	installation process)
	if (us_err = eus_OpenTemplate( (char *) pc_EXTFILENM_SRC_PGP_TEMPLATE, 
								(char *) epc_NULL, &h_PgpTemplate, pc_class))
		return us_err;

	//copy in the ACL from the mail template, and add the user explicitly 
	//	as a manager of the database, if necessary
	if (us_err = NSFDbCopyACL( h_MAIL_TEMPLATE, h_PgpTemplate))
		//For some unknown reason, Notes throws the "Can't change UNID" error 
		//	when copying the ACL, even though the ACL *is* successfully 
		//	copied. So we just ignore this error.
		if (ERR( us_err) != ERR_CANT_CHANGE_UNID)
			goto errJump;
	if (us_err = NSFDbReadACL( h_PgpTemplate, &h_Acl))
		goto errJump;
	if (us_err = us_LookupAclEntry( (char *) pc_USERNM, h_Acl, &f_found, &us, 
																	&us_flags))
		goto errJump;
	if (us != ACL_LEVEL_MANAGER)	{
		if (f_found)	{
			if (us_err = ACLUpdateEntry( h_Acl, (char *) pc_USERNM, 
												ACL_UPDATE_LEVEL, NULL, 
												ACL_LEVEL_MANAGER, NULL, (WORD) 
												(us_flags | ACL_FLAG_PERSON)))
				goto errJump;
		}else
			if (us_err = ACLAddEntry( h_Acl, (char *) pc_USERNM, 
									ACL_LEVEL_MANAGER, NULL, ACL_FLAG_PERSON))
				goto errJump;
		if (us_err = NSFDbStoreACL( h_PgpTemplate, h_Acl, NULL, 0))
			goto errJump;
	} //if (us != ACL_LEVEL_MANAGER)
	OSMemFree( h_Acl);
	h_Acl = NULL;

	//prepare the source template's PGP design elements for production use 
	//	(signing them, mainly)
	if (us_err = us_PrepPgpTemplateProductionDesign( h_PgpTemplate, TRUE, 
												pc_EXTFILENM_MAIL_TEMPLATE, 
												NULL, NULL, NULL, NULL, NULL))
		goto errJump;

	//place a new full copy of the PGP design-elements template database in 
	//	the same directory as the mail template
	TimeConstant( TIMEDATE_WILDCARD, &td);
	if (us_err = NSFDbCreate( (char *) pc_NETFILENM_PGP_TEMPLATE, 
												DBCLASS_TEMPLATEFILE, FALSE))
		goto errJump;
	if (us_err = NSFDbOpen( (char *) pc_NETFILENM_PGP_TEMPLATE, 
														&h_PgpTemplateFinal))
		goto errJump;
	if (us_err = NSFDbCopyACL( h_PgpTemplate, h_PgpTemplateFinal))
		if (ERR( us_err) != ERR_CANT_CHANGE_UNID)
			goto errJump;
	if (us_err = NSFDbInfoGet( h_PgpTemplate, pc_DbInfo))
		goto errJump;
	if (us_err = NSFDbInfoSet( h_PgpTemplateFinal, pc_DbInfo))
		goto errJump;
	if (us_err = NSFDbCopy( h_PgpTemplate, h_PgpTemplateFinal, td, 
															NOTE_CLASS_ALL))
		goto errJump;
	if (us_err = NSFDbClose( h_PgpTemplate))
		goto errJump;

	*ph = h_PgpTemplateFinal;

	return eus_SUCCESS;

errJump:
	if (h_PgpTemplateFinal)
		NSFDbClose( h_PgpTemplateFinal);
	if (h_Acl)
		OSMemFree( h_Acl);
	*pc_class = NULL;
	NSFDbClose( h_PgpTemplate);

	return us_err;
} //us_PlacePgpTemplate(


/** us_TestPgpTemplatePlaced( ***
Checks whether the specified PGP Plugin Template Notes database exists 
already.

--- parameters & return ----
pc_FILENM_PGP_TEMPLATE: pointer to the filename of the PGP Lotus Notes Plugin 
	Template to look for on the given server
pc_SERVERNM: pointer to the name of the Domino server on which the specified 
	template resides
pc_EXTFILENM_MAIL_TEMPLATE: Pointer to the extended filename of the Notes 
	Mail template on the given server. The path component of the filename is 
	used to construct the required directory location of the PGP Plugin 
	Template database (thus the location to check for that template's 
	presence).
ph: Output. Pointer to the variable in which to store the handle of a 
	PGP Plugin Template found to exist already at the required network 
	filename location.
pc_class: Optional Output. Pointer to the string buffer to fill with the class 
	name of the template, if the template is located. A pass of NULL 
	suppresses this output.
pc_netFileNmPgpTemplate: Optional Output. Pointer to the buffer in which to 
	store the required network filename where the PGP Plugin Template 
	resides, or should reside. A pass of NULL suppresses this output.
RETURN:
	eus_SUCCESS if no error occured
	ERR_DIRECTORY if the required location of the PGP Plugin turned out to be 
		a file directory on the server, not a database or non-existent file
	the Notes API error code otherwise

--- revision history -------
7/21/03 PR: minor logic (and signature) adjustment to maintain appropriate 
	constness
3/16/00 PR: minor logic adjustment

9/20/99 PR
+ logic enhancement to utilize common code and to improve exception-handling
+ completed standard documentation

1/12/99 PR: created			*/
static STATUS us_TestPgpTemplatePlaced( 
									const char  pc_FILENM_PGP_TEMPLATE[], 
									const char  pc_SERVERNM[], 
									const char  pc_EXTFILENM_MAIL_TEMPLATE[], 
									DBHANDLE *const  ph, 
									char *const  pc_class, 
									char *const  pc_netFileNmPgpTemplate)	{
	char * pc, * pc_extFileNm;
	UINT  ui = 0;
	DBHANDLE  h;
	WORD  us, us_dmy;
	STATUS  us_err;

	_ASSERTE( pc_FILENM_PGP_TEMPLATE && pc_SERVERNM && 
											pc_EXTFILENM_MAIL_TEMPLATE && ph);

	*ph = NULL;
	if (pc_class)
		*pc_class = NULL;
	if (pc_netFileNmPgpTemplate)
		*pc_netFileNmPgpTemplate = NULL;

	pc = strrchr( pc_EXTFILENM_MAIL_TEMPLATE, ec_PATH_SPECIFIER);
	if (!( pc_extFileNm = malloc( (pc ? (ui = pc - 
									pc_EXTFILENM_MAIL_TEMPLATE + 1) : 0) + 
									strlen( pc_FILENM_PGP_TEMPLATE) + 1)))
		return !eus_SUCCESS;
	if (ui)
		memcpy( pc_extFileNm, pc_EXTFILENM_MAIL_TEMPLATE, ui);

	if (us_err = eus_OpenTemplate( strcpy( pc_extFileNm + ui, 
											pc_FILENM_PGP_TEMPLATE) - ui, 
											(char *) pc_SERVERNM, &h, pc_class))
		switch (ERR( us_err))	{
			case ERR_NOACCESS:
				us = ACL_LEVEL_NOACCESS;
				break;

			case ERR_NOEXIST:
				us_err = eus_SUCCESS;
				goto errJump;

			case mi_ERR_NOT_TEMPLATE:	{
				const char *const  ppc_PART[] = {"The existing PGP Plugin "
										"Template (", ") on server\n\'", 
										"\' has no template class name. "
										"A class name is\nrequired to "
										"refresh the Plugin's Lotus "
										"Notes template design elements."};

				char  pc[ 0x400];
				sprintf( pc, "%s%s%s%s%s", ppc_PART[0], 
									pc_FILENM_PGP_TEMPLATE, ppc_PART[1], 
									pc_SERVERNM, ppc_PART[2]);
				MessageBox( mh_mainWnd, pc, mpc_APPNM, MB_ICONSTOP);

				us_err = mus_HANDLED_EXCEPTION;
				goto errJump;

			}default:
				goto errJump;
		} //switch (ERR( us_err))
	else
		NSFDbAccessGet( h, &us, &us_dmy);

	if (us < ACL_LEVEL_DESIGNER)	{
		const char *const  ppc_PART[] = {"At least Designer access to the "
									"existing\nPGP Plugin Template (", 
									") on server \'", "\' is required to\n"
									"refresh the Plugin's Lotus Notes "
									"template design elements.\nYou "
									"currently do not have Designer access."};

		char  pc[ 0x400];
		sprintf( pc, "%s%s%s%s%s", ppc_PART[0], pc_FILENM_PGP_TEMPLATE, 
									ppc_PART[1], pc_SERVERNM, ppc_PART[2]);
		MessageBox( mh_mainWnd, pc, mpc_APPNM, MB_ICONSTOP);

		us_err = mus_HANDLED_EXCEPTION;
		goto errJump;
	} //if (us < ACL_LEVEL_DESIGNER)

	*ph = h;
	if (pc_netFileNmPgpTemplate)
		OSPathNetConstruct( NULL, (char *) pc_SERVERNM, pc_extFileNm, 
													pc_netFileNmPgpTemplate);

errJump:
	free( pc_extFileNm);
	if (us_err)	{
		if (h)
			NSFDbClose( h);
		if (pc_class)
			*pc_class = NULL;
	}

	return us_err;
} //us_TestPgpTemplatePlaced(


/** us_LookupAclEntry( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
1/12/99 PR: created			*/
//DOC!!
static STATUS us_LookupAclEntry( char pc_USERNM[], 
									HANDLE  h_ACL, 
									BOOL *const  pf_found, 
									WORD *const  pus_level, 
									WORD *const  pus_flags)	{
	AclEntryInfo  t;
	STATUS  us_err;

	_ASSERTE( pc_USERNM && h_ACL && pf_found);

	*pf_found = FALSE;
	if (pus_level)
		*pus_level = NULL;
	if (pus_flags)
		*pus_flags = NULL;

	memset( &t, NULL, sizeof( AclEntryInfo));
	t.pc_name = pc_USERNM;
	if (us_err = ACLEnumEntries( h_ACL, VetAclEntry, &t))
		return us_err;

	if (t.f_found)	{
		*pf_found = TRUE;
		if (pus_level)
			*pus_level = t.us_level;
		if (pus_flags)
			*pus_flags = t.us_flags;
	}

	return eus_SUCCESS;
} //us_LookupAclEntry(


/** VetAclEntry( ***


--- parameters ---------


--- revision history ---
1/12/99 PR: created			*/
//DOC!!
static void LNCALLBACK VetAclEntry( void * pv, 
									char * pc_NAME, 
									WORD  us_LEVEL, 
									ACL_PRIVILEGES * pt_dmy, 
									WORD  us_FLAGS)	{
	AclEntryInfo * pt = pv;

	if (strcmp( pt->pc_name, pc_NAME) != ei_SAME)
		return;

	pt->f_found = TRUE;
	pt->us_level = us_LEVEL;
	pt->us_flags = us_FLAGS;
} //VetAclEntry(





