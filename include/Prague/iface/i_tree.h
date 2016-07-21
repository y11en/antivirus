// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_tree.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_tree__d80a9578_8f6e_4682_b6ec_20152cf26ad9 )
#define __i_tree__d80a9578_8f6e_4682_b6ec_20152cf26ad9
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// Tree interface implementation
// Short comments -- tree sorting and searching interface
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TREE  ((tIID)(64002))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDATA                         tTREENODE; //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define cTREE_NULL                     ((tTREENODE)(0)) //  --
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTREE;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iTreeVtbl;
typedef struct iTreeVtbl iTreeVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTree;
	typedef cTree* hTREE;
#else
	typedef struct tag_hTREE {
		const iTreeVtbl*   vtbl; // pointer to the "Tree" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hTREE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Tree_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpTree_Add)         ( hTREE _this, tTREENODE* result, const tPTR element, tDWORD size ); // -- Add new element;
	typedef   tERROR (pr_call *fnpTree_Remove)      ( hTREE _this, tTREENODE node );        // -- Remove node from tree;
	typedef   tERROR (pr_call *fnpTree_Search)      ( hTREE _this, tTREENODE* result, const tPTR element, tDWORD size ); // -- Search node equil to element;
	typedef   tERROR (pr_call *fnpTree_AtLeast)     ( hTREE _this, tTREENODE* result, const tPTR element, tDWORD size ); // -- Search node equil or next greater to given element;
	typedef   tERROR (pr_call *fnpTree_AtMost)      ( hTREE _this, tTREENODE* result, const tPTR element, tDWORD size ); // -- Search node equil or less to element;
	typedef   tERROR (pr_call *fnpTree_First)       ( hTREE _this, tTREENODE* result );                        // -- find first node in tree;
	typedef   tERROR (pr_call *fnpTree_Last)        ( hTREE _this, tTREENODE* result );                        // -- find last node in tree;
	typedef   tERROR (pr_call *fnpTree_Next)        ( hTREE _this, tTREENODE* result, tTREENODE node );        // -- return next node in tree exluding dups;
	typedef   tERROR (pr_call *fnpTree_Prev)        ( hTREE _this, tTREENODE* result, tTREENODE node );        // -- return previous node in tree excluding dups;
	typedef   tERROR (pr_call *fnpTree_NextDup)     ( hTREE _this, tTREENODE* result, tTREENODE node );        // -- return next node equil to given one in tree;
	typedef   tERROR (pr_call *fnpTree_PrevDup)     ( hTREE _this, tTREENODE* result, tTREENODE node );        // -- return prev node equil to given one in tree;
	typedef   tERROR (pr_call *fnpTree_NodeDataGet) ( hTREE _this, tDWORD* result, tTREENODE node, tPTR buffer, tDWORD size ); // -- Retrieve node data ( previously stored element);
	typedef   tERROR (pr_call *fnpTree_NodeDataSet) ( hTREE _this, tTREENODE* result, tTREENODE node, tPTR buffer, tDWORD size ); // -- Set node data;
	typedef   tERROR (pr_call *fnpTree_Clear)       ( hTREE _this );                        // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iTreeVtbl {
	fnpTree_Add          Add;
	fnpTree_Remove       Remove;
	fnpTree_Search       Search;
	fnpTree_AtLeast      AtLeast;
	fnpTree_AtMost       AtMost;
	fnpTree_First        First;
	fnpTree_Last         Last;
	fnpTree_Next         Next;
	fnpTree_Prev         Prev;
	fnpTree_NextDup      NextDup;
	fnpTree_PrevDup      PrevDup;
	fnpTree_NodeDataGet  NodeDataGet;
	fnpTree_NodeDataSet  NodeDataSet;
	fnpTree_Clear        Clear;
}; // "Tree" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Tree_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION   mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTREE_DUP_ALLOWED    mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgTREE_GRANULARITY    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgTREE_NODE_SIZE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgTREE_COMPARE_OBJECT mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001004 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Tree_Add( _this, result, element, size )               ((_this)->vtbl->Add( (_this), result, element, size ))
	#define CALL_Tree_Remove( _this, node )                             ((_this)->vtbl->Remove( (_this), node ))
	#define CALL_Tree_Search( _this, result, element, size )            ((_this)->vtbl->Search( (_this), result, element, size ))
	#define CALL_Tree_AtLeast( _this, result, element, size )           ((_this)->vtbl->AtLeast( (_this), result, element, size ))
	#define CALL_Tree_AtMost( _this, result, element, size )            ((_this)->vtbl->AtMost( (_this), result, element, size ))
	#define CALL_Tree_First( _this, result )                            ((_this)->vtbl->First( (_this), result ))
	#define CALL_Tree_Last( _this, result )                             ((_this)->vtbl->Last( (_this), result ))
	#define CALL_Tree_Next( _this, result, node )                       ((_this)->vtbl->Next( (_this), result, node ))
	#define CALL_Tree_Prev( _this, result, node )                       ((_this)->vtbl->Prev( (_this), result, node ))
	#define CALL_Tree_NextDup( _this, result, node )                    ((_this)->vtbl->NextDup( (_this), result, node ))
	#define CALL_Tree_PrevDup( _this, result, node )                    ((_this)->vtbl->PrevDup( (_this), result, node ))
	#define CALL_Tree_NodeDataGet( _this, result, node, buffer, size )  ((_this)->vtbl->NodeDataGet( (_this), result, node, buffer, size ))
	#define CALL_Tree_NodeDataSet( _this, result, node, buffer, size )  ((_this)->vtbl->NodeDataSet( (_this), result, node, buffer, size ))
	#define CALL_Tree_Clear( _this )                                    ((_this)->vtbl->Clear( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_Tree_Add( _this, result, element, size )               ((_this)->Add( result, element, size ))
	#define CALL_Tree_Remove( _this, node )                             ((_this)->Remove( node ))
	#define CALL_Tree_Search( _this, result, element, size )            ((_this)->Search( result, element, size ))
	#define CALL_Tree_AtLeast( _this, result, element, size )           ((_this)->AtLeast( result, element, size ))
	#define CALL_Tree_AtMost( _this, result, element, size )            ((_this)->AtMost( result, element, size ))
	#define CALL_Tree_First( _this, result )                            ((_this)->First( result ))
	#define CALL_Tree_Last( _this, result )                             ((_this)->Last( result ))
	#define CALL_Tree_Next( _this, result, node )                       ((_this)->Next( result, node ))
	#define CALL_Tree_Prev( _this, result, node )                       ((_this)->Prev( result, node ))
	#define CALL_Tree_NextDup( _this, result, node )                    ((_this)->NextDup( result, node ))
	#define CALL_Tree_PrevDup( _this, result, node )                    ((_this)->PrevDup( result, node ))
	#define CALL_Tree_NodeDataGet( _this, result, node, buffer, size )  ((_this)->NodeDataGet( result, node, buffer, size ))
	#define CALL_Tree_NodeDataSet( _this, result, node, buffer, size )  ((_this)->NodeDataSet( result, node, buffer, size ))
	#define CALL_Tree_Clear( _this )                                    ((_this)->Clear( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iTree {
		virtual tERROR pr_call Add( tTREENODE* result, const tPTR element, tDWORD size ) = 0; // -- Add new element
		virtual tERROR pr_call Remove( tTREENODE node ) = 0; // -- Remove node from tree
		virtual tERROR pr_call Search( tTREENODE* result, const tPTR element, tDWORD size ) = 0; // -- Search node equil to element
		virtual tERROR pr_call AtLeast( tTREENODE* result, const tPTR element, tDWORD size ) = 0; // -- Search node equil or next greater to given element
		virtual tERROR pr_call AtMost( tTREENODE* result, const tPTR element, tDWORD size ) = 0; // -- Search node equil or less to element
		virtual tERROR pr_call First( tTREENODE* result ) = 0; // -- find first node in tree
		virtual tERROR pr_call Last( tTREENODE* result ) = 0; // -- find last node in tree
		virtual tERROR pr_call Next( tTREENODE* result, tTREENODE node ) = 0; // -- return next node in tree exluding dups
		virtual tERROR pr_call Prev( tTREENODE* result, tTREENODE node ) = 0; // -- return previous node in tree excluding dups
		virtual tERROR pr_call NextDup( tTREENODE* result, tTREENODE node ) = 0; // -- return next node equil to given one in tree
		virtual tERROR pr_call PrevDup( tTREENODE* result, tTREENODE node ) = 0; // -- return prev node equil to given one in tree
		virtual tERROR pr_call NodeDataGet( tDWORD* result, tTREENODE node, tPTR buffer, tDWORD size ) = 0; // -- Retrieve node data ( previously stored element)
		virtual tERROR pr_call NodeDataSet( tTREENODE* result, tTREENODE node, tPTR buffer, tDWORD size ) = 0; // -- Set node data
		virtual tERROR pr_call Clear() = 0;
	};

	struct pr_abstract cTree : public iTree, public iObj {

		OBJ_IMPL( cTree );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTREE()   { return (hTREE)this; }

		tBOOL pr_call get_pgTREE_DUP_ALLOWED() { return (tBOOL)getBool(pgTREE_DUP_ALLOWED); }
		tERROR pr_call set_pgTREE_DUP_ALLOWED( tBOOL value ) { return setBool(pgTREE_DUP_ALLOWED,(tBOOL)value); }

		tDWORD pr_call get_pgTREE_GRANULARITY() { return (tDWORD)getDWord(pgTREE_GRANULARITY); }
		tERROR pr_call set_pgTREE_GRANULARITY( tDWORD value ) { return setDWord(pgTREE_GRANULARITY,(tDWORD)value); }

		tDWORD pr_call get_pgTREE_NODE_SIZE() { return (tDWORD)getDWord(pgTREE_NODE_SIZE); }
		tERROR pr_call set_pgTREE_NODE_SIZE( tDWORD value ) { return setDWord(pgTREE_NODE_SIZE,(tDWORD)value); }

		hOBJECT pr_call get_pgTREE_COMPARE_OBJECT() { return (hOBJECT)getObj(pgTREE_COMPARE_OBJECT); }
		tERROR pr_call set_pgTREE_COMPARE_OBJECT( hOBJECT value ) { return setObj(pgTREE_COMPARE_OBJECT,(hOBJECT)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_tree__37b14184_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end

