// ProxySessionManager.h: interface for the CProxySessionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROXYSESSIONMANAGER_H__BA5BF2C3_A42B_4B0A_94D9_304DFDA42C69__INCLUDED_)
#define AFX_PROXYSESSIONMANAGER_H__BA5BF2C3_A42B_4B0A_94D9_304DFDA42C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mail/structs/s_mc_trafficmonitor.h>
#include "ThreadImp.h"
#include "prague/iface/i_csect.h"
#include <prague/pr_wrappers.h>
#include "ProxySession.h"

class CProxySession;

#define KAVSEND_COMMAND     "KAVSEND"
#define KAVSVC_COMMAND      "KAVSVC"
#define KAVCONN_ID          "KAVCONN_ID"

class CProxySessionManager
{
public:
	CProxySessionManager( hOBJECT hParent = NULL );
	virtual ~CProxySessionManager();

    hOBJECT           m_hParent;
    tWORD             m_ListenPort; // порт, на котором реально слушаем
    tWORD             m_ServPort;
    
    DWORD             m_ThreadID;
    HANDLE            m_ThreadHandle;

	tERROR Init();
	tERROR NeedToSwitchToSSL(cTrafficSelfAskAction* pAskAction);

    tERROR  ProxyMain( SOCKET Server, SOCKET Server_v6, SOCKET CrService, SOCKET CrService_v6 );
    
    // запуск потока Proxy.
    bool    StartManager( USHORT ListenPort, USHORT ServPort );
    tERROR  ProxyManagerThread();
    void    WaitExitThread();

    // остановка всех клиентов и выход из потока прокси
    tERROR  StopManager();

    // приостановка компоненты.
    tERROR  PauseManager();

    CProxySession*  FindSessionByID( __int64& ID );
    
public:
    bool                m_bInited; 
	cCriticalSection*   m_sync;
	HANDLE              m_hRequestSessionStop;		//  акой-то протоколлер захотел остановитьс€
	cVector<hOBJECT>    m_hStoppingProtocollers;	// ’эндлы протоколлера, который захотел остановитьс€
	
    HANDLE              m_hStopEvent;
    
    HANDLE              m_ClientsSemaphore;	
    LONG                m_SessionCount;

	hMSG_RECEIVER           m_hMsgReceiver;	             // ѕриниматель сообщений от протоколлеров
	
    // —писок  запущенных сессий.   списку нет лочки, поэтому доступ к нему только в 1-м потоке !!!
    std::list<CProxySession*>   m_SessionList;      

    // —писок запущенных, но неактивных сессий. ” этих сессий нет своего текущего потока.
    CRITICAL_SECTION            m_WaitSessionListLock;
    std::list<CProxySession*>   m_WaitSessionList;

    // DWORD               GetEvents( std::vector<HANDLE>& handles );
    DWORD               GetDescriptors( fd_set* set );

    //CProxySession*      GetClient( DWORD Index, std::vector<HANDLE>& handles  );
    CProxySession*      GetClient( fd_set* set );
    
    void                AddClient( CProxySession* Client );
    
    void                ProcessPseudoStop();
    
    void                ProcessNewConnection( SOCKET Server );
    
    void                StartClientThread();
    
    void                FinishActiveSessions();
};

#endif // !defined(AFX_PROXYSESSIONMANAGER_H__BA5BF2C3_A42B_4B0A_94D9_304DFDA42C69__INCLUDED_)
