// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  26 June 2006,  13:19 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- qb.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define QB_VERSION ((tVERSION)1)
// AVP Prague stamp end



#define IS if(PR_SUCC(error))
#define MAX_NAME_LEN  0x20

#define sQB_EXT ".klq"
#define sQB_MASK "????????????????" sQB_EXT



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
#include <ProductCore/iface/i_posio_sp.h>
#include <Prague/iface/i_io.h>
#include <ProductCore/iface/i_task.h>
#include <ProductCore/plugin/p_qb.h>
// AVP Prague stamp end



#include <Prague/pr_serializable.h>

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_win32_nfio.h>
#include <string.h>
#include <Prague/iface/i_csect.h>

#define HASH_KEY LONG_LONG_CONST(0xAC5C0E69EC4845E2)

typedef struct tag_tCOUNTERS
{
	tDWORD dwTotal;
	tDWORD dwInfected;
	tDWORD dwProtected;
	tQWORD qwTotalSize;
} tCOUNTERS;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable QBStorage : public cQB {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call getStoragePath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setStoragePath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call StoreObject( hOBJECT p_hObject, tPID p_RestorerPID, const cSerializable* p_pInfo, hPOSIO_SP* p_phSecIO, tOBJECT_ID* p_pObjectID );
	tERROR pr_call RestoreObject( tOBJECT_ID p_ObjectID, const tWSTRING p_sRestoreFullName, tBOOL p_bReplaceExisting );
	tERROR pr_call DeleteObject( tOBJECT_ID p_ObjectID );
	tERROR pr_call GetObject( hPOSIO_SP* p_phSecIO, tOBJECT_ID p_ObjectID, tBOOL p_bWriteAccess, cSerializable* p_pInfo );
	tERROR pr_call GetCount( tDWORD* p_pdwCount, tCOUNT_TYPE p_CountType );
	tERROR pr_call GetObjectByIndex( hPOSIO_SP* p_phSecIO, tOBJECT_ID* p_pObjectID, tDWORD p_dwIndex, tBOOL p_bWriteAccess, cSerializable* p_pInfo );
	tERROR pr_call UpdateObjectInfo( tOBJECT_ID p_ObjectID, const cSerializable* p_pInfo );

// Data declaration
	tDWORD m_dwStorageId; // --
// AVP Prague stamp end



	hObjPtr   m_hPtr;
	tDWORD    m_dwPtrPos;
	tCHAR*    m_pStoragePath;
	hSTRING   m_hStoragePath;
	hCRITICAL_SECTION m_hCritSect;
	tBOOL     m_bStateOk;
	tCOUNTERS m_Counters;

	tERROR Init();
	tERROR iGetObject( hPOSIO_SP* p_phSecIO, tOBJECT_ID p_ObjectID, tBOOL p_bOpenExisting, tBOOL p_bWriteAccess, cSerializable* p_pInfo );
	tERROR iSetObjectName(hPOSIO_SP p_hSecIO, tOBJECT_ID p_ObjectID);
	tERROR iStoreQBObject(hOBJECT p_hObject, hPOSIO_SP* p_phSecIO, tOBJECT_ID* p_pObjectID, const cSerializable* p_pInfo);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(QBStorage)
};
// AVP Prague stamp end



tERROR GetObjectID(hOBJECT p_hObj, tOBJECT_ID* p_pObjectID)
{
	tERROR error;
	unsigned char ucName[0x200];
	tDWORD i;
	tOBJECT_ID id = 0;

	int ii=-1;

	if (PR_FAIL(error = p_hObj->propGetStr(NULL, pgOBJECT_NAME, ucName, 0x200, cCP_ANSI)))
		return error;

	for (i=0; i<sizeof(tOBJECT_ID)*2;i++)
	{
		unsigned char c = ucName[i]|0x20;
		id <<= 4;
		if (c>='0' && c<='9')
			id |= (c - '0');
		else if (c>='a' && c<='f')
			id |= (c - 'a' + 10);
		else
			return errOBJECT_INCOMPATIBLE;
	}
	if (ucName[i] != '.')
		return errOBJECT_INCOMPATIBLE;
	if (p_pObjectID)
		*p_pObjectID = id;
	return errOK;
}

tERROR GetObjectName(tOBJECT_ID p_ObjectID, hSTRING p_hsName)
{
	tDWORD i, error;
	tOBJECT_ID id = p_ObjectID;
	unsigned char uc;
	const char szKlqStr[] = sQB_EXT;

	error = CALL_String_Cut(p_hsName, cSTRING_WHOLE, fSTRING_INNER);


	for (i=0; i<sizeof(tOBJECT_ID)*2;i++)
	{
		tDWORD c = (tDWORD)(id >> 60);
		id <<= 4;
		if (c<=9)
			uc = c + '0';
		else 
			uc = c - 10 + 'a';
		
		IS error = CALL_String_AddFromBuff(p_hsName, 0, &uc, 1, cCP_ANSI, cSTRING_Z);
	}
	
	IS error = CALL_String_AddFromBuff(p_hsName, 0, (void*)szKlqStr, sizeof(szKlqStr), cCP_ANSI, cSTRING_Z);

	return error;
}

tERROR QBStorage::iSetObjectName(hPOSIO_SP p_hSecIO, tOBJECT_ID p_ObjectID)
{
	tERROR error;
	hSTRING hsName;
	tDWORD dwLen;
	unsigned char ucBuff[0x200];

	error = sysCreateObjectQuick((hOBJECT*)&hsName, IID_STRING);
	IS error = GetObjectName(p_ObjectID, hsName);
	IS error = hsName->ExportToBuff(&dwLen, cSTRING_WHOLE, ucBuff, sizeof(ucBuff), cCP_ANSI, cSTRING_Z);
	IS error = p_hSecIO->set_pgOBJECT_NAME(ucBuff, dwLen, cCP_ANSI);
	hsName->sysCloseObject();
	return error;
}


// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "QB". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR QBStorage::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "QB::ObjectInit method" );
	error = sysCreateObjectQuick((hOBJECT*)&m_hCritSect, IID_CRITICAL_SECTION);
	IS error = sysCreateObjectQuick((hOBJECT*)&m_hStoragePath, IID_STRING);
	IS error = sysCreateObjectQuick((hOBJECT*)&m_hPtr, IID_OBJPTR, PID_NATIVE_FIO);
	return error;
}
// AVP Prague stamp end



//tERROR QBStorage::GetStringValueFromReg(hREGISTRY p_hReg, tRegKey p_key, const tSTRING p_sValueName, hSTRING* p_phString)
//{
//	tERROR error;
//	tTYPE_ID type = tid_STRING;
//	tPTR pData;
//	tDWORD dwSize;
//	hSTRING hString = NULL;
//	tBOOL bStrCreated = cFALSE;
//	if (p_hReg == NULL || p_phString == NULL)
//		return errPARAMETER_INVALID;
//	if (*p_phString == NULL)
//	{
//		error = sysCreateObjectQuick((hOBJECT*)&hString, IID_STRING);
//		bStrCreated = cTRUE;
//	}
//	else
//		hString = *p_phString;
//
//	if (PR_SUCC(error))
//		error = p_hReg->GetValue(&dwSize, p_key, p_sValueName, &type, NULL, 0);
//
//	if (PR_SUCC(error) && (type != tid_STRING && type != tid_WSTRING))
//		error = errNOT_MATCHED;
//
//	if (PR_SUCC(error) && (dwSize == 0))
//	{
//		error = m_hCritSect->Enter( SHARE_LEVEL_WRITE );
//		if ( PR_SUCC(error) ) {
//			error = hString->ImportFromBuff(NULL, "", sizeof(tCHAR), cCP_ANSI, cSTRING_Z);
//			m_hCritSect->Leave( 0 );
//		}
//		return error;
//	}
//
//	if (PR_SUCC(error))
//		error = heapAlloc(&pData, dwSize);
//	if (PR_SUCC(error))
//		error = p_hReg->GetValue(NULL, p_key, p_sValueName, &type, pData, dwSize);
//	if (PR_SUCC(error)) {
//		error = m_hCritSect->Enter( SHARE_LEVEL_WRITE );
//		if ( PR_SUCC(error) ) {
//			error = hString->ImportFromBuff(NULL, pData, dwSize, (type == tid_STRING ? cCP_ANSI : cCP_UNICODE), cSTRING_Z);
//			m_hCritSect->Leave( 0 );
//		}
//	}
//	if (PR_FAIL(error) && hString && bStrCreated)
//	{
//		hString->sysCloseObject();
//		hString = NULL;
//	}
//	heapFree(pData);
//	*p_phString = hString;
//	return error;
//}

tERROR QBStorage::Init()
{
	tERROR error;
	
	if (PR_SUCC(error = m_hCritSect->Enter(SHARE_LEVEL_WRITE)))
	{
		char aMask [] = sQB_MASK;
		m_bStateOk = cFALSE;
		
		//error = m_hStoragePath->ExportToProp(NULL, cSTRING_WHOLE, (hOBJECT)m_hPtr, pgOBJECT_PATH);
		//m_hStoragePath->ProveLastSlash();
		error = m_hPtr->ChangeTo(*m_hStoragePath);
		if (PR_FAIL(error))
		{
			PR_TRACE((this, prtERROR, "qb\tInit: ChangeTo(%tstr) failed with %terr", m_hStoragePath, error));
		}
		IS { error = m_hPtr->StepDown();
			if (PR_FAIL(error))
			{
				PR_TRACE((this, prtERROR, "qb\tInit: StepDown(%tstr) failed with %terr", m_hStoragePath, error));
			}
		}
		IS error = m_hPtr->set_pgMASK(aMask, 0, cCP_ANSI);
		IS m_bStateOk = cTRUE;
		IS m_dwPtrPos = (tDWORD)-1;
		m_hCritSect->Leave(NULL);
	}
	IS error = sysRegisterMsgHandler(pmc_POS_IO, rmhLISTENER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	
	return error;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR QBStorage::ObjectInitDone()
{
	tERROR error;
	PR_TRACE_A0( this, "Enter QB::ObjectInitDone method" );

	error = Init();
	if (PR_FAIL(error))
		PR_TRACE((this, prtERROR, "qb\tInit failed with %terr", error));
	else
		PR_TRACE((this, prtNOTIFY, "qb\tInit successed on %tstr", m_hStoragePath));

	PR_TRACE_A1( this, "Leave QB::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR QBStorage::ObjectPreClose()
{
	tERROR error;
	PR_TRACE_A0( this, "Enter QB::ObjectPreClose method" );

	if (m_hPtr)
		error = m_hPtr->sysCloseObject();
	if (m_hStoragePath)
		error = m_hStoragePath->sysCloseObject();
	if (m_hCritSect)
		error = m_hCritSect->sysCloseObject();

	PR_TRACE_A1( this, "Leave QB::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR QBStorage::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	tOBJECT_ID ObjectId;
	tDWORD size;
	PR_TRACE_FUNC_FRAME( "QB::MsgReceive method" );

	if (p_msg_cls_id == pmc_POS_IO)
	{
		if (p_msg_id == pm_QB_IO_DELETE && NULL == p_par_buf)
		{
			 if (PR_SUCC(GetObjectID(p_send_point, &ObjectId)))
			 {
				 p_par_buf = &ObjectId;
				 size = sizeof(ObjectId);
				 p_par_buf_len = &size;
			 }
		}
		error = sysSendMsg(pmc_QBSTORAGE, p_msg_id, p_send_point, p_par_buf, p_par_buf_len);
	}

	return error;
}
// AVP Prague stamp end



//// AVP Prague stamp begin( Property method implementation, getConfig )
//// Interface "QB". Method "Get" for property(s):
////  -- TASK_CONFIG
//tERROR QBStorage::getConfig( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
//{
//	PR_TRACE_A0( this, "Enter *GET* method getConfig for property pgTASK_CONFIG" );
//
//	*out_size = 0;
//
//	PR_TRACE_A1( this, "Leave *GET* method getConfig for property pgTASK_CONFIG, ret tDWORD = %u(size), err=errNOT_SUPPORTED", *out_size );
//	return errNOT_SUPPORTED;
//}
//// AVP Prague stamp end
//
//
//
//// AVP Prague stamp begin( Property method implementation, setConfig )
//// Interface "QB". Method "Set" for property(s):
////  -- TASK_CONFIG
//tERROR QBStorage::setConfig( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
//{
//	tERROR error = errOK;
//	PR_TRACE_A0( this, "Enter *SET* method setConfig for property pgTASK_CONFIG" );
//
//	error = GetStringValueFromReg(*(hREGISTRY*)buffer, cRegRoot, "StoragePath", &m_hStoragePath);
//	if ( PR_SUCC(error) )
//		propInitialized( plQB_STORAGE_PATH, cTRUE );
//	*out_size = sizeof(hREGISTRY);
//
//	PR_TRACE_A2( this, "Leave *SET* method setConfig for property pgTASK_CONFIG, ret tDWORD = %u(size), %terr", *out_size, error );
//	return error;
//}
//// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getStoragePath )
// Interface "QB". Method "Get" for property(s):
//  -- QB_STORAGE_PATH
tERROR QBStorage::getStoragePath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error;
	PR_TRACE_A0( this, "Enter *GET* method getStoragePath for property plQB_STORAGE_PATH" );

	error = m_hCritSect->Enter( SHARE_LEVEL_READ );
	if ( PR_SUCC(error) ) {
		error = m_hStoragePath->ExportToBuff( out_size, cSTRING_WHOLE, buffer, size, cCP_ANSI, cSTRING_Z );
		m_hCritSect->Leave( 0 );
	}

	PR_TRACE_A2( this, "Leave *GET* method getStoragePath for property plQB_STORAGE_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setStoragePath )
// Interface "QB". Method "Set" for property(s):
//  -- QB_STORAGE_PATH
tERROR QBStorage::setStoragePath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method setStoragePath for property plQB_STORAGE_PATH" );

	error = m_hCritSect->Enter( SHARE_LEVEL_WRITE );
	if ( PR_SUCC(error) ) {
		error = m_hStoragePath->ImportFromBuff( out_size, buffer, size, cCP_ANSI, cSTRING_Z );
		m_hCritSect->Leave( 0 );
	}

	PR_TRACE_A2( this, "Leave *SET* method setStoragePath for property plQB_STORAGE_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end

tERROR QBStorage::iStoreQBObject(hOBJECT p_hObject, hPOSIO_SP* p_phSecIO, tOBJECT_ID* p_pObjectID, const cSerializable* p_pInfo)
{
	tERROR error = errOK;
	cAutoObj<cString> hDstName = NULL;
	cAutoObj<cObjPtr> hTmpPtr = NULL;
	tOBJECT_ID id;

	error = GetObjectID(p_hObject, &id);
	IS error = sysCreateObjectQuick((hOBJECT*)&hDstName, IID_STRING);
	IS {
		cAutoCS _lock(m_hCritSect, false);
		error = hDstName->ImportFromStr(NULL, m_hStoragePath, cSTRING_WHOLE);
	}
	IS error = hDstName->ProveLastSlash();
	IS error = hDstName->AddFromProp(NULL, p_hObject, pgOBJECT_NAME);
	IS error = sysCreateObjectQuick((hOBJECT*)&hTmpPtr, IID_OBJPTR, PID_NATIVE_FIO);
	IS error = hTmpPtr->ChangeTo(p_hObject);
	IS error = hTmpPtr->Copy(hDstName, cTRUE);
	IS if (p_pInfo) error = UpdateObjectInfo(id, p_pInfo);

	m_Counters.dwTotal=0;
	m_dwPtrPos = (tDWORD)-2; // need to be resetted on next get
	tDWORD len = sizeof(id);
	IS sysSendMsg(pmc_QBSTORAGE, pm_QB_IO_STORE, NULL, &id, &len);

	if (PR_SUCC(error))
	{
		if (p_pObjectID) 
			*p_pObjectID = id;
		if (p_phSecIO)
			error = iGetObject(p_phSecIO, id, cTRUE, cTRUE, NULL);
	}

	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "qb\tQBStorage::DeleteObject failed with %terr", error));
	}
	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, StoreObject )
// Extended method comment
//    Сохраняет содержимое объекта hObject в хранилище, после чего возвращает объект hSecIO с интерфейсом hPOSIO_SP для созданного объекта и идентификатор созданного объекта. Приложение должно закрыть полученный объект hSecIO после использования вызовом sysObjectClose.
// Behaviour comment
//    Поля sFullName, sUser, sMachine, sDomain используются для генерации идентификатора объекта. Если при добавлении оказывается, что в хранилище уже существует объект с таки-же идентификатором, то старый объект удаляется а новый кладется на его место.
// Parameters are:
//   "RestorerPID" : Идентификатор плагина в котором содержится имплементация IID_QBRESTORER, которая будет использоваться для восстановления объекта.
//   "phSecIO"     : Получив данный объект после вызова StoreObject, приложение может установить на него дополнительные свойства, читать/изменять содержимое объекта и т.п.
tERROR QBStorage::StoreObject( hOBJECT p_hObject, tPID p_RestorerPID, const cSerializable* p_pInfo, hPOSIO_SP* p_phSecIO, tOBJECT_ID* p_pObjectID )
{
	tERROR error;
	tPID RestorerPID = PID_ANY;
	hQBRESTORER hRestorer = NULL;
	tOBJECT_ID ObjectID;
	hPOSIO_SP hSecIO = NULL;

	PR_TRACE_A0( this, "Enter QB::StoreObject method" );

	if (p_hObject == NULL)
		return errPARAMETER_INVALID;
	if (!m_bStateOk)
		return errOBJECT_BAD_INTERNAL_STATE;

	if (p_pObjectID)
		*p_pObjectID = 0;
	if (p_phSecIO)
		*p_phSecIO = NULL;

	if (p_RestorerPID == PID_ANY) // use default implementation from this plugin
		p_RestorerPID = PID_QB;
	
	if ( p_RestorerPID == PID_QB && PID_QB == p_hObject->propGetDWord(pgPLUGIN_ID) && IID_POSIO_SP == p_hObject->propGetDWord(pgINTERFACE_ID))
	{
		error = iStoreQBObject(p_hObject, p_phSecIO, p_pObjectID, p_pInfo);
		return error;
	}


	if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hRestorer, IID_QBRESTORER, p_RestorerPID)))
	{
		error = hRestorer->MakeObjectID(p_hObject, &ObjectID);
		if (PR_SUCC(error) && p_pObjectID)
			*p_pObjectID = ObjectID;
		if (PR_SUCC(error = iGetObject(&hSecIO, ObjectID, cFALSE, cTRUE, NULL)))
		{
			bool bObjectInfoExist = false;
			error = hSecIO->SetStoredProperty(NULL, cNP_QB_ID, &ObjectID, sizeof(ObjectID));
			if (PR_SUCC(error))
				error = hRestorer->StoreObject(p_hObject, hSecIO);
			if (warnOBJECT_INFO_EXIST == error)
				bObjectInfoExist = true;
			if (PR_SUCC(error))
				error = hSecIO->SetStoredProperty(NULL, cNP_QB_RESTORER_PID, &p_RestorerPID, sizeof(tPID));
			if (PR_SUCC(error) && p_pInfo && !bObjectInfoExist)
			{
				tBYTE* pBuff;
				tDWORD dwSize;
				error = g_root->StreamSerialize(p_pInfo, SERID_UNKNOWN, NULL, 0, &dwSize);
				if (PR_SUCC(error))
					error = heapAlloc((tPTR*)&pBuff, dwSize);
				if (PR_SUCC(error))
				{
					error = g_root->StreamSerialize(p_pInfo, SERID_UNKNOWN, pBuff, dwSize, &dwSize);
					if (PR_SUCC(error))
						error = hSecIO->SetStoredProperty(NULL, cNP_QB_INFO, pBuff, dwSize);
					heapFree(pBuff);
				}
			}
			if (PR_SUCC(error))
			{
				tDWORD len = sizeof(ObjectID);
				sysSendMsg(pmc_QBSTORAGE, pm_QB_IO_STORE, NULL, &ObjectID, &len);
			}

			if (PR_SUCC(error))
				error = hSecIO->Flush();

			if (PR_FAIL(error))
				hSecIO->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
			
			if (PR_SUCC(error) && p_phSecIO)
				*p_phSecIO = hSecIO;
			else
				hSecIO->sysCloseObject();
		}

		hRestorer->sysCloseObject();
	}

	m_Counters.dwTotal=0;

	PR_TRACE_A1( this, "Leave QB::StoreObject method, ret %terr", error );
	return error;
}



// AVP Prague stamp begin( External (user called) interface method implementation, RestoreObject )
// Extended method comment
//    Восстанавливает объект по указанному либо оригинальному пути.
// Parameters are:
//   "ObjectID"         : Идентификатор объекта может быть получен из методов StoreObject или GetObjectByIndex
tERROR QBStorage::RestoreObject( tOBJECT_ID p_ObjectID, const tWSTRING p_sRestoreFullName, tBOOL p_bReplaceExisting ) {
	tERROR error;
	hPOSIO_SP hSecIO = NULL;
	PR_TRACE_A0( this, "Enter QB::RestoreObject method" );

	if (!m_bStateOk)
		return errOBJECT_BAD_INTERNAL_STATE;
	error = iGetObject(&hSecIO, p_ObjectID, cTRUE, cFALSE, NULL);
	if (PR_SUCC(error))
	{
		tDWORD len = sizeof(p_ObjectID);
		sysSendMsg(pmc_QBSTORAGE, pm_QB_IO_RESTORE, NULL, &p_ObjectID, &len); // reset driver's cache

		hQBRESTORER hRestorer = NULL;
		tPID RestorerPID = PID_ANY;
		error = hSecIO->GetStoredProperty(NULL, cNP_QB_RESTORER_PID, &RestorerPID, sizeof(RestorerPID));
		if (RestorerPID == PID_ANY) // use default implementation from this plugin
			RestorerPID = PID_QB;
		if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hRestorer, IID_QBRESTORER, RestorerPID)))
		{
			error = hRestorer->RestoreObject(hSecIO, p_sRestoreFullName, p_bReplaceExisting);
			hRestorer->sysCloseObject();
		}
		hSecIO->sysCloseObject();
	}

	PR_TRACE_A1( this, "Leave QB::RestoreObject method, ret %terr", error );
	return error;
}
// AVP Prague stamp end




// AVP Prague stamp begin( External (user called) interface method implementation, DeleteObject )
// Extended method comment
//    Удаляет объект в хранилище по его идентификатору
// Parameters are:
//   "ObjectID" : Идентификатор объекта может быть получен из методов StoreObject или GetObjectByIndex
tERROR QBStorage::DeleteObject( tOBJECT_ID p_ObjectID )
{
	tERROR error;
	hSTRING hFullName, hShortName;
	hObjPtr hTmpPtr;
	PR_TRACE_A0( this, "Enter QB::DeleteObject method" );

	if (!m_bStateOk)
		return errOBJECT_BAD_INTERNAL_STATE;

	if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hFullName, IID_STRING)))
	{
		if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hShortName, IID_STRING)))
		{

			if (PR_SUCC(error = GetObjectName(p_ObjectID, hShortName)))
			{
				error = m_hCritSect->Enter( SHARE_LEVEL_READ );
				if ( PR_SUCC(error) )
				{
					error = hFullName->ImportFromStr(NULL, m_hStoragePath, cSTRING_WHOLE);
					m_hCritSect->Leave( 0 );
				}
				
				IS error = hFullName->ProveLastSlash();
				IS error = hFullName->AddFromStr(NULL, hShortName, cSTRING_WHOLE);
				if (PR_FAIL(error))
				{
					hFullName->sysCloseObject();
					return error;
				}

				error = sysCreateObjectQuick((hOBJECT*)&hTmpPtr, IID_OBJPTR, PID_NATIVE_FIO);
				if (PR_SUCC(error))
				{
					error = hTmpPtr->ChangeTo(*hFullName);
					if (PR_FAIL(error))
					{
						cStrObj cTemp;
						cTemp.assign(*hFullName);
						PR_TRACE((this, prtERROR, "qb\tQBStorage::DeleteObject, ChangeTo(%S) failed with %terr", cTemp.data(), error));
					}
					IS error = hTmpPtr->Delete();
					if (PR_FAIL(error))
					{
						PR_TRACE((this, prtERROR, "qb\tQBStorage::DeleteObject failed with %terr", error));
					}
					m_Counters.dwTotal=0;
					m_dwPtrPos = (tDWORD)-2; // need to be resetted on next get
					
					tDWORD len = sizeof(p_ObjectID);
					IS sysSendMsg(pmc_QBSTORAGE, pm_QB_IO_DELETE, NULL, &p_ObjectID, &len);

					hTmpPtr->sysCloseObject();
				}
			}
			hShortName->sysCloseObject();
		}
		hFullName->sysCloseObject();
	}

	PR_TRACE_A1( this, "Leave QB::DeleteObject method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



tERROR QBStorage::iGetObject( hPOSIO_SP* p_phSecIO, tOBJECT_ID p_ObjectID, tBOOL p_bOpenExisting, tBOOL p_bWriteAccess, cSerializable* p_pInfo )
{
	tERROR error = errOK;
	hPOSIO_SP hSecIO = NULL;
	tQWORD qwHash = HASH_KEY;
	PR_TRACE_A0( this, "Enter QB::iGetObject method" );

	//if (p_phSecIO == NULL)
	//	error = errPARAMETER_INVALID;
	if (!m_bStateOk)
		return errOBJECT_BAD_INTERNAL_STATE;

	if (p_bWriteAccess && PrCreateDir)
		PrCreateDir((hOBJECT)m_hStoragePath);

	if (PR_SUCC(error = sysCreateObject((hOBJECT*)&hSecIO, IID_POSIO_SP, PID_QB)))
	{
		IS error = m_hCritSect->Enter( SHARE_LEVEL_WRITE );
		if ( PR_SUCC(error) ) {
			error = m_hStoragePath->ExportToProp(NULL, cSTRING_WHOLE, *hSecIO, pgOBJECT_PATH);
			m_hCritSect->Leave( 0 );
		}
		
		IS error = iSetObjectName(hSecIO, p_ObjectID);
		IS error = hSecIO->set_pgOBJECT_OPEN_MODE( (p_bOpenExisting ? fOMODE_OPEN_IF_EXIST : fOMODE_CREATE_IF_NOT_EXIST) | (p_bWriteAccess ? fOMODE_SHARE_DENY_RW : 0) | fOMODE_SHARE_DENY_DELETE);
		IS error = hSecIO->set_pgOBJECT_ACCESS_MODE(p_bWriteAccess ? fACCESS_RW : fACCESS_READ);
		IS error = hSecIO->propSet(NULL, plHASH_KEY, &qwHash, sizeof(qwHash));
		IS error = hSecIO->propSetDWord(plHASH_ALGORITHM, cHASH_ALGORITHM_XOR);
		IS error = hSecIO->sysCreateObjectDone();

		if (PR_SUCC(error) && p_pInfo)
		{
			tBYTE* pBuff;
			tDWORD dwSize;
			error = hSecIO->GetStoredProperty(&dwSize, cNP_QB_INFO, NULL, 0);
			if (PR_SUCC(error))
				error = heapAlloc((tPTR*)&pBuff, dwSize);
			if (PR_SUCC(error))
			{
				error = hSecIO->GetStoredProperty(&dwSize, cNP_QB_INFO, pBuff, dwSize);
				if (PR_SUCC(error))
					error = g_root->StreamDeserialize(&p_pInfo, pBuff, dwSize, &dwSize);
				heapFree(pBuff);
			}
		}	

		if (PR_FAIL(error) || p_phSecIO == NULL)
		{
			hSecIO->sysCloseObject();
			hSecIO = NULL;
		}
	}

	if (p_phSecIO)
		*p_phSecIO = hSecIO;
	
	PR_TRACE_A1( this, "Leave QB::GetObject method, ret %terr", error );
	return error;
}


// AVP Prague stamp begin( External (user called) interface method implementation, GetObject )
// Extended method comment
//    Получает объект hPOSIO_SP для доступа к сохраненным данным и свойствам по идентификатору объекта в хранилище. Приложение должно закрыть полученный объект hSecIO после использования вызовом sysObjectClose.
// Parameters are:
//   "phSecIO"  : Получает объект hPOSIO_SP для доступа к сохраненным данным и свойствам.
//   "ObjectID" : Идентификатор объекта может быть получен из методов StoreObject или GetObjectByIndex
tERROR QBStorage::GetObject( hPOSIO_SP* p_phSecIO, tOBJECT_ID p_ObjectID, tBOOL p_bWriteAccess, cSerializable* p_pInfo )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter QB::GetObject method" );

	error = iGetObject(p_phSecIO, p_ObjectID, cTRUE, p_bWriteAccess, p_pInfo);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetCount )
// Extended method comment
//    Возвращает кол-во объектов указанного типа в хранилище.
// Parameters are:
//   "pdwCount"  : Получает кол-во объектов указанного типа в хранилище
//   "CountType" : Указывает, о каких объектах требуется получить ингформацию. Принимает значения cCOUNT_xxx
tERROR QBStorage::GetCount( tDWORD* p_pdwCount, tCOUNT_TYPE p_CountType )
{
	tERROR error;
	PR_TRACE_A0( this, "Enter QB::GetCount method" );
	
	if (p_pdwCount == NULL)
		return errPARAMETER_INVALID;
	if (!m_bStateOk)
		return errOBJECT_BAD_INTERNAL_STATE;

	if (m_Counters.dwTotal==0 || (p_CountType & cCOUNT_REFRESH)) // refresh counts
	{
		if (PR_SUCC(error = m_hCritSect->Enter(SHARE_LEVEL_WRITE)))
		{
			tCOUNTERS Counters;
			memset(&Counters, 0, sizeof(Counters));
			error = m_hPtr->Reset(cFALSE);
			
			while (PR_SUCC(error = m_hPtr->Next()))
			{
				tDWORD dwAttr = m_hPtr->get_pgOBJECT_ATTRIBUTES();
				if (dwAttr & fQB_ATTRIBUTE_LOCKED)
				{
					hIO temp;
					if (PR_SUCC(m_hPtr->IOCreate(&temp, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST)))
					{
						temp->sysCloseObject();
						m_hPtr->set_pgOBJECT_ATTRIBUTES(dwAttr & ~fQB_ATTRIBUTE_LOCKED);
					}
					else
						continue;
				}
				Counters.qwTotalSize += m_hPtr->get_pgOBJECT_SIZE_Q();
				Counters.dwTotal++;
				if (dwAttr & fQB_ATTRIBUTE_INFECTED)
					Counters.dwInfected++;
				if (dwAttr & fQB_ATTRIBUTE_PROTECTED)
					Counters.dwProtected++;
			}
			memcpy(&m_Counters, &Counters, sizeof(Counters));
			m_dwPtrPos = (tDWORD)-2; // need to be resetted on next get
			m_hCritSect->Leave(NULL);

			PR_TRACE((this, prtIMPORTANT, "qb\tcCOUNT_REFRESH: Total(%u), TotalSize(%I64d)", Counters.dwTotal, Counters.qwTotalSize));
		}
	}

	p_CountType &= ~cCOUNT_REFRESH;

	error = errOK;
	switch(p_CountType)
	{
	case cCOUNT_TOTAL:
		*p_pdwCount = m_Counters.dwTotal;
		break;
	case cCOUNT_INFECTED:
		*p_pdwCount = m_Counters.dwInfected;
		break;
	case cCOUNT_PROTECTED:
		*p_pdwCount = m_Counters.dwProtected;
		break;
	case cCOUNT_TOTAL_SIZE:
		if (m_Counters.qwTotalSize > 0xFFFFFFFF)
			*p_pdwCount = 0xFFFFFFFF;
		else
			*p_pdwCount = (tDWORD)m_Counters.qwTotalSize;
		break;
	default:
		error = errPARAMETER_INVALID;
	}

	PR_TRACE_A1( this, "Leave QB::GetCount method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetObjectByIndex )
// Extended method comment
//    Получает объект и/или его идентификатор объекта по его индексу(номеру). Приложение должно закрыть полученный объект hSecIO после использования вызовом sysObjectClose.
// Behaviour comment
//    Если оба параметра phSecIO и pObjectID не указаны (=NULL), метод возвращает errPARAMETER_INVALID.
// Parameters are:
//   "phSecIO"   : Получает объект hPOSIO_SP для доступа к сохранным данным и свойствам.
//   "dwIndex"   : Индекс (номер) объекта, который требуется получить
tERROR QBStorage::GetObjectByIndex( hPOSIO_SP* p_phSecIO, tOBJECT_ID* p_pObjectID, tDWORD p_dwIndex, tBOOL p_bWriteAccess, cSerializable* p_pInfo )
{
	tERROR error;
	hPOSIO_SP hSecIO = NULL;
	PR_TRACE_A0( this, "Enter QB::GetObjectByIndex method" );

	if (p_phSecIO == NULL && p_pObjectID == NULL && p_pInfo == NULL)
		return errPARAMETER_INVALID;
	if (!m_bStateOk)
		return errOBJECT_BAD_INTERNAL_STATE;

	if (p_phSecIO)
		*p_phSecIO = 0;

	if (PR_SUCC(error = m_hCritSect->Enter(SHARE_LEVEL_WRITE)))
	{
		if (p_dwIndex < m_dwPtrPos)
		{
			m_dwPtrPos = (tDWORD)-1;
			error = m_hPtr->Reset(cFALSE);
		}
		
		while (PR_SUCC(error) && p_dwIndex != m_dwPtrPos)
		{
			while (PR_SUCC(error = m_hPtr->Next()))
			{
				if (m_hPtr->get_pgOBJECT_ATTRIBUTES() & fQB_ATTRIBUTE_LOCKED)
					continue;
				m_dwPtrPos++;
				break;
			}
			if (PR_FAIL(error))
				m_dwPtrPos = (tDWORD)-2; // need to be resetted on next get
		}
		
		if (PR_SUCC(error))
		{
			tOBJECT_ID ObjectID;
			error = GetObjectID(*m_hPtr, &ObjectID);
			if (PR_SUCC(error) && p_pObjectID)
				*p_pObjectID = ObjectID;

			if (PR_SUCC(error) && (p_phSecIO || p_pInfo))
				error = iGetObject(p_phSecIO, ObjectID, cTRUE, p_bWriteAccess, p_pInfo);
		}
		m_hCritSect->Leave(NULL);
	}

	PR_TRACE_A1( this, "Leave QB::GetObjectByIndex method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateObjectInfo )
// Extended method comment
//    Сохраняет cSerializable структуру в объекте
// Parameters are:
//   "ObjectID" : Идентификатор объекта может быть получен из методов StoreObject или GetObjectByIndex
tERROR QBStorage::UpdateObjectInfo( tOBJECT_ID p_ObjectID, const cSerializable* p_pInfo )
{
	tERROR error = errOK;
	hPOSIO_SP hSecIO = NULL;
	PR_TRACE_FUNC_FRAME( "QB::UpdateObjectInfo method" );

	if (p_pInfo == NULL)
		return errPARAMETER_INVALID;
	error = iGetObject(&hSecIO, p_ObjectID, cTRUE, cTRUE, NULL);
	if (PR_SUCC(error))
	{
		tBYTE* pBuff;
		tDWORD dwSize;
		tERROR error_close = errOK;
		error = g_root->StreamSerialize(p_pInfo, SERID_UNKNOWN, NULL, 0, &dwSize);
		if (PR_SUCC(error))
			error = heapAlloc((tPTR*)&pBuff, dwSize);
		if (PR_SUCC(error))
		{
			error = g_root->StreamSerialize(p_pInfo, SERID_UNKNOWN, pBuff, dwSize, &dwSize);
			if (PR_SUCC(error))
				error = hSecIO->SetStoredProperty(NULL, cNP_QB_INFO, pBuff, dwSize);
			if (PR_SUCC(error))
			{
				tDWORD len = sizeof(p_ObjectID);
				sysSendMsg(pmc_QBSTORAGE, pm_QB_IO_MODIFIED, NULL, &p_ObjectID, &len);
			}
			heapFree(pBuff);
		}
		if (PR_SUCC(error))
			error = hSecIO->Flush();
		hSecIO->sysCloseObject();
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "QB". Register function
tERROR QBStorage::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(QB_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, QB_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "QB", 3 )
	mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( plQB_STORAGE_PATH, FN_CUST(getStoragePath), FN_CUST(setStoragePath) )
	mpLOCAL_PROPERTY_BUF( plQB_STORAGE_ID, QBStorage, m_dwStorageId, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(QB_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(QB)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(QB)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(QB)
	mEXTERNAL_METHOD(QB, StoreObject)
	mEXTERNAL_METHOD(QB, RestoreObject)
	mEXTERNAL_METHOD(QB, DeleteObject)
	mEXTERNAL_METHOD(QB, GetObject)
	mEXTERNAL_METHOD(QB, GetCount)
	mEXTERNAL_METHOD(QB, GetObjectByIndex)
	mEXTERNAL_METHOD(QB, UpdateObjectInfo)
mEXTERNAL_TABLE_END(QB)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter QB::Register method" );

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_QB,                                 // interface identifier
//! 		PID_QB,                                 // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		QB_VERSION,                             // interface version
//! 		VID_PETROVITCH,                         // interface developer
//! 		&i_QB_vtbl,                             // internal(kernel called) function table
//! 		(sizeof(i_QB_vtbl)/sizeof(tPTR)),       // internal function table size
//! 		&e_QB_vtbl,                             // external function table
//! 		(sizeof(e_QB_vtbl)/sizeof(tPTR)),       // external function table size
//! 		QB_PropTable,                           // property table
//! 		mPROPERTY_TABLE_SIZE(QB_PropTable),     // property table size
//! 		sizeof(QBStorage)-sizeof(cObjImpl),     // memory size
//! 		IFACE_EXPLICIT_PARENT                   // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_QB,                                 // interface identifier
		PID_QB,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		QB_VERSION,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_QB_vtbl,                             // internal(kernel called) function table
		(sizeof(i_QB_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_QB_vtbl,                             // external function table
		(sizeof(e_QB_vtbl)/sizeof(tPTR)),       // external function table size
		QB_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(QB_PropTable),     // property table size
		sizeof(QBStorage)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"QB(IID_QB) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave QB::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call QB_Register( hROOT root ) { return QBStorage::Register(root); }
// AVP Prague stamp end




