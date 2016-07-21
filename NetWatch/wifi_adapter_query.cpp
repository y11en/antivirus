
//#include <ws2tcpip.h>



#include <winsock2.h>
#include <iphlpapi.h>
//#include <stdio.h>
#include "ntddndis.h"
#include "IoctlNdisQuery.h"
#include "CNetWatcherTask.h"


void CNetWatcherTask::query_adapter_description(NETWATCH::Network * netw)
{

	CHAR buf[NETWATCH::ADAPTERDESCLEN+2];
	DWORD ReturnedCount;
	tDWORD RetVal = IoctlNdisQuery(netw->m_sAdapterWinName, OID_GEN_VENDOR_DESCRIPTION,buf, sizeof(buf)-2,&ReturnedCount);
	if(RetVal==0 && ReturnedCount>3)
	{
		buf[ReturnedCount]=0;
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,buf,
			sizeof(buf)-2,netw->AdapterDesc,NETWATCH::ADAPTERDESCLEN);
	}
}
void CNetWatcherTask::ppp_adapter_query(NETWATCH::Network * netw)
{
	if(netw->m_nIfType!=IF_TYPE_PPP)
		return;
	RASENTRYW ras;
	DWORD ras_size=sizeof(ras);
	memset(&ras,0,ras_size);
	ras.dwSize=ras_size;
	
	if(RasGetEntryPropertiesW(0,netw->AdapterFrendlyName,&ras,&ras_size,0,0)==0 && ras_size>0)
	{
		wcsncpy_s(netw->m_sRASPeer,ras.szLocalPhoneNumber,RAS_MaxPhoneNumber);
		wcsncpy_s(netw->m_sRasDeviceType,ras.szDeviceType,RAS_MaxDeviceType);
		wcsncpy_s(netw->m_sRasDeviceName,ras.szDeviceName,RAS_MaxDeviceName);
	}
} 

void CNetWatcherTask::wifi_adapter_query(NETWATCH::Network * netw)
{
	//if(netw->OperStatus==IfOperStatusDown)
		//return;
	tDWORD medium=query_dword_oid(netw->m_sAdapterWinName,OID_GEN_PHYSICAL_MEDIUM);
	PR_TRACE((g_root, prtIMPORTANT, "CNetWatcherTask::wifi_adapter_query adapter %S,medium=%d",netw->AdapterFrendlyName,medium));

	if(netw->m_nIfType!=IF_TYPE_IEEE80211 && medium!=NdisPhysicalMediumWirelessLan && medium!=NdisPhysicalMediumNative802_11)
	{
		PR_TRACE((g_root, prtIMPORTANT, "CNetWatcherTask::wifi_adapter_query is no wifi!"));
		return;
	}	
	netw->m_nIfType=IF_TYPE_IEEE80211;
	netw->m_nWiFiType=query_dword_oid(netw->m_sAdapterWinName,OID_802_11_INFRASTRUCTURE_MODE);
	netw->m_nWiFiSecurity=query_dword_oid(netw->m_sAdapterWinName,OID_802_11_ENCRYPTION_STATUS);
	netw->m_nWiFiAuthMode=query_dword_oid(netw->m_sAdapterWinName,OID_802_11_AUTHENTICATION_MODE);
	
	DWORD RetVal;
	DWORD ReturnedCount;
	DWORD buf[sizeof(_NDIS_802_11_SSID)];
	RetVal = IoctlNdisQuery(netw->m_sAdapterWinName,OID_802_11_SSID,buf, sizeof(buf),&ReturnedCount);
	if(RetVal==0 && ReturnedCount>0)
	{
		_NDIS_802_11_SSID *pssid=(_NDIS_802_11_SSID*)&buf;
		netw->SsidLength=min(pssid->SsidLength,sizeof(netw->SSID)-1);
		memcpy(&netw->SSID,pssid->Ssid,netw->SsidLength);
		netw->SSID[netw->SsidLength]=0;

	}
}