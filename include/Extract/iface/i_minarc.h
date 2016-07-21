// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  16 February 2004,  12:17 --------
// File Name   -- (null)i_minarc.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_minarc__a446e1c1_e9e8_49ef_99b2_8f1fc8d68c2e )
#define __i_minarc__a446e1c1_e9e8_49ef_99b2_8f1fc8d68c2e
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>

#include <Extract/iface/i_uacall.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_MINIARCHIVER  ((tIID)(63003))
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
	typedef tUINT hMINIARC;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iMiniArchiverVtbl;
typedef struct iMiniArchiverVtbl iMiniArchiverVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cMiniArchiver;
	typedef cMiniArchiver* hMINIARC;
#else
	typedef struct tag_hMINIARC {
		const iMiniArchiverVtbl* vtbl; // pointer to the "MiniArchiver" virtual table
		const iSYSTEMVtbl*       sys;  // pointer to the "SYSTEM" virtual table
	} *hMINIARC;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( MiniArchiver_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpMiniArchiver_Reset)          ( hMINIARC _this );             // -- First call of Next method after Reset must return *FIRST* object in enumeration;
	typedef   tERROR (pr_call *fnpMiniArchiver_Next)           ( hMINIARC _this, tQWORD* result, tQWORD qwLastObjectId ); // -- First call of Next method after creating or Reset must return *FIRST* object in enumeration;
	typedef   tERROR (pr_call *fnpMiniArchiver_ObjCreate)      ( hMINIARC _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO ); // -- Create IO from current enumerated object;
	typedef   tERROR (pr_call *fnpMiniArchiver_SetAsCurrent)   ( hMINIARC _this, tQWORD qwObjectId ); // -- Set Object (qwObjectId) as currently enumerated object;
	typedef   tERROR (pr_call *fnpMiniArchiver_RebuildArchive) ( hMINIARC _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iMiniArchiverVtbl {
	fnpMiniArchiver_Reset           Reset;
	fnpMiniArchiver_Next            Next;
	fnpMiniArchiver_ObjCreate       ObjCreate;
	fnpMiniArchiver_SetAsCurrent    SetAsCurrent;
	fnpMiniArchiver_RebuildArchive  RebuildArchive;
}; // "MiniArchiver" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( MiniArchiver_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION      mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgNAME_CASE_SENSITIVE    mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001001 )
//! #define pgOBJECT_NAME            mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_PATH            mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000044 )
//! #define pgOBJECT_FULL_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
#define pgIS_FOLDER              mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001000 )
#define pgNAME_MAX_LEN           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgIS_READONLY            mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001010 )
//! #define pgOBJECT_ORIGIN          mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
#define pgIS_SOLID               mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001011 )
//! #define pgOBJECT_AVAILABILITY    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
//! #define pgOBJECT_OS_TYPE         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000049 )
//! #define pgOBJECT_SIZE_Q          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000045 )
#define pgOBJECT_FILL_CHAR       mPROPERTY_MAKE_GLOBAL( pTYPE_BYTE    , 0x00001004 )
#define pgNAME_DELIMITER         mPROPERTY_MAKE_GLOBAL( pTYPE_WORD    , 0x00001002 )
//! #define pgOBJECT_OPEN_MODE       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004a )
//! #define pgOBJECT_ACCESS_MODE     mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004e )
//! #define pgOBJECT_HASH            mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000055 )
#define pgMINIARC_PROP_OBJECT_ID mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00001012 )
#define pgMULTIVOL_AS_SINGLE_SET mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001013 )
//! #define pgOBJECT_COMPRESSED_SIZE mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000050 )
#define pgOBJECT_VOLUME_NAME     mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001014 )
#define pgOBJECT_COMPRESSION_METHOD	mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001015 ) 
#define pgOBJECT_PASSWORD_PROTECTED	mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001016 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_MiniArchiver_Reset( _this )                                                     ((_this)->vtbl->Reset( (_this) ))
	#define CALL_MiniArchiver_Next( _this, result, qwLastObjectId )                              ((_this)->vtbl->Next( (_this), result, qwLastObjectId ))
	#define CALL_MiniArchiver_ObjCreate( _this, result, qwObjectIdToCreate, hUniArcIO )          ((_this)->vtbl->ObjCreate( (_this), result, qwObjectIdToCreate, hUniArcIO ))
	#define CALL_MiniArchiver_SetAsCurrent( _this, qwObjectId )                                  ((_this)->vtbl->SetAsCurrent( (_this), qwObjectId ))
	#define CALL_MiniArchiver_RebuildArchive( _this, hCallBack, hOutputIo )                      ((_this)->vtbl->RebuildArchive( (_this), hCallBack, hOutputIo ))
#else // if !defined( __cplusplus )
	#define CALL_MiniArchiver_Reset( _this )                                                     ((_this)->Reset( ))
	#define CALL_MiniArchiver_Next( _this, result, qwLastObjectId )                              ((_this)->Next( result, qwLastObjectId ))
	#define CALL_MiniArchiver_ObjCreate( _this, result, qwObjectIdToCreate, hUniArcIO )          ((_this)->ObjCreate( result, qwObjectIdToCreate, hUniArcIO ))
	#define CALL_MiniArchiver_SetAsCurrent( _this, qwObjectId )                                  ((_this)->SetAsCurrent( qwObjectId ))
	#define CALL_MiniArchiver_RebuildArchive( _this, hCallBack, hOutputIo )                      ((_this)->RebuildArchive( hCallBack, hOutputIo ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iMiniArchiver {
		virtual tERROR pr_call Reset() = 0; // -- First call of Next method after Reset must return *FIRST* object in enumeration
		virtual tERROR pr_call Next( tQWORD* result, tQWORD qwLastObjectId ) = 0; // -- First call of Next method after creating or Reset must return *FIRST* object in enumeration
		virtual tERROR pr_call ObjCreate( hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO ) = 0; // -- Create IO from current enumerated object
		virtual tERROR pr_call SetAsCurrent( tQWORD qwObjectId ) = 0; // -- Set Object (qwObjectId) as currently enumerated object
		virtual tERROR pr_call RebuildArchive( hUNIARCCALLBACK hCallBack, hIO hOutputIo ) = 0;
	};

	struct pr_abstract cMiniArchiver : public iMiniArchiver, public iObj {

		OBJ_IMPL( cMiniArchiver );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hMINIARC()   { return (hMINIARC)this; }

		tBOOL pr_call get_pgNAME_CASE_SENSITIVE() { return (tBOOL)getBool(pgNAME_CASE_SENSITIVE); }
		tERROR pr_call set_pgNAME_CASE_SENSITIVE( tBOOL value ) { return setBool(pgNAME_CASE_SENSITIVE,(tBOOL)value); }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_PATH,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_PATH,buff,size,cp); }

		tERROR pr_call get_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_FULL_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_FULL_NAME,buff,size,cp); }

		tBOOL pr_call get_pgIS_FOLDER() { return (tBOOL)getBool(pgIS_FOLDER); }
		tERROR pr_call set_pgIS_FOLDER( tBOOL value ) { return setBool(pgIS_FOLDER,(tBOOL)value); }

		tDWORD pr_call get_pgNAME_MAX_LEN() { return (tDWORD)getDWord(pgNAME_MAX_LEN); }
		tERROR pr_call set_pgNAME_MAX_LEN( tDWORD value ) { return setDWord(pgNAME_MAX_LEN,(tDWORD)value); }

		tBOOL pr_call get_pgIS_READONLY() { return (tBOOL)getBool(pgIS_READONLY); }
		tERROR pr_call set_pgIS_READONLY( tBOOL value ) { return setBool(pgIS_READONLY,(tBOOL)value); }

		tORIG_ID pr_call get_pgOBJECT_ORIGIN() { return (tORIG_ID)getDWord(pgOBJECT_ORIGIN); }
		tERROR pr_call set_pgOBJECT_ORIGIN( tORIG_ID value ) { return setDWord(pgOBJECT_ORIGIN,(tDWORD)value); }

		tBOOL pr_call get_pgIS_SOLID() { return (tBOOL)getBool(pgIS_SOLID); }
		tERROR pr_call set_pgIS_SOLID( tBOOL value ) { return setBool(pgIS_SOLID,(tBOOL)value); }

		tDWORD pr_call get_pgOBJECT_AVAILABILITY() { return (tDWORD)getDWord(pgOBJECT_AVAILABILITY); }
		tERROR pr_call set_pgOBJECT_AVAILABILITY( tDWORD value ) { return setDWord(pgOBJECT_AVAILABILITY,(tDWORD)value); }

		tDWORD pr_call get_pgOBJECT_OS_TYPE() { return (tDWORD)getDWord(pgOBJECT_OS_TYPE); }
		tERROR pr_call set_pgOBJECT_OS_TYPE( tDWORD value ) { return setDWord(pgOBJECT_OS_TYPE,(tDWORD)value); }

		tDWORD pr_call get_pgOBJECT_SIZE_Q() { return (tDWORD)getDWord(pgOBJECT_SIZE_Q); }
		tERROR pr_call set_pgOBJECT_SIZE_Q( tDWORD value ) { return setDWord(pgOBJECT_SIZE_Q,(tDWORD)value); }

		tBYTE pr_call get_pgOBJECT_FILL_CHAR() { return (tBYTE)getByte(pgOBJECT_FILL_CHAR); }
		tERROR pr_call set_pgOBJECT_FILL_CHAR( tBYTE value ) { return setByte(pgOBJECT_FILL_CHAR,(tBYTE)value); }

		tWORD pr_call get_pgNAME_DELIMITER() { return (tWORD)getWord(pgNAME_DELIMITER); }
		tERROR pr_call set_pgNAME_DELIMITER( tWORD value ) { return setWord(pgNAME_DELIMITER,(tWORD)value); }

		tDWORD pr_call get_pgOBJECT_OPEN_MODE() { return (tDWORD)getDWord(pgOBJECT_OPEN_MODE); }
		tERROR pr_call set_pgOBJECT_OPEN_MODE( tDWORD value ) { return setDWord(pgOBJECT_OPEN_MODE,(tDWORD)value); }

		tDWORD pr_call get_pgOBJECT_ACCESS_MODE() { return (tDWORD)getDWord(pgOBJECT_ACCESS_MODE); }
		tERROR pr_call set_pgOBJECT_ACCESS_MODE( tDWORD value ) { return setDWord(pgOBJECT_ACCESS_MODE,(tDWORD)value); }

		tDWORD pr_call get_pgOBJECT_HASH() { return (tDWORD)getDWord(pgOBJECT_HASH); }
		tERROR pr_call set_pgOBJECT_HASH( tDWORD value ) { return setDWord(pgOBJECT_HASH,(tDWORD)value); }

		tQWORD pr_call get_pgMINIARC_PROP_OBJECT_ID() { return (tQWORD)getQWord(pgMINIARC_PROP_OBJECT_ID); }
		tERROR pr_call set_pgMINIARC_PROP_OBJECT_ID( tQWORD value ) { return setQWord(pgMINIARC_PROP_OBJECT_ID,(tQWORD)value); }

		tBOOL pr_call get_pgMULTIVOL_AS_SINGLE_SET() { return (tBOOL)getBool(pgMULTIVOL_AS_SINGLE_SET); }
		tERROR pr_call set_pgMULTIVOL_AS_SINGLE_SET( tBOOL value ) { return setBool(pgMULTIVOL_AS_SINGLE_SET,(tBOOL)value); }

		tDWORD pr_call get_pgOBJECT_COMPRESSED_SIZE() { return (tDWORD)getDWord(pgOBJECT_COMPRESSED_SIZE); }
		tERROR pr_call set_pgOBJECT_COMPRESSED_SIZE( tDWORD value ) { return setDWord(pgOBJECT_COMPRESSED_SIZE,(tDWORD)value); }

		tERROR pr_call get_pgOBJECT_VOLUME_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_VOLUME_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_VOLUME_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_VOLUME_NAME,buff,size,cp); }

		tDWORD pr_call get_pgOBJECT_COMPRESSION_METHOD() { return (tDWORD)getDWord(pgOBJECT_COMPRESSION_METHOD); }
		tERROR pr_call set_pgOBJECT_COMPRESSION_METHOD( tDWORD value ) { return setDWord(pgOBJECT_COMPRESSION_METHOD,(tDWORD)value); }

		tBOOL pr_call get_pgOBJECT_PASSWORD_PROTECTED() { return (tBOOL)getBool(pgOBJECT_PASSWORD_PROTECTED); }
		tERROR pr_call set_pgOBJECT_PASSWORD_PROTECTED( tBOOL value ) { return setBool(pgOBJECT_PASSWORD_PROTECTED,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_minarc__37b13829_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



