// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  26 October 2005,  13:08 --------
// File Name   -- (null)i_schecklist.cpp
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_schecklist__5c0c1906_3194_429c_a971_5035c1bef7bc )
#define __i_schecklist__5c0c1906_3194_429c_a971_5035c1bef7bc
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// SCheckList interface implementation
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_SCHECKLIST  ((tIID)(47))
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
	typedef tUINT hSCHECKLIST;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iSCheckListVtbl;
typedef struct iSCheckListVtbl iSCheckListVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cSCheckList;
	typedef cSCheckList* hSCHECKLIST;
#else
	typedef struct tag_hSCHECKLIST {
		const iSCheckListVtbl* vtbl; // pointer to the "SCheckList" virtual table
		const iSYSTEMVtbl*     sys;  // pointer to the "SYSTEM" virtual table
	} *hSCHECKLIST;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( SCheckList_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpSCheckList_AddObj)            ( hSCHECKLIST _this, tDWORD* obj_id, hOBJECT obj ); // -- add checked object to list;
	typedef   tERROR (pr_call *fnpSCheckList_AddObjByMask)      ( hSCHECKLIST _this, tDWORD* obj_id, tDWORD* obj_count, const tVOID* mask, tCODEPAGE mask_cp ); // -- ;
	typedef   tERROR (pr_call *fnpSCheckList_GetFirstObjId)     ( hSCHECKLIST _this, tDWORD* obj_id ); // -- ;
	typedef   tERROR (pr_call *fnpSCheckList_GetNextObjId)      ( hSCHECKLIST _this, tDWORD* obj_id, tDWORD prev_obj_id ); // -- ;
	typedef   tERROR (pr_call *fnpSCheckList_GetObjProp)        ( hSCHECKLIST _this, tDWORD obj_id, tDWORD prop_id, tVOID* data, tDWORD* data_size ); // -- ;
	typedef   tERROR (pr_call *fnpSCheckList_SetObjProp)        ( hSCHECKLIST _this, tDWORD obj_id, tDWORD prop_id, const tVOID* prop_data, tDWORD prop_data_size ); // -- set property of the object;
	typedef   tERROR (pr_call *fnpSCheckList_SetObjHashingProp) ( hSCHECKLIST _this, tDWORD obj_id, const tVOID* prop_data, tDWORD prop_data_size ); // -- set property of the object;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iSCheckListVtbl {
	fnpSCheckList_AddObj             AddObj;
	fnpSCheckList_AddObjByMask       AddObjByMask;
	fnpSCheckList_GetFirstObjId      GetFirstObjId;
	fnpSCheckList_GetNextObjId       GetNextObjId;
	fnpSCheckList_GetObjProp         GetObjProp;
	fnpSCheckList_SetObjProp         SetObjProp;
	fnpSCheckList_SetObjHashingProp  SetObjHashingProp;
}; // "SCheckList" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( SCheckList_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgDSKM_LIST         mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001000 )
#define pgDSKM_STORED_ERROR mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgPR_STORED_ERROR   mPROPERTY_MAKE_GLOBAL( pTYPE_ERROR   , 0x00001003 )
#define pgDSKM_COMP_ID      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001004 )
#define pgDSKM_OBJ_TYPE     mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001005 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_SCheckList_AddObj( _this, obj_id, obj )                                                   ((_this)->vtbl->AddObj( (_this), obj_id, obj ))
	#define CALL_SCheckList_AddObjByMask( _this, obj_id, obj_count, mask, mask_cp )                        ((_this)->vtbl->AddObjByMask( (_this), obj_id, obj_count, mask, mask_cp ))
	#define CALL_SCheckList_GetFirstObjId( _this, obj_id )                                                 ((_this)->vtbl->GetFirstObjId( (_this), obj_id ))
	#define CALL_SCheckList_GetNextObjId( _this, obj_id, prev_obj_id )                                     ((_this)->vtbl->GetNextObjId( (_this), obj_id, prev_obj_id ))
	#define CALL_SCheckList_GetObjProp( _this, obj_id, prop_id, data, data_size )                          ((_this)->vtbl->GetObjProp( (_this), obj_id, prop_id, data, data_size ))
	#define CALL_SCheckList_SetObjProp( _this, obj_id, prop_id, prop_data, prop_data_size )                ((_this)->vtbl->SetObjProp( (_this), obj_id, prop_id, prop_data, prop_data_size ))
	#define CALL_SCheckList_SetObjHashingProp( _this, obj_id, prop_data, prop_data_size )                  ((_this)->vtbl->SetObjHashingProp( (_this), obj_id, prop_data, prop_data_size ))
#else // if !defined( __cplusplus )
	#define CALL_SCheckList_AddObj( _this, obj_id, obj )                                                   ((_this)->AddObj( obj_id, obj ))
	#define CALL_SCheckList_AddObjByMask( _this, obj_id, obj_count, mask, mask_cp )                        ((_this)->AddObjByMask( obj_id, obj_count, mask, mask_cp ))
	#define CALL_SCheckList_GetFirstObjId( _this, obj_id )                                                 ((_this)->GetFirstObjId( obj_id ))
	#define CALL_SCheckList_GetNextObjId( _this, obj_id, prev_obj_id )                                     ((_this)->GetNextObjId( obj_id, prev_obj_id ))
	#define CALL_SCheckList_GetObjProp( _this, obj_id, prop_id, data, data_size )                          ((_this)->GetObjProp( obj_id, prop_id, data, data_size ))
	#define CALL_SCheckList_SetObjProp( _this, obj_id, prop_id, prop_data, prop_data_size )                ((_this)->SetObjProp( obj_id, prop_id, prop_data, prop_data_size ))
	#define CALL_SCheckList_SetObjHashingProp( _this, obj_id, prop_data, prop_data_size )                  ((_this)->SetObjHashingProp( obj_id, prop_data, prop_data_size ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iSCheckList {
		virtual tERROR pr_call AddObj( tDWORD* obj_id, hOBJECT obj ) = 0; // -- add checked object to list
		virtual tERROR pr_call AddObjByMask( tDWORD* obj_id, tDWORD* obj_count, const tVOID* mask, tCODEPAGE mask_cp ) = 0;
		virtual tERROR pr_call GetFirstObjId( tDWORD* obj_id ) = 0;
		virtual tERROR pr_call GetNextObjId( tDWORD* obj_id, tDWORD prev_obj_id ) = 0;
		virtual tERROR pr_call GetObjProp( tDWORD obj_id, tDWORD prop_id, tVOID* data, tDWORD* data_size ) = 0;
		virtual tERROR pr_call SetObjProp( tDWORD obj_id, tDWORD prop_id, const tVOID* prop_data, tDWORD prop_data_size ) = 0; // -- set property of the object
		virtual tERROR pr_call SetObjHashingProp( tDWORD obj_id, const tVOID* prop_data, tDWORD prop_data_size ) = 0; // -- set property of the object
	};

	struct pr_abstract cSCheckList : public iSCheckList, public iObj {

		OBJ_IMPL( cSCheckList );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSCHECKLIST()   { return (hSCHECKLIST)this; }

		tPTR pr_call get_pgDSKM_LIST() { return (tPTR)getPtr(pgDSKM_LIST); }
		tERROR pr_call set_pgDSKM_LIST( tPTR value ) { return setPtr(pgDSKM_LIST,(tPTR)value); }

		tDWORD pr_call get_pgDSKM_STORED_ERROR() { return (tDWORD)getDWord(pgDSKM_STORED_ERROR); }
		tERROR pr_call set_pgDSKM_STORED_ERROR( tDWORD value ) { return setDWord(pgDSKM_STORED_ERROR,(tDWORD)value); }

		tERROR pr_call get_pgPR_STORED_ERROR() { return (tERROR)getDWord(pgPR_STORED_ERROR); }
		tERROR pr_call set_pgPR_STORED_ERROR( tERROR value ) { return setDWord(pgPR_STORED_ERROR,(tDWORD)value); }

		tERROR pr_call get_pgDSKM_COMP_ID( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgDSKM_COMP_ID,buff,size,cp); }
		tERROR pr_call set_pgDSKM_COMP_ID( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgDSKM_COMP_ID,buff,size,cp); }

		tERROR pr_call get_pgDSKM_OBJ_TYPE( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgDSKM_OBJ_TYPE,buff,size,cp); }
		tERROR pr_call set_pgDSKM_OBJ_TYPE( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgDSKM_OBJ_TYPE,buff,size,cp); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_schecklist__5c0c1906_3194_429c_a971_5035c1bef7bc
// AVP Prague stamp end



