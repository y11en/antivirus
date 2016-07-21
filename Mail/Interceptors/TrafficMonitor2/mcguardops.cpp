#include "stdafx.h"
#include "mcguardops.h"

#include "..\..\windows\hook\hook\idriver.h"
#include "..\..\windows\hook\hook\fsdrvlib.h"

#define CL_FLAGS (_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE)

CGuardClient g_GuardClient;
ConnectionParams g_conParams;
HANDLE g_hSettingsChanged;

//////////////////////////////////////////////////////////////////////////

CGuardClient::CGuardClient ()
{
	m_hDriverThread = NULL;
	ZeroMemory(&m_AppReg, sizeof(m_AppReg));
	g_hSettingsChanged = CreateEvent(0,0,0,0);
}

CGuardClient::~CGuardClient ()
{
	CLOSEHANDLE(g_hSettingsChanged);
}

bool CGuardClient::Start ()
{
	if ((HANDLE)m_hDevice != NULL)
		return false;

	if (!FSDrvInterceptorInitEx (CL_FLAGS, MailChecker_PPP, AVPG_STANDARTRIORITY, 100))
	{			
		KLSTD_TRACE1(KLMC_TRACELEVEL, "FSDrvInterceptorInitEx failed: last error is %#x\n", GetLastError());
		return false;
	}

	KLSTD_TRACE0(KLMC_TRACELEVEL, "FSDrvInterceptorInitEx success\n");
	FSDrvInterceptorSetActive(TRUE);

	m_AppReg.m_AppID = MailChecker_PPP;
	m_AppReg.m_Priority = AVPG_STANDARTRIORITY;
	m_AppReg.m_ClientFlags = CL_FLAGS;
	m_AppReg.m_CurProcId = ::GetCurrentProcessId();
	
	// Таймаут, через который мы отваливаемся (в секундах)
	m_AppReg.m_BlueScreenTimeout = WAIT_ASK_DELETE_TIMEOUT/1000; 

	unsigned long uThreadID = 0;
	m_hDriverThread = (HANDLE)CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)GuardProc, (void*)this, 0, &uThreadID );

	if (!IS_VALID_HANDLE(m_hDriverThread))
		return false;

	HANDLE Handles[2] = {m_hDriverThread, m_hDevice.GetInitializeEvent()};

	DWORD dwRet;
	switch(dwRet = ::WaitForMultipleObjects(2, Handles, FALSE, INFINITE))
	{
	case WAIT_OBJECT_0:
		KLSTD_TRACE0(KLMC_TRACELEVEL, "Guard client thread returns an error\n");
		CloseValidHandle (m_hDriverThread);
		m_hDriverThread = NULL;
		m_hDevice = NULL;
		return false;
	case WAIT_OBJECT_0 + 1:
		break;
	default:
		//not reach
		assert (0);
	}

	return true;
}

void CGuardClient::Stop ()
{
	if( IS_VALID_HANDLE( m_hDevice.GetStopEvent() ) )
		::SetEvent(m_hDevice.GetStopEvent());	

	StopThread(m_hDriverThread);
	m_hDriverThread = NULL;
	m_hDevice = NULL;

	KLSTD_TRACE0(KLMC_TRACELEVEL, "FSDrvInterceptorDone...\n");
	FSDrvInterceptorDone();
}

bool CGuardClient::SetDrvState (DWORD &dwAppID, bool bLive)
{	
		APPSTATE AppState = _AS_Dead,
				 RequiredState;

		APPSTATE_REQUEST Request;

		if( bLive )
		{
			RequiredState = _AS_Active;
			Request = _AS_GoActive;
		} else
		{
			RequiredState = _AS_Sleep;
			Request = _AS_GoSleep;
		}

		HDSTATE state;
		
		IDriverGetState ((HANDLE)m_hDevice, dwAppID, &state);

		if(state.Activity != (UINT)RequiredState)
		{
			IDriverState ((HANDLE)m_hDevice, dwAppID, Request, &AppState);
			if(AppState != RequiredState)
				return false;
		}

		return true;
}

VERDICT CGuardClient::OnEvent (PEVENT_TRANSMIT pET)
{	
	return Verdict_Pass;
}

void CGuardClient::ChangeSetting(ConnectionParams &conParams)
{
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_bCheckPOP3Traffic = %d", conParams.m_bCheckPOP3Traffic);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_bCheckSMTPTraffic = %d", conParams.m_bCheckSMTPTraffic);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_dwProxyPID = %d", conParams.m_dwProxyPID);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_sPOP3ProxyPort = %d", conParams.m_sPOP3ProxyPort);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_sPOP3ServerPort = %d", conParams.m_sPOP3ServerPort);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_sSMTPProxyPort = %d", conParams.m_sSMTPProxyPort);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_sSMTPServerPort = %d", conParams.m_sSMTPServerPort);
	KLSTD_TRACE1(KLMC_TRACELEVEL, "CGuardClient::ChangeSetting => m_ulPID = %d", conParams.m_ulPID);

	BYTE	MyBuffer[ MC_MAX_LOADSTRING ];
	memset(MyBuffer, 0, MC_MAX_LOADSTRING);

	PEXTERNAL_DRV_REQUEST pRequest;	
	unsigned long uRequestSize = 0;		

	pRequest = (PEXTERNAL_DRV_REQUEST)MyBuffer;
	pRequest->m_BufferSize = sizeof(ConnectionParams);
	pRequest->m_DrvID = m_drvInfo.m_pRequest->m_DrvID;
	pRequest->m_IOCTL = m_drvInfo.m_pRequest->m_IOCTL;
	pRequest->m_AppID = m_drvInfo.m_pRequest->m_AppID;
	uRequestSize = sizeof(pRequest);
	memcpy (MyBuffer+sizeof(EXTERNAL_DRV_REQUEST), &conParams, sizeof(ConnectionParams));				

	IDriverExternalDrvRequest(  m_drvInfo.m_hDevice, pRequest, pRequest, &uRequestSize );
}

//MC_CHANGEDRV_STATE_ERR
//MC_DRVREGFAILED_ERR
//MC_GETOSVERINFO_ERR
//MC_OK
unsigned __stdcall CGuardClient::GuardProc(void *pvParam)
{	
	CGuardClient* pThis = (CGuardClient*)pvParam;
	PAPP_REGISTRATION pAppReg = &(pThis->m_AppReg);

		DWORD dwSite = 0;
		OSVERSIONINFO OSVer;
		HANDLE hEvents[4] = {NULL, NULL, pThis->m_hDevice.GetStopEvent(), g_hSettingsChanged}, 
				hStopCheckDriverEvent = NULL,
				hCheckDriverThread = NULL,
				hDevice = NULL;

		BYTE btParams[ MC_MAX_LOADSTRING ] = "";
		PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST)btParams;

		DriverInfoType&	drvInfo = pThis->m_drvInfo;

		pRequest->m_DrvID = FLTTYPE_MCHECK;
		pRequest->m_IOCTL = KLMC_SET_PARAMS;
		pRequest->m_AppID = pAppReg->m_AppID;

		APPSTATE AppState = _AS_Dead;
		APPSTATE RequiredState = _AS_Active;
		APPSTATE_REQUEST Request = _AS_GoActive;

		HDSTATE hClientState;

		GET_EVENT ge = {pAppReg->m_AppID, 0};
		BYTE buff[ MAX_GUARD_QUEUE_LEN ];
		DWORD BytesRet = 0;
		PEVENT_TRANSMIT pET = (PEVENT_TRANSMIT)buff;

		OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		char	szWhistleUp[PROCNAMELEN] = "",
				szWFChanged[PROCNAMELEN] = "";

		unsigned int uThreadID = 0, uErrCode = MC_OK;

		if(!GetVersionEx(&OSVer))
		{		
			uErrCode = MC_GETOSVERINFO_ERR;
			goto FINALIZE;
		}
		
		_snprintf(szWhistleUp, PROCNAMELEN, "Whistleup%u%u%u", ::GetCurrentProcessId (), ::GetCurrentThreadId(), ::GetTickCount());
		_snprintf(szWFChanged, PROCNAMELEN, "WFChanged%u%u%u", ::GetCurrentProcessId (), ::GetCurrentThreadId(), ::GetTickCount());

		hDevice = RegisterApp(pAppReg, &OSVer, &hEvents[0], &hEvents[1], szWhistleUp, szWFChanged);

		if( !(IS_VALID_HANDLE((HANDLE)hDevice) &&
			  IS_VALID_HANDLE(hEvents[0]) &&
			  IS_VALID_HANDLE(hEvents[1])) )
		{
			KLSTD_TRACE0(KLMC_TRACELEVEL, "Error registering application in driver\n");
			uErrCode = MC_DRVREGFAILED_ERR;
			goto FINALIZE;
		}

		if( !pThis->SetDrvState(pAppReg->m_AppID, true) )
		{			
			KLSTD_TRACE0(KLMC_TRACELEVEL, "Cannot change driver state\n");
			uErrCode = MC_CHANGEDRV_STATE_ERR;
			goto UNREGANDFIN;
		}
		
		// return ThreadID from driver if successful, zero if failed		
		if( IDriverRegisterInvisibleThread(hDevice, pAppReg->m_AppID) == 0 )
		{
			KLSTD_TRACE0(KLMC_TRACELEVEL, "Error registering invisible thread in driver\n");
		}

		if (AddFSFilter2(hDevice, pAppReg->m_AppID, "*", 0, 
					FLT_A_POPUP | FLT_A_PASS, 
					FLTTYPE_MCHECK, MJ_CONN_ACTION, -1, 0,
					PreProcessing, NULL, NULL) == 0)
		{
			KLSTD_TRACE0(KLMC_TRACELEVEL, "Cannot add MailChecker filters\n");
			goto UNREGANDFIN;
		}

		pThis->m_hDevice = hDevice; // this will triggers WaitForSingle...
		
		//Сообщаем драйверу настройки
		hStopCheckDriverEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if( !IS_VALID_HANDLE(hStopCheckDriverEvent) )
		{
			KLSTD_TRACE0(KLMC_TRACELEVEL, "Cannot create hStopCheckDriverEvent\n");
		}

		drvInfo.m_hDevice = hDevice;
		drvInfo.m_pRequest = pRequest;
		drvInfo.m_hStopEvent = hStopCheckDriverEvent;

		//Создание потока сообщающего драйверу об изменении настроек
		AddFSFilter2(drvInfo.m_hDevice, drvInfo.m_pRequest->m_AppID, "*",0,
			FLT_A_USECACHE | FLT_A_CHECKNEXTFILTER | FLT_A_PASS,
			FLTTYPE_MCHECK, 0, 0, 0, PreProcessing, &dwSite, NULL, NULL);

		g_GuardClient.ChangeSetting(g_conParams);

		while( true )
		{
			VERDICT Verdict = Verdict_Pass;
			
			DWORD dwResult = WaitForMultipleObjects(4, hEvents, FALSE, INFINITE);

			if( (dwResult == WAIT_OBJECT_0) || (WAIT_TIMEOUT == dwResult) )
			{			
				IDriverGetState(hDevice, pAppReg->m_AppID, &hClientState);
						
				if (hClientState.QUnmarkedLen == 0)			
					continue;			

				while (hClientState.QUnmarkedLen > 0)
				{
					++ge.Mark;

					--hClientState.QUnmarkedLen;			

					if(DeviceIoControl(hDevice, IOCTLHOOK_GetEvent, &ge, sizeof(GET_EVENT), buff, MAX_GUARD_QUEUE_LEN, &BytesRet, NULL))
					{
						if(BytesRet == 0)
						{
							IDriverSkipEvent(hDevice, pAppReg->m_AppID);
							continue;
						} 
						else
						{
							IDriverSetVerdict(hDevice, pAppReg->m_AppID, pThis->OnEvent(pET), ge.Mark, 0);
						}
					} 
				}
			} 
			else if(dwResult == (WAIT_OBJECT_0 + 2))
			{
				break;			
			} 
			else if(dwResult == (WAIT_OBJECT_0 + 3))	// g_hSettingsChanged
			{
				g_GuardClient.ChangeSetting(g_conParams);
			} 
		}
		
		if( IS_VALID_HANDLE(hStopCheckDriverEvent) )
		{
			::SetEvent(hStopCheckDriverEvent);
			StopThread(hCheckDriverThread);
		}

UNREGANDFIN:
		IDriverUnregisterInvisibleThread(hDevice, pAppReg->m_AppID); // return ThreadID from driver if successful, zero if failed
		
		if( IS_VALID_HANDLE(hDevice) )
		{
			IDriverState(hDevice, pAppReg->m_AppID, _AS_GoSleep, NULL);
			KLSTD_TRACE0(KLMC_TRACELEVEL, "Disconnect from driver...\n");
			UnRegisterApp(hDevice, pAppReg, FALSE, &OSVer);
			hDevice = NULL;
		}		

		return MC_OK;

FINALIZE:

		pThis->m_hDevice = NULL;

		return uErrCode;
}

//MC_FSDRVINIT_ERR
//MC_CREATE_HANDLE_ERR
//MC_OK
bool StartGuard()
{
	return g_GuardClient.Start();
}

void StopGuard()
{
	g_GuardClient.Stop();
}
