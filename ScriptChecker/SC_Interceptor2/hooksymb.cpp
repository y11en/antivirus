#include <windows.h>
#include "debug.h"
#include "hooksymb.h"
#include "../../windows/hook/r3hook/hookbase64.h"

extern BOOL MyIsBadReadPtr(CONST VOID *lp, UINT_PTR ucb);

PIMAGE_SECTION_HEADER GetSectionForOffset(HMODULE hModule, PVOID pOffset)
{
	int j;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS PEHeader;
	PIMAGE_SECTION_HEADER SectHeader;

	// Get Entry Point to module 
	pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		ODS(("VBS_EnumSEPs: MZ header not found"));
		return FALSE; // Does not point to a correct value
	}

	PEHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
	if(PEHeader->Signature != IMAGE_NT_SIGNATURE) {
		ODS(("VBS_EnumSEPs: PE header not found"));
		return FALSE; // It is not a PE header position
	}

	SectHeader=IMAGE_FIRST_SECTION(PEHeader);
	for(j=0;(j<PEHeader->FileHeader.NumberOfSections);j++)
	{
		PVOID pSectionStartAddr = (PVOID)((DWORD_PTR)SectHeader->VirtualAddress+(DWORD_PTR)hModule);
		PVOID pSectionEndAddr = (PVOID)((DWORD_PTR)pSectionStartAddr + (DWORD_PTR)SectHeader->Misc.VirtualSize);
		if (pOffset >= pSectionStartAddr && pOffset <= pSectionEndAddr)
		{
			// reference is offset in this section
			return SectHeader;
		}
		SectHeader++;
	}
	// not offset
	return NULL;
}

DWORD SetSymbolsHooks(HMODULE hModule, DWORD nHookCount, SYM_HOOK* pHooks)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS PEHeader;
	PIMAGE_SECTION_HEADER SectHeader;
	DWORD i;
	BYTE* ptr;

	DWORD dwHook;
	
	// Get Entry Point to module 
	pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		ODS(("VBS_EnumSEPs: MZ header not found"));
		return FALSE; // Does not point to a correct value
	}

	PEHeader=(PIMAGE_NT_HEADERS)((PCHAR)hModule+pDosHeader->e_lfanew);
	if(PEHeader->Signature != IMAGE_NT_SIGNATURE) {
		ODS(("VBS_EnumSEPs: PE header not found"));
		return FALSE; // It is not a PE header position
	}

	SectHeader=IMAGE_FIRST_SECTION(PEHeader);
	for(i=0;(i<PEHeader->FileHeader.NumberOfSections);i++)
	{
		if(!(SectHeader->Characteristics & (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE))) // Only data sections...
		{
			ODS(("SetSymbolsHooks: Searching in data section '%s'", SectHeader->Name));
			
			for (ptr=(BYTE*)((DWORD_PTR)SectHeader->VirtualAddress+4+(DWORD_PTR)hModule); ptr<(BYTE*)((DWORD_PTR)SectHeader->VirtualAddress + (DWORD_PTR)hModule + SectHeader->Misc.VirtualSize); ptr+=4)
			{
				PSYM pSYM = (PSYM)ptr;
				for (dwHook=0; dwHook<nHookCount; dwHook++)
				{
					if (pSYM->dwNameLen == pHooks[dwHook].dwNameLen)
					{
						if (pSYM->dwHash == pHooks[dwHook].dwHash || pHooks[dwHook].dwHash == 0)
						{
							// ensure all pointers are valid 
							if (MyIsBadReadPtr(pSYM->wcsName, (pHooks[dwHook].dwNameLen+1)*2)) 
								continue;
							
							if (wcscmp(pSYM->wcsName, pHooks[dwHook].wcsName) == 0)
							{
								ODS(("%08X: Hook #%d (%S) found", pSYM, dwHook, pSYM->wcsName));
								pHooks[dwHook].pSYM = pSYM;
							}
						}
					}
				}
			}
		}
		SectHeader++;
	}


	SectHeader=IMAGE_FIRST_SECTION(PEHeader);
	for(i=0;(i<PEHeader->FileHeader.NumberOfSections);i++)
	{ 
		// enum code section
		if(SectHeader->Characteristics & (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE)) // Only code sections...
		{
			ODS(("SetSymbolsHooks: Searching in code section '%s'", SectHeader->Name));
						
			PVOID pSectionStartAddr = (PVOID)((DWORD_PTR)SectHeader->VirtualAddress+(DWORD_PTR)hModule);
			PVOID pSectionEndAddr = (PVOID)((DWORD_PTR)pSectionStartAddr + (DWORD_PTR)SectHeader->Misc.VirtualSize);
			for (ptr=(BYTE*)(pSectionStartAddr); ptr<(BYTE*)(pSectionEndAddr); ptr++)
			{
				// search byte sequence in code
				if (*(WORD*)(ptr) == 0x45C7) // mov [ebp+?], offset ...
				{
					for (dwHook=0; dwHook<nHookCount; dwHook++)
					{
						if (*(PVOID*)(ptr+3) == pHooks[dwHook].pSYM && pHooks[dwHook].pSYM != NULL && pHooks[dwHook].pProc == NULL)
						{
							ODS(("%08X: Found reference to SYM of Hook #%d. Search for reference to function or SEP...", ptr, dwHook));
							BYTE* ptr2;
							for (ptr2=ptr+7; ptr2<ptr+0x100; ptr2++)
							{
								if (*(WORD*)(ptr2) == 0x45C7) // mov [ebp+?], offset ...
								{
									PVOID pRef = *(PVOID*)(ptr2+3);
									ODS(("%08X: Found some reference to function or SEP", pRef));
									PIMAGE_SECTION_HEADER OffsetSectHeader;
									OffsetSectHeader = GetSectionForOffset(hModule, pRef);
									if (OffsetSectHeader == NULL)
									{
										ODS(("%08X: pRef=%08X and is not offset - continue search", pRef, pRef));
										continue;
									}

									ODS(("%08X: Reference is offset in section '%s'", pRef, OffsetSectHeader->Name));
									
									if(OffsetSectHeader->Characteristics & (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE))
									{
										// reference points into code section, thus we found procedure for SYM
										pHooks[dwHook].pProc = pRef;
										pHooks[dwHook].pHookAddr = (PVOID)(ptr2+3);
									}
									else
									{
										// reference points into data section, SEP?
										// pointer is valid 
										PVBS_SEP pSEP = (PVBS_SEP)pRef;
										if (MyIsBadReadPtr(pSEP, sizeof(PVBS_SEP)) // SEP itself
											|| MyIsBadReadPtr(pSEP->pProc, 0x10) // procedure code
											|| MyIsBadReadPtr(pSEP->IStaticEntryPoint, sizeof(PVOID) // IStaticEntryPoint intreface table
											|| MyIsBadReadPtr(*(PVOID*)(pSEP->IStaticEntryPoint), 0x10))) // IStaticEntryPoint::QueryInterface code
										{
											ODS(("Pointer to SEP (%08X) is not valid", pSEP));
											continue;
										}
										
										OffsetSectHeader = GetSectionForOffset(hModule, pSEP->pProc);
										if (OffsetSectHeader == NULL && !(OffsetSectHeader->Characteristics & (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE))) // not code ???
										{
											ODS(("SEP procedure (%08X) is not in code section???", pSEP->pProc));
											continue;
										}
										
										OffsetSectHeader = GetSectionForOffset(hModule, *(PVOID*)(pSEP->IStaticEntryPoint));
										if (OffsetSectHeader == NULL && !(OffsetSectHeader->Characteristics & (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE))) // not code ???
										{
											ODS(("IStaticEntryPoint::QueryInterface (%08X) is not in code section???", pSEP->IStaticEntryPoint));
											continue;
										}

										// All tests for SEP are passed 
										pHooks[dwHook].pSEP = pSEP;
										pHooks[dwHook].pProc = pSEP->pProc;
										pHooks[dwHook].pHookAddr = &pSEP->pProc;
									}
									break;
								}
							}
						}
					}

				}
			}
		}
		SectHeader++;
	}

#ifdef _DEBUG	
	WCHAR wcsNotHooked[0x200] = L"";
#endif
	for (dwHook=0; dwHook<nHookCount; dwHook++)
	{
		if (pHooks[dwHook].pHookAddr == NULL)
		{
#ifdef _DEBUG	
			wcscat(wcsNotHooked, pHooks[dwHook].wcsName);
			wcscat(wcsNotHooked, L"\n  ");
#endif
			continue;
		}
		HookObjectMethodIfNotHookedBy(&pHooks[dwHook].pHookAddr, 0, pHooks[dwHook].pHookProc, NULL);
	}

#ifdef _DEBUG
	if (wcsNotHooked[0] != 0)
	{
		char errmsg[0x200];
		wsprintf(errmsg, "KAV Script Checker cannot find handlers for follows methods:\n  %S\nKAV Script Checker will continue work without some functionality. Please contact Kaspersky Lab. to fix this problem.", wcsNotHooked);
		ODS(("KAVSC cannot find handlers for follows methods:\n	%S\n", wcsNotHooked));
		//::MessageBox(NULL, errmsg, "KAV Script Checker", MB_ICONINFORMATION);
	}
#endif

	return NULL;
}

