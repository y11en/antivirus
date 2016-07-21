#ifndef __pr_loadr_h
#define __pr_loadr_h

#include <Prague/prague.h>


typedef tERROR (pr_call* root_check_func)( hROOT root, tVOID* params );


#define PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH       0x00000001l
#if defined (_WIN32)
	#define PR_LOADER_FLAG_USE_TRY_CATCH           0x00000002l
#else
	#define PR_LOADER_FLAG_USE_TRY_CATCH           0x00000000l
#endif
#define PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_PG      0x00000004l
#define PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_NATIVE  0x00000008l
#define PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_ALL     0x0000000Cl

#define PR_LOADER_FLAG_DO_NOT_INITIALIZE_COM     0x00100000l
#define PR_LOADER_FLAG_SEND_IDLE_MSGS            0x00200000l

#define PR_LOADER_FLAG_2L_CS_DEFAULT             0x01000000l
#define PR_LOADER_FLAG_APP_ID                    0x02000000l
#define PR_LOADER_START_IDLE_THREAD              0x04000000l
#define PR_LOADER_START_TRACER                   0x08000000l


#define DSKM_STR_ID_SIZE 100

// ---
typedef enum tag_DSKMFileDscType {
	fdt_unk, fdt_ansi_mask, fdt_unicode_mask, fdt_method
} tDSKMFileDscType;


typedef tBOOL (pr_call *pfnSelectFile)( const tVOID* fname, tCODEPAGE cp );

// ---
typedef struct tag_DSKMFileId {
	tDSKMFileDscType m_type;
	const tVOID*     m_ptr;
} tDSKMFileId;


// ---
typedef struct tag_DSKMCompId {
	tCHAR        m_szId[DSKM_STR_ID_SIZE];
	tCHAR        m_szType[DSKM_STR_ID_SIZE];
	tCHAR        m_szVer[DSKM_STR_ID_SIZE];
	tQWORD       m_qwVer;
	tDWORD       m_dwFileIdCount;
	const tDSKMFileId* m_fileIds;
} tDSKMCompId;



// ---
typedef struct tag_DSKMAppId {
	tDWORD             m_AppId_size;
	tDWORD             m_CompId_size;
	tCHAR              m_szId[DSKM_STR_ID_SIZE];
	tCHAR              m_szVer[DSKM_STR_ID_SIZE];
	tQWORD             m_qwVer;
	tDWORD             m_dwCompCount;
	const tDSKMCompId* m_comps;
	tPTR               m_data_folder;
	tCODEPAGE          m_data_folder_cp;
	tUINT              m_data_folder_size;
} tDSKMAppId;


// ---
typedef tERROR (pr_call* PragueLoadFuncOld)( 
	hROOT*          root_object,         // pointer to the variable to store created root object 
	tDWORD          init_flags,          // initialize flags
	root_check_func root_check_func_ptr, // function to call immediately after creating root object
	tPTR            rcf_params           // it's parameters
);


// ---
typedef tERROR (pr_call* PragueLoadFuncEx)( 
	hROOT*          root_object,         // pointer to the variable to store created root object 
	tDWORD          init_flags,          // initialize flags
	root_check_func root_check_func_ptr, // function to call immediately after creating root object
	tPTR            rcf_params,          // it's parameters
	tDSKMAppId*     app_id               // application identification struct
);


// ---
typedef tERROR (pr_call* PragueUnloadFunc)( hROOT root );


// ---
#if defined (_WIN32)
	#define	PR_KERNEL_MODULE_NAME    "prkernel.ppl"
	#define	PR_LOADER_MODULE_NAME    "prloader.dll"
	#define PR_LOADER_DSKM_DIR       "dskm\\"

	#define	PR_KERNEL_MODULE_NAME_W  L"prkernel.ppl"
	#define	PR_LOADER_MODULE_NAME_W  L"prloader.dll"
	#define PR_LOADER_DSKM_DIR_W     L"dskm\\"

	#include <windows.h>

#else
	#define	PR_KERNEL_MODULE_NAME  "prkernel." PLUGIN_EXT
	#define	PR_LOADER_MODULE_NAME  "loader." SHARED_EXT
	#define	PR_KERNEL_WIN32_NAME   "prkernel.ppl"
	#define PR_LOADER_DSKM_DIR     "dskm/"

        #include <string.h>
#endif	

#define PR_LOADER_LOAD_FUNC    "PragueLoad"
#define PR_LOADER_UNLOAD_FUNC  "PragueUnload"

#define PR_LOADER_DLLMAIN_SIGN_CHECK_FUNC   102
#define PR_LOADER_DLLMAIN_SIGN_CHECK_NOTIFY 103

typedef tERROR (pr_call* tSignCheckFunc)( tCHAR* file_name, tCODEPAGE cp );
typedef tERROR (pr_call* tSignCheckNotify)( tCHAR* file_name, tERROR result );

#define CROFLAG_UNSAVE_VTBL (0x00000001L)

// ---
typedef struct tag_CROParam {
	hROOT*  root;	/* tPTR val[3]; */
	tERROR* error;
	tPTR    prot;
	tDWORD  size;
	tDWORD  flags;
} tCROParam;

//#define param_ROOT(param)  ( /*(hROOT*)*/  (((tCROParam*)param)->val[0]) )
//#define param_ERROR(param) ( /*(tERROR*)*/ (((tCROParam*)param)->val[1]) )
//#define param_PROT(param)  ( /*(iPROT*)*/  (((tCROParam*)param)->val[2]) )

#define param_SIZE(param)  ( /*(hROOT*)*/  (((tCROParam*)param)->size) )
#define param_FLAGS(param) ( /*(hROOT*)*/  (((tCROParam*)param)->flags) )
#define param_ROOT(param)  ( /*(hROOT*)*/  (((tCROParam*)param)->root)  )
#define param_ERROR(param) ( /*(tERROR*)*/ (((tCROParam*)param)->error) )
#define param_PROT(param)  ( /*(iPROT*)*/  (((tCROParam*)param)->prot)  )

// Kernel init procedure
// parameters are:
//   root - 
#if defined (_MSC_VER)

typedef tBOOL (__stdcall* tCreateRootObject)( tPTR hInstance, tDWORD dwReason, tCROParam* param );

#if _MSC_VER >= 1400
#	define STRCPY( dst, size, src )   strcpy_s( dst, size, src )
#else
#	define STRCPY( dst, size, src )   strcpy( dst, src )
#endif

#else

typedef tBOOL (* tCreateRootObject)( tPTR hInstance, tDWORD dwReason, tCROParam* param ) __stdcall;

#define STRCPY( dst, size, src )   strcpy( dst, size, src )

#endif

// -------------------------------------------
#if defined(__cplusplus)

#include "iface/i_root.h"

#ifdef _WIN32
	#define LOAD_LIBRARY(library) LoadLibraryExA( library, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
	#define GET_SYMBOL(library,symbol) GetProcAddress ((HMODULE)library, symbol )
	#define UNLOAD_LIBRARY(library) FreeLibrary ((HMODULE) library )
	#define OUTPUT_DEBUG_STRING(str) OutputDebugStringA ( str )
#else
	#include <dlfcn.h>
	#define LOAD_LIBRARY(library) dlopen ( library, RTLD_NOW )
	#define GET_SYMBOL(library,symbol) dlsym ( (void*)library, symbol )
	#define UNLOAD_LIBRARY(library) dlclose ( (void*)library )
	#define OUTPUT_DEBUG_STRING(str) fprintf ( stderr, "%s", str )
#endif


// ---
class CPrague {
public:
	CPrague() : m_module(NULL), m_root(NULL){}
	CPrague( 
		tPTR hModule, 
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,  
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	) : m_module(NULL), m_root(NULL) { 
		Init( hModule, nFlags, root_check_func_ptr,rcf_params, app_id ); 
	}
	~CPrague() { Deinit(); }

	tERROR Init( 
		tPTR hModule, 
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	);

	tERROR InitByFolder( 
		const tCHAR* loader_folder, 
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	);

#if defined (_WIN32)
	tERROR InitByFolderW( 
		const tWCHAR* loader_folder, 
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	);
#endif

	tERROR InitRoot(
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	);

	void     Deinit();
	tERROR   GetError()           { return m_err; };
	operator hROOT()       const  { return m_root; };
	hROOT    operator ->() const  { return m_root; };

protected:
	tPTR      m_module;
	hROOT     m_root;
	tERROR    m_err;
};

// ---
typedef CPrague cPrague;

// ---
class cPragueEx : public CPrague {
public :
	cPragueEx( 
		const tCHAR* loader_folder, 
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,  
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	) : CPrague() { 
		InitByFolder( loader_folder, nFlags, root_check_func_ptr,rcf_params, app_id ); 
	}
	operator hROOT() const  { return m_root; };
};

// ---
#if defined (_WIN32)
class cPragueExW : public CPrague {
public:
	cPragueExW( 
		const tWCHAR* loader_folder, 
		tDWORD nFlags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH,  
		root_check_func root_check_func_ptr = 0, 
		tPTR rcf_params = 0, 
		tDSKMAppId* app_id = 0
	) : CPrague() { 
		InitByFolderW( loader_folder, nFlags, root_check_func_ptr,rcf_params, app_id ); 
	}
	operator hROOT() const  { return m_root; };
};
#endif


// ---
inline tERROR CPrague::Init( tPTR hModule, tDWORD nFlags, root_check_func root_check_func_ptr, tPTR rcf_params, tDSKMAppId* app_id ) {
	m_module = LOAD_LIBRARY( PR_LOADER_MODULE_NAME );

#if defined (_WIN32)
	if( !m_module ) {
		tCHAR szLoadPath[MAX_PATH];
		if( !::GetModuleFileNameA((HMODULE)hModule, szLoadPath, sizeof(szLoadPath)) )
			*szLoadPath = 0;
		tCHAR* pModuleName = strrchr( szLoadPath, '\\' );
		if( !pModuleName )
			pModuleName = szLoadPath;
		else
			pModuleName++;

		STRCPY( pModuleName, pModuleName-szLoadPath, PR_LOADER_MODULE_NAME );
		m_module = LOAD_LIBRARY(szLoadPath);
	}
#elif defined (__unix__)
	if ( m_module ) {
		tPluginInit l_dll_main = (tPluginInit)dlsym( m_module, PLUGIN_INIT_ENTRY );
		tERROR l_reserved;
		if ( !l_dll_main || !l_dll_main (m_module,DLL_PROCESS_ATTACH,&l_reserved) ) { 
			dlclose ( m_module );  
			m_module = 0;                       
		}
	}
#endif 
	
	if( !m_module ) {
		#if defined(_DEBUG)
			tDWORD os_err = GetLastError();
		#endif
		return m_err = errMODULE_NOT_FOUND;
	}

	return InitRoot(nFlags, root_check_func_ptr, rcf_params, app_id);
}


// ---
inline tERROR CPrague::InitByFolder( const tCHAR* loader_folder, tDWORD nFlags, root_check_func root_check_func_ptr, tPTR rcf_params, tDSKMAppId* app_id ) {

	tCHAR szLoadPath[ MAX_PATH ];
	const tCHAR* pModuleName;

	if ( loader_folder && *loader_folder ) {
		tSIZE_T path_len = strlen( loader_folder );
		tSIZE_T name_len = strlen( PR_LOADER_MODULE_NAME ) + 1;
		tSIZE_T add_slash = ((*(loader_folder+path_len-1)) != PATH_DELIMETER_CHAR);
		if ( (path_len + add_slash + name_len) > MAX_PATH )
			return errPARAMETER_INVALID;
		memcpy( szLoadPath, loader_folder, path_len );
		if ( add_slash )
			*(szLoadPath+path_len) = PATH_DELIMETER_CHAR;
		memcpy( szLoadPath+path_len+add_slash, PR_LOADER_MODULE_NAME, name_len );
		pModuleName = szLoadPath;
	}
	else
		pModuleName = PR_LOADER_MODULE_NAME;
	m_module = LOAD_LIBRARY( pModuleName );

#if defined (__unix__)
	if ( m_module ) {
		tPluginInit l_dll_main = (tPluginInit)dlsym( m_module, PLUGIN_INIT_ENTRY );
		tERROR l_reserved;
		if ( !l_dll_main || !l_dll_main (m_module,DLL_PROCESS_ATTACH,&l_reserved) ) { 
			dlclose ( m_module );  
			m_module = 0;                       
		}
	}
#endif 
	
	if( !m_module ) {
		#if defined(_DEBUG)
			tDWORD os_err = GetLastError();
		#endif
		return m_err = errMODULE_NOT_FOUND;
	}
	return InitRoot( nFlags, root_check_func_ptr, rcf_params, app_id );
}


// ---
#if defined(_WIN32)
inline tERROR CPrague::InitByFolderW( const tWCHAR* loader_folder, tDWORD nFlags, root_check_func root_check_func_ptr, tPTR rcf_params, tDSKMAppId* app_id ) {

#define UNICODE_MAX_PATH 32000

	tWCHAR szLoadPath[ 10 /*MAX_PATH*/ ];
	tWCHAR* buff = szLoadPath;
	const tWCHAR* pModuleName;

	if ( loader_folder && *loader_folder ) {
		tSIZE_T path_len = wcslen( loader_folder );
		tSIZE_T name_len = wcslen( PR_LOADER_MODULE_NAME_W ) + 1;
		tSIZE_T add_slash = (*(loader_folder+path_len-1) != PATH_DELIMETER_WIDECHAR);
		if ( (path_len + add_slash + name_len) > countof(szLoadPath) ) {
			if ( (path_len + add_slash + name_len) > UNICODE_MAX_PATH )
				return errPARAMETER_INVALID;
			buff = new tWCHAR[ path_len + add_slash + name_len ];
			if ( !buff )
				return errNOT_ENOUGH_MEMORY;
		}
		memcpy( (tPTR)buff, loader_folder, path_len * sizeof(tWCHAR) );
		if ( add_slash )
			*(buff+path_len) = PATH_DELIMETER_WIDECHAR;
		memcpy( buff+path_len+add_slash, PR_LOADER_MODULE_NAME_W, name_len * sizeof(tWCHAR) );
		pModuleName = buff;
	}
	else
		pModuleName = PR_LOADER_MODULE_NAME_W;
	m_module = LoadLibraryExW( pModuleName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if ( buff != szLoadPath )
		delete [] buff;

	if( !m_module ) {
		#if defined(_DEBUG)
			tDWORD os_err = GetLastError();
		#endif
		return m_err = errMODULE_NOT_FOUND;
	}
	return InitRoot( nFlags, root_check_func_ptr, rcf_params, app_id );
}
#endif


// ---
inline tERROR CPrague::InitRoot( tDWORD nFlags, root_check_func root_check_func_ptr, tPTR rcf_params, tDSKMAppId* app_id ) {
	m_err = errMODULE_CANNOT_BE_LOADED;
	PragueLoadFuncEx plf = (PragueLoadFuncEx)GET_SYMBOL(m_module, PR_LOADER_LOAD_FUNC);
	if( plf )
		m_err = plf(&m_root, nFlags, root_check_func_ptr, rcf_params, app_id);
	if( PR_SUCC(m_err) )
		g_root = m_root;
	return m_err;
}



// ---
inline void CPrague::Deinit() {
	if( m_module ) {
		PragueUnloadFunc pulf = (PragueUnloadFunc)GET_SYMBOL( m_module, PR_LOADER_UNLOAD_FUNC );
		if ( pulf )
			m_err = pulf(m_root);
		g_root = m_root = 0;
#if defined (__unix__)
		tPluginInit l_dll_main = (tPluginInit) ( dlsym ( m_module, PLUGIN_INIT_ENTRY ) );
		tERROR l_reserved;
		if ( l_dll_main )
			l_dll_main ( m_module, DLL_PROCESS_DETACH, &l_reserved );
#endif
		UNLOAD_LIBRARY(m_module);
		m_module = NULL;
	}
}

#endif // __cplusplus

// -------------------------------------------

#endif


