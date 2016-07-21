// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  26 October 2005,  13:08 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- scheck_list.cpp
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define SCheckList_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Dskm/dskm.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end

#include <stdlib.h>

#define  IMPEX_EXPORT_LIB
#include <dskmparm.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/e_loader.h>
#include <Stuff/cpointer.h>
#include "loader.h"


#define DSKM_PROP_ObjTypeId 100
#define CHECK_BUFF_SIZE     0x1000



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cSCheckListImpl : public cSCheckList /*cObjImpl*/ {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call GetPrStoredError( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call AddObj( tDWORD* p_obj_id, hOBJECT p_obj );
	tERROR pr_call AddObjByMask( tDWORD* p_obj_id, tDWORD* p_obj_count, const tVOID* p_mask, tCODEPAGE p_mask_cp );
	tERROR pr_call GetFirstObjId( tDWORD* p_obj_id );
	tERROR pr_call GetNextObjId( tDWORD* p_obj_id, tDWORD p_prev_obj_id );
	tERROR pr_call GetObjProp( tDWORD p_obj_id, tDWORD p_prop_id, tVOID* p_data, tDWORD* p_data_size );
	tERROR pr_call SetObjProp( tDWORD p_obj_id, tDWORD p_prop_id, const tVOID* p_prop_data, tDWORD p_prop_data_size );
	tERROR pr_call SetObjHashingProp( tDWORD p_obj_id, const tVOID* p_prop_data, tDWORD p_prop_data_size );

// Data declaration
	HDSKMLIST m_dskm_list;        // dskm list handle
	tBOOL     m_dskm_list_owner;  // cTRUE will force release on PreClose
	tCHAR     m_dskm_comp_id[100]; // dskm component id
	tCHAR     m_dskm_obj_type[100]; // --
	tDWORD    m_dskm_last_err;    // stored DSKM error
// AVP Prague stamp end



	tBYTE*    m_check_buff;
    HDSKM     m_dskm;

private:
	tERROR _parse_obj( hOBJECT obj, pfnDSKM_GetBuffer_CallBack* pfunc, tVOID** param );
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cSCheckListImpl)
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "SCheckList". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cSCheckListImpl::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "scheck_list::ObjectInitDone method" );

	#if defined( NEW_SIGN_LIB )
		if ( !m_dskm_list ) {
            if ((m_dskm_last_err = ::DSKM_ParList_Create( &m_dskm_list )) == DSKM_ERR_NOT_INITIALIZED)
            {
			    m_dskm = ::DSKM_InitLibraryEx( malloc, free, 0, cTRUE );
			    m_dskm_list_owner = cTRUE;
			    m_dskm_last_err = ::DSKM_ParList_Create( &m_dskm_list );
			    if ( DSKM_NOT_OK(m_dskm_last_err) ) {
				    CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );

                    if (m_dskm) {
                        DSKM_DeInitLibrary(m_dskm, cFALSE) ;
                    }
				    return error;
                }
			}
            else
            {
                if ( DSKM_NOT_OK(m_dskm_last_err) ) {
				    CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
                    return error ;
                }
            }
		}
		else
			m_dskm_last_err = DSKM_ERR_OK;

		if ( *m_dskm_comp_id && !DSKM_ParList_AddStrParam(m_dskm_list,DSKM_KEYFILTER_PARAM_COMPID_ID,m_dskm_comp_id) )
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
		if ( DSKM_OK(m_dskm_last_err) && *m_dskm_obj_type && !DSKM_ParList_AddStrParam(m_dskm_list,DSKM_KEYFILTER_PARAM_TYPE_ID,m_dskm_obj_type) )
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
		if ( DSKM_NOT_OK(m_dskm_last_err) ) {
			CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );

            if (m_dskm) {
                DSKM_DeInitLibrary(m_dskm, cFALSE) ;
            }
			return error;
		}
		error = heapAlloc( (tPTR*)&m_check_buff, CHECK_BUFF_SIZE );
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
tERROR cSCheckListImpl::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "scheck_list::ObjectPreClose method" );

	#if defined( NEW_SIGN_LIB )
		if ( m_dskm_list ) {
			m_dskm_last_err = _dskm_ParList_release( m_dskm_list );
			if ( DSKM_OK(m_dskm_last_err) )
				m_dskm_list = 0;
			else {
				CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
			}
		}
		else
			m_dskm_last_err = DSKM_ERR_OK;
		if ( m_check_buff ) {
			heapFree( m_check_buff );
			m_check_buff = 0;
		}

        if (m_dskm) {
            DSKM_DeInitLibrary(m_dskm, cFALSE) ;
        }
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetPrStoredError )
// Interface "scheck_list". Method "Get" for property(s):
//  -- PR_STORED_ERROR
tERROR cSCheckListImpl::GetPrStoredError( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetPrStoredError for property pgPR_STORED_ERROR" );

	#if defined( NEW_SIGN_LIB )
		if ( buffer ) {
			if ( size >= sizeof(tERROR) ) {
				CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
				*out_size = sizeof(tERROR);
			}
			else {
				error = errBUFFER_TOO_SMALL;
				*out_size = 0;
			}
		}
		else
			*out_size = sizeof(tERROR);
	#else
		error = errNOT_SUPPORTED;
		*out_size = 0;
	#endif

	PR_TRACE_A2( this, "Leave *GET* method GetPrStoredError for property pgPR_STORED_ERROR, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// ---
static inline tERROR _get_unicode_name( cObj* obj, tWCHAR*& name, tDWORD& size ) {
	cERROR err = obj->propGetStr( &size, pgOBJECT_NAME, name, size, cCP_UNICODE );
	if ( err == errBUFFER_TOO_SMALL ) {
		err = obj->heapAlloc( (tPTR*)&name, size );
		if ( PR_SUCC(err) )
			err = obj->propGetStr( &size, pgOBJECT_NAME, name, size, cCP_UNICODE );
	}
	return err;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddObj )
// Parameters are:
tERROR cSCheckListImpl::AddObj( tDWORD* p_obj_id, hOBJECT p_obj ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "scheck_list::AddObject method" );

	#if defined( NEW_SIGN_LIB )

		tWCHAR	fname[MAX_PATH];
		tWCHAR* pname = fname;
		tUINT   name_len = 0;
		
		cDSKMObjParams* params;
		
		if ( p_obj_id )
			*p_obj_id = 0;

		if ( !p_obj ) {
			m_dskm_last_err = DSKM_ERR_INVALID_PARAMETER;
			return errPARAMETER_INVALID;
		}
		m_dskm_last_err = DSKM_ERR_OK;
		
		try {
			error = sysCheckObject( p_obj, IID_SEQIO );
			
			if ( PR_SUCC(error) ) {
				name_len = sizeof(fname);
				error = _get_unicode_name( p_obj, pname, name_len );
				if ( PR_FAIL(error) ) {
					m_dskm_last_err = DSKM_UNKNOWN_ERROR;
					return error;
				}
#if defined (_WIN32)
				_wcslwr_s( pname, wcslen(pname) + 1 );
#endif // WIN32
				PR_TRACE(( this, prtIMPORTANT, "ldr\tcSCheckList::AddObject(%S)...", pname ));
				error = _alloc_param( *this, &params, op_seqio, p_obj, 0, cCP_NULL, 0 );
			}
			else if ( error == errHANDLE_INVALID ) {
				switch( _is_file_handle(p_obj) ) {
					case ft_unk        :
						error = _alloc_param( *this, &params, op_file, 0, MB(p_obj), cCP_ANSI, _toui32((1+strlen(MB(p_obj)))*MBS) );
						break;
						
					case ft_disk_file  :
						m_dskm_last_err = DSKM_ERR_OK;
						error = _alloc_param( *this, &params, op_file, p_obj, 0, cCP_NULL, 0 );
						break;
						
					case ft_other_file :
					default            :
						m_dskm_last_err = DSKM_ERR_INVALID_PARAMETER;
						error = errPARAMETER_INVALID;
						break;
				}
			}
			
			else if ( error != errINTERFACE_INCOMPATIBLE ) {
				m_dskm_last_err = DSKM_UNKNOWN_ERROR;
				return error;
			}
			
			else if ( PR_SUCC(sysCheckObject(p_obj,IID_IO,PID_ANY,0,cFALSE)) ) {
				name_len = sizeof(fname);
				error = _get_unicode_name( p_obj, pname, name_len );
				if ( PR_FAIL(error) ) {
					m_dskm_last_err = DSKM_UNKNOWN_ERROR;
					return error;
				}
#if defined (_WIN32)
				_wcslwr_s( pname, wcslen(pname) + 1 );
#endif //_WIN32
				PR_TRACE(( this, prtIMPORTANT, "ldr\tcSCheckList::AddObject(%S)...", pname ));
				error = _alloc_param( *this, &params, op_io, p_obj, 0, cCP_NULL, 0 );
			}
			
			else if ( PR_SUCC(sysCheckObject(p_obj,IID_STRING,PID_ANY,0,cFALSE)) ) {
				
				tDWORD  name_size = CHECK_BUFF_SIZE;
				tWCHAR* name_ptr = (tWCHAR*)m_check_buff;
				error = _get_unicode_name( p_obj, name_ptr, name_size );
				if ( PR_FAIL(error) )
					m_dskm_last_err = DSKM_UNKNOWN_ERROR;
				if ( PR_SUCC(error) )
					error = _alloc_param( *this, &params, op_file, 0, m_check_buff, cCP_UNICODE, name_size );

				if ( PR_SUCC(error) ) {
					name_len = _receive_unicode_file_name( m_check_buff, cCP_UNICODE, pname, sizeof(fname) );
					PR_TRACE(( this, prtIMPORTANT, "ldr\tcSCheckList::AddObject(%S)...", pname ));
				}
			}
			
			else {
				m_dskm_last_err = DSKM_UNKNOWN_ERROR;
				return errOBJECT_INCOMPATIBLE;
			}

			if ( PR_SUCC(error) ) {
				HDSKMLISTOBJ pobj = DSKM_ParList_AddBufferedObject( m_dskm_list, 0, m_check_buff, CHECK_BUFF_SIZE, _dskm_read_buff, params );
				if ( pobj ) {
					if ( p_obj_id )
						*p_obj_id = DSKM_ParList_GetObjectId( m_dskm_list, pobj );
					if ( name_len ) {
						tPTR data = pname;
						tUINT size = name_len/sizeof(tWCHAR);
#if defined (__unix__)					
						data = toUCS2LE(data,size);
#endif
						m_dskm_last_err = DSKM_ParList_SetObjectHashingProp( m_dskm_list, pobj, data, size*sizeof(tSHORT) );
					}
					params = 0;
				}
				else
					m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
			}
			else
				m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			PR_TRACE(( this, prtIMPORTANT, "ldr\tcSCheckList::AddObject(%s) completed with %terr", pname, error));
		}
		catch(...) {
			m_dskm_last_err = DSKM_UNKNOWN_ERROR;
			error = errUNEXPECTED;
		}
		if ( pname != fname )
			p_obj->heapFree( pname );
		if ( params ) {
			_free_param( params );
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddObjByMask )
// Parameters are:
tERROR cSCheckListImpl::AddObjByMask( tDWORD* p_obj_id, tDWORD* p_obj_count, const tVOID* p_mask, tCODEPAGE p_mask_cp ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheckList::AddObjectByMask method" );

	#if defined(NEW_SIGN_LIB)
		tDSKMAddObjByMask param = { *this, m_dskm_list, 0, m_check_buff, CHECK_BUFF_SIZE };
		error = _find_files_by_mask( p_mask, p_mask_cp, p_obj_count, _add_check_obj, &param );
		if ( p_obj_id ) {
			if ( PR_SUCC(error) )
				*p_obj_id = param.m_obj_id;
			else
				*p_obj_id = 0;
		}
		CONVERT_DSKM_TO_PR_ERR( m_dskm_last_err, error );
	#else
		error = errNOT_SUPPORTED;
	#endif
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetFirstObjId )
// Parameters are:
tERROR cSCheckListImpl::GetFirstObjId( tDWORD* p_obj_id ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheckList::GetFirstObjId method" );

	#if defined( NEW_SIGN_LIB )
		tDWORD obj_id;
		if ( !p_obj_id )
			p_obj_id = &obj_id;

		HDSKMLISTOBJ obj = DSKM_ParList_GetFirstObject( m_dskm_list );
		if ( !obj ) {
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
			*p_obj_id = 0;
			if ( DSKM_OK(m_dskm_last_err) )
				return error = errEND_OF_THE_LIST;
			return _convertSignErr( m_dskm_last_err );
		}
		m_dskm_last_err = DSKM_ERR_OK;
		*p_obj_id = DSKM_ParList_GetObjectId( m_dskm_list, obj );
	#else
		error = errNOT_SUPPORTED;
	#endif
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetNextObjId )
// Parameters are:
tERROR cSCheckListImpl::GetNextObjId( tDWORD* p_obj_id, tDWORD p_prev_obj_id ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "SCheckList::GetNextObjId method" );

	#if defined( NEW_SIGN_LIB )
		tDWORD obj_id;
		if ( !p_obj_id )
			p_obj_id = &obj_id;
		
		HDSKMLISTOBJ obj = DSKM_ParList_GetObject( m_dskm_list, p_prev_obj_id, 0, 0 );
		if ( obj )
			obj = DSKM_ParList_GetNextObject( m_dskm_list, obj );
		if ( !obj ) {
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
			*p_obj_id = 0;
			if ( DSKM_OK(m_dskm_last_err) || (m_dskm_last_err == DSKM_ERR_OBJECT_NOT_FOUND) )
				return error = errEND_OF_THE_LIST;
			return _convertSignErr( m_dskm_last_err );
		}
		error = errOK;
		m_dskm_last_err = DSKM_ERR_OK;
		*p_obj_id = DSKM_ParList_GetObjectId( m_dskm_list, obj );
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetObjProp )
// Parameters are:
tERROR cSCheckListImpl::GetObjProp( tDWORD p_obj_id, tDWORD p_prop_id, tVOID* p_data, tDWORD* p_data_size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "scheck_list::GetObjProp method" );

	#if defined( NEW_SIGN_LIB )
		HDSKMLISTOBJ obj = DSKM_ParList_GetObject( m_dskm_list, p_obj_id, 0, 0 );
		if ( obj )
			m_dskm_last_err = DSKM_ParList_GetObjectProp( m_dskm_list, obj, p_prop_id, p_data, (AVP_dword*)&p_data_size );
		else
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
		error = _convertSignErr( m_dskm_last_err );
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetObjProp )
// Parameters are:
tERROR cSCheckListImpl::SetObjProp( tDWORD p_obj_id, tDWORD p_prop_id, const tVOID* p_prop_data, tDWORD p_prop_data_size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "scheck_list::SetObjProp method" );

	#if defined( NEW_SIGN_LIB )
		HDSKMLISTOBJ obj = DSKM_ParList_GetObject( m_dskm_list, p_obj_id, 0, 0 );
		if ( obj ) {
			m_dskm_last_err = DSKM_ERR_OK;
			DSKM_ParList_SetObjectProp( m_dskm_list, obj, p_prop_id, (tVOID*)p_prop_data, p_prop_data_size );
		}
		else {
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
			error = _convertSignErr( m_dskm_last_err );
		}
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetObjHashingProp )
// Parameters are:
tERROR cSCheckListImpl::SetObjHashingProp( tDWORD p_obj_id, const tVOID* p_prop_data, tDWORD p_prop_data_size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "scheck_list::SetObjHashingProp method" );

	#if defined( NEW_SIGN_LIB )
		HDSKMLISTOBJ obj = DSKM_ParList_GetObject( m_dskm_list, p_obj_id, 0, 0 );
		if ( obj ) {
			tPTR data = (tPTR)p_prop_data;
			tUINT size = p_prop_data_size/sizeof(tWCHAR);
#if defined (__unix__)						
			data = toUCS2LE(data,size);
#endif
			m_dskm_last_err = DSKM_ParList_SetObjectHashingProp( m_dskm_list, obj, data, size*sizeof(tSHORT));
		}
		else
			m_dskm_last_err = DSKM_ParList_GetLastError( m_dskm_list );
		error = _convertSignErr( m_dskm_last_err );
	#else
		error = errNOT_SUPPORTED;
	#endif

	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "SCheckList". Register function
tERROR cSCheckListImpl::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(SCheckList_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, SCheckList_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "scheck_list interface", 22 )
	mpLOCAL_PROPERTY_BUF( pgDSKM_LIST, cSCheckListImpl, m_dskm_list, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgDSKM_STORED_ERROR, cSCheckListImpl, m_dskm_last_err, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_FN( pgPR_STORED_ERROR, FN_CUST(GetPrStoredError), NULL )
	mpLOCAL_PROPERTY_BUF( pgDSKM_COMP_ID, cSCheckListImpl, m_dskm_comp_id, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgDSKM_OBJ_TYPE, cSCheckListImpl, m_dskm_obj_type, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plDSKM_LIST_OWNER, cSCheckListImpl, m_dskm_list_owner, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(SCheckList_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(SCheckList)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(SCheckList)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(SCheckList)
	mEXTERNAL_METHOD(SCheckList, AddObj)
	mEXTERNAL_METHOD(SCheckList, AddObjByMask)
	mEXTERNAL_METHOD(SCheckList, GetFirstObjId)
	mEXTERNAL_METHOD(SCheckList, GetNextObjId)
	mEXTERNAL_METHOD(SCheckList, GetObjProp)
	mEXTERNAL_METHOD(SCheckList, SetObjProp)
	mEXTERNAL_METHOD(SCheckList, SetObjHashingProp)
mEXTERNAL_TABLE_END(SCheckList)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "scheck_list::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_SCHECKLIST,                         // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		SCheckList_VERSION,                     // interface version
		VID_PETROVITCH,                         // interface developer
		&i_SCheckList_vtbl,                     // internal(kernel called) function table
		(sizeof(i_SCheckList_vtbl)/sizeof(tPTR)),// internal function table size
		&e_SCheckList_vtbl,                     // external function table
		(sizeof(e_SCheckList_vtbl)/sizeof(tPTR)),// external function table size
		SCheckList_PropTable,                   // property table
		mPROPERTY_TABLE_SIZE(SCheckList_PropTable),// property table size
		sizeof(cSCheckListImpl)-sizeof(cObjImpl),// memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ldr\tSCheckList(IID_SCHECK_LIST) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

extern "C" tERROR pr_call SCheckList_Register( hROOT root ) { return cSCheckListImpl::Register(root); }
// AVP Prague stamp end



