// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  22 October 2003,  16:57 --------
// File Name   -- (null)i_objptr.c
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_objptr__37b13ad6_ae7b_11d4_b757_00d0b7170e50 )
#define __i_objptr__37b13ad6_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- Object Enumerator (Pointer to Object)
//     Defines behavior of an object container/enumerator
//      Supplies object enumerate functionality on one level of hierarchy only.
//      For example: on logical drive object of this interface must enumerate objects in one folder.
//
//      Also must support creating new IO object in the container. There are two ways to do it. First is three steps protocol:
//         -- ObjectCreate system method with folder object as parent
//         -- dictate all necessary properties for the new object
//         -- ObjectCreateDone
//       Second is CreateNew folder's method which is wrapper of above protocol.
//       Advise: when caller invokes ObjectCreateDone method folder object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_OBJPTR  ((tIID)(11))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tObjPtrState; // status of the object pointer
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )

// ----------  availability flags  ----------
// flags show available features of implementation of the interface
#define fOPTR_AVAIL_MASK               ((tDWORD)(0x00000001L)) // implementation supports navigation methods with mask

// ----------  object ptr state constants  ----------
#define cObjPtrState_UNDEFINED         ((tObjPtrState)(0)) // pointer is undefined
#define cObjPtrState_BEFORE            ((tObjPtrState)(1)) // pointer in "BEFORE_FIRST" state
#define cObjPtrState_PTR               ((tObjPtrState)(2)) // pointer in "POINTER_TO_OBJ" state
#define cObjPtrState_AFTER             ((tObjPtrState)(3)) // pointer in "AFTER_LAST" state

// ----------  object attributes  ----------
#define fATTRIBUTE_READONLY            ((tDWORD)(0x00000001)) //  --
#define fATTRIBUTE_HIDDEN              ((tDWORD)(0x00000002)) //  --
#define fATTRIBUTE_SYSTEM              ((tDWORD)(0x00000004)) //  --
#define fATTRIBUTE_DIRECTORY           ((tDWORD)(0x00000010)) //  --
#define fATTRIBUTE_ARCHIVE             ((tDWORD)(0x00000020)) //  --
#define fATTRIBUTE_ENCRYPTED           ((tDWORD)(0x00000040)) //  --
#define fATTRIBUTE_NORMAL              ((tDWORD)(0x00000080)) //  --
#define fATTRIBUTE_TEMPORARY           ((tDWORD)(0x00000100)) //  --
#define fATTRIBUTE_SPARSE_FILE         ((tDWORD)(0x00000200)) //  --
#define fATTRIBUTE_REPARSE_POINT       ((tDWORD)(0x00000400)) //  --
#define fATTRIBUTE_COMPRESSED          ((tDWORD)(0x00000800)) //  --
#define fATTRIBUTE_OFFLINE             ((tDWORD)(0x00001000)) //  --
#define fATTRIBUTE_NOT_CONTENT_INDEXED ((tDWORD)(0x00002000)) //  --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hObjPtr;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iObjPtrVtbl;
typedef struct iObjPtrVtbl iObjPtrVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cObjPtr;
	typedef cObjPtr* hObjPtr;
#else
	typedef struct tag_hObjPtr 
	{
		const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hObjPtr;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( ObjPtr_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )



	// ----------- operations with the current object ----------
	typedef   tERROR (pr_call *fnpObjPtr_IOCreate) ( hObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ); // -- creates object current in enumeration;
	typedef   tERROR (pr_call *fnpObjPtr_Copy)     ( hObjPtr _this, hOBJECT dst_name, tBOOL overwrite ); // -- copies the object pointed by the ptr;
	typedef   tERROR (pr_call *fnpObjPtr_Rename)   ( hObjPtr _this, hOBJECT new_name, tBOOL overwrite ); // -- renames the object pointed by the ptr;
	typedef   tERROR (pr_call *fnpObjPtr_Delete)   ( hObjPtr _this );                         // -- deletes object pointed by ptr;

	// ----------- navigation methods ----------
	typedef   tERROR (pr_call *fnpObjPtr_Reset)    ( hObjPtr _this, tBOOL to_root );          // -- Reinitializing of ObjEnum object;
	typedef   tERROR (pr_call *fnpObjPtr_Clone)    ( hObjPtr _this, hObjPtr* result );                         // -- Makes another ObjPtr object;
	typedef   tERROR (pr_call *fnpObjPtr_Next)     ( hObjPtr _this );                         // -- Returns next IO object;
	typedef   tERROR (pr_call *fnpObjPtr_StepUp)   ( hObjPtr _this );                         // -- Goes one level up;
	typedef   tERROR (pr_call *fnpObjPtr_StepDown) ( hObjPtr _this );                         // -- Goes one level up;
	typedef   tERROR (pr_call *fnpObjPtr_ChangeTo) ( hObjPtr _this, hOBJECT name );           // -- Changes folder for enumerating;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iObjPtrVtbl 
{

// ----------- operations with the current object ----------
	fnpObjPtr_IOCreate  IOCreate;
	fnpObjPtr_Copy      Copy;
	fnpObjPtr_Rename    Rename;
	fnpObjPtr_Delete    Delete;

// ----------- navigation methods ----------
	fnpObjPtr_Reset     Reset;
	fnpObjPtr_Clone     Clone;
	fnpObjPtr_Next      Next;
	fnpObjPtr_StepUp    StepUp;
	fnpObjPtr_StepDown  StepDown;
	fnpObjPtr_ChangeTo  ChangeTo;
}; // "ObjPtr" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( ObjPtr_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION       mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_ORIGIN           mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
//! #define pgOBJECT_NAME             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_PATH             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000044 )
//! #define pgOBJECT_FULL_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
//! #define pgOBJECT_AVAILABILITY     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
//! #define pgOBJECT_BASED_ON         mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000053 )
#define pgIS_FOLDER               mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgMASK                    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001001 )
#define pgOBJECT_PTR_STATE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
//! #define pgOBJECT_SIZE             mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000045 )
//! #define pgOBJECT_SIZE_Q           mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00000045 )
//! #define pgOBJECT_ATTRIBUTES       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000054 )
//! #define pgOBJECT_HASH             mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00000055 )
//! #define pgOBJECT_REOPEN_DATA      mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00000056 )
#define pgOBJECT_CREATION_TIME    mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001005 )
#define pgOBJECT_LAST_WRITE_TIME  mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001006 )
#define pgOBJECT_LAST_ACCESS_TIME mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001007 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_ObjPtr_IOCreate( _this, result, name, access_mode, open_mode )    ((_this)->vtbl->IOCreate( (_this), result, name, access_mode, open_mode ))
	#define CALL_ObjPtr_Copy( _this, dst_name, overwrite )                         ((_this)->vtbl->Copy( (_this), dst_name, overwrite ))
	#define CALL_ObjPtr_Rename( _this, new_name, overwrite )                       ((_this)->vtbl->Rename( (_this), new_name, overwrite ))
	#define CALL_ObjPtr_Delete( _this )                                            ((_this)->vtbl->Delete( (_this) ))
	#define CALL_ObjPtr_Reset( _this, to_root )                                    ((_this)->vtbl->Reset( (_this), to_root ))
	#define CALL_ObjPtr_Clone( _this, result )                                     ((_this)->vtbl->Clone( (_this), result ))
	#define CALL_ObjPtr_Next( _this )                                              ((_this)->vtbl->Next( (_this) ))
	#define CALL_ObjPtr_StepUp( _this )                                            ((_this)->vtbl->StepUp( (_this) ))
	#define CALL_ObjPtr_StepDown( _this )                                          ((_this)->vtbl->StepDown( (_this) ))
	#define CALL_ObjPtr_ChangeTo( _this, name )                                    ((_this)->vtbl->ChangeTo( (_this), name ))
#else // if !defined( __cplusplus )
	#define CALL_ObjPtr_IOCreate( _this, result, name, access_mode, open_mode )    ((_this)->IOCreate( result, name, access_mode, open_mode ))
	#define CALL_ObjPtr_Copy( _this, dst_name, overwrite )                         ((_this)->Copy( dst_name, overwrite ))
	#define CALL_ObjPtr_Rename( _this, new_name, overwrite )                       ((_this)->Rename( new_name, overwrite ))
	#define CALL_ObjPtr_Delete( _this )                                            ((_this)->Delete( ))
	#define CALL_ObjPtr_Reset( _this, to_root )                                    ((_this)->Reset( to_root ))
	#define CALL_ObjPtr_Clone( _this, result )                                     ((_this)->Clone( result ))
	#define CALL_ObjPtr_Next( _this )                                              ((_this)->Next( ))
	#define CALL_ObjPtr_StepUp( _this )                                            ((_this)->StepUp( ))
	#define CALL_ObjPtr_StepDown( _this )                                          ((_this)->StepDown( ))
	#define CALL_ObjPtr_ChangeTo( _this, name )                                    ((_this)->ChangeTo( name ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iObjPtr 
	{
		virtual tERROR pr_call IOCreate( hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ) = 0; // -- creates object current in enumeration
		virtual tERROR pr_call Copy( hOBJECT dst_name, tBOOL overwrite ) = 0; // -- copies the object pointed by the ptr
		virtual tERROR pr_call Rename( hOBJECT new_name, tBOOL overwrite ) = 0; // -- renames the object pointed by the ptr
		virtual tERROR pr_call Delete() = 0; // -- deletes object pointed by ptr
		virtual tERROR pr_call Reset( tBOOL to_root ) = 0; // -- Reinitializing of ObjEnum object
		virtual tERROR pr_call Clone( hObjPtr* result ) = 0; // -- Makes another ObjPtr object
		virtual tERROR pr_call Next() = 0; // -- Returns next IO object
		virtual tERROR pr_call StepUp() = 0; // -- Goes one level up
		virtual tERROR pr_call StepDown() = 0; // -- Goes one level up
		virtual tERROR pr_call ChangeTo( hOBJECT name ) = 0; // -- Changes folder for enumerating
	};

	struct pr_abstract cObjPtr : public iObjPtr, public iObj {

		OBJ_IMPL( cObjPtr );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hObjPtr()   { return (hObjPtr)this; }

		tORIG_ID pr_call get_pgOBJECT_ORIGIN() { return (tORIG_ID)getDWord(pgOBJECT_ORIGIN); }
		tERROR pr_call set_pgOBJECT_ORIGIN( tORIG_ID value ) { return setDWord(pgOBJECT_ORIGIN,(tDWORD)value); }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_PATH,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_PATH,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_FULL_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_FULL_NAME,buff,size,cp); }

		tDWORD pr_call get_pgOBJECT_AVAILABILITY() { return (tDWORD)getDWord(pgOBJECT_AVAILABILITY); }
		tERROR pr_call set_pgOBJECT_AVAILABILITY( tDWORD value ) { return setDWord(pgOBJECT_AVAILABILITY,(tDWORD)value); }

		hOBJECT pr_call get_pgOBJECT_BASED_ON() { return (hOBJECT)getObj(pgOBJECT_BASED_ON); }
		tERROR pr_call set_pgOBJECT_BASED_ON( hOBJECT value ) { return setObj(pgOBJECT_BASED_ON,(hOBJECT)value); }

		tBOOL pr_call get_pgIS_FOLDER() { return (tBOOL)getBool(pgIS_FOLDER); }
		tERROR pr_call set_pgIS_FOLDER( tBOOL value ) { return setBool(pgIS_FOLDER,(tBOOL)value); }

		tERROR pr_call get_pgMASK( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgMASK,buff,size,cp); }
		tERROR pr_call set_pgMASK( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgMASK,buff,size,cp); }

		tObjPtrState pr_call get_pgOBJECT_PTR_STATE() { tObjPtrState value = {0}; get(pgOBJECT_PTR_STATE,&value,sizeof(value)); return value; }
		tERROR pr_call set_pgOBJECT_PTR_STATE( tObjPtrState value ) { return set(pgOBJECT_PTR_STATE,&value,sizeof(value)); }

		tDWORD pr_call get_pgOBJECT_SIZE() { return (tDWORD)getDWord(pgOBJECT_SIZE); }
		tERROR pr_call set_pgOBJECT_SIZE( tDWORD value ) { return setDWord(pgOBJECT_SIZE,(tDWORD)value); }

		tQWORD pr_call get_pgOBJECT_SIZE_Q() { return (tQWORD)getQWord(pgOBJECT_SIZE_Q); }
		tERROR pr_call set_pgOBJECT_SIZE_Q( tQWORD value ) { return setQWord(pgOBJECT_SIZE_Q,(tQWORD)value); }

		tDWORD pr_call get_pgOBJECT_ATTRIBUTES() { return (tDWORD)getDWord(pgOBJECT_ATTRIBUTES); }
		tERROR pr_call set_pgOBJECT_ATTRIBUTES( tDWORD value ) { return setDWord(pgOBJECT_ATTRIBUTES,(tDWORD)value); }

		tQWORD pr_call get_pgOBJECT_HASH() { return (tQWORD)getQWord(pgOBJECT_HASH); }
		tERROR pr_call set_pgOBJECT_HASH( tQWORD value ) { return setQWord(pgOBJECT_HASH,(tQWORD)value); }

		tERROR pr_call get_pgOBJECT_REOPEN_DATA( tPTR value, tDWORD size ) { return get(pgOBJECT_REOPEN_DATA,value,size); }
		tERROR pr_call set_pgOBJECT_REOPEN_DATA( tPTR value, tDWORD size ) { return set(pgOBJECT_REOPEN_DATA,value,size); }

		tERROR pr_call get_pgOBJECT_CREATION_TIME( tDATETIME* value ) { return get(pgOBJECT_CREATION_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgOBJECT_CREATION_TIME( tDATETIME* value ) { return set(pgOBJECT_CREATION_TIME,value,sizeof(tDATETIME)); }

		tERROR pr_call get_pgOBJECT_LAST_WRITE_TIME( tDATETIME* value ) { return get(pgOBJECT_LAST_WRITE_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgOBJECT_LAST_WRITE_TIME( tDATETIME* value ) { return set(pgOBJECT_LAST_WRITE_TIME,value,sizeof(tDATETIME)); }

		tERROR pr_call get_pgOBJECT_LAST_ACCESS_TIME( tDATETIME* value ) { return get(pgOBJECT_LAST_ACCESS_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgOBJECT_LAST_ACCESS_TIME( tDATETIME* value ) { return set(pgOBJECT_LAST_ACCESS_TIME,value,sizeof(tDATETIME)); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_objptr__37b13ad6_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(ObjPtr_PROXY_DEFINITION)
#define ObjPtr_PROXY_DEFINITION

PR_PROXY_BEGIN(ObjPtr)
	PR_PROXY(ObjPtr, IOCreate,  PR_ARG4(tid_OBJECT|tid_POINTER,tid_OBJECT,tid_DWORD,tid_DWORD))
	PR_PROXY(ObjPtr, Copy,      PR_ARG2(tid_OBJECT,tid_BOOL))
	PR_PROXY(ObjPtr, Rename,    PR_ARG2(tid_OBJECT,tid_BOOL))
	PR_PROXY(ObjPtr, Delete,    PR_ARG0())
	PR_PROXY(ObjPtr, Reset,     PR_ARG1(tid_BOOL))
	PR_PROXY(ObjPtr, Clone,     PR_ARG1(tid_OBJECT|tid_POINTER))
	PR_PROXY(ObjPtr, Next,      PR_ARG0())
	PR_PROXY(ObjPtr, StepUp,    PR_ARG0())
	PR_PROXY(ObjPtr, StepDown,  PR_ARG0())
	PR_PROXY(ObjPtr, ChangeTo,  PR_ARG1(tid_OBJECT))
PR_PROXY_END(ObjPtr, IID_OBJPTR)

#endif // __PROXY_DECLARATION
