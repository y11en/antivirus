#include <windows.h>
#include "CPIPCSendRecvPolicy.h"
#include "debugging.h"
#include "../Service/sa.h"

class CHsa
{
	HSA m_hsa;
public:
	CHsa(DWORD dwAuthority, DWORD dwAccessMask)
	{
		m_hsa = SA_Create(dwAuthority, dwAccessMask);
	}
	~CHsa()
	{
		SA_Destroy(m_hsa);
	}
	operator PSECURITY_ATTRIBUTES () { return SA_Get(m_hsa); }
};

static CHsa saEveryone(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);


#define CPIPC_COMMAND_MUTEX_PREFIX	_T("CPIPC_CMD_MTX_PREFIX_")

///////////////////////////////////////////////////////////////////////////
// CPIPC_BeginCommand - Begins command sending
// ----------------------------------------------------------------------
HANDLE CPIPC_BeginCommand(LPCTSTR pszMutexName)
{
	HANDLE hMutex = NULL;

	TCHAR szFullMutexName[0x100];
	_tcscpy(szFullMutexName, CPIPC_COMMAND_MUTEX_PREFIX);
	_tcscat(szFullMutexName, pszMutexName);
	// BUG 16596 FIX: Если одновременно посылать команду из разных потоков с разными
	//   правами, то второй поток мог обломаться в CreateMutex с Access Denied
	//   и далее пакеты из SendCommand и SendData могли перемешаться!
	hMutex = CreateMutex(saEveryone, FALSE, szFullMutexName);
	if (hMutex)	WaitForSingleObject(hMutex, INFINITE);

	return hMutex;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_EndCommand - Ends command sending
// ----------------------------------------------------------------------
void CPIPC_EndCommand(HANDLE hMutex)
{
	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_AckCommand - Sends command header acknowlege command
// ----------------------------------------------------------------------
// return :				TRUE			- Success
//						FALSE			- Otherwise
BOOL CPIPC_AckCommand(HIPCCONNECTION hConnection, BOOL bAck, DWORD dwTimeout)
{
	BOOL rc = FALSE;
	int rcl;

	if (hConnection)
	{
		AVPCP_SERVICE_COMMAND scSC;		// Acknowlege

		// Send Command Header Ack
		AVPCP_FillCommandAckCommand(&scSC, (bAck)?CP_SCS_ACK:CP_SCS_NACK);
		rcl = IPC_Send(hConnection, &scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
		rc = (CPIPC_ConvertIPCResultToCPIPCResult(rcl) == CPIPC_RC_OK);
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_SendCommand - Sends command header and checks command acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_SendCommand(HIPCCONNECTION hConnection, DWORD dwCommandID, DWORD dwHasParameters, DWORD dwVersion, DWORD dwSenderComponentID, DWORD dwTimeout)
{
	int rc = CPIPC_RC_ERROR_IO, rcl;

	if (hConnection)
	{
		AVPCP_COMMAND_HEADER chCH;		// Command
		AVPCP_FillCommandHeader(&chCH, dwCommandID, dwHasParameters, dwVersion, dwSenderComponentID);

		AVPCP_SERVICE_COMMAND scSC;		// Acknowlege

		// Send Command Header
		rcl = IPC_Send(hConnection, &chCH, AVPCP_COMMAND_HEADER_SIZE, dwTimeout);
		rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
		if (rc == CPIPC_RC_OK)
		{
			// Recv and verify Command Ack
			rcl = IPC_Recv(hConnection, &scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
			rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
			if (rc == CPIPC_RC_OK)
				rc = AVPCP_IsCommandAcknowleged(&scSC)?CPIPC_RC_OK:CPIPC_RC_NACK;
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_SendData - Sends data policy and checks data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_SendData(HIPCCONNECTION hConnection, LPPOLICY pPolicy, DWORD dwTimeout)
{
	if (pPolicy)
		return CPIPC_SendDataEx(hConnection, pPolicy, pPolicy->Size, dwTimeout);

	return CPIPC_RC_ERROR_IO;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_SendDataEx - Sends data and checks data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_SendDataEx(HIPCCONNECTION hConnection, PVOID pBuffer, DWORD dwSize, DWORD dwTimeout)
{
	int rc = CPIPC_RC_ERROR_IO, rcl;

	if (hConnection && pBuffer)
	{
		AVPCP_DATA_HEADER dhDH;			// Data header
		AVPCP_FillDataHeader(&dhDH, dwSize);

		AVPCP_SERVICE_COMMAND scSC;		// Acknowlege

		// Send Data Header
		rcl = IPC_Send(hConnection, &dhDH, AVPCP_DATA_HEADER_SIZE, dwTimeout);
		rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
		if (rc == CPIPC_RC_OK)
		{
			// Recv and verify Data Header Ack
			rcl = IPC_Recv(hConnection, &scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
			rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
			if (rc == CPIPC_RC_OK)
			{
				if (AVPCP_IsDataAcknowleged(&scSC))
				{
					// Send Data
					rcl = IPC_Send(hConnection, pBuffer, dwSize, dwTimeout);
					rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
					if (rc == CPIPC_RC_OK)
					{
						// Recv and verify Data Ack
						rcl = IPC_Recv(hConnection, &scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
						rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);	
						if (rc == CPIPC_RC_OK) rc = AVPCP_IsDataAcknowleged(&scSC)?CPIPC_RC_OK:CPIPC_RC_NACK;
					}
				}
			}
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvCommand - Recvs command header
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvCommand(HIPCCONNECTION hConnection, PAVPCP_COMMAND_HEADER pCommandHeader, DWORD dwTimeout)
{
	int rc = CPIPC_RC_ERROR_IO;

	if (hConnection && pCommandHeader)
		rc = CPIPC_ConvertIPCResultToCPIPCResult(IPC_Recv(hConnection, pCommandHeader, AVPCP_COMMAND_HEADER_SIZE, dwTimeout));

	return rc;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvData - Recvs data policy and sends data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvData(HIPCCONNECTION hConnection, LPPOLICY* ppPolicy, DWORD dwTimeout)
{
	DWORD dwSize;
	return CPIPC_RecvDataEx(hConnection, (PVOID*)ppPolicy, &dwSize, dwTimeout);

	return CPIPC_RC_ERROR_IO;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvDataEx - Recvs data and sends data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvDataEx(HIPCCONNECTION hConnection, PVOID* ppBuffer, DWORD* pdwSize, DWORD dwTimeout)
{
	int rc = CPIPC_RC_ERROR_IO, rcl;

	if (hConnection && ppBuffer && pdwSize)
	{
		AVPCP_DATA_HEADER dhDH;			// Data header

		AVPCP_SERVICE_COMMAND scSC;		// Acknowlege

		// Recv Data Header
		rcl = IPC_Recv(hConnection, &dhDH, AVPCP_DATA_HEADER_SIZE, dwTimeout);
		rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
		if (rc == CPIPC_RC_OK)
		{
			if (AVPCP_IsValidDataHeader(&dhDH))
			{
				// Send Data Header Ack
				AVPCP_FillDataAckCommand(&scSC, (dhDH.m_dwSize < CPIPC_MAX_POLICY_SIZE)?CP_SCS_ACK:CP_SCS_NACK);
				rcl = IPC_Send(hConnection, &scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
				rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
				if (rc == CPIPC_RC_OK)
				{
					*pdwSize = dhDH.m_dwSize;
					*ppBuffer = (LPPOLICY)malloc(dhDH.m_dwSize);
					if (*ppBuffer)
					{
						// Recv Data
						rcl = IPC_Recv(hConnection, *ppBuffer, dhDH.m_dwSize, dwTimeout);
						rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
						if (rc == CPIPC_RC_OK)
						{
							// Send Data Ack
							AVPCP_FillDataAckCommand(&scSC, CP_SCS_ACK);
							rcl = IPC_Send(hConnection, &scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
							rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
						}
						else
						{
							free(*ppBuffer);
							*ppBuffer = NULL;
							*pdwSize = 0;
						}
					}
				}
			}
			else
				rc = CPIPC_RC_ERROR_IO;
		}
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_ConvertIPCResultToCPIPCResult - Converts IPC result to CPIPC Result
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_ConvertIPCResultToCPIPCResult(int res)
{
	int rc = CPIPC_RC_OK;

	switch (res) {
		case 0: 
		case -1: 
			rc = CPIPC_RC_ERROR_IO;
		break;
		case -2: 
			rc = CPIPC_RC_ERROR_TIMEOUT;
		break;
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvDataToPreallocatedBuffer - Recvs data policy to preallocated 
// buffer and sends data acknowlege
// ----------------------------------------------------------------------	
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvDataToPreallocatedBuffer(HIPCCONNECTION hConnection, LPPOLICY* ppPolicy, DWORD dwBufferSize, DWORD dwTimeout)
{
	DWORD dwSize;
	return CPIPC_RecvDataToPreallocatedBufferEx(hConnection, (PVOID*)ppPolicy, dwBufferSize, &dwSize, dwTimeout);
}

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvDataToPreallocatedBufferEx - Recvs data to preallocated buffer 
// and sends data acknowlege
// ----------------------------------------------------------------------	
// return :				CPNET_RC_OK				- Success
//						CPNET_RC_NACK			- NACK ()
//						CPNET_RC_ERROR_IO		- I/O Error
//						CPNET_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvDataToPreallocatedBufferEx(HIPCCONNECTION hConnection, PVOID* ppBuffer, DWORD dwBufferSize, DWORD* pdwSize, DWORD dwTimeout)
{
	int rc = CPIPC_RC_ERROR_IO, rcl;

	if (CHECK_IPC_HCONNECTION(hConnection) && ppBuffer && pdwSize)
	{
		AVPCP_DATA_HEADER dhDH;			// Data header

		AVPCP_SERVICE_COMMAND scSC;		// Acknowlege

		// Recv Data Header
		rcl = IPC_Recv(hConnection, (char*)&dhDH, AVPCP_DATA_HEADER_SIZE, dwTimeout);
		rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
		if (rc == CPIPC_RC_OK)
		{
			if (AVPCP_IsValidDataHeader(&dhDH))
			{
				// Send Data Header Ack
				*pdwSize = dhDH.m_dwSize;
				PVOID pBuffer = *ppBuffer;
				if (pBuffer == NULL || dhDH.m_dwSize > dwBufferSize)
					pBuffer = malloc(dhDH.m_dwSize);

				if (pBuffer != NULL)
				{
					AVPCP_FillDataAckCommand(&scSC, CP_SCS_ACK);
					rcl = IPC_Send(hConnection, (char*)&scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
					rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
					if (rc == CPIPC_RC_OK)
					{
						// Recv Data
						rcl = IPC_Recv(hConnection, pBuffer, dhDH.m_dwSize, dwTimeout);
						rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
						if (rc == CPIPC_RC_OK)
						{
							// Send Data Ack
							AVPCP_FillDataAckCommand(&scSC, CP_SCS_ACK);
							rcl = IPC_Send(hConnection, (char*)&scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
							rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
						}
						else
							*pdwSize = 0;
					}
					if (rc == CPIPC_RC_OK)
						*ppBuffer = pBuffer;
					else
						if (pBuffer != *ppBuffer)
							free(pBuffer);
				}
				else
				{
					AVPCP_FillDataAckCommand(&scSC, CP_SCS_NACK);
					rcl = IPC_Send(hConnection, (char*)&scSC, AVPCP_SERVICE_COMMAND_SIZE, dwTimeout);
					rc = CPIPC_ConvertIPCResultToCPIPCResult(rcl);
					if (rc == CPIPC_RC_OK)
						rc = CPIPC_RC_ERROR_IO;
				}
			}
			else
				rc = CPIPC_RC_ERROR_IO;
		}
	}

	return rc;
}
