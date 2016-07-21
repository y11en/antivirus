#define IMPEX_CURRENT_HEADER  "e_loader.h"
#include <Prague/iface/impexhlp.h>

#if !defined(IMPEX_TABLE_CONTENT) && !defined(PrHMODULE_defined)
	typedef const tVOID* PrHMODULE;
	#define PrHMODULE_defined
#endif

#if !defined(__e_loader_h) || defined(IMPEX_TABLE_CONTENT)

	#if ( IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DECLARE )
		#define __e_loader_h

		#define SKIP_THIS_MODULE "this_module"

		#define DBGINFO_CALLER_ADDR  0
		#define DBGINFO_MODULE_NAME  1
		#define DBGINFO_IMAGE_NAME   2
		#define DBGINFO_SYMBOL_NAME  3
		#define DBGINFO_SRC_FILE     4
		#define DBGINFO_SRC_LINE     5

		#define OSVERSION_UNDEF      0
		#define OSVERSION_9X         1
		#define OSVERSION_NT         2
		#define OSVERSION_2K		 3
		#define OSVERSION_XP		 4
		#define OSVERSION_WIN2003	 5 // XP x64, 2003 server...
		#define OSVERSION_VISTA		 6 // Vista, Longhorn...

		#define SHUTDOWN_LOGOFF      0x01
		#define SHUTDOWN_SHUTDOWN    0x02
		#define SHUTDOWN_RESTART     0x04
		#define SHUTDOWN_STANDBY     0x08
		#define SHUTDOWN_HIBERNATE   0x20
		#define SHUTDOWN_FORCE       0x10

		#define FAC_LOADER     2

		#define errSIGN_NOT_MATCH        PR_MAKE_IMP_ERR( PID_LOADER, 0x0 )
		#define errSIGN_BAD_KEY          PR_MAKE_IMP_ERR( PID_LOADER, 0x1 )
		#define errSIGN_NOTARY_NOT_FOUND PR_MAKE_IMP_ERR( PID_LOADER, 0x2 )

		// PrGetSystemPowerStatus constants
		#if !defined(AC_LINE_OFFLINE)
			#define AC_LINE_OFFLINE                 0x00
			#define AC_LINE_ONLINE                  0x01
			#define AC_LINE_BACKUP_POWER            0x02
			#define AC_LINE_UNKNOWN                 0xFF
			#define BATTERY_FLAG_HIGH               0x01
			#define BATTERY_FLAG_LOW                0x02
			#define BATTERY_FLAG_CRITICAL           0x04
			#define BATTERY_FLAG_CHARGING           0x08
			#define BATTERY_FLAG_NO_BATTERY         0x80
			#define BATTERY_FLAG_UNKNOWN            0xFF
			#define BATTERY_PERCENTAGE_UNKNOWN      0xFF
			#define BATTERY_LIFE_UNKNOWN            0xFFFFFFFF
		#endif

		#if defined(_DEBUG) || defined(USE_DBG_INFO)
			#define PrDbgOutput(params) PrOutputDebugString params
		#else
			#define PrDbgOutput(params)
		#endif

		#if 0
			#define PrDbgGetCallerInfo( szSkipModulesList, dwSkipLevels, pBuffer, dwSize)
			#define PrDbgGetInfoEx( dwAddress, szSkipModulesList, dwSkipLevels, dwInfoType, pBuffer, dwSize, pFFrame)
			#define PrDbgReport( nRptType,szFile,nLine,szModule,szFormat, var_args )
			#define PrDbgReport_v( nRptType,szFile,nLine,szModule,szFormat, var_args )
			#define PrDbgMsgWnd( nRptType, szFile, szLine, szModule, szUserMessage )
			#define PrDbgMsgBox( lpText, lpCaption, uType )

			// defines to make declared functions visible by Visual Assist
			#define PrMakeNamePrefixed(pname,size,global,local_prefix)    PrMakeNamePrefixed(pname,size,global,local_prefix)

			#define Now(pDT)                                 Now(pDT)
			#define NowGreenwich(pDT)                        NowGreenwich(pDT)
			#define PrLoadLibrary(lib_name,cp)               PrLoadLibrary(lib_name,cp)
			#define PrFreeLibrary(lib_handle)                PrFreeLibrary(lib_handle)
			#define PrGetProcAddress(lib_handle,proc_name)   PrGetProcAddress(lib_handle,proc_name)
			#define PrOutputDebugString                      PrOutputDebugString
			#define PrOutputDebugString_v(szFormat, pStack)  PrOutputDebugString_v(szFormat, pStack)
			#define CheckStackAvail(dwProbeSize)             CheckStackAvail(dwProbeSize)
			#define PrGetThreadId()                          PrGetThreadId()
			#define PrGetProcessId()                         PrGetProcessId()
			#define PrSessionId(pid)                         PrSessionId(pid)

			#define pr_strtol(str,endpoint,radix)            pr_strtol(str,endpoint,radix)
			#define pr_strtoul(str,endpoint,radix)           pr_strtoul(str,endpoint,radix)
			#define pr_val2str(buff,len,value,type)          pr_val2str(buff,len,value,type)
			#define pr_str2val(value,len,type,str)           pr_str2val(value,len,type,str)
			#define pr_rand(range)                           pr_rand(range)

			#define GetCallerId( pStackPtr, pdwIDs, dwCount) GetCallerId( pStackPtr, pdwIDs, dwCount)
			#define SignCheck(hObject, bUseDefaultKey, pKeysArray, dwKeyCount, bAllOf, pdwSignLibErrorCode) SignCheck(hObject, bUseDefaultKey, pKeysArray, dwKeyCount, bAllOf, pdwSignLibErrorCode)
			#define PrSignCheckV2(dskm_sess, hObject, szCompId, szObjType, pdwSignLibErrorCode) PrSignCheckV2(dskm_sess, hObject, szCompId, szObjType, pdwSignLibErrorCode)
			#define SignGet(obj,keys,key_count,keys_found,sign_lib_error_code) SignGet(obj,keys,key_count,keys_found,sign_lib_error_code)
			#define PrSignGetContentLen(buff,buff_size,content_len)    PrSignGetContentLen(buff,buff_size,content_len)
			#define PrGetSystemPowerStatus(pnACLineStatus, pnBatteryFlag, pnBatteryLifePercent, pnBatteryLifeTimeSec, pnBatteryFullLifeTimeSec) PrGetSystemPowerStatus(pnACLineStatus, pnBatteryFlag, pnBatteryLifePercent, pnBatteryLifeTimeSec, pnBatteryFullLifeTimeSec)
			#define PrGetTickCount()                                   PrGetTickCount()
			#define PrSleep(ms)                                        PrSleep(ms)

			#define PrGetOsVersion()                                   PrGetOsVersion()
			#define PrMakeUniqueString(buff,size,prefix)               PrMakeUniqueString(buff,size,prefix)
			#define PrSafelyReadMemory(buff,dst,size,read)             PrSafelyReadMemory(buff,dst,size,read)

			#define PrIsSafeBoot()                                     PrIsSafeBoot()
			#define PrShutDownSystem(nFlags)                           PrShutDownSystem(nFlags)

			#define PrSetEnviroment(reg_obj, subscriber_obj)           PrSetEnviroment(reg_obj, subscriber_obj)
			#define PrSignReinit(reg_folder)                           PrSignReinit(reg_folder)

			#define PrExpandEnvironmentVars(hStr, hResult)             PrExpandEnvironmentVars(hStr, hResult)
			#define PrExpandEnvironmentVarsStr(hStr,str_to_expand,cp)  PrExpandEnvironmentVarsStr(hStr,str_to_expand,cp)
			#define PrInterlockedIncrement(pAddend)                    PrInterlockedIncrement(pAddend)
			#define PrInterlockedDecrement(pAddend)                    PrInterlockedDecrement(pAddend)
			#define PrInterlockedExchange(pTarget,value)               PrInterlockedExchange(pTarget,value)
			#define PrInterlockedCompareExchange(pDestination,exchange,comperand) PrInterlockedCompareExchange(pDestination,exchange,comperand)

			#define PrOpenEvent( desired_access, inheritable, name )                  PrOpenEvent( desired_access, inheritable, name )
			#define PrOpenMutex( desired_access, inheritable, name )                  PrOpenMutex( desired_access, inheritable, name )
			#define PrOpenSemaphore( desired_access, inheritable, name )              PrOpenSemaphore( desired_access, inheritable, name )
			#define PrCreateEvent( sec_attr, manual_reset, initial_state, name )      PrCreateEvent( sec_attr, manual_reset, initial_state, name )
			#define PrCreateMutex( sec_attr, initial_owner, name )                    PrCreateMutex( sec_attr, initial_owner, name )
			#define PrCreateSemaphore( sec_attr, initial_count, maximum_count, name ) PrCreateSemaphore( sec_attr, initial_count, maximum_count, name )
		#endif

	#endif

	IMPEX_BEGIN


		IMPEX_NAME_ID( ECLSID_LOADER, 0x1b93b99fl, tDWORD, PrMakeNamePrefixed,    ( tCHAR* pname, tDWORD size, tBOOL global, const tCHAR* local_prefix ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xa941ea2fl, tERROR, Now,                   ( tDT *dt ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x3a5a2bbel, tERROR, NowGreenwich,          ( tDT *dt ) )
	//IMPEX_NAME_ID( ECLSID_LOADER, 0x42819d01l, tDWORD, PrConvertDate,         ( tDT *dt ) )
	//IMPEX_NAME_ID( ECLSID_LOADER, 0x33960e29l, tERROR, PrConvertToDT,         ( tDWORD time_t, tDT* dt) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xded9ffb5l, PrHMODULE, PrLoadLibrary,      ( const tVOID* lib_name, tCODEPAGE cp ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x7f60fcfbl, tERROR,    PrFreeLibrary,      ( PrHMODULE lib_handle ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x92ff8e34l, tFUNC_PTR, PrGetProcAddress,   ( PrHMODULE lib_handle, const tCHAR* proc_name ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x25f0bac6l, tERROR, SignCheck,             ( hOBJECT obj, tBOOL use_default_key, tVOID* keys[], tDWORD key_count, tBOOL all_of, tDWORD* sign_lib_error_code ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x6a76e829l, tERROR, SignGet,               ( hOBJECT obj, tVOID* keys[], tDWORD key_count, tDWORD* keys_found, tDWORD* sign_lib_error_code ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x1d587664l, tERROR, PrSignGetContentLen,   ( const tVOID* buff, tUINT buff_size, tUINT* content_len ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x316657e6l, tERROR, PrOutputDebugString,   ( tCHAR* format, ... ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x38b72636l, tERROR, PrOutputDebugString_v, ( tCHAR* format, tPTR stack_addr ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x259d3150l, tERROR, CheckStackAvail,       ( tDWORD probe_size ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xd758e9b3l, tDWORD, PrGetThreadId,         () )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x0ccf0dc5a, tDWORD, PrGetProcessId,        () )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xbc2d1a96l, tDWORD, PrSessionId,           ( tDWORD pid ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x97bbdd5dl, tERROR, GetCallerId,           ( tPTR stack_ptr, tDATA* ids, tDWORD count ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x98091328l, tINT,   pr_strtol,             ( const tCHAR* str, tCHAR** endpoint, tUINT radix) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x382bca0fl, tUINT,  pr_strtoul,            ( const tCHAR* str, tCHAR** endpoint, tUINT radix) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xae8f0eb4l, tERROR, pr_val2str,            ( tCHAR* buff, tUINT len, tVOID* value, tTYPE_ID type) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xbb0d8244l, tERROR, pr_str2val,            ( tVOID* value, tUINT len, tTYPE_ID type, const tCHAR* str ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xb42fd1edl, tDWORD, pr_rand,               ( tDWORD range ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x0c8fa402l, tDWORD, PrGetTickCount,        () )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xf08380b6l, tERROR, PrSleep,               (tDWORD mlsec) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x78fb55f6l, tDWORD, PrGetOsVersion,        () )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x36cbc824l, tDWORD, PrMakeUniqueString,    (tCHAR* buff, tDWORD size, const tCHAR* prefix) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x02b375dcl, tERROR, PrSafelyReadMemory,    (const tVOID* buff, tVOID* dst, tDWORD size, tDWORD* read) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xf4edddc3l, tBOOL,  PrIsSafeBoot,		      () )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x061042b6c, tBOOL,  PrShutDownSystem,      (tDWORD nFlags) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xd7e5eb5dl, tLONG,  PrInterlockedIncrement,(tLONG* pAddend) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x7bc0105cl, tLONG,  PrInterlockedDecrement,(tLONG* pAddend) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xb662622fl, tLONG,  PrInterlockedExchange, (tLONG* pTarget, tLONG value) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x2f3932a4l, tLONG,  PrInterlockedCompareExchange, (tLONG* pDestination, tLONG exchange, tLONG comperand) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xf6ec6f57,  tERROR, PrSetEnviroment,       (hOBJECT reg_obj, hOBJECT subscriber_obj) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xeb3a4f4cl, tERROR, PrExpandEnvironmentVars,    (hOBJECT str, hOBJECT result) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xac8e1b2el, tERROR, PrExpandEnvironmentVarsStr, (hOBJECT str, const tVOID* str_to_expand, tCODEPAGE cp) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xeb3a4f5dl, tERROR, PrGetSystemPowerStatus,     (tDWORD* pnACLineStatus, tDWORD* pnBatteryFlag, tDWORD* pnBatteryLifePercent, tDWORD* pnBatteryLifeTimeSec, tDWORD* pnBatteryFullLifeTimeSec) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x76f36973l, tERROR, PrSignCheckV2,              ( const tVOID* dskm_sess, hOBJECT obj, const tCHAR* szCompId, const tCHAR* szObjType, tDWORD* sign_lib_error_code ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xb5008956l, tERROR, PrSignReinit,               ( hOBJECT/*must be hSTRING*/ reg_folder ) )
		
		IMPEX_NAME_ID( ECLSID_LOADER, 0xe0bdadefl, tERROR, PrDbgGetCallerInfo,         ( tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tCHAR* pBuffer, tDWORD dwSize ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x9e6207c7l, tERROR, PrDbgGetInfoEx,             ( tDWORD dwAddress, tCHAR* szSkipModulesList, tDWORD dwSkipLevels, tDWORD dwInfoType, tVOID* pBuffer, tDWORD dwSize, tDWORD* pFFrame ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x9bc3bbfdl, tPTR, PrOpenEvent,       ( tDWORD desired_access, tBOOL inheritable, const tCHAR* name ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x4ea00375l, tPTR, PrOpenMutex,       ( tDWORD desired_access, tBOOL inheritable, const tCHAR* name ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x9c5644c5l, tPTR, PrOpenSemaphore,   ( tDWORD desired_access, tBOOL inheritable, const tCHAR* name ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x057f9872l, tPTR, PrCreateEvent,     ( tPTR sec_attr, tBOOL manual_reset, tBOOL initial_state, const tCHAR* name ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xd01c20fal, tPTR, PrCreateMutex,     ( tPTR sec_attr, tBOOL initial_owner, const tCHAR* name ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x4ca045e6l, tPTR, PrCreateSemaphore, ( tPTR sec_attr, tDWORD initial_count, tDWORD maximum_count, const tCHAR* name ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xb86bb317l, tDWORD, PrStartIdle,     () )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x1a679d07l, tDWORD, PrStopIdle,      ( tBOOL force ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0x58955a31l, tBOOL,  PrIsIntegralPlugin, ( tPID pid ) )

		IMPEX_NAME_ID( ECLSID_LOADER, 0xcc656ddal, tINT,   PrDbgReport,        ( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, ... ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0x8d487b78l, tINT,   PrDbgReport_v,      ( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, va_list arglist ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xcd332981l, tINT,   PrDbgMsgWnd,        ( tINT nRptType, const tCHAR* szFile, const tCHAR* szLine, const tCHAR* szModule, const tCHAR* szUserMessage ) )
		IMPEX_NAME_ID( ECLSID_LOADER, 0xdac42514l, tINT,   PrDbgMsgBox,        ( const tCHAR* lpText, const tCHAR* lpCaption, tUINT uType ) )
	IMPEX_END

#endif
