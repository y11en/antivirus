#define MYDRV				L"kl1"
#define LINKNAME_STRING     L"\\DosDevices\\"MYDRV
#define NTDEVICE_STRING     L"\\Device\\"MYDRV

#pragma warning ( disable: 4273 )


typedef
NTSTATUS
  (*t_ZwOpenFile)(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    );

extern "C"
NTSTATUS
  NtOpenFile(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    );

extern "C"
NTSTATUS
  ZwOpenFile(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    );

extern "C" ULONG **KeServiceDescriptorTable;

#define CHECK_IN_BUFFER_SIZE(_size_)                        \
    if ( (ioBuffer == NULL) || ( inBufLength < (_size_) ) )   \
    { \
        ntStatus = KL_BUFFER_TOO_SMALL; \
        break; \
    }

#define CHECK_OUT_BUFFER_SIZE(_size_)                        \
    if ( (ioBuffer == NULL) || ( outBufLength < (_size_) ) )  \
    { \
        ntStatus = KL_BUFFER_TOO_SMALL; \
        break; \
    }