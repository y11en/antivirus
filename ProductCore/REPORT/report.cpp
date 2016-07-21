// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  18 August 2006,  14:21 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- PPP
// Sub project -- Not defined
// Purpose     -- Report Storage
// Author      -- Pavlushchik
// File Name   -- report.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Report_VERSION ((tVERSION)2)
// AVP Prague stamp end



#define MOVE_SIZE 0x200

#define _CRTIMP

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>
#include <ProductCore/plugin/p_report.h>
// AVP Prague stamp end



#include <Prague/pr_msg.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/plugin/p_win32_nfio.h>

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif
#include "memory.h"
#include <stdlib.h>

typedef struct tag_REPORT_HEADER {
	tDWORD dwMagic;                   // report signature (REPORT_MAGIC)
	tDWORD dwVersionMajor;            // report implementation version major
	tDWORD dwVersionMinor;            // report implementation version minor
	tDWORD dwSpecialRecordsCount;     // number of special (reserved) records
	tDWORD dwRecordsCount;            // total records count including special records
	tDWORD dwIndexSize;               // size of index in bytes
	tQWORD qwDataBegin;               // offset to end of index / start of data
	tQWORD qwDataEnd;                 // offset to data end, usually points to end of file
	tDATETIME dtLastModified;
	tDATETIME dtLastRecordAdd;
	tDATETIME dtLastRecordUpdate;
	tDATETIME dtLastRecordDelete;
	tQWORD qwNextRecordUniqueId;             // reserved
} tREPORT_HEADER;

typedef struct tag_RECORD_HEADER {
	tDWORD    dwDataSize;             // size of record data (without header) in bytes 
	tDATETIME dtTimestamp;            // timestamp record added or updated
} tRECORD_HEADER;

typedef struct tag_INDEX_ELEMENT {
	tQWORD    qwOffset;               // offset in file to record header
	tQWORD    qwUniqueId;             // unique id of record
} tINDEX_ELEMENT;

#define REPORT_MAGIC  '2DPR'
#define REPORT_SPECIAL_RECORDS 32
#define _REPORT_SPECIAL_RECORDS REPORT_SPECIAL_RECORDS //replacement for _REPORT_SPECIAL_RECORDS
#define REPORT_INVALID_RECORD_INDEX ((tDWORD) - (REPORT_SPECIAL_RECORDS))
#define REPORT_INDEX_OFFSET sizeof(tREPORT_HEADER)

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable Report : public cReport {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call PropSetIO( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropGetFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropSetFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropGetHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropSetHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropGetFooter( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropSetFooter( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropGetDeleteOnClose( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PropSetDeleteOnClose( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetLastRecordAdd( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetLastRecordUpdate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetLastRecordDelete( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetLastModified( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call GetRecordCount( tDWORD* p_pdwRecords );
	tERROR pr_call GetRecord( tDWORD* p_result, tDWORD p_dwIndex, tVOID* p_pBuffer, tDWORD p_dwSize, tDATETIME* p_pdtTimestamp );
	tERROR pr_call AddRecord( const tVOID* p_pBuffer, tDWORD p_dwSize, tDWORD* p_pdwRecordIndex );
	tERROR pr_call UpdateRecord( tDWORD p_dwIndex, const tVOID* p_pBuffer, tDWORD p_dwSize );
	tERROR pr_call DeleteRecord( tDWORD p_dwIndex );
	tERROR pr_call Compact();
	tERROR pr_call DeleteOldRecords( const tDATETIME* p_dtTime );
	tERROR pr_call GetRecordEx( tDWORD* p_result, tQWORD p_qwRecordId, tVOID* p_pBuffer, tDWORD p_dwSize, tDATETIME* p_pdtTimestamp, tDWORD* p_pdwIndex );
	tERROR pr_call AddRecordEx( const tVOID* p_pBuffer, tDWORD p_dwSize, tDWORD* p_pdwRecordIndex, tQWORD* p_pqwRecordId );
	tERROR pr_call UpdateRecordEx( tQWORD p_qwRecordId, const tVOID* p_pBuffer, tDWORD p_dwSize );
	tERROR pr_call DeleteRecordEx( tQWORD p_qwRecordId );

// Data declaration
	tDWORD    m_dwObjectAccessMode; // Object access mode
	cIO*      m_hReportIO;          // IO to storage for report
	tBOOL     m_bForceFlush;        // Force flush after each report modification
	tBOOL     m_bSynchronized;      // Use synchronization on report operations
	tPTR      m_pFullName;          // --
	tCODEPAGE m_NameCP;             // --
	tBOOL     m_bShareWrite;        // --
	tBOOL     m_bDelayedIOCreate;   // --
	hOBJECT   m_hTM;                // --
// AVP Prague stamp end



private:
	tREPORT_HEADER m_Header;
	tBOOL     m_bWriter;
	cMutex*   m_hSync;
	tDWORD    m_dwFullNameSize;
	tBOOL     m_bInsideDelete;
	tBOOL     m_bFileNotFound;

	tERROR MoveData2End(tDWORD p_dwMoveBytes);
	tERROR GetRecordOffset(tDWORD p_dwIndex, tQWORD* p_pqwRecordOffset, tQWORD* p_pqwRecordId = NULL);
	tERROR SetRecordOffset(tDWORD p_dwIndex, tQWORD  p_qwRecordOffset);
	tERROR SetRecordOffsetAndId(tDWORD p_dwIndex, tQWORD p_qwRecordOffset, tQWORD p_qwRecordId);
	tERROR MoveRecord(tQWORD p_qwRecordOffset, tQWORD p_qwNewRecordOffset, tRECORD_HEADER* p_pRecordHeader, tDWORD* p_pdwHint);
	tERROR FixRecordOffset(tQWORD p_qwRecordOffset, tQWORD p_qwNewRecordOffset, tDWORD* p_pdwHint);
	tERROR FindRecordByOffset(tQWORD p_qwRecordOffset, tDWORD p_dwStartIndex, tDWORD p_dwEndIndex, tDWORD* p_pdwIndex);
	tERROR FindRecordById( tQWORD p_qwRecordId, tDWORD* p_pdwIndex, tQWORD* p_pqwOffset);
	tERROR CreateIO();
	tERROR _Reinit();
	
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(Report)
};
// AVP Prague stamp end



// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Report". Static(shared) property table variables
// AVP Prague stamp end



static unsigned long CountCRC32(unsigned int size, unsigned char* ptr, unsigned long old_crc)
{
    old_crc = ~old_crc;
    while(size-- != 0)
    {
        unsigned long i;
        unsigned long c = *ptr++;
        for( i=0; i<8; i++)
        {
            if (((old_crc^c)&1)!=0)
                old_crc = (old_crc>>1) ^ 0xEDB88320;
            else
                old_crc = (old_crc>>1);
            c>>=1;
        }
    }
    return (~old_crc);
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR Report::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME("Report::ObjectInit method");

	m_dwObjectAccessMode = fACCESS_RW;
	m_bSynchronized = cTRUE;
	m_bForceFlush = cFALSE;
	m_bShareWrite = cFALSE;
	m_NameCP = cCP_UNICODE;
	m_bWriter = cFALSE;
	m_bInsideDelete = cFALSE;
	m_bDelayedIOCreate = cFALSE;

	return error;
}
// AVP Prague stamp end



tERROR Report::CreateIO()
{
	tERROR error = errOK;
	tDWORD dwOpenMode = fOMODE_OPEN_IF_EXIST | fOMODE_DONT_CACHE_POS;
	tDWORD dwBytes;

	if (m_bFileNotFound)
		return errOBJECT_NOT_FOUND;

//	if (m_bSynchronized)
//		m_hSync->Lock(cSYNC_INFINITE);

	if (m_hReportIO != NULL) // everyting already created
	{
//		if (m_bSynchronized)
//			m_hSync->Release();
		return errOK;
	}

	if (m_dwObjectAccessMode & fACCESS_WRITE)
	{
//		dwOpenMode |= fOMODE_SHARE_DENY_DELETE;
		if (!m_bShareWrite)
			dwOpenMode |= fOMODE_SHARE_DENY_WRITE;
		dwOpenMode |= fOMODE_CREATE_IF_NOT_EXIST;
	}

	error = sysCreateObject((hOBJECT*)&m_hReportIO, IID_IO, PID_NATIVE_FIO);
	if (PR_SUCC(error))
	{
		error = m_hReportIO->set_pgOBJECT_ACCESS_MODE(m_dwObjectAccessMode);
		if (PR_SUCC(error))
			error = m_hReportIO->set_pgOBJECT_FULL_NAME(m_pFullName, m_dwFullNameSize, m_NameCP);
		if (PR_SUCC(error))
			error = m_hReportIO->set_pgOBJECT_OPEN_MODE(dwOpenMode);
		if (PR_SUCC(error))
			error = m_hReportIO->propSetBool( plCONTROL_SIZE_ON_READ, cFALSE );
		if (PR_SUCC(error))
			error = m_hReportIO->sysCreateObjectDone();
		if (PR_FAIL(error))
		{
			m_hReportIO->sysCloseObject();
			m_hReportIO=NULL;
		}
	}

	if (PR_FAIL(error))
	{
		if (errOBJECT_NOT_FOUND == error)
			m_bFileNotFound = cTRUE;
		PR_TRACE((this, prtERROR, "Cannot create report IO"));
	}

	if (PR_SUCC(error))
	{
		m_hReportIO->SeekRead(&dwBytes, 0, &m_Header, sizeof(m_Header));
		if (dwBytes != sizeof(m_Header) || m_Header.dwMagic != REPORT_MAGIC || m_Header.dwVersionMajor != Report_VERSION)
		{
			if (m_dwObjectAccessMode & fACCESS_WRITE)
			{
				PR_TRACE((this, prtIMPORTANT, "File new or corrupted - reinit data"));
				if (PR_FAIL(error = _Reinit()))
				{
					PR_TRACE((this, prtERROR, "Reinit failed with %terr", error));
				}
				else
				{
					if (m_hTM)
						m_hTM->sysSendMsg(pmc_REPORT, pm_INTERNAL_NOTIFY_REPORT_CREATED, NULL, NULL, NULL);
				}
			}
			else
			{
				PR_TRACE((this, prtERROR, "report: header is corrupted"));
				error = errOBJECT_DATA_CORRUPTED;
			}
		}
	}

//	if (m_bSynchronized)
//		m_hSync->Release();
	return error;
}

tERROR Report::_Reinit()
{
	tERROR error = errOK;
	tDWORD dwBytes;
	
	memset(&m_Header, 0, sizeof(m_Header));
	m_Header.dwMagic = REPORT_MAGIC;
	m_Header.dwRecordsCount = 0;
	//m_Header.dwSpecialRecordsCount = REPORT_SPECIAL_RECORDS;
	m_Header.dwVersionMajor = Report_VERSION;
	m_Header.qwDataBegin = m_Header.qwDataEnd = sizeof(tREPORT_HEADER);
	m_Header.qwNextRecordUniqueId = REPORT_SPECIAL_RECORDS;
	if (PR_FAIL(error = m_hReportIO->SeekWrite(&dwBytes, 0, &m_Header, sizeof(m_Header))))
	{
		PR_TRACE((this, prtERROR, "SeekWrite failed with %terr", error));
		reterr errOBJECT_CANNOT_BE_INITIALIZED;
	}

	return m_hReportIO->SetSize(sizeof(tREPORT_HEADER));
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR Report::ObjectInitDone()
{
	tERROR error = errOK;
	tDWORD dwReportIOAccessMode;
	tDWORD dwBytes;
	PR_TRACE_FUNC_FRAME("Report::ObjectInitDone method");

	if ((m_dwObjectAccessMode & fACCESS_RW) == 0)
	{
		PR_TRACE((this, prtERROR, "OBJECT_ACCESS_MODE property value unexpected (R/W undefined)"));
		reterr errOBJECT_CANNOT_BE_INITIALIZED;
	}

	if (m_hReportIO == NULL && m_pFullName == NULL)
	{
		PR_TRACE((this, prtERROR, "ether plIO or pgOBJECT_FULL_NAME must be initialized"));
		reterr errOBJECT_CANNOT_BE_INITIALIZED;
	}

	if (!m_hReportIO)
	{
		if (m_dwObjectAccessMode & fACCESS_WRITE)
			m_bWriter = cTRUE;
	}
	else
	{
		dwReportIOAccessMode = m_hReportIO->get_pgOBJECT_ACCESS_MODE();
		if ((dwReportIOAccessMode & fACCESS_READ) == 0)
		{
			PR_TRACE((this, prtERROR, "IO must have read access"));
			reterr errOBJECT_CANNOT_BE_INITIALIZED;
		}

		if ((m_dwObjectAccessMode & fACCESS_WRITE) != 0)
		{
			if ((dwReportIOAccessMode & fACCESS_WRITE) == 0)
			{
				PR_TRACE((this, prtERROR, "IO have no write access"));
				reterr errOBJECT_CANNOT_BE_INITIALIZED;
			}
				
			if (!m_bShareWrite)
			{
				if ((m_hReportIO->get_pgOBJECT_OPEN_MODE() & fOMODE_SHARE_DENY_WRITE) == 0)
				{
					PR_TRACE((this, prtERROR, "IO must be opened with fOMODE_SHARE_DENY_WRITE"));
					reterr errOBJECT_CANNOT_BE_INITIALIZED;
				}
			}
			m_bWriter = cTRUE;
		}
	}

	if (m_bSynchronized)
	{
		tCHAR  sName[0x40];
		tDWORD dwCrc = 0;
		
		if (m_pFullName==NULL)
			reterr errOBJECT_CANNOT_BE_INITIALIZED;

		if (PR_FAIL(error = sysCreateObject((hOBJECT*)&m_hSync, IID_MUTEX)))
		{
			PR_TRACE((this, prtERROR, "Synchronization object creation failed with %terr", error));
			reterr errOBJECT_CANNOT_BE_INITIALIZED;
		}

		dwCrc = CountCRC32(m_dwFullNameSize, (tBYTE*)m_pFullName, 0);
		dwBytes = pr_sprintf((tCHAR*)sName, sizeof(sName), "sync_report_%08X", dwCrc);
		
		if (PR_FAIL(error = m_hSync->set_pgGLOBAL(cTRUE)))
		{
			PR_TRACE((this, prtERROR, "failed to set pgGLOBAL for synchronization object with %terr", error));
		}
		
		if (PR_FAIL(error = m_hSync->set_pgOBJECT_NAME(sName, dwBytes)))
		{
			PR_TRACE((this, prtERROR, "Synchronization object creation failed with %terr", error));
			reterr errOBJECT_CANNOT_BE_INITIALIZED;
		}
		
		if (PR_FAIL(error = m_hSync->sysCreateObjectDone()))
		{
			PR_TRACE((this, prtERROR, "Synchronization object creation(done) failed with %terr", error));
			reterr errOBJECT_CANNOT_BE_INITIALIZED;
		}
		
	}
	
	if (!m_bDelayedIOCreate)
	{
		if (PR_FAIL(error = CreateIO()))
			return error;
	}

	if (PR_SUCC(error))
	{
		if (m_hTM)
			sysRegisterMsgHandler(pmc_REPORT, rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		sysRegisterMsgHandler(pmc_REPORT, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR Report::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME("Report::ObjectPreClose method" );

	PR_TRACE((this, prtIMPORTANT, "Report close: %08X - %S", this, (m_pFullName?m_pFullName:L"")));
	
	if (m_pFullName)
		heapFree(m_pFullName);
	if (m_bSynchronized && m_hSync)
		m_hSync->sysCloseObject();
//	if (m_hReportIO)
//		m_hReportIO->Flush();

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
tERROR Report::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	tDWORD dwRecIndex;
	PR_TRACE_FUNC_FRAME( "Report::MsgReceive method" );

	if (p_msg_cls_id == pmc_REPORT)
	{
		switch(p_msg_id)
		{
		case pm_ADD_RECORD:
			if (p_send_point == m_hTM) // ignore messages not for us
				return errOK;
			if (p_par_buf_len == NULL)
				return errPARAMETER_INVALID;
			error = AddRecord(p_par_buf, *p_par_buf_len, &dwRecIndex);
			if (PR_SUCC(error))
			{
				if (*p_par_buf_len >= sizeof(tDWORD))
					*(tDWORD*)p_par_buf = dwRecIndex;
				*p_par_buf_len = sizeof(tDWORD);
				error = errOK_DECIDED;
			}
			break;
		case pm_INTERNAL_NOTIFY_REPORT_CREATED:
			m_bFileNotFound = cFALSE; // reset open cache
			break;
		default:
			break;
		}
	}
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropSetIO )
// Interface "Report". Method "Set" for property(s):
//  -- IO
tERROR Report::PropSetIO( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	tDWORD dwBytes;
	PR_TRACE_A0( this, "Enter *SET* method ObjectPreClose for property plIO" );

	*out_size = 0;
	if (size != sizeof(hIO) || buffer==NULL)
		error = errPARAMETER_INVALID;
	else
	{
		*out_size = size;
		m_hReportIO = *(hIO*)buffer;

		error = m_hReportIO->propGetStr(&dwBytes, pgOBJECT_FULL_NAME, NULL, 0, cCP_UNICODE);
		if (PR_SUCC(error))
			error = heapRealloc(&m_pFullName, m_pFullName, dwBytes);
		if (PR_SUCC(error))
			m_dwFullNameSize = dwBytes;
		if (PR_SUCC(error))
			error = m_hReportIO->propGetStr(NULL, pgOBJECT_FULL_NAME, m_pFullName, dwBytes, cCP_UNICODE);
		if (PR_SUCC(error))
			error = propInitialized(pgOBJECT_FULL_NAME, cTRUE);
		if (0 == m_dwObjectAccessMode)
		{
			m_dwObjectAccessMode = m_hReportIO->get_pgOBJECT_ACCESS_MODE();
			m_bShareWrite = (0 == (m_dwObjectAccessMode & fOMODE_SHARE_DENY_WRITE));
		}
		if (PR_FAIL(error))
		{
			if (m_pFullName)
				heapFree(m_pFullName);
			m_pFullName = NULL;
			m_dwFullNameSize = 0;
			PR_TRACE((this, prtERROR, "Cannot retrive IO name"));
		}
	}

	PR_TRACE_A2( this, "Leave *SET* method ObjectPreClose for property plIO, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropGetFullName )
// Interface "Report". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR Report::PropGetFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method PropGetFullName for property pgOBJECT_FULL_NAME" );

	*out_size = 0;
	if (m_hReportIO)
		error = m_hReportIO->propGetStr(out_size, prop, buffer, size, m_NameCP);
	else if (m_pFullName)
	{
		memcpy(buffer, m_pFullName, min(m_dwFullNameSize, size));
		if (size < m_dwFullNameSize)
			error = errBUFFER_TOO_SMALL;
		*out_size = m_dwFullNameSize;
	}

	PR_TRACE_A2( this, "Leave *GET* method PropGetFullName for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropSetFullName )
// Interface "Report". Method "Set" for property(s):
//  -- OBJECT_FULL_NAME
tERROR Report::PropSetFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectPreClose for property pgOBJECT_FULL_NAME" );

	*out_size = 0;
	if (size == 0 || buffer==NULL)
		error = errPARAMETER_INVALID;
	else
	{
		if (PR_SUCC(error = heapRealloc(&m_pFullName, m_pFullName, size)))
		{
			*out_size = size;
			memcpy(m_pFullName, buffer, size);
			m_dwFullNameSize = size;
			error = propInitialized(plIO, cTRUE);
		}
	}
	PR_TRACE_A2( this, "Leave *SET* method ObjectPreClose for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropGetHeader )
// Interface "Report". Method "Get" for property(s):
//  -- OPTIONAL_HEADER_DATA
tERROR Report::PropGetHeader( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectPreClose for property plOPTIONAL_HEADER_DATA" );

	*out_size = 0;
	
	error = GetRecord(out_size, cOPTIONAL_HEADER, buffer, size, NULL);
	
	PR_TRACE_A2( this, "Leave *GET* method ObjectPreClose for property plOPTIONAL_HEADER_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropSetHeader )
// Interface "Report". Method "Set" for property(s):
//  -- OPTIONAL_HEADER_DATA
tERROR Report::PropSetHeader( tDWORD* p_out_size, tPROPID p_prop, tCHAR* p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectPreClose for property plOPTIONAL_HEADER_DATA" );

	error = UpdateRecord(cOPTIONAL_HEADER, p_pBuffer, p_dwSize);

	PR_TRACE_A2( this, "Leave *SET* method ObjectPreClose for property plOPTIONAL_HEADER_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropGetFooter )
// Interface "Report". Method "Get" for property(s):
//  -- OPTIONAL_FOOTER_DATA
tERROR Report::PropGetFooter( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectPreClose for property plOPTIONAL_FOOTER_DATA" );

	error = GetRecord(out_size, cOPTIONAL_FOOTER, buffer, size, NULL);

	PR_TRACE_A2( this, "Leave *GET* method ObjectPreClose for property plOPTIONAL_FOOTER_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropSetFooter )
// Interface "Report". Method "Set" for property(s):
//  -- OPTIONAL_FOOTER_DATA
tERROR Report::PropSetFooter( tDWORD* p_out_size, tPROPID p_prop, tCHAR* p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectPreClose for property plOPTIONAL_FOOTER_DATA" );

	error = UpdateRecord(cOPTIONAL_FOOTER, p_pBuffer, p_dwSize);

	PR_TRACE_A2( this, "Leave *SET* method ObjectPreClose for property plOPTIONAL_FOOTER_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropGetDeleteOnClose )
// Interface "Report". Method "Get" for property(s):
//  -- OBJECT_DELETE_ON_CLOSE
tERROR Report::PropGetDeleteOnClose( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method PropGetDeleteOnClose for property plOBJECT_DELETE_ON_CLOSE" );

	*out_size = 0;
	if (m_hReportIO)
		error = m_hReportIO->propGet(out_size, prop, buffer, size);
	else
		error = errOBJECT_NOT_INITIALIZED;

	PR_TRACE_A2( this, "Leave *GET* method PropGetDeleteOnClose for property plOBJECT_DELETE_ON_CLOSE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropSetDeleteOnClose )
// Interface "Report". Method "Set" for property(s):
//  -- OBJECT_DELETE_ON_CLOSE
tERROR Report::PropSetDeleteOnClose( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method PropSetDeleteOnClose for property plOBJECT_DELETE_ON_CLOSE" );

	*out_size = 0;
	if (m_hReportIO)
		error = m_hReportIO->propSet(out_size, prop, buffer, size);
	else
		error = errOBJECT_NOT_INITIALIZED;

	PR_TRACE_A2( this, "Leave *SET* method PropSetDeleteOnClose for property plOBJECT_DELETE_ON_CLOSE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



#define _non_integrated_type_prop_get(_type, _val) \
	{                                            \
		*out_size = sizeof(_type);                 \
		if (NULL != buffer)                        \
		{                                          \
			if (sizeof(_type) < size)                \
				error = errBAD_SIZE;                   \
			else                                     \
				memcpy(buffer, &_val, sizeof(_type));               \
		}                                          \
	}

// AVP Prague stamp begin( Property method implementation, GetLastRecordAdd )
// Interface "Report". Method "Get" for property(s):
//  -- LAST_RECORD_ADD
tERROR Report::GetLastRecordAdd( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetLastRecordAdd for property plLAST_RECORD_ADD" );

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (PR_SUCC(error) && (!m_bWriter || m_bShareWrite))
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(tREPORT_HEADER));

	if (m_bSynchronized)
		m_hSync->Release();

	if (PR_SUCC(error))
		_non_integrated_type_prop_get(tDATETIME, m_Header.dtLastRecordAdd);

	PR_TRACE_A2( this, "Leave *GET* method GetLastRecordAdd for property plLAST_RECORD_ADD, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetLastRecordUpdate )
// Interface "Report". Method "Get" for property(s):
//  -- LAST_RECORD_UPDATE
tERROR Report::GetLastRecordUpdate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetLastRecordUpdate for property plLAST_RECORD_UPDATE" );

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (PR_SUCC(error) && (!m_bWriter || m_bShareWrite))
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(tREPORT_HEADER));

	if (m_bSynchronized)
		m_hSync->Release();

	if (PR_SUCC(error))
		_non_integrated_type_prop_get(tDATETIME, m_Header.dtLastRecordUpdate);

	PR_TRACE_A2( this, "Leave *GET* method GetLastRecordUpdate for property plLAST_RECORD_UPDATE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetLastRecordDelete )
// Interface "Report". Method "Get" for property(s):
//  -- LAST_RECORD_DELETE
tERROR Report::GetLastRecordDelete( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetLastRecordDelete for property plLAST_RECORD_DELETE" );

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (PR_SUCC(error) && (!m_bWriter || m_bShareWrite))
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(tREPORT_HEADER));

	if (m_bSynchronized)
		m_hSync->Release();

	if (PR_SUCC(error))
		_non_integrated_type_prop_get(tDATETIME, m_Header.dtLastRecordDelete);

	PR_TRACE_A2( this, "Leave *GET* method GetLastRecordDelete for property plLAST_RECORD_DELETE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetLastModified )
// Interface "Report". Method "Get" for property(s):
//  -- LAST_MODIFIED
tERROR Report::GetLastModified( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetLastModified for property plLAST_MODIFIED" );

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	if (PR_SUCC(error) && (!m_bWriter || m_bShareWrite))
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(tREPORT_HEADER));

	if (m_bSynchronized)
		m_hSync->Release();

	if (PR_SUCC(error))
		_non_integrated_type_prop_get(tDATETIME, m_Header.dtLastModified);

	PR_TRACE_A2( this, "Leave *GET* method GetLastModified for property plLAST_MODIFIED, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



#define STRUCT_MEMBER_OFFSET(_struct, _member) ( ((tBYTE*)&(_member)) - ((tBYTE*)&(_struct)) ) 

// AVP Prague stamp begin( External (user called) interface method implementation, GetRecordCount )
// Parameters are:
tERROR Report::GetRecordCount( tDWORD* p_result )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter Report::GetRecordCount method" );

	if (p_result == NULL)
		return errPARAMETER_INVALID;

	if (m_bWriter && !m_bShareWrite)
		*p_result = m_Header.dwRecordsCount;
	else
	{
		if (m_bSynchronized)
			m_hSync->Lock(cSYNC_INFINITE);

		if (!m_hReportIO)
		{
			if (PR_FAIL(error = CreateIO()))
			{
				if (m_bSynchronized)
					m_hSync->Release();
				return error;
			}
		}

		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));
		
		if (m_bSynchronized)
			m_hSync->Release();
	}

	*p_result = m_Header.dwRecordsCount;
	if ( *p_result != 0 )
	 *p_result -= _REPORT_SPECIAL_RECORDS;

	PR_TRACE_A1( this, "Leave Report::GetRecordCount method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



tERROR Report::GetRecordOffset(tDWORD p_dwIndex, tQWORD* p_pqwRecordOffset, tQWORD* p_pqwRecordId)
{
	tERROR error;
	tINDEX_ELEMENT Index;
	error = m_hReportIO->SeekRead(NULL, REPORT_INDEX_OFFSET + p_dwIndex*sizeof(tINDEX_ELEMENT), &Index, sizeof(tINDEX_ELEMENT));
	if (PR_SUCC(error))
	{
		if (p_pqwRecordOffset)
		{
			*p_pqwRecordOffset = Index.qwOffset;
			if (Index.qwOffset > m_Header.qwDataEnd || Index.qwOffset < m_Header.qwDataBegin)
			{
				if (Index.qwOffset == 0 && p_dwIndex < _REPORT_SPECIAL_RECORDS)
					;
				else
				{
					INT3;
					return errUNEXPECTED;
				}
			}
		}
		if (p_pqwRecordId)
			*p_pqwRecordId = Index.qwUniqueId;
	}
	return error;
}

tERROR Report::SetRecordOffset(tDWORD p_dwIndex, tQWORD p_qwRecordOffset)
{
	tERROR error;
	if (p_qwRecordOffset != 0)
	{
		if (p_qwRecordOffset > m_Header.qwDataEnd || p_qwRecordOffset < m_Header.qwDataBegin)
		{
			INT3;
			return errUNEXPECTED;
		}
	}
	error = m_hReportIO->SeekWrite(NULL, REPORT_INDEX_OFFSET + p_dwIndex*sizeof(tINDEX_ELEMENT), &p_qwRecordOffset, sizeof(p_qwRecordOffset));
	return error;
}

tERROR Report::SetRecordOffsetAndId(tDWORD p_dwIndex, tQWORD p_qwRecordOffset, tQWORD p_qwRecordId)
{
	tERROR error;
	tINDEX_ELEMENT Index;
	Index.qwOffset = p_qwRecordOffset;
	Index.qwUniqueId = p_qwRecordId;
	if (p_qwRecordOffset != 0)
	{
		if (p_qwRecordOffset > m_Header.qwDataEnd || p_qwRecordOffset < m_Header.qwDataBegin)
		{
			INT3;
			return errUNEXPECTED;
		}
	}
	error = m_hReportIO->SeekWrite(NULL, REPORT_INDEX_OFFSET + p_dwIndex*sizeof(tINDEX_ELEMENT), &Index, sizeof(Index));
	return error;
}


#pragma check_stack(off) 

tERROR Report::FindRecordByOffset(tQWORD p_qwRecordOffset, tDWORD p_dwStartIndex, tDWORD p_dwEndIndex, tDWORD* p_pdwIndex)
{
	tINDEX_ELEMENT Indexes[MOVE_SIZE];
	tERROR error = errOK;
	tDWORD i;
	tDWORD dwOffsetsRead;

	if (p_dwStartIndex > p_dwEndIndex)
	{
		// swap
		i = p_dwStartIndex;
		p_dwStartIndex = p_dwEndIndex;
		p_dwEndIndex = i;
	}
	

	while (PR_SUCC(error))
	{
		dwOffsetsRead = min(p_dwEndIndex - p_dwStartIndex, MOVE_SIZE);
		if (!dwOffsetsRead)
			return errNOT_FOUND;
		error = m_hReportIO->SeekRead(NULL, REPORT_INDEX_OFFSET + (p_dwStartIndex * sizeof(tINDEX_ELEMENT)),&Indexes, dwOffsetsRead*sizeof(tINDEX_ELEMENT));
		if (PR_FAIL(error))
			break;
		for (i=0; i<dwOffsetsRead; i++)
		{
			if (Indexes[i].qwOffset == p_qwRecordOffset)
			{
				*p_pdwIndex = p_dwStartIndex + i;
				return errOK;
			}
		}
		p_dwStartIndex += dwOffsetsRead;
	}

	return error;
}

tERROR Report::FixRecordOffset(tQWORD p_qwRecordOffset, tQWORD p_qwNewRecordOffset, tDWORD* p_pdwHint)
{
	tERROR error = errOK;
	tDWORD dwHint = *p_pdwHint;
	tDWORD dwIndex;
	tQWORD qwRecordOffset;

	if (dwHint >= m_Header.dwRecordsCount)
		dwHint = 0;

	dwIndex = dwHint;
	error = GetRecordOffset(dwIndex, &qwRecordOffset); // try hint index
	if (PR_FAIL(error))
	{
		INT3;
		return error;
	}

	if (qwRecordOffset != p_qwRecordOffset) 
	{
		// hint failed - perform full search
		error = FindRecordByOffset(p_qwRecordOffset, dwHint+1, m_Header.dwRecordsCount, &dwIndex);
		if (PR_FAIL(error))
			error = FindRecordByOffset(p_qwRecordOffset, 0, dwHint, &dwIndex);
		if (PR_FAIL(error))
		{
			INT3;
		}
	}
	
	if (PR_SUCC(error))
	{
		error = SetRecordOffset(dwIndex, p_qwNewRecordOffset);
		if (PR_FAIL(error))
		{
			INT3;
		}
		*p_pdwHint = dwIndex+1;
	}
	return error;
}


tERROR Report::MoveRecord(tQWORD p_qwRecordOffset, tQWORD p_qwNewRecordOffset, tRECORD_HEADER* p_pRecordHeader, tDWORD* p_pdwHint)
{
	tERROR error = errOK;
	tDWORD dwRecordSize;
	tRECORD_HEADER RecordHeader;
	tBYTE  sLocalBuffer[0x100];
	tBYTE* pBuffer = (tBYTE*)&sLocalBuffer;
	
	if (p_pRecordHeader == NULL)
	{
		error = m_hReportIO->SeekRead(NULL, p_qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
		if (PR_FAIL(error))
			return error;
		p_pRecordHeader = &RecordHeader;
	}

	dwRecordSize = sizeof(tRECORD_HEADER) + p_pRecordHeader->dwDataSize;

	if (dwRecordSize > sizeof(sLocalBuffer))
		error = heapAlloc((tPTR*)&pBuffer, dwRecordSize);
	if (PR_SUCC(error))
		error = m_hReportIO->SeekRead(NULL, p_qwRecordOffset, pBuffer, dwRecordSize);
	if (PR_SUCC(error))
		error = m_hReportIO->SeekWrite(NULL, p_qwNewRecordOffset,  pBuffer, dwRecordSize);
	if (PR_SUCC(error))
		m_Header.qwDataEnd = max(m_Header.qwDataEnd, p_qwNewRecordOffset + dwRecordSize);
	if (PR_SUCC(error))
		error = FixRecordOffset(p_qwRecordOffset, p_qwNewRecordOffset, p_pdwHint);
	if (PR_SUCC(error))
	{
		memset(pBuffer, 0, dwRecordSize);
		error = m_hReportIO->SeekWrite(NULL, p_qwRecordOffset, pBuffer, dwRecordSize);
	}
		
	if (pBuffer != (tBYTE*)&sLocalBuffer) // free if allocated 
		heapFree(pBuffer);

	return error;
}

tERROR Report::MoveData2End(tDWORD p_dwMoveBytes)
{
	tERROR error = errOK;
	tRECORD_HEADER RecordHeader; 
	tQWORD qwRecordOffset;
	tDWORD dwRecordSize;
	tDWORD dwBytesMoved = 0;
	tDATETIME dtTimestampZero;
	tDWORD dwHint = 0;
	
	if (m_Header.qwDataBegin == m_Header.qwDataEnd)
	{
		m_Header.dwIndexSize = p_dwMoveBytes;
		m_Header.qwDataBegin = m_Header.qwDataEnd = REPORT_INDEX_OFFSET + m_Header.dwIndexSize;
		return m_hReportIO->SetSize(m_Header.qwDataEnd);
	}

	memset(dtTimestampZero, 0, sizeof(dtTimestampZero));
	do
	{
		qwRecordOffset = m_Header.qwDataBegin;
		error = m_hReportIO->SeekRead(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));

		if (PR_SUCC(error))
		{
			dwRecordSize = sizeof(tRECORD_HEADER) + RecordHeader.dwDataSize;
			
			if (memcmp(RecordHeader.dtTimestamp, dtTimestampZero, sizeof(dtTimestampZero)) == 0)
				error = errOK; // record already deleted if timestamp == zero
			else
				error = MoveRecord(qwRecordOffset, m_Header.qwDataEnd, &RecordHeader, &dwHint);
			
			if (PR_SUCC(error))
			{
				m_Header.qwDataBegin += dwRecordSize;
				m_Header.dwIndexSize += dwRecordSize;
				dwBytesMoved += dwRecordSize;
			}
		}

	} while (PR_SUCC(error) && (dwBytesMoved < p_dwMoveBytes));

	tERROR error2 = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));
	if (PR_SUCC(error))	
		error = error2;

	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetRecord )
// Parameters are:
tERROR Report::GetRecord( tDWORD* p_result, tDWORD p_dwIndex, tVOID* p_pBuffer, tDWORD p_dwSize, tDATETIME* p_pdtTimestamp )
{
	tERROR error = errOK;
	tRECORD_HEADER RecordHeader;
	tQWORD qwRecordOffset;
	PR_TRACE_A0( this, "Enter Report::GetRecord method" );

	if (p_result)
		*p_result = 0;

	if (p_pBuffer==NULL && p_dwSize!=0)
		return errPARAMETER_INVALID;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	memset(&RecordHeader, 0, sizeof(RecordHeader));

	if (PR_SUCC(error) && (!m_bWriter || m_bShareWrite))
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(tREPORT_HEADER));

	p_dwIndex += _REPORT_SPECIAL_RECORDS;
	
	if (PR_SUCC(error) && (m_Header.dwRecordsCount <= p_dwIndex))
		error = errNOT_FOUND;
	
	if (PR_SUCC(error))
	{
		error = GetRecordOffset(p_dwIndex, &qwRecordOffset);

		if (PR_SUCC(error) && qwRecordOffset)
		{
			if (PR_SUCC(error))
				error = m_hReportIO->SeekRead(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));

			if (PR_SUCC(error) && p_dwSize)
				error = m_hReportIO->SeekRead(NULL, qwRecordOffset+sizeof(tRECORD_HEADER), p_pBuffer, min(p_dwSize, RecordHeader.dwDataSize));
		}
	}

	if (m_bSynchronized)
		m_hSync->Release();

	if (PR_SUCC(error) && p_pdtTimestamp)
		memcpy(p_pdtTimestamp, &RecordHeader.dtTimestamp, sizeof(tDATETIME));
		
	if (PR_SUCC(error) && (p_pBuffer != NULL) && (p_dwSize < RecordHeader.dwDataSize))
		error = errBUFFER_TOO_SMALL;

	if (p_result)
		*p_result = RecordHeader.dwDataSize;
	
	PR_TRACE_A1( this, "Leave Report::GetRecord method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddRecord )
// Parameters are:
tERROR Report::AddRecord( const tVOID* p_pBuffer, tDWORD p_dwSize, tDWORD* p_pdwRecordIndex )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter Report::AddRecord method" );

	error = AddRecordEx(p_pBuffer, p_dwSize, p_pdwRecordIndex, NULL);

	PR_TRACE_A1( this, "Leave Report::AddRecord method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateRecord )
// Parameters are:
tERROR Report::UpdateRecord( tDWORD p_dwIndex, const tVOID* p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	tRECORD_HEADER RecordHeader;
	tQWORD qwRecordOffset;
	PR_TRACE_A0( this, "Enter Report::UpdateRecord method" );

	if (!m_bWriter)
		return errACCESS_DENIED;
	
	if (p_pBuffer==NULL || p_dwSize==0)
		return errPARAMETER_INVALID;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}
	
	if (PR_SUCC(error) && m_bShareWrite)
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));

//	if (m_Header.dwSpecialRecordsCount == 0)
//		m_Header.dwSpecialRecordsCount = REPORT_SPECIAL_RECORDS;
	if (m_Header.dwRecordsCount < _REPORT_SPECIAL_RECORDS)
		m_Header.dwRecordsCount = _REPORT_SPECIAL_RECORDS;

	p_dwIndex += _REPORT_SPECIAL_RECORDS;

	if (m_Header.dwRecordsCount <= p_dwIndex)
		error = errNOT_FOUND;

	while ( PR_SUCC(error) && m_Header.dwRecordsCount >= m_Header.dwIndexSize / sizeof(tINDEX_ELEMENT) )
		error = MoveData2End(0x1000);
	
	if (PR_SUCC(error))
		error = GetRecordOffset(p_dwIndex, &qwRecordOffset);
	
	if (PR_FAIL(error) || qwRecordOffset == 0)
	{
		memset(&RecordHeader, 0, sizeof(RecordHeader));
	}
	else
	{
		error = m_hReportIO->SeekRead(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
	}	

	if (PR_SUCC(error))
		error = Now(&RecordHeader.dtTimestamp);

	if (PR_SUCC(error))
	{
		if (RecordHeader.dwDataSize == p_dwSize)
		{
			// update inplace
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
			
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwRecordOffset + sizeof(tRECORD_HEADER), (tPTR)p_pBuffer, p_dwSize);
		}
		else
		{
			// add & delete
			tQWORD qwNewRecordOffset;
			RecordHeader.dwDataSize = p_dwSize;
			qwNewRecordOffset = m_Header.qwDataEnd;
			
			// add new
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwNewRecordOffset, &RecordHeader, sizeof(RecordHeader));
			
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwNewRecordOffset + sizeof(tRECORD_HEADER), (tPTR)p_pBuffer, p_dwSize);
			
			if (PR_SUCC(error))
			{
				m_Header.qwDataEnd += sizeof(tRECORD_HEADER) + p_dwSize;
				error = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));
			}
			
			if (PR_SUCC(error))
				error = SetRecordOffset(p_dwIndex, qwNewRecordOffset);
			
			// delete old
			if (PR_SUCC(error) && qwRecordOffset!=0)
			{
				error = m_hReportIO->SeekRead(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
				memset(&RecordHeader.dtTimestamp, 0, sizeof(RecordHeader.dtTimestamp));
				if (PR_SUCC(error))
					error = m_hReportIO->SeekWrite(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
				if (PR_SUCC(error))
					error = Now(&RecordHeader.dtTimestamp);
			}
		}
	}
	
	if (PR_SUCC(error))
	{
		memcpy(&m_Header.dtLastModified, &RecordHeader.dtTimestamp, sizeof(tDATETIME));
		memcpy(&m_Header.dtLastRecordUpdate, &RecordHeader.dtTimestamp, sizeof(tDATETIME));
		error = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));
	}

	if (m_bForceFlush)
		m_hReportIO->Flush();
	if (m_bSynchronized)
		m_hSync->Release();

	PR_TRACE_A1( this, "Leave Report::UpdateRecord method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



#pragma check_stack(off) 
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteRecord )
// Parameters are:
tERROR Report::DeleteRecord( tDWORD p_dwIndex )
{
	tERROR error = errOK;
	tRECORD_HEADER RecordHeader;
	tQWORD qwRecordOffset;
	tDWORD dwIndexes2Move;
	tINDEX_ELEMENT Indexes[MOVE_SIZE];
	tINDEX_ELEMENT* pIndexes = &Indexes[0];

	PR_TRACE_A0( this, "Enter Report::DeleteRecord method" );

	if (!m_bWriter)
		return errACCESS_DENIED;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	p_dwIndex += _REPORT_SPECIAL_RECORDS;
	
	if (m_bInsideDelete)
	{
		if (m_bSynchronized)
			m_hSync->Release();
		return errACCESS_DENIED;
	}

	m_bInsideDelete = cTRUE;
	
	if (PR_SUCC(error) && m_bShareWrite)
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));

	if (PR_SUCC(error) && p_dwIndex >= m_Header.dwRecordsCount)
		error = errNOT_FOUND;
		
	if (PR_SUCC(error))
		error = GetRecordOffset(p_dwIndex, &qwRecordOffset);

	if (PR_SUCC(error) && qwRecordOffset == 0)
	{
		if (p_dwIndex >= _REPORT_SPECIAL_RECORDS)
			error = errOBJECT_DATA_CORRUPTED;
	}
	else
	{
		if (PR_SUCC(error))
			error = m_hReportIO->SeekRead(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
		
		// use zero timestamp as deleted mark
		memset(RecordHeader.dtTimestamp, 0, sizeof(RecordHeader.dtTimestamp));

		if (PR_SUCC(error))
			error = m_hReportIO->SeekWrite(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
	}

	if (p_dwIndex < _REPORT_SPECIAL_RECORDS)
	{
		tDWORD i;
		if (PR_SUCC(error))
		{
			tDWORD dwIndex, dwSize;
			dwIndex = p_dwIndex;
			dwSize = sizeof(dwIndex);
			sysSendMsg(pmc_REPORT, pm_BEFORE_DELETE_RECORD, NULL, &dwIndex, &dwSize);
		}
		if (PR_SUCC(error))
			error = SetRecordOffset(p_dwIndex, 0);
		for (i=0; i<_REPORT_SPECIAL_RECORDS; i++)
		{
			if (PR_FAIL(error = GetRecordOffset(i, &qwRecordOffset)))
				break;
			if (qwRecordOffset != 0)
				break;
		}
		
		if (PR_SUCC(error) && i==_REPORT_SPECIAL_RECORDS)
			m_Header.dwRecordsCount = 0;
	}
	else
	{
		dwIndexes2Move = m_Header.dwRecordsCount - p_dwIndex - 1;
		while (PR_SUCC(error) && dwIndexes2Move)
		{
			tDWORD dwBytes;
			tDWORD dwMove = min(dwIndexes2Move, MOVE_SIZE);
			tQWORD qwIndexOffset = REPORT_INDEX_OFFSET + (m_Header.dwRecordsCount - dwIndexes2Move - 1) * sizeof(tINDEX_ELEMENT);

			error = m_hReportIO->SeekRead(&dwBytes, qwIndexOffset+sizeof(tINDEX_ELEMENT), pIndexes, dwMove*sizeof(tINDEX_ELEMENT));
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwIndexOffset, pIndexes, dwBytes);
			dwIndexes2Move -= dwMove;
		}
		//if (pIndexes)
		//	heapFree(pIndexes);
		if (PR_SUCC(error))
			error = SetRecordOffset(m_Header.dwRecordsCount-1, 0);
		if (PR_SUCC(error))
		m_Header.dwRecordsCount--;
	}
	
	if (PR_SUCC(error))
	{
		tDATETIME dtNow;
		if (PR_SUCC(Now(&dtNow)))
		{
			memcpy(&m_Header.dtLastModified, &dtNow, sizeof(tDATETIME));
			memcpy(&m_Header.dtLastRecordDelete, &dtNow, sizeof(tDATETIME));
		}

		if (m_Header.dwRecordsCount <= _REPORT_SPECIAL_RECORDS)
			error = Compact();
		else
			error = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));
	}

	m_bInsideDelete = cFALSE;
	if (m_bForceFlush)
		m_hReportIO->Flush();
	if (m_bSynchronized)
		m_hSync->Release();
	
	PR_TRACE_A1( this, "Leave Report::DeleteRecord method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Compact )
// Behaviour comment
// Parameters are:
tERROR Report::Compact()
{
	tERROR error = errOK;
	tQWORD qwRecordOffset;
	tQWORD qwNewRecordOffset;
	tRECORD_HEADER RecordHeader;
	tDWORD dwRecordsMoved = 0;
	tDWORD dwRecordSize;
	tDATETIME dtTimestampZero;
	tDWORD dwHint = 0;


	PR_TRACE_A0( this, "Enter Report::Compact method" );

	if (!m_bWriter)
		return errACCESS_DENIED;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	if (PR_SUCC(error) && m_bShareWrite && m_bInsideDelete == cFALSE)
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));


	memset(dtTimestampZero, 0, sizeof(dtTimestampZero));
	qwRecordOffset = m_Header.qwDataBegin;

	/*if (m_Header.dwRecordsCount == 0)
	{
		m_Header.qwDataBegin = m_Header.qwDataEnd = sizeof(tREPORT_HEADER);
		m_Header.dwIndexSize = 0;
	}
	*/

	m_Header.dwIndexSize = m_Header.dwRecordsCount * sizeof(tINDEX_ELEMENT);
	qwNewRecordOffset = m_Header.qwDataBegin = sizeof(tREPORT_HEADER) + m_Header.dwIndexSize;
	while (PR_SUCC(error) && dwRecordsMoved < m_Header.dwRecordsCount && qwRecordOffset < m_Header.qwDataEnd)
	{
		tDWORD dwRead;
		error = m_hReportIO->SeekRead(&dwRead, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
		if (PR_SUCC(error) && dwRead != sizeof(RecordHeader))
			error = errOBJECT_DATA_CORRUPTED;
		
		if (PR_SUCC(error))
		{
			dwRecordSize = sizeof(tRECORD_HEADER) + RecordHeader.dwDataSize;
			// record deleted if timestamp == zero
			if (memcmp(RecordHeader.dtTimestamp, dtTimestampZero, sizeof(dtTimestampZero)) != 0)
			{
				if (qwRecordOffset + dwRecordSize > m_Header.qwDataEnd)
				{
					error = errOBJECT_DATA_CORRUPTED;
					break;
				}
				
				if (qwRecordOffset == qwNewRecordOffset)
				{
					// skip - record already in place
					qwNewRecordOffset += dwRecordSize;
					dwRecordsMoved++;
				}
				else
				{
					if ((tQWORD)dwRecordSize < qwRecordOffset - qwNewRecordOffset )
					{
						error = MoveRecord(qwRecordOffset, qwNewRecordOffset, &RecordHeader, &dwHint);
						if (PR_SUCC(error))
						{
							dwRecordsMoved++;
							qwNewRecordOffset += dwRecordSize;
						}
					}
					else
					{
						error = MoveRecord(qwRecordOffset, m_Header.qwDataEnd, &RecordHeader, &dwHint);
					}
				}
			}
			qwRecordOffset += dwRecordSize;
		}
	}

	tDATETIME dtNow;
	if (PR_SUCC(Now(&dtNow)))
	{
		memcpy(&m_Header.dtLastModified, &dtNow, sizeof(tDATETIME));
	}

	if (PR_SUCC(error))
	{
		m_Header.qwDataEnd = qwNewRecordOffset;
		if (m_Header.dwRecordsCount == _REPORT_SPECIAL_RECORDS && m_Header.qwDataBegin == m_Header.qwDataEnd)
		{
			// no data in report
			m_Header.dwIndexSize = m_Header.dwRecordsCount /*= m_Header.dwSpecialRecordsCount*/ = 0;
			m_Header.qwDataEnd = m_Header.qwDataBegin = sizeof(tREPORT_HEADER);
		}
	}

	if (PR_SUCC(error))
		error = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));

	if (PR_SUCC(error))
		m_hReportIO->SetSize(m_Header.qwDataEnd);

	if (m_bSynchronized)
		m_hSync->Release();
	
	PR_TRACE_A1( this, "Leave Report::Compact method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



#pragma check_stack(off) 
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteOldRecords )
// Extended method comment
//    //eng: Deletes records with timestamp below specified in dtTime
// Parameters are:
tERROR Report::DeleteOldRecords( const tDATETIME* p_pdtTime )
{
	tERROR error = errOK;
	tDWORD i;
	tQWORD qwRecordOffset;
	tINDEX_ELEMENT Indexes[MOVE_SIZE];
	tINDEX_ELEMENT* pIndexes = &Indexes[0];
	PR_TRACE_FUNC_FRAME( "Report::DeleteOldRecords method" );

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);
	
	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	if (!m_bWriter)
	{
		m_dwObjectAccessMode |= fACCESS_WRITE;
		if (PR_FAIL(error = m_hReportIO->propSetDWord(pgOBJECT_ACCESS_MODE, m_dwObjectAccessMode)) )
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
		m_bWriter = cTRUE;
	}

	if (m_bInsideDelete)
	{
		if (m_bSynchronized)
			m_hSync->Release();
		return errACCESS_DENIED;
	}

	if( !p_pdtTime )
	{
		error = _Reinit();
		if (m_bSynchronized)
			m_hSync->Release();
		return error;
	}
	
	if (PR_SUCC(error) && m_bShareWrite)
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));

	if (m_Header.dwRecordsCount < _REPORT_SPECIAL_RECORDS)
	{
		if (m_bSynchronized)
			m_hSync->Release();
		return errOK;
	}

	m_bInsideDelete = cTRUE;

	for (i=_REPORT_SPECIAL_RECORDS; i<m_Header.dwRecordsCount; i++)
	{
		if (PR_SUCC(error))
			error = GetRecordOffset(i, &qwRecordOffset);

		if (PR_SUCC(error) && qwRecordOffset == 0)
		{
			error = errOBJECT_DATA_CORRUPTED;
		}
		else
		{
			tRECORD_HEADER RecordHeader;

			if (PR_SUCC(error))
				error = m_hReportIO->SeekRead(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
			if (PR_FAIL(error) || DTCmp(&RecordHeader.dtTimestamp, p_pdtTime) >= 0) // no more records to delete
				break;
			if (PR_SUCC(error))
			{
				tDWORD dwIndex, dwSize;
				dwIndex = i;
				dwSize = sizeof(dwIndex);
				sysSendMsg(pmc_REPORT, pm_BEFORE_DELETE_RECORD, NULL, &dwIndex, &dwSize);
			}
			// use zero timestamp as deleted mark
			memset(RecordHeader.dtTimestamp, 0, sizeof(RecordHeader.dtTimestamp));
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwRecordOffset, &RecordHeader, sizeof(RecordHeader));
		}
	}

	tDWORD dwRecords2Delete = i - _REPORT_SPECIAL_RECORDS;

	if (dwRecords2Delete)
	{
		tQWORD qwIndexMoveDstOffset = REPORT_INDEX_OFFSET + _REPORT_SPECIAL_RECORDS * sizeof(tINDEX_ELEMENT);
		tQWORD qwIndexMoveSrcOffset = qwIndexMoveDstOffset + dwRecords2Delete * sizeof(tINDEX_ELEMENT);
		tDWORD dwIndexes2Move = m_Header.dwRecordsCount - _REPORT_SPECIAL_RECORDS - dwRecords2Delete;;
		while (PR_SUCC(error) && dwIndexes2Move)
		{
			tDWORD dwBytes;
			tDWORD dwMove = min(dwIndexes2Move, MOVE_SIZE);

			error = m_hReportIO->SeekRead(&dwBytes, qwIndexMoveSrcOffset, pIndexes, dwMove*sizeof(tINDEX_ELEMENT));
			if (PR_SUCC(error))
				error = m_hReportIO->SeekWrite(NULL, qwIndexMoveDstOffset, pIndexes, dwBytes);
			dwIndexes2Move -= dwMove;
			qwIndexMoveSrcOffset += dwMove*sizeof(tINDEX_ELEMENT);
			qwIndexMoveDstOffset += dwMove*sizeof(tINDEX_ELEMENT);
		}
		memset(&Indexes, 0, sizeof(Indexes));

		if (PR_SUCC(error))
			m_Header.dwRecordsCount -= dwRecords2Delete;

		if (PR_SUCC(error))
		{
			tDWORD dwIndexes2Clean = dwRecords2Delete;
			while (dwIndexes2Clean)
			{
				tDWORD dwClean = min(dwIndexes2Clean, MOVE_SIZE);
				m_hReportIO->SeekWrite(NULL, qwIndexMoveDstOffset, pIndexes, dwClean*sizeof(tINDEX_ELEMENT));
				dwIndexes2Clean -= dwClean;
				qwIndexMoveDstOffset += dwClean*sizeof(tINDEX_ELEMENT);
			}
		}
	}
		
	if (PR_SUCC(error))
	{
		tDATETIME dtNow;
		if (PR_SUCC(Now(&dtNow)))
		{
			memcpy(&m_Header.dtLastModified, &dtNow, sizeof(tDATETIME));
			memcpy(&m_Header.dtLastRecordDelete, &dtNow, sizeof(tDATETIME));
		}

		if ((m_Header.dwRecordsCount <= _REPORT_SPECIAL_RECORDS) || (m_Header.dwRecordsCount <= _REPORT_SPECIAL_RECORDS + 0x10 && dwRecords2Delete > 0x10))
			error = Compact();
		else
			error = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));
	}

	m_bInsideDelete = cFALSE;

	if (m_bSynchronized)
		m_hSync->Release();

	return error;
}
// AVP Prague stamp end



tERROR Report::FindRecordById( tQWORD p_qwRecordId, tDWORD* p_pdwIndex, tQWORD* p_pqwOffset)
{
	tERROR error = errUNEXPECTED;

	tDWORD dwIndex;
	tDWORD dwJump;
	tQWORD qwRecordId;

	dwIndex = min((tDWORD)p_qwRecordId, m_Header.dwRecordsCount-1);
	dwJump = dwIndex + 1;

	tINT dir = 1; // 1 - jump back, 0 - jump forward
	do 
	{
		error = GetRecordOffset(dwIndex, p_pqwOffset, &qwRecordId);
		if (PR_FAIL(error))
			return error;
		if (qwRecordId == p_qwRecordId)
		{
			error = errOK;
			break;
		}
		dwJump = min(abs((tINT)p_qwRecordId - (tINT)qwRecordId), (tINT)dwJump - 1);
		if (dwJump == 0)
		{
			error = errNOT_FOUND;
			break;
		}
		if (dir)
		{
			if (qwRecordId < p_qwRecordId)
			{
				error = errNOT_FOUND;
				break;
			}
			dwIndex -= dwJump;
		}
		else
		{
			dwIndex += dwJump;
		}
		dir = 1-dir;
	} while (1);
	
	if (PR_SUCC(error) && p_pdwIndex)
		*p_pdwIndex = dwIndex;

	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetRecordEx )
// Parameters are:
tERROR Report::GetRecordEx( tDWORD* p_result, tQWORD p_qwRecordId, tVOID* p_pBuffer, tDWORD p_dwSize, tDATETIME* p_pdtTimestamp, tDWORD* p_pdwIndex )
{
	tERROR error = errOK;
	tDWORD dwIndex;
	PR_TRACE_FUNC_FRAME( "Report::GetRecordEx method" );

	if (p_pBuffer==NULL && p_dwSize!=0)
		return errPARAMETER_INVALID;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	error = FindRecordById(p_qwRecordId, &dwIndex, NULL);
	if (PR_SUCC(error))
		error = GetRecord(p_result, dwIndex - _REPORT_SPECIAL_RECORDS, p_pBuffer, p_dwSize, p_pdtTimestamp);

	if (m_bSynchronized)
		m_hSync->Release();

	if (p_pdwIndex)
	{
		if (PR_SUCC(error))
			*p_pdwIndex = dwIndex - _REPORT_SPECIAL_RECORDS;
		else
			*p_pdwIndex = REPORT_INVALID_RECORD_INDEX;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddRecordEx )
// Parameters are:
tERROR Report::AddRecordEx( const tVOID* p_pBuffer, tDWORD p_dwSize, tDWORD* p_pdwRecordIndex, tQWORD* p_pqwRecordId )
{
	tERROR error = errOK;
	tRECORD_HEADER RecordHeader;
	PR_TRACE_FUNC_FRAME( "Report::AddRecordEx method" );

	if (p_pdwRecordIndex)
		*p_pdwRecordIndex = REPORT_INVALID_RECORD_INDEX;

	if (!m_bWriter)
		return errACCESS_DENIED;

	if (p_pBuffer==NULL || p_dwSize==0)
		return errPARAMETER_INVALID;
	
	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	if (PR_SUCC(error))
		error = Now(&RecordHeader.dtTimestamp);
	RecordHeader.dwDataSize = p_dwSize;


	if (PR_SUCC(error) && m_bShareWrite)
		error = m_hReportIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));

//	if (m_Header.dwSpecialRecordsCount == 0)
//		m_Header.dwSpecialRecordsCount = REPORT_SPECIAL_RECORDS;
	if (m_Header.dwRecordsCount < _REPORT_SPECIAL_RECORDS)
		m_Header.dwRecordsCount = _REPORT_SPECIAL_RECORDS;

	while ( PR_SUCC(error) && m_Header.dwRecordsCount >= m_Header.dwIndexSize / sizeof(tINDEX_ELEMENT) )
		error = MoveData2End(0x1000);

	if (PR_SUCC(error))
		error = m_hReportIO->SeekWrite(NULL, m_Header.qwDataEnd, &RecordHeader, sizeof(RecordHeader));

	if (PR_SUCC(error))
		error = m_hReportIO->SeekWrite(NULL, m_Header.qwDataEnd + sizeof(tRECORD_HEADER), (tPTR)p_pBuffer, p_dwSize);

	if (PR_SUCC(error))
	{
		if (p_pqwRecordId)
			*p_pqwRecordId = m_Header.qwNextRecordUniqueId;
		error = SetRecordOffsetAndId(m_Header.dwRecordsCount, m_Header.qwDataEnd, m_Header.qwNextRecordUniqueId);
		m_Header.qwNextRecordUniqueId++;
	}

	if (PR_SUCC(error))
	{
		memcpy(&m_Header.dtLastModified, &RecordHeader.dtTimestamp, sizeof(tDATETIME));
		memcpy(&m_Header.dtLastRecordAdd, &RecordHeader.dtTimestamp, sizeof(tDATETIME));
		if (p_pdwRecordIndex)
			*p_pdwRecordIndex = m_Header.dwRecordsCount - _REPORT_SPECIAL_RECORDS;
		m_Header.dwRecordsCount++;
		m_Header.qwDataEnd += sizeof(tRECORD_HEADER) + p_dwSize;
		error = m_hReportIO->SeekWrite(NULL, 0, &m_Header, sizeof(m_Header));
	}

	if (m_bForceFlush)
		m_hReportIO->Flush();
	if (m_bSynchronized)
		m_hSync->Release();
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateRecordEx )
// Parameters are:
tERROR Report::UpdateRecordEx( tQWORD p_qwRecordId, const tVOID* p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	tDWORD dwIndex;
	PR_TRACE_FUNC_FRAME( "Report::UpdateRecordEx method" );

	if (!m_bWriter)
		return errACCESS_DENIED;
	
	if (p_pBuffer==NULL || p_dwSize==0)
		return errPARAMETER_INVALID;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	error = FindRecordById(p_qwRecordId, &dwIndex, NULL);
	if (PR_SUCC(error))
		error = UpdateRecord(dwIndex - _REPORT_SPECIAL_RECORDS, p_pBuffer, p_dwSize);

	if (m_bSynchronized)
		m_hSync->Release();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteRecordEx )
// Parameters are:
tERROR Report::DeleteRecordEx( tQWORD p_qwRecordId )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD dwIndex;
	PR_TRACE_FUNC_FRAME( "Report::DeleteRecordEx method" );

	if (!m_bWriter)
		return errACCESS_DENIED;

	if (m_bSynchronized)
		m_hSync->Lock(cSYNC_INFINITE);

	if (!m_hReportIO)
	{
		if (PR_FAIL(error = CreateIO()))
		{
			if (m_bSynchronized)
				m_hSync->Release();
			return error;
		}
	}

	error = FindRecordById(p_qwRecordId, &dwIndex, NULL);
	if (PR_SUCC(error))
		error = DeleteRecord(dwIndex - _REPORT_SPECIAL_RECORDS);

	if (m_bSynchronized)
		m_hSync->Release();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Report". Register function
tERROR Report::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Report_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Report_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Report", 7 )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, Report, m_dwObjectAccessMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY( plIO, Report, m_hReportIO, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED, NULL, FN_CUST(PropSetIO) )
	mpLOCAL_PROPERTY_BUF( plFORCE_FLUSH, Report, m_bForceFlush, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plSYNCHRONIZED, Report, m_bSynchronized, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY( pgOBJECT_FULL_NAME, Report, m_pFullName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED, FN_CUST(PropGetFullName), FN_CUST(PropSetFullName) )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME_CP, Report, m_NameCP, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_FN( plOPTIONAL_HEADER_DATA, FN_CUST(PropGetHeader), FN_CUST(PropSetHeader) )
	mpLOCAL_PROPERTY_FN( plOPTIONAL_FOOTER_DATA, FN_CUST(PropGetFooter), FN_CUST(PropSetFooter) )
	mpLOCAL_PROPERTY_FN( plOBJECT_DELETE_ON_CLOSE, FN_CUST(PropGetDeleteOnClose), FN_CUST(PropSetDeleteOnClose) )
	mpLOCAL_PROPERTY_BUF( plSHARE_WRITE, Report, m_bShareWrite, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_FN( plLAST_RECORD_ADD, FN_CUST(GetLastRecordAdd), NULL )
	mpLOCAL_PROPERTY_FN( plLAST_RECORD_UPDATE, FN_CUST(GetLastRecordUpdate), NULL )
	mpLOCAL_PROPERTY_FN( plLAST_RECORD_DELETE, FN_CUST(GetLastRecordDelete), NULL )
	mpLOCAL_PROPERTY_FN( plLAST_MODIFIED, FN_CUST(GetLastModified), NULL )
	mpLOCAL_PROPERTY_BUF( plDELAYED_IO_CREATE, Report, m_bDelayedIOCreate, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plTASK_MANAGER, Report, m_hTM, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(Report_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Report)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Report)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN_EX(ReportReport, Report)
	mEXTERNAL_METHOD(Report, GetRecordCount)
	mEXTERNAL_METHOD(Report, GetRecord)
	mEXTERNAL_METHOD(Report, AddRecord)
	mEXTERNAL_METHOD(Report, UpdateRecord)
	mEXTERNAL_METHOD(Report, DeleteRecord)
	mEXTERNAL_METHOD(Report, Compact)
	mEXTERNAL_METHOD(Report, DeleteOldRecords)
	mEXTERNAL_METHOD(ReportReport, GetRecordEx)
	mEXTERNAL_METHOD(ReportReport, AddRecordEx)
	mEXTERNAL_METHOD(ReportReport, UpdateRecordEx)
	mEXTERNAL_METHOD(ReportReport, DeleteRecordEx)
mEXTERNAL_TABLE_END(Report)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter Report::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_REPORT,                             // interface identifier
		PID_REPORT,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Report_VERSION,                         // interface version
		VID_MIKE,                               // interface developer
		&i_Report_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Report_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Report_vtbl,                         // external function table
		(sizeof(e_Report_vtbl)/sizeof(tPTR)),   // external function table size
		Report_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Report_PropTable), // property table size
		sizeof(Report)-sizeof(cObjImpl),        // memory size
		0/*IFACE_EXPLICIT_PARENT*/              // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Report(IID_REPORT) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave Report::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Report_Register( hROOT root ) { return Report::Register(root); }
// AVP Prague stamp end



