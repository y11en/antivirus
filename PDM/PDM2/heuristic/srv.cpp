#include <assert.h>
#include <memory.h>

#include "srv.h"
#include "pe.h"
#include "trace.h"

#define min(a,b)    (((a) < (b)) ? (a) : (b))

static eExecutableType iIsFileFormatMZPE(cEnvironmentHelper* pEnvironmentHelper, cFile& file)
{
	uint8_t buff[0x1000];
	IMAGE_DOS_HEADER* pImageDosHeader = (IMAGE_DOS_HEADER*)buff;
	WORD* pWordBuffer = (WORD*)buff;
	uint32_t nBytes;
	uint32_t dwImageSize = 0;
	eExecutableType result = eNotExecutable;
	uint64_t nPos = 0;

	FILE_FORMAT_INFO* fi = &file.fi;

	memset(fi, 0, sizeof(FILE_FORMAT_INFO));
	
	fi->filesize = file.Size();

	if (!file.Read(nPos, buff, sizeof(buff), &nBytes))
	{
		PR_TRACE((g_root, prtERROR, TR "iIsFileFormatMZPE: ReadFile for file1 failed with err=%08X", GetLastError()));
		return eNotExecutable;
	}

	if (nBytes < sizeof(IMAGE_DOS_HEADER))
	{
		PR_TRACE((g_root, prtNOTIFY, TR "iIsFileFormatMZPE: nBytes(0x%x) < sizeof(IMAGE_DOS_HEADER)", nBytes));
		return eNotExecutable;
	}

	if( pImageDosHeader->e_magic!=0x4D5A/*'MZ'*/ && pImageDosHeader->e_magic!=0x5A4D /*'ZM'*/)
	{
		PR_TRACE((g_root, prtNOTIFY, TR "iIsFileFormatMZPE: pImageDosHeader->e_magic(%x) != 'MZ'", pImageDosHeader->e_magic));
		return eNotExecutable;
	}

	file.fi.format = eFormatMZ;
	result = eExecutableImage;
	fi->pe_offset = (unsigned)(pImageDosHeader->e_lfanew);

	{
		uint64_t dwHeaderOffset = nPos + (unsigned)(pImageDosHeader->e_lfanew);
		PIMAGE_NT_HEADERS pPE = (PIMAGE_NT_HEADERS)buff;
		PIMAGE_SECTION_HEADER pSH;
		WORD wSections,i;

		if (!file.Read(dwHeaderOffset, buff, sizeof(IMAGE_NT_HEADERS), &nBytes))
		{
			PR_TRACE((g_root, prtERROR, TR "iIsFileFormatMZPE: ReadFile for file1 failed with err=%08X", GetLastError()));
			return eNotExecutable;
		}

		if (nBytes < sizeof(IMAGE_NT_HEADERS))
		{
			PR_TRACE((g_root, prtERROR, TR "iIsFileFormatMZPE: nBytes(0x%x) < sizeof(IMAGE_NT_HEADERS)", nBytes));
			return eNotExecutable;
		}

		if (IMAGE_NT_SIGNATURE != pPE->Signature)
		{
			PR_TRACE((g_root, prtERROR, TR "iIsFileFormatMZPE: IMAGE_NT_SIGNATURE != pPE->Signature"));
			return eNotExecutable;
		}

		file.fi.format = eFormatPE;

		file.fi.timestamp = pPE->FileHeader.TimeDateStamp;
		file.fi.checksum = pPE->OptionalHeader.CheckSum;
		file.fi.entry_point = pPE->OptionalHeader.AddressOfEntryPoint;
		memset(file.fi.entry_point_data, 0, sizeof(file.fi.entry_point_data));
		file.Read(file.fi.entry_point, file.fi.entry_point_data, sizeof(file.fi.entry_point_data), 0);

		uint64_t dwSectionsOffset = dwHeaderOffset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + pPE->FileHeader.SizeOfOptionalHeader;

		PR_TRACE((g_root, prtNOTIFY, TR "iIsFileFormatMZPE: header offset 0x%x, section offset 0x%x", dwHeaderOffset, dwSectionsOffset));

		if (pPE->FileHeader.Characteristics & IMAGE_FILE_DLL)
			result = eExecutableLibrary;
		
		wSections = pPE->FileHeader.NumberOfSections;
		fi->sect_count = wSections;
		nBytes = 0;

		uint64_t CurrentOffset = dwSectionsOffset;
		
		for(i=0; i < wSections; i++)
		{
			if (nBytes < sizeof(IMAGE_SECTION_HEADER))
			{
				if (!file.Read(CurrentOffset, buff, min(wSections*sizeof(IMAGE_SECTION_HEADER), (sizeof(buff)/sizeof(IMAGE_SECTION_HEADER))*sizeof(IMAGE_SECTION_HEADER)), &nBytes))
				{
					PR_TRACE((g_root, prtERROR, TR "iIsFileFormatMZPE: ReadFile for file1 (IMAGE_SECTION_HEADER) failed with err=%08X", GetLastError()));
					return eNotExecutable;
				}
				
				CurrentOffset += nBytes;

				if (nBytes < sizeof(IMAGE_SECTION_HEADER))
				{
					PR_TRACE((g_root, prtERROR, TR "iIsFileFormatMZPE: nBytes(0x%x) < sizeof(IMAGE_SECTION_HEADER)", nBytes));
					return eNotExecutable;
				}
				pSH = (PIMAGE_SECTION_HEADER)buff;
			}

			//FIXME pSH->PointerToRawData need to be aligned to min(SECTOR_SIZE, pe->OptionalHeader.FileAligment) (only if subsystem not NATIVE)
			if(pSH->PointerToRawData + pSH->SizeOfRawData > dwImageSize)
				dwImageSize = pSH->PointerToRawData + pSH->SizeOfRawData;

			if (i<countof(fi->sections))
			{
				fi->sections[i].section_raw_offset = pSH->PointerToRawData;
				fi->sections[i].section_raw_size = pSH->SizeOfRawData;
				fi->sections[i].section_rva_offset = pSH->VirtualAddress;
				//fi->sections[i].section_rva_size = pSH->;
				fi->sections[i].characteristics = pSH->Characteristics;
			}
			
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "iIsFileFormatMZPE: nSize 0x%x %8.8s, praw 0x%x prawsize 0x%x (sum 0x%x)", 
				dwImageSize, pSH->Name, pSH->PointerToRawData, pSH->SizeOfRawData,
				pSH->PointerToRawData + pSH->SizeOfRawData));

			pSH++;
			nBytes -= sizeof(IMAGE_SECTION_HEADER);
		}
	}

	dwImageSize = min((uint32_t)fi->filesize, dwImageSize);
	fi->overlay_size = fi->filesize - dwImageSize;
	fi->image_size = dwImageSize;
	fi->executable_type = result;
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "iIsFileFormatMZPE: dwImageSize = 0x%x", dwImageSize));

	return result;
}

static const tcstring executable_ext[] = { _T("COM"), _T("EXE") }; // UPPERCASE!
static const tcstring script_ext[] = { _T("BAT"), _T("CMD"), _T("VBS"), _T("VBE"), _T("JS"), _T("JSE"), _T("WSF"), _T("WSH"), }; 

static eExecutableType iIsFileExecutableExt(cEnvironmentHelper* pEnvironmentHelper, cFile& file)
{
	size_t i;
	tcstring ext = file.getExtension();
	if (ext)
	{
		for (i=0; i<countof(executable_ext); i++)
		{
			if (0 == tstrcmp(ext, executable_ext[i]))
				return file.fi.executable_type = eExecutableImage;
		}
		if (0 == tstrcmp(ext, _T("DLL")))
		{
			return file.fi.executable_type = eExecutableLibrary;
		}
		for (i=0; i<countof(script_ext); i++)
		{
			if (0 == tstrcmp(ext, script_ext[i]))
			{
				file.fi.format = eFormatText;
				return file.fi.executable_type = eExecutableScript;
			}
		}
	}
	return file.fi.executable_type;
}

static bool iIsFileFormatOle2(cEnvironmentHelper* pEnvironmentHelper, cFile& file)
{
	BYTE filesig[8] = {0};
	const BYTE olesig[8] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 }; 
	if (!file.Read(0, filesig, 8, NULL))
		return false;
	if (0 == memcmp(olesig, filesig, 8))
	{
		file.fi.format = eFormatOLE;
		return true;
	}
	return false;
}

bool srvGetFileFormat(cEnvironmentHelper* pEnvironmentHelper, cFile& file)
{
	if (!file.fi.ext_analysed)
	{
		iIsFileExecutableExt(pEnvironmentHelper, file);
		file.fi.ext_analysed = true;
	}
	if (!file.fi.data_analysed && file.isReadable())
	{
		file.fi.data_analysed = true;
		if (eNotExecutable != iIsFileFormatMZPE(pEnvironmentHelper, file))
			return true;
		if (iIsFileFormatOle2(pEnvironmentHelper, file))
			return true;
	}
	return file.fi.format != eFormatUnknown;
}

bool srvIsFileExecutable(cEnvironmentHelper* pEnvironmentHelper, cFile& file)
{
	if (!srvGetFileFormat(pEnvironmentHelper, file))
		return false;
	return eNotExecutable != file.fi.executable_type;
}


bool srvIsImageCopyRaw(cEnvironmentHelper* pEnvironmentHelper, cFile& src_file, uint64_t src_offset, cFile& dst_file, uint64_t dst_offset, uint64_t size)
{
#define BUFF_SIZE 0x1000
	int8_t bBuff1[BUFF_SIZE], bBuff2[BUFF_SIZE];
	uint32_t nRead;
	uint32_t nBytes1, nBytes2;

	while (size) 
	{
		nRead = (uint32_t)min(BUFF_SIZE, size); 

		src_file.Read(src_offset, bBuff1, nRead, &nBytes1);
		dst_file.Read(dst_offset, bBuff2, nRead, &nBytes2);

		if (nBytes1 != nRead || nBytes2 != nRead)
		{
			PR_TRACE((g_root, prtERROR, TR "IsImageCopy: Data size mismatch (0x%x with 0x%x). Current size 0x%x", 
				nBytes1, nBytes2, nSize));
			return false;
		}
		if (memcmp(bBuff1, bBuff2, nRead) != 0)
		{
			PR_TRACE((g_root, prtNOTIFY, TR "IsImageCopy: Data mismatch"));
			return false;
		}
		size -= nRead;
		src_offset += nRead;
		dst_offset += nRead;
	};

	PR_TRACE((g_root, prtIMPORTANT, TR "IsImageCopyRaw MATCHED: %S:%X %S:%X size:%X", src_file.getFull(), src_offset, dst_file.getFull(), dst_offset, size));
	return true;
}

bool srvIsImageCopy(cEnvironmentHelper* pEnvironmentHelper, cFile& src_file, cFile& dst_file)
{
	if (!dst_file.isExecutable())
		return false;
	if (!src_file.isExecutable())
		return false;
	
	if (src_file.fi.format != dst_file.fi.format)
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "IsImageCopy: File formats mismatch (%X!=%X)", src_file.fi.format, dst_file.fi.format));
		return false;
	}

	switch(src_file.fi.format)
	{
	case eFormatPE:
		if (src_file.fi.image_size != dst_file.fi.image_size || src_file.fi.image_size == 0)
		{
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "IsImageCopy: Images size mismatch (%X!=%X)", src_efi->image_size, dst_efi->image_size));
			return false;
		}

		return srvIsImageCopyRaw(pEnvironmentHelper, 
			src_file, 
			src_file.fi.pe_offset, 
			dst_file, 
			dst_file.fi.pe_offset, 
			src_file.fi.image_size - src_file.fi.pe_offset);
	default:
		{
			uint64_t nSrcSize = src_file.Size();
			uint64_t nDstSize = dst_file.Size();
			if (nSrcSize != nDstSize || nSrcSize == 0)
			{
				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "IsImageCopy: Images size mismatch (%X!=%X)", nSrcSize, nDstSize));
				return false;
			}
			return srvIsImageCopyRaw(pEnvironmentHelper, src_file, 0, dst_file, 0, nSrcSize);
		}
	}
	return false;
}

//#define IS_DELIM(wc) (wc <= ' ' || wc == ',')
#define IS_DELIM(wc, bCMD) ((wc) <= ' ' || (wc) == ',' || ((bCMD) && ((wc) == '>' || (wc) == '<' || (wc) == '|')))
#define NEXT_CHAR() (tstrchar(pwchPtr = tstrinc(pwchPtr))) // *(++pwchPtr)
// !! this function modifies data pointed with sCmdLine parameter!
size_t srvParseCmdLine(cEnvironmentHelper* pEnvironmentHelper, tstring sCmdLine, tcstring sDefaultDir, tcstring* params, size_t max_params_count, bool bInterpreteur)
{
	assert((params && max_params_count) || (!params && !max_params_count));
	bool bCMD = bInterpreteur;
	size_t params_count = 0;
	tstring pwchPtr = sCmdLine;
	tstring param;
	tchar wc;
	if (sCmdLine == NULL)
		return 0;
	if (!params && !max_params_count)
		max_params_count = SIZE_MAX;
	wc = tstrchar(pwchPtr);
	while (wc>0 && wc<=' ')
		wc = NEXT_CHAR();
	sCmdLine = pwchPtr;
	if (wc != '\"') // first param not quoted
	{
		wc = tstrchar(pwchPtr);
		while (wc)
		{
			// find delimiter
			while (wc != 0 && !IS_DELIM(wc, bCMD))
				wc = NEXT_CHAR();
			tstrchar(pwchPtr) = 0;
			tstring file = pEnvironmentHelper->PathFindExecutable(sCmdLine, sDefaultDir); // check long file name without quotes
			tstrchar(pwchPtr) = wc;
			if (file)
			{
				if (params)
				{
					params[0] = sCmdLine;
					tstrchar(pwchPtr) = 0;
				}
				params_count++;
				sCmdLine = pwchPtr;
				if (wc) // we have more args
					sCmdLine++;
				tstrfree(file);
				break;
			}
			if (wc)
				wc = NEXT_CHAR();
		}
	}
	pwchPtr = sCmdLine;
	while (params_count < max_params_count)
	{
		wc = tstrchar(pwchPtr);
		
		// skip delimiters at start
		while (wc != 0 && IS_DELIM(wc, bCMD))
			wc = NEXT_CHAR();
		
		if (wc == 0) // end reached
			break; 

		if (wc == '\"') // quoted param
		{
			wc = NEXT_CHAR();
			if (bCMD && wc == '\"')
				wc = NEXT_CHAR();
			
			if (wc == 0) // end reached
				break;

			param = pwchPtr;
			while (wc != 0 && wc != '\"')
				wc = NEXT_CHAR();
		}
		else
		{
			//if (wc != '/') // if not switch
			param = pwchPtr;
			// modified by Sobko was: while (wc != 0 && !IS_DELIM(wc) && wc != '\"')
			while (wc != 0 && !IS_DELIM(wc, bCMD) && wc != '\"')
				wc = NEXT_CHAR();
		}
		if (param != pwchPtr)
		{
			if (params)
			{
				params[params_count] = param;
				tstrchar(pwchPtr) = 0; // zero terminate parameter
			}
			if (params_count == 0)
			{
				tchar c = tstrchar(pwchPtr);
				tstrchar(pwchPtr) = 0; // zero terminate parameter
				if (tstring file = pEnvironmentHelper->PathFindExecutable(param, sDefaultDir)) // check long file name without quotes
				{
					if (srvComparePath(pEnvironmentHelper, file, _T("%ComSpec%")))
						bCMD = true;
					tstrfree(file);
				}
				tstrchar(pwchPtr) = c; // restore terminate parameter
			}
			params_count++;
		}

		if (wc == 0) // end reached
			break;
		
		wc = NEXT_CHAR();
	}

	return params_count;
}
#undef IS_DELIM
#undef NEXT_CHAR

bool srvComparePath(cEnvironmentHelper* pEnvironmentHelper, tcstring sPath1, tcstring sPath2, bool bCompareImages)
{
	cFile file1(pEnvironmentHelper, sPath1);
	cFile file2(pEnvironmentHelper, sPath2);
	if (0 == tstrcmp(file1.getFull(), file2.getFull()))
		return true;
	if (bCompareImages)
	{
		if (srvIsImageCopy(pEnvironmentHelper, file1, file2))
			return true;
	}
	return false;
}

// ------------------------------------------------------------------------------

enum tSysFileLocation
{
	eRoot,
	eWindows,
	eSystem32,
	eProgramFiles,
	eProgramFilesIE
};

static const tcstring arrSysLocations[] = 
{
	_T("%SYSTEMDRIVE%"),
	_T("%SYSTEMROOT%"),
	_T("%SYSTEMROOT%\\SYSTEM32"),
	_T("%PROGRAMFILES%"),
	_T("%PROGRAMFILES%\\INTERNET EXPLORER"),
};

struct tSystemFile
{
	tcstring filename;
	tSysFileLocation location;
};

static const tSystemFile arrSystemFiles[] =
{
	{ _T("BOOT.INI"), eRoot },
	{ _T("IO.SYS"), eRoot },
	{ _T("MSDOS.SYS"), eRoot },
	{ _T("NTDETECT.COM"), eRoot },
	{ _T("NTLDR"), eRoot },

	{ _T("EXPLORER.EXE"), eWindows},
	{ _T("ALG.EXE"), eSystem32 },
	{ _T("TASKMGR.EXE"), eSystem32 },
	{ _T("SVCHOST.EXE"), eSystem32 },
	{ _T("SMSS.EXE"), eSystem32 },
	{ _T("SPOOLSV.EXE"), eSystem32 },
	{ _T("CSRSS.EXE"), eSystem32 },
	{ _T("WINLOGON.EXE"), eSystem32 },
	{ _T("SERVICES.EXE"), eSystem32 },
	{ _T("LSASS.EXE"), eSystem32 },
	{ _T("WUAUCLT.EXE"), eSystem32 },
	{ _T("DLLHOST.EXE"), eSystem32 },
	{ _T("RUNDLL32.EXE"), eSystem32 },
	{ _T("IEXPLORE.EXE"), eProgramFilesIE },

	{ _T("NTDLL.DLL"), eSystem32 },
	{ _T("KERNEL32.DLL"), eSystem32 },
	{ _T("USER32.DLL"), eSystem32 },
	{ _T("GDI32.DLL"), eSystem32 },
	{ _T("ADVAPI32.DLL"), eSystem32 },
	{ _T("SHELL32.DLL"), eSystem32 },
	{ _T("WS2_32.DLL"), eSystem32 },
	{ _T("MSWSOCK.DLL"), eSystem32 },
	{ _T("WSOCK32.DLL"), eSystem32 },
	{ _T("SHLWAPI.DLL"), eSystem32 },
	{ _T("OLE32.DLL"), eSystem32 },
	{ _T("OLEAUT32.DLL"), eSystem32 },
	{ _T("RASAPI32.DLL"), eSystem32 },
	{ _T("URLMON.DLL"), eSystem32 },
	{ _T("WININET.DLL"), eSystem32 },
	{ _T("MSVCRT.DLL"), eSystem32 },
	{ _T("RPCRT4.DLL"), eSystem32 },
	{ _T("CRTDLL.DLL"), eSystem32 },
	{ _T("DNSAPI.DLL"), eSystem32 },
	{ _T("COMCTL32.DLL"), eSystem32 },
	{ _T("VERSION.DLL"), eSystem32 },
	{ _T("MPR.DLL"), eSystem32 },
	{ _T("WINMM.DLL"), eSystem32 },
	{ _T("IMAGEHLP.DLL"), eSystem32 },
	{ _T("AVICAP32.DLL"), eSystem32 },
	{ _T("MSACM32.DLL"), eSystem32 },
	{ _T("SHDOCVW.DLL"), eSystem32 },
	{ _T("PSAPI.DLL"), eSystem32 },
	{ _T("SHFOLDER.DLL"), eSystem32 },
	{ _T("PSTOREC.DLL"), eSystem32 },
	{ _T("CRYPT32.DLL"), eSystem32 },
	{ _T("ICMP.DLL"), eSystem32 },
	{ _T("IPHLPAPI.DLL"), eSystem32 },
	{ _T("ODBC32.DLL"), eSystem32 },
	{ _T("NETAPI32.DLL"), eSystem32 },
	{ _T("COMDLG32.DLL"), eSystem32 },
	{ _T("WINSPOOL.DRV"), eSystem32 },
	{ _T("SNMPAPI.DLL"), eSystem32 },
	{ _T("INETMIB1.DLL"), eSystem32 },
	{ _T("QMGR.DLL"), eSystem32 },
};

bool srvIsWinSystemFile( cEnvironmentHelper* pEnvironmentHelper, cFile &file, bool* pbSystemName,  bool* pbSystemPath )
{
	tcstring sFullName = file.getFull();
	tcstring sFileName = file.getFilename();
	tstring  sLocation = NULL;
	bool bResult = false;
	if (pbSystemName)
		*pbSystemName = false;
	if (pbSystemPath)
		*pbSystemPath = false;
	for (unsigned i = 0; i < countof(arrSystemFiles); i++)
	{
		if (0 != tstrcmp(arrSystemFiles[i].filename, sFileName))
			continue;

		sLocation = pEnvironmentHelper->ExpandEnvironmentStr(arrSysLocations[arrSystemFiles[i].location]);
		if (!sLocation)
			break;
		if (tstrchar(sLocation) == _T('%') || tstrchar(sLocation) == 0) // expand failed
			break;
		if (sLocation[1] != ':')
			break;
		tstrupr(sLocation);
		if (pbSystemName)
			*pbSystemName = true;
		if (!tstrcmpinc(&sFullName, sLocation))
			break;
		while (tstrchar(sFullName) == '\\')
			sFullName = tstrinc(sFullName);
		if (sFullName != sFileName)
			break;
		if (pbSystemPath)
			*pbSystemPath = true;
		bResult = true;
		break;
	}
	if (sLocation)
		tstrfree(sLocation);
	return bResult;
}

