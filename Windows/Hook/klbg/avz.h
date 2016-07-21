#ifndef		__AVZ_H
#define		__AVZ_H
//---------------------------------------------------------------------------------

#ifdef _AVZ_DEBUG_PRINT
#define zDbgPrint DbgPrint
#else
#define zDbgPrint
#endif //_AVZ_DEBUG_PRINT

#define _CSTRING(_text) #_text
#define CSTRING(_text) _CSTRING (_text)

#define _USTRING(_text) L##_text
#define USTRING(_text) _USTRING (_text)

#define PRESET_UNICODE_STRING(_symbol,_buffer) \
	UNICODE_STRING _symbol = \
			{ \
			sizeof (USTRING (_buffer)) - sizeof (WORD), \
			sizeof (USTRING (_buffer)), \
			USTRING (_buffer) \
			};

// Структуры для работы с SDT
typedef UCHAR               BYTE,  *PBYTE,  **PPBYTE;
typedef USHORT              WORD,  *PWORD,  **PPWORD;
typedef ULONG               DWORD, *PDWORD, **PPDWORD;
typedef NTSTATUS (NTAPI *NTPROC) ();
typedef NTPROC *PNTPROC;
 
// Структуры для работы с SDT (с прагмой pack(1) !)
#include <pshpack1.h>

typedef struct _SYSTEM_SERVICE_TABLE
        {
		PNTPROC ServiceTable;           // array of entry points
		PDWORD  CounterTable;           // array of usage counters
		DWORD   ServiceLimit;           // number of table entries
		PBYTE   ArgumentTable;          // array of byte counts
		}
        SYSTEM_SERVICE_TABLE, * PSYSTEM_SERVICE_TABLE, **PPSYSTEM_SERVICE_TABLE;

#define SYSTEM_SERVICE_TABLE_ sizeof (SYSTEM_SERVICE_TABLE)

// -----------------------------------------------------------------


typedef struct _SERVICE_DESCRIPTOR_TABLE
        {
		SYSTEM_SERVICE_TABLE ntoskrnl;  // ntoskrnl.exe (native api)
		SYSTEM_SERVICE_TABLE win32k;    // win32k.sys   (gdi/user)
		SYSTEM_SERVICE_TABLE Table3;    // not used
		SYSTEM_SERVICE_TABLE Table4;    // not used
		} SERVICE_DESCRIPTOR_TABLE, *PSERVICE_DESCRIPTOR_TABLE, **PPSERVICE_DESCRIPTOR_TABLE;

#define SERVICE_DESCRIPTOR_TABLE_ sizeof (SERVICE_DESCRIPTOR_TABLE)

typedef struct _IDT
{
	USHORT Size;
	ULONG  Base;
} IDT;

#include <poppack.h>

typedef NTSTATUS (NTAPI *NTPROC) ();
typedef NTPROC *PNTPROC;

#define IOCTL_Z_GET_KESDT_PTR   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_SDT_PTR     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_SDT_SIZE    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_SDT_ENTRY   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_SET_SDT_ENTRY   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_DELETE_FILE     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_READ_MEMORY     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_WRITE_MEMORY    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_CPU_176REG  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_SETTRUSTEDPID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x819, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_SET_CPU_176REG  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_IDT         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81b, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_DUMP_MEMORY     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81c, METHOD_BUFFERED, FILE_ANY_ACCESS)

#ifdef __cplusplus
extern "C" {
#endif

NTSTATUS AvzDispatchControl ( __in PIRP pIrp );

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------------
#endif