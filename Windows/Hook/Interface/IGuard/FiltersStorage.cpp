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

CFiltersStorage::CFiltersStorage(HANDLE hDevice)
{
	m_bIsModified = false;
	m_hDevice = hDevice;
}

CFiltersStorage::~CFiltersStorage()
{
	m_FiltersArray.ResetContent();
}

BOOL CFiltersStorage::FilterTransmit(HANDLE hDevice, FILTER_TRANSMIT* pInFilter, FILTER_TRANSMIT* pOutFilter)
{
	DWORD BytesRet = 0;
	pInFilter->m_AppID = AppReg.m_AppID;
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

	CString FilterDescription;
	OutputDebugString("-----------------------------------------------------\nFilters list\n");

	BYTE pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;

	PFILTER_TRANSMIT pFilter;
	_pFilter->m_AppID = AppReg.m_AppID;

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

		GetFilterDesciptions(pFilter, &FilterDescription);

		OutputDebugString(FilterDescription);
		OutputDebugString("\n");

		bRet = FilterTransmit(m_hDevice, _pFilter, _pFilter);
	}
	OutputDebugString("End filters list\n-----------------------------------------------------\n");
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


// -----------------------------------------------------------------------------------------
int SwitchProProt(CFiltersStorage* pFiltersStorage, int Request)
{
	BOOL bStatus = TRUE;
	char pFilterBuffer[FILTER_BUFFER_SIZE];
	
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;
	
	_pFilter->m_AppID = AppReg.m_AppID;
	_pFilter->m_FltCtl = FLTCTL_FIRST;
	
	if (pFiltersStorage->FilterTransmit(pFiltersStorage->m_hDevice, _pFilter, _pFilter) == FALSE)
		return -1;
	
	_pFilter->m_FltCtl = FLTCTL_NEXT;
	while (bStatus)
	{
		if ((_pFilter->m_HookID == FLTTYPE_SYSTEM) && (_pFilter->m_FunctionMj == MJ_SYSTEM_TERMINATE_PROCESS) && (_pFilter->m_FunctionMi == 0))
		{
			switch (Request)
			{
			case 1:
				if (_pFilter->m_Flags & FLT_A_DISABLED)
				{
					_pFilter->m_FltCtl = FLTCTL_ENABLE_FILTER;
					if (pFiltersStorage->FilterTransmit(pFiltersStorage->m_hDevice, _pFilter, _pFilter) == FALSE)
						return -1;
				}
				return 1;
				//break;
			case 0:
				if (!(_pFilter->m_Flags & FLT_A_DISABLED))
				{
					_pFilter->m_FltCtl = FLTCTL_DISABLE_FILTER;
					if (pFiltersStorage->FilterTransmit(pFiltersStorage->m_hDevice, _pFilter, _pFilter) == FALSE)
						return -1;
				}
				return 1;
				//break;
			default:
				if (_pFilter->m_Flags & FLT_A_DISABLED)
					return 0;
				return 1;
				//break;
			}
		}
		bStatus = pFiltersStorage->FilterTransmit(pFiltersStorage->m_hDevice, _pFilter, _pFilter);
	}
	
	return -1;
}

BOOL AddProProt(CFiltersStorage* pFiltersStorage)
{
	if (SwitchProProt(pFiltersStorage, -1) != -1)
		return TRUE;

	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
	BYTE Buf2[sizeof(FILTER_PARAM) + PROCNAMELEN + 1];
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
	
	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
	Param1->m_ParamFlt = FLT_PARAM_EUA;
	Param1->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
	*(DWORD*)Param1->m_ParamValue = (DWORD) -1;
	Param1->m_ParamSize = sizeof(DWORD);

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param2->m_ParamFlt = FLT_PARAM_WILDCARD;
	Param2->m_ParamID = _PARAM_OBJECT_URL;
	lstrcpyn((PCHAR) Param2->m_ParamValue, "KLGuard.exe", MAXPATHLEN);
	Param2->m_ParamSize = lstrlen((PCHAR) Param2->m_ParamValue) + 1;
		
	if (AddFSFilter2(pFiltersStorage->m_hDevice, AppReg.m_AppID, "*", AppReg.m_BlueScreenTimeout, FLT_A_POPUP, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, NULL))
	{
		return TRUE;
	}

/*	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
	
	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param1->m_ParamFlt = FLT_PARAM_EUA;
	Param1->m_ParamID = _PARAM_OBJECT_SOURCE_ID;
	*(DWORD*)Param1->m_ParamValue = (DWORD) -1;
	Param1->m_ParamSize = sizeof(DWORD);

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param2->m_ParamFlt = FLT_PARAM_WILDCARD;
	Param2->m_ParamID = _PARAM_OBJECT_URL;
	lstrcpyn((PCHAR) Param2->m_ParamValue, "KLGuard.exe", MAXPATHLEN);
	Param2->m_ParamSize = lstrlen((PCHAR) Param2->m_ParamValue) + 1;
		
	if (AddFSFilter2(pFiltersStorage->m_hDevice, AppReg.m_AppID, "*", AppReg.m_BlueScreenTimeout, FLT_A_POPUP, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, NULL))
	{
		return TRUE;
	}*/

	
	return FALSE;
}