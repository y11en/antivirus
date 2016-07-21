// KLDriverTestNT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CheckJob.h"

#include <stdio.h> 
#define  NullStr "\0"
CHAR *UnknownStr="-?-";
#include "..\..\..\Ntcommon.cpp"

//+ ----------------------------------------------------------------------------------------
#define _check_system
#define _check_file
#define _check_regestry
#define _check_flt

//+ ----------------------------------------------------------------------------------------

DrvEventList gDrvEventList;
LogEvent Log;

CHAR gModuleName[0x100];
PCHAR gpModuleName = NULL;

PCHAR GetProcessName(PCHAR ModuleName, DWORD ModuleNameSize)
{
	PCHAR pModuleName = NULL;
	GetModuleFileNameA(NULL, ModuleName, ModuleNameSize);

	for (int cou = lstrlenA(ModuleName); cou >= 0; cou--)
	{
		if (ModuleName[cou] == '\\')
		{
			pModuleName = ModuleName + cou + 1;
			break;
		}
	}

	return pModuleName;
}


//+ ----------------------------------------------------------------------------------------
#define _endofline	L"\n"
#define _newevent	L"new_event ------------------------------------------------------------"
#define _endevent	L"end_of_event ---------------------------------------------------------\n"

LogEvent::LogEvent()
{
	m_hFile = CreateFile(L"TestDrv.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
		gDrvEventList.SetError(_ERR_LOG);

	InitializeCriticalSection(&m_Sync);
}
LogEvent::~LogEvent()
{
	if (INVALID_HANDLE_VALUE == m_hFile)
		return;
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	DeleteCriticalSection(&m_Sync);
}

void LogEvent::AddToLogInt(LPCWSTR pwch, BOOL endofline, BOOL bNeedProtect)
{
	BOOL bWriteSucc = TRUE;
	if (INVALID_HANDLE_VALUE == m_hFile)
		return;
	
	if (bNeedProtect)
		EnterCriticalSection(&m_Sync);
	
	DWORD dwWritten;
	bWriteSucc &= WriteFile(m_hFile, pwch, lstrlen(pwch) * sizeof(WCHAR), &dwWritten, NULL);
	if (endofline)
		bWriteSucc &= WriteFile(m_hFile, _endofline, lstrlen(_endofline) * sizeof(WCHAR), &dwWritten, NULL);
	
	if (!bWriteSucc)
		gDrvEventList.SetError(_ERR_ADDLOG);
	else
		FlushFileBuffers(m_hFile);
	
	if (bNeedProtect)
		LeaveCriticalSection(&m_Sync);
}

void LogEvent::AddToLogIntP(LPCWSTR pwch, BOOL endofline)
{
	AddToLogInt(pwch, endofline, TRUE);
}


void LogEvent::AddToLog(LPCWSTR pwch, BOOL endofline)
{
	EnterCriticalSection(&m_Sync);
	AddToLogInt(pwch, endofline, FALSE);
	LeaveCriticalSection(&m_Sync);
}

void LogEvent::AddToLog(PEVENT_TRANSMIT pEvt)
{
	if (INVALID_HANDLE_VALUE == m_hFile)
		return;
	
	WCHAR pBuff[1024];
	
	EnterCriticalSection(&m_Sync);

	AddToLogIntP(_newevent);

	CHAR EventDescr[512];

	NTGetFunctionStr(EventDescr, pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi);
	if (MultiByteToWideChar(CP_ACP, 0, EventDescr, -1, pBuff, sizeof(pBuff)))
		AddToLogIntP(pBuff);
	else
	{
		AddToLogIntP(L"!translate to unicode failed\n");
		gDrvEventList.SetError(_ERR_INTERNAL);
	}
	
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvt->m_Params;
	for (DWORD cou =  0; (cou < pEvt->m_ParamsCount); cou++)
	{
		wsprintf(pBuff, L"Param %#x, size %d", pSingleParam->ParamID, pSingleParam->ParamSize);
		AddToLogIntP(pBuff, FALSE);
		
		switch (pSingleParam->ParamID)
		{
		case _PARAM_OBJECT_ACCESSATTR:
			{
				if (pSingleParam->ParamSize == sizeof(DWORD))
					wsprintf(pBuff, L"...Access attributes. Value %#x", *(DWORD*)(pSingleParam->ParamValue));
				else
				{
					AddToLogIntP(L"...<length mismatch>", FALSE);
					gDrvEventList.SetError(_ERR_EVENT);
				}
				
				AddToLogIntP(pBuff, FALSE);
			}
			break;
		case _PARAM_OBJECT_ORIGINAL_PACKET:
			AddToLogIntP(L"...Original packet <changeable>", FALSE);
			break;
		case _PARAM_OBJECT_SOURCE_ID:
			AddToLogIntP(L"...Source ID <changeable>", FALSE);
			break;
		case _PARAM_OBJECT_DEST_ID:
			AddToLogIntP(L"...Destination ID <changeable>", FALSE);
			break;
		case _PARAM_OBJECT_CONTEXT_FLAGS:
			wsprintf(pBuff, L"...Context flags. Value %#x", *(DWORD*)(pSingleParam->ParamValue));
			AddToLogIntP(pBuff, FALSE);
			break;
		case _PARAM_OBJECT_SID:
			{
				WCHAR pwszDomain[MAX_PATH];
				WCHAR pwszUser[MAX_PATH];
				
				DWORD dwDomain = sizeof(pwszDomain) / sizeof(WCHAR);
				DWORD dwUser = sizeof(pwszUser) / sizeof(WCHAR);

				wsprintf(pBuff, L"...SID Value: ");
				AddToLogIntP(pBuff, FALSE);
				if (!IDriverGetUserFromSidW((SID*) pSingleParam->ParamValue, pwszDomain, &dwDomain, pwszUser, &dwUser))
				{
					AddToLogIntP(L"<invalid>", FALSE);
				}
				else
				{
					AddToLogIntP(pwszDomain, FALSE);
					AddToLogIntP(L"/", FALSE);
					AddToLogIntP(pwszUser, FALSE);
				}
			}
			break;
		default:
			{
				switch (pSingleParam->ParamID)
				{
				case _PARAM_OBJECT_URL:
				case _PARAM_OBJECT_URL_DEST:
					{
						// translate to UNICODE
						int unilen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR) pSingleParam->ParamValue, -1, 0, 0) * sizeof(WCHAR);
						if (unilen == 0)
						{
							AddToLogIntP(L"!translate to unicode failed\n");
							gDrvEventList.SetError(_ERR_INTERNAL);
						}
						else
						{
							PWSTR pwstr = (PWSTR) gDrvEventList.Alloc(unilen);
							if (pwstr == NULL)
							{
								AddToLogIntP(L"!translate to unicode failed - low resource\n");
								gDrvEventList.SetError(_ERR_INTERNAL);
							}
							else
							{
								MultiByteToWideChar(CP_ACP, 0, (LPCSTR) pSingleParam->ParamValue, -1, pwstr, unilen);
								AddToLogIntP(L"...", FALSE);
								AddToLogIntP(pSingleParam->ParamID == _PARAM_OBJECT_URL ? L"Url" : L"Dest url", FALSE);
								AddToLogIntP(L" Value: ", FALSE);
								AddToLogIntP(pwstr, FALSE);
								gDrvEventList.Free((BYTE*) pwstr);
							}
						}
					}
					break;
				case _PARAM_OBJECT_BINARYDATA:
				case _PARAM_OBJECT_DATALEN:
				case _PARAM_OBJECT_SECTOR:
				case _PARAM_OBJECT_BYTEOFFSET:
					{
						if (pSingleParam->ParamSize == sizeof(DWORD))
							wsprintf(pBuff, L"...%#x", *(DWORD*)(pSingleParam->ParamValue));
						else
							wsprintf(pBuff, L"...<hex data>");

						AddToLogIntP(pBuff, FALSE);
					}
					break;
				case _PARAM_OBJECT_URL_W:
				case _PARAM_OBJECT_URL_DEST_W:
					AddToLogIntP(L"...", FALSE);
					AddToLogIntP(pSingleParam->ParamID == _PARAM_OBJECT_URL_W ? L"Url" : L"Dest url", FALSE);
					AddToLogIntP(L" Value: ", FALSE);
					AddToLogIntP((LPWSTR)pSingleParam->ParamValue, FALSE);
					break;
				default:
					AddToLogIntP(L"...unknown param");
				}
			}
		}
		AddToLogIntP(_endofline, FALSE);
		
		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
		else
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	
	AddToLogIntP(_endevent);

	LeaveCriticalSection(&m_Sync);
}	

//+ ----------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	BYTE* pEvt;

	gpModuleName = GetProcessName(gModuleName, sizeof(gModuleName));

	if (gpModuleName == NULL)
		return -1;

	HANDLE hEventAppear = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hEventAppear == NULL)
		return -2;
	//+ ----------------------------------------------------------------------------------------
	//+ system test
#ifdef _check_system
	Log.AddToLog(L"Starting system interceptor check...\n");
	{
		CheckJob_System SystemJob(&gDrvEventList);
		if (false == SystemJob.Start())
		{
			Log.AddToLog(L"Can't start check system !");
			gDrvEventList.SetError(_ERR_DRVFAULT);
		}
		else
		{
			SystemJob.ChangeActiveStatus(true);
			
			//+ ----------------------------------------------------------------------------------------
			//+ open process
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
			if (hProcess != NULL)
			{
				Log.AddToLog(L"protection fault - process opened!");
				gDrvEventList.SetError(_ERR_DRVFAULT);
				CloseHandle(hProcess);
			}
			
			//+ ----------------------------------------------------------------------------------------
			//+ create process (ex)
			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &pi, sizeof(pi) );
			
			WCHAR cmdline[1024];
			if (!ExpandEnvironmentStrings(L"\"%ComSpec%\" /C echo app launched!", cmdline, sizeof(cmdline)))
				gDrvEventList.SetError(_ERR_INTERNAL);
			else
			{
				if (CreateProcess(NULL, cmdline, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
				{
					Log.AddToLog(L"protection fault - process started!");
					gDrvEventList.SetError(_ERR_DRVFAULT);
					WaitForSingleObject(pi.hProcess,INFINITE);
				}
			}
			
			//+ ----------------------------------------------------------------------------------------
			//+ terminate process
			TerminateProcess((HANDLE) -1, -2);
			
			SystemJob.ChangeActiveStatus(false);
		}
		
		while ((pEvt = gDrvEventList.GetFirst()) != NULL)
		{
			Log.AddToLog((PEVENT_TRANSMIT) pEvt);
			gDrvEventList.Free(pEvt);
		}
	}
#endif

	//- end system test
	//- ----------------------------------------------------------------------------------------

	//+ ----------------------------------------------------------------------------------------
	//+ file test
#ifdef _check_file
	Log.AddToLog(L"Starting file interceptor check...\n");
	{

		CheckJob_File FileJob(&gDrvEventList);
		if (false == FileJob.Start())
		{
			Log.AddToLog(L"Can't start check file interceptor!");
			gDrvEventList.SetError(_ERR_DRVFAULT);
		}
		else
		{
			FileJob.SetAllowEvents(true);
			FileJob.ChangeActiveStatus(true);

	#define _testfilename			L"TestFile.tst"
	#define _testfilename_renamed	L"TestFile renamed.tst"
	#define _testfilename_create	L"TestFile check create.tst"

			HANDLE hTestFile = CreateFile(_testfilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hTestFile == INVALID_HANDLE_VALUE)
			{
				FileJob.ChangeActiveStatus(false);

				Log.AddToLog(L"Can't create file TestFile.tst for test");
				gDrvEventList.SetError(_ERR_DRVFAULT);

				FileJob.ChangeActiveStatus(true);
			}
			else
			{
				DWORD dwBytes;

				BYTE buf[1024];
				ZeroMemory(buf, sizeof(buf));

				WriteFile(hTestFile, buf, sizeof(buf), &dwBytes, NULL);
				if (dwBytes != sizeof(buf))
				{
					Log.AddToLog(L"!Write to TestFile.tst failed");
					gDrvEventList.SetError(_ERR_INTERNAL);
				}
				
				FlushFileBuffers(hTestFile);

				SetFilePointer(hTestFile, 0, NULL, FILE_BEGIN);

				ReadFile(hTestFile, buf, sizeof(buf), &dwBytes, NULL);
				if (dwBytes != sizeof(buf))
				{
					Log.AddToLog(L"!Read from TestFile.tst failed");
					gDrvEventList.SetError(_ERR_INTERNAL);
				}

				CloseHandle(hTestFile);
				
				MoveFile(_testfilename, _testfilename_renamed);

				DeleteFile(_testfilename_renamed);

				// check deny create file
				FileJob.SetAllowEvents(false);
				hTestFile = CreateFile(_testfilename_create, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
				if (hTestFile != INVALID_HANDLE_VALUE)
				{
					Log.AddToLog(L"protection fault - file "_testfilename_create L" opened");
					gDrvEventList.SetError(_ERR_DRVFAULT);
					CloseHandle(hTestFile);
				}
			}
			
			FileJob.ChangeActiveStatus(false);

			Sleep(100);
		}

		while ((pEvt = gDrvEventList.GetFirst()) != NULL)
		{
			Log.AddToLog((PEVENT_TRANSMIT) pEvt);
			gDrvEventList.Free(pEvt);
		}
	}
#endif
	//- end files test
	//- ----------------------------------------------------------------------------------------

	//+ ----------------------------------------------------------------------------------------
	//+ registry
#ifdef _check_regestry
	Log.AddToLog(L"Starting registry interceptor check...\n");
	{
		CheckJob_Reg RegJob(&gDrvEventList);
		if (false == RegJob.Start())
		{
			Log.AddToLog(L"Can't start check file interceptor!");
			gDrvEventList.SetError(_ERR_DRVFAULT);
		}
		else
		{
			RegJob.SetAllowEvents(true);
			RegJob.ChangeActiveStatus(true);

			HKEY hKey = NULL;
			if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, L"Software", &hKey))
			{
				DWORD type;
				DWORD cbData = 0;
				RegQueryValueEx(hKey, NULL, 0, &type, NULL, &cbData);

				RegCloseKey(hKey);

				//+ ----------------------------------------------------------------------------------------
				//+ enum
				{
					HKEY hKey;
					if(ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE", &hKey))
					{
						TCHAR SubkeyName[512];
						DWORD dwSubkeyName = sizeof(SubkeyName);
						
						DWORD dwIndex = 0;
						
						if (RegEnumKey(hKey, dwIndex, SubkeyName, dwSubkeyName) == ERROR_SUCCESS)
						{
							// empty if
						}
						
						RegCloseKey(hKey);
					}
				}

				//+ ----------------------------------------------------------------------------------------

			}
			
			RegJob.ChangeActiveStatus(false);
		}

		while ((pEvt = gDrvEventList.GetFirst()) != NULL)
		{
			Log.AddToLog((PEVENT_TRANSMIT) pEvt);
			gDrvEventList.Free(pEvt);
		}
	}
#endif
	//- end registry test
	//- ----------------------------------------------------------------------------------------

	//+ ----------------------------------------------------------------------------------------
	//+ filter's queue - add (top, bottom, position), enumerate, /*find*/, enable, disable, delete, reset
#ifdef _check_flt
	Log.AddToLog(L"Starting check filtering system\n");
	{
		CheckJob_Flt FltJob(&gDrvEventList, hEventAppear);
		if (false == FltJob.Start())
		{
			Log.AddToLog(L"Can't start check filtering system!");
			gDrvEventList.SetError(_ERR_DRVFAULT);
		}
		else
		{
			VERDICT Verdict;
			byte SinglEevent[512];
			PFILTER_EVENT_PARAM pSignleEvt = (PFILTER_EVENT_PARAM) SinglEevent;
			ZeroMemory(SinglEevent, sizeof(SinglEevent));

			pSignleEvt->HookID = FLTTYPE_FLT;
			pSignleEvt->FunctionMj = 0;
			pSignleEvt->FunctionMi = 0;
			pSignleEvt->ParamsCount = 1;
			pSignleEvt->ProcessingType = PreProcessing;
			
			PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pSignleEvt->Params;

			BYTE FltTransmit[4095];
			PFILTER_TRANSMIT pFlt = (PFILTER_TRANSMIT) FltTransmit;
			ZeroMemory(FltTransmit, sizeof(FltTransmit));

			FltJob.SetAllowEvents(true);
			if (false == FltJob.ChangeActiveStatus(true))
			{
				Log.AddToLog(L"Can't activate check filtering system!");
				gDrvEventList.SetError(_ERR_DRVFAULT);
			}
			else
			{
				Log.AddToLog(L"check filter's order...");

				ULONG FltArr[4] = {0};

				FltArr[1] = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 5, FLT_A_DEFAULT, 
					FLTTYPE_FLT, 0, 0, 0, PreProcessing, NULL, NULL);

				FltArr[3] = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 5, FLT_A_DEFAULT, 
					FLTTYPE_FLT, 0, 0, 0, PreProcessing, NULL, NULL);

				if (!FltArr[1] || !FltArr[3])
				{
					Log.AddToLog(L"Add filter to FLTTYPE_FLT failed!");
					gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
				}
				else
				{
					DWORD site = 0;
					FltArr[0] = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 5, FLT_A_DEFAULT, 
						FLTTYPE_FLT, 0, 0, 0, PreProcessing, &site, NULL);

					site = FltArr[1];
					FltArr[2] = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 5, FLT_A_DEFAULT, 
						FLTTYPE_FLT, 0, 0, 0, PreProcessing, &site, NULL);
				}

				if (!FltArr[0] || !FltArr[2])
				{
					Log.AddToLog(L"Add filter to FLTTYPE_FLT failed!");
					gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
				}
				else
				{
					pFlt->m_AppID = FltJob.m_AppID;
					pFlt->m_FltCtl = FLTCTL_FIRST;
					
					if (FALSE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)))
					{
						Log.AddToLog(L"Enumerating filter's failed!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}
					else
					{
						int ienum = 0;
						pFlt->m_FltCtl = FLTCTL_NEXT;
						do
						{
							if (ienum > sizeof(FltArr) / sizeof(FltArr[0]))
							{
								Log.AddToLog(L"Filter's count mismatch!");
								gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
							}
							else
							{
								if (FltArr[ienum] != pFlt->m_FilterID)
								{
									Log.AddToLog(L"Filter's order mismatch!");
									gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
								}

								ienum++;
							}

						} while (TRUE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)));
					}
				}

				if (_ERR_DRVFAULT_FLT & gDrvEventList.m_ErrorFlags)
				{
					Log.AddToLog(L"further filter's check skipped - errors...");
				}
				else
				{
					Log.AddToLog(L"checking enable/disable...");
					pFlt->m_FilterID = FltArr[2];
					pFlt->m_FltCtl = FLTCTL_DISABLE_FILTER;
					if (FALSE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)))
					{
						Log.AddToLog(L"Disable filter failed!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}

					pFlt->m_FltCtl = FLTCTL_ENABLE_FILTER;
					if (FALSE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)))
					{
						Log.AddToLog(L"Enable filter failed!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}

					Log.AddToLog(L"checking delete/reset...");

					ResetEvent(hEventAppear);
					pFlt->m_FltCtl = FLTCTL_DEL;
					if (FALSE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)))
					{
						Log.AddToLog(L"Delete filter failed!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}

					pFlt->m_FltCtl = FLTCTL_RESET_FILTERS;
					if (FALSE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)))
					{
						Log.AddToLog(L"Reset filter failed!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}

					if (WAIT_OBJECT_0 != WaitForSingleObject(hEventAppear, 1000 * 60 * 3))	// 3 min
					{
						Log.AddToLog(L"Error: change filter's queue not detected!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}
				}
			}

#define _flt_timeout	20 /*sec*/
			Log.AddToLog(L"checking filter with timeout...");

			FILETIME STasFT;
			ULARGE_INTEGER ExpTime;
			
			GetSystemTimeAsFileTime(&STasFT);
			ExpTime.LowPart=STasFT.dwLowDateTime;
			ExpTime.HighPart=STasFT.dwHighDateTime;
			STasFT.dwLowDateTime=_flt_timeout;
			ExpTime.QuadPart=ExpTime.QuadPart+(LONGLONG)STasFT.dwLowDateTime * 6000000;	//600000000

			ULONG FltTimeout = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 5, FLT_A_INFO, 
					FLTTYPE_FLT, 0, 0, *((__int64*)&ExpTime), PreProcessing, NULL, NULL);
			if (FltTimeout == 0)
			{
				Log.AddToLog(L"Error: add filter with timeout failed!");
				gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
			}
			else
			{
				pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
				pSingleParam->ParamID = _PARAM_OBJECT_URL_W;
				lstrcpy((PWCHAR)pSingleParam->ParamValue, L"this is test filter with timeout...");
				pSingleParam->ParamSize = (lstrlen((PWCHAR)pSingleParam->ParamValue) + 1) * sizeof(WCHAR);

				if (FALSE == IDriverFilterEvent(FltJob.m_hDevice, pSignleEvt, FALSE, &Verdict))
				{
					Log.AddToLog(L"Error: check filter with timeout failed!");
					gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
				}
				else
				{
					Sleep((_flt_timeout + 1) * 1000);
					
					lstrcpy((PWCHAR)pSingleParam->ParamValue, L"Error: filter with timeout is present in queue after timeout!");
					pSingleParam->ParamSize = (lstrlen((PWCHAR)pSingleParam->ParamValue) + 1) * sizeof(WCHAR);

					if (FALSE == IDriverFilterEvent(FltJob.m_hDevice, pSignleEvt, FALSE, &Verdict))
					{
						Log.AddToLog(L"Error: check filter with timeout failed!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}
				}
			}
	
			while ((pEvt = gDrvEventList.GetFirst()) != NULL)
			{
				Log.AddToLog((PEVENT_TRANSMIT) pEvt);
				gDrvEventList.Free(pEvt);
			}

			// params and cache
#define _test_params			20
			BYTE pbParams[_test_params][sizeof(FILTER_PARAM) + 0x100];
			ZeroMemory(pbParams, sizeof(pbParams));

			if (!(_ERR_DRVFAULT_FLT & gDrvEventList.m_ErrorFlags))
			{
				PFILTER_PARAM pArrFltParam[_test_params];

				// dword - FLT_PARAM_AND, FLT_PARAM_EUA, FLT_PARAM_ABV, FLT_PARAM_BEL, FLT_PARAM_MORE, FLT_PARAM_LESS, FLT_PARAM_MASK, FLT_PARAM_EQU_LIST
				// string - FLT_PARAM_WILDCARD

#define FltParam(_idx, _type, _value) { pArrFltParam[_idx] = (PFILTER_PARAM) pbParams[_idx];\
				pArrFltParam[_idx]->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE;\
				pArrFltParam[_idx]->m_ParamFlt = _idx;\
				pArrFltParam[_idx]->m_ParamID = _idx;\
				pArrFltParam[_idx]->m_ParamSize = sizeof(_type);\
				*(_type*)(pArrFltParam[_idx]->m_ParamValue) = _value;}


				FltParam(FLT_PARAM_AND, BYTE, 1);
				FltParam(FLT_PARAM_ABV, DWORD, 50001);
				FltParam(FLT_PARAM_BEL, DWORD, 50001);
				FltParam(FLT_PARAM_MORE, DWORD, 0x2000003);
				FltParam(FLT_PARAM_LESS, DWORD, 0x2000003);

				ULONG FltTimeout = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 5, FLT_A_INFO | FLT_A_SAVE2CACHE | FLT_A_USECACHE | FLT_A_RESETCACHE, 
					FLTTYPE_FLT, 0, 0, 0, PreProcessing, NULL, 
					pArrFltParam[FLT_PARAM_AND], pArrFltParam[FLT_PARAM_ABV], 
					pArrFltParam[FLT_PARAM_BEL], pArrFltParam[FLT_PARAM_MORE], pArrFltParam[FLT_PARAM_LESS],
					NULL);

				if (FltTimeout == 0)
				{
					Log.AddToLog(L"Error: add filter with parameters failed!");
					gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
				}
				else
				{
					VERDICT VerdictOk, VerdictBad;
					
					pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;

#define _check_flt_op(_str, _op, _type, _val1, _val2) Log.AddToLog(L"Checking flt op " _str L"...");\
					pSingleParam->ParamID = _op;\
					*(_type*)pSingleParam->ParamValue = _val1;\
					pSingleParam->ParamSize = sizeof(_type);\
					if (FALSE == IDriverFilterEvent(FltJob.m_hDevice, pSignleEvt, TRUE, &VerdictOk))\
						 VerdictOk = Verdict_Debug;\
					*(_type*)pSingleParam->ParamValue = _val2;\
					if (FALSE == IDriverFilterEvent(FltJob.m_hDevice, pSignleEvt, TRUE, &VerdictBad))\
						 VerdictBad = Verdict_Debug;\
					if (VerdictOk != Verdict_Pass || VerdictBad != Verdict_NotFiltered)\
					{\
						Log.AddToLog(L"Flt operation" _str L" returned bad result");\
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);\
					}
//+ ----------------------------------------------------------------------------------------
					_check_flt_op(L"AND", FLT_PARAM_AND, BYTE, 1, 2);
//+ ----------------------------------------------------------------------------------------
					_check_flt_op(L"ABV", FLT_PARAM_ABV, DWORD, 50001, 50000);
//+ ----------------------------------------------------------------------------------------
					_check_flt_op(L"BEL", FLT_PARAM_BEL, DWORD, 50001, 50002);
//+ ----------------------------------------------------------------------------------------
					_check_flt_op(L"MORE", FLT_PARAM_MORE, DWORD, 0x2000003, 0x2000004);
//+ ----------------------------------------------------------------------------------------
					_check_flt_op(L"LESS", FLT_PARAM_LESS, DWORD, 0x2000003, 0x2000002);
				}
			}
			
			if (!(_ERR_DRVFAULT_FLT & gDrvEventList.m_ErrorFlags))
			{
				pFlt->m_FltCtl = FLTCTL_RESET_FILTERS;
				if (FALSE == IDriverFilterTransmit(FltJob.m_hDevice, pFlt, pFlt, sizeof(FltTransmit), sizeof(FltTransmit)))
				{
					Log.AddToLog(L"Reset filter failed!");
					gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
				}
			}
			
			if (!(_ERR_DRVFAULT_FLT & gDrvEventList.m_ErrorFlags))
			{
				PFILTER_PARAM pCacheParam = (PFILTER_PARAM) pbParams;
				pCacheParam->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE;
				pCacheParam->m_ParamFlt = FLT_PARAM_WILDCARD;
				pCacheParam->m_ParamID = _PARAM_OBJECT_URL_W;
				lstrcpy((PWCHAR)pCacheParam->m_ParamValue, L"*");
				pCacheParam->m_ParamSize = (lstrlen((PWCHAR)pCacheParam->m_ParamValue) + 1) * sizeof(WCHAR);

				ULONG FltCache = AddFSFilter2(FltJob.m_hDevice, FltJob.m_AppID, gpModuleName, 0, FLT_A_POPUP | FLT_A_SAVE2CACHE | FLT_A_USECACHE, 
					FLTTYPE_FLT, 0, 0, 0, PreProcessing, NULL, pCacheParam, NULL);
				if (!FltCache)
				{
					Log.AddToLog(L"Error: can't add filter for checking cache!");
					gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
				}
				else
				{
					VERDICT Verdict1, Verdict2;
					pSignleEvt->ProcessingType = PreProcessing;
					
					pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
					pSingleParam->ParamID = _PARAM_OBJECT_URL_W;
					lstrcpy((PWCHAR)pSingleParam->ParamValue, L"check cache (must be one such string in log!)");
					pSingleParam->ParamSize = (lstrlen((PWCHAR)pSingleParam->ParamValue) + 1) * sizeof(WCHAR);

					if (FALSE == IDriverFilterEvent(FltJob.m_hDevice, pSignleEvt, FALSE, &Verdict1))
					{
						Log.AddToLog(L"Error: check driver cache!");
						gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
					}
					else
					{
						if (FALSE == IDriverFilterEvent(FltJob.m_hDevice, pSignleEvt, FALSE, &Verdict2))
						{
							Log.AddToLog(L"Error: check driver cache (second event)!");
							gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
						}
						else
						{
							if ((Verdict1 != Verdict_Pass) || (Verdict2 != Verdict_Pass))
							{
								Log.AddToLog(L"Error: check driver cache failed!");
								gDrvEventList.SetError(_ERR_DRVFAULT_FLT);
							}
						}
					}
				}
			}

 			FltJob.ChangeActiveStatus(false);

			while ((pEvt = gDrvEventList.GetFirst()) != NULL)
			{
				Log.AddToLog((PEVENT_TRANSMIT) pEvt);
				gDrvEventList.Free(pEvt);
			}
		}
	}
#endif
	//-
	//- ----------------------------------------------------------------------------------------

	Log.AddToLog(L"\nTest finished.");

	if (gDrvEventList.m_Errors)
	{
		WCHAR err[128];
		wsprintf(err, L"Errors %d, mask %x", gDrvEventList.m_Errors, gDrvEventList.m_ErrorFlags);
		Log.AddToLog(err);
		
		return gDrvEventList.m_ErrorFlags;
	}

	Log.AddToLog(L"\n\nNo errors.");
	
	return 0;
}
