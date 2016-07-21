/////////////////////////////////////////////////////////////////////////////
//
// AVP Network Command IDs
//
/////////////////////////////////////////////////////////////////////////////

#ifndef NET_COMMAND_IDS_H
#define NET_COMMAND_IDS_H

// General Network commands
#define CP_NET_TEST						0x00000000	// Test command

#define CP_NET_SERV_BROWSE_WS			0x00000001	// Request to server to browse your segment
#define CP_NET_SERV_BROWSE_WS_REPLY		0x00000002	// Reply from server with browse data

#define CP_NET_BC_BROWSE_WS				0x00000003	// Broadcast request from server to all WS to get browse data
#define CP_NET_BROWSE_WS_REPLY			0x00000004	// Reply from WS with browse data

#define CP_NET_GET_SETTINGS				0x00000005	// Request from NCC to WS to get settings data
#define CP_NET_GET_SETTINGS_REPLY		0x00000006	// Reply from WS with settings data

#define CP_NET_SET_SETTINGS				0x00000007	// Request from NCC to WS to set settings data
#define CP_NET_NCC_CONNECT				0x00000008	// Request from NCC to connect to WS
#define CP_NET_NCC_DISCONNECT			0x00000011	// Request from NCC to disconnect from WS

#define CP_NET_ADD_OBJECT_TO_NET		0x00000010	// Initialize just Added to AVP net object 
#define CP_NET_DELETE_OBJECT_FROM_NET	0x00000032	// Remove AVP net object from net

#define CP_NET_EXECUTE_TASK				0x00000012	// Execute task on AVPCC WS
#define CP_NET_GET_TASK_STATE			0x00000013	// Get executed task state
#define CP_NET_TASK_STATE				0x00000014	// Executed task state from task
#define CP_NET_WS_TASK_ALERT			0x00000015	// Task alert from WS
#define CP_NET_TASK_STATISTICS			0x00000016	// Task statistics was changed
#define CP_NET_AUTHENTICATE				0x00000017	// Authentication
#define CP_NET_GETMAPIPROFILES			0x00000033	// Get MAPI Profiles list
#define CP_NET_TASK_EVENT				0x00000103	// New event from CC
#define CP_NET_GET_ABOUT_INFO			0x00000104	// Get component about info

#define CP_NET_UPDATE_WS_LIST_ON_SERVER 0x00000031	// Update WS list on server

#define CP_NET_NCC_GET_STATE			0x00000034	// Query Net Object state for NCC
#define CP_NET_SET_ATTENTION_FLAG		0x00000035	// Set Attention Flag
#define CP_NET_RELOAD_BASES				0x0000003C	// Reload bases of specified task on CC

#define CP_NET_AUTHENTICATE_WS			0x0000003B
#define CP_NET_ADD_OBJECT_REPLY			0x00000037	// "Add object to net" reply from object

#define CP_NET_IO_REALLOC_DATA_BUFFER	0x0000003D	// Realloc command parameters recv buffer
#define CP_NET_EXPORT_SETTINGS			0x00000102	// Export object settings
#define CP_NET_RESET_TASK_STATISTICS	0x00000107	// Reset task statistics
#define CP_NET_CCCOMMAND				0x00000112	// Command to CC
#define CP_NET_DISTRIBUTE_KEY_FILE		0x00000114	// Distribute key file over the network
#define CP_NET_CC_CAN_CONNECT_TO_NCC	0x00000115	// CC can connect to NCC
#define CP_NET_CC_CUSTOM_COMMAND		0x00000116	// Custom command from task
#define CP_NET_GET_FUNCTIONALITY		0x00000125	// Get functionality level

// AVPS Specific commands
#define CP_NET_AVPS_LIST_STORAGE					0x00000105	// List avps storage with specified id
#define CP_NET_AVPS_REMOVE_ITEM_FROM_STORAGE		0x00000106	// Remove specified items from avps storage with specified id
#define CP_NET_AVPS_GET_PRODUCT_INSTALLER_PATTERN	0x00000110	// Get product installer pattern file
#define CP_NET_AVPS_ADD_PRODUCT						0x00000111	// Add product
#define CP_NET_AVPS_EXTRACT_FILE_FROM_STORAGE		0x00000120	// Extract file from storage
#define CP_NET_AVPS_ADD_DELAYED_WS_SETTINGS			0x00000121	// Add settings for delayed upload to workstations
#define CP_NET_AVPS_GET_DELAYED_WS_SETTINGS			0x00000122	// Get delayed settings for upload to workstation
#define CP_NET_AVPS_SET_ACTUAL_WS_SETTINGS			0x00000123	// Store actual settings from workstation
#define CP_NET_AVPS_GET_ACTUAL_WS_SETTINGS			0x00000124	// Get actual settings for workstation

// AVPCC Specific commands
#define CP_NET_AVPCC_LIST_STORAGE					CP_NET_AVPS_LIST_STORAGE
#define CP_NET_AVPCC_REMOVE_ITEM_FROM_STORAGE		CP_NET_AVPS_REMOVE_ITEM_FROM_STORAGE
#define CP_NET_AVPCC_EXTRACT_FILE_FROM_STORAGE		CP_NET_AVPS_EXTRACT_FILE_FROM_STORAGE

// NCD Manipulation commands

// Access to network configuration flags
#define AVPNET_ACCESS_NO_ACCESS			0
#define AVPNET_ACCESS_FULL				1
#define AVPNET_ACCESS_READ_ONLY			2
// Access to network configuration flags

#define CP_NET_LOAD_NETWORK_CFG			0x00000100	// Gets NCD for NCC and sets access rights
#define CP_NET_SAVE_NETWORK_CFG			0x00000101	// Saves modified NCD from NCC

// Network File I/O commands
#define CP_NET_IO_REPLY					0x00000024

#define CP_NET_IO_ACCESS				0x00000018
#define CP_NET_IO_SET_FILE_POINTER		0x00000019
#define CP_NET_IO_WRITE_FILE			0x00000020
#define CP_NET_IO_READ_FILE				0x00000021
#define CP_NET_IO_CLOSE_HANDLE			0x00000022
#define CP_NET_IO_CREATE_FILE			0x00000023
#define CP_NET_IO_FINDFIRSTFILE			0x00000025
#define CP_NET_IO_FINDNEXTFILE			0x00000026
#define CP_NET_IO_FIND_CLOSE			0x00000027
#define CP_NET_IO_GET_FILE_SIZE			0x00000028
#define CP_NET_IO_CREATE_FOLDER			0x00000038
#define CP_NET_IO_LOCK_FOLDER			0x00000039
#define CP_NET_IO_UNLOCK_FOLDER			0x0000003A

////////////////////////////////////////////////////////////////////////////////
// Property id's

// File IO properties IDs
#define PROPID_NAME			1000	// name of the file
#define PROPID_ACCESS		1001	// access mode:0,GENERIC_READ,GENERIC_WRITE
#define PROPID_DISPOSITION	1002	// how to create:CREATE_ALWAYS,CREATE_NEW,
#define PROPID_HANDLE		1003	// handle of file to read
#define PROPID_BYTESREAD	1004	// number of bytes to read
#define PROPID_BYTESREADED	1005	// number of bytes read
#define PROPID_FILESIZE		1006	// file size
#define PROPID_DISTANCE		1007	// offset
#define PROPID_METHOD		1008	// move method : FILE_BEGIN, FILE_CURRENT, FILE_END
#define PROPID_STORAGEID	1010	// id of network storage
#define PROPID_BYTESWRITE	1011	// number of bytes to write
#define PROPID_BYTESWRITTEN	1012	// number of bytes written
#define PROPID_SHAREMODE	1013	// file share mode
#define PROPID_ATTRIBUTES	1014	// file attributes
#define PROPID_STORAGEROOT	1015	// Registrry key of network storage
#define PROPID_LAST_ERROR	1016	// Last Error

////////////////////////////////////////////////////////////////////////////////
// Authentication defines
#define AUTH_REPLY_OK				0xABCC0000
#define AUTH_REPLY_FAIL				0xABCC0001

#endif