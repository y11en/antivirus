// ipcBuffer.cpp /////////////////////////////////////////////////////////////

// Interprocess Communication Buffer
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "IPCBuffer.h"

//////////////////////////////////////////////////////////////////////////////

char * C_IPC_BUFFER::IPC_PREFIX_NAME = 0;
const DWORD C_IPC_BUFFER::IPC_BUF_SIZE = ( 4 * 1024 ); // IPC buffer size in bytes

//////////////////////////////////////////////////////////////////////////////

C_IPC_BUFFER::C_IPC_BUFFER()
{
	m_enSide			= IPC_SIDE_UNKNOWN;
	m_hFileMap			= 0;
	m_pbyBuf			= 0;
}

//////////////////////////////////////////////////////////////////////////////

const char * C_IPC_BUFFER::FrefixName()
{
	if ( ! IPC_PREFIX_NAME )
	{
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		if ( GetVersionEx( &osvi ) && osvi.dwMajorVersion >= 5 )
			IPC_PREFIX_NAME = "Global\\IPC32_73ad8270-5f29-11d5-8ee9__";
		else
			IPC_PREFIX_NAME = "IPC32_73ad8270-5f29-11d5-8ee9__";
	}
	return IPC_PREFIX_NAME;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_BUFFER::Create( char *pszBufName, SECURITY_ATTRIBUTES *pSA )
{
	if ( ! pszBufName || ! *pszBufName || strlen( pszBufName ) >= _MAX_PATH ) return false;

	//////////////////////////////////////////////

	HANDLE hFileMap = CreateFileMapping( (HANDLE)0xffffffff, pSA, PAGE_READWRITE, 0, IPC_BUF_SIZE * 2, pszBufName );
	if ( ! hFileMap ) return false;
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		CloseHandle( hFileMap );
		return false;
	}

	BYTE *pbyBuf = (BYTE *)MapViewOfFile(	hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0 );
	if ( ! pbyBuf )
	{
		CloseHandle( hFileMap );
		return false;
	}

	//////////////////////////////////////////////

	m_enSide	= IPC_SIDE_SERVER;
	m_hFileMap	= hFileMap;
	m_pbyBuf	= pbyBuf;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_BUFFER::Open( char *pszBufName )
{
	if ( ! pszBufName || ! *pszBufName || strlen( pszBufName ) >= _MAX_PATH ) return false;

	//////////////////////////////////////////////

	HANDLE hFileMap = OpenFileMapping( FILE_MAP_WRITE | FILE_MAP_READ, FALSE, pszBufName );
	if ( hFileMap == 0 ) return false;

	BYTE * pbyBuf = (BYTE *)MapViewOfFile( hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0 );
	if ( pbyBuf == 0 ) {
		CloseHandle( hFileMap );
		return false;
	}

	//////////////////////////////////////////////

	m_enSide	= IPC_SIDE_CLIENT;
	m_hFileMap	= hFileMap;
	m_pbyBuf	= pbyBuf;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_BUFFER::Close()
{
	if ( ! m_pbyBuf || !UnmapViewOfFile( m_pbyBuf ) || !CloseHandle( m_hFileMap ) ) return false;

	//////////////////////////////////////////////

	m_enSide	= IPC_SIDE_UNKNOWN;
	m_hFileMap	= 0;
	m_pbyBuf	= 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

DWORD C_IPC_BUFFER::Read( BYTE *pbyBuf, DWORD dwBufSize )
{
	if ( ! pbyBuf || dwBufSize > IPC_BUF_SIZE ) return 0xffffffff;

	switch ( m_enSide )
	{
		case IPC_SIDE_SERVER:
			CopyMemory( pbyBuf, m_pbyBuf + IPC_BUF_SIZE, dwBufSize );
			break;
		case IPC_SIDE_CLIENT:
			CopyMemory( pbyBuf, m_pbyBuf, dwBufSize );
			break;
		default:
			return 0xffffffff;
	}

	return dwBufSize;
}

//////////////////////////////////////////////////////////////////////////////

DWORD C_IPC_BUFFER::Write( BYTE *pbyBuf, DWORD dwBufSize )
{
	if ( ! pbyBuf || dwBufSize > IPC_BUF_SIZE ) return 0xffffffff;

	switch ( m_enSide )
	{
		case IPC_SIDE_SERVER:
			CopyMemory( m_pbyBuf, pbyBuf, dwBufSize );
			break;
		case IPC_SIDE_CLIENT:
			CopyMemory( m_pbyBuf + IPC_BUF_SIZE, pbyBuf, dwBufSize );
			break;
		default:
			return 0xffffffff;
	}

	return dwBufSize;
}

// eof ///////////////////////////////////////////////////////////////////////