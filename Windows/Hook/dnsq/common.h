#ifndef __COMMON_H
#define __COMMON_H
//-----------------------------------------------------------------

typedef struct _CLIENT_CONTEXT
{
	APP_REGISTRATION	m_AppReg;
	HANDLE				m_hDevice;
	HANDLE				m_hWhistleup;
	HANDLE				m_hWFChanged;
} CLIENT_CONTEXT, *PCLIENT_CONTEXT;

//-----------------------------------------------------------------
#endif