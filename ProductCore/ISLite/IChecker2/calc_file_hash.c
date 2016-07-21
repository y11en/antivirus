/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyschik
  Revised on	19.05.2004 13:13:00
  Comments:	
	Убран вызов GetFileAttributesExW и использование exception handling.
	Модуль стал конвертируемым и бинарно-переносимым.
 ************************************/

 /************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyschik
  Revised on	15.12.2003 19:02:00
  Comments:	
	Изменен подсчет OLE документов, ранее обсчитывалась только Root Entry, теперь - весь directory stream.
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyschik
  Revised on	17.10.2003 16:22:00
  Comments:	
	Исправлена ошибка -	возникающая при знаковом сравнении e_lfanew
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Andrew Rubin
  Revised on	04.06.2003 16:18:17
  Comments:	
    Добавлена поддержка сборок __unix__ и _NO_PRAGUE_
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	28.05.2003 15:34:35
  Comments:	
    Добавлена обработка OLE2 файлов
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	18.03.2003 15:25:00
  Comments:	
    В результирующий хеш теперь включается и имя файла, чтобы
    избежать возможных пропусков при переименовании файлов.
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Andy Nikishin
  Revised on	06.12.2002 10:50
  Comments:	
		Добавлена работа с ITSS 4.1 (*.CH?)
		Добавлена работа с архивами ZIP (*.ZIP)
		Добавлена работа с архивами RAR (*.RAR)
************************************/

/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	27.11.2002 22:22:41
  Comments:	
	Улучшена проверка в IsFileFormatINI - начало файла проверяется на "бинарность"
	На всех чтениях замеряется время
	Исправлена ошибка в IsFileFormatEXE, из-за которой не обсчитывались relo-таблицы
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	26.11.2002 20:00:00
  Comments:	
	Добавлен timing чтения первых 4Кб
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By:	Mike Pavlyushchik
  Revised on	19.10.2002 17:51:03
  Comments:	
	Изменен прототип метода GetStatus: добавлены параметры pData, dwDataSize
	Добавлены методы FlushStatusDB(), ResetStatusDB()
	Удалены методы SetGarbageCollectionStatus(tBOOL), DoGarbageCollection()
	Удалено свойство plFLUSH
 ************************************/

#if defined(__unix__) || defined(_NO_PRAGUE_)
#define _ICHECKER_NO_MEMIMAGE_
#endif

#include <wincompat.h>
#if defined (__unix__)
typedef unsigned long ULONG;
#endif

#if !defined(_NO_PRAGUE_)
#include <Prague/prague.h>
#include "ichecker2.h"
#include <ProductCore/iface/i_sfdb.h>
#include <Prague/iface/i_hash.h>
#include "../sfdb/sfdb.h"

#include "Prague/pr_oid.h"
#if !defined(__unix__)
#include "../../../extract/memory/memmodscan/io.h"
#endif
#include "../../../prague/hash/md5/plugin_hash_md5.h"

#else // _NO_PRAGUE_
#include "npr.h"
#include "npr_ichecker2.h"
#include <sys/stat.h>
#include <string.h>
#endif // _NO_PRAGUE_

#include "npr_hash.h"
#include "npr_io.h"
#include "npr_mem.h"

//#include "elf.h"
#include "entr_elf.h"
#include "newexe.h"
//#include "pehdr.h"
#include "itss.h"
#include "zip.h"
#include "rar.h"
#include "ole2.h"
#include "calc_file_hash.h"
#include "pe3264.h"

#include "forcedbgout.h"


/*#if !defined(__unix__) && !defined(_NO_PRAGUE_)
#define _CALC_ICHECKER_READ_TIME_
#endif*/

#if defined(_CALC_ICHECKER_READ_TIME_)

#include "../avpgs/timestats.h"

#define CALL_IO_SeekReadTimed(io, pdwResult, qwOffset, pBuffer, dwSize) ((exStatisticEnabled!= 0 && exStatisticEnabled()) ? iSeekReadTimed(io, pdwResult, qwOffset, pBuffer, dwSize) : CALL_IO_SeekRead(io, pdwResult, qwOffset, pBuffer, dwSize))
tERROR iSeekReadTimed(hIO io, tDWORD* pdwResult, tQWORD qwOffset, tPTR pBuffer, tDWORD dwSize)
{
	hOBJECT hIoParent;
	tERROR error;

	hIoParent = CALL_SYS_ParentGet(io, IID_ANY);
	if (PR_FAIL(error=CALL_SYS_ObjectCheck(io,hIoParent,IID_STRING,PID_ANY,0,0)))
		return CALL_IO_SeekRead(io, pdwResult, qwOffset, pBuffer, dwSize);
	
	exStatisticTimingStart(hIoParent, cSTAT_ICHECKER_READ);
	error = CALL_IO_SeekRead(io, pdwResult, qwOffset, pBuffer, dwSize);
	exStatisticTimingStop(hIoParent, cSTAT_ICHECKER_READ);
	
	return error;
}

#else // _CALC_ICHECKER_READ_TIME_

#define CALL_IO_SeekReadTimed CALL_IO_SeekRead

#endif // _CALC_ICHECKER_READ_TIME_





tERROR CalcFileHash(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, hIO io, hHASH hHash);
tERROR CalcFileHashStrepped(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, tDWORD dwStripSize, tDWORD dwPartOfStreepToCheck, hIO io, hHASH hHash);
tERROR CalcFileAttributesIntoHash(hIO io, hHASH hHash);

tBOOL IsFileFormatTTF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatLNK(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatINF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatINI(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatCOM(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatELF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatEXE(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatSYS(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatITSS41(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatZIP(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatRAR(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL IsFileFormatOLE(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);
tBOOL CalcArchiveHash(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash);

#define npKAVI_HASH_AND_DATA   ((tSTRING)("npKAVI_HASH_AND_DATA"))
//static tPROPID propid_hash;
const char szContent[] = "MSCompressed/Content";
const char szControlData[] = "MSCompressed/ControlData";
const char szResetTable[] = "MSCompressed/Transform/{7FC28940-9D31-11D0-9B27-00A0C91E9C7C}/InstanceData/ResetTable";


// GetFileFormat
// Count CRC

// The TrueType font file begins at byte 0 with the Offset Table.
typedef struct tag_TTF_OFFSET_TABLE {
	FIXED	sfnt_version;	// 0x00010000 for version 1.0.
	USHORT 	numTables;	// Number of tables. 
	USHORT 	searchRange;	// (Maximum power of 2   numTables) x 16.
	USHORT 	entrySelector;	// Log2(maximum power of 2   numTables).
	USHORT 	rangeShift;	// NumTables x 16-searchRange.
} TTF_OFFSET_TABLE;

// This is followed at byte 12 by the Table Directory entries. Entries in the Table Directory must be sorted in ascending order by tag.
typedef struct tag_TTF_TABLE_DIRECTORY_ENTRY {
	ULONG	tag;	// 4-byte identifier.
	ULONG	checkSum; // CheckSum for this table.
	ULONG	offset;	// Offset from beginning of TrueType font file.
	ULONG	length;	// Length of this table.
} TTF_TABLE_DIRECTORY_ENTRY;

#define BUFFERSIZE 512
#define BIGBUFFERSIZE BUFFERSIZE*8


#if defined (SPARC)
#define MAKE_WORD(a) (a)
#define MAKE_DWORD(a, b) (((DWORD)(b))|((DWORD)(a)<<16))
#elif defined (_WIN32) || defined (__i386__)
#define SWAP_BYTES(a) (((a&0x00FF)<<8)|((a&0xFF00)>>8))
#define MAKE_WORD(a) (SWAP_BYTES(a))
#define MAKE_DWORD(a, b) (((DWORD)SWAP_BYTES(a))|((DWORD)SWAP_BYTES(b)<<16))
#define ReadDWordPtr(x) (*((tDWORD*)(x)))
#define ReadWordPtr(x) (*((tWORD*)(x)))
#define DW ReadDWordPtr
#define W ReadWordPtr
#else
#define MAKE_WORD(a) (a)
#define MAKE_DWORD(a, b) (((DWORD)(a))|((DWORD)(b)<<16))
#define DW(x) (*((DWORD*)(x)))
#define W(x) (*((WORD*)(x)))
#endif


long __inline A20(tWORD Segment,tWORD Offset);

void __inline check(tPTR buffer, tDWORD Part, hHASH hHash)
{
	CALL_Hash_Update(hHash, buffer, Part);
	return ;
}

tERROR Finalize(tBYTE* pHashBuff, hHASH hHash)
{
	tERROR error;
	char md5hash[16];
	unsigned long ulHashSize = sizeof(md5hash);
	unsigned __int64 iHash;

	if (PR_FAIL(error = CALL_Hash_GetHash(hHash, &md5hash[0], ulHashSize)))
		return error;

	iHash = *(unsigned __int64*)&md5hash[0];
	iHash ^= *(unsigned __int64*)&md5hash[8];
	*(unsigned __int64*)pHashBuff = iHash;
	CALL_Hash_Close(hHash);
	
	return errOK;
}

#define _CLCASE(c) (c>='A' && c<='Z' ? c | 0x20 : c )
int __stricmp(tBYTE* str1, tBYTE* str2)
{
	while (*str1 != 0 && *str2 != 0 && _CLCASE(*str1) == _CLCASE(*str2))
	{
		str1++;
		str2++;
	}
	if (_CLCASE(*str1) < _CLCASE(*str2))
		return -1;
	if (_CLCASE(*str1) > _CLCASE(*str2))
		return 1;
	return 0;
}

#define _CLCASE(c) (c>='A' && c<='Z' ? c | 0x20 : c )
tBYTE* __strlwr(tBYTE* str)
{
	while (*str != 0)
	{
		*str = _CLCASE(*str);
		str++;
	}
	return str;
}

tERROR GetFileHash(hOBJECT object, tBYTE* pHashBuff, tDWORD dwHashBuffSize, tBYTE* pAdditionalData, tDWORD dwAdditionalDataSize, tDWORD dwFlags)
{
tDWORD How;
tERROR error;
tDWORD dwFileSize;
tQWORD wqTmp = 0;
tBYTE buffer[BIGBUFFERSIZE*2]; // Don't make it smaller!
hHASH hHash;
tCHAR szFileName[0x300];
tDWORD dwFNSize;
tIID  iid;
hIO   io = NULL;

	if (object == NULL)
		return errPARAMETER_INVALID;
	if (dwHashBuffSize != 8)
		return errPARAMETER_INVALID;

	iid = CALL_SYS_PropertyGetDWord(object, pgINTERFACE_ID);

	switch (iid)
	{
	case IID_OS:
		error = CALL_SYS_PropertyGet(object, NULL, pgOBJECT_HASH, pHashBuff, dwHashBuffSize);
		if( PR_SUCC(error) && !*(tQWORD*)pHashBuff )
			error = errOBJECT_INCOMPATIBLE;
		return error;
	case IID_IO:
		io = (hIO)object;
		// see below
		break;
	default:
		return errOBJECT_INCOMPATIBLE;
	}

	error = CALL_IO_GetSize(io, &wqTmp, IO_SIZE_TYPE_EXPLICIT);
	if(PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "ichk\tError getting file size err=%08X", error));
		return error;
	}
	if(*(((tDWORD*)&wqTmp)+1)) // file size > 4Gb
		return errMODULE_UNKNOWN_FORMAT;
	
	dwFileSize = (tDWORD)wqTmp;

	if (dwFileSize == 0) // file size is zero
	{
		PR_TRACE(( io, prtERROR, "ichk\tFile size == %d", dwFileSize));
		*(unsigned __int64*)pHashBuff = 0;
		return errOK;
	}

	if(PR_FAIL(error = CALL_IO_SeekReadTimed((hIO)io, &How, 0, buffer, BIGBUFFERSIZE)))
	{
		PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
		return error;
	}

	if (PR_FAIL(error = CALL_Hash_Create(io, &hHash)))
		return error;

	// calculate additional data into hash
	if (pAdditionalData != NULL && dwAdditionalDataSize != 0)
		check(pAdditionalData, dwAdditionalDataSize, hHash);

	check(&dwFileSize, sizeof(dwFileSize), hHash); // add file size!
	
	// calculate file name into hash 
	// added to avoid security breach with following scenario:
	//   1. Add exlude mask for "*.ex"
	//   2. Scan file "virus.ex"
	//         Result is "ok" and the same verdict stored into SFDB
	//   3. Rename "virus.ex" into "virus.exe"
	//   4. Scan "virus.exe"
	//         The file has proper checksum in SFDB and AV check will be skipped.
	if (PR_SUCC(CALL_SYS_PropertyGetStr(io, &dwFNSize, pgOBJECT_NAME, szFileName, sizeof(szFileName)-2, cCP_ANSI)))
	{
		szFileName[dwFNSize] = 0; // ensure zero-terminated
		__strlwr(szFileName);
		check(szFileName, dwFNSize, hHash);
	}
	else
		szFileName[0] = 0;

	if((W(buffer) == 0x5A4D /*'ZM'*/ || W(buffer) == 0x4D5A/*'MZ'*/) && IsFileFormatEXE(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);

	if (DW(buffer) == 0x0000004C && IsFileFormatLNK(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);
	
	if (DW(buffer) == 0x00000100 && IsFileFormatTTF(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);
	
	if (DW(buffer) == 0x464C457Fl && IsFileFormatELF(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);
	
	if (dwFileSize >= 0x1000 && (DW(buffer) == (OLE_SIGN1) || DW(buffer) == (OLE_SIGN2)) && IsFileFormatOLE(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);
	
	if (dwFileSize <= 0xFFFF && W(buffer+2)==0xFFFF && IsFileFormatSYS(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);

//	if ((dwFlags & cICHECKER_ARCHIVES_OFF) == 0) 
	{
		//if (DW(buffer) == (ITSFILE_HEADER_MAGIC) && IsFileFormatITSS41(buffer, How, dwFileSize, io, hHash))
		if (DW(buffer) == (ITSFILE_HEADER_MAGIC) && CalcArchiveHash(buffer, How, dwFileSize, io, hHash))
			return Finalize(pHashBuff, hHash);

// ---------------------------------------------
// 28.01.2005 Mike Pavlyushchik
// IsFileFormatZIP and IsFileFormatRAR are very slow, because in fact read most of file from disk.
// Time overhit is very high. So, after consulting with Eugene Kaspersky decided to calculate beginning 
// of the file plus file size plus file timestamps.
/*
		if (DW(buffer) == (LOCAL_HDR_SIG) && IsFileFormatZIP(buffer, How, dwFileSize, io, hHash))
			return Finalize(pHashBuff, hHash);

		if (DW(buffer) == (RAR_SIG) && IsFileFormatRAR(buffer, How, dwFileSize, io, hHash))
			return Finalize(pHashBuff, hHash);
*/
		if (DW(buffer) == (LOCAL_HDR_SIG) && CalcArchiveHash(buffer, How, dwFileSize, io, hHash))
			return Finalize(pHashBuff, hHash);

		if (DW(buffer) == (RAR_SIG) && CalcArchiveHash(buffer, How, dwFileSize, io, hHash))
			return Finalize(pHashBuff, hHash);
	}

	if (dwFileSize < 0x8000 && IsFileFormatINF(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);
	
	// check for "desktop.ini"
	if (buffer[0]=='[' && dwFNSize == 11 && How == dwFileSize && __stricmp(szFileName, "desktop.ini")==0 && IsFileFormatINI(buffer, How, dwFileSize, io, hHash))
		return Finalize(pHashBuff, hHash);
	
// 13.08.2003 - Mike Pavlyushchik:
//  Т.к. определение формата COM нечеткое, в базу периодически попадают файлы 
//  неизвестных форматов, что может привести к глюкам (новый формат архиватора 
//  занесется в SFDB и не будет проверен даже при появлении разархиватора). 
//  Кроме того формат COM сейчас не является актуальным.
//  Исходя из этого, мы с Graf'ом решили отключить IsFileFormatCOM.
//	if (IsFileFormatCOM(buffer, How, dwFileSize, io, hHash))
//		return Finalize(pHashBuff, hHash);

#if defined(_DEBUG) && !defined(_ICHECKER_NO_MEMIMAGE_)
// store memory images for unknown formats (packed files) into "c:\ichecker_mem_images" folder if exist
	{
		if (OID_MEMORY_PROCESS_MODULE == CALL_SYS_PropertyGetDWord(io, pgOBJECT_ORIGIN))
		{
			char szFileName[0x400];
			char szName[0x300];
			HANDLE hFile;
			if (PR_SUCC(CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_FULL_NAME, szName, sizeof(szName), cCP_ANSI)))
			{
				char* pName = szName;
				char* pPath;
				while (strchr(pName, '|') != NULL)
					pName = strchr(pName, '|') + 1;
				PR_TRACE((io, prtALWAYS_REPORTED_MSG, "ichk\timage name: %s", pName));
				strcpy(szFileName, "c:\\ichecker_mem_images\\");
				strcat(szFileName, pName);
				pPath = strrchr(szFileName, '\\');
				if (pPath != NULL)
				{
					if (strcmp(pPath+1, pName) != 0)
					{
						*pPath = 0;
						CreateDirectory(szFileName, NULL);
						PR_TRACE((io, prtALWAYS_REPORTED_MSG, "ichk\tcreate folder: '%s' err=%08X", szFileName, GetLastError()));
						*pPath = '\\';
					}
				}

				hFile = CreateFile(szFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwBytesRead = 0;
					tQWORD qwOffset = 0;
					char buff[0x1000];
					do{
						tDWORD dwBytesWrite;
						CALL_IO_SeekReadTimed(io, &dwBytesRead, qwOffset, buff, sizeof(buff));
						WriteFile(hFile, buff, dwBytesRead, &dwBytesWrite, NULL);
						qwOffset += dwBytesRead;
					} while (dwBytesRead);
					CloseHandle(hFile);
				}
				else
				{
					PR_TRACE((io, prtERROR, "ichk\tCannot create image file: %s (%08X)", szFileName, GetLastError()));
				}
			}
		}
	}
#endif
    CALL_Hash_Close(hHash);	
	return errMODULE_UNKNOWN_FORMAT;
}

tBOOL CalcArchiveHash(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
	tERROR error;
	tDWORD dwAttr;
	tDATETIME dt;
	tDWORD eof_size;
	// calc file timestamps
	if (PR_FAIL(error = CALL_SYS_PropertyGet(io, NULL, pgOBJECT_LAST_WRITE_TIME, &dt, sizeof(dt))))
	{
		PR_TRACE((io, prtERROR, "ichk\tCan't get pgOBJECT_LAST_WRITE_TIME for archive with %terr", error));
		return cFALSE;
	}
	check(&dt, sizeof(dt), hHash);
	if (PR_FAIL(error = CALL_SYS_PropertyGet(io, NULL, pgOBJECT_CREATION_TIME, &dt, sizeof(dt))))
	{
		PR_TRACE((io, prtERROR, "ichk\tCan't get pgOBJECT_CREATION_TIME for archive with %terr", error));
		return cFALSE;
	}
	check(&dt, sizeof(dt), hHash);
	// calc file attributes
	if (PR_FAIL(error = CALL_SYS_PropertyGet(io, NULL, pgOBJECT_ATTRIBUTES, &dwAttr, sizeof(dwAttr))))
	{
		PR_TRACE((io, prtERROR, "ichk\tCan't get pgOBJECT_ATTRIBUTES for archive with %terr", error));
		return cFALSE;
	}
	check(&dwAttr, sizeof(dwAttr), hHash);
	// calc beginning of the file
	check(buffer, dwDataSize, hHash);
	// calc end of the file
	eof_size = min(BIGBUFFERSIZE, dwFileSize - dwDataSize);
	if (eof_size)
	{
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, dwFileSize - eof_size, buffer, eof_size);
		if (PR_FAIL(error) || dwDataSize != eof_size)
			return cFALSE;
		check(buffer, dwDataSize, hHash);
	}
	return cTRUE;
}


#define _RVAToRaw(rva, pImageNTHeaders, dwDataSize) (bMemoryImage ? rva : RVAToRaw(rva, pImageNTHeaders, dwDataSize))

tDWORD RVAToRaw(tDWORD RVA,PIMAGE_NT_HEADERS3264 pPE, tDWORD dwSizeOfBuffer)
{
PIMAGE_SECTION_HEADER pSH;
tDWORD Sections, i;
tDWORD SecOffset;
	
	if (dwSizeOfBuffer < sizeof(IMAGE_NT_HEADERS))
		return 0;
	if(RVA < pPE->OptionalHeader.SizeOfHeaders)
		return RVA;
	SecOffset = pPE->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(DWORD);
	if (SecOffset > dwSizeOfBuffer)
		return 0;
	Sections = min(pPE->FileHeader.NumberOfSections, (dwSizeOfBuffer - SecOffset) / sizeof(IMAGE_SECTION_HEADER));
	pSH = (PIMAGE_SECTION_HEADER)(((tBYTE*)pPE)+SecOffset);
	for(i=0;i<Sections;i++)
	{
		if(RVA - pSH->VirtualAddress < pSH->SizeOfRawData) // 
			return pSH->PointerToRawData + (RVA - pSH->VirtualAddress);
		pSH++;
	}
	return 0;
}

tDWORD GetSectionNo(tDWORD RVA,PIMAGE_NT_HEADERS3264 pPE, tDWORD dwSizeOfBuffer)
{
PIMAGE_SECTION_HEADER pSH;
tDWORD Sections, i;
tDWORD SecOffset;
	
	if (dwSizeOfBuffer < sizeof(IMAGE_NT_HEADERS))
		return 0;
	if(RVA < pPE->OptionalHeader.SizeOfHeaders)
		return RVA;
	SecOffset = pPE->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(DWORD);
	if (SecOffset > dwSizeOfBuffer)
		return 0;
	Sections = min(pPE->FileHeader.NumberOfSections, (dwSizeOfBuffer - SecOffset) / sizeof(IMAGE_SECTION_HEADER));
	pSH = (PIMAGE_SECTION_HEADER)(((tBYTE*)pPE)+SecOffset);
	for(i=0;i<Sections;i++)
	{
		if(RVA - pSH->VirtualAddress < pSH->SizeOfRawData)
			return i+1;
		pSH++;
	}
	return 0;
}

tDWORD GetOverlayOffset(PIMAGE_NT_HEADERS3264 pPE, tDWORD dwSizeOfBuffer)
{
PIMAGE_SECTION_HEADER pSH;
tDWORD Sections, i;
tDWORD SecOffset;
tDWORD dwEndOfFile = 0;
	
	if (dwSizeOfBuffer < sizeof(IMAGE_NT_HEADERS))
		return 0;
	SecOffset = pPE->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(DWORD);
	if (SecOffset > dwSizeOfBuffer)
		return 0;
	Sections = min(pPE->FileHeader.NumberOfSections, (dwSizeOfBuffer - SecOffset) / sizeof(IMAGE_SECTION_HEADER));
	pSH = (PIMAGE_SECTION_HEADER)(((tBYTE*)pPE)+SecOffset);
	for(i=0;i<Sections;i++)
	{
		if(pSH->PointerToRawData + pSH->SizeOfRawData > dwEndOfFile)
			dwEndOfFile = pSH->PointerToRawData + pSH->SizeOfRawData;
		pSH++;
	}
	return dwEndOfFile;
}

long __inline A20(tWORD Segment,tWORD Offset)
{
	return(( (long)((long)Segment<<4) +(long)(Offset) ) & 0xFFFFFL);
}

tERROR CalcFileHash(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, hIO io, hHASH hHash)
{
	tDWORD how;
	tERROR error = errOK;
	
	if (dwDataSizeInBuffer != 0)
		check(buffer, dwDataSizeInBuffer, hHash);
	
	while (dwReadStartOffset < dwReadEndOffset)
	{
		tDWORD dwReadSize = (dwReadEndOffset-dwReadStartOffset > BIGBUFFERSIZE ? BIGBUFFERSIZE : dwReadEndOffset-dwReadStartOffset);
		error = CALL_IO_SeekReadTimed(io, &how, dwReadStartOffset, buffer, dwReadSize);
		if (PR_FAIL(error) || how == 0) // !!! errOEF not an error (it's warning)
			return error;
		check(buffer, how, hHash);
		dwReadStartOffset += how;
	}
	
	return errOK;
}

tERROR CalcFileHashStrepped(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, tDWORD dwStripSize, tDWORD dwPartOfStreepToCheck, hIO io, hHASH hHash)
{
tDWORD how;
tERROR error = errOK;
	
	if (dwDataSizeInBuffer != 0)
		check(buffer, dwDataSizeInBuffer, hHash);

	while (dwReadStartOffset < dwReadEndOffset)
	{
		tDWORD dwReadSize = (dwReadEndOffset-dwReadStartOffset > dwPartOfStreepToCheck ? dwPartOfStreepToCheck : dwReadEndOffset-dwReadStartOffset);
		error = CALL_IO_SeekReadTimed(io, &how, dwReadStartOffset, buffer, dwReadSize);
		if (PR_FAIL(error) || how == 0) // !!! errOEF not an error (it's warning)
			return error;
		check(buffer, how, hHash);
		dwReadStartOffset += dwStripSize;
	}
	
	return errOK;
}


tBOOL IsFileFormatTTF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
#define W_BIGENDIAN(w) (w>>8|(w & 0xFF)<<8)
#define DW_BIGENDIAN(dw) (W_BIGENDIAN(HIWORD(dw)) | W_BIGENDIAN(LOWORD(dw))<<16)
	TTF_OFFSET_TABLE* pOffsetTable;
	TTF_TABLE_DIRECTORY_ENTRY* pTableDirectoryEntry;
	DWORD dwTables;
	DWORD entrySelector;
	DWORD i;
	DWORD dwTotalSize = 0;
	
	pOffsetTable = (TTF_OFFSET_TABLE*)buffer;
	pTableDirectoryEntry = (TTF_TABLE_DIRECTORY_ENTRY*)(pOffsetTable+1);
	

	if (W_BIGENDIAN(pOffsetTable->sfnt_version.fract)!=1 || pOffsetTable->sfnt_version.value!=0)
		return cFALSE;

	dwTables = W_BIGENDIAN(pOffsetTable->numTables);
	entrySelector = W_BIGENDIAN(pOffsetTable->entrySelector);

	if (((DWORD)(1<<entrySelector))>dwTables || ((DWORD)(1<<(entrySelector+1)))<dwTables)
		return cFALSE;

	entrySelector = sizeof(TTF_OFFSET_TABLE) + dwTables * sizeof(TTF_TABLE_DIRECTORY_ENTRY);
	if (entrySelector > dwDataSize)
		return cFALSE;

	for (i=0; i<dwTables; i++)
	{
/*			dwTag = pTableDirectoryEntry->tag;
		printf("\n %.4s   %08X    %08X    %08X",
			&dwTag,
			DW_BIGENDIAN(pTableDirectoryEntry->checkSum),
			DW_BIGENDIAN(pTableDirectoryEntry->offset),
			DW_BIGENDIAN(pTableDirectoryEntry->length));
*/
		dwTotalSize += ((DW_BIGENDIAN(pTableDirectoryEntry->length)+3) & ~3);
		pTableDirectoryEntry++;
	}
	dwTotalSize+=entrySelector;
	if (dwTotalSize != dwFileSize)
		return cFALSE;

	check(buffer, entrySelector, hHash);
	check((tBYTE*)&dwFileSize, sizeof(dwFileSize), hHash);
	return cTRUE;
#undef W_BIGENDIAN
#undef DW_BIGENDIAN
}

tBOOL IsFileFormatLNK(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
	tDWORD dwFlags;
	tDWORD dwFileLocationsFlags;
	BYTE* ptr;

	if (dwDataSize < 0x68 || dwFileSize != dwDataSize)
		return cFALSE;

	if (DW(buffer) != 0x0000004C)
		return cFALSE;

	if ((DW(buffer+ 4))!=0x00021401 || // check LNK guid {00021401-0000-0000-00C0-000000000046}
		(DW(buffer+ 8))!=0x00000000 ||
		(DW(buffer+12))!=0x000000C0 || 
		(DW(buffer+16))!=0x46000000)
		return cFALSE;

	dwFlags = DW(buffer+0x14);

	memset(buffer+0x1C, 0, 24); // clear filetimes

	ptr = buffer + DW(buffer);

	// Shell item id list.

	if (dwFlags & 1)
	{
		tWORD wListSize = W(ptr);

		if (dwFileSize < DW(buffer) + wListSize)
			return cFALSE;

		memset(ptr, 0, wListSize);
		ptr += wListSize + sizeof(tWORD);
		
//		do {
//			if ((long)(ptr - buffer) < (long)(dwDataSize - sizeof(USHORT)))
//				return cFALSE;
//			dwItemSize = W(buffer);
//			if (dwItemSize)
//				ptr += dwItemSize - sizeof(USHORT);
//		} while (dwItemSize);
	}

	
	// File Location Info

	if (dwFileSize - (ptr - buffer) <  0x1C)
		return cFALSE;

	dwFileLocationsFlags = DW(ptr+8);
	if ( (dwFileLocationsFlags & 1) == 0 )
	{
		// clear garbage in local volume info offsets
		DW(ptr+0x0C) = 0; 
		DW(ptr+0x10) = 0;
	}

	if ( (dwFileLocationsFlags & 2) == 0 )
	{
		// clear garbage in network volume info offsets
		DW(ptr+0x14) = 0;
	}
	
	check(buffer, dwDataSize, hHash);
	return cTRUE;
}

tBOOL IsFileFormatINI(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
	check(buffer, dwDataSize, hHash);
	return cTRUE;
}

tBOOL IsFileFormatINF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
#define SkipSpaces(buffer, i, dwDataSize)	while (i<dwDataSize && (buffer[i]==' ' || buffer[i]==9)) i++;
	DWORD i;
	DWORD dwNonPrintableChars=0;

	if (dwDataSize < 16)
		return cFALSE;

	dwDataSize-=16;
	if (*(WORD*)buffer == 0xFEFF) // Unicode file
		return cFALSE;

	// check first byte
	if (buffer[0] != ' ' &&
		buffer[0] != 9 &&
		buffer[0] != 10 &&
		buffer[0] != 13 &&
		buffer[0] != ';' &&
		buffer[0] != '[')
		return cFALSE;

	if (dwDataSize > 0x100) // check for binary file
	{
		for (i=0; i<0x100; i++)
		{
			tBYTE b = buffer[i];
			if (b<9 || (b>13 && b<32))
			{
				if (++dwNonPrintableChars > 10)
					return cFALSE;
			}
		}
	}

	for (i=0; i<dwDataSize; i++)
	{
		// [version]
		if (buffer[i] == '[' && 
                    (DW(buffer+i+1) | 0x20202020) == MAKE_DWORD(0x7665 /*'ve'*/,0x7273 /*'rs'*/ ) &&
                    (DW(buffer+i+5) | 0x00202020) == MAKE_DWORD(0x696F/*'io'*/,0x6E5D/*'n]'*/) && 
			(i==0 || buffer[i-1] == 0x0a))
		{
			i+=9;
			break;
		}
		
	}
	
	for (; i<dwDataSize; i++)
   	{
		tBOOL bQuote = cFALSE;
		// signature="$********$"
		if (buffer[i] == 0x0a)
			SkipSpaces(buffer, i, dwDataSize);
		if ((DW(buffer+i)   | 0x20202020) == MAKE_DWORD(0x7369/*'si'*/,0x676E/*'gn'*/) &&  
                    (DW(buffer+i+4) | 0x20202020) == MAKE_DWORD(0x6174/*'at'*/,0x7572/*'ur'*/) &&  
			((buffer[i+8])| 0x20) == 'e')
		{
			i+=9;
			SkipSpaces(buffer, i, dwDataSize);
			if (buffer[i++] != '=') break; 
			SkipSpaces(buffer, i, dwDataSize);
			if (buffer[i] == '"') {i++; bQuote=cTRUE;} // skip space
			if (buffer[i++] != '$') break; 
			
			for (;i<dwDataSize; i++)
			{
				if (buffer[i] == '$')
				{
					i++;
					if (buffer[i] == '"') i++; // skip quote
					SkipSpaces(buffer, i, dwDataSize);
					if (buffer[i] != 0x0d && buffer[i] != 0x0a) break;

//					DEBUG(("Found string: Signature=\"$*******$\". Assuming INF file."));
					if (PR_FAIL(CalcFileHash(buffer, dwDataSize, dwDataSize, dwFileSize, io, hHash)))
						return cFALSE;
					return cTRUE;
				}
				if (buffer[i] == 0x0a)
					break;
			}
		}
   	}
	
	return cFALSE;
#undef SkipSpaces
}

tBOOL IsIntel16Code(tBYTE* buffer, tDWORD dwDataSize)
{
#define	d0	((BYTE)w0)
#define	d1	(((BYTE*)(&w0))[1])
#define	WAS_PUSH	0x0001
#define BUFFER_CHECK_SIZE 0x30

	UINT	ptr, flags;
	WORD	w0;

	if (dwDataSize < 2)
		return cFALSE;

	ptr=flags=0;	goto DOS_Loop;

	Opc4:	ptr++;
	Opc3:	ptr++;
	Opc2:	ptr++;
	Opc1:	ptr++;
DOS_Loop:
	if (ptr>BUFFER_CHECK_SIZE || ptr>dwDataSize-2)	goto RetTrue;

	w0=((WORD FAR*)(buffer+ptr))[0];	// Printf("%02X %02X",d0,d1);

	switch (d0&0xF0)	{
		case 0x40:			goto Opc1;
		case 0x70:
			if (d1<0x80 || d1>0xE0)	goto Opc2;	// JMP short
			break;
	}

	if (d0==0xBC)	{
		flags|=WAS_PUSH;
		goto Opc3;
	}

	switch (d0&0xF8)	{
		case 0x50: 	flags|=WAS_PUSH;
		case 0x58:	goto Opc1;
		case 0xB8:	ptr++;
		case 0xB0:	ptr++;
		case 0x90:	goto Opc1;
	}

	switch (d0)	  {
		case 0xE9: case 0xE8:
			d1=((BYTE FAR*)(buffer+ptr+1))[0];
			if (d1<0xA0 || d1>=0xF0)	goto RetTrue;	// JMP or CALL
			d1=(BYTE)(((WORD FAR*)(buffer+ptr+2))[0]);
			if (d1<0xA0 || d1>=0xF0)	goto RetTrue;	// JMP or CALL
			break;
		case 0xEB:
			if (d1<0x80 || d1>0xE0)		goto RetTrue;	// JMP short
			break;

		case 0xC3: case 0xCB:
			if ((flags&WAS_PUSH) && ptr>2)	goto RetTrue;	// RET
			break;

		case 0xCD:
			if (d1<0x40)			goto RetTrue;
			break;
		case 0x9A: case 0xEA:
			if (ptr>5)			goto RetTrue;	// CALL/JMP FAR
			break;
		case 0xFF:
			if (d1>=0xC0 && ptr>2)	{	// must be init
				switch (d1&0x38)	{
					case 0x10: case 0x20:	goto RetTrue;	// CALL/JMP reg
					default: 		break;
				}
			}

			switch (d1)	{
				case 0x16: case 0x1E:			// CALL data
				case 0x26: case 0x2E:	goto RetTrue;	// JMP data
				case 0x36:		flags|=WAS_PUSH;
							goto Opc4;
			}
			break;

		case 0x06: case 0x0E: case 0x16: case 0x1E:
		case 0x07:            case 0x17: case 0x1F:
		case 0x26: case 0x2E: case 0x36: case 0x3E:
		case 0x27:
		case 0x60: case 0x61:
		case 0x99: case 0x9C: case 0x9D:
		case 0xA4: case 0xA5: case 0xA6: case 0xAA: case 0xAB: case 0xAC: case 0xAD:
		case 0xCC:
		case 0xF3:
		case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
							goto Opc1;

		case 0x0D:				if (d1==0x0A)	break;
			
                case 0x05: case 0x25: case 0x2D: case 0x35: case 0x3D:
		case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA9:
							ptr++;
		case 0x04: case 0x24: case 0x2C: case 0x34: case 0x3C:
							goto Opc2;

		case 0x68:				ptr++;
		case 0x6A:	flags|=WAS_PUSH;	goto Opc2;

		case 0x81: case 0xC7:			ptr++;
		case 0x80: case 0x83: case 0xC0: case 0xC1: case 0xC6:
							ptr++;
		case 0x01: case 0x03: case 0x0A: case 0x23: case 0x2A: case 0x2B:
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x39: case 0x3B:
		case 0x86: case 0x87: case 0x88: case 0x89: case 0x8A:
		case 0x8B: case 0x8C: case 0x8D: case 0x8E:
		case 0xC4:
		case 0xD1: case 0xD3:
		case 0xF6: case 0xF7: case 0xFE:
			switch (d1&0xC0)	{
				case 0x00:		if ((d1&7)==6)	ptr+=2;
				case 0xC0:		goto Opc2;
				case 0x80:		goto Opc4;
				case 0x40:		goto Opc3;
			}
			break;
		case 0xE0:
		case 0xE2:
			if (ptr<3)		break;
			if (d1<0xF8-ptr)	break;
                        goto RetTrue;
		case 0xE4:
			if (d1==0x40)	goto Opc2;
			break;
	}

	// Printf("Bad code %02X %02X at %04X",d0,d1,ptr);
	return cFALSE;
	
RetTrue:	// Printf("It is a program");
	return cTRUE;

#undef	BUFFER_CHECK_SIZE
#undef	WAS_PUSH
#undef	d0
#undef	d1
}



tBOOL IsFileFormatSYS(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
tDWORD offset, offset2;
tERROR error;
tDWORD dwDriversCount = 0;
tWORD wOffsetToNextDiver;

	do {
		if (dwDataSize < 0xC)
			return cFALSE;
		if(dwFileSize > 0xffff)
			return cFALSE;
		if (W(buffer+2)!=0xffff)						// *.SYS 
			return cFALSE;

		offset=W(buffer+6);
		if(offset >= dwFileSize) // entry point beyond file end?
			return cFALSE; 

		offset2=W(buffer+8);
		if(offset2 >= dwFileSize) // entry point 2 beyond file end?
			return cFALSE; 
		
		wOffsetToNextDiver = W(buffer);
		dwDriversCount++;

		// Count Header data
		check(buffer, 10, hHash);
		
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatSYS: Error reading file err=%08X", error));
			return cFALSE;
		}
		check(buffer, dwDataSize, hHash);

		error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset2, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatSYS: Error reading file err=%08X", error));
			return cFALSE;
		}
		check(buffer, dwDataSize, hHash);

		if(wOffsetToNextDiver != 0xffff)
		{
			// Read Header
			error = CALL_IO_SeekReadTimed(io, &dwDataSize, wOffsetToNextDiver, buffer, BUFFERSIZE);
			if (PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatSYS: Error reading file err=%08X", error));
				return cFALSE;
			}
//			return IsFileFormatSYS(buffer, dwDataSize, dwFileSize, io, hHash);
		}
	} while (dwDriversCount < 10 && wOffsetToNextDiver != 0xffff);
	
	return cTRUE;
}

// is DOS COM program? ----------------------------------------------
tBOOL IsFileFormatCOM(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
	tDWORD offset=0, oldoffset=0;
	tDWORD dwJmpCount = 0;
	tERROR error;
	
	// retrive entry point offset
	//	if (DW(buffer)==0xffffffff)						// *.SYS 
//		offset=W(buffer+6);
	/*else*/ if (!IsIntel16Code(buffer, dwDataSize))			// *.COM
		return cFALSE;

	else if(buffer[4] == 0xC3 && (buffer[3] &0xF0) == 0x50)	// *.COM (PUSH - RET)
		offset=W(buffer+1) - 0x100; // ((buffer[2]<<8) + buffer[1]) - 0x100;
	else if(buffer[0]==0xEB)								// *.COM (JMP NEAR)  (signed 1 byte offset)
		offset=(tDWORD)((signed char)(buffer[1]))+2;
	else if(buffer[0]==0xE8 && W(buffer+1)!=0)				// *.COM (CALL NEAR) (signed 2 byte offset)
		offset=(W(buffer+1)+3) & 0xFFFF;
	else if(buffer[0]==0xE9) 								// *.COM (JMP SHORT) (signed 2 byte offset)
		offset=(W(buffer+1)+3) & 0xFFFF;
	
	do 
	{
		if(offset>=dwFileSize) // entry point beyond file end?
			return cFALSE; 
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
			return cFALSE;
		}
		if (dwJmpCount++ == 3)
			break;
		oldoffset=offset;
		if ((buffer[0]==0xE8 || buffer[0]==0xE9) && (*(tWORD*)&buffer[1])!=0) // CALL NEAR/JMP SHORT
			offset=(W(buffer+1)+3+oldoffset) & 0xFFFF;
	} while (oldoffset!=offset);
	
	check(buffer, dwDataSize, hHash);
	return cTRUE;
}

tBOOL IsFileFormatELF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
	Elf32_Phdr	*phdr;
	int i;
	tERROR error;
	tDWORD offset=0;
	
	if ( DW(buffer) != 0x464C457Fl )
		return cFALSE;
		
	if (dwDataSize<sizeof(Elf32_Ehdr))
		return cFALSE;

	if (((Elf32_Ehdr*)buffer)->e_type != ET_EXEC && ((Elf32_Ehdr*)buffer)->e_type != ET_DYN)
		return cFALSE;
	if (((Elf32_Ehdr*)buffer)->e_machine != EM_386 && ((Elf32_Ehdr*)buffer)->e_machine != EM_486)
		return cFALSE;
	if (((Elf32_Ehdr*)buffer)->e_version != EV_CURRENT)
		return cFALSE;
	if (((Elf32_Ehdr*)buffer)->e_phnum == 0 || ((Elf32_Ehdr*)buffer)->e_phoff > BIGBUFFERSIZE - SIZEOF_ELF32_PHDR)
		return cFALSE;
	
	for (phdr = (Elf32_Phdr *)(buffer+((Elf32_Ehdr*)buffer)->e_phoff), i = 0; i < ((Elf32_Ehdr*)buffer)->e_phnum; i++)
	{
		if (i*SIZEOF_ELF32_PHDR+((Elf32_Ehdr*)buffer)->e_phoff > BIGBUFFERSIZE - SIZEOF_ELF32_PHDR )
			return cFALSE;
		if (phdr->p_vaddr < ((Elf32_Ehdr*)buffer)->e_entry && (phdr->p_vaddr+phdr->p_filesz) > ((Elf32_Ehdr*)buffer)->e_entry )
		{
			offset = (((Elf32_Ehdr*)buffer)->e_entry-phdr->p_vaddr)+phdr->p_offset;
			break;
		}
		++phdr;
	}

	check(buffer, sizeof(Elf32_Ehdr), hHash);
	
	error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
	if (PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
		return cFALSE;
	}
	check(buffer, dwDataSize, hHash);

	return cTRUE;
	
}

tBOOL IsFileFormatEXE(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
tWORD* WordBuffer;
tDWORD dwDOSEXEEntryPoint;
tBOOL bAlreadyCheckIP;
int iNumberOfJumps;
tERROR error;
tDWORD offset=0, oldoffset=0;
tDWORD dwOffsetToRelo, dwReloSize;
tDWORD dwOffsetToExport, dwExportSize;
tDWORD dwOffsetToOverlay, dwOverlaySize;
tBOOL  bMemoryImage = cFALSE;
tBOOL bSuspectedForSelfExtractor = cFALSE;

	IMAGE_DOS_HEADER* pImageDosHeader = (IMAGE_DOS_HEADER*)buffer;

#if !defined (_ICHECKER_NO_MEMIMAGE_)
	if (OID_MEMORY_PROCESS_MODULE == CALL_SYS_PropertyGetDWord(io, pgOBJECT_ORIGIN))
		bMemoryImage = cTRUE;
#endif

	WordBuffer = (tWORD*)buffer;
	if( pImageDosHeader->e_magic!=0x4D5A/*'MZ'*/ && pImageDosHeader->e_magic!=0x5A4D /*'ZM'*/)
		return cFALSE;

	bAlreadyCheckIP = cFALSE;
	if(dwFileSize < sizeof(IMAGE_DOS_HEADER))
	{
		PR_TRACE(( io, prtNOTIFY, "ichk\tThis is not EXE file"));
		return cFALSE;
	}
	// Skip some data
	((PIMAGE_DOS_HEADER)buffer)->e_csum = 0;
	// Check DOS Header
	check(buffer, sizeof(IMAGE_DOS_HEADER), hHash);

	dwDOSEXEEntryPoint = A20(WordBuffer[11],WordBuffer[10]);
	dwDOSEXEEntryPoint += (((long)WordBuffer[4])<<4);
	
	if( WordBuffer[0xC] == 0x40) //> 0x3F)	// May be NE or PE ? (LX LE)
	{
		//	memcpy(TempBuffer,buffer,512);
		
		// Got EP into already read buffer?
		if(BIGBUFFERSIZE - dwDOSEXEEntryPoint > BUFFERSIZE && (tDWORD)pImageDosHeader->e_lfanew < BIGBUFFERSIZE)
		{
			bAlreadyCheckIP = cTRUE;
			check(buffer+dwDOSEXEEntryPoint, (tDWORD) pImageDosHeader->e_lfanew-dwDOSEXEEntryPoint, hHash);
		}
		
		//Get NewEXE (PE) Header offset
		if((tDWORD)(pImageDosHeader->e_lfanew) > BIGBUFFERSIZE/2)
		{
			error = CALL_IO_SeekReadTimed(io, &dwDataSize, (tDWORD) pImageDosHeader->e_lfanew, buffer, BIGBUFFERSIZE);
			if (PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
				return cFALSE;
			}
			WordBuffer = (tWORD*)buffer;
		}
		else
		{
			dwDataSize = BIGBUFFERSIZE - (tDWORD) pImageDosHeader->e_lfanew;
			WordBuffer = (tWORD*)(buffer + (tDWORD) pImageDosHeader->e_lfanew);
		}
		// New EXE File
		if(WordBuffer[0] == 0x454E/*'EN'*/)
		{
			// Count New EXE Header's CRC
			check((tBYTE*)WordBuffer, (tDWORD)((dwDataSize>(tDWORD)sizeof(NEWEXEHEADER)) ? sizeof(NEWEXEHEADER) : dwDataSize), hHash);
		}
		// Portable Executable
		else if(WordBuffer[0] == 0x4550/*'EP'*/)
		{
			
			//bool bIsThisPEPacked(void)
			PIMAGE_NT_HEADERS3264 pNTHeaders = (PIMAGE_NT_HEADERS3264)WordBuffer;
			PIMAGE_FILE_HEADER  pFileHeader = (PIMAGE_FILE_HEADER)((PBYTE)pNTHeaders + sizeof(DWORD)); // + PE signature
			PIMAGE_SECTION_HEADER pFirstSection = IMAGE_FIRST_SECTION(pNTHeaders); // IMAGE_FIRST_SECTION doesn't need 32/64 versions since the file header is the same either way.
			PIMAGE_DATA_DIRECTORY pDataDirectory = IMAGE_FIRST_DIRECTORY(pNTHeaders);
			if ( pNTHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 
				&& pNTHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
				return cFALSE;

			if ((tBYTE*)(pFirstSection+1) > (tBYTE*)(pNTHeaders) + dwDataSize) // not enought data in buffer
			{
				PR_TRACE((io, prtIMPORTANT, "ichk\tnot enought data in buffer (ntheader+firstsection)"));
				return cFALSE;
			}

			if (!pDataDirectory)
			{
				PR_TRACE((io, prtIMPORTANT, "ichk\tcannot resolve data dir (wrong machine type)"));
				return cFALSE;
			}
			
			// from here check for packed images to avoid pure packers hashes

			// 12.01.2007 Mike - commented ImageBase check 
			// Usually DLLs loaded on the same address into process, even if they are relocated
//#if !defined (_ICHECKER_NO_MEMIMAGE_)
//			if (bMemoryImage) // check image base for memory images
//			{
//				tDWORD hModule;
//				hModule = CALL_SYS_PropertyGetDWord(io, plIO_HMODULE);
//				if (hModule == 0 || hModule != pNTHeaders->OptionalHeader.ImageBase)
//				{
//					PR_TRACE((io, prtIMPORTANT, "ichk\tMemory image has been relocated (hModule=%08X, ImageBase=%08X)", hModule, pNTHeaders->OptionalHeader.ImageBase));
//					return cFALSE;
//				}
//			}
//#endif

			if (bMemoryImage && (pNTHeaders->OptionalHeader.DllCharacteristics & 0x40))
			{
				// Vista ASLR (Address Space Layout Randomization)
				PR_TRACE((io, prtIMPORTANT, "ichk\tPE module: ASLR is set"));
				return FALSE;
			}

			if (pFirstSection->Characteristics & IMAGE_SCN_MEM_WRITE)
			{
				PR_TRACE((io, prtIMPORTANT, "ichk\tPacked PE module: 1st section is writable"));
				bSuspectedForSelfExtractor=cTRUE;
				//return cFALSE;
			}
			if (pNTHeaders->OptionalHeader.AddressOfEntryPoint)
			{
				if ((pNTHeaders->OptionalHeader.AddressOfEntryPoint >= pFirstSection->VirtualAddress + pFirstSection->Misc.VirtualSize) || (pNTHeaders->OptionalHeader.AddressOfEntryPoint < pFirstSection->VirtualAddress ))
				{
					PR_TRACE((io, prtIMPORTANT, "ichk\tPacked PE module: EntryPoint out of 1st section EP=%08X", pNTHeaders->OptionalHeader.AddressOfEntryPoint));
					bSuspectedForSelfExtractor=cTRUE;
					//return cFALSE;
				}
			}
			if (pFirstSection->SizeOfRawData < pFirstSection->Misc.VirtualSize)
			{
				PR_TRACE((io, prtNOTIFY, "ichk\tPacked PE module: 1st section physical size < virtual size (%08X<%08X)", pFirstSection->SizeOfRawData, pFirstSection->Misc.VirtualSize));
				bSuspectedForSelfExtractor=cTRUE;
				//return cFALSE;
			}

			if(bSuspectedForSelfExtractor){
				
				// 12.01.2007 - Mike - уменьшил лимит с 5Mb до 1Mb
				if(dwFileSize > 1*1024*1024)
					return cFALSE;

				if (PR_FAIL(error = CalcFileHash(buffer, 0, 0, dwFileSize, io, hHash)))
				{
					PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
					return cFALSE;
				}
				return cTRUE;
			}
			
			// end of "packed" checks 
			
			// Mike 06.03.2007 - better left them unchanged

			//pNTHeaders->FileHeader.TimeDateStamp = 0; 
			//pNTHeaders->OptionalHeader.MajorLinkerVersion = 0;
			//pNTHeaders->OptionalHeader.MinorLinkerVersion = 0;
			//pNTHeaders->OptionalHeader.SizeOfInitializedData = 0;
			//pNTHeaders->OptionalHeader.SizeOfUninitializedData = 0;
			
			//Resource Table RVA and size
			//pDataDirectory[2].Size = 0;
			//pDataDirectory[2].VirtualAddress = 0;
			
			//pNTHeaders->OptionalHeader.CheckSum = 0;
			
			check((tBYTE*)WordBuffer, sizeof(IMAGE_FILE_HEADER)+4+sizeof(IMAGE_OPTIONAL_HEADER), hHash);

			// 12.01.2007 - Mike - calc sections info
			{
				WORD nCalcSects = min(pNTHeaders->FileHeader.NumberOfSections, 
					((tBYTE*)(pNTHeaders) + dwDataSize - (tBYTE*)pFirstSection) / sizeof(IMAGE_SECTION_HEADER));
				check((tBYTE*)pFirstSection, sizeof(IMAGE_SECTION_HEADER)*nCalcSects, hHash);
			}

			dwOverlaySize = 0;
			if (bMemoryImage)
				dwOffsetToOverlay = 0;
			else
			{
				dwOffsetToOverlay = GetOverlayOffset(pNTHeaders, dwDataSize);
				if(dwOffsetToOverlay)
					dwOverlaySize = dwFileSize - dwOffsetToOverlay;
			}
			
			offset = _RVAToRaw(pNTHeaders->OptionalHeader.AddressOfEntryPoint, pNTHeaders, dwDataSize);

			dwOffsetToRelo = 0;
			dwReloSize = pDataDirectory[5].Size;
			if(dwReloSize != 0)
			{
				dwOffsetToRelo = _RVAToRaw(pDataDirectory[5].VirtualAddress, pNTHeaders, dwDataSize);
				
			}
			dwOffsetToExport = 0;
			dwExportSize = pDataDirectory[0].Size;
			if( dwExportSize != 0)
			{
				dwOffsetToExport = _RVAToRaw(pDataDirectory[0].VirtualAddress, pNTHeaders, dwDataSize);
			}
				
			if(offset != 0)
			{
				error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
				//PR_TRACE(( io, prtNOTIFY, "ichk\tmemmod EP=%08X res=%08X %terr", offset, dwDataSize, error));
				if (PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, dwDataSize, hHash);
			}
			// Relo table
			if(dwOffsetToRelo != 0)
			{
				// fixed wrong read offset (27.11.02)
				error = CALL_IO_SeekReadTimed(io, &dwDataSize, dwOffsetToRelo, buffer, (dwReloSize < BUFFERSIZE ? dwReloSize : BUFFERSIZE));
				//PR_TRACE(( io, prtNOTIFY, "ichk\tmemmod REL=%08X res=%08X %terr", dwOffsetToRelo, dwDataSize, error));
				if (PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, dwDataSize, hHash);
			}
			// Export table
			if(dwOffsetToExport != 0)
			{
				//PR_TRACE(( io, prtNOTIFY, "ichk\tmemmod EXP=%08X size=%08X", dwOffsetToExport, dwExportSize, error));
				error = CalcFileHash(buffer, 0, dwOffsetToExport, dwOffsetToExport+dwExportSize, io, hHash);
				if (PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
					return cFALSE;
				}
			}
			// Overlay
			if(dwOffsetToOverlay != 0)
			{
				//PR_TRACE(( io, prtNOTIFY, "ichk\tmemmod OVL=%08X size=%08X", dwOffsetToOverlay, dwOverlaySize));
				if(dwOverlaySize < BIGBUFFERSIZE)
				{
					if (PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToOverlay, dwOffsetToOverlay+dwOverlaySize, io, hHash)))
					{
						PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
						return cFALSE;
					}
				}
				else
				{
					if (PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToOverlay, dwOffsetToOverlay+BIGBUFFERSIZE, io, hHash)))
					{
						PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
						return cFALSE;
					}
					if (PR_FAIL(error = CalcFileHash(buffer, 0, dwFileSize - BIGBUFFERSIZE, dwFileSize, io, hHash)))
					{
						PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
						return cFALSE;
					}
				}
			}
		}
	}
	
	// Check MZ Entry point
	if(bAlreadyCheckIP)
		return cTRUE;
	
	offset = dwDOSEXEEntryPoint;
	iNumberOfJumps = 0;

	do {
		// If Entry point leads after file end
		// No need check & return
		if(offset >= dwFileSize)
			return cTRUE;
		oldoffset = offset;
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "ichk\tError reading file err=%08X", error));
			return cFALSE;
		}
		if((buffer[0] == 0xE9 || buffer[0] == 0xE8) && (*(tWORD *)&buffer[1]) != 0)
		{
			offset=((*(tWORD *)&buffer[1])+3+oldoffset) & 0xFFFF;
			if((++iNumberOfJumps) == 3)
				break;
		}
	} while (offset != oldoffset);
	check(buffer, dwDataSize, hHash);
	return cTRUE;
}

/*************************************************** ITSS 4.1   *******************************************/
//
//
/**************************************************** ITSS 4.1   ******************************************/
tBOOL IsFileFormatITSS41(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
ITSFileHeader* pH;
DatabaseHeader* pDBh;
NodeHeader* pnh;
DWORD dwRelativeOffset;
tERROR error;
ITSSData iData;
tQWORD offPathMgrData;

	if(dwFileSize < sizeof(ITSFileHeader) + sizeof(DatabaseHeader) + sizeof(LZX_Control_Data))
	{
		PR_TRACE(( io, prtNOTIFY, "ichk\tThis is not ITSS 4.1 file"));
		return cFALSE;
	}
	pH = (ITSFileHeader*)buffer;
	iData.offContent = 0;
	iData.cbContent = 0;
	
	iData.offControlData = 0;
	iData.cbControlData = 0;
	
	iData.offResetTable = 0;
	iData.cbResetTable = 0;

	dwRelativeOffset = 0;
	offPathMgrData = pH->offPathMgrData;
	if(pH->uFormatVersion != 2)
	{
		dwRelativeOffset = (DWORD)pH->offPathMgrOrigin;
	}
	
	// Count ITSS Header data
	check(buffer, sizeof(ITSFileHeader), hHash);
	if(pH->offPathMgrData + sizeof(DatabaseHeader) < dwDataSize )
	{
		pDBh = (DatabaseHeader*)&buffer[pH->offPathMgrData];
	}
	else
	{
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, pH->offPathMgrData, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err = %08X", error));
			return cFALSE;
		}
		pDBh = (DatabaseHeader*)buffer;
	}
	if(pDBh->uiMagic != (ITSDB_HEADER_MAGIC) || pDBh->cbDirectoryBlock > 0x2000)
	{
		PR_TRACE(( io, prtNOTIFY, "ichk\tThis is not ITSS 4.1 file. DataBase header's signature is missing or too big Dir Block"));
		return cFALSE;
	}
	if(pDBh->cBlocks > BIGBUFFERSIZE)
	{
		PR_TRACE(( io, prtNOTIFY, "ichk\tThis is ITSS 4.1 file, but Name block size too large (larger then 4k)"));
		return cFALSE;
	}

	// Check Path Mgr Header Data
	check((tBYTE*)pDBh, sizeof(DatabaseHeader), hHash);
	if(pDBh->iRootDirectory == 0xffffffff)
	{
		pDBh->iRootDirectory = 0;
	}
	// Read Root folder with data space
	PR_TRACE(( io, prtNOTIFY, "ichk\tIsFileFormatITSS41: offPathMgrData=%I64x sizeof(DatabaseHeader)=%x iRootDirectory=%x cbDirectoryBlock=%x cBlocks=%d rootoff=%I64x", 
		offPathMgrData, 
		sizeof(DatabaseHeader),
		pDBh->iRootDirectory,
		pDBh->cbDirectoryBlock,
		pDBh->cBlocks,
		offPathMgrData + sizeof(DatabaseHeader) + (pDBh->iRootDirectory * pDBh->cbDirectoryBlock)
		));
	error = CALL_IO_SeekReadTimed(io, &dwDataSize, offPathMgrData + sizeof(DatabaseHeader) + (pDBh->iRootDirectory * pDBh->cbDirectoryBlock), buffer, pDBh->cbDirectoryBlock);
	if (PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err = %08X", error));
		return cFALSE;
	}
	pnh = (NodeHeader*)buffer;
	if(pnh->uiMagic != ITSBLOCK_LEAF_MAGIC)
	{
		PR_TRACE(( io, prtNOTIFY, "ichk\tThis is ITSS 4.1 file, but Root Name block is not leaf block"));
		return cFALSE;
	}
	DumpPaths(buffer, dwDataSize, &iData);

	if(iData.offResetTable != 0)
	{
		if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offResetTable + dwRelativeOffset, iData.offResetTable + dwRelativeOffset + iData.cbResetTable, io, hHash)))
		{
			PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err=%08X", error));
			return cFALSE;
		}
	}
	if(iData.offContent != 0)
	{
		if(iData.cbContent < BIGBUFFERSIZE)
		{
			if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offContent + dwRelativeOffset, iData.offContent + dwRelativeOffset + iData.cbContent, io, hHash)))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err=%08X", error));
				return cFALSE;
			}
		}
		else
		{
			if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offContent + dwRelativeOffset, iData.offContent + dwRelativeOffset + BIGBUFFERSIZE, io, hHash)))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err=%08X", error));
				return cFALSE;
			}

			if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offContent + dwRelativeOffset + iData.cbContent - BIGBUFFERSIZE, iData.offContent + dwRelativeOffset + iData.cbContent, io, hHash)))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err=%08X", error));
				return cFALSE;
			}
		}
	}
	
	if(iData.offControlData != 0)
	{
		if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offControlData + dwRelativeOffset, iData.offControlData + dwRelativeOffset + iData.cbControlData, io, hHash)))
		{
			PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatITSS41: Error reading file err=%08X", error));
			return cFALSE;
		}
	}

	PR_TRACE(( io, prtIMPORTANT, "ichk\tThis is ITSS file"));
	return cTRUE;
}




void DumpPaths(tBYTE* pData, tDWORD dwSize, ITSSData* pITSSData)
{
	tBYTE* pPointer;
	tBYTE* pEnd;
	tUINT Counter;
	
	pPointer = (tBYTE *)&pData[sizeof(NodeHeader) + sizeof(LeafChainLinks)];
	pEnd = (tBYTE *)&pData[dwSize];
	while(pPointer < pEnd)
	{
		Counter = *pPointer;
		pPointer++;
		if(*pPointer == ':' && *(pPointer+1) == ':')	// DataSpace
		{
			CheckNameSpace(Counter, pPointer, pEnd, pITSSData);
		}
		pPointer += Counter;
		DecodePacked32Value(&pPointer);
		DecodePacked32Value(&pPointer);
		DecodePacked32Value(&pPointer);
	}
}

void CheckNameSpace(tUINT Counter, tBYTE* pPointer, tBYTE* pEnd, ITSSData* pData)
{
	if(pPointer + 0x14 + sizeof(szResetTable) + 6 > pEnd)
	{
		return;
	}
	if(memcmp(&pPointer[0x14], szContent, sizeof(szContent)) == 0)
	{
		pPointer += Counter;
		DecodePacked32Value(&pPointer);
		pData->offContent = DecodePacked32Value(&pPointer);
		pData->cbContent = DecodePacked32Value(&pPointer);
	}
	else if(memcmp(&pPointer[0x14], szControlData, sizeof(szControlData)) == 0)
	{
		pPointer += Counter;
		DecodePacked32Value(&pPointer);
		pData->offControlData = DecodePacked32Value(&pPointer);
		pData->cbControlData = DecodePacked32Value(&pPointer);
	}
	else if(memcmp(&pPointer[0x14], szResetTable, sizeof(szResetTable)) == 0)
	{
		pPointer += Counter;
		DecodePacked32Value(&pPointer);
		pData->offResetTable = DecodePacked32Value(&pPointer);
		pData->cbResetTable = DecodePacked32Value(&pPointer);
	}
}

tDWORD DecodePacked32Value(tBYTE **ppb)
{    
tBYTE *pb = *ppb;
tDWORD ul = 0;
tBYTE b;

	for (;;)
	{
		b= *pb++;

		ul = (ul << 7) | (b & 0x7f);

		if (b < 0x80)
			break;
	}

	*ppb = pb; // Record the new pointer location so we can
	// decode the next packed value.

	return ul;
}


/*************************************************** ZIP ******************************************/
//
//
/*************************************************** ZIP ******************************************/
tBOOL IsFileFormatZIP(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
tINT i;
EndRecord* pEnd;
FileHeader fh;// 2e
LocalFileHeader lfh;
tDWORD dwOffsetToNextFile;
tDWORD dwEndOfDir;
tERROR error;
tDWORD how;
tDWORD dwTmp;

	if(dwFileSize < BIGBUFFERSIZE)
	{
		check(buffer, dwDataSize, hHash);
		return cTRUE;
	}
	error = CALL_IO_SeekReadTimed(io, &dwTmp, dwFileSize - BIGBUFFERSIZE, buffer, BIGBUFFERSIZE );
	if(PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
		return cFALSE;
	}
	// Find end record
	pEnd = NULL;
	for(i = BIGBUFFERSIZE - sizeof(EndRecord); i > sizeof(tDWORD); i--)
	{
		if(*(DWORD*)(&buffer[i]) == END_RECORD_SIG)	// Gotcha
		{
			pEnd = (EndRecord*)&buffer[i];
			break;
		}
	}
	if(pEnd)
	{
		dwOffsetToNextFile = 0;
		i = pEnd->offset_to_starting_directory;
		dwEndOfDir = i + pEnd->size_of_the_central_directory;
		dwOffsetToNextFile = 0;
		
		do
		{
			error = CALL_IO_SeekReadTimed(io, &dwTmp, i, &fh, sizeof(FileHeader));
			if(PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			check((tBYTE*)&fh, sizeof(FileHeader), hHash);
			i += sizeof(FileHeader);
			if(PR_FAIL(error = CalcFileHash(buffer, 0, i, i + fh.filename_length + fh.extra_field_length + fh.file_comment_length, io, hHash)))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			i += (fh.filename_length + fh.extra_field_length + fh.file_comment_length);

		// Read Local File data
			error = CALL_IO_SeekReadTimed(io, &dwTmp, dwOffsetToNextFile, &lfh, sizeof(LocalFileHeader));
			if(PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			check((tBYTE*)&lfh, sizeof(LocalFileHeader), hHash);

			dwOffsetToNextFile += sizeof(LocalFileHeader);
			if(PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToNextFile, dwOffsetToNextFile + lfh.filename_length + lfh.extra_field_length, io, hHash)))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			dwOffsetToNextFile += (lfh.filename_length + lfh.extra_field_length);

			if(fh.compressed_size < 0x1000 || fh.compression_method == STORED)	// Check whole file
			{
				if(PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToNextFile, dwOffsetToNextFile + fh.compressed_size, io, hHash)))
				{
					PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
					return cFALSE;
				}
			}
			else
			{
				if(PR_FAIL(error = CalcFileHashStrepped(buffer, 0, dwOffsetToNextFile, dwOffsetToNextFile + fh.compressed_size, 1024, 256, io, hHash)))
				{
					PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
					return cFALSE;
				}
				error = CALL_IO_SeekReadTimed(io, &how, dwOffsetToNextFile + fh.compressed_size - BUFFERSIZE, buffer, BUFFERSIZE);
				if(PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatZIP: Error reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, how, hHash);
			}
			dwOffsetToNextFile += fh.compressed_size;
		}while(i < (int)dwEndOfDir);
	}
	return TRUE;
}



/*************************************************** RAR new ******************************************/
//
//
/*************************************************** RAR new ******************************************/

#pragma pack(1)
typedef struct sBASE_HDR {
  tWORD  crc;         // crc - read from file
  tBYTE  type;        // type of header - read from file
  tWORD  flags;       // flags of header
  tWORD  size;        // size of header - still in file
  tUINT  hdr_size;
  tDWORD extra_size;
} tBASE_HDR;
#pragma pack()

#define LOW_HEADER_TYPE     0x72
#define HIGH_HEADER_TYPE    0x7B

#define  SIZEOF_SHORTBLOCKHEAD   7

#define  LONG_BLOCK         0x8000

#define  MHD_PASSWORD       0x0080

enum HEADER_TYPE
{
  MARK_HEAD     = 0x72,
  MAIN_HEAD     = 0x73,
  FILE_HEAD     = 0x74,
  COMM_HEAD     = 0x75,
  AV_HEAD       = 0x76,
  SUB_HEAD      = 0x77,
  PROTECT_HEAD  = 0x78,
  SIGN_HEAD     = 0x79,
  NEWSUB_HEAD   = 0x7a,
  ENDARC_HEAD   = 0x7b
};

tBYTE RarReadByte(hIO io, tQWORD * poffset, tBOOL * peof)
{
    tBYTE byte = 0;
    tERROR error;
    tDWORD dwRead;

    error = CALL_IO_SeekRead(io, &dwRead, *poffset, &byte, sizeof(tBYTE));

    if (dwRead != sizeof(tBYTE) && peof)
    {
        *peof = cTRUE;
    }

    *poffset += dwRead;

    return byte;
}
tWORD RarReadWord(hIO io, tQWORD * poffset, tBOOL * peof)
{
    tWORD word;
    word =  RarReadByte(io,poffset,peof);
    word |= RarReadByte(io,poffset,peof) << 8;

    return word;
}
tDWORD RarReadDword(hIO io, tQWORD * poffset, tBOOL * peof)
{
    tDWORD dword;
    dword =  RarReadByte(io,poffset,peof);
    dword |= RarReadByte(io,poffset,peof) << 8;
    dword |= RarReadByte(io,poffset,peof) << 16;
    dword |= RarReadByte(io,poffset,peof) << 24;

    return dword;
}

tERROR ReadBaseHdr(hIO io, tQWORD * poffset, tBOOL * peof, tBASE_HDR* base)
{
  tERROR  rcode;

  rcode = (errOK);
  memset(base, 0, sizeof(tBASE_HDR));
  base->crc = RarReadWord(io, poffset, peof);         // not need to calculate CRC

  if ( !*peof )
  {
    base->type  = RarReadByte(io, poffset, peof);
    base->flags = RarReadWord(io, poffset, peof);
    base->size  = RarReadWord(io, poffset, peof);

    if ( base->type < LOW_HEADER_TYPE || base->type > HIGH_HEADER_TYPE )
    {
      // header outside range
      rcode = (errOBJECT_INVALID);
    }
    else
    {
      base->hdr_size = SIZEOF_SHORTBLOCKHEAD;

      if ( base->flags & LONG_BLOCK )
      {
        // long block
        base->hdr_size += sizeof(tDWORD);
        base->extra_size = RarReadDword(io, poffset, peof);
      }
      else
      {
        base->extra_size = 0;
      }

      if ( base->hdr_size > base->size )
      {
        // invalid block size
        rcode = (errOBJECT_INVALID);
      }
    }
  }
  else
  {
    rcode = (errINTERFACE_INCOMPATIBLE);
  }
  
  return(rcode);
}



tBOOL IsFileFormatRAR(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
  tDWORD BCnt;
  tQWORD offset = 0;
  tQWORD nextblockoffset = 0;
  tDWORD MHDFlags = 0;
  tBASE_HDR  BHdr;
  tBYTE  Buff[0x20];
  tBOOL eof = cFALSE;


  // try parse all headers
  BCnt = 0;
  while(cTRUE)
  {
    offset = nextblockoffset;

    if ( (MHDFlags & MHD_PASSWORD) )
    {
      return cFALSE;
    }
    
    // read base block header
    if ( PR_FAIL(ReadBaseHdr(io, &offset, &eof, &BHdr)) )
    {
      break;
    }

    if (eof)
    {
      break;
    }

    // calc block hash
    memset(Buff, 0, sizeof(Buff));
    *(tWORD*) (&Buff[0])  = BHdr.crc;
    *(tBYTE*) (&Buff[2])  = BHdr.type;
    *(tWORD*) (&Buff[3])  = BHdr.flags;
    *(tWORD*) (&Buff[5])  = BHdr.size;
    *(tDWORD*)(&Buff[7])  = BHdr.hdr_size;
    *(tDWORD*)(&Buff[11]) = BHdr.extra_size;
    *(tDWORD*)(&Buff[15]) = (tDWORD)offset;
    *(tDWORD*)(&Buff[19]) = BCnt++;

    if (BCnt == 2) // Main header
    {
      MHDFlags = BHdr.flags;
    }

    check(&Buff[0], 0x20, hHash);

    if ( BHdr.type == FILE_HEAD || BHdr.type == NEWSUB_HEAD )
    {
      // additional hash for file blocks
      memset(Buff, 0, sizeof(Buff));
      *(tDWORD*)(&Buff[0])  = RarReadDword(io,&offset,&eof);
      *(tBYTE*) (&Buff[4])  = RarReadByte(io,&offset,&eof);
      *(tDWORD*)(&Buff[5])  = RarReadDword(io,&offset,&eof);
      *(tDWORD*)(&Buff[9])  = RarReadDword(io,&offset,&eof);
      *(tBYTE*) (&Buff[13]) = RarReadByte(io,&offset,&eof);
      *(tBYTE*) (&Buff[14]) = RarReadByte(io,&offset,&eof);
      *(tWORD*) (&Buff[15]) = RarReadWord(io,&offset,&eof);
      *(tDWORD*)(&Buff[17]) = RarReadDword(io,&offset,&eof);

      check(&Buff[0], 0x20, hHash);
    }

    if ( BHdr.type == ENDARC_HEAD )
      break; // done scan

    // go to next block
    nextblockoffset += (BHdr.size + BHdr.extra_size);
  }

  return cTRUE;
}



#if 0
typedef struct tag_RAR_DATA {
	NewMainArchiveHeader NewMhd;
	NewFileHeader NewLhd;
	BlockHeader BlockHead;
	tCHAR ArcFileName[MAX_PATH];
	tDWORD NextBlockPos;
	tDWORD CurBlockPos;
	tDWORD dwFileSize;
	tINT LastBlock;
	tDWORD dwPackDataPos;
} RAR_DATA;

tDWORD ReadHeader(hIO ArcPtr, int BlockType, RAR_DATA* data);
tDWORD ReadBlock(hIO ArcPtr, int BlockType, RAR_DATA* data);

tBOOL IsFileFormatRAR(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
tBOOL bSolid;
tDWORD how;
tERROR error;
RAR_DATA sRarData;
RAR_DATA* data = &sRarData;

	if(dwFileSize < SIZEOF_NEWMHD + SIZEOF_MARKHEAD + SIZEOF_NEWLHD)
	{
		PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatRAR: This is not RAR"));
		return cFALSE;
	}
	memset(data->ArcFileName, 0, sizeof(data->ArcFileName));
	memset(&data->NewMhd, 0, sizeof(NewMainArchiveHeader));
	memset(&data->NewLhd, 0, sizeof(NewFileHeader));
	memset(&data->BlockHead, 0, sizeof(BlockHeader));
	data->NextBlockPos = sizeof(MarkHeader);
	data->CurBlockPos = 0;
	data->dwFileSize = dwFileSize;
	data->LastBlock = 0;
	bSolid = cFALSE;

	ReadHeader(io, MAIN_HEAD, data);

	if(data->NewMhd.Flags & MHD_SOLID)
	{
		bSolid = cTRUE;
	}
	
	check((tBYTE*)&data->NewMhd, sizeof(NewMainArchiveHeader), hHash);

	data->NextBlockPos += data->NewMhd.HeadSize - SIZEOF_NEWMHD;
	while (1)
	{
		if(0 == ReadBlock(io, FILE_HEAD /*| READSUBBLOCK*/, data))
			break;
		check((tBYTE*)&data->NewLhd, sizeof(NewFileHeader), hHash);
		check((tBYTE*)&data->ArcFileName, data->NewLhd.NameSize, hHash);

		if((data->NewLhd.Flags & LHD_WINDOWMASK) == LHD_DIRECTORY)
		{
			continue;
		}
/*		PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatRAR: %s", data->ArcFileName));
		printf("Packed size %d\n", data->NewLhd.PackSize);
		printf("Unpacked size %d\n", data->NewLhd.UnpSize);
		printf("CRC %X\n", data->NewLhd.FileCRC);
		printf("Flags %X\n", data->NewLhd.Flags);*/

		if(data->NewLhd.Flags & LHD_SOLID)
		{
//			printf("\t LHD_SOLID\n");
			error = CALL_IO_SeekReadTimed(io, &how, data->dwPackDataPos, buffer, (data->NewLhd.PackSize > BUFFERSIZE ? BUFFERSIZE : data->NewLhd.PackSize));
			if(PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatRAR: Error reading file err=%08X", error));
				return cFALSE;
			}
			check(buffer, how, hHash);
		}
		else
		{
			if(data->NewLhd.PackSize < 0x1000 || data->NewLhd.Method == 0x30)	// Check whole file
			{
				if(PR_FAIL(error = CalcFileHash(buffer, 0, data->dwPackDataPos, data->dwPackDataPos + data->NewLhd.PackSize, io, hHash)))
				{
					PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatRAR: Error reading file err=%08X", error));
					return cFALSE;
				}
			}
			else
			{
				if(PR_FAIL(error = CalcFileHashStrepped(buffer, 0, data->dwPackDataPos, data->dwPackDataPos + data->NewLhd.PackSize, 1024, 256, io, hHash)))
				{
					PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatRAR: Error reading file err=%08X", error));
					return cFALSE;
				}
				error = CALL_IO_SeekReadTimed(io, &how, data->dwPackDataPos + data->NewLhd.PackSize - BUFFERSIZE, buffer, BUFFERSIZE);
				if(PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "ichk\tIsFileFormatRAR: Error reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, how, hHash);
			}
		}
		
		
		/*		if (data->BlockHead.HeadType == SUB_HEAD)
		{
			_llseek(hf,data->NextBlockPos,SEEK_SET);
			continue;
		}*/
		
	}
	return cTRUE;
}

tDWORD ReadBlock(hIO ArcPtr, int BlockType, RAR_DATA* data)
{
NewFileHeader SaveFileHead;
tDWORD Size;
int ReadSubBlock;
tDWORD dwNamePos;

	memcpy(&SaveFileHead,&data->NewLhd,sizeof(SaveFileHead));
	ReadSubBlock = 0;
	if (BlockType & READSUBBLOCK)
		ReadSubBlock = 1;
	
	BlockType &= 0xff;
	
	while (1)
	{
		data->CurBlockPos = data->NextBlockPos;
		Size = ReadHeader(ArcPtr, FILE_HEAD, data);
		dwNamePos = data->NextBlockPos;
		if (Size != 0)
		{
			if (data->NewLhd.HeadSize < SIZEOF_SHORTBLOCKHEAD)
				return 0;
			data->NextBlockPos = data->CurBlockPos + data->NewLhd.HeadSize;
			if (data->NewLhd.Flags & LONG_BLOCK)
			{
				data->dwPackDataPos = data->NextBlockPos;
				data->NextBlockPos += data->NewLhd.PackSize;
			}
			if (data->NextBlockPos <= data->CurBlockPos)
				return 0;
		}
		else
		{
			if (data->dwFileSize < data->NextBlockPos)
				return 0;
		}
		if (Size > 0 && BlockType!=SUB_HEAD)
			data->LastBlock = BlockType;
		if (Size==0 || BlockType==ALL_HEAD || data->NewLhd.HeadType==BlockType ||
			(data->NewLhd.HeadType==SUB_HEAD && ReadSubBlock && data->LastBlock==BlockType))
			break;
		//_llseek(ArcPtr,data->NextBlockPos,SEEK_SET);
	}

	data->BlockHead.HeadCRC = data->NewLhd.HeadCRC;
	data->BlockHead.HeadType = data->NewLhd.HeadType;
	data->BlockHead.Flags = data->NewLhd.Flags;
	data->BlockHead.HeadSize = data->NewLhd.HeadSize;
	data->BlockHead.DataSize = data->NewLhd.PackSize;
	
	if (BlockType != data->NewLhd.HeadType)
		BlockType = ALL_HEAD;
	switch(BlockType)
	{
	    case FILE_HEAD:
			if (Size>0)
			{
				data->NewLhd.NameSize = Min(data->NewLhd.NameSize,MAX_PATH-1);

				CALL_IO_SeekReadTimed(ArcPtr, &Size, dwNamePos, data->ArcFileName, data->NewLhd.NameSize);
				data->ArcFileName[data->NewLhd.NameSize]=0;
				Size += data->NewLhd.NameSize;
//				_llseek(ArcPtr,NextBlockPos,SEEK_SET);
				//ConvertUnknownHeader();
			}
			break;
		default:
			memcpy(&data->NewLhd, &SaveFileHead,sizeof(NewFileHeader));
			data->NextBlockPos = data->CurBlockPos;
//			_llseek(ArcPtr,CurBlockPos,SEEK_SET);
			break;
	}
	return(Size);
}


tDWORD ReadHeader(hIO ArcPtr, int BlockType, RAR_DATA* data)
{
tDWORD Size;
BYTE Header[64];
tERROR error;
tDWORD dwTmp;
	
	error = CALL_IO_SeekReadTimed(ArcPtr, &Size, data->NextBlockPos, Header , sizeof(Header));
	if(error)
	{
		return 0;
	}
	switch(BlockType)
	{
	case MAIN_HEAD:
		data->NewMhd.HeadCRC = GetHeaderWord(0);
		data->NewMhd.HeadType = GetHeaderByte(2);
		data->NewMhd.Flags = GetHeaderWord(3);
		data->NewMhd.HeadSize = GetHeaderWord(5);
		data->NewMhd.Reserved = GetHeaderWord(7);
		data->NewMhd.Reserved1 = GetHeaderDword(9);
		Size = SIZEOF_NEWMHD;
		break;
	case FILE_HEAD:
		Size = SIZEOF_NEWLHD;
		data->NewLhd.HeadCRC = GetHeaderWord(0);
		data->NewLhd.HeadType = GetHeaderByte(2);
		data->NewLhd.Flags = GetHeaderWord(3);
		data->NewLhd.HeadSize = GetHeaderWord(5);
		data->NewLhd.PackSize = GetHeaderDword(7);
		data->NewLhd.UnpSize = GetHeaderDword(11);
		data->NewLhd.HostOS = GetHeaderByte(15);
		data->NewLhd.FileCRC = GetHeaderDword(16);
		data->NewLhd.FileTime = GetHeaderDword(20);
		data->NewLhd.UnpVer = GetHeaderByte(24);
		data->NewLhd.Method = GetHeaderByte(25);
		data->NewLhd.NameSize = GetHeaderWord(26);
		data->NewLhd.FileAttr = GetHeaderDword(28);
		break;
	case ALL_HEAD:
		Size = SIZEOF_SHORTBLOCKHEAD;
		data->BlockHead.HeadCRC = GetHeaderWord(0);
		data->BlockHead.HeadType = GetHeaderByte(2);
		data->BlockHead.Flags = GetHeaderWord(3);
		data->BlockHead.HeadSize = GetHeaderWord(5);
		if (data->BlockHead.Flags & LONG_BLOCK)
		{
			error = CALL_IO_SeekReadTimed(ArcPtr, &dwTmp, data->NextBlockPos + Size, &Header[7], 4);
			Size += 4;
			data->BlockHead.DataSize = GetHeaderDword(7);
		}
		break;
	}
	data->NextBlockPos += Size;
	return Size;
}
#endif

/*************************************************** OLE2 ************************************************/
// Based on directory stream data. 
/*************************************************** OLE2 ************************************************/


typedef struct tag_OLECtx {
	hIO io;
	OLE_HEADER header;
	SECT**     FAT[109];
} OLECtx;

#define CLUSTER_SIZE 0x200


tERROR OLEReadCluster(OLECtx* pOleCtx, tDWORD dwCluster, tBYTE* pBuffer, tDWORD dwBufferSize)
{
	tERROR error;
	tDWORD dwOffset;
	tDWORD dwBytesRead;

	if (pBuffer == NULL || dwBufferSize!=CLUSTER_SIZE)
		return errPARAMETER_INVALID;

	if (dwCluster >= DIFSECT)
		return errUNEXPECTED;
	dwOffset = (dwCluster + 1) * CLUSTER_SIZE; 
	error = CALL_IO_SeekReadTimed(pOleCtx->io, &dwBytesRead, dwOffset, pBuffer, dwBufferSize);
	return error;
}

tERROR OLEGetNextCluster(OLECtx* pOleCtx, tDWORD dwCluster, tDWORD* pdwNextCluster)
{
	tERROR error = errOK;
	tDWORD dwFATCluster;

	// check boundary
	dwFATCluster = dwCluster / (CLUSTER_SIZE / sizeof(SECT));
	if (dwFATCluster >= countof(pOleCtx->header._sectFat))
		return errOUT_OF_OBJECT;

	// fill FAT cache
	if (pOleCtx->FAT[dwFATCluster] == NULL)
	{
		if (PR_FAIL(error = CALL_SYS_ObjHeapAlloc(pOleCtx->io, (tPTR*)&pOleCtx->FAT[dwFATCluster], CLUSTER_SIZE)))
			return error;
		if (PR_FAIL(error = OLEReadCluster(pOleCtx, pOleCtx->header._sectFat[dwFATCluster], (tBYTE*)pOleCtx->FAT[dwFATCluster], CLUSTER_SIZE)))
			return error;
	}

	// get next entry
	if (pdwNextCluster)
		*pdwNextCluster = (tDWORD) pOleCtx->FAT[dwFATCluster][dwCluster % (CLUSTER_SIZE / sizeof(SECT))];

	return errOK;
}

tBOOL IsFileFormatOLE(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, hHASH hHash)
{
	tERROR error = errOK;
	tDWORD dwClusterSize;
	OLECtx sOleCtx;
	OLE_HEADER* pHeader = NULL;
	tDWORD dwCluster;
	tDWORD dwClustersRead = 0;
	tDWORD i;

	// file too small
	if (dwDataSize < sizeof(OLE_HEADER) )
		return cFALSE;
	pHeader = (OLE_HEADER*)buffer;

	// check file signatue
	if ((memcmp(&pHeader->_abSig, bOLE2Sign1, sizeof(bOLE2Sign1)) != 0) &&  
		(memcmp(&pHeader->_abSig, bOLE2Sign2, sizeof(bOLE2Sign2)) != 0))
		return cFALSE;

	// check DIF
	if (pHeader->_sectDifStart != ENDOFCHAIN) // имеется Double-Indirect Fat (файл > 7 MB)
		return cFALSE;

	// prepare context
	memset(&sOleCtx, 0, sizeof(sOleCtx));
	memcpy(&sOleCtx.header, buffer, sizeof(OLE_HEADER));
	sOleCtx.io = io;

	// calculate claster size
	dwClusterSize = 1 << pHeader->_uSectorShift;
	if (dwClusterSize != CLUSTER_SIZE)
		return cFALSE;
	if (dwFileSize % dwClusterSize != 0) // размер файла должен быть кратен размеру кластера
		return cFALSE;
	
	// calc file header (just to be sure)
	check((tBYTE*)pHeader, sizeof(OLE_HEADER), hHash); 

	// read directory
	dwCluster = pHeader->_sectDirStart;
	do 
	{
		tBYTE bData[CLUSTER_SIZE];
		if (PR_FAIL(error = OLEReadCluster(&sOleCtx, dwCluster, bData, sizeof(bData))))
			break;
		check(&bData[0], sizeof(bData), hHash);
		if (PR_FAIL(error = OLEGetNextCluster(&sOleCtx, dwCluster, &dwCluster)))
			break;
		if (dwClustersRead++ == 0x10)
		{
			error = errOUT_OF_OBJECT; // directory is too large - stop processing
			break;
		}
	} while (dwCluster != ENDOFCHAIN);

	// free FAT cache
	for (i=0; i<countof(sOleCtx.FAT); i++)
	{
		if (sOleCtx.FAT[i])
		{
			CALL_SYS_ObjHeapFree(sOleCtx.io, sOleCtx.FAT[i]);
			sOleCtx.FAT[i] = NULL;
		}
	}
	
	if (PR_FAIL(error))
		return cFALSE;

	if (PR_FAIL(CalcFileAttributesIntoHash(io, hHash)))
		return cFALSE;
	
	return cTRUE;
}

tERROR CalcFileAttributesIntoHash(hIO io, hHASH hHash)
{
	tERROR error = errOK;
	tDWORD dwLength;
	tDATETIME dt;
	tDWORD dwAttributes;
	tQWORD qwSize;

	error = CALL_SYS_PropertyGet(io, &dwLength, pgOBJECT_CREATION_TIME, &dt, sizeof(dt));
	if (PR_SUCC(error))
		error = CALL_Hash_Update(hHash, (tBYTE*)&dt, sizeof(dt));

	if (PR_SUCC(error))
		error = CALL_SYS_PropertyGet(io, &dwLength, pgOBJECT_LAST_WRITE_TIME, &dt, sizeof(dt));
	if (PR_SUCC(error))
		error = CALL_Hash_Update(hHash, (tBYTE*)&dt, sizeof(dt));
	
	if (PR_SUCC(error))
		error = CALL_SYS_PropertyGet(io, &dwLength, pgOBJECT_ATTRIBUTES, &dwAttributes, sizeof(dwAttributes));
	if (PR_SUCC(error))
		error = CALL_Hash_Update(hHash, (tBYTE*)&dwAttributes, sizeof(dwAttributes));

	if (PR_SUCC(error))
		error = CALL_IO_GetSize(io, &qwSize, IO_SIZE_TYPE_EXPLICIT);
	if (PR_SUCC(error))
		error = CALL_Hash_Update(hHash, (tBYTE*)&qwSize, sizeof(qwSize));

	return error;
}

