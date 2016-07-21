// Stat.cpp : Implementation of CStat
#include "stdafx.h"
#include "InventorySCUM.h"
#include "Stat.h"
#include "..\include\ckahstat.h"

/////////////////////////////////////////////////////////////////////////////
// CStat

STDMETHODIMP CStat::get_pass_count(__int64* nVal)
{
	__int64 nInDropped, nInPassed, nOutDropped, nOutPassed;
	if (CKAHSTAT::GetPassDropPktStat( &nInDropped, &nInPassed, &nOutDropped, &nOutPassed ) == CKAHUM::srOK)
		*nVal=nInPassed + nOutPassed;

	return S_OK;
}

STDMETHODIMP CStat::get_drop_count(__int64 *nVal)
{
	__int64 nInDropped, nInPassed, nOutDropped, nOutPassed;
	if (CKAHSTAT::GetPassDropPktStat( &nInDropped, &nInPassed, &nOutDropped, &nOutPassed ) == CKAHUM::srOK)
		*nVal=nInDropped + nOutDropped;

	return S_OK;
}

STDMETHODIMP CStat::get_current_conn_count(long *nConn)
{
	CKAHSTAT::GetConnectionCount( (ULONG*)nConn );

	return S_OK;
}

STDMETHODIMP CStat::get_dropped_conn_count(long *nConn)
{
	CKAHSTAT::GetDroppedConnectionCount( (ULONG*)nConn );

	return S_OK;
}
