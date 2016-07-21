// FiltersStorage.cpp: implementation of the CFiltersStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avpgcfg.h"
#include "FiltersStorage.h"

#include <hook\avpgcom.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

PCHAR GetObjNameFromFilter(PFILTER_TRANSMIT pFilter)
{
	PCHAR pObjName = NULL;
	PFILTER_PARAM	pSingleParam;
	if (pFilter->m_ParamsCount != 0)
	{
		pSingleParam = (PFILTER_PARAM) pFilter->m_Params;
		while (pSingleParam != NULL && pSingleParam->m_ParamID != _PARAM_OBJECT_URL)
			pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);

		if (pSingleParam != NULL)
			pObjName = (char*) pSingleParam->m_ParamValue;
	}

	return pObjName;
}

PFILTER_PARAM GetFilterParam(PFILTER_TRANSMIT pFilter, ULONG ParamID)
{
	PCHAR pObjName = NULL;
	PFILTER_PARAM	pSingleParam;

	pSingleParam = (PFILTER_PARAM) pFilter->m_Params;
	for (ULONG cou = 0; cou < pFilter->m_ParamsCount; cou++)
	{
		if (pSingleParam->m_ParamID == ParamID)
			return pSingleParam;
		pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
		
	}
	
	return NULL;
}

BOOL ChangeFilterParamValue(PFILTER_TRANSMIT pFilter, PFILTER_PARAM	pNewParam)
{
	if (pNewParam->m_ParamSize + sizeof(FILTER_TRANSMIT) > FILTER_BUFFER_SIZE)
		return FALSE;

	char bFound = FALSE;
	char pBuff[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT pFilterSave;

	pFilterSave = (PFILTER_TRANSMIT) pBuff;

	memcpy(pFilterSave, pFilter, FILTER_BUFFER_SIZE);

	int ParamCount = 0;

	PFILTER_PARAM	pSingleParam;
	PFILTER_PARAM	pSingleParamSave;
	pSingleParam = (PFILTER_PARAM) pFilter->m_Params;
	pSingleParamSave = (PFILTER_PARAM) pFilterSave->m_Params;

	for (DWORD cou = 0; cou < pFilter->m_ParamsCount; cou++)
	{
		if (pSingleParamSave->m_ParamID != pNewParam->m_ParamID)
		{
			pSingleParam->m_ParamFlags = pSingleParamSave->m_ParamFlags;
			pSingleParam->m_ParamFlt = pSingleParamSave->m_ParamFlt;
			pSingleParam->m_ParamID = pSingleParamSave->m_ParamID;
			pSingleParam->m_ParamSize = pSingleParamSave->m_ParamSize;
			memcpy(pSingleParam->m_ParamValue, pSingleParamSave->m_ParamValue, pSingleParam->m_ParamSize);

			pSingleParam = (PFILTER_PARAM)((BYTE*)pSingleParam + sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
			ParamCount++;
		}
		pSingleParamSave = (PFILTER_PARAM)((BYTE*)pSingleParamSave + sizeof(FILTER_PARAM) + pSingleParamSave->m_ParamSize);
	}

	// add new param
	pSingleParam->m_ParamFlags = pNewParam->m_ParamFlags;
	pSingleParam->m_ParamFlt = pNewParam->m_ParamFlt;
	pSingleParam->m_ParamID = pNewParam->m_ParamID;
	pSingleParam->m_ParamSize = pNewParam->m_ParamSize;
	memcpy(pSingleParam->m_ParamValue, pNewParam->m_ParamValue, pSingleParam->m_ParamSize);
	if(pSingleParam->m_ParamID == _PARAM_OBJECT_URL)
		CharLowerBuff((char*) pSingleParam->m_ParamValue, pSingleParam->m_ParamSize);

	ParamCount++;

	pFilter->m_ParamsCount = ParamCount;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiltersStorage::CFiltersStorage(HANDLE hDevice)
{
	m_bIsModified = false;
	m_hDevice = hDevice;
}

CFiltersStorage::~CFiltersStorage()
{

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

	OutputDebugString("-----------------------------------------------------\nFilters list\n");

	char pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;

	PFILTER_TRANSMIT pFilter;
	_pFilter->m_AppID = AppReg.m_AppID;
	PCHAR pObjName;
	char* pch;

	_pFilter->m_FltCtl = FLTCTL_FIRST;

	bRet = FilterTransmit(m_hDevice, _pFilter, _pFilter);
	if (bRet == FALSE)
		return;

	CString func, maj, min, strtmp, strtmp2;
	_pFilter->m_FltCtl = FLTCTL_NEXT;
	while (bRet)
	{
		func.Empty();
		maj.Empty();
		min.Empty();

		pObjName = GetObjNameFromFilter(_pFilter);
		if (pObjName == NULL)
			pObjName = "";

		pch = FillFuncList(NULL, _pFilter->m_HookID, _pFilter->m_FunctionMj, _pFilter->m_FunctionMi);
		if (pch != NULL)
			func = pch;
		pch = FillMajList(NULL, _pFilter->m_HookID, _pFilter->m_FunctionMj, _pFilter->m_FunctionMi);
		if (pch != NULL)
			maj = pch;
		pch = FillMinList(NULL, _pFilter->m_HookID, _pFilter->m_FunctionMj, _pFilter->m_FunctionMi);
		if (pch != NULL)
			min = pch;
		strtmp2.Empty();
/*		if (Filter.m_Flags & FLT_SYSTEM)
			strtmp2 += "System ";*/
		if (_pFilter->m_Flags & FLT_A_POPUP)
			strtmp2 += "Popup ";
		if (_pFilter->m_Flags & FLT_A_LOG)
			strtmp2 += "Log ";
		if (_pFilter->m_Flags & FLT_A_DENY)
			strtmp2 += "Deny";

		strtmp.Format("Process:'%s', FileName: '%s', Function: '%s'(%d), Major: '%s'(%d), Min: '%s'(%d), Flags: '%s'\n", 
			_pFilter->m_ProcName, pObjName,
			func, _pFilter->m_HookID,
			maj, _pFilter->m_FunctionMj,
			min, _pFilter->m_FunctionMi,
			strtmp2);
		OutputDebugString(strtmp);
		pFilter = (PFILTER_TRANSMIT) (new char [FILTER_BUFFER_SIZE]);
		memcpy(pFilter, _pFilter, FILTER_BUFFER_SIZE);
		m_FiltersArray.Add(pFilter);

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

bool CFiltersStorage::AddNewFilter(PFILTER_TRANSMIT pFilter)
{
	if (m_hDevice == NULL)
		return false;
	
	bool bResult = false;
	pFilter->m_FltCtl = FLTCTL_ADD2TOP;
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
