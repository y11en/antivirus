// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Tuesday,  20 January 2004,  16:36 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- IChecker
// Author      -- Pavlushchik
// File Name   -- ichecker2.c
// Additional info
//    Integrated Solution Lite
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define IChecker_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "ichecker2.h"
// AVP Prague stamp end

#include "../../../prague/core/string/pstring.h"
#include "plugin_ichecker2.h"
#include <ProductCore/iface/i_sfdb.h>
#include <ProductCore/iface/i_ichecker2.h>
#include "calc_file_hash.h"
#include "../sfdb/plugin_sfdb.h"
#include "../sfdb/sfdb.h"
#include "forcedbgout.h"

#define npICHECKER2_HASH   ((tSTRING)("npICHECKER2_HASH"))

static tPROPID propid_hash;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IChecker interface implementation
// Short comments --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface IChecker. Inner data structure

typedef struct tag_ifsData 
{
	hSECUREFILEDB hDataBase;         // База данных, содержащая информацию ассоциированную с хешами файлов
	tCHAR         szDBFileName[260]; // Путь к файлу базы данных
	tDWORD        dwUserDataSize;    // Размер ассоциированных данных сохраняемых в базе вместе с хешем. В байтах.
	tDWORD        dwUserVersion;     // Версия (тип) ассоциированных данных, хранящихся в базе данных
} ifsData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IChecker 
{
	const iICheckerVtbl* vtbl; // pointer to the "IChecker" virtual table
	const iSYSTEMVTBL*   sys;  // pointer to the "SYSTEM" virtual table
	ifsData*             data; // pointer to the "IChecker" data structure
} *hi_IChecker;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IChecker_ObjectInit( hi_IChecker _this );
tERROR pr_call IChecker_ObjectInitDone( hi_IChecker _this );
tERROR pr_call IChecker_ObjectPreClose( hi_IChecker _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IChecker_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       IChecker_ObjectInit,
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



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpIChecker_GetStatus)     ( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ); // -- Возвращает данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpIChecker_UpdateStatus)  ( hi_IChecker _this, hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ); // -- Устанавливает данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpIChecker_DeleteStatus)  ( hi_IChecker _this, hIO io );          // -- Удаляет данные, ассоциированные с файлом.;
typedef   tERROR (pr_call *fnpIChecker_FlushStatusDB) ( hi_IChecker _this );                  // -- Сохраняет БД ассоциированных данных;
typedef   tERROR (pr_call *fnpIChecker_ClearStatusDB) ( hi_IChecker _this );                  // -- Очищает БД ассоциированных данных.;
typedef   tERROR (pr_call *fnpIChecker_GetStatusEx)   ( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize, tDWORD dwFlags ); // -- Возвращает данные, ассоциированные с файлом.;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iICheckerVtbl 
{
	fnpIChecker_GetStatus      GetStatus;
	fnpIChecker_UpdateStatus   UpdateStatus;
	fnpIChecker_DeleteStatus   DeleteStatus;
	fnpIChecker_FlushStatusDB  FlushStatusDB;
	fnpIChecker_ClearStatusDB  ClearStatusDB;
	fnpIChecker_GetStatusEx    GetStatusEx;
}; // "IChecker" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call IChecker_GetStatus( hi_IChecker _this, tDWORD* result, hIO p_io, tBYTE* p_pData, tDWORD p_dwDataSize, tDWORD* p_pStatusBuffer, tDWORD p_dwStatusBufferSize );
tERROR pr_call IChecker_UpdateStatus( hi_IChecker _this, hIO p_io, tDWORD* p_pStatusBuffer, tDWORD p_dwStatusBufferSize );
tERROR pr_call IChecker_DeleteStatus( hi_IChecker _this, hIO p_io );
tERROR pr_call IChecker_FlushStatusDB( hi_IChecker _this );
tERROR pr_call IChecker_ClearStatusDB( hi_IChecker _this );
tERROR pr_call IChecker_GetStatusEx( hi_IChecker _this, tDWORD* result, hIO p_io, tBYTE* p_pData, tDWORD p_dwDataSize, tDWORD* p_pStatusBuffer, tDWORD p_dwStatusBufferSize, tDWORD p_dwFlags );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iICheckerVtbl e_IChecker_vtbl = 
{
	IChecker_GetStatus,
	IChecker_UpdateStatus,
	IChecker_DeleteStatus,
	IChecker_FlushStatusDB,
	IChecker_ClearStatusDB,
	IChecker_GetStatusEx
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IChecker". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define IChecker_VERSION ((tVERSION)2)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IChecker_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IChecker_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "IChecker2", 10 )
	mLOCAL_PROPERTY_BUF( plDB_PATHNAME, ifsData, szDBFileName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( plDB_USER_DATA_SIZE, ifsData, dwUserDataSize, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( plDB_USER_VERSION, ifsData, dwUserVersion, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
mPROPERTY_TABLE_END(IChecker_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "IChecker". Register function
tERROR pr_call IChecker_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter IChecker::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_ICHECKER2,                          // interface identifier
		PID_ICHECKER2,                          // plugin identifier
		0x00000000,                             // subtype identifier
		IChecker_VERSION,                       // interface version
		VID_MIKE,                               // interface developer
		&i_IChecker_vtbl,                       // internal(kernel called) function table
		(sizeof(i_IChecker_vtbl)/sizeof(tPTR)), // internal function table size
		&e_IChecker_vtbl,                       // external function table
		(sizeof(e_IChecker_vtbl)/sizeof(tPTR)), // external function table size
		IChecker_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(IChecker_PropTable),// property table size
		sizeof(ifsData),                        // memory size
		0                                       // interface flags
	);

	if ( PR_FAIL(CALL_Root_RegisterCustomPropId(root, &propid_hash, npICHECKER2_HASH, pTYPE_QWORD)))
		return cFALSE;

	if ( PR_FAIL(error) )
		PR_TRACE( (root,prtDANGER,"IChecker(IID_ICHECKER) registered [error=0x%08x]",error) );

	PR_TRACE_A1( root, "Leave IChecker::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call IChecker_ObjectInit( hi_IChecker _this )
{
	tERROR error;
	PR_TRACE_A0( _this, "Enter IChecker::ObjectInit method" );

	error = CALL_SYS_ObjectCreateQuick(_this, &_this->data->hDataBase, IID_SECUREFILEDB, PID_SFDB, SUBTYPE_ANY );
	if ( PR_SUCC(error) )
		error = CALL_SYS_PropertySetDWord( _this->data->hDataBase , plSERIALIZE_OPTIONS, cSFDB_SERIALIZE_CHECKSUM | cSFDB_SERIALIZE_CRYPT );

	PR_TRACE_A1( _this, "Leave IChecker::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IChecker_ObjectInitDone( hi_IChecker _this )
{
	tERROR error;
	PR_TRACE_A0( _this, "Enter IChecker::ObjectInitDone method" );

	if ((_this->data->dwUserDataSize % 4) != 0)
	{
		PR_TRACE((_this, prtERROR, "DB_USER_DATA_SIZE must be aligned on 4, errOBJECT_CANNOT_BE_INITIALIZED"));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	if (PR_SUCC(error = CALL_SecureFileDatabase_Load(_this->data->hDataBase, _this->data->szDBFileName)))
	{
		tDWORD dwUserVersion; // AKA RecordIdAlg
		tDWORD dwRecordIDSize;
		tDWORD dwRecordDataSize;
		if (PR_SUCC(error = CALL_SecureFileDatabase_GetDBInfo(_this->data->hDataBase, &dwUserVersion, &dwRecordIDSize, &dwRecordDataSize)))
		{
			if (dwUserVersion != _this->data->dwUserVersion || dwRecordIDSize != 8 || 
				dwRecordDataSize != _this->data->dwUserDataSize)
			{
				PR_TRACE((_this, prtERROR, "IChecker_ObjectInitDone: errOBJECT_ALREADY_EXISTS"));
				error = errOBJECT_ALREADY_EXISTS;
			}
		}
	}
	if(PR_FAIL(error))
	{
		error = CALL_SecureFileDatabase_InitNew(_this->data->hDataBase, _this->data->dwUserVersion, 8, _this->data->dwUserDataSize, 0);
		if (PR_FAIL(error))
			PR_TRACE((_this, prtERROR, "SecureFileDatabase_InitNew failed %terr", error));
	}
	
	if (PR_FAIL(error))
	{
		CALL_SYS_ObjectClose(_this->data->hDataBase);
		_this->data->hDataBase = 0;
	}

	PR_TRACE_A1( _this, "Leave IChecker::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
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

	if (_this->data->hDataBase)
		error = CALL_SecureFileDatabase_Save(_this->data->hDataBase, _this->data->szDBFileName);

	PR_TRACE_A1( _this, "Leave IChecker::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



//#define _CALC_ICHECKER_TIME_
#if 0 //(defined(_DEBUG) || defined(_CALC_ICHECKER_TIME_))

#include "../../avpgs/timestats.h"

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

#define StatisticEnabled() (cFALSE)
#define ICheckerStatisticTimingStart(io) (errOK)
#define ICheckerStatisticTimingStop(io, error) (errOK)

#endif // _CALC_ICHECKER_TIME_



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetStatus )
// Extended method comment
//    Для получения ассоциированных данных, метод считает уникальный хеш для файла. Хеш считается по выборочным данным из файла (в зависимости от структуры файла), а также данным указанных в параметре pData. Затем, находит в базе данных значения, ассоциированные с этим хешем и возвращает их. Размер ассоциированных данных ограничен двумя  DWORD'ами.
// Behaviour comment
//    Возвращает errMODULE_UNKNOWN_FORMAT в случае, когда невозможно посчитать хеш для данного файла.
// Parameters are:
//   "dwDataSize"         : Указывается в байтах
tERROR pr_call IChecker_GetStatus( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize )
{
	return IChecker_GetStatusEx(_this, result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize, 0);
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UpdateStatus )
// Parameters are:
tERROR pr_call IChecker_UpdateStatus( hi_IChecker _this, hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize )
{
	tERROR error;
	tQWORD qwHash;
	tBOOL bStatEnabled;
	PR_TRACE_A0( _this, "Enter IChecker::UpdateStatus method" );

	if (dwStatusBufferSize != _this->data->dwUserDataSize)
		return errPARAMETER_INVALID;

	if ((bStatEnabled = StatisticEnabled())==cTRUE)
		ICheckerStatisticTimingStart(io);

	// Get CRC and extra info from io's custom property
	error = CALL_SYS_PropertyGet(io, NULL, propid_hash, &qwHash, sizeof(qwHash));
	if(PR_SUCC(error)){

		// Update in DataBase
		error = CALL_SecureFileDatabase_UpdateRecord(_this->data->hDataBase, &qwHash, pStatusBuffer);
#if defined(_PRAGUE_TRACE_)
		if (PR_SUCC(error))
		{
			char szFileName[0x200];
			CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szFileName, 0x200, cCP_ANSI);
			PR_TRACE((_this, prtNOTIFY, "iFileSecureStatus_UpdateStatus: %s - %I64X %08X %08X", szFileName, qwHash, pStatusBuffer[0], pStatusBuffer[1]));
		}
#endif
	}

	if (bStatEnabled)
		ICheckerStatisticTimingStop(io, error);

	PR_TRACE_A1( _this, "Leave IChecker::UpdateStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteStatus )
// Parameters are:
tERROR pr_call IChecker_DeleteStatus( hi_IChecker _this, hIO io )
{
	tERROR error;
	tQWORD qwHash;
	tBOOL bStatEnabled;
	PR_TRACE_A0( _this, "Enter IChecker::DeleteStatus method" );

	if ((bStatEnabled = StatisticEnabled())==cTRUE)
		ICheckerStatisticTimingStart(io);

	// Get CRC and extra info from io's custom property
	error = CALL_SYS_PropertyGet(io, NULL, propid_hash, &qwHash, sizeof(qwHash));
	if(PR_SUCC(error)){
		// Delete Custom property
		// Delete in DataBase
		error = CALL_SecureFileDatabase_DeleteRecord(_this->data->hDataBase, &qwHash);
#if defined(_PRAGUE_TRACE_)
		if (PR_SUCC(error))
		{
			char szFileName[0x200];
			CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szFileName, 0x200, cCP_ANSI);
			PR_TRACE((_this, prtNOTIFY, "iFileSecureStatus_DeleteStatus: %s - %I64X", szFileName, qwHash));
		}
#endif
	}

	if (bStatEnabled)
		ICheckerStatisticTimingStop(io, error);

	PR_TRACE_A1( _this, "Leave IChecker::DeleteStatus method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FlushStatusDB )
// Extended method comment
//    Метод предназначен для предотвращения потери накопленных в течении длительной сессии данных в результате действия внешних факторов (например, неожиданное выключение питания).
// Parameters are:
tERROR pr_call IChecker_FlushStatusDB( hi_IChecker _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IChecker::FlushStatusDB method" );

	if (!_this->data->hDataBase)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_SecureFileDatabase_Save(_this->data->hDataBase, _this->data->szDBFileName);

	PR_TRACE_A1( _this, "Leave IChecker::FlushStatusDB method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ClearStatusDB )
// Parameters are:
tERROR pr_call IChecker_ClearStatusDB( hi_IChecker _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IChecker::ClearStatusDB method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave IChecker::ClearStatusDB method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetStatusEx )
// Extended method comment
//    Для получения ассоциированных данных, метод считает уникальный хеш для файла. Хеш считается по выборочным данным из файла (в зависимости от структуры файла), а также данным указанных в параметре pData. Затем, находит в базе данных значения, ассоциированные с этим хешем и возвращает их. Размер ассоциированных данных ограничен двумя  DWORD'ами.
// Behaviour comment
//    Возвращает errMODULE_UNKNOWN_FORMAT в случае, когда невозможно посчитать хеш для данного файла.
// Parameters are:
//   "dwDataSize"         : Указывается в байтах
tERROR pr_call IChecker_GetStatusEx( hi_IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize, tDWORD dwFlags ) 
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD qwHash;
	tBOOL bStatEnabled;
	PR_TRACE_A0( _this, "Enter IChecker::GetStatusEx method" );

	// GetFormatType
	// CountCRC

	if (dwStatusBufferSize != _this->data->dwUserDataSize)
		return errPARAMETER_INVALID;

	if ((bStatEnabled = StatisticEnabled())==cTRUE)
		ICheckerStatisticTimingStart(io);

	error = GetFileHash((hOBJECT)io, (tBYTE*)&qwHash, sizeof(qwHash), pData, dwDataSize, dwFlags);
	if(PR_SUCC(error))
	{
		
		// Set hash to IO as custom properties
		if (PR_FAIL(error = CALL_SYS_PropertySet(io, NULL, propid_hash, &qwHash, sizeof(qwHash))))
			return error;
		
		// Find In database
		error = CALL_SecureFileDatabase_GetRecord(_this->data->hDataBase, &qwHash, pStatusBuffer);
		if (error == errNOT_FOUND)
		{
			// Return Status Found
			error = errOK;
		}
		
#if defined(_PRAGUE_TRACE_)
		if (PR_SUCC(error))
		{
			char szFileName[0x200];
			CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szFileName, 0x200, cCP_ANSI);
			PR_TRACE((_this, prtNOTIFY, "iFileSecureStatus_GetStatus: %s - %I64X %08X %08X", szFileName, qwHash, *pStatusBuffer, *pStatusBuffer+1));
		}
#endif

	}

	if ( result )
		*result = _this->data->dwUserDataSize;
	PR_TRACE_A2( _this, "Leave IChecker::GetStatusEx method, ret tDWORD = %u, error = 0x%08x", _this->data->dwUserDataSize, error );
	
	if (bStatEnabled)
		ICheckerStatisticTimingStop(io, error);

	return error;}
// AVP Prague stamp end



