// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  07 March 2003,  19:02 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- ichecker.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IChecker_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <windows.h>
#include <prague.h>
#include "ichecker.h"
#include "iface/i_string.h"
#include "iface/i_hash.h"
#include "../../hash/md5/plugin_hash_md5.h"
#include "string.h"
#include <iface/e_ktime.h>
#include <iface/e_loader.h>
#include "../../nfio/win32_io.h"
// #include "sfc_api.h"
// AVP Prague stamp end



#define KAVICHS ":KAVICHS"
#define MAX_BUFFER_SIZE 0x100
#define ICS_BLOCK_SIGNATURE '1SCI' 
#define ICS_BLOCK_END 0 

typedef struct tag_ICS_DATA {
	tDWORD dwBlockSignature;
	tDWORD dwBlockDataSize;
	tBYTE  bHash[8];
	// tBYTE bData[0];
} ICS_DATA;

static tPROPID propICS_HASH = 0;
extern OSVERSIONINFO os;

// AVP Prague stamp begin( Interface comment,  )
// IChecker interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IChecker. Inner data structure


// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IChecker 
{
	const iICheckerVtbl* vtbl; // pointer to the "IChecker" virtual table
	const iSYSTEMVTBL*   sys;  // pointer to the "SYSTEM" virtual table
} *hi_IChecker;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IChecker_ObjectInitDone( hi_IChecker _this );
tERROR pr_call IChecker_ObjectPreClose( hi_IChecker _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IChecker_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   IChecker_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   IChecker_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpIChecker_GetStatus)     ( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ); // -- Возвращает данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpIChecker_UpdateStatus)  ( hi_IChecker _this, hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ); // -- Устанавливает данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpIChecker_DeleteStatus)  ( hi_IChecker _this, hIO io );          // -- Удаляет данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpIChecker_FlushStatusDB) ( hi_IChecker _this );                  // -- Сохраняет БД ассоциированных данных;
typedef   tERROR (pr_call *fnpIChecker_ClearStatusDB) ( hi_IChecker _this );                  // -- Очищает БД ассоциированных данных.;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iICheckerVtbl 
{
	fnpIChecker_GetStatus      GetStatus;
	fnpIChecker_UpdateStatus   UpdateStatus;
	fnpIChecker_DeleteStatus   DeleteStatus;
	fnpIChecker_FlushStatusDB  FlushStatusDB;
	fnpIChecker_ClearStatusDB  ClearStatusDB;
}; // "IChecker" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call IChecker_GetStatus( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize );
tERROR pr_call IChecker_UpdateStatus( hi_IChecker _this, hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize );
tERROR pr_call IChecker_DeleteStatus( hi_IChecker _this, hIO io );
tERROR pr_call IChecker_FlushStatusDB( hi_IChecker _this );
tERROR pr_call IChecker_ClearStatusDB( hi_IChecker _this );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iICheckerVtbl e_IChecker_vtbl = 
{
	IChecker_GetStatus,
	IChecker_UpdateStatus,
	IChecker_DeleteStatus,
	IChecker_FlushStatusDB,
	IChecker_ClearStatusDB
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IChecker". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IChecker_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IChecker_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IChecker_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "iChecker Streams", 17 )
mPROPERTY_TABLE_END(IChecker_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IChecker". Register function
tERROR pr_call IChecker_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter IChecker::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_ICHECKER2,                          // interface identifier
		PID_ICHSTRMS,                           // plugin identifier
		0x00000000,                             // subtype identifier
		IChecker_VERSION,                       // interface version
		VID_MIKE,                               // interface developer
		&i_IChecker_vtbl,                       // internal(kernel called) function table
		(sizeof(i_IChecker_vtbl)/sizeof(tPTR)), // internal function table size
		&e_IChecker_vtbl,                       // external function table
		(sizeof(e_IChecker_vtbl)/sizeof(tPTR)), // external function table size
		IChecker_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(IChecker_PropTable),// property table size
		0,                                      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IChecker(IID_ICHECKER2) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave IChecker::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IChecker_ObjectInitDone( hi_IChecker _this )
{
	tERROR error = errOK;
	hROOT  hRoot;
	PR_TRACE_A0( _this, "Enter IChecker::ObjectInitDone method" );

	if (propICS_HASH == 0)
	{
		hRoot = (hROOT)CALL_SYS_ParentGet(_this, IID_ROOT);
		error = CALL_Root_RegisterCustomPropId(hRoot, &propICS_HASH, cICS_PROP_HASH, pTYPE_BINARY);
	}

	PR_TRACE_A1( _this, "Leave IChecker::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call IChecker_ObjectPreClose( hi_IChecker _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IChecker::ObjectPreClose method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave IChecker::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



tERROR iCreateStreamIoFromObj(hi_IChecker _this, hOBJECT hObject, tDWORD dwOpenMode, hIO* pIo)
{
	tERROR error;
	hSTRING hStreamName = NULL;
	hIO hIo;

	if (!pIo)
		return errPARAMETER_INVALID;

	*pIo = NULL;

	if (PR_SUCC(error = CALL_SYS_ObjectCheck(_this, hObject, IID_IO, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		if (PR_FAIL(error = CALL_SYS_ObjectCreateQuick(hObject, &hStreamName, IID_STRING, PID_ANY, SUBTYPE_ANY)))
			return error;
		CALL_String_SetCP(hStreamName, cCP_UNICODE);
		if (PR_FAIL(error = CALL_String_ImportFromProp(hStreamName, NULL, (hOBJECT)hStreamName, pgOBJECT_FULL_NAME)))
		{
			CALL_SYS_ObjectClose(hStreamName);
			return error;
		}
	}
	else
	if (PR_SUCC(error = CALL_SYS_ObjectCheck(_this, hObject, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE)))
	{
		if (PR_FAIL(error = CALL_SYS_ObjectCreateQuick(hObject, &hStreamName, IID_STRING, PID_ANY, SUBTYPE_ANY)))
			return error;
		if (PR_FAIL(error = CALL_String_ImportFromStr(hStreamName, NULL, (hSTRING)hObject, cSTRING_WHOLE)))
		{
			CALL_SYS_ObjectClose(hStreamName);
			return error;
		}
	}
	else
		return errOBJECT_INCOMPATIBLE;

	if (PR_FAIL(error = CALL_String_AddFromBuff(hStreamName, NULL, KAVICHS, 0, cCP_ANSI, cSTRING_Z)))
	{
		CALL_SYS_ObjectClose(hStreamName);
		return error;
	}

	if (PR_SUCC(error = CALL_SYS_ObjectCreate(hObject, &hIo, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY)))
	{
		error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, dwOpenMode);
		if (PR_SUCC(error))
			error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_ACCESS_MODE, fACCESS_READ | fACCESS_WRITE);
		if (PR_SUCC(error))
			error = CALL_String_ExportToProp(hStreamName, NULL, cSTRING_WHOLE, (hOBJECT) hIo, pgOBJECT_NAME);
		if (PR_SUCC(error))
			error = CALL_SYS_ObjectCreateDone(hIo);	
		if (PR_FAIL(error))
			CALL_SYS_ObjectClose(hIo);
	}
	CALL_SYS_ObjectClose(hStreamName);

	if (PR_FAIL(error))
		return error;

	if (pIo)
		*pIo = hIo;

	return error;
}

tERROR iGetICSDataFromStreamData(hi_IChecker _this, tBYTE* pHash, tBYTE* pStreamData, tDWORD dwStreamSize, ICS_DATA** ppICSData)
{
	ICS_DATA* pICData;
	tDWORD dwICDataOffset = 0;
	
	if (ppICSData)
		*ppICSData = NULL;

	while (dwICDataOffset != dwStreamSize)
	{
		if (dwICDataOffset + sizeof(tDWORD) >= dwStreamSize || *(tDWORD*)(pStreamData + dwICDataOffset)==ICS_BLOCK_END)
			return errOBJECT_NOT_FOUND;
		if (dwICDataOffset + sizeof(ICS_DATA) + 1 > dwStreamSize) // invalid data
			return errOBJECT_DATA_SIZE_UNDERSTATED;
		pICData = (ICS_DATA*)(pStreamData+dwICDataOffset);
		if (pICData->dwBlockSignature == 0) // invalidated stream
			return errOBJECT_NOT_FOUND;
		if (pICData->dwBlockSignature != ICS_BLOCK_SIGNATURE)
			return errOBJECT_INVALID;
		
		if (memcmp(pICData->bHash, pHash, sizeof(pICData->bHash)) == 0)
		{
			if (ppICSData)
				*ppICSData = pICData;
			return errOK;
		}
		dwICDataOffset += sizeof(ICS_DATA) + pICData->dwBlockDataSize;
	}

	return errOBJECT_NOT_FOUND;
}

tERROR iDeleteStatus(hi_IChecker _this, ICS_DATA* pICData, tBYTE* pStreamData, tDWORD dwStreamSize, tDWORD* dwNewStreamSize)
{
	tDWORD dwICDataOffset = (tDWORD)((tBYTE*)pICData - pStreamData);
	if (dwICDataOffset + sizeof(ICS_DATA) + pICData->dwBlockDataSize > dwStreamSize)
		return errOBJECT_DATA_SIZE_UNDERSTATED;
	*dwNewStreamSize = dwStreamSize - sizeof(ICS_DATA) - pICData->dwBlockDataSize;
	memmove(pStreamData + dwICDataOffset, 
			pStreamData + dwICDataOffset + sizeof(ICS_DATA) + pICData->dwBlockDataSize, 
			dwStreamSize - (dwICDataOffset + sizeof(ICS_DATA) + pICData->dwBlockDataSize));
	return errOK;
}

tERROR iAddStatus(hi_IChecker _this, tBYTE* pHash, tBYTE* pStatusBuffer, tDWORD dwStatusBufferSize, tBYTE* pStreamData, tDWORD dwBufferSize, tDWORD dwStreamSize, tDWORD* dwNewStreamSize)
{
	ICS_DATA* pICData;
	if (dwStreamSize >= sizeof(tDWORD) && *(tDWORD*)(pStreamData+dwStreamSize-sizeof(tDWORD)) == ICS_BLOCK_END)
		dwStreamSize-=sizeof(tDWORD);
	if (dwStreamSize + sizeof(ICS_DATA) + dwStatusBufferSize + sizeof(tDWORD)> dwBufferSize)
		return errBUFFER_TOO_SMALL;

	pICData = (ICS_DATA*)(pStreamData+dwStreamSize);
	pICData->dwBlockSignature = ICS_BLOCK_SIGNATURE;
	memcpy(pICData->bHash, pHash, sizeof(pICData->bHash));
	pICData->dwBlockDataSize = dwStatusBufferSize;
	memcpy(pICData+1, pStatusBuffer, dwStatusBufferSize);
	dwStreamSize += sizeof(ICS_DATA) + dwStatusBufferSize;
	*(tDWORD*)(pStreamData+dwStreamSize) = ICS_BLOCK_END;
	dwStreamSize += sizeof(tDWORD);
	*dwNewStreamSize = dwStreamSize;

	return errOK;
}

tERROR iAddStatusEx(hi_IChecker _this, tBYTE* pHash, tBYTE* pStatusBuffer, tDWORD dwStatusBufferSize, tBYTE* pStreamData, tDWORD dwBufferSize, tDWORD dwStreamSize, tDWORD* dwNewStreamSize)
{
	tERROR error;
	ICS_DATA* pICData = (ICS_DATA*)pStreamData;

	while (errBUFFER_TOO_SMALL == (error = iAddStatus(_this, pHash, pStatusBuffer, dwStatusBufferSize, pStreamData, dwBufferSize, dwStreamSize, dwNewStreamSize)))
	{
		if (pICData->dwBlockSignature != ICS_BLOCK_SIGNATURE) // no any valid block
			break;
		if (PR_FAIL(error = iDeleteStatus(_this, pICData, pStreamData, dwStreamSize, &dwStreamSize)))
			break;
	}

	return error;
}

tERROR iWriteStream(hIO hStreamIo, tDWORD* result, tPTR pBuffer, tDWORD dwBufferSize)
{
	tERROR error;
	HANDLE hFile = (HANDLE)CALL_SYS_PropertyGetDWord(hStreamIo, plNATIVE_HANDLE);
	if (hFile==0 || hFile==INVALID_HANDLE_VALUE)
		return errUNEXPECTED;

	if(os.dwMajorVersion > 4){
		FILETIME dummy;
		dummy.dwHighDateTime = -1;
		dummy.dwLowDateTime  = -1;
		SetFileTime(hFile, &dummy, &dummy, &dummy); // Trick to disable LastWriteTime modification
		error = CALL_IO_SeekWrite(hStreamIo, result, 0, pBuffer, dwBufferSize);
	}else{
		FILETIME t1, t2, t3;
		if(!GetFileTime(hFile, &t1, &t2, &t3))
			return errUNEXPECTED;
		error = CALL_IO_SeekWrite(hStreamIo, result, 0, pBuffer, dwBufferSize);
		if (!SetFileTime(hFile, &t1, &t2, &t3))
			return errUNEXPECTED;
	}
	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatus )
// Extended method comment
//    Для получения ассоциированных данных, метод считает уникальный хеш для файла. Хеш считается по выборочным данным из файла (в зависимости от структуры файла), а также данным указанных в параметре pData. Затем, находит в базе данных значения, ассоциированные с этим хешем и возвращает их. Размер ассоциированных данных ограничен двумя  DWORD'ами.
// Behaviour comment
//    Возвращает errMODULE_UNKNOWN_FORMAT в случае, когда невозможно посчитать хеш для данного файла.
// Parameters are:
//   "dwDataSize"         : Указывается в байтах
tERROR pr_call IChecker_GetStatus( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize )
{
	tERROR error = errOK;
	hIO hStreamIo = NULL;
	tDWORD dwStreamSize;
	ICS_DATA* pICData;
	tBYTE  bStreamBuffer[MAX_BUFFER_SIZE];
	tBYTE  bHash[8];

	PR_TRACE_A0( _this, "Enter IChecker::GetStatus method" );

	if (result)
		*result = 0;

	if (PR_SUCC(error))
	{
		tDWORD* pShortHash = (tDWORD*)bHash;
		hHASH hHashMD5;

		error = CALL_SYS_ObjectCreateQuick(io, &hHashMD5, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY);
		if (PR_SUCC(error))
		{
			tDWORD bHashMD5[4];
			

//			if (PR_SUCC(error = CALL_SYS_ObjectCheck(_this, (hOBJECT)io, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE)))
			{
				tDWORD dwLength;
				hSTRING str = (hSTRING)io;
				WCHAR wcszFileName[MAX_PATH];
				WCHAR* pwcszFileName = wcszFileName;
				WCHAR  wcszPrefix[] = L"\\\\?\\";
#define PREFIX_LEN 4
#define PREFIX_LEN_W (PREFIX_LEN*sizeof(wcszPrefix[0]))


				if (PR_SUCC(error = CALL_SYS_PropertyGetStr(io, &dwLength, pgOBJECT_FULL_NAME, 0, 0, cCP_UNICODE)))
				{
					if (dwLength + PREFIX_LEN_W > sizeof(wcszFileName))
						error = CALL_SYS_ObjHeapAlloc(str, &pwcszFileName, dwLength + PREFIX_LEN_W);
					else
						pwcszFileName = wcszFileName; // use local buffer
				}
				
				if (PR_SUCC(error))
				{
					memcpy(pwcszFileName, wcszPrefix, PREFIX_LEN * sizeof(wcszPrefix[0]));
					//error = CALL_String_ExportToBuff(str, &dwLength, cSTRING_WHOLE, pwcszFileName + PREFIX_LEN, dwLength, cCP_UNICODE, cSTRING_Z);
					error = CALL_SYS_PropertyGetStr(io, &dwLength, pgOBJECT_FULL_NAME, pwcszFileName + PREFIX_LEN, dwLength, cCP_UNICODE);
				}
				
				if (PR_SUCC(error))
				{
					WIN32_FILE_ATTRIBUTE_DATA sFAData;
					if (!GetFileAttributesExW(pwcszFileName, GetFileExInfoStandard, &sFAData))
					{
						tDWORD dwW32Err = GetLastError();
						error = errUNEXPECTED;
					}
					else
					{
						tDWORD i;
						sFAData.ftLastAccessTime.dwHighDateTime = 0;
						sFAData.ftLastAccessTime.dwLowDateTime  = 0;
						sFAData.dwFileAttributes &= ~(FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL); // ignore ARCHIVE & NORMAL attribute (Backup agents may change archive attribute)
						error = CALL_Hash_Update(hHashMD5, (tBYTE*)&sFAData, sizeof(sFAData));
						
						// extract file name from path
						dwLength += PREFIX_LEN_W;
						for (i=dwLength/2-2; i>0; i--)
						{
							if (pwcszFileName[i] == '\\' || pwcszFileName[i] == '/')
								break;
						}
						error = CALL_Hash_Update(hHashMD5, (tBYTE*)(pwcszFileName+i+1), dwLength-(i+2)*2);
					}
				}

				if (pwcszFileName && pwcszFileName != wcszFileName)
					CALL_SYS_ObjHeapFree(str, pwcszFileName);
			}

			if (PR_SUCC(error) && dwDataSize != 0)
				error = CALL_Hash_Update(hHashMD5, pData, dwDataSize);
			if (PR_SUCC(error))
				error = CALL_Hash_GetHash(hHashMD5, (tBYTE*)&bHashMD5[0], sizeof(bHashMD5));
			if (PR_SUCC(error))
			{
				pShortHash[0] = bHashMD5[0] ^ bHashMD5[2] ^ IChecker_VERSION;
				pShortHash[1] = bHashMD5[1] ^ bHashMD5[3];
			}
			CALL_SYS_ObjectClose(hHashMD5);
		}
		if (PR_SUCC(error))
			CALL_SYS_PropertySet(io, NULL, propICS_HASH, bHash, sizeof(bHash));
	}

	if (PR_FAIL(error))
		return error;
	
	if (PR_FAIL(error = iCreateStreamIoFromObj(_this, (hOBJECT)io, fOMODE_OPEN_IF_EXIST, &hStreamIo)))
	{
		// failed to create stream IO
		if (error == errOBJECT_NOT_FOUND)
		{
			error = warnNOT_FOUND;
			if (pStatusBuffer)
				memset(pStatusBuffer, 0, dwStatusBufferSize); // return zero buffer
			if (result)
				*result = dwStatusBufferSize;
		}
		return error;
	}
	
	error = CALL_IO_SeekRead(hStreamIo, &dwStreamSize, 0, &bStreamBuffer, sizeof(bStreamBuffer));

	if (PR_SUCC(error) && (dwStreamSize < sizeof(ICS_DATA) + 1))
		error = errOBJECT_INVALID;

	
	if (PR_SUCC(error) || (error == errOUT_OF_OBJECT && dwStreamSize == 0))
	{
		if (PR_SUCC(error = iGetICSDataFromStreamData(_this, &bHash[0], &bStreamBuffer[0], dwStreamSize, &pICData)))
		{
			if (pICData->dwBlockDataSize != dwStatusBufferSize)
			{
				// 03.12.2003 Mike - changed from errPARAMETER_INVALID to errOBJECT_NOT_FOUND because 
				// it was impossible to update stream if data size changed but hash stays the same
				error = errOBJECT_NOT_FOUND;
				PR_TRACE((_this, prtERROR, "Requested data size not match stored data size, %terr", error));
			}
			else
			{
				if (pStatusBuffer)
					memcpy(pStatusBuffer, pICData+1, dwStatusBufferSize); // return data
				if (result)
					*result = dwStatusBufferSize;
			}
		}
		if (error == errOBJECT_NOT_FOUND)
		{
			error = warnNOT_FOUND;
			if (pStatusBuffer)
				memset(pStatusBuffer, 0, dwStatusBufferSize); // return zero buffer
			if (result)
				*result = dwStatusBufferSize;
		}
	}

	CALL_SYS_ObjectClose(hStreamIo);

	PR_TRACE_A2( _this, "Leave IChecker::GetStatus method, ret tDWORD = %u, error = 0x%08x", 0, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateStatus )
// Parameters are:
tERROR pr_call IChecker_UpdateStatus( hi_IChecker _this, hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize )
{
	tERROR error;
	hOBJECT obj = (hOBJECT)io; // actually it may be hSTRING or other object type
	hIO hStreamIo = NULL;
	tDWORD dwStreamSize;
	ICS_DATA* pICData;
	tBYTE  bStreamBuffer[MAX_BUFFER_SIZE];
	tBYTE  bHash[8];
	tDWORD dwAttrib;
	tWCHAR* pwcszFileName = NULL;
	tIID iid;

	PR_TRACE_A0( _this, "Enter IChecker::UpdateStatus method" );

	if (pStatusBuffer == NULL || dwStatusBufferSize == 0)
		return errPARAMETER_INVALID;

	if (PR_FAIL(error = CALL_SYS_PropertyGet(obj, NULL, propICS_HASH, bHash, sizeof(bHash))))
		return errPARAMETER_INVALID;

	if (PR_FAIL(error = CALL_SYS_PropertyGet(obj, NULL, pgINTERFACE_ID, &iid, sizeof(iid))))
		return error;
	
	switch(iid)
	{
	case IID_STRING:
		{
			tDWORD dwLen;
			hSTRING str = (hSTRING)obj;

			error = CALL_String_ExportToBuff(str, &dwLen, cSTRING_WHOLE, 0, 0, cCP_UNICODE, cSTRING_Z);
			if (PR_SUCC(error))
				error = CALL_SYS_ObjHeapAlloc(_this, &pwcszFileName, dwLen);
			if (PR_SUCC(error))
				error = CALL_String_ExportToBuff(str, NULL, cSTRING_WHOLE, pwcszFileName, dwLen, cCP_UNICODE, cSTRING_Z);
			if (PR_SUCC(error))
			{
				dwAttrib = GetFileAttributesW(pwcszFileName);
				if (dwAttrib == -1)
					error = errUNEXPECTED;
			}
		}
		break;
	case IID_IO:
		error = CALL_SYS_PropertyGet(obj, NULL, pgOBJECT_ATTRIBUTES, &dwAttrib, sizeof(dwAttrib));
		break;
	default:
		error = errOBJECT_INCOMPATIBLE;
	}

/*	
	if (fSfcFileException) // SFC inited
	{
		WCHAR wcszFileName[MAX_PATH];
		if (PR_SUCC(error = CALL_SYS_ObjectCheck(_this, (hOBJECT)io, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE)))
			error =  CALL_String_ExportToBuff((hSTRING)io, NULL, cSTRING_WHOLE, wcszFileName, sizeof(wcszFileName), cCP_UNICODE, cSTRING_Z);
		else
		if (PR_SUCC(error = CALL_SYS_ObjectCheck(_this, (hOBJECT)io, IID_IO, PID_ANY, SUBTYPE_ANY, cFALSE)))
			error =  CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_FULL_NAME, wcszFileName, sizeof(wcszFileName), cCP_UNICODE);

		if (PR_SUCC(error))
		{
			tBOOL bResult = fSfcFileException(NULL, wcszFileName, DISABLE_FILE_ACTION_MODIFIED);
			PR_TRACE((_this, prtALWAYS_REPORTED_MSG, "IChecker_UpdateStatus: fSfcFileException returned %s for %S", (bResult?"TRUE":"FALSE"), wcszFileName));
		}
		else
		{
			PR_TRACE((_this, prtERROR, "IChecker_UpdateStatus: failed to get filename with %terr", error));
		}
	}
*/
	if (PR_SUCC(error))
		error = iCreateStreamIoFromObj(_this, obj, fOMODE_OPEN_ALWAYS, &hStreamIo);
	if (PR_SUCC(error))
	{
		error = CALL_IO_SeekRead(hStreamIo, &dwStreamSize, 0, &bStreamBuffer, sizeof(bStreamBuffer));

		if (dwStreamSize < 4)
			dwStreamSize = 0;
		
		if(PR_SUCC(error) && dwStreamSize != 0)
		{
			//check stream for erased data by driver
			if (*(tDWORD*)bStreamBuffer == ICS_BLOCK_END)
				dwStreamSize = 0;
		}

		if (PR_SUCC(error) || (error == errOUT_OF_OBJECT && dwStreamSize == 0))
		{
			if (PR_SUCC(error = iGetICSDataFromStreamData(_this, &bHash[0], &bStreamBuffer[0], dwStreamSize, &pICData)))
			{
				if (pICData->dwBlockDataSize != dwStatusBufferSize)
				{
					if (PR_SUCC(error = iDeleteStatus(_this, pICData, &bStreamBuffer[0], dwStreamSize, &dwStreamSize)))
					{
						error = iAddStatusEx(_this, &bHash[0], (tBYTE*)pStatusBuffer, dwStatusBufferSize, &bStreamBuffer[0], sizeof(bStreamBuffer), dwStreamSize, &dwStreamSize);
					}
				}
				else
				{
					memcpy(pICData+1, pStatusBuffer, dwStatusBufferSize); // update data
					error = errOK;
				}
			}
			else
			{
				error = iAddStatusEx(_this, bHash, (tBYTE*)pStatusBuffer, dwStatusBufferSize, &bStreamBuffer[0], sizeof(bStreamBuffer), dwStreamSize, &dwStreamSize);
			}
		}

		
		if (PR_SUCC(error))
			error = iWriteStream(hStreamIo, &dwStreamSize, &bStreamBuffer, dwStreamSize);

		CALL_SYS_ObjectClose(hStreamIo);
		hStreamIo = NULL;

		// The code below may lead to incorrect attributes in some rare case.
		// The problem scenario is: 
		// 1. Writer writes some data into file, archive attribute still not set
		// 2. The Scanner scansthe file and call IChecker_UpdateStatus
		// 3. IChecker_UpdateStatus gets attributes
		// 4. IChecker_UpdateStatus creates & writes stream => archive attribute appears for file
		// 5. Writer closes handle => archive attribute already set
		// 6. IChecker_UpdateStatus checks for old file attributes and drops archive bit (code below).
		//
		// As a result file has been modified, but archive attribute remain clear. This occures only
		// if writer closes handle during IChecker_UpdateStatus.
		if (!(dwAttrib & fATTRIBUTE_ARCHIVE))
		{
			dwAttrib &= ~fATTRIBUTE_ARCHIVE;
			switch(iid)
			{
			case IID_STRING:
				SetFileAttributesW(pwcszFileName, dwAttrib);
				break;
			case IID_IO:
				CALL_SYS_PropertySet(obj, NULL, pgOBJECT_ATTRIBUTES, &dwAttrib, sizeof(dwAttrib));
				break;
			}

		}
	}

	// cleanup
	if (pwcszFileName)
		CALL_SYS_ObjHeapFree(_this, pwcszFileName);

	PR_TRACE_A1( _this, "Leave IChecker::UpdateStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteStatus )
// Parameters are:
tERROR pr_call IChecker_DeleteStatus( hi_IChecker _this, hIO io )
{
	tERROR error;
	hIO hStreamIo;
//	tQWORD qwStreamSize;
	tDWORD dwStreamSize;
	ICS_DATA* pICData;
	tBYTE  bStreamBuffer[MAX_BUFFER_SIZE];
	tBYTE  bHash[8];

	PR_TRACE_A0( _this, "Enter IChecker::DeleteStatus method" );

	if (PR_FAIL(error = CALL_SYS_PropertyGet(io, NULL, propICS_HASH, bHash, sizeof(bHash))))
		return errPARAMETER_INVALID;

	if (PR_FAIL(error = iCreateStreamIoFromObj(_this, (hOBJECT)io, fOMODE_OPEN_IF_EXIST, &hStreamIo)))
	{
		if (error == errOBJECT_NOT_FOUND)
			error = errOK;
		return error;
	}

	/*error = CALL_IO_GetSize(hStreamIo, &qwStreamSize, IO_SIZE_TYPE_EXPLICIT);
	if (PR_SUCC(error) && (qwStreamSize > MAX_BUFFER_SIZE))
		error = errOBJECT_INCOMPATIBLE;
	dwStreamSize = (tDWORD)qwStreamSize;
	
	if (PR_SUCC(error))*/
		error = CALL_IO_SeekRead(hStreamIo, &dwStreamSize, 0, &bStreamBuffer, sizeof(bStreamBuffer));

	if (error == errOUT_OF_OBJECT && dwStreamSize == 0)
	{
		error = CALL_SYS_PropertySetBool(hStreamIo, pgOBJECT_DELETE_ON_CLOSE, cTRUE);
	}
	else
	{
		if (PR_SUCC(error))
		{
			if (PR_SUCC(error = iGetICSDataFromStreamData(_this, &bHash[0], &bStreamBuffer[0], dwStreamSize, &pICData)))
			{
				error = iDeleteStatus(_this, pICData, &bStreamBuffer[0], dwStreamSize, &dwStreamSize);
			}
		}

		if (PR_SUCC(error))
			error = iWriteStream(hStreamIo, &dwStreamSize, &bStreamBuffer, dwStreamSize);
	}

	CALL_SYS_ObjectClose(hStreamIo);	
	
	PR_TRACE_A1( _this, "Leave IChecker::DeleteStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, FlushStatusDB )
// Extended method comment
//    Метод предназначен для предотвращения потери накопленных в течении длительной сессии данных в результате действия внешних факторов (например, неожиданное выключение питания).
// Parameters are:
tERROR pr_call IChecker_FlushStatusDB( hi_IChecker _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IChecker::FlushStatusDB method" );

	// Nothing to do here

	PR_TRACE_A1( _this, "Leave IChecker::FlushStatusDB method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ClearStatusDB )
// Parameters are:
tERROR pr_call IChecker_ClearStatusDB( hi_IChecker _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IChecker::ClearStatusDB method" );

	// Cannot clean files streams

	PR_TRACE_A1( _this, "Leave IChecker::ClearStatusDB method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



