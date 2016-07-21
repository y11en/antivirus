#ifndef __reg_comm_h
#define __reg_comm_h

#include <windows.h>


// ---
typedef enum _KEY_INFORMATION_CLASS {
  KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
} KEY_INFORMATION_CLASS;



// ---
typedef struct _KEY_BASIC_INFORMATION {
  LARGE_INTEGER LastWriteTime;
  ULONG   TitleIndex;
  ULONG   NameLength;
  WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

// ---
typedef struct _KEY_NODE_INFORMATION {
  LARGE_INTEGER LastWriteTime;
  ULONG   TitleIndex;
  ULONG   ClassOffset;
  ULONG   ClassLength;
  ULONG   NameLength;
  WCHAR   Name[1];            // Variable length string
  //          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

// ---
typedef struct _KEY_FULL_INFORMATION {
  LARGE_INTEGER LastWriteTime;
  ULONG   TitleIndex;
  ULONG   ClassOffset;
  ULONG   ClassLength;
  ULONG   SubKeys;
  ULONG   MaxNameLen;
  ULONG   MaxClassLen;
  ULONG   Values;
  ULONG   MaxValueNameLen;
  ULONG   MaxValueDataLen;
  WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;


// ---
typedef DWORD (__stdcall* tZwQueryKey) (
  IN HANDLE KeyHandle,
  IN KEY_INFORMATION_CLASS KeyInformationClass,
  OUT PVOID KeyInformation,
  IN ULONG Length,
  OUT PULONG ResultLength
);


tZwQueryKey pZwQueryKey;
BOOL        bWOW64;
BOOL		bWOWFlagsAvailable;

#endif