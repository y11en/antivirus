// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  14 April 2004,  12:00 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- IWorms Generic Detection
// Author      -- Pavlushchik
// File Name   -- converter.cpp
// -------------------------------------------
// AVP Prague stamp end



#define UNI_MAX_PATH 64*1024 // 32K unicode chars
#define BUFF_SIZE 0x400

#define npMAILER_PID                   ((tSTRING)("MAILER_PID")) // PID локального процесса, инициализировавшего соединение

#define OUTPUT_SIGNATURE               ((tSTRING)("\xBA\xD0\xF1\x1E\x20IWGDv1.0")) 

#define _CRTIMP // override __declspec(dllimport) for runtime functions

#include <windows.h>
#include <psapi.h>
#include <wchar.h>

extern DWORD (WINAPI *g_pGetModuleFileNameExW)(HANDLE hProcess,  HMODULE hModule,  LPWSTR lpFilename,  DWORD nSize);

// AVP Prague stamp begin( Interface version,  )
#define Converter_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_msg.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_root.h>

#include <Extract/plugin/p_iwgen.h>

#include <hash/md5/plugin_hash_md5.h>
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable IWGen : public cConverter /*cObjImpl*/ {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();

// Property function declarations

public:
// External function declarations
	tERROR pr_call Convert( hOBJECT* result, hOBJECT p_hInputObj );

// Data declaration
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(IWGen)
};
// AVP Prague stamp end



size_t __cdecl wcslen (
        const wchar_t * wcs
        )
{
        const wchar_t *eos = wcs;

        while( *eos++ ) ;

        return( (size_t)(eos - wcs - 1) );
}


// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Converter". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR IWGen::ObjectInit() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Converter::ObjectInit method" );


	if (g_pGetModuleFileNameExW == NULL)
	{
		PR_TRACE((this, prtERROR, "iwg\tIWGen::Convert - load failed, GetModuleFileNameExW not resolved"));
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - loaded"));

	return error;
}
// AVP Prague stamp end




tERROR CreateIO(hOBJECT hParent, hIO* result, tWCHAR* wcszFileName, tDWORD dwOpenMode, tDWORD dwAccessMode)
{
	tERROR err;
	hIO hIo = NULL;
	
	if (hParent==NULL || result==NULL || wcszFileName==NULL)
		return errPARAMETER_INVALID;
	
	err = hParent->sysCreateObject((hOBJECT*)&hIo, IID_IO, PID_NATIVE_FIO);
	if (PR_SUCC(err))
	{
		err = hIo->set_pgOBJECT_OPEN_MODE(dwOpenMode);
		if (PR_SUCC(err))
			err = hIo->set_pgOBJECT_ACCESS_MODE(dwAccessMode);
		if (PR_SUCC(err))
			err = hIo->set_pgOBJECT_FULL_NAME(wcszFileName, (tDWORD)((wcslen(wcszFileName) + 1)*sizeof(tWCHAR)), cCP_UNICODE);
		if (PR_SUCC(err))
			err = hIo->sysCreateObjectDone();
		if (PR_FAIL(err))
		{
			hIo->sysCreateObjectDone();
			PR_TRACE((hParent, prtERROR, "CreateIO: IO create failed with %terr for file '%S'", err, wcszFileName));
			return err;
		}
	}
	
	*result = hIo;
	
	return err;
}

tBOOL IsEqualFileData(hIO hFile1, tDWORD dwSize1,
					  hIO hFile2, tDWORD dwSize2)
{
	tBYTE  bBuff1[BUFF_SIZE], bBuff2[BUFF_SIZE];
	tDWORD dwRead, dwSize;
	tDWORD dwBytes1, dwBytes2;
	tQWORD qwPos = 0;

	if (dwSize1 != dwSize2 || dwSize1 == 0)
		return cFALSE;
	dwSize = dwSize1;
	while (dwSize)
	{
		dwRead = min(BUFF_SIZE, dwSize);
		hFile1->SeekRead(&dwBytes1, qwPos, bBuff1, dwRead);
		hFile2->SeekRead(&dwBytes2, qwPos, bBuff2, dwRead);
		if (dwBytes1 != dwRead || dwBytes2 != dwRead)
			return cFALSE;
		if (memcmp(bBuff1, bBuff2, dwRead) != 0)
			return cFALSE;
		dwSize -= dwRead;
		qwPos  += dwRead;
	};

	return cTRUE;
}


tBOOL GetImageSize(hIO hFile, tDWORD* pdwImageSize)
{
	tBYTE pBuff[BUFF_SIZE];
	IMAGE_DOS_HEADER* pImageDosHeader = (IMAGE_DOS_HEADER*)pBuff;
	tWORD* pWordBuffer = (tWORD*)pBuff;
	tDWORD dwBytes;
	tDWORD dwImageSize = 0;

	if (PR_FAIL(hFile->SeekRead(&dwBytes, 0, pBuff, BUFF_SIZE)))
		return cFALSE;

	if (dwBytes < sizeof(IMAGE_DOS_HEADER))
		return cFALSE;

	if( pImageDosHeader->e_magic!=0x4D5A/*'MZ'*/ && pImageDosHeader->e_magic!=0x5A4D /*'ZM'*/)
		return cFALSE;

	if( pWordBuffer[0xC] != 0x40) //> 0x3F)	// May be NE or PE ? (LX LE)
		return cFALSE;

	{
		tDWORD dwHeaderOffset = (tDWORD)(pImageDosHeader->e_lfanew);
		tDWORD dwSectionOffset;
		PIMAGE_NT_HEADERS pPE = (PIMAGE_NT_HEADERS)pBuff;
		PIMAGE_SECTION_HEADER pSH;
		tWORD wSections,i;

		if (PR_FAIL(hFile->SeekRead(&dwBytes, dwHeaderOffset, pBuff, sizeof(IMAGE_NT_HEADERS))))
			return cFALSE;

		if (dwBytes < sizeof(IMAGE_NT_HEADERS))
			return cFALSE;
		
		wSections = pPE->FileHeader.NumberOfSections;
		dwBytes = 0;
		dwSectionOffset = dwHeaderOffset
						  + sizeof(DWORD)                  // Signature
						  + sizeof(IMAGE_FILE_HEADER)
						  + pPE->FileHeader.SizeOfOptionalHeader;
		for(i=0; i<wSections; i++)
		{
			if (dwBytes < sizeof(IMAGE_SECTION_HEADER))
			{
				if (PR_FAIL(hFile->SeekRead(&dwBytes, dwSectionOffset, pBuff, min(wSections*sizeof(IMAGE_SECTION_HEADER), (BUFF_SIZE/sizeof(IMAGE_SECTION_HEADER))*sizeof(IMAGE_SECTION_HEADER)))))
					return cFALSE;
				if (dwBytes < sizeof(IMAGE_SECTION_HEADER))
					return cFALSE;
				dwSectionOffset+=dwBytes;
				pSH = (PIMAGE_SECTION_HEADER)pBuff;
			}

			if(pSH->PointerToRawData + pSH->SizeOfRawData > dwImageSize)
				dwImageSize = pSH->PointerToRawData + pSH->SizeOfRawData;
			
			pSH++;
			dwBytes -= sizeof(IMAGE_SECTION_HEADER);
		}
	}

	*pdwImageSize = dwImageSize;

	return cTRUE;
}

tBOOL IsEqualImages(hIO hFile1, hIO hFile2)
{
	tDWORD dwSize1, dwSize2;

	if (!GetImageSize(hFile1, &dwSize1))
		return cFALSE;
	if (!GetImageSize(hFile2, &dwSize2))
		return cFALSE;

	return IsEqualFileData(hFile1, dwSize1, hFile2, dwSize2);
}



// AVP Prague stamp begin( External (user called) interface method implementation, Convert )
// Behaviour comment
//    Обычно реализация метода создает TemporaryIO и помещает в него преобразованный поток данных. TemporaryIO возвращается  в параметре result.
// Parameters are:
tERROR IWGen::Convert( hOBJECT* result, hOBJECT p_hInputObj ) {
	
	tERROR  error = errOK;
	hOBJECT ret_val = NULL;

	HANDLE hProcess;
	DWORD  pid;
	tWCHAR wcsName[0x100];
	tWCHAR *pwcsName = &wcsName[0];
	DWORD  dwLen;
	hIO    hFileIO;
	tBYTE  bMZSig[2];
	tDWORD dwBytes;
	hIO    hInputIO;
	hIO    hTempIO = NULL;
	static tPROPID propSenderPID = 0;
	tQWORD qwWritePos = 0;
	tBOOL  bEqual;
	
	PR_TRACE_FUNC_FRAME( "Converter::Convert method" );

	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert"));

	if (!result || !p_hInputObj)
		return errPARAMETER_INVALID;

	if (PR_FAIL(error = sysCheckObject(p_hInputObj, IID_IO)))
	{
		PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - object incompatible"));
		return error;
	}

	hInputIO = (hIO)p_hInputObj;

	if (!propSenderPID)
	{
		error = sysGetRoot()->RegisterCustomPropId(&propSenderPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);
		if (PR_FAIL(error))
			return error;
	}

	pid = p_hInputObj->propGetDWord(propSenderPID);
	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - Sender PID=%d", pid));
	if (pid == 0)
		return errNOT_OK;

	error = hInputIO->SeekRead(&dwBytes, 0, &bMZSig, sizeof(bMZSig));
	if (PR_FAIL(error))
		return error;

	if (dwBytes == sizeof(bMZSig) &&
		((bMZSig[0] == 'M' && bMZSig[1] == 'Z')
		|| (bMZSig[0] == 'Z' && bMZSig[1] == 'M')))
		;
	else
	{
		PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - this is not MZ file"));
		return errNOT_OK;
	}


	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!hProcess)
	{
		PR_TRACE((this, prtERROR, "iwg\tIWGen::Convert - Cannot open process by PID=%d", pid));
		return errUNEXPECTED;
	}

	dwLen = g_pGetModuleFileNameExW(hProcess, NULL, pwcsName, countof(wcsName));
	if (dwLen == 0)
	{
		PR_TRACE((this, prtERROR, "iwg\tIWGen::Convert - Cannot retrieve modulename by PID=%d", pid));
		CloseHandle(hProcess);
		return errUNEXPECTED;
	}
	
	if ( countof(wcsName) == dwLen) // buffer to small
	{
		 error = heapAlloc((tPTR*)&pwcsName, UNI_MAX_PATH);
		 if (PR_FAIL(error))
		 {
			 CloseHandle(hProcess);
			 return error;
		 }
		 if (!g_pGetModuleFileNameExW(hProcess, NULL, pwcsName, UNI_MAX_PATH))
		 {
			 CloseHandle(hProcess);
			 heapFree(pwcsName);
			 return errUNEXPECTED;
		 }
	}
	//graf 06.10.2004
	CloseHandle(hProcess);

	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - PID=%d, Module %S", pid, pwcsName));
	error = CreateIO(*this, &hFileIO, pwcsName, fOMODE_OPEN_IF_EXIST , fACCESS_READ);

	if (pwcsName != &wcsName[0]) // free allocated memory
		heapFree(pwcsName);

	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "iwg\tIWGen::Convert - Cannot create IO for Module %S", pwcsName));
		return error;
	}
	
	bEqual = IsEqualImages(hFileIO, hInputIO);
	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - IsEqualImages() returned %d, Module %S", bEqual, pwcsName));

	hFileIO->sysCloseObject();

	if (!bEqual)
		return errNOT_OK;

	{
		// notify PDM about detected process
		tDWORD size = sizeof(pid);
		sysSendMsg(pmc_REMOTE_GLOBAL, pm_IWGEN_SELF_MAILER_DETECTED, NULL, &pid, &size);
	}

	error = hInputIO->sysCreateObjectQuick((hOBJECT*)&hTempIO, IID_IO, PID_TMPFILE);
	if (PR_FAIL(error))
		return errNOT_OK;

	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - detected, creating 'special' file, Module %S", pwcsName));

	error = hTempIO->SeekWrite(&dwBytes, qwWritePos, OUTPUT_SIGNATURE, (tDWORD)strlen(OUTPUT_SIGNATURE));
	qwWritePos += dwBytes;
	if (PR_SUCC(error))
	{
		const char fill[0x10] = "\0\0\0\0\0\0\0\0\0\0";
		if (dwBytes < 0x10)
			error = hTempIO->SeekWrite(&dwBytes, qwWritePos, (tPTR)&fill[0], 0x10 - dwBytes);
		qwWritePos += dwBytes;
	}

	if (PR_SUCC(error))
	{
		hHASH hHashMD5;
		tBYTE bHash[0x10];
		if (PR_SUCC(error))
			error = sysCreateObjectQuick((hOBJECT*)&hHashMD5, IID_HASH, PID_HASH_MD5);
		if (PR_SUCC(error))
		{
			// calc MD5 hash of image
			tBYTE  buff[BUFF_SIZE];
			tQWORD qwReadPos = 0;
			tDWORD dwRead;
			tDWORD dwBytes;
			tDWORD dwSize;

			if (!GetImageSize(hInputIO, &dwSize))
				error = errUNEXPECTED;
			while (PR_SUCC(error) && dwSize)
			{
				dwRead = min(BUFF_SIZE, dwSize);
				error = hInputIO->SeekRead(&dwBytes, qwReadPos, buff, dwRead);
				if (dwBytes != dwRead)
					error = errOBJECT_READ;
				if (PR_SUCC(error))
					error = hHashMD5->Update(buff, dwBytes);
				dwSize -= dwRead;
				qwReadPos  += dwRead;
			};
			if (PR_SUCC(error))
				error = hHashMD5->GetHash(bHash, sizeof(bHash));
			hHashMD5->sysCloseObject();
			if (PR_SUCC(error))
			{
				error = hTempIO->SeekWrite(&dwBytes, qwWritePos, bHash, sizeof(bHash));
				qwWritePos += dwBytes;
			}
		}
	}

	if (PR_FAIL(error))
	{
		hTempIO->sysCloseObject();
		return error;
	}

	PR_TRACE((this, prtNOTIFY, "iwg\tIWGen::Convert - 'special' file created, Module %S", pwcsName));

	*result = (hOBJECT)hTempIO;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Converter". Register function
tERROR IWGen::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Converter_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Converter_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
mpPROPERTY_TABLE_END(Converter_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Converter)
	mINTERNAL_METHOD(ObjectInit)
mINTERNAL_TABLE_END(Converter)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Converter)
	mEXTERNAL_METHOD(Converter, Convert)
mEXTERNAL_TABLE_END(Converter)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Converter::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_CONVERTER,                          // interface identifier
		PID_IWGEN,                              // plugin identifier
		0x00000000,                             // subtype identifier
		Converter_VERSION,                      // interface version
		VID_MIKE,                               // interface developer
		&i_Converter_vtbl,                      // internal(kernel called) function table
		(sizeof(i_Converter_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Converter_vtbl,                      // external function table
		(sizeof(e_Converter_vtbl)/sizeof(tPTR)),// external function table size
		Converter_PropTable,                    // property table
		mPROPERTY_TABLE_SIZE(Converter_PropTable),// property table size
		sizeof(IWGen)-sizeof(cObjImpl),         // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Converter(IID_CONVERTER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Converter_Register( hROOT root ) { return IWGen::Register(root); }
// AVP Prague stamp end



