#ifndef _MAILCHECKER_GUARD_OPERATIONS_H_
#define _MAILCHECKER_GUARD_OPERATIONS_H_

#include "..\..\windows\hook\hook\hookspec.h"
#include "..\..\windows\hook\hook\avpgcom.h"
#include "..\..\windows\hook\hook\klmc_api.h"
#include "mchandleops.h"
#include "mcretcodes.h"

#define WAIT_ASK_DELETE_TIMEOUT		21000	//Время, через которое ответ на вопрос драйвера выбирается автоматически
#define MC_MAX_LOADSTRING			250
#define MAX_GUARD_QUEUE_LEN			1024

extern ConnectionParams g_conParams;
extern HANDLE g_hSettingsChanged;

bool StartGuard();
void StopGuard();

typedef struct 
{
	PEXTERNAL_DRV_REQUEST	m_pRequest;
	HANDLE					m_hDevice,
							m_hStopEvent;	
} DriverInfoType;

class CGuardClient
{
public:
	CGuardClient();
	~CGuardClient();

	bool Start ();
	void Stop ();

	void ChangeSetting(ConnectionParams &conParams);

protected:

	bool SetDrvState(DWORD &dwAppID, bool bLive);
	VERDICT OnEvent (PEVENT_TRANSMIT pET);

	static unsigned __stdcall GuardProc(void *pvParam);
//	static unsigned __stdcall CheckDriverProc(void *pvParam);

	APP_REGISTRATION	m_AppReg;
	HANDLE_EX			m_hDevice;
	HANDLE				m_hDriverThread;
	DriverInfoType		m_drvInfo;
};


#endif