#ifndef _PROXY_MANAGER_H__
#define _PROXY_MANAGER_H__

#include "CKAH/CKAHUM.h"
#include "CKAH/ckahcr.h"

#include <prague/prague.h>
#include <prague/iface/i_root.h>
#include <prague/iface/i_reg.h>
#include <ProductCore/iface/i_task.h>
#include <mail/plugin/p_trafficmonitor.h>
#include <mail/structs/s_mc_trafficmonitor.h>

#include "proxysessionmanager.h"

#define DEFAULT_PORT_RANGE			1000	//Range of ports to test

class CProxySessionManager;

class CProxyManager
{
public:    
    hOBJECT                  m_hTrafficMonitor;
    cTrafficMonitorSettings* m_settings;
    CProxySessionManager*    m_manager;    
    
    USHORT                   m_ListenPort;      // порт, на котором намереваемся слушать. ( данные настроек )    
    USHORT                   m_ServicePort;     // порт, на котором слушаем сервисные команды от драйвера
    
    tBOOL                    m_bProxyHasBeenStarted;

    CKAHCR::HPROXY            m_hCRProxy;
    cVector<CKAHCR::HEXCLUDE> m_aExcludes;
    
    tDWORD                   m_state;

    CProxyManager( CProxySessionManager* psm, hOBJECT hTMObject, cTrafficMonitorSettings* settings );

    virtual ~CProxyManager();

    tERROR SetState(tDWORD p_state);
    tDWORD GetState();
    tBOOL  IsRunningState();
    tBOOL  IsPausedState();

    tERROR  StartManager();
    tERROR  PauseManager();
    tERROR  StopManager();
    tERROR  ResetManager( hOBJECT hTrafficMonitor, cTrafficMonitorSettings* pSettings );

    tERROR  RemoveExcludes();

    tERROR  AddPort_and_Excludes( hOBJECT hTrafficMonitor, cTrafficMonitorSettings* pSettings );
};

#endif