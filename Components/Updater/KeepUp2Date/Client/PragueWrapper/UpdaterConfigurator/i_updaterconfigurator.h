// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  05 October 2007,  18:32 --------
// File Name   -- (null)i_updaterconfigurator.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_updaterconfigurator__2ea3d3f6_ad1c_466c_afdd_eaf5a1eda196 )
#define __i_updaterconfigurator__2ea3d3f6_ad1c_466c_afdd_eaf5a1eda196
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// UpdaterConfigurator interface implementation
// Short comments -- interface to configure Updater
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_UPDATERCONFIGURATOR  ((tIID)(24000))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_UPDATER_CONFIGURATOR 0xead1419d // updater settings request from product

	#define pm_GET_SETTINGS 0x00001000 // (4096) -- asks product state whether some components are not to be updated

	#define pm_UPDATE_RESULT 0x00001001 // (4097) --

	#define pm_UPDATE_SCHEDULE 0x00001002 // (4098) --

	#define pm_ROLLBACK_AVAILABILITY 0x00001003 // (4099) --

	#define pm_CHECK_BASES_DATE 0x00001004 // (4100) -- Check downloading bases date before download
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hUPDATERCONFIGURATOR;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iUpdaterConfiguratorVtbl;
typedef struct iUpdaterConfiguratorVtbl iUpdaterConfiguratorVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cUpdaterConfigurator;
	typedef cUpdaterConfigurator* hUPDATERCONFIGURATOR;
#else
	typedef struct tag_hUPDATERCONFIGURATOR 
	{
		const iUpdaterConfiguratorVtbl* vtbl; // pointer to the "UpdaterConfigurator" virtual table
		const iSYSTEMVtbl*              sys;  // pointer to the "SYSTEM" virtual table
	} *hUPDATERCONFIGURATOR;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( UpdaterConfigurator_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef void (*fnpUpdaterConfigurator_dummy)( hUPDATERCONFIGURATOR _this );


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iUpdaterConfiguratorVtbl 
{
	fnpUpdaterConfigurator_dummy  dummy;
}; // "UpdaterConfigurator" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( UpdaterConfigurator_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION               mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT               mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgupdateDate                      mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001002 )
#define pgupdateOnlyBlackList             mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001003 )
#define pgcomponentsToUpdate              mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001004 )
#define pgupdateResultFixNames            mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001006 )
#define pgupdateResultNeedReboot          mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001008 )
#define pgupdateScheduleTimeoutOnSuccess  mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001009 )
#define pgupdateScheduleTimeoutOnFailure  mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000100a )
#define pgrollbackAvailable               mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100b )
#define pgretranslateOnlyListedComponents mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100c )
#define pgcomponentsToRetranslate         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x0000100d )
#define pgrollbackMode                    mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100e )
#define pginteractionWithUserEnabled      mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000100f )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
#else // if !defined( __cplusplus )
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)

	struct cUpdaterConfigurator : private iEmpty, public iObj 
	{

		OBJ_IMPL( cUpdaterConfigurator );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hUPDATERCONFIGURATOR()   { return (hUPDATERCONFIGURATOR)this; }

		tERROR pr_call get_pgupdateDate( tDATETIME* value ) { return get(pgupdateDate,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgupdateDate( tDATETIME* value ) { return set(pgupdateDate,value,sizeof(tDATETIME)); }

		tBOOL pr_call get_pgupdateOnlyBlackList() { return (tBOOL)getBool(pgupdateOnlyBlackList); }
		tERROR pr_call set_pgupdateOnlyBlackList( tBOOL value ) { return setBool(pgupdateOnlyBlackList,(tBOOL)value); }

		tERROR pr_call get_pgcomponentsToUpdate( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgcomponentsToUpdate,buff,size,cp); }
		tERROR pr_call set_pgcomponentsToUpdate( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgcomponentsToUpdate,buff,size,cp); }

		tERROR pr_call get_pgupdateResultFixNames( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgupdateResultFixNames,buff,size,cp); }
		tERROR pr_call set_pgupdateResultFixNames( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgupdateResultFixNames,buff,size,cp); }

		tBOOL pr_call get_pgupdateResultNeedReboot() { return (tBOOL)getBool(pgupdateResultNeedReboot); }
		tERROR pr_call set_pgupdateResultNeedReboot( tBOOL value ) { return setBool(pgupdateResultNeedReboot,(tBOOL)value); }

		tDWORD pr_call get_pgupdateScheduleTimeoutOnSuccess() { return (tDWORD)getDWord(pgupdateScheduleTimeoutOnSuccess); }
		tERROR pr_call set_pgupdateScheduleTimeoutOnSuccess( tDWORD value ) { return setDWord(pgupdateScheduleTimeoutOnSuccess,(tDWORD)value); }

		tDWORD pr_call get_pgupdateScheduleTimeoutOnFailure() { return (tDWORD)getDWord(pgupdateScheduleTimeoutOnFailure); }
		tERROR pr_call set_pgupdateScheduleTimeoutOnFailure( tDWORD value ) { return setDWord(pgupdateScheduleTimeoutOnFailure,(tDWORD)value); }

		tBOOL pr_call get_pgrollbackAvailable() { return (tBOOL)getBool(pgrollbackAvailable); }
		tERROR pr_call set_pgrollbackAvailable( tBOOL value ) { return setBool(pgrollbackAvailable,(tBOOL)value); }

		tBOOL pr_call get_pgretranslateOnlyListedComponents() { return (tBOOL)getBool(pgretranslateOnlyListedComponents); }
		tERROR pr_call set_pgretranslateOnlyListedComponents( tBOOL value ) { return setBool(pgretranslateOnlyListedComponents,(tBOOL)value); }

		tERROR pr_call get_pgcomponentsToRetranslate( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgcomponentsToRetranslate,buff,size,cp); }
		tERROR pr_call set_pgcomponentsToRetranslate( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgcomponentsToRetranslate,buff,size,cp); }

		tBOOL pr_call get_pgrollbackMode() { return (tBOOL)getBool(pgrollbackMode); }
		tERROR pr_call set_pgrollbackMode( tBOOL value ) { return setBool(pgrollbackMode,(tBOOL)value); }

		tBOOL pr_call get_pginteractionWithUserEnabled() { return (tBOOL)getBool(pginteractionWithUserEnabled); }
		tERROR pr_call set_pginteractionWithUserEnabled( tBOOL value ) { return setBool(pginteractionWithUserEnabled,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_updaterconfigurator__2ea3d3f6_ad1c_466c_afdd_eaf5a1eda196
// AVP Prague stamp end



