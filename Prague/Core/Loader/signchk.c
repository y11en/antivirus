#include <string.h>
#include <stdlib.h>

#include <dskmparm.h>

#include <Prague/prague.h>
#include <Prague/pr_loadr.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_scheck.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>



#include "loader.h"

#define  IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>


//#define SIGN_CHECK_PARAM_APP_ID   40001
//#define SIGN_CHECK_PARAM_APP_VAL  1165

//#define SIGN_CHECK_PARAM_COMP_ID  40002
//#define SIGN_CHECK_PARAM_COMP_VAL 78


// public method implemented in the module
tERROR IMPEX_FUNC(PrSignGetContentLen)( const tVOID* buff, tUINT buff_size, tUINT* content_len );
tERROR IMPEX_FUNC(SignCheck)( hOBJECT obj, tBOOL use_default_key, tVOID* keys[], tDWORD key_count, tBOOL all_of, tDWORD* sign_lib_error_code );
tERROR IMPEX_FUNC(SignGet)( hOBJECT obj, tVOID* keys[], tDWORD key_count, tDWORD* keys_found, tDWORD* sign_lib_error_code );


tSignCheckFunc   g_signCheckFunc;
tSignCheckNotify g_signCheckNotify;

extern tBOOL g_bUnderNT;

#if defined( NEW_SIGN_LIB )
	static tWCHAR g_dskm_main_folder[MAX_PATH] = {0};
#endif

#include <signchk_inc.c>


#if defined( NEW_SIGN_LIB )

	// ---
	tERROR pr_call _obj_alloc( hOBJECT obj, tPTR* ptr, tUINT size ) {
		if ( obj )
			return CALL_SYS_ObjHeapAlloc( obj, ptr, size );
		*ptr = _os_alloc( size );
		if ( !*ptr )
			return errNOT_ENOUGH_MEMORY;
		return errOK;
	}

	// ---
	tVOID pr_call _obj_free( hOBJECT obj, tPTR ptr ) {
		if ( obj )
			CALL_SYS_ObjHeapFree( obj, ptr );
		else
			_os_free( ptr );
	}


	// ---
	tERROR pr_call _alloc_param( hOBJECT obj, cDSKMObjParams** par, tDSKMObjParamType type, tVOID* val, const tVOID* name, tCODEPAGE cp, tUINT size ) {
		tERROR e;
		if ( !par )
			return errPARAMETER_INVALID;
		switch( type ) {
			case op_io:
			case op_seqio:
			case op_file:
				break;
			default:
				return errPARAMETER_INVALID;
		}

		e = _obj_alloc( obj, (tPTR*)par, sizeof(cDSKMObjParams) );
		if ( PR_FAIL(e) )
			return e;

		(*par)->m_type = type; 
		(*par)->m_allocator = obj;
		(*par)->m_close_it = cFALSE;

		switch( type ) {
			case op_io:     (*par)->m_io.io = (hIO)val; (*par)->m_io.offset = 0; break;
			case op_seqio:  (*par)->m_seq_io = (hSEQ_IO)val; break;
			case op_file:   (*par)->m_file = (kl_file_t)val; (*par)->m_close_it = val ? cFALSE : cTRUE; break;
				break;
		}
		if ( name ) {
			e = _obj_alloc( obj, &(*par)->m_name, size );
			if ( PR_FAIL(e) ) {
				_obj_free( obj, *par );
				*par = 0;
				return e;
			}
			memcpy( (*par)->m_name, name, size );
			(*par)->m_name_size = size;
			(*par)->m_name_cp = cp;
		}
		else {
			(*par)->m_name = 0;
			(*par)->m_name_size = 0;
			(*par)->m_name_cp = cCP_NULL;
		}

		return errOK;
	}
	
	// ---
	tVOID _destruct_param( cDSKMObjParams* par ) {
		if ( (par->m_type == op_file) && par->m_close_it && par->m_file && (par->m_file != INVALID_FILE) )
			closeFile( par->m_file );
		if ( par->m_name )
			_obj_free( par->m_allocator, par->m_name );
		par->m_file = 0;
		par->m_type = op_unk;
		par->m_name = 0;
		par->m_name_size = 0;
		par->m_name_cp = cCP_NULL;
		par->m_io.io = 0;
		par->m_io.offset = 0;
		par->m_close_it = cFALSE;
	}


	// ---
	int SIGN_API _dskm_read_buff( void* params, void* pvBuffer, AVP_dword dwBufferSize, AVP_dword dwCommand ) {
		cDSKMObjParams* par = (cDSKMObjParams*)params;
		if ( !par )
			return -1;
		switch( par->m_type ) {
			case op_io :
				switch( dwCommand ) {
					case DSKM_CB_OPEN  :
						par->m_io.offset = 0;
						return 0;
					case DSKM_CB_READ  :
						if ( PR_SUCC(CALL_IO_SeekRead(par->m_io.io,&dwBufferSize,par->m_io.offset,pvBuffer,dwBufferSize)) ) {
							par->m_io.offset += dwBufferSize;
							return dwBufferSize;
						}
						return -1;
					case DSKM_CB_WRITE : return -1;
					case DSKM_CB_CLOSE : return 0;
				}
				break;
			case op_seqio :
				switch( dwCommand ) {
					case DSKM_CB_OPEN  :
						if ( PR_SUCC(CALL_SeqIO_SeekSet(par->m_seq_io,0,0)) )
							return 0;
						return -1;
					case DSKM_CB_READ  :
						if ( PR_SUCC(CALL_SeqIO_Read(par->m_seq_io,&dwBufferSize,pvBuffer,dwBufferSize)) )
							return dwBufferSize;
						return -1;
					case DSKM_CB_WRITE : return -1;
					case DSKM_CB_CLOSE : return 0;
				}
				break;
			case op_file  :
				switch( dwCommand ) {
					case DSKM_CB_OPEN :
						if ( !par->m_file ) {
							if ( !par->m_name || (par->m_name_cp == cCP_NULL) )
								return -1;
							if ( PR_FAIL(_open_file(par->m_name,par->m_name_cp,&par->m_file)) )
								return -1;
						}
						if ( !par->m_file || (par->m_file == INVALID_FILE) || PR_FAIL(_set_file_cp(par->m_file,0)) )
							return -1;
						return 0;
					case DSKM_CB_READ  :
						if ( par->m_file && (par->m_file != INVALID_FILE) )
							return _old_read_buff_by_file( par->m_file, pvBuffer, dwBufferSize );
						return -1;
					case DSKM_CB_WRITE : return -1;
					case DSKM_CB_CLOSE :
						if ( par->m_file ) {
							if ( par->m_file != INVALID_FILE )
								closeFile( par->m_file );
							par->m_file = 0;
						}
						return 0;
				}
				break;
		}
		return -1;
	}

	// ---
	tDWORD SIGN_API _dskm_ParList_release( HDSKMLIST list ) {
		HDSKMLISTOBJ o;
		if ( !list )
			return DSKM_ERR_INVALID_PARAMETER;
		o = DSKM_ParList_GetFirstObject( list );
		while( o ) {
			cDSKMObjParams* par = 0;
			tDWORD          s = sizeof(cDSKMObjParams*);
			tDWORD          e = DSKM_ParList_GetObjectProp( list, o, DSKM_OBJ_EXTERNAL_PARAM_PTR, &par, &s );
			if ( DSKM_OK(e) && par ) {
				_free_param( par );
			}
			o = DSKM_ParList_GetNextObject( list, o );
		}
		return DSKM_ParList_Delete( list );
	}

	// ---
	int pr_call _convertFileErrorToSignError( tERROR err ) {
		switch( err ) {
			case errOBJECT_NOT_FOUND :
			case errACCESS_DENIED    : 
			default                  : return DSKM_UNKNOWN_ERROR;
		}
	}

	
	// ---
	tERROR pr_call _convertSignErr( tDWORD dskm_err ) {
		tERROR pr_err;
		CONVERT_DSKM_TO_PR_ERR( dskm_err, pr_err );
		return pr_err;
	}
	

	//DSKM_Io* InitIoW();
	//DSKM_Io* InitIoA();


	// ---
	// main session
	static tDSKMSession g_dskm_main;

	// ---
	tVOID  pr_call  _init_dskm_main_sync( tVOID ) {
		CALL_SYS_ObjectCreateQuick( g_root, (hOBJECT)&g_dskm_main.m_prot, IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT, 0 );
	}

	// ---
	tVOID  pr_call _deinit_dskm_main_sync( tVOID ) {
		if ( g_dskm_main.m_prot ) {
			CALL_SYS_ObjectClose( g_dskm_main.m_prot );
			g_dskm_main.m_prot = 0;
		}
	}




	// ---
	tERROR pr_call  _free_dskm_main_session() {
		if ( g_dskm_main.m_prot )
			CALL_CriticalSection_Enter( g_dskm_main.m_prot, SHARE_LEVEL_WRITE );
		if ( !g_dskm_main.m_lib ) {
			if (g_dskm_main.m_prot)
				CALL_CriticalSection_Leave( g_dskm_main.m_prot, 0 );
			return errOK;
		}
		DSKM_DeInitLibrary( g_dskm_main.m_lib, cFALSE );
		g_dskm_main.m_lib = 0;
		if ( g_dskm_main.m_prot )
			CALL_CriticalSection_Leave( g_dskm_main.m_prot, 0 );
		return errOK;
	}



	// ---
	tERROR pr_call _reinit_dskm_main_session( const tVOID* new_folder, cCODEPAGE cp, tDWORD* dskm_err ) {
		tERROR err;
		_free_dskm_main_session();
		if ( g_dskm_main.m_prot )
			CALL_CriticalSection_Enter( g_dskm_main.m_prot, SHARE_LEVEL_WRITE );
		if ( new_folder && *MB(new_folder) ) {
			if ( g_bUnderNT ) {
				if ( cp == cCP_UNICODE )
					wcscpy_s( g_dskm_main_folder, countof(g_dskm_main_folder), UNI(new_folder) );
				else
					mbstowcs_s( NULL, g_dskm_main_folder, countof(g_dskm_main_folder), MB(new_folder), countof(g_dskm_main_folder) );
			}
			else if ( cp == cCP_UNICODE )
				wcstombs_s( NULL, MB(g_dskm_main_folder), sizeof(g_dskm_main_folder), UNI(new_folder), sizeof(g_dskm_main_folder) );
			else
				strcpy_s( MB(g_dskm_main_folder), sizeof(g_dskm_main_folder), MB(new_folder) );
		}
		err = _init_dskm_session( (hOBJECT)g_root, &g_dskm_main.m_lib, g_dskm_main_folder, g_bUnderNT ? cCP_UNICODE : cCP_ANSI, dskm_err );
		if ( g_dskm_main.m_prot )
			CALL_CriticalSection_Leave( g_dskm_main.m_prot, 0 );
		return err;
	}



	// ---
	tERROR pr_call _get_dskm_main_session( tDSKMSession* session, tDWORD* dskm_err ) {

		if ( !session )
			return errPARAMETER_INVALID;

		session->m_lib = 0;
		session->m_prot = g_dskm_main.m_prot;
		if ( session->m_prot )
			CALL_CriticalSection_Enter( session->m_prot, SHARE_LEVEL_READ );

		if ( !g_dskm_main.m_lib ) {
			tERROR err = errOK;
			if ( !g_dskm_main_folder[0] )
				_init_dskm_main_folder();
			if ( session->m_prot ) {
#if defined (__unix__)
				CALL_CriticalSection_Leave( session->m_prot, 0 );
#endif
				CALL_CriticalSection_Enter( session->m_prot, SHARE_LEVEL_WRITE );
                        }
			err = _init_dskm_session( (hOBJECT)g_root, &g_dskm_main.m_lib, g_dskm_main_folder, g_bUnderNT ? cCP_UNICODE : cCP_ANSI, dskm_err );
#if defined (_WIN32)
			if ( session->m_prot )
				CALL_CriticalSection_Leave( session->m_prot, 0 );
#endif
			if ( PR_FAIL(err) ) {
				if ( session->m_prot )
					CALL_CriticalSection_Leave( session->m_prot, 0 );
				return err;
			}
		}
		session->m_lib = g_dskm_main.m_lib;
		if ( dskm_err )
			*dskm_err = DSKM_ERR_OK;
		return errOK;
	}



	// ---
	tERROR pr_call _release_dskm_main_session( tDSKMSession* session ) {
		if ( session->m_prot )
			CALL_CriticalSection_Leave( session->m_prot, 0 );
		session->m_lib = 0;
		session->m_prot = 0;
		return errOK;
	}
	


	// ---
	tERROR _init_dskm_session( hOBJECT pr_obj, HDSKM* lib, const tVOID* dskm_path, cCODEPAGE dskm_path_cp, tDWORD* dskm_err ) {

		tDWORD l_dskm_err;
		tERROR pr_err = errOK;

		if ( !dskm_err )
			dskm_err = &l_dskm_err;
		*dskm_err = DSKM_ERR_OK;

		if ( !*lib ) {
			HDSKMLIST list;
			tBYTE     buff[0x1000];

			*lib = DSKM_InitLibraryEx( malloc, free, 0/*g_bUnderNT ? InitIoW() : InitIoA()*/, cTRUE );
			if ( !*lib ) {
				*dskm_err = DSKM_UNKNOWN_ERROR;
				return errUNEXPECTED;
			}

			*dskm_err = DSKM_ParList_Create( &list );
			if ( DSKM_NOT_OK(*dskm_err) )
				pr_err = _convertSignErr( *dskm_err );

			if ( PR_SUCC(pr_err) ) {
				tDSKMAddObjByMask param = { pr_obj, list, 0, buff, sizeof(buff) };
				tWCHAR  mask[MAX_PATH];
				tUINT   len = _toui32(wcslen(dskm_path));
				if ( dskm_path_cp == cCP_UNICODE ) {
					wcscpy_s( mask, countof(mask), dskm_path );
					len = _toui32(wcslen(mask));
					if ( len && (PATH_DELIMETER_WIDECHAR != mask[len-1]) ) {
						mask[len++] = PATH_DELIMETER_WIDECHAR;
						mask[len] = 0;
					}
					wcscat_s( mask, countof(mask), L"*.*" );
				}
				else {
					strcpy_s( MB(mask), sizeof(mask), MB(dskm_path) );
					len = _toui32(strlen(MB(mask)));
					if ( len && (PATH_DELIMETER_CHAR != MB(mask)[len-1]) ) {
						MB(mask)[len++] = PATH_DELIMETER_CHAR;
						MB(mask)[len] = 0;
					}
					strcat_s( MB(mask), sizeof(mask), "*.*" );
				}
				pr_err = _find_files_by_mask( mask, cCP, 0, _add_reg_by_mask, &param );
			}
			if ( PR_SUCC(pr_err) && g_app_id && !DSKM_ParList_AddStrParam(list,DSKM_KEYFILTER_PARAM_APPID_ID,g_app_id->m_szId) )
				*dskm_err = DSKM_ParList_GetLastError( list );
			if ( DSKM_OK(*dskm_err) && PR_SUCC(pr_err) )
				*dskm_err = DSKM_PrepareRegsSet( *lib, list, 0/*DSKM_EXE_OBJ_TYPE*/ ); 
			if ( PR_SUCC(pr_err) && DSKM_NOT_OK(*dskm_err) )
				pr_err = _convertSignErr( *dskm_err );
			_dskm_ParList_release( list );
			if ( PR_FAIL(pr_err) ) {
				DSKM_DeInitLibrary( *lib, cFALSE );
				*lib = 0;
			}
		}
		return pr_err;
	}



	// ---
	const tDSKMCompId* pr_call _find_comp_id( const tWCHAR* plugin ) {
		tUINT              i, c;
		const tDSKMCompId* pcomp;
		tCHAR              pname[DSKM_STR_ID_SIZE] = {0,};

		if ( !g_app_id )
			return 0;
		for( i=0,c=g_app_id->m_dwCompCount,pcomp=g_app_id->m_comps; i<c; ++i,++pcomp ) {
			tUINT              j, k;
			const tDSKMFileId* one;
			for( j=0,k=pcomp->m_dwFileIdCount,one=pcomp->m_fileIds; j<k; ++j,++one ) {
				switch ( one->m_type ) {
					case fdt_ansi_mask :
						if ( !pname[0] )
							wcstombs_s( NULL, pname, countof(pname), plugin, sizeof(pname) );
						if ( !strcmp(MB(one->m_ptr),pname) )
							return pcomp;
						break;
					case fdt_unicode_mask:
						if ( !wcscmp(UNI(one->m_ptr),plugin) )
							return pcomp;
						break;
					case fdt_method :
						if ( ((pfnSelectFile)one->m_ptr)(plugin,cCP_UNICODE) )
							return pcomp;
					default :
						break;
				}
			}
		}
		return 0;
	}
	


	// ---
	tERROR pr_call _add_reg_by_mask( const tVOID* name, tCODEPAGE cp, tUINT len, tVOID* param ) {
		
		tDSKMAddObjByMask* par = (tDSKMAddObjByMask*)param;
		cDSKMObjParams*    obj_param = 0;
		tERROR             err = _alloc_param( par->m_obj, &obj_param, op_file, 0, name, cp, len );

		if ( PR_SUCC(err) ) {
			HDSKMLISTOBJ obj = DSKM_ParList_AddBufferedReg( par->m_list, 0, par->m_buff, par->m_buff_len, _dskm_read_buff, obj_param );
			if ( !obj ) {
				tDWORD dskm_err = DSKM_ParList_GetLastError( par->m_list );
				CONVERT_DSKM_TO_PR_ERR( dskm_err, err );
			}
		}
		return err;
	}
	



	// ---
	tERROR pr_call _add_check_obj( const tVOID* name, tCODEPAGE cp, tUINT len, tVOID* param ) {
		tDSKMAddObjByMask* par = (tDSKMAddObjByMask*)param;
		tERROR pr_err;
		tUINT  fname_len;
		tWCHAR fname[MAX_PATH] = {0};
		tDWORD dskm_err = DSKM_ERR_OK;
		cDSKMObjParams* obj_param = 0;
		const tDSKMCompId* comp = 0;
		tPTR data = 0;
		tUINT size = 0;
		
		fname_len = _receive_unicode_file_name( name, cp, fname, sizeof(fname) );
		if ( !fname_len )
			return errUNEXPECTED;
		
		pr_err = _alloc_param( par->m_obj, &obj_param, op_file, 0, name, cp, len );
		if ( PR_FAIL(pr_err) )
			return pr_err;

		if ( g_app_id )
			comp = _find_comp_id( fname );

		if ( DSKM_OK(dskm_err) ) {
			HDSKMLISTOBJ list_obj = DSKM_ParList_AddBufferedObject( par->m_list, 0, par->m_buff, par->m_buff_len, _dskm_read_buff, obj_param );
			if ( list_obj ) {
				if ( !par->m_obj_id )
					par->m_obj_id = DSKM_ParList_GetObjectId( par->m_list, list_obj );
				data = fname;
				size = fname_len/sizeof(tWCHAR);
#if defined (__unix__)				
				data = toUCS2LE(data,size);
#endif
				dskm_err = DSKM_ParList_SetObjectHashingProp( par->m_list, list_obj, data, size*sizeof(tSHORT));
				if ( DSKM_OK(dskm_err) && comp && !DSKM_ParList_AddObjectStrParam(par->m_list,list_obj,DSKM_KEYFILTER_PARAM_COMPID_ID,comp->m_szId) )
					dskm_err = DSKM_ParList_GetLastError( par->m_list );
				if ( DSKM_OK(dskm_err) && comp && !DSKM_ParList_AddObjectStrParam(par->m_list,list_obj,DSKM_KEYFILTER_PARAM_TYPE_ID,comp->m_szType) )
					dskm_err = DSKM_ParList_GetLastError( par->m_list );
			}
			else
				dskm_err = DSKM_ParList_GetLastError( par->m_list );
			if ( DSKM_NOT_OK(dskm_err) ) {
				if ( list_obj )
					DSKM_ParList_RemoveObjectByH( par->m_list, list_obj );
				_free_param( obj_param );
			}
		}
		return _convertSignErr( dskm_err );
	}
	



	// ---
	static tBOOL _is_single_obj_in_list( HDSKMLIST list ) {
		tUINT  count = 0;
		HDSKMLISTOBJ obj = DSKM_ParList_GetFirstObject( list );
		while( obj) {
			tDWORD dskm_err = DSKM_ParList_GetLastError( list );
			if ( DSKM_NOT_OK(dskm_err) )
				break;
			if ( ++count > 1 )
				break;
			obj = DSKM_ParList_GetNextObject( list, obj );
		}
		return count == 1;
	}



	// ---
	tERROR _check_list( HDSKM lib, hSCHECKLIST list, tDWORD* p_dskm_err ) {
		tDWORD       l_dskm_err = DSKM_ERR_NOT_INITIALIZED;
		tDSKMSession session = { 0, 0 };
		HDSKMLIST    dskm_list;
		tERROR       pr_err;

		if ( !p_dskm_err )
			p_dskm_err = &l_dskm_err;
		*p_dskm_err = DSKM_ERR_NOT_INITIALIZED;

		if ( lib ) {
			pr_err = errOK;
			session.m_lib = lib;
		}
		else
			pr_err = _get_dskm_main_session( &session, p_dskm_err );

		if ( PR_SUCC(pr_err) ) {
			pr_err = CALL_SYS_PropertyGet( list, 0, pgDSKM_LIST, &dskm_list, sizeof(dskm_list) );
			if ( PR_SUCC(pr_err) )
				*p_dskm_err = DSKM_CheckObjectsUsingRegsSet( session.m_lib, dskm_list, 0/*DSKM_EXE_OBJ_TYPE*/ );
			else {
				*p_dskm_err = DSKM_UNKNOWN_ERROR;
				PR_TRACE(( g_root, prtERROR, "ldr\tdskm - %terr, cannot get dskm main session", pr_err ));
			}

			if ( DSKM_OK(*p_dskm_err) ) {
				tDWORD id = DSKM_ParList_GetFirstObjectError( dskm_list, p_dskm_err );
				while( id && DSKM_OK(*p_dskm_err) )
					id = DSKM_ParList_GetNextObjectError( dskm_list, id, p_dskm_err );
			}
			else if ( (*p_dskm_err == DSKM_ERR_NOT_ALL_OBJECTS_FOUND) && _is_single_obj_in_list(dskm_list) )
				DSKM_ParList_GetFirstObjectError( dskm_list, p_dskm_err );
			
			_release_dskm_main_session( &session );

			CONVERT_DSKM_TO_PR_ERR( *p_dskm_err, pr_err );
		}
		PR_TRACE(( list, PR_TRACE_LEVEL(pr_err,prtNOTIFY,prtERROR), "ldr\tdskm - %terr, list", pr_err ));
		return pr_err;
	}
	
	

	// ---
	tERROR _check_by_mask_session( HDSKM dskm_session, const tVOID* mask, tCODEPAGE cp, tDWORD* dskm_err ) {
		tERROR pr_err;
		tDWORD files_found;
		tWCHAR buff[0x1000];
		tDSKMAddObjByMask obj_param = { (hOBJECT)g_root, 0, 0, (tCHAR*)buff, sizeof(buff) };
		tDSKMSession session = {0,0};
		PR_TRACE_VARS;

		if ( !dskm_session ) {
			pr_err = _get_dskm_main_session( &session, dskm_err );
			if ( PR_FAIL(pr_err) ) {
				PR_TRACE(( g_root, prtERROR, "ldr\tdskm - %terr, cannot get dskm main session", pr_err ));
				return pr_err;
			}
			dskm_session = session.m_lib;
		}

		*dskm_err = DSKM_ParList_Create( &obj_param.m_list );
		if ( DSKM_OK(*dskm_err) ) {
			files_found = 0;
			pr_err = _find_files_by_mask( mask, cp, &files_found, _add_check_obj, &obj_param );
			if ( PR_SUCC(pr_err) )
				*dskm_err = DSKM_CheckObjectsUsingRegsSet( dskm_session, obj_param.m_list, 0/*DSKM_EXE_OBJ_TYPE*/ );
			else
				*dskm_err = DSKM_UNKNOWN_ERROR;

			if ( DSKM_OK(*dskm_err) ) {
				tDWORD id = DSKM_ParList_GetFirstObjectError( obj_param.m_list, dskm_err );
				while( id && DSKM_OK(*dskm_err) )
					id = DSKM_ParList_GetNextObjectError( obj_param.m_list, id, dskm_err );
			}
			else if ( (*dskm_err == DSKM_ERR_NOT_ALL_OBJECTS_FOUND) && _is_single_obj_in_list(obj_param.m_list) )
				DSKM_ParList_GetFirstObjectError( obj_param.m_list, dskm_err );

			_dskm_ParList_release( obj_param.m_list );
		}

		_release_dskm_main_session( &session );
		CONVERT_DSKM_TO_PR_ERR( *dskm_err, pr_err );
		PR_TRACE(( g_root, PR_TRACE_COND_LEVEL(pr_err,prtNOTIFY,prtERROR), "ldr\tdskm - %terr, mask(%s%S)", pr_err, PR_TRACE_PREPARE(mask) ));
		return pr_err;
	}
	
	

	// ---
	tERROR _check_by_mask( const tVOID* mask, tCODEPAGE cp ) {
		tDWORD dskm_err;
		tDSKMSession session;
		tERROR pr_err = _get_dskm_main_session( &session, &dskm_err );
		if ( PR_FAIL(pr_err) ) {
			PR_TRACE(( g_root, prtERROR, "ldr\tdskm - %terr, cannot get dskm main session", pr_err ));
			return pr_err;
		}
		pr_err = _check_by_mask_session( session.m_lib, mask, cp, &dskm_err );
		_release_dskm_main_session( &session );
		return pr_err;
	}
	
	
	
	// ---
	tERROR IMPEX_FUNC(PrSignGetContentLen)( const tVOID* buff, tUINT buff_size, tUINT* content_len ) {
		if ( !buff || !buff_size || !content_len )
			return errPARAMETER_INVALID;
		
		*content_len = buff_size;
		return errOK;
	}
	

	
	// ---
	tERROR IMPEX_FUNC(SignCheck)( hOBJECT obj, tBOOL use_default_key, tVOID* keys[], tDWORD key_count, tBOOL all_of, tDWORD* sign_lib_error_code ) {

		tDWORD l_err;
		tERROR pr_err;
		tDSKMSession session;

		if ( !sign_lib_error_code )
			sign_lib_error_code = &l_err;

		if ( !obj || keys || key_count ) {
			*sign_lib_error_code = DSKM_ERR_INVALID_PARAMETER;
			return errPARAMETER_INVALID;
		}

		pr_err = _get_dskm_main_session( &session, sign_lib_error_code );
		if ( PR_FAIL(pr_err) ) {
			PR_TRACE(( g_root, prtERROR, "ldr\tdskm - %terr, cannot get dskm main session", pr_err ));
			return pr_err;
		}

		pr_err = PrSignCheckV2( session.m_lib, obj, 0, 0, sign_lib_error_code );
		_release_dskm_main_session( &session );
		return pr_err;
	}



	// ---
	tERROR pr_call _check_by_params( HDSKM dskm_lib, cDSKMObjParams* par, const tCHAR* szCompId, const tCHAR* szObjType, tDWORD* sign_lib_error_code ) {
		
		tERROR    error = errOK;
		tUINT     name_len = 0;
		tCHAR     buff[0x1000] = {0};
		tWCHAR		fname[MAX_PATH] = L"<unk>";
		tWCHAR*   pname = fname;
		HDSKMLIST list = 0;
		tDSKMSession session = {0,0};

		if ( !dskm_lib ) {
			error = _get_dskm_main_session( &session, sign_lib_error_code );
			if ( PR_FAIL(error) ) {
				PR_TRACE(( g_root, prtERROR, "ldr\tdskm - %terr, cannot get dskm main session", error ));
				return error;
			}
			dskm_lib = session.m_lib;
		}

		*sign_lib_error_code = DSKM_ParList_Create( &list );
		if ( DSKM_OK(*sign_lib_error_code) ) {

			error = CALL_SYS_PropertyGetStr( par->m_obj, &name_len, pgOBJECT_NAME, fname, sizeof(fname), cCP_UNICODE );
			if ( error == errBUFFER_TOO_SMALL ) {
				error = CALL_SYS_ObjHeapAlloc( par->m_obj, (tPTR*)&pname, name_len );
				if ( PR_SUCC(error) )
					error = CALL_SYS_PropertyGetStr( par->m_obj, &name_len, pgOBJECT_NAME, pname, name_len, cCP_UNICODE );
			}
			if ( PR_FAIL(error) ) {
				PR_TRACE(( par->m_obj, prtERROR, "ldr\tdskm - %terr, <unk> object", error ));
				DSKM_ParList_Delete( list );
				_release_dskm_main_session( &session );
				return error;
			}
#if defined (_WIN32)
			_wcslwr_s( pname, name_len );
#endif // _WIN32
			PR_TRACE(( par->m_obj, prtIMPORTANT, "ldr\tdskm mask(%S)...", pname ));

			if ( g_app_id ) {
				const tDSKMCompId* comp = 0;
				if ( (!szCompId || !*szCompId) ) {
					comp = _find_comp_id( pname );
					if ( comp )
						szCompId = comp->m_szId;
				}
				if ( (!szObjType || !*szObjType) ) {
					if ( !comp )
						comp = _find_comp_id( pname );
					if ( comp )
						szObjType = comp->m_szType;
				}
			}

			if ( szCompId && *szCompId && !DSKM_ParList_AddStrParam(list,DSKM_KEYFILTER_PARAM_COMPID_ID,szCompId) )
				*sign_lib_error_code = DSKM_ParList_GetLastError( list );
			if ( szObjType && *szObjType && !DSKM_ParList_AddStrParam(list,DSKM_KEYFILTER_PARAM_TYPE_ID,szObjType) )
				*sign_lib_error_code = DSKM_ParList_GetLastError( list );
		}

		if ( DSKM_OK(*sign_lib_error_code) ) {
			HDSKMLISTOBJ pobj = DSKM_ParList_AddBufferedObject( list, 0, buff, sizeof(buff), _dskm_read_buff, par );
			if ( pobj ) {
				tPTR data = pname;
				tUINT size = name_len/sizeof(tWCHAR);
#if defined (__unix__)				
				data = toUCS2LE(data,size);
#endif
				*sign_lib_error_code = DSKM_ParList_SetObjectHashingProp( list, pobj, data, size*sizeof(tSHORT) );
			}
			else
				*sign_lib_error_code = DSKM_ParList_GetLastError( list );
			if ( DSKM_OK(*sign_lib_error_code) )
				*sign_lib_error_code = DSKM_CheckObjectsUsingRegsSet( dskm_lib, list, 0/*DSKM_EXE_OBJ_TYPE*/ );

			if ( DSKM_OK(*sign_lib_error_code) ) {
				tDWORD id = DSKM_ParList_GetFirstObjectError( list, sign_lib_error_code );
				while( id && DSKM_OK(*sign_lib_error_code) )
					id = DSKM_ParList_GetNextObjectError( list, id, sign_lib_error_code );
			}
			else if ( (*sign_lib_error_code == DSKM_ERR_NOT_ALL_OBJECTS_FOUND) && _is_single_obj_in_list(list) )
				DSKM_ParList_GetFirstObjectError( list, sign_lib_error_code );

			DSKM_ParList_Delete( list );
		}
		
		_release_dskm_main_session( &session );

		error = _convertSignErr( *sign_lib_error_code );
		PR_TRACE(( par->m_obj, PR_TRACE_LEVEL(error,prtNOTIFY,prtERROR), "ldr\tdskm - %terr, mask(%S)", error, pname ));

		if ( pname && (pname != fname) )
			CALL_SYS_ObjHeapFree( par->m_obj, pname );

		return error;
	}



	// ---
	tERROR pr_call PrSignCheckV2( const tVOID* dskm_sess, hOBJECT obj, const tCHAR* szCompId, const tCHAR* szObjType, tDWORD* sign_lib_error_code ) {
		HDSKM dskm_lib = (HDSKM)dskm_sess;
		tDWORD l_err;
		tCHAR  buff[0x1000] = {0};
		tERROR error = errOK;

		cDSKMObjParams params = { op_unk, 0, 0, 0, cCP_NULL, cFALSE, {{0,0}} };

		if ( !sign_lib_error_code )
			sign_lib_error_code = &l_err;

		if ( !obj ) {
			*sign_lib_error_code = DSKM_ERR_INVALID_PARAMETER;
			return errPARAMETER_INVALID;
		}

		TRY {
			error = CALL_SYS_ObjectCheck( g_root, obj, IID_SEQIO, PID_ANY, 0, cFALSE );
			
			if ( PR_SUCC(error) ) { // it is a hSeqIO
				params.m_type = op_seqio;
				params.m_seq_io = (hSEQ_IO)obj;
				error = _check_by_params( dskm_lib, &params, szCompId, szObjType, sign_lib_error_code );
				_destruct_param( &params );
				return error;
			}

			if ( error == errHANDLE_INVALID ) { // it is a file handle or string
				tFileType file_type = _is_file_handle( obj );
				switch( file_type ) {
					case ft_disk_file  : 
						params.m_type = op_file; 
						params.m_file = (kl_file_t)obj;
						error = _check_by_params( dskm_lib, &params, szCompId, szObjType, sign_lib_error_code ); 
						break;
					case ft_other_file : 
						error = errPARAMETER_INVALID; 
						break;
					case ft_unk : // try it as string
					default     : 
						error = _check_by_mask_session( dskm_lib, (const tCHAR*)obj, cCP_ANSI, sign_lib_error_code ); 
						break;
				}
				_destruct_param( &params );
				return error;
			}
			
			if ( error != errINTERFACE_INCOMPATIBLE ) {
				*sign_lib_error_code = errUNEXPECTED;
				return error;
			}

			if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_IO,PID_ANY,0,cFALSE)) ) {
				params.m_type = op_io;
				params.m_io.io = (hIO)obj;
				error = _check_by_params( dskm_lib, &params, szCompId, szObjType, sign_lib_error_code );
				_destruct_param( &params );
				return error;
			}

			if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_STRING,PID_ANY,0,cFALSE)) ) {
				tCODEPAGE cp = cCP;
				if ( PR_SUCC(error=CALL_String_ExportToBuff((hSTRING)obj,0,cSTRING_WHOLE,buff,sizeof(buff),cp,cSTRING_Z)) ) {
					error = _check_by_mask_session( dskm_lib, buff, cp, sign_lib_error_code );
					_destruct_param( &params );
				}
				else
					*sign_lib_error_code = DSKM_UNKNOWN_ERROR;
				return error;
			}

			if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_SCHECKLIST,PID_ANY,0,cFALSE)) )
				return _check_list( dskm_lib, (hSCHECKLIST)obj, sign_lib_error_code );

			*sign_lib_error_code = DSKM_UNKNOWN_ERROR;
			return errOBJECT_INCOMPATIBLE;
		}
		EXCEPT
		return error;
	}



	// ---
	tERROR pr_call PrSignReinit( hOBJECT reg_folder ) {
		
		tERROR    err = errOK;
		tDWORD    dskm_err = 0;
		cCODEPAGE cp = cCP_NULL;
		tWCHAR    path[MAX_PATH];
		tVOID*    ppath = path;

		if ( reg_folder && PR_SUCC(CALL_SYS_ObjectCheck(g_root,reg_folder,IID_STRING,PID_ANY,SUBTYPE_ANY,cTRUE)) ) {
			tDWORD size;
			if ( g_bUnderNT )
				cp = cCP_UNICODE;
			else
				cp = cCP_ANSI;
			err = CALL_String_ExportToBuff( (hSTRING)reg_folder, &size, cSTRING_WHOLE, path, sizeof(path), cp, cSTRING_Z );
			if ( err == errBUFFER_TOO_SMALL ) {
				err = CALL_SYS_ObjHeapAlloc( reg_folder, &ppath, size );
				if ( PR_FAIL(err) )
					return err;
				err = CALL_String_ExportToBuff( (hSTRING)reg_folder, 0, cSTRING_WHOLE, ppath, size, cp, cSTRING_Z );
			}
		}
		else
			err = errPARAMETER_INVALID;
		if ( PR_SUCC(err) )
			err = _reinit_dskm_main_session( ppath, cp, &dskm_err );
		if ( ppath != path )
			CALL_SYS_ObjHeapFree( reg_folder, ppath );
		return err;
	}



	// ---
	tERROR IMPEX_FUNC(SignGet)( hOBJECT obj, tVOID* keys[], tDWORD key_count, tDWORD* keys_found, tDWORD* sign_lib_error_code ) {
		return errNOT_IMPLEMENTED;
	}


#elif defined( OLD_SIGN_LIB )

	// ---
	int SIGN_API _old_read_buff_by_io( void* params, void* buffer, tINT buff_len ) {
		tIOParams* p = (tIOParams*)params;
		if ( buff_len == -1 ) {
			p->offset = 0;
			return 0;
		}
		if ( PR_SUCC(CALL_IO_SeekRead(p->io,&buff_len,p->offset,buffer,buff_len)) ) {
			p->offset += buff_len;
			return buff_len;
		}
		return -1;
	}
	
	// ---
	int SIGN_API _old_read_buff_by_seqio( void* params, void* buffer, int buff_len ) {
		hSEQ_IO seqio = (hSEQ_IO)params;
		if ( buff_len == -1 ) {
			if ( PR_SUCC(CALL_SeqIO_SeekSet(seqio,0,0)) )
				return 0;
			return -1;
		}
		if ( PR_SUCC(CALL_SeqIO_Read(seqio,&buff_len,buffer,buff_len)) )
			return buff_len;
		return -1;
	}
	
	
	// ---
	int pr_call _convertFileErrorToSignError( tERROR err ) {
		switch( err ) {
			case errOBJECT_NOT_FOUND :
			case errACCESS_DENIED    : return SIGN_CANT_READ;
			default                  : return SIGN_UNKNOWN_ERROR;
		}
	}

	
	// ---
	tERROR pr_call _convertSignErr( tDWORD sign_lib_error_code ) {
		switch ( sign_lib_error_code ) {
			case SIGN_OK                     : return errOK;                    break;
			case SIGN_UNKNOWN_ERROR          : return errUNEXPECTED;            break;
			case SIGN_NOT_MATCH              : return errSIGN_NOT_MATCH;        break;
			case SIGN_BAD_OPEN_KEY           :
			case SIGN_BAD_KEY_VERSION        : return errSIGN_BAD_KEY;          break;
			case SIGN_BAD_PARAMETERS         : return errPARAMETER_INVALID;     break;
			case SIGN_BUFFER_HAS_NO_NOTARIES :
			case SIGN_NOTARY_NOT_FOUND       : return errSIGN_NOTARY_NOT_FOUND; break;
			case SIGN_CALLBACK_FAILED        : return errUNEXPECTED;            break;
			case SIGN_BUFFER_TOO_SMALL       : return errBUFFER_TOO_SMALL;      break;
			default                          : return errUNEXPECTED;            break;
		}
	}
	

	// ---
	tINT check_cb_not( tVOID* params, tVOID* buffer, tINT buff_len ) {
		const char fake[] = "            \x0d\x0a; 0XLSznpdI71fB300e7Uwj1LzYNRfZTtAVrOQVoai2zm7Jz8ud+YoOlfYtx­­";
		if ( *(int*)params == (int)params ) {
			(*((int*)params))++;
			memcpy( buffer, fake, sizeof(fake) );
			return sizeof(fake)-1;
		}
		return 0;
	}
	

	// ---
	tERROR _check_by_mask( const tVOID* mask, tCODEPAGE cp ) {
		tERROR error = errOK;
		PR_TRACE_VARS;

		if ( g_signCheckFunc )
			error = g_signCheckFunc( (tCHAR*)mask, cp );
		
		else {
			
			int   ch = (int)&ch;
			tCHAR buff[0x1000] = {0};
			
			PR_TRACE(( 0, prtIMPORTANT, "ldr\tStart module injtializing... (\"%s%S\")", PR_TRACE_PREPARE(mask) ));
			
			error = _convertSignErr( sign_check_buffer_callback(check_cb_not,&ch,buff,sizeof(buff),0,1,0,0,0) );
			if ( PR_SUCC(error) ) {
				PR_TRACE(( 0, prtERROR, "ldr\tModule injtializing failed(%terr)! \"%s%S\"", errMODULE_NOT_VERIFIED, PR_TRACE_PREPARE(mask) ));
				error = errMODULE_NOT_VERIFIED;
			}
			else {
				kl_file_t f = INVALID_FILE;
				if ( PR_FAIL(error=_open_file(mask,cp,&f)) ) {
					PR_TRACE(( 0, prtERROR, "ldr\tCannot open plugin (%terr) \"%s%S\"", error, PR_TRACE_PREPARE(mask) ));
				}
				else {
					error = _convertSignErr( sign_check_buffer_callback(check_cb,&f,buff,sizeof(buff),0,1,0,0,0) );
					closeFile( f );
					if ( PR_FAIL(error) ) {
						PR_TRACE(( 0, prtERROR, "ldr\tModule injtializing failed (%terr) \"%s%S\"", error, PR_TRACE_PREPARE(mask) ));
						error = errMODULE_NOT_VERIFIED;
					}
				}
			}
		}
		
		if ( PR_SUCC(error) )
			PR_TRACE(( 0, prtIMPORTANT, "ldr\tFinish module injtializing... (\"%s%S\")", PR_TRACE_PREPARE(mask) ));
		
		if ( (error == errMODULE_NOT_VERIFIED) && g_signCheckNotify )
			error = g_signCheckNotify( (tCHAR*)mask, errMODULE_NOT_VERIFIED );
		
		return error;
	}
	
	
	// ---
	static tERROR _getBuffSign( tVOID* keys[], tDWORD key_count, tBYTE* buff, tUINT len, tUINT offset, tDWORD* keys_found, tDWORD* sign_lib_error_code ) {
		tERROR error;
		
		*sign_lib_error_code = sign_buffer_info( buff, len, &offset, keys_found );
		error = _convertSignErr( *sign_lib_error_code );
		
		if ( PR_FAIL(error) )
			return error;
		
		if ( !*keys_found )
			return errNOT_FOUND;
		
		if ( keys && key_count ) {
			tUINT count = min( *keys_found, key_count );
			memcpy( keys, buff+offset, count*NOTARY_TXT_SIZE );
		}
		return error;
	}




	// ---
	static tERROR _getFileSign( tVOID* keys[], tDWORD key_count, const tCHAR* fname, tBYTE* buff, tUINT len, tDWORD* keys_found, tDWORD* sign_lib_error_code ) {
		tERROR  error = errOK;
		HANDLE  file = 0;
		tUINT   file_length = 0;
		tUINT   offset = 0;
		
		if ( !*fname ) {
			*sign_lib_error_code = SIGN_BAD_PARAMETERS;
			return errOBJECT_BAD_PTR;
		}
		
		if ( PR_FAIL(error=_open_file((const tSTRING)fname,cCP_ANSI,&file)) ) {
			*sign_lib_error_code = _convertFileErrorToSignError( error );
			return error;
		}
		
		file_length = getFileSize( file );
		if ( file_length > len )
			offset = file_length - len;
		else {
			len = file_length;
			offset = 0;
		}
		
		if ( offset && (-1==setFilePointer(file,offset)) )
			return errOBJECT_SEEK;
		
		len = _old_read_buff_by_file( file, buff, len );
		if ( !len || (len == -1) )
			return errOBJECT_READ;
		
		closeFile( file );
		
		return _getBuffSign( keys, key_count, buff, len, offset, keys_found, sign_lib_error_code );
	}
	

	
	// ---
	tERROR IMPEX_FUNC(PrSignGetContentLen)( const tVOID* buff, tUINT buff_size, tUINT* content_len ) {
		int    notaries_found = 0;
		tERROR error;
		
		if ( !buff || !buff_size || !content_len )
			return errPARAMETER_INVALID;
		
		error = _convertSignErr( sign_buffer_info((tBYTE*)buff,buff_size,content_len,&notaries_found) );
		if ( (error == errSIGN_NOTARY_NOT_FOUND) || !notaries_found ) {
			*content_len = buff_size;
			return errOK;
		}
		return error;
	}
	
	
	
	// ---
	tERROR IMPEX_FUNC(SignCheck)( hOBJECT obj, tBOOL use_default_key, tVOID* keys[], tDWORD key_count, tBOOL all_of, tDWORD* sign_lib_error_code ) {
		int       l_err;
		tCHAR     buff[0x1000] = {0};
		tERROR    error = errOK;
		kl_file_t file = INVALID_FILE;

		#ifdef _PRAGUE_TRACE_
			tCHAR name[0x100] = "<unknown>";
		#endif

		if ( !sign_lib_error_code )
			sign_lib_error_code = &l_err;

		if ( !obj ) {
			*sign_lib_error_code = SIGN_BAD_PARAMETERS;
			return errPARAMETER_INVALID;
		}

		TRY {
			tERROR error = CALL_SYS_ObjectCheck( g_root, obj, IID_SEQIO, PID_ANY, 0, cFALSE );
			
			if ( PR_SUCC(error) ) {
				#ifdef _PRAGUE_TRACE_
					CALL_SYS_PropertyGetStr(obj, NULL, pgOBJECT_FULL_NAME, name, sizeof(name), cCP_ANSI);
					PR_TRACE((g_root, prtIMPORTANT, "ldr\tSignCheck(%s)...", name));
				#endif
				*sign_lib_error_code = sign_check_buffer_callback( _old_read_buff_by_seqio, obj, buff, sizeof(buff), 0, use_default_key, keys, key_count, all_of );
			}
			else if ( error == errHANDLE_INVALID ) {
				
				if ( INVALID_FILE == (kl_file_t)obj ) {
					*sign_lib_error_code = SIGN_BAD_PARAMETERS;
					return errPARAMETER_INVALID;
				}

				if ( ft_disk_file == _is_file_handle(obj) ) {
					error = errOK;
					file = (kl_file_t)obj;
				}
				else if ( !*((tSTRING)obj) ) {
					*sign_lib_error_code = SIGN_BAD_PARAMETERS;
					return errPARAMETER_INVALID;
				}
				else {
					PR_TRACE(( g_root, prtIMPORTANT, "ldr\tSignCheck(%s)...", (tSTRING)obj ));
					error = _open_file( (tSTRING)obj, cCP_ANSI, &file );
					if ( PR_FAIL(error) ) {
						*sign_lib_error_code = _convertFileErrorToSignError( error );
						return error;
					}
				}

				*sign_lib_error_code = sign_check_buffer_callback( _old_read_buff_by_file, (tPTR)file, buff, sizeof(buff), 0, use_default_key, keys, key_count, all_of );
				if ( file != (kl_file_t)obj )
					closeFile(file);
			}
			
			else if ( error != errINTERFACE_INCOMPATIBLE ) {
				*sign_lib_error_code = errUNEXPECTED;
				return error;
			}
  
			else if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_IO,PID_ANY,0,cFALSE)) ) {
				tIOParams params = { (hIO)obj, 0 };
				#ifdef _PRAGUE_TRACE_
					CALL_SYS_PropertyGetStr(obj, NULL, pgOBJECT_FULL_NAME, name, sizeof(name), cCP_ANSI);
					PR_TRACE((g_root, prtIMPORTANT, "ldr\tSignCheck(%s)...", name));
				#endif
				*sign_lib_error_code = sign_check_buffer_callback( _old_read_buff_by_io, &params, buff, sizeof(buff), 0, use_default_key, keys, key_count, all_of );
			}

			else if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_STRING,PID_ANY,0,cFALSE)) ) {
				if ( PR_FAIL(error=CALL_String_ExportToBuff((hSTRING)obj,0,cSTRING_WHOLE,buff,sizeof(buff),cCP,cSTRING_Z)) ) {
					*sign_lib_error_code = SIGN_UNKNOWN_ERROR;
					return error;
				}
				PR_TRACE(( g_root, prtIMPORTANT, "ldr\tSignCheck(%S)...", buff ));
				if ( PR_FAIL(error=_open_file(buff,cCP,&file)) )
					*sign_lib_error_code = _convertFileErrorToSignError( error );
				else {
					*sign_lib_error_code = sign_check_buffer_callback( _old_read_buff_by_file, (tPTR)file, buff, sizeof(buff), 0, use_default_key, keys, key_count, all_of );
					closeFile( file );
				}
			}
			else {
				*sign_lib_error_code = SIGN_UNKNOWN_ERROR;
				return errOBJECT_INCOMPATIBLE;
			}
			error = _convertSignErr( *sign_lib_error_code );
			if ( PR_FAIL(error) )
				PR_TRACE(( g_root, prtIMPORTANT, "ldr\tSignCheck (%s) completed with %terr", name, error ));
			return error;
		}
		EXCEPT
  
		return error;
	}



	// ---
	tERROR IMPEX_FUNC(PrSignCheckV2)(  tVOID* dskm_lib, hOBJECT obj, const tCHAR* szCompId, const tCHAR* szObjType, tDWORD* sign_lib_error_code ) {
		return errNOT_SUPPORTED;
	}


	// ---
	tERROR pr_call PrSignReinit( hOBJECT reg_folder ) {
		return errNOT_SUPPORTED;
	}

	// ---
	tERROR IMPEX_FUNC(SignGet)( hOBJECT obj, tVOID* keys[], tDWORD key_count, tDWORD* keys_found, tDWORD* sign_lib_error_code ) {
		int     l_err;
		int     not_found = 0;
		
		if ( !sign_lib_error_code )
			sign_lib_error_code = &l_err;
		
		if ( !obj ) {
			*sign_lib_error_code = SIGN_BAD_PARAMETERS;
			return errPARAMETER_INVALID;
		}
		
		if ( !keys_found )
			keys_found = &not_found;
		
		*keys_found = 0;
		
		if ( keys && key_count )
			memset( keys, 0, key_count*NOTARY_TXT_SIZE );
		
		TRY {
			tBYTE buff[1024];
			tQWORD file_length = 0;
			tQWORD offset = 0;
			tDWORD len;
			tERROR error = CALL_SYS_ObjectCheck( g_root, obj, IID_SEQIO, PID_ANY, 0, cFALSE );
			
			if ( error == errHANDLE_INVALID ) // it is not a prague object
				return _getFileSign( keys, key_count, (const tCHAR*)obj, buff, sizeof(buff), keys_found, sign_lib_error_code );
			
			if ( error != errINTERFACE_INCOMPATIBLE ) {
				*sign_lib_error_code = errUNEXPECTED;
				return error;
			}
			
			if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_IO,PID_ANY,0,cFALSE)) ) {
				error = CALL_IO_GetSize( (hIO)obj, &file_length, IO_SIZE_TYPE_EXPLICIT );
				if ( PR_FAIL(error) )
					return error;
				
				if ( file_length > sizeof(buff) ) {
					len = sizeof(buff);
					offset = file_length - sizeof(buff);
				}
				else {
					len = (tDWORD)file_length;
					offset = 0;
				}
				
				error = CALL_IO_SeekRead( (hIO)obj, 0, offset, buff, len );
				if ( PR_FAIL(error) )
					return error;
				return _getBuffSign( keys, key_count, buff, len, (tDWORD)offset, keys_found, sign_lib_error_code );
			}
			
			if ( PR_SUCC(CALL_SYS_ObjectCheck(g_root,obj,IID_STRING,PID_ANY,0,cFALSE)) ) {
				if ( PR_FAIL(error=CALL_String_ExportToBuff((hSTRING)obj,0,cSTRING_WHOLE,buff,sizeof(buff),cCP_ANSI,cSTRING_Z)) ) {
					*sign_lib_error_code = SIGN_UNKNOWN_ERROR;
					return error;
				}
				return _getFileSign( keys, key_count, buff, buff, sizeof(buff), keys_found, sign_lib_error_code );
			}
			
			*sign_lib_error_code = SIGN_UNKNOWN_ERROR;
			return errOBJECT_INCOMPATIBLE;
		}
		EXCEPT

		return errUNEXPECTED;
	}

#else
  // ---
	tERROR IMPEX_FUNC(PrSignGetContentLen)( const tVOID* buff, tUINT buff_size, tUINT* content_len ) {
		return errNOT_SUPPORTED;
	}
	// ---
	tERROR IMPEX_FUNC(SignCheck)( hOBJECT obj, tBOOL use_default_key, tVOID* keys[], tDWORD key_count, tBOOL all_of, tDWORD* sign_lib_error_code ) {
		return errNOT_SUPPORTED;
	}
	// ---
	tERROR IMPEX_FUNC(SignGet)( hOBJECT obj, tVOID* keys[], tDWORD key_count, tDWORD* keys_found, tDWORD* sign_lib_error_code ) {
		return errNOT_SUPPORTED;
	}
	// ---
	tERROR pr_call PrSignReinit( hOBJECT reg_folder ) {
		return errNOT_SUPPORTED;
	}
	// ---
	tERROR IMPEX_FUNC(PrSignCheckV2)( const tVOID* dskm_lib, hOBJECT obj, const tCHAR* szCompId, const tCHAR* szObjType, tDWORD* sign_lib_error_code ) {
		return errNOT_SUPPORTED;
	}


#endif




