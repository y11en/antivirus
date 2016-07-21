#if !defined( __pr_time_h )
#define __pr_time_h

#if defined(__cplusplus)

#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_root.h>

#include <time.h>
#include <memory.h>


#if defined(ITS_LOADER)
	inline bool _check_f() { return true; }
	#define CF( name, id )  _check_f()
	#define CF_LDR( name, id ) _check_f()
	#define CF_KRN( name, id ) _check_f()
#else
//	#if !defined(IID_ROOT)
//		#if !defined(_USE_VTBL)
//			struct cRoot;
//			typedef cRoot* hROOT;
//		#else
//			struct tag_hROOT {
//				const iRootVtbl*   vtbl; // pointer to the "Root" virtual table
//				const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
//			};
//		#endif 
//	#endif

	extern "C" hROOT g_root;
	inline bool _check_f( tVOID** func, tUINT id, tUINT cls_id ) {
		if ( !*func )
			::g_root->ResolveImportFunc( (tFUNC_PTR*)func, cls_id, id, ((tPID)(2)) );
		return 0 != *func;
	}
	#define CF( name, id )     _check_f((tVOID**)&name,id,ECLSID_LOADER)
	#define CF_LDR( name, id ) _check_f((tVOID**)&name,id,ECLSID_LOADER)
	#define CF_KRN( name, id ) _check_f((tVOID**)&name,id,ECLSID_KERNEL)
#endif

#define  _TQW(dt)  (*(tQWORD*)&(dt))

#define  DATETIME_TIME_T_BASE        LONG_LONG_CONST(0x563aff9bad170000)
#define  DATETIME_FILETIME_BASE      LONG_LONG_CONST(0x46120ce758a60000)

#define  TIMET_TO_DATETIME(p_timet)  (DATETIME_TIME_T_BASE + ((tQWORD)(p_timet) * (100000000)))
#define  DATETIME_TO_TIMET(p_dt)     ((time_t)((_TQW(p_dt) - DATETIME_TIME_T_BASE) / (100000000)))

#define  FILETIME_TO_DATETIME(p_ft)  (DATETIME_FILETIME_BASE + (_TQW(p_ft) * (10)))
#define  DATETIME_TO_FILETIME(p_dt)  ((_TQW(p_dt) - DATETIME_FILETIME_BASE) / (10))

#define  FILETIME_TO_TIMET(p_ft)     ((time_t)((FILETIME_TO_DATETIME(p_ft) - DATETIME_TIME_T_BASE) / (100000000)))
#define  TIMET_TO_FILETIME(p_timet)  ((TIMET_TO_DATETIME(p_timet) - DATETIME_FILETIME_BASE) / (10))

// ---
// cDateTime class
struct cDateTime {
	public:
		typedef enum enInitType {
			zero,
			current_utc,
			current_local,
		} tInitType;

		typedef enum enTimeZone {
			tz_local,
			tz_utc,
			tz_0,
			tz_1,
		} tTimeZone;

	public:
		cDateTime( tInitType init = current_utc )     { Init(init); }
		cDateTime( const tDATETIME* p_dt )            { if (p_dt) _TQW(qw) = _TQW(*p_dt); else Init(zero); }
		cDateTime( const cDateTime& p_dt )            { _TQW(qw) = _TQW(p_dt.qw); }

		static cDateTime nil()                        { return cDateTime(zero); }
		static cDateTime now_utc()                    { return cDateTime(current_utc); }
		static cDateTime now_local()                  { return cDateTime(current_local); }

		cDateTime& operator = ( const tDATETIME* p_dt ) { if (!p_dt) return Init(zero); _TQW(qw) = _TQW(p_dt); return *this; }
		cDateTime& operator = ( const cDateTime& p_dt ) { _TQW(qw) = _TQW(p_dt.qw); return *this; }

		cDateTime( time_t p_time )                    { _TQW(qw) = TIMET_TO_DATETIME(p_time); }
		cDateTime& operator = ( time_t p_time )       { _TQW(qw) = TIMET_TO_DATETIME(p_time); return *this; }
		operator time_t()                       const { return DATETIME_TO_TIMET(qw); }

		operator const tDATETIME& ()            const { return qw; }
		operator tDATETIME* ()                        { return &qw; }
		operator const tDATETIME* ()            const { return &qw; }

	protected:
		cDateTime  operator +  ( const cDateTime& o ) const;
		cDateTime  operator -  ( const cDateTime& o ) const;
		
		cDateTime& operator += ( const cDateTime& o );
		cDateTime& operator -= ( const cDateTime& o );

		cDateTime& operator += ( time_t o ) { return *this += cDateTime(o); }
		cDateTime& operator -= ( time_t o ) { return *this -= cDateTime(o); }

	public:
		bool operator == ( const cDateTime& p_dt ) const { return _TQW(qw) == _TQW(p_dt.qw); }
		bool operator != ( const cDateTime& p_dt ) const { return _TQW(qw) != _TQW(p_dt.qw); }
		bool operator <  ( const cDateTime& p_dt ) const { return _TQW(qw) <  _TQW(p_dt.qw); }
		bool operator <= ( const cDateTime& p_dt ) const { return _TQW(qw) <= _TQW(p_dt.qw); }
		bool operator >  ( const cDateTime& p_dt ) const { return _TQW(qw) >  _TQW(p_dt.qw); }
		bool operator >= ( const cDateTime& p_dt ) const { return _TQW(qw) >= _TQW(p_dt.qw); }

		cDateTime& Init( tInitType init = current_utc );
		cDateTime& MakeCurrentUTC()                         { return Init(current_utc); }

		cDateTime& SetUTC( tDWORD dwYear, tDWORD dwMonth, tDWORD dwDay, tDWORD dwHour, tDWORD dwMinute, tDWORD dwSecond, tDWORD dwNs = 0);

		tERROR     GetUTC( tDWORD *pdwYear, tDWORD *pdwMonth = 0, tDWORD *pdwDay = 0, tDWORD *pdwHour = 0, tDWORD *pdwMinute = 0, tDWORD *pdwSecond = 0, tDWORD *pdwNs = 0) const;

		tUINT      WeekDay( tTimeZone tz, tBOOL bSunday ) const;
		tUINT      WeekNo ( tTimeZone tz, tBOOL bSunday ) const;
		tUINT      DayNo  ( tTimeZone tz ) const;
		
		cDateTime& IntervalShift ( tQWORD qwIntervals, tQWORD one_slice_in_ns );
		tERROR     IntervalGet   ( const cDateTime& p_dt, tLONGLONG* pqwIntervals, tQWORD one_slice_in_ns ) const;
		
		tLONGLONG  Diff          ( const cDateTime* other, tQWORD one_slice_in_ns = 1000000 ) const; // other can be NULL, if one_slice_in_ns == one_slice_in_ns, result in milliseconds
		tERROR     DiffGet       ( const cDateTime& p_dt, tDWORD *pdwYear, tDWORD *pdwMonth = 0, tDWORD *pdwDay = 0, tDWORD *pdwHour = 0, tDWORD *pdwMinute = 0, tDWORD *pdwSecond = 0 ) const;
		cDateTime& TimeShift     ( tINT nHour, tINT nMinute, tINT nSecond );
		cDateTime& DateShift     ( tINT nYear, tINT nMonth,  tINT nDay );

		cDateTime  diff          ( const cDateTime& o ) const;

		void CopyTo(tDATETIME& dst) {
			memcpy( &dst, &qw, sizeof(tDATETIME) );
		}

	protected:
		tDATETIME qw;
};




// ---
inline cDateTime cDateTime::operator + ( const cDateTime& o ) const {
	cDateTime result( cDateTime::zero );
	if ( CF_KRN(DTIntervalGet,0x534b7b79l) && CF_KRN(DTIntervalShift,0x6fc88b07l) ) {
		tLONGLONG int1 = 0;
		tLONGLONG int2 = 0;
		DTIntervalGet( &qw, 0, &int1, 10 );
		DTIntervalGet( o, 0, &int2, 10 );
		int1 += int2;
		DTIntervalShift( result, int1, 10 );
	}
	return result;
}

// ---
inline cDateTime cDateTime::operator - ( const cDateTime& o ) const {
	return diff(o);
}

// ---
inline cDateTime& cDateTime::operator += ( const cDateTime& o ) {
	if ( CF_KRN(DTIntervalGet,0x534b7b79l) && CF_KRN(DTIntervalShift,0x6fc88b07l) ) {
		tLONGLONG intervals = 0;
		DTIntervalGet( o, 0, &intervals, 10 );
		DTIntervalShift( &qw, intervals, 10 );
	}
	return *this;
}

// ---
inline cDateTime& cDateTime::operator -= ( const cDateTime& o ) {
	if ( CF_KRN(DTIntervalGet,0x534b7b79l) && CF_KRN(DTIntervalShift,0x6fc88b07l) ) {
		tLONGLONG intervals = 0;
		DTIntervalGet( o, 0, &intervals, 10 );
		DTIntervalShift( &qw, -intervals, 10 );
	}
	return *this;
}


// ---
inline cDateTime cDateTime::diff( const cDateTime& o ) const {
	cDateTime res( zero );
	if ( CF_KRN(DTIntervalGet,0x534b7b79l) && CF_KRN(DTIntervalShift,0x6fc88b07l) ) {
		tLONGLONG intervals = 0;
		DTIntervalGet( &qw, o, &intervals, 10 );
		DTIntervalShift( res, intervals, 10 );
	}
	return res;
}


// ---
inline cDateTime& cDateTime::Init( tInitType init ) { 
	switch ( init ) {
		case zero          : _TQW(qw) = 0; break;
		case current_utc   : if ( CF_LDR(::NowGreenwich,0x3a5a2bbel) ) ::NowGreenwich( *this ); break;
		case current_local : if ( CF_LDR(::Now,0xa941ea2fl) ) ::Now( *this ); break;
		default            : _TQW(qw) = 0; break;
	}
	return *this;
}

inline cDateTime& cDateTime::SetUTC( tDWORD dwYear, tDWORD dwMonth, tDWORD dwDay, tDWORD dwHour, tDWORD dwMinute, tDWORD dwSecond, tDWORD dwNs ) {
	if ( CF_KRN(::DTSet,0x0e262b4fl) )
		::DTSet( *this, dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwNs );
	return *this;
}

inline tERROR cDateTime::GetUTC( tDWORD *pdwYear, tDWORD *pdwMonth, tDWORD *pdwDay, tDWORD *pdwHour, tDWORD *pdwMinute, tDWORD *pdwSecond, tDWORD *pdwNs ) const {
	if ( CF_KRN(::DTGet,0x150920e3l) )
		return ::DTGet( *this, pdwYear, pdwMonth, pdwDay, pdwHour, pdwMinute, pdwSecond, pdwNs );
	return errNOT_FOUND;
}

inline tUINT cDateTime::WeekDay( tTimeZone tz, tBOOL bSunday ) const {
	if ( tz != tz_utc )
	{
		PR_RPT(("cDateTime::WeekDay not implemented for this time zone"));
		return (tUINT)-1;
	}
	if ( CF_KRN(::DTWeekDay,0x9509cc14l) )
		return ::DTWeekDay( *this, bSunday );
	return (tUINT)-1;
}

inline tUINT cDateTime::WeekNo( tTimeZone tz, tBOOL bSunday ) const {
	if ( tz != tz_utc )
	{
		PR_RPT(("cDateTime::WeekNo not implemented for this time zone"));
		return (tUINT)-1;
	}
	if ( CF_KRN(::DTWeekNo,0xa4bb231al) )
		return ::DTWeekNo( *this, bSunday );
	return (tUINT)-1;
}


inline tUINT cDateTime::DayNo( tTimeZone tz ) const {
	if ( tz != tz_utc )
	{
		PR_RPT(("cDateTime::DayNo not implemented for this time zone"));
		return (tUINT)-1;
	}
	if ( CF_KRN(::DTDayNo,0x7c09e787l) )
		return ::DTDayNo( *this );
	return (tUINT)-1;
}

inline cDateTime& cDateTime::IntervalShift( tQWORD qwIntervals, tQWORD one_slice_in_ns ) {
	if ( CF_KRN(::DTIntervalShift,0x6fc88b07l) )
		::DTIntervalShift( *this, qwIntervals, one_slice_in_ns );
	return *this;
}

inline tERROR cDateTime::IntervalGet( const cDateTime& p_dt, tLONGLONG* pqwIntervals, tQWORD one_slice_in_ns ) const {
	if ( CF_KRN(::DTIntervalGet,0x534b7b79l) )
		return ::DTIntervalGet( *this, p_dt, pqwIntervals, one_slice_in_ns );
	return errNOT_FOUND;
}

// ---
inline tLONGLONG cDateTime::Diff( const cDateTime* other, tQWORD one_slice_in_ns ) const {
	cDateTime nil( zero );
	tLONGLONG interv;
	if ( !other )
		other = &nil;
	if ( CF_KRN(::DTIntervalGet,0x534b7b79l) && PR_SUCC(::DTIntervalGet(*this,*other,&interv,one_slice_in_ns)) )
		return interv;
	return -1;
}

inline tERROR cDateTime::DiffGet( const cDateTime& p_dt, tDWORD *pdwYear, tDWORD *pdwMonth, tDWORD *pdwDay, tDWORD *pdwHour, tDWORD *pdwMinute, tDWORD *pdwSecond ) const {
	if ( CF_KRN(::DTDiffGet,0x19ac2fd1l) )
		return ::DTDiffGet( *this, p_dt, pdwYear, pdwMonth, pdwDay, pdwHour, pdwMinute, pdwSecond );
	return errNOT_FOUND;
}

inline cDateTime& cDateTime::TimeShift( tINT nHour, tINT nMinute, tINT nSecond ) {
	if ( CF_KRN(::DTTimeShift,0x4d4306c9l) )
		::DTTimeShift( *this, nHour, nMinute, nSecond );
	return *this;
}

inline cDateTime& cDateTime::DateShift( tINT nYear, tINT nMonth,  tINT nDay ) {
	if ( CF_KRN(::DTDateShift,0xc0208610l) )
		::DTDateShift( *this, nYear, nMonth, nDay );
	return *this;
}

#endif // __cplusplus

#endif // __pr_time_h
