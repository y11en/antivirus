/////////////////////////////////////////////////////////////////////////////
//
// AVP Control Centre Client Command IDs
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CCCLIENT_COMMAND_IDS_H
#define CCCLIENT_COMMAND_IDS_H

#include "NetCommandIDs.h"

#define EVENT_CCCONNECTION_READY	"AVPCCC_READY"

/////////////////////////////////////////////////////////////////////////////
// Control Commands to Client
#define CP_IPC_CID_CCCL_TASK_START				0xFF220001
#define CP_IPC_CID_CCCL_TASK_STOP				0xFF220002
#define CP_IPC_CID_CCCL_TASK_PAUSE				0xFF220003
#define CP_IPC_CID_CCCL_TASK_RESUME				0xFF220004
#define CP_IPC_CID_CCCL_TASK_STATE				0xFF220005
#define CP_IPC_CID_CCCL_RELOAD_BASES			0xFF220006
#define CP_IPC_CID_CCCL_RELOAD_SETTINGS			0xFF220007
#define CP_IPC_CID_CCCL_RESET_TASK_STATISTICS	0xFF220008
#define CP_IPC_CID_CCCL_SINCHRONOUS_COMMAND_END	0xFF220009

/////////////////////////////////////////////////////////////////////////////
// Commands to Control Centre
#define CP_IPC_CID_CC_TASK_STATE				0xFF110001
#define CP_IPC_CID_CC_TASK_STATISTICS			0xFF110002
#define CP_IPC_CID_CC_TASK_ALERT				0xFF110003
// #define CP_IPC_CID_CC_TASK_EVENT				0xFF110004	- defined in other module !!!
#define CP_IPC_CID_CC_CUSTOM_COMMAND			0xFF110010
#define CP_IPC_CID_CC_NEED_RESTART				0xFF110011
#define CP_IPC_CID_CC_TASK_UPDATE_SETTINGS		0xFF110012
#define CP_IPC_CID_CC_MOVE_FILE_TO_QUARANTINE	0xFF110014
#define CP_IPC_CID_CC_CUSTOM_COMMAND_EX			0xFF110015
#define CP_IPC_CID_CC_QUERYIMPERSONATION		0xFF110016
#define CP_IPC_CID_CC_TASK_STATISTICS_EX		0xFF110017
#define CP_IPC_CID_CC_QUARANTINE_COMMAND		0xFF110018

// Network File I/O commands
#define CP_IPC_IO_ACCESS						CP_NET_IO_ACCESS
#define CP_IPC_IO_SET_FILE_POINTER				CP_NET_IO_SET_FILE_POINTER
#define CP_IPC_IO_WRITE_FILE					CP_NET_IO_WRITE_FILE
#define CP_IPC_IO_READ_FILE						CP_NET_IO_READ_FILE
#define CP_IPC_IO_CLOSE_HANDLE					CP_NET_IO_CLOSE_HANDLE
#define CP_IPC_IO_CREATE_FILE					CP_NET_IO_CREATE_FILE
#define CP_IPC_IO_FINDFIRSTFILE					CP_NET_IO_FINDFIRSTFILE
#define CP_IPC_IO_FINDNEXTFILE					CP_NET_IO_FINDNEXTFILE
#define CP_IPC_IO_FIND_CLOSE					CP_NET_IO_FIND_CLOSE
#define CP_IPC_IO_GET_FILE_SIZE					CP_NET_IO_GET_FILE_SIZE
#define CP_IPC_IO_CREATE_FOLDER					CP_NET_IO_CREATE_FOLDER
#define CP_IPC_IO_LOCK_FOLDER					CP_NET_IO_LOCK_FOLDER
#define CP_IPC_IO_UNLOCK_FOLDER					CP_NET_IO_UNLOCK_FOLDER

#define CP_IPC_IO_REPLY							CP_NET_IO_REPLY

#endif