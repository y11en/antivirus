#include "stdafx.h"
#include "IDS.h"
#include "IDSClient.h"
#include "winsock.h"
#include "CKAHManifest.h"
#include "UnicodeConv.h"
#include "../../windows/Hook/Plugins/Include/attacknotify_api.h"

CKAHUM_IDS::CKAHUM_IDS () :
	m_fnAttackNotifyCallback (NULL),
	m_lpCallbackParam (NULL)
{
	m_pIDSClient = new CIDSClient (this);
}

CKAHUM_IDS::~CKAHUM_IDS ()
{
	delete m_pIDSClient;
}

bool CKAHUM_IDS::Enable (bool bEnable)
{
	if (!m_pIDSClient)
		return false;

	log.WriteFormat (_T("[CKAHUM_IDS::Enable] %s IDS..."), PEL_INFO, bEnable ? _T("Enabling") : _T("Disabling"));

	if (bEnable)
	{
		bool bRetValue = m_pIDSClient->Create (g_hStopEvent);
		if (bRetValue)
			log.WriteFormat (_T("[CKAHUM_IDS::Enable] IDS client successfully created"), PEL_INFO);
		else
			log.WriteFormat (_T("[CKAHUM_IDS::Enable] Failed to create IDS client"), PEL_ERROR);
		return bRetValue;
	}
	else
	{
		m_pIDSClient->StopClient ();
		log.WriteFormat (_T("[CKAHUM_IDS::Enable] IDS client stopped"), PEL_INFO);
	}
	
	return true;
}

VERDICT CKAHUM_IDS::OnEvent( PVOID pMessage )
{
	ULONG HookId, Mj, Mi, FilterId, EventFlags, ProcessId;

	DRV_GetEventInfo( pMessage, &HookId, &Mj, &Mi, &FilterId, &EventFlags, &ProcessId );
	log.WriteFormat (_T("[CKAHUM_IDS::OnEvent] Processing event ")
						_T("HookId = %d, Mj = %d, Mi = %d, FltId = 0x%08x, ")
						_T("EvtFl = 0x%08x, ProcId = %d, ")
						, PEL_INFO, HookId, Mj, Mi, FilterId, EventFlags, ProcessId);

	PSINGLE_PARAM pAttackIDParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_ID, sizeof (ULONG));

	if (pAttackIDParam)
	{
		ULONG AttackID = *(ULONG *)pAttackIDParam->ParamValue;


		PSINGLE_PARAM pProtoParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_PROTOCOL, sizeof (USHORT));
		USHORT Proto = 0;
		if (pProtoParam)
			Proto = *(USHORT *)pProtoParam->ParamValue;

		CKAHUM::IP AttackerIP (0);

		PSINGLE_PARAM pAttackerIPv4Param = DRV_GetEventParam( pMessage, 0, ID_ATTACK_ATTACKER_IP, sizeof (ULONG));
		if (pAttackerIPv4Param)
			AttackerIP.Setv4 ( *(ULONG *)pAttackerIPv4Param->ParamValue );

		PSINGLE_PARAM pAttackerIPv6Param = DRV_GetEventParam( pMessage, 0, ID_ATTACK_ATTACKER_IPV6, 0);
        PSINGLE_PARAM pAttackerIPv6ZoneParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_ATTACKER_IPV6_ZONE, 0);
		if (pAttackerIPv6Param && pAttackerIPv6Param->ParamSize == sizeof (CKAHUM::OWord) &&
            pAttackerIPv6ZoneParam && pAttackerIPv6ZoneParam->ParamSize == sizeof (DWORD))
			AttackerIP.Setv6 ( *(CKAHUM::OWord *)pAttackerIPv6Param->ParamValue, 
                               *(DWORD *)pAttackerIPv6ZoneParam->ParamValue );

		PSINGLE_PARAM pLocalPortParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_LOCAL_PORT, 0);
		USHORT LocalPort = 0;
		if (pLocalPortParam && pLocalPortParam->ParamSize == sizeof (USHORT))
			LocalPort = *(USHORT *)pLocalPortParam->ParamValue;

		PSINGLE_PARAM pResponseFlagsParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_RESPOND_FLAGS, 0);
		ULONG ResponseFlags = 0;
		if (pResponseFlagsParam && pResponseFlagsParam->ParamSize == sizeof (ULONG))
			ResponseFlags = *(ULONG *)pResponseFlagsParam->ParamValue;
		
		PSINGLE_PARAM pTimeParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_TIME, 0);
		__int64 Time = 0;
		if (pTimeParam && pTimeParam->ParamSize == sizeof (__int64))
			Time = *(__int64 *)pTimeParam->ParamValue;

		PSINGLE_PARAM pAttackNameParam = DRV_GetEventParam( pMessage, 0, ID_ATTACK_NAME, 0);
		std::wstring AttackName;
		if (pAttackNameParam && pAttackNameParam->ParamSize > 0)
			AttackName = __LPWSTR (std::string ((CHAR *)pAttackNameParam->ParamValue, pAttackNameParam->ParamSize));

        if (AttackerIP.Isv4())
        {
		    in_addr attacker_addr;
		    attacker_addr.s_addr = AttackerIP.v4;
    		
		    log.WriteFormat (_T("[CKAHUM_IDS::OnEvent] Attack params: AttackID = 0x%08x, Proto = 0x%08x, IP = %hs, LocalPort = %d, Flags = 0x%08x"), PEL_INFO,
							    AttackID, Proto, inet_ntoa (attacker_addr), LocalPort, ResponseFlags);
        }

		CKAHIDS::AttackNotify notify;

		notify.AttackID = AttackID;
		notify.AttackDescription = AttackName.c_str ();
		notify.Proto = (UCHAR)Proto;
		notify.AttackerIP = AttackerIP;
		notify.LocalPort = LocalPort;
		notify.ResponseFlags = ResponseFlags;
		notify.Time = Time;

		if (m_fnAttackNotifyCallback)
		{
			log.WriteFormat (_T("[CKAHUM_IDS::OnEvent] Calling user attack notify callback..."), PEL_INFO);

			try
			{
				m_fnAttackNotifyCallback (m_lpCallbackParam, &notify);
			}
			catch (...)
			{
				log.WriteFormat (_T("[CKAHUM_IDS::OnEvent] User attack notify callback failed"), PEL_ERROR);
			}

			log.WriteFormat (_T("[CKAHUM_IDS::OnEvent] User attack notify callback returns"), PEL_INFO);
		}
	}
	else
	{
		log.WriteFormat (_T("[CKAHUM_IDS::OnEvent] No attack ID"), PEL_ERROR);
	}

	return Verdict_Pass;
}
