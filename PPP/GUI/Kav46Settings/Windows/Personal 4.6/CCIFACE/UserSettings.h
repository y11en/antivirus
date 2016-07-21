#ifndef __USERSETTINGS_H__
#define __USERSETTINGS_H__

#include "../release_type.h"

#include "DEFS.H"
#include "basetypes.h"
#include "BaseMarshal.h"

#include "../cc/Common/Pid.h"

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


#include "UserSettingsExclude.h"
#include "UserSettingsUpdater.h"
#include "UserSettingsOG.h"
#include "UserSettingsSC.h"
#include "UserSettingsScanners.h"
#include "UserSettingsRTP.h"
#include "UserSettingsGUI.h"
#include "UserSettingsReport.h"
#include "UserSettingsSchedule.h"
#include "UserSettingsLogon.h"
#include "UserSettingsAK.h"
#include "UserSettingsInstallKey.h"

namespace Private
{
	//! Настройки восстановления для карантина и бэкапа.
	struct CCIFACEEXPORT CQBRestore_v1 : public CCBaseMarshal<PID_USER_SETTINGS_QB_RESTORE>
	{
		bool m_bRestoreToRestoreFolder;	//!< Восстанавливать в каталог восстановления
		std::wstring m_sRestoreFolder;	//!< Путь к каталогу восстановления.
		bool m_bAskFolderConfirmation;	//!< Запрашивать подтверждение пути перед восстановлением.

        CQBRestore_v1 ()
		:   m_bRestoreToRestoreFolder ( false ),
			m_bAskFolderConfirmation ( false )
		{
		}

	    bool operator==(const CQBRestore_v1 &that) const
		{
			return m_bRestoreToRestoreFolder == that.m_bRestoreToRestoreFolder
					&& m_sRestoreFolder == that.m_sRestoreFolder
					&& m_bAskFolderConfirmation == that.m_bAskFolderConfirmation;
		}
		bool operator!=(const CQBRestore_v1 &that) const
		{
			return !operator==(that);
		}

        virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_bRestoreToRestoreFolder
				<< m_sRestoreFolder << m_bAskFolderConfirmation;
		};
	};
}

typedef DeclareExternalName<Private::CQBRestore_v1> CQBRestore;

namespace Private
{
	//! Действие карантина если его размер превышает установленный.
	enum EOverflowQBAction
	{
		eOverflowQBAction_Notify,	//!< Уведомлять пользователя.
		eOverflowQBAction_DeleteOld	//!< Удалять самые старые.
	};

	struct CCIFACEEXPORT CQuarantineSettings_v1 : public CCBaseMarshal<PID_USER_SETTINGS_QUARANTINE>
	{
		CQuarantineSettings_v1 ()
			: m_bAutoScan (false)
			, m_bMaxSizeOn (false)
			, m_bMaxTimeOn (true)
			, m_nMaxSize (100)
			, m_nMaxTime (90)
		{
		}
		
		bool operator==(const CQuarantineSettings_v1 &that) const
		{
			return m_bAutoScan == that.m_bAutoScan
					&& m_bMaxSizeOn == that.m_bMaxSizeOn
					&& m_bMaxTimeOn == that.m_bMaxTimeOn
					&& m_nMaxSize == that.m_nMaxSize
					&& m_nMaxTime == that.m_nMaxTime
					;
		}
		bool operator!=(const CQuarantineSettings_v1 &that) const
		{
			return !operator==(that);
		}

		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt )
				<< m_bAutoScan << m_bMaxSizeOn << m_bMaxTimeOn << m_nMaxSize << m_nMaxTime;
		}
		
		bool m_bAutoScan; //Scan after bases update
		bool m_bMaxSizeOn;
		bool m_bMaxTimeOn;
		int m_nMaxSize;
		int m_nMaxTime;
	};

	struct CCIFACEEXPORT CQuarantineSettings_v2 : public DeclareNextVersion<CQuarantineSettings_v1>
	{
		typedef Private::EOverflowQBAction EOverflowQBAction;
		EOverflowQBAction	m_eOverflowAction;	//!< Действие карантина если его размер превышает установленный.
		CQBRestore			m_Restore;			//!< Настройки восстановления.

		CQuarantineSettings_v2 ()
        :   m_eOverflowAction ( eOverflowQBAction_Notify )
        {
        }

		bool operator==(const CQuarantineSettings_v2 &that) const
		{
			return CQuarantineSettings_v1::operator==(that)
					&& m_eOverflowAction == that.m_eOverflowAction
					&& m_Restore == that.m_Restore;
		}
		bool operator!=(const CQuarantineSettings_v2 &that) const
		{
			return !operator==(that);
		}

		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << SIMPLE_DATA_SER(m_eOverflowAction) << m_Restore;
		};
	};

	struct CCIFACEEXPORT CQuarantineSettings_v3 : public DeclareNextVersion<CQuarantineSettings_v2>
	{
		std::wstring m_FilesPath;

		bool operator==(const CQuarantineSettings_v3 &that) const
		{
			return CQuarantineSettings_v2::operator==(that)
					&& m_FilesPath == that.m_FilesPath
					;
		}
		bool operator!=(const CQuarantineSettings_v3 &that) const
		{
			return !operator==(that);
		}

		virtual PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_FilesPath;
		};
	};

	struct CCIFACEEXPORT CBackupSettings_v1 : public CCBaseMarshal<PID_USER_SETTINGS_BACKUP>
	{
		CBackupSettings_v1 ()
		:   m_bMaxSizeOn ( false ),
			m_bMaxTimeOn ( true ),
			m_nMaxSize ( 100 ),
			m_nMaxTime ( 90 )
        {
        }
		
		bool operator==(const CBackupSettings_v1 &that) const
		{
			return m_bMaxSizeOn == that.m_bMaxSizeOn
					&& m_bMaxTimeOn == that.m_bMaxTimeOn
					&& m_nMaxSize == that.m_nMaxSize
					&& m_nMaxTime == that.m_nMaxTime
					;
		}
		bool operator!=(const CBackupSettings_v1 &that) const
		{
			return !operator==(that);
		}

		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt )
				<< m_bMaxSizeOn << m_bMaxTimeOn << m_nMaxSize << m_nMaxTime;
		}
		
		bool m_bMaxSizeOn;
		bool m_bMaxTimeOn;
		int m_nMaxSize;
		int m_nMaxTime;
	};

	struct CCIFACEEXPORT CBackupSettings_v2 : public DeclareNextVersion<CBackupSettings_v1>
	{
		typedef Private::EOverflowQBAction EOverflowQBAction;
		EOverflowQBAction	m_eOverflowAction;	//!< Действие бэкапа если его размер превышает установленный.
		CQBRestore			m_Restore;			//!< Настройки восстановления.
		
		CBackupSettings_v2 ()
        :   m_eOverflowAction ( eOverflowQBAction_Notify )
        {
        }

		bool operator==(const CBackupSettings_v2 &that) const
		{
			return CBackupSettings_v1::operator==(that)
					&& m_eOverflowAction == that.m_eOverflowAction
					&& m_Restore == that.m_Restore;
		}
		bool operator!=(const CBackupSettings_v2 &that) const
		{
			return !operator==(that);
		}
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << SIMPLE_DATA_SER(m_eOverflowAction) << m_Restore;
		};
	};

	struct CCIFACEEXPORT CBackupSettings_v3 : public DeclareNextVersion<CBackupSettings_v2>
	{
		std::wstring m_FilesPath;

		bool operator==(const CBackupSettings_v3 &that) const
		{
			return CBackupSettings_v2::operator==(that)
					&& m_FilesPath == that.m_FilesPath
					;
		}
		bool operator!=(const CBackupSettings_v3 &that) const
		{
			return !operator==(that);
		}

		virtual PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_FilesPath;
		};
	};
	
    struct CCIFACEEXPORT CAVSettings_v1 : public CCBaseMarshal < PID_USER_SETTINGS_AV >
    {
        enum EAVDBType { eNormal, eExtended, eParanoid };
        
        CAVSettings_v1 ()
        {
#ifdef __WKS
            m_AVDBType = eExtended;
#else
            m_AVDBType = eNormal;
#endif
        }
        EAVDBType m_AVDBType;
        
        virtual PoliType & MarshalImpl ( PoliType & pt )
        {
            return BaseMarshal::MarshalImpl ( pt ) << SIMPLE_DATA_SER ( m_AVDBType );
        }
		
		bool operator==(const CAVSettings_v1 &that) const
		{
			return m_AVDBType == that.m_AVDBType;
		}

		bool operator!=(const CAVSettings_v1 &that) const
		{
			return !operator==(that);
		}
	};
} // namespace Private

typedef DeclareExternalName<Private::CQuarantineSettings_v3> CQuarantineSettings;
typedef DeclareExternalName<Private::CBackupSettings_v3> CBackupSettings;
typedef DeclareExternalName<Private::CAVSettings_v1> CAVSettings;

//******************************************************************************
// 
//******************************************************************************
namespace Private
{
	class CCIFACEEXPORT CUserSettings_v1 : public CCBaseMarshal<PID_USER_SETTINGS>
	{
	public:
		CUserSettingsRTP			m_RTPSettings;
		CUserSettingsUpdater		m_UpdaterSettings;
		CUserSettingsUpdaterGUI		m_UpdaterSettingsGUI;
		CUserSettingsODS			m_ODSSettings;
		CUserSettingsGUI			m_GUISettings;
		CCSelectObjectDlg_Objects	m_SelectObjectSettingsGUI;
		CScheduleSettings			m_ScheduleSettings;
		CQuarantineSettings			m_QuarantineSettings;
		// не забыть про применение настроек при импорте из файла!
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt )
				<< m_RTPSettings
				<< m_UpdaterSettings
				<< m_ODSSettings
				<< m_GUISettings
				<< m_SelectObjectSettingsGUI
				<< m_UpdaterSettingsGUI
				<< m_ScheduleSettings
				<< m_QuarantineSettings;
		}
		
	};
	
	class CCIFACEEXPORT CUserSettings_v2 : public DeclareNextVersion<CUserSettings_v1>
	{
	public:
		CBackupSettings	m_BackupSettings;
		// не забыть про применение настроек при импорте из файла!
		
		virtual PoliType & MarshalImpl ( PoliType & pt ){return BaseMarshal::MarshalImpl ( pt ) << m_BackupSettings;}
	};
	
	class CCIFACEEXPORT CUserSettings_v3 : public DeclareNextVersion<CUserSettings_v2>
	{
	public:
		CAVSettings	m_AVSettings;
		// не забыть про применение настроек при импорте из файла!

		CExcludeList m_ExcludeList; //!< Список исключений с вердиктами для всех задач.
		
		virtual PoliType &MarshalImpl (PoliType &pt)
		{
			return BaseMarshal::MarshalImpl (pt) << m_AVSettings << m_ExcludeList;
		}
	};
} //end of Private

typedef DeclareExternalName < Private::CUserSettings_v3 > CUserSettings;

#include "UserSettingsPolicy.h"

#endif // __USERSETTINGS_H__