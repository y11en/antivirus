#ifndef _PE3264_H_
#define _PE3264_H_

#include <winnt.h>

// this stuctures define common fields for 32 and 64 

typedef struct _IMAGE_OPTIONAL_HEADER3264 {
    WORD        Magic;
    BYTE        MajorLinkerVersion;
    BYTE        MinorLinkerVersion;
    DWORD       SizeOfCode;
    DWORD       SizeOfInitializedData;
    DWORD       SizeOfUninitializedData;
    DWORD       AddressOfEntryPoint;
    DWORD       BaseOfCode;
	DWORD       Mismatch1; // BaseOfData;
    DWORD       Mismatch2; // ImageBase;
    DWORD       SectionAlignment;
    DWORD       FileAlignment;
    WORD        MajorOperatingSystemVersion;
    WORD        MinorOperatingSystemVersion;
    WORD        MajorImageVersion;
    WORD        MinorImageVersion;
    WORD        MajorSubsystemVersion;
    WORD        MinorSubsystemVersion;
    DWORD       Win32VersionValue;
    DWORD       SizeOfImage;
    DWORD       SizeOfHeaders;
    DWORD       CheckSum;
    WORD        Subsystem;
    WORD        DllCharacteristics;
    //ULONGLONG   SizeOfStackReserve;
    //ULONGLONG   SizeOfStackCommit;
    //ULONGLONG   SizeOfHeapReserve;
    //ULONGLONG   SizeOfHeapCommit;
    //DWORD       LoaderFlags;
    //DWORD       NumberOfRvaAndSizes;
    //IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER3264, *PIMAGE_OPTIONAL_HEADER3264;

typedef struct _IMAGE_NT_HEADERS3264 {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER3264 OptionalHeader;
} IMAGE_NT_HEADERS3264, *PIMAGE_NT_HEADERS3264;

#define IMAGE_FIRST_DIRECTORY(pNTHeaders) (pNTHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ? \
		(((PIMAGE_NT_HEADERS32)pNTHeaders)->OptionalHeader.DataDirectory) : \
		pNTHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 ? \
		(((PIMAGE_NT_HEADERS64)pNTHeaders)->OptionalHeader.DataDirectory) : \
		NULL)

#endif  _PE3264_H_