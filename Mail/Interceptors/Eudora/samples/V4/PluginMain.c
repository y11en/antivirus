/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: PluginMain.c 28687 2004-10-18 13:54:27Z pbj $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

// Shared Headers
#include "pgpClientLib.h"
#include "pgpLnLib.h"
#include "pgpWin32IPC.h"
#include "PGPsc.h"
#include "pgpVersionHeader.h"
#include "Prefs.h"

// Project Headers
#include "PluginMain.h"
#include "Translators.h"
#include "pgpRc.h"

// Global Variables
HINSTANCE			g_hinst				= NULL;
PGPContextRef		g_pgpContext		= NULL;
PGPtlsContextRef	g_tlsContext		= NULL;
HWND				g_hwndHidden		= NULL;
PGPChar *				g_szWipeFile		= NULL;

static BOOL s_bEncryptByDefault = FALSE;
static BOOL s_bSignByDefault = FALSE;
static UINT s_nPrefsReloadMsg = 0;

static HWND CreateHiddenWindow(void);
static void sWipeFile(void);


LRESULT CALLBACK HiddenWindowProc(HWND hwnd, 
								  UINT msg, 
								  WPARAM wParam, 
								  LPARAM lParam);

BOOL 
WINAPI 
DllMain (	HINSTANCE hinstDll, 
			DWORD fdwReason,
			LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
//			BOOL bInstalled=FALSE;
//			HKEY hkey;

			g_hinst = hinstDll;
/*
			if (RegOpenKeyEx(HKEY_CURRENT_USER, 
				PGPTXT_MACHINE("Software\\PGP Corporation\\PGP\\InstalledComponents"),
				0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
			{
				DWORD dwValue;
				DWORD dwSize = sizeof(dwValue);
				DWORD dwType;

				if(RegQueryValueEx(hkey, 
					PGPTXT_MACHINE("EUDORA"), 
					NULL, &dwType,
					(LPBYTE)&dwValue, &dwSize)==ERROR_SUCCESS)
				{
					if(dwValue == 1)
						bInstalled=TRUE;
				}

				RegCloseKey(hkey);
			}

			if(!bInstalled)
				return FALSE;
*/
			break;
		}

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
   }
   return(TRUE);
}


/***************************************************************************/
/* TRANSLATER API FUNCTIONS */

/*****\
*
* ems_plugin_version: Get the version of the API used for this plugin
*
\*****/

long
WINAPI
ems_plugin_version(
    short FAR* api_version      /* Place to return api version */
)
{
	*api_version = EMS_VERSION;
	return (EMSR_OK);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_plugin_init: Initialize plugin and get its basic info
*
\*****/

long
WINAPI
ems_plugin_init(
    void FAR*FAR* globals,      /* Out: Return for instance structure */
    short eudAPIVersion,        /* In: The API version eudora is using */
    emsMailConfigP mailConfig,  /* In: Eudoras mail configuration */
    emsPluginInfoP pluginInfo   /* Out: Return Plugin Information */
)
{
	long returnValue = EMSR_OK;
	PGPError error;

	error = PGPclInitLibrary(&g_pgpContext, kPGPclEnableNewUserWizard);

	if( IsPGPError(error) )
	{
		if (error == kPGPError_FeatureNotAvailable)
		{
			PGPChar szMessage[256];
			PGPChar szTitle[256];

			PGPLoadString(g_hinst, IDS_E_EXPIRED, szMessage, sizeof(szMessage)/sizeof(PGPChar));
			PGPLoadString(g_hinst, IDS_E_EXPIREDTITLE, szTitle, sizeof(szTitle)/sizeof(PGPChar));

			MessageBox(NULL, szMessage, szTitle, MB_OK);
		}
		else
			PGPclErrorBox (	NULL, error);

		PGPclCloseLibrary();
		g_pgpContext = NULL;
		return EMSR_UNKNOWN_FAIL;
	}

	// has this beta version expired?
	PGPlnLicenseCheck (g_pgpContext);
	if (PGPlnIsExpired(NULL, kPGPlnModuleGraceExpiration))
	{
		PGPclCloseLibrary();
		g_pgpContext = NULL;
		return EMSR_UNKNOWN_FAIL;
	}

	PGPNewTLSContext( g_pgpContext, &g_tlsContext );

	// Initialize the Encrypt by Default and Sign by Default settings

	s_bEncryptByDefault = ByDefaultEncrypt(PGPPeekContextMemoryMgr(g_pgpContext));
	s_bSignByDefault = ByDefaultSign(PGPPeekContextMemoryMgr(g_pgpContext));

	// Create a hidden window to catch messages

	g_hwndHidden = CreateHiddenWindow();

	if (pluginInfo &&
		returnValue == EMSR_OK)
	{
		pluginInfo->numTrans = kNumTrans;
		pluginInfo->numAttachers = 0;
		pluginInfo->numSpecials = kNumSpecial;
		
		pluginInfo->desc = strdup(pgpVersionHeaderString());
		
		pluginInfo->icon = (HICON*)malloc(sizeof(HICON));

		if(pluginInfo->icon)
		{
			*pluginInfo->icon = LoadIcon(	g_hinst, 
											MAKEINTRESOURCE( IDI_PGP32 ) );
		}
		else
		{
			returnValue = EMSR_UNKNOWN_FAIL;
		}
		
		pluginInfo->id = kPluginID;
	}
	
	return (returnValue);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_translator_info: Get details about a translator in a plugin
*
\*****/

long
WINAPI
ems_translator_info(
    void FAR* globals,          /* Out: Return for instance structure */
    emsTranslatorP transInfo    /* In/Out: Return Translator Information */
)
{
	TransInfoStruct *InfoPtr = NULL;
	long returnValue = EMSR_OK;

	if ((transInfo) && (gTransInfo))
	{
		const long id = transInfo->id;

		if ((id > 0) && 
			(id <= kNumTrans))
		{
			InfoPtr = gTransInfo + (id - 1);

			transInfo->type = InfoPtr->type;
			transInfo->flags = InfoPtr->flags;
#if SENDOK
			if(	kEncryptTranslatorID == id && s_bEncryptByDefault )
			{
				transInfo->flags |= EMSF_DEFAULT_Q_ON;
			}
			else if( kSignTranslatorID == id && s_bSignByDefault )
			{
				transInfo->flags |= EMSF_DEFAULT_Q_ON;
			}
#endif // SENDOK
			transInfo->desc = strdup(InfoPtr->description);
			transInfo->icon = (HICON *)malloc(sizeof(HICON));

			if(transInfo->icon)
			{
				*transInfo->icon = LoadIcon(g_hinst, 
					MAKEINTRESOURCE(gTransInfo[id - 1].nIconID));
			}
			else
			{
				returnValue = EMSR_UNKNOWN_FAIL;
			}
		}
		else
		{
			returnValue = EMSR_INVALID_TRANS;
		}
	}
	else
	{
		returnValue = EMSR_UNKNOWN_FAIL;
	}

	return (returnValue);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_can_translate_file:
*     Check and see if a translation can be performed (file version)
*
\*****/

long
WINAPI
ems_can_translate(
    void FAR* globals,            /* Out: Return for instance structure */
    emsTranslatorP transInfo,     /* In: Translator Info */
    emsDataFileP inTransData,     /* In: What to translate */
    emsResultStatusP transStatus  /* Out: Translations Status information */
)
{
	long returnValue = EMSR_OK;

	if ((transInfo) && (inTransData))
	{
		const long id = transInfo->id;
		const long context = inTransData->context;
		const emsMIMEtypeP in_mime = inTransData->info;

		if ((id > 0) && 
			(id <= kNumTrans))
		{
			returnValue = CanPerformTranslation(	
										id, 
										in_mime, 
										context);
		}
		else
		{
			returnValue = EMSR_INVALID_TRANS;
		}
	}
	else
	{
		returnValue = EMSR_UNKNOWN_FAIL;
	}

	return returnValue;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_translate_file: Actually perform a translation on a file
*
\*****/

long
WINAPI
ems_translate_file(
    void FAR* globals,           /* Out: Return for instance structure */
    emsTranslatorP transInfo,    /* In: Translator Info */
    emsDataFileP inFileData,     /* In: What to translate */
    emsProgress progress,        /* Func to report progress/check for abort */
    emsDataFileP outFileData,    /* Out: Result of the translation */
    emsResultStatusP transStatus /* Out: Translations Status information */
)
{
	long returnValue = EMSR_OK;
 
	if(	inFileData &&
		outFileData && 
		transInfo &&
		transStatus)
	{
		const PGPChar8 *in_file = inFileData->fileName;
		const PGPChar8 *out_file = outFileData->fileName;
		const long id = transInfo->id;
		emsMIMEtypeP in_mime = inFileData->info;
		emsMIMEtypeP FAR *out_mime = &(outFileData->info);
		PGPChar8 FAR* FAR*out_desc = &(transStatus->desc);

		// see if this is one of our id's
		if ((id > 0) && 
			(id <= kNumTrans))
		{
			HANDLE hOutput = NULL;

			returnValue = PerformTranslation(	
										id, 
										in_file, 
										out_file, 
										inFileData->header, 
										in_mime,
										out_mime);
		}
		else
		{
			returnValue = EMSR_INVALID_TRANS;
		}
	}

	return returnValue;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_plugin_config: Call the plug-ins configuration Interface
*
\*****/

long
WINAPI
ems_plugin_config(
    void FAR* globals,			/* Out: Return for instance structure */
    emsMailConfigP mailConfig	/* In: Eudora mail info */
)
{
	PGPclPreferences (g_pgpContext, NULL, kPGPclMessagingPrefs, NULL);

	return (EMSR_OK);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_special_info: Get details about a attacher in a plugin
*
\*****/

long WINAPI ems_special_info(
    void FAR* globals,          /* Out: Return for instance structure */
    emsMenuP specialMenu         /* Out: The menu */
)
{
	TransInfoStruct *InfoPtr = NULL;
	long returnValue = EMSR_OK;

	if ( specialMenu->id != kSpecialLaunchKeysID )
	{
		return (EMSR_INVALID_TRANS);
	}

	specialMenu->flags = gSpecialInfo->flags;
	specialMenu->desc = strdup(gSpecialInfo->description);

	specialMenu->icon = (HICON *)malloc(sizeof(HICON));

	if(specialMenu->icon)
	{
		*specialMenu->icon = LoadIcon(g_hinst, 
			MAKEINTRESOURCE(gSpecialInfo->nIconID));
	}
	else
	{
		returnValue = EMSR_UNKNOWN_FAIL;
	}

	return returnValue;
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_special_hook: Get details about a attacher in a plugin
*
\*****/

long WINAPI ems_special_hook(
    void FAR* globals,          /* Out: Return for instance structure */
    emsMenuP specialMenu        /* In: The menu */
)
{
	if (!specialMenu) 
	{
		return (EMSR_UNKNOWN_FAIL);
	}
	
	if (specialMenu->id == kSpecialLaunchKeysID)
	{
		PGPChar szPath[MAX_PATH];
		PGPError error = kPGPError_NoErr;

		error = PGPclGetPath (kPGPclPGPkeysExeFile,
			szPath, sizeof(szPath));

		if( IsntPGPError(error) )
		{
			PGPclExecute(szPath, SW_SHOW);
		}
		else
		{
			PGPChar szError[256];

			PGPLoadString(g_hinst, IDS_E_LAUNCHPGPKEYS, szError,
				sizeof(szError)/sizeof(PGPChar));

			MessageBox(NULL, 
				szError, 
				0, 
				MB_OK);
		}
	}

	return (EMSR_OK);
}



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_plugin_finish: End use of a plugin and clean up
*
\*****/

long
WINAPI
ems_plugin_finish(
    void FAR* globals    /* Pointer to translator instance structure */
)
{
	sWipeFile();

	if (g_hwndHidden != NULL)
		DestroyWindow(g_hwndHidden);

	if(g_tlsContext != NULL)
	{
		PGPFreeTLSContext(g_tlsContext);
		g_tlsContext = NULL;
	}

	if(g_pgpContext != NULL)
	{
		PGPclCloseLibrary();
		g_pgpContext = NULL;
	}
	
	return (EMSR_OK); 
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*****\
*
* ems_free: Free memory allocated by EMS plug-in
*
\*****/

long
WINAPI
ems_free(
    void FAR* mem               /* Memory to free */
)
{
	if (mem)
	{
		free(mem); 
		mem = NULL; 
	} 

	return (EMSR_OK);
}


HWND CreateHiddenWindow(void)
{
	HWND hwnd;
	WNDCLASS wc;

	// Register the Window Class

	wc.style			= 0;
	wc.lpfnWndProc		= HiddenWindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= g_hinst;
	wc.hIcon			= 0;
	wc.hCursor			= 0;
	wc.hbrBackground	= 0;
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= PGPTXT_MACHINE("EudoraPluginV4 Hidden Window");

	RegisterClass(&wc);

	hwnd = CreateWindow(PGPTXT_MACHINE("EudoraPluginV4 Hidden Window"), 
		PGPTXT_MACHINE("EudoraPluginV4 Hidden Window"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, NULL, NULL, g_hinst, NULL);

	return hwnd;
}


LRESULT CALLBACK HiddenWindowProc(HWND hwnd, 
								  UINT msg, 
								  WPARAM wParam, 
								  LPARAM lParam)
{
	static PGPBoolean	s_bReloadMessageShown		= FALSE;

	if (msg == WM_CREATE)
	{
		s_nPrefsReloadMsg = RegisterWindowMessage(PGP_RELOADPREFSMSG);
		ShowWindow(hwnd, SW_HIDE);
		return 0;
	}

	if (msg == WM_TIMER)
	{
		KillTimer(hwnd, 1);
		sWipeFile();
		return 0;
	}

	if (!s_bReloadMessageShown && (msg == s_nPrefsReloadMsg))
	{
		BOOL bEncryptByDefault;
		BOOL bSignByDefault;

		bEncryptByDefault = ByDefaultEncrypt(PGPPeekContextMemoryMgr(g_pgpContext));
		bSignByDefault = ByDefaultSign(PGPPeekContextMemoryMgr(g_pgpContext));

		if ((bEncryptByDefault != s_bEncryptByDefault) ||
			(bSignByDefault != s_bSignByDefault))
		{
			PGPChar szTitle[255];
			PGPChar szBuffer[1024];

			s_bReloadMessageShown = TRUE;

			PGPLoadString(g_hinst, IDS_TITLE_PGPERROR, szTitle, sizeof(szTitle)/sizeof(PGPChar));
			PGPLoadString(g_hinst, IDS_MUSTRELOAD, szBuffer, sizeof(szBuffer)/sizeof(PGPChar));
			MessageBox(GetForegroundWindow(), szBuffer, szTitle, MB_ICONEXCLAMATION);
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


static void sWipeFile()
{
	FILELIST *pList = NULL;

	if ((g_pgpContext != NULL) && (g_szWipeFile != NULL))
	{
		if (PGPscAddToFileList(&pList, g_szWipeFile, NULL, NULL))
			PGPscWipeFileList(NULL, g_pgpContext, pList, 
				WIPE_DONOTWARN | WIPE_DONOTPROGRESS,0);

		free(g_szWipeFile);
		g_szWipeFile = NULL;
	}

	return;
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
