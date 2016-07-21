/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: PgpHandling.c 25356 2004-07-14 22:43:40Z wjb $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Module provides a staging area from which calls into regular PGP source code 
may be made. Customized to the needs of the Lotus Notes PGP Plug-In.

--- revision history --------
10/13/03 Version 1.2.3 Paul Ryan
+ minor documentation adjustment

6/26/03 Version 1.2.2 Paul Ryan
+ Enhancement toward enabling the pull-down of missing ADKs, causing extension 
  of encode-context structure and ei_PgpLookupEmailLocal() signature, and 
  creation of i_NoteMissingAdks() & ei_PgpLookupMissingAdks(), and 
  enhancement of ei_PgpLookupEmailViaServers()
+ accommodated shared-code change in i_DecryptVerifyBuffer()
+ minor adjustment in support of maintaining appropriate constness
+ documentation adjustment, token renaming, listing format adjustment

3/14/03 Version 1.2 Paul Ryan
+ added ef_findPgpEncryptedBlock()
+ Logic enhancement in support of enabling the Tempest viewer in the plug-in. 
  Included renaming i_GetPgpAsciiCoords() to i_PgpAsciiCoordsOrTempest(), 
  adding eui_ShowInTempestWnd()
+ various minor enhancments: documentation, listing formatting, signature 
  sharpening

11/13/02 Version 1.1.6.1 Paul Ryan
+ logic enhancement to fix bug in ei_FindAndDecodePgpBlock()

10/30/02 Version 1.1.6 Paul Ryan
+ signature change to ei_ResolveEncryptionKeys() in conjunction with 
  finer-grained handling of PGP key-server searches
+ logic adjustment of ei_PgpLookupEmailViaServers() to support better 
  user-cancelation and timeout of key-server searches
+ listing format adjustment, documentation adjustment, token renaming

9/6/02 Version 1.1.5 Paul Ryan
+ workarounds to PGP decoding-engine oddities in ei_TestForPgpAscii(), 
  i_GetPgpAsciiCoords(), i_DecryptVerify() & ei_FindAndDecodePgpBlock()
+ listing format adjustment, minor documentation adjustment, token renaming

7/2/01 Version 1.1.4.1 Paul Ryan
+ minor string adjustment

9/16/00 Version 1.1.4 Paul Ryan
+ added functions for PGP wiping & decoding files

8/9/00 Version 1.1.3 Paul Ryan
+ accommodated recent change made to the DecodeEventData structure used in 
  the PGP shared code, whose functionality is somewhat reproduced in this 
  module

3/20/00 Version 1.1.2 Paul Ryan
+ adjustment to support PGP 7.0

9/26/99 Version 1.1.1 Paul Ryan
+ worked around minor bug in PGP client code (see ei_FindAndDecodePgpBlock() 
  for details)

9/12/99 Version 1.1 Paul Ryan
+ PGP 6.5.1 compatibility
+ logic enhancements
+ documentation updates

12/18/98 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "pgprc.h"
#include "PgpHandling.h"


//global-scope declarations
const char  epc_APPNM[] = "PGP Lotus Notes Plugin";
const int  ei_USER_ABORT = kPGPError_UserAbort;

HWND  eh_mainWnd;


//module-scope declarations
static const char  mpc_PLUGIN_APP[] = "Notes", mpc_MODULENM[] = "ndbPGP.dll";

static PGPContextRef  m_pgpContext = kInvalidPGPContextRef;
static PGPtlsContextRef  m_pgpTlsContext = kInvalidPGPtlsContextRef;


/** ei_InitializePgpContext( ***
Purpose is to set up the ability for this DLL to make calls into the PGP SDK 
and related functionality.

--- return -------------
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history ---
7/2/01 PR: listing format adjustment
3/20/00 PR: adjustment to support Notes R5 and PGP 7.0
9/12/99 PR: documentation adjustment
11/23/98 PR: created		*/
PGPError ei_InitializePgpContext()	{
	//static to suppress multiple nags
	static BOOL  f_LicenseExpired;

	const char  pc_CLASSNM_NOTES[] = "NOTES", 
				pc_CLASSNM_NOTES_FRAME[] = "NOTESframe";

	HWND  h;
	char pc[ sizeof( pc_CLASSNM_NOTES_FRAME) > sizeof( pc_CLASSNM_NOTES) ? 
											sizeof( pc_CLASSNM_NOTES_FRAME) : 
											sizeof( pc_CLASSNM_NOTES)];
	PGPError  i_err;

	//if we have expired, just get out with error 
	if (f_LicenseExpired) 
		return kPGPError_Win32_Expired; 

	//if the PGP environment has already been loaded, don't do it again
	if (PGPContextRefIsValid( m_pgpContext))
		return kPGPError_NoErr;

	//Store the handle of the main window associated with the rich-text 
	//	window currently being operated on by user. Loop is used because 
	//	the different releases of Notes have different window hierarchies.
	if (h = GetFocus())	{
		do
			if (GetClassName( h, pc, sizeof( pc)))
				if (strcmp( pc, pc_CLASSNM_NOTES) == ei_SAME || strcmp( pc, 
										pc_CLASSNM_NOTES_FRAME) == ei_SAME)	{
					eh_mainWnd = h;
					break;
				}
		while (h = GetParent( h));
		if (!eh_mainWnd)
			MessageBox( NULL, "The parent Lotus Notes window\ncould not be "
													"determined", epc_APPNM, 
													MB_OK | MB_ICONEXCLAMATION);
	} //if (h = GetFocus())

	//create a new PGP context to use in this DLL & initialize the 
	//	PGP Common Libraries
	if (IsPGPError( i_err = PGPclInitLibrary( &m_pgpContext, 
														(PGPUInt32) NULL)))	{
		if (i_err == kPGPError_FeatureNotAvailable)	{
			const char  pc_EXPIRED_MSG[] = "The evaluation period for PGP has "
											"passed.\nThe Lotus Notes Plug-In "
											"will no longer function.", 
						pc_EXPIRED_TITLE[] = "PGP Plug-In Expired";

			MessageBox( eh_mainWnd, pc_EXPIRED_MSG, pc_EXPIRED_TITLE, MB_OK);
		}else
			PGPclErrorBox( eh_mainWnd, i_err);

		PGPclCloseLibrary();
		m_pgpContext = kInvalidPGPContextRef;
		return i_err;
	} //if (IsPGPError( i_err = PGPclInitLibrary(

	//if the license has expired, return failure
	if (IsPGPError( i_err = PGPclIsExpired( eh_mainWnd, 
													kPGPclDefaultExpired)))	{
		f_LicenseExpired = TRUE;
		PGPclCloseLibrary();
		m_pgpContext = kInvalidPGPContextRef;
		return i_err;
	}

	if (IsPGPError( i_err = PGPNewTLSContext( m_pgpContext, 
														&m_pgpTlsContext)))	{
		PGPclCloseLibrary();
		m_pgpContext = kInvalidPGPContextRef;
		return i_err;
	}

	return IsPGPError( i_err) ? i_err : kPGPError_NoErr;
} //ei_InitializePgpContext(


/** ei_PgpEncodeBuffer( ***
PGP encode the content buffer provided.

--- parameters & return ----
PC: Address of the input content to be encoded. If content is ASCII, content 
	should be null-terminated.
ul_LEN: length of the input content to be encoded
pt_context: Input & Output. Address of information structure
f_BINARY: flag telling whether the input content is	binary in nature, not 
	ASCII
pl_lenOutput: Optional. Pointer to the variable in which to store the length 
	of the PGP-encoded output content. If passed in null, this functionality 
	will be ignored by the procedure.
ppc_output: Memory address of the pointer in which to store the address of 
	the memory buffer allocated to accommodate the null-terminated 
	PGP-encoded output. Caller is responsible for freeing this memory buffer. 
	In case of an error, the pointer is guaranteed to be null.
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history -------
6/26/03 PR: casting adjustments to support maintenance of apporpriate constness

3/20/00 PR
+ logic adjustment to support PGP 7.0
+ standard documentation completed

12/10/98 PR: created		*/
PGPError ei_PgpEncodeBuffer( char *const  PC, 
								const DWORD  ul_LEN, 
								PgpEncodeContext *const  pt_context, 
								const BOOL  f_BINARY, 
								long *const  pl_lenOutput, 
								char * *const  ppc_output)	{
	PGPOptionListRef  oneTimeOptions;
	long  l_lenOutput;
	PGPError  i_err, i_error;

	if (!( PC && pt_context && (pt_context->f_Sign || 
										pt_context->f_Encrypt) && ppc_output))
		return !kPGPError_NoErr;

	if (pl_lenOutput)
		*pl_lenOutput = (DWORD) NULL;
	*ppc_output = NULL;

	//if this is a buffer of binary data and it just needs to be signed, 
	//	have it encoded with ASCII Armor
	if (f_BINARY && pt_context->f_Sign && !pt_context->f_Encrypt)
		i_err = PGPBuildOptionList( m_pgpContext, &oneTimeOptions, 
										PGPOArmorOutput( m_pgpContext, TRUE), 
										PGPOLastOption( m_pgpContext));
	else
		i_err = PGPBuildOptionList( m_pgpContext, &oneTimeOptions, 
											PGPOLastOption( m_pgpContext));
	if (IsPGPError( i_err))	{
		DisplayPgpError( __FILE__, __LINE__, mpc_MODULENM, i_err);
		return i_err;
	}
		
	i_err = EncryptSignBuffer( eh_Instance, eh_mainWnd, m_pgpContext, 
								m_pgpTlsContext, (char *) mpc_PLUGIN_APP, 
								(char *) mpc_MODULENM, PC, ul_LEN, 
								&pt_context->t_recipients, oneTimeOptions, 
								&pt_context->pgpUserOptions, ppc_output, 
								&l_lenOutput, pt_context->f_Encrypt, 
								pt_context->f_Sign, f_BINARY, FALSE);

	if (pl_lenOutput)
		*pl_lenOutput = l_lenOutput;

	i_error = PGPFreeOptionList( oneTimeOptions);

	return i_err ? i_err : i_error;
} //ei_PgpEncodeBuffer(


/** ei_PgpEncodeFile( ***
PGP encode the specified file on the user's file system.

--- parameters & return ----
pt_context: address of the information structure used by the Notes Plug-In to 
	hold a common set of PGP contextual information through the encoding 
	process
pc_EXTFILENM_ORIG: address of the string specifying the current extended 
	filename (path plus filename) of the file to be encoded
pc_EXTFILENM_NEW: address of the string specifying what the extended filename 
	of the encoded file should be	
RETURN: kPGPError_NoErr if successful; the PGP error code otherwise

--- revision history -------
6/26/03 PR: casting adjustments to support maintenance of apporpriate constness
3/14/03 PR: got rid of remmed-out code
9/16/00 PR: inconsequential signature change
3/20/00 PR: provided standard documentation
12/15/98 PR: created		*/
PGPError ei_PgpEncodeFile( PgpEncodeContext *const  pt_ctx, 
							const char  pc_EXTFILENM_ORIG[], 
							const char  pc_EXTFILENM_NEW[])	{
	if (!( pt_ctx && (pt_ctx->f_Sign || pt_ctx->f_Encrypt) && 
										pc_EXTFILENM_ORIG && pc_EXTFILENM_NEW))
		return !kPGPError_NoErr;

	return EncryptSignFile( eh_Instance, eh_mainWnd, m_pgpContext, 
							m_pgpTlsContext, (char *) mpc_PLUGIN_APP, (char *) 
							mpc_MODULENM, (char *) pc_EXTFILENM_ORIG, 
							&pt_ctx->t_recipients, NULL, 
							&pt_ctx->pgpUserOptions, (char *) pc_EXTFILENM_NEW, 
							pt_ctx->f_Encrypt, pt_ctx->f_Sign, TRUE);
} //ei_PgpEncodeFile(


/** ei_PgpDecodeFile( ***
PGP decode the specified file on the user's file system.

--- parameters & return ----
PC: address of the string specifying the extended filename (path plus filename) 
	of the file to be decoded
ppc: Output. Address of the pointer to receive the address of the string naming 
	the decoded file. Caller is responsible for freeing, via PGP mechanism, the 
	allocated string.
RETURN: kPGPError_NoErr if successful; the PGP error code otherwise

--- revision history -------
6/26/03 PR: casting adjustments to support maintenance of apporpriate 
	constness, including a minor signature adjustment
3/14/03 PR: listing format adjustment, documentation adjustment
9/16/00 PR: created			*/
PGPError ei_PgpDecodeFile( const char  PC[], 
							char * *const  ppc)	{
	BOOL  f_dmy;

	if (!( PC && ppc))
		return !kPGPError_NoErr;

	return DecryptVerifyFile( eh_Instance, eh_mainWnd, m_pgpContext, 
									m_pgpTlsContext, (char *) mpc_PLUGIN_APP, 
									(char *) mpc_MODULENM, (char *) PC, FALSE, 
									TRUE, ppc, NULL, NULL, &f_dmy);
} //ei_PgpDecodeFile(


/** ei_PgpWipeFile( ***
PGP safe erase (wipe) the specified file from the user's file system, 
indicating the result in PGP-error format.

--- parameter & return ----
PC: address of the string specifying the extended filename (path plus 
	filename) of the file to be wiped
RETURN: kPGPError_NoErr if successful; !kPGPError_NoErr code otherwise

--- revision history ------
6/26/03 PR: casting adjustments to support maintenance of apporpriate 
	constness, including a minor signature adjustment
9/16/00 PR: created			*/
PGPError ei_PgpWipeFile( const char  PC[])	{
	FILELIST * pt = NULL;

	if (!PC)
		return !kPGPError_NoErr;

	PGPscAddToFileList( &pt, (char *) PC, NULL, NULL);
	return PGPscWipeFileList( eh_mainWnd, m_pgpContext, pt, WIPE_DONOTWARN | 
										WIPE_DONOTPROGRESS, (ULONG) NULL) ? 
										kPGPError_NoErr : !kPGPError_NoErr;
} //ei_PgpWipeFile(


/** xs_PgpWipeFile( ***
If not already OS locked, PGP safe erase (wipe) the specified file from the 
user's file system, indicating result in a LotusScript compatible format.

--- parameter & return ----
PC: address of the string specifying the extended filename (path plus 
	filename) of the file to be wiped
RETURN:
	kPGPError_NoErr if successful
	eus_ERR_INVLD_ARG if parameter passed is obviously invalid
	errno if specified file does not exist or is locked for writing
	PGP error code if unanticipated error occurs, cast to a short

--- revision history ------
9/16/00 PR: created			*/
short xs_PgpWipeFile( char  PC[])	{
	int  i_err;

	if (!PC)
		return eus_ERR_INVLD_ARG;

	if (i_err = ei_FileAccess( PC, ei_FILE_ACCS_READWRITE, TRUE))
		return i_err;

	return ei_PgpWipeFile( PC);
} //xs_PgpWipeFile(


/** ei_SetUpPgpEncodeContext( ***
Initializes a Notes Plug-In information structure for use in ensuing PGP 
encodings.

--- parameters & return ----
f_SIGN: flag telling whether content is to be signed when encoded
f_ENCRYPT: flag telling whether content is to be encrypted when encoded
pt_ctx: Output. Address of information structure used by the Notes Plug-In to 
	receive a common set of PGP contextual information through multiple 
	encoding processes which the plug-in may choose to engage.
pf_SyncUnKys: Output. Address of the variable in which to store whether the 
	user wishes PGP to attempt to find unknown keys via a search of certificate 
	servers listed in her preferences. Relevant only when encryption is 
	involved. Ignored if omitted.
RETURN: kPGPError_NoErr if successful; the PGP error code otherwise

--- revision history -------
6/26/03 PR: minor logic shortening, token rename
3/14/03 PR: listing format adjustment, token renaming, minor documentation 
	adjustment

3/20/00 PR
+ adjustment to support PGP 7.0
+ documentation adjustment

9/12/99 PR
+ split out encryption-key resolution into ei_ResolveEncryptionKeys()
+ completed header documentation

12/7/98 PR: created			*/
PGPError ei_SetUpPgpEncodeContext( const BOOL  f_SIGN, 
									const BOOL  f_ENCRYPT, 
									PgpEncodeContext *const  pt_ctx, 
									BOOL *const  pf_SyncUnKys)	{
	PGPclRecipientDialogStruct * pt_recipients;
	PGPError  i_err;

	if (!( (f_SIGN || f_ENCRYPT) && pt_ctx))
		return !kPGPError_NoErr;

	memset( pt_ctx, (BYTE) NULL, sizeof( PgpEncodeContext));
	if (pf_SyncUnKys)
		*pf_SyncUnKys = FALSE;

	pt_ctx->pgpUserOptions = kInvalidPGPOptionListRef;

	pt_recipients = &pt_ctx->t_recipients;
	if (IsPGPError( i_err = PGPclOpenDefaultKeyrings( m_pgpContext, 
										kPGPOpenKeyDBFileOptions_Mutable, 
										&pt_recipients->keydbOriginal)))	{
		PGPclEncDecErrorBox( eh_mainWnd, i_err);
		return i_err;
	}

	pt_ctx->f_Sign = f_SIGN;
	pt_ctx->f_Encrypt = f_ENCRYPT;

	if (!f_ENCRYPT)
		return kPGPError_NoErr;

	pt_recipients->context = m_pgpContext;
	pt_recipients->tlsContext = m_pgpTlsContext;
	pt_recipients->Version = kPGPCurrentRecipVersion;
	pt_recipients->hwndParent = eh_mainWnd;
	pt_recipients->dwOptions = kPGPclASCIIArmor;	
	pt_recipients->dwDisableFlags = kPGPclDisableWipeOriginal + 
											kPGPclDisableASCIIArmor + 
											kPGPclDisableInputIsText + 
											kPGPclDisableSelfDecryptingArchive;

	//if necessary, determine whether the user has it set that keys not located 
	//	should be sought via certificate servers she's got specified
	if (pf_SyncUnKys)	{
		PGPPrefRef	pgpPrefs;
		PGPBoolean  f;

		if (i_err = PGPclPeekClientLibPrefRefs( &pgpPrefs, NULL))
			return i_err;
		i_err = PGPclGetPrefBoolean( pgpPrefs, 
										kPGPPrefKeyServerSyncUnknownKeys, &f);
		if (IsntPGPError( i_err))
			*pf_SyncUnKys = f;
	} //if (pf_SyncUnKys)

	return i_err;
} //ei_SetUpPgpEncodeContext(


/** ei_ResolveEncryptionKeys( ***
Obtain, using the standard PGP client infrastructure, the list of encryption 
keys for use in ensuing encodings, based upon a set of e-mail addresses.

--- parameters & return ----
ppc_RECPTS: address of a string-array list of the e-mail addresses to be 
	resolved to encryption keys
ui_RECPTS: the number of e-mail addresses in the ppc_RECPTS list
f_AMBIG: flag telling whether the recipients array contains addresses that were 
	derived via a prior address-resolution process which encountered ambiguity 
	vis-à-vis the input addresses and which reacted by adding the ambiguous 
	addresses to the resolved list and flipping an "ambiguous" flag to notify 
	downstream logic of the condition
f_DISABL_SYNC_UNK_KYS: flag telling whether searching of PGP key servers for 
	unknown keys should be suppressed (possibly because such searching has 
	already been done upstream)
pt_ctx: Input & Output. Address of the information structure used by the Notes 
	Plug-In to hold a common set of PGP contextual information through the 
	encoding process. The t_recipients member of the structure will be adjusted 
	by the procedure.
RETURN: kPGPError_NoErr if successful; the PGP error code otherwise

--- revision history -------
7/21/03 PR: inconsequential code cleanup
6/26/03 PR: token renaming, listing format adjustment

3/14/03 PR
+ logic enhancement to avoid deletion of keys in user's main key database which 
  have been merged with key elements pulled down from key servers in order to 
  include those elements as input to the recipients-dialog engine
+ logic enhancement in support of enabling the Tempest viewer in the plug-in
+ fixed variable-initialization bugs

10/30/02 PR
+ logic enhancement (and concomitant signature change) in support of allowing 
  suppression of PGP key-server searches for unknown keys
+ listing format adjustment, token renaming, documentation adjustment

3/20/00 PR: adjustment to support PGP 7.0
9/12/99 PR: created			*/
PGPError ei_ResolveEncryptionKeys( char * *const  ppc_RECPTS, 
									const UINT  ui_RECPTS, 
									const BOOL  f_AMBIG, 
									const BOOL  f_DISABL_SYNC_UNK_KYS, 
									PgpEncodeContext *const  pt_ctx)	{
	const char  pc_RECPTS_DLG_TITL[] = "PGP Notes - Encrypt Message To...";

	PGPPrefRef  pt_prefs;
	PGPclRecipientDialogStruct * pt_recpts;
	PGPKeyDBRef  pt_kyDbAdded, pt_kyDbOrig = kInvalidPGPKeyDBRef;
	PGPKeySetRef  pt_kySetAdded = kInvalidPGPKeySetRef;
	PGPUInt32  ul;
	PGPBoolean  f_abortd, f_resetSyncUnkKys = FALSE;
	PGPError  i_err = (int) kPGPError_NoErr, i_error;

	PGPValidateParam( pt_ctx && pt_ctx->f_Encrypt && PGPContextRefIsValid( 
											pt_ctx->t_recipients.context) && 
											PGPtlsContextRefIsValid( 
											pt_ctx->t_recipients.tlsContext) && 
											ppc_RECPTS && ui_RECPTS);

	pt_recpts = &pt_ctx->t_recipients;
	pt_recpts->szTitle = (char *) pc_RECPTS_DLG_TITL;
	pt_recpts->dwNumRecipients = ui_RECPTS;
	pt_recpts->szRecipientArray = ppc_RECPTS;

	//If the keydbAdded member seems to have been populated elsewhere, the 
	//	added keys were presumably pulled down from key servers. Avoid looking 
	//	the keys up again or prompting for an import before the recipients 
	//	dialog is shown...
	if (PGPKeyDBRefIsValid( pt_kyDbAdded = pt_recpts->keydbAdded))	{
		//if any keys are in the "added" database...
		pt_recpts->keydbAdded = kInvalidPGPKeyDBRef;
		if (IsPGPError( i_err = PGPNewKeySet( pt_kyDbAdded, &pt_kySetAdded)))
			goto errJump;
		if (IsPGPError( i_err = PGPCountKeys( pt_kySetAdded, &ul)))
			goto errJump;
		if (ul)	{
			PGPKeyDBRef  pt_kyDb;
			PGPKeySetRef  pt_kySet;

			//Copy the added keys to a copy of the user's main key database so 
			//	they'll be included as input to the recipients dialog. Maintain 
			//	the reference to the added keys so they may be referenced 
			//	independently after the recipients dialog is closed (allowing 
			//	us to prompt to import downloaded keys, _after_ the recipients 
			//	dialog has been presented).
			if (IsPGPError( i_err = PGPNewKeyDB( m_pgpContext, &pt_kyDb)))
				goto errJump;
			if (IsPGPError( i_err = PGPNewKeySet( pt_kyDbOrig = 
									pt_recpts->keydbOriginal, &pt_kySet)))	{
				PGPFreeKeyDB( pt_kyDb);
				goto errJump;
			}
			if (IsPGPError( i_err = PGPCopyKeys( pt_kySet, pt_kyDb, NULL)))	{
				PGPFreeKeyDB( pt_kyDb);
				goto errJump;
			}
			if (IsPGPError( i_err = PGPFreeKeySet( pt_kySet)))	{
				PGPFreeKeyDB( pt_kyDb);
				goto errJump;
			}
			if (IsPGPError( i_err = PGPCopyKeys( pt_kySetAdded, pt_kyDb, 
																	NULL)))	{
				PGPFreeKeyDB( pt_kyDb);
				goto errJump;
			}
			pt_recpts->keydbOriginal = pt_kyDb;

			//free resource no longer needed
			i_err = PGPFreeKeySet( pt_kySetAdded);
		//else clean up as though the empty database never existed
		}else	{
			i_err = PGPFreeKeyDB( pt_kyDbAdded);
			pt_kyDbAdded = kInvalidPGPKeyDBRef;
		} //if (ul)
		pt_kySetAdded = kInvalidPGPKeySetRef;
		if (IsPGPError( i_err))
			goto errJump;
	} //if (PGPKeyDBRefIsValid( pt_kyDbAdded =

	//if caller wants further PGP key-server searches suppressed, comply
	if (f_DISABL_SYNC_UNK_KYS)	{
		if (IsPGPError( i_err = PGPclPeekClientLibPrefRefs( &pt_prefs, NULL)))
			goto errJump;
		if (IsPGPError( i_err = PGPGetPrefBoolean( pt_prefs, 
											kPGPPrefKeyServerSyncUnknownKeys,
											&f_resetSyncUnkKys)))
			goto errJump;
		if (f_resetSyncUnkKys)	{
			if (IsPGPError( i_err = PGPSetPrefBoolean( pt_prefs, 
									kPGPPrefKeyServerSyncUnknownKeys, FALSE)))
				goto errJump;
			if (IsPGPError( i_err = PGPclFlushClientLibPrefs( pt_prefs, 
														kInvalidPGPPrefRef)))
				goto errJump;
		}
	} //if (f_DISABL_SYNC_UNK_KYS)

	//if ambiguity is involved in the recipients list or if <Shift> is 
	//	depressed, force the dialog to pop
	if (f_AMBIG || GetAsyncKeyState( VK_SHIFT) & 0x8000)
		pt_recpts->dwDisableFlags += kPGPclDisableAutoMode;

	//see whom we wish to encrypt this to
	f_abortd = !PGPclRecipientDialog( pt_recpts);

	//If keys were pulled down from key servers before the calling of the 
	//	recipients dialog, free resources used for the enhanced copy of the 
	//	user's main key database. (The user has not yet confirmed that she 
	//	wishes to import those new keys into her main database.) And reset to 
	//	start using the main database.
	if (PGPKeyDBRefIsValid( pt_kyDbOrig))	{
		if (IsPGPError( i_err = PGPFreeKeyDB( pt_recpts->keydbOriginal)))
			goto errJump;
		pt_recpts->keydbOriginal = pt_kyDbOrig;
		pt_kyDbOrig = kInvalidPGPKeyDBRef;
	}

	//note whether user wants the recipient to use the Tempest viewer when 
	//	decoding (and so whether the Notes rich-text version of the message is 
	//	unnecessary)
	pt_ctx->f_asciiOnlyBdy = !!(pt_recpts->dwOptions & kPGPclSecureViewer);

	//if any brand-new keys seem to have been pulled down from key servers...
	if (PGPKeyDBRefIsValid( pt_recpts->keydbAdded))
		//if no keys were pulled down _before_ the calling of the recipients 
		//	dialog...
		if (!PGPKeyDBRefIsValid( pt_kyDbAdded))	{
			//if the just-pulled-down key database contains brand-new keys...
			if (IsPGPError( i_err = PGPCountKeysInKeyDB( pt_recpts->keydbAdded, 
																		&ul)))
				goto errJump;
			if (ul)
				//note that it's the database we'll use for the import to follow
				pt_kyDbAdded = pt_recpts->keydbAdded;
			//else free resources allocated to the empty database
			else	{
				i_err = PGPFreeKeyDB( pt_recpts->keydbAdded);
				pt_recpts->keydbAdded = kInvalidPGPKeyDBRef;
			} //if (ul)
		//else add the just-pulled-down keys to the new keys pulled down before 
		//	the calling of the recipients dialog
		}else	{
			if (IsPGPError( i_err = PGPNewKeySet( pt_recpts->keydbAdded, 
															&pt_kySetAdded)))
				goto errJump;
			if (IsPGPError( i_err = PGPCopyKeys( pt_kySetAdded, pt_kyDbAdded, 
																		NULL)))
				goto errJump;
			i_err = PGPFreeKeyDB( pt_recpts->keydbAdded);
			pt_recpts->keydbAdded = kInvalidPGPKeyDBRef;
			pt_kySetAdded = kInvalidPGPKeySetRef;
			if (IsPGPError( i_err))
				goto errJump;
		} //if (!PGPKeyDBRefIsValid( pt_kyDbAdded))

	//allow the user to import any keys pulled down from key servers
	if (PGPKeyDBRefIsValid( pt_kyDbAdded))	{
//		PGPBoolean  f;
		if (IsPGPError( i_err = PGPNewKeySet( pt_kyDbAdded, &pt_kySetAdded)))
			goto errJump;
		if (IsPGPError( i_err = PGPclImportKeys( pt_recpts->context, 
										pt_recpts->tlsContext, 
										pt_recpts->hwndParent, pt_kySetAdded, 
										pt_recpts->keydbOriginal, 
										kPGPclNoDialogForValidSingletons)))
			goto errJump;
/*		if (IsPGPError( i_err = PGPKeyDBIsUpdated( pt_recpts->keydbOriginal, 
																		&f)))
			goto errJump;
*/		if (IsPGPError( i_err = PGPFreeKeyDB( pt_kyDbAdded)))	{
			pt_kySetAdded = kInvalidPGPKeySetRef;
			pt_kyDbAdded = kInvalidPGPKeyDBRef;
		}
	} //if (PGPKeyDBRefIsValid( pt_kyDbAdded))

errJump:
	//reset to user's current preference of wanting PGP key servers searched if 
	//	encrypting to unknown keys
	if (f_resetSyncUnkKys)	{
		if (IsPGPError( i_err = PGPclPeekClientLibPrefRefs( &pt_prefs, NULL)))
			goto errJump;
		if (IsPGPError( i_error = PGPSetPrefBoolean( pt_prefs, 
											kPGPPrefKeyServerSyncUnknownKeys, 
											TRUE)) && !IsPGPError( i_err))
			i_err = i_error;
		else if (IsPGPError( i_error = PGPclFlushClientLibPrefs( pt_prefs, 
									kInvalidPGPPrefRef)) && !IsPGPError( i_err))
			i_err = i_error;
	} //if (f_resetSyncUnkKys)

	if (!IsPGPError( i_err))
		return f_abortd ? kPGPError_UserAbort : kPGPError_NoErr;

	//free up resources no longer needed
	if (PGPKeyDBRefIsValid( pt_kyDbOrig))	{
		PGPFreeKeyDB( pt_recpts->keydbOriginal);
		pt_recpts->keydbOriginal = pt_kyDbOrig;
	}
	if (PGPKeyDBRefIsValid( pt_kyDbAdded))	{
		PGPFreeKeyDB( pt_kyDbAdded);
		pt_kySetAdded = kInvalidPGPKeySetRef;
	}
	if (PGPKeySetRefIsValid( pt_kySetAdded))
		PGPFreeKeySet( pt_kySetAdded);
	if (PGPKeyDBRefIsValid( pt_recpts->keydbAdded))	{
		PGPFreeKeyDB( pt_recpts->keydbAdded);
		pt_recpts->keydbAdded = kInvalidPGPKeyDBRef;
	}

	return i_err;
} //ei_ResolveEncryptionKeys(


/** ef_FreePgpEncodeContext( ***
Free PGP resources allocated for use in the encoding activities conducted by 
the Notes Plug-In.

--- parameter & return ----
pt_ctx: Input & Output. Address of the PGP contextual information used by the 
	Notes Plug-In in conducting content encodings. A successful run of this 
	procedure invalidates the information structure.
RETURN: TRUE if successful; FALSE if the pt_ctx input is obviously invalid

--- revision history ------
6/26/03 PR
+ logic adjustment to accomodate extension to encode-context structure
+ token renaming, listing format adjustment, minor documentation adjustment

3/20/00 PR: adjustment to support PGP 7.0
9/12/99 PR: completed header documentation
12/15/98 PR: created			*/
PGPError ef_FreePgpEncodeContext( PgpEncodeContext *const  pt_ctx)	{
	PGPclRecipientDialogStruct * pt;

	if (!pt_ctx)
		return FALSE;

	if (PGPOptionListRefIsValid( pt_ctx->pgpUserOptions))
		PGPFreeOptionList( pt_ctx->pgpUserOptions);

	if (PGPKeyDBRefIsValid( (pt = &pt_ctx->t_recipients)->keydbSelected))
		PGPFreeKeyDB( pt->keydbSelected);
	if (PGPKeyDBRefIsValid( pt->keydbAdded))
		PGPFreeKeyDB( pt->keydbAdded);
	if (pt_ctx->pt_missngAdks)
		e_FreeList( &pt_ctx->pt_missngAdks, TRUE);
	PGPFreeKeyDB( pt->keydbOriginal);

	pt->context = kInvalidPGPContextRef;
	pt->tlsContext = kInvalidPGPtlsContextRef;

	return TRUE;
} //ef_FreePgpEncodeContext(


/** ei_PgpDecodeBuffer( ***
Purpose is to conduct the PGP decoding (to ASCII or binary) of the specified 
ASCII content string.

--- parameters & return ----
PUC: address of the input content to be decoded
UL: Optional. The length of the buffer to be decoded. If null, procedure 
	assumes the buffer content to be null-terminated.
f_BINARY: Flag telling whether the PGP-encoded input should be decoded as 
	binary, not ASCII, output. This drives, respectively, whether any signature 
	status shows in PGPlog or in-line in the ASCII output.
ppc_outp: Address of the pointer in which to store the address of the memory 
	buffer allocated to accommodate the null-terminated PGP-decoded output. 
	Caller is responsible for freeing this memory buffer. In case of an error, 
	the pointer is guaranteed to be null.
pul_lenOutp: Optional. Address of the variable in which to store the length 
	of the PGP-decoded output content. If passed in null, this functionality 
	will be ignored by the function.
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history -------
6/26/03 PR: casting adjustments to support maintenance of apporpriate constness

3/14/03 PR
+ minor signature change to provide appropriate constness
+ listing format adjustment, token renaming, documentation adjustment

3/20/00 PR
+ adjustment to support PGP 7.0
+ added length input to accommodate the cases where PGP does not null-terminate 
  ASCII-Armored output when encoding binary content
+ documentation improvement

9/12/99 PR
+ documentation adjustment wrt the decoding of binary content
+ minor logic enhancement

12/12/98 PR: created			*/
PGPError ei_PgpDecodeBuffer( const BYTE *const  PUC, 
								const DWORD  UL, 
								const BOOL  f_BINARY, 
								const char * *const  ppc_outp, 
								DWORD *const  pul_lenOutp)	{
	DWORD  ul_lenOutput;
	BOOL  f_tmpst;
	PGPError  i_err;

	//If any of the passed-in parameters or needed module-scope variables 
	//	are invalid, short-circuit with failure. The input buffer _may_ be of 
	//	zero length.
	PGPValidateParam( PUC && ppc_outp && mpc_PLUGIN_APP && mpc_MODULENM && 
								PGPContextRefIsValid( m_pgpContext) && 
								PGPtlsContextRefIsValid( m_pgpTlsContext));

	if (pul_lenOutp)
		*pul_lenOutp = (DWORD) NULL;

	//decode the input content
	i_err = DecryptVerifyBuffer( eh_Instance, eh_mainWnd, m_pgpContext, 
								m_pgpTlsContext, (char *) mpc_PLUGIN_APP, 
								(char *) mpc_MODULENM, (void *) PUC, UL ? UL : 
								strlen( PUC), FALSE, f_BINARY, FALSE, (char **) 
								ppc_outp, &ul_lenOutput, &f_tmpst);

	//if requested, tell the caller the length of the PGP-decoded output
	if (!IsPGPError( i_err) && pul_lenOutp)
		*pul_lenOutp = ul_lenOutput;

	return i_err;
} //ei_PgpDecodeBuffer(


/** eui_ShowInTempestWnd( ***
Show the given content in the Secure Viewer (Tempest) window.

--- parameters & return ----
PC: address of null-terminated string content to be displayed
f_WARN: tells whether a message informing of the extra security about to be 
	employed should be shown to the user
RETURN:

--- revision history -------
3/14/03 PR: created		*/
UINT eui_ShowInTempestWnd( const char  PC[], 
							const BOOL  f_WARN)	{
	return PGPscTempestViewer( m_pgpContext, eh_mainWnd, (char *) PC, 
														strlen( PC), f_WARN);
} //eui_ShowInTempestWnd(


/** ef_findPgpEncryptedBlock( ***
Locate the PGP-encrypted ASCII-Armor block within a given run of ASCII content.

--- parameters & return ----
PUC: address of content buffer to seek for a PGP-encrypted ASCII-Armor block
ul_in: Optional. Length of content buffer. If null, procedure assumes content 
	buffer is null-terminated.
ppc: Output. Address of pointer variable to receive the start address of a 
	discovered PGP-encrypted ASCII-Armor block.
pul: Optional Output. Address of variable to receive the length of the 
	discovered block. If null, output is suppressed.
RETURN: ??

--- revision history -------
3/14/03 PR: created		*/
//DOC!!
BOOL ef_findPgpEncryptedBlock( const char  PUC[], 
								ULONG  ul_in, 
								const char * *const  ppc, 
								ULONG *const  pul)	{
	ULONG  ul_strt, ul_end;

	PGPValidateParam( PUC && (!ul_in ? *PUC : TRUE) && ppc);

	*ppc = NULL;
	if (pul)
		*pul = (ULONG) NULL;

	if (!ul_in)
		ul_in = strlen( PUC);

	if (!FindEncryptedBlock( PUC, ul_in, &ul_strt, &ul_end))
		return FALSE;

	if (pul)
		*pul = ul_end - ul_strt + 1;
	*ppc = PUC + ul_strt;

	return TRUE;
} //ef_findPgpEncryptedBlock(


/** e_FreePgpMem( ***
Wrapper procedure that allows memory objects allocated by PGP functions to be 
freed as required by the PGP SDK.

--- parameter -----------
puc: address of the memory object to be freed

--- revision history ----
9/6/02 PR: minor documentation adjustment
11/25/98 PR: created		*/
void e_FreePgpMem( BYTE *const  puc)	{
	PGPFreeData( puc);
} //e_FreePgpMem(


/** e_ReleasePgpContext( ***
Purpose is to release all general resources involved with providing PGP 
functionality to this plug-in.

--- revision history -------
3/20/00 PR: adjustment to support PGP 7.0
11/20/98 PR: created		*/
void e_ReleasePgpContext()	{
	if (!PGPContextRefIsValid( m_pgpContext))
		return;

	PGPFreeTLSContext( m_pgpTlsContext);
	m_pgpTlsContext = kInvalidPGPtlsContextRef;
	PGPclCloseLibrary();
	m_pgpContext = kInvalidPGPContextRef;
} //e_ReleasePgpContext(


/** ei_TestForPgpAscii( ***
Purpose is to carry out a test of whether PGP-encoded ASCII is present in the 
passed-in content.

--- parameters & return ----
PC: pointer to the content to be tested for PGP-encoded ASCII
pf_hasPgpAscii: Output. Pointer to the variable to set to indicate whether 
	PGP-encoded ASCII is present in the passed-in content.
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history -------
6/26/03 PR: casting adjustments to support maintenance of apporpriate constness

9/6/02 PR
+ logic enhancment to provide workarounds to
  - "bad packet" PGP error when extended international characters are sent 
    through PGP decoding engines prior to PGP clients 7.1
  - "unknown version" and "bad packet" PGP errors that can occur when content 
    starting with the character 'Ä' is sent through the decoding engine

3/20/00 PR: adjustment to support PGP 7.0
9/12/99 PR: minor logic enhancement
11/23/98 PR: created		*/
PGPError ei_TestForPgpAscii( const char *const  PC, 
								BOOL *const  pf_hasPgpAscii)	{
	PgpBasicInput  t_basics;
	SpecialPlugInTaskInfo  t_task;
	BOOL  f_TestSuccessful;
	const char * pc = PC;
	PGPError  i_err;

	//if any of the passed-in parameters are invalid, indicate the problem 
	//	to user
	PGPValidateParam( PC && pf_hasPgpAscii);

	//if the beginning character of conetnt is an Ä character (0xC4), find the 
	//	first ensuing chracter that isn't, and start the decoding from there so 
	//	we can avoid the "unknown version" or "bad packet" errors the decoding 
	//	engine may throw because fooled into thinking that the content is 
	//	binary encoded content
	while (*pc == 'Ä')
		pc++;

	//fill in a structure of basic information used by PGP functions 
	//	downstream
	t_basics.h_Instance = eh_Instance;
	t_basics.h_wndMain = eh_mainWnd;
	t_basics.pgpContext = m_pgpContext;
	t_basics.pgpTlsContext = m_pgpTlsContext;
	t_basics.pc_AppNm = (char *) mpc_PLUGIN_APP;
	t_basics.pc_ModuleNm = (char *) mpc_MODULENM;

	//fill in the structure of information describing the special task we 
	//	wish the PGP decoding mechanism to carry out
	t_task.i_Task = mi_TEST_FOR_PGP_ASCII;
	t_task.pv_ctx = pf_hasPgpAscii;
	t_task.pf_Successful = &f_TestSuccessful;

	//decode the input content while executing our special task
	i_err = i_DecryptVerifyBuffer( &t_basics, pc, strlen( pc), FALSE, FALSE, 
											FALSE, t_task, NULL, NULL, NULL);

	if (f_TestSuccessful)	{
		_ASSERTE( i_err == kPGPError_UserAbort);
		i_err = kPGPError_NoErr;
	}else if (kPGPError_BadPacket == i_err && (PGPVERSIONMAJOR < 7 ||  
								PGPVERSIONMAJOR == 7 && PGPVERSIONMINOR < 1))
		i_err = kPGPError_NoErr;

	return i_err;
} //ei_TestForPgpAscii(


/** i_PgpAsciiCoordsOrTempest( ***
If user who encoded the content did not request use of the Tempest viewer when 
decoding, procedure obtains coordinate information about the first PGP-encoded 
ASCII block in the content. Else decode all the content for the Tempest display.

--- parameters & return ----
ppc: Input & Output. Address of the pointer to the string of content to be 
	analyzed for PGP-encoded ASCII. The pointer may be moved because an extra 
	two CRLFs will be appended to the content, potentially forcing a 
	reällocation of the content buffer. Also, the content itself will change 
	slightly internally if the workaround to the Outlook Express plug-in bug of 
	a malformed end-PGP ASCII-Armor line is required.
ul_LEN_INP: the length of the null-terminated content buffer
pul_offstBgn: Output. Address of the variable in which to store the one-based 
	offset from the beginning of the content string to the character after 
	which the first line of a PGP-encoded ASCII block begins.
pul_offstEnd: Output. Address of the variable in which to store the one-based 
	offset from the beginning of the content string to the end of the last line 
	of the initial PGP-encoded ASCII block, including any existing CRLF. 
	"Initial" because the block will shorten by one character if the workaround 
	to the Outlook Express plug-in bug of a malformed end-PGP ASCII-Armor line 
	is required.
ppc_outp: Optional Output. Address of the pointer variable in which to store 
	the address of a buffer containing the null-terminated, decoded version of 
	the input content. Output will only be filled if the user who encoded the 
	input content requested that the Tempest viewer be used when the content is 
	decoded. If null, no Tempest checking will be done.
pui_lenOutp: Optional Output. Address of the variable in which to store the 
	length of the decoded version of the input content. Output will only be 
	filled if the user who encoded the input content requested that the Tempest 
	viewer be used when the content is decoded. If null, output will be ignored.
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- suggested enhancement ----
9/6/02 PR: if Outlook-Express workaround continues to be needed, make 
	before-line analysis more compatible with what the PGP decoding engine can 
	handle (preceding whitespace, '>', etc.; if snippet characters are present 
	[like '>'], they should be the same for the begin & end lines of the block)

--- revision history -------
6/26/03 PR: casting adjustments to support maintenance of apporpriate constness
3/14/03 PR: name & signature adjustment and associated logic enhancement in 
	support of enabling the Tempest viewer in the plug-in

9/6/02 PR
+ logic enhancment to provide workarounds to
  - "bad packet" PGP error when extended international characters are sent 
    through PGP decoding engines prior to PGP clients 7.1
  - "unknown version" and "bad packet" PGP errors that can occur when content 
    starting with the character 'Ä' is sent through the decoding engine
  - the Outlook Express plug-in bug where an extra space can be included in the 
    end-PGP ASCII-Armor line
+ listing format adjustment, documentation adjustment

3/20/00 PR: adjustment to support PGP 7.0

9/12/99 PR
+ code adjustment to handle content offsets in a consistent manner
+ documentation adjustment
+ minor logic enhancement

11/23/98 PR: created		*/
static PGPError i_PgpAsciiCoordsOrTempest( char * *const  ppc, 
											const DWORD  ul_LEN_INP, 
											DWORD *const  pul_offstBgn, 
											DWORD *const  pul_offstEnd, 
											char * *const  ppc_outp, 
											int *const  pui_lenOutp)	{
	static const char  pc_PGPBLK_BEGIN[] = "-----BEGIN PGP ", 
						pc_PGPBLK_END_BAD[] = "-----END  PGP ", 
						pc_PGPBLK_END_B4_BAD[] = "-----END";

	DWORD  ul;
	PgpBasicInput  t_basics;
	SpecialPlugInTaskInfo  t_task;
	BOOL  f_TaskSuccessful, f_tmpst;
	PgpBlockCoordInfo  t_coordInfo;
	char * pc, * pc2, * pc_bgn = NULL;
	PGPError  i_err;

	//if any of the passed-in parameters are invalid, short-circuit with failure
	PGPValidateParam( ppc && *ppc && ul_LEN_INP > 1 && pul_offstBgn && 
																pul_offstEnd);

	//initialize the output offset variables to a default value indicating that 
	//	no offset was found
	*pul_offstBgn = *pul_offstEnd = ei_NOT_FOUND;

	//Append a couple CRLFs to the input content to ensure that the content 
	//	does not end precisely with the end of a PGP-encoded ASCII block. This 
	//	measure is neccessary to allow our override of the PGP-decode callback 
	//	function to inform the i_DecryptVerifyBuffer() procedure that the task 
	//	of locating coördinates was successful. The measure would not have been 
	//	necessary if the kPGPEvent_FinalEvent pass in the PGP-decode callback 
	//	recognized the callback's return value. If it did recoginize the return 
	//	value, a combination of kPGPEvent_NullEvent and kPGPEvent_FinalEvent 
	//	could be used to determine the coördinates of a content-ending 
	//	PGP-encoded block.
	if (!( *ppc = realloc( *ppc, ul = ul_LEN_INP + 2 * eui_LEN_CRLF)))
		return !kPGPError_NoErr;
	strcat( strcat( *ppc, epc_CRLF), epc_CRLF);

	//seek evidence that an Outlook Express plug-in bug, a malformed end-PGP 
	//	ASCII-Armor line, may be present...
	pc = pc2 = *ppc;
	while (pc = strstr( pc, pc_PGPBLK_END_BAD))	{
		//if the bug has very likely been encountered, rectify the end line so 
		//	it will pass through the PGP decoding engine properly, and abort 
		//	further seeking
		if (pc == *ppc || memcmp( pc - eui_LEN_CRLF, epc_CRLF, eui_LEN_CRLF) != 
																ei_SAME)	{
			pc++;
			continue;
		}
		while ((pc2 = strstr( pc2, pc_PGPBLK_BEGIN)) && pc2 < pc)	{
			if (pc2 == *ppc || memcmp( pc2 - eui_LEN_CRLF, epc_CRLF, 
													eui_LEN_CRLF) == ei_SAME)
				pc_bgn = pc2;
			pc2++;
		}
		if (pc_bgn)	{
			pc += sizeof( pc_PGPBLK_END_B4_BAD) - 1;
			memmove( pc, pc + 1, *ppc + ul - (pc + 1));
			ul--;
			break;
		}

		//loop to see if another candidate further on is available
		pc++, pc_bgn = NULL;
	} //while (pc = strstr( pc, pc_PGPBLK_END_BAD))

	//if the beginning character of conetnt is an Ä character (0xC4), find the 
	//	first ensuing chracter that isn't, and start the decoding from there so 
	//	we can avoid the "unknown version" or "bad packet" errors the decoding 
	//	engine may throw because fooled into thinking that the content is 
	//	binary encoded content
	pc = *ppc;
	while (*pc == 'Ä')
		pc++, ul--;

	//fill in a structure of basic information used by PGP functions downstream
	t_basics.h_Instance = eh_Instance;
	t_basics.h_wndMain = eh_mainWnd;
	t_basics.pgpContext = m_pgpContext;
	t_basics.pgpTlsContext = m_pgpTlsContext;
	t_basics.pc_AppNm = (char *) mpc_PLUGIN_APP;
	t_basics.pc_ModuleNm = (char *) mpc_MODULENM;

	//fill in the structure of information describing the special task we wish 
	//	the PGP decoding mechanism to carry out
	t_task.i_Task = mi_GET_PGP_BLOCK_COORDS;
	t_task.pv_ctx = &t_coordInfo;
	t_task.pf_Successful = &f_TaskSuccessful;

	//decode the input content while executing our special task
	i_err = i_DecryptVerifyBuffer( &t_basics, pc, ul, FALSE, FALSE, FALSE, 
									t_task, ppc_outp, pui_lenOutp, &f_tmpst);

	//If the task was successfully completed, set the offset outputs 
	//	accordingly, accounting if necessary for the trailing CRLFs appended to 
	//	the input content and any workaround done to avoid the Outlook Express 
	//	plug-in bug. Remember, ul_LEN_INP sizes the null-terminated buffer, not 
	//	the contained string.
	if (f_TaskSuccessful)	{
		if (!t_coordInfo.f_tmpst)	{
			*pul_offstBgn = t_coordInfo.ul_offsetBegin + (pc - *ppc);
			*pul_offstEnd = t_coordInfo.ul_offsetEnd + (pc - *ppc) + !!pc_bgn;
			*pul_offstEnd -= *pul_offstEnd - eui_LEN_CRLF == ul_LEN_INP - 1 ? 
													eui_LEN_CRLF : (DWORD) NULL;
			_ASSERTE( i_err == kPGPError_UserAbort);
			i_err = kPGPError_NoErr;
		} //if (!t_coordInfo.f_tmpst)
	}else if (kPGPError_BadPacket == i_err && (PGPVERSIONMAJOR < 7 ||  
								PGPVERSIONMAJOR == 7 && PGPVERSIONMINOR < 1))
		i_err = kPGPError_NoErr;

	return i_err;
} //i_PgpAsciiCoordsOrTempest(


/** ei_FindAndDecodePgpBlock( ***
Attempts to locate a PGP block within the given ASCII input content. If one is 
found, the block is decoded and its content returned to the caller.

--- parameters & return ----
ppc_inp: Input & Output. Address of the pointer to the string of content to be 
	analyzed for PGP-encoded ASCII. The pointer may be moved because an extra 
	two CRLFs will be appended to the content, potentially forcing a 
	reallocation (move) of the content buffer. Furthermore, the content may 
	undergo a slight shift to work around the Outlook Express plug-in bug of a 
	malformed end-PGP ASCII-Armor line.
ul_LEN_INP: the length of the null-terminated content buffer
pf_tmpst: Input & Output. Address of flag variable telling on input whether 
	user wants decodings viewed using the Tempest viewer (TRUE). If user has 
	not instructed this (FALSE), procedure will set variable to TRUE if the 
	user who encoded the conent requested that the Tempest viewer be used upon 
	decodeing.
ppc_blk: Optional Output. Address of the pointer variable in which to store the 
	address at which the first PGP block was located within the input content. 
	If address is null, procedure will ignore this functionality.
pul_lenBlk: Optional Output. Address of the variable in which to store the 
	length of the initial PGP block found within the input content. "Initial" 
	because block will shorten by one character if a workaround to the Outlook 
	Express plug-in bug of a malformed end-PGP ASCII-Armor line is required. If 
	address is null, procedure will ignore this functionality.
ppc_outp: Output. Address of the pointer variable in which to store the address 
	of the buffer containing the null-terminated, decoded version of either the 
	first PGP block found within the input content (if Tempest viewing has not 
	been slated) or of the entiere input content (if Tempest viewing has been 
	slated).
pui_lenOutp: Optional Output. Address of the variable in which to store the 
	length of the decoded version of either the first PGP block found within 
	the input content (if Tempest viewing has not been slated) or of the 
	entiere input content (if Tempest viewing has been slated). If address is 
	null, procedure will ignore this functionality.
RETURN: kPGPError_NoErr if no error occurred; the PGP error code otherwise.

--- revision history -------
3/14/03 PR: signature adjustment and associated logic enhancement in support of 
	enabling the Tempest viewer in the plug-in
11/13/02 PR: logic improvement to fix bug that could cause client crash if 
	passphrases aren't being cached
10/30/02 PR: listing format adjustment, minor logic improvement

9/6/02 PR
+ added message-box notification when PGP oddly doesn't find an expected PGP 
  block to decode (as it might with HTML messages sent from Outlook Express 
  where a PGP-encrypted block contains only spaces)
+ logic adjustment to accommodate workaround to Outlook Express plug-in bug of 
  a malformed end-PGP ASCII-Armor line
+ listing format adjustment, documentation adjustment, token renaming

3/20/00 PR
+ adjustment to support PGP 7.0
+ accommodated change in signature to ei_PgpDecodeBuffer()
+ completed standard documentation

9/26/99 PR: worked around false-error-return bug in PGPclCloseClientPrefs()
9/12/99 PR: "Bug" fixes related to returned offsets, lengths. More really the 
	enforcement of a system-wide standard with respect to offsets.
1/3/99 PR: created			*/
PGPError ei_FindAndDecodePgpBlock( char * *const  ppc_inp, 
									const DWORD  ul_LEN_INP, 
									BOOL *const  pf_tmpst, 
									char * *const  ppc_blk, 
									DWORD *const  pul_lenBlk, 
									char * *const  ppc_outp, 
									int *const  pui_lenOutp)	{
	static const char  c_PGPBLK_END_LAST_CHAR = '-', 
						pc_PGPBLK_END_LAST_CHARS[] = "-\r\n";

	int  ui_lenOutp;
	PGPBoolean	f_CachePassphrase = TRUE;
	PGPPrefRef	pgpPrefs = kInvalidPGPPrefRef;
	PGPError  i_err;

	if (!( ppc_inp && *ppc_inp && ul_LEN_INP > 1 && ppc_outp))
		return !kPGPError_NoErr;

	//default any return variables to null
	if (ppc_blk)
		*ppc_blk = NULL;
	if (pul_lenBlk)
		*pul_lenBlk = (DWORD) NULL;
	*ppc_outp = NULL;
	if (pui_lenOutp)
		*pui_lenOutp = (int) NULL;

	//if the user hasn't instructed that all decodings should be shown via the 
	//	Tempest viewer...
	if (!*pf_tmpst)	{
		PGPUInt32  ui;
		DWORD  ul_offstBgn, ul_offstEnd;
		char * pc_blk;
		PGPBoolean  f_OutlookExpBugShift;

		//determine whether the user has it set that her decryption passphrase 
		//	should be cached
		if (IsPGPError( i_err = PGPclPeekClientLibPrefRefs( &pgpPrefs, NULL)))
			return i_err;
		if (IsPGPError( i_err = PGPGetPrefNumber( pgpPrefs, kPGPPrefCacheType, 
																		&ui)))
			return i_err;
		f_CachePassphrase = ui != kPGPPrefCacheTypeNone;

		//If the user does not want her passphrase cached, we need to override 
		//	her wishes very temporarily so that she won't be prompted twice to 
		//	enter her passphrase if this is an encrypted message. Twice because 
		//	one pass is made to get the coordinates of the PGP ASCII-Armored 
		//	block in the message, and a second time to do the actual 
		//	decryption. If the kPGPError_SkipSection return from PGPDecode()'s 
		//	callback function worked properly, this fudge would not be 
		//	neccessary...
		if (!f_CachePassphrase)	{
			//toggle on decrypt-passphrase caching
			if (IsPGPError( i_err = PGPSetPrefNumber( pgpPrefs, 
													kPGPPrefCacheType, 
													kPGPPrefCacheTypeLogon)))
				return i_err;

			//save the preferences file
			if (IsPGPError( i_err = PGPclFlushClientLibPrefs( pgpPrefs, 
														kInvalidPGPPrefRef)))
				goto cleanUp;
		} //if (!f_CachePassphrase)

		//determine the coordinates within the textual content of the 
		//	PGP-encoded block
		if (IsPGPError( i_err = i_PgpAsciiCoordsOrTempest( ppc_inp, ul_LEN_INP, 
													&ul_offstBgn, &ul_offstEnd, 
													ppc_outp, &ui_lenOutp)))
			goto cleanUp;

		//if the user who encoded the content instructed that the Tempest 
		//	viewer should be used on decoding, inform caller of this and then 
		//	short-circuit silently
		if (*ppc_outp)	{
			*pf_tmpst = TRUE;
			goto cleanUp;
		}

		//if no PGP-encoded ASCII block was found, inform user and then 
		//	short-circuit silently
		if (ul_offstEnd == (DWORD) ei_NOT_FOUND)	{
			MessageBox( eh_mainWnd, "No PGP block was found.", epc_APPNM, 
													MB_OK | MB_ICONEXCLAMATION);
			goto cleanUp;
		} //if (ui_lenOutp)

		//determine the starting point of the block to be decoded, accounting 
		//	if necessary for any workaround used to avoid the Outlook Express 
		//	plug-in bug of a malformed end-PGP ASCII-Armor line
		f_OutlookExpBugShift = !(*(*ppc_inp + ul_offstEnd - 1) == 
									c_PGPBLK_END_LAST_CHAR || strncmp( 
									*ppc_inp + ul_offstEnd - 1 - eui_LEN_CRLF, 
									pc_PGPBLK_END_LAST_CHARS, sizeof( 
									pc_PGPBLK_END_LAST_CHARS) - 1) == ei_SAME);
		_ASSERTE( !f_OutlookExpBugShift || *(*ppc_inp + ul_offstEnd - 2) == 
									c_PGPBLK_END_LAST_CHAR || strncmp( 
									*ppc_inp + ul_offstEnd - 2 - eui_LEN_CRLF, 
									pc_PGPBLK_END_LAST_CHARS, sizeof( 
									pc_PGPBLK_END_LAST_CHARS) - 1) == ei_SAME);
		pc_blk = *ppc_inp + ul_offstBgn;

		//decode just the portion containing the PGP-encoded block
		if (IsPGPError( i_err = ei_PgpDecodeBuffer( pc_blk, ul_offstEnd - 
											f_OutlookExpBugShift - ul_offstBgn, 
											FALSE, ppc_outp, &ui_lenOutp)))
			goto cleanUp;

		if (ppc_blk)
			*ppc_blk = pc_blk;
		if (pul_lenBlk)
			*pul_lenBlk = ul_offstEnd - ul_offstBgn;
	//else decode the entire input buffer since viewing by Tempest viewer has 
	//	been slated
	}else
		i_err = ei_PgpDecodeBuffer( *ppc_inp, ul_LEN_INP, FALSE, ppc_outp, 
																&ui_lenOutp);

cleanUp:
	if (pui_lenOutp && !IsPGPError( i_err))
		*pui_lenOutp = ui_lenOutp;

	//if we temporarily toggled caching of the decryption passphrase on...
	if (!f_CachePassphrase)	{
		PGPError  i_error;

		//purge the cached passphrase, presuming there is one
		if (IsPGPError( i_error = PGPPurgePassphraseCache( m_pgpContext)) && 
															!IsPGPError( i_err))
			i_err = i_error;

		//save the preferences file with caching turned off
		if (IsPGPError( i_error = PGPclPeekClientLibPrefRefs( &pgpPrefs, NULL)) 
														&& !IsPGPError( i_err))
			i_err = i_error;
		if (!i_error && IsPGPError( i_error = PGPSetPrefNumber( pgpPrefs, 
												kPGPPrefCacheType, 
												kPGPPrefCacheTypeNone)))	{
			if (!IsPGPError( i_err))
				i_err = i_error;
		}else if (IsPGPError( i_error = PGPclFlushClientLibPrefs( pgpPrefs, 
									kInvalidPGPPrefRef)) && !IsPGPError(i_err))
			i_err = i_error;
	} //if (!f_CachePassphrase)

	return i_err;
} //ei_FindAndDecodePgpBlock(


/** ef_alwaysUseTempest( ***
Inform caller whether the PGP toggle (preference) is on or off as to whether 
the Tempest viewer (Secure Viewer) should be used when displaying decoded 
content.

--- return --------------
RETURN: TRUE if user wants the Tempest viewer used when showing decoded 
	content, FALSE otherwise

--- revision history ----
3/14/03 PR: created			*/
BOOL ef_alwaysUseTempest()	{
	return PGPscGetSecureViewerPref( m_pgpContext);
} //ef_alwaysUseTempest(


/** xs_AutomaticDecryptVerify( ***
Inform caller whether the PGP automatic decrypt/verify toggle (preference) is 
on or off.

--- return --------------
RETURN: VisualBasic True if the automatic decrypt/verify toggle is on; 
	VisualBasic False if the toggle is off.

--- revision history ----
3/14/03 PR: listing format adjustment, minor documentation adjustment
3/20/00 PR: adjustment to support PGP 7.0
11/18/98 PR: created		*/
short xs_AutomaticDecryptVerify()	{
	return AutoDecrypt( PGPPeekContextMemoryMgr( m_pgpContext)) ? 
													ms_VB_TRUE : ms_VB_FALSE;
} //xs_AutomaticDecryptVerify(


/** xs_DefaultSign( ***
Inform caller whether the PGP toggle (preference) for default signing messages 
is on or off.

--- return --------------
RETURN: VisualBasic True if the default-PGP sign toggle is on; VisualBasic 
	False if the toggle is off.

--- revision history ----
3/14/03 PR: listing format adjustment, minor documentation adjustment
3/20/00 PR: adjustment to support PGP 7.0
12/9/98 PR: created			*/
short xs_DefaultSign()	{
	return ByDefaultSign( PGPPeekContextMemoryMgr( m_pgpContext)) ? 
													ms_VB_TRUE : ms_VB_FALSE;
} //xs_DefaultSign(


/** xs_DefaultEncrypt( ***
Inform caller whether the PGP toggle (preference) for default encrypting 
messages is on or off.

--- return --------------
RETURN: VisualBasic True if the default-PGP encrypt toggle is on; VisualBasic 
	False if the toggle is off.

--- revision history ----
3/14/03 PR: listing format adjustment, minor documentation adjustment
3/20/00 PR: adjustment to support PGP 7.0
12/9/98 PR: created			*/
short xs_DefaultEncrypt()	{
	return ByDefaultEncrypt( PGPPeekContextMemoryMgr( m_pgpContext)) ? 
													ms_VB_TRUE : ms_VB_FALSE;
} //xs_DefaultEncrypt(


/** i_DecryptVerifyBuffer( ***
Purpose is to emulate the DecryptVerifyBuffer() function in the shared-code PGP 
module DecryptVerify.c, enhanced to handle special tasks required by this 
plug-in in conjunction with decoding PGP content and pared down internally to 
remove unnecessary complication given the current capabilities of the Notes 
Plug-In.

--- parameters & return ----
pt_BASICS: address of structure containing general contextual information 
	needed by this procedure and procedures downstream
pc_INPUT: address of the content to be PGP decoded
ul_LEN_INPUT: the length of the content to be PGP decoded
f_MIME: Flag telling whether the input message-content is in PGP/MIME format. 
	Since the Notes Plug-In does not yet support this native format, this input 
	is ignored by the procedure.
f_BINARY: flag telling whether the decoded content will be treated as being 
	binary in nature
f_MARKUP: Flag telling whether the encoded input, and therefore decoded output 
	should be treated as marked-up text, meaning either HTML or Microsoft RTF. 
	Since the Notes Plug-In does not yet support either of these native 
	formats, this input is ignored by the procedure.
t_TASK: information structure identifying and possibly describing any special 
	plug-in task associated with this decoding event
ppv_output: Optional Output. Address of the pointer in which to return the 
	memory address of the PGP-decoded content. Memory will be allocated by 
	this procedure to hold the content; CALLER IS RESPONSIBLE for freeing the 
	allocation. Passing this parameter in as null indicates that this procedure 
	will dispose of any decoded content itself.
pui_lenOutput: Optional Output. Address of the variable in which to store the 
	length of the PGP-decoded ASCII content. Output is used only if the decoded 
	output is to be returned to the caller (i.e. the ppc_output parameter is 
	passed in non-null).
pf_tmpst: Optional Output. Address of variable in which to store whether the 
	decoded output should be displayed in the Tempest viewer. Only required if 
	the decoded output is to be returned to the caller (i.e. the ppc_output 
	parameter is passed in non-null).
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history -------
6/26/03 PR: accommodated extension to OUTBUFFLIST by Bill Blanke
3/14/03 PR: listing format adjustment
10/30/02 PR: minor documentation adjustment, listing format adjustment
9/6/02 PR: listing format adjustment
7/2/01 PR: removed remmed-out code
3/20/00 PR: adjustment to support PGP 7.0
9/12/99 PR: minor logic improvement in handling ppc_output parameter
12/12/98 PR: created		*/
static PGPError i_DecryptVerifyBuffer( const PgpBasicInput *const  pt_BASICS, 
										const char *const  pc_INPUT, 
										const DWORD  ul_LEN_INPUT, 
										const BOOL  f_MIME, 
										const BOOL  f_BINARY, 
										const BOOL  f_MARKUP, 
										SpecialPlugInTaskInfo  t_task, 
										void * *const  ppv_output, 
										PGPSize *const pui_lenOutput,
										BOOL *const  pf_tmpst)	{
	const PGPContextRef  pgpCONTEXT = pt_BASICS ? pt_BASICS->pgpContext : NULL;

	void * pv_output = NULL;
	BOOL  f_tmpst, f_kysOnly;
	PGPSize  ui_lenOutput;
	PGPOptionListRef  options = NULL;
	VerificationBlock *	pt_block = NULL, * pt_tmpBlock, * pt_indxBlock;
	PGPMemoryMgrRef  t_memoryMgr;
	OUTBUFFLIST * nobl, * obl = NULL;
	PGPError  i_err, i_error;

	PGPValidateParam( pt_BASICS && pc_INPUT && pt_BASICS->pc_AppNm && 
								pt_BASICS->pc_ModuleNm && PGPContextRefIsValid( 
								pgpCONTEXT) && PGPtlsContextRefIsValid( 
								pt_BASICS->pgpTlsContext));

	if (ppv_output)
		*ppv_output = NULL;
	if (pui_lenOutput)
		*pui_lenOutput = (PGPSize) NULL;
	if (pf_tmpst)
		*pf_tmpst = FALSE;

	t_memoryMgr = PGPPeekContextMemoryMgr( pgpCONTEXT);

	if (f_BINARY)
		i_err = PGPBuildOptionList( pgpCONTEXT, &options, PGPOInputBuffer( 
										pgpCONTEXT, pc_INPUT, ul_LEN_INPUT), 
										PGPOAllocatedOutputBuffer( pgpCONTEXT, 
										&pv_output, INT_MAX, &ui_lenOutput), 
										PGPOLastOption( pgpCONTEXT));
	else
		i_err = PGPBuildOptionList( pgpCONTEXT, &options, PGPOInputBuffer( 
										pgpCONTEXT, pc_INPUT, ul_LEN_INPUT), 
										PGPOLastOption( pgpCONTEXT));

	if (IsPGPError( i_err))	{
		DisplayPgpError( __FILE__, __LINE__, pt_BASICS->pc_ModuleNm, i_err);
		goto cleanUp;
	}

	if (!f_BINARY)
		pt_block = (VerificationBlock *) PGPNewData( t_memoryMgr,
													sizeof( VerificationBlock),
													kPGPMemoryMgrFlags_Clear);

	//Decode the buffer. The last parameter (output) is special to this module, 
	//	not found in model procedure in shared DecryptVerify.c.	
	i_err = i_DecryptVerify( pt_BASICS, options, f_MIME, t_task, pt_block, 
															TRUE, &f_kysOnly);

	//if we're dealing with regular ASCII decoding...
	if (!f_BINARY)
		//if a PGP error was thrown (if a special plug-in task was involved and 
		//	one was thrown, it was probably done on purpose) or if the caller 
		//	doesn't want or need ASCII output, free all the "verification" 
		//	blocks allocated to this decoding attempt, and skip past the rest 
		//	of this procedure's processing
		if (IsPGPError( i_err) || !ppv_output || f_kysOnly)
			do {
				if (pt_block->pOutput)
					PGPFreeData( pt_block->pOutput);
				if (pt_block->szBlockBegin)
					PGPFreeData( pt_block->szBlockBegin);
				if (pt_block->szBlockEnd)
					PGPFreeData( pt_block->szBlockEnd);

				pt_tmpBlock = pt_block;
				pt_block = pt_block->next;
				PGPFreeData( pt_tmpBlock);
			} while (pt_block);
		//else carry out the normal procedure for putting together the decoded 
		//	content
		else	{
			//arrange together the pieces of decoded content
			pt_indxBlock = pt_block;
			do	{
				if (pt_indxBlock->szBlockBegin)	{
					nobl = PGPscMakeOutBuffItem( &obl);
					nobl->bHeadFoot = TRUE;
					nobl->pBuff = pt_indxBlock->szBlockBegin;
					nobl->dwBuffSize = strlen( pt_indxBlock->szBlockBegin);
				}

				if (pt_indxBlock->pOutput)	{
					nobl = PGPscMakeOutBuffItem( &obl);
					nobl->pBuff = pt_indxBlock->pOutput;
					nobl->dwBuffSize = strlen( pt_indxBlock->pOutput);
					nobl->FYEO = pt_indxBlock->FYEO;
				}

				if (pt_indxBlock->szBlockEnd)	{
					nobl = PGPscMakeOutBuffItem( &obl);
					nobl->bHeadFoot = TRUE;
					nobl->pBuff = pt_indxBlock->szBlockEnd;
					nobl->dwBuffSize = strlen( pt_indxBlock->szBlockEnd);
				}

				pt_tmpBlock = pt_indxBlock;
				pt_indxBlock = pt_indxBlock->next;
				PGPFreeData( pt_tmpBlock);
			} while (pt_indxBlock);

			//copy & concatenate all the pieces into a buffer to be returned to 
			//	caller
			PGPscConcatOutBuffList( (void *) pgpCONTEXT, obl, (char **) 
													&pv_output, &ui_lenOutput, 
													&f_tmpst);
		} //if (IsPGPError( i_err) || !ppv_output || f_kysOnly)

	if (pf_tmpst)
		*pf_tmpst = f_tmpst;
	if (pui_lenOutput)
		*pui_lenOutput = ui_lenOutput;
	if (ppv_output)
		*ppv_output = pv_output;

cleanUp:
	if (!ppv_output && pv_output)
		if (IsPGPError( i_error = PGPFreeData( pv_output)) && 
															!IsPGPError( i_err))
			i_err = i_error;
	if (options)
		if (IsPGPError( i_error = PGPFreeOptionList( options)) && 
															!IsPGPError( i_err))
			i_err = i_error;

	return i_err;
} //i_DecryptVerifyBuffer(


/** DisplayPgpError( ***
Emulates the DisplayPgpError() function in shared-code PGP module 
DecryptVerify.c.

--- parameters ---------
pc_FILE: pointer to the name of the current source file being run
i_LINE: the line-number of the calling function in the source file being run
pc_MODULE: pointer to the name of the run-time module containing the current 
	source file
i_errCode: the code of the error being reported in this function

--- revision history ------
9/6/02 PR: token renaming
11/20/98 PR: created		*/
static void DisplayPgpError( const char *const  pc_FILE, 
								const int  i_LINE, 
								const char *const  pc_MODULE, 
								int  i_errCode)	{
	char pc_msg[255];

	if (i_errCode == kPGPError_BadPacket)
		i_errCode = kPGPError_CorruptData;

	PGPclEncDecErrorToString( i_errCode, pc_msg, sizeof( pc_msg) - 1);

#ifdef _DEBUG
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW);
	_CrtDbgReport( _CRT_ERROR, pc_FILE, i_LINE, pc_MODULE, pc_msg);
#endif

	MessageBox( eh_mainWnd, pc_msg, pc_MODULE, MB_ICONEXCLAMATION);
} //DisplayPgpError(


/** i_DecryptVerify( ***
Emulates the DecryptVerify() function in the shared-code PGP module 
DecryptVerify.c, enhanced mainly to override the DecodeEventHandler() callback 
function in DecryptVerify.c in order to handle special tasks required by this 
plug-in in conjunction with decoding PGP content, and enhanced so that user is 
notified if any unanticipated errors occur when importing keys found during the 
decoding.

--- parameters & return ----
pt_BASICS: address of structure containing general contextual information 
	needed by this procedure and procedures downstream
options: Opaque PGP storage needed by procedures downstream. Includes somehow 
	the input message-content buffer.
f_MIME: flag telling whether the input message-content is in PGP/MIME format
t_TSK: information structure identifying and possibly describing any special 
	plug-in task associated with this decoding event
pt_blk: Output. Address of the PGP information structure describing obtained 
	output from a decoding event.
f_FORCE_IMPRT_FND_KYS: flag telling whether this procedure should import 
	any keys obtained via the decoding
pf_kysOnly: Optional Output. Address of the flag in which to store whether the 
	decoding resulted in the user importing public key(s). If address is null, 
	this functionality is ignored.
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history -------
6/26/03 PR: token renaming, listing format adjustment, minor documentation 
	adjustment

3/14/03 PR
+ updated logic to reflect recent change to model procedure DecryptVerify() in 
  shared PGP client code
+ listing format adjustment, minor documentation adjustment

9/6/02 PR
+ logic enhancment to provide workaround to "bad packet" PGP error when 
  extended international characters are sent through PGP decoding engines prior 
  to PGP clients 7.1
+ listing format adjustment

8/9/00 PR: accommodated recent change made to the DecodeEventData structure 
	utilized in the PGP shared-code module DecryptVerify.c, whose functionality 
	is somewhat reproduced here so certain efficiencies can be gained

3/20/00 PR
+ adjustment to support PGP 7.0
+ decommissioned "SendMessage" workaround for the PGP 6.0x problem (fixed in 
  PGP 6.5) with the destruction of the progress dialog, a workaround which 
  itself began with Notes R5 to cause an invalid-pointer error when closing 
  Notes

9/12/99 PR: added the pf_keyDecoding flag output functionality
11/23/98 PR: created		*/
static PGPError i_DecryptVerify( const PgpBasicInput *const  pt_BASICS,
									PGPOptionListRef  options,
									const BOOL  f_MIME, 
									const SpecialPlugInTaskInfo  t_TSK, 
									VerificationBlock *const  pt_blk, 
									const BOOL  f_FORCE_IMPRT_FND_KYS, 
									BOOL *const  pf_kysOnly)	{
	static const int  CONTENTS_ENCRYPTED = 0x01, CONTENTS_SIGNED = 0x02;

	const PGPContextRef  pt_CTX = pt_BASICS ? pt_BASICS->pgpContext : NULL;

	PGPKeyDBRef  pt_kyDb = kInvalidPGPKeyDBRef, pt_kyDbNew = 
															kInvalidPGPKeyDBRef;
	PGPUInt32  nNumKeys = 0;
	HWND  hwndWorking = NULL;
	char  szWorkingTitle[256];
	MyDecodeEventData  t_myDecdData;
	DecodeEventData *const  pt_nrmlData = &t_myDecdData.t_normalData;
	BOOL  f_kysOnly = FALSE;
	PGPError  i_err, i_error;

	PGPValidateParam( pt_CTX && pt_BASICS->h_Instance && 
										pt_BASICS->pgpTlsContext && 
										t_TSK.i_Task != mi_NO_TASK ? (int) 
										t_TSK.pf_Successful : TRUE && options);

	if (pf_kysOnly)
		*pf_kysOnly = FALSE;

	UpdateWindow( pt_BASICS->h_wndMain);

	//Check license number and expiration with clientlib functions. This is to 
	//	avoid having to compile all the PGPlnLib code into the Notes plugin.
	if (!PGPclPGPpluginsEnabled())	{
		PGPclNoLicense( pt_BASICS->h_wndMain, kPGPclLicensePlugins);
		return kPGPError_UserAbort;
	}

	// Check for demo expiration
	if (PGPclIsExpired( pt_BASICS->h_wndMain, (PGPclExpType) 
						kPGPlnOperationGraceExpiration) != kPGPError_NoErr)	{
		PGPclErrorBox( pt_BASICS->h_wndMain, kPGPError_Win32_Expired);
		return kPGPError_UserAbort;
	}

	pt_nrmlData->h_Instance = pt_BASICS->h_Instance;
	pt_nrmlData->h_wndMain = pt_BASICS->h_wndMain;
	pt_nrmlData->pgpTlsContext = pt_BASICS->pgpTlsContext;
	pt_nrmlData->pc_AppNm = pt_BASICS->pc_AppNm;
	pt_nrmlData->recipients = kInvalidPGPKeySetRef;
	pt_nrmlData->ul_keyCount = 0;
	pt_nrmlData->pt_keyIds = NULL;
	pt_nrmlData->pt_block = pt_blk;
	pt_nrmlData->contents = 0;
	pt_nrmlData->fsrFileRef = NULL;

	//store a copy of the structure containing information about the special 
	//	plug-in task we've got in mind so that our decode callback procedure 
	//	can carry out the task
	t_myDecdData.t_task = t_TSK;

	if (IsPGPError( i_err = PGPclOpenDefaultKeyrings( pt_CTX, 
								kPGPOpenKeyDBFileOptions_Mutable, &pt_kyDb)) && 
								IsPGPError( i_err = PGPclOpenDefaultKeyrings( 
								pt_CTX, kPGPOpenKeyDBFileOptions_None, 
								&pt_kyDb)))	{
		DisplayPgpError( __FILE__, __LINE__, pt_BASICS->pc_ModuleNm, i_err);
		return i_err;
	}

	pt_nrmlData->keyDB = pt_kyDb;
	pt_nrmlData->keydbNew = kInvalidPGPKeyDBRef;
	if (IsPGPError( i_err = PGPNewKeyDB( pt_CTX, &pt_kyDbNew)))
		goto cleanUp;

	//set up the working (progress-bar) dialog only if no special plug-in task 
	//	is involved that should be transparent to the user
	if (t_TSK.i_Task == mi_NO_TASK)	{
		LoadString( pt_BASICS->h_Instance, IDS_WORKINGDECRYPT, 
									szWorkingTitle, sizeof( szWorkingTitle));

		hwndWorking = WorkingDlgProcThread( GetModuleHandle( 
													pt_BASICS->pc_ModuleNm), 
													pt_BASICS->h_Instance, 
													NULL, szWorkingTitle, "");
	}
	pt_nrmlData->h_wndWorking = hwndWorking;

	//decode the message using a callback procedure that we control to manage 
	//	our special PGP tasks
	i_err = PGPDecode( pt_CTX, options, PGPOPassThroughIfUnrecognized( pt_CTX, 
								(PGPBoolean) !f_MIME), PGPOPassThroughKeys( 
								pt_CTX, TRUE), PGPOEventHandler( pt_CTX, 
								i_DecodeEventHandlerOverride, &t_myDecdData), 
								PGPOSendNullEvents( pt_CTX, 100), 
								PGPOImportKeysTo( pt_CTX, pt_kyDbNew), 
								PGPOKeyDBRef( pt_CTX, pt_kyDb), 
								PGPOLastOption( pt_CTX));

	if (hwndWorking)
		DestroyWindow( hwndWorking);

	//if the user or one of the plug-in's special tasks aborted the decoding 
	//	process, skip past the rest of the procedure's processing
	if (kPGPError_UserAbort == i_err || kPGPError_BadPacket == i_err && 
								(PGPVERSIONMAJOR < 7 || 
								PGPVERSIONMAJOR == 7 && PGPVERSIONMINOR < 1))
		goto cleanUp;

	if (IsPGPError( i_err))	{
		DisplayPgpError( __FILE__, __LINE__, pt_BASICS->pc_ModuleNm, i_err);
		goto cleanUp;
	}

	//if the decoded content contains PGP public keys to import...
	if (PGPKeyDBRefIsValid( pt_kyDbNew))	{
		PGPKeySetRef pt_kySetNew;

		if (IsPGPError( i_err = PGPNewKeySet( pt_kyDbNew, &pt_kySetNew)))
			goto cleanUp;
		if (IsPGPError( i_err = PGPCountKeys( pt_kySetNew, &nNumKeys)))
			goto cleanUp;
		if (nNumKeys)	{
			//determine whether the decoded content contained only keys and 
			//	nothing else
			if (!( pt_nrmlData->contents & CONTENTS_ENCRYPTED || 
									pt_nrmlData->contents & CONTENTS_SIGNED))
				f_kysOnly = TRUE;

			//if caller wishes it in all cases or if more was decoded than just 
			//	keys..
			if (f_FORCE_IMPRT_FND_KYS || !f_kysOnly)	{
				//prompt the user to add the keys to her keyring (though it 
				//	seems by the "singletons" flag that the dialog won't be 
				//	shown in certain cases; I don't know this behavior, but all 
				//	the other plug-in's use it, so I will too)
				i_err = PGPclImportKeys( pt_CTX, pt_BASICS->pgpTlsContext, 
											pt_BASICS->h_wndMain, pt_kySetNew, 
											pt_kyDb, 
											kPGPclNoDialogForValidSingletons);

				//If the user didn't cancel the import process and no error 
				//	occurred, set the output flag that this was a key-import 
				//	decoding (if appropriate). If an error did occur, inform 
				//	the user.
				if (IsPGPError( i_err))
					if (i_err != kPGPError_UserAbort)
						DisplayPgpError( __FILE__, __LINE__, 
												pt_BASICS->pc_ModuleNm, i_err);
					else
						i_err = kPGPError_NoErr;
				else if (pf_kysOnly)
					*pf_kysOnly = f_kysOnly;
			} //if (f_FORCE_IMPRT_FND_KYS || !f_kysOnly)
		} //if (nNumKeys)

		if (IsPGPError( i_error = PGPFreeKeySet(pt_kySetNew)) && 
															!IsPGPError( i_err))
			i_err = i_error;
	} //if (pt_kyDbNew)

cleanUp:
	if (PGPKeySetRefIsValid( pt_nrmlData->recipients))
		if (IsPGPError( i_error = PGPFreeKeySet( pt_nrmlData->recipients)) && 
												!IsPGPError( i_err))
			i_err = i_error;
	if (pt_nrmlData->pt_keyIds)
		free( pt_nrmlData->pt_keyIds);
	if (PGPKeyDBRefIsValid( pt_kyDbNew))
		if (IsPGPError( i_error = PGPFreeKeyDB( pt_kyDbNew)) && 
															!IsPGPError( i_err))
			i_err = i_error;
	if (PGPKeyDBRefIsValid( pt_kyDb))
		if (IsPGPError( i_error = PGPFreeKeyDB( pt_kyDb)) && 
															!IsPGPError( i_err))
			i_err = i_error;

	return i_err;
} //i_DecryptVerify(


/** i_DecodeEventHandlerOverride( ***
Purpose is to override (subclass) the DecodeEventHandler() function in the 
shared-code PGP module DecryptVerify.c. The override allows us to accommodate 
special tasks associated with PGP decoding ASCII content required by this 
plug-in.

--- parameters & return ----
pgpContext: opaque PGP contextual information
pt_event: pointer to PGP event information and surrounding PGP event data
pv_userValue: Input & Output. Pointer to caller-supplied information to be 
	passed into the PGPDecode() call. Output for the purposes of this plug-in.
RETURN: kPGPError_NoErr if no error occurred; a PGP error code otherwise.

--- revision history -------
10/13/03 PR: listing format adjustment
10/30/02 PR: listing format adjustment

9/12/99 PR
+ documentation re our treatment of the binary-decoding process, other 
  adjustments
+ got rid of workaround to PGP bug, now fixed, where crash would occur when 
  decoding an empty, clear-signed PGP block

12/6/98 PR: created			*/
static PGPError i_DecodeEventHandlerOverride( PGPContextRef  pgpContext, 
												PGPEvent * pt_event, 
												PGPUserValue  pv_userValue)	{
	PGPValidateParam( PGPContextRefIsValid( pgpContext) && pt_event);

	//if applicable, check whether this PGP decoding event is one that needs 
	//	special handling
	if (pv_userValue)	{
		SpecialPlugInTaskInfo  t_task = ((MyDecodeEventData *) 
														pv_userValue)->t_task;

		switch (pt_event->type)	{
			//if this is the start of the PGP decoding event, initialize any 
			//	special output data to default values (non-success)
			case kPGPEvent_InitialEvent:
				switch (t_task.i_Task)	{
					case mi_TEST_FOR_PGP_ASCII:
						*(BOOL *) t_task.pv_ctx = FALSE;
						break;

					case mi_GET_PGP_BLOCK_COORDS:	{
						PgpBlockCoordInfo *const pt_coordInfo = 
											(PgpBlockCoordInfo *) t_task.pv_ctx;
						pt_coordInfo->ul_offsetBegin = 
												pt_coordInfo->ul_offsetEnd = 
												ei_NOT_FOUND;
						pt_coordInfo->f_tmpst = FALSE;
					}
				} //switch (t_task.i_Task)
				if (t_task.i_Task != mi_NO_TASK)
					*t_task.pf_Successful = FALSE;

				break;

			//If the PGP decoding has just found a new block of content and our 
			//	uncompleted task is to get the coordinates of the content's PGP 
			//	block and an ending offset has not yet been stored...
			case kPGPEvent_BeginLexEvent:	{
				PgpBlockCoordInfo * pt_coordInfo;

				if (mi_GET_PGP_BLOCK_COORDS == t_task.i_Task && 
										!*t_task.pf_Successful && 
										(pt_coordInfo = (PgpBlockCoordInfo *) 
										t_task.pv_ctx)->ul_offsetEnd == 
										(DWORD) ei_NOT_FOUND)
					//If a beginning offset has already been stored...
					if (pt_coordInfo->ul_offsetBegin != (DWORD) 
															ei_NOT_FOUND)	{
						//The current block, because it is the following block, 
						//	ipso facto points to the end of the PGP-encoded 
						//	block in the content, so set the ending offset as 
						//	this block's beginning offset. We don't subtract 
						//	one because PGP uses a zero-based offset, while we 
						//	are using a one-based offset.
						pt_coordInfo->ul_offsetEnd = 
									pt_event->data.beginLexData.sectionOffset;

						//note that our task is complete
						*t_task.pf_Successful = TRUE;

						//if the content block wasn't marked that it should be 
						//	viewed via the Tempest viewer, we can now abort the 
						//	decoding process since we've located the first 
						//	PGP block (if we're to use Tempest, we want all the 
						//	content, not just the first PGP block)
						if (!pt_coordInfo->f_tmpst)
							return kPGPError_UserAbort;
					//Else store the one-based beginning offset to the location 
					//	after which the block starts, even though we don't know 
					//	yet whether this block is PGP-encoded or not. The 
					//	ensuing Analyze event will tell us that.
					}else
						pt_coordInfo->ul_offsetBegin = 
									pt_event->data.beginLexData.sectionOffset;

				break;
			} //case kPGPEvent_BeginLexEvent

			//The Analyze event tells us what type of content block we've got, 
			//	before the PGP SDK actually decodes it. We use this event to 
			//	help execute any special task we need to carry out in 
			//	conjunction with this decoding.
			case kPGPEvent_AnalyzeEvent:
				switch (t_task.i_Task)	{
					//if we just need to know whether the content being PGP 
					//	decoded contains any PGP-encoded blocks and the current 
					//	block is PGP-encoded, indicate that the content does 
					//	contain PGP-encoded content
					case mi_TEST_FOR_PGP_ASCII:
						if (pt_event->data.analyzeData.sectionType < 
													kPGPAnalyze_Unknown)	{
							*(BOOL *) t_task.pv_ctx = TRUE;

							//we can abort the decoding process since we've 
							//	accomplished our task
							*t_task.pf_Successful = TRUE;
							return kPGPError_UserAbort;
						}

						break;

					//If we still need to get the coordinates of the 
					//	PGP-encoded block in the content and if this is not a 
					//	PGP-encoded block, reinitialize the beginning-offset 
					//	variable in order that the next BeginLex event will 
					//	know that it doesn't need to store a corresponding 
					//	ending offset.
					case mi_GET_PGP_BLOCK_COORDS:
						if (!*t_task.pf_Successful && 
									pt_event->data.analyzeData.sectionType == 
									kPGPAnalyze_Unknown)
							((PgpBlockCoordInfo *) 
											t_task.pv_ctx)->ul_offsetBegin = 
											(DWORD) ei_NOT_FOUND;
						break;
				} //switch (t_task.i_Task)
				break;

			case kPGPEvent_OutputEvent:
				if (mi_GET_PGP_BLOCK_COORDS == t_task.i_Task && 
									!*t_task.pf_Successful && 
									pt_event->data.outputData.forYourEyesOnly)
					((PgpBlockCoordInfo *) t_task.pv_ctx)->f_tmpst = TRUE;
		} //switch (pt_event->type)
	} //if (pv_userValue)

	//our special pre-processing done, allow the regular handler to do its work
	return DecodeEventHandler( pgpContext, pt_event, pv_userValue);
} //i_DecodeEventHandlerOverride(


/** ei_PgpLookupEmailLocal( ***
Look up a series of e-mail addresses in the user's default keyrings and output 
the success of each look up. The given addresses are assumed to be the core 
addresses undecorated by, for instance, SMTP phrases (a.k.a. display names).

--- parameters & return ----
pt_ctx: Input & Output. Address of the information structure used by the Notes 
	Plug-In to hold a common set of PGP contextual information through the 
	encoding process. The "missing ADKs" member of the structure may be updated 
	by this procedure, carrying an aggregate list of such ADKs among calls to 
	this and possibly other procedures.
pt_nd: Input & Output. Address of the head node of the list of e-mail address 
	information structures, each of whose e-mail addresses is to be looked up 
	for a key match. The f_found member flag of the information structure will 
	be set according to whether a match was found or not.
RETURN: kPGPError_NoErr if successful; a PGP error code otherwise

--- revision history -------
8/21/03 PR
+ relaxed lookup criterion so at least case-insensitive matching will occur
+ documentation adjustment for clarity

6/26/03 PR
+ logic extension to allow noting of missing associated ADKs, attendant 
  signature change
+ token renaming

3/20/00 PR: adjustment to support PGP 7.0
9/12/99 PR: created			*/
PGPError ei_PgpLookupEmailLocal( const BOOL  f_NOTE_MISSNG_ADKS, 
									PgpEncodeContext *const  pt_ctx, 
									NameFoundNode * pt_nd)	{
	const PGPContextRef  pt_CTX = pt_ctx ? 
											pt_ctx->t_recipients.context : 
											kInvalidPGPContextRef;
	const PGPKeyDBRef  pt_KYDB = pt_ctx ? 
									pt_ctx->t_recipients.keydbOriginal : 
									kInvalidPGPKeyDBRef;

	char * pc;
	PGPKeySetRef  pt_rsltSet;
	PGPFilterRef  pt_fltr;
	PGPUInt32  ul;
	PGPError  i_err = kPGPError_NoErr, 
				i_error = kPGPError_NoErr;

	PGPValidateParam( pt_nd && PGPContextRefIsValid( pt_CTX) && 
												PGPKeyDBRefIsValid( pt_KYDB));

	//for each e-mail address node in the list...
	do	{
		//if the address is already found or null, iterate to the next node in 
		//	the list
		if (pt_nd->pt_nm->f_found || !*(pc = pt_nd->pt_nm->pc_nm))
			continue;

		//Look up the address for a matching key. We use "substring" instead of 
		//	"equal" as the "criterion" because "equal" is for some 
		//	(unappreciated) reason case sensitive.
		if (IsPGPError( i_err = PGPNewKeyDBObjDataFilter( pt_CTX, 
									kPGPUserIDProperty_EmailAddress, 
									pc, strlen( pc), 
									kPGPMatchCriterion_SubString, &pt_fltr)))
			break;
		i_err = PGPFilterKeyDB( pt_KYDB, pt_fltr, &pt_rsltSet);
		if (IsPGPError( i_error = PGPFreeFilter( pt_fltr)))	{
			PGPFreeKeySet( pt_rsltSet);
			break;
		}
		if (IsPGPError( i_err))
			break;

		//if a key match was found for the address...
		if (IsntPGPError( i_err = PGPCountKeys( pt_rsltSet, &ul)) && ul)	{
			//make a note of it
			pt_nd->pt_nm->f_found = TRUE;

			//if caller wants us to, take note of any ADKs missing from the 
			//	local keyring
			if (f_NOTE_MISSNG_ADKS)
				i_err = i_NoteMissingAdks( pt_rsltSet, &pt_ctx->pt_missngAdks);
		} //if (IsntPGPError( i_err = PGPCountKeys(
		if (IsPGPError( i_error = PGPFreeKeySet( pt_rsltSet)) || 
															IsPGPError( i_err))
			break;
	} while (pt_nd = pt_nd->pt_nxt);

	return IsPGPError( i_err) ? i_err : i_error;
} //ei_PgpLookupEmailLocal(


/** i_NoteMissingAdks( ***
Queries each of the given set of PGP keys for any associated "requested" 
additional decryption keys (ADKs) it might have. The local keyring is queried 
for the presence of each ADK request found. If the associated ADK is not found 
in the local keyring, its key ID is added to the output list, if needed.

--- parameters & returns ----
pt_KYST: address of information structure describing a set of PGP keys
ppt_nd: Input & Output. Pointer to address of head node of a linked list 
	telling the IDs of any known missing ADKs. If procedure discovers any other 
	missing ADKs, their IDs are added to the list. If address is null and a 
	missing ADK is identified, procedure will start the list and reset the 
	address to the head node.
RETURN: kPGPError_NoErr if successful; a PGP error code otherwise

--- revision history --------
6/26/03 PR: created			*/
static PGPError i_NoteMissingAdks( const PGPKeySetRef  pt_KYST, 
									KeyIdNode * *const  ppt_nd)	{
	PGPKeyIterRef  pt_itr;
	PGPKeyDBObjRef  pt_ky;
	KeyIdNode * pt_nd;
	PGPError  i_err;

	_ASSERTE( PGPKeySetRefIsValid( pt_KYST) && ppt_nd);

	//for each key found...
	if (IsPGPError( i_err = PGPNewKeyIterFromKeySet( pt_KYST, &pt_itr)))
		return i_err;
	if (IsPGPError( i_err = PGPKeyIterNextKeyDBObj( pt_itr, 
												kPGPKeyDBObjType_Key, &pt_ky)))
		goto errJmp;
	pt_nd = *ppt_nd;
	do	{
		//for each ADK requested by the key...
		ULONG  ul, ul_;
		if (IsPGPError( i_err = PGPCountAdditionalRecipientRequests( pt_ky, 
																		&ul)))
			goto errJmp;
		for (ul_ = 0; ul_ < ul; ul_++)	{
			//if the ADK is not present on the local keyring...
			PGPKeyID  t_id;
			PGPKeyDBObjRef  pt_adk;
			if (IsPGPError( i_err = PGPGetIndexedAdditionalRecipientRequestKey( 
										pt_ky, ul_, &pt_adk, &t_id, NULL)) && 
										!(i_err == kPGPError_ItemNotFound && 
										*(DWORD* ) &t_id))	{
				_ASSERT( TRUE);
				continue;
			}
			if (!PGPKeyDBObjRefIsValid( pt_adk))	{
				PGPKeyID * pt_kyId;
				BOOL  f;

				//add it to the list of missing ADKs (if not there already)
				if (!ef_AddListNodeUnique( &t_id, TRUE, sizeof( PGPKeyID), 
															&f, &pt_nd))	{
					i_err = kPGPError_OutOfMemory;
					goto errJmp;
				}
				if (!*ppt_nd)
					*ppt_nd = pt_nd;
				if (f)	{
					if (!( pt_kyId = malloc( sizeof( PGPKeyID))))	{
						(*ppt_nd)->pt_kyId = NULL;
						i_err = kPGPError_OutOfMemory;
						goto errJmp;
					}
					memcpy( pt_kyId, &t_id, sizeof( PGPKeyID));
					pt_nd->pt_kyId = pt_kyId;
				} //if (f)
			} //if (!PGPKeyDBObjRefIsValid( pt_adk))
		} //for (ULONG ul_ = 0; ul_ < ul
		if (IsPGPError( i_err = PGPKeyIterNextKeyDBObj( pt_itr, 
												kPGPKeyDBObjType_Key, &pt_ky)))
			if (kPGPError_EndOfIteration == i_err)
				i_err = kPGPError_NoErr;
			else
				goto errJmp;
	} while (pt_ky);

errJmp:
	PGPFreeKeyIter( pt_itr);

	return i_err;
} //i_NoteMissingAdks(


/** ei_PgpLookupEmailViaServers( ***
Look up a series of e-mail addresses against the certificate servers specified 
in the user's PGP preferences, and to output the success of each look up.

--- parameters & return ----
pt_ctx: Input & Output. Address of the information structure used by the Notes 
	Plug-In to hold a common set of PGP contextual information through the 
	encoding process. The "missing ADKs" member of the structure and the 
	"added keys" member of the t_recipients member of the structure may be 
	updated by this procedure, carrying an aggregate list of such ADKs or 
	added keys among calls to this and possibly other procedures.
pt_nd: Input & Output. Pointer to the head node of the list of e-mail address 
	information structures, each of whose e-mail addresses is to be looked up 
	for a key match. The f_found member of the information structure will be 
	set according to wherever a match was found.
RETURN: kPGPError_NoErr if successful; a PGP error code otherwise

--- revision history -------
8/21/03 PR: relaxed lookup criterion so at least case-insensitive matching will 
	occur

6/26/03 PR
+ logic extension to allow for noting of missing associated ADKs
+ token renaming, documentation adjustment, exception logic improvement

10/30/02 PR
+ logic adjustments to support gracefully user-cancelation and key-server 
  results timeout
+ token renaming, listing format adjustment

3/20/00 PR: logic adjustment to support PGP 7.0
9/12/99 PR: created			*/
PGPError ei_PgpLookupEmailViaServers( PgpEncodeContext *const  pt_ctx, 
										NameFoundNode * pt_nd)	{
	const PGPContextRef  pt_CTX = pt_ctx ? pt_ctx->t_recipients.context : 
														kInvalidPGPContextRef;
	const PGPKeyDBRef  pt_KYDB = pt_ctx ? pt_ctx->t_recipients.keydbOriginal : 
															kInvalidPGPKeyDBRef;

	PGPtlsContextRef  pt_tlsCtx;
	PGPKeyDBRef * ppt_addedKyDb, pt_rsltDb = kInvalidPGPKeyDBRef;
	BOOL  f_strtdAddedKyDb = FALSE;
	HANDLE  h_wnd;
	char * pc_addr, pc[ 0x100];
	PGPFilterRef  pt_fltr;
	PGPKeySetRef  pt_rsltSet;
	PGPUInt32  ul;
	PGPError  i_err, i_error;

	PGPValidateParam( pt_nd && PGPContextRefIsValid( pt_CTX) && 
												PGPKeyDBRefIsValid( pt_KYDB));

	//if hasn't been started already, start an "added keys" key database
	if (!PGPKeyDBRefIsValid( *(ppt_addedKyDb = 
										&pt_ctx->t_recipients.keydbAdded)))	{
		if (IsPGPError( i_err = PGPNewKeyDB( pt_CTX, ppt_addedKyDb)))
			return i_err;
		f_strtdAddedKyDb = TRUE;
	}

	//for each e-mail address node in the list...
	pt_tlsCtx = pt_ctx->t_recipients.tlsContext;
	h_wnd = pt_ctx->t_recipients.hwndParent;
	do	{
		//if the address is already found or null, iterate to the next node in 
		//	the list
		if (pt_nd->pt_nm->f_found || !*(pc_addr = pt_nd->pt_nm->pc_nm))
			continue;

		//Look up the address for a matching key. We do not use the more 
		//	precise filter-search method because HTTP key servers do not 
		//	support it (LDAP does). Too bad, because it is HTTP key servers 
		//	also that treat a slash as an "or" operator in the string, meaning 
		//	that results can be returned that do not match, say, a hierarchical 
		//	Notes address. If PGP gives back an error...
		sprintf( pc, "%s%s%s", "<", pc_addr, ">");
		if (IsPGPError( i_err = PGPclSearchServerForUserID( pt_CTX, pt_tlsCtx, 
												h_wnd, pc, kPGPclDefaultServer, 
												pt_KYDB, &pt_rsltDb)))	{
			//Suppress any "partial search results" error--this condition 
			//	arises from either a timeout in getting results back from the 
			//	server or, presumably, from some condition at the server where 
			//	only partial results are returned; so far, I've seen only the 
			//	former condition [at the default Europe LDAP server] with 
			//	"partial results" meaning no results--or any "server not found" 
			//	error.
			if (i_err == kPGPError_ServerPartialSearchResults || i_err == 
											kPGPError_SocketsDomainServerError)
				i_err = kPGPError_NoErr;

			//if it looks like we got a no-results timeout or no-find 
			//	condition, or if a real error occurred, short-circuit
			if (i_err || !pt_rsltDb)
				goto errJmp;
		} //if (IsPGPError( i_err = PGPclSearchServerForUserID(

		//Ensure that we did find what we were looking for (compensate for the 
		//	slash problem with HTTP servers). We use "substring" instead of 
		//	"equal" as the "criterion" because "equal" is for some 
		//	(unappreciated) reason case sensitive.
		if (IsPGPError( i_err = PGPNewKeyDBObjDataFilter( pt_CTX, 
									kPGPUserIDProperty_EmailAddress, 
									pc_addr, strlen( pc_addr), 
									kPGPMatchCriterion_SubString, &pt_fltr)))
			goto errJmp;
		i_err = PGPFilterKeyDB( pt_rsltDb, pt_fltr, &pt_rsltSet);
		if (IsPGPError( i_error = PGPFreeFilter( pt_fltr)) && !IsPGPError( 
																		i_err))
			i_err = i_error;
		if (IsPGPError( i_err))
			goto errJmp;

		//if a key match was found for the address...
		if (IsPGPError( i_err = PGPCountKeys( pt_rsltSet, &ul)))
			goto errJmp;
		if (ul)	{
			//Indicate the match and add the discovered key(s) to the running 
			//	set of keys. PGPCopyKeys() doesn't seem to duplicate keys, so I 
			//	don't concern myself with that potential problem. (Also note 
			//	that freeing a key database automatically frees any dependent 
			//	key sets.)o
			pt_nd->pt_nm->f_found = TRUE;
			if (IsPGPError( i_err = PGPCopyKeys( pt_rsltSet, *ppt_addedKyDb, 
																		NULL)))
				goto errJmp;

			//take note of any ADKs missing from the local keyring
			if (IsPGPError( i_err = PGPCheckKeyRingSigs( pt_rsltSet, 
										kInvalidPGPKeyDBRef, FALSE, NULL, 0)))
				goto errJmp;
			if (IsPGPError( i_err = i_NoteMissingAdks( pt_rsltSet, 
													&pt_ctx->pt_missngAdks)))
				goto errJmp;
		} //if (ul)
		i_err = PGPFreeKeyDB( pt_rsltDb);
		pt_rsltSet = kInvalidPGPKeySetRef;
		pt_rsltDb = kInvalidPGPKeyDBRef;
		if (IsPGPError( i_err))
			goto errJmp;
	} while (pt_nd = pt_nd->pt_nxt);

	return kPGPError_NoErr;

errJmp:
	if (PGPKeyDBRefIsValid( pt_rsltDb))
		PGPFreeKeyDB( pt_rsltDb);
	if (i_err != kPGPError_UserAbort && f_strtdAddedKyDb)	{
		PGPFreeKeyDB( *ppt_addedKyDb);
		*ppt_addedKyDb = kInvalidPGPKeyDBRef;
	}

	return i_err;
} //ei_PgpLookupEmailViaServers(


/** ei_PgpLookupMissingAdks( ***
On the client's currently configured set of key servers, lookup any PGP 
additional decryption keys (ADKs) noted as missing.

--- parameter & return -------
pt_ctx: Input & Output. Address of the information structure used by the Notes 
	Plug-In to hold a common set of PGP contextual information through an 
	encoding process. The "missing ADKs" member of the structure and the 
	"added keys" member of the t_recipients member of the structure may be 
	updated by this procedure, carrying an aggregate list of such ADKs or 
	added keys among calls to this and possibly other procedures.
RETURN: kPGPError_NoErr if successful; a PGP error code otherwise

--- suggested enhancement ----
6/26/03 PR: remove from the caller's list of missing ADKs, the IDs associated 
	with any found ADKs, as PDL'd below

--- revision history ---------
6/26/03 PR: created			*/
PGPError ei_PgpLookupMissingAdks( PgpEncodeContext *const  pt_ctx)	{
	const PGPContextRef  pt_CTX = pt_ctx ? pt_ctx->t_recipients.context : 
														kInvalidPGPContextRef;
	const PGPKeyDBRef  pt_KYDB = pt_ctx ? pt_ctx->t_recipients.keydbOriginal : 
															kInvalidPGPKeyDBRef;

	PGPKeyID * pt_kyId;
	ULONG  ul;
	PGPKeyDBRef * ppt_addedKyDb, pt_rsltDb;
	BOOL  f_strtdAddedKyDb = FALSE;
	PGPKeySetRef  pt_rsltSet;
	PGPError  i_err;

	PGPValidateParam( pt_ctx && pt_ctx->pt_missngAdks && PGPContextRefIsValid( 
									pt_CTX) && PGPKeyDBRefIsValid( pt_KYDB));

	//if hasn't been started already, start an "added keys" key database
	if (!PGPKeyDBRefIsValid( *(ppt_addedKyDb = 
										&pt_ctx->t_recipients.keydbAdded)))	{
		if (IsPGPError( i_err = PGPNewKeyDB( pt_CTX, ppt_addedKyDb)))
			goto errJmp;
		f_strtdAddedKyDb = TRUE;
	}

	//make an array out of the list of key IDs
	if (!ef_listToArray( pt_ctx->pt_missngAdks, sizeof( PGPKeyID), &pt_kyId, 
																		&ul))
		return kPGPError_OutOfMemory;

	//Look up the key IDs for matching keys. If any problem occurs...
	if (IsPGPError( i_err = PGPclSearchServerForKeyIDs( pt_CTX, 
											pt_ctx->t_recipients.tlsContext, 
											pt_ctx->t_recipients.hwndParent, 
											pt_kyId, ul, kPGPclDefaultServer, 
											pt_KYDB, &pt_rsltDb)))	{
		//Suppress any "partial search results" error--this condition arises 
		//	from either a timeout in getting results back from the server or, 
		//	presumably, from some condition at the server where only partial 
		//	results are returned; so far, I've seen only the former condition 
		//	[at the default Europe LDAP server] with "partial results" meaning 
		//	no results--or any "server not found" error.
		if (i_err == kPGPError_ServerPartialSearchResults || i_err == 
											kPGPError_SocketsDomainServerError)
			i_err = kPGPError_NoErr;

		//if it looks like we got a no-results timeout or no-find 
		//	condition, or if a real error occurred, short-circuit
		if (i_err || !pt_rsltDb)
			goto errJmp;
	} //if (IsPGPError( i_err = PGPClSearchServerForKeyIDs(

	//if any key match was found for the address, add the discovered key(s) to 
	//	the running set of keys. PGPCopyKeys() doesn't seem to duplicate keys, 
	//	so I don't concern myself with that potential problem. (Also note that 
	//	freeing a key database automatically frees any dependent key sets.)
	if (IsPGPError( i_err = PGPCountKeysInKeyDB( pt_rsltDb, &ul)) || !ul)
		goto errJmp;
	if (IsPGPError( i_err = PGPNewKeySet( pt_rsltDb, &pt_rsltSet)))
		goto errJmp;
	if (IsPGPError( i_err = PGPCopyKeys( pt_rsltSet, *ppt_addedKyDb, NULL)))
		goto errJmp;

	//remove the IDs of the found ADKs from the caller's list (NOT YET DONE)

errJmp:
	if (PGPKeyDBRefIsValid( pt_rsltDb))	{
		PGPError  i_error;
		if (IsPGPError( i_error = PGPFreeKeyDB( pt_rsltDb)) && IsntPGPError( 
																		i_err))
			i_err = i_error;
	}
	if ((!ul || IsPGPError( i_err)) && f_strtdAddedKyDb && i_err != 
													kPGPError_UserAbort)	{
		PGPFreeKeyDB( *ppt_addedKyDb);
		*ppt_addedKyDb = kInvalidPGPKeyDBRef;
	}
	free( pt_kyId);

	return i_err;
} //ei_PgpLookupMissingAdks(


