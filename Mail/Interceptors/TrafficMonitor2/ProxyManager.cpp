#include "stdafx.h"
#include "ProxyManager.h"
#include <Updater/updater.h>
#include <ProductCore/iptoa.h>
#include <CKAH/ipconv.h>

CProxyManager::CProxyManager(CProxySessionManager* psm, hOBJECT hTMObject, cTrafficMonitorSettings* settings ) :
    m_manager(psm),             m_hTrafficMonitor(hTMObject),
    m_settings(settings),       m_ListenPort( 1110 ),
    m_ServicePort(19780),
    m_hCRProxy( NULL ),         m_bProxyHasBeenStarted( cFALSE )
{
}

CProxyManager::~CProxyManager()
{
}

tERROR
CProxyManager::StartManager()
{
	if ( 0 != SockInitialize() )
	{
		KLSTD_TRACE0(KLMC_TRACELEVEL, "SockInitialize failed\n");
		return errUNEXPECTED;
	}

	cStrObj str("%Bases%\\CKAH.set");            
    m_hTrafficMonitor->sysSendMsg( pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (str), 0, 0 );
    cStrBuff wstrManifest(str, cCP_UNICODE);

    KLSTD_TRACE1(MC_TRACE_LEVEL, "MC_TM\tmanifest %S", (LPWSTR)wstrManifest);

    CKAHUM::OpResult opres = CKAHUM::srOK;

    {
        cBasesLoadLocker lck1(m_hTrafficMonitor, cUPDATE_CATEGORY_ANTIHACKER_I386, cBasesLoadLocker::withLock);
        cBasesLoadLocker lck2(m_hTrafficMonitor, cUPDATE_CATEGORY_ANTIHACKER_X64, cBasesLoadLocker::withLock);
        opres = CKAHUM::Reload( (LPWSTR)wstrManifest);
    }

    if ( CKAHUM::srNeedReboot == opres )
    {
        // нужен рестарт продукта, т.к. появились новые базы или восстановлены поврежденные.
        // Если базы старые, то запускаем работу на текущих базах
        KLSTD_TRACE0(MC_TRACE_LEVEL, "MC_TM\tneed restart after update!");
        m_hTrafficMonitor->sysSendMsg(pmc_TASK_OS_REBOOT_REQUEST, 0, m_hTrafficMonitor, 0, 0);
    }

    if ( m_bProxyHasBeenStarted )
    {
        ResetManager( m_hTrafficMonitor, m_settings );        
        return errOK;        
    }
    
    USHORT FinalPort;
    if( !ScanPorts( m_ListenPort, DEFAULT_PORT_RANGE, FinalPort ) )
    {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Error scanning ports for proxy\n");
        return errUNEXPECTED;
    }
    m_ListenPort = FinalPort;

    if( !ScanPorts( m_ServicePort, DEFAULT_PORT_RANGE, FinalPort ) )
    {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Error scanning ports for proxy\n");
        return errUNEXPECTED;
    }
    m_ServicePort = FinalPort;

    if ( CKAHUM::srOK != CKAHCR::Start() ) {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Error starting CKAHCR\n");
        return errUNEXPECTED;
    }
    
    opres = m_settings->m_bUseKavsend ? CKAHUM::srOpFailed : CKAHCR::RegisterProxy2( m_ListenPort, m_ServicePort, ::GetCurrentProcessId(), &m_hCRProxy );


    if ( CKAHUM::srOK == opres )
    {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Mailtask : Set complex mode\n" );
    }
    else
    {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Mailtask : Set normal mode\n" );

        opres = CKAHCR::RegisterProxy( m_ListenPort, ::GetCurrentProcessId(), &m_hCRProxy );    
    }

    if ( CKAHUM::srOK != opres ) 
    {
        KLSTD_TRACE1(KLMC_TRACELEVEL, "Error RegisterProxy CKAHCR: %d\n", opres);
        return false;
    }


    AddPort_and_Excludes( m_hTrafficMonitor, m_settings );

    m_manager->StartManager( m_ListenPort, m_ServicePort );

    if ( CKAHUM::srOK != CKAHCR::Resume() ) {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Error resuming CKAHCR:\n");
        return errUNEXPECTED;
    }

    m_bProxyHasBeenStarted = cTRUE;

    return errOK;
}

tERROR
CProxyManager::PauseManager()
{
    ResetManager( m_hTrafficMonitor, NULL );        
    
    return errOK;
}

tERROR
CProxyManager::StopManager()
{  
    CKAHCR::Drv_SetQuitEvent();

    m_manager->StopManager();

    CKAHCR::Proxy_Unregister( m_hCRProxy );        
    CKAHCR::Stop();

    m_aExcludes.clear();

	SockDone();

	return errOK;
}

tERROR
CProxyManager::ResetManager( hOBJECT hTrafficMonitor, cTrafficMonitorSettings* pSettings )
{
    CKAHUM::OpResult opres = CKAHUM::srOK;

    opres = CKAHCR::Proxy_Unregister( m_hCRProxy );

    if ( CKAHUM::srOK == opres )
    {
        RemoveExcludes();

        if ( pSettings )
        {
            opres = CKAHCR::RegisterProxy2( m_ListenPort, m_ServicePort, ::GetCurrentProcessId(), &m_hCRProxy );

            if ( CKAHUM::srOK == opres )
            {
                KLSTD_TRACE0(KLMC_TRACELEVEL, "Mailtask : Set complex mode\n" );
            }
            else
            {
                KLSTD_TRACE0(KLMC_TRACELEVEL, "Mailtask : Set normal mode\n" );

                opres = CKAHCR::RegisterProxy( m_ListenPort, ::GetCurrentProcessId(), &m_hCRProxy );    
            }

            if ( CKAHUM::srOK == opres )
            {
                AddPort_and_Excludes( hTrafficMonitor, pSettings );
            }
        }
    }

    return errOK;
}

tDWORD
CProxyManager::GetState()
{
    return m_state;
}

tBOOL
CProxyManager::IsRunningState()
{
    return (m_state == TASK_STATE_RUNNING);
}

tBOOL
CProxyManager::IsPausedState()
{
    return (m_state == TASK_STATE_PAUSED);
}


tERROR
CProxyManager::RemoveExcludes()
{
    for ( tDWORD i = 0; i < m_aExcludes.size(); i++ )
        CKAHCR::Proxy_RemoveExclude( m_aExcludes.at(i) );

    m_aExcludes.clear();

    return errOK;
}

#define PROXY_ADDPORT(_proxy, _port, _enabled)	\
if ( _enabled )	                                                            \
{	                                                                        \
    if ( CKAHUM::srOK != (opres = CKAHCR::Proxy_AddPort( _proxy, _port )))	\
    {	                                                                    \
        KLSTD_TRACE1(KLMC_TRACELEVEL, "Error Proxy_AddPort(%d)", _port);	\
        return false;	                                                    \
    }	                                                                    \
    else	                                                                \
    {	                                                                    \
        KLSTD_TRACE1(KLMC_TRACELEVEL, "Proxy_AddPort(%d)", _port);	        \
    }	                                                                    \
}

tERROR  
CProxyManager::AddPort_and_Excludes( hOBJECT hTrafficMonitor, cTrafficMonitorSettings* pSettings )
{
    CKAHUM::OpResult opres;
    tDWORD dwCounter = 0; 
    CKAHCR::HEXCLUDE he;

    for ( dwCounter = 0; dwCounter < pSettings->m_aExcludes.count(); dwCounter++ )
    {
        if ( pSettings->m_aExcludes.at(dwCounter).m_bEnabled && 
            !pSettings->m_aExcludes.at(dwCounter).m_bSSLOnly )
        {
            cStrObj sImagePath(pSettings->m_aExcludes.at(dwCounter).m_sImagePath);
            hTrafficMonitor->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sImagePath), 0, 0);

            if ( 
                INVALID_FILE_ATTRIBUTES == GetFileAttributesW((wchar_t*)sImagePath.c_str(cCP_UNICODE)) &&
                INVALID_FILE_ATTRIBUTES == GetFileAttributesA(( char*  )sImagePath.c_str(cCP_ANSI))
                )
                continue;

            cVector<cIP> hosts;
            cVector<tWORD> ports;

            if (pSettings->m_aExcludes.at(dwCounter).m_Hosts.size())
                hosts = pSettings->m_aExcludes.at(dwCounter).m_Hosts;
            else
                hosts.push_back(cIP( (tDWORD)0 ));

            if (pSettings->m_aExcludes.at(dwCounter).m_Ports.size())
                ports = pSettings->m_aExcludes.at(dwCounter).m_Ports;
            else
                ports.push_back(0);

            for ( tDWORD h = 0; h < hosts.size(); ++h)
            {
                for ( tDWORD p = 0; p < ports.size(); ++p)
                {
                    opres = CKAHCR::Proxy_AddExclude( 
                        &cIP_CKAHUMIP(hosts[h]),
                        ports[p],
                        (wchar_t*)sImagePath.c_str(cCP_UNICODE),
                        sImagePath.memsize(cCP_UNICODE),
                        &he
                        );

                    KLSTD_TRACE4(KLMC_TRACELEVEL, "Proxy_AddExclude %s (%s, %d, %s)", 
                        ( CKAHUM::srOK == opres ) ? "OK" : "ERROR",
                        cIPToA(hosts[h]),
                        ports[p],
                        (char*)sImagePath.c_str(cCP_ANSI),
                        );

                    if ( CKAHUM::srOK == opres )
                        m_aExcludes.push_back( he );
                }
            }
        }
    }

    if ( pSettings->m_bAllPorts )
    {
        PROXY_ADDPORT(m_hCRProxy, 0, pSettings->m_bAllPorts);
    }
    else
    {
        for ( dwCounter = 0; dwCounter < pSettings->m_aPorts.count(); dwCounter++ )
        {
            PROXY_ADDPORT(m_hCRProxy, pSettings->m_aPorts.at(dwCounter).m_nPort, pSettings->m_aPorts.at(dwCounter).m_bEnabled);
        }
    }

    return true;
}