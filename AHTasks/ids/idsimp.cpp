#include "idsimp.h"
#include <CKAH/ipconv.h>

void CALLBACK idsTrace(CKAHUM::LogLevel Level, LPCSTR szString)
{
	switch (Level)
	{
	case CKAHUM::lError:
		PR_TRACE((g_root, prtDANGER, "IDS\t%s", szString));
		break;
	case CKAHUM::lWarning:
		PR_TRACE((g_root, prtERROR, "IDS\t%s", szString));
		break;
	default:
		PR_TRACE((g_root, prtIMPORTANT, "IDS\t%s", szString));
	}
}


void CALLBACK cbAttacksNotify(LPVOID lpContext, const CKAHIDS::AttackNotify *pAttackNotify)
{
	if (!pAttackNotify || !lpContext)
	{
		PR_TRACE((0, prtERROR, "IDSTASK\tattack notify with bad data (lpContext or pAttackNotify is null)"));
		return;
	}

	_idstask* pTask = (_idstask*) lpContext;
	PR_TRACE((pTask->m_hThis, prtIMPORTANT, "IDSTASK\tattack detected id 0x%x", pAttackNotify->AttackID));

	pTask->m_Statistics.m_nNumAttacksDetected++;
	pTask->m_Statistics.m_timeLastObject = time(NULL);
	
	cIDSEventReport Report;
	
	Report.m_Time = time(NULL);								//pAttackNotify->Time;		

	Report.m_nSessionId = -1;
	Report.m_strTaskType.assign(L"ids", cCP_UNICODE);

	Report.m_AttackDescription.assign(pAttackNotify->AttackDescription, cCP_UNICODE);
	Report.m_ObjectType = OBJTYPE_NETWORK;
#if VER_PRODUCTVERSION_HIGH <= 7
	Report.m_ObjectStatus = OBJSTATUS_SUSPICION;
#else // VER_PRODUCTVERSION_HIGH
	Report.m_ObjectStatus = OBJSTATUS_DENIED;
#endif // VER_PRODUCTVERSION_HIGH
	Report.m_DetectType = DETYPE_ATTACK;

	Report.m_Proto = pAttackNotify->Proto;
	Report.m_AttackerIP = CKAHUMIP_cIP(pAttackNotify->AttackerIP);
	Report.m_LocalPort = pAttackNotify->LocalPort;
	Report.m_ResponseFlags = pAttackNotify->ResponseFlags;

	pTask->m_hThis->sysSendMsg(pmc_EVENTS_IMPORTANT, Report.m_ObjectStatus, NULL, &Report, SER_SENDMSG_PSIZE);
	
    if ( pTask->m_Settings.m_bEnableBan )
    {
        int connections = 0;
        CKAHFW::HCONNLIST hconnections;
        if (CKAHUM::srOK == CKAHFW::GetConnectionList(&hconnections))
        {
            if ( CKAHUM::srOK == CKAHFW::ConnectionList_GetSize(hconnections, &connections) )
            {                
                CKAHFW::Connection connection;

                for ( int cou = 0; cou < connections; cou++ )
                {
                    if ( CKAHUM::srOK == CKAHFW::ConnectionList_GetItem(hconnections, cou, &connection) )
                    {
                        if ( connection.RemoteIP == pAttackNotify->AttackerIP )
                        {
                            CKAHFW::BreakConnection(connection.Proto, 
                                &connection.LocalIP, connection.LocalPort,
                                &connection.RemoteIP, connection.RemotePort);
                        }
                    }
                }

            }

            CKAHFW::ConnectionList_Delete(hconnections);
        }
    }
}
