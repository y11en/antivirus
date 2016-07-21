// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  12 February 2004,  13:07 --------
// File Name   -- (null)i_os.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_os__37b13a8a_ae7b_11d4_b757_00d0b7170e50 )
#define __i_os__37b13a8a_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
// Short comments -- object system interface
//    Interface defines behavior of an object system. It responds for
//      - enumerate IO and Folder objects by creating Folder objects.
//      - create and delete IO and Folder objects
//    There are two possible ways to create or open IO object on OS. First is three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_OS  ((tIID)(12))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// OS type constants
#define OS_TYPE_GENERIC                ((tOS_ID)(1)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_OS_VOLUME 0x00001000 // (4096) -- сообщения о томах

	#define pm_OS_VOLUME_HASH 0x00001000 // (4096) -- сообщение о найденном томе
	// context comment
	//    hIO
	//
	// data content comment
	//    QWORD hash

	#define pm_OS_VOLUME_SET_INCOMPLETE 0x00001001 // (4097) -- сообщение о том, что найденный набор томов не является полным
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hOS;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iOSVtbl;
typedef struct iOSVtbl iOSVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cOS;
	typedef cOS* hOS;
#else
	typedef struct tag_hOS {
		const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hOS;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( OS_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpOS_PtrCreate) ( hOS _this, hObjPtr* result, hOBJECT name );              // -- opens ObjPtr object;
	typedef   tERROR (pr_call *fnpOS_IOCreate)  ( hOS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ); // -- opens IO object;
	typedef   tERROR (pr_call *fnpOS_Copy)      ( hOS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ); // -- copies object inside OS;
	typedef   tERROR (pr_call *fnpOS_Rename)    ( hOS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ); // -- rename/move object by name inside OS;
	typedef   tERROR (pr_call *fnpOS_Delete)    ( hOS _this, hOBJECT name );              // -- phisically deletes an entity on OS;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iOSVtbl {
	fnpOS_PtrCreate  PtrCreate;
	fnpOS_IOCreate   IOCreate;
	fnpOS_Copy       Copy;
	fnpOS_Rename     Rename;
	fnpOS_Delete     Delete;
}; // "OS" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( OS_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION   mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_OS_TYPE      mPROPERTY_MAKE_GLOBAL( pTYPE_OS_ID   , 0x00000049 )
//! #define pgOBJECT_NAME         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_AVAILABILITY mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
//! #define pgOBJECT_BASED_ON     mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000053 )
#define pgNAME_MAX_LEN        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgNAME_CASE_SENSITIVE mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001001 )
#define pgNAME_DELIMITER      mPROPERTY_MAKE_GLOBAL( pTYPE_WORD    , 0x00001002 )
#define pgFORMAT_MEDIA        mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001003 )
//! #define pgOBJECT_HASH         mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00000055 )
//! #define pgOBJECT_REOPEN_DATA  mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00000056 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_OS_PtrCreate( _this, result, name )                       ((_this)->vtbl->PtrCreate( (_this), result, name ))
	#define CALL_OS_IOCreate( _this, result, name, access_mode, open_mode ) ((_this)->vtbl->IOCreate( (_this), result, name, access_mode, open_mode ))
	#define CALL_OS_Copy( _this, src_name, dst_name, overwrite )           ((_this)->vtbl->Copy( (_this), src_name, dst_name, overwrite ))
	#define CALL_OS_Rename( _this, old_name, new_name, overwrite )         ((_this)->vtbl->Rename( (_this), old_name, new_name, overwrite ))
	#define CALL_OS_Delete( _this, name )                                  ((_this)->vtbl->Delete( (_this), name ))
#else // if !defined( __cplusplus )
	#define CALL_OS_PtrCreate( _this, result, name )                       ((_this)->PtrCreate( result, name ))
	#define CALL_OS_IOCreate( _this, result, name, access_mode, open_mode ) ((_this)->IOCreate( result, name, access_mode, open_mode ))
	#define CALL_OS_Copy( _this, src_name, dst_name, overwrite )           ((_this)->Copy( src_name, dst_name, overwrite ))
	#define CALL_OS_Rename( _this, old_name, new_name, overwrite )         ((_this)->Rename( old_name, new_name, overwrite ))
	#define CALL_OS_Delete( _this, name )                                  ((_this)->Delete( name ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iOS {
		virtual tERROR pr_call PtrCreate( hObjPtr* result, hOBJECT name ) = 0; // -- opens ObjPtr object
		virtual tERROR pr_call IOCreate( hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ) = 0; // -- opens IO object
		virtual tERROR pr_call Copy( hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ) = 0; // -- copies object inside OS
		virtual tERROR pr_call Rename( hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ) = 0; // -- rename/move object by name inside OS
		virtual tERROR pr_call Delete( hOBJECT name ) = 0; // -- phisically deletes an entity on OS
	};

	struct pr_abstract cOS : public iOS, public iObj {

		OBJ_IMPL( cOS );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hOS()   { return (hOS)this; }

		tOS_ID pr_call get_pgOBJECT_OS_TYPE() { return (tOS_ID)getDWord(pgOBJECT_OS_TYPE); }
		tERROR pr_call set_pgOBJECT_OS_TYPE( tOS_ID value ) { return setDWord(pgOBJECT_OS_TYPE,(tDWORD)value); }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tDWORD pr_call get_pgOBJECT_AVAILABILITY() { return (tDWORD)getDWord(pgOBJECT_AVAILABILITY); }
		tERROR pr_call set_pgOBJECT_AVAILABILITY( tDWORD value ) { return setDWord(pgOBJECT_AVAILABILITY,(tDWORD)value); }

		hOBJECT pr_call get_pgOBJECT_BASED_ON() { return (hOBJECT)getObj(pgOBJECT_BASED_ON); }
		tERROR pr_call set_pgOBJECT_BASED_ON( hOBJECT value ) { return setObj(pgOBJECT_BASED_ON,(hOBJECT)value); }

		tDWORD pr_call get_pgNAME_MAX_LEN() { return (tDWORD)getDWord(pgNAME_MAX_LEN); }
		tERROR pr_call set_pgNAME_MAX_LEN( tDWORD value ) { return setDWord(pgNAME_MAX_LEN,(tDWORD)value); }

		tBOOL pr_call get_pgNAME_CASE_SENSITIVE() { return (tBOOL)getBool(pgNAME_CASE_SENSITIVE); }
		tERROR pr_call set_pgNAME_CASE_SENSITIVE( tBOOL value ) { return setBool(pgNAME_CASE_SENSITIVE,(tBOOL)value); }

		tWORD pr_call get_pgNAME_DELIMITER() { return (tWORD)getWord(pgNAME_DELIMITER); }
		tERROR pr_call set_pgNAME_DELIMITER( tWORD value ) { return setWord(pgNAME_DELIMITER,(tWORD)value); }

		tBOOL pr_call get_pgFORMAT_MEDIA() { return (tBOOL)getBool(pgFORMAT_MEDIA); }
		tERROR pr_call set_pgFORMAT_MEDIA( tBOOL value ) { return setBool(pgFORMAT_MEDIA,(tBOOL)value); }

		tQWORD pr_call get_pgOBJECT_HASH() { return (tQWORD)getQWord(pgOBJECT_HASH); }
		tERROR pr_call set_pgOBJECT_HASH( tQWORD value ) { return setQWord(pgOBJECT_HASH,(tQWORD)value); }

		tERROR pr_call get_pgOBJECT_REOPEN_DATA( tPTR value, tDWORD size ) { return get(pgOBJECT_REOPEN_DATA,value,size); }
		tERROR pr_call set_pgOBJECT_REOPEN_DATA( tPTR value, tDWORD size ) { return set(pgOBJECT_REOPEN_DATA,value,size); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_os__37b13a8a_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(OS_PROXY_DEFINITION)
#define OS_PROXY_DEFINITION

PR_PROXY_BEGIN(OS)
	PR_PROXY(OS, PtrCreate, PR_ARG2(tid_OBJECT|tid_POINTER,tid_OBJECT))
	PR_PROXY(OS, IOCreate,  PR_ARG4(tid_OBJECT|tid_POINTER,tid_OBJECT,tid_DWORD,tid_DWORD))
	PR_PROXY(OS, Copy,      PR_ARG3(tid_OBJECT,tid_OBJECT,tid_BOOL))
	PR_PROXY(OS, Rename,    PR_ARG3(tid_OBJECT,tid_OBJECT,tid_BOOL))
	PR_PROXY(OS, Delete,    PR_ARG1(tid_OBJECT))
PR_PROXY_END(OS, IID_OS)

#endif // __PROXY_DECLARATION
