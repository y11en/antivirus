#ifndef CPIPC_SEND_RECV_POLICY_H
#define CPIPC_SEND_RECV_POLICY_H

#include <CCClient\policy.h>
//#include <windows\ipc\LoadIPCDll.h>
#include <windows\ipc\ipc.h>
#include "CmdRoutine.h"

///////////////////////////////////////////////////////////////////////////
// Functions return codes
#define CPIPC_RC_OK				0		// Success
#define CPIPC_RC_NACK			1		// Command not acknowleged
#define CPIPC_RC_ERROR_IO		2		// I/O error
#define CPIPC_RC_ERROR_TIMEOUT	3		// Timeout expired
		

///////////////////////////////////////////////////////////////////////////

#define CPIPC_TIMEOUT			30000		// Send/Receive function timeout

#define CPIPC_MAX_POLICY_SIZE	0x8FFFFFFF	// Max Data policy size

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// CPIPC_BeginCommand - Begins command sending
// ----------------------------------------------------------------------
HANDLE CPIPC_BeginCommand(LPCTSTR pszMutexName);

///////////////////////////////////////////////////////////////////////////
// CPIPC_EndCommand - Ends command sending
// ----------------------------------------------------------------------
void CPIPC_EndCommand(HANDLE hMutex);

///////////////////////////////////////////////////////////////////////////
// CPIPC_AckCommand - Sends command header acknowlege command
// ----------------------------------------------------------------------
// return :				TRUE			- Success
//						FALSE			- Otherwise
BOOL CPIPC_AckCommand(HIPCCONNECTION hConnection,
					  BOOL bAck = TRUE,
					  DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_ConvertIPCResultToCPIPCResult - Converts IPC result to CPIPC Result
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_ConvertIPCResultToCPIPCResult(int rc);

///////////////////////////////////////////////////////////////////////////
// CPIPC_SendCommand - Sends command header and checks command acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_SendCommand	(HIPCCONNECTION hConnection,
						 DWORD dwCommandID = CP_CID_NULL, 
						 DWORD dwHasParameters = TRUE,
						 DWORD dwVersion = CP_DEF_COMMAND_VERSION,
						 DWORD dwSenderComponentID = CP_DEF_SENDER,
						 DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_SendData - Sends data policy and checks data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_SendData		(HIPCCONNECTION hConnection,
						 LPPOLICY pPolicy,
 						 DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_SendDataEx - Sends data and checks data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_SendDataEx	(HIPCCONNECTION hConnection, 
						 PVOID pBuffer, 
						 DWORD dwSize, 
						 DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvCommand - Recvs command header
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvCommand	(HIPCCONNECTION hConnection,
						 PAVPCP_COMMAND_HEADER pCommandHeader,
						 DWORD dwTimeout = CPIPC_TIMEOUT);


///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvData - Recvs data policy and sends data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvData		(HIPCCONNECTION hConnection,
						 LPPOLICY* ppPolicy,
						 DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvDataEx - Recvs data and sends data acknowlege
// ----------------------------------------------------------------------
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvDataEx	(HIPCCONNECTION hConnection, 
						 PVOID* ppBuffer, 
						 DWORD* pdwSize, 
						 DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvDataToPreallocatedBuffer - Recvs data policy to preallocated 
// buffer and sends data acknowlege
// ----------------------------------------------------------------------	
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvDataToPreallocatedBuffer(HIPCCONNECTION hConnection, 
									   LPPOLICY* ppPolicy,	//!< [in/out] Буфер. Если dwBufferSize недостаточно. То выделяется необходимая память с помощью malloc.
									   DWORD dwBufferSize,	//!< [in] Размер буфера ppPolicy.
									   DWORD dwTimeout = CPIPC_TIMEOUT);

///////////////////////////////////////////////////////////////////////////
// CPIPC_RecvDataToPreallocatedBufferEx - Recvs data to preallocated buffer 
// and sends data acknowlege
// ----------------------------------------------------------------------	
// return :				CPIPC_RC_OK				- Success
//						CPIPC_RC_NACK			- NACK ()
//						CPIPC_RC_ERROR_IO		- I/O Error
//						CPIPC_RC_ERROR_TIMEOUT	- Timeout
int CPIPC_RecvDataToPreallocatedBufferEx(HIPCCONNECTION hConnection, 
										 PVOID* ppBuffer,	//!< [in/out] Буфер. Если dwBufferSize недостаточно. То выделяется необходимая память с помощью malloc.
										 DWORD dwBufferSize,	//!< [in] Размер буфера ppBuffer.
										 DWORD* pdwSize, 
										 DWORD dwTimeout = CPIPC_TIMEOUT);

#endif