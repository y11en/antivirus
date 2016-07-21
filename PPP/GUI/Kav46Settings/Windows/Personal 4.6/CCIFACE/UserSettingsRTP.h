#ifndef __USERSETTINGS_RTP_H__
#define __USERSETTINGS_RTP_H__

#define SS_NO_CONVERSION
#define SS_NO_BSTR
#include "DEFS.H"
#include "BaseMarshal.h"
#include "UserSettingsScanners.h"
#include "UserSettingsOG.h"
#include "UserSettingsSC.h"
#include "UserSettingsCKAH.h"
#include "TaskStatus.h"

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


namespace Private
{
struct CCIFACEEXPORT CUserSettingsRTP_v1 : public CCBaseMarshal < PID_USER_SETTINGS_RTP >
{
	
	enum ERTPScanLevel
    {
		eScanLevel_Max  = 0x1,
		eScanLevel_Rec  = 0x2,
		eScanLevel_Min  = 0x3,
	};
	
	CUserSettingsOAS::ERTPUserAction m_eUserActionDeprecated;
	ERTPScanLevel   m_eScanLevel;
	
	bool			m_bRTPDisable;
	bool			m_bMonitorDisable;
	bool			m_bMCDisable;
	bool			m_bSCDisable;
	bool			m_bOkInfoEventsOff;

	// Устаревшие настройки
	bool            m_bDoNotScanSectorsDeprecated;
	bool			m_bStopAfterDeprecated;
	int				m_nStopTimeDeprecated;
	bool			m_bCheckExcludeListDeprecated;
	std::vector<std::wstring>   m_ExcludeMaskDeprecated;

// auxiliary data
	std::string		m_strStartTaskExternalReportName;
	
	virtual PoliType & MarshalImpl ( PoliType & pt )
    {
        return BaseMarshal::MarshalImpl  (pt)
            << SIMPLE_DATA_SER ( m_eUserActionDeprecated )
            << SIMPLE_DATA_SER ( m_eScanLevel )
            << m_bRTPDisable
            << m_bMonitorDisable
            << m_bMCDisable
            << m_bSCDisable
            << m_bDoNotScanSectorsDeprecated
            << m_bStopAfterDeprecated
            << m_nStopTimeDeprecated
            << m_bCheckExcludeListDeprecated
            << m_ExcludeMaskDeprecated
            << m_bOkInfoEventsOff
            << m_strStartTaskExternalReportName;
    }
};

//////////////////////////////////////////////////////////////////////////

class CCIFACEEXPORT CUserSettingsRTP_v2 : public DeclareNextVersion<CUserSettingsRTP_v1>
{
public:
	bool	m_bCKAHDisable;
	
	bool	m_bCKAHDoNotNotifyUserDeprecated;
	bool	m_bCKAHBanDeprecated;
	DWORD	m_dwCKAHBanTimeDeprecated; // seconds, 0 - no ban
	bool	m_bCKAHUseStealthModeDeprecated;

	PoliType & MarshalImpl ( PoliType & pt )
    {
        return BaseMarshal::MarshalImpl ( pt ) << m_bCKAHDisable << m_bCKAHDoNotNotifyUserDeprecated
            << m_bCKAHBanDeprecated << m_dwCKAHBanTimeDeprecated << m_bCKAHUseStealthModeDeprecated;
    };
};

//******************************************************************************
// 
//******************************************************************************
class CCIFACEEXPORT CUserSettingsRTP_v3 : public DeclareNextVersion<CUserSettingsRTP_v2>
{
public:
	//! Типы условий старта RTP.
	enum ERTPStartCondition
	{
		eStartCondition_StartNow = 0x1,		//!< RTP запущен.
		eStartCondition_Manual = 0x2,		//!< Стартовать в ручную.
		eStartCondition_OnTimeout = 0x3,	//!< Стартовать по таймауту.
		eStartCondition_OnInetAlive = 0x4,	//!< Стартовать когда зашевелится интернет.
        eDefault = 0x3
	};

    enum { DefaultTimeout = 5 * 60 };

	// Настройки паузы.
	ERTPStartCondition m_eCondition;	//! Условие старта RTP.
	bool	m_bStartOnBoot;	//!< Если true, то при загрузке переводит m_eCondition в eStartCondition_StartNow.
	CSysTimeSer	m_tmStartTime;	//!< Если m_eCondition == eStartCondition_OnTimeout то UTC время перевода в eStartCondition_StartNow.


	// Дополнительные настройки для Personal Pro.
	bool	m_bOGDisable;		//!< Флаг отключения Office Guard.
	COfficeGuardSettings m_OG;	//!< Настройки Office Guard.
	CUserSettingsMC		m_MC;	//!< Настройки Mail Checker.
	CUserSettingsSC		m_SC;	//!< Настройки Script Checker.
	CUserSettingsOAS	m_OAS;	//!< Настройки On-Access Scanner.

	PoliType & MarshalImpl ( PoliType & pt )
    {
		BaseMarshal::MarshalImpl ( pt );
		if (pt.error_() == PoliType::eNoErrorOldDataVersion)
		{
			// Надо upgrade'ить m_bRTPDisable
			if (m_bRTPDisable)
			{
				m_eCondition = eStartCondition_Manual;
				m_bRTPDisable = false;
			}
			m_OAS.m_bScanBootSectors = !m_bDoNotScanSectorsDeprecated;
			m_OAS.m_bScanTimeLimit = m_bStopAfterDeprecated;
			m_OAS.m_nMaxScanTimeSec = m_nStopTimeDeprecated;
			m_OAS.m_eUserAction = m_eUserActionDeprecated;
		}
        return BaseMarshal::MarshalImpl ( pt ) << m_bStartOnBoot << SIMPLE_DATA_SER ( m_eCondition )
			<< m_tmStartTime << m_bOGDisable << m_OG << m_MC << m_SC << m_OAS;
	}
};

//******************************************************************************
// 
//******************************************************************************
class CCIFACEEXPORT CUserSettingsRTP_v4 : public DeclareNextVersion<CUserSettingsRTP_v3>
{
public:
	bool m_bCKAHDriversDisabled;	//!< Флаг отключения базовых драйверов CKAH 
									//!< флаг всегда имеет значение запущенности драйверов при старте
									//!< системы, установка в противоположное значение трактуется как запрос
									//!< на изменение
	PoliType &MarshalImpl(PoliType &pt)
    {
        return BaseMarshal::MarshalImpl(pt) << m_bCKAHDriversDisabled;
	}
};

//FW
class CCIFACEEXPORT CUserSettingsRTP_v5 : public DeclareNextVersion<CUserSettingsRTP_v4>
{
public:
	bool m_bFirewallDisabled; // флаг включения firewall
	CUserSettingsFirewall m_FW; // настройки firewall

	PoliType &MarshalImpl(PoliType &pt)
    {
        return BaseMarshal::MarshalImpl(pt) << m_bFirewallDisabled << m_FW;
	}
};

class CCIFACEEXPORT CUserSettingsRTP_v6 : public DeclareNextVersion<CUserSettingsRTP_v5>
{
public:
	CUserSettingsCKAH m_CKAH; // настройки IDS

	PoliType &MarshalImpl(PoliType &pt)
    {
		BaseMarshal::MarshalImpl ( pt );
		if (pt.error_() == PoliType::eNoErrorOldDataVersion)
		{
			m_CKAH.m_bNotifyUser = !m_bCKAHDoNotNotifyUserDeprecated;
			m_CKAH.m_bBan = m_bCKAHBanDeprecated;
			m_CKAH.m_dwBanTime = m_dwCKAHBanTimeDeprecated;
			m_CKAH.m_bUseStealthMode = m_bCKAHUseStealthModeDeprecated;
		}
        return pt << m_CKAH;
	}
};

}

struct CCIFACEEXPORT CUserSettingsRTP : public DeclareExternalName<Private::CUserSettingsRTP_v6>
{
	CUserSettingsRTP()
	{
		// V1
		m_eScanLevel = eScanLevel_Rec;
		m_bRTPDisable = false;
		m_bMonitorDisable = false;
		m_bMCDisable = false;
		m_bSCDisable = false;
		m_bOkInfoEventsOff = true;
		// Устаревшие настройки
		m_eUserActionDeprecated = CUserSettingsOAS::eUserAction_DenyAccessAskUser;
		m_bDoNotScanSectorsDeprecated = false;
        m_bStopAfterDeprecated = false;
		m_nStopTimeDeprecated = 60;
		m_bCheckExcludeListDeprecated = false;
		// V2
		m_bCKAHDisable = false;
		m_bCKAHDoNotNotifyUserDeprecated = false;
		m_bCKAHBanDeprecated = false;
		m_dwCKAHBanTimeDeprecated = 60 * 60;
		m_bCKAHUseStealthModeDeprecated = false;
		// V3
		m_bStartOnBoot = true;
		m_eCondition = eStartCondition_StartNow;
		m_bOGDisable = false;
		// V4
		m_bCKAHDriversDisabled = false;
		// V5
		m_bFirewallDisabled = false;
		// V6
	}

	bool IsOASSettingsChange(const CUserSettingsRTP& Settings) const
	{
		return m_OAS.m_eUserAction != Settings.m_OAS.m_eUserAction
			|| m_eScanLevel != Settings.m_eScanLevel
			|| m_OAS.m_bScanBootSectors != Settings.m_OAS.m_bScanBootSectors
			|| m_OAS.m_bScanTimeLimit != Settings.m_OAS.m_bScanTimeLimit
			|| m_OAS.m_nMaxScanTimeSec != Settings.m_OAS.m_nMaxScanTimeSec;
	}

	bool IsOASDisabled(bool bInstalled) const { return !bInstalled || m_eCondition != eStartCondition_StartNow || m_bRTPDisable || m_bMonitorDisable; }
    bool IsMailCheckerDisabled(bool bInstalled) const { return !bInstalled || m_eCondition != eStartCondition_StartNow || m_bRTPDisable || m_bMCDisable; }
    bool IsScriptCheckerDisabled(bool bInstalled) const { return !bInstalled || m_eCondition != eStartCondition_StartNow || m_bRTPDisable || m_bSCDisable; }
    bool IsCKAHDisabled(bool bInstalled) const { return !bInstalled || m_eCondition != eStartCondition_StartNow || m_bRTPDisable || m_bCKAHDisable; }
    bool IsOfficeGuardDisabled(bool bInstalled) { return !bInstalled || m_eCondition != eStartCondition_StartNow || m_bRTPDisable || m_bOGDisable; }

	bool IsExclude(const CProductInfo& productInfo) const;
	void SetRecommended();
	bool IsDefault(const CProductInfo& productInfo) const;
	
	bool IsMaximal(const CProductInfo& productInfo) const;
	bool IsRecommended(const CProductInfo& productInfo) const;
	bool IsMoreThanRecommended(const CProductInfo& productInfo) const;
	bool IsMinimal(const CProductInfo& productInfo) const;
};


#endif // __USERSETTINGS_RTP_H__