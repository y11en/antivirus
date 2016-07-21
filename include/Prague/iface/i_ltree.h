// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_ltree.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_ltree__00df5ccb_77df_480b_b2b7_8536db94133c )
#define __i_ltree__00df5ccb_77df_480b_b2b7_8536db94133c
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// LTree interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_LTREE  ((tIID)(64010))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDATA                         tLTREENODE; //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define cLTREE_NULL                    ((tLTREENODE)(0)) //  --
#define cLTREE_AFTER                   ((tDWORD)(0)) //  --
#define cLTREE_BEFORE                  ((tDWORD)(1)) //  --
#define cLTREE_FIRST                   ((tDWORD)(2)) //  --
#define cLTREE_LAST                    ((tDWORD)(3)) //  --
#define cLTREE_ORDER                   ((tDWORD)(4)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hLTREE;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iLTreeVtbl;
typedef struct iLTreeVtbl iLTreeVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cLTree;
	typedef cLTree* hLTREE;
#else
	typedef struct tag_hLTREE {
		const iLTreeVtbl*  vtbl; // pointer to the "LTree" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hLTREE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( LTree_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpLTree_Add)       ( hLTREE _this, tLTREENODE* result, tPTR data, tDWORD size, tLTREENODE where, tDWORD mode ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_Remove)    ( hLTREE _this, tLTREENODE node );        // -- ;
	typedef   tERROR (pr_call *fnpLTree_Search)    ( hLTREE _this, tLTREENODE* result, tPTR data, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_AtLeast)   ( hLTREE _this, tLTREENODE* result, tPTR data, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_AtMost)    ( hLTREE _this, tLTREENODE* result, tPTR data, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_TreeFirst) ( hLTREE _this, tLTREENODE* result );                         // -- ;
	typedef   tERROR (pr_call *fnpLTree_TreeLast)  ( hLTREE _this, tLTREENODE* result );                         // -- ;
	typedef   tERROR (pr_call *fnpLTree_TreeNext)  ( hLTREE _this, tLTREENODE* result, tLTREENODE node );        // -- ;
	typedef   tERROR (pr_call *fnpLTree_TreePrev)  ( hLTREE _this, tLTREENODE* result, tLTREENODE node );        // -- ;
	typedef   tERROR (pr_call *fnpLTree_ListFirst) ( hLTREE _this, tLTREENODE* result );                         // -- ;
	typedef   tERROR (pr_call *fnpLTree_ListLast)  ( hLTREE _this, tLTREENODE* result );                         // -- ;
	typedef   tERROR (pr_call *fnpLTree_ListNext)  ( hLTREE _this, tLTREENODE* result, tLTREENODE node );        // -- ;
	typedef   tERROR (pr_call *fnpLTree_ListPrev)  ( hLTREE _this, tLTREENODE* result, tLTREENODE node );        // -- ;
	typedef   tERROR (pr_call *fnpLTree_ListMove)  ( hLTREE _this, tLTREENODE node, tLTREENODE where, tDWORD mode ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_DataGet)   ( hLTREE _this, tDWORD* result, tLTREENODE node, tPTR buffer, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_DataSet)   ( hLTREE _this, tLTREENODE* result, tLTREENODE node, tPTR buffer, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpLTree_Clear)     ( hLTREE _this );                         // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iLTreeVtbl {
	fnpLTree_Add        Add;
	fnpLTree_Remove     Remove;
	fnpLTree_Search     Search;
	fnpLTree_AtLeast    AtLeast;
	fnpLTree_AtMost     AtMost;
	fnpLTree_TreeFirst  TreeFirst;
	fnpLTree_TreeLast   TreeLast;
	fnpLTree_TreeNext   TreeNext;
	fnpLTree_TreePrev   TreePrev;
	fnpLTree_ListFirst  ListFirst;
	fnpLTree_ListLast   ListLast;
	fnpLTree_ListNext   ListNext;
	fnpLTree_ListPrev   ListPrev;
	fnpLTree_ListMove   ListMove;
	fnpLTree_DataGet    DataGet;
	fnpLTree_DataSet    DataSet;
	fnpLTree_Clear      Clear;
}; // "LTree" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( LTree_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION    mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgLTREE_DUP_ALLOWED    mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgLTREE_GRANULARITY    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgLTREE_NODE_SIZE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgLTREE_COMPARE_OBJECT mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001004 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_LTree_Add( _this, result, data, size, where, mode )   ((_this)->vtbl->Add( (_this), result, data, size, where, mode ))
	#define CALL_LTree_Remove( _this, node )                           ((_this)->vtbl->Remove( (_this), node ))
	#define CALL_LTree_Search( _this, result, data, size )             ((_this)->vtbl->Search( (_this), result, data, size ))
	#define CALL_LTree_AtLeast( _this, result, data, size )            ((_this)->vtbl->AtLeast( (_this), result, data, size ))
	#define CALL_LTree_AtMost( _this, result, data, size )             ((_this)->vtbl->AtMost( (_this), result, data, size ))
	#define CALL_LTree_TreeFirst( _this, result )                      ((_this)->vtbl->TreeFirst( (_this), result ))
	#define CALL_LTree_TreeLast( _this, result )                       ((_this)->vtbl->TreeLast( (_this), result ))
	#define CALL_LTree_TreeNext( _this, result, node )                 ((_this)->vtbl->TreeNext( (_this), result, node ))
	#define CALL_LTree_TreePrev( _this, result, node )                 ((_this)->vtbl->TreePrev( (_this), result, node ))
	#define CALL_LTree_ListFirst( _this, result )                      ((_this)->vtbl->ListFirst( (_this), result ))
	#define CALL_LTree_ListLast( _this, result )                       ((_this)->vtbl->ListLast( (_this), result ))
	#define CALL_LTree_ListNext( _this, result, node )                 ((_this)->vtbl->ListNext( (_this), result, node ))
	#define CALL_LTree_ListPrev( _this, result, node )                 ((_this)->vtbl->ListPrev( (_this), result, node ))
	#define CALL_LTree_ListMove( _this, node, where, mode )            ((_this)->vtbl->ListMove( (_this), node, where, mode ))
	#define CALL_LTree_DataGet( _this, result, node, buffer, size )    ((_this)->vtbl->DataGet( (_this), result, node, buffer, size ))
	#define CALL_LTree_DataSet( _this, result, node, buffer, size )    ((_this)->vtbl->DataSet( (_this), result, node, buffer, size ))
	#define CALL_LTree_Clear( _this )                                  ((_this)->vtbl->Clear( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_LTree_Add( _this, result, data, size, where, mode )   ((_this)->Add( result, data, size, where, mode ))
	#define CALL_LTree_Remove( _this, node )                           ((_this)->Remove( node ))
	#define CALL_LTree_Search( _this, result, data, size )             ((_this)->Search( result, data, size ))
	#define CALL_LTree_AtLeast( _this, result, data, size )            ((_this)->AtLeast( result, data, size ))
	#define CALL_LTree_AtMost( _this, result, data, size )             ((_this)->AtMost( result, data, size ))
	#define CALL_LTree_TreeFirst( _this, result )                      ((_this)->TreeFirst( result ))
	#define CALL_LTree_TreeLast( _this, result )                       ((_this)->TreeLast( result ))
	#define CALL_LTree_TreeNext( _this, result, node )                 ((_this)->TreeNext( result, node ))
	#define CALL_LTree_TreePrev( _this, result, node )                 ((_this)->TreePrev( result, node ))
	#define CALL_LTree_ListFirst( _this, result )                      ((_this)->ListFirst( result ))
	#define CALL_LTree_ListLast( _this, result )                       ((_this)->ListLast( result ))
	#define CALL_LTree_ListNext( _this, result, node )                 ((_this)->ListNext( result, node ))
	#define CALL_LTree_ListPrev( _this, result, node )                 ((_this)->ListPrev( result, node ))
	#define CALL_LTree_ListMove( _this, node, where, mode )            ((_this)->ListMove( node, where, mode ))
	#define CALL_LTree_DataGet( _this, result, node, buffer, size )    ((_this)->DataGet( result, node, buffer, size ))
	#define CALL_LTree_DataSet( _this, result, node, buffer, size )    ((_this)->DataSet( result, node, buffer, size ))
	#define CALL_LTree_Clear( _this )                                  ((_this)->Clear( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iLTree {
		virtual tERROR pr_call Add( tLTREENODE* result, tPTR data, tDWORD size, tLTREENODE where, tDWORD mode ) = 0;
		virtual tERROR pr_call Remove( tLTREENODE node ) = 0;
		virtual tERROR pr_call Search( tLTREENODE* result, tPTR data, tDWORD size ) = 0;
		virtual tERROR pr_call AtLeast( tLTREENODE* result, tPTR data, tDWORD size ) = 0;
		virtual tERROR pr_call AtMost( tLTREENODE* result, tPTR data, tDWORD size ) = 0;
		virtual tERROR pr_call TreeFirst( tLTREENODE* result ) = 0;
		virtual tERROR pr_call TreeLast( tLTREENODE* result ) = 0;
		virtual tERROR pr_call TreeNext( tLTREENODE* result, tLTREENODE node ) = 0;
		virtual tERROR pr_call TreePrev( tLTREENODE* result, tLTREENODE node ) = 0;
		virtual tERROR pr_call ListFirst( tLTREENODE* result ) = 0;
		virtual tERROR pr_call ListLast( tLTREENODE* result ) = 0;
		virtual tERROR pr_call ListNext( tLTREENODE* result, tLTREENODE node ) = 0;
		virtual tERROR pr_call ListPrev( tLTREENODE* result, tLTREENODE node ) = 0;
		virtual tERROR pr_call ListMove( tLTREENODE node, tLTREENODE where, tDWORD mode ) = 0;
		virtual tERROR pr_call DataGet( tDWORD* result, tLTREENODE node, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call DataSet( tLTREENODE* result, tLTREENODE node, tPTR buffer, tDWORD size ) = 0;
		virtual tERROR pr_call Clear() = 0;
	};

	struct pr_abstract cLTree : public iLTree, public iObj {

		OBJ_IMPL( cLTree );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hLTREE()   { return (hLTREE)this; }

		tBOOL pr_call get_pgLTREE_DUP_ALLOWED() { return (tBOOL)getBool(pgLTREE_DUP_ALLOWED); }
		tERROR pr_call set_pgLTREE_DUP_ALLOWED( tBOOL value ) { return setBool(pgLTREE_DUP_ALLOWED,(tBOOL)value); }

		tDWORD pr_call get_pgLTREE_GRANULARITY() { return (tDWORD)getDWord(pgLTREE_GRANULARITY); }
		tERROR pr_call set_pgLTREE_GRANULARITY( tDWORD value ) { return setDWord(pgLTREE_GRANULARITY,(tDWORD)value); }

		tDWORD pr_call get_pgLTREE_NODE_SIZE() { return (tDWORD)getDWord(pgLTREE_NODE_SIZE); }
		tERROR pr_call set_pgLTREE_NODE_SIZE( tDWORD value ) { return setDWord(pgLTREE_NODE_SIZE,(tDWORD)value); }

		hOBJECT pr_call get_pgLTREE_COMPARE_OBJECT() { return (hOBJECT)getObj(pgLTREE_COMPARE_OBJECT); }
		tERROR pr_call set_pgLTREE_COMPARE_OBJECT( hOBJECT value ) { return setObj(pgLTREE_COMPARE_OBJECT,(hOBJECT)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_ltree__37b141ae_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end

