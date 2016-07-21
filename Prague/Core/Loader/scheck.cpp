// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  08 November 2005,  15:24 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- scheck.cpp
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define SCheck_VERSION ((tVERSION)1)
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_schecklist.h>
#include <Dskm/dskm.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end

#include <stdlib.h>
#include "loader.h"

#define  IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>
#include "loader.h"
#include <dskmparm.h>
#include <Stuff/cpointer.h>


#define INIT_BUFF_SIZE 0x1000

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cSCheckImp : public cSCheck /*cObjImpl*/ {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call GetPrErr( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

#if defined( NEW_SIGN_LIB )
	tERROR pr_call _init_par_list();
#endif

public:
// External function declarations
	tERROR pr_call InitParamByObj( hOBJECT p_obj );
	tERROR pr_call InitParamByMask( tDWORD* p_params_found, const tVOID* p_mask, tCODEPAGE p_mask_cp );
	tERROR pr_call InitCompleted();
	tERROR pr_call CheckObj( hOBJECT p_obj, const tCHAR* p_dskm_comp_id, const tCHAR* p_dskm_obj_type );
	tERROR pr_call CheckObjByMask( tDWORD* p_objs_found, const tVOID* p_mask, tCODEPAGE p_mask_cp, const tCHAR* p_dskm_comp_id, const tCHAR* p_dskm_obj_type );
	tERROR pr_call CheckObjByList( hSCHECKLIST p_list );
	tERROR pr_call GetObjHash( tDWORD* p_hash_size, hOBJECT p_obj, tBYTE* p_hash_buff, tDWORD p_hash_buff_size );

// Data declaration
	HDSKM     m_dskm_lib;         // handle of DSKM lib
	tDWORD    m_dskm_last_err;    // error of the last DSKM operation

#if defined( NEW_SIGN_LIB )
	HDSKMLIST m_dskm_init_list;   // dskm parameter list for init session
	tBOOL     m_global_app_id;    // TRUE will force using global application id
	tCHAR     m_dskm_app_id[100]; // dskm application id
	tCHAR     m_dskm_comp_id[100]; // dskm component id
	tCHAR     m_dskm_obj_type[100]; // dskm object type
// AVP Prague stamp end

	tBYTE*    m_dskm_init_buff;
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cSCheckImp)
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "SCheck". Static(shared) property table variables
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
tERROR cSCheckImp::ObjectInit() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::ObjectInit method" );

	#if defined( NEW_SIGN_LIB )
		m_dskm_lib = DSKM_InitLibraryEx( malloc, free, 0, cTRUE );
		if ( !m_dskm_lib )
			error = errUNEXPECTED;
		else {
			//m_dskm_last_err = DSKM_ParList_Create( &m_dskm_init_list );
			//if ( DSKM_NOT_OK(m_dskm_last_err) ) {
			//	DSKM_DeInitLibrary( m_dskm_lib, FALSE );
			//	m_dskm_init_list = 0;
			//	CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
			//}
		}
	#else
		m_dskm_lib = 0;
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cSCheckImp::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::ObjectInitDone method" );

	#if defined( NEW_SIGN_LIB )
		error = heapAlloc( (tPTR*)&m_dskm_init_buff, INIT_BUFF_SIZE );
		if ( PR_SUCC(error) )
			error = _init_par_list();
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cSCheckImp::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::ObjectPreClose method" );

	#if defined( NEW_SIGN_LIB )
		if ( m_dskm_init_buff ) {
			heapFree( m_dskm_init_buff );
			m_dskm_init_buff = 0;
		}
		if ( m_dskm_init_list ) {
			_dskm_ParList_release( m_dskm_init_list );
			m_dskm_init_list = 0;
		}
		if ( m_dskm_lib ) {
			DSKM_DeInitLibrary( m_dskm_lib, cFALSE );
			m_dskm_lib = 0;
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetPrErr )
// Interface "SCheck". Method "Get" for property(s):
//  -- PR_STORED_ERROR
tERROR cSCheckImp::GetPrErr( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetPrErr for property pgPR_STORED_ERROR" );

	if ( buffer ) {
		if ( size >= sizeof(tERROR) ) {
			#if defined( NEW_SIGN_LIB )
				CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, *((tERROR*)buffer) );
			#else
				*((tERROR*)buffer) = errNOT_SUPPORTED;
			#endif
			*out_size = sizeof(tERROR);
		}
		else {
			error = errBUFFER_TOO_SMALL;
			*out_size = 0;
		}
	}
	else
		*out_size = sizeof(tERROR);

	PR_TRACE_A2( this, "Leave *GET* method GetPrErr for property pgPR_STORED_ERROR, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



#if defined( NEW_SIGN_LIB )
	// ---
	tERROR cSCheckImp::_init_par_list() {

		if ( m_dskm_init_list )
			return errOK;

		tERROR error = errOK;
		HDSKMLIST list = 0;
		tDWORD dskm_err = DSKM_ParList_Create( &list );

		if ( DSKM_OK(dskm_err) && m_global_app_id ) {
			if ( g_app_id )
				strcpy_s( m_dskm_app_id, countof(m_dskm_app_id), g_app_id->m_szId );
			else
				m_dskm_app_id[0] = 0;
		}

		if ( DSKM_OK(dskm_err) && m_dskm_app_id[0] && !DSKM_ParList_AddStrParam(list,DSKM_KEYFILTER_PARAM_APPID_ID,m_dskm_app_id) )
			dskm_err = DSKM_ParList_GetLastError( list );

		if ( DSKM_OK(dskm_err) && m_dskm_comp_id[0] && !DSKM_ParList_AddStrParam(list,DSKM_KEYFILTER_PARAM_COMPID_ID,m_dskm_comp_id) )
			dskm_err = DSKM_ParList_GetLastError( list );

		if ( DSKM_OK(dskm_err) && m_dskm_obj_type[0] && !DSKM_ParList_AddStrParam(list,DSKM_KEYFILTER_PARAM_TYPE_ID,m_dskm_obj_type) )
			dskm_err = DSKM_ParList_GetLastError( list );

		if ( DSKM_NOT_OK(dskm_err) ) {
			CONVERT_DSKM_TO_PR_ERR( dskm_err, error );
			if ( list ) {
				_dskm_ParList_release( list );
				list = 0;
			}
		}

		m_dskm_init_list = list;
		m_dskm_last_err = dskm_err;
		return error;
	}
#endif


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, InitParamByObj )
// Parameters are:
tERROR cSCheckImp::InitParamByObj( hOBJECT p_obj ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::InitParamByObj method" );

	#if defined( NEW_SIGN_LIB )

		cDSKMObjParams* params = 0;
		
		error = _init_par_list();
		if ( PR_SUCC(error) )
			error = sysCheckObject( p_obj, IID_SEQIO, PID_ANY, 0, cFALSE );
		
		if ( PR_SUCC(error) )
			error = _alloc_param( *this, &params, op_seqio, p_obj, 0, cCP_NULL, 0 );
		
		else if ( error != errINTERFACE_INCOMPATIBLE ) {
			m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			return error;
		}
		
		else if ( PR_SUCC(sysCheckObject(p_obj,IID_IO,PID_ANY,0,cFALSE)) )
			error = _alloc_param( *this, &params, op_io, p_obj, 0, cCP_NULL, 0 );
		
		else if ( PR_SUCC(sysCheckObject(p_obj,IID_STRING,PID_ANY,0,cFALSE)) ) {
			
			tWCHAR*  mask = 0;
			cString* str = (hSTRING)p_obj;
			tDWORD   len;

			error = str->LengthEx( &len, cSTRING_WHOLE, cCP_UNICODE, cSTRING_Z );
			if ( PR_SUCC(error) )
				error = heapAlloc( (tPTR*)&mask, len );
			if ( PR_SUCC(error) )
				error = str->ExportToBuff( 0, cSTRING_WHOLE, mask, len, cCP_UNICODE, cSTRING_Z );
			if ( PR_SUCC(error) ) {
				tDWORD files_found = 0;
				tDSKMAddObjByMask param = { *this, m_dskm_init_list, 0, m_dskm_init_buff, INIT_BUFF_SIZE };
				error = _find_files_by_mask( mask, cCP_UNICODE, &files_found, _add_reg_by_mask, &param );
			}
			
			if ( mask )
				heapFree( mask );
			
			if ( PR_SUCC(error) )
				m_dskm_last_err = DSKM_ERR_OK;
			else
				m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			return error;
		}
		
		else {
			m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			return errOBJECT_INCOMPATIBLE;
		}
		
		if ( PR_FAIL(error) )
			return error;

		if ( !DSKM_ParList_AddBufferedReg(m_dskm_init_list,0,m_dskm_init_buff,INIT_BUFF_SIZE,_dskm_read_buff,params) ) {
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_init_list );
			_free_param( params );
		}

		if ( DSKM_NOT_OK(m_dskm_last_err) ) {
			CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
		}
	#else
		error = SignCheck( p_obj, cTRUE, 0, 0, cTRUE, 0 );
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, InitParamByMask )
// Parameters are:
tERROR cSCheckImp::InitParamByMask( tDWORD* p_params_found, const tVOID* p_mask, tCODEPAGE p_mask_cp ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "SCheck::InitParamByFileMask method" );

	#if defined(NEW_SIGN_LIB)
		error = _init_par_list();
		if ( PR_SUCC(error) ) {
			tDSKMAddObjByMask param = { *this, m_dskm_init_list, 0, m_dskm_init_buff, INIT_BUFF_SIZE };
			error = _find_files_by_mask( p_mask, p_mask_cp, p_params_found, _add_reg_by_mask, &param );
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, InitCompleted )
// Parameters are:
tERROR cSCheckImp::InitCompleted() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::InitDone method" );

	#if defined( NEW_SIGN_LIB )
		if ( !m_dskm_init_list )
			return error = errOBJECT_BAD_INTERNAL_STATE;
		m_dskm_last_err = DSKM_PrepareRegsSet( m_dskm_lib, m_dskm_init_list, 0/*DSKM_EXE_OBJ_TYPE*/ );
		if ( DSKM_OK(m_dskm_last_err) ) {
			_dskm_ParList_release( m_dskm_init_list );
			m_dskm_init_list = 0;
		}
		else {
			CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CheckObj )
// Parameters are:
tERROR cSCheckImp::CheckObj( hOBJECT p_obj, const tCHAR* p_dskm_comp_id, const tCHAR* p_dskm_obj_type ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::CheckObj method" );

	return error = PrSignCheckV2( m_dskm_lib, p_obj, p_dskm_comp_id, p_dskm_obj_type, &m_dskm_last_err );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CheckObjByMask )
// Parameters are:
tERROR cSCheckImp::CheckObjByMask( tDWORD* p_objs_found, const tVOID* p_mask, tCODEPAGE p_mask_cp, const tCHAR* p_dskm_comp_id, const tCHAR* p_dskm_obj_type ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheck::CheckObjByMask method" );

	#if defined(NEW_SIGN_LIB)
		tDWORD l_objs_found;
		cAutoObj<cSCheckList> list;
		if ( !p_objs_found )
			p_objs_found = &l_objs_found;
		error = sysCreateObject( list, IID_SCHECKLIST );
		if ( PR_SUCC(error) && p_dskm_comp_id && *p_dskm_comp_id )
			error = list->set_pgDSKM_COMP_ID( (tVOID*)p_dskm_comp_id, cCP_ANSI );
		if ( PR_SUCC(error) && p_dskm_obj_type && *p_dskm_obj_type )
			error = list->set_pgDSKM_OBJ_TYPE( (tVOID*)p_dskm_obj_type, cCP_ANSI );
		if ( PR_SUCC(error) )
			error = list->sysCreateObjectDone();
		if ( PR_SUCC(error) )
			error = list->AddObjByMask( 0, p_objs_found, p_mask, p_mask_cp );
		if ( PR_SUCC(error) ) {
			if ( *p_objs_found )
				error = CheckObjByList( list );
			else
				error = errOBJECT_NOT_FOUND;
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CheckObjByList )
// Parameters are:
tERROR cSCheckImp::CheckObjByList( hSCHECKLIST p_list ) {

	tERROR error = errOK;
	#if defined( NEW_SIGN_LIB )
		HDSKMLIST list;
	#endif

	PR_TRACE_FUNC_FRAME( "SCheck::CheckObjectList method" );

	#if defined( NEW_SIGN_LIB )
		error = sysCheckObject( (cObj*)p_list, IID_SCHECKLIST, PID_ANY, SUBTYPE_ANY, cTRUE );
		if ( PR_SUCC(error) )
			error = p_list->propGet( 0, pgDSKM_LIST, &list, sizeof(list) );
		if ( PR_SUCC(error) ) {
			m_dskm_last_err = DSKM_ParList_GetLastError( list );
			if ( DSKM_OK(m_dskm_last_err) )
				m_dskm_last_err = DSKM_CheckObjectsUsingRegsSet( m_dskm_lib, list, 0/*DSKM_EXE_OBJ_TYPE*/ );
			CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
			PR_TRACE(( this, PR_TRACE_LEVEL(error,prtNOTIFY,prtERROR), "ldr\tdskm - %terr, list", error ));
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetObjHash )
// Parameters are:
tERROR cSCheckImp::GetObjHash( tDWORD* p_hash_size, hOBJECT p_obj, tBYTE* p_hash_buff, tDWORD p_hash_buff_size ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "SCheck::GetObjHash method" );

	#if defined( NEW_SIGN_LIB )
		cDSKMObjParams par = { op_unk, 0, 0, 0, cCP_NULL, cFALSE, {{0,0}} };

		error = sysCheckObject( p_obj, IID_SEQIO );
		
		if ( p_hash_size )
			*p_hash_size = 0;

		if ( PR_SUCC(error) ) {
			par.m_type = op_seqio;
			par.m_seq_io = (cSeqIO*)p_obj;
		}
		else if ( error == errHANDLE_INVALID )
			m_dskm_last_err = DSKM_ERR_INVALID_PARAMETER;
		
		else if ( error != errINTERFACE_INCOMPATIBLE )
			m_dskm_last_err = DSKM_ERR_INVALID_PARAMETER;
		
		else if ( PR_SUCC(sysCheckObject(p_obj,IID_IO)) ) {
			error = errOK;
			par.m_type = op_io;
			par.m_io.io = (cIO*)p_obj;
			par.m_io.offset = 0;
		}
		
		else if ( PR_SUCC(sysCheckObject(p_obj,IID_STRING)) ) {
			
			tCODEPAGE cp = cCP;
			tDWORD name_size;
			tWCHAR name[MAX_PATH];
			error = ((cString*)p_obj)->ExportToBuff( &name_size, cSTRING_WHOLE, name, sizeof(name), cp, cSTRING_Z );
			if ( PR_FAIL(error) )
				m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			if ( PR_SUCC(error) ) {
				par.m_type = op_file;
				par.m_name = name;
				par.m_name_size = name_size;
				par.m_name_cp = cp;
			}
		}
		
		else {
			m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			error = errOBJECT_INCOMPATIBLE;
		}

		if ( PR_SUCC(error) ) {
			m_dskm_last_err = DSKM_HashObjectByBufferGivenHashBuffer( m_dskm_lib, m_dskm_init_buff, INIT_BUFF_SIZE, _dskm_read_buff, &par, p_hash_buff, (AVP_dword*)&p_hash_buff_size );
			if ( p_hash_size && DSKM_OK(m_dskm_last_err) )
				*p_hash_size = p_hash_buff_size;
			CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
		}

		par.m_name = 0;
		_destruct_param( &par );
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "SCheck". Register function
tERROR cSCheckImp::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(SCheck_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, SCheck_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "scheck interface", 17 )
	mpLOCAL_PROPERTY_BUF( pgDSKM_SESSION, cSCheckImp, m_dskm_lib, cPROP_BUFFER_READ )
#if defined( NEW_SIGN_LIB )
	mpLOCAL_PROPERTY_BUF( pgDSKM_APP_ID, cSCheckImp, m_dskm_app_id, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgDSKM_COMP_ID, cSCheckImp, m_dskm_comp_id, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgDSKM_OBJ_TYPE, cSCheckImp, m_dskm_obj_type, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plDSKM_GLOBAL_APP_ID, cSCheckImp, m_global_app_id, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
#endif
	mpLOCAL_PROPERTY_BUF( pgDSKM_STORED_ERROR, cSCheckImp, m_dskm_last_err, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_FN( pgPR_STORED_ERROR, FN_CUST(GetPrErr), NULL )
mpPROPERTY_TABLE_END(SCheck_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(SCheck)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(SCheck)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(SCheck)
	mEXTERNAL_METHOD(SCheck, InitParamByObj)
	mEXTERNAL_METHOD(SCheck, InitParamByMask)
	mEXTERNAL_METHOD(SCheck, InitCompleted)
	mEXTERNAL_METHOD(SCheck, CheckObj)
	mEXTERNAL_METHOD(SCheck, CheckObjByMask)
	mEXTERNAL_METHOD(SCheck, CheckObjByList)
	mEXTERNAL_METHOD(SCheck, GetObjHash)
mEXTERNAL_TABLE_END(SCheck)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "SCheck::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_SCHECK,                             // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		SCheck_VERSION,                         // interface version
		VID_PETROVITCH,                         // interface developer
		&i_SCheck_vtbl,                         // internal(kernel called) function table
		(sizeof(i_SCheck_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_SCheck_vtbl,                         // external function table
		(sizeof(e_SCheck_vtbl)/sizeof(tPTR)),   // external function table size
		SCheck_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(SCheck_PropTable), // property table size
		sizeof(cSCheckImp)-sizeof(cObjImpl),    // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ldr\tSCheck(IID_SCHECK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

extern "C" tERROR pr_call SCheck_Register( hROOT root ) { return cSCheckImp::Register(root); }
// AVP Prague stamp end



