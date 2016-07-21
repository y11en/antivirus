#ifndef __fidbox_h
#define __fidbox_h

#include "..\debug.h"
#include "..\InvThread.h"

void
FidBox_Allow ();

NTSTATUS
FidBox_Disable ();

#define _FIDBOX_CLIENT_STRUCT_SIZE	 28

// Gruzdev ----------------------------------------------------------------------------------
#define _FIDBOX2_CLIENT_STRUCT_SIZE	  4
//-------------------------------------------------------------------------------------------

typedef struct _FILE_OBJECTID_BUFFER {
    BYTE  ObjectId[16];
    union {
        struct {
            BYTE  BirthVolumeId[16];
            BYTE  BirthObjectId[16];
            BYTE  DomainId[16];
        } ;
        BYTE  ExtendedInfo[48];
    };

} FILE_OBJECTID_BUFFER, *PFILE_OBJECTID_BUFFER;

NTSTATUS
FidBox_GetData (
	PFIDBOX_REQUEST_GET pFidGet,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	ULONG* pRetSize
	);


NTSTATUS
FidBox_GetDataByFO (
	PFILE_OBJECT fileObject,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG* pFidDataBufferSize
	);

NTSTATUS
FidBox_SetData (
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize
	);

// Gruzdev ----------------------------------------------------------------------------------
NTSTATUS
FidBox2_GetData (
	PFIDBOX_REQUEST_GET pFidGet,
	PFIDBOX_REQUEST_DATA pFidData,
	ULONG* pRetSize
	);

NTSTATUS
FidBox2_SetData (
	PFIDBOX_REQUEST_DATA pFidData
	);

//-------------------------------------------------------------------------------------------

NTSTATUS
FidBox_ClearValidFlag(PWCHAR pwchFile, ULONG namelen);

#define bool		char
#define false		0
#define true		1

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

//+ ------------------------------------------------------------------------------------------

#define HRESULT	NTSTATUS

#define	ERROR_SEEK					STATUS_INVALID_DISPOSITION
#define ERROR_WRITE_FAULT			STATUS_UNSUCCESSFUL
#define ERROR_READ_FAULT			STATUS_UNSUCCESSFUL
#define ERROR_FILE_INVALID			STATUS_INVALID_HANDLE
#define ERROR_NOT_ENOUGH_MEMORY		STATUS_NO_MEMORY
#define ERROR_INVALID_PARAMETER		STATUS_INVALID_PARAMETER
#define ERROR_BAD_LENGTH			STATUS_INVALID_BLOCK_LENGTH
#define ERROR_INSUFFICIENT_BUFFER	STATUS_BUFFER_TOO_SMALL
#define ERROR_NOT_FOUND				STATUS_NOT_FOUND
#define ERROR_BUFFER_OVERFLOW		STATUS_BUFFER_OVERFLOW
#define ERROR_INTERNAL_ERROR		STATUS_INTERNAL_ERROR

//+ ------------------------------------------------------------------------------------------
typedef struct tag_tHeap tHeap;

typedef struct tag_tHeap {
	void* hHeap;
	bool (__stdcall * Alloc)(tHeap* pHeap, size_t size, void** ppMem);
	bool (__stdcall * Realloc)(tHeap* pHeap, void* pMem, size_t size, void** ppMem);
	bool (__stdcall * Free)(tHeap* pHeap, void* pMem);
	void (__stdcall * Destroy)(tHeap* pHeap);
} tHeap;

typedef long			tFilePos;
typedef unsigned long	tFileSize;
#define SEEK_SET		0

#define tFileHandle	PFILE_OBJECT	

typedef struct tag_tFile tFile;
typedef struct tag_tFile
{
	tFileHandle		hHandle;
	HANDLE			hFile;
	tFilePos nPos; // some implementation has no own position
	bool (__stdcall * SeekRead)(tFile* pFile, tFilePos nPos, void* pData, size_t nSize, size_t* pnBytesRead);
	bool (__stdcall * SeekWrite)(tFile* pFile, tFilePos nPos, void* pData, size_t nSize, size_t* pnBytesWritten);
	bool (__stdcall * Read)(tFile* pFile, void* pData, size_t nSize, size_t* pnBytesRead);
	bool (__stdcall * Write)(tFile* pFile, void* pData, size_t nSize, size_t* pnBytesWritten);
	bool (__stdcall * Seek)(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos);
	bool (__stdcall * GetSize)(tFile* pFile, tFileSize* pnSize);
	bool (__stdcall * SetSize)(tFile* pFile, tFileSize nSize);
	bool (__stdcall * Flush)(tFile* pFile);
	bool (__stdcall * Close)(tFile* pFile);
} tFile;

void FidBox_GlobalInit();
void FidBox_GlobalDone();

#endif __fidbox_h