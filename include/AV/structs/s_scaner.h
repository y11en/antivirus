#ifndef __S_SACNER_H
#define __S_SACNER_H

#include <AV/plugin/p_ods.h>
#include <AV/structs/s_avs.h>

#include <ProductCore/common/UserBan.h>
#include <ProductCore/structs/s_taskmanager.h>

//-----------------------------------------------------------------------------
// ScanObject structure

enum enScanObjType
{
	OBJECT_TYPE_MY_COMPUTER          = 0x0,
	OBJECT_TYPE_DRIVE                = 0x1,
	OBJECT_TYPE_FOLDER               = 0x2,
	OBJECT_TYPE_FILE                 = 0x3,
	OBJECT_TYPE_MEMORY               = 0x4,
	OBJECT_TYPE_STARTUP              = 0x5,
	OBJECT_TYPE_MAIL                 = 0x6,
	OBJECT_TYPE_ALL_DRIVES           = 0x7,
	OBJECT_TYPE_ALL_REMOVABLE_DRIVES = 0x8,
	OBJECT_TYPE_ALL_FIXED_DRIVES     = 0x9,
	OBJECT_TYPE_ALL_NETWORK_DRIVES   = 0xa,
	OBJECT_TYPE_BOOT_SECTORS         = 0xb,
	OBJECT_TYPE_MASKED               = 0xc, // masked object (file or folder)
	OBJECT_TYPE_QUARANTINE           = 0xd,
	OBJECT_TYPE_SYSTEM_RESTORE       = 0xe,
};

//-----------------------------------------------------------------------------

typedef cVector<cScanObject> scanobjarr_t;

struct cScanObjects : public cProtectionSettings
{
	cScanObjects() :
		m_bScanRemovable(false),
		m_bScanFixed(true),
		m_bScanNetwork(false),
		m_bScanSectors(true)
	{}
	
	#define PID_OAS  ((tPID)(49023))
	DECLARE_IID( cScanObjects, cProtectionSettings, PID_OAS, 3 );

	scanobjarr_t    m_aScanObjects;

	tBOOL           m_bScanRemovable;
	tBOOL           m_bScanFixed;
	tBOOL           m_bScanNetwork;
	tBOOL           m_bScanSectors;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScanObjects, 0 )
	SER_BASE( cProtectionSettings,  0 )
	SER_VECTOR( m_aScanObjects,     cScanObject::eIID, "ScanObjects" )
	SER_VALUE( m_bScanRemovable,   tid_BOOL,          "ScanRemovable" )
	SER_VALUE( m_bScanFixed,       tid_BOOL,          "ScanFixed" )
	SER_VALUE( m_bScanNetwork,     tid_BOOL,          "ScanNetwork" )
	SER_VALUE( m_bScanSectors,     tid_BOOL,          "ScanSectors" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cOASSettings structure
/*
enum enWorkingMode
{
	wmOnAccess  = 0x0,
	wmOnModify  = 0x1,
	wmOnExecute = 0x2,
	wmOnOpen    = 0x3,
	wmOnSmart   = 0x4,
};

struct cOASSettings : public cScanObjects
{
	cOASSettings() :
		m_nPostponeSizeLimit(10, cFALSE),
		m_nBanPeriod(2, cFALSE),
		m_nWorkingMode(wmOnSmart)
	{}

	DECLARE_IID_SETTINGS( cOASSettings, cScanObjects, PID_OAS );

	cSettingsCheckT<long>  m_nPostponeSizeLimit;
	cSettingsCheckT<long>  m_nBanPeriod;
	enWorkingMode          m_nWorkingMode;
	tBOOL                  m_bSendNotification;
	cStrObj                m_sSendTemplate;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cOASSettings, "OnAccessSettings" )
	SER_BASE( cScanObjects,  0 )
	SER_VALUE( m_nPostponeSizeLimit.m_on,  tid_BOOL,     "UseCompoundPostponeSize" )
	SER_VALUE( m_nPostponeSizeLimit.m_val, tid_DWORD,    "CompoundPostponeSize" )
	SER_VALUE( m_nWorkingMode,             tid_DWORD,    "WorkingMode" )
	SER_VALUE( m_nBanPeriod.m_on,          tid_BOOL,     "BanUser" )
	SER_VALUE( m_nBanPeriod.m_val,         tid_DWORD,    "BanUserPeriod" )
	SER_VALUE( m_bSendNotification,        tid_BOOL,     "SendNotification" )
	SER_VALUE( m_sSendTemplate,            tid_STRING_OBJ, "SendTemplate" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cOASStatistics structure

struct cOASStatistics : public cProtectionStatisticsEx
{
	DECLARE_IID_STATISTICS( cOASStatistics, cProtectionStatisticsEx, PID_OAS );

	cUserBanList           m_aBanList;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cOASStatistics, 0 )
	SER_BASE( cProtectionStatisticsEx,  0 )
	SER_VALUE( m_aBanList,  cUserBanList::eIID, "BanList" )
IMPLEMENT_SERIALIZABLE_END()
*/
//-----------------------------------------------------------------------------

struct cODSSettings : public cScanObjects
{
    cODSSettings() :
		m_bScanMemory(true),
		m_bScanStartup(true),
		m_bScanMail(true),
		m_bScanNTFSStreams(true),
		m_bCountProgress(true),
		m_bSyncPostponeProcess(false),
		m_bRootkitScan(true),
		m_bDeepRootkitScan(false)
	{
		m_bScanArchived = true;
		m_bScanSFXArchived = true;
	}

	DECLARE_IID_SETTINGS( cODSSettings, cScanObjects, PID_ODS );

	tBOOL           m_bScanMemory;
	tBOOL           m_bScanStartup;
	tBOOL           m_bScanMail;
	tBOOL           m_bScanNTFSStreams;
	tBOOL           m_bCountProgress;
	tBOOL           m_bSyncPostponeProcess;
	tBOOL           m_bRootkitScan;
	tBOOL           m_bDeepRootkitScan;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cODSSettings, "OnDemandSettings" )
	SER_BASE(  cScanObjects,   0 )
	SER_VALUE( m_bScanMemory,      tid_BOOL, "ScanMemory" )
	SER_VALUE( m_bScanStartup,     tid_BOOL, "ScanStartUpObjects" )
	SER_VALUE( m_bScanMail,        tid_BOOL, "ScanMail" )
	SER_VALUE( m_bScanNTFSStreams, tid_BOOL, "ScanNTFSStreams" )
	SER_VALUE( m_bCountProgress,   tid_BOOL, "CountProgress" )
	SER_VALUE( m_bSyncPostponeProcess,   tid_BOOL, "SyncPostponeProcess" )
	SER_VALUE( m_bRootkitScan,   tid_BOOL, "RootkitScan" )
	SER_VALUE( m_bDeepRootkitScan,   tid_BOOL, "DeepRootkitScan" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cProtectionStatistics structure

struct cODSStatistics : public cProtectionStatisticsEx
{
	cODSStatistics() :
		m_nCurObject(0),
		m_nReinitCompletion(0),
		m_nReinitMask(0)
	{}

	DECLARE_IID_STATISTICS( cODSStatistics, cProtectionStatisticsEx, PID_ODS );

	tDWORD              m_nCurObject;
	tDWORD              m_nReinitCompletion;
	tDWORD              m_nReinitMask;
	cStrObj             m_sReinitName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cODSStatistics, 0 )
	SER_BASE( cProtectionStatisticsEx, 0 )
	SER_VALUE( m_nCurObject,        tid_DWORD,         0 )
	SER_VALUE( m_nReinitCompletion, tid_DWORD,         0 )
	SER_VALUE( m_nReinitMask,       tid_DWORD,         0 )
	SER_VALUE( m_sReinitName,       tid_STRING_OBJ,    0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cODSRuntimeParams structure

struct cODSRuntimeParams : public cSerializable
{
	cODSRuntimeParams() :
		m_bEnableStopMode(cFALSE),
		m_eStopMode(stop_Shutdown)
	{}

	enum Actions{ GET = eIID, SET = 0 };
	enum StopMode{ stop_Shutdown = 1, stop_Sleep = 2, stop_Hibernate = 3, stop_Restart = 4 };

	DECLARE_IID( cODSRuntimeParams, cSerializable, PID_ODS, 7 );

	tBOOL      m_bEnableStopMode;
	StopMode   m_eStopMode;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cODSRuntimeParams, 0 )
	SER_VALUE( m_bEnableStopMode,  tid_BOOL,  "EnableStopMode" )
	SER_VALUE( m_eStopMode,        tid_DWORD, "StopMode" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#endif//  __S_SACNER_H
