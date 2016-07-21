#if !defined(AFX_SYSTIME_H__A08D81A1_A582_42E0_84DA_7EA131ADEFC4__INCLUDED_)
#define AFX_SYSTIME_H__A08D81A1_A582_42E0_84DA_7EA131ADEFC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winbase.h>
#include <time.h>
#include "../Stuff/StdString.h"
#include "defs.h"

namespace KLUTIL
{
	const __int64 FILETIME_IN_SECOND = 10000000I64;

	class KLUTIL_API CTimeInterval
	{
	public:
		CTimeInterval(): m_Interval(0) {}
		explicit CTimeInterval(__int64 n100NanoSec): m_Interval(n100NanoSec) {}
		void Set(int nDays, int nHours = 0, int nMin = 0, int nSec = 0)
		{
			m_Interval = 
				FILETIME_IN_SECOND * (((nDays * 24 + nHours) * 60 + nMin) * 60 + nSec);
		}
		void Set100NanoSec(__int64 t) { m_Interval = t; }
		int GetFullDays() const { return int(m_Interval / (FILETIME_IN_SECOND*60*60*24)); }
		int GetFullHours() const { return int(m_Interval / (FILETIME_IN_SECOND*60*60)); }
		int GetFullMinutes() const { return int(m_Interval / (FILETIME_IN_SECOND*60)); }
		int GetFullSeconds() const { return int(m_Interval / FILETIME_IN_SECOND); }
		__int64 GetFull100NanoSec() const { return m_Interval; }
	protected:
		CTimeInterval(int nDays, int nHours, int nMin, int nSec)
			: m_Interval(FILETIME_IN_SECOND * (((nDays * 24 + nHours) * 60 + nMin) * 60 + nSec))
		{
		}
	private:
		__int64 m_Interval;
	};

	class CDayInterval: public CTimeInterval
	{
	public:
		CDayInterval(int nDays): CTimeInterval(nDays, 0, 0, 0) {}
	};

	class CHourInterval: public CTimeInterval
	{
	public:
		CHourInterval(int nHours): CTimeInterval(0, nHours, 0, 0) {}
	};

	class CMinuteInterval: public CTimeInterval
	{
	public:
		CMinuteInterval(int nMinutes): CTimeInterval(0, 0, nMinutes, 0) {}
	};

	class CSecondInterval: public CTimeInterval
	{
	public:
		CSecondInterval(int nSeconds): CTimeInterval(0, 0, 0, nSeconds) {}
	};

	class KLUTIL_API CLocTime;

	class KLUTIL_API CSysTime
	{
	public:
		CSysTime();
		CSysTime(const SYSTEMTIME& st);
		CSysTime(const CSysTime& st);
		CSysTime(const CLocTime& lt);		
		void SetTime(time_t t);
		void SetTime(const FILETIME& ft);
		void SetTime(__int64 t);
		FILETIME GetFileTime() const;
		time_t GetTimeT(bool bUseBigYearTweak = false) const;
		__int64 GetInt64() const;
		void SetCurrentTime();
		static CSysTime GetCurrentTime();

		void SetInvalid();
		bool IsInvalid() const;
		bool IsValid() const;

		/*!
			If pszFormat is NULL, the function uses the time format of the user locale.
			Characters in the format string that are enclosed in single
			quotation marks will appear in the same location and unchanged
			in the output string.
			Picture	Meaning 
			h		Hours with no leading zero for single-digit hours; 12-hour clock. 
			hh		Hours with leading zero for single-digit hours; 12-hour clock. 
			H		Hours with no leading zero for single-digit hours; 24-hour clock. 
			HH		Hours with leading zero for single-digit hours; 24-hour clock. 
			m		Minutes with no leading zero for single-digit minutes. 
			mm		Minutes with leading zero for single-digit minutes. 
			s		Seconds with no leading zero for single-digit seconds. 
			ss		Seconds with leading zero for single-digit seconds. 
			t		One character time-marker string, such as A or P. 
			tt		Multicharacter time-marker string, such as AM or PM. 	
			
			For example, to get the time string "11:29:40 PM"
			use the following picture string: "hh':'mm':'ss tt"
		*/
		CStdString GetDateTimeString(
			bool bShowLocal = true,	//!< [in] ѕоказать локальное врем€ (подразумеваетс€, что внутри хранитс€ UTC врем€).
			LPCTSTR pszFormat = NULL
			) const;
		CStdString GetTimeString(
			bool bShowLocal = true,	//!< [in] ѕоказать локальное врем€ (подразумеваетс€, что внутри хранитс€ UTC врем€).
			bool bShowSec = true,	//!< [in] ѕоказать секунды.
			LPCTSTR pszFormat = NULL
			) const;
		CStdString GetDateString(
			bool bShowLocal = true,	//!< [in] ѕоказать локальное врем€ (подразумеваетс€, что внутри хранитс€ UTC врем€).
			LPCTSTR pszFormat = NULL
			) const;

		CSysTime& operator+=(const CTimeInterval& rhs);
		CSysTime& operator-=(const CTimeInterval& rhs);

		bool operator==(const CSysTime& rhs) const;
		bool operator!=(const CSysTime& rhs) const;
		CSysTime& operator=(const CSysTime& rhs);

		bool operator<(const CSysTime& rhs) const;
		bool operator<=(const CSysTime& rhs) const;
		bool operator>(const CSysTime& rhs) const;
		bool operator>=(const CSysTime& rhs) const;

		WORD GetYear() const { return m_st.wYear; }
		WORD GetMonth() const { return m_st.wMonth; }
		WORD GetDay() const { return m_st.wDay; }
		WORD GetHour() const { return m_st.wHour; }
		WORD GetMinute() const { return m_st.wMinute; }
		WORD GetSecond() const { return m_st.wSecond; }
		WORD GetMilliseconds() const { return m_st.wMilliseconds; }
		WORD GetDayOfWeek() const { return m_st.wDayOfWeek; }

		void SetYear(WORD n) { m_st.wYear = n; }
		void SetMonth(WORD n) { m_st.wMonth = n; }
		void SetDay(WORD n) { m_st.wDay = n; }
		void SetHour(WORD n) { m_st.wHour = n; }
		void SetMinute(WORD n) { m_st.wMinute = n; }
		void SetSecond(WORD n) { m_st.wSecond = n; }
		void SetMilliseconds(WORD n) { m_st.wMilliseconds = n; }
		void SetDayOfWeek(WORD n) { m_st.wDayOfWeek = n; }
		void AdjustWeekDay();

		void CopyTo(SYSTEMTIME& st) const;

		// ѕользоватьс€ только при крайней необходимости!
		SYSTEMTIME& GetInternalSystemTime() { return m_st; }

	protected:
		void LocalTimeToUtcTime();
		void UtcTimeToLocalTime();
		CStdString GetDateTimeStringInt(LPCTSTR pszFormat) const;
		CStdString GetTimeStringInt(bool bShowSec, LPCTSTR pszFormat) const;
		CStdString GetDateStringInt(LPCTSTR pszFormat) const;

		SYSTEMTIME m_st;
	};

	class KLUTIL_API CLocTime: public CSysTime
	{
		using CSysTime::SetTime;
		using CSysTime::GetFileTime;
		using CSysTime::GetTimeT;
		using CSysTime::GetInt64;
		using CSysTime::SetCurrentTime;

	public:
		CLocTime();
		CLocTime(const CSysTime& st);
		CLocTime(const CLocTime& st);

		void SetLocalTime(SYSTEMTIME& locTime);
		
		static CLocTime GetCurrentTime();

		CStdString GetDateTimeString(
			bool bShowLocal = true,	//!< [in] ѕоказать локальное врем€ (подразумеваетс€, что внутри хранитс€ UTC врем€).
			LPCTSTR pszFormat = NULL
			) const;
		CStdString GetTimeString(
			bool bShowLocal = true,	//!< [in] ѕоказать локальное врем€ (подразумеваетс€, что внутри хранитс€ UTC врем€).
			bool bShowSec = true,	//!< [in] ѕоказать секунды.
			LPCTSTR pszFormat = NULL
			) const;
		CStdString GetDateString(
			bool bShowLocal = true,	//!< [in] ѕоказать локальное врем€ (подразумеваетс€, что внутри хранитс€ UTC врем€).
			LPCTSTR pszFormat = NULL
			) const;
	};

	KLUTIL_API CTimeInterval operator-(const CSysTime& lhs, const CSysTime& rhs);
	KLUTIL_API CSysTime operator+(const CSysTime& lhs, const CTimeInterval& rhs);
	KLUTIL_API CSysTime operator-(const CSysTime& lhs, const CTimeInterval& rhs);
}

#endif // !defined(AFX_SYSTIME_H__A08D81A1_A582_42E0_84DA_7EA131ADEFC4__INCLUDED_)
