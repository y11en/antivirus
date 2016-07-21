#ifndef _SCHEDULE_HELPER
#define _SCHEDULE_HELPER

#pragma once

#include "DEFS.H"
#include "UserSettingsSchedule.h"
#include <vector>


namespace Private
{
	struct CCIFACEEXPORT CSchedulePoint_v1 : public CCBaseMarshal<PID_SCHEDULE_POINT>
	{
		__int64 m_tStart;
		__int64 m_tSpan;
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_tStart << m_tSpan;
		}
	};
	
	struct CCIFACEEXPORT CSchedulePoint_v2 : public DeclareNextVersion<CSchedulePoint_v1>
	{
		bool m_bNeedInternet;	//!< true - после наступления нужного времени, ждать появление Интернета

		CSchedulePoint_v2(): m_bNeedInternet(false) {}

		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_bNeedInternet;
		}
	};
}

typedef DeclareExternalName<Private::CSchedulePoint_v2> CSchedulePointSer;

struct CCIFACEEXPORT CSchedulePoint : public CSchedulePointSer
{
	CSchedulePoint() { m_tStart = 0I64; m_tSpan = 0I64; }
	CSchedulePoint(__int64 start, __int64 span, bool bUseNetDetect)
	{
		m_tStart = start; m_tSpan = span; m_bNeedInternet = bUseNetDetect;
	}
	__int64 GetNextStart (__int64 base_time) const;
	__int64 GetNearestTime (__int64 base_time) const;
	bool operator==(const CSchedulePoint& rhs) const
	{
		return m_tStart ==  rhs.m_tStart && m_tSpan == rhs.m_tSpan && m_bNeedInternet == rhs.m_bNeedInternet;
	}
};

CCIFACEEXPORT __int64 GetLocalTimeFT();

CCIFACEEXPORT bool CreateSchedule(const CScheduleSettings &schedule,
								  std::vector<CSchedulePoint> &Schedule);

CCIFACEEXPORT __int64 GetNextStart(__int64 ftLastExecute,
								   const std::vector<CSchedulePoint> &Schedule,
								   bool& bNeedInternet);

CCIFACEEXPORT void Int64ToSystemTime(__int64 file_time, SYSTEMTIME& sys_time);

CCIFACEEXPORT __int64 SystemTimeToInt64(const SYSTEMTIME& sys_time);

#endif //_SCHEDULE_HELPER