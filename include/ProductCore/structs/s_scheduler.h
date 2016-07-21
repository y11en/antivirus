#ifndef __S_SCHEDULE_H
#define __S_SCHEDULE_H

#include <ProductCore/plugin/p_scheduler.h>

//-----------------------------------------------------------------------------

struct tTIME_SHIFT
{
	tDWORD dwYears;
	tDWORD dwMonths;
	tDWORD dwTimeShiftDayOfMonth;
	tDWORD dwDays;
	tDWORD dwHours;
	tDWORD dwMinutes;
	tDWORD dwSeconds;
	tTIME_SHIFT() : dwYears(0), dwMonths(0), dwTimeShiftDayOfMonth(0), dwDays(0), dwHours(0), dwMinutes(0), dwSeconds(0) {}
};

struct cScheduleSettings : public cSerializable
{
    cScheduleSettings() :
		cSerializable(),
		m_dwScheduleClass(0),
		m_dwScheduleID(0),
		//m_dtNextRun( NULL ),
		m_bEnabled(cTRUE),
		m_bRaiseIfSkipped(cFALSE),
		m_TimeShift(),
		m_bRelativeTimeShift(cFALSE),
		
		m_bSunday(cFALSE),

		//m_dtAllowWindowStart( NULL ),
		//m_dtAllowWindowEnd( NULL ),
		
		m_AllowWindowTimeShift(),
		m_dwTimeStartDelta(0)
		{ 
			::memset(&m_dtFirstRun,0,sizeof(m_dtNextRun)); 
			::memset(&m_dtLastRun,0,sizeof(m_dtNextRun)); 
			::memset(&m_dtNextRun,0,sizeof(m_dtNextRun)); 
			::memset(&m_dtAllowWindowStart,0,sizeof(m_dtAllowWindowStart)); 
			::memset(&m_dtAllowWindowEnd,0,sizeof(m_dtAllowWindowEnd)); 
			::memset(&m_bWeekDays2Skip,0,sizeof(m_bWeekDays2Skip));
		}

	DECLARE_IID( cScheduleSettings, cSerializable, PID_SCHEDULER, 1 );

	tDWORD    m_dwScheduleClass;
	tDWORD    m_dwScheduleID;

	tDATETIME m_dtFirstRun;
	tDATETIME m_dtLastRun;
	tDATETIME m_dtNextRun;

	tBOOL     m_bEnabled;
	tBOOL     m_bRaiseIfSkipped;

	tTIME_SHIFT m_TimeShift;
	tBOOL     m_bRelativeTimeShift;

	tBOOL     m_bWeekDays2Skip[7];
	tBOOL     m_bSunday;

	tDATETIME m_dtAllowWindowStart;
	tDATETIME m_dtAllowWindowEnd;
	tTIME_SHIFT m_AllowWindowTimeShift;

	tDWORD    m_nType;
	tDWORD    m_dwTimeStartDelta;       // delta to randomize start time, in seconds
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScheduleSettings , 0 )
	SER_VALUE( m_dwScheduleClass,                 tid_DWORD,    "ScheduleClass" )
	SER_VALUE( m_dwScheduleID,                    tid_DWORD,    "ScheduleID" )
	SER_VALUE( m_dtFirstRun,                      tid_DATETIME, "FirstRun" )
	SER_VALUE( m_dtLastRun,                       tid_DATETIME, "LastRun" )
	SER_VALUE( m_dtNextRun,                       tid_DATETIME, "NextRun" )
	SER_VALUE( m_bEnabled,                        tid_BOOL,     "Enabled" )
	SER_VALUE( m_bRaiseIfSkipped,                 tid_BOOL,     "RaiseIfSkipped" )
	SER_VALUE( m_TimeShift.dwYears,               tid_DWORD,    "TimeShift.dwYears" )
	SER_VALUE( m_TimeShift.dwMonths,              tid_DWORD,    "TimeShift.dwMonths" )
	SER_VALUE( m_TimeShift.dwDays,                tid_DWORD,    "TimeShift.dwDays" )
	SER_VALUE( m_TimeShift.dwHours,               tid_DWORD,    "TimeShift.dwHours" )
	SER_VALUE( m_TimeShift.dwMinutes,             tid_DWORD,    "TimeShift.dwMinutes" )
	SER_VALUE( m_TimeShift.dwSeconds,             tid_DWORD,    "TimeShift.dwSeconds" )
	SER_VALUE( m_bRelativeTimeShift,              tid_BOOL,     "RelativeTimeShift" )
	SER_VALUE( m_bWeekDays2Skip[0],               tid_BOOL,     "WeekDays2Skip0" )
	SER_VALUE( m_bWeekDays2Skip[1],               tid_BOOL,     "WeekDays2Skip1" )
	SER_VALUE( m_bWeekDays2Skip[2],               tid_BOOL,     "WeekDays2Skip2" )
	SER_VALUE( m_bWeekDays2Skip[3],               tid_BOOL,     "WeekDays2Skip3" )
	SER_VALUE( m_bWeekDays2Skip[4],               tid_BOOL,     "WeekDays2Skip4" )
	SER_VALUE( m_bWeekDays2Skip[5],               tid_BOOL,     "WeekDays2Skip5" )
	SER_VALUE( m_bWeekDays2Skip[6],               tid_BOOL,     "WeekDays2Skip6" )
	SER_VALUE( m_bSunday,                         tid_BOOL,     "Sunday" )
	SER_VALUE( m_dtAllowWindowStart,              tid_DATETIME, "AllowWindowStart" )
	SER_VALUE( m_dtAllowWindowEnd,                tid_DATETIME, "AllowWindowEnd" )
	SER_VALUE( m_AllowWindowTimeShift.dwYears,    tid_DWORD,    "AllowWindowTimeShift.dwYears" )
	SER_VALUE( m_AllowWindowTimeShift.dwMonths,   tid_DWORD,    "AllowWindowTimeShift.dwMonths" )
	SER_VALUE( m_AllowWindowTimeShift.dwDays,     tid_DWORD,    "AllowWindowTimeShift.dwDays" )
	SER_VALUE( m_AllowWindowTimeShift.dwHours,    tid_DWORD,    "AllowWindowTimeShift.dwHours" )
	SER_VALUE( m_AllowWindowTimeShift.dwMinutes,  tid_DWORD,    "AllowWindowTimeShift.dwMinutes" )
	SER_VALUE( m_AllowWindowTimeShift.dwSeconds,  tid_DWORD,    "AllowWindowTimeShift.dwSeconds" )
	SER_VALUE( m_nType,                           tid_DWORD,    "Type" )
	SER_VALUE( m_TimeShift.dwTimeShiftDayOfMonth, tid_DWORD,    "TimeShift.dwTimeShiftDayOfMonth" )
	SER_VALUE( m_dwTimeStartDelta,                tid_DWORD,    "TimeStartDelta" )
IMPLEMENT_SERIALIZABLE_END()

// --------------------------------------------------
// base structure for schedule settings

typedef enum enScheduleMode {
	schmManual  = 0,
	schmEnabled = 1,
	schmAuto    = 2,
} tScheduleMode;

typedef enum enScheduleType {
	schtMinutely  = 0,
	schtHourly    = 1,
	schtDaily     = 2,
	schtWeekly    = 3,
	schtExactTime = 4,
	schtMonthly   = 5,
	schtOnStartup = 6,
	schtAfterUpdate = 7,
} tScheduleType;

typedef enum enScheduleEvery {
	schEveryDays     = 0,
	schEveryWeekday  = 1,
	schEveryHoliday  = 2
} tScheduleEvery;

struct cTaskSchedule : public cSerializable {
	cTaskSchedule() : 
		cSerializable(),
		m_eMode( schmManual ),
		m_eType( schtDaily ),
		m_eEvery( schEveryDays ),
		m_nMinutes( 1 ),
		m_nHours( 1 ),
		m_nDays( 1 ),
		m_nMonths( 1 ),
		m_bUseTime( cFALSE ),
		m_nFirstRunTime( 0 ), // 12:00:00
		m_nLastRunTime( 0 ), // 12:00:00
		m_bRaiseIfSkipped(cFALSE),
		m_nStartDelta(0),
		m_nPrepareStartDelta(0),
		m_nWorkingTimeout(0),
		m_nStartDelay(0)
	{
		for(int i = 0; i < sizeof(m_bWeekDays)/sizeof(m_bWeekDays[0]); i++)
			m_bWeekDays[i] = cFALSE;
	}

	DECLARE_IID( cTaskSchedule, cSerializable, PID_SCHEDULER, 2 );

	tScheduleMode   m_eMode;
	tScheduleType   m_eType;
	tScheduleEvery  m_eEvery;
	tWORD           m_nMinutes;
	tWORD           m_nHours;
	tDWORD          m_nDays;
	tDWORD          m_nMonths;

	tBOOL           m_bWeekDays[7];

	tBOOL           m_bUseTime;
	__time32_t      m_nFirstRunTime;
	__time32_t      m_nLastRunTime;

	tBOOL           m_bRaiseIfSkipped;
	tDWORD          m_nStartDelta;
	tDWORD          m_nPrepareStartDelta;
	tDWORD          m_nWorkingTimeout;
	tDWORD          m_nStartDelay;

	tScheduleMode& mode()            { return m_eMode; }
	tScheduleMode  mode()      const { return m_eMode; }
	bool           auto_mode() const { return m_eMode == schmAuto; }
	bool           enabled()   const { return m_eMode == schmEnabled; }

	tScheduleType& type()            { return m_eType; }
	tScheduleType  type()      const { return m_eType; }
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskSchedule, 0 )
	SER_VALUE( m_eMode,               tid_DWORD,   "Mode" )
	SER_VALUE( m_eType,               tid_DWORD,   "Type" )
	SER_VALUE( m_eEvery,              tid_DWORD,   "Every" )
	SER_VALUE( m_nMinutes,            tid_WORD,    "MinutesCount" )
	SER_VALUE( m_nHours,              tid_WORD,    "HoursCount" )
	SER_VALUE( m_nDays,               tid_DWORD,   "DaysCount" )
	SER_VALUE( m_nMonths,             tid_DWORD,   "MonthsCount" )
	SER_VALUE( m_bWeekDays[0],        tid_BOOL,    "WeekDay0" )
	SER_VALUE( m_bWeekDays[1],        tid_BOOL,    "WeekDay1" )
	SER_VALUE( m_bWeekDays[2],        tid_BOOL,    "WeekDay2" )
	SER_VALUE( m_bWeekDays[3],        tid_BOOL,    "WeekDay3" )
	SER_VALUE( m_bWeekDays[4],        tid_BOOL,    "WeekDay4" )
	SER_VALUE( m_bWeekDays[5],        tid_BOOL,    "WeekDay5" )
	SER_VALUE( m_bWeekDays[6],        tid_BOOL,    "WeekDay6" )
	SER_VALUE( m_nFirstRunTime,       tid_DWORD,   "Time" )
	SER_VALUE( m_nLastRunTime,        tid_DWORD,   "LastRunTime" )
	SER_VALUE( m_bUseTime,            tid_BOOL,    "UseTime" )
	SER_VALUE( m_bRaiseIfSkipped,     tid_BOOL,    "RaiseIfSkipped" )
	SER_VALUE( m_nStartDelta,         tid_DWORD,   "StartDelta" )
	SER_VALUE( m_nPrepareStartDelta,  tid_DWORD,   "PrepareStartDelta" )
	SER_VALUE( m_nWorkingTimeout,     tid_DWORD,   "WorkingTimeout" )
	SER_VALUE( m_nStartDelay,         tid_DWORD,   "StartDelay" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

#endif//  __S_SCHEDULE_H
