// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Sunday,  14 March 2004,  11:19 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- loader.cpp
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pfinder_h__b63906d9_d81a_4080_baeb_c68427103c3f )
#define __pfinder_h__b63906d9_d81a_4080_baeb_c68427103c3f
// AVP Prague stamp end

#if defined (_WIN32)
#	define OLD_METADATA_SUPPORT
#	define ADVANCED_PLUGIN_PROTECTOR // advanced synchro technic
#	if !defined( ADVANCED_PLUGIN_PROTECTOR )
#		define GLOBAL_PLUGIN_PROTECTOR	  // switches on global synchro object to protect plugin's loading
#	endif
#endif

#define NEW_SIGN_LIB
//#define ELF_SUPPORT
//#define _PR_PLUGIN_DONT_REAL_UNLOAD_


// ---
#if defined( __cplusplus )
	extern "C" {
#endif

tERROR pr_call initPluginProtection();
tVOID  pr_call deinitPluginProtection();

#if defined( __cplusplus )
	}
#endif

#if defined(_DEBUG)
	#define NOT_VERIFY
#else
	#define NOT_VERIFY
#endif

//#define NEW_SIGN_LIB -- it's now defined in project file - loader.dsp 
//#define LATE_CHECK

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_schecklist.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end



#if defined( NEW_SIGN_LIB )

	#include <Dskm/dskm.h>
	#define SIGN_API           DSKMAPI
	//#define DSKM_EXE_OBJ_TYPE  32758

#else

	#include <Sign/SIGN.h>
	#define SIGN_API SIGNAPI

#endif


#define PR_TRACE_PREPARE(s)    PR_TRACE_PREPARE_STR_CP( s, cp )
#define PR_TRACE_PREPARE_NT(s) PR_TRACE_PREPARE_STR_CP( s, (g_bUnderNT ? cCP_UNICODE : cCP_ANSI) )


#if defined(__cplusplus)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct LoaderData : public cLoader /*cObjImpl*/ {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();

// Property function declarations

public:
// External function declarations
	tERROR pr_call FindModules( const tVOID* p_param_pool, tDWORD p_param_pool_size, tDWORD p_flags );
	tERROR pr_call LoadModules( tDWORD p_format_flags );
	tERROR pr_call UnloadModules( tDWORD p_format_flags );
	tERROR pr_call RegisterModuleInterfaces();
	tERROR pr_call UnregisterModuleInterfaces();
	tERROR pr_call CheckAutoLoad();
	tERROR pr_call IntegrateModules( const tVOID* p_param_pool, tDWORD p_param_pool_size, tDWORD p_load_flags );
	tERROR pr_call InegrateCommit();
	tERROR pr_call IntegrateRollback();

// Data declaration
	tDWORD m_commit_flags; // flags accepted for commit
	tDWORD m_plugins;    // --
// AVP Prague stamp end



	LoaderData();
	cBuff<cPlugin*,32> m_pluginList;

private:
	tERROR pr_call i_FindModules( const tVOID* param_pool, tDWORD param_pool_size, tDWORD flags );
	tERROR pr_call i_LoadModules( tDWORD format_flags );
	tERROR pr_call i_RegisterModuleInterfaces();
	tERROR pr_call i_CheckAutoLoad();
	tERROR pr_call i_InegrateCommit();

	tERROR pr_call create_module( const tVOID* module_name, tUINT len, tCODEPAGE cp );
	tBOOL  pr_call check_plugin_by_name( const tVOID* module_name, tCODEPAGE cp );

	tERROR pr_call find_modules_in_folder  ( const tVOID* param_pool, tDWORD param_pool_size, tCODEPAGE cp );
	tERROR pr_call find_modules_by_ptr_list( const tVOID* param_pool, tDWORD param_pool_size );
	tERROR pr_call find_modules_by_ptr_list_w( const tVOID* param_pool, tDWORD param_pool_size );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(LoaderData)
};
// AVP Prague stamp end



#endif // __cplusplus

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define PID_WIN32_PLUGIN_LOADER  ((tPID)(5))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Short comments -- prague plugin loader
//    Supplies facility to find, load metadata, register interfaces, etc. of prague loadable plugins.
//    Must create objects of "Plugin" interafce. One object to one loadable plugin
// Loader interface implementation
// Short comments -- prague loadable plugin finder
// Extended comment -
//  Supplies facility to find prague loadable plugins. Must create objects of "Plugin" interafce. One object to one loadable plugin
// AVP Prague stamp end



#if defined( __cplusplus )
	extern "C"
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
tERROR pr_call Loader_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// data structure  is declared in o:\prague\loader\loader.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// ---

#if defined( __cplusplus )
	extern "C" {
#endif

	#include <Prague/pr_loadr.h>
	#include "loader_inc.c"
	
	#if defined (OLD_METADATA_SUPPORT)
		#include <serialize/kldtser.h>
	#endif //OLD_METADATA_SUPPORT

  extern tPTR  g_plugins_path;
	tERROR pr_call _check_by_mask ( const tVOID* mask, tCODEPAGE cp );
	tERROR pr_call CreateModule( hROOT root, tPTR inst, const tVOID* module, tCODEPAGE cp, tPID pid );

#if defined( __cplusplus )
}
#endif


// ---
#define MB(s)    ((tCHAR*)(s))
#define CMB(s)   ((const tCHAR*)(s))
#define UNI(s)   ((tWCHAR*)(s))
#define CUNI(s)  ((const tWCHAR*)(s))

#define MBS     sizeof(tCHAR)
#define UNIS    sizeof(tWCHAR)


// ---
#if defined( _WIN32 )

	typedef HANDLE kl_file_t;

	#define INVALID_FILE                  INVALID_HANDLE_VALUE
	#define closeFile(handle)             CloseHandle(handle)
	#define getFileSize(handle)	          GetFileSize( handle, 0 )
	#define setFilePointer(handle,offset) SetFilePointer( handle, offset, 0, FILE_BEGIN )
			
	#define SLEN(s)  (g_bUnderNT ? (UNIS * wcslen(CUNI(s))) : (MBS*strlen(CMB(s))))
	#define cCP      (g_bUnderNT ? cCP_UNICODE : cCP_ANSI)

	typedef HANDLE  tProtector;
#elif defined (__unix__)

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <errno.h>
	#include <pthread.h>

	typedef int kl_file_t;
	#define INVALID_FILE -1
	#define INVALID_FILE_SIZE -1
	#define closeFile(fd) close (fd); fd = -1
	#define setFilePointer(handle,offset) lseek ( handle, offset, SEEK_SET )

	#define SLEN(s)  (MBS*strlen(CMB(s)))
	#define cCP      cCP_ANSI

	typedef pthread_mutex_t tProtector;
#else

	#error __FILE__LINE__ "must define the same entities as above"

#endif


// ---
typedef enum tag_FileType { 
	ft_unk, ft_disk_file, ft_other_file 
} tFileType;

#if defined( __cplusplus )
extern "C" {
#endif

	tFileType pr_call _is_file_handle( void* obj );

#if defined( __cplusplus )
}
#endif



// ---
typedef struct tag_IOParams {
	hIO    io;
	tDWORD offset;
} tIOParams;


// ---
typedef enum tag_DSKMObjParamType { 
	op_unk, op_io, op_seqio, op_file
} tDSKMObjParamType;



// ---
typedef struct tag_DSKMObjParams {
	tDSKMObjParamType m_type;
	hOBJECT           m_allocator;
	tVOID*            m_name;
	tUINT             m_name_size;
	tCODEPAGE         m_name_cp;
	tBOOL             m_close_it;
	union {
		tIOParams m_io;
		hOBJECT   m_obj;
		hSEQ_IO   m_seq_io;
		kl_file_t m_file;
	};
} cDSKMObjParams;



#if defined( _WIN32 )
	#include "kldirobj/kldirobj.h"
	extern  cKLDirObject      g_NObj;
	extern  HMODULE           g_NObj_dll;
	#define KLDirObjDLL       "kldirobj.dll"
#endif


// ---
tERROR pr_call _open_file( const tVOID* name, tCODEPAGE cp, kl_file_t* file );
tERROR pr_call _set_file_cp( kl_file_t file, tULONG offset );

// ---
typedef tPTR/* unsigned long */ hPLUGININSTANCE;



#if defined( NEW_SIGN_LIB )

	// ---
	typedef struct tag_DSKMAddObjByMask {
		hOBJECT   m_obj;
		HDSKMLIST m_list;
		tDWORD    m_obj_id;
		tBYTE*    m_buff;
		tUINT     m_buff_len;
	} tDSKMAddObjByMask;


	#if defined( __cplusplus )
		extern "C" {
	#endif

			extern tDSKMAppId* g_app_id;
			typedef struct tag_DSKMSession {
				HDSKM             m_lib;
				hCRITICAL_SECTION m_prot;
			} tDSKMSession;

			tVOID              pr_call  _init_dskm_main_sync( tVOID );
			tVOID              pr_call  _deinit_dskm_main_sync( tVOID );
			tVOID              pr_call  _init_dskm_main_folder( tVOID );
			tERROR             pr_call  _reinit_dskm_main_session( const tVOID* new_folder, cCODEPAGE cp, tDWORD* dskm_err );
			tERROR             pr_call  _free_dskm_main_session();
			tERROR             pr_call  _get_dskm_main_session( tDSKMSession* session, tDWORD* dskm_err );
			tERROR             pr_call  _release_dskm_main_session( tDSKMSession* session );

			tERROR             pr_call  _init_dskm_session( hOBJECT obj, HDSKM* session, const tVOID* dskm_path, cCODEPAGE dskm_path_cp, tDWORD* dskm_err );

			tERROR             pr_call  _obj_alloc( hOBJECT obj, tPTR* ptr, tUINT size );
			tVOID              pr_call  _obj_free( hOBJECT obj, tPTR ptr );
			tERROR             pr_call  _alloc_param( hOBJECT obj, cDSKMObjParams** p, tDSKMObjParamType type, tVOID* val, const tVOID* name, tCODEPAGE cp, tUINT size );
			tVOID              pr_call  _destruct_param( cDSKMObjParams* p );

			#define                     _free_param( p ) \
			                              _destruct_param( p ); \
			                              _obj_free( p->m_allocator, p )

			int                pr_call  _convertFileErrorToSignError( tERROR err );
			tERROR             pr_call  _convertSignErr( tDWORD dskm_err );
			const tDSKMCompId* pr_call  _find_comp_id( const tWCHAR* plugin );

			typedef tERROR (pr_call *pfnAddObjectCallback)( const tVOID* obj_name, tCODEPAGE cp, tUINT len, tVOID* param );
			tERROR             pr_call  _find_files_by_mask( const tVOID* mask, tCODEPAGE cp, tDWORD* files_found, pfnAddObjectCallback obj_callback, tVOID* obj_param );
			tERROR             pr_call  _add_reg_by_mask( const tVOID* obj_name, tCODEPAGE cp, tUINT len, tVOID* param );
			tERROR             pr_call  _add_check_obj( const tVOID* obj_name, tCODEPAGE cp, tUINT len, tVOID* param );
			
			int                SIGN_API _dskm_read_buff( void* params, void* pvBuffer, AVP_dword dwBufferSize, AVP_dword dwCommand );
			tDWORD             SIGN_API _dskm_ParList_release( HDSKMLIST hList );

			tUINT              pr_call  _receive_unicode_file_name( const tVOID* path, tCODEPAGE cp, tWCHAR* fname, tUINT fname_size );


			tPTR               pr_call  _os_alloc( tUINT size );
			tVOID              pr_call  _os_free( tPTR ptr );
			tERROR             pr_call  _check_list ( HDSKM lib, hSCHECKLIST list, tDWORD* sign_lib_err_code );

	#if defined( __cplusplus )
		}
	#endif

	#define CONVERT_DSKM_TO_PR_ERR( dskm_err, pr_err )                                                        \
		switch ( dskm_err ) {                                                                                   \
			case DSKM_ERR_OK                        /* 0xe9ba5770 */ : pr_err = errOK;                     break; \
			case DSKM_ERR_NOT_INITIALIZED           /* 0xd10cc37a */ : pr_err = errOBJECT_NOT_INITIALIZED; break; \
			case DSKM_ERR_CANNOT_OPEN_REG_FILE      /* 0x3d0507bd */ : pr_err = errOBJECT_READ;            break; \
			case DSKM_ERR_CANNOT_READ_REG_FILE      /* 0xfd6eb8be */ : pr_err = errOBJECT_READ;            break; \
			case DSKM_ERR_INVALID_REG_FILE          /* 0xc2fef413 */ : pr_err = errOBJECT_INVALID;         break; \
			case DSKM_ERR_INVALID_REG_FILE_VERSION  /* 0x2a592774 */ : pr_err = errOBJECT_INVALID;         break; \
			case DSKM_ERR_INVALID_REG_FILE_OBJTYPE  /* 0x4ef35e4d */ : pr_err = errOBJECT_INVALID;         break; \
			case DSKM_ERR_KEY_NOT_FOUND             /* 0x408b1c02 */ : pr_err = errSIGN_NOTARY_NOT_FOUND;  break; \
			case DSKM_ERR_REG_NOT_FOUND             /* 0x8f995ffd */ : pr_err = errSIGN_NOTARY_NOT_FOUND;  break; \
			case DSKM_ERR_NOT_ALL_OBJECTS_FOUND     /* 0xc58506ed */ : pr_err = errSIGN_NOT_MATCH;         break; \
			case DSKM_ERR_INVALID_SIGN              /* 0x38f3542d */ : pr_err = errSIGN_NOT_MATCH;         break; \
			case DSKM_ERR_INVALID_BUFFER            /* 0xdeabf3f6 */ : pr_err = errPARAMETER_INVALID;      break; \
			case DSKM_ERR_INVALID_PARAMETER         /* 0x5aaeeae0 */ : pr_err = errPARAMETER_INVALID;      break; \
			case DSKM_ERR_INVALID_OBJTYPE           /* 0xe75abf28 */ : pr_err = errBAD_TYPE;               break; \
			case DSKM_ERR_HASH_NOT_FOUND            /* 0x9cef5380 */ : pr_err = errSIGN_NOT_MATCH;         break; \
			case DSKM_ERR_OBJECT_NOT_FOUND          /* 0xa200485b */ : pr_err = errOBJECT_NOT_FOUND;       break; \
			                                                                                                      \
			case DSKM_ERR_ANOTHER_BASE_REG_FOUND    /* 0xdec30ebe */ :                                            \
			case DSKM_ERR_LIB_CODE_HACKED           /* 0x7051b0ac */ :                                            \
			case DSKM_ERR_DATA_PROCESSING           /* 0x6eb9a4a4 */ :                                            \
			case DSKM_ERR_CANNOT_CALC_OBJ_HASH      /* 0x7d11f7e2 */ :                                            \
			case DSKM_ERR_CANNOT_CREATE_SIGN        /* 0x5b118908 */ :                                            \
			case DSKM_ERR_CRIPTO_LIB                /* 0xa0f61271 */ :                                            \
			case DSKM_ERR_CANNOT_CREATE_REG_FILE    /* 0x68b0acf0 */ :                                            \
			case DSKM_ERR_CANNOT_WRITE_REG_FILE     /* 0x308015e7 */ :                                            \
			case DSKM_ERR_CANNOT_WRITE_REG_TOMEM    /* 0xdab8d467 */ :                                            \
			case DSKM_ERR_IO_NOT_LOADED             /* 0x48097982 */ :                                            \
			case DSKM_ERR_ANOTHER_SIGN_FOUND        /* 0xde3346ac */ :                                            \
			case DSKM_UNKNOWN_ERROR                 /* 0x9f1e269c */ :                                            \
			default                                                  : pr_err = errUNEXPECTED;             break; \
		}

	#define PR_TRACE_LEVEL(err,lok,lerr)  (PR_SUCC(err) ? (lok) : (lerr))

#else

	#define _init_dskm_main_folder()
	#define _init_dskm_main_sync()
	#define _deinit_dskm_main_sync()
	#define _free_dskm_main_session()

#endif


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // loader_cpp
// AVP Prague stamp end



