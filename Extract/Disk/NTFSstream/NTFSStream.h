#if !defined (__NTFS_STREAM) && !defined (__unix__)
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
	FileAlternateNameInformation, // ???
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation, // ???
	FileMailslotSetInformation,   // ???
	FileCompressionInformation,   // ???
	FileCopyOnWriteInformation,   // ???
	FileCompletionInformation,
	FileMoveClusterInformation,   // ???
	FileOleClassIdInformation,    // ???
	FileOleStateBitsInformation,  // ???
	FileNetworkOpenInformation,
	FileObjectIdInformation,
	FileOleAllInformation,        // ???
	FileOleDirectoryInformation,  // ???
	FileContentIndexInformation,  // ???
	FileInheritContentIndexInformation, // ???
	FileOleInformation,                 // ???
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

//
// Define the various structures which are returned on query operations
//

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct _FILE_OBJECTID_INFORMATION {
    LONGLONG FileReference;
    UCHAR ObjectId[16];
    union {
        struct {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        } ;
        UCHAR ExtendedInfo[48];
    };
} FILE_OBJECTID_INFORMATION, *PFILE_OBJECTID_INFORMATION;

//
//  The following constants provide addition meta characters to fully
//  support the more obscure aspects of DOS wild card processing.
//

#define ANSI_DOS_STAR   ('<')
#define ANSI_DOS_QM     ('>')
#define ANSI_DOS_DOT    ('"')

#define DOS_STAR        (L'<')
#define DOS_QM          (L'>')
#define DOS_DOT         (L'"')

//
// NtQuery(Set)InformationFile return types:
//
//      FILE_BASIC_INFORMATION
//      FILE_STANDARD_INFORMATION
//      FILE_INTERNAL_INFORMATION
//      FILE_EA_INFORMATION
//      FILE_ACCESS_INFORMATION
//      FILE_POSITION_INFORMATION
//      FILE_MODE_INFORMATION
//      FILE_ALIGNMENT_INFORMATION
//      FILE_NAME_INFORMATION
//      FILE_ALL_INFORMATION
//
//      FILE_NETWORK_OPEN_INFORMATION
//
//      FILE_ALLOCATION_INFORMATION
//      FILE_COMPRESSION_INFORMATION   // absent
//      FILE_DISPOSITION_INFORMATION
//      FILE_END_OF_FILE_INFORMATION
//      FILE_LINK_INFORMATION
//      FILE_MOVE_CLUSTER_INFORMATION  // absent
//      FILE_RENAME_INFORMATION
//      FILE_STREAM_INFORMATION
//      FILE_COMPLETION_INFORMATION
//
//      FILE_PIPE_INFORMATION
//      FILE_PIPE_LOCAL_INFORMATION
//      FILE_PIPE_REMOTE_INFORMATION
//
//      FILE_MAILSLOT_QUERY_INFORMATION  // absent
//      FILE_MAILSLOT_SET_INFORMATION    // absent
//      FILE_REPARSE_POINT_INFORMATION   // absent
//

typedef struct _FILE_BASIC_INFORMATION {                    // ntddk wdm nthal
    LARGE_INTEGER CreationTime;                             // ntddk wdm nthal
    LARGE_INTEGER LastAccessTime;                           // ntddk wdm nthal
    LARGE_INTEGER LastWriteTime;                            // ntddk wdm nthal
    LARGE_INTEGER ChangeTime;                               // ntddk wdm nthal
    ULONG FileAttributes;                                   // ntddk wdm nthal
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;         // ntddk wdm nthal
                                                            // ntddk wdm nthal
typedef struct _FILE_STANDARD_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER AllocationSize;                           // ntddk wdm nthal
    LARGE_INTEGER EndOfFile;                                // ntddk wdm nthal
    ULONG NumberOfLinks;                                    // ntddk wdm nthal
    BOOLEAN DeletePending;                                  // ntddk wdm nthal
    BOOLEAN Directory;                                      // ntddk wdm nthal
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;   // ntddk wdm nthal
                                                            // ntddk wdm nthal
typedef struct _FILE_INTERNAL_INFORMATION {
    LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

typedef struct _FILE_EA_INFORMATION {
    ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

typedef struct _FILE_POSITION_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER CurrentByteOffset;                        // ntddk wdm nthal
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;   // ntddk wdm nthal
                                                            // ntddk wdm nthal
typedef struct _FILE_MODE_INFORMATION {
    ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

typedef struct _FILE_ALIGNMENT_INFORMATION {                // ntddk nthal
    ULONG AlignmentRequirement;                             // ntddk nthal
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION; // ntddk nthal
                                                            // ntddk nthal
typedef struct _FILE_NAME_INFORMATION {                     // ntddk
    ULONG FileNameLength;                                   // ntddk
    WCHAR FileName[1];                                      // ntddk
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;           // ntddk
                                                            // ntddk
typedef struct _FILE_ALL_INFORMATION {
    FILE_BASIC_INFORMATION BasicInformation;
    FILE_STANDARD_INFORMATION StandardInformation;
    FILE_INTERNAL_INFORMATION InternalInformation;
    FILE_EA_INFORMATION EaInformation;
    FILE_ACCESS_INFORMATION AccessInformation;
    FILE_POSITION_INFORMATION PositionInformation;
    FILE_MODE_INFORMATION ModeInformation;
    FILE_ALIGNMENT_INFORMATION AlignmentInformation;
    FILE_NAME_INFORMATION NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

typedef struct _FILE_NETWORK_OPEN_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER CreationTime;                                 // ntddk wdm nthal
    LARGE_INTEGER LastAccessTime;                               // ntddk wdm nthal
    LARGE_INTEGER LastWriteTime;                                // ntddk wdm nthal
    LARGE_INTEGER ChangeTime;                                   // ntddk wdm nthal
    LARGE_INTEGER AllocationSize;                               // ntddk wdm nthal
    LARGE_INTEGER EndOfFile;                                    // ntddk wdm nthal
    ULONG FileAttributes;                                       // ntddk wdm nthal
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;   // ntddk wdm nthal
                                                                // ntddk wdm nthal
typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION {               // ntddk nthal
    ULONG FileAttributes;                                       // ntddk nthal
    ULONG ReparseTag;                                           // ntddk nthal
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;  // ntddk nthal
                                                                // ntddk nthal
typedef struct _FILE_ALLOCATION_INFORMATION {
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;


typedef struct _FILE_DISPOSITION_INFORMATION {                  // ntddk nthal
    BOOLEAN DeleteFile;                                         // ntddk nthal
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION; // ntddk nthal
                                                                // ntddk nthal
typedef struct _FILE_END_OF_FILE_INFORMATION {                  // ntddk nthal
    LARGE_INTEGER EndOfFile;                                    // ntddk nthal
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION; // ntddk nthal
                                                                // ntddk nthal
#ifdef _MAC
#pragma warning( disable : 4121)
#endif

typedef struct _FILE_LINK_INFORMATION {
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_LINK_INFORMATION, *PFILE_LINK_INFORMATION;


typedef struct _FILE_RENAME_INFORMATION {
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

#ifdef _MAC
#pragma warning( default : 4121 )
#endif

typedef struct _FILE_STREAM_INFORMATION {
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

typedef struct _FILE_TRACKING_INFORMATION {
    HANDLE DestinationFile;
    ULONG ObjectInformationLength;
    CHAR ObjectInformation[1];
} FILE_TRACKING_INFORMATION, *PFILE_TRACKING_INFORMATION;

typedef struct _FILE_COMPLETION_INFORMATION {
    HANDLE Port;
    PVOID Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

typedef struct _FILE_PIPE_INFORMATION {
     ULONG ReadMode;
     ULONG CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

typedef struct _FILE_PIPE_LOCAL_INFORMATION {
     ULONG NamedPipeType;
     ULONG NamedPipeConfiguration;
     ULONG MaximumInstances;
     ULONG CurrentInstances;
     ULONG InboundQuota;
     ULONG ReadDataAvailable;
     ULONG OutboundQuota;
     ULONG WriteQuotaAvailable;
     ULONG NamedPipeState;
     ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef struct _FILE_PIPE_REMOTE_INFORMATION {
     LARGE_INTEGER CollectDataTime;
     ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

//
// NtQuery(Set)EaFile
//
// The offset for the start of EaValue is EaName[EaNameLength + 1]
//

// begin_ntddk begin_wdm

typedef struct _FILE_FULL_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

// end_ntddk end_wdm

typedef struct _FILE_GET_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR EaNameLength;
    CHAR EaName[1];
} FILE_GET_EA_INFORMATION, *PFILE_GET_EA_INFORMATION;

//
// NtQuery(Set)QuotaInformationFile
//

typedef struct _FILE_GET_QUOTA_INFORMATION {
    ULONG NextEntryOffset;
    ULONG SidLength;
    SID Sid;
} FILE_GET_QUOTA_INFORMATION, *PFILE_GET_QUOTA_INFORMATION;

typedef struct _FILE_QUOTA_INFORMATION {
    ULONG NextEntryOffset;
    ULONG SidLength;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER QuotaUsed;
    LARGE_INTEGER QuotaThreshold;
    LARGE_INTEGER QuotaLimit;
    SID Sid;
} FILE_QUOTA_INFORMATION, *PFILE_QUOTA_INFORMATION;

// begin_ntddk begin_wdm begin_nthal
//
// Define the file system information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation       = 1,
    FileFsLabelInformation,      // 2
    FileFsSizeInformation,       // 3
    FileFsDeviceInformation,     // 4
    FileFsAttributeInformation,  // 5
    FileFsControlInformation,    // 6
    FileFsFullSizeInformation,   // 7
    FileFsObjectIdInformation,   // 8
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

// end_ntddk end_wdm end_nthal
//
// NtQuery[Set]VolumeInformationFile types:
//
//  FILE_FS_LABEL_INFORMATION
//  FILE_FS_VOLUME_INFORMATION
//  FILE_FS_SIZE_INFORMATION
//  FILE_FS_DEVICE_INFORMATION
//  FILE_FS_ATTRIBUTE_INFORMATION
//  FILE_FS_CONTROL_INFORMATION
//  FILE_FS_OBJECTID_INFORMATION
//

typedef struct _FILE_FS_LABEL_INFORMATION {
    ULONG VolumeLabelLength;
    WCHAR VolumeLabel[1];
} FILE_FS_LABEL_INFORMATION, *PFILE_FS_LABEL_INFORMATION;

typedef struct _FILE_FS_VOLUME_INFORMATION {
    LARGE_INTEGER VolumeCreationTime;
    ULONG VolumeSerialNumber;
    ULONG VolumeLabelLength;
    BOOLEAN SupportsObjects;
    WCHAR VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

typedef struct _FILE_FS_SIZE_INFORMATION {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

typedef struct _FILE_FS_FULL_SIZE_INFORMATION {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER CallerAvailableAllocationUnits;
    LARGE_INTEGER ActualAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION, *PFILE_FS_FULL_SIZE_INFORMATION;

typedef struct _FILE_FS_OBJECTID_INFORMATION {
    UCHAR ObjectId[16];
    UCHAR ExtendedInfo[48];
} FILE_FS_OBJECTID_INFORMATION, *PFILE_FS_OBJECTID_INFORMATION;

//typedef struct _FILE_FS_DEVICE_INFORMATION {                    // ntddk wdm nthal
//    DEVICE_TYPE DeviceType;                                     // ntddk wdm nthal
//    ULONG Characteristics;                                      // ntddk wdm nthal
//} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;     // ntddk wdm nthal
//                                                                // ntddk wdm nthal
typedef struct _FILE_FS_ATTRIBUTE_INFORMATION {
    ULONG FileSystemAttributes;
    LONG MaximumComponentNameLength;
    ULONG FileSystemNameLength;
    WCHAR FileSystemName[1];
} FILE_FS_ATTRIBUTE_INFORMATION, *PFILE_FS_ATTRIBUTE_INFORMATION;


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

