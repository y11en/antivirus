/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: PGPPlug.c 10788 2002-11-12 18:59:33Z pbj $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

// Shared Headers
#include "pgpClientLib.h"
#include "pgpLnLib.h"
#include "pgpVersionHeader.h"

// Project Headers
#include "PGPPlug.h"
#include "Translators.h"
#include "CBTProc.h"

// Global Variables
HINSTANCE			g_hinst				= NULL;
PGPContextRef		g_pgpContext		= NULL;
PGPtlsContextRef	g_tlsContext		= NULL;
HHOOK				g_hCBTHook			= NULL;


BOOL 
WINAPI 
DllMain (	HINSTANCE hinstDll, 
			DWORD fdwReason,
			LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			g_hinst = hinstDll;
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
   }
   return(TRUE);
}


/*****\
*
* ems_plugin_version: Get the version of the API used for this plugin
*
\*****/

long WINAPI ems_plugin_version(
    short FAR* api_version      /* Place to return api version */
)
{
	*api_version = EMS_VERSION;
	return (EMSR_OK);
}


/*****\
*
* ems_plugin_finish: End use of a plugin and clean up
*
\*****/

long WINAPI ems_plugin_finish(
    void FAR* globals        /* Pointer to translator instance structure */
)
{ 
	/*PGPclCloseLibrary();

	if( PGPRefIsValid(g_pgpContext) )
	{
		PGPFreeContext(g_pgpContext);
	}*/

	return (EMSR_OK); 
}
 
 
/*****\
*
* ems_plugin_init: Initialize plugin and get its basic info
*
\*****/

long WINAPI ems_plugin_init(
    void FAR*FAR* globals,      /* Return for allocated instance structure */
    char FAR* config_dir,       /* Optional directory for config file */
    char FAR* user_name,        /* Users full name from Eudora config */
    char FAR* user_addr,        /* Users address (RFC-822 addr spec) */
    short FAR* num_trans,       /* Place to return num of translators */
    char FAR*FAR* plugin_desc,  /* Return for string description of plugin */
    short FAR* plugin_id,       /* Place to return unique plugin id */
    HICON FAR*FAR* plugin_icon	/* Return for plugin icon data */
)
{
	long ReturnValue = EMSR_OK;
	PGPError err;

	// Initialize the common library
	err = PGPclInitLibrary(&g_pgpContext, kPGPclEnableNewUserWizard);

	if( IsPGPError(err) )
	{
		if(err == kPGPError_FeatureNotAvailable)
		{
			char szMessage[256];
			char szTitle[256];

			LoadString(g_hinst, IDS_E_EXPIRED, szMessage, sizeof(szMessage));
			LoadString(g_hinst, IDS_E_EXPIREDTITLE, szTitle, sizeof(szTitle));

			MessageBox(NULL, szMessage, szTitle, MB_OK);
		}
		else
		{
			PGPclErrorBox (NULL, err);
		}

		PGPclCloseLibrary();
		return EMSR_UNKNOWN_FAIL;
	}

	// has this beta version expired?
	PGPlnLicenseCheck (g_pgpContext);
	if(PGPlnIsExpired(NULL, kPGPlnModuleGraceExpiration))
	{
		PGPclCloseLibrary();
		return EMSR_UNKNOWN_FAIL;
	}

	PGPNewTLSContext(g_pgpContext, &g_tlsContext);

	// Set the number of translators in this module
	if (num_trans)
	{
		*num_trans = kPGPNumTranslators;
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	// Set the module's description
	if (plugin_desc)
	{
		*plugin_desc = strdup(pgpVersionHeaderString());
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}
	
	// set the plugin id
	if (plugin_id)
	{
		*plugin_id = kPGPPluginID;
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	// Set the plugin's icon
	if (plugin_icon)
	{
		*plugin_icon = (HICON *)malloc(sizeof(HICON));

		if(*plugin_icon)
		{
			**plugin_icon = LoadIcon(	g_hinst, 
										MAKEINTRESOURCE( IDI_PGP32 ) );
		}
		else
		{
			ReturnValue = EMSR_UNKNOWN_FAIL;
		}
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	g_hCBTHook = SetWindowsHookEx(	WH_CBT, 
									(HOOKPROC)CBTProc, 
									NULL, 
									GetCurrentThreadId());

	return (ReturnValue);
}


/*****\
*
* ems_translator_info: Get details about a translator in a plugin
*
\*****/

long WINAPI ems_translator_info(
    void FAR* globals,			/* Pointer to translator instance structure */
    short trans_id,				/* ID of tRanslator to get info for */
    long FAR* trans_type,		/* Return for translator type*/
    long FAR* trans_sub_type,	/* Return for translator subtype */
    unsigned long FAR* trans_flags, /* Return for translator flags */
    char FAR*FAR* trans_desc,	/* Return for translator string description */
    HICON FAR*FAR* trans_icon	/* Return for translator icon data */
)
{
	long ReturnValue = EMSR_OK;
	TransInfoStruct *InfoPtr = NULL;

	// make sure we have this many plug-ins
	if ( (trans_id <= 0) || (trans_id > kNumTrans) )
	{
		return (EMSR_INVALID_TRANS);
	}

	// index the plug-in
	InfoPtr = gTransInfo + (trans_id - 1);

	// fill out the various members from our structure
	if (trans_type)
	{
		*trans_type = InfoPtr->type;
	}

	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	if (trans_sub_type)
	{
		*trans_sub_type = InfoPtr->subtype;
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	if (trans_flags)
	{
		*trans_flags = InfoPtr->flags;
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	if (trans_desc)
	{
		*trans_desc = strdup(InfoPtr->description);
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	// give it the right icons
	if (trans_icon)
	{
		*trans_icon = (HICON *)malloc(sizeof(HICON));

		if(*trans_icon)
		{
			**trans_icon = LoadIcon(g_hinst, 
							MAKEINTRESOURCE(gTransInfo[trans_id-1].nIconID));
		}
		else
		{
			ReturnValue = EMSR_UNKNOWN_FAIL;
		}
	}
	else
	{
		ReturnValue = EMSR_UNKNOWN_FAIL;
	}

	return (ReturnValue);
}

/*****\
*
* ems_can_translate_file:
*     Check and see if a translation can be performed (file version)
*
\*****/

long WINAPI ems_can_translate(
    void FAR* globals,          /* Pointer to translator instance structure */
    long context,               /* Context for check; e.g. EMSF_ON_xxx */
    short trans_id,             /* ID of translator to call */
    emsMIMEtypeP in_mime,       /* MIME type of data to check */
    char FAR*FAR* addresses,    /* List of addresses (sender and recipients) */
    char FAR* properties,       /* Properties for queued translations */
    char FAR*FAR* out_error,    /* Place to return string with error message */
    long FAR* out_code          /* Return for translator-specific result code */
)
{
	PluginError	err	= EMSR_CANT_TRANS;

	InitPtrToNull( out_error );

	// see if this is one of our id's
	if ((trans_id > 0) && (trans_id < kLastTranslatorIDPlusOne))
	{
		err = CanPerformTranslation(trans_id, in_mime, context);
	}
	else
	{
		err = EMSR_INVALID_TRANS;
	}
	
	return err;
}

/*****\
*
* ems_translate_file: Actually perform a translation on a file
*
\*****/

long WINAPI ems_translate_file(
    void FAR* globals,          /* Pointer to translator instance structure */
    long context,               /* Context for translation; e.g. EMSF_ON_xxx */
    short trans_id,             /* ID of translator to call */
    emsMIMEtypeP in_mime,       /* MIME type of input data */
    char FAR* in_file,          /* The input file name */
    char FAR*FAR* addresses,    /* List of addresses (sender and recipients) */
    char FAR* properties,       /* Properties for queued translations */
    emsProgress progress,       /* Func to report progress / check for abort */
    emsMIMEtypeP FAR* out_mime, /* Place to return MIME type of result */
    char FAR* out_file,       /* The output file (specified by Eudora) */
    HICON FAR*FAR* out_icon,  /* Place to return icon representing result */
    char FAR*FAR* out_desc,   /* Returned string for display with the result */
    char FAR*FAR* out_error,  /* Place to return string with error message */
    long FAR* out_code        /* Return for translator-specific result code */
)
{
	PluginError	err	= EMSR_CANT_TRANS;

	InitPtrToNull( out_icon );
	InitPtrToNull( out_desc );
	InitPtrToNull( out_error );

	// see if this is one of our id's
	if ((trans_id > 0) && (trans_id < kLastTranslatorIDPlusOne))
	{
		err = PerformTranslation(	trans_id, 
									in_file, 
									out_file, 
									addresses, 
									in_mime,
									out_mime);
	}
	else
	{
		err = EMSR_INVALID_TRANS;
	}

	return err;
}


/*****\
*
* ems_plugin_config: Call the plug-ins configuration Interface 
*
\*****/

long WINAPI ems_plugin_config(
    void FAR* globals,          /* Pointer to translator instance structure */
    char FAR* config_dir,       /* Optional directory for config file */
    char FAR* user_name,        /* Users full name from Eudora config */
    char FAR* user_addr         /* Users address (RFC-822 addr spec) */
)
{
	PGPclPreferences (g_pgpContext, NULL, kPGPclEmailPrefs, NULL);

	return EMSR_OK;
}

/*****\
*
* ems_free: Free memory allocated by EMS plug-in
*
\*****/

long WINAPI ems_free(
    void FAR* mem               /* Memory to free */
)
{
	if (mem)
	{
		free(mem);
	}

	return (EMSR_OK);
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
