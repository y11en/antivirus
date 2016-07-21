#include "stdafx.h"
#include "PacketClient.h"
#include "ClientIDs.h"
#include "../../windows/Hook/Plugins/Include/fw_ev_api.h"

CPacketClient::CPacketClient () :
	m_fnPacketNotifyCallback (NULL),
	m_lpCallbackParam (NULL)	
{
}

CPacketClient::~CPacketClient ()
{
	StopClient ();
}

bool CPacketClient::Create (HANDLE hShutdownEvent)
{
	if (!CGuardClient::Create (hShutdownEvent,
								CLIENT_APP_ID_FW_PACKET,
								AVPG_STANDARTRIORITY,
								_CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE /*|
								_CLIENT_FLAG_SAVE_FILTERS*/))
	{
		return false;
	}
	
	return true;	
}

bool CPacketClient::AddBlockFilters (bool bIsBlock)
{
	DWORD dwFlags = bIsBlock ? (FLT_A_DEFAULT | FLT_A_DENY) : (FLT_A_DEFAULT | FLT_A_PASS);
    dwFlags |= FLT_A_DISPATCH_MIRROR;

	CGuardFilter flt (FLTTYPE_KLICK, FW_NDIS_PACKET_OUT, 0, dwFlags);
	CGuardFilter flt2 (FLTTYPE_KLICK, FW_NDIS_PACKET_IN, 0, dwFlags);
	
	DWORD dwID = 0;
	
	flt.AddParam (PF_BLOCK_FILTER, FLT_PARAM_NOP, 4, &dwID, _FILTER_PARAM_FLAG_NONE);
	flt2.AddParam (PF_BLOCK_FILTER, FLT_PARAM_NOP, 4, &dwID, _FILTER_PARAM_FLAG_NONE);
	
	return AddFilter2Top (flt) && AddFilter2Top (flt2);
}

void CPacketClient::SignalStop ()
{
	if (m_DriverContext)
		DRV_ChangeActiveStatus( m_DriverContext, FALSE );
}

void CPacketClient::StopClient ()
{
	CGuardClient::StopClient (false);
}

bool CPacketClient::Enable (bool bEnable)
{
	if (bEnable)
		return Create (g_hStopEvent);
	else
		StopClient ();
	
	return true;
}

VERDICT CPacketClient::OnEvent( PVOID pMessage )
{
	ULONG HookId, Mj, Mi, FilterId, EventFlags, ProcessId;

	DRV_GetEventInfo( pMessage, &HookId, &Mj, &Mi, &FilterId, &EventFlags, &ProcessId );
	log.WriteFormat (_T("[CKAHFW_PacketClient::OnEvent] Processing event ")
						_T("HookId = %d, Mj = %d, Mi = %d, FltId = 0x%08x, ")
						_T("EvtFl = 0x%08x, ProcId = %d, ")
						, PEL_INFO, HookId, Mj, Mi, FilterId, EventFlags, ProcessId);

	if ((EventFlags & _EVENT_FLAG_ALERT1) == 0 && 
			(EventFlags & _EVENT_FLAG_LOGTHIS) == 0)
	{
		if (_EVENT_FLAG_AUTO_VERDICT_DISCARD & EventFlags)
			return Verdict_Discard;

		return Verdict_Pass;
	}

	PSINGLE_PARAM pRuleNameParam = DRV_GetEventParam( pMessage, 0, PACKET_FILTER_NAME, 0 );
	LPCWSTR RuleName = NULL;
	if (pRuleNameParam && pRuleNameParam->ParamSize > 0)
		RuleName = (LPCWSTR)pRuleNameParam->ParamValue;

	UCHAR Proto = 0;
	PSINGLE_PARAM pProtoParam = DRV_GetEventParam( pMessage, 0, FW_ID_PROTOCOL, sizeof (UCHAR));
	if (pProtoParam)
		Proto = *(UCHAR *)pProtoParam->ParamValue;

    ULONG Stream = FW_STREAM_UNDEFINED;
	PSINGLE_PARAM pStreamParam = DRV_GetEventParam( pMessage, 0, FW_ID_STREAM_DIRECTION, sizeof (ULONG));
	if (pStreamParam)
		Stream = *(ULONG *)pStreamParam->ParamValue;

    CKAHUM::IP RemoteAddress (0);
	PSINGLE_PARAM pRemoteIPv4AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IP, sizeof (ULONG));
	if (pRemoteIPv4AddressParam)
		RemoteAddress.Setv4 ( *(ULONG *)pRemoteIPv4AddressParam->ParamValue );

	PSINGLE_PARAM pRemoteIPv6AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IPV6, 0);
    PSINGLE_PARAM pRemoteIPv6AddrZoneParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_IPV6_ZONE, 0);
    if (pRemoteIPv6AddressParam && pRemoteIPv6AddressParam->ParamSize == sizeof (CKAHUM::OWord) &&
        pRemoteIPv6AddrZoneParam && pRemoteIPv6AddrZoneParam->ParamSize == sizeof (DWORD))
		RemoteAddress.Setv6 ( *(CKAHUM::OWord *)pRemoteIPv6AddressParam->ParamValue, 
                              *(DWORD *)pRemoteIPv6AddrZoneParam->ParamValue );

	CKAHUM::IP LocalAddress (0);
	PSINGLE_PARAM pLocalIPv4AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IP, sizeof (ULONG));
	if (pLocalIPv4AddressParam)
		LocalAddress.Setv4 ( *(ULONG *)pLocalIPv4AddressParam->ParamValue );

	PSINGLE_PARAM pLocalIPv6AddressParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IPV6, 0);
    PSINGLE_PARAM pLocalIPv6AddrZoneParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_IPV6_ZONE, 0);
	if (pLocalIPv6AddressParam && pLocalIPv6AddressParam->ParamSize == sizeof (CKAHUM::OWord) &&
        pLocalIPv6AddrZoneParam && pLocalIPv6AddrZoneParam->ParamSize == sizeof (DWORD))
		LocalAddress.Setv6 ( *(CKAHUM::OWord *)pLocalIPv6AddressParam->ParamValue, 
                             *(DWORD *)pLocalIPv6AddrZoneParam->ParamValue );

	USHORT TCPUDPRemotePort = 0, TCPUDPLocalPort = 0;
	USHORT ICMPType = 0, ICMPCode = 0;
	if (Proto == CKAHFW::PROTO_TCP || Proto == CKAHFW::PROTO_UDP)
	{
		PSINGLE_PARAM pTCPUDPRemotePortParam = DRV_GetEventParam( pMessage, 0, FW_ID_REMOTE_PORT, sizeof (USHORT));
		if (pTCPUDPRemotePortParam)
			TCPUDPRemotePort = *(USHORT *)pTCPUDPRemotePortParam->ParamValue;

		PSINGLE_PARAM pTCPUDPLocalPortParam = DRV_GetEventParam( pMessage, 0, FW_ID_LOCAL_PORT, sizeof (USHORT));
		if (pTCPUDPLocalPortParam)
			TCPUDPLocalPort = *(USHORT *)pTCPUDPLocalPortParam->ParamValue;
	}
	else if (Proto == CKAHFW::PROTO_ICMP || Proto == CKAHFW::PROTO_ICMPV6)
	{
		PSINGLE_PARAM pICMPTypeParam = DRV_GetEventParam( pMessage, 0, FW_ID_ICMP_TYPE, sizeof (UCHAR));
		if (pICMPTypeParam)
			ICMPType = *(USHORT *)pICMPTypeParam->ParamValue;
		PSINGLE_PARAM pICMPCodeParam = DRV_GetEventParam( pMessage, 0, FW_ID_ICMP_CODE, sizeof (UCHAR));
		if (pICMPCodeParam)
			ICMPCode = *(USHORT *)pICMPCodeParam->ParamValue;
	}

	__int64 Time = 0;
	PSINGLE_PARAM pTimeParam = DRV_GetEventParam( pMessage, 0, FW_ID_EVENT_TIME, sizeof (__int64));
	if (pTimeParam)
		Time = *(__int64 *)pTimeParam->ParamValue;

	PSINGLE_PARAM pUserDataParam = DRV_GetEventParam( pMessage, 0, PF_FILTER_USER_DATA, 0 );
	std::vector<UCHAR> UserData;
	if (pUserDataParam)
		UserData.assign (pUserDataParam->ParamValue, pUserDataParam->ParamValue + pUserDataParam->ParamSize);

	CKAHFW::PacketRuleNotify notify;
	memset (&notify, 0, sizeof (notify));

	notify.RuleName = RuleName;
	notify.IsBlocking = (EventFlags & _EVENT_FLAG_AUTO_VERDICT_DISCARD) != 0;
	notify.IsIncoming = Mj == FW_NDIS_PACKET_IN;
    notify.IsStream = Stream != FW_STREAM_UNDEFINED;
    notify.IsStreamIncoming = Stream == FW_STREAM_IN;
	notify.Proto = Proto;
	notify.RemoteAddress = RemoteAddress;
	notify.LocalAddress = LocalAddress;
	notify.TCPUDPRemotePort = TCPUDPRemotePort;
	notify.TCPUDPLocalPort = TCPUDPLocalPort;
	notify.ICMPType = (UCHAR)ICMPType;
	notify.ICMPCode = (UCHAR)ICMPCode;
	notify.Notify = (EventFlags & _EVENT_FLAG_ALERT1) != 0;
	notify.Log = (EventFlags & _EVENT_FLAG_LOGTHIS) != 0;
	notify.Time = Time;
	notify.pUserData = UserData.size () ? &UserData[0] : NULL;
	notify.UserDataSize = UserData.size ();

	if (m_fnPacketNotifyCallback)
	{
		log.WriteFormat (_T("[CKAHFW_PacketClient::OnEvent] Calling user packet rule notify callback..."), PEL_INFO);

		try
		{
			m_fnPacketNotifyCallback (m_lpCallbackParam, &notify);
		}
		catch (...)
		{
			log.WriteFormat (_T("[CKAHFW_PacketClient::OnEvent] User packet rule notify callback failed"), PEL_ERROR);
		}

		log.WriteFormat (_T("[CKAHFW_PacketClient::OnEvent] User packet rule notify callback returns"), PEL_INFO);
	}

	if (_EVENT_FLAG_AUTO_VERDICT_DISCARD & EventFlags)
		return Verdict_Discard;

	return Verdict_Pass;
}
