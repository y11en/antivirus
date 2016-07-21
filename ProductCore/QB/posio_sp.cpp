// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  01 March 2005,  16:01 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- posio_sp.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define PosIO_SP_VERSION ((tVERSION)1)
// AVP Prague stamp end



#define IS if(PR_SUCC(error))

#define MAGIC_KLQB 'BQLK'
#define IO_BLOCK_SIZE 0x1000

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_posio.h>
#include <Prague/iface/i_reg.h>
#include <ProductCore/plugin/p_qb.h>
// AVP Prague stamp end



#include <Prague/plugin/p_win32_nfio.h>
#include "../../Prague/TempFile2/plugin_temporaryio2.h"
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_seqio.h>
#include "string.h"
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

typedef struct tag_tFILE_HEADER
{
	tDWORD dwMagic;
	tDWORD dwVersion;
	tDWORD dwHeaderSize;
	tQWORD qwPropOffset;
	tDWORD dwPropsCount;
	tQWORD qwPropSize;
	tQWORD qwDataOffset;
	tQWORD qwDataSize;
	tDWORD dwHashAlg;
} tFILE_HEADER;

typedef struct tag_tNPROP_DATA {
	tDWORD dwNameLen;
	tDWORD dwDataLen;
} tNPROP_DATA;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable PosIO_SP : public cPosIO_SP {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call Get( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call Set( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations

// Standard IO methods
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Additional PosIO methods
	tERROR pr_call Seek( tQWORD* result, tLONGLONG p_offset, tDWORD p_origin );
	tERROR pr_call Read( tDWORD* result, tPTR p_buffer, tDWORD p_count );
	tERROR pr_call Write( tDWORD* result, tPTR p_buffer, tDWORD p_count );

//  Additional PosIO_PS methods
	tERROR pr_call GetStoredProperty( tDWORD* result, const tSTRING p_sPropName, tPTR p_pBuffer, tDWORD p_dwSize );
	tERROR pr_call SetStoredProperty( tDWORD* result, const tSTRING p_sPropName, const tPTR p_pBuffer, tDWORD p_dwSize );
	tERROR pr_call DeleteStoredProperty( const tSTRING p_sPropName );
	tERROR pr_call GetStoredPropertyName( tDWORD* result, tDWORD p_dwIndex, tSTRING p_pBuffer, tDWORD p_dwSize );

// Data declaration
	tORIG_ID m_OrigID;      // --
	hOBJECT  m_hBasedOn;    // --
	tDWORD   m_dwHashAlg;   // Алгоритм хэширования (cHASH_ALGORITHM_xxx)
	tBYTE*   m_pHashKey;    // Ключ хеширования
	tDWORD   m_dwHashKeySize; // --
	hIO      m_hHashedIO;   // --
// AVP Prague stamp end



	tFILE_HEADER m_Header;
	hPOSIO  m_hTempIo;
	tQWORD  m_qwCurPos;
	tBOOL   m_bWriteMode;
	tBOOL   m_bHashAlg1Byte;
	tBOOL   m_bModified;
	tBOOL   m_bWasEverModified;
	hLIST   m_hPropList;
	hIO     m_hMyHashedIO;   // --

	tERROR  AjustWriteMode();
	tERROR  SerializeAll();
	
	tERROR  DeserializeHeader();
	tERROR  DeserializeNamedProp(tQWORD* p_pqwOffset);
	tERROR  DeserializeNamedProps();
	tERROR  DeserializeTempIO();

	tERROR  FindStoredProperty(tLISTNODE* p_pNode, const tSTRING p_sPropName, tDWORD p_dwIndex, tPTR* p_pData, tDWORD* p_dwDataSize);

	tERROR  EncryptBuffer(tQWORD p_qwOffset, tBYTE* p_pBuffer, tDWORD p_dwSize, tPTR p_pCryptContext);
	tERROR  DecryptBuffer(tQWORD p_qwOffset, tBYTE* p_pBuffer, tDWORD p_dwSize, tPTR p_pCryptContext);

	tERROR  XorBuffer(tQWORD p_qwOffset, tBYTE* p_pBuffer, tDWORD p_dwSize, tPTR p_pCryptContext);
#define EncryptBufferXor	XorBuffer
#define DecryptBufferXor	XorBuffer

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(PosIO_SP)
};
// AVP Prague stamp end



tERROR  PosIO_SP::XorBuffer(tQWORD p_qwOffset, tBYTE* p_pBuffer, tDWORD p_dwSize, tPTR p_pCryptContext)
{
	tDWORD i;
	tDWORD p;
	if (m_dwHashKeySize == 0)
		return errUNEXPECTED;
	p = (tDWORD)(p_qwOffset % m_dwHashKeySize);
	for (i=0; i<p_dwSize; i++, p++)
	{
		if (p>=m_dwHashKeySize)
			p = 0;
		p_pBuffer[i] ^= m_pHashKey[p];
	}
	return errOK;
}

tERROR  PosIO_SP::EncryptBuffer(tQWORD p_qwOffset, tBYTE* p_pBuffer, tDWORD p_dwSize, tPTR p_pCryptContext)
{
	if (m_dwHashKeySize == 0)
		return errOK;
	switch(m_Header.dwHashAlg)
	{
	case cHASH_ALGORITHM_NONE:
		return errOK;
	case cHASH_ALGORITHM_XOR:
		return EncryptBufferXor(p_qwOffset, p_pBuffer, p_dwSize, p_pCryptContext);
	default:
		return errMETHOD_NOT_FOUND;
	}
}

tERROR  PosIO_SP::DecryptBuffer(tQWORD p_qwOffset, tBYTE* p_pBuffer, tDWORD p_dwSize, tPTR p_pCryptContext)
{
	if (m_dwHashKeySize == 0)
		return errOK;
	switch(m_Header.dwHashAlg)
	{
	case cHASH_ALGORITHM_NONE:
		return errOK;
	case cHASH_ALGORITHM_XOR:
		return DecryptBufferXor(p_qwOffset, p_pBuffer, p_dwSize, p_pCryptContext);
	default:
		return errMETHOD_NOT_FOUND;
	}
}

tERROR  PosIO_SP::SerializeAll()
{
	tERROR error = errOK;
	tDWORD dwPropsCount = 0;
	tDWORD dwCookie = 0;
	hROOT  hRoot = sysGetRoot();
	tBYTE* pData = NULL; 
	tDWORD dwDataSize = 0;
	tQWORD qwPos;
	tDWORD dwBytesWritten;


	if (m_bModified == cFALSE)
		return errOK;

	// serialize header

	if (PR_SUCC(error))
	{
		error = m_hHashedIO->SeekWrite(&dwBytesWritten, 0, &m_Header, sizeof(m_Header));
		qwPos = dwBytesWritten;
	}

	// serialize file data

	if (m_hTempIo != NULL)
	{
		if (PR_SUCC(error) && (m_hPropList==NULL && m_Header.dwPropsCount))
			error = DeserializeNamedProps();
		if (PR_SUCC(error))
			error = heapRealloc((tPTR*)&pData, pData, IO_BLOCK_SIZE);
		if (PR_SUCC(error))
			dwDataSize = IO_BLOCK_SIZE;
		if (PR_SUCC(error))
		{
			tQWORD qwTempOffset = 0;
			tDWORD dwBytes;
			m_Header.qwDataOffset = qwPos;
			do 
			{
				if (PR_FAIL(error = m_hTempIo->SeekRead(&dwBytes, qwTempOffset, pData, IO_BLOCK_SIZE)))
					break;
				if (PR_FAIL(error = EncryptBuffer(qwTempOffset, (tBYTE*)pData, dwBytes, NULL)))
					break;
				if (PR_FAIL(error = m_hTempIo->SeekWrite(&dwBytesWritten, qwPos, pData, dwBytes)))
					break;
				if (dwBytes != dwBytesWritten)
				{
					error = errOBJECT_WRITE;
					break;
				}
				qwTempOffset += dwBytes;
				qwPos += dwBytes;
			} while (dwBytes);
			m_Header.qwDataSize = qwPos - m_Header.qwDataOffset;
		}
	}
	else // file has not been modified or direct write used
	{
		qwPos = m_Header.qwDataOffset + m_Header.qwDataSize;
	}
		
	// serialize props

	m_Header.qwPropOffset = qwPos;
	m_Header.qwPropSize = 0;

	if (m_Header.dwPropsCount)
	{
		if (m_hPropList==NULL)
			error = errUNEXPECTED;
		if (PR_SUCC(error))
		{
			tLISTNODE node;
			if (m_hPropList && PR_SUCC(m_hPropList->First(&node)))
			{
				tDWORD dwNodeSize;
				do 
				{
					error = m_hPropList->DataGet(&dwNodeSize, node, NULL, 0);
					IS if (dwDataSize < dwNodeSize)
					{
						error = heapRealloc((tPTR*)&pData, pData, dwNodeSize);
						IS dwDataSize = dwNodeSize;
					}
					IS error = m_hPropList->DataGet(NULL, node, pData, dwDataSize);
					IS error = EncryptBuffer(0, pData, dwNodeSize, NULL);
					IS error = m_hHashedIO->SeekWrite(NULL, qwPos, &dwNodeSize, sizeof(dwNodeSize));
					IS error = m_hHashedIO->SeekWrite(&dwBytesWritten, qwPos + sizeof(dwNodeSize), pData, dwNodeSize);
					IS qwPos += dwBytesWritten + sizeof(dwNodeSize);
					IS dwPropsCount++;

				} while (PR_SUCC(m_hPropList->Next(&node, node)));
			}
			m_Header.qwPropSize = qwPos - m_Header.qwPropOffset;
		}
	}

	// update header

	if (PR_SUCC(error))
	{
		error = m_hHashedIO->SeekWrite(&dwBytesWritten, 0, &m_Header, sizeof(m_Header));
		qwPos = dwBytesWritten;
	}

	if (pData)
		heapFree(pData);

	return error;
}

tERROR  PosIO_SP::DeserializeHeader()
{
	tERROR error = errOK;

	error = m_hHashedIO->SeekRead(NULL, 0, &m_Header, sizeof(m_Header));
	if (PR_FAIL(error) || m_Header.dwMagic != MAGIC_KLQB)
	{
		// init new
		memset(&m_Header, 0, sizeof(m_Header));
		m_Header.dwMagic = MAGIC_KLQB;
		m_Header.dwVersion = 1;
		m_Header.dwHeaderSize = sizeof(m_Header);
		m_Header.qwPropOffset = sizeof(m_Header);
		m_Header.qwPropSize = 0;
		m_Header.qwDataOffset = sizeof(m_Header);
		m_Header.qwDataSize = 0;
		m_Header.dwHashAlg = m_dwHashAlg;
		error = errOK;
	}

	return error;
}

tERROR  PosIO_SP::DeserializeNamedProp(tQWORD* p_pqwOffset)
{
	tERROR error = errOK;
	tDWORD dwDataSize;
	tBYTE* pData = NULL;
	tDWORD dwBytesRead;
	tQWORD qwOffset = *p_pqwOffset;

	IS error = m_hHashedIO->SeekRead(&dwBytesRead, qwOffset, &dwDataSize, sizeof(dwDataSize));
	IS qwOffset += dwBytesRead;
	if (dwDataSize > 4*1024*1024) // >4Mb
		error = errOBJECT_DATA_CORRUPTED;
	IS error = heapAlloc((tPTR*)&pData, dwDataSize);
	IS error = m_hHashedIO->SeekRead(&dwBytesRead, qwOffset, pData, dwDataSize);
	IS qwOffset += dwBytesRead;
	IS error = DecryptBuffer(0, pData, dwDataSize, NULL);
	IS error = m_hPropList->Add(NULL, pData, dwDataSize, NULL, cLIST_LAST);
	*p_pqwOffset = qwOffset;
	if (pData)
		heapFree(pData);

	return error;
}

tERROR  PosIO_SP::DeserializeNamedProps()
{
	tERROR error = errOK;
	tDWORD i;
	tQWORD qwOffset;

	if (m_hPropList == NULL)
		error = sysCreateObjectQuick((hOBJECT*)&m_hPropList, IID_LIST);
	
	if (PR_SUCC(error))
	{
		qwOffset = m_Header.qwPropOffset;
		for (i=0; i<m_Header.dwPropsCount; i++)
		{
			if (PR_FAIL(error = DeserializeNamedProp(&qwOffset)))
				break;
		}
	}

	if (PR_SUCC(error) && ((qwOffset != m_Header.qwPropOffset + m_Header.qwPropSize) || (i != m_Header.dwPropsCount)))
		error = errOBJECT_DATA_CORRUPTED;
	m_Header.dwPropsCount = i;

	return error;
}

tERROR  PosIO_SP::DeserializeTempIO()
{
	tERROR error = errOK;
	tBYTE* pData = NULL;

	if (m_hTempIo)
		return errOK;

	error = sysCreateObjectQuick((hOBJECT*)&m_hTempIo, IID_IO, PID_TEMPIO2, SUBTYPE_ANY);
	if (PR_SUCC(error))
		error = heapAlloc((tPTR*)&pData, IO_BLOCK_SIZE);
	if (PR_SUCC(error) && m_Header.qwDataSize)
	{
		tDWORD dwBytesRead;
		tQWORD qwTempOffset = 0;
		tQWORD qwHashOffset = m_Header.qwDataOffset;
		tQWORD qwData = m_Header.qwDataSize;
		for (;;)
		{
			error = m_hHashedIO->SeekRead(&dwBytesRead, qwHashOffset, pData, (qwData>IO_BLOCK_SIZE ? IO_BLOCK_SIZE : (tDWORD)qwData));
			if (dwBytesRead == 0)
				break;
			qwHashOffset += dwBytesRead;
			qwData -= dwBytesRead;
			if (PR_FAIL(error = DecryptBuffer(qwTempOffset, pData, dwBytesRead, NULL)))
				break;
			if (PR_FAIL(error = m_hTempIo->SeekWrite(NULL, qwTempOffset, pData, dwBytesRead)))
				break;
			qwTempOffset += dwBytesRead;
		}
	}
	if (pData)
		heapFree(pData);

	return error;
}

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "PosIO_SP". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR PosIO_SP::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter PosIO_SP::ObjectInit method" );

	error = sysCreateObject((hOBJECT*)&m_hHashedIO, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY);
	IS error = m_hHashedIO->propSetBool(plIGNORE_RO_ATTRIBUTE, cTRUE);
	m_dwHashAlg = cHASH_ALGORITHM_NONE;
	m_bHashAlg1Byte = cTRUE;
	m_hMyHashedIO = m_hHashedIO;

	PR_TRACE_A1( this, "Leave PosIO_SP::ObjectInit method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


tERROR PosIO_SP::AjustWriteMode()
{
	tERROR error = errOK;
	hObjPtr hTmpPtr;
	tBOOL  bOldWriteMode = m_bWriteMode;
	m_bWriteMode = !!(m_hHashedIO->get_pgOBJECT_ACCESS_MODE() & fACCESS_WRITE);
	if (bOldWriteMode != m_bWriteMode)
	{
		if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hTmpPtr, IID_OBJPTR, PID_NATIVE_FIO)))
		{
			tDWORD dwAttr = hTmpPtr->get_pgOBJECT_ATTRIBUTES();
			if (m_bWriteMode)
				dwAttr |= fQB_ATTRIBUTE_LOCKED;
			else
				dwAttr &= ~fQB_ATTRIBUTE_LOCKED;
			error = hTmpPtr->ChangeTo(*this);
			IS error = hTmpPtr->set_pgOBJECT_ATTRIBUTES( dwAttr );
			hTmpPtr->sysCloseObject();
		}
	}
	return error;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR PosIO_SP::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter PosIO_SP::ObjectInitDone method" );

	if (m_hHashedIO == m_hMyHashedIO)
		error = m_hHashedIO->sysCreateObjectDone();
	else
	{
		m_hMyHashedIO->sysCloseObject();
		m_hMyHashedIO = NULL;
	}

	IS error = sysRegisterMsgHandler(pmc_IO, rmhLISTENER, m_hHashedIO, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	
	AjustWriteMode();

	if (PR_SUCC(error))
		error = DeserializeHeader();

	PR_TRACE_A1( this, "Leave PosIO_SP::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR PosIO_SP::ObjectPreClose()
{
	tERROR error = errOK;
	hObjPtr hTmpPtr;
	PR_TRACE_A0( this, "Enter PosIO_SP::ObjectPreClose method" );

	if (m_bWriteMode)
	{
		if (m_bModified)
			error = Flush();

		if (PR_SUCC(error = sysCreateObjectQuick((hOBJECT*)&hTmpPtr, IID_OBJPTR, PID_NATIVE_FIO)))
		{
			error = hTmpPtr->ChangeTo(*this);
			IS error = hTmpPtr->set_pgOBJECT_ATTRIBUTES(hTmpPtr->get_pgOBJECT_ATTRIBUTES() & ~fQB_ATTRIBUTE_LOCKED);
			hTmpPtr->sysCloseObject();
		}
	}

	if (m_pHashKey)
		heapFree(m_pHashKey);
	
	if( m_hHashedIO && m_hHashedIO->propGetBool(pgOBJECT_DELETE_ON_CLOSE) )
	{
		sysSendMsg(pmc_POS_IO, pm_QB_IO_DELETE, NULL, NULL, NULL);
		m_bWasEverModified = cTRUE;
	}

	if (m_hHashedIO && m_hHashedIO == m_hMyHashedIO)
	{
		m_hHashedIO->sysCloseObject();
		m_hMyHashedIO = NULL;
		m_hHashedIO = NULL;
	}
	
	if (m_bWasEverModified)
		sysSendMsg(pmc_POS_IO, pm_IO_MODIFIED, NULL, NULL, NULL);
	

	PR_TRACE_A1( this, "Leave PosIO_SP::ObjectPreClose method, ret %terr", error );
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
tERROR PosIO_SP::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "PosIO_SP::MsgReceive method" );

	if (p_msg_cls_id == pmc_IO && p_send_point == (hOBJECT)m_hHashedIO)
	{
		switch (p_msg_id)
		{
			case  pm_IO_DELETE_ON_CLOSE_SUCCEED:
			case  pm_IO_DELETE_ON_CLOSE_FAILED:
				error = sysSendMsg(p_msg_cls_id, p_msg_id, p_ctx, p_par_buf, p_par_buf_len);
				break;
		}
	}
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Get )
// Interface "PosIO_SP". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
//  -- OBJECT_DELETE_ON_CLOSE
//  -- OBJECT_AVAILABILITY
//  -- HASH_KEY
tERROR PosIO_SP::Get( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method ObjectPreClose" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
		case pgOBJECT_OPEN_MODE:
		case pgOBJECT_ACCESS_MODE:
		case pgOBJECT_DELETE_ON_CLOSE:
		case pgOBJECT_AVAILABILITY:
		case pgOBJECT_ATTRIBUTES:
			error = m_hHashedIO->propGet(out_size, prop, buffer, size);
			break;

		case plHASH_KEY:
			*out_size = m_dwHashKeySize;
			if (size < m_dwHashKeySize)
				error = errBUFFER_TOO_SMALL;
			else
				error = errOK;
			if (size && m_dwHashKeySize)
				memcpy(buffer, m_pHashKey, min(size, m_dwHashKeySize));
			break;

		default:
			error = errPROPERTY_NOT_FOUND;
			*out_size = 0;
			break;
	}
	
	PR_TRACE_A2( this, "Leave *GET* multyproperty method ObjectPreClose, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Set )
// Interface "PosIO_SP". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
//  -- OBJECT_DELETE_ON_CLOSE
//  -- HASH_KEY
tERROR PosIO_SP::Set( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* multyproperty method ObjectPreClose" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
		case pgOBJECT_OPEN_MODE:
		case pgOBJECT_DELETE_ON_CLOSE:
		case pgOBJECT_ATTRIBUTES:
			error = m_hHashedIO->propSet(out_size, prop, buffer, size);
			break;

		case pgOBJECT_ACCESS_MODE:
			error = m_hHashedIO->propSet(out_size, prop, buffer, size);
			AjustWriteMode();
			break;

		case plHASH_ALGORITHM:
			if (size < sizeof(m_dwHashAlg))
				error = errBUFFER_TOO_SMALL;
			else
			{
				tDWORD dwAlg = *(tDWORD*)buffer;
				switch (dwAlg)
				{
				case cHASH_ALGORITHM_NONE:
				case cHASH_ALGORITHM_XOR:
					m_dwHashAlg = dwAlg;
					m_bHashAlg1Byte = cTRUE;
					break;
				default:
					error = errPARAMETER_INVALID;
				}
			}
			break;

		case plHASH_KEY:
			*out_size = 0;
			m_dwHashKeySize = 0;
			if (size == 0)
				error = errOK;
			else
			{
				error = heapRealloc((tPTR*)&m_pHashKey, m_pHashKey, size);
				if (PR_SUCC(error))
				{
					memcpy(m_pHashKey, buffer, size);
					m_dwHashKeySize = size;
				}
			}
			break;

		default:
			error = errPROPERTY_NOT_FOUND;
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( this, "Leave *SET* multyproperty method ObjectPreClose, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR PosIO_SP::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter PosIO_SP::SeekRead method" );

	if (!m_hTempIo && m_bHashAlg1Byte)
	{
		if (p_offset > m_Header.qwDataSize)
		{
			error = errOUT_OF_OBJECT;
		}
		else if (p_offset == m_Header.qwDataSize)
		{
			error = errEOF;
		}
		else
		{
			error = m_hHashedIO->SeekRead(&ret_val, m_Header.qwDataOffset + p_offset, p_buffer, (tDWORD)(min(m_Header.qwDataSize - p_offset, p_size)) );
		}

		if (PR_SUCC(error))
		{
			error = DecryptBuffer(p_offset, (tBYTE*)p_buffer, ret_val, NULL);
			if (ret_val < p_size)
				error = errEOF;
		}

	}
	else
	{
		if (!m_hTempIo)
			error = DeserializeTempIO();
		IS error = m_hTempIo->SeekRead(&ret_val, p_offset, p_buffer, p_size);
	}

	PR_TRACE((this, prtALWAYS_REPORTED_MSG, "PosIO_SP::SeekRead(%x, %x), DataSize=%x (result=%x, %terr)", (tDWORD)p_offset, p_size, (tDWORD)m_Header.qwDataSize, ret_val, error));

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::SeekRead method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR PosIO_SP::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter PosIO_SP::SeekWrite method" );

	if (!m_bWriteMode)
		error = errACCESS_DENIED;
	else
	{
		m_bWasEverModified = m_bModified = cTRUE;
		if (m_bHashAlg1Byte)
		{
			// use direct write

			tDWORD dwOffset;
			tBYTE* pData;
			tDWORD dwBytesWritten;

			if (m_hPropList==NULL && m_Header.dwPropsCount)
				error = DeserializeNamedProps();

			if (PR_SUCC(error = heapAlloc((tPTR*)&pData, IO_BLOCK_SIZE)))
			{
				for (dwOffset=0; dwOffset<p_size; dwOffset+=IO_BLOCK_SIZE)
				{
					tDWORD dwDataSize = min(p_size - dwOffset, IO_BLOCK_SIZE);
					memcpy(pData, (tBYTE*)p_buffer + dwOffset, dwDataSize);
					error = EncryptBuffer(p_offset + dwOffset, pData, dwDataSize, NULL);
					IS error = m_hHashedIO->SeekWrite(&dwBytesWritten, p_offset + m_Header.qwDataOffset + dwOffset, pData, dwDataSize);
					if (PR_FAIL(error))
						break;
					ret_val += dwBytesWritten;
				}
				m_Header.qwDataSize = max(m_Header.qwDataSize, p_offset + ret_val);
				heapFree(pData);
			}
		}
		else
		{
			// write into temp IO

			if (!m_hTempIo)
				error = DeserializeTempIO();
			if (PR_SUCC(error))
				error = m_hTempIo->SeekWrite(&ret_val, p_offset, p_buffer, p_size);
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::SeekWrite method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR PosIO_SP::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
	tERROR error = errOK;
	tQWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter PosIO_SP::GetSize method" );

	if (!m_bWriteMode || !m_hTempIo)
		ret_val = m_Header.qwDataSize;
	else
		error = m_hTempIo->GetSize(&ret_val, p_type);

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::GetSize method, ret tQWORD = %I64u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR PosIO_SP::SetSize( tQWORD p_new_size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter PosIO_SP::SetSize method" );

	if (!m_bWriteMode)
		error = errACCESS_DENIED;
	else
	{
		if (m_Header.qwDataSize = p_new_size)
			return errOK;
		m_bWasEverModified = m_bModified = cTRUE;
		if (m_bHashAlg1Byte)
		{
			// use direct write
			if (m_hPropList==NULL && m_Header.dwPropsCount)
				error = DeserializeNamedProps();
			if (PR_SUCC(error) && p_new_size > m_Header.qwDataSize)
				error = m_hHashedIO->SetSize(p_new_size);
			if (PR_SUCC(error))
				m_Header.qwDataSize = p_new_size;
		}
		else
		{
			if (!m_hTempIo)
				error = DeserializeTempIO();
			if (PR_SUCC(error))
				error = m_hTempIo->SetSize(p_new_size);
		}

	}
	PR_TRACE_A1( this, "Leave PosIO_SP::SetSize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR PosIO_SP::Flush()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter PosIO_SP::Flush method" );

	error = SerializeAll();
	if (PR_SUCC(error))
		m_bModified = cFALSE;

	PR_TRACE_A1( this, "Leave PosIO_SP::Flush method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Seek )
// Extended method comment
//    Текущая позиция сдвигается в зависимости от параметра origin, задающего точку отсчета,  и на величину параметра offset.
// Behaviour comment
//    Возвращает текущую позицию после сдвига.
//
//    Для выяснения текущей позиции в IO можно воспользоваться вызовом Seek(io, &qwCurPos, 0, cSEEK_CUR)
// Parameters are:
//   "origin" : Принимает значения:
//                cSEEK_SET - отсчет от начала файла
//                cSEEK_CUR - отсчет от текущей позиции
//                cSEEK_END - отсчет от конца файла
//
tERROR PosIO_SP::Seek( tQWORD* result, tLONGLONG p_offset, tDWORD p_origin )
{
	tERROR error = errOK;
	tQWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter PosIO_SP::Seek method" );

	switch(p_origin)
	{
	case cSEEK_SET:
		if (p_offset < 0)
			error = errBOF;
		else
			m_qwCurPos = p_offset;
		break;
	case cSEEK_BACK:
		p_offset = -p_offset;
	case cSEEK_CUR: // 	AKA cSEEK_FORWARD:
		if (p_offset < 0 && (tQWORD)(-p_offset) > m_qwCurPos)
			error = errBOF;
		else
			m_qwCurPos += p_offset;
		break;
	case cSEEK_END:
		if (p_offset > 0 && (tQWORD)p_offset > m_Header.qwDataSize)
			error = errEOF;
		else
			m_qwCurPos = m_Header.qwDataSize + p_offset;
		break;
	default:
		error = errPARAMETER_INVALID;
	}
	
	ret_val = m_qwCurPos;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::Seek method, ret tQWORD = %I64u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Read )
// Behaviour comment
//    Возвращает кол-во реально прочитанных байт. Текущая позиция смещается на кол-во реально прочитанных байт.
// Parameters are:
tERROR PosIO_SP::Read( tDWORD* result, tPTR p_buffer, tDWORD p_count )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter PosIO_SP::Read method" );

	error = SeekRead(&ret_val, m_qwCurPos, p_buffer, p_count);
	m_qwCurPos += ret_val;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::Read method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Write )
// Behaviour comment
//    Возвращает кол-во реально записанных байт. Текущая позиция смещается на кол-во реально записанных байт
// Parameters are:
tERROR PosIO_SP::Write( tDWORD* result, tPTR p_buffer, tDWORD p_count )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter PosIO_SP::Write method" );

	error = SeekWrite(&ret_val, m_qwCurPos, p_buffer, p_count);
	m_qwCurPos += ret_val;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::Write method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



tERROR  PosIO_SP::FindStoredProperty(tLISTNODE* p_pNode, const tSTRING p_sPropName, tDWORD p_dwIndex, tPTR* p_pData, tDWORD* p_dwDataSize)
{
	tERROR error = errOK;
	tLISTNODE node = NULL;
	tDWORD dwPropIndex = 0;

	*p_pNode = NULL;

	if (m_hPropList == NULL)
		error = DeserializeNamedProps();
	if (PR_SUCC(error = m_hPropList->First(&node)))
	{
		tBYTE* pData = NULL;
		tDWORD dwDataSize = 0;
		tDWORD dwNodeSize;
		do 
		{
			error = m_hPropList->DataGet(&dwNodeSize, node, NULL, 0);
			IS if (dwDataSize < dwNodeSize)
			{
				error = heapRealloc((tPTR*)&pData, pData, dwNodeSize);
				IS dwDataSize = dwNodeSize;
			}
			IS error = m_hPropList->DataGet(NULL, node, pData, dwDataSize);
			IS if ((p_sPropName && strcmp((char*)pData+sizeof(tDWORD), p_sPropName) == 0) || p_dwIndex == dwPropIndex)
			{
				*p_pNode = node;
				if (p_pData)
					*p_pData = pData;
				else
					heapFree(pData);
				if (p_dwDataSize)
					*p_dwDataSize = dwNodeSize;
				break;
			}
		} while (PR_SUCC(error = m_hPropList->Next(&node, node)));
		if (PR_FAIL(error) && pData)
			heapFree(pData);
	}

	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetStoredProperty )
// Extended method comment
//    Получает свойство, которое ранее было установлено на этом объекте через SetStoredProperty.
// Behaviour comment
//    Если размер буфера недостаточен для помещения в него всех данных, метод помещает в result необходимый размер буфера и возвращает errBUFFER_TOO_SMALL.
// Parameters are:
//   "sPropName" : Имя сохраняемого свойства объекта в ANSI кодировке
//   "pBuffer"   : Буфер, получающий значение свойства.
//   "dwSize"    : Указывает размер буфера в байтах
tERROR PosIO_SP::GetStoredProperty( tDWORD* result, const tSTRING p_sPropName, tPTR p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	tLISTNODE node;
	tBYTE* pData = NULL;
	tDWORD dwNodeSize;
	tDWORD dwNameLen;
	tDWORD dwDataOffset;
	tDWORD dwDataLen;
	PR_TRACE_A0( this, "Enter PosIO_SP::GetStoredProperty method" );

	if (p_sPropName == NULL || (p_dwSize==0 && result==NULL) || (p_dwSize!=0 && p_pBuffer==NULL))
		return errPARAMETER_INVALID;
	
	if (m_Header.dwPropsCount == 0)
		return errNOT_FOUND;

	if (m_hPropList == NULL)
		error = DeserializeNamedProps();
	
	IS error = FindStoredProperty(&node, p_sPropName, -1, (tPTR*)&pData, &dwNodeSize);
	if (PR_SUCC(error))
	{	
		dwNameLen = *(tDWORD*)pData;
		dwDataOffset = *(tDWORD*)pData + sizeof(tDWORD);
		dwDataLen = dwNodeSize - dwDataOffset;
	
		ret_val = dwDataLen;
		if (p_pBuffer)
			memcpy(p_pBuffer, pData+dwDataOffset, min(dwDataLen, p_dwSize));
		if ((p_dwSize || p_pBuffer) && (p_dwSize < dwDataLen))
			error = errBUFFER_TOO_SMALL;
	}

	if (pData)
		heapFree(pData);

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::GetStoredProperty method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetStoredProperty )
// Extended method comment
//    Устанавливает свойство на объекте, которое при закрытии объекта будет сохранено, а при последующем его открытии опять доступно.
// Parameters are:
//   "sPropName" : Имя сохраняемого свойства объекта в ANSI кодировке
//   "pBuffer"   : Буфер, содержащий значение свойства.
//   "dwSize"    : Указывает размер буфера в байтах
tERROR PosIO_SP::SetStoredProperty( tDWORD* result, const tSTRING p_sPropName, const tPTR p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	tDWORD dwNameLen;
	tBYTE* pData = NULL;
	PR_TRACE_A0( this, "Enter PosIO_SP::SetStoredProperty method" );

	if (p_sPropName == NULL || p_pBuffer==NULL)
		return errPARAMETER_INVALID;

	if (!m_bWriteMode)
		return errACCESS_DENIED;

	if (m_hPropList == NULL)
		error = DeserializeNamedProps();
	
	IS DeleteStoredProperty(p_sPropName); // don't check result error here
	IS dwNameLen = strlen(p_sPropName) + 1;
	IS error = heapAlloc((tPTR*)&pData, sizeof(tDWORD) + dwNameLen + p_dwSize);
	IS *(tDWORD*)pData = dwNameLen;
	IS memcpy(pData+sizeof(tDWORD), p_sPropName, dwNameLen);
	IS memcpy(pData+sizeof(tDWORD)+dwNameLen, p_pBuffer, p_dwSize);
	IS error = m_hPropList->Add(NULL, pData, sizeof(tDWORD)+dwNameLen+p_dwSize, NULL, cLIST_LAST);
	IS m_Header.dwPropsCount++;
	IS m_bWasEverModified = m_bModified = cTRUE;
	IS ret_val = p_dwSize;

	if (pData)
		heapFree(pData);

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::SetStoredProperty method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteStoredProperty )
// Extended method comment
//    Удаляет сохраняемое свойство с объекта
// Parameters are:
//   "sPropName" : Имя сохраняемого свойства объекта в ANSI кодировке
tERROR PosIO_SP::DeleteStoredProperty( const tSTRING p_sPropName )
{
	tERROR error = errOK;
	tLISTNODE node;
	PR_TRACE_A0( this, "Enter PosIO_SP::DeleteStoredProperty method" );

	if (p_sPropName == NULL)
		return errPARAMETER_INVALID;
	
	if (!m_bWriteMode)
		return errACCESS_DENIED;

	if (m_Header.dwPropsCount == 0)
		return errNOT_FOUND;

	if (m_hPropList == NULL)
		error = DeserializeNamedProps();

	IS error = FindStoredProperty(&node, p_sPropName, -1, NULL, NULL);
	IS error = m_hPropList->Remove(node);
	IS m_Header.dwPropsCount--;
	IS m_bWasEverModified = m_bModified = cTRUE;

	PR_TRACE_A1( this, "Leave PosIO_SP::DeleteStoredProperty method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStoredPropertyName )
// Extended method comment
//    Получает имя сохраняемого свойства по его индексу(номеру)
// Behaviour comment
//    Если размер буфера недостаточен для помещения в него имени, метод помещает в result необходимый размер буфера и возвращает errBUFFER_TOO_SMALL.
// Parameters are:
//   "dwSize"  : Размер буфера, принимающеко имя свойства, в байтах
tERROR PosIO_SP::GetStoredPropertyName( tDWORD* result, tDWORD p_dwIndex, tSTRING p_pBuffer, tDWORD p_dwSize )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	tLISTNODE node;
	tBYTE* pData = NULL;
	tDWORD dwNodeSize;
	tDWORD dwNameLen;
	PR_TRACE_A0( this, "Enter PosIO_SP::GetStoredPropertyName method" );

	if ((p_dwSize==0 && result==NULL) || (p_dwSize!=0 && p_pBuffer==NULL))
		return errPARAMETER_INVALID;

	if (m_Header.dwPropsCount == 0)
		return errNOT_FOUND;

	if (m_hPropList == NULL)
		error = DeserializeNamedProps();
	
	IS error = FindStoredProperty(&node, NULL, p_dwIndex, (tPTR*)&pData, &dwNodeSize);
	if (PR_SUCC(error))
	{	
		dwNameLen = *(tDWORD*)pData;

		ret_val = dwNameLen;
		if (p_pBuffer)
			memcpy(p_pBuffer, pData+sizeof(tDWORD), min(dwNameLen, p_dwSize));
		if ((p_pBuffer || p_dwSize) && (p_dwSize < dwNameLen))
			error = errBUFFER_TOO_SMALL;
	}
	if (pData)
		heapFree(pData);

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave PosIO_SP::GetStoredPropertyName method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "PosIO_SP". Register function
tERROR PosIO_SP::Register( hROOT root ) {
	tERROR error = errOK;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(PosIO_SP_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, PosIO_SP_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )

// IO properties
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, PosIO_SP, m_OrigID, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FULL_NAME, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( pgOBJECT_OPEN_MODE, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, FN_CUST(Get), NULL )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, PosIO_SP, m_hBasedOn, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, FN_CUST(Get), FN_CUST(Set) )

// PosIO interface is compatible with IO one
	mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(IID_IO)) )
	mpLOCAL_PROPERTY( plHASH_ALGORITHM, PosIO_SP, m_dwHashAlg, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT, NULL, FN_CUST(Set) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( plHASH_KEY, FN_CUST(Get), FN_CUST(Set) )
	mpLOCAL_PROPERTY_BUF( plHASHED_IO, PosIO_SP, m_hHashedIO, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(PosIO_SP_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(PosIO_SP)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(PosIO_SP)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(PosIO_SP)
	mEXTERNAL_METHOD(PosIO_SP, SeekRead)
	mEXTERNAL_METHOD(PosIO_SP, SeekWrite)
	mEXTERNAL_METHOD(PosIO_SP, GetSize)
	mEXTERNAL_METHOD(PosIO_SP, SetSize)
	mEXTERNAL_METHOD(PosIO_SP, Flush)
	mEXTERNAL_METHOD(PosIO_SP, Seek)
	mEXTERNAL_METHOD(PosIO_SP, Read)
	mEXTERNAL_METHOD(PosIO_SP, Write)
	mEXTERNAL_METHOD(PosIO_SP, GetStoredProperty)
	mEXTERNAL_METHOD(PosIO_SP, SetStoredProperty)
	mEXTERNAL_METHOD(PosIO_SP, DeleteStoredProperty)
	mEXTERNAL_METHOD(PosIO_SP, GetStoredPropertyName)
mEXTERNAL_TABLE_END(PosIO_SP)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter PosIO_SP::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_POSIO_SP,                           // interface identifier
		PID_QB,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		PosIO_SP_VERSION,                       // interface version
		VID_PETROVITCH,                         // interface developer
		&i_PosIO_SP_vtbl,                       // internal(kernel called) function table
		(sizeof(i_PosIO_SP_vtbl)/sizeof(tPTR)), // internal function table size
		&e_PosIO_SP_vtbl,                       // external function table
		(sizeof(e_PosIO_SP_vtbl)/sizeof(tPTR)), // external function table size
		PosIO_SP_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(PosIO_SP_PropTable),// property table size
		sizeof(PosIO_SP)-sizeof(cObjImpl),      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"PosIO_SP(IID_POSIO_SP) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave PosIO_SP::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call PosIO_SP_Register( hROOT root ) { return PosIO_SP::Register(root); }
// AVP Prague stamp end



