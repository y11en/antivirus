#if !defined (_SIGNCHK_UNIX_INC_)
#define _SIGNCHK_UNIX_INC_

#include <glob.h>
#include <wchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "loader.h"

int getFileSize( kl_file_t handle ) {
  struct stat l_stat;
  if ( fstat( handle, &l_stat ) ) 
    return INVALID_FILE_SIZE;
  return l_stat.st_size;
}


#define TRY
#define EXCEPT
 
// ---
int check_cb( void* params, void* buffer, int buff_len ) {
  if ( ( buff_len = read (*(kl_file_t*)params,buffer,buff_len ) ) > 0 )
    return buff_len;
  else
    return -1;
}

// ---
int SIGN_API _old_read_buff_by_file( kl_file_t file, void* buffer, int buff_len ) {
	if ( ( buff_len = read ( file, buffer, buff_len ) ) >= 0 )
		return buff_len;
	return -1;
}

// ---
#if defined( __cplusplus )
extern "C" {
#endif
	tFileType pr_call _is_file_handle ( void* file ) {
		struct stat l_stat;
		if ( fstat ( (kl_file_t)file, &l_stat ) ) 
			return ft_unk;
		if ( S_ISREG ( l_stat.st_mode) )
			return ft_disk_file;
		return ft_unk;
	}
#if defined( __cplusplus )
}
#endif

tERROR pr_call _open_file(  const tVOID* name, tCODEPAGE cp, kl_file_t* file ) {
  if ( cp != cCP_UNICODE)	
    *file = open ( MB(name), O_RDONLY );
  else {
    tCHAR l_name [MAX_PATH] = {0};
    tERROR error = CopyTo (l_name, sizeof(l_name), cCP_ANSI, name, 0, cp, cSTRING_CONTENT_ONLY, 0 );
    if (PR_FAIL(error))
      return error;
    *file = open ( l_name, O_RDONLY );
  }
    
  if ( *file == -1 ) {
    switch( errno ) {
			case ENOENT: return errOBJECT_NOT_FOUND;
			case EACCES: return errACCESS_DENIED;
			default    : return errUNEXPECTED;
    }
  }
  else
    return errOK;
}

// ---
tERROR pr_call _read_file( kl_file_t file, void* buffer, tULONG size, tULONG* read_size ) {
  if ( ( *read_size = read ( file, buffer, size ) ) >= 0 )
    return errOK;
  *read_size = 0;
  return errOBJECT_READ;
}

// ---
tERROR pr_call _set_file_cp( kl_file_t file, tULONG offset ) {
  if ( offset == lseek( file, offset, SEEK_SET ) )
    return errOK;
  return errOBJECT_SEEK;
}

tVOID _init_dskm_main_folder() {
  tUINT len = 0;
  PR_TRACE_VARS;

  if ( g_app_id && g_app_id->m_data_folder && MB(g_app_id->m_data_folder)[0] ) {
    if ( g_app_id->m_data_folder_cp == cCP_UNICODE )
      wcstombs( MB(g_dskm_main_folder), UNI(g_app_id->m_data_folder), sizeof(g_dskm_main_folder) );
    else
      lstrcpy( MB(g_dskm_main_folder), MB(g_app_id->m_data_folder) );
    PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM preloaded path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
  }
  else {
    lstrcpy( MB(g_dskm_main_folder), MB(g_plugins_path) ); 
    PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM shell path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
  }
  
  len = strlen( MB(g_dskm_main_folder) );
  if ( len && (MB(g_dskm_main_folder)[len-1] != '/') )
    MB(g_dskm_main_folder)[len++] = '/';
  
  lstrcpy( MB(g_dskm_main_folder)+len, PR_LOADER_DSKM_DIR );
  //SHCreateDirectoryExA( 0, MB(g_dskm_main_folder), 0 );
  PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
}


// ---
tERROR pr_call _find_files_by_mask( const tVOID* mask, tCODEPAGE cp, tDWORD* files_found, pfnAddObjectCallback obj_callback, tVOID* obj_param ) {

  glob_t           find;
  tUINT            dir_len;
  tCHAR           file_mask[MAX_PATH] = {0};
  const tCHAR*     dslash;
  tDWORD           file_count = 0;
  PR_TRACE ((g_root, prtNOTIFY, "ldr\t%s ", __FUNCTION__));
  if ( files_found )
    *files_found = 0;

  if ( cp == cCP_UNICODE ) {
    mbstate_t state;
    memset ( &state, 0, sizeof ( state ) );
    const wchar_t* tmp = (const wchar_t *) ( mask );

    size_t ret_val = wcsrtombs ( file_mask, 
                                 &tmp,
                                 wcslen ( tmp ),
                                 &state );    

    if ( ret_val  == (size_t) ( -1 ) )
      return errUNEXPECTED;
  }
  else
    strncpy( file_mask, MB(mask), sizeof ( file_mask ) );

  dslash = strrchr( file_mask, '/' );
  if ( dslash )
    *((tBYTE**)&dslash) += 1;
  else
    dslash = file_mask;

  dir_len = dslash - file_mask;

  if ( !glob ( file_mask, 0, 0, &find  ) ) {
    size_t i = 0;
    for (; i < find.gl_pathc; ++i ) {
      tUINT len;
      struct stat l_stat;
      if ( stat ( find.gl_pathv [i], &l_stat ) )
        continue;
      if ( S_ISDIR(l_stat.st_mode ) )
        continue;
      PR_TRACE ((g_root, prtNOTIFY, "ldr\t%s found file %s", __FUNCTION__, find.gl_pathv [i]));
      len = 1 + strlen(find.gl_pathv [i]);
      if ( PR_SUCC(obj_callback(find.gl_pathv [i],cp,len,obj_param)) )
        ++file_count;
    }
    globfree( &find );
  }
  if ( files_found )
    *files_found = file_count;
  return errOK;
}

// ---
tPTR  pr_call _os_alloc( tUINT size ) {
  return calloc( sizeof ( char ), size );
}


// ---
tVOID pr_call _os_free( tPTR ptr ) {
  free ( ptr );
}

// ---
tUINT pr_call _receive_unicode_file_name( const tVOID* path, tCODEPAGE cp, tWCHAR* fname, tUINT fname_size ) {
	tUINT len;
	if ( cp == cCP_UNICODE ) {
		tWCHAR* pslash = wcsrchr( UNI(path), L'/' );
		if ( pslash )
			++pslash;
		else
			pslash = UNI(path);
		len = sizeof(tWCHAR) * (1 + wcslen(pslash));
		if ( len > fname_size )
			return 0;
		wcsncpy( fname, pslash, len );
	}
	else {
		tCHAR* pslash = strrchr( MB(path), '/' );
		if ( pslash )
			++pslash;
		else
			pslash = MB(path);
		len = _toui32(1 + strlen(pslash));
		if ( (sizeof(tWCHAR)*len) > fname_size )
			return 0;
		len = mbstowcs( fname, pslash, len );
		if ( len == -1 )
			return 0;
		fname [ len ] = 0;
		len += 1;
		len *= sizeof(tWCHAR);
	}
	return len;
}

#endif //_SIGNCHK_UNIX_INC_
