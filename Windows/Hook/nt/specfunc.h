#include "../osspec.h"

//! \fn				: SpecialFileProcessing
//! \brief			: удаление файлов по списку из файла на старте драйвера + обработка групповых файловых операций
//! \return			: void 
void SpecialFileProcessing();


NTSTATUS
SpecFileRequest(
	PSPECFILEFUNC_REQUEST pSpecRequest,
	ULONG ReqSize,
	PVOID OutputBuffer,
	ULONG OutputBufferLength,
	ULONG* pOutSize);

VOID
SPImpProcessList();

NTSTATUS
OpenFile(
	PWSTR PathName,
	PHANDLE FileHandle,
	ACCESS_MASK AMask,
	ULONG ShareMask,
	ULONG CreateDisposition,
	ULONG FileAttr,
	ULONG CreateOptions,
	BOOLEAN bKernel);

NTSTATUS
GetFileSize(
	HANDLE FileHandle,
	PULONG FileSize);

NTSTATUS
ReadFile(
	HANDLE FileHandle,
	PVOID Buffer,
	ULONG ToRead,
	PLARGE_INTEGER pOffset,
	PULONG pRead);

NTSTATUS
WriteFile(
	HANDLE FileHandle,
	PVOID Buffer,
	ULONG ToWrite,
	PLARGE_INTEGER pOffset,
	PULONG pWrite);

NTSTATUS
SetFilePointer(
	HANDLE FileHandle,
	PLARGE_INTEGER pFilePos);

NTSTATUS
SetDeleteInfo (
	PFILE_OBJECT pFileObject,
	PDEVICE_OBJECT pDeviceObject );