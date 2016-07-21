// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  23 October 2001,  16:20 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- csection.c
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Time_CriticalSection_PRIVATE_DEFINITION
// AVP Prague stamp end

#define USE_DBG_INFO

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include "csection.h"
#include <windows.h>
#include "../codegen/prdbginfo/prdbginfo.h"
// AVP Prague stamp end

unsigned __int64 gTimePerfomanceCounter = 1;

//#define IMPEX_TABLE_CONTENT
//IMPORT_TABLE(imp_loader)
//#include "IFace/e_loader.h"
//IMPORT_TABLE_END

//+ ----------------------------------------------------------------------------------------
//#define _COLLECT_CALLERS

#define STAT_DATA_SIZE 1000000

typedef struct tag_CStatData {
	tDWORD dwCallerAddress;
	tDWORD dwReenterCount;
	unsigned __int64 TimeUnderCS;
} CStatData;

#if defined( __cplusplus )
extern "C" {
#endif
	typedef struct tag_CTimeSectData {
		CRITICAL_SECTION cs;          // critical section Windows object
		tDWORD           thread_id;   // thread which is own the object
		HANDLE           owned_event; // event is signaled when cs is owned
		BOOL (*try_enter)(LPCRITICAL_SECTION);   // NT TryEnter function pointer
		tDWORD           last_error;  // last win error
		HINSTANCE        nt_kernel;   // NT kernel instance
		
		unsigned __int64 TotalTimeWait;
#ifdef _COLLECT_CALLERS
		unsigned __int64 TotalTimeUnderCS;
		unsigned __int64 EnterCriticalSectionTime;
		CStatData* pStatData;
		tDWORD dwStatDataUsed;
		tDWORD dwReenterCount;
		tDWORD dwReenterMaxCount;
#endif
	} CTimeSectData;
	
#if defined( __cplusplus )
}
#endif
// }

#if defined (_WIN32)
#define WIN32LIB "kernel32.dll"
#elif defined (__unix__)
#define WIN32LIB "libcompat."SHARED_EXT
#else
#error "Consideration is needed!"
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// CriticalSection interface implementation
// Short comments -- Critical section behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end


//#define Time_CS_TRY
//#define Time_CS_EXCEPT

#define Time_CS_TRY     __try
#define Time_CS_EXCEPT  __except( EXCEPTION_EXECUTE_HANDLER ) { error = errUNEXPECTED; }

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Time_CriticalSection_ObjectInitDone( hi_CriticalSection _this );
tERROR pr_call Time_CriticalSection_ObjectClose( hi_CriticalSection _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Time_CriticalSection_vtbl = {
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   Time_CriticalSection_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      Time_CriticalSection_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpTime_CriticalSection_Enter)    ( hi_CriticalSection _this, tSHARE_LEVEL level ); // -- locks synchronization object;
typedef   tERROR (pr_call *fnpTime_CriticalSection_Leave)    ( hi_CriticalSection _this );                // -- unlocks synchronization object;
typedef   tERROR (pr_call *fnpTime_CriticalSection_TryEnter) ( hi_CriticalSection _this, tSHARE_LEVEL level ); // -- try enter critical section, but do not block a calling thread;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iTime_CriticalSectionVtbl {
	fnpTime_CriticalSection_Enter     Enter;
	fnpTime_CriticalSection_Leave     Leave;
	fnpTime_CriticalSection_TryEnter  TryEnter;
}; // "CriticalSection" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Time_CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level );
tERROR pr_call Time_CriticalSection_Leave( hi_CriticalSection _this );
tERROR pr_call Time_CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static struct iTime_CriticalSectionVtbl e_Time_CriticalSection_vtbl = {
	Time_CriticalSection_Enter,
	Time_CriticalSection_Leave,
	Time_CriticalSection_TryEnter
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Time_CriticalSection". Global(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "Time_CriticalSection". Property table
mPROPERTY_TABLE(Time_CriticalSection_PropTable)
	mSHARED_PROPERTY( pgSHARE_LEVEL_SUPPORT, cFALSE )
	mLOCAL_PROPERTY_BUF( pgLAST_OS_ERROR, CTimeSectData, last_error, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "critical section object", 24 )
mPROPERTY_TABLE_END(Time_CriticalSection_PropTable)
// AVP Prague stamp end


//+ ----------------------------------------------------------------------------------------
__inline tDWORD _iGetCallerAddress(tDWORD dwSkipLevels, tDWORD* pFFrame)
{
	tDWORD dwAddress;
	
	if (pFFrame == NULL)
		__asm mov pFFrame, ebp;
	
	if (dwSkipLevels)
		dwAddress = _iGetCallerAddress(dwSkipLevels-1, (tDWORD*)pFFrame[0]);
	else
		dwAddress = pFFrame[1]; // return address
	
	return dwAddress;
}

__inline tDWORD GetCallerAddress(tDWORD dwSkipLevels)
{
	tERROR error;
	Time_CS_TRY {
		return _iGetCallerAddress(dwSkipLevels, NULL);
	}
	Time_CS_EXCEPT;
	return 0;
}

#ifdef _COLLECT_CALLERS
tERROR DropStatistic(hi_CriticalSection _this)
{
	CTimeSectData* d;
	tERROR error = errOK;
	tDWORD i, j;
	char buff[0x100];
	Time_CS_TRY {
		d = (CTimeSectData*) _this->data;
		wsprintf(buff, "CS %08X STATISTIC: --------------------------------- (%#x)\n", _this, GetModuleHandle("prkernel.ppl"));
		OutputDebugString(buff);
		for (i=0; i<STAT_DATA_SIZE; i++)
		{
			if (d->pStatData[i].dwCallerAddress)
			{
				tDWORD dwCallerAddress = d->pStatData[i].dwCallerAddress;
				unsigned __int64	TimeCSMin = d->pStatData[i].TimeUnderCS;
				unsigned __int64	TimeCSMax = d->pStatData[i].TimeUnderCS;
				unsigned __int64	TimeCSTotal = 0;
				ULONG CallCount = 0;
				ULONG MinReenter = (ULONG) -1;
				ULONG MaxReenter = 0;
				for (j=i; j<STAT_DATA_SIZE; j++)
				{
					if (d->pStatData[j].dwCallerAddress == dwCallerAddress)
					{
						d->pStatData[j].dwCallerAddress = 0;
						if (TimeCSMin > d->pStatData[j].TimeUnderCS)
							TimeCSMin = d->pStatData[j].TimeUnderCS;
						if (TimeCSMax < d->pStatData[j].TimeUnderCS)
							TimeCSMax = d->pStatData[j].TimeUnderCS;

						if (MinReenter > d->pStatData[j].dwReenterCount)
							MinReenter = d->pStatData[j].dwReenterCount;
						if (MaxReenter < d->pStatData[j].dwReenterCount)
							MaxReenter = d->pStatData[j].dwReenterCount;
						
						TimeCSTotal += d->pStatData[j].TimeUnderCS;
						CallCount++;
					}
				}
		
				wsprintf(buff, "Caller: %08X  (calls %6u) Time: Total=%I64d.%04I64d (%I64d) Min=%I64d Max=%I64d (Renter %d-%d) -> ",
					dwCallerAddress, CallCount, TimeCSTotal / gTimePerfomanceCounter, (TimeCSTotal % gTimePerfomanceCounter) * 10000 / gTimePerfomanceCounter, 
					TimeCSTotal, TimeCSMin, TimeCSMax, MinReenter, MaxReenter);
				OutputDebugString(buff);

				if (PrDbgGetInfoEx == NULL) 
					OutputDebugString("\n");
				else
				{
					char dbginfo[0x100];
					if (PR_SUCC(PrDbgGetInfoEx(dwCallerAddress, NULL, 0, DBGINFO_SYMBOL_NAME, dbginfo, sizeof(dbginfo), NULL)))
						OutputDebugString(dbginfo);
					OutputDebugString("\n");
				}
			}
		}
		
	}
	Time_CS_EXCEPT;
	
	d->dwStatDataUsed = 0;
	
	return error;
}
#endif

//+ ----------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Time_CriticalSection". Register function
tERROR pr_call Time_CriticalSection_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"Time_CriticalSection::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_CRITICAL_SECTION,                   // interface identifier
		PID_WIN32_PLUGIN_LOADER,                // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Time_CriticalSection_vtbl,                // internal(kernel called) function table
		(sizeof(i_Time_CriticalSection_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Time_CriticalSection_vtbl,                // external function table
		(sizeof(e_Time_CriticalSection_vtbl)/sizeof(tPTR)),// external function table size
		Time_CriticalSection_PropTable,              // property table
		mPROPERTY_TABLE_SIZE(Time_CriticalSection_PropTable),// property table size
		sizeof(CTimeSectData),                      // memory size
		0                                       // interface flags
	);

	if (QueryPerformanceFrequency((LARGE_INTEGER*)&gTimePerfomanceCounter) == FALSE)
		gTimePerfomanceCounter = 1;

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Time_CriticalSection(IID_CRITICAL_SECTION) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"Time_CriticalSection::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Time_CriticalSection_ObjectInitDone( hi_CriticalSection _this ) {

	CTimeSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"Time_CriticalSection::ObjectInitDone\" method" );

	Time_CS_TRY {
		d = (CTimeSectData*) _this->data;
		if ( 0x800000000l > GetVersion() ) { // it is a NT OS
			d->nt_kernel = LoadLibrary( WIN32LIB );
			if ( d->nt_kernel )
				d->try_enter = (BOOL (*)(LPCRITICAL_SECTION))GetProcAddress( d->nt_kernel, "TryEnterCriticalSection" );
		}
#ifdef _COLLECT_CALLERS
		error = CALL_SYS_ObjHeapAlloc(_this, &d->pStatData, STAT_DATA_SIZE * sizeof(CStatData));
#endif
	}
	Time_CS_EXCEPT;
	
	PR_TRACE_A1( _this, "Leave \"Time_CriticalSection::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call Time_CriticalSection_ObjectClose( hi_CriticalSection _this ) {
	
	CTimeSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"Time_CriticalSection::ObjectClose\" method" );
	
	Time_CS_TRY {

		d = (CTimeSectData*) _this->data;
		{
			char Output[0x100];
			wsprintf(Output, "\nPrague: Time_CriticalSection total wait time %I64d PerfCount %I64d\n\n", d->TotalTimeWait / gTimePerfomanceCounter, gTimePerfomanceCounter);
			OutputDebugString(Output);
		}
#ifdef _COLLECT_CALLERS		
		DropStatistic(_this);
#endif
		
		if ( d->nt_kernel )
			FreeLibrary( d->nt_kernel );
#ifdef _COLLECT_CALLERS
		if (d->pStatData)
			CALL_SYS_ObjHeapFree(_this, d->pStatData);
		memset( d, 0, sizeof(CTimeSectData) );
#endif
		error = errOK;
	}
	Time_CS_EXCEPT;
	{
	}
	
	PR_TRACE_A1( _this, "Leave \"Time_CriticalSection::ObjectClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Enter )
// Parameters are:
// tERROR pr_call Time_CriticalSection_Enter( hi_CriticalSection _this ) {
tERROR pr_call Time_CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
	CTimeSectData* d;
	tERROR error;
	
	unsigned __int64 TimeBefore;
	unsigned __int64 TimeAfter;
	
	PR_TRACE_A0( _this, "Enter \"Time_CriticalSection::Enter\" method" );
	
	
	Time_CS_TRY {
		d = (CTimeSectData*) _this->data;
		//if ( d->owned_event ) {
		QueryPerformanceCounter((LARGE_INTEGER*) &TimeBefore);
		
		EnterCriticalSection( &d->cs );
		
		QueryPerformanceCounter((LARGE_INTEGER*) &TimeAfter);
		
		d->TotalTimeWait += TimeAfter - TimeBefore;
#ifdef _COLLECT_CALLERS
		if (d->dwReenterCount++ == 0) // first enter
		{
			d->EnterCriticalSectionTime = TimeAfter;
			d->dwReenterMaxCount = 1;
			d->pStatData[d->dwStatDataUsed].dwCallerAddress = GetCallerAddress(2);
		}
		else
		{
			{
				char dbginfo[0x100];
				char output[MAX_PATH];
				wsprintf(output, "\n --- Reentering %#x --- count %d (%#x)\n", GetCallerAddress(2), d->dwReenterCount, PrDbgGetInfoEx);
				OutputDebugString(output);
				
				if (!PrDbgGetInfoEx)
				{
					OutputDebugString("PrDbgGetInfoEx is NULL\n");
				}
				else
				{
					if (PR_FAIL(PrDbgGetInfoEx(GetCallerAddress(2), NULL, 0, DBGINFO_SYMBOL_NAME, dbginfo, sizeof(dbginfo), NULL)))
					{
						OutputDebugString("PrDbgGetInfoEx failed!\n");
					}
					else
					{
						OutputDebugString("something...");
						OutputDebugString(dbginfo);
						OutputDebugString("\n");
					}
				}
				
			}

			if (d->dwReenterMaxCount < d->dwReenterCount)
				d->dwReenterMaxCount = d->dwReenterCount;
		}
#endif
		
		error = errOK;
	}
	Time_CS_EXCEPT;
	
	PR_TRACE_A1( _this, "Leave \"Time_CriticalSection::Enter\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Leave )
// Parameters are:
tERROR pr_call Time_CriticalSection_Leave( hi_CriticalSection _this ) {
	CTimeSectData* d;
	tERROR error;

#ifdef _COLLECT_CALLERS
	unsigned __int64 TimeBefore;
	int tmpIdx;
#endif

	PR_TRACE_A0( _this, "Enter \"Time_CriticalSection::Leave\" method" );

	Time_CS_TRY {
		d = (CTimeSectData*) _this->data;
#ifdef _COLLECT_CALLERS	
		tmpIdx = d->dwStatDataUsed;
		d->dwStatDataUsed++;
		QueryPerformanceCounter((LARGE_INTEGER*) &TimeBefore);

		if (d->dwReenterCount == 0)
			OutputDebugString("\n\n ------------ Leave with zero count ---------------- \n\n");
		else
		{
			if (--(d->dwReenterCount) == 0)
			{
				TimeBefore = TimeBefore - (d->EnterCriticalSectionTime);
				d->TotalTimeUnderCS += TimeBefore;
				
				d->pStatData[tmpIdx].dwReenterCount = d->dwReenterMaxCount;
				d->pStatData[tmpIdx].TimeUnderCS = TimeBefore;
				
				if (tmpIdx >= STAT_DATA_SIZE - 1) // buffer full
					DropStatistic(_this);
			}
		}
#endif		
		LeaveCriticalSection( &d->cs );
		
		error = errOK;
	}
	Time_CS_EXCEPT;
	
	PR_TRACE_A1( _this, "Leave \"Time_CriticalSection::Leave\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TryEnter )
// Parameters are:
// tERROR pr_call Time_CriticalSection_TryEnter( hi_CriticalSection _this ) {
tERROR pr_call Time_CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
	CTimeSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"Time_CriticalSection::TryEnter\" method" );
	
	Time_CS_TRY {
		d = (CTimeSectData*) _this->data;
		if ( d->try_enter && d->try_enter(&d->cs) )
			error = errOK;
		else
			error = errNOT_OK;
	}
	Time_CS_EXCEPT;
	
	PR_TRACE_A1( _this, "Leave \"Time_CriticalSection::TryEnter\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



