#ifndef __USERSETTINGS_SCANNERS_H__
#define __USERSETTINGS_SCANNERS_H__

#include "DEFS.H"
#include "BaseMarshal.h"

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information

//! Типы объектов сканирования.
enum AVP_OT
{
	AVP_OT_MYCOMPUTER	= 1,
	AVP_OT_DRIVE		= 2,
	AVP_OT_FOLDER		= 3,
	AVP_OT_FILE			= 4,
	AVP_OT_MAIL			= 5,
	AVP_OT_REMOVABLE	= 6,
	AVP_OT_STARTUP		= 7,
	AVP_OT_QUARANTINED_SCAN_AND_CURE = 8,
	AVP_OT_QUARANTINED_SCAN_ONLY = 9,
	AVP_OT_MEMORY_WITHOUT_BUDDIES = 10,
	AVP_OT_DIRECT		= 11,
	AVP_OT_MEMORY		= 12,
	AVP_OT_HARDDRIVES	= 13,
	AVP_OT_BOOT_SECTORS = 14,
	AVP_OT_BOOTCURE_RESULT = 15,
	AVP_OT_BOOT_SECTORS_FIXED = 16,
	AVP_OT_BOOT_SECTORS_REMOVABLE = 17,
	AVP_OT_NETDRIVES = 18,
	AVP_OT_ALL_QUARANTINE = 19,
	
	
	AVP_OT_FAKE			= 255,
};

//! Типы сканирования.
enum EScanType
{
	eScanType_All			= 0x1,	//!< Сканировать все файлы.
	eScanType_Infectable	= 0x2,	//!< Сканировать все файлы, которые могут быть заражены.
	eScanType_InfectableExt	= 0x3,	//!< Сканировать все файлы с расширением, которые могут быть заражены.
};
IMPLEMENT_MARSHAL_ENUM(EScanType);

//! Типы работы iChecker.
enum EICheckerType
{
	eICheckerType_Off		= 0x1,	//!< iChecker отключен.
	eICheckerType_ShortQ	= 0x2,	//!< iChecker в режиме Short Quarantine.
	eICheckerType_LongQ		= 0x3,	//!< iChecker в режиме Long Quarantine.
};
IMPLEMENT_MARSHAL_ENUM(EICheckerType);

//========================================================================================
namespace Private
{
	struct CCIFACEEXPORT CExcludedProcessObject_v1 : public CCBaseMarshal<PID_USER_SETTINGS_EXCLUDED_PROCESS_OBJECT>
	{
		CExcludedProcessObject_v1()
		{
			memset(m_ProcessHash, 0, sizeof(m_ProcessHash));
		}

		std::wstring m_ProcessName;			//!< Имя файла процесса.
		std::wstring m_ProcessDir;			//!< Папка процесса. Может включать маску.
		std::wstring m_OriginalProcessPath;	//!< Путь к файлу, для которого считался хеш.
		unsigned char m_ProcessHash[16];

		bool operator==(const CExcludedProcessObject_v1 &that) const
		{
			return m_ProcessName == that.m_ProcessName &&
					m_ProcessDir == that.m_ProcessDir &&
					m_OriginalProcessPath == that.m_OriginalProcessPath &&
					memcmp(m_ProcessHash, that.m_ProcessHash, 16) == 0;
		}
		bool operator!=(const CExcludedProcessObject_v1 &that) const
		{
			return !operator==(that);
		}

		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) 
					<< m_ProcessName
					<< m_ProcessDir
					<< m_OriginalProcessPath
					<< SIMPLE_DATA_SER(m_ProcessHash);
		}
	};
}

typedef DeclareExternalName<Private::CExcludedProcessObject_v1> CExcludedProcessObject;

//========================================================================================
namespace Private
{
	struct CCIFACEEXPORT CExcludedProcesses_v1 : public CCBaseMarshal<PID_USER_SETTINGS_EXCLUDED_PROCESSES>
	{
		std::vector<CExcludedProcessObject> m_Processes;

		bool operator==(const CExcludedProcesses_v1 &that) const
		{
			return m_Processes == that.m_Processes;
		}
		bool operator!=(const CExcludedProcesses_v1 &that) const
		{
			return !operator==(that);
		}

		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_Processes;
		}
	};
}

typedef DeclareExternalName<Private::CExcludedProcesses_v1> CExcludedProcesses;

//========================================================================================
namespace Private
{
	//! Дополнительные настройки On-Access Scanner для Personal Pro.
	struct CCIFACEEXPORT CUserSettingsOAS_v1: public CCBaseMarshal < PID_USER_SETTINGS_OAS >
	{
		EScanType		m_eScanType;		//!< Тип сканирования.
		bool			m_bScanFixed;		//!< Флаг сканирования жестких дисков.
		bool			m_bScanRemovable;	//!< Флаг сканирования переносимых дисков.
		bool			m_bScanNetwork;		//!< Флаг сканирования сетевых дисков.
		bool			m_bScanOle;			//!< Флаг сканирования составных OLE-объектов.
		bool			m_bScanPacked;		//!< Флаг сканирования упакованных объектов.
		bool			m_bScanSfxArchived;	//!< Флаг сканирования самораспаковывающихся архивов.
		EICheckerType	m_eUseIChecker;		//!< Флаг использования технологии iChecker.
		EICheckerType	m_eUseIStreams;		//!< Флаг использования технологии iStreams.
		
		
		PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_eScanType << m_bScanFixed << m_bScanRemovable
				<< m_bScanNetwork << m_bScanOle << m_bScanPacked << m_bScanSfxArchived << m_eUseIChecker
				<< m_eUseIStreams;
		}
	};
	struct CCIFACEEXPORT CUserSettingsOAS_v2: public DeclareNextVersion<CUserSettingsOAS_v1>
	{
		enum ERTPUserAction
		{
			eUserAction_DenyAccessAskUser		=	0x1,
			eUserAction_DenyAccessDoNotAskUser	=	0x2,
			eUserAction_Recommended			    =	0x3,
			eUserAction_AutoDel				    =	0x4,
			eUserAction_CureDeleteIfNotCured	=	0x5,
			eUserAction_Quarantine				=	0x6  // for suspics only
		};

		bool			m_bScanBootSectors;		//!< Флаг сканирования boot секторов.
		bool			m_bScanTimeLimit;		//!< Флаг ограничения времени сканирования.
		int				m_nMaxScanTimeSec;		//!< Максимальное время сканирования в секундах.
		ERTPUserAction  m_eUserAction;			//!< Действие над зараженными объекты.
		ERTPUserAction	m_eUserSuspiciousAction;//!< Действие над подозрительными объектами (PPro only).

		PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl ( pt ) << m_bScanBootSectors << m_bScanTimeLimit
				<< m_nMaxScanTimeSec << SIMPLE_DATA_SER(m_eUserAction) << SIMPLE_DATA_SER(m_eUserSuspiciousAction);
		}
	};
	struct CCIFACEEXPORT CUserSettingsOAS_v3 : public DeclareNextVersion<CUserSettingsOAS_v2>
	{
		CUserSettingsOAS_v3() :
			m_bEnableExcludedProcesses(true)
		{
		}

		bool m_bEnableExcludedProcesses;		// использовать список исключаемых процессов
		CExcludedProcesses m_ExcludedProcesses; // исключаемые процессы
												// сейчас не влияют ни на уровень, ни на сравнение

		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_bEnableExcludedProcesses << m_ExcludedProcesses;
		}
				
	};	
} // namespace Private

struct CCIFACEEXPORT CUserSettingsOAS : public DeclareExternalName<Private::CUserSettingsOAS_v3>
{
	CUserSettingsOAS ();

	bool operator == (const CUserSettingsOAS& Settings) const;
	
	bool IsDefault() const;

	void PPro_SetMaximalProtection();
	void PPro_SetRecommended();
	void PPro_SetMaximalSpeed();

	bool PPro_IsMaximalProtection() const;
	bool PPro_IsRecommended() const;
	bool PPro_IsMaximalSpeed() const;
};


namespace Private
{

class CCIFACEEXPORT CUserSettingsODS_v1 : public CCBaseMarshal<PID_USER_SETTINGS_ODS>
{
public:
	enum EODSUserAction
    {
		eUserAction_Ask						= 0x2,
		eUserAction_AutoRec					= 0x3,
		eUserAction_AutoDel					= 0x4,
		eUserAction_ReportOnly				= 0x5,
		eUserAction_TIF						= 0x6,
		eUserAction_CureDeleteIfNotCured	= 0x7,
		eUserAction_Quarantine				= 0x8  // for suspics only
	};
	
	//! Уровень сканирования.
	enum EODSScanLevel
	{
		eScanLevel_Max          = 0x1,
		eScanLevel_Rec          = 0x2,
		eScanLevel_Min          = 0x3,
		eScanLevel_Custom       = 0x4, //!< Уровень для Personal Pro.

		eScanLevel_Last
	};
	
	
	EODSUserAction                  m_UserAction;
	//! В Personal Pro влияет только на положение рычага в GUI.	Сканирование осуществляется в соответствии с флагами.
	EODSScanLevel                   m_ScanLevel;
	bool                            m_blScanSectors;
	bool                            m_blScanMemory;
	bool                            m_blScanPacked;
	bool                            m_blScanArchives;
	bool                            m_blScanStartup;
	bool                            m_blScanMail;
	bool					        m_blExcludeEnable;
	std::vector < std::wstring >    m_ExcludeMaskDeprecated;
	bool							m_blOkInfoEventsOff;
    bool                            m_bDoNotAskPassword;
	
	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		return BaseMarshal::MarshalImpl  (pt)<<SIMPLE_DATA_SER(m_UserAction)<<SIMPLE_DATA_SER(m_ScanLevel)<<m_blScanSectors<<\
			m_blScanMemory<<m_blScanPacked<<m_blScanArchives<<m_blScanStartup<<m_blScanMail<<m_blExcludeEnable<<m_ExcludeMaskDeprecated<<m_blOkInfoEventsOff << m_bDoNotAskPassword;
	}
	
};

// Вот так уж исторически получилось, что этот класс нужен...
class CCIFACEEXPORT CUserSettingsODS_v2 : public DeclareNextVersion<CUserSettingsODS_v1>
{
};	

class CCIFACEEXPORT CUserSettingsODS_v3 : public DeclareNextVersion<CUserSettingsODS_v2>
{
public:
	// Настройки Personal Pro
	EScanType		m_eScanType;	//!< Тип сканирования.
	bool			m_bUseSizeLimit;	//!< Флаг ограничения на размер сканируемых файлов.
	DWORD			m_dwSizeLimit;		//!< Ограничение на размер сканируемых файлов в байтах.
	bool			m_bUseTimeLimit;	//!< Флаг ограничения на время сканирования файла.
	DWORD			m_dwTimeLimit;		//!< Ограничение на время сканирования файла в секундах.
	bool			m_bScanSfxArchived;	//!< Флаг сканирования самораспаковывающихся архивов.
	bool			m_bScanOle;			//!< Флаг сканирования составных OLE-объектов.
	bool			m_bScanNtfsStreams;	//!< Флаг сканирования альтернативных NTFS-потоков.
	EICheckerType	m_eUseIChecker;		//!< Флаг использования технологии iChecker
	EICheckerType	m_eUseIStreams;		//!< Флаг использования технологии iStreams
	bool			m_bScanPlainMail;	//!< Флаг сканирования почтовых файлов.
	bool			m_bScanMailDB;		//!< Флаг сканирования почтовых баз.

	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		BaseMarshal::MarshalImpl (pt);
		return pt << m_eScanType
			<< m_bUseSizeLimit << m_dwSizeLimit << m_bUseTimeLimit << m_dwTimeLimit
			<< m_bScanSfxArchived << m_bScanOle << m_bScanNtfsStreams
			<< m_eUseIChecker << m_eUseIStreams
			<< m_bScanPlainMail << m_bScanMailDB;
	}
};

class CCIFACEEXPORT CUserSettingsODS_v4 : public DeclareNextVersion<CUserSettingsODS_v3>
{
public:
	EODSUserAction m_UserSuspiciousAction;	//!< Действие для подозрительных объектов. (PPro only)
	
	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		return BaseMarshal::MarshalImpl (pt) << SIMPLE_DATA_SER(m_UserSuspiciousAction);
	}
};

class CCIFACEEXPORT CUserSettingsODS_v5 : public DeclareNextVersion<CUserSettingsODS_v4>
{
public:
	bool	m_blScanSectorsOnFixedOnStartup;
	bool	m_blScanMemoryOnStartup;
	bool	m_blScanStartupOnStartup;
	bool	m_blScanSectorsOnRemovableOnStartup;
	
	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		return BaseMarshal::MarshalImpl (pt) 
				<< m_blScanSectorsOnFixedOnStartup
				<< m_blScanMemoryOnStartup
				<< m_blScanStartupOnStartup
				<< m_blScanSectorsOnRemovableOnStartup;
	}
};

}

class CCIFACEEXPORT CUserSettingsODS: public DeclareExternalName<Private::CUserSettingsODS_v5>
{
public:
	CUserSettingsODS ();

	void SetRecommended ();
	bool IsExclude () const;
	bool IsDefault() const;

	void PPro_SetMaximalProtection();
	void PPro_SetRecommended();
	void PPro_SetMaximalSpeed();
	
	bool PPro_IsMaximalProtection() const;
	bool PPro_IsRecommended() const;
	bool PPro_IsMaximalSpeed() const;
};

namespace Private {

//! Дополнительные настройки Mail Checker для Personal Pro.
struct CCIFACEEXPORT CUserSettingsMC_v1: public CCBaseMarshal < PID_USER_SETTINGS_MC >
{
	//! Типы действий над зараженными почтовыми объектыми.
	enum EMCInfectedAction
	{
		eMCInfectedAction_Disinfect = 0x1,	//!< Лечить, удалять если не лечится.
		eMCInfectedAction_Delete = 0x2,		//!< Удалять.
	};
	
	//! Типы действий над зараженными почтовыми объектыми.
	enum EMCSuspiciousAction
	{
		eMCSuspiciousAction_Quarantine = 0x1,	//!< Помещать в карантин.
		eMCSuspiciousAction_Delete = 0x2,		//!< Удалять.
	};

	EScanType			m_eScanType;		//!< Тип сканирования.
	EMCInfectedAction	m_eInfectedMailAction;	//!< Действие на зараженные объекты.
	EMCSuspiciousAction	m_eSuspiciousMailAction;	//!< Действие на подзрительные объекты.
	bool	m_bScanOutlookIncoming;	//!< Флаг сканирования входящей почты Outlook.
	bool	m_bScanOutlookOutgoing;	//!< Флаг сканирования исходящей почты Outlook.
	bool	m_bScanPop3;			//!< Флаг сканирования POP3-трафика.
	DWORD	m_dwPop3Port;			//!< Порт POP3.
	bool	m_bScanSmtp;			//!< Флаг сканирования SMTP-трафика.
	DWORD	m_dwSmtpPort;			//!< Порт SMTP.
	bool	m_bScanArchives;		//!< Флаг сканирования архивов.
	bool	m_bLimitArchiveSize;	//!< Флаг ограничения размера сканируемых архивов.
	DWORD	m_nMaxArchiveSizeMb;	//!< Максимальный размер сканируемого архива в мегабайтах.
	bool	m_bScanTimeLimit;		//!< Флаг ограничения времени сканирования.
	int		m_nMaxScanTimeSec;		//!< Максимальное время сканирования в секундах.
	bool	m_bScanAttachedMailBases;//!< Флаг сканирования прикреплённых файлов почтовых форматов.
	bool	m_bUseIChecker;			//!< Флаг использования IChecker.

	PoliType & MarshalImpl ( PoliType & pt )
    {
        return BaseMarshal::MarshalImpl ( pt ) << m_eScanType
			<< SIMPLE_DATA_SER(m_eInfectedMailAction) << SIMPLE_DATA_SER(m_eSuspiciousMailAction)
			<< m_bScanOutlookIncoming << m_bScanOutlookOutgoing << m_bScanPop3 << m_dwPop3Port
			<< m_bScanSmtp << m_dwSmtpPort << m_bScanArchives << m_bLimitArchiveSize
			<< m_nMaxArchiveSizeMb << m_bScanTimeLimit << m_nMaxScanTimeSec << m_bScanAttachedMailBases
			<< m_bUseIChecker;
	}
};
}

struct CCIFACEEXPORT CUserSettingsMC: public DeclareExternalName<Private::CUserSettingsMC_v1>
{
	CUserSettingsMC();
	bool operator == (const CUserSettingsMC& Settings) const;
	
	bool IsDefault() const
	{
		return *this == CUserSettingsMC();
	}

	void PPro_SetMaximalProtection();
	void PPro_SetRecommended();
	void PPro_SetMaximalSpeed();
	
	bool PPro_IsMaximalProtection() const;
	bool PPro_IsRecommended() const;
	bool PPro_IsMaximalSpeed() const;
};

namespace Private
{
	struct CCIFACEEXPORT CCODSScanObject_v1 : public CCBaseMarshal<PID_USER_SETTINGS_SCAN_OBJECT>
	{
		DWORD m_Type;
		std::wstring m_Name;
		
		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl  (pt)<<m_Type<<m_Name;
		}
	};
	
	struct CCIFACEEXPORT CCODSScanObject_v2 : public DeclareNextVersion<CCODSScanObject_v1>
	{
		int m_Action;
		std::vector < unsigned char > m_DirectScanData; //m_Type = AVP_OT_DIRECT;
		
		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl  (pt)<<m_Action<<m_DirectScanData;
		}
	};
	
	struct CCIFACEEXPORT CCODSScanObject_v3 : public DeclareNextVersion<CCODSScanObject_v2>
	{
		int m_ActionOnWarning;
		DWORD m_dwObjectUID;

		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl  (pt)<<m_ActionOnWarning<<m_dwObjectUID;
		}
	};

	struct CCIFACEEXPORT CCODSScanObject_v4 : public DeclareNextVersion<CCODSScanObject_v3>
	{
		// Если указано, то в отчёте отображается не m_Name, а m_ReportName (Объекты карантина отображаются иначе)
		std::wstring m_ReportName;
		
		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl  (pt)<<m_ReportName;
		}
	};
}

typedef DeclareExternalName<Private::CCODSScanObject_v4> CCODSScanObject_Ser;

struct CCIFACEEXPORT CCODSScanObject : public CCODSScanObject_Ser
{
	CCODSScanObject ()
	{
	}
	
	CCODSScanObject ( DWORD Type, const std::wstring & Name = L"" )
	{
		m_Type = Type;
		m_Name = Name;
	}
	
	CCODSScanObject (int type, unsigned char * pDirectScanData, size_t nDataSize, int action )
	{
		m_Type = type;
		m_Action = action;
		if (nDataSize && pDirectScanData)
			m_DirectScanData.assign (pDirectScanData, pDirectScanData + nDataSize);
	}
	
	
};

namespace Private
{
	struct CCIFACEEXPORT CCODSScanObjects_v1 : public CCBaseMarshal<PID_USER_SETTINGS_SCAN_OBJECTS>
	{
		CCODSScanObjects_v1 ()
		{
		}
		
		CCODSScanObjects_v1 ( const CCODSScanObjects_v1 & That )
		{
			m_List = That.m_List;
		}
		
		std::list < CCODSScanObject > m_List;
		
		virtual PoliType & MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl  (pt)<<m_List;
		}
	};
	
}

typedef DeclareExternalName<Private::CCODSScanObjects_v1> CCODSScanObjects;

//////////////////////////////////////////////////////////////////////////

//FW
namespace Private
{
	struct CCIFACEEXPORT CUserSettingsFirewall_v1 : public CCBaseMarshal<PID_USER_SETTINGS_FIREWALL>
	{
		CUserSettingsFirewall_v1() :
			m_WorkingMode(wmAllowNotFiltered)
		{
		}

		enum WorkingMode
		{
			wmBlockAll,
			wmRejectNotFiltered,
			wmAskUser,
			wmAllowNotFiltered,
			wmAllowAll
		};
		
		WorkingMode m_WorkingMode;
		std::vector<BYTE> m_PacketRulesBlob;
		std::vector<BYTE> m_ApplicationRulesBlob;

		virtual PoliType & MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl(pt) 
						<< SIMPLE_DATA_SER(m_WorkingMode)
						<< m_PacketRulesBlob
						<< m_ApplicationRulesBlob;
		}
	};
}

typedef DeclareExternalName<Private::CUserSettingsFirewall_v1> CUserSettingsFirewall;

#endif // __USERSETTINGS_SCANNERS_H__