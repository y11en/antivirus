#ifndef IPCIOS_H
#define IPCIOS_H

#include "StorageIDs.h"

#define IPCIOHANDLE PVOID			// IO File operations handle
#define IOHANDLE	IPCIOHANDLE		// IO File operations handle

/////////////////////////////////////////////////////////////////////////////////////////
// CCClient IOs

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOLockFolder
//	Prevents specified folder to be modified by other programs
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFolderName			Folder to lock
//	
//	Return : 
//	TRUE -	Folder locked successfully
//	FALSE - Folder already locked by other program
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOLockFolder(DWORD dwStorageID, LPCTSTR szFolderName);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOLOCKFOLDER)(DWORD dwStorageID, LPCTSTR szFolderName);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOUnLockFolder
//	Unlocks specified folder for other program
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFolderName			Folder to unlock
//	
//	Return : 
//	TRUE -	Folder unlocked successfully
//	FALSE - Folder already locked by other program
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOUnLockFolder(DWORD dwStorageID, LPCTSTR szFolderName);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOUNLOCKFOLDER)(DWORD dwStorageID, LPCTSTR szFolderName);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCreateDirectory
//	Create folder under specified storage.
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFolderName			Folder to create
//	
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOCreateDirectory(DWORD dwStorageID, LPCTSTR szFolderName);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOCREATEDIRECTORY)(DWORD dwStorageID, LPCTSTR szFolderName);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOAccess
//	Determine file-access permission.
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//  LPCTSTR szFileName				file name
//	
//	Return : 
//	Success - 0
//	Fail	- -1
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API int CCCLIENT_API_CALL CCClientIOAccess(DWORD dwStorageID, LPCTSTR szFileName, DWORD dwAccess);
#else
	typedef CCCLIENT_API int (CCCLIENT_API_CALL *CCCLIENTIOACCESS)(DWORD dwStorageID, LPCTSTR szFileName, DWORD dwAccess);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOGetFileSize - Gets size of the file
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				Handle of opened file
//	
//	Return : DWORD - Size of file if successful, 0xFFFFFFFF - otherwise
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API DWORD CCCLIENT_API_CALL CCClientIOGetFileSize(IPCIOHANDLE hFile);
#else
	typedef CCCLIENT_API DWORD (CCCLIENT_API_CALL *CCCLIENTIOGETFILESIZE)(IPCIOHANDLE hFile);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCreateFile
//	The CCClientIOCreateFile function creates or opens file
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID				ID of AVP Server storage (SID_UPDATES - updates folder)
//	LPCTSTR pFileName				pointer to name of the file
//	DWORD dwDesiredAccess			access (read-write) mode
//	DWORD dwShareMode				share mode
//	DWORD dwCreationDisposition		how to create
//	DWORD dwFlagsAndAttributes		file attributes
//
//	Return : 
//	Success - File handle
//	Fail	- INVALID_HANDLE_VALUE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API IPCIOHANDLE CCCLIENT_API_CALL CCClientIOCreateFile(DWORD dwStorageID, LPCTSTR pFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);
#else
	typedef CCCLIENT_API IPCIOHANDLE (CCCLIENT_API_CALL *CCCLIENTIOCREATEFILE)(DWORD dwStorageID, LPCTSTR pFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOSetFilePointer	
//	The function moves the file pointer of an open file. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				file handle
//  LONG lDistanceToMove,			number of bytes to move file pointer
//  DWORD dwMoveMethod				how to move (FILE_BEGIN, FILE_CURRENT, FILE_END)
//	
//	Return : 
//	Success - new file pointer
//	Fail	- 0xFFFFFFFF
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API DWORD CCCLIENT_API_CALL CCClientIOSetFilePointer(IPCIOHANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod);
#else
	typedef CCCLIENT_API DWORD (CCCLIENT_API_CALL *CCCLIENTIOSETFILEPOINTER)(IPCIOHANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOWriteFile
//	The CCClientIOWriteFile function writes data to a file 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile				// handle to file to write to
//	LPCVOID pBuffer,				// pointer to data to write to file
//	DWORD nNumberOfBytesToWrite		// number of bytes to write
//	LPDWORD pNumberOfBytesWritten	// pointer to number of bytes written	
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOWriteFile(IPCIOHANDLE hFile, LPCVOID pBuffer, DWORD nNumberOfBytesToWrite, LPDWORD pNumberOfBytesWritten);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOWRITEFILE)(IPCIOHANDLE hFile, LPCVOID pBuffer, DWORD nNumberOfBytesToWrite, LPDWORD pNumberOfBytesWritten);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOReadFile
//	The CCClientIOReadFile function reads data from a file, 
//	starting at the position indicated by the file pointer. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of file to read
//	LPVOID pBuffer				pointer to buffer that receives data
//	DWORD nNumberOfBytesToRead	number of bytes to read
//	LPDWORD pNumberOfBytesRead	pointer to number of bytes read
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOReadFile(IPCIOHANDLE hFile, LPVOID pBuffer, DWORD nNumberOfBytesToRead, LPDWORD pNumberOfBytesRead);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOREADFILE)(IPCIOHANDLE hFile, LPVOID pBuffer, DWORD nNumberOfBytesToRead, LPDWORD pNumberOfBytesRead);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOCloseHandle
//	Close an open object handle. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of object to close
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOCloseHandle(IPCIOHANDLE hFile);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOCLOSEHANDLE)(IPCIOHANDLE hFile);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindFirstFile
//	Searches a directory for a file whose name matches the specified filename. 
//	FindFirstFile examines subdirectory names as well as filenames. 
//	---------------------------------------------------------------------------------
//	DWORD dwStorageID					ID of AVP Server storage (SID_UPDATES - updates folder)
//	LPCTSTR pFileName					File to search
//	LPWIN32_FIND_DATA pFindFileData		pointer to structure for data on found file
//
//	Return : 
//	Success - Find handle
//	Fail	- NULL
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API IPCIOHANDLE CCCLIENT_API_CALL CCClientIOFindFirstFile(DWORD dwStorageID, LPCTSTR pFileName, LPWIN32_FIND_DATA pFindFileData);
#else
	typedef CCCLIENT_API IPCIOHANDLE (CCCLIENT_API_CALL *CCCLIENTIOFINDFIRSTFILE)(DWORD dwStorageID, LPCTSTR pFileName, LPWIN32_FIND_DATA pFindFileData);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindNextFile
//	Continues a file search from a previous call to the CCClientIOFindFirstFile function. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hSearch					// handle to search
//	LPWIN32_FIND_DATA pFindFileData		// pointer to structure for data on found file
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOFindNextFile(IPCIOHANDLE hSearch, LPWIN32_FIND_DATA pFindFileData);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOFINDNEXTTFILE)(IPCIOHANDLE hSearch, LPWIN32_FIND_DATA pFindFileData);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  CCClientIOFindClose
//	Close an open FindFirstFile handle. 
//	---------------------------------------------------------------------------------
//	IPCIOHANDLE hFile			handle of FindFirstFile object to close
//
//	Return : 
//	Success - TRUE
//	Fail	- FALSE
#ifdef CCCLIENT_STATIC_LINK
	CCCLIENT_API BOOL CCCLIENT_API_CALL CCClientIOFindClose(IPCIOHANDLE hSearch);
#else
	typedef CCCLIENT_API BOOL (CCCLIENT_API_CALL *CCCLIENTIOFINDCLOSE)(IPCIOHANDLE hSearch);
#endif

#endif