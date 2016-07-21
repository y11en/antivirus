// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  07 July 2006,  17:23 --------
// File Name   -- (null)i_ifenum.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_ifenum__37b137e1_ae7b_11d4_b757_00d0b7170e50 )
#define __i_ifenum__37b137e1_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_iface.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IFaceEnum interface implementation
// Short comments -- registered interface enumerator interface
//    Supplies facility to enumerate registered interfaces
//    Also possible to get shared properties of registered iterface
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_IFACE_ENUM  ((tIID)(3))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
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
	typedef tUINT hIFACE_ENUM;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iIFaceEnumVtbl;
typedef struct iIFaceEnumVtbl iIFaceEnumVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cIFaceEnum;
	typedef cIFaceEnum* hIFACE_ENUM;
#else
	typedef struct tag_hIFACE_ENUM {
		const iIFaceEnumVtbl* vtbl; // pointer to the "IFaceEnum" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hIFACE_ENUM;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( IFaceEnum_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpIFaceEnum_Init)         ( hIFACE_ENUM _this, tIID iid, tPID pid, tVID vid, hOBJECT object ); // -- Initialization procedure;
	typedef   tERROR (pr_call *fnpIFaceEnum_Clone)        ( hIFACE_ENUM _this, hIFACE_ENUM* result, hOBJECT dad );     // -- Clone current enumerator;
	typedef   tERROR (pr_call *fnpIFaceEnum_Reset)        ( hIFACE_ENUM _this );                  // -- Resets current position of enumerator;
	typedef   tERROR (pr_call *fnpIFaceEnum_Next)         ( hIFACE_ENUM _this );                  // -- Advance current pointer;
	typedef   tERROR (pr_call *fnpIFaceEnum_GetIFaceProp) ( hIFACE_ENUM _this, tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD size ); // -- Get static properties of current interface;
	typedef   tERROR (pr_call *fnpIFaceEnum_ObjectCreate) ( hIFACE_ENUM _this, hOBJECT* result, hOBJECT parent );  // -- Create instance of current interface;
	typedef   tERROR (pr_call *fnpIFaceEnum_Recognize)    ( hIFACE_ENUM _this, hOBJECT object, tDWORD type ); // -- regonize the "object";
	typedef   tERROR (pr_call *fnpIFaceEnum_Load)         ( hIFACE_ENUM _this );                  // -- load the interface and increment objects count;
	typedef   tERROR (pr_call *fnpIFaceEnum_Free)         ( hIFACE_ENUM _this );                  // -- decrements objects count and unload the interface if possible;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iIFaceEnumVtbl {
	fnpIFaceEnum_Init          Init;
	fnpIFaceEnum_Clone         Clone;
	fnpIFaceEnum_Reset         Reset;
	fnpIFaceEnum_Next          Next;
	fnpIFaceEnum_GetIFaceProp  GetIFaceProp;
	fnpIFaceEnum_ObjectCreate  ObjectCreate;
	fnpIFaceEnum_Recognize     Recognize;
	fnpIFaceEnum_Load          Load;
	fnpIFaceEnum_Free          Free;
}; // "IFaceEnum" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( IFaceEnum_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION  mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT  mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgPRESCRIBED_IID     mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00001000 )
#define pgPRESCRIBED_PID     mPROPERTY_MAKE_GLOBAL( pTYPE_PID     , 0x00001001 )
#define pgPRESCRIBED_VID     mPROPERTY_MAKE_GLOBAL( pTYPE_VID     , 0x00001002 )
#define pgPRESCRIBED_OBJECT  mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001003 )
#define pgPRESCRIBED_TYPE    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001009 )
//! #define pgOBJECT_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgPLUGIN             mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001004 )
#define pgINDEX              mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgIFACE_FLAGS        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001006 )
#define pgIFACE_LOAD_ERR     mPROPERTY_MAKE_GLOBAL( pTYPE_ERROR   , 0x00001007 )
#define pgINCLUDE_INTERFACES mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x0000100a )
#define pgEXCLUDE_INTERFACES mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x0000100b )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_IFaceEnum_Init( _this, iid, pid, vid, object )                       ((_this)->vtbl->Init( (_this), iid, pid, vid, object ))
	#define CALL_IFaceEnum_Clone( _this, result, dad )                                ((_this)->vtbl->Clone( (_this), result, dad ))
	#define CALL_IFaceEnum_Reset( _this )                                             ((_this)->vtbl->Reset( (_this) ))
	#define CALL_IFaceEnum_Next( _this )                                              ((_this)->vtbl->Next( (_this) ))
	#define CALL_IFaceEnum_GetIFaceProp( _this, result, prop_id, buffer, size )       ((_this)->vtbl->GetIFaceProp( (_this), result, prop_id, buffer, size ))
	#define CALL_IFaceEnum_ObjectCreate( _this, result, parent )                      ((_this)->vtbl->ObjectCreate( (_this), result, parent ))
	#define CALL_IFaceEnum_Recognize( _this, object, type )                           ((_this)->vtbl->Recognize( (_this), object, type ))
	#define CALL_IFaceEnum_Load( _this )                                              ((_this)->vtbl->Load( (_this) ))
	#define CALL_IFaceEnum_Free( _this )                                              ((_this)->vtbl->Free( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_IFaceEnum_Init( _this, iid, pid, vid, object )                       ((_this)->Init( iid, pid, vid, object ))
	#define CALL_IFaceEnum_Clone( _this, result, dad )                                ((_this)->Clone( result, dad ))
	#define CALL_IFaceEnum_Reset( _this )                                             ((_this)->Reset( ))
	#define CALL_IFaceEnum_Next( _this )                                              ((_this)->Next( ))
	#define CALL_IFaceEnum_GetIFaceProp( _this, result, prop_id, buffer, size )       ((_this)->GetIFaceProp( result, prop_id, buffer, size ))
	#define CALL_IFaceEnum_ObjectCreate( _this, result, parent )                      ((_this)->ObjectCreate( result, parent ))
	#define CALL_IFaceEnum_Recognize( _this, object, type )                           ((_this)->Recognize( object, type ))
	#define CALL_IFaceEnum_Load( _this )                                              ((_this)->Load( ))
	#define CALL_IFaceEnum_Free( _this )                                              ((_this)->Free( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iIFaceEnum {
		virtual tERROR pr_call Init( tIID iid, tPID pid, tVID vid, hOBJECT object ) = 0; // -- Initialization procedure
		virtual tERROR pr_call Clone( hIFACE_ENUM* result, hOBJECT dad ) = 0; // -- Clone current enumerator
		virtual tERROR pr_call Reset() = 0; // -- Resets current position of enumerator
		virtual tERROR pr_call Next() = 0; // -- Advance current pointer
		virtual tERROR pr_call GetIFaceProp( tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD size ) = 0; // -- Get static properties of current interface
		virtual tERROR pr_call ObjectCreate( hOBJECT* result, hOBJECT parent ) = 0; // -- Create instance of current interface
		virtual tERROR pr_call Recognize( hOBJECT object, tDWORD type ) = 0; // -- regonize the "object"
		virtual tERROR pr_call Load() = 0; // -- load the interface and increment objects count
		virtual tERROR pr_call Free() = 0; // -- decrements objects count and unload the interface if possible
	};

	struct pr_abstract cIFaceEnum : public iIFaceEnum, public iObj {

		OBJ_IMPL( cIFaceEnum );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hIFACE_ENUM()   { return (hIFACE_ENUM)this; }

		tIID pr_call get_pgPRESCRIBED_IID() { return (tIID)getDWord(pgPRESCRIBED_IID); }
		tERROR pr_call set_pgPRESCRIBED_IID( tIID value ) { return setDWord(pgPRESCRIBED_IID,(tDWORD)value); }

		tPID pr_call get_pgPRESCRIBED_PID() { return (tPID)getDWord(pgPRESCRIBED_PID); }
		tERROR pr_call set_pgPRESCRIBED_PID( tPID value ) { return setDWord(pgPRESCRIBED_PID,(tDWORD)value); }

		tVID pr_call get_pgPRESCRIBED_VID() { return (tVID)getDWord(pgPRESCRIBED_VID); }
		tERROR pr_call set_pgPRESCRIBED_VID( tVID value ) { return setDWord(pgPRESCRIBED_VID,(tDWORD)value); }

		hOBJECT pr_call get_pgPRESCRIBED_OBJECT() { return (hOBJECT)getObj(pgPRESCRIBED_OBJECT); }
		tERROR pr_call set_pgPRESCRIBED_OBJECT( hOBJECT value ) { return setObj(pgPRESCRIBED_OBJECT,(hOBJECT)value); }

		tDWORD pr_call get_pgPRESCRIBED_TYPE() { return (tDWORD)getDWord(pgPRESCRIBED_TYPE); }
		tERROR pr_call set_pgPRESCRIBED_TYPE( tDWORD value ) { return setDWord(pgPRESCRIBED_TYPE,(tDWORD)value); }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		hOBJECT pr_call get_pgPLUGIN() { return (hOBJECT)getObj(pgPLUGIN); }
		tERROR pr_call set_pgPLUGIN( hOBJECT value ) { return setObj(pgPLUGIN,(hOBJECT)value); }

		tDWORD pr_call get_pgINDEX() { return (tDWORD)getDWord(pgINDEX); }
		tERROR pr_call set_pgINDEX( tDWORD value ) { return setDWord(pgINDEX,(tDWORD)value); }

		tDWORD pr_call get_pgIFACE_FLAGS() { return (tDWORD)getDWord(pgIFACE_FLAGS); }
		tERROR pr_call set_pgIFACE_FLAGS( tDWORD value ) { return setDWord(pgIFACE_FLAGS,(tDWORD)value); }

		tERROR pr_call get_pgIFACE_LOAD_ERR() { return (tERROR)getDWord(pgIFACE_LOAD_ERR); }
		tERROR pr_call set_pgIFACE_LOAD_ERR( tERROR value ) { return setDWord(pgIFACE_LOAD_ERR,(tDWORD)value); }

		tERROR pr_call get_pgINCLUDE_INTERFACES( tPTR value, tDWORD size ) { return get(pgINCLUDE_INTERFACES,value,size); }
		tERROR pr_call set_pgINCLUDE_INTERFACES( tPTR value, tDWORD size ) { return set(pgINCLUDE_INTERFACES,value,size); }

		tERROR pr_call get_pgEXCLUDE_INTERFACES( tPTR value, tDWORD size ) { return get(pgEXCLUDE_INTERFACES,value,size); }
		tERROR pr_call set_pgEXCLUDE_INTERFACES( tPTR value, tDWORD size ) { return set(pgEXCLUDE_INTERFACES,value,size); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_ifenum__37b137e1_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



