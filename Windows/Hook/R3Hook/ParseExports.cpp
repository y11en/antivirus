#include "ParseExports.h"

// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
#define GetImgDirEntryRVA( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)
#define GetImgDirEntrySize( pNTHdr, IDE ) \
	(pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

//================================================================================
//
// Given an RVA, look up the section header that encloses it and return a
// pointer to its IMAGE_SECTION_HEADER
//
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;
    
    for ( i=0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++ )
    {
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		DWORD size = section->Misc.VirtualSize;
		if ( 0 == size )
			size = section->SizeOfRawData;
			
        // Is the RVA within this section?
        if ( (rva >= section->VirtualAddress) && 
             (rva < (section->VirtualAddress + size)))
            return section;
    }
    
    return 0;
}

LPVOID GetPtrFromRVA( DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase )
{
	PIMAGE_SECTION_HEADER pSectionHdr;
	INT delta;
		
	pSectionHdr = GetEnclosingSectionHeader( rva, pNTHeader );
	if ( !pSectionHdr )
		return 0;

	delta = (INT)(pSectionHdr->VirtualAddress-pSectionHdr->PointerToRawData);
	return (PVOID) ( imageBase + rva - delta );
}


PIMAGE_NT_HEADERS GetNTHeader( PIMAGE_DOS_HEADER dosHeader )
{
    PIMAGE_NT_HEADERS pNTHeader;
    PBYTE pImageBase = (PBYTE)dosHeader;
    
	if (!dosHeader)
		return NULL;
	
	// Make pointers to 32 and 64 bit versions of the header.
    pNTHeader = MakePtr( PIMAGE_NT_HEADERS, dosHeader,
                                dosHeader->e_lfanew );

    // First, verify that the e_lfanew field gave us a reasonable
    // pointer, then verify the PE signature.
	if ( IsBadReadPtr( pNTHeader, sizeof(pNTHeader->Signature) ) )
	{
		//printf("Not a Portable Executable (PE) EXE\n");
		return NULL;
	}

    if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
    {
        //printf("Not a Portable Executable (PE) EXE\n");
        return NULL;
    }
	return pNTHeader;
}

DWORD GetProcRVA(PBYTE pImageBase, char* pProcName)
{
    PIMAGE_EXPORT_DIRECTORY pExportDir;
    PIMAGE_SECTION_HEADER header;
	PIMAGE_NT_HEADERS pNTHeader;
    INT delta; 
    PSTR pszFilename;
    DWORD i;
    PDWORD pdwFunctions;
    PWORD pwOrdinals;
    DWORD *pszFuncNames;
    DWORD exportsStartRVA, exportsEndRVA;

	pNTHeader = GetNTHeader((PIMAGE_DOS_HEADER)pImageBase);
    
    exportsStartRVA = GetImgDirEntryRVA(pNTHeader,IMAGE_DIRECTORY_ENTRY_EXPORT);
    exportsEndRVA = exportsStartRVA +
	   				GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);

    // Get the IMAGE_SECTION_HEADER that contains the exports.  This is
    // usually the .edata section, but doesn't have to be.
    header = GetEnclosingSectionHeader( exportsStartRVA, pNTHeader );
    if ( !header )
        return NULL;

    delta = (INT)(header->VirtualAddress - header->PointerToRawData);
        
    pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetPtrFromRVA(exportsStartRVA, pNTHeader, pImageBase);
        
    pszFilename = (PSTR)GetPtrFromRVA( pExportDir->Name, pNTHeader, pImageBase );
        
    pdwFunctions =	(PDWORD)GetPtrFromRVA( pExportDir->AddressOfFunctions, pNTHeader, pImageBase );
    pwOrdinals =	(PWORD)	GetPtrFromRVA( pExportDir->AddressOfNameOrdinals, pNTHeader, pImageBase );
    pszFuncNames =	(DWORD *)GetPtrFromRVA( pExportDir->AddressOfNames, pNTHeader, pImageBase );

    for (	i=0;
			i < pExportDir->NumberOfFunctions;
			i++, pdwFunctions++ )
    {
        DWORD entryPointRVA = *pdwFunctions;

        if ( entryPointRVA == 0 )   // Skip over gaps in exported function
            continue;               // ordinals (the entrypoint is 0 for
                                    // these functions).

//        printf("  %08X  %4u", entryPointRVA, i + pExportDir->Base );

        // Is it a forwarder?  If so, the entry point RVA is inside the
        // .edata section, and is an RVA to the DllName.EntryPointName
        if ( (entryPointRVA >= exportsStartRVA)
             && (entryPointRVA <= exportsEndRVA) )
        {
//            printf(" (forwarder -> %s)", GetPtrFromRVA(entryPointRVA, pNTHeader, pImageBase) );
        }
		else
		{
			if ((DWORD_PTR)pProcName > 0x10000)
			{
				// See if this function has an associated name exported for it.
				for ( unsigned j=0; j < pExportDir->NumberOfNames; j++ )
				{
					if ( pwOrdinals[j] == i )
					{
						//printf("  %s", GetPtrFromRVA(pszFuncNames[j], pNTHeader, pImageBase) );
						char* pName = (char*) GetPtrFromRVA(pszFuncNames[j], pNTHeader, pImageBase);
						if (strcmp(pName, pProcName) == 0)
							return entryPointRVA;
					}
				}
			}
			else
			{
				if ((DWORD_PTR)i + pExportDir->Base == (DWORD_PTR)pProcName)
					return entryPointRVA;
			}
		}

        //printf("\n");
    }

	//printf( "\n" );
	return NULL;
}

FARPROC GetRealProcAddress(HMODULE hBaseAddress, PBYTE pModuleData, char* pProcName)
{
		DWORD rva = GetProcRVA(pModuleData, pProcName);
		if (!rva)
			return NULL;
		return (FARPROC)((ULONG_PTR)hBaseAddress + rva);
}

BOOL LoadModuleDataByHandle(HMODULE hModule, PBYTE* ppModuleData, DWORD* pdwSize)
{
	TCHAR szModulePath[MAX_PATH];
	if (!GetModuleFileName(hModule, szModulePath, MAX_PATH))
		return FALSE;
	return LoadModuleData(szModulePath, ppModuleData, pdwSize);
}

BOOL LoadModuleData(TCHAR* pModulePath, PBYTE* ppData, DWORD* pdwSize)
{
	HANDLE hFile = CreateFile(pModulePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	BOOL bRes = FALSE;
	DWORD dwSize = GetFileSize(hFile, 0);
	if (dwSize && dwSize != (DWORD)-1)
	{
		PBYTE pData = (PBYTE)HeapAlloc(GetProcessHeap(), 0, dwSize);
		if (pData)
		{
			DWORD dwRead;
			if (ReadFile(hFile, pData, dwSize, &dwRead, 0) && dwRead == dwSize)
			{
				if (ppData)
					*ppData = pData;
				if (pdwSize)
					*pdwSize = dwSize;
				bRes = TRUE;
			}
			if (!bRes || !ppData)
				HeapFree(GetProcessHeap(), 0, pData);
		}
	}
	CloseHandle(hFile);
	return bRes;
}

void FreeModuleData(PBYTE* ppData, DWORD* pdwSize)
{
	if (ppData && *ppData)
	{
		HeapFree(GetProcessHeap(), 0, *ppData);
		*ppData = 0;
	}
}

/*
typedef void* (__stdcall *tGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
int main(int argc, char* argv[])
{
	cFileExports kernel32(GetModuleHandle("kernel32"));
	
	DWORD rva = kernel32.GetProcRVA("LoadLibraryA");

	HMODULE hKernel32 = GetModuleHandle("kernel32");

	tGetProcAddress pfGetProcAddress = (tGetProcAddress) kernel32.GetProcAddress(hKernel32, "GetProcAddress");

	void* pLL1 = GetProcAddress(hKernel32, "LoadLibraryA");
	void* pLL2 = pfGetProcAddress(hKernel32, "LoadLibraryA");

	return 0;
}
*/

