// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  03 May 2007,  19:53 --------
// File Name   -- (null)i_productlogic.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_productlogic__9fb22ffd_ccba_4d8a_93fd_3f04c05827bd )
#define __i_productlogic__9fb22ffd_ccba_4d8a_93fd_3f04c05827bd
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ProductLogic interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_PRODUCTLOGIC  ((tIID)(43))
// AVP Prague stamp end



/*
// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tCustomizeSettingsDirection; // direction of a settings customization
// AVP Prague stamp end



*/
typedef enum enCustomizeSettingsDirection {
	csdFROM_DATA_TO_TASK = 1,
	csdFROM_DATA_TO_GUI,
	csdFROM_GUI_TO_DATA,
	csdDATA_INIT,
	csdFROM_TASK_TO_DATA,
	csdFROM_POLICY_TO_DATA,
} tCustomizeSettingsDirection;


/*
// AVP Prague stamp begin( Interface constants,  )
#define csdFROM_DATA_TO_TASK           ((tCustomizeSettingsDirection)(1)) // customization from settings storage to task
#define csdFROM_DATA_TO_GUI            ((tCustomizeSettingsDirection)(2)) //  --
#define csdFROM_GUI_TO_DATA            ((tCustomizeSettingsDirection)(3)) // customization from user interface to settings storage
#define csdDATA_INIT                   ((tCustomizeSettingsDirection)(4)) //  --
#define csdFROM_TASK_TO_DATA           ((tCustomizeSettingsDirection)(5)) //  --
// AVP Prague stamp end



*/
// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_PRODUCT 0x2937151d //

	#define pm_PRODUCT_STATE_CHANGED 0xc1d5efa5 // (3252023205) --

	#define pm_PRODUCT_STATISTICS_CHANGED 0xd51f059b // (3575580059) --

	#define pm_PRODUCT_KLOP_LIMETED 0xd51f059c // (3575580060) --

	#define pm_FULLSCAN_COMPLETED 0x45aa72e6 // (1168798438) --

// message class
#define pmc_REBOOT 0xb37f57b2 //

	#define pm_REBOOT_APPLICATION 0x00001000 // (4096) --

	#define pm_REBOOT_OS 0x00001001 // (4097) --

// message class
#define pmc_PRODUCT_UPDATE 0x0ac7ff79 //

	#define pm_UPDATE_EXECUTABLES 0x047452ec // (74732268) --

// message class
#define pmc_PRODUCT_STARTUP_CHANGE 0xb88c6d59 // request for changing product startup state

	#define pm_PRODUCT_STARTUP_CHANGE 0x00001000 // (4096) --
	// data content comment
	//    pointer to tBOOL value. cTRUE means: product must be started on OS boot, cFALSE - otherwise

// message class
#define pmc_PRODUCT_NOTIFICATION 0xa5b477be //

// message class
#define pmc_FSSYNC 0xae5e2ea3 //

	#define pm_FSSYNC_RegisterInvisibleThread 0x00001000 // (4096) --

	#define pm_FSSYNC_UnregisterInvisibleThread 0x00001001 // (4097) --

// message class
#define pmc_SETTINGS 0x15b04e90 //

	#define pm_SAVE_SETTINGS 0xe0d3a62c // (3771967020) --

	#define pm_LOAD_SETTINGS 0xf29abcab // (4070227115) --

	#define pm_SETTINGS_UPGRADE 0xf29abcac // (4070227116) -- Upgrades settings from previous releases
	// context comment
	//    NULL
	// data content comment
	//    pointer to cProfileEx "Protection" profile

// message class
#define pmc_PRODUCT_OPERATION 0xf1f532d2 //

	#define pm_PRODUCT_CHECK_STATUS 0xfcfbc360 // (4244357984) --

	#define pm_FW_UPDATE_NETWORKS 0x5ede6106 // (1591632134) --

	#define pm_SRVMGR_REQUEST 0x8e1d2a7f // (2384276095) --

	#define pm_FW_ADD_NETWORK 0xfcfbc361 // (4244357985) --

	#define pm_FW_DELETE_NETWORK 0xfcfbc362 // (4244357986) --

	#define pm_FW_CHANGE_NETWORK 0xfcfbc363 // (4244357987) --

// message class
#define pmc_LOCALIZE_INFO 0x54c1ec56 //

	#define pm_LOCALIZE_NOTIFICATION 0x7ea80f1e // (2124943134) --

	#define pm_LOCALIZE_PROFILE_NAME 0xe8d6a742 // (3906381634) --
// AVP Prague stamp end



// message class
//#define pmc_REMOTE_GLOBAL 0x3e9642f5 //

	#define pm_REMOTE_GLOBAL_SELFPROT_ENABLED 0xf39b5e52

	#define pm_REMOTE_GLOBAL_SERVICECONTROL_ENABLED 0x27ce7c62

	#define pm_REMOTE_GLOBAL_SELFPROT_INFO 0x19c0f5f7

	#define pm_REMOTE_GLOBAL_SET_AFFINITYMASK 0xe41ac530

	#define pm_REMOTE_GLOBAL_SET_CHECK_ACTIVITY 0xd9bdf5fe

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hPRODUCTLOGIC;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iProductLogicVtbl;
typedef struct iProductLogicVtbl iProductLogicVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cProductLogic;
	typedef cProductLogic* hPRODUCTLOGIC;
#else
	typedef struct tag_hPRODUCTLOGIC 
	{
		const iProductLogicVtbl* vtbl; // pointer to the "ProductLogic" virtual table
		const iSYSTEMVtbl*       sys;  // pointer to the "SYSTEM" virtual table
	} *hPRODUCTLOGIC;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( ProductLogic_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpProductLogic_SetSettings)           ( hPRODUCTLOGIC _this, const cSerializable* settings ); // -- ;
	typedef   tERROR (pr_call *fnpProductLogic_CustomizeSettings)     ( hPRODUCTLOGIC _this, const tCHAR* profile, cSerializable* settings, tDWORD level, tCustomizeSettingsDirection direction, tBOOL* make_persistent ); // -- setarates settings for the embedded profile;
	typedef   tERROR (pr_call *fnpProductLogic_SelectSettings)        ( hPRODUCTLOGIC _this, const tCHAR* profile, const cSerializable* settings, const tCHAR* sub_profile, cSerializable* sub_settings ); // -- setarates settings for the embedded profile;
	typedef   tERROR (pr_call *fnpProductLogic_GetStatistics)         ( hPRODUCTLOGIC _this, const tCHAR* profile, cSerializable* statistics ); // -- ;
	typedef   tERROR (pr_call *fnpProductLogic_CalcStatistics)        ( hPRODUCTLOGIC _this, const tCHAR* profile_name, const cSerializable* preliminary_data, cSerializable* statistics ); // -- returns combined task profile, task settings or task schedule settings;
	typedef   tERROR (pr_call *fnpProductLogic_GetService)            ( hPRODUCTLOGIC _this, tTaskId task_id, hOBJECT client, tServiceId service_id, hOBJECT* service ); // -- returns extension interface implemented by task;
	typedef   tERROR (pr_call *fnpProductLogic_ReleaseService)        ( hPRODUCTLOGIC _this, tTaskId task_id, hOBJECT service ); // -- ;
	typedef   tERROR (pr_call *fnpProductLogic_AskAction)             ( hPRODUCTLOGIC _this, const cSerializable* task_info, tActionId action_id, cSerializable* action_info ); // -- ;
	typedef   tERROR (pr_call *fnpProductLogic_TestUpdaterConnection) ( hPRODUCTLOGIC _this, const cSerializable* Settings ); // -- ;
	typedef   tERROR (pr_call *fnpProductLogic_FilterTaskMessage)     ( hPRODUCTLOGIC _this, const cSerializable* task_info, const cSerializable* report_info, tDWORD msg_cls, tDWORD msg_id, cSerializable* params ); // -- if returns errOK - TM will save it into report;
	typedef   tERROR (pr_call *fnpProductLogic_GetSettings)           ( hPRODUCTLOGIC _this, cSerializable* settings ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iProductLogicVtbl 
{
	fnpProductLogic_SetSettings            SetSettings;
	fnpProductLogic_CustomizeSettings      CustomizeSettings;
	fnpProductLogic_SelectSettings         SelectSettings;
	fnpProductLogic_GetStatistics          GetStatistics;
	fnpProductLogic_CalcStatistics         CalcStatistics;
	fnpProductLogic_GetService             GetService;
	fnpProductLogic_ReleaseService         ReleaseService;
	fnpProductLogic_AskAction              AskAction;
	fnpProductLogic_TestUpdaterConnection  TestUpdaterConnection;
	fnpProductLogic_FilterTaskMessage      FilterTaskMessage;
	fnpProductLogic_GetSettings            GetSettings;
}; // "ProductLogic" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( ProductLogic_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgPROFILE_CONFIG          mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001000 )
#define pgTM_OBJECT               mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001001 )
#define pgBL_PRODUCT_NAME         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001004 )
#define pgBL_PRODUCT_VERSION      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001005 )
#define pgBL_PRODUCT_PATH         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001006 )
#define pgBL_PRODUCT_DATA_PATH    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001007 )
#define pgBL_COMPANY_NAME         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001008 )
#define pgBL_PRODUCT_DATA_STORAGE mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001009 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_ProductLogic_SetSettings( _this, settings )                                                                    ((_this)->vtbl->SetSettings( (_this), settings ))
	#define CALL_ProductLogic_CustomizeSettings( _this, profile, settings, level, direction, make_persistent )                  ((_this)->vtbl->CustomizeSettings( (_this), profile, settings, level, direction, make_persistent ))
	#define CALL_ProductLogic_SelectSettings( _this, profile, settings, sub_profile, sub_settings )                             ((_this)->vtbl->SelectSettings( (_this), profile, settings, sub_profile, sub_settings ))
	#define CALL_ProductLogic_GetStatistics( _this, profile, statistics )                                                       ((_this)->vtbl->GetStatistics( (_this), profile, statistics ))
	#define CALL_ProductLogic_CalcStatistics( _this, profile_name, preliminary_data, statistics )                               ((_this)->vtbl->CalcStatistics( (_this), profile_name, preliminary_data, statistics ))
	#define CALL_ProductLogic_GetService( _this, task_id, client, service_id, service )                                         ((_this)->vtbl->GetService( (_this), task_id, client, service_id, service ))
	#define CALL_ProductLogic_ReleaseService( _this, task_id, service )                                                         ((_this)->vtbl->ReleaseService( (_this), task_id, service ))
	#define CALL_ProductLogic_AskAction( _this, task_info, action_id, action_info )                                             ((_this)->vtbl->AskAction( (_this), task_info, action_id, action_info ))
	#define CALL_ProductLogic_TestUpdaterConnection( _this, Settings )                                                          ((_this)->vtbl->TestUpdaterConnection( (_this), Settings ))
	#define CALL_ProductLogic_FilterTaskMessage( _this, task_info, report_info, msg_cls, msg_id, params )                       ((_this)->vtbl->FilterTaskMessage( (_this), task_info, report_info, msg_cls, msg_id, params ))
	#define CALL_ProductLogic_GetSettings( _this, settings )                                                                    ((_this)->vtbl->GetSettings( (_this), settings ))
#else // if !defined( __cplusplus )
	#define CALL_ProductLogic_SetSettings( _this, settings )                                                                    ((_this)->SetSettings( settings ))
	#define CALL_ProductLogic_CustomizeSettings( _this, profile, settings, level, direction, make_persistent )                  ((_this)->CustomizeSettings( profile, settings, level, direction, make_persistent ))
	#define CALL_ProductLogic_SelectSettings( _this, profile, settings, sub_profile, sub_settings )                             ((_this)->SelectSettings( profile, settings, sub_profile, sub_settings ))
	#define CALL_ProductLogic_GetStatistics( _this, profile, statistics )                                                       ((_this)->GetStatistics( profile, statistics ))
	#define CALL_ProductLogic_CalcStatistics( _this, profile_name, preliminary_data, statistics )                               ((_this)->CalcStatistics( profile_name, preliminary_data, statistics ))
	#define CALL_ProductLogic_GetService( _this, task_id, client, service_id, service )                                         ((_this)->GetService( task_id, client, service_id, service ))
	#define CALL_ProductLogic_ReleaseService( _this, task_id, service )                                                         ((_this)->ReleaseService( task_id, service ))
	#define CALL_ProductLogic_AskAction( _this, task_info, action_id, action_info )                                             ((_this)->AskAction( task_info, action_id, action_info ))
	#define CALL_ProductLogic_TestUpdaterConnection( _this, Settings )                                                          ((_this)->TestUpdaterConnection( Settings ))
	#define CALL_ProductLogic_FilterTaskMessage( _this, task_info, report_info, msg_cls, msg_id, params )                       ((_this)->FilterTaskMessage( task_info, report_info, msg_cls, msg_id, params ))
	#define CALL_ProductLogic_GetSettings( _this, settings )                                                                    ((_this)->GetSettings( settings ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iProductLogic 
	{
		virtual tERROR pr_call SetSettings( const cSerializable* settings ) = 0;
		virtual tERROR pr_call CustomizeSettings( const tCHAR* profile, cSerializable* settings, tDWORD level, tCustomizeSettingsDirection direction, tBOOL* make_persistent ) = 0; // -- setarates settings for the embedded profile
		virtual tERROR pr_call SelectSettings( const tCHAR* profile, const cSerializable* settings, const tCHAR* sub_profile, cSerializable* sub_settings ) = 0; // -- setarates settings for the embedded profile
		virtual tERROR pr_call GetStatistics( const tCHAR* profile, cSerializable* statistics ) = 0;
		virtual tERROR pr_call CalcStatistics( const tCHAR* profile_name, const cSerializable* preliminary_data, cSerializable* statistics ) = 0; // -- returns combined task profile, task settings or task schedule settings
		virtual tERROR pr_call GetService( tTaskId task_id, hOBJECT client, tServiceId service_id, hOBJECT* service ) = 0; // -- returns extension interface implemented by task
		virtual tERROR pr_call ReleaseService( tTaskId task_id, hOBJECT service ) = 0;
		virtual tERROR pr_call AskAction( const cSerializable* task_info, tActionId action_id, cSerializable* action_info ) = 0;
		virtual tERROR pr_call TestUpdaterConnection( const cSerializable* Settings ) = 0;
		virtual tERROR pr_call FilterTaskMessage( const cSerializable* task_info, const cSerializable* report_info, tDWORD msg_cls, tDWORD msg_id, cSerializable* params ) = 0; // -- if returns errOK - TM will save it into report
		virtual tERROR pr_call GetSettings( cSerializable* settings ) = 0;
	};

	struct pr_abstract cProductLogic : public iProductLogic, public iObj 
	{

		OBJ_IMPL( cProductLogic );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hPRODUCTLOGIC()   { return (hPRODUCTLOGIC)this; }

		hOBJECT pr_call get_pgPROFILE_CONFIG() { return (hOBJECT)getObj(pgPROFILE_CONFIG); }
		tERROR pr_call set_pgPROFILE_CONFIG( hOBJECT value ) { return setObj(pgPROFILE_CONFIG,(hOBJECT)value); }

		hOBJECT pr_call get_pgTM_OBJECT() { return (hOBJECT)getObj(pgTM_OBJECT); }
		tERROR pr_call set_pgTM_OBJECT( hOBJECT value ) { return setObj(pgTM_OBJECT,(hOBJECT)value); }

		tERROR pr_call get_pgBL_PRODUCT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_NAME,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_NAME,buff,size,cp); }

		tERROR pr_call get_pgBL_PRODUCT_VERSION( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_VERSION,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_VERSION( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_VERSION,buff,size,cp); }

		tERROR pr_call get_pgBL_PRODUCT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_PATH,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_PATH,buff,size,cp); }

		tERROR pr_call get_pgBL_PRODUCT_DATA_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_PRODUCT_DATA_PATH,buff,size,cp); }
		tERROR pr_call set_pgBL_PRODUCT_DATA_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_PRODUCT_DATA_PATH,buff,size,cp); }

		tERROR pr_call get_pgBL_COMPANY_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBL_COMPANY_NAME,buff,size,cp); }
		tERROR pr_call set_pgBL_COMPANY_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBL_COMPANY_NAME,buff,size,cp); }

		hOBJECT pr_call get_pgBL_PRODUCT_DATA_STORAGE() { return (hOBJECT)getObj(pgBL_PRODUCT_DATA_STORAGE); }
		tERROR pr_call set_pgBL_PRODUCT_DATA_STORAGE( hOBJECT value ) { return setObj(pgBL_PRODUCT_DATA_STORAGE,(hOBJECT)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_productlogic__9fb22ffd_ccba_4d8a_93fd_3f04c05827bd
// AVP Prague stamp end



