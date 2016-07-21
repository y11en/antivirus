#include "stdafx.h"
#include "IDSClient.h"
#include "ClientIDs.h"

CIDSClient::CIDSClient (CKAHUM_IDS *pKAHUM) :
	m_uFilterID (0),
	m_pKAHUM (pKAHUM)
{
}

CIDSClient::~CIDSClient ()
{
	StopClient ();
}

bool CIDSClient::Create (HANDLE hShutdownEvent)
{
	if (!CGuardClient::Create (hShutdownEvent, CLIENT_APP_ID_IDS, AVPG_STANDARTRIORITY, _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE /*| _CLIENT_FLAG_SAVE_FILTERS*/))
	{
		return false;
	}

	if (!IsFiltersPresent())
		return AddIDSFilter ();

	return true;	
}

bool CIDSClient::AddIDSFilter ()
{
	log.WriteFormat (_T("[IDSCLIENT] Adding IDS filter..."), PEL_INFO);

	if (IsFiltersPresent ())
	{
		log.WriteFormat (_T("[IDSCLIENT] IDS filter already present"), PEL_INFO);
		return false;
	}

	CGuardFilter filter (FLTTYPE_KLICK, 1, 0, FLT_A_INFO | FLT_A_DENY);

	DWORD dwOrder = -1;
	m_uFilterID = AddFilter (filter, dwOrder);

	bool bFilterSet = m_uFilterID != 0;

	if (bFilterSet)
		log.WriteFormat (_T("[IDSCLIENT] IDS filter successfully added"), PEL_INFO);
	else
		log.WriteFormat (_T("[IDSCLIENT] Failed to add IDS filter"), PEL_ERROR);

	return bFilterSet;
}

void CIDSClient::RemoveIDSFilter ()
{
	RemoveAllFilters ();
	m_uFilterID = 0;
}

void CIDSClient::OnStopClient (BOOL bResidend)
{
	if (!bResidend)
		RemoveIDSFilter ();
}

void CIDSClient::StopClient ()
{
	if (m_DriverContext)
		DRV_ChangeActiveStatus( m_DriverContext, FALSE );

	CGuardClient::StopClient (false);
}

VERDICT CIDSClient::OnEvent( PVOID pMessage )
{
	return m_pKAHUM ? m_pKAHUM->OnEvent (pMessage) : Verdict_Pass;
}
