// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:22 --------
// File Name   -- (null)i_iface.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_iface__14c38743_39ab_461e_a176_7d2d133967f3 )
#define __i_iface__14c38743_39ab_461e_a176_7d2d133967f3
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_methodinfo.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IFace interface implementation
// Short comments -- Interface type information
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_IFACE  ((tIID)(57))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         ifEntity; // enum type for interface entities
typedef tDWORD                         ifCommentType; // enum type for comment entities
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )

// interface entity constants
// defines id for interface entity
#define ifAny                          ((ifEntity)(0)) // refers to 'all of' or 'any' entity
#define ifType                         ((ifEntity)(1)) // refers to interface defined types
#define ifConstant                     ((ifEntity)(2)) // refers to interface defined constants
#define ifErrCode                      ((ifEntity)(3)) // refers to interface defined error codes
#define ifProperty                     ((ifEntity)(4)) // refers to interface defined property
#define ifPubMethod                    ((ifEntity)(5)) // refers to interface defined public method
#define ifIntMethod                    ((ifEntity)(6)) // refers to interface defined internal method (implementation only)
#define ifSubtype                      ((ifEntity)(7)) // refers to interface defined subtype (implementation only)
#define ifID                           ((ifEntity)(8)) // refers to interface ID
#define ifMnemonicID                   ((ifEntity)(9)) // refers to interface defined MnemonicID (implementation only)
#define ifRevision                     ((ifEntity)(10)) // refers to interface revision
#define ifCreationDate                 ((ifEntity)(11)) // refers to interface last modified date
#define ifIFaceName                    ((ifEntity)(12)) // refers to interface name
#define ifObjectName                   ((ifEntity)(13)) // refers to interface type name
#define ifDefValue                     ((ifEntity)(14)) // default value of the some entity
#define ifIFace                        ((ifEntity)(15)) // refers to interface
#define ifPlugin                       ((ifEntity)(16)) // refers to plugin
#define ifMethodParam                  ((ifEntity)(17)) // refers to method parameter
#define ifDataStruct                   ((ifEntity)(18)) // refers to data structure
#define ifDataStructMember             ((ifEntity)(19)) // refers to data structure member
#define ifMsgClass                     ((ifEntity)(20)) // message class
#define ifMsg                          ((ifEntity)(21)) // message identifier

// comment entity constants
#define ifcShort                       ((ifCommentType)(0)) // refers to short comment of some entity
#define ifcLarge                       ((ifCommentType)(1)) // refers to expanded comment of some entity
#define ifcBehavior                    ((ifCommentType)(2)) // refers to behavior comment of some entity
#define ifcValue                       ((ifCommentType)(3)) // refers to value comment of some entity
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hIFACE;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iIFaceVtbl;
typedef struct iIFaceVtbl iIFaceVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cIFace;
	typedef cIFace* hIFACE;
#else
	typedef struct tag_hIFACE {
		const iIFaceVtbl*  vtbl; // pointer to the "IFace" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hIFACE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( IFace_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpIFace_NumberOf)         ( hIFACE _this, tDWORD* result, ifEntity entity ); // -- number of items of the requested entity type defined by the interface;
	typedef   tERROR (pr_call *fnpIFace_NameOf)           ( hIFACE _this, tDWORD* result, ifEntity entity, tDWORD ind, tSTRING buffer, tDWORD size ); // -- name of the requested entity by index;
	typedef   tERROR (pr_call *fnpIFace_TypeStrOf)        ( hIFACE _this, tDWORD* result, ifEntity entity, tDWORD ind, tSTRING buffer, tDWORD size ); // -- type name of the requested entity by index;
	typedef   tERROR (pr_call *fnpIFace_TypeIDOf)         ( hIFACE _this, tDWORD* result, ifEntity entity, tDWORD ind ); // -- type id of the requested entity;
	typedef   tERROR (pr_call *fnpIFace_ValOf)            ( hIFACE _this, tDWORD* result, ifEntity entity, tDWORD ind, tVOID* buffer, tDWORD size ); // -- value of the requested entity by index;
	typedef   tERROR (pr_call *fnpIFace_CommentOf)        ( hIFACE _this, tDWORD* result, ifEntity entity, ifCommentType comment, tDWORD ind, tSTRING buffer, tDWORD size ); // -- comment for the requested entity by index and comment type;
	typedef   tERROR (pr_call *fnpIFace_IndOf)            ( hIFACE _this, tDWORD* result, ifEntity entity, const tCHAR* buffer ); // -- index of entity by name;
	typedef   tERROR (pr_call *fnpIFace_MethodName)       ( hIFACE _this, tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ); // -- name of the method by index;
	typedef   tERROR (pr_call *fnpIFace_MethodParamNum)   ( hIFACE _this, tDWORD* result, tDWORD ind );      // -- number of the method parameters;
	typedef   tERROR (pr_call *fnpIFace_MethodRetTypeStr) ( hIFACE _this, tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ); // -- return type name of the method by index;
	typedef   tERROR (pr_call *fnpIFace_MethodRetTypeID)  ( hIFACE _this, tDWORD* result, tDWORD ind );      // -- return type id;
	typedef   tERROR (pr_call *fnpIFace_MethodRetVal)     ( hIFACE _this, tDWORD* result, tDWORD ind, tVOID* buffer, tDWORD size ); // -- default return value of the method by index;
	typedef   tERROR (pr_call *fnpIFace_MethodComment)    ( hIFACE _this, tDWORD* result, tDWORD ind, ifCommentType comment, tSTRING buffer, tDWORD size ); // -- commant of the method by index and comment type;
	typedef   tERROR (pr_call *fnpIFace_ParamName)        ( hIFACE _this, tDWORD* result, tDWORD mt_ind, tDWORD par_ind, tSTRING buffer, tDWORD size ); // -- name of the parameter by method and param indexes;
	typedef   tERROR (pr_call *fnpIFace_ParamTypeStr)     ( hIFACE _this, tDWORD* result, tDWORD mt_ind, tDWORD par_ind, tSTRING buffer, tDWORD size ); // -- type name of the parameter by method and param indexes;
	typedef   tERROR (pr_call *fnpIFace_ParamTypeID)      ( hIFACE _this, tDWORD* result, tDWORD mt_ind, tDWORD par_ind ); // -- identifier of the type of the parameter by method and param indexes;
	typedef   tERROR (pr_call *fnpIFace_ParamComment)     ( hIFACE _this, tDWORD* result, tDWORD mt_ind, tDWORD par_ind, ifCommentType comment, tSTRING buffer, tDWORD size ); // -- commanr for the parameter by method and param indexes and commant type;
	typedef   tERROR (pr_call *fnpIFace_PropName)         ( hIFACE _this, tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ); // -- name of the property by index;
	typedef   tERROR (pr_call *fnpIFace_PropID)           ( hIFACE _this, tDWORD* result, tDWORD ind );      // -- identifier of the property by index;
	typedef   tERROR (pr_call *fnpIFace_PropTypeStr)      ( hIFACE _this, tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ); // -- type name of the property by index;
	typedef   tERROR (pr_call *fnpIFace_PropTypeID)       ( hIFACE _this, tDWORD* result, tDWORD ind );      // -- type id of the property by index;
	typedef   tERROR (pr_call *fnpIFace_PropComment)      ( hIFACE _this, tDWORD* result, tDWORD ind, ifCommentType comment, tSTRING buffer, tDWORD size ); // -- comment for the property by index and comment type;
	typedef   tERROR (pr_call *fnpIFace_CreateMethod)     ( hIFACE _this, hMETHODINFO* result, tDWORD ind );      // -- create an interface method object to get method info;
	typedef   tERROR (pr_call *fnpIFace_CreateType)       ( hIFACE _this );                  // -- ;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iIFaceVtbl {
	fnpIFace_NumberOf          NumberOf;
	fnpIFace_NameOf            NameOf;
	fnpIFace_TypeStrOf         TypeStrOf;
	fnpIFace_TypeIDOf          TypeIDOf;
	fnpIFace_ValOf             ValOf;
	fnpIFace_CommentOf         CommentOf;
	fnpIFace_IndOf             IndOf;
	fnpIFace_MethodName        MethodName;
	fnpIFace_MethodParamNum    MethodParamNum;
	fnpIFace_MethodRetTypeStr  MethodRetTypeStr;
	fnpIFace_MethodRetTypeID   MethodRetTypeID;
	fnpIFace_MethodRetVal      MethodRetVal;
	fnpIFace_MethodComment     MethodComment;
	fnpIFace_ParamName         ParamName;
	fnpIFace_ParamTypeStr      ParamTypeStr;
	fnpIFace_ParamTypeID       ParamTypeID;
	fnpIFace_ParamComment      ParamComment;
	fnpIFace_PropName          PropName;
	fnpIFace_PropID            PropID;
	fnpIFace_PropTypeStr       PropTypeStr;
	fnpIFace_PropTypeID        PropTypeID;
	fnpIFace_PropComment       PropComment;
	fnpIFace_CreateMethod      CreateMethod;
	fnpIFace_CreateType        CreateType;
}; // "IFace" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( IFace_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgIFACE_ID          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgIFACE_PLUGIN_ID   mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
#define pgIFACE_VENDOR_ID   mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgIFACE_SUBTYPE     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgIFACE_MNEMONIC    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001004 )
#define pgIFACE_REVISION    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgIFACE_DATE        mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001006 )
#define pgIFACE_TYPE_NAME   mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001007 )
#define pgIFACE_AUTHOR      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x0000100a )
#define pgIFACE_DATA        mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x0000100b )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_IFace_NumberOf( _this, result, entity )                                       ((_this)->vtbl->NumberOf( (_this), result, entity ))
	#define CALL_IFace_NameOf( _this, result, entity, ind, buffer, size )                      ((_this)->vtbl->NameOf( (_this), result, entity, ind, buffer, size ))
	#define CALL_IFace_TypeStrOf( _this, result, entity, ind, buffer, size )                   ((_this)->vtbl->TypeStrOf( (_this), result, entity, ind, buffer, size ))
	#define CALL_IFace_TypeIDOf( _this, result, entity, ind )                                  ((_this)->vtbl->TypeIDOf( (_this), result, entity, ind ))
	#define CALL_IFace_ValOf( _this, result, entity, ind, buffer, size )                       ((_this)->vtbl->ValOf( (_this), result, entity, ind, buffer, size ))
	#define CALL_IFace_CommentOf( _this, result, entity, comment, ind, buffer, size )          ((_this)->vtbl->CommentOf( (_this), result, entity, comment, ind, buffer, size ))
	#define CALL_IFace_IndOf( _this, result, entity, buffer )                                  ((_this)->vtbl->IndOf( (_this), result, entity, buffer ))
	#define CALL_IFace_MethodName( _this, result, ind, buffer, size )                          ((_this)->vtbl->MethodName( (_this), result, ind, buffer, size ))
	#define CALL_IFace_MethodParamNum( _this, result, ind )                                    ((_this)->vtbl->MethodParamNum( (_this), result, ind ))
	#define CALL_IFace_MethodRetTypeStr( _this, result, ind, buffer, size )                    ((_this)->vtbl->MethodRetTypeStr( (_this), result, ind, buffer, size ))
	#define CALL_IFace_MethodRetTypeID( _this, result, ind )                                   ((_this)->vtbl->MethodRetTypeID( (_this), result, ind ))
	#define CALL_IFace_MethodRetVal( _this, result, ind, buffer, size )                        ((_this)->vtbl->MethodRetVal( (_this), result, ind, buffer, size ))
	#define CALL_IFace_MethodComment( _this, result, ind, comment, buffer, size )              ((_this)->vtbl->MethodComment( (_this), result, ind, comment, buffer, size ))
	#define CALL_IFace_ParamName( _this, result, mt_ind, par_ind, buffer, size )               ((_this)->vtbl->ParamName( (_this), result, mt_ind, par_ind, buffer, size ))
	#define CALL_IFace_ParamTypeStr( _this, result, mt_ind, par_ind, buffer, size )            ((_this)->vtbl->ParamTypeStr( (_this), result, mt_ind, par_ind, buffer, size ))
	#define CALL_IFace_ParamTypeID( _this, result, mt_ind, par_ind )                           ((_this)->vtbl->ParamTypeID( (_this), result, mt_ind, par_ind ))
	#define CALL_IFace_ParamComment( _this, result, mt_ind, par_ind, comment, buffer, size )   ((_this)->vtbl->ParamComment( (_this), result, mt_ind, par_ind, comment, buffer, size ))
	#define CALL_IFace_PropName( _this, result, ind, buffer, size )                            ((_this)->vtbl->PropName( (_this), result, ind, buffer, size ))
	#define CALL_IFace_PropID( _this, result, ind )                                            ((_this)->vtbl->PropID( (_this), result, ind ))
	#define CALL_IFace_PropTypeStr( _this, result, ind, buffer, size )                         ((_this)->vtbl->PropTypeStr( (_this), result, ind, buffer, size ))
	#define CALL_IFace_PropTypeID( _this, result, ind )                                        ((_this)->vtbl->PropTypeID( (_this), result, ind ))
	#define CALL_IFace_PropComment( _this, result, ind, comment, buffer, size )                ((_this)->vtbl->PropComment( (_this), result, ind, comment, buffer, size ))
	#define CALL_IFace_CreateMethod( _this, result, ind )                                      ((_this)->vtbl->CreateMethod( (_this), result, ind ))
	#define CALL_IFace_CreateType( _this )                                                     ((_this)->vtbl->CreateType( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_IFace_NumberOf( _this, result, entity )                                       ((_this)->NumberOf( result, entity ))
	#define CALL_IFace_NameOf( _this, result, entity, ind, buffer, size )                      ((_this)->NameOf( result, entity, ind, buffer, size ))
	#define CALL_IFace_TypeStrOf( _this, result, entity, ind, buffer, size )                   ((_this)->TypeStrOf( result, entity, ind, buffer, size ))
	#define CALL_IFace_TypeIDOf( _this, result, entity, ind )                                  ((_this)->TypeIDOf( result, entity, ind ))
	#define CALL_IFace_ValOf( _this, result, entity, ind, buffer, size )                       ((_this)->ValOf( result, entity, ind, buffer, size ))
	#define CALL_IFace_CommentOf( _this, result, entity, comment, ind, buffer, size )          ((_this)->CommentOf( result, entity, comment, ind, buffer, size ))
	#define CALL_IFace_IndOf( _this, result, entity, buffer )                                  ((_this)->IndOf( result, entity, buffer ))
	#define CALL_IFace_MethodName( _this, result, ind, buffer, size )                          ((_this)->MethodName( result, ind, buffer, size ))
	#define CALL_IFace_MethodParamNum( _this, result, ind )                                    ((_this)->MethodParamNum( result, ind ))
	#define CALL_IFace_MethodRetTypeStr( _this, result, ind, buffer, size )                    ((_this)->MethodRetTypeStr( result, ind, buffer, size ))
	#define CALL_IFace_MethodRetTypeID( _this, result, ind )                                   ((_this)->MethodRetTypeID( result, ind ))
	#define CALL_IFace_MethodRetVal( _this, result, ind, buffer, size )                        ((_this)->MethodRetVal( result, ind, buffer, size ))
	#define CALL_IFace_MethodComment( _this, result, ind, comment, buffer, size )              ((_this)->MethodComment( result, ind, comment, buffer, size ))
	#define CALL_IFace_ParamName( _this, result, mt_ind, par_ind, buffer, size )               ((_this)->ParamName( result, mt_ind, par_ind, buffer, size ))
	#define CALL_IFace_ParamTypeStr( _this, result, mt_ind, par_ind, buffer, size )            ((_this)->ParamTypeStr( result, mt_ind, par_ind, buffer, size ))
	#define CALL_IFace_ParamTypeID( _this, result, mt_ind, par_ind )                           ((_this)->ParamTypeID( result, mt_ind, par_ind ))
	#define CALL_IFace_ParamComment( _this, result, mt_ind, par_ind, comment, buffer, size )   ((_this)->ParamComment( result, mt_ind, par_ind, comment, buffer, size ))
	#define CALL_IFace_PropName( _this, result, ind, buffer, size )                            ((_this)->PropName( result, ind, buffer, size ))
	#define CALL_IFace_PropID( _this, result, ind )                                            ((_this)->PropID( result, ind ))
	#define CALL_IFace_PropTypeStr( _this, result, ind, buffer, size )                         ((_this)->PropTypeStr( result, ind, buffer, size ))
	#define CALL_IFace_PropTypeID( _this, result, ind )                                        ((_this)->PropTypeID( result, ind ))
	#define CALL_IFace_PropComment( _this, result, ind, comment, buffer, size )                ((_this)->PropComment( result, ind, comment, buffer, size ))
	#define CALL_IFace_CreateMethod( _this, result, ind )                                      ((_this)->CreateMethod( result, ind ))
	#define CALL_IFace_CreateType( _this )                                                     ((_this)->CreateType( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iIFace {
		virtual tERROR pr_call NumberOf( tDWORD* result, ifEntity entity ) = 0; // -- number of items of the requested entity type defined by the interface
		virtual tERROR pr_call NameOf( tDWORD* result, ifEntity entity, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- name of the requested entity by index
		virtual tERROR pr_call TypeStrOf( tDWORD* result, ifEntity entity, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- type name of the requested entity by index
		virtual tERROR pr_call TypeIDOf( tDWORD* result, ifEntity entity, tDWORD ind ) = 0; // -- type id of the requested entity
		virtual tERROR pr_call ValOf( tDWORD* result, ifEntity entity, tDWORD ind, tVOID* buffer, tDWORD size ) = 0; // -- value of the requested entity by index
		virtual tERROR pr_call CommentOf( tDWORD* result, ifEntity entity, ifCommentType comment, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- comment for the requested entity by index and comment type
		virtual tERROR pr_call IndOf( tDWORD* result, ifEntity entity, const tCHAR* buffer ) = 0; // -- index of entity by name
		virtual tERROR pr_call MethodName( tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- name of the method by index
		virtual tERROR pr_call MethodParamNum( tDWORD* result, tDWORD ind ) = 0; // -- number of the method parameters
		virtual tERROR pr_call MethodRetTypeStr( tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- return type name of the method by index
		virtual tERROR pr_call MethodRetTypeID( tDWORD* result, tDWORD ind ) = 0; // -- return type id
		virtual tERROR pr_call MethodRetVal( tDWORD* result, tDWORD ind, tVOID* buffer, tDWORD size ) = 0; // -- default return value of the method by index
		virtual tERROR pr_call MethodComment( tDWORD* result, tDWORD ind, ifCommentType comment, tSTRING buffer, tDWORD size ) = 0; // -- commant of the method by index and comment type
		virtual tERROR pr_call ParamName( tDWORD* result, tDWORD mt_ind, tDWORD par_ind, tSTRING buffer, tDWORD size ) = 0; // -- name of the parameter by method and param indexes
		virtual tERROR pr_call ParamTypeStr( tDWORD* result, tDWORD mt_ind, tDWORD par_ind, tSTRING buffer, tDWORD size ) = 0; // -- type name of the parameter by method and param indexes
		virtual tERROR pr_call ParamTypeID( tDWORD* result, tDWORD mt_ind, tDWORD par_ind ) = 0; // -- identifier of the type of the parameter by method and param indexes
		virtual tERROR pr_call ParamComment( tDWORD* result, tDWORD mt_ind, tDWORD par_ind, ifCommentType comment, tSTRING buffer, tDWORD size ) = 0; // -- commanr for the parameter by method and param indexes and commant type
		virtual tERROR pr_call PropName( tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- name of the property by index
		virtual tERROR pr_call PropID( tDWORD* result, tDWORD ind ) = 0; // -- identifier of the property by index
		virtual tERROR pr_call PropTypeStr( tDWORD* result, tDWORD ind, tSTRING buffer, tDWORD size ) = 0; // -- type name of the property by index
		virtual tERROR pr_call PropTypeID( tDWORD* result, tDWORD ind ) = 0; // -- type id of the property by index
		virtual tERROR pr_call PropComment( tDWORD* result, tDWORD ind, ifCommentType comment, tSTRING buffer, tDWORD size ) = 0; // -- comment for the property by index and comment type
		virtual tERROR pr_call CreateMethod( hMETHODINFO* result, tDWORD ind ) = 0; // -- create an interface method object to get method info
		virtual tERROR pr_call CreateType() = 0;
	};

	struct pr_abstract cIFace : public iIFace, public iObj {

		OBJ_IMPL( cIFace );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hIFACE()   { return (hIFACE)this; }

		tDWORD pr_call get_pgIFACE_ID() { return (tDWORD)getDWord(pgIFACE_ID); }
		tERROR pr_call set_pgIFACE_ID( tDWORD value ) { return setDWord(pgIFACE_ID,(tDWORD)value); }

		tDWORD pr_call get_pgIFACE_PLUGIN_ID() { return (tDWORD)getDWord(pgIFACE_PLUGIN_ID); }
		tERROR pr_call set_pgIFACE_PLUGIN_ID( tDWORD value ) { return setDWord(pgIFACE_PLUGIN_ID,(tDWORD)value); }

		tDWORD pr_call get_pgIFACE_VENDOR_ID() { return (tDWORD)getDWord(pgIFACE_VENDOR_ID); }
		tERROR pr_call set_pgIFACE_VENDOR_ID( tDWORD value ) { return setDWord(pgIFACE_VENDOR_ID,(tDWORD)value); }

		tDWORD pr_call get_pgIFACE_SUBTYPE() { return (tDWORD)getDWord(pgIFACE_SUBTYPE); }
		tERROR pr_call set_pgIFACE_SUBTYPE( tDWORD value ) { return setDWord(pgIFACE_SUBTYPE,(tDWORD)value); }

		tERROR pr_call get_pgIFACE_MNEMONIC( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgIFACE_MNEMONIC,buff,size,cp); }
		tERROR pr_call set_pgIFACE_MNEMONIC( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgIFACE_MNEMONIC,buff,size,cp); }

		tDWORD pr_call get_pgIFACE_REVISION() { return (tDWORD)getDWord(pgIFACE_REVISION); }
		tERROR pr_call set_pgIFACE_REVISION( tDWORD value ) { return setDWord(pgIFACE_REVISION,(tDWORD)value); }

		tERROR pr_call get_pgIFACE_DATE( tDATETIME* value ) { return get(pgIFACE_DATE,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgIFACE_DATE( tDATETIME* value ) { return set(pgIFACE_DATE,value,sizeof(tDATETIME)); }

		tERROR pr_call get_pgIFACE_TYPE_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgIFACE_TYPE_NAME,buff,size,cp); }
		tERROR pr_call set_pgIFACE_TYPE_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgIFACE_TYPE_NAME,buff,size,cp); }

		tERROR pr_call get_pgIFACE_AUTHOR( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgIFACE_AUTHOR,buff,size,cp); }
		tERROR pr_call set_pgIFACE_AUTHOR( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgIFACE_AUTHOR,buff,size,cp); }

		tERROR pr_call get_pgIFACE_DATA( tPTR value, tDWORD size ) { return get(pgIFACE_DATA,value,size); }
		tERROR pr_call set_pgIFACE_DATA( tPTR value, tDWORD size ) { return set(pgIFACE_DATA,value,size); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_iface__a7c26f7b_c5fa_11d4_b764_00d0b7170e50
// AVP Prague stamp end

