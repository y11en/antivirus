// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  19 October 2002,  18:34 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Integrated Solution Lite
// Purpose     -- Предназначен для хранения информации о дате проверки файлов
// Author      -- Andy Nikishin
// File Name   -- ifilesec.c
// AVP Prague stamp end

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

/*
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ifilesec.c
*		\brief	Stores/Retrives data associated with files by there content (not name)
*		
*		\author Andy Nikishin
*/		



// AVP Prague stamp begin( Private definitions,  )
#define iFileSecureStatus_PRIVATE_DEFINITION
// AVP Prague stamp end



#define _WIN32_WINNT 0x0400
// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include <iface/i_sfdb.h>

#include "../hash_api/hash.h"
#include "../hash_api/md5/hash_md5.h"
//#include "../hash_api/crapi/hash_cr.h"
#include "../sfdb/sfdb.h"
#include "ifilesec.h"
//#include "elf.h"
#include "entr_elf.h"
#include "newexe.h"
#include "pehdr.h"
#include "pr_oid.h"
#include "../memmodscan/io.h"
#include "itss.h"
#include "zip.h"
#include "rar.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Short comments --
// Short comments --
// iFileSecureStatus interface implementation
// AVP Prague stamp end

// --------------------------------------------------------------------------------------------

#define _CALC_ICHECKER_READ_TIME_
#if (defined(_DEBUG) || defined(_CALC_ICHECKER_READ_TIME_))

#include "/prague/avpgs/timestats.h"

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

// --------------------------------------------------------------------------------------------

#define _CALC_ICHECKER_TIME_
#if (defined(_DEBUG) || defined(_CALC_ICHECKER_TIME_))

#include "/prague/avpgs/timestats.h"

#define StatisticEnabled() (exStatisticEnabled!=0 && exStatisticEnabled())
#define ICheckerStatisticTimingStart(io) (StatisticEnabled() ? iICheckerStatisticTimingStart(io) : errOK)
#define ICheckerStatisticTimingStop(io, error) (StatisticEnabled() ? iICheckerStatisticTimingStop(io, error) : errOK)

tERROR iICheckerStatisticTimingStart(hIO io)
{
	hOBJECT hIoParent;
	tERROR error;

	hIoParent = CALL_SYS_ParentGet(io, IID_ANY);
	if (PR_FAIL(error=CALL_SYS_ObjectCheck(io,hIoParent,IID_STRING,PID_ANY,0,0)))
		return errOK;
	exStatisticTimingStart(hIoParent, cSTAT_ICHECKER);
	return errOK;
}

tERROR iICheckerStatisticTimingStop(hIO io, tDWORD dwStatus)
{
	hOBJECT hIoParent;
	tERROR error;

	hIoParent = CALL_SYS_ParentGet(io, IID_ANY);
	if (PR_FAIL(error=CALL_SYS_ObjectCheck(io,hIoParent,IID_STRING,PID_ANY,0,0)))
		return errOK;
	exStatisticTimingStop(hIoParent, cSTAT_ICHECKER);
	if (dwStatus)
		exStatisticSet(hIoParent, cSTAT_ICHECKER, dwStatus);
	return errOK;
}

#else // _CALC_ICHECKER_TIME_

#define StatisticEnabled() (errOK)
#define ICheckerStatisticTimingStart(io) (errOK)
#define ICheckerStatisticTimingStop(io, error) (errOK)

#endif // _CALC_ICHECKER_TIME_

// --------------------------------------------------------------------------------------------

HASH_API* hash_api;

typedef struct _tag_HashAndData_
{
	ULARGE_INTEGER byHash;
	tDWORD dwData1;
	tDWORD dwData2;
}HashAndData;


// AVP Prague stamp begin( Data structure,  )
// Interface iFileSecureStatus. Inner data structure

typedef struct tag_ifsData {
	hSECUREFILEDB hDataBase; // --
	tCHAR         szDBFileName[260];   // Путь к файлу базы данных
	
	NewMainArchiveHeader NewMhd;
	NewFileHeader NewLhd;
	BlockHeader BlockHead;
	tCHAR ArcFileName[MAX_PATH];
	tDWORD NextBlockPos;
	tDWORD CurBlockPos;
	tDWORD dwFileSize;
	tINT LastBlock;
	tDWORD dwPackDataPos;
} ifsData;

// AVP Prague stamp end


tERROR CheckFileEx(hIO handle, HashAndData* data, tBYTE* pAdditionalData, tDWORD dwAdditionalDataSize, ifsData* pIfsData);
tERROR CalcFileHash(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, hIO io, void* pMD5Ctx);
tERROR CalcFileHashStrepped(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, tDWORD dwStripSize, tDWORD dwPartOfStreepToCheck, hIO io, void* pMD5Ctx);

tBOOL IsFileFormatTTF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatLNK(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatINF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatINI(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatCOM(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatELF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatEXE(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatSYS(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatITSS41(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatZIP(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx);
tBOOL IsFileFormatRAR(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx, ifsData* data);

#define npKAVI_HASH_AND_DATA   ((tSTRING)("npKAVI_HASH_AND_DATA"))
static tPROPID propid_hash;
const char szContent[] = "MSCompressed/Content";
const char szControlData[] = "MSCompressed/ControlData";
const char szResetTable[] = "MSCompressed/Transform/{7FC28940-9D31-11D0-9B27-00A0C91E9C7C}/InstanceData/ResetTable";





// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_iFileSecureStatus 
{
	const iiFileSecureStatusVtbl* vtbl; // pointer to the "iFileSecureStatus" virtual table
	const iSYSTEMVTBL*            sys;  // pointer to the "SYSTEM" virtual table
	ifsData*                      data; // pointer to the "iFileSecureStatus" data structure
} *hi_iFileSecureStatus;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call iFileSecureStatus_ObjectInit( hi_iFileSecureStatus _this );
tERROR pr_call iFileSecureStatus_ObjectInitDone( hi_iFileSecureStatus _this );
tERROR pr_call iFileSecureStatus_ObjectPreClose( hi_iFileSecureStatus _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_iFileSecureStatus_vtbl = {
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       iFileSecureStatus_ObjectInit,
	(tIntFnObjectInitDone)   iFileSecureStatus_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   iFileSecureStatus_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpiFileSecureStatus_SetGarbageCollectionStatus)( hi_iFileSecureStatus _this, tBOOL bStatus ); // -- ;
typedef   tERROR (pr_call *fnpiFileSecureStatus_DoGarbageCollection)   ( hi_iFileSecureStatus _this ); // -- ;
typedef   tERROR (pr_call *fnpiFileSecureStatus_GetStatus)     ( hi_iFileSecureStatus _this, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pdwData1, tDWORD* pdwData2 ); // -- Возвращает данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpiFileSecureStatus_UpdateStatus)          ( hi_iFileSecureStatus _this, hIO io, tDWORD dwData1, tDWORD dwData2 ); // -- ;
typedef   tERROR (pr_call *fnpiFileSecureStatus_DeleteStatus)          ( hi_iFileSecureStatus _this, hIO io ); // -- ;
typedef   tERROR (pr_call *fnpiFileSecureStatus_FlushStatusDB) ( hi_iFileSecureStatus _this );         // -- Сохраняет БД ассоциированных данных;
typedef   tERROR (pr_call *fnpiFileSecureStatus_ClearStatusDB) ( hi_iFileSecureStatus _this );         // -- Очищает БД ассоциированных данных.;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
	fnpiFileSecureStatus_SetGarbageCollectionStatus  SetGarbageCollectionStatus;
	fnpiFileSecureStatus_DoGarbageCollection         DoGarbageCollection;
struct iiFileSecureStatusVtbl {
	fnpiFileSecureStatus_GetStatus                   GetStatus;
	fnpiFileSecureStatus_UpdateStatus                UpdateStatus;
	fnpiFileSecureStatus_DeleteStatus                DeleteStatus;
	fnpiFileSecureStatus_FlushStatusDB  FlushStatusDB;
	fnpiFileSecureStatus_ClearStatusDB  ClearStatusDB;
}; // "iFileSecureStatus" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call iFileSecureStatus_SetGarbageCollectionStatus( hi_iFileSecureStatus _this, tBOOL bStatus );
tERROR pr_call iFileSecureStatus_DoGarbageCollection( hi_iFileSecureStatus _this );
tERROR pr_call iFileSecureStatus_GetStatus( hi_iFileSecureStatus _this, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pdwData1, tDWORD* pdwData2 );
tERROR pr_call iFileSecureStatus_UpdateStatus( hi_iFileSecureStatus _this, hIO io, tDWORD dwData1, tDWORD dwData2 );
tERROR pr_call iFileSecureStatus_DeleteStatus( hi_iFileSecureStatus _this, hIO io );
tERROR pr_call iFileSecureStatus_FlushStatusDB( hi_iFileSecureStatus _this );
tERROR pr_call iFileSecureStatus_ClearStatusDB( hi_iFileSecureStatus _this );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iiFileSecureStatusVtbl e_iFileSecureStatus_vtbl = {
	iFileSecureStatus_GetStatus,
	iFileSecureStatus_UpdateStatus,
	iFileSecureStatus_DeleteStatus,
	iFileSecureStatus_FlushStatusDB,
	iFileSecureStatus_ClearStatusDB
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "iFileSecureStatus". Static(shared) property table variables
const tVERSION dwVersion = 2; // must be READ_ONLY at runtime
// Interface "iFileSecureStatus". Global(shared) property table variables
const tSTRING szComment = "File Secure interface"; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(iFileSecureStatus_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, dwVersion, sizeof(dwVersion) )
	mLOCAL_PROPERTY_BUF( plDB_PATHNAME, ifsData, szDBFileName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 22 )
mPROPERTY_TABLE_END(iFileSecureStatus_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "iFileSecureStatus". Register function
tERROR pr_call iFileSecureStatus_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter iFileSecureStatus::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IFILESECURESTATUS,                  // interface identifier
		PID_IFILESEC,                           // plugin identifier
		0x00000000,                             // subtype identifier
		dwVersion,                              // interface version
		VID_ANDY,                               // interface developer
		&i_iFileSecureStatus_vtbl,              // internal(kernel called) function table
		(sizeof(i_iFileSecureStatus_vtbl)/sizeof(tPTR)),// internal function table size
		&e_iFileSecureStatus_vtbl,              // external function table
		(sizeof(e_iFileSecureStatus_vtbl)/sizeof(tPTR)),// external function table size
		iFileSecureStatus_PropTable,            // property table
		mPROPERTY_TABLE_SIZE(iFileSecureStatus_PropTable),// property table size
		sizeof(ifsData),                        // memory size
		0                                       // interface flags
	);

	if ( PR_FAIL(CALL_Root_RegisterCustomPropId(root, &propid_hash, npKAVI_HASH_AND_DATA, pTYPE_BINARY )))
		return cFALSE;

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"iFileSecureStatus(IID_IFILESECURESTATUS) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave iFileSecureStatus::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call iFileSecureStatus_ObjectInit( hi_iFileSecureStatus _this ) {
tERROR error;

	PR_TRACE_A0( _this, "Enter iFileSecureStatus::ObjectInit method" );
	
	hash_api = (HASH_API*)&hash_api_md5;
	
	error = CALL_SYS_ObjectCreateQuick(_this, &_this->data->hDataBase, IID_SECUREFILEDB, PID_SFDB, SUBTYPE_ANY );
	if ( PR_SUCC(error) )
		error = CALL_SYS_PropertySetDWord( _this->data->hDataBase , plSERIALIZE_OPTIONS, cSFDB_SERIALIZE_CHECKSUM | cSFDB_SERIALIZE_CRYPT );

	PR_TRACE_A1( _this, "Leave iFileSecureStatus::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call iFileSecureStatus_ObjectInitDone( hi_iFileSecureStatus _this ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter iFileSecureStatus::ObjectInitDone method" );

	error = CALL_SecureFileDatabase_Load(_this->data->hDataBase, _this->data->szDBFileName);
	if(PR_FAIL(error)){
		error = CALL_SecureFileDatabase_InitNew(_this->data->hDataBase, 'MD5 ', 8, 8, 0);
	}

	PR_TRACE_A1( _this, "Leave iFileSecureStatus::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call iFileSecureStatus_ObjectPreClose( hi_iFileSecureStatus _this )
{
	tERROR error = errOK;
	
	PR_TRACE_A0( _this, "Enter iFileSecureStatus::ObjectPreClose method" );
	
	if (_this->data->hDataBase)
		error = CALL_SecureFileDatabase_Save(_this->data->hDataBase, _this->data->szDBFileName);
	
	PR_TRACE_A1( _this, "Leave iFileSecureStatus::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatus )
// Parameters are:
tERROR pr_call iFileSecureStatus_GetStatus( hi_iFileSecureStatus _this, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pdwData1, tDWORD* pdwData2 ) 
{
HashAndData data;
tERROR error;
tBOOL bStatEnabled;

	PR_TRACE_A0( _this, "Enter iFileSecureStatus::GetStatus method" );

	if (bStatEnabled = StatisticEnabled())
		ICheckerStatisticTimingStart(io);

	// GetFormatType
	// CountCRC
	error = CheckFileEx(io, &data, pData, dwDataSize, _this->data);
	if(PR_SUCC(error)){
		// Find In database
		error = CALL_SecureFileDatabase_GetRecord(_this->data->hDataBase, &data.byHash, &data.dwData1);
		if(PR_SUCC(error)){
			*pdwData1 = data.dwData1;
			*pdwData2 = data.dwData2;

			// Set CRC and some extra info to io as custom properties
			error = CALL_SYS_PropertySet(io, NULL, propid_hash, &data, sizeof(data));
			// Return Status Found
			//error = errOK;
		}
		else if (error == errNOT_FOUND){
			*pdwData1 = data.dwData1 = 0;
			*pdwData2 = data.dwData2 = 0;

			// Set CRC and some extra info to io as custom properties
			error = CALL_SYS_PropertySet(io, NULL, propid_hash, &data, sizeof(data));
			// Return Status Found
			//error = errOK;
		}
		// error -- return status "error"

#ifdef _DEBUG
		if (PR_SUCC(error))
		{
			char szFileName[0x200];
			CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szFileName, 0x200, cCP_ANSI);
			PR_TRACE((_this, prtNOTIFY, "iFileSecureStatus_GetStatus: %s - %I64X %08X %08X", szFileName, *(tQWORD*)&data.byHash, data.dwData1, data.dwData2));
		}
#endif

	}

	if (bStatEnabled)
		ICheckerStatisticTimingStop(io, error);
	
	PR_TRACE_A1( _this, "Leave iFileSecureStatus::GetStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateStatus )
// Parameters are:
tERROR pr_call iFileSecureStatus_UpdateStatus( hi_iFileSecureStatus _this, hIO io, tDWORD dwData1, tDWORD dwData2 ) {
HashAndData data;
tERROR error;
tBOOL bStatEnabled;

	PR_TRACE_A0( _this, "Enter iFileSecureStatus::UpdateStatus method" );
	error = errOK;

	if (bStatEnabled = StatisticEnabled())
		ICheckerStatisticTimingStart(io);

	// Get CRC and extra info from io's custom property
	error = CALL_SYS_PropertyGet(io, NULL, propid_hash, &data, sizeof(data));
	if(PR_SUCC(error)){
		data.dwData1 = dwData1;
		data.dwData2 = dwData2;
		//data.dwData1
		// Delete Custom property
		// Update in DataBase
		error = CALL_SecureFileDatabase_UpdateRecord(_this->data->hDataBase, &data.byHash, &data.dwData1);
#ifdef _DEBUG
		if (PR_SUCC(error))
		{
			char szFileName[0x200];
			CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szFileName, 0x200, cCP_ANSI);
			PR_TRACE((_this, prtNOTIFY, "iFileSecureStatus_UpdateStatus: %s - %I64X %08X %08X", szFileName, *(tQWORD*)&data.byHash, data.dwData1, data.dwData2));
		}
#endif
	}

	if (bStatEnabled)
		ICheckerStatisticTimingStop(io, error);

	PR_TRACE_A1( _this, "Leave iFileSecureStatus::UpdateStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteStatus )
// Parameters are:
tERROR pr_call iFileSecureStatus_DeleteStatus( hi_iFileSecureStatus _this, hIO io ) {
HashAndData data;
tERROR error;
tBOOL bStatEnabled;

	PR_TRACE_A0( _this, "Enter iFileSecureStatus::DeleteStatus method" );
	error = errOK;
	
	if (bStatEnabled = StatisticEnabled())
		ICheckerStatisticTimingStart(io);

	// Get CRC and extra info from io's custom property
	error = CALL_SYS_PropertyGet(io, NULL, propid_hash, &data, sizeof(data));
	if(PR_SUCC(error)){
		// Delete Custom property
		// Delete in DataBase
		error = CALL_SecureFileDatabase_DeleteRecord(_this->data->hDataBase, &data.byHash);
#ifdef _DEBUG
		if (PR_SUCC(error))
		{
			char szFileName[0x200];
			CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szFileName, 0x200, cCP_ANSI);
			PR_TRACE((_this, prtNOTIFY, "iFileSecureStatus_DeleteStatus: %s - %I64X", szFileName, *(tQWORD*)&data.byHash));
		}
#endif
	}
	
	if (bStatEnabled)
		ICheckerStatisticTimingStop(io, error);

	PR_TRACE_A1( _this, "Leave iFileSecureStatus::DeleteStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, FlushStatusDB )
// Extended method comment
//    Метод предназначен для предотвращения потери накопленных в течении длительной сессии данных в результате действия внешних факторов (например, неожиданное выключение питания).
// Parameters are:
tERROR pr_call iFileSecureStatus_FlushStatusDB( hi_iFileSecureStatus _this ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter iFileSecureStatus::FlushStatusDB method" );

	if (!_this->data->hDataBase)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_SecureFileDatabase_Save(_this->data->hDataBase, _this->data->szDBFileName);

	PR_TRACE_A1( _this, "Leave iFileSecureStatus::FlushStatusDB method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ClearStatusDB )
// Parameters are:
tERROR pr_call iFileSecureStatus_ClearStatusDB( hi_iFileSecureStatus _this ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter iFileSecureStatus::ClearStatusDB method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave iFileSecureStatus::ClearStatusDB method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end





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
#elif defined (_WIN32)
#define SWAP_BYTES(a) (((a&0x00FF)<<8)|((a&0xFF00)>>8))
#define MAKE_WORD(a) (SWAP_BYTES(a))
#define MAKE_DWORD(a, b) (((DWORD)SWAP_BYTES(a))|((DWORD)SWAP_BYTES(b)<<16))
#define ReadDWordPtr(x) (*((tDWORD*)(x)))
#define ReadWordPtr(x) (*((tWORD*)(x)))
#else
#define MAKE_WORD(a) (a)
#define MAKE_DWORD(a, b) (((DWORD)(a))|((DWORD)(b)<<16))
#endif

#define DW ReadDWordPtr
#define W ReadWordPtr

tDWORD RVAToRaw(tDWORD RVA,PIMAGE_NT_HEADERS pPE, tDWORD dwSizeOfBuffer);
tDWORD GetOverlayOffset(PIMAGE_NT_HEADERS pPE, tDWORD dwSize);
long __inline A20(tWORD Segment,tWORD Offset);

void __inline check(tBYTE* buffer, tDWORD Part, tVOID* pCtx)
{
	hash_api->Update(pCtx, buffer, Part);
	return ;
}

tERROR Finalize(HashAndData* data, tVOID* pCtx)
{
	char md5hash[16];
	tDWORD dwHashSize = sizeof(md5hash);

	hash_api->Finalize(pCtx, &dwHashSize, md5hash);
	
	if (dwHashSize != sizeof(md5hash))
		return errUNEXPECTED;

	data->byHash.QuadPart =  *(unsigned __int64*)&md5hash[0];
	data->byHash.QuadPart ^= *(unsigned __int64*)&md5hash[8];
	
	return errOK;
}

#define _CLCASE(c) (c>='A' && c<='Z' ? c | 0x20 : c )
int __stricmp(tBYTE* str1, tBYTE* str2)
{
	while (*str1 != 0 && *str2 != 0 || _CLCASE(*str1) == _CLCASE(*str2))
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

tERROR CheckFileEx(hIO handle, HashAndData* data, tBYTE* pAdditionalData, tDWORD dwAdditionalDataSize, ifsData* pIfsData)
{
tDWORD offset=0,oldoffset=0;
tDWORD How;
tERROR error;
tDWORD dwFileSize;
tQWORD wqTmp;
tBYTE buffer[BIGBUFFERSIZE*2]; // Don't make it smaller!
tBYTE MD5Ctx[0x100];
tBYTE filename[0x20];
tDWORD dwFNSize;

	if (!hash_api->Init(CALG_MD5, &MD5Ctx, hash_api->ContextSize, 16, NULL))
	{
		return errUNEXPECTED;
	}

	error = CALL_IO_GetSize(handle, &wqTmp, IO_SIZE_TYPE_EXPLICIT);
	if(PR_FAIL(error))
	{
		PR_TRACE(( handle, prtERROR, "Error getting file size err=%08X", error));
		return error;
	}
	if(*(((tDWORD*)&wqTmp)+1)) // file size > 4Gb
		return errMODULE_UNKNOWN_FORMAT;
	
	dwFileSize = (tDWORD)wqTmp;

	if (dwFileSize == 0) // file size is zero
	{
		PR_TRACE(( handle, prtERROR, "File size == 0"));
		data->byHash.QuadPart = 0;
		return errOK;
	}

	if(PR_FAIL(error = CALL_IO_SeekReadTimed((hIO)handle, &How, 0, buffer, BIGBUFFERSIZE)))
	{
		PR_TRACE(( handle, prtERROR, "Error reading file err=%08X", error));
		return error;
	}

	// calculate additional data into hash
	if (pAdditionalData != NULL && dwAdditionalDataSize != 0)
		check(pAdditionalData, dwAdditionalDataSize, &MD5Ctx);

	if((W(buffer) == 'ZM' || W(buffer) == 'MZ') && IsFileFormatEXE(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);

	PR_TRACE((handle, prtNOTIFY, "Not EXE file"));
	
	if (DW(buffer) == 0x0000004C && IsFileFormatLNK(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);
	
	if (DW(buffer) == 0x00000100 && IsFileFormatTTF(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);
	
	if (DW(buffer) == 0x464C457Fl && IsFileFormatELF(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);
	
	if (dwFileSize < 0x8000 && IsFileFormatINF(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);
	
	if (dwFileSize <= 0xFFFF && W(buffer+2)==0xFFFF && IsFileFormatSYS(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);

	if (DW(buffer) == (ITSFILE_HEADER_MAGIC) && IsFileFormatITSS41(buffer, How, dwFileSize, handle, &MD5Ctx))
	{
		return Finalize(data, &MD5Ctx);
	}
	if (DW(buffer) == (LOCAL_HDR_SIG) && IsFileFormatZIP(buffer, How, dwFileSize, handle, &MD5Ctx))
	{
		return Finalize(data, &MD5Ctx);
	}
	if (DW(buffer) == (RAR_SIG) && IsFileFormatRAR(buffer, How, dwFileSize, handle, &MD5Ctx, pIfsData))
	{
		return Finalize(data, &MD5Ctx);
	}
	
	if (IsFileFormatCOM(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);

	// get file name
	if (PR_FAIL(CALL_SYS_PropertyGetStr(handle, &dwFNSize, pgOBJECT_NAME, filename, sizeof(filename)-1, cCP_ANSI)))
		return errMODULE_UNKNOWN_FORMAT;

	// check for "desktop.ini"
	if (dwFNSize == 11 && How == dwFileSize && __stricmp(filename, "desktop.ini")==0 && IsFileFormatINI(buffer, How, dwFileSize, handle, &MD5Ctx))
		return Finalize(data, &MD5Ctx);

#ifdef _DEBUG // store memory images for unknown formats (packed files) into "c:\ichecker_mem_images" folder if exist
	{
		if (OID_MEMORY_PROCESS_MODULE == CALL_SYS_PropertyGetDWord(handle, pgOBJECT_ORIGIN))
		{
			char szFileName[0x400];
			char szName[0x300];
			HANDLE hFile;
			if (PR_SUCC(CALL_SYS_PropertyGetStr(handle, NULL, pgOBJECT_FULL_NAME, szName, sizeof(szName), cCP_ANSI)))
			{
				char* pName = szName;
				char* pPath;
				while (strchr(pName, '|') != NULL)
					pName = strchr(pName, '|') + 1;
				PR_TRACE((handle, prtALWAYS_REPORTED_MSG, "image name: %s", pName));
				strcpy(szFileName, "c:\\ichecker_mem_images\\");
				strcat(szFileName, pName);
				pPath = strrchr(szFileName, '\\');
				if (pPath != NULL)
				{
					if (strcmp(pPath+1, pName) != 0)
					{
						*pPath = 0;
						CreateDirectory(szFileName, NULL);
						PR_TRACE((handle, prtALWAYS_REPORTED_MSG, "create folder: '%s' err=%08X", szFileName, GetLastError()));
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
						CALL_IO_SeekReadTimed(handle, &dwBytesRead, qwOffset, buff, sizeof(buff));
						WriteFile(hFile, buff, dwBytesRead, &dwBytesWrite, NULL);
						qwOffset += dwBytesRead;
					} while (dwBytesRead);
					CloseHandle(hFile);
				}
				else
				{
					PR_TRACE((handle, prtERROR, "Cannot create image file: %s (%08X)", szFileName, GetLastError()));
				}
			}
		}
	}
#endif
	
	return errMODULE_UNKNOWN_FORMAT;
}


tDWORD RVAToRaw(tDWORD RVA,PIMAGE_NT_HEADERS pPE, tDWORD dwSizeOfBuffer)
{
PIMAGE_SECTION_HEADER pSH;
tWORD Sections,i;
tDWORD Pos,TempRVA;
	
	__try
	{
		Sections=pPE->FileHeader.NumberOfSections;
		Pos = pPE->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(tDWORD);
		if(RVA<pPE->OptionalHeader.SizeOfHeaders)
			return RVA;
		for(i=0;i<Sections;i++)
		{
			if(Pos + sizeof(IMAGE_SECTION_HEADER) > dwSizeOfBuffer)
				return 0;
			pSH=(PIMAGE_SECTION_HEADER)(((tBYTE*)pPE)+Pos);
			TempRVA=RVA-pSH->VirtualAddress;
			if(TempRVA>=0 && TempRVA<=pSH->SizeOfRawData)//(pSH->Misc.VirtualSize==0 ? pSH->SizeOfRawData : pSH->Misc.VirtualSize)))
				return pSH->PointerToRawData+TempRVA;
			Pos += sizeof(IMAGE_SECTION_HEADER);
		}
	}
	__except(1)
	{
		return 0;
	}
	return 0;
}

tDWORD GetSectionNo(tDWORD RVA,PIMAGE_NT_HEADERS pPE, tDWORD dwSizeOfBuffer)
{
PIMAGE_SECTION_HEADER pSH;
tWORD Sections,i;
tDWORD Pos,TempRVA;
	
	__try
	{
		Sections=pPE->FileHeader.NumberOfSections;
		Pos = pPE->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(tDWORD);
		if(RVA<pPE->OptionalHeader.SizeOfHeaders)
			return RVA;
		for(i=0;i<Sections;i++)
		{
			if(Pos + sizeof(IMAGE_SECTION_HEADER) > dwSizeOfBuffer)
				return 0;
			pSH=(PIMAGE_SECTION_HEADER)&(((TBYTE*)pPE)[Pos]);
			TempRVA=RVA-pSH->VirtualAddress;
			if(TempRVA>=0 && TempRVA<=pSH->SizeOfRawData)//(pSH->Misc.VirtualSize==0 ? pSH->SizeOfRawData : pSH->Misc.VirtualSize)))
				return i+1;
			Pos += sizeof(IMAGE_SECTION_HEADER);
		}
	}
	__except(1)
	{
		return 0;
	}
	return 0;
}

long __inline A20(tWORD Segment,tWORD Offset)
{
	return(( (long)((long)Segment<<4) +(long)(Offset) ) & 0xFFFFF);
}

tERROR CalcFileHash(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, hIO io, void* pMD5Ctx)
{
	tDWORD how;
	tERROR error = errOK;
	
	if (dwDataSizeInBuffer != 0)
		check(buffer, dwDataSizeInBuffer, pMD5Ctx);
	
	while (dwReadStartOffset < dwReadEndOffset)
	{
		tDWORD dwReadSize = (dwReadEndOffset-dwReadStartOffset > BIGBUFFERSIZE ? BIGBUFFERSIZE : dwReadEndOffset-dwReadStartOffset);
		error = CALL_IO_SeekReadTimed(io, &how, dwReadStartOffset, buffer, dwReadSize);
		if (PR_FAIL(error) || how == 0) // !!! errOEF not an error (it's warning)
			return error;
		check(buffer, how, pMD5Ctx);
		dwReadStartOffset += how;
	}
	
	return errOK;
}

tERROR CalcFileHashStrepped(tBYTE* buffer, tDWORD dwDataSizeInBuffer, tDWORD dwReadStartOffset, tDWORD dwReadEndOffset, tDWORD dwStripSize, tDWORD dwPartOfStreepToCheck, hIO io, void* pMD5Ctx)
{
tDWORD how;
tERROR error = errOK;
	
	if (dwDataSizeInBuffer != 0)
		check(buffer, dwDataSizeInBuffer, pMD5Ctx);

	while (dwReadStartOffset < dwReadEndOffset)
	{
		tDWORD dwReadSize = (dwReadEndOffset-dwReadStartOffset > dwPartOfStreepToCheck ? dwPartOfStreepToCheck : dwReadEndOffset-dwReadStartOffset);
		error = CALL_IO_SeekReadTimed(io, &how, dwReadStartOffset, buffer, dwReadSize);
		if (PR_FAIL(error) || how == 0) // !!! errOEF not an error (it's warning)
			return error;
		check(buffer, how, pMD5Ctx);
		dwReadStartOffset += dwStripSize;
	}
	
	return errOK;
}


tBOOL IsFileFormatTTF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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

	check(buffer, entrySelector, pMD5Ctx);
	check((tBYTE*)&dwFileSize, sizeof(dwFileSize), pMD5Ctx);
	return cTRUE;
#undef W_BIGENDIAN
#undef DW_BIGENDIAN
}

tBOOL IsFileFormatLNK(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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
		DW(ptr+0x0C) = 0; DW(ptr+0x10) = 0;
	}

	if ( (dwFileLocationsFlags & 2) == 0 )
	{
		// clear garbage in network volume info offsets
		DW(ptr+0x14) = 0;
	}
	
	check(buffer, dwDataSize, pMD5Ctx);
	return cTRUE;
}

tBOOL IsFileFormatINI(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
{
	check(buffer, dwDataSize, pMD5Ctx);
	return cTRUE;
}

tBOOL IsFileFormatINF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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
			(DW(buffer+i+1) | 0x20202020) == MAKE_DWORD('ve','rs') &&
			(DW(buffer+i+5) | 0x00202020) == MAKE_DWORD('io','n]') && 
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
		if ((DW(buffer+i)   | 0x20202020) == MAKE_DWORD('si','gn') &&  
			(DW(buffer+i+4) | 0x20202020) == MAKE_DWORD('at','ur') &&  
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
					if (PR_FAIL(CalcFileHash(buffer, dwDataSize, dwDataSize, dwFileSize, io, pMD5Ctx)))
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



tBOOL IsFileFormatSYS(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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
		check(buffer, 10, pMD5Ctx);
		
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "IsFileFormatSYS: Error reading file err=%08X", error));
			return cFALSE;
		}
		check(buffer, dwDataSize, pMD5Ctx);

		error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset2, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "IsFileFormatSYS: Error reading file err=%08X", error));
			return cFALSE;
		}
		check(buffer, dwDataSize, pMD5Ctx);

		if(wOffsetToNextDiver != 0xffff)
		{
			// Read Header
			error = CALL_IO_SeekReadTimed(io, &dwDataSize, wOffsetToNextDiver, buffer, BUFFERSIZE);
			if (PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatSYS: Error reading file err=%08X", error));
				return cFALSE;
			}
//			return IsFileFormatSYS(buffer, dwDataSize, dwFileSize, io, pMD5Ctx);
		}
	} while (dwDriversCount < 10 && wOffsetToNextDiver != 0xffff);
	
	return cTRUE;
}

// is DOS COM program? ----------------------------------------------
tBOOL IsFileFormatCOM(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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
			PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
			return cFALSE;
		}
		if (dwJmpCount++ == 3)
			break;
		oldoffset=offset;
		if ((buffer[0]==0xE8 || buffer[0]==0xE9) && (*(tWORD*)&buffer[1])!=0) // CALL NEAR/JMP SHORT
			offset=(W(buffer+1)+3+oldoffset) & 0xFFFF;
	} while (oldoffset!=offset);
	
	check(buffer, dwDataSize, pMD5Ctx);
	return cTRUE;
}

tBOOL IsFileFormatELF(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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

	check(buffer, sizeof(Elf32_Ehdr), pMD5Ctx);
	
	error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
	if (PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
		return cFALSE;
	}
	check(buffer, dwDataSize, pMD5Ctx);

	return cTRUE;
	
}

#define _RVAToRaw(rva, pImageNTHeaders, dwDataSize) (bMemoryImage ? (pImageNTHeaders)->OptionalHeader.ImageBase + rva : RVAToRaw(rva, pImageNTHeaders, dwDataSize))

tBOOL IsFileFormatEXE(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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

	if (OID_MEMORY_PROCESS_MODULE == CALL_SYS_PropertyGetDWord(io, pgOBJECT_ORIGIN))
		bMemoryImage = cTRUE;

	WordBuffer = (tWORD*)buffer;
	if( pImageDosHeader->e_magic !='MZ' && pImageDosHeader->e_magic!='ZM')
		return cFALSE;

	bAlreadyCheckIP = cFALSE;
	if(dwFileSize < sizeof(IMAGE_DOS_HEADER))
	{
		PR_TRACE(( io, prtNOTIFY, "This is not EXE file"));
		return cFALSE;
	}
	// Skip some data
	((PIMAGE_DOS_HEADER)buffer)->e_csum = 0;
	// Check DOS Header
	check(buffer, sizeof(IMAGE_DOS_HEADER), pMD5Ctx);

	dwDOSEXEEntryPoint = A20(WordBuffer[11],WordBuffer[10]);
	dwDOSEXEEntryPoint += (((long)WordBuffer[4])<<4);
	
	if( WordBuffer[0xC] == 0x40) //> 0x3F)	// May be NE or PE ? (LX LE)
	{
		//	memcpy(TempBuffer,buffer,512);
		
		// Got EP into already read buffer?
		if(BIGBUFFERSIZE - dwDOSEXEEntryPoint > BUFFERSIZE && (tDWORD)pImageDosHeader->e_lfanew < BIGBUFFERSIZE)
		{
			bAlreadyCheckIP = cTRUE;
			check(buffer+dwDOSEXEEntryPoint, pImageDosHeader->e_lfanew-dwDOSEXEEntryPoint, pMD5Ctx);
		}
		
		//Get NewEXE (PE) Header offset
		if((tDWORD)(pImageDosHeader->e_lfanew) > BIGBUFFERSIZE/2)
		{
			error = CALL_IO_SeekReadTimed(io, &dwDataSize, pImageDosHeader->e_lfanew, buffer, BIGBUFFERSIZE);
			if (PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
				return cFALSE;
			}
			WordBuffer = (tWORD*)buffer;
		}
		else
		{
			dwDataSize = BIGBUFFERSIZE - pImageDosHeader->e_lfanew;
			WordBuffer = (tWORD*)(buffer + pImageDosHeader->e_lfanew);
		}
		// New EXE File
		if(WordBuffer[0] == 'EN')
		{
			// Count New EXE Header's CRC
			check((tBYTE*)WordBuffer, (tDWORD)((dwDataSize>(tDWORD)sizeof(NEWEXEHEADER)) ? sizeof(NEWEXEHEADER) : dwDataSize), pMD5Ctx);
		}
		// Portable Executable
		else if(WordBuffer[0] == 'EP')
		{
			
			//bool bIsThisPEPacked(void)
			PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)WordBuffer;
			PIMAGE_SECTION_HEADER pFirstSection = IMAGE_FIRST_SECTION(pNTHeader);

			if ((tBYTE*)(pFirstSection+1) > (tBYTE*)(pNTHeader) + dwDataSize) // not enought data in buffer
			{
				PR_TRACE((io, prtIMPORTANT, "not enought data in buffer (ntheader+firstsection)"));
				return cFALSE;
			}
			
			// from here check for packed images to avoid pure packers hashes

			if (bMemoryImage) // check image base for memory images
			{
				tDWORD hModule;
				hModule = CALL_SYS_PropertyGetDWord(io, plIO_HMODULE);
				if (hModule == 0 || hModule != pNTHeader->OptionalHeader.ImageBase)
				{
					PR_TRACE((io, prtIMPORTANT, "Memory image has been relocated (hModule=%08X, ImageBase=%08X)", hModule, pNTHeader->OptionalHeader.ImageBase));
					return cFALSE;
				}
			}

			if (pFirstSection->Characteristics & IMAGE_SCN_MEM_WRITE)
			{
				PR_TRACE((io, prtIMPORTANT, "Packed PE module: 1st section is writable"));
				bSuspectedForSelfExtractor=cTRUE;
				//return cFALSE;
			}
			if (pNTHeader->OptionalHeader.AddressOfEntryPoint)
			{
				if ((pNTHeader->OptionalHeader.AddressOfEntryPoint >= pFirstSection->VirtualAddress + pFirstSection->Misc.VirtualSize) || (pNTHeader->OptionalHeader.AddressOfEntryPoint < pFirstSection->VirtualAddress ))
				{
					PR_TRACE((io, prtIMPORTANT, "Packed PE module: EntryPoint out of 1st section EP=%08X, Base=%08X", pNTHeader->OptionalHeader.AddressOfEntryPoint, pNTHeader->OptionalHeader.ImageBase));
					bSuspectedForSelfExtractor=cTRUE;
					//return cFALSE;
				}
			}
			if (pFirstSection->SizeOfRawData < pFirstSection->Misc.VirtualSize)
			{
				PR_TRACE((io, prtIMPORTANT, "Packed PE module: 1st section physical size < virtual size (%08X<%08X)", pFirstSection->SizeOfRawData, pFirstSection->Misc.VirtualSize));
				bSuspectedForSelfExtractor=cTRUE;
				//return cFALSE;
			}

			if(bSuspectedForSelfExtractor){
				
				if(dwFileSize > 5000000)
					return cFALSE;

				if (PR_FAIL(error = CalcFileHash(buffer, 0, 0, dwFileSize, io, pMD5Ctx)))
				{
					PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
					return cFALSE;
				}
				return cTRUE;
			}
			
			// end of "packed" checks 
			
			// Skip Some data in header
			pNTHeader->FileHeader.TimeDateStamp = 0;
			
			pNTHeader->OptionalHeader.MajorLinkerVersion = 0;
			pNTHeader->OptionalHeader.MinorLinkerVersion = 0;
			pNTHeader->OptionalHeader.SizeOfInitializedData = 0;
			pNTHeader->OptionalHeader.SizeOfUninitializedData = 0;
			
			//Resource Table RVA and size
			pNTHeader->OptionalHeader.DataDirectory[2].Size = 0;
			pNTHeader->OptionalHeader.DataDirectory[2].VirtualAddress = 0;
			
			pNTHeader->OptionalHeader.CheckSum = 0;
			
			check((tBYTE*)WordBuffer, sizeof(IMAGE_FILE_HEADER)+4+sizeof(IMAGE_OPTIONAL_HEADER), pMD5Ctx);

			dwOverlaySize = 0;
			dwOffsetToOverlay = GetOverlayOffset(pNTHeader, dwDataSize);
			if(dwOffsetToOverlay)
				dwOverlaySize = dwFileSize - dwOffsetToOverlay;
			
			offset = _RVAToRaw(pNTHeader->OptionalHeader.AddressOfEntryPoint, pNTHeader, dwDataSize);

			dwOffsetToRelo = 0;
			dwReloSize = pNTHeader->OptionalHeader.DataDirectory[5].Size;
			if(dwReloSize != 0)
			{
				dwOffsetToRelo = _RVAToRaw(pNTHeader->OptionalHeader.DataDirectory[5].VirtualAddress, pNTHeader, dwDataSize);
				
			}
			dwOffsetToExport = 0;
			dwExportSize = pNTHeader->OptionalHeader.DataDirectory[0].Size;
			if( dwExportSize != 0)
			{
				dwOffsetToExport = _RVAToRaw(pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress, pNTHeader, dwDataSize);
			}
				
			if(offset != 0)
			{
				error = CALL_IO_SeekReadTimed(io, &dwDataSize, offset, buffer, BUFFERSIZE);
				if (PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, dwDataSize, pMD5Ctx);
			}
			// Relo table
			if(dwOffsetToRelo != 0)
			{
				// fixed wrong read offset (27.11.02)
				error = CALL_IO_SeekReadTimed(io, &dwDataSize, dwOffsetToRelo, buffer, (dwReloSize < BUFFERSIZE ? dwReloSize : BUFFERSIZE));
				if (PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, dwDataSize, pMD5Ctx);
			}
			// Export table
			if(dwOffsetToExport != 0)
			{
				if (PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToExport, dwOffsetToExport+dwExportSize, io, pMD5Ctx)))
				{
					PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
					return cFALSE;
				}
			}
			// Overlay
			if(dwOffsetToOverlay != 0)
			{
				if(dwOverlaySize < BIGBUFFERSIZE)
				{
					if (PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToOverlay, dwOffsetToOverlay+dwOverlaySize, io, pMD5Ctx)))
					{
						PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
						return cFALSE;
					}
				}
				else
				{
					if (PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToOverlay, dwOffsetToOverlay+BIGBUFFERSIZE, io, pMD5Ctx)))
					{
						PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
						return cFALSE;
					}
					if (PR_FAIL(error = CalcFileHash(buffer, 0, dwFileSize - BIGBUFFERSIZE, dwFileSize, io, pMD5Ctx)))
					{
						PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
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
			PR_TRACE(( io, prtERROR, "Error reading file err=%08X", error));
			return cFALSE;
		}
		if((buffer[0] == 0xE9 || buffer[0] == 0xE8) && (*(tWORD *)&buffer[1]) != 0)
		{
			offset=((*(tWORD *)&buffer[1])+3+oldoffset) & 0xFFFF;
			if((++iNumberOfJumps) == 3)
				break;
		}
	} while (offset != oldoffset);
	check(buffer, dwDataSize, pMD5Ctx);
	return cTRUE;
}

tDWORD GetOverlayOffset(PIMAGE_NT_HEADERS pPE, tDWORD dwSize)
{
PIMAGE_SECTION_HEADER pSH;
tWORD Sections,i;
tDWORD Pos;
tDWORD dwEndOfFile;
	
	__try
	{
		dwEndOfFile = 0;
		Sections = pPE->FileHeader.NumberOfSections;
		Pos = pPE->FileHeader.SizeOfOptionalHeader+sizeof(IMAGE_FILE_HEADER)+sizeof(tDWORD);
		for(i=0;i<Sections;i++)
		{
			pSH=(PIMAGE_SECTION_HEADER)&(((TBYTE*)pPE)[Pos]);
			if(pSH->PointerToRawData + pSH->SizeOfRawData > dwEndOfFile)
				dwEndOfFile = pSH->PointerToRawData + pSH->SizeOfRawData;

			Pos += sizeof(IMAGE_SECTION_HEADER);
			if(Pos > dwSize)
				return 0;
		}
		return dwEndOfFile;
	}
	__except(1)
	{
		return 0;
	}
	return 0;
}

/*************************************************** ITSS 4.1   ******************************************
/
/
/*************************************************** ITSS 4.1   ******************************************/
tBOOL IsFileFormatITSS41(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
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
		PR_TRACE(( io, prtNOTIFY, "This is not ITSS 4.1 file"));
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
	check(buffer, sizeof(ITSFileHeader), pMD5Ctx);
	if(pH->offPathMgrData + sizeof(DatabaseHeader) < dwDataSize )
	{
		pDBh = (DatabaseHeader*)&buffer[pH->offPathMgrData];
	}
	else
	{
		error = CALL_IO_SeekReadTimed(io, &dwDataSize, pH->offPathMgrData, buffer, BUFFERSIZE);
		if (PR_FAIL(error))
		{
			PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err = %08X", error));
			return cFALSE;
		}
		pDBh = (DatabaseHeader*)buffer;
	}
	if(pDBh->uiMagic != (ITSDB_HEADER_MAGIC) || pDBh->cbDirectoryBlock > 0x2000)
	{
		PR_TRACE(( io, prtNOTIFY, "This is not ITSS 4.1 file. DataBase header's signature is missing or too big Dir Block"));
		return cFALSE;
	}
	if(pDBh->cBlocks > BIGBUFFERSIZE)
	{
		PR_TRACE(( io, prtNOTIFY, "This is ITSS 4.1 file, but Name block size too large (larger then 4k)"));
		return cFALSE;
	}

	// Check Path Mgr Header Data
	check((tBYTE*)pDBh, sizeof(DatabaseHeader), pMD5Ctx);
	if(pDBh->iRootDirectory == 0xffffffff)
	{
		pDBh->iRootDirectory = 0;
	}
	// Read Root folder with data space
	error = CALL_IO_SeekReadTimed(io, &dwDataSize, offPathMgrData + sizeof(DatabaseHeader) + (pDBh->iRootDirectory * pDBh->cbDirectoryBlock), buffer, pDBh->cbDirectoryBlock);
	if (PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err = %08X", error));
		return cFALSE;
	}
	pnh = (NodeHeader*)buffer;
	if(pnh->uiMagic != ITSBLOCK_LEAF_MAGIC)
	{
		PR_TRACE(( io, prtNOTIFY, "This is ITSS 4.1 file, but Root Name block is not leaf block"));
		return cFALSE;
	}
	DumpPaths(buffer, dwDataSize, &iData);

	if(iData.offResetTable != 0)
	{
		if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offResetTable + dwRelativeOffset, iData.offResetTable + dwRelativeOffset + iData.cbResetTable, io, pMD5Ctx)))
		{
			PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err=%08X", error));
			return cFALSE;
		}
	}
	if(iData.offContent != 0)
	{
		if(iData.cbContent < BIGBUFFERSIZE)
		{
			if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offContent + dwRelativeOffset, iData.offContent + dwRelativeOffset + iData.cbContent, io, pMD5Ctx)))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err=%08X", error));
				return cFALSE;
			}
		}
		else
		{
			if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offContent + dwRelativeOffset, iData.offContent + dwRelativeOffset + BIGBUFFERSIZE, io, pMD5Ctx)))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err=%08X", error));
				return cFALSE;
			}

			if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offContent + dwRelativeOffset + iData.cbContent - BIGBUFFERSIZE, iData.offContent + dwRelativeOffset + iData.cbContent, io, pMD5Ctx)))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err=%08X", error));
				return cFALSE;
			}
		}
	}
	
	if(iData.offControlData != 0)
	{
		if (PR_FAIL(error = CalcFileHash(buffer, 0, iData.offControlData + dwRelativeOffset, iData.offControlData + dwRelativeOffset + iData.cbControlData, io, pMD5Ctx)))
		{
			PR_TRACE(( io, prtERROR, "IsFileFormatITSS41: Error reading file err=%08X", error));
			return cFALSE;
		}
	}
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


/*************************************************** ZIP ******************************************
/
/
/*************************************************** ZIP ******************************************/
tBOOL IsFileFormatZIP(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx)
{
tINT i;
EndRecord* pEnd;
FileHeader fh;// 2e
LocalFileHeader lfh;
tDWORD dwOffsetToNextFile;
tDWORD dwEndOfDir;
tERROR error;
tDWORD how;

	if(dwFileSize < BIGBUFFERSIZE)
	{
		check(buffer, dwDataSize, pMD5Ctx);
		return cTRUE;
	}
	error = CALL_IO_SeekReadTimed(io, NULL, dwFileSize - BIGBUFFERSIZE, buffer, BIGBUFFERSIZE );
	if(PR_FAIL(error))
	{
		PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
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
			error = CALL_IO_SeekReadTimed(io, NULL, i, &fh, sizeof(FileHeader));
			if(PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			check((tBYTE*)&fh, sizeof(FileHeader), pMD5Ctx);
			i += sizeof(FileHeader);
			if(PR_FAIL(error = CalcFileHash(buffer, 0, i, i + fh.filename_length + fh.extra_field_length + fh.file_comment_length, io, pMD5Ctx)))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			i += (fh.filename_length + fh.extra_field_length + fh.file_comment_length);

		// Read Local File data
			error = CALL_IO_SeekReadTimed(io, NULL, dwOffsetToNextFile, &lfh, sizeof(LocalFileHeader));
			if(PR_FAIL(error))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			check((tBYTE*)&lfh, sizeof(LocalFileHeader), pMD5Ctx);

			dwOffsetToNextFile += sizeof(LocalFileHeader);
			if(PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToNextFile, dwOffsetToNextFile + lfh.filename_length + lfh.extra_field_length, io, pMD5Ctx)))
			{
				PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
				return cFALSE;
			}
			dwOffsetToNextFile += (lfh.filename_length + lfh.extra_field_length);

			if(fh.compressed_size < 0x1000 || fh.compression_method == STORED)	// Check whole file
			{
				if(PR_FAIL(error = CalcFileHash(buffer, 0, dwOffsetToNextFile, dwOffsetToNextFile + fh.compressed_size, io, pMD5Ctx)))
				{
					PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
					return cFALSE;
				}
			}
			else
			{
				if(PR_FAIL(error = CalcFileHashStrepped(buffer, 0, dwOffsetToNextFile, dwOffsetToNextFile + fh.compressed_size, 1024, 256, io, pMD5Ctx)))
				{
					PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
					return cFALSE;
				}
				error = CALL_IO_SeekReadTimed(io, &how, dwOffsetToNextFile + fh.compressed_size - BUFFERSIZE, buffer, BUFFERSIZE);
				if(PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "IsFileFormatZIP: Error reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, how, pMD5Ctx);
			}
			dwOffsetToNextFile += fh.compressed_size;
		}while(i < (int)dwEndOfDir);
	}
	return TRUE;
}



/*************************************************** RAR new ******************************************
/
/
/*************************************************** RAR new ******************************************/
tDWORD ReadHeader(hIO ArcPtr, int BlockType, ifsData* data);
tDWORD ReadBlock(hIO ArcPtr, int BlockType, ifsData* data);

tBOOL IsFileFormatRAR(tBYTE* buffer, tDWORD dwDataSize, tDWORD dwFileSize, hIO io, void* pMD5Ctx, ifsData* data)
{
tBOOL bSolid;
tDWORD how;
tERROR error;

	if(dwFileSize < SIZEOF_NEWMHD + SIZEOF_MARKHEAD + SIZEOF_NEWLHD)
	{
		PR_TRACE(( io, prtERROR, "IsFileFormatRAR: This is not RAR"));
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
	
	check((tBYTE*)&data->NewMhd, sizeof(NewMainArchiveHeader), pMD5Ctx);

	data->NextBlockPos += data->NewMhd.HeadSize - SIZEOF_NEWMHD;
	while (1)
	{
		if(0 == ReadBlock(io, FILE_HEAD | READSUBBLOCK, data))
			break;
		check((tBYTE*)&data->NewLhd, sizeof(NewFileHeader), pMD5Ctx);
		check((tBYTE*)&data->ArcFileName, data->NewLhd.NameSize, pMD5Ctx);

		if((data->NewLhd.Flags & LHD_WINDOWMASK) == LHD_DIRECTORY)
		{
			continue;
		}
/*		PR_TRACE(( io, prtERROR, "IsFileFormatRAR: %s", data->ArcFileName));
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
				PR_TRACE(( io, prtERROR, "IsFileFormatRAR: Error reading file err=%08X", error));
				return cFALSE;
			}
			check(buffer, how, pMD5Ctx);
		}
		else
		{
			if(data->NewLhd.PackSize < 0x1000 || data->NewLhd.Method == 0x30)	// Check whole file
			{
				if(PR_FAIL(error = CalcFileHash(buffer, 0, data->dwPackDataPos, data->dwPackDataPos + data->NewLhd.PackSize, io, pMD5Ctx)))
				{
					PR_TRACE(( io, prtERROR, "IsFileFormatRAR: Error reading file err=%08X", error));
					return cFALSE;
				}
			}
			else
			{
				if(PR_FAIL(error = CalcFileHashStrepped(buffer, 0, data->dwPackDataPos, data->dwPackDataPos + data->NewLhd.PackSize, 1024, 256, io, pMD5Ctx)))
				{
					PR_TRACE(( io, prtERROR, "IsFileFormatRAR: Error reading file err=%08X", error));
					return cFALSE;
				}
				error = CALL_IO_SeekReadTimed(io, &how, data->dwPackDataPos + data->NewLhd.PackSize - BUFFERSIZE, buffer, BUFFERSIZE);
				if(PR_FAIL(error))
				{
					PR_TRACE(( io, prtERROR, "IsFileFormatRAR: Error reading file err=%08X", error));
					return cFALSE;
				}
				check(buffer, how, pMD5Ctx);
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

tDWORD ReadBlock(hIO ArcPtr, int BlockType, ifsData* data)
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


tDWORD ReadHeader(hIO ArcPtr, int BlockType, ifsData* data)
{
tDWORD Size;
BYTE Header[64];
tERROR error;
	
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
			error = CALL_IO_SeekReadTimed(ArcPtr, NULL, data->NextBlockPos + Size, &Header[7], 4);
			Size += 4;
			data->BlockHead.DataSize = GetHeaderDword(7);
		}
		break;
	}
	data->NextBlockPos += Size;
	return Size;
}

