#include <fltKernel.h>
#include <ntimage.h>

#include "mtypes.h"
#include "debug.h"
#include "hook.h"
#include "osspec.h"

#ifdef _WIN64
#pragma message("------------------- 64 bit")
#endif

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(ULONG RVA, PIMAGE_NT_HEADERS pNtHeader)
{
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
    ULONG i;
    
    for (i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSection++)
    {
		ULONG SectionSize = pSection->Misc.VirtualSize;
		if (0 == SectionSize)
			SectionSize = pSection->SizeOfRawData;
			
        if ( RVA >= pSection->VirtualAddress && 
             RVA <  pSection->VirtualAddress+SectionSize)
            return pSection;
    }
    
    return NULL;
}

// получаем хедер секции, которая по адресу идет сразу за данной секцией
PIMAGE_SECTION_HEADER GetFollowingSectionHeader(PIMAGE_NT_HEADERS pNtHeader, PIMAGE_SECTION_HEADER pSectionHeader)
{
    PIMAGE_SECTION_HEADER pCurrSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	ULONG SourceSectionRva = pSectionHeader->VirtualAddress;
    ULONG i;
	ULONG MinRva = -1L;
	PIMAGE_SECTION_HEADER pFollowingSectionHeader = NULL;

    for (i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pCurrSectionHeader++)
    {
		ULONG CurrRva = pCurrSectionHeader->VirtualAddress;
// выше по rva
		if (CurrRva > SourceSectionRva)
		{
			if (CurrRva < MinRva)
			{
				MinRva = CurrRva;
				pFollowingSectionHeader = pCurrSectionHeader;
			}
		}
    }

	return pFollowingSectionHeader;
}

PVOID GetExport(IN PVOID ImageBase, IN PCHAR NativeName, OUT PVOID *p_ExportAddr OPTIONAL, OUT PULONG pNativeSize OPTIONAL) 
{
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	ULONG i;
	PULONG pFunctionRVAs;
	PUSHORT pOrdinals;
	PULONG pFuncNameRVAs;
	ULONG exportsStartRVA;

	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;

	__try
	{
		pDosHeader= (PIMAGE_DOS_HEADER)ImageBase;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return NULL;

		pNTHeader = (PIMAGE_NT_HEADERS)((PCHAR)ImageBase+pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
			return NULL;
	    
		exportsStartRVA = IMG_DIR_ENTRY_RVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);
		pExportDir = (PIMAGE_EXPORT_DIRECTORY)PTR_FROM_RVA(ImageBase, pNTHeader, exportsStartRVA);
		if (!pExportDir)
			return NULL;

		pFunctionRVAs = (PULONG)PTR_FROM_RVA(ImageBase, pNTHeader, pExportDir->AddressOfFunctions);
		if (!pFunctionRVAs)
			return NULL;
		pOrdinals = (PUSHORT)PTR_FROM_RVA(ImageBase, pNTHeader, pExportDir->AddressOfNameOrdinals);
		if (!pOrdinals)
			return NULL;
		pFuncNameRVAs = (PULONG)PTR_FROM_RVA(ImageBase, pNTHeader, pExportDir->AddressOfNames);
		if (!pFuncNameRVAs)
			return NULL;

		for (i = 0; i < pExportDir->NumberOfNames; i++)
		{
			PCHAR FuncName;
			ULONG FuncNameRVA = pFuncNameRVAs[i];

			FuncName = PTR_FROM_RVA(ImageBase, pNTHeader, FuncNameRVA);
			if (!FuncName)
				continue;

			if(0 == strcmp(FuncName, NativeName))
			{
				USHORT Ordinal = pOrdinals[i];
				ULONG FuncRVA = pFunctionRVAs[Ordinal];
				
				if(p_ExportAddr)
					*p_ExportAddr = &pFunctionRVAs[Ordinal];

				if (pNativeSize)
				{
					ULONG j;
					ULONG MinRVA = MAXULONG;

					for (j = 0; j < pExportDir->NumberOfFunctions; j++)
					{
						ULONG CurrRVA = pFunctionRVAs[j];

						if (CurrRVA > FuncRVA && CurrRVA < MinRVA)
							MinRVA = CurrRVA;
					}

					*pNativeSize = MinRVA-FuncRVA;
				}

				return PTR_FROM_RVA(ImageBase, pNTHeader, FuncRVA);
			}
		}
	}
	__except(CheckException())
	{
		return NULL;
	}

	return NULL;
}

ULONG
GetNativeID (
	__in PVOID NativeBase,
	__in PSTR NativeName
	)
{
	PVOID NativeEP;
	PVOID paddr = 0;
	
	if (!NativeBase)
		return 0;

	NativeEP = GetExport(NativeBase, NativeName, &paddr, NULL);
	if(NativeEP)
	{
		if(((UCHAR*)NativeEP)[0] == 0xB8) // MOV EAX,XXXXXXXX?
			return ((ULONG*)((PCHAR)NativeEP+1))[0];
	}
	
	return 0;
}
