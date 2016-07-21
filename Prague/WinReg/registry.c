// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  28 June 2006,  19:23 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- registry.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Registry_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "registry.h"
#include <windows.h>
#include "reg_comm.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <windows.h>
#include <Prague/plugin/p_win32_reg.h>
// AVP Prague stamp end

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>
#undef IMPEX_IMPORT_LIB

#include <Prague/iface/i_string.h>
#include <Prague/iface/i_token.h>


#define HK(a)  ((HKEY)(a))
#define PHK(a) ((HKEY*)(a))

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Registry interface implementation
// Short comments -- access to storage like registry
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Registry. Inner data structure

typedef struct tag_RegData {
 	tBOOL   m_ro;      // is in RO mode?
	HKEY    m_root;                // current root
  HKEY    m_root_opened_on_init; // root which has been opened on InitDone
	tSTRING m_root_path;           // path to current root
	HKEY    m_hive;                // current hive
	tBOOL   m_hive_opened;         // hive has to be closed
 	tBOOL   m_clean;               // format on ObjectInit
  tDWORD  m_last_err;            // last windows error
	tRegKey m_last_keyenum_key;    // Last key used in GetKeyNameByIndex
	tDWORD  m_last_keyenum_index;  // Last index used in GetKeyNameByIndex
	tBOOL   m_inpersonate_user;    // has to impersonate user
	tBOOL   m_is_wow64_64key;      // true to access a 64-bit key from either a 32-bit or 64-bit application
	tBOOL   m_is_wow64_32key;      // true to access a 32-bit key from either a 32-bit or 64-bit application
} RegData;

// AVP Prague stamp end



const tCHAR g_hklm[] = "hklm";
const tCHAR g_HKLM[] = "HKEY_LOCAL_MACHINE";
const tCHAR g_hkcu[] = "hkcu";
const tCHAR g_HKCU[] = "HKEY_CURRENT_USER";
const tCHAR g_hkcr[] = "hkcr";
const tCHAR g_HKCR[] = "HKEY_CLASSES_ROOT";
const tCHAR g_hku[]  = "hku";
const tCHAR g_HKU[]  = "HKEY_USERS";
const tCHAR g_hkpd[] = "hkpd";
const tCHAR g_HKPD[] = "HKEY_PERFORMANCE_DATA";
const tCHAR g_hkcc[] = "hkcc";
const tCHAR g_HKCC[] = "HKEY_CURRENT_CONFIG";
const tCHAR g_hkdd[] = "hkdd";
const tCHAR g_HKDD[] = "HKEY_DYN_DATA";


typedef enum { os_unk, os_nt, os_9x } tOSType;
tOSType _get_os_type();

// ---
tERROR pr_call get_subkey_name( RegData* d, tRegKey key, tUINT ind, tCHAR* name, tUINT size, tDWORD* out_size );
tERROR pr_call get_value_name( RegData* d, tRegKey key, tUINT ind, tCHAR* name, tUINT size, tDWORD* out_size );
tERROR pr_call adopt_key( RegData* d, tRegKey* key );

tERROR pr_call _parse_reg_env(hOBJECT hObj, tTYPE_ID type, tPTR * pValue, tDWORD size);
void   pr_call _deparse_reg_env(tTYPE_ID type, tPTR pValue, tDWORD size);


tERROR pr_call Registry_GetSerVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, DWORD* reg_type, tPTR pValue, tDWORD* out_size );
tERROR pr_call Registry_SetSerVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, tPTR pValue );

tERROR pr_call Registry_GetBufVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, DWORD* reg_type, tPTR pValue, tDWORD* out_size );
tERROR pr_call Registry_SetBufVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, tPTR pValue );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Registry {
	const iRegistryVtbl* vtbl; // pointer to the "Registry" virtual table
	const iSYSTEMVTBL*   sys;  // pointer to the "SYSTEM" virtual table
	RegData*             data; // pointer to the "Registry" data structure
} *hi_Registry;

// AVP Prague stamp end



// ---
static HKEY _get_current_user( hi_Registry _this, tBOOL classes, tBOOL* opened );

tDWORD winnt_del_key( hi_Registry _this, HKEY key, const tCHAR* name );
tDWORD win9x_del_key( hi_Registry _this, HKEY key, const tCHAR* name );


// ---
#define _ConvertErr( reg_err )  ( (ERROR_SUCCESS == (e=reg_err)) ? (e=errOK) : (e=_ConvertRegErr(e)) )
tERROR pr_call _ConvertRegErr( long err ) {
	switch( err ) {
    case ERROR_SUCCESS           : return errOK;
		case ERROR_MORE_DATA         : return errBUFFER_TOO_SMALL;
		case ERROR_FILE_NOT_FOUND    :
		case ERROR_BAD_PATHNAME      : return errKEY_NOT_FOUND;
    case ERROR_NOT_ENOUGH_MEMORY : return errNOT_ENOUGH_MEMORY;
    case ERROR_INVALID_PARAMETER : return errPARAMETER_INVALID;
		case ERROR_NO_MORE_ITEMS     : return errKEY_NOT_FOUND;
		case ERROR_ACCESS_DENIED     : return errACCESS_DENIED;
		default                      : return errUNEXPECTED;
	}
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Registry_ObjectInit( hi_Registry _this );
tERROR pr_call Registry_ObjectInitDone( hi_Registry _this );
tERROR pr_call Registry_ObjectClose( hi_Registry _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Registry_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        Registry_ObjectInit,
	(tIntFnObjectInitDone)    Registry_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       Registry_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpRegistry_OpenKey)             ( hi_Registry _this, tRegKey* result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist ); // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning;
typedef   tERROR (pr_call *fnpRegistry_OpenKeyByIndex)     ( hi_Registry _this, tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev ); // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning;
typedef   tERROR (pr_call *fnpRegistry_OpenNextKey)        ( hi_Registry _this, tRegKey* result, tRegKey key, tBOOL bClosePrev ); // -- opens next key on the same level;
typedef   tERROR (pr_call *fnpRegistry_CloseKey)           ( hi_Registry _this, tRegKey key ); // -- closes the key;
typedef   tERROR (pr_call *fnpRegistry_GetKeyName)         ( hi_Registry _this, tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ); // -- returns name of the key;
typedef   tERROR (pr_call *fnpRegistry_GetKeyNameByIndex)   ( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ); // -- returns name of the subkey by index;
typedef   tERROR (pr_call *fnpRegistry_GetValue)            ( hi_Registry _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size ); // -- gets key value, returns byte length of the value put in the buffer;
typedef   tERROR (pr_call *fnpRegistry_GetValueByIndex)    ( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ); // -- gets key value, returns byte length of the value put in the buffer;
typedef   tERROR (pr_call *fnpRegistry_GetValueNameByIndex)( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size ); // -- gets key name, returns byte length of the name put in the buffer;
typedef   tERROR (pr_call *fnpRegistry_SetValue)            ( hi_Registry _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ); // -- set key value. Creates key if it is not exist and parameter bCreateIfNotExist is true. Returns warning errKEY_NOT_EXISTS in this case;
typedef   tERROR (pr_call *fnpRegistry_SetValueByIndex)    ( hi_Registry _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ); // -- set key value, creates key if it is not exist and return warning errKEY_NOT_EXISTS in this case;
typedef   tERROR (pr_call *fnpRegistry_GetKeyCount)        ( hi_Registry _this, tDWORD* result, tRegKey key ); // -- gets subkey count;
typedef   tERROR (pr_call *fnpRegistry_GetValueCount)      ( hi_Registry _this, tDWORD* result, tRegKey key ); // -- gets subkey count;
typedef   tERROR (pr_call *fnpRegistry_DeleteKey)           ( hi_Registry _this, tRegKey key, const tCHAR* szSubKeyName ); // -- deletes the key;
typedef   tERROR (pr_call *fnpRegistry_DeleteKeyByInd)     ( hi_Registry _this, tRegKey key, tDWORD index ); // -- deletes the key;
typedef   tERROR (pr_call *fnpRegistry_DeleteValue)         ( hi_Registry _this, tRegKey key, const tCHAR* szValueName ); // -- deletes the key value;
typedef   tERROR (pr_call *fnpRegistry_DeleteValueByInd)   ( hi_Registry _this, tRegKey key, tDWORD index ); // -- deletes the key value;
typedef   tERROR (pr_call *fnpRegistry_Clean)               ( hi_Registry _this );            // -- clean the object contained registry data from the root key;
typedef   tERROR (pr_call *fnpRegistry_CopyKey)             ( hi_Registry _this, tRegKey src_key, hREGISTRY destination, tRegKey dest_key ); // -- copy key to another hREGISTRY;
typedef   tERROR (pr_call *fnpRegistry_CopyKeyByName)       ( hi_Registry _this, const tCHAR* src, hREGISTRY dst ); // -- ;
typedef   tERROR (pr_call *fnpRegistry_SetRootKey)          ( hi_Registry _this, tRegKey root, tBOOL close_prev ); // -- ;
typedef   tERROR (pr_call *fnpRegistry_SetRootStr)          ( hi_Registry _this, const tCHAR* root, tBOOL close_prev ); // -- ;
typedef   tERROR (pr_call *fnpRegistry_Flush)               ( hi_Registry _this, tBOOL p_force ); // -- ;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iRegistryVtbl {
	fnpRegistry_OpenKey              OpenKey;
	fnpRegistry_OpenKeyByIndex       OpenKeyByIndex;
	fnpRegistry_OpenNextKey          OpenNextKey;
	fnpRegistry_CloseKey             CloseKey;
	fnpRegistry_GetKeyName           GetKeyName;
	fnpRegistry_GetKeyNameByIndex    GetKeyNameByIndex;
	fnpRegistry_GetValue             GetValue;
	fnpRegistry_GetValueByIndex      GetValueByIndex;
	fnpRegistry_GetValueNameByIndex  GetValueNameByIndex;
	fnpRegistry_SetValue             SetValue;
	fnpRegistry_SetValueByIndex      SetValueByIndex;
	fnpRegistry_GetKeyCount          GetKeyCount;
	fnpRegistry_GetValueCount        GetValueCount;
	fnpRegistry_DeleteKey            DeleteKey;
	fnpRegistry_DeleteKeyByInd       DeleteKeyByInd;
	fnpRegistry_DeleteValue          DeleteValue;
	fnpRegistry_DeleteValueByInd     DeleteValueByInd;
	fnpRegistry_Clean                Clean;
  fnpRegistry_CopyKey              CopyKey;
  fnpRegistry_CopyKeyByName        CopyKeyByName;
	fnpRegistry_SetRootKey           SetRootKey;
	fnpRegistry_SetRootStr           SetRootStr;
	fnpRegistry_Flush                Flush;
}; // "Registry" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Registry_OpenKey( hi_Registry _this, tRegKey* result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist );
tERROR pr_call Registry_OpenKeyByIndex( hi_Registry _this, tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev );
tERROR pr_call Registry_OpenNextKey( hi_Registry _this, tRegKey* result, tRegKey key, tBOOL bClosePrev );
tERROR pr_call Registry_CloseKey( hi_Registry _this, tRegKey key );
tERROR pr_call Registry_GetKeyName( hi_Registry _this, tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName );
tERROR pr_call Registry_GetKeyNameByIndex( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName );
tERROR pr_call Registry_GetValue( hi_Registry _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size );
tERROR pr_call Registry_GetValueByIndex( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size );
tERROR pr_call Registry_GetValueNameByIndex( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size );
tERROR pr_call Registry_SetValue( hi_Registry _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist );
tERROR pr_call Registry_SetValueByIndex( hi_Registry _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist );
tERROR pr_call Registry_GetKeyCount( hi_Registry _this, tDWORD* result, tRegKey key );
tERROR pr_call Registry_GetValueCount( hi_Registry _this, tDWORD* result, tRegKey key );
tERROR pr_call Registry_DeleteKey( hi_Registry _this, tRegKey key, const tCHAR* szSubKeyName );
tERROR pr_call Registry_DeleteKeyByInd( hi_Registry _this, tRegKey key, tDWORD index );
tERROR pr_call Registry_DeleteValue( hi_Registry _this, tRegKey key, const tCHAR* szValueName );
tERROR pr_call Registry_DeleteValueByInd( hi_Registry _this, tRegKey key, tDWORD index );
tERROR pr_call Registry_Clean( hi_Registry _this );
tERROR pr_call Registry_CopyKey( hi_Registry _this, tRegKey key, hREGISTRY destination, tRegKey dest_key );
tERROR pr_call Registry_CopyKeyByName( hi_Registry _this, const tCHAR* src, hREGISTRY dst ); // -- ;
tERROR pr_call Registry_SetRootKey( hi_Registry _this, tRegKey root, tBOOL close_prev ); // -- ;
tERROR pr_call Registry_SetRootStr( hi_Registry _this, const tCHAR* root, tBOOL close_prev ); // -- ;
tERROR pr_call Registry_Flush( hi_Registry _this, tBOOL p_force ); // -- ;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iRegistryVtbl e_Registry_vtbl = {
	(fnpRegistry_OpenKey)     Registry_OpenKey,
	(fnpRegistry_OpenKeyByIndex) Registry_OpenKeyByIndex,
	(fnpRegistry_OpenNextKey) Registry_OpenNextKey,
	(fnpRegistry_CloseKey)    Registry_CloseKey,
	(fnpRegistry_GetKeyName)  Registry_GetKeyName,
	(fnpRegistry_GetKeyNameByIndex) Registry_GetKeyNameByIndex,
	(fnpRegistry_GetValue)    Registry_GetValue,
	(fnpRegistry_GetValueByIndex) Registry_GetValueByIndex,
	(fnpRegistry_GetValueNameByIndex) Registry_GetValueNameByIndex,
	(fnpRegistry_SetValue)    Registry_SetValue,
	(fnpRegistry_SetValueByIndex) Registry_SetValueByIndex,
	(fnpRegistry_GetKeyCount) Registry_GetKeyCount,
	(fnpRegistry_GetValueCount) Registry_GetValueCount,
	(fnpRegistry_DeleteKey)   Registry_DeleteKey,
	(fnpRegistry_DeleteKeyByInd) Registry_DeleteKeyByInd,
	(fnpRegistry_DeleteValue) Registry_DeleteValue,
	(fnpRegistry_DeleteValueByInd) Registry_DeleteValueByInd,
	(fnpRegistry_Clean)       Registry_Clean,
	(fnpRegistry_CopyKey)     Registry_CopyKey,
	(fnpRegistry_CopyKeyByName) Registry_CopyKeyByName,
	(fnpRegistry_SetRootKey)  Registry_SetRootKey,
	(fnpRegistry_SetRootStr)  Registry_SetRootStr,
	(fnpRegistry_Flush)       Registry_Flush
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Registry_PROP_get_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_set_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_get_max_value_size( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
//tERROR pr_call Registry_PROP_set_root_key( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Registry". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Registry_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Registry_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Registry_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "windows registry interface", 27 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_RO, RegData, m_ro, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mSHARED_PROPERTY( pgMAX_NAME_LEN, ((tDWORD)(cRegMaxName)) )
	mLOCAL_PROPERTY_BUF( pgCLEAN, RegData, m_clean, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mSHARED_PROPERTY( pgSAVE_RESULTS_ON_CLOSE, ((tBOOL)(cTRUE)) )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgROOT_POINT, Registry_PROP_get_root, Registry_PROP_set_root )
	mLOCAL_PROPERTY_BUF( pgROOT_KEY, RegData, m_root, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgMAX_VALUE_SIZE, Registry_PROP_get_max_value_size, NULL )
	mLOCAL_PROPERTY_BUF( plNATIVE_ERROR, RegData, m_last_err, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plIMPERSONATE, RegData, m_inpersonate_user, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plWOW64_64KEY, RegData, m_is_wow64_64key, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plWOW64_32KEY, RegData, m_is_wow64_32key, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mPROPERTY_TABLE_END(Registry_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Registry". Register function
tERROR pr_call Registry_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter Registry::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_REGISTRY,                           // interface identifier
		PID_WIN32_REG,                          // plugin identifier
		0x00000000,                             // subtype identifier
		Registry_VERSION,                       // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Registry_vtbl,                       // internal(kernel called) function table
		(sizeof(i_Registry_vtbl)/sizeof(tPTR)), // internal function table size
		&e_Registry_vtbl,                       // external function table
		(sizeof(e_Registry_vtbl)/sizeof(tPTR)), // external function table size
		Registry_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(Registry_PropTable),// property table size
		sizeof(RegData),                        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Registry(IID_REGISTRY) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave Registry::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_root )
// Interface "Registry". Method "Get" for property(s):
//  -- ROOT_POINT
tERROR pr_call Registry_PROP_get_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR  e;
  tSTRING s;
  RegData* d;
  PR_TRACE_A0( _this, "Enter *GET* method Registry_PROP_get_root for property pgROOT_POINT" );

  e = errOK;
  d = _this->data;
  s = d->m_root_path;
  if ( !buffer )
    *out_size = 6 + (s ? ((tDWORD)strlen(s)) : 0);
  else if ( !size ) {
    *out_size = 0;
    e = errPARAMETER_INVALID;
  }
  else if ( !s || !*s ) {
    *out_size = 1;
    *buffer = 0;
  }
  else {
    tUINT pl, l;
    const tCHAR* prefix;
    switch( (ULONG_PTR)d->m_hive ) {
      case HKEY_LOCAL_MACHINE    : prefix = g_hklm; pl = 4; break;
      case HKEY_CURRENT_USER     : prefix = g_hkcu; pl = 4; break;
      case HKEY_CLASSES_ROOT     : prefix = g_hkcr; pl = 4; break;
      case HKEY_USERS            : prefix = g_hku;  pl = 3; break;
      case HKEY_PERFORMANCE_DATA : prefix = g_hkpd; pl = 4; break;
      case HKEY_CURRENT_CONFIG   : prefix = g_hkcc; pl = 4; break;
      case HKEY_DYN_DATA         : prefix = g_hkdd; pl = 4; break;
			default                    : prefix = 0; pl = 0; break;
    }
    l = (s ? ((tUINT)strlen(s)) : 0) + 1;
    if ( size >= (l+pl) ) {
			if ( prefix )
				memcpy( buffer, prefix, pl );
			if ( s ) {
				if ( prefix )
					*(buffer+pl++) = '\\';
				memcpy( buffer+pl, s, l );
			}
				*out_size = pl + l;
    }
    else {
      *out_size = 0;
      e = errBUFFER_TOO_SMALL;
    }
  }
  PR_TRACE_A2( _this, "Leave *GET* method Registry_PROP_get_root for property pgROOT_POINT, ret tDWORD = %u(size), error = 0x%08x", *out_size, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_root_key )
// Interface "Registry". Method "Set" for property(s):
//  -- ROOT_KEY
//tERROR pr_call Registry_PROP_set_root_key( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
//	tERROR error = errOK;
//	PR_TRACE_A0( _this, "Enter *SET* method ObjectClose for property pgROOT_KEY" );
//
//	*out_size = 0;
//	if ( !CALL_SYS_PropertyGetBool(_this,psOBJECT_OPERATIONAL) )
//	  error = errOBJECT_NOT_INITIALIZED;
//	else {
//		tDWORD size;
//		RegData* d = _this->data;
//		d->m_hive = 0;
//		d->m_last_err = 0;
//		if ( d->root )
//			RegCloseKey( d->root );
//		d->root = *(HKEY*)buffer;
//		error = Registry_GetKeyName( _this, &size, cRegRoot, 0, 0, cFALSE );
//		if ( PR_FAIL(error) )
//			;
//		else if ( size ) {
//			error = CALL_SYS_ObjHeapRealloc( _this, &d->m_root_path, d->m_root_path, size );
//			if ( PR_SUCC(error) )
//				error = Registry_GetKeyName( _this, &size, cRegRoot, d->m_root_path, size, cFALSE );
//		}
//		else if ( d->m_root_path ) {
//			CALL_SYS_ObjHeapFree( _this, d->m_root_path );
//			d->m_root_path = 0;
//		}
//	}
//
//	PR_TRACE_A2( _this, "Leave *SET* method ObjectClose for property pgROOT_KEY, ret tDWORD = %u(size), %terr", *out_size, error );
//	return error;
//}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call Registry_ObjectInit( hi_Registry _this ) {
	PR_TRACE_A0( _this, "Enter Registry::ObjectInit method" );
  _this->data->m_ro = cTRUE;

	PR_TRACE_A0( _this, "Leave Registry::ObjectInit method, ret tERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Registry_ObjectInitDone( hi_Registry _this ) {
	tERROR e = errOK;
	RegData* d;
	tDWORD disposition = 0;
	PR_TRACE_A0( _this, "Enter Registry::ObjectInitDone method" );
	
	d = _this->data;
	if ( d->m_root_path ) {
		DWORD WOW64FLAG = bWOWFlagsAvailable ? ((d->m_is_wow64_64key ? KEY_WOW64_64KEY : 0) | (d->m_is_wow64_32key ? KEY_WOW64_32KEY : 0)) : 0;
		if ( d->m_ro )
			d->m_last_err = RegOpenKeyEx( d->m_hive, d->m_root_path, 0, KEY_READ | WOW64FLAG, &d->m_root );
		else
			d->m_last_err = RegCreateKeyEx( d->m_hive, d->m_root_path, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | WOW64FLAG, 0, &d->m_root, &disposition );
		if ( d->m_last_err == ERROR_SUCCESS )
			d->m_root_opened_on_init = d->m_root;
		else
			e = _ConvertRegErr( d->m_last_err );
	}
	if ( PR_SUCC(e) && d->m_clean )
		e = Registry_Clean( _this );

	PR_TRACE_A1( _this, "Leave Registry::ObjectInitDone method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call Registry_ObjectClose( hi_Registry _this ) {
	RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::ObjectClose method" );

	d = _this->data;
	if( d->m_root_opened_on_init )
		RegCloseKey( d->m_root_opened_on_init );
	if ( d->m_root_path )
		CALL_SYS_ObjHeapFree( _this, d->m_root_path );
	if ( d->m_hive && d->m_hive_opened )
		RegCloseKey( d->m_hive );
	d->m_hive = 0;
	d->m_hive_opened = cFALSE;

	PR_TRACE_A0( _this, "Leave Registry::ObjectClose method, ret tERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_root )
// Interface "Registry". Method "Set" for property(s):
//  -- ROOT_POINT
tERROR pr_call Registry_PROP_set_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR   e;
	RegData* d;
	HKEY     hive = 0;
	tBOOL    hive_opened = cFALSE;
	
	PR_TRACE_A0( _this, "Enter *SET* method Registry_PROP_set_root for property pgROOT_POINT" );
	
	if ( !buffer ) {
		*out_size = cRegMaxName;
		e = errOK;
	}
	else if ( !size ) {
		*out_size = 0;
		e = errPARAMETER_INVALID;
	}
	else if ( !*buffer ) { // special case: set root to nothing
		d = _this->data;
		d->m_last_err = ERROR_SUCCESS;
		if ( d->m_root_path )
			CALL_SYS_ObjHeapFree( _this, d->m_root_path );
		d->m_root_path = 0;
		e = errOK;
	}
	else {
		tUINT    len, flen, old_root_len;
		tSTRING  nroot = 0;
		tBOOL    absolute;
		tUINT    name_shift = 0;
		
		e = errOK;
		d = _this->data;
		d->m_last_err = ERROR_SUCCESS;
		if ( *buffer == '\\' ) {
			absolute = cTRUE;
			buffer++;
		}
		else
			absolute = cFALSE;
		
		len = 1;

		#define sizeofs(s) ((sizeof(s)-sizeof(s[0]))/sizeof(s[0]))
		if ( !_strnicmp(buffer,g_hklm,sizeofs(g_hklm)) )
			name_shift = sizeofs(g_hklm), absolute = cTRUE, hive = HKEY_LOCAL_MACHINE;
		else if ( !_strnicmp(buffer,g_HKLM,sizeofs(g_HKLM)) )
			name_shift = sizeofs(g_HKLM), absolute = cTRUE, hive = HKEY_LOCAL_MACHINE;
		else if ( !_strnicmp(buffer,g_hkcu,sizeofs(g_hkcu)) )
			name_shift = sizeofs(g_hkcu), absolute = cTRUE, hive = _get_current_user( _this, cFALSE, &hive_opened );
		else if ( !_strnicmp(buffer,g_HKCU,sizeofs(g_HKCU)) )
			name_shift = sizeofs(g_HKCU), absolute = cTRUE, hive = _get_current_user( _this, cFALSE, &hive_opened );
		else if ( !_strnicmp(buffer,g_hkcr,sizeofs(g_hkcr)) )
			name_shift = sizeofs(g_hkcr), absolute = cTRUE, hive = _get_current_user( _this, cTRUE, &hive_opened );
		else if ( !_strnicmp(buffer,g_HKCR,sizeofs(g_HKCR)) )
			name_shift = sizeofs(g_HKCR), absolute = cTRUE, hive = _get_current_user( _this, cTRUE, &hive_opened );
		else if ( !_strnicmp(buffer,g_hku,sizeofs(g_hku)) )
			name_shift = sizeofs(g_hku), absolute = cTRUE, hive = HKEY_USERS;
		else if ( !_strnicmp(buffer,g_HKU,sizeofs(g_HKU)) )
			name_shift = sizeofs(g_HKU), absolute = cTRUE, hive = HKEY_USERS;
		else if ( !_strnicmp(buffer,g_hkpd,sizeofs(g_hkpd)) )
			name_shift = sizeofs(g_hkpd), absolute = cTRUE, hive = HKEY_PERFORMANCE_DATA;
		else if ( !_strnicmp(buffer,g_HKPD,sizeofs(g_HKPD)) )
			name_shift = sizeofs(g_HKPD), absolute = cTRUE, hive = HKEY_PERFORMANCE_DATA;
		else if ( !_strnicmp(buffer,g_hkcc,sizeofs(g_hkcc)) )
			name_shift = sizeofs(g_hkcc), absolute = cTRUE, hive = HKEY_CURRENT_CONFIG;
		else if ( !_strnicmp(buffer,g_HKCC,sizeofs(g_HKCC)) )
			name_shift = sizeofs(g_HKCC), absolute = cTRUE, hive = HKEY_CURRENT_CONFIG;
		else if ( !_strnicmp(buffer,g_hkdd,sizeofs(g_hkdd)) )
			name_shift = sizeofs(g_hkdd), absolute = cTRUE, hive = HKEY_DYN_DATA;
		else if ( !_strnicmp(buffer,g_HKDD,sizeofs(g_HKDD)) )
			name_shift = sizeofs(g_HKDD), absolute = cTRUE, hive = HKEY_DYN_DATA;
		else if ( absolute )
			len = 0, hive = HKEY_LOCAL_MACHINE;
		else
			len = 0, hive = d->m_hive, hive_opened = d->m_hive_opened;
		
		PR_TRACE((_this, prtNOTIFY, "Registry_PROP_set_root_point RegPath: %s RegHandle: %08x", buffer, hive)) ;

		if ( name_shift ) {
			if ( !hive )
				e = errREG_IMPERSONATION;
			else
				buffer += name_shift;
		}

		if ( PR_SUCC(e) ) {
			if ( len && (*buffer == '\\') )
				buffer++;
			
			for( len=0; (len<size) && buffer[len]; len++ )
				;
			
			flen = len+(2+cRegMaxName);
			if ( absolute )
				old_root_len = 0;
			else
				flen += 1 + (old_root_len = (d->m_root_path ? ((tUINT)strlen(d->m_root_path)) : 0));
			
			e = CALL_SYS_ObjHeapAlloc( _this, &nroot, flen );
		}

		if ( PR_FAIL(e) )
			*out_size = 0;
		else {
			if ( len && buffer[len-1] == '\\' )
				len--;
			if ( !len )
				e = errPARAMETER_INVALID;
			else {
				tCHAR* root_str;
				
				if ( old_root_len ) {
					memcpy( nroot, d->m_root_path, old_root_len );
					nroot[old_root_len++] = '\\';
				}
				
				root_str = nroot + old_root_len;
				memcpy( root_str, buffer, len );
				root_str[len] = 0;
				
				*out_size = len;
				if ( d->m_root_path )
					CALL_SYS_ObjHeapFree( _this, d->m_root_path );
				d->m_root_path = nroot;
				d->m_hive = hive;
				d->m_hive_opened = hive_opened;
				hive = 0;
				hive_opened = cFALSE;
			}
		}
	}
	
	if ( hive_opened && hive )
		RegCloseKey( hive );

	PR_TRACE_A2( _this, "Leave *SET* method Registry_PROP_set_root for property pgROOT_POINT, ret tDWORD = %u(size), error = 0x%08x", *out_size, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_max_value_size )
// Interface "Registry". Method "Get" for property(s):
//  -- MAX_VALUE_SIZE
tERROR pr_call Registry_PROP_get_max_value_size( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method get_max_value_size for property pgMAX_VALUE_SIZE" );

	*out_size = sizeof(tDWORD);
	if ( buffer ) {
		if ( size < sizeof(tDWORD) ) {
			error = errBUFFER_TOO_SMALL;
			*out_size = 0;
		}
		else if ( !(0x80000000 & GetVersion()) )
			*(tDWORD*)buffer = -1;
		else
			*(tDWORD*)buffer = 16300;
	}

	PR_TRACE_A2( _this, "Leave *GET* method get_max_value_size for property pgMAX_VALUE_SIZE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenKey )
// Extended method comment
//  if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR pr_call Registry_OpenKey( hi_Registry _this, tRegKey* result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist ) {
	tERROR   e;
	tRegKey  okey;
	DWORD    disposition;
  RegData* d;
  DWORD WOW64FLAG;
	PR_TRACE_A0( _this, "Enter Registry::OpenKey method" );

	d = _this->data;
	d->m_last_err = 0;
	disposition = 0;

	if ( !szSubKeyName )
		szSubKeyName = "";

	WOW64FLAG = bWOWFlagsAvailable ? ((d->m_is_wow64_64key ? KEY_WOW64_64KEY : 0) | (d->m_is_wow64_32key ? KEY_WOW64_32KEY : 0)) : 0;
	if ( PR_FAIL(e=adopt_key(d,&key)) )
		;
	else if ( d->m_ro )
		e = _ConvertRegErr( d->m_last_err=RegOpenKeyEx(HK(key),szSubKeyName,0, KEY_READ | WOW64FLAG,PHK(&okey)) );
	else if ( bCreateIfNotExist )
		e = _ConvertRegErr( d->m_last_err=RegCreateKeyEx(HK(key),szSubKeyName,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS | WOW64FLAG,0,PHK(&okey),&disposition) );
	else {
		e = _ConvertRegErr( d->m_last_err=RegOpenKeyEx(HK(key),szSubKeyName,0, KEY_ALL_ACCESS | WOW64FLAG,PHK(&okey)) );
		if ( e == errACCESS_DENIED )
			e = _ConvertRegErr( d->m_last_err=RegOpenKeyEx(HK(key),szSubKeyName,0, KEY_READ | WOW64FLAG,PHK(&okey)) );
	}

	if ( PR_FAIL(e) )
		okey = 0;
	else if ( disposition == REG_CREATED_NEW_KEY )
		e = warnKEY_NOT_EXISTS;

	if ( result )
		*result = okey;
	PR_TRACE_A2( _this, "Leave Registry::OpenKey method, ret tRegKey = %p, error = 0x%08x", okey, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenKeyByIndex )
// Extended method comment
//  if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR pr_call Registry_OpenKeyByIndex( hi_Registry _this, tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev ) {
	tERROR  e;
	tDWORD  sub_keys =0;
	tRegKey	ret_val = 0;
	tCHAR   name[MAX_PATH+1];
  RegData* d;
  DWORD WOW64FLAG;
	PR_TRACE_A0( _this, "Enter Registry::OpenKeyByIndex method" );

  d = _this->data;
  d->m_last_err = 0;
  WOW64FLAG = bWOWFlagsAvailable ? ((d->m_is_wow64_64key ? KEY_WOW64_64KEY : 0) | (d->m_is_wow64_32key ? KEY_WOW64_32KEY : 0)) : 0;
  if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
	else if ( PR_FAIL(e=get_subkey_name(d,key,index,name,sizeof(name),0)) )
		;
	else if ( PR_FAIL(e=_ConvertRegErr(d->m_last_err=RegOpenKeyEx(HK(key),name,0,KEY_ALL_ACCESS | WOW64FLAG,PHK(&ret_val)))) )
		;
	else if ( bClosePrev )
		e = _ConvertErr( d->m_last_err=RegCloseKey(HK(key)) );

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Registry::OpenKeyByIndex method, ret tRegKey = %p, error = 0x%08x", ret_val, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenNextKey )
// Parameters are:
tERROR pr_call Registry_OpenNextKey( hi_Registry _this, tRegKey* result, tRegKey key, tBOOL bClosePrev ) {
	PR_TRACE_A0( _this, "Enter Registry::OpenNextKey method" );
	// cant get next key in a windows registry
  _this->data->m_last_err = 0;
	if ( result )
		*result = 0;
	PR_TRACE_A0( _this, "Leave Registry::OpenNextKey method, ret tRegKey = NULL, error = errNOT_SUPPORTED" );
	return errNOT_SUPPORTED;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CloseKey )
// Parameters are:
tERROR pr_call Registry_CloseKey( hi_Registry _this, tRegKey key ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::CloseKey method" );

  d = _this->data;
  d->m_last_err = 0;
	if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else
    e = _ConvertErr( d->m_last_err=RegCloseKey(HK(key)) );

	PR_TRACE_A1( _this, "Leave Registry::CloseKey method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteKey )
// Parameters are:
tERROR pr_call Registry_DeleteKey( hi_Registry _this, tRegKey key, const tCHAR* szSubKeyName ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::DeleteKey method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( PR_FAIL(e=adopt_key(_this->data,&key)) )
    ;
  else {
	  tDWORD (*del_proc)(hi_Registry,HKEY,const tCHAR*) = !(0x80000000 & GetVersion()) ? winnt_del_key : win9x_del_key;
	  e = _ConvertErr( d->m_last_err=del_proc(_this, HK(key), szSubKeyName) );
  }

	PR_TRACE_A1( _this, "Leave Registry::DeleteKey method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteKeyByInd )
// Parameters are:
tERROR pr_call Registry_DeleteKeyByInd( hi_Registry _this, tRegKey key, tDWORD index ) {
	tERROR   e;
	tCHAR    name[MAX_PATH+1];
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::DeleteKeyByInd method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else if ( PR_FAIL(e=get_subkey_name(d,key,index,name,sizeof(name),0)) )
		;
	else
		e = _ConvertErr( d->m_last_err=RegDeleteKey(HK(key),name) );

	PR_TRACE_A1( _this, "Leave Registry::DeleteKeyByInd method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteValue )
// Parameters are:
tERROR pr_call Registry_DeleteValue( hi_Registry _this, tRegKey key, const tCHAR* szValueName ) {
  tERROR   e;
  RegData* d;
  PR_TRACE_A0( _this, "Enter \"Registry::DeleteValue\" method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else
    e = _ConvertErr( d->m_last_err=RegDeleteValue(HK(key),szValueName) );

  PR_TRACE_A1( _this, "Leave \"Registry::DeleteValue\" method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteValueByInd )
// Parameters are:
tERROR pr_call Registry_DeleteValueByInd( hi_Registry _this, tRegKey key, tDWORD index ) {
  tERROR   e;
  tCHAR    n[MAX_PATH+1];
  RegData* d;
  PR_TRACE_A0( _this, "Enter \"Registry::DeleteValueByInd\" method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else if ( PR_FAIL(e=get_value_name(d,key,index,n,sizeof(n),0)) )
    ;
  else
    e = _ConvertErr( d->m_last_err=RegDeleteValue(HK(key),n) );

  PR_TRACE_A1( _this, "Leave \"Registry::DeleteValueByInd\" method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyName )
// Parameters are:
// "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR pr_call Registry_GetKeyName( hi_Registry _this, tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) {
  tERROR                 e;
  KEY_BASIC_INFORMATION* key_basic = 0;
  ULONG                  result_len = 0;
  DWORD                  err;
  tDWORD                 s = 0;
  RegData*               d;
  PR_TRACE_A0( _this, "Enter Registry::GetKeyName method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( !pZwQueryKey )
    e = errNOT_SUPPORTED;     // windows could not support us to get key name :-(
  else if ( bFullName )
    e = errPARAMETER_INVALID; // windows could not support us to get key name :-(
  else if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else if ( PR_FAIL(e=CALL_SYS_ObjHeapAlloc(_this,&key_basic,0x400)) )
    ;
  else if ( 0 != (err=pZwQueryKey((HANDLE)key,KeyBasicInformation,key_basic,0x400,&result_len)) )
    e = errUNEXPECTED;
  else if ( !name_buff )
    s = key_basic->NameLength;
  else if ( !size )
    e = errPARAMETER_INVALID;
  else if ( size < key_basic->NameLength )
    e = errBUFFER_TOO_SMALL;
  else if ( !key_basic->NameLength )
    *name_buff = 0;
  else if ( 0 == (s=((tDWORD)wcstombs(name_buff,key_basic->Name,size))) ) {
    d->m_last_err = GetLastError();
    e = errUNEXPECTED;
  }

  if ( key_basic )
    CALL_SYS_ObjHeapFree( _this, key_basic );

  if ( result )
		*result = s;
	PR_TRACE_A2( _this, "Leave Registry::GetKeyName method, ret tDWORD = %u, error = 0x%08x", s, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyNameByIndex )
// Parameters are:
//   "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR pr_call Registry_GetKeyNameByIndex( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) {
	tERROR e;
	tDWORD ret_val;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::GetKeyNameByIndex method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( bFullName ) {
    ret_val = 0;
    e = errNOT_SUPPORTED; // windows could not support us to get key name :-(
  }
  else if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else
    e = get_subkey_name( d, key, index, name_buff, size, &ret_val );

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Registry::GetKeyNameByIndex method, ret tDWORD = %u, error = 0x%08x", ret_val, e );
	return e;
}
// AVP Prague stamp end



// ---
tERROR pr_call Registry_GetWStrValue( hi_Registry _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tDWORD* reg_type, tPTR pValue, tDWORD insize ) {
	tERROR e;
	RegData* d = _this->data;

	if ( os_nt == _get_os_type() ) { // it's nt
		tWCHAR  l_wname[50];
		tWCHAR* wname;
		tUINT len = lstrlen(szValueName) + 1;
		if ( len <= countof(l_wname) ) {
			e = errOK;
			wname = l_wname;
		}
		else
			e = CALL_SYS_ObjHeapAlloc( _this, &wname, len*sizeof(tWCHAR) );
		if ( PR_SUCC(e) ) {
			mbstowcs( wname, szValueName, len );
			
			*result = insize;
			d->m_last_err = RegQueryValueExW( HK(key), wname, 0, reg_type, (LPBYTE)pValue, result );
			e = _ConvertRegErr( d->m_last_err );
			
			if ( PR_SUCC(e) ) {
				if ( (*reg_type != REG_SZ) && (*reg_type != REG_EXPAND_SZ) )
					e = errOBJECT_INCOMPATIBLE;
				else if ( !*result ) {
					*result = sizeof(tWCHAR);
					if ( pValue )
						*(tWCHAR*)pValue = 0;
				}
			}

			if ( wname != l_wname )
				CALL_SYS_ObjHeapFree( _this, wname );
		}
		else
			*result = 0;
	}
	else { // it's 9x
		tBYTE* buff; 
		tDWORD nMbSize;
		insize /= sizeof(tWCHAR);
		nMbSize = insize * 5;
		if ( pValue )
			e = CALL_SYS_ObjHeapAlloc( _this, &buff, nMbSize );
		else {
			e = errOK;
			buff = 0;
		}

		if ( PR_SUCC(e) ) {
			d->m_last_err = RegQueryValueEx( HK(key), szValueName, 0, reg_type, buff, &nMbSize );
			e = _ConvertRegErr( d->m_last_err );
		}

		if ( PR_SUCC(e) ) {
			if ( (*reg_type != REG_SZ) && (*reg_type != REG_EXPAND_SZ) ) {
				e = errOBJECT_INCOMPATIBLE;
				nMbSize = 0;
			}
			else if ( nMbSize ) {
				if ( pValue )
					*result = MultiByteToWideChar( CP_ACP, 0, buff, nMbSize, pValue, insize );
				else
					*result = nMbSize;
				*result *= sizeof(tWCHAR);
			}
			else {
				if( pValue && (*result >= 1) )
					*(tWCHAR*)pValue = 0;
				*result = sizeof(tWCHAR);
			}
		}

		if ( buff )
			CALL_SYS_ObjHeapFree( _this, buff );
	}
	return e;
}



// ---
tERROR pr_call Registry_GetStrObjValue( hi_Registry _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tDWORD* reg_type, tPTR pValue, tDWORD insize ) {
	hSTRING  s;
	RegData* d = _this->data;
	tERROR   e = Registry_GetWStrValue( _this, result, key, szValueName, reg_type, 0, 0 );

	if ( PR_FAIL(e) )
		return e;

	if ( !*(tVOID**)pValue )
		return errOK;

	if ( PR_FAIL(CALL_SYS_ObjectValid(_this,*(hOBJECT*)pValue)) ) {
		sString* str = ((sString*)pValue);
		if ( *result ) {
			if ( *result > str->m_size ) {
				e = CALL_SYS_ObjHeapRealloc( g_root, (tPTR*)&str->m_data, str->m_data, *result );
				if ( PR_FAIL(e) )
					return e;
				str->m_size = *result;
			}
			e = Registry_GetWStrValue( _this, result, key, szValueName, reg_type, str->m_data, *result );
		}
		else if ( str->m_data )
			*str->m_data = 0;
		if ( PR_FAIL(e) )
			*result = 0;
		str->m_len = *result;
		return e;
	}

	e = CALL_SYS_ObjectCheck( _this, *(hOBJECT*)pValue, IID_STRING, PID_ANY, SUBTYPE_ANY, cTRUE );
	if ( PR_FAIL(e) )
		return e;

	s = *(hSTRING*)pValue;
	if ( *result ) {
		tWCHAR buff[0x100];
		tPTR   pbuff = buff;
		if ( *result > sizeof(buff) ) {
			e = CALL_SYS_ObjHeapAlloc( s, &pbuff, *result );
			if ( PR_FAIL(e) )
				return e;
		}
		e = Registry_GetWStrValue( _this, result, key, szValueName, reg_type, pbuff, *result );
		if ( PR_SUCC(e) )
			e = CALL_String_ImportFromBuff( s, result, pbuff, *result, cCP_UNICODE, cSTRING_Z );
		if ( pbuff != buff )
			CALL_SYS_ObjHeapFree( s, pbuff );
	}
	else
		e = CALL_String_ImportFromBuff( s, result, 0, 0, cCP_UNICODE, cSTRING_Z );

	return e;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValue )
// Parameters are:
tERROR pr_call Registry_GetValue( hi_Registry _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size ) {
	tERROR   e;
	RegData* d;
	tBOOL    uni = cFALSE;
	tDWORD   reg_type;
	tTYPE_ID ptype = tid_VOID;
	PR_TRACE_A0( _this, "Enter \"Registry::GetValue\" method" );

	d = _this->data;
	d->m_last_err = 0;

	if ( !type )
		type = &ptype;

	if ( (!szValueName || !*szValueName) && !((*type==tid_STRING) || (*type==tid_WSTRING)) )
		e = errPARAMETER_INVALID;
	else
		e = errOK/*, *type = tid_VOID*/;

	if ( PR_FAIL(e) )
		size = 0;
	else if ( PR_FAIL(e=adopt_key(d,&key)) )
		size = 0;
	else if ( *type == tid_WSTRING ) {
		uni = cTRUE;
		e = Registry_GetWStrValue( _this, &size, key, szValueName, &reg_type, pValue, size );
	}
	else if ( *type == tid_STRING_OBJ ) {
		uni = cTRUE;
		e = Registry_GetStrObjValue( _this, &size, key, szValueName, &reg_type, pValue, size );
	}
	else if ( *type == tid_VECTOR )
		e = errNOT_SUPPORTED; // i have no type info of vector element here :-(
	else if ( *type == tid_SERIALIZABLE )
		e = Registry_GetSerVal( &d->m_last_err, key, szValueName, &reg_type, pValue, &size );
	else if ( *type == tid_BUFFER )
		e = Registry_GetBufVal( &d->m_last_err, key, szValueName, &reg_type, pValue, &size );
	else {
		d->m_last_err = RegQueryValueEx( HK(key), szValueName, 0, &reg_type, (LPBYTE)pValue, &size );
		e = _ConvertRegErr( d->m_last_err );
		if ( PR_SUCC(e) && !size && ((reg_type == REG_SZ) || (reg_type == REG_EXPAND_SZ)) ) {
			size = sizeof(tCHAR);
			if ( pValue )
				*(tCHAR*)pValue = 0;
		}
	}

	if ( PR_FAIL(e) && (e != errBUFFER_TOO_SMALL) )
		size = 0;

	else if ( type ) {
		switch( reg_type ) {
			default:
			case REG_NONE      : break;                      //  ( 0 )   No value type
			case REG_SZ        :                             //  ( 1 )   Unicode nul terminated string
			case REG_EXPAND_SZ : *type = uni ? tid_WSTRING : tid_STRING;  break; //  ( 2 )   Unicode nul terminated string (with environment variable references)
			case REG_BINARY    : *type = tid_BINARY;  break; //  ( 3 )   Free form binary
			case REG_DWORD     : *type = tid_DWORD;   break; //  ( 4 )   32-bit number
		}
	}
		
	if( bWOW64 )
		_deparse_reg_env(*type, pValue, size);
	
	if ( result )
		*result = size;
	PR_TRACE_A2( _this, "Leave \"Registry::GetValue\" method, ret tDWORD = %u, error = 0x%08x", size, e );
	return e;
}
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueByIndex )
// Parameters are:
tERROR pr_call Registry_GetValueByIndex( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ) {
	tERROR   e;
	RegData* d;
	tCHAR    name[MAX_PATH+1];
	//tDWORD   sub_vals = -1;
	//tTYPE_ID ptype = tid_VOID;
  
	PR_TRACE_A0( _this, "Enter \"Registry::GetValueByIndex\" method" );

	//if ( !type )
	//	type = &ptype;
	
	d = _this->data;
	d->m_last_err = 0;

	if ( PR_FAIL(e=adopt_key(d,&key)) )
		size = 0;
	else if ( PR_FAIL(e=get_value_name(d,key,index,name,sizeof(name),0)) )
		size = 0;
	else
		e = Registry_GetValue( _this, &size, key, name, type, pValue, size );

	//else if ( (index > 10) && PR_FAIL(e=_ConvertRegErr(d->m_last_err=RegQueryInfoKey(HK(key),0,0,0,0,0,0,&sub_vals,0,0,0,0))) )
	//	size = 0;
	//else if ( (index+1) > sub_vals )
	//	size = 0, e = errKEY_NOT_FOUND;

	//else {
	//	tUINT  i=0;
	//	tDWORD reg_type;
	//	tCHAR  name[MAX_PATH+1];

	//	if (d->last_valueenum_key==key && d->last_valueenum_index==index+1)
	//		i=index;
	//	for(; i<=index; i++ ) {
	//		tDWORD  sname = sizeof(name);
	//		LPBYTE  pval;
	//		LPDWORD psize;
	//		LPDWORD ptype;
	//		if ( i < index ) {
	//			ptype = 0;
	//			pval = 0;
	//			psize = 0;
	//		}
	//		else {
	//			ptype = &reg_type;
	//			pval = (LPBYTE)pValue;
	//			psize = &size;
	//		}

	//		d->m_last_err = RegEnumValue( HK(key), i, name, &sname, 0, 0, 0, 0 ); //ptype, pval, psize );
	//		e = _ConvertRegErr( d->m_last_err );

	//		if ( PR_FAIL(e) && (e != errBUFFER_TOO_SMALL) )
	//			break;
	//	}

	//	if ( PR_FAIL(e) && (e != errBUFFER_TOO_SMALL) )
	//		size = 0;
	//	else {
	//		d->last_valueenum_key = key;
	//		d->last_valueenum_index = index;
	//		if ( type ) {
	//			switch( reg_type ) {
	//				case REG_NONE      : *type = tid_VOID;   break; //  ( 0 )   No value type
	//				case REG_SZ        :                            //  ( 1 )   Unicode nul terminated string
	//				case REG_EXPAND_SZ : *type = tid_STRING; break; //  ( 2 )   Unicode nul terminated string (with environment variable references)
	//				case REG_BINARY    : *type = tid_BINARY; break; //  ( 3 )   Free form binary
	//				case REG_DWORD     : *type = tid_DWORD;  break; //  ( 4 )   32-bit number
	//			}
	//		}
	//		e = Registry_GetValue( _this, &size, key, name, type, pValue, size );		
	//	}
	//}
	//
	//if ( bWOW64 )
	//	_deparse_reg_env(*type, pValue, size);

	if ( result )
		*result = size;
	PR_TRACE_A2( _this, "Leave \"Registry::GetValueByIndex\" method, ret tDWORD = %u, error = 0x%08x", size, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueNameByIndex )
// Parameters are:
tERROR pr_call Registry_GetValueNameByIndex( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter \"Registry::GetValueNameByIndex\" method" );

  d = _this->data;
  d->m_last_err = 0;
  if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else
    e = get_value_name( d, key, index, name_buff, size, result );

	PR_TRACE_A2( _this, "Leave \"Registry::GetValueNameByIndex\" method, ret tDWORD = %u, error = 0x%08x", (result ? *result : -1), e );
	return e;
}
// AVP Prague stamp end



//tBOOL ValueEqual( hOBJECT obj, HKEY key, tPTR name, tDWORD rtype, tPTR value, tDWORD size, tBOOL wc)
//{
//	tDWORD old_size;
//	tDWORD old_type=rtype;
//	tBOOL b_equal=cFALSE;
//	LONG ret;
//
//	if(wc)
//		ret=RegQueryValueExW( key, name, 0, &old_type, 0, &old_size );
//	else
//		ret=RegQueryValueEx( key, name, 0, &old_type, 0, &old_size );
//
//	if(ERROR_SUCCESS == ret)
//	{
//		if(old_size==size)
//		{
//			tPTR tmp;
//			if(PR_SUCC(CALL_SYS_ObjHeapAlloc( obj, &tmp, old_size) ))
//			{
//				if(wc)
//					ret=RegQueryValueExW( key, name, 0, &old_type, tmp, &old_size );
//				else
//					ret=RegQueryValueEx( key, name, 0, &old_type, tmp, &old_size );
//
//				if(ERROR_SUCCESS == ret)
//				{
//					if(old_size==size && !memcmp(tmp,value,size))
//					{
//						b_equal=cTRUE;
//						if(wc)
//							PR_TRACE(( obj, prtIMPORTANT, "REG\t RegWrite skipped (value the same): %S", name ));
//						else
//							PR_TRACE(( obj, prtIMPORTANT, "REG\t RegWrite skipped (value the same): %s", name ));
//					}
//				}
//				CALL_SYS_ObjHeapFree(obj, tmp);
//			}
//		}
//	}
//	return b_equal;
//}



// ---
tOSType _get_os_type() {
	static tOSType g_os = os_unk;
	if ( g_os == os_unk ) {
		if ( GetVersion() < 0x80000000 ) // Windows NT/2000, Whistler
			g_os = os_nt;
		else                             // Windows 95/98/Me/Win32s
			g_os = os_9x;
	}
	return g_os;
}


// ---
tERROR pr_call Registry_SetWStrValue( hi_Registry _this, tRegKey key, const tCHAR* szValueName, const tWCHAR* pValue, tDWORD size ) {
	tERROR   e = errOK;
	RegData* d = _this->data;
	tCHAR    l_buff[100];
	tCHAR*   p_buff = l_buff;
	tUINT    len;

	d->m_last_err = 0;
	if ( pValue && size && *(tWCHAR*)pValue ) {
		if ( os_nt == _get_os_type() ) {
			len = 1 + lstrlen(szValueName);
			if ( len > (countof(l_buff)/sizeof(tWCHAR)) )
				e = CALL_SYS_ObjHeapAlloc( _this, &p_buff, len*sizeof(tWCHAR) );
			if ( PR_SUCC(e) )
			{
				tPTR _val = (tPTR)pValue;
				if( bWOW64 )
					e = _parse_reg_env((hOBJECT)_this, tid_WSTRING, &_val, size);

				if ( PR_SUCC(e) )
				{
					mbstowcs( (tWCHAR*)p_buff, szValueName, len );
					d->m_last_err = RegSetValueExW( HK(key), (tWCHAR*)p_buff, 0, REG_SZ, (LPBYTE)_val, size );
				}

				if( _val != pValue && _val )
					CALL_SYS_ObjHeapFree( _this, _val );
			}
		}
		else {
			tDWORD nMbSize = (len = 1 + ((tUINT)wcslen( (tWCHAR*)pValue )))*5;
			if ( nMbSize > countof(l_buff) )
				e = CALL_SYS_ObjHeapAlloc( _this, &p_buff, nMbSize );
			if ( PR_SUCC(e) ) {
				nMbSize = WideCharToMultiByte(CP_ACP, 0, (tWCHAR*)pValue, len, p_buff, nMbSize, NULL, NULL);
				d->m_last_err = RegSetValueEx( HK(key), szValueName, 0, REG_SZ, p_buff, nMbSize );
			}
		}
	}
	else
		d->m_last_err = RegSetValueEx( HK(key), szValueName, 0, REG_SZ, "", sizeof("") );
	if ( PR_SUCC(e) )
		e = _ConvertRegErr( d->m_last_err );
	
	if ( p_buff != l_buff )
		CALL_SYS_ObjHeapFree( _this, p_buff );
	return e;
}



// ---
tERROR pr_call Registry_SetStrObjValue( hi_Registry _this, tRegKey key, const tCHAR* szValueName, tPTR pValue ) {
	tERROR    e;
	hSTRING   str;
	tCODEPAGE cp;
	tDWORD    size;
	RegData*  d = _this->data;

	if ( !pValue || !*(tVOID**)pValue ) {
		d->m_last_err = RegSetValueEx( HK(key), szValueName, 0, REG_SZ, "", sizeof(tCHAR) );
		return _ConvertRegErr( d->m_last_err );
	}
	
	if ( PR_FAIL(CALL_SYS_ObjectValid(_this,*(hOBJECT*)pValue)) ) {
		sString* str = (sString*)pValue;
		if ( str->m_len )
			return Registry_SetWStrValue( _this, key, szValueName, str->m_data, (str->m_len+1)*sizeof(tWCHAR) );
		return Registry_SetWStrValue( _this, key, szValueName, L"", sizeof(tWCHAR) );
	}

	e = CALL_SYS_ObjectCheck( _this, *(hOBJECT*)pValue, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE );
	if ( PR_FAIL(e) )
		return e;

	str = *(hSTRING*)pValue;
	cp = (os_nt == _get_os_type()) ? cCP_UNICODE : cCP_ANSI;
	e = CALL_String_LengthEx( str, &size, cSTRING_WHOLE, cp, cSTRING_Z );

	if ( PR_SUCC(e) ) {
		tWCHAR  l_buff[0x100];
		tPTR    ptr = l_buff;
		if ( size > sizeof(l_buff) )
			e = CALL_SYS_ObjHeapAlloc( _this, &ptr, size );
		if ( PR_SUCC(e) )
			e = CALL_String_ExportToBuff( str, 0, cSTRING_WHOLE, ptr, size, cp, cSTRING_Z );
		if ( PR_SUCC(e) ) {
			if ( cp == cCP_ANSI ) {
				d->m_last_err = RegSetValueEx( HK(key), szValueName, 0, REG_SZ, (LPBYTE)pValue, size );
				e = _ConvertRegErr( d->m_last_err );
			}
			else
				e = Registry_SetWStrValue( _this, key, szValueName, ptr, size );
		}
		if ( ptr && (ptr != l_buff) )
			CALL_SYS_ObjHeapFree( _this, ptr );
	}
	return e;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetValue )
// Parameters are:
tERROR pr_call Registry_SetValue( hi_Registry _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) {
	tERROR   e = errOK;
	tDWORD   rtype;
	tDWORD   rval;
  RegData* d;
	PR_TRACE_A0( _this, "Enter \"Registry::SetValue\" method" );

  d = _this->data;
  d->m_last_err = 0;

  if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else switch( type ) {
		case tid_VOID        : rtype = REG_NONE;   e = errPARAMETER_INVALID; break;
		case tid_BYTE        : rtype = REG_DWORD;  if (size < sizeof(tBYTE)) e = errPARAMETER_INVALID; else rval = *(tBYTE*)pValue; break;
		case tid_WORD        : rtype = REG_DWORD;  if (size < sizeof(tWORD)) e = errPARAMETER_INVALID; else rval = *(tWORD*)pValue; break;
		case tid_DWORD       : rtype = REG_DWORD;  if (size < sizeof(tDWORD)) e = errPARAMETER_INVALID; else rval = *(tDWORD*)pValue; break;
		case tid_QWORD       : rtype = REG_BINARY; if (size < sizeof(tQWORD)) e = errPARAMETER_INVALID; break;
		case tid_BOOL        : 
			rtype = REG_DWORD;
			if ( size == sizeof(tBOOL) )
				rval = *(tBOOL*)pValue;
			else if ( size == 1 )
				rval = !!*(tBYTE*)pValue;
			else
				e = errPARAMETER_INVALID;
			break;
		case tid_CHAR        : rtype = REG_DWORD;  if (size < sizeof(tCHAR)) e = errPARAMETER_INVALID; else rval = *(tCHAR*)pValue; break;
		case tid_WCHAR       : rtype = REG_DWORD;  if (size < sizeof(tWCHAR)) e = errPARAMETER_INVALID; else rval = *(tWCHAR*)pValue; break;
		case tid_STRING      : rtype = REG_SZ;     if ( !size ) size = lstrlen((tCHAR*)pValue); break;
		case tid_WSTRING     : rtype = REG_SZ;     if ( !size ) size = ((tDWORD)(sizeof(tWCHAR)* wcslen((tWCHAR*)pValue))); break;
		case tid_ERROR       : rtype = REG_DWORD;  if (size < sizeof(tERROR)) e = errPARAMETER_INVALID; else rval = *(tERROR*)pValue; break;
		case tid_PTR         : rtype = REG_DWORD;  if (size < sizeof(tPTR)) e = errPARAMETER_INVALID; else rval = (tDWORD)*(tPTR*)pValue; break;
		case tid_INT         : rtype = REG_DWORD;  if (size < sizeof(tINT)) e = errPARAMETER_INVALID; else rval = *(tINT*)pValue; break; // native signed integer type
		case tid_UINT        : rtype = REG_DWORD;  if (size < sizeof(tUINT)) e = errPARAMETER_INVALID; else rval = *(tUINT*)pValue; break; // native unsigned integer type
		case tid_SBYTE       : rtype = REG_DWORD;  if (size < sizeof(tSBYTE)) e = errPARAMETER_INVALID; else rval = *(tSBYTE*)pValue; break;

		case tid_SHORT       : rtype = REG_DWORD;  if (size < sizeof(tSHORT)) e = errPARAMETER_INVALID; else rval = *(tSHORT*)pValue; break;
		case tid_LONG        : rtype = REG_DWORD;  if (size < sizeof(tLONG)) e = errPARAMETER_INVALID; else rval = *(tLONG*)pValue; break;
		case tid_LONGLONG    : rtype = REG_BINARY; if (size < sizeof(tLONGLONG)) e = errPARAMETER_INVALID; break;
		case tid_IID         : rtype = REG_DWORD;  if (size < sizeof(tIID)) e = errPARAMETER_INVALID; else rval = *(tIID*)pValue; break; // plugin interface identifier
		case tid_PID         : rtype = REG_DWORD;  if (size < sizeof(tPID)) e = errPARAMETER_INVALID; else rval = *(tPID*)pValue; break; // plugin identifier
		case tid_ORIG_ID     : rtype = REG_DWORD;  if (size < sizeof(tORIG_ID)) e = errPARAMETER_INVALID; else rval = *(tORIG_ID*)pValue; break; // object origin identifier
		case tid_OS_ID       : rtype = REG_DWORD;  if (size < sizeof(tOS_ID)) e = errPARAMETER_INVALID; else rval = *(tOS_ID*)pValue; break; // folder type identifier
		case tid_VID         : rtype = REG_DWORD;  if (size < sizeof(tVID)) e = errPARAMETER_INVALID; else rval = *(tVID*)pValue; break; // vendor ideftifier
		case tid_PROPID      : rtype = REG_DWORD;  if (size < sizeof(tPROPID)) e = errPARAMETER_INVALID; else rval = *(tPROPID*)pValue; break; // property identifier
		case tid_VERSION     : rtype = REG_DWORD;  if (size < sizeof(tVERSION)) e = errPARAMETER_INVALID; else rval = *(tVERSION*)pValue; break; // version of any identifier: High word - version, Low word - subversion
		case tid_CODEPAGE    : rtype = REG_DWORD;  if (size < sizeof(tCODEPAGE)) e = errPARAMETER_INVALID; else rval = *(tCODEPAGE*)pValue; break; // codepage identifier
		case tid_LCID        : rtype = REG_DWORD;  if (size < sizeof(tLCID)) e = errPARAMETER_INVALID; else rval = *(tLCID*)pValue; break; // codepage identifier
		case tid_DATA        : rtype = REG_DWORD;  if (size < sizeof(tDATA)) e = errPARAMETER_INVALID; else rval = *(tDWORD*)pValue; break; // universal data storage
		case tid_DATETIME    : rtype = REG_BINARY; if (size < sizeof(tDATETIME)) e = errPARAMETER_INVALID; break; // universal date storage
		case tid_FUNC_PTR    : rtype = REG_DWORD;  if (size < sizeof(tFUNC_PTR)) e = errPARAMETER_INVALID; else rval = (tDWORD)*(tFUNC_PTR*)pValue; break; // universal function ptr
		case tid_DOUBLE      : rtype = REG_BINARY; if (size < sizeof(tDOUBLE)) e = errPARAMETER_INVALID; break; // universal function ptr
		
		// additional types
		case tid_BINARY      : rtype = REG_BINARY; break;
		case tid_IFACEPTR    : rtype = REG_DWORD;  if (size < sizeof(tFUNC_PTR)) e = errPARAMETER_INVALID; else rval = (tDWORD)*(tFUNC_PTR*)pValue; break;
		case tid_OBJECT      : rtype = REG_DWORD;  if (size < sizeof(hOBJECT)) e = errPARAMETER_INVALID; else rval = (tDWORD)*(hOBJECT*)pValue; break;
		case tid_EXPORT      : rtype = REG_BINARY; if (size < sizeof(tEXPORT)) e = errPARAMETER_INVALID; break;
		case tid_IMPORT      : rtype = REG_BINARY; if (size < sizeof(tIMPORT)) e = errPARAMETER_INVALID; break;
		case tid_TRACE_LEVEL : rtype = REG_DWORD;  if (size < sizeof(tTRACE_LEVEL)) e = errPARAMETER_INVALID; else rval = *(tTRACE_LEVEL*)pValue; break;
		case tid_TYPE_ID     : rtype = REG_DWORD;  if (size < sizeof(tTYPE_ID)) e = errPARAMETER_INVALID; else rval = *(tTYPE_ID*)pValue; break;
		case tid_VECTOR      : rtype = REG_NONE;   e = errNOT_SUPPORTED; break; // i have no type info of vector element here :-(
		case tid_STRING_OBJ  : rtype = REG_SZ;     break;
		case tid_SERIALIZABLE: rtype = REG_BINARY; break;
		case tid_BUFFER      : rtype = REG_BINARY; break;

		case tid_SIZE_T      : rtype = REG_BINARY; if (size < sizeof(tSIZE_T)) e = errPARAMETER_INVALID; else rval = *(tSIZE_T*)pValue; break;
		case tid_IMPL_ID     : rtype = REG_DWORD;  if (size < sizeof(tIMPL_ID)) e = errPARAMETER_INVALID; else rval = *(tIMPL_ID*)pValue; break;
		case tid_MSG_ID      : rtype = REG_DWORD;  if (size < sizeof(tMSG_ID)) e = errPARAMETER_INVALID; else rval = *(tMSG_ID*)pValue; break;
		case tid_MSGCLS_ID   : rtype = REG_DWORD;  if (size < sizeof(tMSGCLS_ID)) e = errPARAMETER_INVALID; else rval = *(tMSGCLS_ID*)pValue; break;

		default              : rtype = REG_NONE;   e = errPARAMETER_INVALID; break;
	}

	if ( PR_SUCC(e) ) {
		if ( rtype == REG_DWORD ) {
			pValue = &rval;
			size = sizeof(tDWORD);
		}
		if ( type == tid_WSTRING )
			e = Registry_SetWStrValue( _this, key, szValueName, (tWCHAR*)pValue, size );
		else if ( type == tid_STRING_OBJ )
			e = Registry_SetStrObjValue( _this, key, szValueName, pValue );
		else if ( type == tid_SERIALIZABLE )
			e = Registry_SetSerVal( &d->m_last_err, key, szValueName, pValue );
		else if ( type == tid_BUFFER )
			e = Registry_SetBufVal( &d->m_last_err, key, szValueName, pValue );
		else {
			d->m_last_err = RegSetValueEx( HK(key), szValueName, 0, rtype, (LPBYTE)pValue, size );
			e = _ConvertRegErr( d->m_last_err );
		}
	}
	
	PR_TRACE_A1( _this, "Leave \"Registry::SetValue\" method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetValueByIndex )
// Parameters are:
tERROR pr_call Registry_SetValueByIndex( hi_Registry _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) {
	tERROR   e;
	tDWORD   c = 0;
	tCHAR    n[MAX_PATH+1];
  RegData* d;
	PR_TRACE_A0( _this, "Enter \"Registry::SetValueByIndex\" method" );

  d = _this->data;
  d->m_last_err = 0;

  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else {
    d->m_last_err = RegQueryInfoKey( HK(key), 0, 0, 0, 0, 0, 0, &c, 0, 0, 0, 0 );
    e = _ConvertRegErr( d->m_last_err );
  }

	if ( PR_FAIL(e) )
		;
	else if ( index < c ) {
    if ( PR_SUCC(e=get_value_name(d,key,index,n,sizeof(n),0)) )
			e = Registry_SetValue( _this, key, n, type, pValue, size, cFALSE );
	}
	else if ( bCreateIfNotExist ) {
		int      len;
    GUID     guid;
		LPOLESTR str = 0;
		CoCreateGuid( &guid );
		StringFromCLSID( &guid, &str );
    len = wsprintf( n, "%04d__", c );
    wcstombs( n+len, str, sizeof(n)-len );
		e = Registry_SetValue( _this, key, n, type, pValue, size, cTRUE );
		CoTaskMemFree( str );
	}
	else
		e = errKEY_INDEX_OUT_OF_RANGE;

	PR_TRACE_A1( _this, "Leave \"Registry::SetValueByIndex\" method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyCount )
// Parameters are:
tERROR pr_call Registry_GetKeyCount( hi_Registry _this, tDWORD* result, tRegKey key ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::GetKeyCount method" );

  d = _this->data;
  d->m_last_err = 0;

  if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else {
    d->m_last_err = RegQueryInfoKey( HK(key), 0, 0, 0, result, 0, 0, 0, 0, 0, 0, 0 );
    e = _ConvertRegErr( d->m_last_err );
  }

  PR_TRACE_A2( _this, "Leave Registry::GetKeyCount method, ret tDWORD = %u, error = 0x%08x", (result ? *result : -1), e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueCount )
// Parameters are:
tERROR pr_call Registry_GetValueCount( hi_Registry _this, tDWORD* result, tRegKey key ) {
  tERROR   e;
  RegData* d;
  PR_TRACE_A0( _this, "Enter \"Registry::GetValueCount\" method" );

  d = _this->data;
  d->m_last_err = 0;

  if ( PR_FAIL(e=adopt_key(d,&key)) )
    ;
  else {
    d->m_last_err = RegQueryInfoKey( HK(key), 0, 0, 0, 0, 0, 0, result, 0, 0, 0, 0 );
    e = _ConvertRegErr( d->m_last_err );
  }
		
  PR_TRACE_A2( _this, "Leave \"Registry::GetValueCount\" method, ret tDWORD = %u, error = 0x%08x", (result ? *result : -1), e );
  return e;
}
// AVP Prague stamp end



// ---
tDWORD winnt_del_key( hi_Registry _this, HKEY key, const tCHAR* name ) {
	HKEY    sub_key = 0;
	tDWORD  key_count = 0;
	tDWORD  key_max_len = 0;
	tCHAR   key_name[50];
	tSTRING pkey_name;
	tDWORD  err;
	DWORD   WOW64FLAG;

	typedef LONG (WINAPI *tRegDeleteKeyExA)(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved);
	HMODULE hAdvApi;
	tRegDeleteKeyExA fnRegDeleteKeyExA;

	WOW64FLAG = bWOWFlagsAvailable ? ((_this->data->m_is_wow64_64key ? KEY_WOW64_64KEY : 0) | (_this->data->m_is_wow64_32key ? KEY_WOW64_32KEY : 0)) : 0;
	err = RegOpenKeyEx( key, name, 0, KEY_ALL_ACCESS | WOW64FLAG, &sub_key );
	if ( ERROR_SUCCESS != err )
		return err;

	err = RegQueryInfoKey( sub_key, 0, 0, 0, &key_count, &key_max_len, 0, 0, 0, 0, 0, 0 );
	if ( ERROR_SUCCESS != err )
		return err;

	if ( ++key_max_len > sizeof(key_name) ) {
		if ( PR_FAIL(CALL_SYS_ObjHeapAlloc(_this,&pkey_name,key_max_len)) )
			return ERROR_NOT_ENOUGH_MEMORY;
	}
	else
		pkey_name = key_name;
	for( ; key_count && (err==ERROR_SUCCESS); key_count-- ) {
		tDWORD tsize = key_max_len;
		err = RegEnumKeyEx( sub_key, 0, pkey_name, &tsize, 0, 0, 0, 0 );
		if ( ERROR_SUCCESS == err )
			err = winnt_del_key( _this, sub_key, pkey_name );
	}
	if ( pkey_name != key_name )
		CALL_SYS_ObjHeapFree( _this, pkey_name );

	RegCloseKey( sub_key );

	if ( bWOW64 ) {
		hAdvApi = GetModuleHandle("advapi32.dll");
		fnRegDeleteKeyExA = hAdvApi ? (tRegDeleteKeyExA)GetProcAddress(hAdvApi, "RegDeleteKeyExA") : NULL;
		if ( fnRegDeleteKeyExA )
			return fnRegDeleteKeyExA(key, name, WOW64FLAG, 0);
	}

	return RegDeleteKey( key, name );
}



// ---
tDWORD win9x_del_key( hi_Registry _this, HKEY key, const tCHAR* name ) {
  return RegDeleteKey( key, name );
}


  
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clean )
// Parameters are:
tERROR pr_call Registry_Clean( hi_Registry _this ) {
  tERROR e;
  RegData* d;
  PR_TRACE_A0( _this, "Enter Registry::Clean method" );

  d = _this->data;
  d->m_last_err = 0;

  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( !d->m_root )
    e = errKEY_CANNOT_BE_DELETED;
  //else if ( _strnicmp(d->m_root_path,"software\\KasperskyLab",21) )
  //  e = errKEY_CANNOT_BE_DELETED;
  else {
    tDWORD key_count;
    tDWORD key_max_len;
    tDWORD val_count;
    tDWORD val_max_len;
    tBOOL  nt = !(0x80000000 & GetVersion());
    tDWORD (*del_proc)(hi_Registry,HKEY,const tCHAR*) = nt ? winnt_del_key : win9x_del_key;

    e = errOK;
    d->m_last_err = RegQueryInfoKey( d->m_root, 0, 0, 0, &key_count, &key_max_len, 0, &val_count, &val_max_len, 0, 0, 0 );
    if ( ERROR_SUCCESS == d->m_last_err ) {
      tUINT  tsize;
      tUINT  nsize;
      tCHAR  name[MAX_PATH];
      tCHAR* pname;
      if ( key_max_len > val_max_len )
        nsize = key_max_len;
      else
        nsize = val_max_len;
      nsize++;
      if ( nsize > sizeof(name) )
        e = CALL_SYS_ObjHeapAlloc( _this, &pname, nsize );
      else
        pname = name;

      if ( PR_SUCC(e) ) {
        for( ; key_count && (ERROR_SUCCESS==d->m_last_err); key_count-- ) {
          tsize = nsize;
          d->m_last_err = RegEnumKeyEx( d->m_root, 0, pname, &tsize, 0, 0, 0, 0 );
          if ( ERROR_SUCCESS == d->m_last_err )
            d->m_last_err = del_proc( _this, d->m_root, pname );
        }
        for( ; val_count && (ERROR_SUCCESS==d->m_last_err); val_count-- ) {
          tsize = nsize;
          d->m_last_err = RegEnumValue( d->m_root, 0, pname, &tsize, 0, 0, 0, 0 );
          if ( ERROR_SUCCESS == d->m_last_err )
            d->m_last_err = RegDeleteValue( d->m_root, pname );
        }
      }
      if ( pname != name )
        CALL_SYS_ObjHeapFree( _this, pname );
    }
    if ( PR_SUCC(e) )
      e = _ConvertRegErr( d->m_last_err );
  }

  PR_TRACE_A1( _this, "Leave Registry::Clean method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// ---
tERROR Reg_CopyKey( hREGISTRY from, tRegKey from_key, hREGISTRY to, tRegKey to_key );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CopyKey )
// Parameters are:
tERROR pr_call Registry_CopyKey( hi_Registry _this, tRegKey key, hREGISTRY destination, tRegKey dest_key ) {
  tERROR err;
  RegData* d;
  PR_TRACE_A0( _this, "Enter Registry::CopyKey method" );

  d = _this->data;
  d->m_last_err = 0;
	
  if ( PR_FAIL(err=adopt_key(d,&key)) )
    ;
	else
		err = Reg_CopyKey( (hREGISTRY)_this, key, destination, dest_key );

  PR_TRACE_A1( _this, "Leave Registry::CopyKey method, ret %terr", err );
  return err;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CopyKeyByName )
// Parameters are:
tERROR pr_call Registry_CopyKeyByName( hi_Registry _this, const tCHAR* src, hREGISTRY dst ) {
  tRegKey key;
  tERROR  err;
  PR_TRACE_A0( _this, "Enter Registry::CopyKeyByName method" );

  err = Registry_OpenKey( _this, &key, cRegRoot, (const tCHAR*)src, cFALSE );
  if ( PR_SUCC(err) ) {
    err = Reg_CopyKey( (hREGISTRY)_this, key, dst, cRegRoot );
		Registry_CloseKey( _this, key );
	}

  PR_TRACE_A1( _this, "Leave Registry::CopyKeyByName method, ret %terr", err );
  return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetRootKey )
// Parameters are:
tERROR pr_call Registry_SetRootKey( hi_Registry _this, tRegKey p_root, tBOOL p_close_prev ) {
  tERROR e;
  RegData* d;
	tDWORD size;
	PR_TRACE_A0( _this, "Enter Registry::SetRootKey method" );

	d = _this->data;
	if ( d->m_hive_opened && d->m_hive )
		RegCloseKey( d->m_hive );
	d->m_hive = 0;
	d->m_hive_opened = cFALSE;
	d->m_last_err = ERROR_SUCCESS;
	
	if ( d->m_root && (d->m_root != (HKEY)p_root) && p_close_prev )
		RegCloseKey( d->m_root );

  if ( p_root == cRegNothing ) // special case: set root to nothing
		d->m_root = cRegRoot;
	else
		d->m_root = (HKEY)p_root;

	e = Registry_GetKeyName( _this, &size, cRegRoot, 0, 0, cFALSE );
	if ( PR_FAIL(e) )
		;
	else if ( size ) {
		e = CALL_SYS_ObjHeapRealloc( _this, &d->m_root_path, d->m_root_path, size );
		if ( PR_SUCC(e) )
			e = Registry_GetKeyName( _this, &size, cRegRoot, d->m_root_path, size, cFALSE );
	}
	else if ( d->m_root_path ) {
		CALL_SYS_ObjHeapFree( _this, d->m_root_path );
		d->m_root_path = 0;
	}

	PR_TRACE_A1( _this, "Leave Registry::SetRootKey method, ret %terr", e );
	return e;
}
// AVP Prague stamp end



// ---
HKEY _get_current_user( hi_Registry _this, tBOOL classes, tBOOL* opened )
{
	HKEY    key = classes ? HKEY_CLASSES_ROOT : HKEY_CURRENT_USER;
	hTOKEN  hToken;
	tERROR  err;
	tCHAR   buf[0x100] = {0};
	DWORD   WOW64FLAG;

	if ( opened )
		*opened = cFALSE;

	err = CALL_SYS_ObjectCreateQuick( _this, &hToken, IID_TOKEN, PID_ANY, SUBTYPE_ANY );
	if( PR_FAIL(err) ) {
		PR_TRACE(( _this, prtIMPORTANT, "WinReg\tcannot get token (err = %terr)", err ));
		return key;
	}
	
	err = CALL_SYS_PropertyGetStr( hToken, NULL, pgOBJECT_FULL_NAME, buf, sizeof(buf), cCP_ANSI );
	CALL_SYS_Release( hToken );
	
	if( PR_FAIL(err) ) {
		PR_TRACE(( _this, prtIMPORTANT, "WinReg\tcannot get token full name (err = %terr)", err ));
		return key;
	}

	if ( classes )
		strcat(buf, "_Classes");
	
	PR_TRACE((_this, prtIMPORTANT, "WinReg\ttry to open key \"%s\"", buf));
	WOW64FLAG = bWOWFlagsAvailable ? ((_this->data->m_is_wow64_64key ? KEY_WOW64_64KEY : 0) | (_this->data->m_is_wow64_32key ? KEY_WOW64_32KEY : 0)) : 0;
	if( RegOpenKeyEx(HKEY_USERS, buf, 0, KEY_ALL_ACCESS | WOW64FLAG, &key ) != ERROR_SUCCESS ) {
		PR_TRACE((_this, prtIMPORTANT, "WinReg\tcannot open key \"%s\" (winerr = %u)", buf, GetLastError() ));
		key = classes ? HKEY_CLASSES_ROOT : HKEY_CURRENT_USER;
		if ( opened )
			*opened = cFALSE;
	}
	else if ( opened )
		*opened = cTRUE;
	
	return key;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetRootStr )
// Parameters are:
tERROR pr_call Registry_SetRootStr( hi_Registry _this, const tCHAR* p_root, tBOOL p_close_prev ) {
  tERROR   e = errOK;
  RegData* d;
  HKEY     hive = 0;
	tBOOL    hive_opened = cFALSE;
  DWORD    WOW64FLAG;

	PR_TRACE_A0( _this, "Enter Registry::SetRootStr method" );

  if ( !p_root || !*p_root ) { // special case: set root to nothing
    d = _this->data;
		d->m_last_err = ERROR_SUCCESS;
    if ( d->m_root_path )
      CALL_SYS_ObjHeapFree( _this, d->m_root_path );
    d->m_root_path = 0;
    if ( d->m_root && p_close_prev )
      RegCloseKey( d->m_root );
		d->m_root = 0;
  }
  else {
    HKEY     key;
    tUINT    len, flen, old_root_len;
    tSTRING  nroot = 0;
		tBOOL    absolute;
		tUINT    name_shift = 0;

    d = _this->data;
		d->m_last_err = ERROR_SUCCESS;
    if ( *p_root == '\\' ) {
			absolute = cTRUE;
      p_root++;
		}
		else
			absolute = cFALSE;

    len = 1;
    #define sizeofs(s) ((sizeof(s)-sizeof(s[0]))/sizeof(s[0]))

    if ( !_strnicmp(p_root,g_hklm,sizeofs(g_hklm)) )
      name_shift =  sizeofs(g_hklm), absolute = cTRUE, key = hive = HKEY_LOCAL_MACHINE;
    else if ( !_strnicmp(p_root,g_HKLM,sizeofs(g_HKLM)) )
      name_shift =  sizeofs(g_HKLM), absolute = cTRUE, key = hive = HKEY_LOCAL_MACHINE;
    else if ( !_strnicmp(p_root,g_hkcu,sizeofs(g_hkcu)) )
      name_shift =  sizeofs(g_hkcu), absolute = cTRUE, key = hive = _get_current_user( _this, cFALSE, &hive_opened );
    else if ( !_strnicmp(p_root,g_HKCU,sizeofs(g_HKCU)) )
      name_shift =  sizeofs(g_HKCU), absolute = cTRUE, key = hive = _get_current_user( _this, cFALSE, &hive_opened );
    else if ( !_strnicmp(p_root,g_hkcr,sizeofs(g_hkcr)) )
      name_shift =  sizeofs(g_hkcr), absolute = cTRUE, key = hive = _get_current_user( _this, cTRUE, &hive_opened );
    else if ( !_strnicmp(p_root,g_HKCR,sizeofs(g_HKCR)) )
      name_shift =  sizeofs(g_HKCR), absolute = cTRUE, key = hive = _get_current_user( _this, cTRUE, &hive_opened );
    else if ( !_strnicmp(p_root,g_hku,sizeofs(g_hku)) )
      name_shift =  sizeofs(g_hku), absolute = cTRUE, key = hive = HKEY_USERS;
    else if ( !_strnicmp(p_root,g_HKU,sizeofs(g_HKU)) )
      name_shift =  sizeofs(g_HKU), absolute = cTRUE, key = hive = HKEY_USERS;
    else if ( !_strnicmp(p_root,g_hkpd,sizeofs(g_hkpd)) )
      name_shift =  sizeofs(g_hkpd), absolute = cTRUE, key = hive = HKEY_PERFORMANCE_DATA;
    else if ( !_strnicmp(p_root,g_HKPD,sizeofs(g_HKPD)) )
      name_shift =  sizeofs(g_HKPD), absolute = cTRUE, key = hive = HKEY_PERFORMANCE_DATA;
    else if ( !_strnicmp(p_root,g_hkcc,sizeofs(g_hkcc)) )
      name_shift =  sizeofs(g_hkcc), absolute = cTRUE, key = hive = HKEY_CURRENT_CONFIG;
    else if ( !_strnicmp(p_root,g_HKCC,sizeofs(g_HKCC)) )
      name_shift =  sizeofs(g_HKCC), absolute = cTRUE, key = hive = HKEY_CURRENT_CONFIG;
    else if ( !_strnicmp(p_root,g_hkdd,sizeofs(g_hkdd)) )
      name_shift =  sizeofs(g_hkdd), absolute = cTRUE, key = hive = HKEY_DYN_DATA;
    else if ( !_strnicmp(p_root,g_HKDD,sizeofs(g_HKDD)) )
      name_shift =  sizeofs(g_HKDD), absolute = cTRUE, key = hive = HKEY_DYN_DATA;
    else if ( absolute )
      len = 0, key = hive = HKEY_LOCAL_MACHINE;
		else
			len = 0, key = d->m_root, hive = d->m_hive, hive_opened = d->m_hive_opened;

		PR_TRACE((_this, prtNOTIFY, "Registry_PROP_set_root_str RegPath: %s RegHandle: %08x", p_root, hive)) ;

		if ( name_shift ) {
			if ( !hive )
				e = errREG_IMPERSONATION;
			else
				p_root += name_shift;
		}

		if ( PR_SUCC(e) ) {
			if ( len && (*p_root == '\\') )
				p_root++;

			for( len=0; p_root[len]; len++ )
				;

			flen = len+(2+cRegMaxName);
			if ( absolute )
				old_root_len = 0;
			else
				flen += 1 + (old_root_len = (d->m_root_path ? ((tUINT)strlen(d->m_root_path)) : 0));

			e = CALL_SYS_ObjHeapAlloc( _this, &nroot, flen );
		}

    if ( PR_SUCC(e) ) {
			if ( len && p_root[len-1] == '\\' )
        len--;
			if ( !len )
				e = errPARAMETER_INVALID;
			else {
				HKEY   root;
				tBOOL  op;
				tCHAR* root_str;

				if ( old_root_len ) {
					memcpy( nroot, d->m_root_path, old_root_len );
					nroot[old_root_len++] = '\\';
				}

				root_str = nroot + old_root_len;
        memcpy( root_str, p_root, len );
				root_str[len] = 0;

				op = CALL_SYS_PropertyGetBool( _this, psOBJECT_OPERATIONAL );
				WOW64FLAG = bWOWFlagsAvailable ? ((d->m_is_wow64_64key ? KEY_WOW64_64KEY : 0) | (d->m_is_wow64_32key ? KEY_WOW64_32KEY : 0)) : 0;
				if ( !op || (ERROR_SUCCESS == (d->m_last_err=RegOpenKeyEx(key, root_str, 0, KEY_ALL_ACCESS | WOW64FLAG, &root))) ) {
					if ( d->m_root_path )
						CALL_SYS_ObjHeapFree( _this, d->m_root_path );
					d->m_root_path = nroot;
					d->m_hive = hive;
					d->m_hive_opened = hive_opened;
					hive = 0;
					hive_opened = cFALSE;
					if ( d->m_root && p_close_prev )
						RegCloseKey( d->m_root );
					d->m_root = root;
				}
				else {
					CALL_SYS_ObjHeapFree( _this, nroot );
					e = _ConvertRegErr( d->m_last_err );
				}
			}
    }
  }

	if ( hive_opened && hive )
		RegCloseKey( hive );

	PR_TRACE_A1( _this, "Leave Registry::SetRootStr method, ret %terr", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Parameters are:
tERROR pr_call Registry_Flush( hi_Registry _this, tBOOL p_force ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Registry::Flush method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Registry::Flush method, ret %terr", error );
	return error;
}
// AVP Prague stamp end




// ---
tERROR pr_call get_subkey_name( RegData* d, tRegKey key, tUINT index, tCHAR* name, tUINT size, tDWORD* out_size ) {
	tUINT   i;
	tERROR  e;
	tDWORD  name_size;
	tDWORD* osize;
	tCHAR   sname[MAX_PATH];
	
	if ( out_size )
		*out_size = 0;
	
	if ( index > 10 ) {
		tDWORD sub_keys;
		if ( PR_FAIL(e = _ConvertErr(d->m_last_err=RegQueryInfoKey(HK(key),0,0,0,&sub_keys,0,0,0,0,0,0,0))) )
			return e;
		if ( (index+1) > sub_keys )
			return errKEY_INDEX_OUT_OF_RANGE;
	}
	if (key!=d->m_last_keyenum_key || index != d->m_last_keyenum_index+1)
	{
	  // enum from 0 index (as described in MSDN)
	  for( i=0; i<index; i++ ) {
			name_size = sizeof(sname);
			d->m_last_err = RegEnumKeyEx( HK(key), i, sname, &name_size, 0, 0, 0, 0 );
			if ( PR_FAIL(e=_ConvertErr(d->m_last_err)) )
				return e;
		}
	}
	else
		i=index;

	if ( out_size )
		osize = out_size;
	else
		osize = &name_size;
	if ( name ) 
		*osize = size;
	else {
		name = sname;
		*osize = sizeof(sname);
	}

	*name = 0;
	d->m_last_err = RegEnumKeyEx( HK(key), i, name, osize, 0, 0, 0, 0 );
	if ( PR_SUCC(e=_ConvertErr(d->m_last_err)) )
	{
		(*osize)++;
		d->m_last_keyenum_key = key;
		d->m_last_keyenum_index = index;
	}

	return e;
}



// ---
tERROR pr_call get_value_name( RegData* d, tRegKey key, tUINT index, tCHAR* name, tUINT size, tDWORD* out_size ) {
  tUINT   i;
  tERROR  e = errOK;
  tDWORD  lsize;
  tDWORD* osize;
  tCHAR   sname[MAX_PATH];
  
  if ( out_size )
    *out_size = 0;
  
  if ( index > 10 ) {
    tDWORD sub_vals;
    if ( PR_FAIL(e = _ConvertErr(d->m_last_err=RegQueryInfoKey(HK(key),0,0,0,0,0,0,&sub_vals,0,0,0,0))) )
      return e;
    if ( (index+1) > sub_vals )
      return errKEY_INDEX_OUT_OF_RANGE;
  }
  

  //if (key!=d->last_valueenum_key || index!=d->last_valueenum_index+1) 
	{
    // enum from 0 index (as described in MSDN)
		for(i=0; i<index; i++ ) {
			lsize = sizeof(sname);
			d->m_last_err = RegEnumValue( HK(key), i, sname, &lsize, 0, 0, 0, 0 );
			if ( PR_FAIL(e=_ConvertErr(d->m_last_err)) )
				return e;
		}
  }
  //else
	//  i=index;
 
  if ( out_size )
    osize = out_size;
  else
    osize = &lsize;
  if ( name ) 
    *osize = size;
  else {
    name = sname;
    *osize = sizeof(sname);
  }
  d->m_last_err = RegEnumValue( HK(key), i, name, osize, 0, 0, 0, 0 );
  if ( PR_SUCC(e=_ConvertErr(d->m_last_err)) )
  {
    (*osize)++;
    //d->last_valueenum_key = key;
    //d->last_valueenum_index = index;
  }

  return e;
}




// ---
tERROR pr_call adopt_key( RegData* d, tRegKey* key ) {
  
  if ( !d->m_root )
    return errOK;
  
  switch( (tDWORD)*key ) {
    case cHKEY_CLASSES_ROOT     : // HKEY classes root
    case cHKEY_CURRENT_USER     : // HKEY current user
    case cHKEY_LOCAL_MACHINE    : // HKEY local machine
    case cHKEY_USERS            : // HKEY users
    case cHKEY_PERFORMANCE_DATA : //
    case cHKEY_CURRENT_CONFIG   : //
    case cHKEY_DYN_DATA         : //
      *key = 0;
      return errKEY_NOT_FOUND;
    
    case cRegRoot :
      *key = (tRegKey)d->m_root;
      return errOK;

    default:
      return errOK;
  }
}

// ---
#define REGWOW64_ENV_PRGFLS_W		L"%ProgramFiles%"
#define REGWOW64_ENV_PRGFLS_A		"%ProgramFiles%"
#define REGWOW64_ENV_PRGFLS_LEN		14

#define REGWOW64_ENV_PRGFLS_SUBST_W	L"%PrgFls______%"
#define REGWOW64_ENV_PRGFLS_SUBST_A	"%PrgFls______%"

tERROR _parse_reg_env(hOBJECT hObj, tTYPE_ID type, tPTR * pValue, tDWORD size)
{
	tPTR dataNew = NULL;
	tERROR err;
	sString strSrc;
	tDWORD nPos;

	if( type != tid_WSTRING || !(size/sizeof(tWCHAR)) )
		return errOK;
	
	strSrc.m_data = (tWCHAR *)*pValue;
	strSrc.m_len = size/sizeof(tWCHAR) - 1;
	strSrc.m_size = 0;
	
	nPos = cSTRING_EMPTY_LENGTH;
	FindBuff(&strSrc, 0, cSTRING_WHOLE_LENGTH, REGWOW64_ENV_PRGFLS_W, 0, cCP_UNICODE, fSTRING_COMPARE_CASE_INSENSITIVE, &nPos);
	if( nPos == cSTRING_EMPTY_LENGTH )
		return errOK;
	
	err = CALL_SYS_ObjHeapAlloc( hObj, &dataNew, size);
	if( PR_FAIL(err) )
		return err;

	memcpy(dataNew, *pValue, size);

	do 
	{
		memcpy((tWCHAR *)dataNew + nPos, REGWOW64_ENV_PRGFLS_SUBST_W, REGWOW64_ENV_PRGFLS_LEN*sizeof(tWCHAR));
		nPos += REGWOW64_ENV_PRGFLS_LEN;

		FindBuff(&strSrc, nPos, cSTRING_WHOLE_LENGTH, REGWOW64_ENV_PRGFLS_W, 0, cCP_UNICODE, fSTRING_COMPARE_CASE_INSENSITIVE, &nPos);
	} while( nPos != cSTRING_EMPTY_LENGTH );

	*pValue = dataNew;
	return errOK;
}

void _deparse_reg_env(tTYPE_ID type, tPTR pValue, tDWORD size)
{
	if( !pValue )
		return;
	
	switch( type )
	{
	case tid_WSTRING:
		{
			tWCHAR * str = (tWCHAR *)pValue;
			tDWORD len = size/sizeof(tWCHAR);

			if( len ) len--;

			if( len && !str[len] )
			{
				while( str = wcsstr(str, REGWOW64_ENV_PRGFLS_SUBST_W) )
				{
					memcpy(str, REGWOW64_ENV_PRGFLS_W, REGWOW64_ENV_PRGFLS_LEN*sizeof(tWCHAR));
					str += REGWOW64_ENV_PRGFLS_LEN;
				}
			}
		}
		break;

	case tid_STRING:
		{
			tCHAR * str = (tCHAR *)pValue;
			tDWORD len = size/sizeof(tCHAR);

			if( len ) len--;

			if( len && !str[len] )
			{
				while( str = strstr(str, REGWOW64_ENV_PRGFLS_SUBST_A) )
				{
					memcpy(str, REGWOW64_ENV_PRGFLS_A, REGWOW64_ENV_PRGFLS_LEN*sizeof(tCHAR));
					str += REGWOW64_ENV_PRGFLS_LEN;
				}
			}
		}
		break;
	}
}
