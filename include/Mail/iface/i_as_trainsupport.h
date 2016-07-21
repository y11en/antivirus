// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  07 December 2007,  18:41 --------
// File Name   -- (null)i_as_trainsupport.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_as_trainsupport__15bf5fd2_a275_452f_8be4_7dc7e72ba710 )
#define __i_as_trainsupport__15bf5fd2_a275_452f_8be4_7dc7e72ba710
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



#include <Mail/structs/s_as_trainsupport.h>


// AVP Prague stamp begin( Interface comment,  )
// AS_TrainSupport interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_AS_TRAINSUPPORT  ((tIID)(40012))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hAS_TRAINSUPPORT;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAS_TrainSupportVtbl;
typedef struct iAS_TrainSupportVtbl iAS_TrainSupportVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAS_TrainSupport;
	typedef cAS_TrainSupport* hAS_TRAINSUPPORT;
#else
	typedef struct tag_hAS_TRAINSUPPORT {
		const iAS_TrainSupportVtbl* vtbl; // pointer to the "AS_TrainSupport" virtual table
		const iSYSTEMVtbl*          sys;  // pointer to the "SYSTEM" virtual table
	} *hAS_TRAINSUPPORT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AS_TrainSupport_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAS_TrainSupport_GetFolderInfoList)       ( hAS_TRAINSUPPORT _this, FolderInfo_t* pFolderInfo, FolderInfoList_t* pFolderInfoList ); // -- Return one level of folder hierarchy;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_Train)                   ( hAS_TRAINSUPPORT _this, cSerializable* pFolderInfoList ); // -- Does antispam trainig in the folder;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_GetMAPIEntryID)          ( hAS_TRAINSUPPORT _this, FolderInfo_t* pFolderInfo, EntryID_t* pEntryID ); // -- ;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_CreateSubFolder)         ( hAS_TRAINSUPPORT _this, FolderInfo_t* pFolderInfo, FolderInfo_t* pNewFolderInfo ); // -- ;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_TrainAsync)              ( hAS_TRAINSUPPORT _this, cSerializable* pFolderInfoList ); // -- ;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_TrainOnOutLookSentItems) ( hAS_TRAINSUPPORT _this ); // -- ;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_TrainOnIntMailSentItems) ( hAS_TRAINSUPPORT _this ); // -- ;
	typedef   tERROR (pr_call *fnpAS_TrainSupport_TrainOnAllSentItemsAsync)( hAS_TRAINSUPPORT _this, cSerializable* Settings ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAS_TrainSupportVtbl {
	fnpAS_TrainSupport_GetFolderInfoList         GetFolderInfoList;
	fnpAS_TrainSupport_Train                     Train;
	fnpAS_TrainSupport_GetMAPIEntryID            GetMAPIEntryID;
	fnpAS_TrainSupport_CreateSubFolder           CreateSubFolder;
	fnpAS_TrainSupport_TrainAsync                TrainAsync;
	fnpAS_TrainSupport_TrainOnOutLookSentItems   TrainOnOutLookSentItems;
	fnpAS_TrainSupport_TrainOnIntMailSentItems   TrainOnIntMailSentItems;
	fnpAS_TrainSupport_TrainOnAllSentItemsAsync  TrainOnAllSentItemsAsync;
}; // "AS_TrainSupport" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AS_TrainSupport_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgTrainSupp_TM      mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001000 )
#define pgForceUISuppress   mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001001 )
#define pgNeedHamTrain      mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001002 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AS_TrainSupport_GetFolderInfoList( _this, pFolderInfo, pFolderInfoList )                     ((_this)->vtbl->GetFolderInfoList( (_this), pFolderInfo, pFolderInfoList ))
	#define CALL_AS_TrainSupport_Train( _this, pFolderInfoList )                                              ((_this)->vtbl->Train( (_this), pFolderInfoList ))
	#define CALL_AS_TrainSupport_GetMAPIEntryID( _this, pFolderInfo, pEntryID )                               ((_this)->vtbl->GetMAPIEntryID( (_this), pFolderInfo, pEntryID ))
	#define CALL_AS_TrainSupport_CreateSubFolder( _this, pFolderInfo, pNewFolderInfo )                        ((_this)->vtbl->CreateSubFolder( (_this), pFolderInfo, pNewFolderInfo ))
	#define CALL_AS_TrainSupport_TrainAsync( _this, pFolderInfoList )                                         ((_this)->vtbl->TrainAsync( (_this), pFolderInfoList ))
	#define CALL_AS_TrainSupport_TrainOnOutLookSentItems( _this )                                             ((_this)->vtbl->TrainOnOutLookSentItems( (_this) ))
	#define CALL_AS_TrainSupport_TrainOnIntMailSentItems( _this )                                             ((_this)->vtbl->TrainOnIntMailSentItems( (_this) ))
	#define CALL_AS_TrainSupport_TrainOnAllSentItemsAsync( _this, Settings )                                  ((_this)->vtbl->TrainOnAllSentItemsAsync( (_this), Settings ))
#else // if !defined( __cplusplus )
	#define CALL_AS_TrainSupport_GetFolderInfoList( _this, pFolderInfo, pFolderInfoList )                     ((_this)->GetFolderInfoList( pFolderInfo, pFolderInfoList ))
	#define CALL_AS_TrainSupport_Train( _this, pFolderInfoList )                                              ((_this)->Train( pFolderInfoList ))
	#define CALL_AS_TrainSupport_GetMAPIEntryID( _this, pFolderInfo, pEntryID )                               ((_this)->GetMAPIEntryID( pFolderInfo, pEntryID ))
	#define CALL_AS_TrainSupport_CreateSubFolder( _this, pFolderInfo, pNewFolderInfo )                        ((_this)->CreateSubFolder( pFolderInfo, pNewFolderInfo ))
	#define CALL_AS_TrainSupport_TrainAsync( _this, pFolderInfoList )                                         ((_this)->TrainAsync( pFolderInfoList ))
	#define CALL_AS_TrainSupport_TrainOnOutLookSentItems( _this )                                             ((_this)->TrainOnOutLookSentItems( ))
	#define CALL_AS_TrainSupport_TrainOnIntMailSentItems( _this )                                             ((_this)->TrainOnIntMailSentItems( ))
	#define CALL_AS_TrainSupport_TrainOnAllSentItemsAsync( _this, Settings )                                  ((_this)->TrainOnAllSentItemsAsync( Settings ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAS_TrainSupport {
		virtual tERROR pr_call GetFolderInfoList( FolderInfo_t* pFolderInfo, FolderInfoList_t* pFolderInfoList ) = 0; // -- Return one level of folder hierarchy
		virtual tERROR pr_call Train( cSerializable* pFolderInfoList ) = 0; // -- Does antispam trainig in the folder
		virtual tERROR pr_call GetMAPIEntryID( FolderInfo_t* pFolderInfo, EntryID_t* pEntryID ) = 0;
		virtual tERROR pr_call CreateSubFolder( FolderInfo_t* pFolderInfo, FolderInfo_t* pNewFolderInfo ) = 0;
		virtual tERROR pr_call TrainAsync( cSerializable* pFolderInfoList ) = 0;
		virtual tERROR pr_call TrainOnOutLookSentItems() = 0;
		virtual tERROR pr_call TrainOnIntMailSentItems() = 0;
		virtual tERROR pr_call TrainOnAllSentItemsAsync( cSerializable* Settings ) = 0;
	};

	struct pr_abstract cAS_TrainSupport : public iAS_TrainSupport, public iObj {

		OBJ_IMPL( cAS_TrainSupport );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hAS_TRAINSUPPORT()   { return (hAS_TRAINSUPPORT)this; }

		hOBJECT pr_call get_pgTrainSupp_TM() { return (hOBJECT)getObj(pgTrainSupp_TM); }
		tERROR pr_call set_pgTrainSupp_TM( hOBJECT value ) { return setObj(pgTrainSupp_TM,(hOBJECT)value); }

		tBOOL pr_call get_pgForceUISuppress() { return (tBOOL)getBool(pgForceUISuppress); }
		tERROR pr_call set_pgForceUISuppress( tBOOL value ) { return setBool(pgForceUISuppress,(tBOOL)value); }

		tBOOL pr_call get_pgNeedHamTrain() { return (tBOOL)getBool(pgNeedHamTrain); }
		tERROR pr_call set_pgNeedHamTrain( tBOOL value ) { return setBool(pgNeedHamTrain,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_as_trainsupport__15bf5fd2_a275_452f_8be4_7dc7e72ba710
// AVP Prague stamp end



