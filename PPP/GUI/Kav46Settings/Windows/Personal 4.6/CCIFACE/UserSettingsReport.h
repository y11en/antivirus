#ifndef __USERSETTINGS_REPORT_H__
#define __USERSETTINGS_REPORT_H__

#define SS_NO_CONVERSION
#define SS_NO_BSTR
#include "DEFS.H"
#include "BaseMarshal.h"
#include "PredefinedTaskNames.h"
#include "UserSettingsExclude.h"
#include "UserSettingsScanners.h"
#include "UserSettingsRTP.h"
#include "UserSettingsUpdater.h"

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


//******************************************************************************
// эта структура используется для хранения настроек ODS в журнале
//******************************************************************************
namespace Private
{
struct CCIFACEEXPORT CCODSReportSettings_v1 : public CCBaseMarshal<PID_REPORT_SETTINGS_ODS>
{
	
	CUserSettingsODS m_UserSettings;
	CCODSScanObjects m_ScanObjects;

    virtual PoliType &MarshalImpl (PoliType& pt)
	{
		return BaseMarshal::MarshalImpl (pt) << m_UserSettings << m_ScanObjects;
	}
};


struct CCIFACEEXPORT CCODSReportSettings_v2 : public DeclareNextVersion<CCODSReportSettings_v1>
{
	CExcludeList m_ExcludeList;	//!< Список исключений с вердиктами.
	std::string m_sTaskName;	//!< Имя задачи (нужно для выуживания исключений из m_ExcludeList)

    virtual PoliType &MarshalImpl (PoliType& pt)
	{
		BaseMarshal::MarshalImpl (pt);
		if (pt.error_() == PoliType::eNoErrorOldDataVersion)
		{
			m_sTaskName = CC_TASK_UID_FULLSCAN;
			// Надо перевести старые exclude на новые.
			if (m_UserSettings.m_blExcludeEnable)
			{
				for (std::vector<std::wstring>::const_iterator it = m_UserSettings.m_ExcludeMaskDeprecated.begin()
					, end = m_UserSettings.m_ExcludeMaskDeprecated.end(); it != end; ++it)
				{
					CExclude excl(*it);
					excl.m_Tasks.push_back(m_sTaskName);
					m_ExcludeList.m_List.push_back(excl);
				}
			}
		}
		return BaseMarshal::MarshalImpl (pt) << m_ExcludeList << m_sTaskName;
	}
};


struct CCIFACEEXPORT CCODSReportSettings_v3 : public DeclareNextVersion<CCODSReportSettings_v2>
{
	CLogonCredentials m_LogonInfo;

    virtual PoliType &MarshalImpl (PoliType& pt)
	{
		BaseMarshal::MarshalImpl (pt);
		if (pt.error_() == PoliType::eNoErrorOldDataVersion)
		{
			m_LogonInfo.m_bUseCredentials = false;
		}
		return BaseMarshal::MarshalImpl (pt) << m_LogonInfo;
	}
};
}


typedef DeclareExternalName<Private::CCODSReportSettings_v3> CCODSReportSettings_Ser;

struct CCIFACEEXPORT CCODSReportSettings : public CCODSReportSettings_Ser
{
	CCODSReportSettings ()
	{
	}
	
	CCODSReportSettings (const CUserSettingsODS &UserSettings, const CCODSScanObjects &ScanObjects)
	{
        m_dwID = PID_REPORT_SETTINGS_ODS;
		m_UserSettings.Assign ((CBase *)&UserSettings);
		m_ScanObjects.Assign ((CBase *)&ScanObjects);
	}
	
};

namespace Private
{
	struct CCIFACEEXPORT CCRTPReportSettings_v1 : public CCBaseMarshal<PID_REPORT_SETTINGS_RTP>
	{
		CUserSettingsRTP m_UserSettings;
		CExcludeList m_ExcludeList;	//!< Список исключений с вердиктами.
		std::string m_sTaskName;	//!< Имя задачи (нужно для выуживания исключений из m_ExcludeList)
		
		virtual PoliType &MarshalImpl (PoliType& pt)
		{
			return BaseMarshal::MarshalImpl (pt) << m_UserSettings << m_ExcludeList << m_sTaskName;
		}
	};
}

typedef DeclareExternalName<Private::CCRTPReportSettings_v1> CCRTPReportSettings;

namespace Private
{
	struct CCIFACEEXPORT CCUpdaterReportSettings_v1 : public CCBaseMarshal<PID_REPORT_SETTINGS_UPDATER>
	{
		// тип задачи
		enum ETaskType
		{
			eUpdaterTask = 2,
			eRollbackTask = 4
		};
		CUserSettingsUpdater m_UserSettings;
		ETaskType m_eTaskType;
		
		virtual PoliType &MarshalImpl (PoliType& pt)
		{
			return BaseMarshal::MarshalImpl (pt) << m_UserSettings << SIMPLE_DATA_SER(m_eTaskType);
		}
	};
}

typedef DeclareExternalName<Private::CCUpdaterReportSettings_v1> CCUpdaterReportSettings;

#endif // __USERSETTINGS_REPORT_H__