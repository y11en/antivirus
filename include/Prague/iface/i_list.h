// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_list.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_list__e1a85068_c120_4517_a863_da19fd7a1e80 )
#define __i_list__e1a85068_c120_4517_a863_da19fd7a1e80
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// List interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_LIST  ((tIID)(64012))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDATA                         tLISTNODE; //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define cLIST_NULL                     ((tLISTNODE)(0)) //  --
#define cLIST_AFTER                    ((tDWORD)(0)) //  --
#define cLIST_BEFORE                   ((tDWORD)(1)) //  --
#define cLIST_FIRST                    ((tDWORD)(2)) //  --
#define cLIST_LAST                     ((tDWORD)(3)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hLIST;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iListVtbl;
typedef struct iListVtbl iListVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cList;
	typedef cList* hLIST;
#else
	typedef struct tag_hLIST {
		const iListVtbl*   vtbl; // pointer to the "List" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hLIST;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( List_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpList_Add)     ( hLIST _this, tLISTNODE* result, tPTR data, tDWORD count, tLISTNODE where, tDWORD mode ); // -- ;
	typedef   tERROR (pr_call *fnpList_Remove)  ( hLIST _this, tLISTNODE node );            // -- ;
	typedef   tERROR (pr_call *fnpList_Move)    ( hLIST _this, tLISTNODE node, tLISTNODE where, tDWORD mode ); // -- ;
	typedef   tERROR (pr_call *fnpList_First)   ( hLIST _this, tLISTNODE* result );                            // -- ;
	typedef   tERROR (pr_call *fnpList_Last)    ( hLIST _this, tLISTNODE* result );                            // -- ;
	typedef   tERROR (pr_call *fnpList_Next)    ( hLIST _this, tLISTNODE* result, tLISTNODE node );            // -- ;
	typedef   tERROR (pr_call *fnpList_Prev)    ( hLIST _this, tLISTNODE* result, tLISTNODE node );            // -- ;
	typedef   tERROR (pr_call *fnpList_DataGet) ( hLIST _this, tDWORD* result, tLISTNODE node, tPTR buffer, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpList_DataSet) ( hLIST _this, tLISTNODE* result, tLISTNODE node, tPTR buffer, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpList_Clear)   ( hLIST _this );                            // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iListVtbl {
	fnpList_Add      Add;
	fnpList_Remove   Remove;
	fnpList_Move     Move;
	fnpList_First    First;
	fnpList_Last     Last;
	fnpList_Next     Next;
	fnpList_Prev     Prev;
	fnpList_DataGet  DataGet;
	fnpList_DataSet  DataSet;
	fnpList_Clear    Clear;
}; // "List" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( List_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgLIST_GRANULARITY  mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgLIST_NODE_SIZE    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_List_Add( _this, result, data, count, where, mode )  ((_this)->vtbl->Add( (_this), result, data, count, where, mode ))
	#define CALL_List_Remove( _this, node )                           ((_this)->vtbl->Remove( (_this), node ))
	#define CALL_List_Move( _this, node, where, mode )                ((_this)->vtbl->Move( (_this), node, where, mode ))
	#define CALL_List_First( _this, result )                          ((_this)->vtbl->First( (_this), result ))
	#define CALL_List_Last( _this, result )                           ((_this)->vtbl->Last( (_this), result ))
	#define CALL_List_Next( _this, result, node )                     ((_this)->vtbl->Next( (_this), result, node ))
	#define CALL_List_Prev( _this, result, node )                     ((_this)->vtbl->Prev( (_this), result, node ))
	#define CALL_List_DataGet( _this, result, node, buffer, size )    ((_this)->vtbl->DataGet( (_this), result, node, buffer, size ))
	#define CALL_List_DataSet( _this, result, node, buffer, size )    ((_this)->vtbl->DataSet( (_this), result, node, buffer, size ))
	#define CALL_List_Clear( _this )                                  ((_this)->vtbl->Clear( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_List_Add( _this, result, data, count, where, mode )  ((_this)->Add( result, data, count, where, mode ))
	#define CALL_List_Remove( _this, node )                           ((_this)->Remove( node ))
	#define CALL_List_Move( _this, node, where, mode )                ((_this)->Move( node, where, mode ))
	#define CALL_List_First( _this, result )                          ((_this)->First( result ))
	#define CALL_List_Last( _this, result )                           ((_this)->Last( result ))
	#define CALL_List_Next( _this, result, node )                     ((_this)->Next( result, node ))
	#define CALL_List_Prev( _this, result, node )                     ((_this)->Prev( result, node ))
	#define CALL_List_DataGet( _this, result, node, buffer, size )    ((_this)->DataGet( result, node, buffer, size ))
	#define CALL_List_DataSet( _this, result, node, buffer, size )    ((_this)->DataSet( result, node, buffer, size ))
	#define CALL_List_Clear( _this )                                  ((_this)->Clear( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iList {
		virtual tERROR pr_call Add( tLISTNODE* result, tPTR data, tDWORD count, tLISTNODE where, tDWORD mode ) = 0;
		virtual tERROR pr_call Remove( tLISTNODE node ) = 0;
		virtual tERROR pr_call Move( tLISTNODE node, tLISTNODE where, tDWORD mode ) = 0;
		virtual tERROR pr_call First( tLISTNODE* result ) = 0;
		virtual tERROR pr_call Last( tLISTNODE* result ) = 0;
		virtual tERROR pr_call Next( tLISTNODE* result, tLISTNODE node ) = 0;
		virtual tERROR pr_call Prev( tLISTNODE* result, tLISTNODE node ) = 0;
		virtual tERROR pr_call DataGet( tDWORD* result, tLISTNODE node, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call DataSet( tLISTNODE* result, tLISTNODE node, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call Clear() = 0;
	};

	struct pr_abstract cList : public iList, public iObj {

		OBJ_IMPL( cList );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hLIST()   { return (hLIST)this; }

		tDWORD pr_call get_pgLIST_GRANULARITY() { return (tDWORD)getDWord(pgLIST_GRANULARITY); }
		tERROR pr_call set_pgLIST_GRANULARITY( tDWORD value ) { return setDWord(pgLIST_GRANULARITY,(tDWORD)value); }

		tDWORD pr_call get_pgLIST_NODE_SIZE() { return (tDWORD)getDWord(pgLIST_NODE_SIZE); }
		tERROR pr_call set_pgLIST_NODE_SIZE( tDWORD value ) { return setDWord(pgLIST_NODE_SIZE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_list__37b141fa_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end

