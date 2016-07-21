// RebarPos.cpp: implementation of the CRebarPos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RebarPos.h"
#include <boost/scoped_array.hpp>
#include <commctrl.h>

namespace
{
	LPCTSTR BARPOS_VALUE = _T("BarPos");
}

bool CRebarPos::SaveToRegistry(CRegKey& key) const
{
	if (m_arrBands.size() <= 0)
	{
		key.DeleteValue(BARPOS_VALUE);
		return true;
	}

	// form a binary buffer
	size_t dwBufSize = 4 * sizeof(DWORD) * m_arrBands.size();
	boost::scoped_array<BYTE> buf(new BYTE[dwBufSize]);
	for (size_t n = 0; n < m_arrBands.size(); ++n)
	{
		DWORD* pBuf = reinterpret_cast<DWORD*>(&buf[n * 4 * sizeof(DWORD)]);

		pBuf[0] = m_arrBands[n].m_dwID;
		pBuf[1] = m_arrBands[n].m_dwCX;
		pBuf[2] = m_arrBands[n].m_dwIndex;
		pBuf[3] = m_arrBands[n].m_bUseFullLine ? 1 : 0;
	}

	return (RegSetValueEx(key, BARPOS_VALUE, 0, REG_BINARY, buf.get(), (DWORD)dwBufSize) 
		== ERROR_SUCCESS) ? true : false;
}

bool CRebarPos::LoadFromRegistry(CRegKey& key)
{
	m_arrBands.clear();

	DWORD dwBufSize = 200;
	boost::scoped_array<BYTE> buf(new BYTE[dwBufSize]);

	DWORD nType = REG_NONE;
	if (RegQueryValueEx(key, BARPOS_VALUE, 0, &nType, buf.get(), &dwBufSize) != ERROR_SUCCESS)
		return false;

	if ((nType != REG_BINARY) || (dwBufSize < 4) || ((dwBufSize % 4) != 0))
		return false;	// corrupted buffer

	int nBands = dwBufSize / (4 * sizeof(DWORD));
	for (int n = 0; n < nBands; ++n)
	{
		DWORD* pBuf = reinterpret_cast<DWORD*>(&buf[n * 4 * sizeof(DWORD)]);

		TBand band;
		band.m_dwID = pBuf[0];
		band.m_dwCX = pBuf[1];
		band.m_dwIndex = pBuf[2];
		band.m_bUseFullLine = (pBuf[3] != 0) ? true : false;

		m_arrBands.push_back(band);
	}

	return true;
}

bool CRebarPos::FillFromRebar(HWND hRebar)
{
	if (!IsWindow(hRebar))
		return false;

	m_arrBands.clear();

	int nCount = (int)SendMessage(hRebar, RB_GETBANDCOUNT, 0, 0);
	for (int n = 0; n < nCount; ++n)
	{
		REBARBANDINFO rbi;
		rbi.cbSize = sizeof(rbi);
		rbi.fMask = RBBIM_ID | RBBIM_SIZE | RBBIM_STYLE;

		if (SendMessage(hRebar, RB_GETBANDINFO, n, reinterpret_cast<LPARAM>(&rbi)) == 0)
			continue;

		TBand band;
		band.m_dwIndex = n;
		band.m_dwID = rbi.wID;
		band.m_dwCX = rbi.cx;
		band.m_bUseFullLine = ((rbi.fStyle & RBBS_BREAK) == RBBS_BREAK);

		m_arrBands.push_back(band);
	}

	return true;
}

bool CRebarPos::ApplyToRebar(HWND hRebar) const
{
	if (!IsWindow(hRebar) || (m_arrBands.size() <= 0))
		return false;

	for (size_t n = 0; n < m_arrBands.size(); ++n)
	{
		int nCurIndex = (int)SendMessage(hRebar, RB_IDTOINDEX, m_arrBands[n].m_dwID, 0);
		if (nCurIndex < 0)
			continue;

		if (SendMessage(hRebar, RB_MOVEBAND, nCurIndex, m_arrBands[n].m_dwIndex) == 0) 
			continue;

		REBARBANDINFO rbi;
		rbi.cbSize = sizeof(rbi);
		rbi.fMask = RBBIM_ID | RBBIM_SIZE | RBBIM_STYLE;

		SendMessage(hRebar, RB_GETBANDINFO, m_arrBands[n].m_dwIndex, reinterpret_cast<LPARAM>(&rbi));
		
		rbi.cx = m_arrBands[n].m_dwCX;
		if (m_arrBands[n].m_bUseFullLine)
			rbi.fStyle |= RBBS_BREAK;
		else
			rbi.fStyle &= ~RBBS_BREAK;
		
		SendMessage(hRebar, RB_SETBANDINFO, m_arrBands[n].m_dwIndex, reinterpret_cast<LPARAM>(&rbi));
	}

	return true;
}

void CRebarPos::GetBand(DWORD dwID, OUT TBand& band) const
{
	for (BANDS_ARRAY::const_iterator it = m_arrBands.begin(); it != m_arrBands.end(); ++it)
	{
		if (it->m_dwID == dwID)
			band = *it;
	}
}

