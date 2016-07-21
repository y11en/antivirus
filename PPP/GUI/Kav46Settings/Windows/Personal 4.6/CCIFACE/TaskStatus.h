#ifndef __TASKSTATUS_H__
#define __TASKSTATUS_H__

#include "DEFS.H"
#include "BaseMarshal.h"

struct CUserSettingsRTP;
class CUserSettingsODS;

namespace Private
{
	class CCIFACEEXPORT CProductInfo_v1 : public CCBaseMarshal < PID_PRODUCT_INFO >
	{
	public:
		enum EProductType
		{
			ptUnknown			= 0,
			ptFamilyMask		= 0xFFFF0000,
			ptPersonalFamily	= 0x00010000,
			ptPersonalProFamily = 0x00020000,
			ptWorkstationFamily = 0x00030000,

			ptPersonal = ptPersonalFamily,
			ptPersonalHome,
			ptPersonalHomePolsky,

			ptPersonalPro		= ptPersonalProFamily,
			
			ptWorkstation		= ptWorkstationFamily,
			ptWorkstationLite,
		};
		EProductType m_ProductType;

		bool m_bRTPInstalled;
		bool m_bOASInstalled;
		bool m_bMCInstalled;
		bool m_bSCInstalled;
		bool m_bCKAHInstalled;
		bool m_bOGInstalled;

		bool IsOASInstalled() const { return m_bRTPInstalled && m_bOASInstalled; }
		bool IsMCInstalled() const { return m_bRTPInstalled && m_bMCInstalled; }
		bool IsSCInstalled() const { return m_bRTPInstalled && m_bSCInstalled; }
		bool IsCKAHInstalled() const { return m_bRTPInstalled && m_bCKAHInstalled; }
		bool IsOGInstalled() const { return m_bRTPInstalled && m_bOGInstalled; }
		
		CProductInfo_v1()
		{
			m_bRTPInstalled = true;
			m_bOASInstalled = true;
			m_bMCInstalled = true;
			m_bSCInstalled = true;
			m_bCKAHInstalled = true;
			m_bOGInstalled = true;
			m_ProductType = ptUnknown;
		}
		
		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl ( pt ) << SIMPLE_DATA_SER(m_ProductType)
				<< m_bRTPInstalled << m_bOASInstalled << m_bMCInstalled << m_bSCInstalled
				<< m_bCKAHInstalled << m_bOGInstalled;
		}
	};
}
typedef DeclareExternalName<Private::CProductInfo_v1> CProductInfo;

//******************************************************************************
//
//******************************************************************************
struct CCIFACEEXPORT CRTPGUIStatus : public CCBaseMarshal<PID_PRODUCT_STATUS_RTP>
{
	CRTPGUIStatus() :
		m_Realtime(eChanging),
		m_Settings(eCustom),
		m_OASStatus(eChanging),
		m_MCStatus(eChanging),
		m_SCStatus(eChanging),
		m_CKAHStatus(eChanging),
		m_OGStatus(eChanging),
		m_CKAHRebootRequestReason(eNone)
	{
	}
	
	enum RealtimeStatus
	{
		eOff = 0x1,
		eOn = 0x2,
		eChanging = 0x3,
		eError = 0x4,
		ePaused = 0x5,	//!< Временно приостановлен.
		eNotInstalled = 0x6,

		// Эти статусы может иметь только RTP в целом
		ePartialOn = 0x10,	//!< Некоторые задачи отключены
		ePartialError = 0x11,	//!< Некоторые задачи сфайлили
	};
	
	enum SettingsStatus
	{
		eRec = 0x1,			// рекомендуемый
		eMax = 0x2,			// максимальный
		eCustom = 0x3,		// пользовательский
		eCustomMax = 0x4	// пользовательский, выше рекомендованного
	};

	// причина требования перезагрузки IDS
	enum CKAHRebootRequestReason
	{
		eNone,
		eBaseDriversRemoved,	// включенность базовых драйверов не совпадает c желанием пользователя (администратора)
		eBaseDriversInstalled,
		eBases		// базы требуют перезагрузки	
	};
	
	void SetSettingsStatus (const CUserSettingsRTP& settings, const CProductInfo& productInfo);

	bool operator==(const CRTPGUIStatus & gui_status) const
	{
		return m_Realtime == gui_status.m_Realtime
			&& m_Settings == gui_status.m_Settings
			&& m_OASStatus == gui_status.m_OASStatus
			&& m_MCStatus == gui_status.m_MCStatus
			&& m_SCStatus == gui_status.m_SCStatus
			&& m_CKAHStatus == gui_status.m_CKAHStatus
			&& m_OGStatus == gui_status.m_OGStatus
			&& m_CKAHRebootRequestReason == gui_status.m_CKAHRebootRequestReason
			;
	}
	
	bool operator!=(const CRTPGUIStatus & gui_status) const
	{
		return !operator==(gui_status);
	}
	
	virtual PoliType & MarshalImpl(PoliType& pt);
	    
	RealtimeStatus m_Realtime;	// состояние защиты реального времени
	SettingsStatus m_Settings;	// настройки защиты реального времени

	// Статусы задач
	RealtimeStatus m_OASStatus;
	RealtimeStatus m_MCStatus;
	RealtimeStatus m_SCStatus;
	RealtimeStatus m_CKAHStatus;
	RealtimeStatus m_OGStatus;

	CKAHRebootRequestReason m_CKAHRebootRequestReason;
};

//******************************************************************************
//
//******************************************************************************
struct CCIFACEEXPORT CFullScanStatus : public CCBaseMarshal<PID_PRODUCT_STATUS_FULLSCAN>
{
	CFullScanStatus() :
		m_bRunning(false),
		m_bSettingsRec(false),
		m_bSchedule(false),
		m_bError(false),
		m_bShutdown(false),
		m_nNotifyFullscanStatusPeriod1(14),
		m_nNotifyFullscanStatusPeriod2(30)
	{
	}
	
	bool operator==(const CFullScanStatus &status) const
	{
		return (m_tmLastFullScan == status.m_tmLastFullScan &&
				m_bRunning == status.m_bRunning &&
				m_bSettingsRec == status.m_bSettingsRec &&
				m_bSchedule == status.m_bSchedule &&
				m_bError == status.m_bError &&
				m_bShutdown == status.m_bShutdown &&
				m_tmNextScheduledStart == status.m_tmNextScheduledStart &&
				m_nNotifyFullscanStatusPeriod1 == status.m_nNotifyFullscanStatusPeriod1 &&
				m_nNotifyFullscanStatusPeriod2 == status.m_nNotifyFullscanStatusPeriod2);
	}

	bool operator!=(const CFullScanStatus &status) const
	{
		return !operator==(status);
	}

	virtual PoliType &MarshalImpl(PoliType &pt);
	
	void SetSettingsStatus(
		const CUserSettingsODS& settings,	//!< [in] Настройки FullScan.
		bool bHasExcludes					//!< [in] Флаг того, что есть исключения из проверки.
		);
	
	CSysTimeSer m_tmLastFullScan;			// время последнего сканирования
	bool m_bRunning;						// запущено ли сканирование
	bool m_bSettingsRec;					// были ли настройки рекомендуемыми при последнем сканировании
	bool m_bSchedule;						// запланировано ли сканирование
	bool m_bError;							// завершилось ли последнее сканирование с ошибкой
	bool m_bShutdown;						// начАть shutdown системы
	CSysTimeSer m_tmNextScheduledStart;		// время следующего авто-запуска
	DWORD m_nNotifyFullscanStatusPeriod1;	// порог 1 уведомления о невыполненном fullscan
	DWORD m_nNotifyFullscanStatusPeriod2;	// порог 2 уведомления о невыполненном fullscan
};

//******************************************************************************
//
//******************************************************************************
struct CCIFACEEXPORT CUPDStatus : public CCBaseMarshal<PID_PRODUCT_STATUS_UPDATER>
{
	enum EAVBState
	{
		eAVBOk	= 0x1,
		eAVUpdating,
		eAVWaitingConnection, // net detect
		eAVBDamage,
		eAVBReloadingBases,
		eAVBRollingBack	//!< Загруженные базы повреждены, откатываюсь к прошлым базам
	};
	
	enum EAVEFailReason
	{
		eAVFR_UNKNOWN = -1,
		eAVFR_OK = 0,											// 0, AVS not failed
		eAVFR_PRAGUE_INIT_FAILED,								// 1, PRInitialize failed
		eAVFR_CREATE_ENGINE_OBJ_FAILED,							// 2, engine object creation failed
		eAVFR_NO_BASES_PATH,									// 3, no AV bases path in registry
		eAVFR_LOCK_BASES_FAILED,								// 4, check tool failed to lock bases
		eAVFR_LOCK_BASES_FAILED_INVALID_PARAMETER,				// 5, check tool failed (ошибка во входных параметрах метода)
		eAVFR_LOCK_BASES_FAILED_LOW_RESOURCE,					// 6, check tool failed (недостаточно системных ресурсов (к примеру: нет свободной памяти))
		eAVFR_LOCK_BASES_FAILED_OBJECT_LOCK_FAILED,				// 7, check tool failed (объект(ы) не залочен)
		eAVFR_LOCK_BASES_FAILED_INIT_FAILED,					// 8, check tool failed (внутренняя ошибка инициализации)
		eAVFR_LOCK_BASES_FAILED_XML_PARSE_FAILED,				// 9, check tool failed (ошибка разбора xml)
		eAVFR_LOCK_BASES_FAILED_INVALID_SIGN,					// 10, check tool failed (файл не подписан или несоответствие подписи)
		eAVFR_LOCK_BASES_FAILED_INVALID_SIZE,					// 11, check tool failed (размер файла не совпадает с описанием)
		eAVFR_LOCK_BASES_FAILED_PROCESSING_FAILED,				// 12, check tool failed (ошибка обработки)
		eAVFR_LOCK_BASES_FAILED_NOT_FOUND,						// 13, check tool failed (объект не найден)
		eAVFR_CREATE_DTREG_OBJ_FAILED,							// 14, DTReg object creation failed
		eAVFR_INVALID_BASES_CONFIG,								// 15, invalid bases configuration (unspecified reason)
		eAVFR_INVALID_BASES_CONFIG_BAD_CONFIGURATION,			// 16, invalid bases configuration (errENGINE_BAD_CONFIGURATION)
		eAVFR_INVALID_BASES_CONFIG_OS_STOPPED,					// 17, invalid bases configuration (errENGINE_OS_STOPPED)
		eAVFR_INVALID_BASES_CONFIG_IO_MODIFIED,					// 18, invalid bases configuration (errENGINE_IO_MODIFIED)
		eAVFR_INVALID_BASES_CONFIG_ACTION_SKIP,					// 19, invalid bases configuration (errENGINE_ACTION_SKIP)
		eAVFR_INVALID_BASES_CONFIG_CONFIGURATION_NOT_LOADED,	// 20, invalid bases configuration (errENGINE_CONFIGURATION_NOT_LOADED)
		eAVFR_INVALID_BASES_CONFIG_BAD_ACTION_CONFIGURATION,	// 21, invalid bases configuration (errENGINE_BAD_ACTION_CONFIGURATION)
		eAVFR_INVALID_BASES_CONFIG_BAD_REGISTRY_CONFIGURATION,	// 22, invalid bases configuration (errENGINE_BAD_REGISTRY_CONFIGURATION)
		eAVFR_INVALID_BASES_CONFIG_BAD_AVP_SET,					// 23, invalid bases configuration (errENGINE_BAD_AVP_SET)
		eAVFR_INVALID_BASES_CONFIG_MT_DRIVER_NOT_LOADED,		// 24, invalid bases configuration (errENGINE_MT_DRIVER_NOT_LOADED)
		eAVFR_INVALID_BASES_CONFIG_BAD_AVP1_CONFIGURATION,		// 25, invalid bases configuration (errENGINE_BAD_AVP1_CONFIGURATION)
		eAVFR_BASES_INTEGRITY_FAILED,							// 26, AV bases integrity failed
		eAVFR_BASE_ERROR_OPEN									// 27, AV bases open error
	};

	CUPDStatus() :
		m_AVBState(eAVBDamage),
		m_AVEFailReason(eAVFR_OK),
		m_bAutoUpdateOn(false),
		m_bLicensingValid(false),
		m_nKnownVirusBody(0),
		m_bIsTrialKey (false),
		m_bRTPTasksAllowedToRun(true),
		m_nNotifyAVBasesExpiredPeriod1(7),
		m_nNotifyAVBasesExpiredPeriod2(14)
	{
	}
	
	virtual PoliType & MarshalImpl(PoliType& pt);

	bool operator == (const CUPDStatus & status) const
	{
		return (m_AVBState == status.m_AVBState &&
				m_AVEFailReason == status.m_AVEFailReason &&
				m_bAutoUpdateOn == status.m_bAutoUpdateOn &&
				m_bLicensingValid == status.m_bLicensingValid &&
				m_tmLastUpdate == status.m_tmLastUpdate &&
				m_tmUpdateStarted == status.m_tmUpdateStarted &&
				m_bIsTrialKey == status.m_bIsTrialKey && 
				m_bRTPTasksAllowedToRun == status.m_bRTPTasksAllowedToRun && 
				m_tmNextUpdate == status.m_tmNextUpdate && 
				m_tmLastSuccessfulUpdate == status.m_tmLastSuccessfulUpdate &&
				m_sRunningTask == status.m_sRunningTask &&
				m_sScheduledTask == status.m_sScheduledTask &&
				m_nNotifyAVBasesExpiredPeriod1 == status.m_nNotifyAVBasesExpiredPeriod1 &&
				m_nNotifyAVBasesExpiredPeriod2 == status.m_nNotifyAVBasesExpiredPeriod2);
	}

	bool operator != (const CUPDStatus & status) const
	{
		return !operator == (status);
	}

	EAVBState m_AVBState;					// состояние updater-а
	EAVEFailReason	m_AVEFailReason;		// причина незапуска движка
	bool m_bAutoUpdateOn;					// запланировано ли обновление
	bool m_bLicensingValid;					// как дела с лицензией
	bool m_bIsTrialKey;						// используется ли триальный ключ
	CSysTimeSer m_tmLastUpdate;				// версия баз
	CSysTimeSer m_tmUpdateStarted;			// время, в которое начато обновление (m_AVBState == eAVUpdating)
	long m_nKnownVirusBody;					//записей в базе
	bool m_bRTPTasksAllowedToRun;
	CSysTimeSer m_tmNextUpdate;
	CSysTimeSer m_tmLastSuccessfulUpdate;	// дата/время последнего успешного обновления баз
	std::string m_sRunningTask;				//!< CC_TASK_ имя запущенной задачи
	std::string m_sScheduledTask;			//!< CC_TASK_ имя для ближайшей стартующей задачи.
	DWORD m_nNotifyAVBasesExpiredPeriod1;	// порог 1 уведомления об устаревании баз
	DWORD m_nNotifyAVBasesExpiredPeriod2;	// порог 2 уведомления об устаревании баз
};

//******************************************************************************
//
//******************************************************************************
struct CCIFACEEXPORT CProductStatistics : public CCBaseMarshal<PID_PRODUCT_STATISTICS>
{
	unsigned long m_nFileObjects;		//!< Количество провереных OAS и ODS объектов.
	unsigned long m_nScripts;			//!< Количество провереных скриптов.
	unsigned long m_nScriptsBlocked;	//!< Количество блокированных скриптов.
	unsigned long m_nMailObjects;		//!< Количество провереных MC объектов.
	unsigned long m_nMacros;			//!< Количество провереных макросов.
	unsigned long m_nMacrosBlocked;		//!< Количество блокированных макросов.
	unsigned long m_nInfectedObjects;	//!< Количество обнаруженных инфицированных объектов.
	unsigned long m_nSuspiciousDetected;//!< Количество обнаруженных подозрительных объектов (probably contain malicious code).
	unsigned long m_nWarningsDetected;	//!< Количество обнаруженных подозрительных объектов (possibly infected with a virus).
	unsigned long m_nAttacksDetected;	//!< Количество обнаруженных атак.
	
	unsigned long m_nErrors;			//!< Количество ошибок.
	unsigned long m_nFoldersProcessed;	//!< Количество обработаных папок.
	unsigned long m_nBackup;			//!< Количество backuped объектов.
	unsigned long m_nDisinfected;		//!< Количество вылеченных объектов.
	unsigned long m_nRemoved;			//!< Количество удалённых объектов.
	unsigned long m_nQuarantined;		//!< Количество объектов, помещённых в карантин.
	unsigned long m_nNotDisinfected;	//!< Количество объектов, которые не могут быть вылечены.
	unsigned long m_nNotRemoved;		//!< Количество объектов, которые не могут быть удалены.
	unsigned long m_nNotProcessed;		//!< Количество не обработанных объектов.
	unsigned long m_nVirusBodiesFound;	//!< Количество найденных virus bodies.
	unsigned long m_nVirusesDetected;	//!< Количество обнаруженных вирусов.
	
	unsigned long GetProcessedObjects() const
	{
		return m_nFileObjects + m_nMailObjects + m_nScripts + m_nMacros;
	}
	
	CProductStatistics()
		: m_nFileObjects(0), m_nScripts(0), m_nMailObjects(0), m_nMacros(0), m_nMacrosBlocked(0)
		, m_nInfectedObjects(0), m_nSuspiciousDetected(0), m_nWarningsDetected(0)
		, m_nAttacksDetected(0), m_nScriptsBlocked(0)
		, m_nErrors(0), m_nFoldersProcessed(0), m_nBackup(0), m_nDisinfected(0)
		, m_nRemoved(0), m_nQuarantined(0), m_nNotDisinfected(0), m_nNotRemoved(0)
		, m_nNotProcessed(0), m_nVirusBodiesFound(0), m_nVirusesDetected(0)
	{
	}
	virtual PoliType & MarshalImpl(PoliType& pt)
	{
		return BaseMarshal::MarshalImpl  (pt) << m_nFileObjects << m_nScripts << m_nScriptsBlocked
			<< m_nMailObjects << m_nMacros << m_nMacrosBlocked << m_nInfectedObjects << m_nSuspiciousDetected
			<< m_nWarningsDetected << m_nAttacksDetected
			<< m_nErrors << m_nFoldersProcessed << m_nBackup << m_nDisinfected
			<< m_nRemoved << m_nQuarantined << m_nNotDisinfected << m_nNotRemoved
			<< m_nNotProcessed << m_nVirusBodiesFound << m_nVirusesDetected;
	}
};

//******************************************************************************
//
//******************************************************************************
namespace Private
{
struct CCIFACEEXPORT CProductStatus_v1 : public CCBaseMarshal<PID_PRODUCT_STATUS>
{
	//////////////////////////////////////////////////////////////////////////
	// Эти структуры не версионируются! Если маршалить, то не их самих, а их члены!
	//////////////////////////////////////////////////////////////////////////
    CRTPGUIStatus m_RTPStatus;
    CFullScanStatus m_FullScanStatus;
    CUPDStatus m_UpdaterStatus;
	CProductStatistics m_ProductStatistics;
	//////////////////////////////////////////////////////////////////////////

    virtual PoliType & MarshalImpl(PoliType& pt)
    {
        return BaseMarshal::MarshalImpl(pt)
            << m_FullScanStatus.m_tmLastFullScan
			<< m_FullScanStatus.m_bSettingsRec
			<< m_FullScanStatus.m_bError;
    }
    
};

struct CCIFACEEXPORT CProductStatus_v2 : public DeclareNextVersion<CProductStatus_v1>
{
	CProductStatus_v2(): m_bBlackListFound(false) {}

	bool m_bBlackListFound;	//!< В загруженных базах найден "чёрный" список. После этого ключ всегда проверяется по нему. (BUG 16464 FIX)
	
    virtual PoliType & MarshalImpl(PoliType& pt)
    {
        return BaseMarshal::MarshalImpl(pt) << m_bBlackListFound;
    }
    
};

struct CCIFACEEXPORT CProductStatus_v3 : public DeclareNextVersion<CProductStatus_v2>
{
    virtual PoliType &MarshalImpl(PoliType &pt)
    {
        return BaseMarshal::MarshalImpl(pt) << m_UpdaterStatus.m_tmLastSuccessfulUpdate;
    }    
};

struct CCIFACEEXPORT CProductStatus_v4 : public DeclareNextVersion<CProductStatus_v3>
{
	CProductInfo m_ProductInfo;

    virtual PoliType &MarshalImpl(PoliType &pt)
    {
        return BaseMarshal::MarshalImpl(pt) << m_ProductInfo;
    }    
};

}

typedef DeclareExternalName<Private::CProductStatus_v4> CProductStatus;

//******************************************************************************
//
//******************************************************************************
namespace Private
{
struct CCIFACEEXPORT CProductVersion_v1 : public CCBaseMarshal<PID_PRODUCT_VERSION_INFO>
{
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwLanguageId;

	virtual PoliType & MarshalImpl(PoliType& pt);
};

struct CCIFACEEXPORT CProductVersion_v2 : public DeclareNextVersion<CProductVersion_v1>
{
	std::string m_ProductName; // название продукта, типа "Антивирус Касперского Personal Pro"
	CSysTimeSer m_InstallTime; // дата/время установки продукта
	CSysTimeSer m_LastProductUpdate; // дата/время установки последнего патча продукта

	virtual PoliType & MarshalImpl(PoliType& pt);
};

struct CCIFACEEXPORT CProductVersion_v3 : public DeclareNextVersion<CProductVersion_v2>
{
	DWORD dwNagentMajorVersion;
	DWORD dwNagentMinorVersion;
	DWORD dwNagentBuildNumber;

	virtual PoliType & MarshalImpl(PoliType& pt);
};

} //end of Private

struct CCIFACEEXPORT CProductVersion: public DeclareExternalName<Private::CProductVersion_v3>
{
	CProductVersion();

	bool operator==(const CProductVersion &rhs) const
	{
		return dwMajorVersion == rhs.dwMajorVersion
			&& dwMinorVersion == rhs.dwMinorVersion
			&& dwBuildNumber == rhs.dwBuildNumber
			&& m_ProductName == rhs.m_ProductName
			&& m_InstallTime == rhs.m_InstallTime
			&& m_LastProductUpdate == rhs.m_LastProductUpdate
			&& dwNagentMajorVersion == rhs.dwNagentMajorVersion
			&& dwNagentMinorVersion == rhs.dwNagentMinorVersion
			&& dwNagentBuildNumber == rhs.dwNagentBuildNumber
			;
	}
	
	bool operator!=(const CProductVersion &rhs) const
	{
		return !operator==(rhs);
	}
};

#endif // __TASKSTATUS_H__
