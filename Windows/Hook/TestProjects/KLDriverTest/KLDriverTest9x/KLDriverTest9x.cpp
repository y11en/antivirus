// KLDriverTest9x.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CheckJob.h"

#include <stdio.h> 
#define  NullStr "\0"
CHAR *UnknownStr="-?-";
#include "..\..\..\Ntcommon.cpp"
#include "..\..\..\vxdcommn.cpp"

//+ ----------------------------------------------------------------------------------------

DrvEventList gDrvEventList;

//+ ----------------------------------------------------------------------------------------


class LogEvent
{
#define _endofline	"\n"
#define _newevent	"new_event ------------------------------------------------------------\n"
#define _endevent	"end_of_event ---------------------------------------------------------\n\n"
	
private:
	HANDLE m_hFile;
public:
	LogEvent()
	{
		m_hFile = CreateFile("TestDrv.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
			gDrvEventList.SetError(_ERR_LOG);
	}
	~LogEvent()
	{
		if (INVALID_HANDLE_VALUE == m_hFile)
			return;
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	HANDLE GetFile() {return m_hFile;};

public:
	void AddToLog(LPCSTR pwch, BOOL endofline = TRUE)
	{
		BOOL bWriteSucc = TRUE;
		if (INVALID_HANDLE_VALUE == m_hFile)
			return;
		
		DWORD dwWritten;
		bWriteSucc &= WriteFile(m_hFile, pwch, lstrlen(pwch), &dwWritten, NULL);
		if (endofline)
			bWriteSucc &= WriteFile(m_hFile, _endofline, lstrlen(_endofline), &dwWritten, NULL);
		
		if (!bWriteSucc)
			gDrvEventList.SetError(_ERR_ADDLOG);
		else
			FlushFileBuffers(m_hFile);
	}

	//+ ----------------------------------------------------------------------------------------
	//+ log main function

	void AddToLog(PEVENT_TRANSMIT pEvt)
	{
		if (INVALID_HANDLE_VALUE == m_hFile)
			return;
		
		CHAR pBuff[1024];
		
		AddToLog(_newevent, FALSE);

		CHAR EventDescr[512];

		VxDGetFunctionStr(EventDescr, pEvt->m_HookID, pEvt->m_FunctionMj, pEvt->m_FunctionMi);
		AddToLog(EventDescr);
		
		PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvt->m_Params;
		for (DWORD cou =  0; (cou < pEvt->m_ParamsCount); cou++)
		{
			wsprintf(pBuff, "Param %#x, size %d", pSingleParam->ParamID, pSingleParam->ParamSize);
			AddToLog(pBuff, FALSE);
			
			switch (pSingleParam->ParamID)
			{
			case _PARAM_OBJECT_ACCESSATTR:
				{
					if (pSingleParam->ParamSize == sizeof(DWORD))
						wsprintf(pBuff, "...Access attributes. Value %#x", *(DWORD*)(pSingleParam->ParamValue));
					else
					{
						AddToLog("...<length mismatch>", FALSE);
						gDrvEventList.SetError(_ERR_EVENT);
					}
					
					AddToLog(pBuff, FALSE);
				}
				break;
			case _PARAM_OBJECT_ORIGINAL_PACKET:
				AddToLog("...Original packet <changeable>", FALSE);
				break;
			case _PARAM_OBJECT_SOURCE_ID:
				AddToLog("...Source ID <changeable>", FALSE);
				break;
			case _PARAM_OBJECT_DEST_ID:
				AddToLog("...Destination ID <changeable>", FALSE);
				break;
			case _PARAM_OBJECT_CONTEXT_FLAGS:
				wsprintf(pBuff, "...Context flags. Value %#x", *(DWORD*)(pSingleParam->ParamValue));
				AddToLog(pBuff, FALSE);
				break;
			case _PARAM_OBJECT_SID:
				{
					CHAR pwszDomain[MAX_PATH];
					CHAR pwszUser[MAX_PATH];
					
					DWORD dwDomain = sizeof(pwszDomain);
					DWORD dwUser = sizeof(pwszUser);

					wsprintf(pBuff, "...SID Value: ");
					AddToLog(pBuff, FALSE);
					if (!IDriverGetUserFromSid((SID*) pSingleParam->ParamValue, pwszDomain, &dwDomain, pwszUser, &dwUser))
					{
						AddToLog("<invalid>", FALSE);
					}
					else
					{
						AddToLog(pwszDomain, FALSE);
						AddToLog("/", FALSE);
						AddToLog(pwszUser, FALSE);
					}
				}
				break;
			default:
				{
					switch (pSingleParam->ParamID)
					{
					case _PARAM_OBJECT_BINARYDATA:
					case _PARAM_OBJECT_DATALEN:
					case _PARAM_OBJECT_SECTOR:
					case _PARAM_OBJECT_BYTEOFFSET:
						{
							if (pSingleParam->ParamSize == sizeof(DWORD))
								wsprintf(pBuff, "...%#x", *(DWORD*)(pSingleParam->ParamValue));
							else
								wsprintf(pBuff, "...<hex data>");

							AddToLog(pBuff, FALSE);
						}
						break;
					case _PARAM_OBJECT_URL:
					case _PARAM_OBJECT_URL_DEST:
						AddToLog("...", FALSE);
						AddToLog(pSingleParam->ParamID == _PARAM_OBJECT_URL_W ? "Ur" : "Dest ur", FALSE);
						AddToLog(" Value: ", FALSE);
						AddToLog((LPSTR)pSingleParam->ParamValue, FALSE);
						break;
					default:
						AddToLog("...unknown param");
					}
				}
			}
			AddToLog(_endofline, FALSE);
			
			if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
				pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
			else
				pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
		}
		
		AddToLog(_endevent, FALSE);
	}	
	//- end log main function
	//- ----------------------------------------------------------------------------------------
};


//+ ----------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	LogEvent Log;
	BYTE* pEvt;

	//+ ----------------------------------------------------------------------------------------
	//+ file test
	Log.AddToLog("Starting file interceptor check...\n");
	CheckJob_File FileJob(&gDrvEventList);
	if (false == FileJob.Start())
	{
		Log.AddToLog("Can't start check file interceptor!");
		gDrvEventList.SetError(_ERR_DRVFAULT);
	}
	else
	{
		FileJob.SetAllowEvents(true);
		FileJob.ChangeActiveStatus(true);

#define _testfilename			"TestFile.tst"
#define _testfilename_renamed	"TestFile renamed.tst"
#define _testfilename_create	"TestFile check create.tst"

		HANDLE hTestFile = CreateFile(_testfilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTestFile == INVALID_HANDLE_VALUE)
		{
			FileJob.ChangeActiveStatus(false);

			Log.AddToLog("Can't create file TestFile.tst for test");
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
				Log.AddToLog("!Write to TestFile.tst failed");
				gDrvEventList.SetError(_ERR_INTERNAL);
			}
			
			FlushFileBuffers(hTestFile);

			SetFilePointer(hTestFile, 0, NULL, FILE_BEGIN);

			ReadFile(hTestFile, buf, sizeof(buf), &dwBytes, NULL);
			if (dwBytes != sizeof(buf))
			{
				Log.AddToLog("!Read from TestFile.tst failed");
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
				Log.AddToLog("protection fault - file "_testfilename_create " opened");
				gDrvEventList.SetError(_ERR_DRVFAULT);
				CloseHandle(hTestFile);
			}
		}
		
		FileJob.ChangeActiveStatus(false);
	}

	while ((pEvt = gDrvEventList.GetFirst()) != NULL)
	{
		Log.AddToLog((PEVENT_TRANSMIT) pEvt);
		gDrvEventList.Free(pEvt);
	}
	
	//- end files test
	//- ----------------------------------------------------------------------------------------

	Log.AddToLog("\n\nNo errors.");
	
	return 0;
}
