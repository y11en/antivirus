// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  02 February 2006,  14:19 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- AVP manager
// Sub project -- DTReg
// Purpose     -- secure registry unterface based on DataTree technology
// Author      -- petrovitch
// File Name   -- dtreg.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Registry_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "dtreg.h"
//! #include "dtreg.h"
// AVP Prague stamp end



#include <Prague/iface/e_ktrace.h>
#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>
#include <m_utils.h>
#include <Serialize/KLDTSer.h>
#include <hashutil/hashutil.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_dtreg.h>
// AVP Prague stamp end



#define APP           100
#define KEY_CRC       MAKE_AVP_PID( 0xffff, APP, avpt_dword, 0 )
#define KEY_KEY_NUM   MAKE_AVP_PID( 0xfffe, APP, avpt_dword, 0 )
#define KEY_VAL_NUM   MAKE_AVP_PID( 0xfffd, APP, avpt_dword, 0 )
//#define KEY_FIRST_ENT MAKE_AVP_PID( 2,      APP, avpt_dword, 0 )

static tCHAR  g_root_name[] = "root key";

#define PROLOG()                     \
  e = errOK;                         \
  d = _this->data;                   \
  if ( p_key == cRegRoot ) {         \
    if ( 0 == (k=d->m_root) ) {      \
      e = errOBJECT_NOT_INITIALIZED; \
      goto exit;                     \
    }                                \
  }                                  \
  else if ( 0 == (k=(HDATA)p_key) ) {\
    e = errPARAMETER_INVALID;        \
    goto exit;                       \
  }


tBOOL   pr_call find_key_by_handle( HDATA data, HDATA key );
HDATA   pr_call add_key( HDATA data, const tCHAR* kname );
HPROP   pr_call add_val( HDATA data, const tCHAR* vname, tTYPE_ID type );
HPROP   pr_call get_entry( HPROP pcrc, HPROP* pname );
tUINT   pr_call get_val_count( HDATA key, tDWORD* new_id );
HPROP   pr_call get_first_val( HDATA pcrc );
HPROP   pr_call get_next_val( HPROP pcrc );
tBOOL   pr_call check_entry( HPROP pcrc, tDWORD crc, HPROP pname, tSTRING name );
tERROR  pr_call check_type( tTYPE_ID type, AVP_TYPE* tv, tPTR pValue, tDWORD size, AVP_size_t* rval, AVP_dword* used );
tERROR  pr_call conv_type( AVP_TYPE tv, tTYPE_ID* type );
tERROR  pr_call get_key_name( HDATA k, tDWORD* out_size, tSTRING name_buff, tDWORD size, tBOOL bFullName );
tERROR  pr_call get_str( tSTRING s, tDWORD* out_size, tCHAR* buffer, tDWORD size );
tERROR  pr_call prove_right_slash( hOBJECT obj, tCHAR** str, tBOOL in_place );
#ifdef _DEBUG
	tERROR  pr_call check_tree( HDATA tree );
#endif  

    
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Registry interface implementation
// Short comments -- access to storage like registry
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Registry. Inner data structure

typedef struct tag_RegData {
	HDATA   m_tree;             // data tree handle
	HDATA   m_root;             // current root key
	tSTRING m_root_path;        // path to the current root key
	tDWORD  m_root_path_size;   // --
	tBOOL   m_ro;               // object is in "read only" state
	tBOOL   m_save_on_close;    // save changes on close
	hIO     m_base;             // base IO object
	tBOOL   m_modified;         // data tree modified
	tBOOL   m_obj_new;          // object must be formated
	tSTRING m_file_name;        // storage file name
	tCHAR*  m_convert_buff;			//
	tUINT   m_convert_buff_len;	//
	tBOOL   m_init_from_base;   // init from base if true
	tDWORD  m_case_sensitive;   // --
} RegData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Registry {
	const iRegistryVtbl* vtbl; // pointer to the "Registry" virtual table
	const iSYSTEMVTBL*   sys;  // pointer to the "SYSTEM" virtual table
	RegData*             data; // pointer to the "Registry" data structure
} *hi_Registry;

// AVP Prague stamp end



tERROR        pr_call open_branch( RegData* d, HDATA root, HDATA* ret, const tCHAR* path, tBOOL create_if_not_exist, tBOOL ro );
HDATA         pr_call find_key( RegData* d, HDATA root, const tCHAR* kname, tUINT size );
HPROP         pr_call find_val( RegData* d, HDATA data, const tCHAR* vname, tTYPE_ID* type, HPROP* pcrc, HPROP* pname );
const tCHAR*  pr_call my_strchr( const tCHAR* str, tINT ch );
tINT          pr_call my_strnicmp( const tCHAR* s1, const tCHAR* s2, tUINT s );
//const tCHAR*  pr_call my_strlwr( hi_Registry reg, const tCHAR* str );
tERROR        pr_call replace_str( hi_Registry reg, tSTRING* str_new, tSTRING* str_old, tDWORD* outsize, tSTRING buffer, tDWORD size );
tERROR        pr_call replace_root_str( hi_Registry _this, HDATA new_root );
tERROR        pr_call open_base_by_name( hi_Registry _this );


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Registry_ObjectInit( hi_Registry _this );
tERROR pr_call Registry_ObjectInitDone( hi_Registry _this );
tERROR pr_call Registry_ObjectPreClose( hi_Registry _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Registry_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        Registry_ObjectInit,
	(tIntFnObjectInitDone)    Registry_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Registry_ObjectPreClose,
	(tIntFnObjectClose)       NULL,
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
typedef   tERROR (pr_call *fnpRegistry_OpenKeyByIndex)      ( hi_Registry _this, tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev ); // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning;
typedef   tERROR (pr_call *fnpRegistry_OpenNextKey)         ( hi_Registry _this, tRegKey* result, tRegKey key, tBOOL bClosePrev ); // -- opens next key on the same level;
typedef   tERROR (pr_call *fnpRegistry_CloseKey)            ( hi_Registry _this, tRegKey key ); // -- closes the key;
typedef   tERROR (pr_call *fnpRegistry_GetKeyName)          ( hi_Registry _this, tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ); // -- returns name of the key;
typedef   tERROR (pr_call *fnpRegistry_GetKeyNameByIndex)   ( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ); // -- returns name of the subkey by index;
typedef   tERROR (pr_call *fnpRegistry_GetValue)            ( hi_Registry _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size ); // -- gets key value, returns byte length of the value put in the buffer;
typedef   tERROR (pr_call *fnpRegistry_GetValueByIndex)     ( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ); // -- gets key value, returns byte length of the value put in the buffer;
typedef   tERROR (pr_call *fnpRegistry_GetValueNameByIndex) ( hi_Registry _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size ); // -- gets key name, returns byte length of the name put in the buffer;
typedef   tERROR (pr_call *fnpRegistry_SetValue)            ( hi_Registry _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ); // -- set key value. Creates key if it is not exist and parameter bCreateIfNotExist is true. Returns warning errKEY_NOT_EXISTS in this case;
typedef   tERROR (pr_call *fnpRegistry_SetValueByIndex)     ( hi_Registry _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ); // -- set key value, creates key if it is not exist and return warning errKEY_NOT_EXISTS in this case;
typedef   tERROR (pr_call *fnpRegistry_GetKeyCount)         ( hi_Registry _this, tDWORD* result, tRegKey key ); // -- gets subkey count;
typedef   tERROR (pr_call *fnpRegistry_GetValueCount)       ( hi_Registry _this, tDWORD* result, tRegKey key ); // -- gets subkey count;
typedef   tERROR (pr_call *fnpRegistry_DeleteKey)           ( hi_Registry _this, tRegKey key, const tCHAR* szSubKeyName ); // -- deletes the key;
typedef   tERROR (pr_call *fnpRegistry_DeleteKeyByInd)      ( hi_Registry _this, tRegKey key, tDWORD index ); // -- deletes the key;
typedef   tERROR (pr_call *fnpRegistry_DeleteValue)         ( hi_Registry _this, tRegKey key, const tCHAR* szValueName ); // -- deletes the key value;
typedef   tERROR (pr_call *fnpRegistry_DeleteValueByInd)    ( hi_Registry _this, tRegKey key, tDWORD index ); // -- deletes the key value;
typedef   tERROR (pr_call *fnpRegistry_Clean)               ( hi_Registry _this );            // -- clean the object contained registry data from the root key;
typedef   tERROR (pr_call *fnpRegistry_CopyKey)             ( hi_Registry _this, tRegKey src_key, hREGISTRY destination, tRegKey dest_key ); // -- copy key to another hREGISTRY;
typedef   tERROR (pr_call *fnpRegistry_CopyKeyByName)       ( hi_Registry _this, const tCHAR* src, hREGISTRY dst );
typedef   tERROR (pr_call *fnpRegistry_SetRootKey)          ( hi_Registry _this, tRegKey root, tBOOL close_prev );
typedef   tERROR (pr_call *fnpRegistry_SetRootStr)          ( hi_Registry _this, const tCHAR* root, tBOOL close_prev );
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
tERROR pr_call Registry_CopyKey( hi_Registry _this, tRegKey src_key, hREGISTRY destination, tRegKey dest_key );
tERROR pr_call Registry_CopyKeyByName( hi_Registry _this, const tCHAR* src, hREGISTRY dst );
tERROR pr_call Registry_SetRootKey( hi_Registry _this, tRegKey root, tBOOL close_prev );
tERROR pr_call Registry_SetRootStr( hi_Registry _this, const tCHAR* root, tBOOL close_prev );
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
tERROR pr_call Registry_PROP_set_save_on_close( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_get_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_set_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_set_base( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_get_name( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Registry_PROP_set_name( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
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
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "secure registry unterface based on DataTree technology", 55 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_RO, RegData, m_ro, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgMAX_NAME_LEN, ((tDWORD)(cMAX_SHORT_INT)) )
	mLOCAL_PROPERTY_BUF( pgCLEAN, RegData, m_obj_new, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY( pgSAVE_RESULTS_ON_CLOSE, RegData, m_save_on_close, cPROP_BUFFER_READ, NULL, Registry_PROP_set_save_on_close )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgROOT_POINT, Registry_PROP_get_root, Registry_PROP_set_root )
	mLOCAL_PROPERTY_BUF( pgROOT_KEY, RegData, m_root, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY( pgOBJECT_BASED_ON, RegData, m_base, cPROP_BUFFER_READ | cPROP_WRITABLE_ON_INIT, NULL, Registry_PROP_set_base )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, Registry_PROP_get_name, Registry_PROP_set_name )
	mLOCAL_PROPERTY_BUF( plINIT_FROM_BASE, RegData, m_init_from_base, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plCASE_SENSITIVE_NAMES, RegData, m_case_sensitive, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
    PID_DTREG,                              // plugin identifier
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
  tERROR e;
  PR_TRACE_A0( _this, "Enter *GET* method Registry_PROP_get_root for property pgROOT_POINT" );
  e = get_str( _this->data->m_root_path, out_size, buffer, size );
  PR_TRACE_A2( _this, "Leave *GET* method Registry_PROP_get_root for property pgROOT_POINT, ret tDWORD = %u(size), error = 0x%08x", *out_size, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_root )
// Interface "Registry". Method "Set" for property(s):
//  -- ROOT_POINT
tERROR pr_call Registry_PROP_set_root( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR e;
	PR_TRACE_A0( _this, "Enter *SET* method set_root for property pgROOT_POINT" );

	e = replace_str( _this, &_this->data->m_root_path, 0, out_size, buffer, size );
	if ( PR_SUCC(e) )
		prove_right_slash( (hOBJECT)_this, &_this->data->m_root_path, cTRUE );

	PR_TRACE_A2( _this, "Leave *SET* method set_root for property pgROOT_POINT, ret tDWORD = %u(size), %terr", *out_size, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_save_on_close )
// Interface "Registry". Method "Set" for property(s):
//  -- SAVE_RESULTS_ON_CLOSE
tERROR pr_call Registry_PROP_set_save_on_close( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	RegData* d;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method ObjectPreClose for property pgSAVE_RESULTS_ON_CLOSE" );

	d = _this->data;
	if ( !(*(tBOOL*)buffer) ) {
		d->m_save_on_close = cFALSE;
		*out_size = sizeof(tBOOL);
	}
	else if ( d->m_base ) {
		d->m_save_on_close = cTRUE;
		*out_size = sizeof(tBOOL);
	}
	else if ( !CALL_SYS_PropertyGetBool(_this,psOBJECT_OPERATIONAL) ) {
		d->m_save_on_close = cTRUE;
		*out_size = sizeof(tBOOL);
	}
	else {
		error = errPROPERTY_RDONLY;
		*out_size = 0;
	}

	PR_TRACE_A2( _this, "Leave *SET* method ObjectPreClose for property pgSAVE_RESULTS_ON_CLOSE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// ---
tERROR pr_call replace_root_str( hi_Registry _this, HDATA new_root ) {
  tERROR e;
  tDWORD new_size;
	RegData* d = _this->data;

  if ( PR_FAIL(e=get_key_name(new_root,&new_size,0,0,cTRUE)) )
		;
  else if ( d->m_root_path_size >= new_size )
		e = get_key_name( new_root, &new_size, d->m_root_path, new_size, cTRUE );
	else if ( PR_SUCC(e=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->m_root_path,d->m_root_path,new_size)) )
		e = get_key_name( new_root, &d->m_root_path_size, d->m_root_path, new_size, cTRUE );
	return e;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_root_key )
// Interface "Registry". Method "Set" for property(s):
//  -- ROOT_POINT_BY_KEY
tERROR pr_call Registry_PROP_set_root_key( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR   e;
  RegData* d;
  PR_TRACE_A0( _this, "Enter *SET* method ObjectPreClose for property pgROOT_POINT_BY_KEY" );
	
  *out_size = 0;
  d = _this->data;
  if ( !CALL_SYS_PropertyGetBool(_this,psOBJECT_OPERATIONAL) )
    e = errOBJECT_NOT_INITIALIZED;
	else if ( d->m_root == *(HDATA*)buffer ) {
		*out_size = sizeof(HDATA);
		e = errOK;
	}
  else if ( (d->m_tree != *(HDATA*)buffer) && !find_key_by_handle(d->m_tree,*(HDATA*)buffer) )
		e = errPARAMETER_INVALID;
  else {
		*out_size = sizeof(HDATA);
		replace_root_str( _this, d->m_root = *(HDATA*)buffer );
		e = errOK;
  }
	
  PR_TRACE_A2( _this, "Leave *SET* method ObjectPreClose for property pgROOT_POINT_BY_KEY, ret tDWORD = %u(size), %terr", *out_size, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_base )
// Interface "Registry". Method "Set" for property(s):
//  -- OBJECT_BASED_ON
tERROR pr_call Registry_PROP_set_base( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	RegData* d;
	hOBJECT  base;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method ObjectPreClose for property pgOBJECT_BASED_ON" );

	d = _this->data;
	base = *(hOBJECT*)buffer;
	if ( !base || PR_SUCC(CALL_SYS_ObjectCheck(_this,base,IID_IO,PID_ANY,SUBTYPE_ANY,cFALSE)) ) {
		d->m_base = (hIO)base;
		if ( base )
			d->m_init_from_base = cTRUE;
		*out_size = sizeof(hIO);
	}
	else {
		error = errOBJECT_INCOMPATIBLE;
		*out_size = 0;
	}

	PR_TRACE_A2( _this, "Leave *SET* method ObjectPreClose for property pgOBJECT_BASED_ON, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_name )
// Interface "Registry". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR pr_call Registry_PROP_get_name( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR e;
	PR_TRACE_A0( _this, "Enter *GET* method Registry_PROP_get_name for property pgOBJECT_NAME" );
  e = get_str( _this->data->m_file_name, out_size, buffer, size );
	PR_TRACE_A2( _this, "Leave *GET* method Registry_PROP_get_name for property pgOBJECT_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_name )
// Interface "Registry". Method "Set" for property(s):
//  -- OBJECT_NAME
tERROR pr_call Registry_PROP_set_name( hi_Registry _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter *SET* method Registry_PROP_set_name for property pgOBJECT_NAME" );

  d = _this->data;
  e = replace_str( _this, &d->m_file_name, 0, out_size, buffer, size );
  if ( PR_SUCC(e) ) {
    tBOOL val = d->m_file_name && *d->m_file_name;
		if ( val )
			d->m_init_from_base = cTRUE;
    CALL_SYS_PropertyInitialized( _this, pgOBJECT_BASED_ON, val );
  }

	PR_TRACE_A2( _this, "Leave *SET* method Registry_PROP_set_name for property pgOBJECT_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, e );
	return e;
}
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
	RegData* d;
  hOBJECT dad;
  PR_TRACE_A0( _this, "Enter Registry::ObjectInit method" );

	d = _this->data;
  dad = CALL_SYS_ParentGet( _this, IID_ANY );
  if ( dad && PR_SUCC(CALL_SYS_ObjectCheck(dad,dad,IID_IO,PID_ANY,SUBTYPE_ANY,cFALSE)) ) {
    d->m_base = (hIO)dad;
    CALL_SYS_PropertyInitialized( _this, pgOBJECT_BASED_ON, cTRUE );
		d->m_init_from_base = cTRUE;
  }
  d->m_ro = cTRUE;
	d->m_save_on_close = cFALSE;
	//d->m_case_sensitive = cTRUE;

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
  tERROR   e = errOK;
  RegData* d;
  tDWORD   ap;
  tPTR     ptr = 0;
  tQWORD   size;
  tDWORD   read;
  PR_TRACE_A0( _this, "Enter Registry::ObjectInitDone method" );

  d = _this->data;

  if ( d->m_obj_new && d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( !d->m_base && !d->m_file_name && (d->m_save_on_close || d->m_init_from_base) )
    e = errOBJECT_NOT_INITIALIZED;
  else if ( !d->m_base && d->m_file_name && d->m_init_from_base )
		e = open_base_by_name( _this );
	else if ( d->m_base && !d->m_ro && d->m_save_on_close && !(fACCESS_WRITE & (ap=CALL_SYS_PropertyGetDWord(d->m_base,pgOBJECT_ACCESS_MODE))) )
    e = CALL_SYS_PropertySetDWord( d->m_base, pgOBJECT_ACCESS_MODE, ap|fACCESS_WRITE );

  if ( PR_FAIL(e) ) {
		if ( e != errACCESS_DENIED )
			e = errOBJECT_NOT_INITIALIZED;
	}
  else if ( d->m_obj_new ) {
    d->m_tree = add_key( 0, g_root_name );
    if ( !d->m_tree )
      e = errNOT_ENOUGH_MEMORY;
  }
	else if ( d->m_init_from_base ) {
		if ( !d->m_base )
			e = errOBJECT_CANNOT_BE_INITIALIZED;
		else if ( PR_FAIL(e=CALL_IO_GetSize(d->m_base,&size,IO_SIZE_TYPE_EXPLICIT)) )
			;
		else if ( PR_FAIL(e=CALL_SYS_ObjHeapAlloc(_this,&ptr,(tDWORD)size)) || !ptr )
			;
		else if ( PR_FAIL(e=CALL_IO_SeekRead(d->m_base,&read,0,ptr,(tDWORD)size)) )
			;
		else if ( read < size )
			e = errOBJECT_READ;
		else if ( !KLDT_DeserializeFromMemoryUsingSWM(ptr,read,&d->m_tree) )
			e = errOBJECT_INCOMPATIBLE;
	}

  if ( ptr )
    CALL_SYS_ObjHeapFree( _this, ptr );

	if ( !d->m_tree ) {
		d->m_root = d->m_tree = add_key( 0, g_root_name );
		if ( !d->m_tree )
			e = errNOT_ENOUGH_MEMORY;
	}

  if ( PR_SUCC(e) ) {
    if ( d->m_root_path && *d->m_root_path )
			e = open_branch( d, d->m_tree, &d->m_root, d->m_root_path, d->m_obj_new, d->m_ro );
		else
			d->m_root = d->m_tree;
		if ( PR_SUCC(e) && d->m_obj_new )
			e = Registry_Clean( _this );
  }

  #ifdef _DEBUG
		if ( PR_SUCC(e) && !d->m_obj_new && d->m_init_from_base && d->m_root && PR_FAIL(check_tree(d->m_root)) )
			e = warnOBJECT_DATA_CORRUPTED;
  #endif

  PR_TRACE_A1( _this, "Leave Registry::ObjectInitDone method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call Registry_ObjectPreClose( hi_Registry _this ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::ObjectPreClose method" );

  d = _this->data;
	e = Registry_Flush( _this, cFALSE );

	if ( d->m_tree ) {
		DATA_Remove( d->m_tree, 0 );
		d->m_tree = 0;
	}

	if ( d->m_file_name )
		CALL_SYS_ObjHeapFree( _this, d->m_file_name );
	
	if ( d->m_root_path )
		CALL_SYS_ObjHeapFree( _this, d->m_root_path );

	if ( d->m_convert_buff )
		CALL_SYS_ObjHeapFree( _this, d->m_convert_buff );

	PR_TRACE_A1( _this, "Leave Registry::ObjectPreClose method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenKey )
// Extended method comment
//    if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR pr_call Registry_OpenKey( hi_Registry _this, tRegKey* result, tRegKey p_key, const tCHAR* p_szSubKeyName, tBOOL p_bCreateIfNotExist ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
	tCHAR*   s;
  PR_TRACE_A0( _this, "Enter Registry::OpenKey method" );

  PROLOG();
	//p_szSubKeyName = my_strlwr( _this, p_szSubKeyName );
	s = (tCHAR*)p_szSubKeyName;
	e = prove_right_slash( (hOBJECT)_this, &s, cFALSE );
	if ( PR_SUCC(e) )
		e = open_branch( d, k, &k, s, p_bCreateIfNotExist, d->m_ro );
	else
		k = 0;

	if ( s != p_szSubKeyName )
		CALL_SYS_ObjHeapFree( _this, s );

  if ( result )
    *result = (tRegKey)k;
exit:;
  PR_TRACE_A2( _this, "Leave Registry::OpenKey method, ret tRegKey = %p, error = 0x%08x", k, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenKeyByIndex )
// Extended method comment
//    if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR pr_call Registry_OpenKeyByIndex( hi_Registry _this, tRegKey* result, tRegKey p_key, tDWORD index, tBOOL bClosePrev ) {
  tERROR    e;
  HDATA     k;
  RegData*  d;
  tUINT     i;
  PR_TRACE_A0( _this, "Enter Registry::OpenKeyByIndex method" );
  PROLOG();

  if ( index < 10 )
    e = errOK;
  else {
    AVP_dword n;
    HPROP     p = DATA_Find_Prop( k, 0, KEY_KEY_NUM );
    if ( !p || (sizeof(n) != PROP_Get_Val(p,&n,sizeof(n))) )
      e = errUNEXPECTED;
    else if ( index >= n )
      e = errKEY_INDEX_OUT_OF_RANGE;
    else
      e = errOK;
  }

  if ( PR_SUCC(e) ) {
    k = DATA_Get_First( k, 0 );
    for( i=0; k && (i<index); i++ )
      k = DATA_Get_Next( k, 0 );
    if ( k )
      e = errOK;
    else
      e = errKEY_INDEX_OUT_OF_RANGE;
  }
  else
    k = 0;
exit:;
  if ( result )
    *result = (tRegKey)k;
  PR_TRACE_A2( _this, "Leave Registry::OpenKeyByIndex method, ret tRegKey = %p, error = 0x%08x", k, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenNextKey )
// Parameters are:
tERROR pr_call Registry_OpenNextKey( hi_Registry _this, tRegKey* result, tRegKey p_key, tBOOL bClosePrev ) {
  tERROR   e;
  HDATA    k;
  RegData* d;
  PR_TRACE_A0( _this, "Enter Registry::OpenNextKey method" );

  PROLOG();
  k = DATA_Get_Next( k, 0 );
  if ( k )
    e = errOK;
  else
    e = errKEY_NOT_FOUND;

exit:;
     if ( result )
    *result = (tRegKey)k;
  PR_TRACE_A2( _this, "Leave Registry::OpenNextKey method, ret tRegKey = %p, error = 0x%08x", k, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CloseKey )
// Parameters are:
tERROR pr_call Registry_CloseKey( hi_Registry _this, tRegKey key ) {
  tERROR   e;
  PR_TRACE_A0( _this, "Enter Registry::CloseKey method" );
  if ( key == cRegRoot )
    e = errKEY_CANNOT_BE_CLOSED;
  else
    e = errOK;
  PR_TRACE_A1( _this, "Leave Registry::CloseKey method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyName )
// Parameters are:
//   "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR pr_call Registry_GetKeyName( hi_Registry _this, tDWORD* result, tRegKey p_key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  tDWORD   s = 0;
  PR_TRACE_A0( _this, "Enter Registry::GetKeyName method" );

  PROLOG();
  e = get_key_name( k, &s, name_buff, size, bFullName );
exit:;
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
tERROR pr_call Registry_GetKeyNameByIndex( hi_Registry _this, tDWORD* result, tRegKey p_key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) {
  tERROR   e;
  HDATA    k;
  RegData* d;
  tUINT    i;
  tDWORD   s = 0;
  PR_TRACE_A0( _this, "Enter Registry::GetKeyNameByIndex method" );

  PROLOG();
  k = DATA_Get_First( k, 0 );
  for( i=0; k && (i<index); i++ )
    k = DATA_Get_Next( k, 0 );

  if ( !k ) {
    s = 0;
    e = errKEY_INDEX_OUT_OF_RANGE;
  }
  else
    e = get_key_name( k, &s, name_buff, size, bFullName );

exit:;
  if ( result )
    *result = s;
  PR_TRACE_A2( _this, "Leave Registry::GetKeyNameByIndex method, ret tDWORD = %u, error = 0x%08x", s, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValue )
// Parameters are:
tERROR pr_call Registry_GetValue( hi_Registry _this, tDWORD* result, tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size ) {
	tERROR   e;
	HDATA    k;
	HPROP    p;
	RegData* d;
	tDWORD   s = 0;
	PR_TRACE_A0( _this, "Enter Registry::GetValue method" );

	PROLOG();
	//p_szValueName = my_strlwr( _this, p_szValueName );
	p = find_val( d, k, p_szValueName, p_type, 0, 0 );
	if ( p ) {
		if( !p_size )
			p_pValue = NULL;

		s = PROP_Get_Val( p, p_pValue, p_size );
		if ( !s )
			e = errUNEXPECTED;
		else if ( !p_size )
			e = errOK;
		else if( p_size < s )
			e = errBUFFER_TOO_SMALL;
		else
			e = errOK;
	}
	else
		e = errKEY_NOT_FOUND;

exit:;
	if ( result )
		*result = s;
	PR_TRACE_A2( _this, "Leave Registry::GetValue method, ret tDWORD = %u, error = 0x%08x", s, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueByIndex )
// Parameters are:
tERROR pr_call Registry_GetValueByIndex( hi_Registry _this, tDWORD* result, tRegKey p_key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  HPROP    p;
  tDWORD   s = 0;
  PR_TRACE_A0( _this, "Enter Registry::GetValueByIndex method" );

  PROLOG();
  if ( index < 10 )
    e = errOK;
  else {
    AVP_dword n;
    p = DATA_Find_Prop( k, 0, KEY_VAL_NUM );
    if ( !p || (sizeof(n) != PROP_Get_Val(p,&n,sizeof(n))) )
      e = errUNEXPECTED;
    else if ( index >= n )
      e = errKEY_INDEX_OUT_OF_RANGE;
    else
      e = errOK;
  }

  if ( PR_SUCC(e) ) {
    tUINT i;
    HPROP pv;
    for( i=0,p=get_first_val(k); p && i<index; i++,p=get_next_val(p) )
      ;
    if ( p && (0 != (pv=get_entry(p,0))) ) {
      if ( type && PR_FAIL(e=conv_type(PROP_Get_Type(pv),type)) )
        ;
      else if ( 0 == (s=PROP_Get_Val(pv,pValue,size)) )
        e = errUNEXPECTED;
    }
    else
      e = errKEY_INDEX_OUT_OF_RANGE;
  }
exit:;
  if ( result )
    *result = s;
  PR_TRACE_A2( _this, "Leave Registry::GetValueByIndex method, ret tDWORD = %u, error = 0x%08x", s, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueNameByIndex )
// Parameters are:
tERROR pr_call Registry_GetValueNameByIndex( hi_Registry _this, tDWORD* result, tRegKey p_key, tDWORD index, tCHAR* name_buff, tDWORD size ) {
	tERROR   e;
	RegData* d;
	HDATA    k;
	HPROP    p;
	tDWORD   s = 0;
	PR_TRACE_A0( _this, "Enter Registry::GetValueNameByIndex method" );

	PROLOG();
	if ( index < 10 )
		e = errOK;
	else {
		AVP_dword n;
		p = DATA_Find_Prop( k, 0, KEY_VAL_NUM );
		if ( !p || (sizeof(n) != PROP_Get_Val(p,&n,sizeof(n))) )
			e = errUNEXPECTED;
		else if ( index >= n )
			e = errKEY_INDEX_OUT_OF_RANGE;
		else
			e = errOK;
	}

	if ( PR_SUCC(e) ) {
		tUINT i;
		HPROP pn;
		for( i=0,p=get_first_val(k); p && i<index; i++, p=get_next_val(p) )
			;
		if ( p && (0 != get_entry(p,&pn)) && pn ) {
			s = PROP_Get_Val( pn, name_buff, size );
			if ( !s )
				e = errUNEXPECTED;
		}
		else
			e = errKEY_INDEX_OUT_OF_RANGE;
	}
exit:;
	if ( result )
		*result = s;
	PR_TRACE_A2( _this, "Leave Registry::GetValueNameByIndex method, ret tDWORD = %u, error = 0x%08x", size, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetValue )
// Parameters are:
tERROR pr_call Registry_SetValue( hi_Registry _this, tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist ) {
	tERROR   e;
	RegData* d;
	HDATA    k;
	PR_TRACE_A0( _this, "Enter Registry::SetValue method" );

	PROLOG();
	if( !p_szValueName )
		e = errOK; // !!!!!!!!!!!!!!!!!!!!!!
	else if ( d->m_ro )
		e = errOBJECT_READ_ONLY;
	else {
		HPROP      p;
		AVP_TYPE   tv;
		AVP_size_t rval;
		AVP_dword  used;
		tBOOL      change_str_type = cFALSE;

		//p_szValueName = my_strlwr( _this, p_szValueName );
		p = find_val( d, k, p_szValueName, 0/*&ts*/, 0, 0 );
		if ( !p && !p_bCreateIfNotExist )
			e = errKEY_NOT_FOUND;
		else {
			e = check_type( p_type, &tv, p_pValue, p_size, &rval, &used );
			if ( PR_FAIL(e) )
				;
			else if ( p ) {
				AVP_TYPE ts = PROP_Get_Type( p );
				if ( ts != tv ) {
					change_str_type = ( ((ts==avpt_str) && (tv==avpt_wstr)) || ((ts==avpt_wstr) && (tv==avpt_str)) );
					if ( !change_str_type && used ) {
						AVP_size_t size = PROP_Get_Val( p, 0, 0 );
						if ( used > size )
							e = errPARAMETER_INVALID;
						else
							p_size = used;
					}
				}
			}
			else if ( 0 == (p=add_val(k,p_szValueName,tv)) )
				e = errNOT_ENOUGH_MEMORY;
			if ( PR_SUCC(e) ) {
				if ( p_size && (tv == avpt_wstr) )
					p_size /= sizeof(tWCHAR);
				if ( change_str_type ) {
					AVP_dword id   = PROP_Get_Id( p );
					AVP_dword oid  = GET_AVP_PID_ID( id );
					AVP_dword oapp = GET_AVP_PID_APP( id );

					AVP_dword nid = MAKE_AVP_PID( oid, oapp, tv, 0 );
					HPROP np = PROP_Replace( p, nid, rval, p_size );
					if ( np )
						p_size = PROP_Get_Val( np, 0, 0 );
					else
						p_size = 0;
				}
				else {
					p_size = PROP_Set_Val( p, rval, p_size );
					if ( p_size && (tv == avpt_wstr) )
						p_size *= sizeof(tWCHAR);
				}
				if ( !p_size )
					e = errUNEXPECTED;
				else
					d->m_modified = cTRUE;
			}
		}
	}
exit:;
	PR_TRACE_A1( _this, "Leave Registry::SetValue method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetValueByIndex )
// Parameters are:
tERROR pr_call Registry_SetValueByIndex( hi_Registry _this, tRegKey p_key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) {
	tERROR     e;
	RegData*   d;
	HDATA      k;
	AVP_TYPE   tv;
	AVP_size_t rval;
	AVP_dword  used;
	PR_TRACE_A0( _this, "Enter Registry::SetValueByIndex method" );

	PROLOG();
	if ( d->m_ro )
		e = errOBJECT_READ_ONLY;
	else if ( PR_FAIL(e=check_type(type,&tv,pValue,size,&rval,&used)) )
		;
	else {
		tUINT  i;
		HPROP  p, pv;

		for( i=0,p=get_first_val(k); p && (i<index); p=get_next_val(p), i++ )
			;
		if ( p && (0 != (pv=get_entry(p,0))) ) {
			AVP_TYPE ts = PROP_Get_Type( pv );
			e = errOK;
			if ( (ts != tv) && used ) {
				AVP_size_t size = PROP_Get_Val( pv, 0, 0 );
				if ( used > size )
					e = errPARAMETER_INVALID;
				else
					size = used;
			}
		}
		else if ( !bCreateIfNotExist || (index != i) ) // index must be key count+1 (or index==i, that's the same)
			e = errKEY_INDEX_OUT_OF_RANGE;
		else {
			tCHAR    name[50];
			tDWORD   second, ns;
			tDT      dt;

			Now( &dt );
			DTGet( (const tDT*)&dt, 0, 0, 0, 0, 0, &second, &ns );
			pr_sprintf( name, sizeof(name), "%04d_key_%08x_%08x", index, second, ns );
			pv = add_val( k, name, tv );
			if ( pv )
				e = errOK;
			else
				e = errUNEXPECTED;
		}
		if ( PR_SUCC(e) ) {
			if ( PROP_Set_Val(pv,rval,size) )
				d->m_modified = cTRUE;
			else
				e = errUNEXPECTED;
		}
	}
exit:;
	PR_TRACE_A1( _this, "Leave Registry::SetValueByIndex method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyCount )
// Parameters are:
tERROR pr_call Registry_GetKeyCount( hi_Registry _this, tDWORD* result, tRegKey p_key ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  HPROP    p;
  tDWORD   r = 0;
  PR_TRACE_A0( _this, "Enter Registry::GetKeyCount method" );

  PROLOG();
  p = DATA_Find_Prop( k, 0, KEY_KEY_NUM );
  if ( p && (sizeof(r) == PROP_Get_Val(p,&r,sizeof(r))) )
    e = errOK;
  else
    e = errUNEXPECTED;
exit:;
  if ( result )
    *result = r;
  PR_TRACE_A2( _this, "Leave Registry::GetKeyCount method, ret tDWORD = %u, error = 0x%08x", r, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueCount )
// Parameters are:
tERROR pr_call Registry_GetValueCount( hi_Registry _this, tDWORD* result, tRegKey p_key ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  HPROP    p;
  tDWORD   r = 0;
  PR_TRACE_A0( _this, "Enter Registry::GetValueCount method" );

  PROLOG();
  p = DATA_Find_Prop( k, 0, KEY_VAL_NUM );
  if ( p && (sizeof(r) == PROP_Get_Val(p,&r,sizeof(r))) )
    e = errOK;
  else
    e = errUNEXPECTED;
exit:;
  if ( result )
    *result = r;
  PR_TRACE_A2( _this, "Leave Registry::GetValueCount method, ret tDWORD = %u, error = 0x%08x", r, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteKey )
// Parameters are:
tERROR pr_call Registry_DeleteKey( hi_Registry _this, tRegKey p_key, const tCHAR* p_szSubKeyName ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  PR_TRACE_A0( _this, "Enter Registry::DeleteKey method" );

  PROLOG();

  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else if ( /*p_szSubKeyName=my_strlwr(_this,p_szSubKeyName),*/ PR_FAIL(e=open_branch(d,k,&k,p_szSubKeyName,cFALSE,cTRUE)) )
    ;
  else if ( !DATA_Remove(k,0) )
    e = errUNEXPECTED;
  else if ( (d->m_modified=cTRUE), (k == d->m_tree) )
    d->m_tree = DATA_Add( 0, 0, AVP_PID_NOTHING, 0, 0 );
exit:;
  PR_TRACE_A1( _this, "Leave Registry::DeleteKey method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteKeyByInd )
// Parameters are:
tERROR pr_call Registry_DeleteKeyByInd( hi_Registry _this, tRegKey p_key, tDWORD index ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  tUINT    i;
  HPROP    p;
  tDWORD   count;
  PR_TRACE_A0( _this, "Enter Registry::DeleteKeyByInd method" );

  PROLOG();
  p = DATA_Find_Prop( k, 0, KEY_KEY_NUM );
  if ( !p || (sizeof(count) != PROP_Get_Val(p,&count,sizeof(count))) )
    e = errUNEXPECTED;
  else if ( index >= count )
    e = errKEY_INDEX_OUT_OF_RANGE;
  else {
    k = DATA_Get_First( k, 0 );
    for( i=0; k && (i<index); i++ )
      k = DATA_Get_Next( k, 0 );
    if ( !k )
      e = errKEY_INDEX_OUT_OF_RANGE;
    else if ( !DATA_Remove(k,0) )
      e = errUNEXPECTED;
    else {
      d->m_modified = cTRUE;
      e = errOK;
      PROP_Set_Val( p, --count, sizeof(count) );
    }
  }
exit:;
  PR_TRACE_A1( _this, "Leave Registry::DeleteKeyByInd method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteValue )
// Parameters are:
tERROR pr_call Registry_DeleteValue( hi_Registry _this, tRegKey p_key, const tCHAR* p_szValueName ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  HPROP    pnum;
  tDWORD   count;
  PR_TRACE_A0( _this, "Enter Registry::DeleteValue method" );

  PROLOG();
  pnum = DATA_Find_Prop( k, 0, KEY_VAL_NUM );
  if ( !pnum || (sizeof(count) != PROP_Get_Val(pnum,&count,sizeof(count))) )
    e = errUNEXPECTED;
  else {
    HPROP pcrc, pname, pval;
		//p_szValueName = my_strlwr( _this, p_szValueName );
    pval = find_val( d, k, p_szValueName, 0, &pcrc, &pname );
    if ( !pval )
      e = errKEY_INDEX_OUT_OF_RANGE;
    else {
      tBOOL del_crc  = DATA_Remove_Prop_H( k, 0, pcrc );
      tBOOL del_name = DATA_Remove_Prop_H( k, 0, pname );
      tBOOL del_val  = DATA_Remove_Prop_H( k, 0, pval );
      if ( del_crc && del_name && del_val ) {
        e = errOK;
        d->m_modified = cTRUE;
        PROP_Set_Val( pnum, --count, sizeof(count) );
      }
      else
        e = errUNEXPECTED;
    }
  }
exit:;
  PR_TRACE_A1( _this, "Leave Registry::DeleteValue method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteValueByInd )
// Parameters are:
tERROR pr_call Registry_DeleteValueByInd( hi_Registry _this, tRegKey p_key, tDWORD index ) {
  tERROR   e;
  RegData* d;
  HDATA    k;
  tUINT    i;
  HPROP    pnum;
  tDWORD   count;
  PR_TRACE_A0( _this, "Enter Registry::DeleteValueByInd method" );

  PROLOG();
  pnum = DATA_Find_Prop( k, 0, KEY_VAL_NUM );
  if ( !pnum || (sizeof(count) != PROP_Get_Val(pnum,&count,sizeof(count))) )
    e = errUNEXPECTED;
  else if ( index >= count )
    e = errKEY_INDEX_OUT_OF_RANGE;
  else {
    HPROP pcrc, pname, pval;
    for( i=0,pcrc=get_first_val(k); pcrc && (i<index); ++i, pcrc=get_next_val(pcrc) )
      ;
    if ( !pcrc || (0 == (pval=get_entry(pcrc,&pname))) || !pname )
      e = errKEY_INDEX_OUT_OF_RANGE;
    else {
      tBOOL del_crc  = DATA_Remove_Prop_H( k, 0, pcrc );
      tBOOL del_name = DATA_Remove_Prop_H( k, 0, pname );
      tBOOL del_val  = DATA_Remove_Prop_H( k, 0, pval );
      if ( del_crc && del_name && del_val ) {
        e = errOK;
        d->m_modified = cTRUE;
        PROP_Set_Val( pnum, --count, sizeof(count) );
      }
      else
        e = errUNEXPECTED;
    }
  }
exit:;
  PR_TRACE_A1( _this, "Leave Registry::DeleteValueByInd method, ret tERROR = 0x%08x", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clean )
// Parameters are:
tERROR pr_call Registry_Clean( hi_Registry _this ) {
	tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::Clean method" );

  d = _this->data;
  if ( d->m_ro )
    e = errOBJECT_READ_ONLY;
  else {
    HDATA cd, nd;
    HPROP cp, np;
    for( cd=DATA_Get_First(d->m_root,0); cd; cd=nd ) {
      nd = DATA_Get_Next( cd, 0 );
      DATA_Remove( cd, 0 );
    }
    for( cp=get_first_val(d->m_root); cp; cp=np ) {
      HPROP pval, pname;
      np = get_next_val( cp );
      pval = get_entry( cp, &pname );
      if ( pval )
        DATA_Remove_Prop_H( d->m_root, 0, pval );
      if ( pname )
        DATA_Remove_Prop_H( d->m_root, 0, pname );
      DATA_Remove_Prop_H( d->m_root, 0, cp );
    }
    e = errOK;
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
tERROR pr_call Registry_CopyKey( hi_Registry _this, tRegKey p_key, hREGISTRY destination, tRegKey dest_key ) {
  tERROR err;
  PR_TRACE_A0( _this, "Enter Registry::CopyKey method" );

  err = Reg_CopyKey( (hREGISTRY)_this, p_key, destination, dest_key );

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

  err = Registry_OpenKey( _this, &key, cRegRoot, src, cFALSE );
  if ( PR_SUCC(err) )
    err = Reg_CopyKey( (hREGISTRY)_this, key, dst, cRegRoot );

  PR_TRACE_A1( _this, "Leave Registry::CopyKeyByName method, ret %terr", err );
  return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetRootKey )
// Parameters are:
tERROR pr_call Registry_SetRootKey( hi_Registry _this, tRegKey p_root, tBOOL p_close_prev ) {
  tERROR   e;
  RegData* d;
	PR_TRACE_A0( _this, "Enter Registry::SetRootKey method" );

  d = _this->data;
	if ( d->m_root == (HDATA)p_root )
		e = errOK;
  else if ( (d->m_tree != (HDATA)p_root) && !find_key_by_handle(d->m_tree,(HDATA)p_root) )
		e = errPARAMETER_INVALID;
  else {
		replace_root_str( _this, d->m_root = (HDATA)p_root );
		e = errOK;
  }

	PR_TRACE_A1( _this, "Leave Registry::SetRootKey method, ret %terr", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetRootStr )
// Parameters are:
tERROR pr_call Registry_SetRootStr( hi_Registry _this, const tCHAR* p_root, tBOOL p_close_prev ) {
  tERROR   e;
  RegData* d;
	HDATA    new_root;
	PR_TRACE_A0( _this, "Enter Registry::SetRootStr method" );

  d = _this->data;
  if ( p_root && ((*p_root == '\\') || (*p_root == '/')) )
    p_root++;
	
  if ( 0 == (new_root=find_key(d,d->m_tree,p_root,0)) )
		e = errKEY_NOT_FOUND;
	else {
    e = errOK;
		replace_root_str( _this, d->m_root = new_root );
	}
	
	PR_TRACE_A1( _this, "Leave Registry::SetRootStr method, ret %terr", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Parameters are:
tERROR pr_call Registry_Flush( hi_Registry _this, tBOOL p_force ) {

  RegData* d;
	tERROR   e = errOK;
	PR_TRACE_A0( _this, "Enter Registry::Flush method" );

  d = _this->data;
  if ( !p_force && (!d->m_modified || !d->m_save_on_close) )
    e = errOK;
	else if ( !d->m_base && PR_FAIL(e=open_base_by_name(_this)) )
		;
  else if ( d->m_tree ) {
    tPTR ptr = 0;
    AVP_dword size = KLDT_SerializeToMemoryUsingSWM( 0, 0, d->m_tree, "" );
    if ( !size )
      e = errUNEXPECTED;
    else if ( PR_FAIL(e=CALL_SYS_ObjHeapAlloc(_this,&ptr,size)) )
      ;
    else if ( !ptr )
      e = errNOT_ENOUGH_MEMORY;
    else if ( !KLDT_SerializeToMemoryUsingSWM(ptr,size,d->m_tree,"") )
      e = errUNEXPECTED;
		else {
			tDWORD access = CALL_SYS_PropertyGetDWord( d->m_base, pgOBJECT_ACCESS_MODE );
			if ( !(access&fACCESS_WRITE) && PR_FAIL(e=CALL_SYS_PropertySetDWord(d->m_base,pgOBJECT_ACCESS_MODE,access|fACCESS_WRITE)) )
				;
			else if ( PR_FAIL(e=CALL_IO_SeekWrite(d->m_base,0,0,ptr,size)) )
				;
		}
    if ( ((hOBJECT)_this) == CALL_SYS_ParentGet(d->m_base,IID_ANY) ) {
      CALL_SYS_ObjectClose( d->m_base );
      d->m_base = 0;
    }
    if ( ptr )
      CALL_SYS_ObjHeapFree( _this, ptr );
  }

	PR_TRACE_A1( _this, "Leave Registry::Flush method, ret %terr", e );
	return e;
}
// AVP Prague stamp end




// ---
tERROR pr_call open_branch( RegData* d, HDATA root, HDATA* ret, const tCHAR* path, tBOOL create_if_not_exist, tBOOL ro ) {
  tBOOL  loop = cTRUE;
  tBOOL  creating = cFALSE;
  tERROR err;
  
  if ( !path || !*path ) {
    *ret = root;
    return errOK;
  }
  
  if ( (*path == '\\') || (*path == '/') )
    path++;
  
  err = errOK;
  while( root && loop ) {
    tUINT        size;
    const tCHAR* end;
    
    end = my_strchr( path, '\\' );
    if ( !end )
      end = my_strchr( path, '/' );
    
    if ( end )
      size = (tUINT)(end - path);
    else {
      loop = cFALSE;
      size = (tUINT)strlen( path );
    }
    
    if ( creating ) {
      root = add_key( root, path );
      if ( !root )
        err = errNOT_ENOUGH_MEMORY;
    }
    else {
      HDATA next = find_key( d, root, path, size );
      if ( next )
        root = next;
      else if ( !create_if_not_exist ) {
        root = 0;
        err = errKEY_NOT_FOUND;
      }
      else if ( ro ) {
        root = 0;
        err = errOBJECT_READ_ONLY;
      }
      else {
        root = add_key( root, path );
        if ( !root )
          err = errNOT_ENOUGH_MEMORY;
        else
          creating = cTRUE;
      }
    }
    path += size + 1;
  }
  *ret = root;
  return err;
}



// ---
tBOOL pr_call find_key_by_handle( HDATA data, HDATA key ) {
	HDATA tmp;
	HDATA sub_key = DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
	for( tmp=sub_key; tmp; tmp=DATA_Get_Next(tmp,0) ) {
		if ( tmp == key )
			return cTRUE;
	}
	for( tmp=sub_key; tmp; tmp=DATA_Get_Next(tmp,0) ) {
		if ( find_key_by_handle(tmp,key) )
			return cTRUE;
	}
	return cFALSE;
}



// ---
HDATA pr_call find_key( RegData* d, HDATA data, const tCHAR* kname, tUINT size ) {
	tUINT crc;
  HDATA sub_key = DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
	if ( !size )
		size = (tUINT)strlen(kname);
	if ( d->m_case_sensitive )
		crc = iCountCRC32( size, kname, 0 );
  for( ; sub_key; sub_key=DATA_Get_Next(sub_key,0) ) {
    AVP_dword cid;
    AVP_char  name[260];
    HPROP prop;
		if ( d->m_case_sensitive ) {
			prop = DATA_Find_Prop( sub_key, 0, KEY_CRC );
			if ( !prop )
				continue;
			if ( sizeof(cid) != PROP_Get_Val(prop,&cid,sizeof(cid)) )
				continue;
			if ( cid != crc )
				continue;
		}
    if ( 0 == DATA_Get_Val(sub_key,0,AVP_PID_VALUE,&name,sizeof(name)) )
      continue;
    if ( my_strnicmp(kname,name,size) )
      continue;
    return sub_key;
  }
  return 0;
}



// ---
HDATA pr_call add_key( HDATA data, const tCHAR* kname ) {
  HDATA     ndata = 0;
  HPROP     prop = 0;
  AVP_dword num;
	AVP_dword crc = iCountCRC32str( kname );

  if ( !data )
    num = 0;
  else if ( 0 == (prop=DATA_Find_Prop(data,0,KEY_KEY_NUM)) )
    return 0;
  else if ( sizeof(num) != PROP_Get_Val(prop,&num,sizeof(num)) )
    return 0;

  if ( 0 == (ndata=DATA_Add(data,0,MAKE_AVP_PID(num,APP,avpt_str,0),(AVP_dword)kname,0)) )
    ;
  else if ( 
    !DATA_Add_Prop( ndata, 0, KEY_CRC,     crc, sizeof(crc)       ) ||
    !DATA_Add_Prop( ndata, 0, KEY_KEY_NUM, 0,   sizeof(AVP_dword) ) ||
    !DATA_Add_Prop( ndata, 0, KEY_VAL_NUM, 0,   sizeof(AVP_dword) )
  ) {
    DATA_Remove( ndata, 0 );
    ndata = 0;
  }
  else if ( prop )
    PROP_Set_Val( prop, ++num, sizeof(num) );

  return ndata;
}



// ---
HPROP pr_call find_val( RegData* d, HDATA data, const tCHAR* vname, tTYPE_ID* type, HPROP* pcrc, HPROP* pname ) {
  HPROP lpname;
  HPROP lpcrc;
	tUINT vcrc;

	if ( d->m_case_sensitive )
		vcrc = iCountCRC32str( vname );
  
  if ( !pcrc )
    pcrc = &lpcrc;
  if ( !pname )
    pname = &lpname;

  for( *pcrc=get_first_val(data); *pcrc; *pcrc=get_next_val(*pcrc) ) {
    AVP_dword ccrc;
    AVP_char  cname[260];
		AVP_dword sname;
    HPROP pval = get_entry( *pcrc, pname );
    if ( !pval || !*pname )
      return 0;
    if ( (sizeof(ccrc) != PROP_Get_Val(*pcrc,&ccrc,sizeof(ccrc))) || (d->m_case_sensitive && (ccrc != vcrc)) ) 
      continue;
    if ( (0 == (sname=PROP_Get_Val(*pname,cname,sizeof(cname)))) || my_strnicmp(cname,vname,sname) )
      continue;
    if ( type && PR_FAIL(conv_type(PROP_Get_Type(pval),type)) )
      return 0;
    return pval;
  }
  return 0;
}



// ---
HPROP pr_call add_val( HDATA data, const tCHAR* vname, tTYPE_ID type ) {
  AVP_dword num;
  HPROP     pval;
  HPROP     pcrc, pname;
  HPROP     prop = DATA_Find_Prop( data, 0, KEY_VAL_NUM );
	AVP_dword crc  = iCountCRC32str( vname );
	tUINT     id   = 0;
  
  if ( !prop )
    return 0;
  
	//if ( sizeof(num) != PROP_Get_Val(prop,&num,sizeof(num)) )
  //  return 0;
	num = get_val_count( data, &id );
  
  pval = 0;
  if ( 0 == (pcrc=DATA_Add_Prop(data,0,MAKE_AVP_PID(2*(id+1),APP,avpt_dword,0),crc,sizeof(crc))) ) 
    ;
  else if ( 0 == (pname=DATA_Add_Prop(data,0,MAKE_AVP_PID(2*(id+1),APP,avpt_str,0),(AVP_dword)vname,0)) )
    DATA_Remove_Prop_H( data, 0, pcrc );
  else if ( 0 == (pval=DATA_Add_Prop(data,0,MAKE_AVP_PID(2*(id+1)+1,APP,type,0),0,0)) ) {
    DATA_Remove_Prop_H( data, 0, pcrc );
    DATA_Remove_Prop_H( data, 0, pname );
  }
  else
    PROP_Set_Val( prop, ++num, sizeof(num) );
  
  return pval;
}



// ---
HPROP pr_call get_entry( HPROP prop, HPROP* pname ) {
  if ( pname )
    *pname = 0;
  if ( !prop )
    return 0;
  if ( 0 == (prop=PROP_Get_Next(prop)) ) // get to name
    return 0;
  if ( pname )
    *pname = prop;
  return PROP_Get_Next( prop ); // get to value
}



// ---
tUINT pr_call get_val_count( HDATA key, tDWORD* new_id ) {
	tUINT c;
	HPROP p;
	tUINT id = 0;
	for( c=0,p=get_first_val(key); p; ++c,p=get_next_val(p) ) {
		/* VIK 18.02.05
		if ( new_id && (id <= GET_AVP_PID_ID(p)) )
			id = GET_AVP_PID_ID(p) + 2;
		*/
		if ( new_id && (id <= GET_AVP_PID_ID(PROP_Get_Id(p))) )
			id = GET_AVP_PID_ID(PROP_Get_Id(p)) + 2;
	}
	if ( new_id )
		*new_id = id / 2;
	return c;
}


// ---
HPROP pr_call get_first_val( HDATA key ) {
  tUINT i;
	HPROP p = DATA_Find_Prop( key, 0, AVP_PID_VALUE );
  for( i=0; p && (i<4); i++,p=PROP_Get_Next(p) )
    ;
  return p;
}



// ---
HPROP pr_call get_next_val( HPROP pcrc ) {
  tUINT i;
  for( i=0; pcrc && (i<3); i++,pcrc=PROP_Get_Next(pcrc) )
    ;
  return pcrc;
}



// ---
tERROR pr_call check_type( tTYPE_ID type, AVP_TYPE* tv, tPTR pValue, tDWORD size, AVP_size_t* rval, AVP_dword* usd ) {
  tERROR e = errOK;
#define INVP e = errPARAMETER_INVALID
  switch( type ) {
    case tid_VOID        : *tv = avpt_nothing;  *usd = 0; e = errPARAMETER_INVALID; break;
    case tid_BYTE        : *tv = avpt_byte;     *usd = sizeof(AVP_size_t); if (size < sizeof(tBYTE))     INVP;    else *rval = *(tBYTE*)pValue; break;
    case tid_WORD        : *tv = avpt_word;     *usd = sizeof(AVP_size_t); if (size < sizeof(tWORD))     INVP;    else *rval = *(tWORD*)pValue; break;
    case tid_DWORD       : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tDWORD))    INVP;    else *rval = *(tDWORD*)pValue; break;
    case tid_QWORD       : *tv = avpt_qword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tQWORD))    INVP;    else *rval = (tDATA)pValue; break;
    case tid_BOOL        : *tv = avpt_bool;     *usd = sizeof(AVP_size_t); if (size < sizeof(tBOOL))     INVP;    else *rval = *(tBOOL*)pValue; break;
    case tid_CHAR        : *tv = avpt_char;     *usd = sizeof(AVP_size_t); if (size < sizeof(tCHAR))     INVP;    else *rval = *(tCHAR*)pValue; break;
    case tid_WCHAR       : *tv = avpt_wchar;    *usd = sizeof(AVP_size_t); if (size < sizeof(tWCHAR))    INVP;    else *rval = *(tWCHAR*)pValue; break;
    case tid_STRING      : *tv = avpt_str;      *usd = 0; *rval = (tDATA)pValue; break;
    case tid_WSTRING     : *tv = avpt_wstr;     *usd = 0; *rval = (tDATA)pValue; break;
    case tid_ERROR       : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tERROR))    INVP;    else *rval = *(tERROR*)pValue; break;
    case tid_PTR         : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tPTR))      INVP;    else *rval = (tDATA)*(tPTR*)pValue; break;
    case tid_INT         : *tv = avpt_int;      *usd = sizeof(AVP_size_t); if (size < sizeof(tINT))      INVP;    else *rval = *(tINT*)pValue; break; // native signed integer type
    case tid_UINT        : *tv = avpt_uint;     *usd = sizeof(AVP_size_t); if (size < sizeof(tUINT))     INVP;    else *rval = *(tUINT*)pValue; break; // native unsigned integer type
    case tid_SBYTE       : *tv = avpt_byte;     *usd = sizeof(AVP_size_t); if (size < sizeof(tSBYTE))    INVP;    else *rval = *(tSBYTE*)pValue; break;

    case tid_SHORT       : *tv = avpt_short;    *usd = sizeof(AVP_size_t); if (size < sizeof(tSHORT))    INVP;    else *rval = *(tSHORT*)pValue; break;
    case tid_LONG        : *tv = avpt_long;     *usd = sizeof(AVP_size_t); if (size < sizeof(tLONG))     INVP;    else *rval = *(tLONG*)pValue; break;
    case tid_LONGLONG    : *tv = avpt_longlong; *usd = sizeof(AVP_size_t); if (size < sizeof(tLONGLONG)) INVP;    else *rval = (tDATA)pValue; break;
    case tid_DOUBLE      : *tv = avpt_longlong; *usd = sizeof(AVP_size_t); if (size < sizeof(tLONGLONG)) INVP;    else *rval = (tDATA)pValue; break;
    case tid_IID         : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tIID))      INVP;    else *rval = *(tIID*)pValue; break; // plugin interface identifier
    case tid_PID         : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tPID))      INVP;    else *rval = *(tPID*)pValue; break; // plugin identifier
    case tid_ORIG_ID     : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tORIG_ID))  INVP;    else *rval = *(tORIG_ID*)pValue; break; // object origin identifier
    case tid_OS_ID       : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tOS_ID))    INVP;    else *rval = *(tOS_ID*)pValue; break; // folder type identifier
    case tid_VID         : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tVID))      INVP;    else *rval = *(tVID*)pValue; break; // vendor ideftifier
    case tid_PROPID      : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tPROPID))   INVP;    else *rval = *(tPROPID*)pValue; break;          // property identifier
    case tid_VERSION     : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tVERSION))  INVP;    else *rval = *(tVERSION*)pValue; break;         // version of any identifier: High word - version, Low word - subversion
    case tid_CODEPAGE    : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tCODEPAGE)) INVP;    else *rval = *(tCODEPAGE*)pValue; break;        // codepage identifier
    case tid_LCID        : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tLCID))     INVP;    else *rval = *(tLCID*)pValue; break;            // codepage identifier
    case tid_DATA        : *tv = avpt_size_t;   *usd = sizeof(AVP_size_t); if (size < sizeof(tDATA))     INVP;    else *rval = *(tDATA*)pValue; break;            // universal data storage
    case tid_DATETIME    : *tv = avpt_date;     *usd = sizeof(AVP_size_t); if (size < sizeof(tDATETIME)) INVP;    else *rval = (tDATA)pValue; break;              // universal date storage
    case tid_FUNC_PTR    : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tFUNC_PTR)) INVP;    else *rval = (tDATA)*(tFUNC_PTR*)pValue; break; // universal function ptr
                                                                                                                          
    // additional types                                                                                                  
    case tid_BINARY      : *tv = avpt_bin;      *usd = 0; *rval = (tDATA)pValue; break;
    case tid_IFACEPTR    : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tFUNC_PTR)) INVP;    else *rval = (tDATA)*(tFUNC_PTR*)pValue; break;
    case tid_OBJECT      : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(hOBJECT))   INVP;    else *rval = (tDATA)*(hOBJECT*)pValue; break;
    case tid_EXPORT      : *tv = avpt_bin;      *usd = sizeof(tEXPORT);    if (size < sizeof(tEXPORT))   INVP;    else *rval = (tDATA)pValue; break;
    case tid_IMPORT      : *tv = avpt_bin;      *usd = sizeof(tIMPORT);    if (size < sizeof(tIMPORT))   INVP;    else *rval = (tDATA)pValue; break;
    case tid_TRACE_LEVEL : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tTRACE_LEVEL)) INVP; else *rval = *(tTRACE_LEVEL*)pValue; break;
    case tid_TYPE_ID     : *tv = avpt_dword;    *usd = sizeof(AVP_size_t); if (size < sizeof(tTYPE_ID))  INVP;    else *rval = *(tTYPE_ID*)pValue; break;
    default              : *tv = avpt_nothing;  *usd = 0; INVP; break;
  }
#undef INVP
  return e;
}



// ---
tERROR pr_call conv_type( AVP_TYPE tv, tTYPE_ID* type ) {
  switch( tv ) {
    case avpt_nothing  : *type = tid_VOID;     break;
    case avpt_char     : *type = tid_CHAR;     break;
    case avpt_wchar    : *type = tid_WCHAR;    break;
    case avpt_short    : *type = tid_SHORT;    break;
    case avpt_long     : *type = tid_LONG;     break;
    case avpt_byte     : *type = tid_BYTE;     break;
    case avpt_group    : *type = tid_BYTE;     break;
    case avpt_word     : *type = tid_WORD;     break;
    case avpt_dword    : *type = tid_DWORD;    break;
    case avpt_bool     : *type = tid_BOOL;     break;
//    case avpt_date     : return errUNEXPECTED;
    case avpt_time     : return errUNEXPECTED;
    case avpt_date     : *type = tid_DATETIME; break;
    case avpt_int      : *type = tid_INT;      break;
    case avpt_uint     : *type = tid_UINT;     break;
    case avpt_qword    : *type = tid_QWORD;    break;
    case avpt_longlong : *type = tid_LONGLONG; break;
    case avpt_str      : *type = tid_STRING;   break;
    case avpt_wstr     : *type = tid_WSTRING;  break;
    case avpt_bin      : *type = tid_BINARY;   break;
    default            : return errUNEXPECTED;
  }
  return errOK;
}



// ---
tERROR pr_call get_str( tSTRING s, tDWORD* out_size, tCHAR* buffer, tDWORD size ) {
  if ( !buffer )
    *out_size = 1 + (s ? (tDWORD)strlen(s) : 0);
  else if ( !s || !*s ) {
    if ( size >= 1 ) {
      *out_size = 1;
      *buffer = 0;
    }
    else
      return errBUFFER_TOO_SMALL;
  }
  else {
    tUINT l = 1 + (tUINT)strlen(s);
    if ( size >= l )
      memcpy( buffer, s, *out_size=l );
    else {
      *out_size = 0;
      return errBUFFER_TOO_SMALL;
    }
  }
  return errOK;
}




// ---
tERROR pr_call prove_right_slash( hOBJECT obj, tCHAR** str, tBOOL in_place ) {
	tCHAR* copy;
	tERROR err;
	tUINT  count;
	tBOOL  found;
	tCHAR* ptr = *str;

	if ( !ptr )
		return errOK;

	if ( in_place ) {
		for( ; *ptr; ++ptr ) {
			if ( *ptr == '/' )
				*ptr = '\\';
		}
		return errOK;
	}

	for( found=cFALSE,count=0; *ptr; ++count,++ptr ) {
		if ( !found )
			found = (*ptr == '/');
	}

	if ( !found )
		return errOK;

	err = CALL_SYS_ObjHeapAlloc( obj, (tPTR*)&copy, count+1 );
	if ( PR_FAIL(err) )
		return err;

	for( ptr=*str,*str=copy; *ptr; ++ptr,++copy ) {
		if ( *ptr == '/' )
			*copy = '\\';
		else
			*copy = *ptr;
	}
	*copy = 0;
	return errOK;
}



// ---
tERROR pr_call replace_str( hi_Registry _this, tSTRING* pstring, tSTRING* old, tDWORD* out_size, tSTRING buffer, tDWORD size ) {
  tUINT   l;
  tERROR  e;
  tSTRING new_string;

  if ( old )
    *old = 0;

  if ( !buffer ) {
    *out_size = 260;
    return errOK;
  }

  if ( !size )
    return errPARAMETER_INVALID;

  for( l=0; (l<size) && buffer[l]; l++ )
    ;
  if ( l ) {
    e = CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&new_string, l+1 );
    if ( PR_FAIL(e) )
      return e;
  }
  else
    new_string = 0;
  
  if ( *pstring ) {
    if ( old )
      *old = *pstring;
    else
      CALL_SYS_ObjHeapFree( _this, *pstring );
  }
  if ( new_string ) {
    memcpy( new_string, buffer, l );
    new_string[l] = 0;
  }
  *pstring = new_string;
  *out_size = l;
  return errOK;
}



// ---
tERROR pr_call get_key_name( HDATA k, tDWORD* out_size, tSTRING name_buff, tDWORD size, tBOOL bFullName ) {
  tERROR e;
  tDWORD cs;
  tDWORD cumul = 1; // zero terminated

  if ( !bFullName ) {
    if ( 0 == (*out_size=DATA_Get_Val(k,0,AVP_PID_VALUE,name_buff,size)) )
      return errUNEXPECTED;
    else
      return errOK;
  }
  else if ( !name_buff ) {
    while( k ) {
      e = get_key_name( k, &cs, 0, 0, cFALSE );
      if ( PR_SUCC(e) ) {
        cumul += cs; // +1 for slash and -1 for zero terminated
        k = DATA_Get_Dad( k, 0 );
      }
      else {
        *out_size = 0;
        return e;
      }
    }
  }
  else {
    HDATA curr;
    HDATA dad;
    HDATA top = 0;
    while( top != k ) {
      if ( size <= 2 ) {
        *out_size = 0;
        return errBUFFER_TOO_SMALL;
      }

      dad = DATA_Get_Dad( curr=k, 0 );
      while ( dad != top )
        dad = DATA_Get_Dad( curr=dad, 0 );
      
      cs = 0;
      e = get_key_name( curr, &cs, name_buff+1, size-1, cFALSE );
      if ( PR_SUCC(e) ) {
        *name_buff = '\\';
        name_buff += cs;
        size -= cs;
        cumul += cs;
        top = curr;
      }
      else {
        *out_size = 0;
        return e;
      }
    }
  }
  *out_size = cumul;
  return errOK;
}


#define CUST(s) (*(tSTRING*)&s)

// ---
const tCHAR* pr_call my_strchr( const tCHAR* str, tINT ch ) {
  if ( !str )
    return 0;
  for( ; *str; CUST(str)++ ) {
    if ( *str == ch )
      return str;
  }
  return 0;
}




// ---
tINT pr_call my_strnicmp( const tCHAR* s1, const tCHAR* s2, tUINT s ) {
	if ( CmpStr ) {
		tERROR err = CmpStr( s1, s, cCP_ANSI, s2, s, cCP_ANSI, fSTRING_COMPARE_CASE_INSENSITIVE );
		if ( PR_SUCC(err) )
			return 0;

	}

  if ( !s1 != !s2 )
    return (tINT)(s1 - s2);
  if ( s1 ) {
    tUINT i;
    for( i=0; i<s; i++,CUST(s1)++,CUST(s2)++ ) {
      if ( *s1 != *s2 )
        return *s1 - *s2;
    }
  }
  return 0;
}



// ---
//const tCHAR* pr_call my_strlwr( hi_Registry _this, const tCHAR* str ) {
//	tCHAR* dst;
//	RegData* d;
//	tUINT do_not_need_convert=1, c=0;
//
//	d = _this->data;
//	if ( d->m_case_sensitive )
//		return str;
//
//	for( ; str[c]; ++c ) {
//		if ( do_not_need_convert )
//			do_not_need_convert = islower( str[c] );
//	}
//	++c;
//	if ( do_not_need_convert )
//		return str;
//
//	if ( d->m_convert_buff_len < c ) {
//		if ( PR_FAIL(CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->m_convert_buff,d->m_convert_buff,c)) )
//			return str;
//		d->m_convert_buff_len = c;
//	}
//
//	dst = (tCHAR*)d->m_convert_buff;
//	for( ; *str; ++str,++dst )
//		*dst = tolower( *str );
//	*dst = 0;
//	return d->m_convert_buff;
//}



// ---
tERROR pr_call open_base_by_name( hi_Registry _this ) {
	tERROR   e;
	RegData* d;
	tDWORD access;
	tDWORD omode;

	d = _this->data;
	if ( !d->m_file_name )
		return errOBJECT_NOT_INITIALIZED;

	if ( d->m_obj_new )
		omode = fOMODE_CREATE_ALWAYS;
	else
		omode = fOMODE_OPEN_IF_EXIST;

	if ( !d->m_ro && d->m_save_on_close )
		access = fACCESS_READ | fACCESS_WRITE;
	else
		access = fACCESS_READ;

	if ( PR_FAIL(e=CALL_SYS_ObjectCreate(_this,&d->m_base,IID_IO,PID_NATIVE_FIO,SUBTYPE_ANY)) )
		;
	else if ( PR_FAIL(e=CALL_SYS_PropertySetStr(d->m_base,0,pgOBJECT_NAME,d->m_file_name,0,cCP_ANSI)) )
		;
	else if ( PR_FAIL(e=CALL_SYS_PropertySetDWord(d->m_base,pgOBJECT_OPEN_MODE,omode)) )
		;
	else if ( PR_FAIL(e=CALL_SYS_PropertySetDWord(d->m_base,pgOBJECT_ACCESS_MODE,access)) )
		;
	else if ( PR_FAIL(e=CALL_SYS_ObjectCreateDone(d->m_base)) )
		;

	if ( PR_FAIL(e) && d->m_base ) {
		CALL_SYS_ObjectClose( d->m_base );
		d->m_base = 0;
	}

	return e;
}


#ifdef _DEBUG

// ---
tERROR pr_call check_tree( HDATA tree ) {
	HDATA  sub_key;
	HPROP  pcrc, pname, pkeynum, pvalnum, pval;
	tDWORD v, c;
	tCHAR  name[260];

	if ( 0 == (pcrc=DATA_Find_Prop(tree,0,KEY_CRC)) )
		return errOBJECT_BAD_INTERNAL_STATE;
	if ( 0 == (pkeynum=DATA_Find_Prop(tree,0,KEY_KEY_NUM)) )
		return errOBJECT_BAD_INTERNAL_STATE;
	if ( 0 == (pvalnum=DATA_Find_Prop(tree,0,KEY_VAL_NUM)) )
		return errOBJECT_BAD_INTERNAL_STATE;

	PROP_Get_Val( pcrc, &v, sizeof(v) );
	DATA_Get_Val( tree, 0, AVP_PID_VALUE, name, sizeof(name) );
	c = iCountCRC32str( name );
	if ( v != c )
		return errOBJECT_BAD_INTERNAL_STATE;

	// check property values
	PROP_Get_Val( pvalnum, &v, sizeof(v) );
	pcrc = get_first_val( tree );
	if ( !v != !pcrc )
		return errOBJECT_BAD_INTERNAL_STATE;

	for( c=0; pcrc; c++,pcrc=get_next_val(pcrc) ) {
		tDWORD crc;
		if ( c >= v )
			return errOBJECT_BAD_INTERNAL_STATE;
		pval = get_entry( pcrc, &pname );
		if ( !pval || !pname )
			return errOBJECT_BAD_INTERNAL_STATE;
		PROP_Get_Val( pcrc, &crc, sizeof(crc) );
		PROP_Get_Val( pname, name, sizeof(name) );
		if ( crc != iCountCRC32str(name) )
			return errOBJECT_BAD_INTERNAL_STATE;
	}
	if ( c < v )
		return errOBJECT_BAD_INTERNAL_STATE;

	// check keys
	PROP_Get_Val( pkeynum, &v, sizeof(v) );
	sub_key = DATA_Get_First( tree, 0 );
	for( c=0; sub_key; c++,sub_key=DATA_Get_Next(sub_key,0) ) {
		tERROR e;
		if ( c >= v )
			return errOBJECT_BAD_INTERNAL_STATE;
#ifdef _DEBUG
		if ( PR_FAIL(e=check_tree(sub_key)) )
			return e;
#endif
	}
	if ( c < v )
		return errOBJECT_BAD_INTERNAL_STATE;

	return errOK;
}

#endif



// ---
//#if !defined(_DEBUG) && defined (__KLDTSER_H__) && !defined (__unix__)
//size_t __cdecl _mbslen( const unsigned char* s ) {
//  return strlen( s );
//}
//#endif




