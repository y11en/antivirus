// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  05 April 2005,  14:52 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- qbrestorer.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define QBRestorer_VERSION ((tVERSION)1)
// AVP Prague stamp end



#define IS if(PR_SUCC(error))
#define IO_BLOCK_SIZE 0x1000
#define MAX_NAME_LEN  0x20

#define _CRTIMP

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_posio.h>
#include <Prague/iface/i_io.h>
#include <ProductCore/iface/i_posio_sp.h>
#include <Prague/iface/i_reg.h>
#include <ProductCore/plugin/p_qb.h>
// AVP Prague stamp end



#include <string.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <Extract/plugin/p_windiskio.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_csect.h>
#include "../../Prague/Hash/MD5/plugin_hash_md5.h"
#include <Prague/iface/i_objptr.h>
#if defined (_WIN32)
#include "../../extract/disk/NTFSstream/plugin_ntfsstream.h"
#include "../../extract/disk/NTFSstream/stream_optr.h"
#elif defined (__unix__)
#include <unix/compatutils/compatutils.h>
#endif
#include <Prague/iface/e_ktrace.h>
#include <AV/iface/i_engine.h>
#include <Prague/pr_cpp.h>
#include <ProductCore/structs/s_qb.h>
#include <Prague/pr_time.h>


#define cNP_QB_SAVED_STREAM                    ((tSTRING)("cNP_QB_SAVED_STREAM")) //  --

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable QBRestorer : public cQBRestorer {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call RestoreObject( hPOSIO_SP p_hSecIO, const tWSTRING p_sRestoreFullName, tBOOL p_bReplaceExisting );
	tERROR pr_call StoreObject( hOBJECT p_hObject, hPOSIO_SP p_hSecIO );
	tERROR pr_call MakeObjectID( hOBJECT p_hObject, tOBJECT_ID* p_pObjectID );

// Data declaration
// AVP Prague stamp end


private:
	tERROR StoreKAV50WS_QBFile(hIO hInputIO, hPOSIO_SP p_hSecIO);
	tERROR StoreKAV50P_QBFile(hIO hInputIO, hPOSIO_SP p_hSecIO);
	tERROR _DeserializeWStr(hIO hInputIO, tQWORD* pqwPos, tWCHAR** ppwstr, tDWORD* pdwSize);

	
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(QBRestorer)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "QBRestorer". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR QBRestorer::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "QBRestorer::ObjectInit method" );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR QBRestorer::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "QBRestorer::ObjectPreClose method" );

	
	return error;
}
// AVP Prague stamp end



#define COPY_BUFFER_SIZE 0x4000
tERROR CopyIOContent(hIO dest_io,hIO src_io)
{
	tBYTE buf[COPY_BUFFER_SIZE];
	tQWORD written=0;
	tDWORD size=0;
	tQWORD qwSize=0;
	tERROR error=CALL_IO_GetSize((hIO)src_io, &qwSize,IO_SIZE_TYPE_EXPLICIT);

	while(PR_SUCC(error))
	{
		error=CALL_IO_SeekRead(src_io,&size,written,buf,COPY_BUFFER_SIZE);
		if(size==0)
			break;
		if(PR_SUCC(error))
		{
			error=CALL_IO_SeekWrite(dest_io,&size,written,buf,size);
			written+=size;
		}
		if(PR_SUCC(error))
			error = CALL_SYS_SendMsg(src_io,pmc_PROCESSING,pm_PROCESSING_YIELD,0,0,0);
	}

	if(written==qwSize)
		error=errOK;
	else if(PR_SUCC(error))
		error=errNOT_OK;

	if(PR_SUCC(error)) 
		error=CALL_IO_SetSize(dest_io,written);

	if(PR_SUCC(error)) 
		error=CALL_IO_Flush(dest_io);
	return error;
}


// AVP Prague stamp begin( External (user called) interface method implementation, RestoreObject )
// Extended method comment
//    Восстанавливает объект по исходному, либо указанному местоположению.
// Parameters are:
//   "p_hSecIO"           : Метод восстанавливает объект из хранилища базируясь на данных сохраненных в p_hSecIO.
//   "sRestoreFullName" : Путь, по которому нужно восстановить объект.
tERROR QBRestorer::RestoreObject( hPOSIO_SP p_hSecIO, const tWSTRING p_sRestoreFullName, tBOOL p_bReplaceExisting )
{
	tERROR error = errOK;
	hIO hRestoreIO = NULL;
	tDWORD dwAttributes;
	tDATETIME dt;

	PR_TRACE_A0( this, "Enter QBRestorer::RestoreObject method" );

	error = sysCreateObject((hOBJECT*)&hRestoreIO, IID_IO, PID_NATIVE_FIO);
	if (PR_SUCC(error))
	{
		tWSTRING sRestoreFullName = NULL;
		if (p_sRestoreFullName != NULL)
			sRestoreFullName = p_sRestoreFullName;
		else
		{
			tDWORD dwNameSize;
			error = p_hSecIO->GetStoredProperty(&dwNameSize, cNP_QB_FULLNAME, NULL, 0);
			IS error = heapAlloc((tPTR*)&sRestoreFullName, dwNameSize);
			IS error = p_hSecIO->GetStoredProperty(NULL, cNP_QB_FULLNAME, sRestoreFullName, dwNameSize);
		}
		IS error = hRestoreIO->set_pgOBJECT_FULL_NAME(sRestoreFullName, 0, cCP_UNICODE);
		IS error = hRestoreIO->set_pgOBJECT_OPEN_MODE((p_bReplaceExisting ? fOMODE_CREATE_ALWAYS : fOMODE_CREATE_IF_NOT_EXIST) | fOMODE_SHARE_DENY_WRITE | fOMODE_SHARE_DENY_DELETE);
		IS error = hRestoreIO->set_pgOBJECT_ACCESS_MODE(fACCESS_RW);
		IS error = hRestoreIO->sysCreateObjectDone();
		
		if (PR_SUCC(error))
		{
			// restore security
			tDWORD dwSize = 0;
			if (PR_SUCC(p_hSecIO->GetStoredProperty(&dwSize, cNP_QB_FILE_SECURITY, NULL, 0)) && dwSize > 0)
			{
				tPTR pSecurity;
				error = heapAlloc((tPTR*)&pSecurity, dwSize);
				if (PR_SUCC(error))
				{
					error = p_hSecIO->GetStoredProperty(&dwSize, cNP_QB_FILE_SECURITY, pSecurity, dwSize);
					if (PR_SUCC(error))
						error = hRestoreIO->propSet(NULL, plSECURITY_ATTRIBUTES, pSecurity, dwSize);
					heapFree(pSecurity);
				}
				PR_TRACE((this, prtIMPORTANT, "qb\trestore security %terr, size %u", error, dwSize));
			}

			// copy file data
			if (PR_SUCC(error))
				error=CopyIOContent( hRestoreIO, (hIO) p_hSecIO);

			// restore file attributes
			if (PR_SUCC(error))
			{
				tERROR error = errOK;
				IS if (PR_SUCC(p_hSecIO->GetStoredProperty(NULL, cNP_QB_FILE_ATTRIBUTES, &dwAttributes, sizeof(dwAttributes))))
					error = hRestoreIO->propSetDWord(pgOBJECT_ATTRIBUTES, dwAttributes);

				// restore file times
				IS if (PR_SUCC(p_hSecIO->GetStoredProperty(NULL, cNP_QB_FILE_CREATION_TIME, &dt, sizeof(dt))))
					error = hRestoreIO->propSet(NULL, pgOBJECT_CREATION_TIME, &dt, sizeof(dt));
				IS if (PR_SUCC(p_hSecIO->GetStoredProperty(NULL, cNP_QB_FILE_LAST_WRITE_TIME, &dt, sizeof(dt))))
					error = hRestoreIO->propSet(NULL, pgOBJECT_LAST_WRITE_TIME, &dt, sizeof(dt));
				
				// restore last access time, but actually has no effect...
				IS if (PR_SUCC(p_hSecIO->GetStoredProperty(NULL, cNP_QB_FILE_LAST_ACCESS_TIME, &dt, sizeof(dt))))
					error = hRestoreIO->propSet(NULL, pgOBJECT_LAST_ACCESS_TIME, &dt, sizeof(dt));				
#if defined (_WIN32)
				// restore streams
				if (PR_SUCC(error))
				{
					tUINT nStreamsCount = 0;

					do 
					{
						hIO hStream = NULL;
						char sStreamName[0x40];
						tPTR pStreamData = NULL;
						hObjPtr hStreams = NULL;
						tDWORD dwSize;
						pr_sprintf(sStreamName, sizeof(sStreamName)-1, "%s%dn", cNP_QB_SAVED_STREAM, nStreamsCount);

						error = p_hSecIO->GetStoredProperty(&dwSize, sStreamName, NULL, 0);
						if (PR_FAIL(error))
						{
							error = errOK;
							break;
						}
						error = sysCreateObject((hOBJECT*)&hStreams, IID_OBJPTR, PID_NTFSSTREAM);
						if (PR_SUCC(error))
						{
							error = hStreams->propSetStr(NULL, plBASE_FULL_NAME, sRestoreFullName, 0, cCP_UNICODE);
							IS error = hStreams->propSetBool(plCREATE_EMPTY, cTRUE);
							IS error = hStreams->sysCreateObjectDone();
							if (PR_SUCC(error))
							{
								error = p_hSecIO->GetStoredProperty(&dwSize, sStreamName, NULL, 0);
								if (PR_SUCC(error))
									error = heapAlloc(&pStreamData, dwSize);
								if (PR_SUCC(error))
								{
									error = p_hSecIO->GetStoredProperty(NULL, sStreamName, pStreamData, dwSize);
									if (PR_SUCC(error))
									{
										hSTRING _name = NULL;
										if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&_name, IID_STRING)))
										{
											error = _name->ImportFromBuff(NULL, pStreamData, dwSize, cCP_UNICODE, 0);
											if (PR_SUCC(error))
												error = hStreams->IOCreate(&hStream, _name, fACCESS_WRITE, fOMODE_CREATE_ALWAYS);
											_name->sysCloseObject();
										}
									}
									heapFree(pStreamData);
								}
								pr_sprintf(sStreamName, sizeof(sStreamName)-1, "%s%d", cNP_QB_SAVED_STREAM, nStreamsCount);
								if (PR_SUCC(error))
									error = p_hSecIO->GetStoredProperty(&dwSize, sStreamName, NULL, 0);
								if (PR_SUCC(error))
									error = heapAlloc(&pStreamData, dwSize);
								if (PR_SUCC(error))
								{
									error = p_hSecIO->GetStoredProperty(NULL, sStreamName, pStreamData, dwSize);
									if (PR_SUCC(error))
										error = hStream->SeekWrite(NULL, 0, pStreamData, dwSize);
									heapFree(pStreamData);
								}
								if (PR_SUCC(error))
									nStreamsCount++;
								if (hStream)
									hStream->sysCloseObject();
							}
							hStreams->sysCloseObject();
						}
					} while (PR_SUCC(error));
				}
#endif //_WIN32
			}

			if (PR_FAIL(error))
				hRestoreIO->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
		}
		hRestoreIO->sysCloseObject();

		if (sRestoreFullName && sRestoreFullName != p_sRestoreFullName)
			heapFree(sRestoreFullName);
	}

	PR_TRACE_A1( this, "Leave QBRestorer::RestoreObject method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



/*
tERROR iCalcWStrHash(hHASH p_hHash, tWSTRING p_sStr)
{
	tERROR error = errOK;
	if (p_sStr)
		error = p_hHash->Update((tBYTE*)p_sStr, (wcslen(p_sStr)+1) * sizeof(tWCHAR));
	return error;
}
*/

tERROR _StoreKAV50P_QBFile_GetVirusName(QBRestorer * pThis, hIO hInputIO, cStrObj& strVirusName);

tERROR QBRestorer::StoreKAV50P_QBFile(hIO hInputIO, hPOSIO_SP p_hSecIO)
{
	tERROR error = errOK;
	tDWORD dwRead;
	cQBObject qbObj;
	tWCHAR* pFileName = NULL;
	tBYTE*  pKey = NULL;
	tBOOL bObjectInfoExist = cFALSE;

	// local KAV50Personal definitions
	#define QIO_SIGN        'AQLK'
	#define QIO_CURVER      0x00000001
	#define QIO_HASHSIZE    0x1000
	
	struct QBIO_HDR
	{
		tDWORD sign;
		tDWORD version;
		tDWORD hdrsize;
		tDWORD originalsize;
		tDWORD status;
		tDWORD basestime;
		tDWORD type;
		tDWORD namesize;
	};
	
	QBIO_HDR hdr;
	IS error = p_hSecIO->GetStoredProperty(NULL, cNP_QB_ID, &qbObj.m_qwObjectId, sizeof(qbObj.m_qwObjectId));
	IS error = hInputIO->SeekRead(&dwRead, 0, &hdr, sizeof(hdr));
	IS if (dwRead != sizeof(hdr)) error = errOBJECT_INCOMPATIBLE;
	IS if (hdr.sign != QIO_SIGN) error = errOBJECT_INCOMPATIBLE;
	IS if (hdr.version != QIO_CURVER) error = errOBJECT_INCOMPATIBLE;

	IS error = heapAlloc((tPTR*)&pFileName, hdr.namesize);
	IS error = hInputIO->SeekRead(&dwRead, sizeof(hdr), pFileName, hdr.namesize);
	IS if (dwRead != hdr.namesize) error = errOBJECT_INCOMPATIBLE;
	IS qbObj.m_strObjectName = pFileName;
	IS error = heapAlloc((tPTR*)&pKey, QIO_HASHSIZE);
	IS error = hInputIO->SeekRead(&dwRead, sizeof(hdr)+hdr.namesize, pKey, QIO_HASHSIZE);
	qbObj.m_qwSize = hdr.originalsize;
	qbObj.m_tmTimeStamp = cDateTime::now_utc();

	IS
	{
		_StoreKAV50P_QBFile_GetVirusName(this, hInputIO, qbObj.m_strDetectName);
	}

	// ajust status
	IS
	{
		switch (hdr.status)
		{
			case 1: qbObj.m_nObjectStatus = OBJSTATUS_INFECTED; qbObj.m_nDetectStatus = DSTATUS_SURE; break;
			case 2: qbObj.m_nObjectStatus = OBJSTATUS_INFECTED; qbObj.m_nDetectStatus = DSTATUS_PARTIAL; break;
			case 3: qbObj.m_nObjectStatus = OBJSTATUS_SUSPICION; qbObj.m_nDetectStatus = DSTATUS_HEURISTIC; break;
			case 4: qbObj.m_nObjectStatus = OBJSTATUS_ADDEDBYUSER; break;
			case 5: qbObj.m_nObjectStatus = OBJSTATUS_FALSEALARM; break;
			case 6: qbObj.m_nObjectStatus = OBJSTATUS_DISINFECTED; break;
			case 7: qbObj.m_nObjectStatus = OBJSTATUS_OK; break;
			case 8: qbObj.m_nObjectStatus = OBJSTATUS_PASSWORD_PROTECTED; break;
		}

		if( !qbObj.m_strDetectName.empty() )
		{
			qbObj.m_nObjectStatus = OBJSTATUS_INFECTED;
			qbObj.m_nDetectStatus = DSTATUS_SURE;
		}
	}	
	
	// serialize cQBObject struct
	IS
	{
		tBYTE* pBuff;
		tDWORD dwSize;
		error = g_root->StreamSerialize(&qbObj, SERID_UNKNOWN, NULL, 0, &dwSize);
		if (PR_SUCC(error))
			error = heapAlloc((tPTR*)&pBuff, dwSize);
		if (PR_SUCC(error))
		{
			error = g_root->StreamSerialize(&qbObj, SERID_UNKNOWN, pBuff, dwSize, &dwSize);
			if (PR_SUCC(error))
				error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_INFO, pBuff, dwSize);
			heapFree(pBuff);
		}
	}
	
	// copy quarantined file body
	IS
	{
		tBYTE* pBuff;
		tDWORD dwWritten, i, dwPos = 0;
		error = heapAlloc((tPTR*)&pBuff, 0x1000);
		IS while (dwPos < hdr.originalsize)
		{
			error = hInputIO->SeekRead(&dwRead, dwPos+hdr.hdrsize, pBuff, min((tDWORD)0x1000, hdr.originalsize-dwPos)); 
			if (PR_FAIL(error) || dwRead==0) 
				break;
			// decrypt
			for (i=0; i<dwRead; i++)
				pBuff[i] ^= pKey[(dwPos+i) % QIO_HASHSIZE];
			error = p_hSecIO->SeekWrite(&dwWritten, dwPos, pBuff, dwRead);
			IS if (dwWritten != dwRead) error = errOBJECT_WRITE;
			if (PR_FAIL(error))
				break;
			dwPos += dwRead;
		}
	}

	if (pFileName)
		heapFree(pFileName);
	if (pKey)
		heapFree(pKey);
	return error;
}


tERROR QBRestorer::_DeserializeWStr(hIO hInputIO, tQWORD* pqwPos, tWCHAR** ppwstr, tDWORD* pdwSize)
{
	tERROR error = errOK;
	tDWORD dwLen, dwRead;
	tWCHAR* pwstr;
	*pdwSize = 0;
	IS { error = hInputIO->SeekRead(&dwRead, *pqwPos, &dwLen, 4); *pqwPos += dwRead; }
	IS if (dwLen > 100*1024) error = errOBJECT_INCOMPATIBLE;
	IS error = heapRealloc((tPTR*)ppwstr, *ppwstr, dwLen+2); pwstr = *ppwstr;
	IS pwstr[dwLen/sizeof(tWCHAR)]=0;
	if (dwLen)
	{
		IS { error = hInputIO->SeekRead(&dwRead, *pqwPos, pwstr, dwLen); *pqwPos += dwRead; }
	}
	*pdwSize = dwLen+2;
	return error;
}

tERROR QBRestorer::StoreKAV50WS_QBFile(hIO hInputIO, hPOSIO_SP p_hSecIO)
{
	tERROR error = errOK;
	tQWORD qwSize;
	tQWORD qwPos;
	tDWORD dwData;
	tDWORD dwFileSize;
	cQBObject qbObj;
	tWCHAR* pwstr = NULL;
	tDWORD dwRead;
	// props
	tDWORD dwRecordType;
	tDWORD dwRecordStatus;
	unsigned __int64 ftCreationTime;
	unsigned __int64 ftLastAccessTime;
	unsigned __int64 ftLastWriteTime;
	
	// local KAV50WS enum's definitions
	enum RecordType
	{
        BaseRecordType  = 0,
		FileType        = 1,
		EmailType       = 2,
		StatisticType   = 3
	};
		
	enum RecordStatus
	{
        StatusMask              =       0x00FF,
		Protected               =       0x8000,                         //! file protected
		Manually                =       0x4000,                         //! Помещен пользователем
		Container               =       0x1000,                         //! Файл контейнер
		Infected				=		1,                              //! Зараженный
		Warning                 =		2,                              //! непонятно что??
		Suspicious              =       3,                              //! Подозрительный
		ByUser                  =       4,								//! Помещен пользователем
		FalseAlarm              =       5,                              //! Ложная тревога (например, после проверки с новыми базами).
		Cured                   =       6,                              //! Вылеченный обьект
		ManuallyByUser  =       Manually | ByUser,
		ProtectByUser   =       ManuallyByUser | Protected				//! Помещен пользователем и защищен от удаления
	};	
	
	qbObj.m_qwSize = 0;
	IS error = p_hSecIO->GetStoredProperty(NULL, cNP_QB_ID, &qbObj.m_qwObjectId, sizeof(qbObj.m_qwObjectId));
	IS error = hInputIO->GetSize(&qwSize,IO_SIZE_TYPE_EXPLICIT);
	IS error = hInputIO->SeekRead(NULL, qwSize-4, &dwData, 4);
	IS if (0 != memcmp(&dwData, "REC", 4)) error = errOBJECT_INCOMPATIBLE;
	IS error = hInputIO->SeekRead(NULL, qwSize-8, &dwData, 4);
	IS if (dwData >= qwSize) error = errOBJECT_INCOMPATIBLE;
	IS qwPos = qwSize - dwData - 8;
	IS dwFileSize = (tDWORD)(qwPos - 5);

	// deserialize props
	IS error = hInputIO->SeekRead(&dwRead, qwPos, &dwRecordType, 4); qwPos += dwRead;
	IS if (dwRecordType != FileType && dwRecordType != EmailType) error = errOBJECT_INCOMPATIBLE;
	IS error = hInputIO->SeekRead(&dwRead, qwPos, &dwRecordStatus, 4); qwPos += dwRead;
	IS error = hInputIO->SeekRead(&dwRead, qwPos, &qbObj.m_nSendToKL, 4); qwPos += dwRead;
	IS error = hInputIO->SeekRead(&dwRead, qwPos, &qbObj.m_tmTimeStamp, 4); qwPos += dwRead;
	IS error = hInputIO->SeekRead(&dwRead, qwPos, &dwData, 4); qwPos += dwRead; // database time
	IS error = hInputIO->SeekRead(&dwRead, qwPos, &qbObj.m_qwSize, 4); qwPos += dwRead; 
	if (dwRecordType == FileType) // file type
	{
		IS error = hInputIO->SeekRead(&dwRead, qwPos, &ftCreationTime, 8); qwPos += dwRead;
		IS ftCreationTime = DATETIME_FILETIME_BASE + ftCreationTime*10; // FILETIME -> tDATETIME
		IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_CREATION_TIME, &ftCreationTime, sizeof(ftCreationTime));
		IS error = hInputIO->SeekRead(&dwRead, qwPos, &ftLastAccessTime, 8); qwPos += dwRead;
		IS ftLastAccessTime = DATETIME_FILETIME_BASE + ftLastAccessTime*10; // FILETIME -> tDATETIME
		IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_LAST_ACCESS_TIME, &ftLastAccessTime, sizeof(ftLastAccessTime));
		IS error = hInputIO->SeekRead(&dwRead, qwPos, &ftLastWriteTime, 8); qwPos += dwRead;
		IS ftLastWriteTime = DATETIME_FILETIME_BASE + ftLastWriteTime*10; // FILETIME -> tDATETIME
		IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_LAST_WRITE_TIME, &ftLastWriteTime, sizeof(ftLastWriteTime));
		IS error = hInputIO->SeekRead(&dwRead, qwPos, &dwData, 4); qwPos += dwRead; // attributes
		IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_ATTRIBUTES, &dwData, sizeof(dwData));
	}	
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // id - KAV50WS
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // filename = id
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // user_name
	IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_USER, pwstr, dwData);
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // computer_name
	IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_MACHINE, pwstr, dwData);
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // computer_domain
	IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_DOMAIN, pwstr, dwData);
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // virus_name
	IS qbObj.m_strDetectName = pwstr;
	IS error = _DeserializeWStr(hInputIO, &qwPos, &pwstr, &dwData); // original_path
	IS error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FULLNAME, pwstr, dwData);
	IS qbObj.m_strObjectName = pwstr;
	// -- virus_type
	if (pwstr)
		heapFree(pwstr);

	// ajust status
	IS {
		switch (dwRecordStatus & StatusMask)
		{
			case Infected:   
				qbObj.m_nObjectStatus = OBJSTATUS_INFECTED; 
				qbObj.m_nDetectStatus = DSTATUS_SURE; 
				qbObj.m_nDetectType = DETYPE_VIRWARE; 
				qbObj.m_nDetectDanger = DETDANGER_HIGH; 
				break;
			case Warning:    
				qbObj.m_nObjectStatus = OBJSTATUS_INFECTED; 
				qbObj.m_nDetectStatus = DSTATUS_PARTIAL; 
				qbObj.m_nDetectType = DETYPE_VIRWARE; 
				qbObj.m_nDetectDanger = DETDANGER_HIGH; 
				break;
			case Suspicious: 
				qbObj.m_nObjectStatus = OBJSTATUS_SUSPICION; 
				qbObj.m_nDetectStatus = DSTATUS_HEURISTIC; 
				qbObj.m_nDetectType = DETYPE_VIRWARE; 
				qbObj.m_nDetectDanger = DETDANGER_HIGH; 
				break;
			case ByUser:     qbObj.m_nObjectStatus = OBJSTATUS_ADDEDBYUSER; break;
			case FalseAlarm: qbObj.m_nObjectStatus = OBJSTATUS_FALSEALARM; break;
			case Cured:      qbObj.m_nObjectStatus = OBJSTATUS_DISINFECTED; break;
		}
		// !!! we dont care about Protected, Manually, Container flags here
	}	
	// serialize cQBObject struct
	IS {
		tBYTE* pBuff;
		tDWORD dwSize;
		error = g_root->StreamSerialize(&qbObj, SERID_UNKNOWN, NULL, 0, &dwSize);
		if (PR_SUCC(error))
			error = heapAlloc((tPTR*)&pBuff, dwSize);
		if (PR_SUCC(error))
		{
			error = g_root->StreamSerialize(&qbObj, SERID_UNKNOWN, pBuff, dwSize, &dwSize);
			if (PR_SUCC(error))
				error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_INFO, pBuff, dwSize);
			heapFree(pBuff);
		}
	}
	// copy quarantined file body
	IS {
		tBYTE* pBuff;
		const __int64 xor_key = LONG_LONG_CONST(0xAC5C0E69EC4845E2);
		const tBYTE* _xor_key = (const tBYTE*)&xor_key;
		tDWORD dwWritten, i, dwPos = 0;
		error = heapAlloc((tPTR*)&pBuff, 0x1000);
		IS while (dwPos < dwFileSize)
		{
			error = hInputIO->SeekRead(&dwRead, dwPos+5, pBuff, min((tDWORD)0x1000, dwFileSize-dwPos)); 
			if (PR_FAIL(error) || dwRead==0) 
				break;
			// decrypt
			for (i=0; i<dwRead; i++)
				pBuff[i] ^= _xor_key[(dwPos+i) % 8];
			error = p_hSecIO->SeekWrite(&dwWritten, dwPos, pBuff, dwRead);
			IS if (dwWritten != dwRead) error = errOBJECT_WRITE;
			if (PR_FAIL(error))
				break;
			dwPos += dwRead;
		}
	}

	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, StoreObject )
// Parameters are:
tERROR QBRestorer::StoreObject( hOBJECT p_hObject, hPOSIO_SP p_hSecIO )
{
	tERROR error = errOK;
	tDWORD   dwNameSize;
	tWSTRING sFullName;
	hIO      hInputIO = NULL;
	tBOOL    bNFIO = cFALSE;

	PR_TRACE_FUNC_FRAME( "QBRestorer::StoreObject method" );

	PR_TRACE_A0( this, "Enter QB::StoreObject method" );

	if (p_hObject == NULL)
		return errPARAMETER_INVALID;
	if (PR_FAIL(error = sysCheckObject(p_hObject, IID_IO)))
		return error;
	
	hInputIO = (hIO)p_hObject;

	tPID PluginId = hInputIO->propGetDWord(pgPLUGIN_ID);
	if (PID_WINDISKIO == PluginId)
		return errNOT_SUPPORTED;

	bNFIO = (PID_NATIVE_FIO == PluginId);

	if (bNFIO)
	{
		tDWORD dwMagic;
		// for NFIO - check previous QB format
		if (PR_SUCC(hInputIO->SeekRead(NULL, 0, &dwMagic, 4)))
		{
			if (dwMagic == 'FQLK')
			{
				error = StoreKAV50WS_QBFile(hInputIO, p_hSecIO);
				if (PR_SUCC(error))
					error = warnOBJECT_INFO_EXIST;
				return error;
			}
			if (dwMagic == 'AQLK')
			{
				error = StoreKAV50P_QBFile(hInputIO, p_hSecIO);
				if (PR_SUCC(error))
					error = warnOBJECT_INFO_EXIST;
				return error;
			}
		}
	}

	error = p_hObject->propGetStr(&dwNameSize, pgOBJECT_FULL_NAME, NULL, 0, cCP_UNICODE);
	if (PR_FAIL(error) || dwNameSize <= sizeof(tWCHAR))
		return errPARAMETER_INVALID;
	if (PR_FAIL(error = heapAlloc((tPTR*)&sFullName, dwNameSize)))
		return error;
	if (PR_FAIL(error = p_hObject->propGetStr(NULL, pgOBJECT_FULL_NAME, sFullName, dwNameSize, cCP_UNICODE)))
	{
		heapFree(sFullName);
		return error;
	}

	// store object properties
	error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FULLNAME, sFullName,  dwNameSize);

	if (PR_SUCC(error) && bNFIO)
	{
		// for NFIO - store file attributes and times
		tDWORD dwAttributes;
		tDATETIME dt;
		if (PR_SUCC(hInputIO->propGet(NULL, pgOBJECT_ATTRIBUTES, &dwAttributes, sizeof(dwAttributes))))
			error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_ATTRIBUTES, &dwAttributes, sizeof(dwAttributes));
		IS if (PR_SUCC(hInputIO->propGet(NULL, pgOBJECT_CREATION_TIME, &dt, sizeof(dt))))
			error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_CREATION_TIME, &dt, sizeof(dt));
		IS if (PR_SUCC(hInputIO->propGet(NULL, pgOBJECT_LAST_ACCESS_TIME, &dt, sizeof(dt))))
			error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_LAST_ACCESS_TIME, &dt, sizeof(dt));
		IS if (PR_SUCC(hInputIO->propGet(NULL, pgOBJECT_LAST_WRITE_TIME, &dt, sizeof(dt))))
			error = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_LAST_WRITE_TIME, &dt, sizeof(dt));

		{
			tERROR loc_err;
			tDWORD dwSize;
			loc_err = hInputIO->propGet(&dwSize, plSECURITY_ATTRIBUTES, NULL, 0);
			if (PR_FAIL(loc_err) || dwSize == 0)
			{
				PR_TRACE((this, prtERROR, "qb\tget security size failed, size %u, %terr", dwSize, loc_err));
			}
			else
			{
				tPTR pSecurity;
				loc_err = heapAlloc((tPTR*)&pSecurity, dwSize);
				PR_TRACE((this, prtNOTIFY, "qb\tsecurity size = %u, 0x%x (loc err 0x%x)", dwSize, error, loc_err));
				if (PR_SUCC(loc_err))
				{
					tDWORD newsize = 0;
					loc_err = hInputIO->propGet(&newsize, plSECURITY_ATTRIBUTES, pSecurity, dwSize);
					if (PR_FAIL(loc_err))
					{
						PR_TRACE((this, prtERROR, "qb\tget security failed, size %u, new size %d locerr 0x%x, winerr 0x%x",
							dwSize,
							newsize,
							loc_err,
							hInputIO->propGetDWord(pgNATIVE_ERR)));
					}
					else
					{
						loc_err = p_hSecIO->SetStoredProperty(NULL, cNP_QB_FILE_SECURITY, pSecurity, dwSize);
						PR_TRACE((this, prtIMPORTANT, "qb\tstore security, size %u, %terr", dwSize, loc_err));
					}
					heapFree(pSecurity);
				}
			}
		}

#if defined (_WIN32)
		if (PR_SUCC(error))
		{
			tUINT nStreamsCount = 0;
			hObjPtr hStreams = NULL;
			tERROR err_tmp = sysCreateObject((hOBJECT*)&hStreams, IID_OBJPTR, PID_NTFSSTREAM);
			if (PR_SUCC(err_tmp))
			{
				IS err_tmp = hStreams->propSetStr(NULL, plBASE_FULL_NAME, sFullName, dwNameSize, cCP_UNICODE);
				IS err_tmp = hStreams->propSetBool(plCREATE_EMPTY, cTRUE);
				IS err_tmp = hStreams->sysCreateObjectDone();
				if (PR_SUCC(err_tmp))
				{
					while (PR_SUCC(hStreams->Next()))
					{
						hIO hStream = NULL;
						tPTR pStreamData = NULL;
						char sStreamName[0x40];
						tDWORD dwSize = hStreams->get_pgOBJECT_SIZE();
						
						if (dwSize > 1024*1024) // > 1Mb
						{
							PR_TRACE((this, prtIMPORTANT, "qb\tStream of size %u>1Mb, skipped", dwSize));
							continue;
						}
						pr_sprintf(sStreamName, sizeof(sStreamName)-1, "%s%d", cNP_QB_SAVED_STREAM, nStreamsCount);

						err_tmp = hStreams->IOCreate(&hStream, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
						if (PR_SUCC(err_tmp))
						{
							if (PR_SUCC(err_tmp = heapAlloc(&pStreamData, dwSize)))
							{
								if (PR_SUCC(err_tmp = hStream->SeekRead(NULL, 0, pStreamData, dwSize)))
									err_tmp = p_hSecIO->SetStoredProperty(NULL, sStreamName, pStreamData, dwSize);
								heapFree(pStreamData);
							}
							hStream->sysCloseObject();
						}
						if (PR_SUCC(err_tmp))
							err_tmp = hStreams->propGetStr(&dwSize, pgOBJECT_NAME, NULL, 0, cCP_UNICODE);
						if (PR_SUCC(err_tmp))
						{
							if (PR_SUCC(err_tmp = heapAlloc(&pStreamData, dwSize)))
							{
								strcat(sStreamName, "n");
								err_tmp = hStreams->propGetStr(NULL, pgOBJECT_NAME, pStreamData, dwSize, cCP_UNICODE);
								if (PR_SUCC(err_tmp))
									err_tmp = p_hSecIO->SetStoredProperty(NULL, sStreamName, pStreamData, dwSize);
								heapFree(pStreamData);
							}
						}
						if (PR_SUCC(err_tmp))
							nStreamsCount++;
					}
				}
				hStreams->sysCloseObject();
			}
		}
#endif //_WIN32
	}
	heapFree(sFullName);

	if (PR_SUCC(error))
		error=CopyIOContent( (hIO)p_hSecIO, hInputIO );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, MakeObjectID )
// Parameters are:
tERROR QBRestorer::MakeObjectID( hOBJECT p_hObject, tOBJECT_ID* p_pObjectID )
{
	tERROR   error;
	hHASH    hHash;
	tDWORD   dwHash[4];
	tQWORD*  pqwHash = (tQWORD*)&dwHash;
	PR_TRACE_FUNC_FRAME( "QBRestorer::MakeObjectID method" );

	static tPROPID propid_virtual_name = 0;

	if(!propid_virtual_name) 
		error=CALL_Root_RegisterCustomPropId( g_root, &propid_virtual_name, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING );

	cStrObj sName;
	if(PR_FAIL(error = sName.assign(p_hObject, pgOBJECT_FULL_NAME)))
		return error;
	if(sName.length() == 0)
		return errPARAMETER_INVALID;

	if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5)))
	{
		sName.tolower();
		error = hHash->Update((tBYTE*)sName.data(), sName.length()*sizeof(tWCHAR));
		if (PR_SUCC(error) && propid_virtual_name && PR_SUCC(sName.assign(p_hObject, propid_virtual_name)))
		{
			sName.tolower();
			error = hHash->Update((tBYTE*)sName.data(), sName.length()*sizeof(tWCHAR));
		}
		/*
		if ((PID_NATIVE_FIO == p_hObject->propGetDWord(pgPLUGIN_ID)) && (IID_IO == p_hObject->propGetDWord(pgINTERFACE_ID)))
		{
			tDWORD dwAttributes;
			tDATETIME dt;
			hIO hInputIO = (hIO)p_hObject;
			// for NFIO - calc file attributes and times into hash
			IS error = hInputIO->propGet(NULL, pgOBJECT_ATTRIBUTES, &dwAttributes, sizeof(dwAttributes));
			IS error = hHash->Update((tBYTE*)&dwAttributes, sizeof(dwAttributes));
			IS error = hInputIO->propGet(NULL, pgOBJECT_CREATION_TIME, &dt, sizeof(dt));
			IS error = hHash->Update((tBYTE*)&dt, sizeof(dt));
			IS error = hInputIO->propGet(NULL, pgOBJECT_LAST_WRITE_TIME, &dt, sizeof(dt));
			IS error = hHash->Update((tBYTE*)&dt, sizeof(dt));
		}		
		*/
		IS if (IID_IO == p_hObject->propGetDWord(pgINTERFACE_ID))
		{
			hIO hInputIO = (hIO)p_hObject;
			tQWORD qwSize;
			tBYTE data[0x100];
			tDWORD i;
			// for IO - calc size and first 4Kb of content
			IS error = hInputIO->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
			IS error = hHash->Update((tBYTE*)&qwSize, sizeof(qwSize));
			for (i=0; i<0x4096 && error == errOK;)
			{
				tDWORD dwRead;
				IS error = hInputIO->SeekRead(&dwRead, i, data, sizeof(data));
				IS hHash->Update(data, dwRead);
				i += dwRead;
			}
		}		
		IS error = hHash->GetHash((tBYTE*)&dwHash, sizeof(dwHash));
		hHash->sysCloseObject();
	}
	
	if (PR_FAIL(error))
		return error;

	dwHash[0] ^= dwHash[2];
	dwHash[1] ^= dwHash[3];
	if (p_pObjectID)
		*p_pObjectID = *pqwHash;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "QBRestorer". Register function
tERROR QBRestorer::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(QBRestorer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, QBRestorer_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "QBRestorer", 11 )
mpPROPERTY_TABLE_END(QBRestorer_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(QBRestorer)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(QBRestorer)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(QBRestorer)
	mEXTERNAL_METHOD(QBRestorer, RestoreObject)
	mEXTERNAL_METHOD(QBRestorer, StoreObject)
	mEXTERNAL_METHOD(QBRestorer, MakeObjectID)
mEXTERNAL_TABLE_END(QBRestorer)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter QBRestorer::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_QBRESTORER,                         // interface identifier
		PID_QB,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		QBRestorer_VERSION,                     // interface version
		VID_PETROVITCH,                         // interface developer
		&i_QBRestorer_vtbl,                     // internal(kernel called) function table
		(sizeof(i_QBRestorer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_QBRestorer_vtbl,                     // external function table
		(sizeof(e_QBRestorer_vtbl)/sizeof(tPTR)),// external function table size
		QBRestorer_PropTable,                   // property table
		mPROPERTY_TABLE_SIZE(QBRestorer_PropTable),// property table size
		sizeof(QBRestorer)-sizeof(cObjImpl),    // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"QBRestorer(IID_QBRESTORER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave QBRestorer::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call QBRestorer_Register( hROOT root ) { return QBRestorer::Register(root); }
// AVP Prague stamp end

//##################################################################
//##################################################################

tERROR _StoreKAV50P_QBFile_GetVirusName(QBRestorer * pThis, hIO hInputIO, cStrObj& strVirusName)
{
	{
		cAutoObj<cObjPtr> pRepts; tERROR err = pThis->sysCreateObject((cObject**)&pRepts, IID_OBJPTR, PID_NATIVE_FIO);
		if( PR_SUCC(err) )
		{
			cStrObj(hInputIO, pgOBJECT_PATH).copy(pRepts, pgOBJECT_PATH);
			err = pRepts->sysCreateObjectDone();
		}
		
		if( PR_SUCC(err) )
		{
			pRepts->propSetStr(NULL, pgMASK, (tPTR)"*.rept");
			pRepts->Reset(cFALSE);
			if( PR_SUCC(err = pRepts->Next()) && pRepts->get_pgIS_FOLDER() )
				err = errNOT_FOUND;
		}			
		
		if( PR_SUCC(err) )
		{
			cStrObj strReptFile; strReptFile.assign(pRepts, pgOBJECT_FULL_NAME);
                        cIOObj pReptFile (*pThis, cAutoString(strReptFile), fACCESS_READ);
			if( !pReptFile )
                        	return pReptFile.last_error();

		}
	}


	cStrObj strQBName(hInputIO, pgOBJECT_NAME); // Имя файла QB
	
	// pReptFile - I/O отчета .rept

	// ................
	return errNOT_IMPLEMENTED;
}

//##################################################################
//##################################################################
