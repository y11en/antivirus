////////////////////////////////////////////////////////////////////////////////
// AVP Communication Protocols general defines and functions
// Header file : CmdRoutine.h

#ifndef AVP_COMMAND_ROUTINE_H
#define AVP_COMMAND_ROUTINE_H

#include <Windows.h>

////////////////////////////////////////////////////////////////////
// General protocol
//
// N | Client							|		Server
// -----------------------------------------------------------------
// 1.	Client Sends command ID
//		Send:AVPCP_COMMAND_HEADER		-->		Recv:AVPCP_COMMAND_HEADER		
// 2.	Server acknowleges (or not) client command
//		Recv:AVPCP_SERVICE_COMMAND		<--		Send:AVPCP_SERVICE_COMMAND(CP_SCS_ACK | CP_SCS_NACK)
// 3.	Client Sends command data header
//		Send:AVPCP_DATA_HEADER(datasize)-->		Recv:AVPCP_DATA_HEADER
// 4.	Server acknowleges (or not) client command data
//		Recv:AVPCP_SERVICE_COMMAND		<--		Send:AVPCP_SERVICE_COMMAND(CP_SCS_ACK | CP_SCS_NACK)
// 5.	Client Sends command data
//		Send:<data>						-->		Recv:<data>
//
//	....
//
// 3, 4, 5 ... 3, 4, 5
//
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
// Defines
//
////////////////////////////////////////////////////////////////////

// Predefined command IDs
#define CP_CID_NULL					0x00000000	// Null command

// Predefined service command IDs
#define CP_SCID_NULL				0x10000000	// Null service command
#define CP_SCID_ACK_COMMAND			0x10000001	// Acknowlege Command (used Status field)
#define CP_SCID_ACK_DATA			0x10000002	// Acknowlege Data (used Status field)

// Predefined service command Status values
#define CP_SCS_NULL					0x00000000	// Null status
#define CP_SCS_ACK					0x00000001	// Acknowleged (TRUE)
#define CP_SCS_NACK					0x00000000	// Not acknowleged (FALSE)

// AVP Communication Protocols 
#define CP_CH_PATTERN				0x41565043	// Command Header Pattern "AVPC"
#define CP_SC_PATTERN				0x41565053	// Service Command Pattern "AVPS"
#define CP_DH_PATTERN				0x41565044	// Service Command Pattern "AVPD"

// Predefined AVP Communication Protocols Command Versions
#define CP_DEF_COMMAND_VERSION		0x00000000	// Default version number

// Predefined AVP Communication Protocols Command Senders
#define CP_DEF_SENDER				0x00000000	// Default command sender

////////////////////////////////////////////////////////////////////
//
// Structures
//
////////////////////////////////////////////////////////////////////

// AVP Communication Protocols Command Header

#define AVPCP_COMMAND_HEADER_SIZE		sizeof(AVPCP_COMMAND_HEADER)

typedef struct AVPCP_COMMAND_HEADER {
	DWORD m_dwPattern;					// Command header pattern (Always = CPCH_PATTERN)
	DWORD m_dwCommandID;				// Command ID
	DWORD m_dwVersion;					// Version of command
	DWORD m_dwSenderComponentID;		// Component id of command's sender
	DWORD m_dwHasParameters;			// Flag : Command has parameters block
	DWORD m_Reserved[3];				// Reserved fields
} *PAVPCP_COMMAND_HEADER;

// AVP Communication Protocols Service Command

#define AVPCP_SERVICE_COMMAND_SIZE		sizeof(AVPCP_SERVICE_COMMAND)

typedef struct AVPCP_SERVICE_COMMAND {
	DWORD m_dwPattern;					// Service Command pattern (Always = CPSC_PATTERN)
	DWORD m_dwCommandID;				// Command ID
	DWORD m_dwVersion;					// Version of command
	DWORD m_dwSenderComponentID;		// Component id of command's sender
	DWORD m_dwStatus;					// Command status
	DWORD m_Reserved[3];				// Reserved fields
} *PAVPCP_SERVICE_COMMAND;

// AVP Communication Protocols Command's Data Header

#define AVPCP_DATA_HEADER_SIZE			sizeof(AVPCP_DATA_HEADER)

typedef struct AVPCP_DATA_HEADER {
	DWORD m_dwPattern;					// Data header pattern (Always = CPDH_PATTERN)
	DWORD m_dwSize;						// Data size
	DWORD m_Reserved[6];				// Reserved fields
} *PAVPCP_DATA_HEADER;

////////////////////////////////////////////////////////////////////
//
// Routines
//
////////////////////////////////////////////////////////////////////

void AVPCP_FillCommandHeader		(PAVPCP_COMMAND_HEADER pHeader, 
									 DWORD dwCommandID = CP_CID_NULL,
									 DWORD dwHasParameters = TRUE,
									 DWORD dwVersion = CP_DEF_COMMAND_VERSION,
									 DWORD dwSenderComponentID = CP_DEF_SENDER);

void AVPCP_FillServiceCommand		(PAVPCP_SERVICE_COMMAND pCommand, 
									 DWORD dwCommandID = CP_SCID_NULL, 
									 DWORD dwStatus = CP_SCS_NULL, 
									 DWORD dwVersion = CP_DEF_COMMAND_VERSION,
									 DWORD dwSenderComponentID = CP_DEF_SENDER);

void AVPCP_FillDataHeader			(PAVPCP_DATA_HEADER pHeader,
									 DWORD dwSize);

void AVPCP_FillCommandAckCommand	(PAVPCP_SERVICE_COMMAND pCommand, 
									 DWORD dwStatus = CP_SCID_ACK_COMMAND, 
									 DWORD dwVersion = CP_DEF_COMMAND_VERSION,
									 DWORD dwSenderComponentID = CP_DEF_SENDER);

void AVPCP_FillDataAckCommand		(PAVPCP_SERVICE_COMMAND pCommand,
									 DWORD dwStatus = CP_SCID_ACK_DATA, 
									 DWORD dwVersion = CP_DEF_COMMAND_VERSION,
									 DWORD dwSenderComponentID = CP_DEF_SENDER);

////////////////////////////////////////////////////////////////////

BOOL AVPCP_IsValidCommandHeader		(PAVPCP_COMMAND_HEADER pHeader, 
									 DWORD dwMaxVersion = CP_DEF_COMMAND_VERSION);

BOOL AVPCP_IsValidServiceCommand	(PAVPCP_SERVICE_COMMAND pCommand, 
									 DWORD dwMaxVersion = CP_DEF_COMMAND_VERSION);

BOOL AVPCP_IsValidDataHeader		(PAVPCP_DATA_HEADER pHeader);

BOOL AVPCP_IsCommandAcknowleged		(PAVPCP_SERVICE_COMMAND pCommand);

BOOL AVPCP_IsDataAcknowleged		(PAVPCP_SERVICE_COMMAND pCommand);

////////////////////////////////////////////////////////////////////

#endif