#if ! defined (_MOD_LOAD_WIN32_INC_)
#define _MOD_LOAD_WIN32_INC_

#include <windows.h>

#if defined(ELF_SUPPORT)
	#include <elf_ldr.h>

	using namespace elf;

#endif

#if defined (_MSC_VER)
#pragma warning( disable: 4715)
#pragma hdrstop
#endif

#define TRY      try
#define EXCEPT   catch(...)


#if defined( __cplusplus )
	extern "C" {
#endif

tERROR lockProtector( HANDLE theProtector, hOBJECT obj, const tVOID* name, tCODEPAGE cp );
tERROR unlockProtector( HANDLE theProtector, hOBJECT obj, const tVOID* name, tCODEPAGE cp );

#if defined( __cplusplus )
	}
#endif

// ---
tERROR pr_call _ConvertWin32Err( tDWORD win32err ) {
	switch( win32err ) {
		case ERROR_SUCCESS               : return errOK;

		case ERROR_FILE_NOT_FOUND        :
		case ERROR_PATH_NOT_FOUND        : 
		case ERROR_BAD_NETPATH           : return errOBJECT_NOT_FOUND;

		case ERROR_ALREADY_EXISTS        :
		case ERROR_FILE_EXISTS           : return errOBJECT_ALREADY_EXISTS;

		case ERROR_SHARING_VIOLATION     : 
		case ERROR_LOCK_VIOLATION        :
		case ERROR_DRIVE_LOCKED          : return errLOCKED;

		case ERROR_ACCESS_DENIED         : 
		case ERROR_NETWORK_ACCESS_DENIED : return errACCESS_DENIED;

		case ERROR_NO_MORE_FILES         : return errEND_OF_THE_LIST;
		case ERROR_WRITE_PROTECT         : return errWRITE_PROTECT;

		case ERROR_DISK_FULL             : return errOUT_OF_SPACE;

		case ERROR_NOT_READY             : return errNOT_READY;
		default                          : return errUNEXPECTED;
	}
}



// ---
static HMODULE _load_dll( const tVOID* name, tCODEPAGE cp, tDWORD flags ) {
	size_t size;
	tCHAR  buff[MAX_PATH];
  PR_TRACE_VARS;

	if ( cp != cCP_UNICODE )
	  return LoadLibraryExA( MB(name), NULL, flags );
	if ( g_bUnderNT )
		return LoadLibraryExW( UNI(name), NULL, flags );
	wcstombs_s( &size, buff, countof(buff), UNI(name), _TRUNCATE );
	if ( size != -1 )
		return LoadLibraryExA( buff, NULL, flags );
	PR_TRACE(( 0, prtERROR, "ldr\t_load_dll: cannot convert plugin file name \"%s%S\" to multibyte string", PR_TRACE_PREPARE(name) ));
	return 0;
}



#if defined (OLD_METADATA_SUPPORT)
extern "C" tERROR pr_call GetDllMetaData( const tVOID* szFileName, tCODEPAGE cp, tPTR hModuleInfo ) {
  HMODULE  mod;
  char*    mem;
  HGLOBAL  res;
  HRSRC    src;
  tERROR   error = errMODULE_IS_BAD;
  HDATA*   aModuleInfo = (HDATA*) hModuleInfo;
  PR_TRACE_VARS;

	mod = _load_dll( szFileName, cp, LOAD_LIBRARY_AS_DATAFILE | LOAD_WITH_ALTERED_SEARCH_PATH );
  
  if ( mod ) {
    if ((0 != (src=FindResource(mod,MAKEINTRESOURCE(1),RT_RCDATA))) &&
        (0 != (res=LoadResource(mod,src))) &&
        (0 != (mem=(char*)LockResource(res))) )
    {
      HDATA aDataTree = 0;
      if ( KLDT_DeserializeFromMemoryUsingSWM(mem,SizeofResource(mod, src), &aDataTree ) ) {
        *aModuleInfo = aDataTree;
        error = errOK;
       }
      else {
        PR_TRACE(( 0, prtERROR, "ldr\tDescription for module \"%s%S\" cannot be deserialized !", PR_TRACE_PREPARE(szFileName) ));
        error = errUNEXPECTED;
      }
    }
    FreeLibrary( mod );
  }
  return error;
}

#endif // OLD_METADATA_SUPPORT

// ---
extern "C" tPluginInit pr_call GetStartAddress( hPLUGININSTANCE hInst ) {
  PIMAGE_DOS_HEADER pDosHeader;
  PIMAGE_NT_HEADERS pNTHeader;
  DWORD dwEntryPoint = 0;
  
  
  _try {
    if( *((LPDWORD)hInst) == cHEADER_SIGNATURE )
      return (tPluginInit)((tBYTE*)hInst+((PrFileHeader*)hInst)->dwEntryPoint);

		#if defined(ELF_SUPPORT)
		if ( checkElfHeader(hInst) ) {
				//GetElfEntryPoint( hInst );
			}
		#endif

    pDosHeader = (PIMAGE_DOS_HEADER)hInst;
    if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE ) {
      PR_TRACE(( 0, prtERROR, "ldr\tMZ header not found" ));
      return 0; // Does not point to a correct value
    }
    
    pNTHeader=(PIMAGE_NT_HEADERS)((PCHAR)hInst+pDosHeader->e_lfanew);
    if( pNTHeader->Signature != IMAGE_NT_SIGNATURE ) {
			PR_TRACE(( 0, prtERROR, "ldr\tPE header not found" ));
			return 0; // It is not a PE header position
    }
    
    return (tPluginInit)( (DWORD)hInst + pNTHeader->OptionalHeader.AddressOfEntryPoint );
  }
  __except ( EXCEPTION_EXECUTE_HANDLER ) {
    return 0;
  }
}

#if defined(SHOW_START_ADDRESS)
void ShowPluginStartAddress(tCHAR* pluginName, tDWORD Addr)
{
	HKEY hReg = NULL;
	if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, "Software\\KasperskyLab", &hReg))
	{
		DWORD type = REG_DWORD;
		DWORD bShowAddress;
		DWORD ShowAddress_size = sizeof(bShowAddress);
		if (ERROR_SUCCESS == RegQueryValueEx(hReg, "PrLoader_AInf", 0, &type, (LPBYTE) &bShowAddress, &ShowAddress_size))
		{
			if (bShowAddress)
			{
				char addrinfo[512];
				wsprintf(addrinfo, "0x%x %.260s\n", Addr, pluginName);
				OutputDebugString(addrinfo);
			}
		}
		RegCloseKey(hReg);
	}
}
#endif





// ---
static tERROR pr_call LoadDllPlugin( const tVOID* szPluginName, tCODEPAGE cp, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) {
	tERROR err;
	DWORD  old_error_mode;
  PR_TRACE_VARS;
	
	PR_TRACE(( 0, prtSPAM, "ldr\tLoadDllPlugin: Loading native plugin \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));

	if ( !pInst || !pEntryPoint )
		return errPARAMETER_INVALID;
	
	*pInst = 0;
	*pEntryPoint = 0;
	old_error_mode = SetErrorMode( SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX );

	try {
		*pInst = _load_dll( szPluginName, cp, LOAD_WITH_ALTERED_SEARCH_PATH );

		if ( *pInst ) {
			err = errOK;
			*pEntryPoint = GetStartAddress( *pInst );
			PR_TRACE(( 0, prtSPAM, "ldr\tLoadDllPlugin.GetProcAddress \"%s%S\" with %X", PR_TRACE_PREPARE(szPluginName), pEntryPoint ));
		}
		else {
			err = errMODULE_CANNOT_BE_LOADED;
			PR_TRACE(( 0, prtERROR, "ldr\tCannot load library \"%s%S\" (win error - 0x%x)", PR_TRACE_PREPARE(szPluginName), GetLastError() ));
		}
	}
  catch(...) {
		*pInst = 0;
    *pEntryPoint = 0;
		err = errUNEXPECTED;
		PR_TRACE( (0,prtERROR,"ldr\tLoadDllPlugin: exception catched!") );
	}
	SetErrorMode( old_error_mode );
	return err;
}




//
// Unloading function
//

tERROR pr_call UnloadDllPlugin( hPLUGININSTANCE hInst ) {
  FreeLibrary( (HINSTANCE)hInst );
  return errOK;
}



#if defined( ELF_SUPPORT )

	// ---
//	static ELFIO_Err __cdecl elfReadFunc( void* ptr, void* buff, Elf32_Word offset, Elf32_Word size, Elf32_Word& read ) {
//		if ( !ptr )
//			return ERR_CANT_READ;
//		WholeFileMapping* map = static_cast<WholeFileMapping*>(ptr);
//		WholeFileMapping::kl_size_t msize = map->size();
//		if ( offset > msize )
//			return ERR_CANT_READ;
//		if ( (offset+size) > msize )
//			size = msize - offset;
//		tBYTE* mdata = (tBYTE*)map->data();
//		__try {
//			memcpy( buff, mdata+offset, size );
//			read = size;
//			return ERR_NO_ERROR;
//		}
//		__except( EXCEPTION_EXECUTE_HANDLER ) {
//			read = 0;
//			return ERR_CANT_READ;
//		}
//	}



	// ---
	static bool _check_jmp( cJmpRelocationEnum& jmp, Elf32_Word sym ) {
		for( jmp.reset(); jmp; ++jmp ) {
			Elf32_Word jms = jmp.symbol();
			if ( jms == sym )
				return true;
		}
		return false;
	}


	// ---
	class cPointer {
		protected:
			void* m_ptr;
		public:
			cPointer( void* base ) : m_ptr(base) {}
			~cPointer() {
				if ( m_ptr )
					VirtualFree( m_ptr, 0, MEM_FREE );
			}
			operator bool  ()       const { return !!m_ptr; }
			operator void* ()       const { return m_ptr; }
			operator const void* () const { return m_ptr; }
			operator char* ()       const { return (char*)m_ptr; }
			operator const char* () const { return (char*)m_ptr; }

			bool  operator ! ()                    const { return !m_ptr; }
			char* operator + ( Elf32_Addr addend ) const { return ((char*)m_ptr) + addend; }

			Elf32_Addr addr() const { return (Elf32_Addr)m_ptr; }
			void* relinquish() { void* t=m_ptr; m_ptr=0; return t; }
	};


	// ---
	static tERROR pr_call LoadElfPlugin( const tVOID* szPluginName, tCODEPAGE cp, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint, void (__cdecl ** finish_proc)(void) ) {
		
		PR_TRACE_VARS;
		
		if ( pInst )
			*pInst = 0;
		if ( pEntryPoint )
			*pEntryPoint = 0;

		WholeFileMapping theFileMapping;
		tCHAR*           theContent = 0;

		#if defined (_WIN32)
			if ( cp == cCP_UNICODE )
				theContent = (tCHAR*)theFileMapping.map( UNI(szPluginName), KL_FILEMAP_READ );
			else
		#endif
				theContent = (tCHAR*)theFileMapping.map( MB(szPluginName), KL_FILEMAP_READ );

		if ( !theContent || (theContent == (tVOID*)-1) ) {
			PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Cannot map Elf plugin \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
			return errUNEXPECTED;
		}

		cElfHeader elf( theContent );
		if ( !elf ) {
			PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Cannot check/load Elf plugin \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
			return errMODULE_CANNOT_BE_LOADED;
		}

		Elf32_Addr max_size = 0;
		cSegmentEnum seg( elf, PT_LOAD );
		for( ; seg; ++seg ) {
			Elf32_Word msize = seg.msize();
			if ( !msize )
				continue;
			Elf32_Addr addr = seg.vaddr();
			addr += msize;
			if ( max_size < addr )
				max_size = addr;
		}

		cPointer base = VirtualAlloc( 0, max_size, MEM_COMMIT, PAGE_READWRITE );
		if ( !base ) {
			PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: not enough memory to load Elf plugin \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
			return errMODULE_CANNOT_BE_LOADED;
		}

		for( seg.reset(); seg; ++seg ) {

			Elf32_Word msize = seg.msize();
			if ( !msize )
				continue;

			Elf32_Word fsize = seg.fsize();
			Elf32_Word foffs = seg.foffset();
			Elf32_Addr maddr = seg.vaddr();
			try {
				memcpy( base+maddr, theContent+foffs, fsize );
				if ( fsize < msize )
					memset( base+maddr+fsize, 0, msize-fsize );
			}
			catch(...) {
				PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: memory exception to load Elf plugin \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
				return errUNEXPECTED;
			}
		}

		DWORD oldProtect = 0;
		VirtualProtect( base, max_size, PAGE_EXECUTE_READWRITE, &oldProtect );

		if ( !seg.reset(PT_DYNAMIC) ) {
			PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Elf plugin has no Dynamic segment \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
			return errUNEXPECTED;
		}

		//cStringTbl         str_tbl( elf, seg );
		cSymbolTbl         sym_tbl( elf, seg );
		cRelocationEnum    rel( elf, seg );
		cJmpRelocationEnum jmp( elf, seg );
		if ( /*!str_tbl ||*/ !sym_tbl || !rel ) {
			PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Elf plugin has wrong Dynamic segment content\"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
			return errMODULE_CANNOT_BE_LOADED;
		}

		for( ; rel; ++rel ) {
			Elf32_Word  type = rel.type();
			Elf32_Word  sndx;
			Elf32_Addr* addr;
			cSymbol     sym;
			//const char* name;
			switch( type ) {
				case R_386_NONE     : /* none none        */
					break;
				case R_386_32       : /* word32 S + A     */
					sndx = rel.symbol();
					if ( !sndx || _check_jmp(jmp,sndx) )
						break;
					sym = sym_tbl[sndx];
					if ( !sym ) {
						PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Elf plugin has unresolved symbol in relocation entry \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
						return errMODULE_CANNOT_BE_LOADED;
					}
					//name = sym.name( str_tbl );
					addr = (Elf32_Addr*)(base + rel.offset());
					*addr += sym.val();
					break;
				case R_386_GLOB_DAT : /* word32 S         */
				case R_386_JMP_SLOT : /* word32 S         */
					sndx = rel.symbol();
					if ( !sndx || _check_jmp(jmp,sndx) )
						break;
					sym = sym_tbl[sndx];
					if ( !sym ) {
						PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Elf plugin has unresolved symbol in relocation entry \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
						return errMODULE_CANNOT_BE_LOADED;
					}
					//name = sym.name( str_tbl );
					addr = (Elf32_Addr*)(base + rel.offset());
					*addr = sym.val();
					break;
				case R_386_RELATIVE : /* word32 B + A     */
					addr = (Elf32_Addr*)(base + rel.offset());
					*addr += base.addr();
					break;
				case R_386_PC32     : /* word32 S + A - P   */
				case R_386_GOT32    : /* word32 G + A - P   */
				case R_386_PLT32    : /* word32 L + A - P   */
				case R_386_COPY     : /* none none          */
				case R_386_GOTOFF   : /* word32 S + A - GOT */
				case R_386_GOTPC    : /* word32 GOT + A - P */
				default             : 
					PR_TRACE(( 0, prtERROR, "ldr\tLoadElfPlugin: Elf plugin has unsupported relocation entry \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
					return errMODULE_CANNOT_BE_LOADED;
			}
		}

		cDynEntryEnum proc( seg, DT_INIT );
		if ( proc && proc.ptr() )
			((void(*)())(base + proc.ptr()))();

		if ( finish_proc && proc.reset(DT_FINI) )
			*finish_proc = ((void(*)())(base + proc.ptr()));

		if ( elf.entry() )
			((void(*)())(base + elf.entry()))();

		if ( pEntryPoint ) {
			int i = 1;
			for( cSymbol sym = sym_tbl[i]; sym; sym = sym_tbl[++i] ) {
				if ( STT_FUNC == sym.type() ) {
					const char* name = sym.name();
					if ( name && *name && !memcmp(name,"DllMain",8) ) {
						*pEntryPoint = (tPluginInit)(base+sym.val());
						break;
					}
				}
			}
		}
		if ( pInst )
			*pInst = (hPLUGININSTANCE)base.relinquish();

		return errOK;
	}



	// ---
	int checkElfPlugin( const void* theFileMapping ) {

		cElfHeader elf( theFileMapping );
		if ( !elf )
			return cUNEXPECTED_ERROR;

		if ( ELFCLASS32 != elf.class_id() )
			return cUNEXPECTED_ERROR;

		if ( EV_CURRENT != elf.elf_version() )
			return cUNEXPECTED_ERROR;

		if ( ELFDATA2LSB != elf.encoding() )
			return cUNEXPECTED_ERROR;

		if ( ET_DYN != elf.type() )
			return cUNEXPECTED_ERROR;

		if ( EM_386 != elf.machine() )
			return cUNEXPECTED_ERROR;

		if ( 0 == elf.segment_num() )
			return cUNEXPECTED_ERROR;

		cSegmentEnum seg( elf, PT_INTERP );
		if ( seg )
			return cUNEXPECTED_ERROR;

		if ( !seg.reset(PT_DYNAMIC) )
			return cUNEXPECTED_ERROR;

		cDynEntryEnum dyn( seg, DT_NEEDED );
		if ( dyn )
			return cUNEXPECTED_ERROR;
		return cELF_PLUGIN_FOUND;

	}
#endif // defined( ELF_SUPPORT )



// ---
int checkNativePlugin( const void* thePlugin ) {
  unsigned char * aRaw = (unsigned char *) thePlugin;
  if ( (aRaw[0]=='M') && (aRaw[1]=='Z') ) {
    PR_TRACE( (0,prtSPAM,"ldr\tNative plugin found") );
    return cNATIVE_PLUGIN_FOUND;
  }

	#if defined( ELF_SUPPORT )
		return checkElfPlugin( thePlugin );
	#else
		return cUNEXPECTED_ERROR;
	#endif
}
#endif // _MOD_LOAD_WIN32_INC_


