#ifndef _popupchk_task_structs_
#define _popupchk_task_structs_

#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

#include <AV/structs/s_avs.h>
#include <PopupChecker/plugin/p_popupchk.h>
#include <ProductCore/structs/s_taskmanager.h>


typedef enum
{
		e_cPopupCheckRequest = 1,
} _popupchk_structs_ids;


//-----------------------------------------------------------------------------
//! Settings

struct cPopupCheckSettings : public cTaskSettings
{
	cPopupCheckSettings(): cTaskSettings(),
		m_bAllowFromLocalZone(cTRUE),
		m_bAllowFromTrustedZone(cTRUE),
		m_bAllowFromIntranetZone(cTRUE)
	{}
	
	DECLARE_IID_SETTINGS(cPopupCheckSettings, cTaskSettings, PID_POPUPCHK);

	tBOOL				m_bAllowFromLocalZone;
	tBOOL				m_bAllowFromTrustedZone;
	tBOOL				m_bAllowFromIntranetZone;
	cVector<cEnabledStrItem>	m_vAllowDomainsList;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPopupCheckSettings, 0 )
	SER_BASE( cTaskSettings, 0 )
	SER_VALUE_M(bAllowFromLocalZone,	tid_BOOL)
	SER_VALUE_M(bAllowFromTrustedZone,	tid_BOOL)
	SER_VALUE_M(bAllowFromIntranetZone,	tid_BOOL)
	SER_VECTOR_STREAM(m_vAllowDomainsList,cEnabledStrItem::eIID, "vAllowDomainsList")
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
//! Statistics

struct cPopupCheckStatistics : public cTaskStatistics
{
	cPopupCheckStatistics(): cTaskStatistics(),
		m_nPopupsChecked(0),
		m_nPopupsBlocked(0),
		m_nAllowDomainsListCount(0)
	{}

	DECLARE_IID_STATISTICS(cPopupCheckStatistics, cTaskStatistics, PID_POPUPCHK);

	tUINT             m_nPopupsChecked;             // Number of pop-ups checked
	tUINT             m_nPopupsBlocked;             // Number of pop-ups blocked
	tUINT             m_nAllowDomainsListCount;		// Number of domains in allow list
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPopupCheckStatistics, 0 )
	SER_BASE( cTaskStatistics, 0 )
	SER_VALUE_M( nPopupsChecked,         tid_UINT )
	SER_VALUE_M( nPopupsBlocked,         tid_UINT )
	SER_VALUE_M( nAllowDomainsListCount, tid_UINT )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//! Check Request

struct cPopupCheckRequest : public cInfectedObjectInfo
{
	cPopupCheckRequest(): 
		cInfectedObjectInfo(),
		m_sSourceUrl(),
		m_sDestinationUrl(),
		m_nSourceUrlZone(-1),
		m_nDestinationUrlZone(-1),
		m_pCallbackFn(NULL),
		m_pCallbackCtx(NULL)
	{
	}

	DECLARE_IID(cPopupCheckRequest, cInfectedObjectInfo, PID_POPUPCHK, e_cPopupCheckRequest);

	cStringObj        m_sSourceUrl;             
	cStringObj        m_sDestinationUrl;
	tDWORD            m_nSourceUrlZone;
	tDWORD            m_nDestinationUrlZone;
	tPTR              m_pCallbackFn;
	tPTR              m_pCallbackCtx;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPopupCheckRequest, 0 )
	SER_BASE( cInfectedObjectInfo, 0 )
	SER_VALUE_M( sSourceUrl,         tid_STRING_OBJ )
	SER_VALUE_M( sDestinationUrl,    tid_STRING_OBJ )
	SER_VALUE_M( nSourceUrlZone,     tid_DWORD )
	SER_VALUE_M( nDestinationUrlZone, tid_DWORD )
	SER_VALUE_M( pCallbackFn,        tid_PTR )
	SER_VALUE_M( pCallbackCtx,       tid_PTR )
IMPLEMENT_SERIALIZABLE_END()

#endif // _popupchk_task_structs_

