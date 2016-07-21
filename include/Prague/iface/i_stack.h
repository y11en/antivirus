// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  17 November 2004,  18:11 --------
// File Name   -- (null)i_stack.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_stack__37b1421f_ae7b_11d4_b757_00d0b7170e50 )
#define __i_stack__37b1421f_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end


#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// Stack interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_STACK  ((tIID)(64013))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
#define errSTACK_OVERFLOW                        PR_MAKE_DECL_ERR(IID_STACK, 0x001) // 0x8fa0d001,-1885286399 (1) --
#define errSTACK_UNDERFLOW                       PR_MAKE_DECL_ERR(IID_STACK, 0x002) // 0x8fa0d002,-1885286398 (2) --
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSTACK;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iStackVtbl;
typedef struct iStackVtbl iStackVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cStack;
	typedef cStack* hSTACK;
#else
	typedef struct tag_hSTACK {
		const iStackVtbl*  vtbl; // pointer to the "Stack" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hSTACK;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Stack_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpStack_Push)  ( hSTACK _this, tPTR data, tDWORD size );     // -- ;
	typedef   tERROR (pr_call *fnpStack_Pop)   ( hSTACK _this, tDWORD* result, tPTR buffer, tDWORD size );   // -- ;
	typedef   tERROR (pr_call *fnpStack_Get)   ( hSTACK _this, tDWORD* result, tDWORD index, tPTR buffer, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpStack_Set)   ( hSTACK _this, tDWORD index, tPTR buffer, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpStack_Size)  ( hSTACK _this, tDWORD* result );                             // -- ;
	typedef   tERROR (pr_call *fnpStack_Clear) ( hSTACK _this, tDWORD index );               // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iStackVtbl {
	fnpStack_Push   Push;
	fnpStack_Pop    Pop;
	fnpStack_Get    Get;
	fnpStack_Set    Set;
	fnpStack_Size   Size;
	fnpStack_Clear  Clear;
}; // "Stack" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Stack_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgSTACK_GRANULARITY mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgSTACK_NODE_SIZE   mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgSTACK_LIMIT       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Stack_Push( _this, data, size )                   ((_this)->vtbl->Push( (_this), data, size ))
	#define CALL_Stack_Pop( _this, result, buffer, size )          ((_this)->vtbl->Pop( (_this), result, buffer, size ))
	#define CALL_Stack_Get( _this, result, index, buffer, size )   ((_this)->vtbl->Get( (_this), result, index, buffer, size ))
	#define CALL_Stack_Set( _this, index, buffer, size )           ((_this)->vtbl->Set( (_this), index, buffer, size ))
	#define CALL_Stack_Size( _this, result )                       ((_this)->vtbl->Size( (_this), result ))
	#define CALL_Stack_Clear( _this, index )                       ((_this)->vtbl->Clear( (_this), index ))
#else // if !defined( __cplusplus )
	#define CALL_Stack_Push( _this, data, size )                   ((_this)->Push( data, size ))
	#define CALL_Stack_Pop( _this, result, buffer, size )          ((_this)->Pop( result, buffer, size ))
	#define CALL_Stack_Get( _this, result, index, buffer, size )   ((_this)->Get( result, index, buffer, size ))
	#define CALL_Stack_Set( _this, index, buffer, size )           ((_this)->Set( index, buffer, size ))
	#define CALL_Stack_Size( _this, result )                       ((_this)->Size( result ))
	#define CALL_Stack_Clear( _this, index )                       ((_this)->Clear( index ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iStack {
		virtual tERROR pr_call Push( tPTR data, tDWORD size ) = 0;
		virtual tERROR pr_call Pop( tDWORD* result, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call Get( tDWORD* result, tDWORD index, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call Set( tDWORD index, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call Size( tDWORD* result ) = 0;
		virtual tERROR pr_call Clear( tDWORD index ) = 0;
	};

	struct pr_abstract cStack : public iStack, public iObj {

		OBJ_IMPL( cStack );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSTACK()   { return (hSTACK)this; }

		tDWORD pr_call get_pgSTACK_GRANULARITY() { return (tDWORD)getDWord(pgSTACK_GRANULARITY); }
		tERROR pr_call set_pgSTACK_GRANULARITY( tDWORD value ) { return setDWord(pgSTACK_GRANULARITY,(tDWORD)value); }

		tDWORD pr_call get_pgSTACK_NODE_SIZE() { return (tDWORD)getDWord(pgSTACK_NODE_SIZE); }
		tERROR pr_call set_pgSTACK_NODE_SIZE( tDWORD value ) { return setDWord(pgSTACK_NODE_SIZE,(tDWORD)value); }

		tDWORD pr_call get_pgSTACK_LIMIT() { return (tDWORD)getDWord(pgSTACK_LIMIT); }
		tERROR pr_call set_pgSTACK_LIMIT( tDWORD value ) { return setDWord(pgSTACK_LIMIT,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_stack__37b1421f_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



