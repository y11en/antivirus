#ifndef __USERSETTINGS_SCHEDULE_H__
#define __USERSETTINGS_SCHEDULE_H__

#define SS_NO_CONVERSION
#define SS_NO_BSTR
#include "DEFS.H"
#include "BaseMarshal.h"
#include "../../../CommonFiles/KLUtil/inl/tmutil.h"
#include <time.h>

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information

namespace Private
{
	// helper serialization operator for SYSTEMTIME type
	inline PoliType &operator<<(PoliType &pt, SYSTEMTIME &st)
	{
		return pt << st.wYear << st.wMonth << st.wDay << st.wDayOfWeek
			<< st.wHour << st.wMinute << st.wSecond << st.wMilliseconds;
	}

	//******************************************************************************
	// CScheduleSettings_v1
	//******************************************************************************

	struct CCIFACEEXPORT CScheduleSettings_v1 : public CCBaseMarshal<PID_USER_SETTINGS_SCHEDULE>
	{
		enum EScanInterval
		{
			eEveryHour = 0,
			eEveryDay = 1,
			eEveryWeek = 2,
			eDoNotScanObsoleted = 3,
			/// on application startup
			eOnStartup = 4,
			// запуск вручную (значение введено для совместимости с AK расписанием)
			// GUI должен показывать, что расписание отключено, как бы если m_bUseSchedule == false
			eManually = 5
		};

		struct CCIFACEEXPORT WeekDays
		{
			WeekDays();
			bool operator==(const WeekDays &)const;

			bool bMonday;
			bool bTuesday;
			bool bWednesday;
			bool bThursday;
			bool bFriday;
			bool bSaturday;
			bool bSunday;
		};


		virtual PoliType &MarshalImpl(PoliType &);
		
		EScanInterval m_eScanInterval;
		int m_nFrequent;
		CSysTimeSer m_tmStartTimeDeprecated;
		WeekDays m_WeekDays;
	};

	IMPLEMENT_MARSHAL_ENUM(CScheduleSettings_v1::EScanInterval);

	inline CScheduleSettings_v1::WeekDays::WeekDays()
	 : bMonday(false), bTuesday(false), bWednesday(false), bThursday(false),
	   bFriday(false), bSaturday(false), bSunday(false)
	{
	}
	inline bool CScheduleSettings_v1::WeekDays::operator==(const WeekDays &left)const
	{
		return bMonday == left.bMonday && bTuesday == left.bTuesday
			&& bWednesday == left.bWednesday && bThursday == left.bThursday
			&& bFriday == left.bFriday && bSaturday == left.bSaturday
			&& bSunday == left.bSunday;
	}

	inline PoliType &operator<<(PoliType &pt, CScheduleSettings_v1::WeekDays &a)
	{
		return pt << a.bMonday << a.bTuesday << a.bWednesday
			<< a.bThursday << a.bFriday << a.bSaturday << a.bSunday;
	}

	inline PoliType & CScheduleSettings_v1::MarshalImpl(PoliType& pt)
	{
		return BaseMarshal::MarshalImpl(pt) << m_eScanInterval << m_nFrequent << m_WeekDays << m_tmStartTimeDeprecated;
	}

	IMPLEMENT_MARSHAL(CScheduleSettings_v1);

	//******************************************************************************
	// CScheduleSettings_v2
	//******************************************************************************

	struct CCIFACEEXPORT CScheduleSettings_v2 : public DeclareNextVersion<CScheduleSettings_v1>
	{
		virtual PoliType & MarshalImpl(PoliType &);

		SYSTEMTIME m_stLocalStartTime;
	};

	
	inline PoliType &CScheduleSettings_v2::MarshalImpl(PoliType &pt)
	{
		// сериализуем только базовую часть
		BaseMarshal::MarshalImpl(pt);

		if (pt.is_read_from_pol_ () && GetDataVersion() < CurrentVersion)
		{
			KLUTIL::CLocTime(m_tmStartTimeDeprecated).CopyTo(m_stLocalStartTime);
		}
		else
			pt << m_stLocalStartTime;

		return pt;
	};

	IMPLEMENT_MARSHAL(CScheduleSettings_v2);


	//******************************************************************************
	// CScheduleSettings_v3
	//******************************************************************************

	struct CCIFACEEXPORT CScheduleSettings_v3 : public DeclareNextVersion<CScheduleSettings_v2>
	{
		virtual PoliType & MarshalImpl(PoliType& pt);
	};

	inline PoliType & CScheduleSettings_v3::MarshalImpl(PoliType &pt)
	{
		bool m_bUseMinBatteryChargePercentDeprecated = false;
		int m_nMinBatteryChargePercentDeprecated = 30;
		return BaseMarshal::MarshalImpl(pt) << m_bUseMinBatteryChargePercentDeprecated << m_nMinBatteryChargePercentDeprecated;
	};

	IMPLEMENT_MARSHAL(CScheduleSettings_v3);

	//******************************************************************************
	// CScheduleSettings_v4
	//******************************************************************************

	struct CCIFACEEXPORT CScheduleSettings_v4 : public DeclareNextVersion<CScheduleSettings_v3>
	{
		virtual PoliType &MarshalImpl(PoliType &);

		// shcedule usage flag
		bool m_bUseSchedule;
		// Internet presence flag (вычисляется в сервисе)
		bool m_bUseNetDetect;
	};


	IMPLEMENT_MARSHAL(CScheduleSettings_v4);

	inline PoliType &CScheduleSettings_v4::MarshalImpl(PoliType &pt)
	{
		BaseMarshal::MarshalImpl(pt);
		if(pt.is_read_from_pol_() && GetDataVersion() < CurrentVersion)
		{
			m_bUseSchedule = m_eScanInterval != eDoNotScanObsoleted;
			if(m_eScanInterval == eDoNotScanObsoleted)
				m_eScanInterval = eEveryDay;
		}
		else
			pt << m_bUseSchedule;

		return pt << m_bUseNetDetect;
	}

	//******************************************************************************
	// CScheduleSettings_v5
	//******************************************************************************

	struct CCIFACEEXPORT CScheduleSettings_v5 : public DeclareNextVersion<CScheduleSettings_v4>
	{
		virtual PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt)
						<< m_randomOffsetSeconds
						<< m_bDeferredSchedule
						<< m_bAskUser
						;
		}

		/// specifies random offset to execute task.
		///  This parameter influence task schedule calculation by appending random
		/// offset from given time. In case of 0 seconds offset the task will be executed
		/// in exact given time with no random offset.
		unsigned m_randomOffsetSeconds;

		// запускать ли с просроченным расписанием
		bool m_bDeferredSchedule;

		// спрашивать пользователя перед запуском
		bool m_bAskUser;
	};

	//******************************************************************************
	// CScheduleSettings_v6
	//******************************************************************************

	struct CCIFACEEXPORT CScheduleSettings_v6 : public DeclareNextVersion<CScheduleSettings_v5>
	{
		virtual PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt)
						<< m_bUseTaskMaxRunningTime
						<< m_nTaskMaxRunningTimeMilliseconds
						;
		}

		// максимальное время задачи
		bool m_bUseTaskMaxRunningTime;
		unsigned m_nTaskMaxRunningTimeMilliseconds;
	};
}

struct CScheduleSettings: public DeclareExternalName<Private::CScheduleSettings_v6>
{
	CScheduleSettings()
	{
		m_eScanInterval = eEveryWeek;
		m_nFrequent = 1;
		m_WeekDays.bFriday = true;
		
		KLUTIL::CLocTime::GetCurrentTime().CopyTo(m_stLocalStartTime);
		m_stLocalStartTime.wHour = 20;
		m_stLocalStartTime.wMinute = m_stLocalStartTime.wSecond
			= m_stLocalStartTime.wMilliseconds = 0;
		
		m_bUseSchedule = true;
		m_bUseNetDetect = false;
		m_randomOffsetSeconds = 0;
		m_bDeferredSchedule = false;
		m_bAskUser = false;

		m_bUseTaskMaxRunningTime = false;
		m_nTaskMaxRunningTimeMilliseconds = -1;
	}

	bool operator==(const CScheduleSettings& rhs) const
	{
		const bool bTmIsEqual = memcmp(&m_stLocalStartTime, &rhs.m_stLocalStartTime, sizeof(m_stLocalStartTime)) == 0;
		bool bIntervalEq = true;
		if (m_eScanInterval == eEveryHour || m_eScanInterval == eEveryDay)
			bIntervalEq = (m_nFrequent == rhs.m_nFrequent) && bTmIsEqual;
		else if (m_eScanInterval == eEveryWeek)
			bIntervalEq = (m_WeekDays == rhs.m_WeekDays) && bTmIsEqual;
		
		return m_eScanInterval == rhs.m_eScanInterval
			&& bIntervalEq
			&& m_bUseSchedule == rhs.m_bUseSchedule
			&& m_bUseNetDetect == rhs.m_bUseNetDetect
			&& m_randomOffsetSeconds == rhs.m_randomOffsetSeconds
			&& m_bDeferredSchedule == rhs.m_bDeferredSchedule
			&& m_bAskUser == rhs.m_bAskUser
			&& m_bUseTaskMaxRunningTime == rhs.m_bUseTaskMaxRunningTime
			&& (!m_bUseTaskMaxRunningTime || m_nTaskMaxRunningTimeMilliseconds == rhs.m_nTaskMaxRunningTimeMilliseconds)
			;
	}
	bool operator!=(const CScheduleSettings& rhs) const { return !operator==(rhs); }
};

#endif // __USERSETTINGS_SCHEDULE_H__