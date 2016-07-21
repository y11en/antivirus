// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  30 November 2005,  09:12 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Alexey Antropov
// File Name   -- startupenum2.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __startupenum2_cpp__4e1fd007_d4be_409e_a652_c75739be435e )
#define __startupenum2_cpp__4e1fd007_d4be_409e_a652_c75739be435e
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



class EnumContext;
struct StartUpEnum2;

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_inifile.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_hash_md5.h>
#include <Prague/plugin/p_inifile.h>

#include <AV/plugin/p_startupenum2.h>
#include <AV/structs/s_startupenum2.h>
// AVP Prague stamp end

#include <Windows.h>
#include <shlobj.h>
#include <Mstask.h>

#include "EvCache.h"
#include "defines.h"
#include "Initialization.h"
#include "EnumFunc.h"
#include "regenum.h"
#include "ParsLineForFileNames.h"

//typedef struct _UNLOAD_PROFILE_NAMES{
//	DWORD		Count;
//	DWORD		cProfileName[1];
//}UNLOAD_PROFILE_NAMES, *pUNLOAD_PROFILE_NAMES;
//

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable StartUpEnum2 : public cStartUpEnum2 /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call StartEnum( hOBJECT p_pRetObject, tBOOL p_bAdvancedDisinfection );
//	tERROR pr_call CleanUp( hSTRING p_DelFName, hOBJECT p_pRetObject );
	tERROR pr_call CleanUp( hSTRING p_DelFName, hOBJECT p_pRetObject, tSTRING p_DetectedVirusName );

// Data declaration
// AVP Prague stamp end

	

public:
	hDECODEIO				m_hDecodeIO;
	hINIFILE				m_hBaseIni;
	tPROPID					m_propId_PROGRESS;

	HMODULE					m_hRegLib;
	HMODULE					m_hFileLib;
	HMODULE					m_hSecurityLib;
	HMODULE					m_hMsiLib;
	HMODULE					m_hShellLib;

	HANDLE					m_hDevice;
	DWORD					m_AppID;
	HANDLE					m_hWhistleup;
	HANDLE					m_hWFChanged;

	HOSTS_ANALYSE_CONTEXT	m_HostsStrings;

	LONG					m_nScanActiv;
	tBOOL					m_bHashKnownFilesExist;

	PHashTree				m_pHashKnownFiles;

	wchar_t*				m_sOBJECT_FULL_NAME;
	tDWORD					m_sizeOFN;

	tBOOL					m_bMainAdvancedDisinfection;
	CRITICAL_SECTION        m_sCleanupCriticalSection;
	CRITICAL_SECTION	    m_sCommonCriticalSection;

	cPrStrW 				m_sSystemDrive;
	cPrStrW 				m_sWindowsDirectory;
	cPrStrW 				m_sSystemDirectory;
	cPrStrW 				m_sSystem32Directory;
	cPrStrW 				m_sSystem32DriversDirectory;
	LONG                    m_nModifyCounter;
	tDWORD                  m_nPathEnvLen;

	tBOOL					m_bRegUsersLoaded;

	pUNLOAD_PROFILE_NAMES	pNotLoadedProfilesList;

	tERROR					ReInitObjects();
private:
	tERROR                  FreeHostsLinks();
	void					LoadProfiles();
	void					UnLoadProfiles();
	BOOL					Privilege(BOOL bEnable,PTOKEN_PRIVILEGES* pOldToken);
	tERROR					LoadSubCureLibrary();
	tERROR					DeInitLibrary(/*tBYTE* hBuf,*/hPLUGININSTANCE hInst/*,tPluginInit pSpecialRepairMain*/);
//	tERROR					InitLibrary(hPLUGININSTANCE hInst,tPluginInit pSpecialRepairMain,pGET_FUNCTIONS  sFunction);
//	tERROR					MapLibrary(const tWCHAR* szPluginName, tBYTE** pImage, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint);
	pUNLOAD_PROFILE_NAMES	pUnloadProfilesList;
	hPLUGININSTANCE			hSubCureLib;
//	tBYTE*					hSubCureFreeBuf;
//	tPluginInit				pfSpecialRepairMain;
	pfSpecialRepairPtr		pfSpecialRepair;
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(StartUpEnum2)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
//! #define pmc_STARTUPENUM2 0x5d55e35e //
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// StartUpEnum2 interface implementation
// Short comments --
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call StartUpEnum2_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure StartUpEnum2 is declared in O:\PPP\StartUpEnum2\startupenum2.c source file
  // data structure  is declared in O:\PPP\StartUpEnum2\startupenum2.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // startupenum2_cpp
// AVP Prague stamp end



