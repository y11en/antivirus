
#include "CNetWatcherTask.h"
#include "ntddndis.h"


DWORD CNetWatcherTask::FindZoneByNet(NETWATCH::Network *netw)
{
	if(netw->OperStatus!=IfOperStatusUp || 
		(netw->number_of_ipv4==0 && netw->number_of_ipv6==0))
		return 0;

	cNetZone *pZona;
	DWORD index;
	int level=-1;
	for(DWORD x=1;x<m_zones.size();x++)
	{	
		pZona =&m_zones[x];
		if(netw->m_nIfType==IF_TYPE_SOFTWARE_LOOPBACK && netw->m_nIfType==pZona->m_nIfType)
		{
			level=10;
			index=x;
			break;
		}
		if(pZona->QState(fTouch) || netw->m_nIfType!=pZona->m_nIfType || 
			(pZona->m_nFlags & IP_ADAPTER_DHCP_ENABLED)!=(netw->m_nFlags & IP_ADAPTER_DHCP_ENABLED))
			continue;
		if(!pZona->Connected() && (pZona->QState(fTemporary) || pZona->QState(fDeleted)))
			continue;
		BOOL this_zone=FALSE;
		if(pZona->Connected() && pZona->m_sAdapterWinName==netw->m_sAdapterWinName)
			this_zone=TRUE;
		if(netw->m_nIfType==IF_TYPE_PPP && pZona->m_sConnectionName==netw->AdapterFrendlyName)
		{
			int new_level=5;
			if(pZona->m_sRASPeer==netw->m_sRASPeer)
			{
				index=x;
				level=10;
				break;
			}
			if(new_level>level)
			{
				index=x;
				level=new_level;
			}
			continue;
		}
		if(netw->m_nIfType==IF_TYPE_IEEE80211 && pZona->m_sConnectionName==netw->SSID &&
			pZona->m_nWiFiType==netw->m_nWiFiType)
		{
			int new_level=1;
			if(pZona->m_nWiFiAuthMode==netw->m_nWiFiAuthMode &&
				pZona->m_nWiFiSecurity==netw->m_nWiFiSecurity)
				new_level+=4;
			if(new_level>level)
			{
				index=x;
				level=new_level;
				if(level>4)
					break;
			}
			continue;
		}
		if(netw->m_nIfType!=IF_TYPE_PPP && netw->m_nIfType!=IF_TYPE_IEEE80211)
		{
			int new_level=0;
			cIpMask IP;
			if(netw->m_nIP4Net>0)
			{
				IP.SetV4(netw->m_nIP4Net);
				IP.SetV4MaskByIp(netw->m_nIP4NetMask);
			}
			if(!(pZona->m_nFlags & IP_ADAPTER_DHCP_ENABLED))
			{
				if(pZona->m_sAdapterWinName==netw->m_sAdapterWinName && IP.CmpNet(&pZona->m_IP))
				{
					level=10;
					index=x;
					break;
				}
				continue;
			}
			if(netw->m_nGwMacLen==0)
			{
				if(this_zone && pZona->m_nGwMacLen==0)
					new_level+=1;
			}
			else
			{
				if(pZona->m_nGwMacLen==netw->m_nGwMacLen && pZona->m_nGwMac==netw->m_nGwMac)
					new_level+=5;
				else
				{
					if(pZona->m_nGwMacLen!=0)
						new_level-=6;
					if(this_zone)
						new_level+=5;
				}
			}

			if(netw->m_nDhcpMacLen==0)
			{
				if(this_zone && pZona->m_nDhcpMacLen==0)
					new_level+=1;			
			}
			else
			{
				if(pZona->m_nDhcpMacLen==netw->m_nDhcpMacLen && pZona->m_nDhcpMac==netw->m_nDhcpMac)
					new_level+=5;
				else
				{
					if(pZona->m_nDhcpMacLen!=0)
						new_level-=6;
					if(this_zone)
						new_level+=5;
				}
			}
			if(pZona->m_IP.IsV4() && IP.CmpNet(&pZona->m_IP))
				new_level+=1;
			if(pZona->m_sDnsSuffix==netw->DnsSuffix && this_zone)
				new_level+=5;
			if(pZona->m_sDnsSuffix==netw->DnsSuffix)
				new_level+=10;
			else
				new_level-=10;
			if(new_level>level)
			{
				index=x;
				level=new_level;
				if(level>10)
					break;
			}
			continue;
		}
	}
	if(level>0)
		pZona =&m_zones[index];
	if(netw->m_nIfType==IF_TYPE_IEEE80211 && level<5)
	{
		if(netw->m_nWiFiAuthMode==Ndis802_11AuthModeOpen && netw->m_nWiFiSecurity==Ndis802_11EncryptionDisabled)
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Warning !!! Security mode changed. "));
		}	
	}
	if(level<5)
	{
		pZona=NewZoneByNet(netw);
		if(!pZona)
			return 0;
	}
	else
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: level %d %S.\n",level,pZona->m_sFrendlyName));
		if(netw->HasIPv4)
		{
			if(netw->m_nIP4Net>0)
			{
				pZona->m_IP.SetV4(netw->m_nIP4Net);
				pZona->m_IP.SetV4MaskByIp(netw->m_nIP4NetMask);
			}

			if(pZona->m_nDhcpIPv4==0)
				pZona->m_nDhcpIPv4=netw->m_nDhcpIPv4;
			if(pZona->m_nDhcpMacLen==0)
			{
				pZona->m_nDhcpMac=netw->m_nDhcpMac;
				pZona->m_nDhcpMacLen=netw->m_nDhcpMacLen;
			}

			if(pZona->m_nGwIPv4==0)
				pZona->m_nGwIPv4=netw->m_nGwIPv4;
			if(pZona->m_nGwMacLen==0)
			{
				pZona->m_nGwMac=netw->m_nGwMac;
				pZona->m_nGwMacLen=netw->m_nGwMacLen;
			}
		}
		if(!pZona->Connected())
		{
			pZona->SetConnected(TRUE);
			ConnectZone(pZona,netw);
		}
	}
	_time32((__time32_t*)&pZona->m_nTimeConnected);
	pZona->SetState(fTouch,TRUE);
	netw->zone_ID=pZona->m_nID;
	netw->ARPWatch=pZona->m_nSettings & cNetZone::fARPWatch;
	cNetAdapter * pAdapter=FindAdapterByID(netw->adapter_ID);
	if(netw->adapter_ID>0 && pAdapter)
	{
		pAdapter->m_nZoneID=pZona->m_nID;
		if(pAdapter->QState(fGateway))
		{
			m_zones[0].m_sFrendlyName=m_zones[0].m_sConnectionName;
			m_zones[0].m_sFrendlyName+=" thru ";
			m_zones[0].m_sFrendlyName+=pZona->m_sFrendlyName;
		}
	}
	return 1;
}
void  CNetWatcherTask::ConnectZone(cNetZone * pZona,NETWATCH::Network  * netw)
{
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Welcome again to %S.\n",pZona->m_sFrendlyName));
	if(pZona->m_nSettings & cNetZone::fDefPrinter)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Set default printer to %S.\n",pZona->m_sDefPrinter.data()));
		DWORD retval=SetDefaultPrinterW(pZona->m_sDefPrinter.data());
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: returned %d.\n",retval));
	}
	if(pZona->m_nSettings & cNetZone::fWallPaper)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Set wallpaper to %S.\n",pZona->m_sWallPaper.data()));
		DWORD retval=SystemParametersInfoW(SPI_SETDESKWALLPAPER,0,(PVOID)pZona->m_sWallPaper.data(),0);
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: returned %d.\n",retval));	
	}
	if (pZona->m_nSettings & cNetZone::fConnectedProgram)
	{	
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		memset( &si,0, sizeof(si) );
		si.cb = sizeof(si);
		memset( &pi,0, sizeof(pi) ); 
	//	WCHAR str1[100];
	//	wcsncpy(str1,pZona->m_sConnectedProgram.data(),sizeof(str1)/sizeof(WCHAR));
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: run program %S.\n",pZona->m_sConnectedProgram.data()));
		DWORD retval=CreateProcessW( NULL, (LPWSTR)pZona->m_sConnectedProgram.data(), NULL, NULL, TRUE,
			0, NULL, NULL, &si, &pi);
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: returned %d.\n",retval));
		if(!retval)
			return ;
	//	return;
		// Close process and thread handles.
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

	}
}
cNetZone * CNetWatcherTask::NewZoneByNet(NETWATCH::Network * netw)
{
	//if (netw->m_nFlags & IP_ADAPTER_DHCP_ENABLED && netw->DnsSuffix[0]==0)
	//	return 1;
	cNetAdapter *pAdapter=FindAdapterByID(netw->adapter_ID);
	if(!pAdapter)
	{
		PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask::NewZoneByNet Error: cant find adapter with ID=%d",netw->adapter_ID));
		return 0;
	}

	Lock_settings();
	DWORD max_ID=m_settings.m_NextZoneID;
	while(TRUE)
	{
		DWORD x=0;
		while(x<m_adapters.size() || x<m_zones.size())
		{
			if(x<m_adapters.size() && m_adapters[x].m_nZoneID>=max_ID)
				break;
			if(x<m_zones.size() && m_zones[x].m_nID>=max_ID)
				break;
			x++;
		}
		if(x>=m_adapters.size() && x>=m_zones.size())
			break;
		max_ID++;
	}
	m_settings.m_NextZoneID=max_ID+1;
	Unlock_settings();

	cNetZone zone;
	cNetZone *pZona=&zone;
	if(netw->m_nIfType!=IF_TYPE_SOFTWARE_LOOPBACK)
	{
		if(pAdapter->m_nSettings & cNetAdapter::fAskWhenNewZones)
		{
			pZona->m_nSettings|=cNetZone::fNeedToAsk;
		}
		else
		{
			if(FindZoneByID(pAdapter->m_nUndefZoneID))
			{
				zone=*FindZoneByID(pAdapter->m_nUndefZoneID);
				pZona->SetState(fTemporary,TRUE);
			}
			else
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Cant find default zone for adapter %S.",pAdapter->m_sAdapterFrendlyName.data()));
				pZona->m_nSettings|=cNetZone::fNeedToAsk;
			}	
		}
	}
	pZona->m_nID=max_ID;
	pZona->SetState(fVisible,TRUE); 
	pZona->SetConnected(TRUE);
	pZona->m_sAdapterWinName=netw->m_sAdapterWinName;
	pZona->m_nIfType=netw->m_nIfType;
	pZona->m_nAdapterID=netw->adapter_ID;
	_time32((__time32_t*)&pZona->m_nTimeCreated);
	pZona->m_sDnsSuffix=netw->DnsSuffix;
	pZona->m_nFlags=netw->m_nFlags;
	pZona->m_nTypeOfInterface=pAdapter->m_nTypeOfInterface;
	if(netw->HasIPv4)
	{
		if(netw->m_nIP4Net>0)
		{
			pZona->m_IP.SetV4(netw->m_nIP4Net);
			pZona->m_IP.SetV4MaskByIp(netw->m_nIP4NetMask);
		}
		pZona->m_nDhcpIPv4=netw->m_nDhcpIPv4;
		pZona->m_nDhcpMac=netw->m_nDhcpMac;
		pZona->m_nDhcpMacLen=netw->m_nDhcpMacLen;
		pZona->m_nGwIPv4=netw->m_nGwIPv4;
		pZona->m_nGwMac=netw->m_nGwMac;
		pZona->m_nGwMacLen=netw->m_nGwMacLen;
	}
	switch(netw->m_nIfType)
	{
		case MIB_IF_TYPE_PPP:
			pZona->m_sConnectionName=netw->AdapterFrendlyName;
			pZona->m_sFrendlyName=netw->AdapterFrendlyName;
			pZona->m_nSettings-=pZona->m_nSettings & cNetZone::fUptoGateway;
			pZona->m_nSeverity = cNetZone::znUntrusted;
			pZona->m_sRASPeer=netw->m_sRASPeer;
			break;
		case IF_TYPE_IEEE80211:
			pZona->m_sConnectionName=netw->SSID;
			pZona->m_sFrendlyName=netw->SSID;
			if(pZona->m_sDnsSuffix.size()>1)
			{
				if(pZona->m_sConnectionName.size()>0)
					pZona->m_sFrendlyName+="(";
				pZona->m_sFrendlyName+=pZona->m_sDnsSuffix;
				if(pZona->m_sConnectionName.size()>0)
					pZona->m_sFrendlyName+=")";
			}
			pZona->m_nWiFiSecurity=netw->m_nWiFiSecurity;
			pZona->m_nWiFiAuthMode=netw->m_nWiFiAuthMode;
			pZona->m_nWiFiType=netw->m_nWiFiType;
			if(!IsLocalArea(&pZona->m_IP) || pZona->m_nWiFiAuthMode==Ndis802_11AuthModeOpen || (
				pZona->m_nWiFiSecurity==Ndis802_11EncryptionNotSupported ||
				pZona->m_nWiFiSecurity==Ndis802_11EncryptionDisabled ||
				pZona->m_nWiFiSecurity==Ndis802_11Encryption1KeyAbsent
				))
				pZona->m_nSeverity = cNetZone::znUntrusted;
			else
				pZona->m_nSeverity=cNetZone::znNetBIOS;

			break;
		case IF_TYPE_SOFTWARE_LOOPBACK: 
			pZona->m_nTypeOfInterface=If_LoopBack;
			pZona->m_sFrendlyName="localhost";
			//pZona->SetState(fVisible,FALSE);
			pZona->m_nSeverity=cNetZone::znTrusted;
			pZona->m_nSettings-=pZona->m_nSettings & cNetZone::fNeedToAsk;
			break;

		default:  //ethernet
			if(IsLocalArea(&pZona->m_IP))
				pZona->m_nSeverity=cNetZone::znNetBIOS;		
			else
				pZona->m_nSeverity=cNetZone::znUntrusted;
			pZona->m_nMac=netw->m_nMac;
			pZona->m_nMacLen=netw->m_nMacLen;
			pZona->m_sConnectionName=netw->AdapterDesc;
			pZona->m_sFrendlyName=netw->AdapterFrendlyName;

			if(netw->m_nFlags & IP_ADAPTER_DHCP_ENABLED && pZona->m_sDnsSuffix.size()>0)
			{
				pZona->m_sFrendlyName=pZona->m_sDnsSuffix;
				pZona->m_sFrendlyName+="(";
				pZona->m_sFrendlyName+=netw->AdapterFrendlyName;
				pZona->m_sFrendlyName+=")";
			}
			else
			{
				if(netw->m_nIP4Net>0)
				{
					cIpMask ip; 
					ip.SetV4(netw->m_nIP4Net);
					ip.SetV4MaskByIp(netw->m_nIP4NetMask);
					char str1[100];
					ip.ToStr(str1,100);
					pZona->m_sFrendlyName=str1;
					pZona->m_sFrendlyName+="(";
					pZona->m_sFrendlyName+=netw->AdapterFrendlyName;
					pZona->m_sFrendlyName+=")";
				}
			}
			pZona->m_nSettings|=cNetZone::fARPWatch;
	}
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Add new zone %S\n",zone.m_sFrendlyName));
	if(pZona->QState(fTemporary))
		pZona->m_sFrendlyName+="(temporary)";
	pZona=&m_zones.push_back(zone);
	return pZona;
}
void CNetWatcherTask::DisconnectAll()
{
	cNetZone *pZona;
	for(DWORD x=0;x<m_zones.size();x++)
	{
		pZona=&m_zones[x];
		pZona->SetConnected(FALSE);
		pZona->SetState(fTouch,FALSE);

	}
	cNetAdapter *pAdapter;
	for(DWORD x=0;x<m_adapters.size();x++)
	{
		pAdapter=&m_adapters[x];
		pAdapter->SetConnected(FALSE);
		pAdapter->SetState(fTouch,FALSE);
	}
}
void CNetWatcherTask::UnTouchZones()
{
	cNetZone *pZona;
	for(DWORD x=0;x<m_zones.size();x++)
	{
		pZona=&m_zones[x];
		if(pZona->Connected() && !pZona->QState(fTouch))
		{
			pZona->SetConnected(FALSE);
			DisconnectZone(pZona);
			if(pZona->QState(fTemporary))
				m_zones.remove(x);
		}
		else
			pZona->SetState(fTouch,FALSE);

		if(pZona->Connected() && pZona->m_nSettings & cNetZone::fNeedToAsk )
		{
			cNetAdapter *pAdapter=FindAdapterByID(pZona->m_nAdapterID);
			cAskActionNewZone data;
			if(pZona->m_nSeverity==cNetZone::znUntrusted)
				data.m_nDetectDanger = DETDANGER_HIGH;
			else
				data.m_nDetectDanger = DETDANGER_INFORMATIONAL;
			data.m_nApplyToAll = APPLYTOALL_NONE;
			data.m_NewZone=*pZona;
			data.m_NetAdapter=*pAdapter;
			tERROR err = m_task->sysSendMsg(pmcASK_ACTION, cAskActionNewZone::eIID, NULL, &data, SER_SENDMSG_PSIZE);
			if(err==errOK_DECIDED)
			{
				*pZona=data.m_NewZone;
				pZona->m_nSettings-= pZona->m_nSettings & cNetZone::fNeedToAsk;
			}
			else
				m_NewList->Flags=1;
		}
	}
}
void CNetWatcherTask::DisconnectZone(cNetZone * pZona)
{
	if(pZona->m_nSettings & cNetZone::fSavDefPrinter)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Get default printer\n"));
		const DWORD size_str1=200;
		WCHAR str1[size_str1];

		if(GetDefaultPrinterW(str1,(LPDWORD) &size_str1))
		{
			pZona->m_nSettings|=cNetZone::fDefPrinter;
			pZona->m_sDefPrinter=str1;
		}

	}
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: leave zone %S\n",pZona->m_sFrendlyName));

}
cNetZone * CNetWatcherTask::FindZoneByID(DWORD num )
{
	for(DWORD x=0;x<m_zones.size();x++)
	{
		if(m_zones[x].m_nID==num)
			return &m_zones[x];
	}
	PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask::FindZoneByID Error: ID %d not found!",num));
	return 0;
}

cNetAdapter * CNetWatcherTask::FindAdapterByID(DWORD num )
{
	for(DWORD x=0;x<m_adapters.size();x++)
	{
		if(m_adapters[x].m_nID==num)
			return &m_adapters[x];
	}
	PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask::FindAdapterByID Error: ID %d not found!",num));
	return 0;
}
cNetAdapter *CNetWatcherTask::FindAdapterByNet(NETWATCH::Network *netw)
{
	
	if(netw->m_nIfType==IF_TYPE_SOFTWARE_LOOPBACK)
		return 0;
	cNetAdapter *pAdapter;
	DWORD x;
	for(x=1;x<m_adapters.size();x++)
	{
		pAdapter=&m_adapters[x];
		if(netw->m_nIfType==IF_TYPE_SOFTWARE_LOOPBACK && pAdapter->m_nIfType==netw->m_nIfType)
			break;
		if(netw->m_nIfType!=IF_TYPE_PPP && pAdapter->m_sAdapterWinName==netw->m_sAdapterWinName)
			break;
		if(netw->m_nIfType==IF_TYPE_PPP && pAdapter->m_nIfType==netw->m_nIfType && pAdapter->QState(fTouch)==FALSE 
			&& pAdapter->m_sRasDeviceType==netw->m_sRasDeviceType && pAdapter->m_sRasDeviceName==netw->m_sRasDeviceName)
			break;
	}
	if(x==m_adapters.size())
	{
		//new adapter
		pAdapter=&m_adapters.push_back();
		DWORD max_ID=m_settings.m_NextAdapterID;
		Lock_settings();
		while(TRUE)
		{
			DWORD x=0;
			while(x<m_adapters.size() || x<m_zones.size())
			{
				if(x<m_adapters.size() && m_adapters[x].m_nID>=max_ID)
					break;
				if(x<m_zones.size() && m_zones[x].m_nAdapterID>=max_ID)
					break;
				x++;
			}
			if(x>=m_adapters.size() && x>=m_zones.size())
				break;
			max_ID++;
		}
		m_settings.m_NextAdapterID=max_ID+1;
		Unlock_settings();
		pAdapter->m_nID=max_ID;
		pAdapter->m_nIfType=netw->m_nIfType;
		pAdapter->SetState(fVisible,TRUE);
		switch(pAdapter->m_nIfType)
		{
			case IF_TYPE_IEEE80211:
				query_adapter_description(netw);
				pAdapter->m_sAdapterFrendlyName=netw->AdapterDesc;
				pAdapter->m_nTypeOfInterface=If_WiFi;
				break;
			case IF_TYPE_PPP:
				{
					cStringObj RasDeviceType=netw->m_sRasDeviceType;
					cStringObj RasDeviceName=netw->m_sRasDeviceName;

					pAdapter->m_nTypeOfInterface=If_PPP;
					pAdapter->m_sAdapterFrendlyName=netw->AdapterDesc;
					if(RasDeviceType==RASDT_Modem)
					{
						pAdapter->m_nTypeOfInterface=If_Modem;
						if(RasDeviceName.size()>0)
							pAdapter->m_sAdapterFrendlyName=RasDeviceName;
					}
					if(RasDeviceType==RASDT_PPPoE)
						pAdapter->m_nTypeOfInterface=If_PPPoE;
					if(RasDeviceType==RASDT_Vpn)
						pAdapter->m_nTypeOfInterface=If_VPN;


					pAdapter->m_sRasDeviceType=netw->m_sRasDeviceType;
					pAdapter->m_sRasDeviceName=netw->m_sRasDeviceName;
				}
				break;
			case IF_TYPE_SOFTWARE_LOOPBACK:
				pAdapter->m_sAdapterFrendlyName="LOOPBACK Adapter";
				pAdapter->m_nTypeOfInterface=If_LoopBack;
				pAdapter->SetConnected(TRUE);
				break;
			default:
				query_adapter_description(netw);
				pAdapter->m_sAdapterFrendlyName=netw->AdapterDesc;
				pAdapter->m_nTypeOfInterface=If_Ethernet;
				pAdapter->m_sAdapterFrendlyName=netw->AdapterDesc;
		}
		_time32((__time32_t*)&pAdapter->m_nTimeCreated);
		pAdapter->SetState(fChanged,TRUE);
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask::FindAdapterByNet Add new adapter %S",pAdapter->m_sAdapterFrendlyName));
	}
	netw->adapter_ID=pAdapter->m_nID;
	tDWORD time_now;
	_time32((__time32_t*)&time_now);
	pAdapter->m_nMac=netw->m_nMac;
	pAdapter->m_nMacLen=netw->m_nMacLen;
	pAdapter->m_nFlags=netw->m_nFlags;
	pAdapter->m_nIfIndex=netw->m_nIfIndex;
	pAdapter->m_sAdapterWinName=netw->m_sAdapterWinName;
	pAdapter->SetState(fTouch,TRUE);

	if(!pAdapter->Connected() && netw->OperStatus==IfOperStatusUp)
		pAdapter->m_nTimeConnected=time_now;

	pAdapter->SetConnected(netw->OperStatus==IfOperStatusUp);
	if(pAdapter->Connected() && time_now-pAdapter->m_nTimeConnected<3 && netw->inRouteTable<5 && netw->m_nFlags & IP_ADAPTER_DHCP_ENABLED)
	{
		m_NewList->Flags=1;
		netw->OperStatus=IfOperStatusDown;
		if(netw->inRouteTable>1)
			m_level=5;
	}
	if(netw->m_nIfIndex==m_NewList->IPv4gw.dwIndex && netw->OperStatus==IfOperStatusUp)
	{

		m_zones[0].SetConnected(TRUE);
		m_zones[0].SetState(fTouch,TRUE);
		m_adapters[0].SetConnected(TRUE);
		pAdapter->SetState(fGateway,TRUE);
	}
	else
		pAdapter->SetState(fGateway,FALSE);
	pAdapter->m_nZoneID=0;
	return pAdapter;
}
void CNetWatcherTask::UnTouchAdapters()
{
	cNetAdapter *pAdapter;
	for(DWORD x=1;x<m_adapters.size();x++)
	{
		pAdapter=&m_adapters[x];
		if(!pAdapter->QState(fTouch))
		{
			pAdapter->SetState(fGateway,FALSE);
			pAdapter->SetConnected(FALSE);
		}
		pAdapter->SetState(fTouch,FALSE);
	}
}