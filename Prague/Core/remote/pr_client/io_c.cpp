#include "StdAfx.h"
#include <rpc_io.h>

struct IOProxy_Data
{
	hIO			tmp_io;
	tQWORD		offset;
	unsigned	modify:1;
	unsigned	native:1;
	unsigned	read_all:1;
	unsigned	init_fail:1;
};

tERROR InitBuffering(hIO _this, IOProxy_Data *data)
{
	if( data->init_fail )
		return errOBJECT_BAD_INTERNAL_STATE;

	if( data->tmp_io )
		return errOK;

	tCHAR path[MAX_PATH];
	tERROR l_error = CALL_SYS_PropertyGetStr(_this, NULL, pgOBJECT_NATIVE_PATH, path, sizeof(path), cCP_ANSI);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_ObjectCreate(_this, &data->tmp_io, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY);
	if( PR_SUCC(l_error) )
	{
		CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
		CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetStr(data->tmp_io, 0, pgOBJECT_NAME, path, 0, cCP_ANSI);
		l_error = CALL_SYS_ObjectCreateDone(data->tmp_io);	
	}

	if( PR_SUCC(l_error) )
	{
		data->native = 1;
		data->read_all = 1;
	}
	else
	{
		if( data->tmp_io )
			CALL_SYS_ObjectClose(data->tmp_io);
		l_error = CALL_SYS_ObjectCreate(_this, &data->tmp_io, IID_IO, PID_TMPFILE, 0);
	}

	if( PR_SUCC(l_error) )
	{
		CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetDWord(data->tmp_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
		l_error = CALL_SYS_ObjectCreateDone(data->tmp_io);
	}

	if( PR_FAIL(l_error) )
		data->init_fail = 1;

	return l_error;
}

tERROR ReadBuffering(hIO _this, IOProxy_Data *data, tQWORD size)
{
	tCHAR	l_buffer[0x2000];
	tDWORD	l_read_size = sizeof(l_buffer);
	tQWORD	l_size = 0;
	tERROR	l_error = errOK;

	if( size < sizeof(l_buffer) )
		size = sizeof(l_buffer);

	while( PR_SUCC(l_error) && l_read_size == sizeof(l_buffer) && l_size < size )
	{
		PR_PROXY_ERRCALL(l_error, IO_SeekRead(PR_THIS_PROXY, &l_read_size, data->offset, l_buffer, sizeof(l_buffer)) );
		if( l_error == errEOF )
			data->read_all = 1;

		if( PR_SUCC(l_error) )
			l_error = CALL_IO_SeekWrite( data->tmp_io, NULL, data->offset, l_buffer, l_read_size );

		data->offset += l_read_size;
		l_size += l_read_size;
	}

	return l_error;
}

tERROR FlushBuffering(hIO _this)
{
	PR_USE_DATA(IOProxy_Data)

	if( !data->modify || !data->tmp_io )
		return errOK;

	tERROR l_error = errOK;
	if( !data->native )
	{
		tCHAR	l_buffer[0x2000];
		tDWORD	l_read_size = sizeof(l_buffer);
		tQWORD	l_offset = 0;

		while( PR_SUCC(l_error) && l_read_size == sizeof(l_buffer) )
		{
			l_error = CALL_IO_SeekRead( data->tmp_io, &l_read_size, l_offset, l_buffer, l_read_size );

			if( PR_SUCC(l_error) )
				PR_PROXY_ERRCALL(l_error, IO_SeekWrite(PR_THIS_PROXY, NULL, l_offset, l_buffer, l_read_size) );

			l_offset += l_read_size;
		}

		if( PR_SUCC(l_error) )
			PR_PROXY_ERRCALL(l_error, IO_SetSize( PR_THIS_PROXY, l_offset ) );
	}
	else
	{
		tBYTE ch;
		PR_PROXY_ERRCALL(l_error, IO_SeekRead(PR_THIS_PROXY, NULL, 0, (tCHAR*)&ch, sizeof(ch)));
		PR_PROXY_ERRCALL(l_error, IO_SeekWrite(PR_THIS_PROXY, NULL, 0, (tCHAR*)&ch, sizeof(ch)));
	}

	data->modify = 0;
	return l_error;
}

// ---
tERROR pr_call Local_IO_SeekRead( hIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	PR_USE_DATA(IOProxy_Data)

	tERROR error = InitBuffering(_this, data);

	if( PR_SUCC(error) && !data->read_all && data->offset < (offset + size) )
		error = ReadBuffering(_this, data, offset + size - data->offset);

	if( PR_SUCC(error) )
		error = CALL_IO_SeekRead(data->tmp_io, result, offset, buffer, size);

	return error;
}
// ---
tERROR pr_call Local_IO_SeekWrite( hIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	PR_USE_DATA(IOProxy_Data)

	tERROR error = InitBuffering(_this, data);

	if( PR_SUCC(error) && !data->read_all && data->offset < (offset + size) )
		error = ReadBuffering(_this, data, offset + size - data->offset);

	if( PR_SUCC(error) )
		error = CALL_IO_SeekWrite(data->tmp_io, result, offset, buffer, size);

	if( PR_SUCC(error) )
		data->modify = 1;

	return error;
}
// ---
tERROR pr_call Local_IO_GetSize( hIO _this, tQWORD* result, IO_SIZE_TYPE type )
{
	PR_USE_DATA(IOProxy_Data)

	if( data->read_all )
		return CALL_IO_GetSize(data->tmp_io, result, type);

	PR_PROXY_CALL( IO_GetSize( PR_THIS_PROXY, result, type ) );
}
// ---
tERROR pr_call Local_IO_SetSize( hIO _this, tQWORD new_size )
{
	PR_USE_DATA(IOProxy_Data)

	tERROR l_error;
	if( data->read_all || new_size < data->offset )
	{
		l_error = CALL_IO_SetSize(data->tmp_io, new_size);
		if( PR_SUCC(l_error) )
			data->read_all = 1;
	}
	else
		PR_PROXY_ERRCALL(l_error, IO_SetSize( PR_THIS_PROXY, new_size ) );

	if( PR_SUCC(l_error) )
		data->modify = 1;

	return l_error;
}
// ---
tERROR pr_call Local_IO_Flush( hIO _this )
{
	tERROR error = FlushBuffering(_this);
	if( PR_FAIL(error) )
		return error;

	PR_PROXY_CALL( IO_Flush( PR_THIS_PROXY ) );
}

tERROR pr_call IO_ObjectInitDone(hIO _this)
{
	return errOK;
}

tERROR pr_call IO_ObjectPreClose(hIO _this)
{
	return FlushBuffering(_this);
}

iINTERNAL io_internal =
{
	NULL,// IntFnRecognize,
	NULL,// IntFnObjectNew,
	NULL,// IntFnObjectInit,
	(tIntFnObjectInitDone)IO_ObjectInitDone,
	NULL,// IntFnObjectCheck,
	(tIntFnObjectPreClose)IO_ObjectPreClose,
	NULL,// IntFnObjectClose,
	NULL,// IntFnChildNew,
	NULL,// IntFnChildInitDone,
	NULL,// IntFnChildClose,
	NULL,// IntFnMsgReceive,
	NULL// IntFnIFaceTransfer
};

// ---
iIOVtbl io_iface = {
	Local_IO_SeekRead,
	Local_IO_SeekWrite,
	Local_IO_GetSize,
	Local_IO_SetSize,
	Local_IO_Flush,
}; // "IO" external virtual table prototype

PR_IMPLEMENT_PROXY_EX(IID_IO, io_iface, &io_internal, sizeof(IOProxy_Data))


