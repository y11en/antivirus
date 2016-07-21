#ifndef __NTFS_STREAM
#define __NTFS_STREAM

#include "windows.h"

#define IN
#define OUT

#include <ntsecapi.h> // for LsaNtStatusToWinError()
#pragma hdrstop

//
// Define the file information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.
//

typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileCopyOnWriteInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileOleClassIdInformation,
	FileOleStateBitsInformation,
	FileNetworkOpenInformation,
	FileObjectIdInformation,
	FileOleAllInformation,
	FileOleDirectoryInformation,
	FileContentIndexInformation,
	FileInheritContentIndexInformation,
	FileOleInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

//
// Define the various structures which are returned on query operations
//

typedef struct _FILE_BASIC_INFORMATION {
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

typedef struct _FILE_POSITION_INFORMATION {
	LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

typedef struct _FILE_ALIGNMENT_INFORMATION {
	ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

typedef struct _FILE_NETWORK_OPEN_INFORMATION {
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

typedef struct _FILE_DISPOSITION_INFORMATION {
	BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;

typedef struct _FILE_END_OF_FILE_INFORMATION {
	LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;


typedef struct _FILE_FULL_EA_INFORMATION {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;


struct FILE_STREAM_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG StreamNameLength;
	LARGE_INTEGER StreamSize;
	LARGE_INTEGER StreamAllocationSize;
	WCHAR StreamName[1];
};

//
// Define the base asynchronous I/O argument types
//

typedef struct _IO_STATUS_BLOCK {
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


DWORD __stdcall NtQueryInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
	);

typedef DWORD (__stdcall *NQIF)(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
	);

// -----------------------------------------------------------------------------------------

typedef DWORD (__stdcall *NQIF)(IN HANDLE /*FileHandle*/, OUT PIO_STATUS_BLOCK /*IoStatusBlock*/, OUT PVOID /*FileInformation*/, IN ULONG /*Length*/, IN FILE_INFORMATION_CLASS /*FileInformationClass*/);

typedef struct _tNTFS_ENV
{
	HINSTANCE hNtdll;
	NQIF nqif;
}NTFS_ENV, *PNTFS_ENV;

BOOL NTFSStreamInit();
void NTFSStreamDone();
void NTFSStreamProceedFile(char* pchFileName);

#endif //__NTFS_STREAM

