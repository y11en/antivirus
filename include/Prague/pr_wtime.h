#if defined( _WINBASE_ )

#if !defined( __pr_wtime_h )
#define __pr_wtime_h

#include <Prague/pr_time.h>

struct cWinDateTime : public cDateTime {
	cWinDateTime( tInitType init = current_utc ) : cDateTime(init) {}
	cWinDateTime( const cDateTime& p_dt )      : cDateTime(p_dt) {}

	cWinDateTime( const FILETIME& p_ft ) : cDateTime(zero) { *(tQWORD*)&qw = (DATETIME_FILETIME_BASE + (*(tQWORD*)&p_ft * 10)); }
	cWinDateTime& operator = ( const FILETIME& p_ft )                    { *(tQWORD*)&qw = (DATETIME_FILETIME_BASE + (*(tQWORD*)&p_ft * 10)); return *this; }
	operator FILETIME() const	{ 
		tQWORD tmp = (*(tQWORD*)&qw - DATETIME_FILETIME_BASE) / 10; 
		return *(FILETIME*)&tmp; 
	}
	
	cWinDateTime( const SYSTEMTIME& p_st ) { SetUTC(p_st.wYear,p_st.wMonth,p_st.wDay,p_st.wHour,p_st.wMinute,p_st.wSecond,p_st.wMilliseconds*1000000); }
	cWinDateTime& operator = ( const SYSTEMTIME& p_st )  { SetUTC(p_st.wYear,p_st.wMonth,p_st.wDay,p_st.wHour,p_st.wMinute,p_st.wSecond,p_st.wMilliseconds*1000000); return*this; }
	operator SYSTEMTIME() const;

};

inline cWinDateTime::operator SYSTEMTIME() const {
	SYSTEMTIME st;
	tDWORD dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwNs;
	GetUTC( &dwYear, &dwMonth, &dwDay, &dwHour, &dwMinute, &dwSecond, &dwNs );
	st.wYear = (tWORD)dwYear;
	st.wMonth = (tWORD)dwMonth;
	st.wDay = (tWORD)dwDay;
	st.wHour = (tWORD)dwHour;
	st.wMinute = (tWORD)dwMinute;
	st.wSecond = (tWORD)dwSecond;
	st.wMilliseconds = (tWORD)(dwNs / 1000000);
	return st;
}

#endif // __pr_wtime_h

#endif // _WINBASE_