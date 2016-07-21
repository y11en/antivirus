////////////////////////////////////////////////////////////////////////////////
// AVP Communication Protocols general defines and functions
// Implementation file : CmdRoutine.cpp

#include "CmdRoutine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AVPCP_FillCommandHeader(PAVPCP_COMMAND_HEADER pHeader, DWORD dwCommandID, DWORD dwHasParameters, DWORD dwVersion, DWORD dwSenderComponentID)
{
	if (pHeader)
	{
		pHeader->m_dwPattern = CP_CH_PATTERN;
		pHeader->m_dwCommandID = dwCommandID;
		pHeader->m_dwHasParameters = dwHasParameters;
		pHeader->m_dwVersion = dwVersion;
		pHeader->m_dwSenderComponentID = dwSenderComponentID;
	}
}

void AVPCP_FillServiceCommand(PAVPCP_SERVICE_COMMAND pCommand, DWORD dwCommandID, DWORD dwStatus, DWORD dwVersion, DWORD dwSenderComponentID)
{
	if (pCommand)
	{
		pCommand->m_dwPattern = CP_SC_PATTERN;
		pCommand->m_dwCommandID = dwCommandID;
		pCommand->m_dwStatus = dwStatus;
		pCommand->m_dwVersion = dwVersion;
		pCommand->m_dwSenderComponentID = dwSenderComponentID;
	}
}

void AVPCP_FillDataHeader(PAVPCP_DATA_HEADER pHeader, DWORD dwSize)
{
	if (pHeader)
	{
		pHeader->m_dwPattern = CP_DH_PATTERN;
		pHeader->m_dwSize = dwSize;
	}
}

void AVPCP_FillCommandAckCommand(PAVPCP_SERVICE_COMMAND pCommand, DWORD dwStatus, DWORD dwVersion, DWORD dwSenderComponentID)
{
	AVPCP_FillServiceCommand(pCommand, CP_SCID_ACK_COMMAND,	dwStatus, dwVersion, dwSenderComponentID);
}

void AVPCP_FillDataAckCommand(PAVPCP_SERVICE_COMMAND pCommand, DWORD dwStatus, DWORD dwVersion, DWORD dwSenderComponentID)
{
	AVPCP_FillServiceCommand(pCommand, CP_SCID_ACK_DATA, dwStatus, dwVersion, dwSenderComponentID);
}

BOOL AVPCP_IsCommandAcknowleged(PAVPCP_SERVICE_COMMAND pCommand)
{
	if (pCommand)
	{
		if ((AVPCP_IsValidServiceCommand(pCommand)) &&
			(pCommand->m_dwCommandID == CP_SCID_ACK_COMMAND) &&
			(pCommand->m_dwStatus == CP_SCS_ACK)) return TRUE;
	}

	return FALSE;
}

BOOL AVPCP_IsDataAcknowleged(PAVPCP_SERVICE_COMMAND pCommand)
{
	if (pCommand)
	{
		if ((AVPCP_IsValidServiceCommand(pCommand)) &&
			(pCommand->m_dwCommandID == CP_SCID_ACK_DATA) &&
			(pCommand->m_dwStatus == CP_SCS_ACK)) return TRUE;
	}

	return FALSE;
}

BOOL AVPCP_IsValidCommandHeader(PAVPCP_COMMAND_HEADER pHeader, DWORD dwMaxVersion)
{
	if (pHeader)
	{
		if ((pHeader->m_dwPattern == CP_CH_PATTERN) &&
			(pHeader->m_dwVersion <= dwMaxVersion)) return TRUE;
	}

	return FALSE;
}

BOOL AVPCP_IsValidServiceCommand(PAVPCP_SERVICE_COMMAND pCommand, DWORD dwMaxVersion)
{
	if (pCommand)
	{
		if ((pCommand->m_dwPattern == CP_SC_PATTERN) &&
			(pCommand->m_dwVersion <= dwMaxVersion)) return TRUE;
	}

	return FALSE;
}

BOOL AVPCP_IsValidDataHeader(PAVPCP_DATA_HEADER pHeader)
{
	if (pHeader)
	{
		if (pHeader->m_dwPattern == CP_DH_PATTERN) return TRUE;
	}

	return FALSE;
}
