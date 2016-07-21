#include "StdAfx.h"
#include "HipsDataSerializer.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsDataSerializer::CHipsDataSerializer(void)
{
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsDataSerializer::~CHipsDataSerializer(void)
{
}
////////////////////////////////////////////////////////////////////////////////
//	serialize one param
////////////////////////////////////////////////////////////////////////////////
bool CHipsDataSerializer::SerializeParam(hREGISTRY pReg, tRegKey hKey, CHipsResourceParam * pParam, tDWORD ParamID)
{
	assert(pReg || hKey || pParam);
	if (!pReg || !hKey || !pParam)
		return false;

	char chParamId[10];
	char chMBStr[MAX_PATH * 2];
	sprintf(chParamId, "%d", ParamID);

	tRegKey hParam;
	tERROR Error;

	Error = pReg->OpenKey(&hParam, hKey, chParamId, cTRUE);
	if (PR_FAIL(Error))
		return false;

	Error = pReg->SetValue(hParam, HIPS_SER_PARAM_TYPE_STR, tid_DWORD, &pParam->m_ResParamType, sizeof(pParam->m_ResParamType), cTRUE );
	switch (pParam->m_ResParamType)
	{
		case ehrptNum:
			{
				Error = pReg->SetValue(hParam, HIPS_SER_PARAM_VALUE_NUM_STR, tid_DWORD, &pParam->m_ResParamValueNum, sizeof(pParam->m_ResParamValueNum), cTRUE );
				break;
			}
		case ehrptStr:
			{
				WideCharToMultiByte(CP_UTF8, 0, pParam->m_ResParamValueStr.c_str(cCP_UNICODE), -1, chMBStr, MAX_PATH * 2, 0, 0);
			    Error = pReg->SetValue(hParam, HIPS_SER_PARAM_VALUE_STR_STR, tid_STRING, chMBStr, (tDWORD)strlen(chMBStr), cTRUE );
				break;
			}
		default:
			{
				break;
			}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool CHipsDataSerializer::SerializeResource(hREGISTRY pReg, tRegKey hKey, CHipsResourceItem * pItem)
{
	assert(pReg || hKey || pItem);
	if (!pReg || !hKey || !pItem)
		return false;

	char chResourceId[10];
	char chMBStr[MAX_PATH * 2];
	sprintf(chResourceId, "%d", pItem->m_ResID);

	tRegKey hResource, hParamsList;
	tERROR Error;

	Error = pReg->OpenKey(&hResource, hKey, chResourceId, cTRUE);
	if (PR_FAIL(Error))
		return false;

	Error = pReg->SetValue(hResource, HIPS_SER_RES_ID, tid_DWORD, &pItem->m_ResID, sizeof(pItem->m_ResID), cTRUE );
    Error = pReg->SetValue(hResource, HIPS_SER_RES_TYPE, tid_DWORD, &pItem->m_ResType, sizeof(pItem->m_ResType), cTRUE );


	WideCharToMultiByte(CP_UTF8, 0, pItem->m_ResDescr.c_str(cCP_UNICODE), -1, chMBStr, MAX_PATH * 2, 0, 0);
    Error = pReg->SetValue(hResource, HIPS_SER_RES_DESC, tid_STRING, chMBStr, strlen(chMBStr), cTRUE );

	//	saving param list
	Error = pReg->OpenKey(&hParamsList, hResource, HIPS_SER_PARAM_LIST, cTRUE);
	if (PR_SUCC(Error))
	{
		for (DWORD j = 0; j < pItem->m_vResParamList.count(); j++)
		{
			//	save one param
			SerializeParam(pReg, hParamsList, &pItem->m_vResParamList[j], j);
		}
	}
	return true;
}
