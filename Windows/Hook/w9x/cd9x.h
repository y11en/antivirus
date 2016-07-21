#define SECTOR_SIZE 	0x800
#define SECTOR_COUNT 	0x08
#define TIMEOUT_VALUE	0x0E

typedef LONG	NTSTATUS;
typedef UCHAR	SRBSTATUS;

#define SaveStdRegister			\
		_asm push	ebx			\
		_asm push	ecx			\
		_asm push	edx			\
		_asm push	esi			\
		_asm push	edi

#define RestoreStdRegister			\
		_asm pop		edi			\
		_asm pop		esi			\
		_asm pop		edx			\
		_asm pop		ecx			\
		_asm pop		ebx

#undef VxDJmp
#undef VMMCall
	
#define VMMCall VxDCall

#define VxDJmp(Service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit ((Service) & 0xff) \
    _asm _emit (((Service) >> 8) & 0xff) | 0x80 \
    _asm _emit ((Service) >> 16) & 0xff \
    _asm _emit ((Service) >> 24) & 0xff

typedef struct _t_cdb_
{
	WORD	length;
	BYTE	flags;
	BYTE	bytes_in_command;
	DWORD	p_to_cp;
	DWORD	number_of_bytes;
	DWORD	p_st_data;
	WORD	access_timeout;
	WORD	sec_inc;
}_cdb_;

#define SRB_FUNCTION_EXECUTE_SCSI           0x00

#define SRB_FLAGS_DISABLE_DISCONNECT        0x00000004
#define SRB_FLAGS_DISABLE_AUTOSENSE         0x00000020
#define SRB_FLAGS_DATA_IN                   0x00000040

#define SRB_STATUS_SUCCESS                  0x01

#define SENSE_LEN							0x0E

NTSTATUS __stdcall InitializeDriver(VOID);
NTSTATUS __stdcall OpenCDROM(DWORD LetterIndex,DWORD MaximumDataTransferLength,HANDLE *Handle,PVOID *Buffer);
VOID __stdcall CloseCDROM(HANDLE Handle);
SRBSTATUS __stdcall SendCDB(HANDLE Handle,PVOID Cdb,DWORD CdbLength,DWORD DataTransferLength);

