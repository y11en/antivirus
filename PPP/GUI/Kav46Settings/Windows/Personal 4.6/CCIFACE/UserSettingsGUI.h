#ifndef __USERSETTINGS_GUI_H__
#define __USERSETTINGS_GUI_H__

#define SS_NO_CONVERSION
#define SS_NO_BSTR
#include "DEFS.H"
#include "BaseMarshal.h"
#include "CCTasks.h"
//#include "../../../CommonFiles/CCClient/taskstat.h"
#include "UserSettingsRTP.h"

//#include "../Rpt/rptdbtype.h"
#ifndef _RPTDBTYPE_H_
	typedef int TRptID;
	#define RPTID_INVALID     ((TRptID)(-1))
#endif

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information

namespace Private
{
class CCIFACEEXPORT CUserSettingsGUI_v1 : public CCBaseMarshal<PID_USER_SETTINGS_GUI>
{
public:
	CUserSettingsGUI_v1 ()
	{
        // эти балуны просто задолбали ...
#ifdef _DEBUG
        m_bDoNotShowBalloons = true;
#else
		m_bDoNotShowBalloons = false;
#endif
		m_bSoundOff = false;
		m_bOkInfoEventsOff = true;
		m_bRestrictReportLife = true;
		m_bRestrictBackupLife = true;
		m_bDoNotStartProduct = false;
		
		m_nReportOld = 30;
		m_nBackupOld = 30;

        m_bUsePassword = false;
    }


	bool operator == ( const CUserSettingsGUI_v1 & That ) const
	{
		return ( m_bDoNotShowBalloons == That.m_bDoNotShowBalloons &&
					m_bSoundOff == That.m_bSoundOff &&
					m_bOkInfoEventsOff == That.m_bOkInfoEventsOff &&
					m_bRestrictReportLife == That.m_bRestrictReportLife &&
					m_bRestrictBackupLife == That.m_bRestrictBackupLife &&
					m_bDoNotStartProduct == That.m_bDoNotStartProduct &&
					m_nReportOld == That.m_nReportOld &&
					m_nBackupOld == That.m_nBackupOld &&
					m_bUsePassword == That.m_bUsePassword &&
					( m_bUsePassword == false || m_PasswordDeprecated == That.m_PasswordDeprecated )
				);
	}

	bool operator != ( const CUserSettingsGUI_v1 & That ) const
	{
		return ! operator == ( That );
	}

	bool m_bDoNotShowBalloons;
	bool m_bSoundOff;
	bool m_bOkInfoEventsOff;
	bool m_bRestrictReportLife;
	bool m_bRestrictBackupLife;
	bool m_bDoNotStartProduct;

	int m_nReportOld;
	int m_nBackupOld;
	
    bool m_bUsePassword;
    std::string m_PasswordDeprecated; // Теперь хранится md5 пароля в m_PasswordHash
	
    virtual PoliType & MarshalImpl ( PoliType & pt )
	{
		return BaseMarshal::MarshalImpl ( pt ) << m_bDoNotShowBalloons
            << m_bSoundOff << m_bOkInfoEventsOff << m_bRestrictReportLife
            << m_bRestrictBackupLife << m_bDoNotStartProduct 
            << m_nReportOld << m_nBackupOld << m_bUsePassword << m_PasswordDeprecated
            ;
	}
};

class CCIFACEEXPORT CUserSettingsGUI_v2 : public DeclareNextVersion<CUserSettingsGUI_v1>
{
public:
	CUserSettingsGUI_v2() : m_bAskStartExit(true), m_bAskBeforeStopScan(true), m_bShutdownFullscanFinish(false){}
	bool operator == ( const CUserSettingsGUI_v2 & That ) const
	{
		return CUserSettingsGUI_v1::operator==(That) 
			&& m_bAskStartExit==That.m_bAskStartExit
			&& m_bAskBeforeStopScan==That.m_bAskBeforeStopScan;
	}
	
	bool operator != ( const CUserSettingsGUI_v2 & That ) const
	{
		return ! operator == ( That );
	}
	
	virtual PoliType & MarshalImpl ( PoliType & pt )
	{
		return BaseMarshal::MarshalImpl ( pt ) << m_bAskStartExit<<m_bAskBeforeStopScan<<m_bShutdownFullscanFinish;
	}
	
	
	bool m_bAskStartExit;
	bool m_bAskBeforeStopScan;
	bool m_bShutdownFullscanFinish;
};

//******************************************************************************/
// 
//******************************************************************************
class CCIFACEEXPORT CUserSettingsGUI_v3 : public DeclareNextVersion < CUserSettingsGUI_v2 >
{
public:
    CUserSettingsGUI_v3 ()
    :   m_bAskPauseRtp ( true ),
        m_PauseDlgLastCondition ( CUserSettingsRTP::eDefault ),
        m_PauseDlgLastTimeInterval ( CUserSettingsRTP::DefaultTimeout ),
        m_PauseDlgLastStartOnBoot ( true ),
		m_bAskCancelTIF (true)
    {
    }

    bool operator == ( const CUserSettingsGUI_v3 & That ) const
    {
        return CUserSettingsGUI_v2::operator == ( That ) && 
            m_bAskPauseRtp == That.m_bAskPauseRtp &&
            m_PauseDlgLastCondition == That.m_PauseDlgLastCondition &&
            m_PauseDlgLastTimeInterval == That.m_PauseDlgLastTimeInterval &&
            m_PauseDlgLastStartOnBoot == That.m_PauseDlgLastStartOnBoot &&
			m_bAskCancelTIF == That.m_bAskCancelTIF;
    }
    
    bool operator != ( const CUserSettingsGUI_v3 & That ) const
    {
        return !operator == ( That );
    }
    
    virtual PoliType & MarshalImpl ( PoliType & pt )
    {
        return BaseMarshal::MarshalImpl ( pt ) << 
			m_bAskPauseRtp << 
			SIMPLE_DATA_SER ( m_PauseDlgLastCondition ) << 
			m_PauseDlgLastTimeInterval << 
			m_PauseDlgLastStartOnBoot <<
			m_bAskCancelTIF;
    }
    bool m_bAskPauseRtp;
    CUserSettingsRTP::ERTPStartCondition m_PauseDlgLastCondition;
    DWORD m_PauseDlgLastTimeInterval;
    bool m_PauseDlgLastStartOnBoot;
	bool m_bAskCancelTIF;
};

//******************************************************************************
// 
//******************************************************************************
class CCIFACEEXPORT CUserSettingsGUI_v4 : public DeclareNextVersion < CUserSettingsGUI_v3 >
{
public:
    CUserSettingsGUI_v4 ()
        :   m_bIconAnimation ( true )
    {
    }
    
    bool m_bIconAnimation;
    
    bool operator == ( const CUserSettingsGUI_v4 & That ) const
    {
        return CUserSettingsGUI_v3::operator == ( That ) && 
			m_bIconAnimation == That.m_bIconAnimation;
    }
    
    bool operator != ( const CUserSettingsGUI_v4 & That ) const
    {
        return !operator == ( That );
    }

    virtual PoliType & MarshalImpl ( PoliType & pt )
    {
        return BaseMarshal::MarshalImpl ( pt ) << m_bIconAnimation;
    }
};

//******************************************************************************
// 
//******************************************************************************
class CCIFACEEXPORT CUserSettingsGUI_v5 : public DeclareNextVersion<CUserSettingsGUI_v4>
{
public:
    CUserSettingsGUI_v5() :
		m_bDisableFullscanMailScanning(true),
		m_bInteractWithUser(false),
		m_bStartGUIOnStart(true),
		m_nNotifyAVBasesExpiredPeriod1(7),
		m_nNotifyAVBasesExpiredPeriod2(14),
		m_nNotifyFullscanStatusPeriod1(14),
		m_nNotifyFullscanStatusPeriod2(30),
		m_bUseMinBatteryChargePercent(true),
		m_nMinBatteryChargePercent(30),
		m_bUseMaxSystemBusyLevel(true),
		m_nMaxSystemBusyLevel(20),
		m_bShowAdminTasks(true),
		m_bUseUninstallPassword(false),
#ifdef _DEBUG
		m_bUseWatchDog(false)
#else
		m_bUseWatchDog(true)
#endif
    {
    }
    
	std::string m_TechnicalEmail;				// E-mail адрес суппорта
	bool m_bDisableFullscanMailScanning;		// отключение проверки почты при fullscan
	bool m_bInteractWithUser;					// поднимать ли какие-либо ask action пользователю
	bool m_bStartGUIOnStart;					// запускать ли GUI при старте антивируса
	DWORD m_nNotifyAVBasesExpiredPeriod1;		// порог 1 уведомления об устаревании баз
	DWORD m_nNotifyAVBasesExpiredPeriod2;		// порог 2 уведомления об устаревании баз
	DWORD m_nNotifyFullscanStatusPeriod1;		// порог 1 уведомления о невыполненном fullscan
	DWORD m_nNotifyFullscanStatusPeriod2;		// порог 2 уведомления о невыполненном fullscan
	bool m_bUseUninstallPassword;				// использовать пароль для деинсталяции
	std::string m_UninstallPasswordHash;		// md5 хэш пароля для деинсталяции (строка из 32 символов)
	std::string m_PasswordHash;					// md5 хэш пароля администратора(строка из 32 символов)
	bool m_bUseMinBatteryChargePercent;			// использовать ли ограничение на заряд батареи при запуске по расписанию
	int m_nMinBatteryChargePercent;				// минимальный заряд батареи для запуска по расписанию
	bool m_bUseMaxSystemBusyLevel;				// использовать ли ограничение на загрузку файловой системы
	int m_nMaxSystemBusyLevel;					// загрузка файловой системы, выше которой останавливается сканирование
	bool m_bUseWatchDog;						// использовать систему слежения за модулями
	bool m_bShowAdminTasks;						// показывать ли пользователю задачи администратора
    
    bool operator==(const CUserSettingsGUI_v5 &That) const
    {
        return CUserSettingsGUI_v4::operator==(That)
			&& m_TechnicalEmail == That.m_TechnicalEmail
			&& m_bDisableFullscanMailScanning == That.m_bDisableFullscanMailScanning
			&& m_bInteractWithUser == That.m_bInteractWithUser
			&& m_bStartGUIOnStart == That.m_bStartGUIOnStart
			&& m_nNotifyAVBasesExpiredPeriod1 == That.m_nNotifyAVBasesExpiredPeriod1
			&& m_nNotifyAVBasesExpiredPeriod2 == That.m_nNotifyAVBasesExpiredPeriod2
			&& m_nNotifyFullscanStatusPeriod1 == That.m_nNotifyFullscanStatusPeriod1
			&& m_nNotifyFullscanStatusPeriod2 == That.m_nNotifyFullscanStatusPeriod2
			&& m_UninstallPasswordHash == That.m_UninstallPasswordHash
			&& m_PasswordHash == That.m_PasswordHash
			&& m_bUseMinBatteryChargePercent == That.m_bUseMinBatteryChargePercent
			&& m_nMinBatteryChargePercent == That.m_nMinBatteryChargePercent
			&& m_bUseMaxSystemBusyLevel == That.m_bUseMaxSystemBusyLevel
			&& m_nMaxSystemBusyLevel == That.m_nMaxSystemBusyLevel
			&& m_bUseWatchDog == That.m_bUseWatchDog
			&& m_bShowAdminTasks == That.m_bShowAdminTasks
			&& m_bUseUninstallPassword == That.m_bUseUninstallPassword
			;
    }
    
    bool operator!=(const CUserSettingsGUI_v5 &That) const
    {
        return !operator==(That);
    }

    virtual PoliType &MarshalImpl(PoliType &pt)
    {
        return BaseMarshal::MarshalImpl(pt) 
				<< m_TechnicalEmail
				<< m_bDisableFullscanMailScanning
				<< m_bInteractWithUser
				<< m_bStartGUIOnStart
				<< m_nNotifyAVBasesExpiredPeriod1
				<< m_nNotifyAVBasesExpiredPeriod2
				<< m_nNotifyFullscanStatusPeriod1
				<< m_nNotifyFullscanStatusPeriod2
				<< m_UninstallPasswordHash
				<< m_PasswordHash
				<< m_bUseMinBatteryChargePercent
				<< m_nMinBatteryChargePercent
				<< m_bUseMaxSystemBusyLevel
				<< m_nMaxSystemBusyLevel
				<< m_bUseWatchDog
				<< m_bShowAdminTasks
				<< m_bUseUninstallPassword
				;
    }
};

}

typedef DeclareExternalName <Private::CUserSettingsGUI_v5> CUserSettingsGUI;

//******************************************************************************
// 
//******************************************************************************
struct CCIFACEEXPORT CCTaskStateInfo : public CCBaseMarshal<PID_TASK_STATE>
{
	CCTaskStateInfo () :
		m_dwState (TS_UNKNOWN),
		m_dwStateParam (0),
		m_dwStateParam2 (0),
		m_ReportID (RPTID_INVALID),
		m_dwSessionID (0),
		m_StartType (eNotStarted)
	{
	}
		
	ETaskState m_dwState;
	DWORD m_dwStateParam; // ETaskExecErrCode для m_dwState == TS_ERREXEC
	DWORD m_dwStateParam2; // Подробное описание m_dwStateParam
	TRptID m_ReportID;
	std::vector<BYTE> m_Statistic;
	DWORD m_dwSessionID;
	EStartType m_StartType;
	
	virtual PoliType &MarshalImpl (PoliType& pt)
	{
		return BaseMarshal::MarshalImpl (pt)
					<< SIMPLE_DATA_SER(m_dwState)
					<< m_dwStateParam
					<< m_dwStateParam2
					<< m_ReportID
					<< m_Statistic
					<< m_dwSessionID
					<< SIMPLE_DATA_SER(m_StartType)
					;
	}
};

IMPLEMENT_MARSHAL(CCTaskStateInfo);

namespace Private
{
	struct CCIFACEEXPORT CCSelectObjectDlg_Object_v1 : public CCBaseMarshal<PID_USER_SETTINGS_SELECT_OBJECT_OBJECT>
	{
		DWORD m_Type;
		DWORD m_Flags;
		std::wstring m_Name;
		
		CCSelectObjectDlg_Object_v1 ()
		{
		}
		
		CCSelectObjectDlg_Object_v1 ( DWORD Type, DWORD Flags, const std::wstring & Name = L"" )
			: m_Type ( Type )
			, m_Name ( Name )
			, m_Flags ( Flags )
		{
		}
		
		CCSelectObjectDlg_Object_v1 ( const CCSelectObjectDlg_Object_v1 & That )
		{
			m_Type = That.m_Type;
			m_Name = That.m_Name;
			m_Flags = That.m_Flags;
		}
		
		CCSelectObjectDlg_Object_v1 & operator = ( const CCSelectObjectDlg_Object_v1 & That )
		{
			m_Type = That.m_Type;
			m_Name = That.m_Name;
			m_Flags = That.m_Flags;
			return *this;
		}
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_Type << m_Name << m_Flags;
		}
	};
}
typedef DeclareExternalName<Private::CCSelectObjectDlg_Object_v1> CCSelectObjectDlg_Object;

namespace Private
{
	struct CCIFACEEXPORT CCSelectObjectDlg_Objects_v1 : public CCBaseMarshal<PID_USER_SETTINGS_SELECT_OBJECT_OBJECTS>
	{
		CCSelectObjectDlg_Objects_v1 ()
		{
		}
		
		CCSelectObjectDlg_Objects_v1 ( const CCSelectObjectDlg_Objects_v1 & That )
		{
			m_List = That.m_List;
		}
		
		std::list < CCSelectObjectDlg_Object > m_List;
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_List;
		}
	};

}
typedef DeclareExternalName<Private::CCSelectObjectDlg_Objects_v1> CCSelectObjectDlg_Objects;

namespace Private
{
	class CCIFACEEXPORT CUserSettingsUpdaterGUI_v1 : public CCBaseMarshal<PID_USER_SETTINGS_UPDATER_GUI>
	{
	public:
		CUserSettingsUpdaterGUI_v1 ()
		{
			m_bAutoClose = TRUE;
		}
		
		BOOL m_bAutoClose;
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_bAutoClose;
		}
	};
}

typedef DeclareExternalName<Private::CUserSettingsUpdaterGUI_v1> CUserSettingsUpdaterGUI;

#endif // __USERSETTINGS_GUI_H__