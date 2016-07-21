// kernel time library
// linear date/time, starting from 01.01.0001 00:00:00.00000000, 10 ns/interval
// all parameters with type tDT* and 'const' qualifier can be DT_NULL, it means NULL time (01.01.0001 00:00:00)

#define IMPEX_CURRENT_HEADER  "e_ktime.h"
#include <Prague/iface/impexhlp.h>
	
#if !defined(__e_ktime_h) || defined(IMPEX_TABLE_CONTENT)
#if ( IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DECLARE )
#define __e_ktime_h
	
	#define DT_NULL (tDT)(0)

	// linear date/time, starting from 01.01.0001 00:00:00.00000000, 10 ns/interval

	typedef tDWORD tDT_FORMAT;

	/* compare codes */
	#define cCOMPARE_LESS                  ((tINT)(-1))
	#define cCOMPARE_EQUIL                 ((tINT)(0))
	#define cCOMPARE_GREATER               ((tINT)(1))

	// defines to make declared fuction visible by Visual Assist
#define DTSet(pDT, dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwNs ) DTSet(pDT, dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwNs )
#define DTGet(pDT, pdwYear, pdwMonth, pdwDay, pdwHour, pdwMinute, pdwSecond, pdwNs ) DTGet(pDT, pdwYear, pdwMonth, pdwDay, pdwHour, pdwMinute, pdwSecond, pdwNs )
#define DTCmp(pDT1, pDT2) DTCmp(pDT1, pDT2)
#define DTCpy(pDT_Dest, pDT_Src) DTCpy(pDT_Dest, pDT_Src)
#define DTWeekDay(pDT, bSunday) DTWeekDay(pDT, bSunday)
#define DTWeekNo(pDT, bSunday) DTWeekNo(pDT, bSunday)
#define DTDayNo(pDT) DTDayNo(pDT)
#define DTIntervalShift(pDT, qwIntervals, qwNs) DTIntervalShift(pDT, qwIntervals, qwNs)
#define DTIntervalGet(pDT1, pDT2, pqwIntervals, qwNs) DTIntervalGet(pDT1, pDT2, pqwIntervals, qwNs)
#define DTTimeShift(pDT, iHour, iMinute, iSecond) DTTimeShift(pDT, iHour, iMinute, iSecond)
#define DTDateShift(pDT, iYear, iMonth, iDay) DTDateShift(pDT, iYear, iMonth, iDay)
#define DTDiffGet(pDT1, pDT2, pdwYear, pdwMonth, pdwDay, pdwHour, pdwMinute, pdwSecond) DTDiffGet(pDT1, pDT2, pdwYear, pdwMonth, pdwDay, pdwHour, pdwMinute, pdwSecond)
#endif
	
	IMPEX_BEGIN
	
		// base functions
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x0e262b4fl, tERROR, DTSet,           (       tDT *dt, tDWORD year, tDWORD month, tDWORD day, tDWORD hour, tDWORD minute, tDWORD second, tDWORD ns ) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x150920e3l, tERROR, DTGet,           ( const tDT *dt, tDWORD *year, tDWORD *month, tDWORD *day, tDWORD *hour, tDWORD *minute, tDWORD *second, tDWORD *ns) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0xddb4c62el, tINT  , DTCmp,           ( const tDT *dt1, const tDT *dt2) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x5b041296l, tERROR, DTCpy,           (       tDT *dest, const tDT *src) )
		
		// information functions
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x9509cc14l, tUINT, DTWeekDay,        ( const tDT *dt, tBOOL sunday ) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0xa4bb231al, tUINT, DTWeekNo,         ( const tDT *dt, tBOOL sunday ) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x7c09e787l, tUINT, DTDayNo,          ( const tDT *dt ) )
		
		// conversion functions
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x6fc88b07l, tERROR, DTIntervalShift, (       tDT *dt, tLONGLONG intervals, tQWORD ns) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x534b7b79l, tERROR, DTIntervalGet,   ( const tDT *dt1, const tDT *dt2, tLONGLONG *intervals, tQWORD ns) )
		
		// relational operations
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x19ac2fd1l, tERROR, DTDiffGet,       ( const tDT *dt1, const tDT *dt2, tDWORD *year, tDWORD *month, tDWORD *day, tDWORD *hour, tDWORD *minute, tDWORD *second) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0x4d4306c9l, tERROR, DTTimeShift,     (       tDT *dt, tINT hour, tINT minute, tINT second) )
		IMPEX_NAME_ID( ECLSID_KERNEL, 0xc0208610l, tERROR, DTDateShift,     (       tDT *dt, tINT year, tINT month, tINT day) )
	
	IMPEX_END
	
#endif
