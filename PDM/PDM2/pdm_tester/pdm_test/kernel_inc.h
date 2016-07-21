#ifndef _KERNEL_INC_
#define _KERNEL_INC_

 
#define OBJ_CASE_INSENSITIVE 0x40
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L

typedef DWORD ULONG;
typedef WORD  USHORT;
typedef ULONG NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_SUCCESS					((NTSTATUS)0x00000000L)

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;


typedef struct _STRING {
  USHORT  Length;
  USHORT  MaximumLength;
  PCHAR  Buffer;
} ANSI_STRING, *PANSI_STRING;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;


typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

typedef
NTSTATUS (__stdcall *NtCreateKeyFunc)(
		HANDLE KeyHandle, 
		ULONG DesiredAccess, 
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG TitleIndex, 
		PUNICODE_STRING Class, 
		ULONG CreateOptions, 
		PULONG Disposition 
		);

typedef
NTSTATUS (__stdcall *NtSetValueKeyFunc)(
		IN HANDLE  KeyHandle,
		IN PUNICODE_STRING  ValueName,
		IN ULONG  TitleIndex,			/* optional */
		IN ULONG  Type,
		IN PVOID  Data,
		IN ULONG  DataSize
		);


typedef
NTSTATUS (__stdcall *NtDeleteKeyFunc)(
		HANDLE KeyHandle
		);

typedef
NTSTATUS (WINAPI * RtlAnsiStringToUnicodeStringFunc)
	(PUNICODE_STRING  DestinationString,
	 IN PANSI_STRING  SourceString,
	 IN BOOLEAN);

typedef
VOID (WINAPI * RtlInitAnsiStringFunc)
	(IN OUT PANSI_STRING  DestinationString,
	 IN PCHAR  SourceString);

typedef
VOID (WINAPI * RtlFreeUnicodeStringFunc)
	(IN PUNICODE_STRING  UnicodeString);

typedef
NTSTATUS (WINAPI * NtOpenSectionFunc)
	(OUT PHANDLE  SectionHandle,
	 IN ACCESS_MASK  DesiredAccess,
	 IN POBJECT_ATTRIBUTES  ObjectAttributes);

typedef
NTSTATUS (WINAPI *NtMapViewOfSectionFunc)
	(HANDLE, HANDLE, LPVOID, ULONG, SIZE_T, LARGE_INTEGER*, SIZE_T*, 
                SECTION_INHERIT, ULONG, ULONG );

typedef
VOID (CALLBACK *PAPCFUNC)
	(ULONG_PTR dwParam);


extern NtCreateKeyFunc NtCreateKey;
extern NtSetValueKeyFunc NtSetValueKey;
extern NtDeleteKeyFunc NtDeleteKey;
extern NtMapViewOfSectionFunc NtMapViewOfSection;

extern RtlAnsiStringToUnicodeStringFunc RtlAnsiStringToUnicodeString;
extern RtlInitAnsiStringFunc RtlInitAnsiString;
extern RtlFreeUnicodeStringFunc RtlFreeUnicodeString;
extern NtOpenSectionFunc NtOpenSection;

HANDLE openPhyMem();
bool LocateNTDLLFunctions();
BOOL assignACL(void);
LPVOID NTAPI MapViewOfFileExWrap(HANDLE hProcess, HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, 
        DWORD dwFileOffsetLow, DWORD dwNumberOfBytesToMap, LPVOID lpBaseAddress);



#endif
