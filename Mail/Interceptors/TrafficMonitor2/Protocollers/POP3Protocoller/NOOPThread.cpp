// NOOPThread.cpp: implementation of the CNOOPThread class.
//
//////////////////////////////////////////////////////////////////////

#include "NOOPThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNOOPThread::CNOOPThread():
	m_bStarted(cFALSE),
	m_dwWaitingForResponse(0)
{
}

CNOOPThread::~CNOOPThread()
{
	Stop();
}

tERROR CNOOPThread::Init(InitParams_t* pInitParams)
{
	if ( !pInitParams ) 
		return errNOT_OK;
	cERROR err = errOK;
	m_InitParams = *pInitParams;
	return errOK;
}

tERROR CNOOPThread::_Run()
{
	InterlockedExchange(&m_bStarted, 1);

	hIO hNoopData = NULL;
	tDWORD dwWritten = 0;
	
    cERROR err = m_InitParams.hParent->sysCreateObjectQuick( (hOBJECT*)&hNoopData, IID_IO, PID_TMPFILE );	
    if ( PR_SUCC(err) )
		err = hNoopData->SeekWrite( &dwWritten, 0, m_InitParams.data, m_InitParams.size );
	if ( PR_SUCC(err) )
	{
		tDWORD dwRes = 0;
		process_msg_t process_msg;

		process_msg.assign(*m_InitParams.pSession, false);
		process_msg.dsDataTarget = m_InitParams.dsTo;
		process_msg.psStatus     = psKeepAlive;
		process_msg.hData        = hNoopData;

		while ( WAIT_TIMEOUT == (dwRes = WaitForSingleObject(m_hStopEvent, m_InitParams.dwTimeout)) )
		{
			err = m_InitParams.hParent->sysSendMsg( 
				                            pmcTRAFFIC_PROTOCOLLER, 
				                            pmKEEP_ALIVE, 
				                            0, 
				                            &process_msg, 
				                            SER_SENDMSG_PSIZE 
				                            );
			if ( PR_SUCC(err) )
			{
				InterlockedIncrement(&m_dwWaitingForResponse);
			}
		}
	}

	if ( hNoopData )
    {
		hNoopData->sysCloseObject();
	    hNoopData = NULL;
    }

    InterlockedExchange(&m_bStarted, 0);

	return errOK;
}

tERROR CNOOPThread::Start(session_t* pSession, tCHAR* data, tDWORD size, data_target_t dsTo, tDWORD dwTimeout, hOBJECT hParent)
{
	CNOOPThread::InitParams_t hNOOPParams;

	hNOOPParams.hParent   = hParent;
	hNOOPParams.pSession  = pSession;
	hNOOPParams.data      = (tBYTE*)data;
	hNOOPParams.size      = size;
	hNOOPParams.dsTo      = dsTo;
	hNOOPParams.dwTimeout = dwTimeout;
	
    cERROR err = Init(&hNOOPParams);

	if ( PR_SUCC(err) )
	{
		InterlockedExchange(&m_bStarted, 1);
		err = CThreadImp::Start("NOOP");
	}
	return err;
}

void  CNOOPThread::FlushResponse()
{
	InterlockedDecrement(&m_dwWaitingForResponse);
};