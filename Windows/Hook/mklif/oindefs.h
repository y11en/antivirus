#define HRESULT						NTSTATUS

#define	ERROR_SUCCESS				STATUS_SUCCESS
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

#define bool		char
#define false		0
#define true		1

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

//+ ------------------------------------------------------------------------------------------
typedef struct tag_tHeap tHeap;

typedef struct tag_tHeap {
	void* hHeap;
	bool (__stdcall * Alloc)(tHeap* pHeap, size_t size, void** ppMem, ULONG tag);
	bool (__stdcall * Realloc)(tHeap* pHeap, void* pMem, size_t size, void** ppMem);
	bool (__stdcall * Free)(tHeap* pHeap, void* pMem);
} tHeap;

typedef long			tFilePos;
typedef unsigned long	tFileSize;
#define SEEK_SET		0

#define tFileHandle PFILE_OBJECT


typedef struct tag_tFile tFile;
typedef struct tag_tFile
{
	tFileHandle		FileAccess;
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
