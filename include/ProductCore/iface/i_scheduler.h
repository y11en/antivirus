// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  07 December 2005,  12:13 --------
// File Name   -- (null)i_scheduler.cpp
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_scheduler__82daf49f_c1bd_4b18_bd1f_2f5a9f797c86 )
#define __i_scheduler__82daf49f_c1bd_4b18_bd1f_2f5a9f797c86
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_serializable.h>


// AVP Prague stamp begin( Interface comment,  )
// Scheduler interface implementation
// Short comments -- Simple Scheduler
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_SCHEDULER  ((tIID)(58018))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_SCHEDULER 0x66a39c45 //
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSCHEDULER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iSchedulerVtbl;
typedef struct iSchedulerVtbl iSchedulerVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cScheduler;
	typedef cScheduler* hSCHEDULER;
#else
	typedef struct tag_hSCHEDULER 
	{
		const iSchedulerVtbl* vtbl; // pointer to the "Scheduler" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hSCHEDULER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Scheduler_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpScheduler_SetSchedule)     ( hSCHEDULER _this, tDWORD dwScheduleClass, tDWORD dwScheduleID, const tPTR* pSchedule, const tPTR* pScheduleParams ); // -- ;
	typedef   tERROR (pr_call *fnpScheduler_GetSchedule)     ( hSCHEDULER _this, tDWORD dwScheduleClass, tDWORD dwScheduleID, tPTR* pSchedule, tPTR* pScheduleParams ); // -- ;
	typedef   tERROR (pr_call *fnpScheduler_DeleteSchedule)  ( hSCHEDULER _this, tDWORD dwScheduleClass, tDWORD dwScheduleID ); // -- ;
	typedef   tERROR (pr_call *fnpScheduler_GetNextSchedule) ( hSCHEDULER _this, tDWORD dwScheduleClass, tDWORD dwScheduleID, cSerializable* pSchedule, cSerializable* pScheduleParams ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iSchedulerVtbl 
{
	fnpScheduler_SetSchedule      SetSchedule;
	fnpScheduler_GetSchedule      GetSchedule;
	fnpScheduler_DeleteSchedule   DeleteSchedule;
	fnpScheduler_GetNextSchedule  GetNextSchedule;
}; // "Scheduler" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Scheduler_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgFREQUENCY         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Scheduler_SetSchedule( _this, dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams )                   ((_this)->vtbl->SetSchedule( (_this), dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams ))
	#define CALL_Scheduler_GetSchedule( _this, dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams )                   ((_this)->vtbl->GetSchedule( (_this), dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams ))
	#define CALL_Scheduler_DeleteSchedule( _this, dwScheduleClass, dwScheduleID )                                            ((_this)->vtbl->DeleteSchedule( (_this), dwScheduleClass, dwScheduleID ))
	#define CALL_Scheduler_GetNextSchedule( _this, dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams )               ((_this)->vtbl->GetNextSchedule( (_this), dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams ))
#else // if !defined( __cplusplus )
	#define CALL_Scheduler_SetSchedule( _this, dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams )                   ((_this)->SetSchedule( dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams ))
	#define CALL_Scheduler_GetSchedule( _this, dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams )                   ((_this)->GetSchedule( dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams ))
	#define CALL_Scheduler_DeleteSchedule( _this, dwScheduleClass, dwScheduleID )                                            ((_this)->DeleteSchedule( dwScheduleClass, dwScheduleID ))
	#define CALL_Scheduler_GetNextSchedule( _this, dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams )               ((_this)->GetNextSchedule( dwScheduleClass, dwScheduleID, pSchedule, pScheduleParams ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iScheduler 
	{
		virtual tERROR pr_call SetSchedule( tDWORD dwScheduleClass, tDWORD dwScheduleID, const cSerializable* pSchedule, const cSerializable* pScheduleParams ) = 0;
		virtual tERROR pr_call GetSchedule( tDWORD dwScheduleClass, tDWORD dwScheduleID, cSerializable* pSchedule, cSerializable* pScheduleParams ) = 0;
		virtual tERROR pr_call DeleteSchedule( tDWORD dwScheduleClass, tDWORD dwScheduleID ) = 0;
		virtual tERROR pr_call GetNextSchedule( tDWORD dwScheduleClass, tDWORD dwScheduleID, cSerializable* pSchedule, cSerializable* pScheduleParams ) = 0;
	};

	struct pr_abstract cScheduler : public iScheduler, public iObj 
	{
		OBJ_IMPL( cScheduler );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSCHEDULER()   { return (hSCHEDULER)this; }

		tDWORD pr_call get_pgFREQUENCY() { return (tDWORD)getDWord(pgFREQUENCY); }
		tERROR pr_call set_pgFREQUENCY( tDWORD value ) { return setDWord(pgFREQUENCY,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end

#define _TIME_(tm) ((tm) % (24*60*60))
#define _DATE_(tm) (((tm) / (24*60*60)) * (24*60*60))

// AVP Prague stamp begin( Header endif,  )
#endif // __i_scheduler__82daf49f_c1bd_4b18_bd1f_2f5a9f797c86
// AVP Prague stamp end



