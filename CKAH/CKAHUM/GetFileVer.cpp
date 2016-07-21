#include "stdafx.h"
#include "GetFileVer.h"
#include "hook/kl1_api.h"

bool read_buffer(HANDLE p_hFile, DWORD p_dwOffset, LPVOID p_pBuffer, DWORD p_dwBufferSize)
{
	if (::SetFilePointer(p_hFile, p_dwOffset, NULL, FILE_BEGIN) != p_dwOffset)
	{
		return false ;
	}

	DWORD dwReadSize = 0 ;
	BOOL blResult = ::ReadFile(p_hFile, p_pBuffer, p_dwBufferSize, &dwReadSize, NULL) ;

    extern int g_bUseKL1;
    if (g_bUseKL1)
    {
        // unXOR file before getting file version
        unsigned char KL1Key[] = KL1_KEY;
        for (DWORD i = 0; i < dwReadSize; ++i)
        {
            ((char*)p_pBuffer)[i] ^= KL1Key [ (i + p_dwOffset) % KL1_KEY_SIZE ];
        }
    }

	return blResult && (p_dwBufferSize == dwReadSize) ;
}

bool get_file_header(HANDLE p_hFile, IMAGE_FILE_HEADER& p_Header, LONG& p_rOffset)
{
	IMAGE_DOS_HEADER DosHeader ;
	if (!read_buffer(p_hFile, 0, &DosHeader, sizeof(DosHeader)))
	{
		return false ;
	}

	if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		return false ;
	}

	if (DosHeader.e_lfanew < 0)
	{
		return false ;
	}

	IMAGE_NT_HEADERS32 PE ;
	if (!read_buffer(p_hFile, DosHeader.e_lfanew, &PE, sizeof(PE)))
	{
		return false ;
	}

	if (PE.Signature != IMAGE_NT_SIGNATURE)
	{
		return false ;
	}

	p_Header  = PE.FileHeader ;
	p_rOffset = DosHeader.e_lfanew ;
	return true ;
}

bool get_version_from_dir(HANDLE p_hFile,
						  IMAGE_RESOURCE_DIRECTORY& p_ResDir,
						  DWORD p_dwOffset,
						  DWORD p_dwPointerToSectionRawData,
						  DWORD p_dwSectionSize,
						  LPVOID p_pBuff,
						  DWORD lBuffSize,
						  LPDWORD p_lResBuffSize,
						  bool p_blFirstIteration = true)
{
	IMAGE_RESOURCE_DIRECTORY_ENTRY Entry ;
	DWORD dwEntryCount =  p_ResDir.NumberOfIdEntries + p_ResDir.NumberOfNamedEntries ;
	for (UINT unIndex = 0; unIndex < dwEntryCount; ++unIndex)
	{
		if (!read_buffer(p_hFile, p_dwOffset, &Entry, sizeof(Entry)))
		{
			return false ;
		}

		if (Entry.DataIsDirectory)
		{
			if ((p_blFirstIteration && MAKEINTRESOURCE(Entry.Id) == RT_VERSION) || !p_blFirstIteration)
			{
				IMAGE_RESOURCE_DIRECTORY ResSubdir ;
				if (!read_buffer(p_hFile, p_dwPointerToSectionRawData + Entry.OffsetToDirectory, &ResSubdir, sizeof(ResSubdir)))
				{
					return false ;
				}

				if (!get_version_from_dir(p_hFile,
											ResSubdir,
											p_dwPointerToSectionRawData + Entry.OffsetToDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY),
											p_dwPointerToSectionRawData,
											p_dwSectionSize,
											p_pBuff,
											lBuffSize,
											p_lResBuffSize,
											false))
				{
					return false ;
				}
			}
		}
		else
		{
			IMAGE_RESOURCE_DATA_ENTRY DataEntry ;
			if (!read_buffer(p_hFile, p_dwPointerToSectionRawData + Entry.OffsetToData, &DataEntry, sizeof(DataEntry)))
			{
				return false ;
			}

			if(p_lResBuffSize)
				*p_lResBuffSize = DataEntry.Size;

			if(p_pBuff)
			{
				if(lBuffSize < DataEntry.Size)
					return false;

				memset(p_pBuff, 0, DataEntry.Size) ;

				if (!read_buffer(p_hFile, p_dwPointerToSectionRawData + DataEntry.OffsetToData - p_dwSectionSize, p_pBuff, DataEntry.Size))
				{
					return false ;
				}
			}
			return true ;
		}
		p_dwOffset += sizeof(Entry) ;
	}

	return true ;
}

bool get_version(HANDLE p_hFile, LPVOID p_pBuff, DWORD lBuffSize, LPDWORD p_lResBuffSize)
{
	IMAGE_FILE_HEADER FH ;
	LONG lOffset = 0 ;
	if (!get_file_header(p_hFile, FH, lOffset))
	{
		return false ;
	}

	bool bIs32 = FH.SizeOfOptionalHeader == IMAGE_SIZEOF_NT_OPTIONAL32_HEADER;
	bool bIs64 = FH.SizeOfOptionalHeader == IMAGE_SIZEOF_NT_OPTIONAL64_HEADER;

	IMAGE_NT_HEADERS32 PE32 ;
	IMAGE_NT_HEADERS64 PE64 ;

	if (!bIs32 && !bIs64)
	{
		return false ;
	}

	if(bIs32 && !read_buffer(p_hFile, lOffset, &PE32, sizeof(PE32)))
		return false;
	if(bIs64 && !read_buffer(p_hFile, lOffset, &PE64, sizeof(PE64)))
		return false;

	if (bIs32 && PE32.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ||
		bIs64 && PE64.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		return false ;
	}

	if(bIs32)
		lOffset += FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader) + FH.SizeOfOptionalHeader ;
	else
		lOffset += FIELD_OFFSET(IMAGE_NT_HEADERS64, OptionalHeader) + FH.SizeOfOptionalHeader ;

	IMAGE_SECTION_HEADER SH ;
	for (UINT unIndex = 0; unIndex < FH.NumberOfSections; ++unIndex)
	{
		if (!read_buffer(p_hFile, lOffset, &SH, sizeof(SH)))
		{
			return false ;
		}

		if (bIs32 && SH.VirtualAddress == PE32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress ||
			bIs64 && SH.VirtualAddress == PE64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress)
		{
			IMAGE_RESOURCE_DIRECTORY ResDir ;
			if (!read_buffer(p_hFile, SH.PointerToRawData, &ResDir, sizeof(ResDir)))
			{
				return false ;
			}

			LONG lEntryOffset = SH.PointerToRawData + sizeof(IMAGE_RESOURCE_DIRECTORY) ;
			if (!get_version_from_dir(p_hFile,
										ResDir,
										SH.PointerToRawData + sizeof(IMAGE_RESOURCE_DIRECTORY),
										SH.PointerToRawData,
										SH.VirtualAddress,
										p_pBuff,
										lBuffSize,
										p_lResBuffSize))
			{
				return false ;
			}
		}

		lOffset += sizeof(IMAGE_SECTION_HEADER) ;
	}

	return true;
}

DWORD _GetFileVersionInfoSizeW(LPCWSTR lptstrFilename)
{
	if(!g_bIsNT)
		return 0;
	HANDLE hFile = CreateFileW(lptstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	DWORD ResBuffSize = 0;
	bool bGetVerResult = get_version(hFile, NULL, 0, &ResBuffSize);
	DWORD dwLastErr = GetLastError();
	CloseHandle(hFile);
	SetLastError(dwLastErr);
	return bGetVerResult ? ResBuffSize : 0;
}

BOOL _GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwLen, LPVOID lpData)
{
	if(!g_bIsNT)
		return 0;
	HANDLE hFile = CreateFileW(lptstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	bool bGetVerResult = get_version(hFile, lpData, dwLen, NULL);
	DWORD dwLastErr = GetLastError();
	CloseHandle(hFile);
	SetLastError(dwLastErr);
	return bGetVerResult != false;
}