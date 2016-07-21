#ifndef __S_OAS_H
#define __S_OAS_H

#include <OAS/plugin/p_oas.h>
#include <AV/structs/s_scaner.h>

//-----------------------------------------------------------------------------
// cOASSettings structure

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

//-----------------------------------------------------------------------------

#endif//  __S_OAS_H
