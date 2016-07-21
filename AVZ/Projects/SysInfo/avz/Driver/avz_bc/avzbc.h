//#define _AVZ_DEBUG_PRINT

#ifdef _AVZ_DEBUG_PRINT
#define zDbgPrint DbgPrint
#else
#define zDbgPrint
#endif //_AVZ_DEBUG_PRINT


// Контекст устройства
typedef struct _DEVICE_CONTEXT
    {
    PDRIVER_OBJECT pDriverObject;
    PDEVICE_OBJECT pDeviceObject;
    }
    DEVICE_CONTEXT, *PDEVICE_CONTEXT, **PPDEVICE_CONTEXT;

#define DEVICE_CONTEXT_ sizeof (DEVICE_CONTEXT)


// Структуры для работы с SDT
typedef UCHAR               BYTE,  *PBYTE,  **PPBYTE;
typedef USHORT              WORD,  *PWORD,  **PPWORD;
typedef ULONG               DWORD, *PDWORD, **PPDWORD;
typedef NTSTATUS (NTAPI *NTPROC) ();
typedef NTPROC *PNTPROC;

// Структуры для работы с SDT (с прагмой pack(1) !)
#pragma pack(1)
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

