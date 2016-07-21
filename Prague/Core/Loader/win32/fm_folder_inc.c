#if !defined (_FM_FOLDER_WIN32_INC_)
#define _FM_FOLDER_WIN32_INC_

#include <windows.h>

// ---
static tBOOL pr_call check_name( cPlugin* plg, const tVOID* module_name, tCODEPAGE cp ) {
  cStrObj name;
  if ( PR_FAIL(name.assign(plg,pgMODULE_NAME)) )
    return cFALSE;
  tDWORD pos = name.find( module_name, cp, 0, cStrObj::whole, fSTRING_FIND_FORWARD|fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE );
  if ( pos != cStrObj::npos )
    return cTRUE;
  return cFALSE;
}


// ---
tERROR pr_call LoaderData::find_modules_in_folder( const tVOID* param_pool, tDWORD param_pool_size, tCODEPAGE cp ) {

  tERROR  error = errOK;
  cStrObj path;

  if ( param_pool && param_pool_size )
    path.assign( param_pool, cp );
  else
    path.assign( ::g_plugins_path, cCP );

  PR_TRACE(( this, prtNOTIFY, "ldr\tSearch folder is \"%tS\"", path.data() ));

	#if !defined(NOT_VERIFY) && !defined(LATE_CHECK) && defined(NEW_SIGN_LIB)
		
		cSCheckList* list;
		error = sysCreateObjectQuick( (hOBJECT*)&list, IID_SCHECKLIST );
		if ( PR_SUCC(error) ) {
			tUINT  err_count   = 0;
			tUINT  loads_count = 0;
			tUINT  files_count = 0;
			cDSKMObjParams* par;
			path.add_path_sect( L"*.ppl", cCP_UNICODE );
				
			cStrBuff mask( path, cp );
			error = list->AddObjByMask( 0, &files_count, mask, cp );

			if ( PR_SUCC(error) ) {
				tDWORD id = 0;

				PR_TRACE(( this, prtIMPORTANT, "ldr\tmodules loading" ));
				_check_list( 0, list, 0 );
				PR_TRACE(( this, prtIMPORTANT, "ldr\tmodules loading, phase 2" ));

				error = list->GetFirstObjId( &id );
				while( PR_SUCC(error) && files_count ) {
					tDWORD dskm_err = DSKM_ERR_OK;
					tDWORD dskm_err_size( sizeof(dskm_err) );
					tDWORD par_size( sizeof(par) );

					--files_count;

					list->GetObjProp( id, DSKM_OBJ_PROCESSING_ERROR, &dskm_err, &dskm_err_size );

					if ( DSKM_NOT_OK(dskm_err) ) {
						dskm_err = DSKM_ERR_OK;
						list->GetObjProp( id, DSKM_OBJ_EXTERNAL_PARAM_PTR, &par, &par_size );
						++err_count;
					}
					else if ( PR_FAIL(list->GetObjProp(id,DSKM_OBJ_EXTERNAL_PARAM_PTR,&par,&par_size)) || !par->m_name_size )
						++err_count;
					else if ( PR_FAIL(create_module(par->m_name,par->m_name_size,par->m_name_cp)) )
						++err_count;
					else
						++loads_count;
					error = list->GetNextObjId( &id, id );
				}
				if ( error == errEND_OF_THE_LIST )
					error = errOK;
				PR_TRACE(( this, prtIMPORTANT, "ldr\tmodules loaded, files=%d, loads=%d, err=%d", err_count+loads_count, loads_count, err_count ));
			}

			list->sysCloseObject();
		}

	#else

		HANDLE           fh;
		cStrObj          mask( path );
		WIN32_FIND_DATAW fd;
		
		mask.add_path_sect( L"*.ppl", cCP_UNICODE );
		memset( &fd, 0, sizeof(fd) );
		
		cStrBuff name( mask, cp );

		PR_TRACE(( this, prtIMPORTANT, "ldr\tmodules loading." ));
		if ( cp == cCP_UNICODE )
			fh = FindFirstFileW( (LPCWSTR)(tWCHAR*)name, &fd );
		else
			fh = FindFirstFileA( (tCHAR*)name, (WIN32_FIND_DATA*)&fd );
		
		if ( fh && (fh != INVALID_HANDLE_VALUE) ) {
			
			BOOL res;
			
			do {
				tERROR err;
				cStrObj curr = path;
				curr.add_path_sect( fd.cFileName, cp );
				
				cStrBuff name( curr, cp );
				err = create_module( name, name.used(), cp );
				
				if ( PR_SUCC(error) && PR_FAIL(err) )
					error = err;
				
				if ( cp == cCP_UNICODE )
					res = FindNextFileW( fh, &fd );
				else
					res = FindNextFileA( fh, (WIN32_FIND_DATA*)&fd );
			} while( res );
			
			if ( PR_FAIL(error) ) {
				PR_TRACE(( this, prtERROR, "ldr\tError finding modules - %terr", error ));
			}
			
			FindClose( fh );
		}
		else {
			error = errMODULE_NOT_FOUND;
			PR_TRACE(( this, prtERROR, "ldr\tFolder has no prague modules" ));
		}
		PR_TRACE(( this, prtIMPORTANT, "ldr\tmodules loaded." ));
	#endif

	return error;
}


#endif //_FM_FOLDER_WIN32_INC_
