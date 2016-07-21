#include "stdafx.h"
#include "SysTime.h"

namespace KLUTIL
{

//////////////////////////////////////////////////////////////////////////
// CSysTime
//////////////////////////////////////////////////////////////////////////

CSysTime::CSysTime()
{
	SetInvalid();
}

CSysTime::CSysTime(const SYSTEMTIME& st)
{
	memcpy(&m_st, &st, sizeof(m_st));
}

CSysTime::CSysTime(const CSysTime& st)
{
	memcpy(&m_st, &st.m_st, sizeof(m_st));
}

CSysTime::CSysTime(const CLocTime& lt)
{
	memcpy(&m_st, &lt.m_st, sizeof(m_st));
	LocalTimeToUtcTime();
}

void CSysTime::SetTime(time_t t)
{
	if (t == 0 || t == -1 || t == INT_MAX)
	{
		SetInvalid();
		return;
	}
	__int64 newTime;
	if (t < 0)
	{
		newTime = 0x100000000I64 + __int64(t);
		newTime -= 0x7fffffffI64 + 0x2000000I64/* 512sec intervals beetween 1600 and 1970 years*/;
		newTime *= 512;
		newTime = newTime * FILETIME_IN_SECOND + 116444736000000000I64;
	}
	else
		newTime = __int64(t) * FILETIME_IN_SECOND + 116444736000000000I64;
	SetTime(newTime);
}

void CSysTime::SetTime(const FILETIME& ft)
{
	FileTimeToSystemTime(&ft, &m_st);
}

void CSysTime::SetTime(__int64 t)
{
	FILETIME ft;
	ft.dwLowDateTime = (DWORD)(t & 0xFFFFFFFF);
	ft.dwHighDateTime = (DWORD)(t >> 32);
	FileTimeToSystemTime(&ft, &m_st);
}

FILETIME CSysTime::GetFileTime() const
{
	FILETIME ft;
    memset(&ft, 0, sizeof ft);
	SystemTimeToFileTime(&m_st, &ft);
	return ft;
}

time_t CSysTime::GetTimeT(bool bUseBigYearTweak) const
{
	if (IsInvalid())
		return -1;
	
	__int64 ret = (GetInt64() - 116444736000000000I64) / FILETIME_IN_SECOND;

	if (bUseBigYearTweak)
	{
		if (ret < 100000 || ret > 0x7ff00000)
		{
			ret /= 512;
			ret += 0x7fffffffI64 + 0x2000000I64/* 512sec intervals beetween 1600 and 1970 years*/;
		}
	}
	else
	{
		// не будем обрезать на крайних значениях, оставим небольшую дельту
		// на случай всяких конвертаций, чтобы не выйти за допустимый диапозон.
		if (ret < 100000) // 01/02/1970 03:46:40
			ret = 100000;
		if (ret > 0x7ff00000) // 01/06/2038 23:57:52
			ret = 0x7ff00000;
	}

	return time_t(ret);
}

__int64 CSysTime::GetInt64() const
{
	__int64 res;
	FILETIME ft;
    memset(&ft, 0, sizeof ft);
	SystemTimeToFileTime(&m_st, &ft);
	res = (__int64(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
	return res;
}

void CSysTime::LocalTimeToUtcTime()
{
	FILETIME utcFt;
	LocalFileTimeToFileTime(&GetFileTime(), &utcFt);
	SetTime(utcFt);
}

void CSysTime::UtcTimeToLocalTime()
{
	FILETIME utcFt;
	FileTimeToLocalFileTime(&GetFileTime(), &utcFt);
	SetTime(utcFt);
}

void CSysTime::SetCurrentTime()
{
	GetSystemTime(&m_st);
}

CSysTime CSysTime::GetCurrentTime()
{
	CSysTime res;
	res.SetCurrentTime();
	return res;
}

void CSysTime::SetInvalid()
{
	memset(&m_st, 0, sizeof(m_st));
}

bool CSysTime::IsInvalid() const
{
	return *this == CSysTime();
}

bool CSysTime::IsValid() const
{
	return !IsInvalid();
}

CStdString CSysTime::GetDateTimeString(bool bShowLocal,
									   LPCTSTR pszFormat) const
{
	return bShowLocal ? CLocTime(*this).GetDateTimeString(bShowLocal, pszFormat) : GetDateTimeStringInt(pszFormat);
}

CStdString CSysTime::GetTimeString(bool bShowLocal,
								   bool bShowSec,
								   LPCTSTR pszFormat) const
{
	return bShowLocal ? CLocTime(*this).GetTimeString(bShowLocal, bShowSec, pszFormat) : GetTimeStringInt(bShowSec, pszFormat);
}

CStdString CSysTime::GetDateString(bool bShowLocal,
								   LPCTSTR pszFormat) const
{
	return bShowLocal ? CLocTime(*this).GetDateString(bShowLocal, pszFormat) : GetDateStringInt(pszFormat);
}

CStdString CSysTime::GetDateTimeStringInt(LPCTSTR pszFormat) const
{
	CStdString tm;
	int nSize = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_st, pszFormat, NULL, 0);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_st, pszFormat, tm.GetBuffer(nSize), nSize);
	tm.ReleaseBuffer();
	
	CStdString dt;
	nSize = GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_st, pszFormat, NULL, 0);
	GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_st, pszFormat, dt.GetBuffer(nSize), nSize);
	dt.ReleaseBuffer();
	
	return dt + _T(" ") + tm;
}

CStdString CSysTime::GetTimeStringInt(bool bShowSec, LPCTSTR pszFormat) const
{
	CStdString tm;
	int nSize = GetTimeFormat(LOCALE_USER_DEFAULT, bShowSec ? 0 : TIME_NOSECONDS, &m_st, pszFormat, NULL, 0);
	GetTimeFormat(LOCALE_USER_DEFAULT, bShowSec ? 0 : TIME_NOSECONDS, &m_st, pszFormat, tm.GetBuffer(nSize), nSize);
	tm.ReleaseBuffer();
	return tm;
}

CStdString CSysTime::GetDateStringInt(LPCTSTR pszFormat) const
{
	CStdString dt;
	int nSize = GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_st, pszFormat, NULL, 0);
	GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_st, pszFormat, dt.GetBuffer(nSize), nSize);
	dt.ReleaseBuffer();	
	return dt;
}

CSysTime& CSysTime::operator+=(const CTimeInterval& rhs)
{
	SetTime(GetInt64() + rhs.GetFull100NanoSec());
	return *this;
}

CSysTime& CSysTime::operator-=(const CTimeInterval& rhs)
{
	SetTime(GetInt64() - rhs.GetFull100NanoSec());
	return *this;
}

CSysTime operator+(const CSysTime& lhs, const CTimeInterval& rhs)
{
	CSysTime res(lhs);
	res += rhs;
	return res;
}

CSysTime operator-(const CSysTime& lhs, const CTimeInterval& rhs)
{
	CSysTime res(lhs);
	res -= rhs;
	return res;
}

CTimeInterval operator-(const CSysTime& lhs, const CSysTime& rhs)
{
	return CTimeInterval(lhs.GetInt64() - rhs.GetInt64());
}

bool CSysTime::operator==(const CSysTime& rhs) const
{
	return memcmp(&m_st, &rhs.m_st, sizeof(m_st)) == 0;
}

bool CSysTime::operator!=(const CSysTime& rhs) const
{
	return !operator==(rhs);
}

CSysTime& CSysTime::operator=(const CSysTime& rhs)
{
	memcpy(&m_st, &rhs.m_st, sizeof(m_st));
	return *this;
}

bool CSysTime::operator<(const CSysTime& rhs) const
{
	return GetInt64() < rhs.GetInt64();
}

bool CSysTime::operator<=(const CSysTime& rhs) const
{
	return GetInt64() <= rhs.GetInt64();
}

bool CSysTime::operator>(const CSysTime& rhs) const
{
	return GetInt64() > rhs.GetInt64();
}

bool CSysTime::operator>=(const CSysTime& rhs) const
{
	return GetInt64() >= rhs.GetInt64();
}

void CSysTime::AdjustWeekDay()
{
	SetTime(GetFileTime());
}

void CSysTime::CopyTo(SYSTEMTIME& st) const
{
	memcpy(&st, &m_st, sizeof(st));
}

//////////////////////////////////////////////////////////////////////////
// CLocTime
//////////////////////////////////////////////////////////////////////////

CLocTime::CLocTime()
{
}

CLocTime::CLocTime(const CLocTime& st)
	:	CSysTime(st.m_st)
{
}

CLocTime::CLocTime(const CSysTime& st)
	:	CSysTime(st)
{
	UtcTimeToLocalTime();
}

void CLocTime::SetLocalTime(SYSTEMTIME& locTime)
{
	memcpy(&m_st, &locTime, sizeof(m_st));
}
	
CLocTime CLocTime::GetCurrentTime()
{
	return CLocTime(CSysTime::GetCurrentTime());
}

CStdString CLocTime::GetDateTimeString(bool bShowLocal, LPCTSTR pszFormat) const
{
	return !bShowLocal ? CSysTime(*this).GetDateTimeString(bShowLocal, pszFormat) : GetDateTimeStringInt(pszFormat);
}

CStdString CLocTime::GetTimeString(bool bShowLocal, bool bShowSec, LPCTSTR pszFormat) const
{
	return !bShowLocal ? CSysTime(*this).GetTimeString(bShowLocal, bShowSec, pszFormat) : GetTimeStringInt(bShowSec, pszFormat);
}

CStdString CLocTime::GetDateString(bool bShowLocal, LPCTSTR pszFormat) const
{
	return !bShowLocal ? CSysTime(*this).GetDateString(bShowLocal, pszFormat) : GetDateStringInt(pszFormat);
}

} // namespace KLUTIL
