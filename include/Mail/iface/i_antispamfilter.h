// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  16 March 2005,  14:38 --------
// File Name   -- (null)i_antispamfilter.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_antispamfilter__86b1b8e2_e570_4ebe_b60a_587a4c2a817a )
#define __i_antispamfilter__86b1b8e2_e570_4ebe_b60a_587a4c2a817a
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_serializable.h>
#include <ProductCore/iface/i_taskmanager.h>


// AVP Prague stamp begin( Interface comment,  )
// AntispamFilter interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_ANTISPAMFILTER  ((tIID)(40011))
// AVP Prague stamp end



enum AntispamResult_t
{
	AntispamResult_HAM = 0,
	AntispamResult_PROBABLE_SPAM,
	AntispamResult_SPAM
};

// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_ANTISPAM_TRAIN 0x7c47bd9d //

// message class
#define pmc_ANTISPAM_PROCESS 0x7b85cef0 //

// message class
#define pmc_ANTISPAM_REPORT 0x3e006bbe //
// AVP Prague stamp end

// message class
#define pmc_ANTISPAM_HAS_BEEN_TRAINED 0x171cefa3 /*is a crc32 for "pmc_ANTISPAM_HAS_BEEN_TRAINED"*/
#define pmc_ANTISPAM_NEED_TRAINING    0xc03dc416 /*is a crc32 for "pmc_ANTISPAM_NEED_TRAINING"*/


// AVP Prague stamp begin( Interface defined errors,  )
#define errNeedMoreData                          PR_MAKE_DECL_WARN(IID_ANTISPAMFILTER, 0x001) // 0x09c4b001,163885057 (1) -- Мало данных в базе антиспама. Работать можно только в режиме обучения.
// AVP Prague stamp end

#define pmc_ANTISPAM_ASK_PLUGIN 0x28c7ee3a 
#define npMESSAGE_UNCOMPLETE    ((tSTRING)("MESSAGE_UNCOMPLETE"))  // Флаг, выставляемый перехватчиком: данное письмо скачано не полностью, его не надо проверять всеми доступными алгоритмами


// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hANTISPAMFILTER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iAntispamFilterVtbl;
typedef struct iAntispamFilterVtbl iAntispamFilterVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cAntispamFilter;
	typedef cAntispamFilter* hANTISPAMFILTER;
#else
	typedef struct tag_hANTISPAMFILTER {
		const iAntispamFilterVtbl* vtbl; // pointer to the "AntispamFilter" virtual table
		const iSYSTEMVtbl*         sys;  // pointer to the "SYSTEM" virtual table
	} *hANTISPAMFILTER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( AntispamFilter_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpAntispamFilter_Process)       ( hANTISPAMFILTER _this, tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ); // -- ;
	typedef   tERROR (pr_call *fnpAntispamFilter_Train)         ( hANTISPAMFILTER _this, cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ); // -- ;
	typedef   tERROR (pr_call *fnpAntispamFilter_SetState)      ( hANTISPAMFILTER _this, tTaskState p_state ); // -- ;
	typedef   tERROR (pr_call *fnpAntispamFilter_GetStatistics) ( hANTISPAMFILTER _this, cSerializable* p_statistics ); // -- ;
	typedef   tERROR (pr_call *fnpAntispamFilter_SetSettings)   ( hANTISPAMFILTER _this, const cSerializable* p_pSettings ); // -- ;
	typedef   tERROR (pr_call *fnpAntispamFilter_GetSettings)   ( hANTISPAMFILTER _this, cSerializable* p_pSettings ); // -- ;
	typedef   tERROR (pr_call *fnpAntispamFilter_Init)          ( hANTISPAMFILTER _this, const cSerializable* p_pSettings ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iAntispamFilterVtbl {
	fnpAntispamFilter_Process        Process;
	fnpAntispamFilter_Train          Train;
	fnpAntispamFilter_SetState       SetState;
	fnpAntispamFilter_GetStatistics  GetStatistics;
	fnpAntispamFilter_SetSettings    SetSettings;
	fnpAntispamFilter_GetSettings    GetSettings;
	fnpAntispamFilter_Init           Init;
}; // "AntispamFilter" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( AntispamFilter_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_AntispamFilter_Process( _this, result, p_pProcessParams, p_hObjectToCheck, p_dwTimeOut, p_pStatistics )            ((_this)->vtbl->Process( (_this), result, p_pProcessParams, p_hObjectToCheck, p_dwTimeOut, p_pStatistics ))
	#define CALL_AntispamFilter_Train( _this, p_pTrainParams, p_hObjectForCheck, p_dwTimeOut, p_pStatistics )                       ((_this)->vtbl->Train( (_this), p_pTrainParams, p_hObjectForCheck, p_dwTimeOut, p_pStatistics ))
	#define CALL_AntispamFilter_SetState( _this, p_state )                                                                          ((_this)->vtbl->SetState( (_this), p_state ))
	#define CALL_AntispamFilter_GetStatistics( _this, p_statistics )                                                                ((_this)->vtbl->GetStatistics( (_this), p_statistics ))
	#define CALL_AntispamFilter_SetSettings( _this, p_pSettings )                                                                   ((_this)->vtbl->SetSettings( (_this), p_pSettings ))
	#define CALL_AntispamFilter_GetSettings( _this, p_pSettings )                                                                   ((_this)->vtbl->GetSettings( (_this), p_pSettings ))
	#define CALL_AntispamFilter_Init( _this, p_pSettings )                                                                          ((_this)->vtbl->Init( (_this), p_pSettings ))
#else // if !defined( __cplusplus )
	#define CALL_AntispamFilter_Process( _this, result, p_pProcessParams, p_hObjectToCheck, p_dwTimeOut, p_pStatistics )            ((_this)->Process( result, p_pProcessParams, p_hObjectToCheck, p_dwTimeOut, p_pStatistics ))
	#define CALL_AntispamFilter_Train( _this, p_pTrainParams, p_hObjectForCheck, p_dwTimeOut, p_pStatistics )                       ((_this)->Train( p_pTrainParams, p_hObjectForCheck, p_dwTimeOut, p_pStatistics ))
	#define CALL_AntispamFilter_SetState( _this, p_state )                                                                          ((_this)->SetState( p_state ))
	#define CALL_AntispamFilter_GetStatistics( _this, p_statistics )                                                                ((_this)->GetStatistics( p_statistics ))
	#define CALL_AntispamFilter_SetSettings( _this, p_pSettings )                                                                   ((_this)->SetSettings( p_pSettings ))
	#define CALL_AntispamFilter_GetSettings( _this, p_pSettings )                                                                   ((_this)->GetSettings( p_pSettings ))
	#define CALL_AntispamFilter_Init( _this, p_pSettings )                                                                          ((_this)->Init( p_pSettings ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iAntispamFilter {
		virtual tERROR pr_call Process( tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ) = 0;
		virtual tERROR pr_call Train( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics ) = 0;
		virtual tERROR pr_call SetState( tTaskRequestState p_state ) = 0;
		virtual tERROR pr_call GetStatistics( cSerializable* p_statistics ) = 0;
		virtual tERROR pr_call SetSettings( const cSerializable* p_pSettings ) = 0;
		virtual tERROR pr_call GetSettings( cSerializable* p_pSettings ) = 0;
		virtual tERROR pr_call Init( const cSerializable* p_pSettings ) = 0;
	};

	struct pr_abstract cAntispamFilter : public iAntispamFilter, public iObj {
		OBJ_IMPL( cAntispamFilter );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hANTISPAMFILTER()   { return (hANTISPAMFILTER)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_antispamfilter__86b1b8e2_e570_4ebe_b60a_587a4c2a817a
// AVP Prague stamp end



