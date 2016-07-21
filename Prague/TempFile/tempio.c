// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  18 April 2003,  09:56 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Base Technologies
// Purpose     -- Предоставляет виртуальное IO
// Author      -- Pavlushchik
// File Name   -- posio.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define TempIO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include <tempio.h>
// AVP Prague stamp end


// AVP Prague stamp begin( Interface comment,  )
// TempIO interface implementation
// Short comments -- Common TempIO with Position fucnctions for IID_IO & IID_POSIO interfaces of PID_TEMPIO
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure "sTempLocals" is described in "O:\Prague\TempFile\tempio.h" header file
// AVP Prague stamp end


// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_TempIO {
	const void*        vtbl; // pointer to the "TempIO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	sTempLocals*       data; // pointer to the "TempIO" data structure
} *hi_TempIO;
// AVP Prague stamp end


// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call TempIO_ObjectInit( hi_TempIO _this );
tERROR pr_call TempIO_ObjectInitDone( hi_TempIO _this );
tERROR pr_call TempIO_ObjectPreClose( hi_TempIO _this );
// AVP Prague stamp end


// AVP Prague stamp begin( External method table prototypes,  )
// Standard IO methods
tERROR pr_call TempIO_SeekRead( hi_TempIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call TempIO_SeekWrite( hi_TempIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call TempIO_GetSize( hi_TempIO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call TempIO_SetSize( hi_TempIO _this, tQWORD new_size );
tERROR pr_call TempIO_Flush( hi_TempIO _this );
// Additional TempIO methods
tERROR pr_call TempIO_Seek( hi_TempIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin );
tERROR pr_call TempIO_Read( hi_TempIO _this, tDWORD* result, tPTR buffer, tDWORD count );
tERROR pr_call TempIO_Write( hi_TempIO _this, tDWORD* result, tPTR buffer, tDWORD count );
// AVP Prague stamp end


// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call TempIO_PROP_CommonGet( hi_TempIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call TempIO_PROP_CommonSet( hi_TempIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end






#include "cache.c"


// AVP Prague stamp begin( Property method implementation, CommonGet )
// Interface "TempIO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
tERROR pr_call TempIO_PROP_CommonGet( hi_TempIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	sLocals* data =_this->data;

	PR_TRACE_A0( _this, "Enter *GET* multyproperty method TempIO_PROP_CommonGet" );

	if ( buffer && size )
	{
		switch( prop )
		{
			case pgOBJECT_FULL_NAME:
			{
			tUINT ps = lstrlen( data->m_szObjectPath );
			tUINT ns = lstrlen( data->m_szObjectName );

				if ( ps+ns+1 >= size )
				{
					error = errBUFFER_TOO_SMALL;
				}
				else
				{
					memcpy( buffer,    data->m_szObjectPath, ps );
					memcpy( buffer+ps, data->m_szObjectName, ns+1 );
				}
				size = ps + ns + 1;
				break;
			}
			case pgOBJECT_ACCESS_MODE:
				if(size >= sizeof(tDWORD))
				{
					*((tDWORD*)buffer) = data->m_dwfAccessMode;
				}
				else
				{
					error = errBUFFER_TOO_SMALL;
				}
				size = sizeof(tDWORD);
				break;
			case pgOBJECT_OPEN_MODE:
				if(size >= sizeof(tDWORD))
				{
					*((tDWORD*)buffer) = data->m_dwfOpenMode;
				}
				else
				{
					error = errBUFFER_TOO_SMALL;
				}
				size = sizeof(tDWORD);
				break;
		}
	}
	else if ( !buffer && !size )
	{
		switch(prop)
		{
			case pgOBJECT_FULL_NAME:
				size = lstrlen( data->m_szObjectPath ) + lstrlen( data->m_szObjectName ) + 1;
				break;
			case pgOBJECT_ACCESS_MODE:
			case pgOBJECT_OPEN_MODE:
				size = sizeof(tDWORD);
				break;
		}
	}
	else
	{
		error = errPARAMETER_INVALID;
		size = 0;
	}

	if(out_size)
		*out_size = size;

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method TempIO_PROP_CommonGet, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonSet )
// Interface "TempIO". Method "Set" for property(s):
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
tERROR pr_call TempIO_PROP_CommonSet( hi_TempIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	sLocals* data =_this->data;

	PR_TRACE_A0( _this, "Enter *SET* multyproperty method TempIO_PROP_CommonSet" );

	error = errOK;
	if ( buffer && size )
	{
		switch( prop )
		{
			case pgOBJECT_ACCESS_MODE:
				size = sizeof(tDWORD);
				data->m_dwfAccessMode = *((tDWORD*)buffer);
				break;
			case pgOBJECT_OPEN_MODE:
				size = sizeof(tDWORD);
				data->m_dwfOpenMode = *((tDWORD*)buffer);
				break;
		}
	}
	else if ( !buffer && !size )
	{
		switch(prop)
		{
			case pgOBJECT_ACCESS_MODE:
			case pgOBJECT_OPEN_MODE:
				size = sizeof(tDWORD);
				break;
		}
	}
	else
	{
		error = errPARAMETER_INVALID;
		size = 0;
	}
	if(out_size)
		*out_size = size;

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method TempIO_PROP_CommonSet, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call TempIO_ObjectInit( hi_TempIO _this ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter TempIO::ObjectInit method" );

	_this->data->m_ObjectOrigin = OID_GENERIC_IO;
	_this->data->bDeleteOnClose = cFALSE;
	_this->data->m_dwfAccessMode = fACCESS_RW;
	_this->data->m_dwfOpenMode = fOMODE_CREATE_ALWAYS;
	error=CALL_SYS_ObjectCreateQuick( _this, &_this->data->hStr, IID_STRING, PID_ANY, SUBTYPE_ANY );

	PR_TRACE_A1( _this, "Leave TempIO::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call TempIO_ObjectInitDone( hi_TempIO _this ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter TempIO::ObjectInitDone method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave TempIO::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call TempIO_ObjectPreClose( hi_TempIO _this ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter TempIO::ObjectPreClose method" );

	error=SectorCacheFlushIO((hi_TempIO)_this,1,0);

	if(_this->data->real_io)
		error = CALL_SYS_ObjectClose(_this->data->real_io);
//should be deleted automatically - by fOMODE_TEMPORARY flag.

	if(_this->data->hStr)
		CALL_SYS_ObjectClose(_this->data->hStr);

	PR_TRACE_A1( _this, "Leave TempIO::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR pr_call TempIO_SeekRead( hi_TempIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
	tERROR error = errOK;
	tBOOL  eof=cFALSE;
	
	PR_TRACE_A0( _this, "Enter TempIO::SeekRead method" );
	
    if(result)
		*result = 0;
	
    if (offset > _this->data->FileSize){
		size = 0;
		error=errOUT_OF_OBJECT;
	}
	else if ((offset + size) > _this->data->FileSize){
		size = (tDWORD) (_this->data->FileSize - offset);
		eof=1;
	}
	
    while( size && PR_SUCC(error) )
    {
		tBYTE* data;
		tDWORD readoff = (tDWORD)( offset & ~(CSECT_MASK) );
		tDWORD count = CSECT_SIZE - readoff;
		if( count > size ) count = size;
		
		EnterCriticalSection(&hCS_Cache);
		if(PR_SUCC(error=SectorCacheMapSector(_this, &data, offset & CSECT_MASK, 0, 0)))
			memcpy(buffer, data + readoff, count);
		else
			count=0;
		LeaveCriticalSection(&hCS_Cache);

		if(result)
			*result += count;
        offset += count;
        ((tBYTE*)buffer) += count;
        size -= count;
    }

	if(eof && PR_SUCC(error))
		error=errEOF;

	PR_TRACE_A2( _this, "Leave TempIO::SeekRead method, ret tDWORD = %u, error = 0x%08x",  (result?*result:0), error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call TempIO_SeekWrite( hi_TempIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter TempIO::SeekWrite method" );

    if(result)
		*result = 0;
	
    while( size && PR_SUCC(error) )
    {
		tBYTE* data;
		tDWORD readoff = (tDWORD)( offset & ~(CSECT_MASK) );
		tDWORD count = CSECT_SIZE - readoff;
		if( count > size ) count = size;
		
		EnterCriticalSection(&hCS_Cache);
		if(PR_SUCC(error=SectorCacheMapSector(_this, &data, offset & CSECT_MASK, 1, 0)))
			memcpy(data + readoff, buffer, count);
		else
			count=0;
		LeaveCriticalSection(&hCS_Cache);

		if(result)
			*result += count;
        offset += count;
        ((tBYTE*)buffer) += count;
        size -= count;

		if( _this->data->FileSize < offset)
			_this->data->FileSize = offset;
    }

	PR_TRACE_A2( _this, "Leave TempIO::SeekWrite method, ret tDWORD = %u, error = 0x%08x", (result?*result:0), error );
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
tERROR pr_call TempIO_GetSize( hi_TempIO _this, tQWORD* result, IO_SIZE_TYPE type ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter TempIO::GetSize method" );

	if(result)
		*result = _this->data->FileSize;
	else
		error = errPARAMETER_INVALID;

	PR_TRACE_A2( _this, "Leave TempIO::GetSize method, ret tQWORD = %I64u, error = 0x%08x", _this->data->FileSize, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call TempIO_SetSize( hi_TempIO _this, tQWORD new_size ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter TempIO::SetSize method" );

	_this->data->FileSize = new_size;
//Graf: it is not necessary.
//	if (_this->data->m_qwCurPos > new_size)
//		_this->data->m_qwCurPos = new_size;
	error=SectorCacheFlushIO((hi_TempIO)_this,0,0);

	PR_TRACE_A1( _this, "Leave TempIO::SetSize method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call TempIO_Flush( hi_TempIO _this ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter TempIO::Flush method" );

	// Implemened by cache

	PR_TRACE_A1( _this, "Leave TempIO::Flush method, ret tERROR = 0x%08x", error );
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
tERROR pr_call TempIO_Seek( hi_TempIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter TempIO::Seek method" );

	switch(origin)
	{
	case cSEEK_SET:
		if (offset < 0)
			error = errBOF;
		else
			_this->data->m_qwCurPos = offset;
		break;
	case cSEEK_BACK:
		offset = -offset;
	case cSEEK_CUR: // 	AKA cSEEK_FORWARD:
		if (offset < 0 && (tQWORD)(-offset) > _this->data->m_qwCurPos)
			error = errBOF;
		else
			_this->data->m_qwCurPos += offset;
		break;
	case cSEEK_END:
		if (offset > 0 && (tQWORD)offset > _this->data->FileSize)
			error = errEOF;
		else
			_this->data->m_qwCurPos = _this->data->FileSize + offset;
		break;
	default:
		error = errPARAMETER_INVALID;
	}

	if ( result )
		*result = _this->data->m_qwCurPos;
	PR_TRACE_A2( _this, "Leave TempIO::Seek method, ret tQWORD = %I64u, error = 0x%08x", _this->data->m_qwCurPos, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Read )
// Behaviour comment
//    Возвращает кол-во реально прочитанных байт. Текущая позиция смещается на кол-во реально прочитанных байт.
// Parameters are:
tERROR pr_call TempIO_Read( hi_TempIO _this, tDWORD* result, tPTR buffer, tDWORD count ) {
	tERROR error = errOK;
	tDWORD dwBytesRead = 0;
	PR_TRACE_A0( _this, "Enter TempIO::Read method" );

	error = TempIO_SeekRead(_this, &dwBytesRead, _this->data->m_qwCurPos, buffer, count);
	_this->data->m_qwCurPos += dwBytesRead;

	if ( result )
		*result = dwBytesRead;
	PR_TRACE_A2( _this, "Leave TempIO::Read method, ret tDWORD = %u, error = 0x%08x", dwBytesRead, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Write )
// Behaviour comment
//    Возвращает кол-во реально записанных байт. Текущая позиция смещается на кол-во реально записанных байт
// Parameters are:
tERROR pr_call TempIO_Write( hi_TempIO _this, tDWORD* result, tPTR buffer, tDWORD count ) {
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD dwBytesWritten = 0;
	PR_TRACE_A0( _this, "Enter TempIO::Write method" );

	error = TempIO_SeekWrite(_this, &dwBytesWritten, _this->data->m_qwCurPos, buffer, count);
	_this->data->m_qwCurPos += dwBytesWritten;

	if ( result )
		*result = dwBytesWritten;
	PR_TRACE_A2( _this, "Leave TempIO::Write method, ret tDWORD = %u, error = 0x%08x", dwBytesWritten, error );
	return error;
}
// AVP Prague stamp end



