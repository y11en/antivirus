// FiltersStorage.cpp: implementation of the CFiltersStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FiltersStorage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILTER_BUFFER_SIZE	0xf000

CFiltersStorage::CFiltersStorage(HANDLE hDevice, ULONG AppID)
{
	m_bIsModified = false;
	m_hDevice = hDevice;
	m_AppID = AppID;
}

CFiltersStorage::~CFiltersStorage()
{
	m_FiltersArray.ResetContent();
}

BOOL CFiltersStorage::FilterTransmit(HANDLE hDevice, FILTER_TRANSMIT* pInFilter, FILTER_TRANSMIT* pOutFilter)
{
	DWORD BytesRet = 0;
	pInFilter->m_AppID = m_AppID;
	return DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pInFilter, FILTER_BUFFER_SIZE, pOutFilter, FILTER_BUFFER_SIZE, &BytesRet, NULL);
}

void CFiltersStorage::ResetState()
{
	m_FiltersArray.ResetContent();
}

void CFiltersStorage::ReloadFiltersArray(void)
{
	BOOL bRet;
	ResetState();

	BYTE pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;

	PFILTER_TRANSMIT pFilter;
	_pFilter->m_AppID = m_AppID;

	_pFilter->m_FltCtl = FLTCTL_FIRST;

	bRet = FilterTransmit(m_hDevice, _pFilter, _pFilter);
	if (bRet == FALSE)
		return;

	CString func, maj, min, strtmp, strtmp2;
	_pFilter->m_FltCtl = FLTCTL_NEXT;
	while (bRet)
	{
		pFilter = (PFILTER_TRANSMIT) (new char [FILTER_BUFFER_SIZE]);
		CopyMemory(pFilter, _pFilter, FILTER_BUFFER_SIZE);

		m_FiltersArray.Add(pFilter);

		bRet = FilterTransmit(m_hDevice, _pFilter, _pFilter);
	}
}

bool CFiltersStorage::DeleteFilter(PFILTER_TRANSMIT pFilter, bool bAlternate)
{
	if (m_hDevice == NULL)
		return false;
	
	bool bResult = false;
	PFILTER_TRANSMIT pFilterTmp;
	if (bAlternate == false)
	{
		int size = m_FiltersArray.GetSize();
		for (int cou = 0; (cou < size) && (bResult == false); cou++)
		{
			pFilterTmp = m_FiltersArray[cou];
			if (pFilterTmp != NULL && pFilterTmp->m_FilterID == pFilter->m_FilterID)
			{
				pFilter->m_FltCtl = FLTCTL_DEL;
				if (FilterTransmit(m_hDevice, pFilter, pFilter) != FALSE)
				{	
					m_FiltersArray.RemoveAt(cou);
					delete pFilterTmp;
					bResult = true;
				}
			}
		}
	}
	else
	{
		pFilter->m_FltCtl = FLTCTL_DEL;
		if (FilterTransmit(m_hDevice, pFilter, pFilter) != FALSE)
			bResult = true;
	}

	if (bResult)
		m_bIsModified = true;
	return bResult;
}

bool CFiltersStorage::AddNewFilter(PFILTER_TRANSMIT pFilter, DWORD* pAdditionSite)
{
	if (m_hDevice == NULL)
		return false;
	
	bool bResult = false;
	
	if (pAdditionSite == NULL)
		pFilter->m_FltCtl = FLTCTL_ADD;
	else
	{
		if (*pAdditionSite == 0)
			pFilter->m_FltCtl = FLTCTL_ADD2TOP;
		else
		{
			pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
			pFilter->m_FilterID = *pAdditionSite;
		}
		
	}

	if (FilterTransmit(m_hDevice, pFilter, pFilter) != FALSE)
		bResult = true;

	if (bResult)
		m_bIsModified = true;

	return bResult;
}

bool CFiltersStorage::IsFiltered(PFILTER_TRANSMIT pFilter)
{
	if (m_hDevice == NULL)
		return false;
	
	bool bResult = false;
	pFilter->m_FltCtl = FLTCTL_FIND;
	if (FilterTransmit(m_hDevice, pFilter, pFilter))
		bResult = true;

	return bResult;
}

