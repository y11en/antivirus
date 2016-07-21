#if !defined (_SIGNCHK_WIN32_INC_)
#define _SIGNCHK_WIN32_INC_


#include <windows.h>
#include <shlobj.h>
#include <shfolder.h>

#define TRY     __try

#if defined( NEW_SIGN_LIB )

	#define EXCEPT  __except( EXCEPTION_EXECUTE_HANDLER ) \
		{ *sign_lib_error_code = DSKM_UNKNOWN_ERROR; \
			return errUNEXPECTED; \
		}

#else

	#define EXCEPT  __except( EXCEPTION_EXECUTE_HANDLER ) \
		{ *sign_lib_error_code = SIGN_UNKNOWN_ERROR; \
			return errUNEXPECTED; \
		}

#endif


// ---
int check_cb( void* params, void* buffer, int buff_len ) {
  if ( ReadFile(*(HANDLE*)params,buffer,buff_len,&buff_len,0) )
    return buff_len;
  return -1;
}



// ---
#if defined( __cplusplus )
extern "C" {
#endif
	tFileType pr_call _is_file_handle( void* obj ) {
		tDWORD type = GetFileType( (HANDLE)obj );
		switch( type ) {
			case FILE_TYPE_DISK    : return ft_disk_file;
			case FILE_TYPE_CHAR    :
			case FILE_TYPE_PIPE    : return ft_other_file;
			case FILE_TYPE_UNKNOWN :
			default                : return ft_unk;
		}
	}
#if defined( __cplusplus )
}
#endif


// ---
tERROR pr_call _open_file( const tVOID* name, tCODEPAGE cp, HANDLE* file ) {
    if ( cp == cCP_UNICODE ) {
		*file = CreateFileW( UNI(name), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
        PR_TRACE(( g_root, prtNOTIFY, "ldr\t_open_file file unicode - %S  file - 0x%08x", name, *file));
    }

    if (cp != cCP_UNICODE || !(*file) || *file == INVALID_HANDLE_VALUE) {
        tVOID* buff = NULL ;
        if ( cp == cCP_UNICODE ) {
            int nMBMaxLength =  MB_CUR_MAX * (wcslen(name) + 1) ;
            buff = malloc(nMBMaxLength) ;
            memset(buff, 0, nMBMaxLength) ;
            wcstombs_s(NULL, (char*)buff, nMBMaxLength, name, wcslen(name)) ;
            name = buff ;
        }

		*file = CreateFileA( MB(name), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

        PR_TRACE(( g_root, prtNOTIFY, "ldr\t_open_file file ansii - %s  file - 0x%08x", name, *file));

        if (buff) {
            free(buff) ;
        }
    }
	
	if ( *file == INVALID_HANDLE_VALUE ) {
		DWORD win32err = GetLastError();
		switch( win32err ) {
			case ERROR_FILE_NOT_FOUND :
			case ERROR_PATH_NOT_FOUND : return errOBJECT_NOT_FOUND;
			case ERROR_ACCESS_DENIED  : return errACCESS_DENIED;
			default                   : return errUNEXPECTED;
		}
	}
	return errOK;
}


// ---
int SIGN_API _old_read_buff_by_file( kl_file_t file, void* buffer, int size ) {
	tDWORD read_size;
	if ( ReadFile(file,buffer,size,&read_size,0) )
		return read_size;
	return -1;
}


// ---
tERROR pr_call _set_file_cp( kl_file_t file, tULONG offset ) {
	if ( offset == SetFilePointer(file,offset,0,SEEK_SET) )
		return errOK;
	return errOBJECT_SEEK;
}


// ---
tPTR  pr_call _os_alloc( tUINT size ) {
	return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, size );
}


// ---
tVOID pr_call _os_free( tPTR ptr ) {
	HeapFree( GetProcessHeap(), 0, ptr );
}


// ---
tUINT pr_call _receive_unicode_file_name( const tVOID* path, tCODEPAGE cp, tWCHAR* fname, tUINT fname_size ) {
	tUINT len;
	if ( cp == cCP_UNICODE ) {
		tWCHAR* pbackslash = wcsrchr( UNI(path), PATH_DELIMETER_WIDECHAR );
		tWCHAR* pslash = wcsrchr( UNI(path), L'/' );
		if ( pbackslash > pslash )
			pslash = pbackslash;
		if ( pslash )
			++pslash;
		else
			pslash = UNI(path);
		len = _toui32(sizeof(tWCHAR) * (1 + wcslen(pslash)));
		if ( len > fname_size )
			return 0;
		memcpy( fname, pslash, len );
	}
	else {
		tCHAR* pbackslash = strrchr( MB(path), PATH_DELIMETER_CHAR );
		tCHAR* pslash = strrchr( MB(path), '/' );
		if ( pbackslash > pslash )
			pslash = pbackslash;
		if ( pslash )
			++pslash;
		else
			pslash = MB(path);
		len = _toui32(1 + strlen(pslash));
		if ( (sizeof(tWCHAR)*len) > fname_size )
			return 0;
		mbstowcs_s(&len, fname, fname_size, pslash, len);
		if ( len == -1 )
			return 0;
		len += 1;
		len *= sizeof(tWCHAR);
	}
	_wcslwr_s( fname, fname_size );
	return len;
}


// ---
#if defined( NEW_SIGN_LIB )

	//#define DSKM_FILE_API
	#if defined(DSKM_FILE_API)
		#include <_avpio.h>

		// ---
		static unsigned int my_AddRef( DSKM_Io *This ) {
			return ++This->m_cRef;
		}

		// ---
		static unsigned int my_Release( DSKM_Io *This ) {
			--This->m_cRef;
			return This->m_cRef;
		}

		// ---
		static HANDLE my_w_CreateFile( DSKM_Io *This, LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, SECURITY_ATTRIBUTES FAR* lpSecurityAttributes, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) {
			return CreateFileW( (wchar_t *)lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
		}

		// ---
		static HANDLE my_a_CreateFile( DSKM_Io *This, LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, SECURITY_ATTRIBUTES FAR* lpSecurityAttributes, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) {
			return CreateFileA( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
		}
		
		// ---
		static BOOL my_CloseHandle( DSKM_Io *This, HANDLE hObject ) {
			return CloseHandle( hObject );
		}

		// ---
		static BOOL my_ReadFile( DSKM_Io *This, HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, OVERLAPPED FAR* lpOverlapped ) {
			return ReadFile( hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped );
		}

		// ---
		static BOOL my_WriteFile( DSKM_Io *This, HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, OVERLAPPED FAR* lpOverlapped ) {
			return WriteFile( hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped );
		}

		// ---
		static DWORD my_SetFilePointer( DSKM_Io *This, HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod ) {
			return SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
		}

		// ---
		static DWORD my_GetFileSize( DSKM_Io *This, HANDLE hFile, LPDWORD lpHigh ) {
			return GetFileSize( hFile, lpHigh );
		}

		// ---
		typedef struct tag_FFContent {
			tBOOL             m_nt;
			HANDLE					  m_hFFHandle;
			tWCHAR*           m_dir;
			tUINT             m_len;
			WIN32_FIND_DATAW  m_FFData;
		} FFContent;


		// ---
		static HANDLE my_w_FindFirstFile( DSKM_Io *This, LPCTSTR lpFileFolder, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {

			FFContent* pContent = malloc( sizeof(FFContent) );
			
			if ( pContent ) {
				tWCHAR* dir;
				tINT    dir_len = 0;
				memset( pContent, 0, sizeof(FFContent) );
				
				pContent->m_nt = g_bUnderNT;

				if ( !lpFileFolder || !*lpFileFolder ) {
					dir_len = 0;
					dir = L"*";
				}
				else {
					tBOOL add_slash;
					tWCHAR* folder = (tWCHAR*)lpFileFolder;
					dir_len = wcslen( folder );
					if ( (folder[dir_len-1] == PATH_DELIMETER_WIDECHAR) || (folder[dir_len-1] == L'/') ) {
						add_slash = cFALSE;
						dir = malloc( (dir_len+2) * sizeof(tWCHAR) );
					}
					else {
						add_slash = cTRUE;
						dir = malloc( (dir_len+3) * sizeof(tWCHAR) );
					}
					if ( !dir ) {
						free( pContent );
						return 0;
					}

					memcpy( dir, folder, dir_len * sizeof(tWCHAR) );
					if ( add_slash )
						dir[dir_len++] = PATH_DELIMETER_WIDECHAR;
					dir[dir_len+0] = L'*';
					dir[dir_len+1] = 0;
				}
				
				pContent->m_hFFHandle = FindFirstFileW( dir, &pContent->m_FFData );

				if ( pContent->m_hFFHandle != INVALID_HANDLE_VALUE ) {
					tUINT name_len = sizeof(tWCHAR) * wcslen(pContent->m_FFData.cFileName);

					dir_len *= sizeof(tWCHAR);

					if ( dwFileNameSize >= (sizeof(tWCHAR) + dir_len + name_len) ) {
						dir[dir_len-1] = 0;
						pContent->m_dir = dir;
						pContent->m_len = dir_len;
						memcpy( lpFoundFileName,     dir, dir_len );
						memcpy( lpFoundFileName+dir_len, pContent->m_FFData.cFileName, name_len+sizeof(tWCHAR) );
						dir = 0;
					}
					else {
						free( pContent );
						pContent = 0;
					}
				}
				if ( dir_len && dir )
					free( dir );
			}
			return pContent;
		}



		// ---
		static HANDLE my_a_FindFirstFile( DSKM_Io *This, LPCTSTR lpFileFolder, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
			
			FFContent* pContent = malloc( sizeof(FFContent) );
			
			if ( pContent ) {
				tCHAR*  dir;
				tINT    dir_len = 0;
				memset( pContent, 0, sizeof(FFContent) );
				
				pContent->m_nt = g_bUnderNT;
				if ( !lpFileFolder || !*lpFileFolder ) {
					dir_len = 0;
					dir = "*";
				}
				else {
					tBOOL add_slash;
					tCHAR* folder = (tCHAR*)lpFileFolder;
					dir_len = lstrlen( folder );
					if ( (folder[dir_len-1] == PATH_DELIMETER_CHAR) || (folder[dir_len-1] == '/') ) {
						add_slash = cFALSE;
						dir = malloc( (dir_len+2) * sizeof(tCHAR) );
					}
					else {
						add_slash = cTRUE;
						dir = malloc( (dir_len+3) * sizeof(tCHAR) );
					}
					if ( !dir ) {
						free( pContent );
						return 0;
					}
					
					memcpy( dir, folder, dir_len * sizeof(tCHAR) );
					if ( add_slash )
						dir[dir_len++] = PATH_DELIMETER_CHAR;
					dir[dir_len+0] = '*';
					dir[dir_len+1] = 0;
				}
				
				pContent->m_hFFHandle = FindFirstFile( dir, (WIN32_FIND_DATA*)&pContent->m_FFData );
				
				if ( pContent->m_hFFHandle != INVALID_HANDLE_VALUE ) {
					tUINT name_len = sizeof(tCHAR) * lstrlen( MB(pContent->m_FFData.cFileName) );
					
					dir_len *= sizeof(tCHAR);
					
					if ( dwFileNameSize >= (sizeof(tCHAR) + dir_len + name_len) ) {
						dir[dir_len-1] = 0;
						pContent->m_dir = (tWCHAR*)dir;
						pContent->m_len = dir_len;
						memcpy( lpFoundFileName,     dir, dir_len );
						memcpy( lpFoundFileName+dir_len, pContent->m_FFData.cFileName, name_len+sizeof(tCHAR) );
						dir = 0;
					}
					else {
						free( pContent );
						pContent = 0;
					}
				}
				if ( dir_len && dir )
					free( dir );
			}
			return pContent;
		}
		
		
		
		// ---
		static BOOL my_w_FindNextFile( DSKM_Io *This, HANDLE hFindFile, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
			FFContent* ff = (FFContent *)hFindFile;
			if ( ff && ff->m_hFFHandle != INVALID_HANDLE_VALUE ) {
				if ( FindNextFileW(ff->m_hFFHandle,&ff->m_FFData) ) {
					tUINT name_len = sizeof(tWCHAR) * wcslen(ff->m_FFData.cFileName);

					if ( dwFileNameSize >= (sizeof(tWCHAR) + ff->m_len + name_len) ) {
						memcpy( lpFoundFileName,           ff->m_dir, ff->m_len );
						memcpy( lpFoundFileName+ff->m_len, ff->m_FFData.cFileName, name_len+sizeof(tWCHAR) );
						return TRUE;
					}
				}
			}
			return FALSE;
		}


		// ---
		static BOOL my_a_FindNextFile( DSKM_Io *This, HANDLE hFindFile, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
			FFContent* ff = (FFContent *)hFindFile;
			if ( ff && ff->m_hFFHandle != INVALID_HANDLE_VALUE ) {
				if ( FindNextFile(ff->m_hFFHandle,(WIN32_FIND_DATA*)&ff->m_FFData) ) {
					tUINT name_len = sizeof(tCHAR) * wcslen(ff->m_FFData.cFileName);
					
					if ( dwFileNameSize >= (sizeof(tCHAR) + ff->m_len + name_len) ) {
						memcpy( lpFoundFileName,           ff->m_dir, ff->m_len );
						memcpy( lpFoundFileName+ff->m_len, ff->m_FFData.cFileName, name_len+sizeof(tCHAR) );
						return TRUE;
					}
				}
			}
			return FALSE;
		}
		
		
		// ---
		static BOOL my_FindClose( DSKM_Io *This, HANDLE hFindFile ) {
			FFContent* ff = (FFContent *)hFindFile;
			if ( ff && ff->m_hFFHandle != INVALID_HANDLE_VALUE ) {
				BOOL bResult = FindClose( ff->m_hFFHandle );
				free( ff->m_dir );
				free( ff );
				return bResult;
			}
			return FALSE;
		}



		DSKM_Io_Vtbl g_w_IoVtbl;
		DSKM_Io      g_w_IO = { &g_w_IoVtbl, 0 };

		// ---
		DSKM_Io* InitIoW() {
			LoadIO();
			
			memset( &g_w_IoVtbl, 0, sizeof(g_w_IoVtbl) );
			
			g_w_IoVtbl.AddRef = my_AddRef;
			g_w_IoVtbl.Release = my_Release;
			g_w_IoVtbl.CreateFile = my_w_CreateFile;
			g_w_IoVtbl.CloseHandle = my_CloseHandle;
			g_w_IoVtbl.ReadFile = my_ReadFile;
			g_w_IoVtbl.WriteFile = my_WriteFile; 
			g_w_IoVtbl.SetFilePointer = my_SetFilePointer;
			g_w_IoVtbl.GetFileSize = my_GetFileSize;
			g_w_IoVtbl.FindFirstFile = my_w_FindFirstFile;
			g_w_IoVtbl.FindNextFile = my_w_FindNextFile;
			g_w_IoVtbl.FindClose = my_FindClose;
			
			my_AddRef( &g_w_IO );
			return &g_w_IO;
		}


		DSKM_Io_Vtbl g_a_IoVtbl;
		DSKM_Io      g_a_IO = { &g_a_IoVtbl, 0 };

		// ---
		DSKM_Io* InitIoA() {
			LoadIO();
			
			memset( &g_a_IoVtbl, 0, sizeof(g_a_IoVtbl) );
			
			g_a_IoVtbl.AddRef = my_AddRef;
			g_a_IoVtbl.Release = my_Release;
			g_a_IoVtbl.CreateFile = my_a_CreateFile;
			g_a_IoVtbl.CloseHandle = my_CloseHandle;
			g_a_IoVtbl.ReadFile = my_ReadFile;
			g_a_IoVtbl.WriteFile = my_WriteFile; 
			g_a_IoVtbl.SetFilePointer = my_SetFilePointer;
			g_a_IoVtbl.GetFileSize = my_GetFileSize;
			g_a_IoVtbl.FindFirstFile = my_a_FindFirstFile;
			g_a_IoVtbl.FindNextFile = my_a_FindNextFile;
			g_a_IoVtbl.FindClose = my_FindClose;
			
			my_AddRef( &g_a_IO );
			return &g_a_IO;
		}
	#endif // #if defined(DSKM_FILE_API)
	


	// ---
	tVOID _init_dskm_main_folder() {
		tUINT len = 0;
		PR_TRACE_VARS;

		if ( g_bUnderNT ) {
			if ( g_app_id && UNI(g_app_id->m_data_folder)[0] ) {
				if ( g_app_id->m_data_folder_cp == cCP_UNICODE )
					wcscpy_s( UNI(g_dskm_main_folder), countof(g_dskm_main_folder), UNI(g_app_id->m_data_folder) );
				else
					mbstowcs_s( NULL, UNI(g_dskm_main_folder), countof(g_dskm_main_folder), MB(g_app_id->m_data_folder), countof(g_dskm_main_folder) );
				PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM NT preloaded path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
			}
			else {
				HINSTANCE lib = LoadLibrary( "shell32.dll" );
				PFNSHGETFOLDERPATHW pgf = lib ? (PFNSHGETFOLDERPATHW)GetProcAddress(lib,"SHGetFolderPathW") : 0;
				if ( !pgf || FAILED(pgf(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_CURRENT,UNI(g_dskm_main_folder))) )
					wcscpy_s( UNI(g_dskm_main_folder), countof(g_dskm_main_folder), UNI(g_plugins_path) );
				if ( lib )
					FreeLibrary( lib );
				PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM shell NT path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
			}

			len = _toui32(wcslen(UNI(g_dskm_main_folder)));
			if ( len && (UNI(g_dskm_main_folder)[len-1] != L'\\') )
				UNI(g_dskm_main_folder)[len++] = L'\\';

			wcscpy_s( UNI(g_dskm_main_folder)+len, countof(g_dskm_main_folder) - len, PR_LOADER_DSKM_DIR_W );
			//SHCreateDirectoryExW( 0, UNI(g_dskm_main_folder), 0 );
		}
		else {
			if ( g_app_id && MB(g_app_id->m_data_folder)[0] ) {
				if ( g_app_id->m_data_folder_cp == cCP_UNICODE )
					wcstombs_s( NULL, MB(g_dskm_main_folder), sizeof(g_dskm_main_folder), UNI(g_app_id->m_data_folder), sizeof(g_dskm_main_folder) );
				else
					strcpy_s( MB(g_dskm_main_folder), sizeof(g_dskm_main_folder), MB(g_app_id->m_data_folder) );
				PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM 9x preloaded path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
			}
			else {
				HINSTANCE lib = LoadLibrary( "shell32.dll" );
				PFNSHGETFOLDERPATHA pgf = lib ? (PFNSHGETFOLDERPATHA)GetProcAddress(lib,"SHGetFolderPathA") : 0;
				if ( !pgf || FAILED(pgf(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_CURRENT,MB(g_dskm_main_folder))) )
					strcpy_s( MB(g_dskm_main_folder), sizeof(g_dskm_main_folder), MB(g_plugins_path) ); 
				if ( lib )
					FreeLibrary( lib );
				PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM shell 9x path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
			}
			
			len = lstrlen( MB(g_dskm_main_folder) );
			if ( len && (MB(g_dskm_main_folder)[len-1] != '\\') )
				MB(g_dskm_main_folder)[len++] = '\\';
			
			strcpy_s( MB(g_dskm_main_folder)+len, sizeof(g_dskm_main_folder) - len, PR_LOADER_DSKM_DIR );
			//SHCreateDirectoryExA( 0, MB(g_dskm_main_folder), 0 );
		}
		PR_TRACE(( g_root, prtNOTIFY, "ldr\tDSKM path - \"%s%S\"", PR_TRACE_PREPARE_NT(g_dskm_main_folder) ));
	}


	// ---
	tERROR pr_call _find_files_by_mask( const tVOID* mask, tCODEPAGE cp, tDWORD* files_found, pfnAddObjectCallback obj_callback, tVOID* obj_param ) {

		HANDLE           find;
		tUINT            dir_len;
		tUINT            char_size;
		tWCHAR           file_mask[MAX_PATH];
		const tVOID*     dslash;
		const tVOID*     bslash;
		WIN32_FIND_DATAW fd;
		tDWORD           file_count = 0;

		typedef HANDLE (WINAPI  *tFindFirstFile)( const tVOID* fn, tVOID* fd );
		typedef BOOL   (WINAPI  *tFindNextFile)( HANDLE fh, tVOID* fd );
		typedef tUINT  (__cdecl *tSlen)( const tVOID* str );
		typedef tVOID* (__cdecl *tBakSrch)( const tVOID* str, tINT symb );

		tFindFirstFile findFirstFile;
		tFindNextFile  findNextFile;
		tSlen          slen;
		tBakSrch       srch;


		if ( files_found )
			*files_found = 0;

		if ( cp == cCP_UNICODE ) {
			char_size = sizeof(tWCHAR);

			findFirstFile = (tFindFirstFile)FindFirstFileW;
			findNextFile = (tFindNextFile)FindNextFileW;
			slen = (tSlen)wcslen;
			srch = (tBakSrch)wcsrchr;
			wcscpy_s( file_mask, countof(file_mask), UNI(mask) );
		}
		else {
			char_size = sizeof(tCHAR);
			
			findFirstFile = (tFindFirstFile)FindFirstFileA;
			findNextFile = (tFindNextFile)FindNextFileA;
			slen = (tSlen)strlen;
			srch = (tBakSrch)strrchr;
			strcpy_s( MB(file_mask), sizeof(file_mask), MB(mask) );
		}

		bslash = srch( file_mask, PATH_DELIMETER_CHAR );
		dslash = srch( file_mask, '/' );
		if ( dslash < bslash )
			dslash = bslash;
		if ( dslash )
			*((tBYTE**)&dslash) += char_size;
		else
			dslash = mask;

		dir_len = _toui32(MB(dslash) - MB(file_mask));

		find = findFirstFile( file_mask, &fd );
		if ( INVALID_HANDLE_VALUE != find ) {
			BOOL next_file = TRUE;
			for ( ; next_file; next_file = findNextFile(find,&fd) ) {
				tUINT len;
				if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					continue;
				len = char_size * (1 + slen(fd.cFileName));
				memcpy( MB(file_mask)+dir_len, fd.cFileName, len );
				if ( PR_SUCC(obj_callback(file_mask,cp,dir_len+len,obj_param)) )
					++file_count;
			}
			FindClose( find );
		}
		if ( files_found )
			*files_found = file_count;
		return errOK;
	}

#endif


#endif //_SIGNCHK_WIN32_INC_
