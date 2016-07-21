#ifndef __EVTHREAD_H
#define __EVTHREAD_H

#include <windows.h>
#include <process.h>

#include "..\..\hook\avpgcom.h"

#if defined( __cplusplus )
extern "C" 
{
#endif

typedef struct _THREAD_PARAM
{
	HANDLE hDevice;
	DWORD AppID;
	HANDLE hWhistleup;
	GET_EVENT Event;
	SET_VERDICT SetVerdict;
	OSVERSIONINFO* pOSVer;
	BYTE bEventData[sizeof(EVENT_TRANSMIT) + (MAX_PATH + 1) * 4];
}THREAD_PARAM, *PTHREAD_PARAM;

DWORD WINAPI WatchDog(void* pParam);
DWORD WINAPI EventWaiterThread(void* pParam);

extern LONG EvExit;
#if defined( __cplusplus )
}
#endif


#endif