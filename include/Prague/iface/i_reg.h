// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  28 June 2006,  20:32 --------
// File Name   -- (null)i_reg.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_reg__dca1bc60_8555_4ed5_8c4b_a10fdf3d8f74 )
#define __i_reg__dca1bc60_8555_4ed5_8c4b_a10fdf3d8f74
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Registry interface implementation
// Short comments -- access to storage like registry
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_REGISTRY  ((tIID)(13))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
typedef tPTR                         tRegKey; // seance unique key of the registry branch
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
#define cRegRoot                       ((tRegKey)(0)) // root of the current registry object
#define cRegNothing                    ((tRegKey)(-1)) //  --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
#define warnKEY_NOT_EXISTS                       PR_MAKE_DECL_WARN(IID_REGISTRY, 0x001) // 0x0000d001,53249 (1) -- this key(value) was not exist but created
#define errKEY_NOT_FOUND                         PR_MAKE_DECL_ERR(IID_REGISTRY, 0x002) // 0x8000d002,-2147430398 (2) -- key(value) not found
#define errKEY_ENUM_FAILED                       PR_MAKE_DECL_ERR(IID_REGISTRY, 0x003) // 0x8000d003,-2147430397 (3) -- key(value) enumeration failed
#define errKEY_INDEX_OUT_OF_RANGE                PR_MAKE_DECL_ERR(IID_REGISTRY, 0x004) // 0x8000d004,-2147430396 (4) -- indicated key (value) index is out of renge
#define errKEY_CANNOT_BE_CLOSED                  PR_MAKE_DECL_ERR(IID_REGISTRY, 0x005) // 0x8000d005,-2147430395 (5) -- key cannot be closed
#define errKEY_CANNOT_BE_DELETED                 PR_MAKE_DECL_ERR(IID_REGISTRY, 0x006) // 0x8000d006,-2147430394 (6) -- key cannot be deleted or cleaned
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hREGISTRY;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iRegistryVtbl;
typedef struct iRegistryVtbl iRegistryVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cRegistry;
	typedef cRegistry* hREGISTRY;
#else
	typedef struct tag_hREGISTRY {
		const iRegistryVtbl* vtbl; // pointer to the "Registry" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hREGISTRY;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Registry_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpRegistry_OpenKey)             ( hREGISTRY _this, tRegKey* result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist ); // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning;
	typedef   tERROR (pr_call *fnpRegistry_OpenKeyByIndex)      ( hREGISTRY _this, tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev ); // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning;
	typedef   tERROR (pr_call *fnpRegistry_OpenNextKey)         ( hREGISTRY _this, tRegKey* result, tRegKey key, tBOOL bClosePrev ); // -- opens next key on the same level;
	typedef   tERROR (pr_call *fnpRegistry_CloseKey)            ( hREGISTRY _this, tRegKey key ); // -- closes the key;
	typedef   tERROR (pr_call *fnpRegistry_GetKeyName)          ( hREGISTRY _this, tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ); // -- returns name of the key;
	typedef   tERROR (pr_call *fnpRegistry_GetKeyNameByIndex)   ( hREGISTRY _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ); // -- returns name of the subkey by index;
	typedef   tERROR (pr_call *fnpRegistry_GetValue)            ( hREGISTRY _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size ); // -- gets key value, returns byte length of the value put in the buffer;
	typedef   tERROR (pr_call *fnpRegistry_GetValueByIndex)     ( hREGISTRY _this, tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ); // -- gets key value, returns byte length of the value put in the buffer;
	typedef   tERROR (pr_call *fnpRegistry_GetValueNameByIndex) ( hREGISTRY _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size ); // -- gets key name, returns byte length of the name put in the buffer;
	typedef   tERROR (pr_call *fnpRegistry_SetValue)            ( hREGISTRY _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ); // -- set key value. Creates key if it is not exist and parameter bCreateIfNotExist is true. Returns warning errKEY_NOT_EXISTS in this case;
	typedef   tERROR (pr_call *fnpRegistry_SetValueByIndex)     ( hREGISTRY _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ); // -- set key value, creates key if it is not exist and return warning errKEY_NOT_EXISTS in this case;
	typedef   tERROR (pr_call *fnpRegistry_GetKeyCount)         ( hREGISTRY _this, tDWORD* result, tRegKey key ); // -- gets subkey count;
	typedef   tERROR (pr_call *fnpRegistry_GetValueCount)       ( hREGISTRY _this, tDWORD* result, tRegKey key ); // -- gets subkey count;
	typedef   tERROR (pr_call *fnpRegistry_DeleteKey)           ( hREGISTRY _this, tRegKey key, const tCHAR* szSubKeyName ); // -- deletes the key;
	typedef   tERROR (pr_call *fnpRegistry_DeleteKeyByInd)      ( hREGISTRY _this, tRegKey key, tDWORD index ); // -- deletes the key;
	typedef   tERROR (pr_call *fnpRegistry_DeleteValue)         ( hREGISTRY _this, tRegKey key, const tCHAR* szValueName ); // -- deletes the key value;
	typedef   tERROR (pr_call *fnpRegistry_DeleteValueByInd)    ( hREGISTRY _this, tRegKey key, tDWORD index ); // -- deletes the key value;
	typedef   tERROR (pr_call *fnpRegistry_Clean)               ( hREGISTRY _this );            // -- clean the object contained registry data from the root key;
	typedef   tERROR (pr_call *fnpRegistry_CopyKey)             ( hREGISTRY _this, tRegKey src_key, hREGISTRY destination, tRegKey dest_key ); // -- copy key to another hREGISTRY;
	typedef   tERROR (pr_call *fnpRegistry_CopyKeyByName)       ( hREGISTRY _this, const tCHAR* src, hREGISTRY dst ); // -- ;
	typedef   tERROR (pr_call *fnpRegistry_SetRootKey)          ( hREGISTRY _this, tRegKey root, tBOOL close_prev ); // -- ;
	typedef   tERROR (pr_call *fnpRegistry_SetRootStr)          ( hREGISTRY _this, const tCHAR* root, tBOOL close_prev ); // -- ;
	typedef   tERROR (pr_call *fnpRegistry_Flush)               ( hREGISTRY _this, tBOOL force ); // -- saved all changes to persistent storage if acceptable;


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
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Registry_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION     mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT     mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgOBJECT_RO             mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgMAX_NAME_LEN          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgCLEAN                 mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgSAVE_RESULTS_ON_CLOSE mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001004 )
#define pgROOT_POINT            mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001005 )
#define pgROOT_KEY              mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001006 )
#define pgMAX_VALUE_SIZE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001007 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Registry_OpenKey( _this, result, key, szSubKeyName, bCreateIfNotExist )                           ((_this)->vtbl->OpenKey( (_this), result, key, szSubKeyName, bCreateIfNotExist ))
	#define CALL_Registry_OpenKeyByIndex( _this, result, key, index, bClosePrev )                                  ((_this)->vtbl->OpenKeyByIndex( (_this), result, key, index, bClosePrev ))
	#define CALL_Registry_OpenNextKey( _this, result, key, bClosePrev )                                            ((_this)->vtbl->OpenNextKey( (_this), result, key, bClosePrev ))
	#define CALL_Registry_CloseKey( _this, key )                                                                   ((_this)->vtbl->CloseKey( (_this), key ))
	#define CALL_Registry_GetKeyName( _this, result, key, name_buff, size, bFullName )                             ((_this)->vtbl->GetKeyName( (_this), result, key, name_buff, size, bFullName ))
	#define CALL_Registry_GetKeyNameByIndex( _this, result, key, index, name_buff, size, bFullName )               ((_this)->vtbl->GetKeyNameByIndex( (_this), result, key, index, name_buff, size, bFullName ))
	#define CALL_Registry_GetValue( _this, result, key, szValueName, type, pValue, size )                          ((_this)->vtbl->GetValue( (_this), result, key, szValueName, type, pValue, size ))
	#define CALL_Registry_GetValueByIndex( _this, result, key, index, type, pValue, size )                         ((_this)->vtbl->GetValueByIndex( (_this), result, key, index, type, pValue, size ))
	#define CALL_Registry_GetValueNameByIndex( _this, result, key, index, name_buff, size )                        ((_this)->vtbl->GetValueNameByIndex( (_this), result, key, index, name_buff, size ))
	#define CALL_Registry_SetValue( _this, key, szValueName, type, pValue, size, bCreateIfNotExist )               ((_this)->vtbl->SetValue( (_this), key, szValueName, type, pValue, size, bCreateIfNotExist ))
	#define CALL_Registry_SetValueByIndex( _this, key, index, type, pValue, size, bCreateIfNotExist )              ((_this)->vtbl->SetValueByIndex( (_this), key, index, type, pValue, size, bCreateIfNotExist ))
	#define CALL_Registry_GetKeyCount( _this, result, key )                                                        ((_this)->vtbl->GetKeyCount( (_this), result, key ))
	#define CALL_Registry_GetValueCount( _this, result, key )                                                      ((_this)->vtbl->GetValueCount( (_this), result, key ))
	#define CALL_Registry_DeleteKey( _this, key, szSubKeyName )                                                    ((_this)->vtbl->DeleteKey( (_this), key, szSubKeyName ))
	#define CALL_Registry_DeleteKeyByInd( _this, key, index )                                                      ((_this)->vtbl->DeleteKeyByInd( (_this), key, index ))
	#define CALL_Registry_DeleteValue( _this, key, szValueName )                                                   ((_this)->vtbl->DeleteValue( (_this), key, szValueName ))
	#define CALL_Registry_DeleteValueByInd( _this, key, index )                                                    ((_this)->vtbl->DeleteValueByInd( (_this), key, index ))
	#define CALL_Registry_Clean( _this )                                                                           ((_this)->vtbl->Clean( (_this) ))
	#define CALL_Registry_CopyKey( _this, src_key, destination, dest_key )                                         ((_this)->vtbl->CopyKey( (_this), src_key, destination, dest_key ))
	#define CALL_Registry_CopyKeyByName( _this, src, dst )                                                         ((_this)->vtbl->CopyKeyByName( (_this), src, dst ))
	#define CALL_Registry_SetRootKey( _this, root, close_prev )                                                    ((_this)->vtbl->SetRootKey( (_this), root, close_prev ))
	#define CALL_Registry_SetRootStr( _this, root, close_prev )                                                    ((_this)->vtbl->SetRootStr( (_this), root, close_prev ))
	#define CALL_Registry_Flush( _this, force )                                                                    ((_this)->vtbl->Flush( (_this), force ))
#else // if !defined( __cplusplus )
	#define CALL_Registry_OpenKey( _this, result, key, szSubKeyName, bCreateIfNotExist )                           ((_this)->OpenKey( result, key, szSubKeyName, bCreateIfNotExist ))
	#define CALL_Registry_OpenKeyByIndex( _this, result, key, index, bClosePrev )                                  ((_this)->OpenKeyByIndex( result, key, index, bClosePrev ))
	#define CALL_Registry_OpenNextKey( _this, result, key, bClosePrev )                                            ((_this)->OpenNextKey( result, key, bClosePrev ))
	#define CALL_Registry_CloseKey( _this, key )                                                                   ((_this)->CloseKey( key ))
	#define CALL_Registry_GetKeyName( _this, result, key, name_buff, size, bFullName )                             ((_this)->GetKeyName( result, key, name_buff, size, bFullName ))
	#define CALL_Registry_GetKeyNameByIndex( _this, result, key, index, name_buff, size, bFullName )               ((_this)->GetKeyNameByIndex( result, key, index, name_buff, size, bFullName ))
	#define CALL_Registry_GetValue( _this, result, key, szValueName, type, pValue, size )                          ((_this)->GetValue( result, key, szValueName, type, pValue, size ))
	#define CALL_Registry_GetValueByIndex( _this, result, key, index, type, pValue, size )                         ((_this)->GetValueByIndex( result, key, index, type, pValue, size ))
	#define CALL_Registry_GetValueNameByIndex( _this, result, key, index, name_buff, size )                        ((_this)->GetValueNameByIndex( result, key, index, name_buff, size ))
	#define CALL_Registry_SetValue( _this, key, szValueName, type, pValue, size, bCreateIfNotExist )               ((_this)->SetValue( key, szValueName, type, pValue, size, bCreateIfNotExist ))
	#define CALL_Registry_SetValueByIndex( _this, key, index, type, pValue, size, bCreateIfNotExist )              ((_this)->SetValueByIndex( key, index, type, pValue, size, bCreateIfNotExist ))
	#define CALL_Registry_GetKeyCount( _this, result, key )                                                        ((_this)->GetKeyCount( result, key ))
	#define CALL_Registry_GetValueCount( _this, result, key )                                                      ((_this)->GetValueCount( result, key ))
	#define CALL_Registry_DeleteKey( _this, key, szSubKeyName )                                                    ((_this)->DeleteKey( key, szSubKeyName ))
	#define CALL_Registry_DeleteKeyByInd( _this, key, index )                                                      ((_this)->DeleteKeyByInd( key, index ))
	#define CALL_Registry_DeleteValue( _this, key, szValueName )                                                   ((_this)->DeleteValue( key, szValueName ))
	#define CALL_Registry_DeleteValueByInd( _this, key, index )                                                    ((_this)->DeleteValueByInd( key, index ))
	#define CALL_Registry_Clean( _this )                                                                           ((_this)->Clean( ))
	#define CALL_Registry_CopyKey( _this, src_key, destination, dest_key )                                         ((_this)->CopyKey( src_key, destination, dest_key ))
	#define CALL_Registry_CopyKeyByName( _this, src, dst )                                                         ((_this)->CopyKeyByName( src, dst ))
	#define CALL_Registry_SetRootKey( _this, root, close_prev )                                                    ((_this)->SetRootKey( root, close_prev ))
	#define CALL_Registry_SetRootStr( _this, root, close_prev )                                                    ((_this)->SetRootStr( root, close_prev ))
	#define CALL_Registry_Flush( _this, force )                                                                    ((_this)->Flush( force ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct iRegistry {
		virtual tERROR pr_call OpenKey( tRegKey* result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist ) = 0;
		virtual tERROR pr_call OpenKeyByIndex( tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev ) = 0;
		virtual tERROR pr_call OpenNextKey( tRegKey* result, tRegKey key, tBOOL bClosePrev ) = 0; // -- opens next key on the same level
		virtual tERROR pr_call CloseKey( tRegKey key ) = 0; // -- closes the key
		virtual tERROR pr_call GetKeyName( tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) = 0; // -- returns name of the key
		virtual tERROR pr_call GetKeyNameByIndex( tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) = 0; // -- returns name of the subkey by index
		virtual tERROR pr_call GetValue( tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size ) = 0; // -- gets key value, returns byte length of the value put in the buffer
		virtual tERROR pr_call GetValueByIndex( tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ) = 0; // -- gets key value, returns byte length of the value put in the buffer
		virtual tERROR pr_call GetValueNameByIndex( tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size ) = 0; // -- gets key name, returns byte length of the name put in the buffer
		virtual tERROR pr_call SetValue( tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) = 0; // -- set key value. Creates key if it is not exist and parameter bCreateIfNotExist is true. Returns warning errKEY_NOT_EXISTS in this case
		virtual tERROR pr_call SetValueByIndex( tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) = 0; // -- set key value, creates key if it is not exist and return warning errKEY_NOT_EXISTS in this case
		virtual tERROR pr_call GetKeyCount( tDWORD* result, tRegKey key ) = 0; // -- gets subkey count
		virtual tERROR pr_call GetValueCount( tDWORD* result, tRegKey key ) = 0; // -- gets subkey count
		virtual tERROR pr_call DeleteKey( tRegKey key, const tCHAR* szSubKeyName ) = 0; // -- deletes the key
		virtual tERROR pr_call DeleteKeyByInd( tRegKey key, tDWORD index ) = 0; // -- deletes the key
		virtual tERROR pr_call DeleteValue( tRegKey key, const tCHAR* szValueName ) = 0; // -- deletes the key value
		virtual tERROR pr_call DeleteValueByInd( tRegKey key, tDWORD index ) = 0; // -- deletes the key value
		virtual tERROR pr_call Clean() = 0; // -- clean the object contained registry data from the root key
		virtual tERROR pr_call CopyKey( tRegKey src_key, hREGISTRY destination, tRegKey dest_key ) = 0; // -- copy key to another hREGISTRY
		virtual tERROR pr_call CopyKeyByName( const tCHAR* src, hREGISTRY dst ) = 0;
		virtual tERROR pr_call SetRootKey( tRegKey root, tBOOL close_prev ) = 0;
		virtual tERROR pr_call SetRootStr( const tCHAR* root, tBOOL close_prev ) = 0;
		virtual tERROR pr_call Flush( tBOOL force ) = 0; // -- saved all changes to persistent storage if acceptable
	};

	struct pr_abstract cRegistry : public iRegistry, public iObj {

		OBJ_IMPL( cRegistry );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hREGISTRY()   { return (hREGISTRY)this; }

		tBOOL pr_call get_pgOBJECT_RO() { return (tBOOL)getBool(pgOBJECT_RO); }
		tERROR pr_call set_pgOBJECT_RO( tBOOL value ) { return setBool(pgOBJECT_RO,(tBOOL)value); }

		tDWORD pr_call get_pgMAX_NAME_LEN() { return (tDWORD)getDWord(pgMAX_NAME_LEN); }
		tERROR pr_call set_pgMAX_NAME_LEN( tDWORD value ) { return setDWord(pgMAX_NAME_LEN,(tDWORD)value); }

		tDWORD pr_call get_pgCLEAN() { return (tDWORD)getDWord(pgCLEAN); }
		tERROR pr_call set_pgCLEAN( tDWORD value ) { return setDWord(pgCLEAN,(tDWORD)value); }

		tBOOL pr_call get_pgSAVE_RESULTS_ON_CLOSE() { return (tBOOL)getBool(pgSAVE_RESULTS_ON_CLOSE); }
		tERROR pr_call set_pgSAVE_RESULTS_ON_CLOSE( tBOOL value ) { return setBool(pgSAVE_RESULTS_ON_CLOSE,(tBOOL)value); }

		tERROR pr_call get_pgROOT_POINT( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgROOT_POINT,buff,size,cp); }
		tERROR pr_call set_pgROOT_POINT( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgROOT_POINT,buff,size,cp); }

		tPTR pr_call get_pgROOT_KEY() { return getPtr(pgROOT_KEY); }
		tERROR pr_call set_pgROOT_KEY( tPTR value ) { return setPtr(pgROOT_KEY,value); }

		tDWORD pr_call get_pgMAX_VALUE_SIZE() { return (tDWORD)getDWord(pgMAX_VALUE_SIZE); }
		tERROR pr_call set_pgMAX_VALUE_SIZE( tDWORD value ) { return setDWord(pgMAX_VALUE_SIZE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



	

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_reg__dca1bc60_8555_4ed5_8c4b_a10fdf3d8f74
// AVP Prague stamp end



