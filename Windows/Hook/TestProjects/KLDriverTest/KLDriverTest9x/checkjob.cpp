#include "stdafx.h"
#include "CheckJob.h"

#include "Hook\IDriver.h"

PCHAR GetProcessName(PCHAR ModuleName, DWORD size)
{
	PCHAR pModuleName = NULL;
	GetModuleFileName(NULL, ModuleName, size);

	for (int cou = lstrlen(ModuleName); cou >= 0; cou--)
	{
		if (ModuleName[cou] == L'\\')
		{
			pModuleName = ModuleName + cou + 1;
			break;
		}
	}

	if (pModuleName != NULL)
	{
		int cou2 = lstrlen(pModuleName) - 1;
		char ch;
		while (cou2 > 0)
		{
			ch = pModuleName[cou2];
			pModuleName[cou2] = 0;
			if (ch == '.')
				break;

			cou2--;
		}
	}

	return pModuleName;
}

bool GetEventData(DrvEventList* pDrvEventList, HANDLE hDevice, ULONG AppID, BYTE** pData, ULONG *pMark)
{
	DWORD BytesRet;
	BOOL bReq;

	static LONG Mark = 0;

	InterlockedIncrement(&Mark);
	if (Mark == 0)
		InterlockedIncrement(&Mark);

	ULONG EventDataSize = 4096;
	BYTE* pEventData = pDrvEventList->Alloc(EventDataSize);
	if (pEventData != NULL)
	{
		*pMark = Mark;
		bReq = IDriverGetEvent(hDevice, AppID, Mark, pEventData, EventDataSize, &BytesRet);
		if (bReq == FALSE || BytesRet == 0)
		{
			DWORD BytesRet = 0;
			EventDataSize = 0;
			if(DeviceIoControl(hDevice, IOCTLHOOK_GetEventSize, &AppID, sizeof(AppID), &EventDataSize, sizeof(EventDataSize), &BytesRet, NULL))
			{
				HeapFree(GetProcessHeap(), 0, pEventData);
				pEventData = NULL;
				if (EventDataSize != 0)
				{
					pEventData = pDrvEventList->Alloc(EventDataSize);
					if (pEventData != NULL)
					{
						bReq = IDriverGetEvent(hDevice, AppID, Mark, pEventData, EventDataSize, &BytesRet);
						if (bReq == FALSE || BytesRet == 0)
						{
							pDrvEventList->Free(pEventData);
							pEventData = NULL;
						}
					}
				}
			}
		}
	}
		
	if (pEventData != NULL)
	{
		*pData = pEventData;
		return TRUE;
	}

	IDriverSkipEvent(hDevice, AppID);
	
	return FALSE;
}

//+ ----------------------------------------------------------------------------------------

CheckJob_File::CheckJob_File(DrvEventList* pDrvEventList)
{
	m_bAllow = true;
	m_pDrvEventList = pDrvEventList;
}

CheckJob_File::~CheckJob_File()
{
}

bool CheckJob_File::Start()
{
	return HookTask::Start(this);
}

bool CheckJob_File::AddFilters()
{
	CHAR ModuleName[0x100];
	PCHAR pModuleName = GetProcessName(ModuleName, sizeof(ModuleName));

	if (pModuleName == NULL)
		return false;
	
	AddFSFilter(m_hDevice, m_AppID, "*~*", pModuleName, 3, FLT_A_DEFAULT, FLTTYPE_IFS, -1, -1, 0, PreProcessing, NULL);
	AddFSFilter2(m_hDevice, m_AppID, pModuleName, 5, FLT_A_POPUP, FLTTYPE_IFS, -1, -1, 0, PreProcessing, NULL, NULL);
	AddFSFilter2(m_hDevice, m_AppID, pModuleName, 5, FLT_A_POPUP, FLTTYPE_I21, -1, -1, 0, PreProcessing, NULL, NULL);
	AddFSFilter2(m_hDevice, m_AppID, pModuleName, 5, FLT_A_POPUP, FLTTYPE_R3, -1, -1, 0, PreProcessing, NULL, NULL);

	return true;
}
	
void CheckJob_File::SingleEvent()
{
	VERDICT Verdict = Verdict_Discard;
	BYTE* pEventData = NULL;
	ULONG Mark;
	if (false == GetEventData(m_pDrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
		m_pDrvEventList->SetError(_ERR_EVENT);		//event skipped
	else
	{
		PEVENT_TRANSMIT pEvt = (PEVENT_TRANSMIT) pEventData;
		if (pEvt->m_HookID == FLTTYPE_IFS && pEvt->m_FunctionMj == IFSFN_ENUMHANDLE)
		{
		}

		else
		{
			if (false == m_pDrvEventList->AddNew(pEventData))
			{
				m_pDrvEventList->Free(pEventData);
				m_pDrvEventList->SetError(_ERR_INTERNAL);		// push failed
			}
		}

		if (m_bAllow)
			Verdict = Verdict_Default;
	
		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
}

void CheckJob_File::UploadEvent()
{
	HDSTATE Activity;
	Activity.Activity = _AS_DontChange;
	Activity.AppID = m_AppID;
	Activity.QLen = 0;
	Activity.QUnmarkedLen = 0;

	if (!IDriverGetState(m_hDevice, m_AppID, &Activity))
		return;

	while (Activity.QUnmarkedLen > 0)
	{
		SingleEvent();
		Activity.QUnmarkedLen--;
	}
}

