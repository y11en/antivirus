// ipcServer.cpp /////////////////////////////////////////////////////////////

// Interprocess Communication Server
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "IPCServer.h"
#include "ipc_def.h"
#include "DateTime.h"
#include "IPCInfo.h"

//////////////////////////////////////////////////////////////////////////////

const char *const C_IPC_SERVER::IPC_ADD_NAME_MAPFILE_BUFFER	= "__SERVER_MAPFILE_BUFFER";

const char *const C_IPC_SERVER::IPC_ADD_NAME_BREAK			= "__SERVER_BREAK";

const char *const C_IPC_SERVER::IPC_ADD_NAME_WAIT_BUSY		= "__SERVER_WAIT_BUSY";
const char *const C_IPC_SERVER::IPC_ADD_NAME_CONNECT_BUSY	= "__SERVER_CONNECT_BUSY";

const char *const C_IPC_SERVER::IPC_ADD_NAME_CONNECT_NEW	= "__SERVER_CONNECT_NEW";

const char *const C_IPC_SERVER::IPC_ADD_NAME_SEND			= "__SERVER_SEND";
const char *const C_IPC_SERVER::IPC_ADD_NAME_RECV			= "__SERVER_RECV";

//////////////////////////////////////////////////////////////////////////////

C_IPC_SERVER::C_IPC_SERVER()
{
	m_boInited			= false;

	m_hBreak			= 0;

	m_hWaitBusy			= 0;
	m_hConnectBusy		= 0;

	m_hConnectNew		= 0;

	m_hSend				= 0;
	m_hRecv				= 0;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_SERVER::IsValidServerName( char * pszServerName ) const
{
	if ( !pszServerName || !*pszServerName ) return false;

	DWORD dwPartOfFullLen = strlen( FrefixName() ) + strlen( pszServerName );

	if( dwPartOfFullLen + strlen( IPC_ADD_NAME_MAPFILE_BUFFER )		>= _MAX_PATH ||

		dwPartOfFullLen + strlen( IPC_ADD_NAME_BREAK )				>= _MAX_PATH ||

		dwPartOfFullLen + strlen( IPC_ADD_NAME_WAIT_BUSY )			>= _MAX_PATH ||
		dwPartOfFullLen + strlen( IPC_ADD_NAME_CONNECT_BUSY )		>= _MAX_PATH ||

		dwPartOfFullLen + strlen( IPC_ADD_NAME_CONNECT_NEW )		>= _MAX_PATH ||

		dwPartOfFullLen + strlen( IPC_ADD_NAME_SEND )				>= _MAX_PATH ||
		dwPartOfFullLen + strlen( IPC_ADD_NAME_RECV )				>= _MAX_PATH )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_SERVER::Create( char * pszServerName, SECURITY_ATTRIBUTES * pSA )
{
	if( m_boInited || !IsValidServerName( pszServerName ) ) return false;
	char szFirstPartOfName[ _MAX_PATH ];
	strcpy( szFirstPartOfName, FrefixName() ); strcat( szFirstPartOfName, pszServerName );
	char szName[ _MAX_PATH ];

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_MAPFILE_BUFFER );
	if( !C_IPC_BUFFER::Create( szName, pSA ) ) return false;

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_BREAK );
	HANDLE hBreak = CreateEvent( pSA, TRUE/*manually*/, FALSE/*nonsignaled*/, szName );
	if( !hBreak || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hBreak ) CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_WAIT_BUSY );
	HANDLE hWaitBusy = CreateSemaphore( pSA, 1, 1, szName );
	if( !hWaitBusy || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if( hWaitBusy ) CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CONNECT_BUSY );
	HANDLE hConnectBusy = CreateSemaphore( pSA, 1, 1, szName );
	if( !hWaitBusy || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hWaitBusy ) CloseHandle( hWaitBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CONNECT_NEW );
	HANDLE hConnect = CreateSemaphore( pSA, 0, 1, szName );
	if( !hConnect || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hConnect ) CloseHandle( hConnect );
		CloseHandle( hConnectBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SEND );
	HANDLE hSend = CreateSemaphore( pSA, 0, 1, szName );
	if( !hSend || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hSend )	CloseHandle( hSend );
		CloseHandle( hConnect );
		CloseHandle( hConnectBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_RECV );
	HANDLE hRecv = CreateSemaphore( pSA, 0, 1, szName );
	if( !hRecv || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hRecv )	CloseHandle( hRecv );
		CloseHandle( hSend );
		CloseHandle( hConnect );
		CloseHandle( hConnectBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	m_boInited			= true;

	m_hBreak			= hBreak;

	m_hWaitBusy			= hWaitBusy;
	m_hConnectBusy		= hConnectBusy;

	m_hConnectNew		= hConnect;

	m_hSend				= hSend;
	m_hRecv				= hRecv;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_SERVER::Open( char *pszServerName )
{
	if( m_boInited || !IsValidServerName( pszServerName ) ) return false;
	char szFirstPartOfName[ _MAX_PATH ];
	strcpy( szFirstPartOfName, FrefixName() ); strcat( szFirstPartOfName, pszServerName );
	char szName[ _MAX_PATH ];

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_MAPFILE_BUFFER );
	if( !C_IPC_BUFFER::Open( szName ) ) return false;

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_BREAK );
	HANDLE hBreak = OpenEvent( EVENT_ALL_ACCESS, FALSE, szName );
	if( !hBreak )
	{
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_WAIT_BUSY );
	HANDLE hWaitBusy = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hWaitBusy )
	{
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CONNECT_BUSY );
	HANDLE hConnectBusy = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hWaitBusy )
	{
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CONNECT_NEW );
	HANDLE hConnect = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hConnect )
	{
		CloseHandle( hConnectBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SEND );
	HANDLE hSend = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hSend )
	{
		CloseHandle( hConnect );
		CloseHandle( hConnectBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_RECV );
	HANDLE hRecv = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hRecv )
	{
		CloseHandle( hSend );
		CloseHandle( hConnect );
		CloseHandle( hConnectBusy );
		CloseHandle( hWaitBusy );
		CloseHandle( hBreak );
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	m_boInited			= true;

	m_hBreak			= hBreak;

	m_hWaitBusy			= hWaitBusy;
	m_hConnectBusy		= hConnectBusy;

	m_hConnectNew		= hConnect;

	m_hSend				= hSend;
	m_hRecv				= hRecv;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_SERVER::Close()
{
	if ( !m_boInited ) return false;

	switch ( m_enSide )
	{
		case IPC_SIDE_SERVER:
			if ( !::SetEvent( m_hBreak ) ||
				::WaitForSingleObject( m_hWaitBusy, INFINITE ) != WAIT_OBJECT_0 )
				return false;
			break;
		case IPC_SIDE_CLIENT:
			break;
		default:
			return false;
	}

	if ( !::CloseHandle( m_hRecv ) ||
		 !::CloseHandle( m_hSend ) ||
		 !::CloseHandle( m_hConnectNew ) ||
		 !::CloseHandle( m_hConnectBusy ) ||
		 !::CloseHandle( m_hWaitBusy ) ||
		 !::CloseHandle( m_hBreak ) ||
		 !C_IPC_BUFFER::Close()
	)
		return false;

	//////////////////////////////////////////////

	m_boInited		= false;

	m_hBreak		= 0;

	m_hWaitBusy		= 0;
	m_hConnectBusy	= 0;

	m_hConnectNew	= 0;

	m_hSend			= 0;
	m_hRecv			= 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

C_IPC_CONNECTION * C_IPC_SERVER::WaitForConnection( DWORD dwTimeout, SECURITY_ATTRIBUTES * pSA )
{
	HANDLE hObjects[ 2 ] = { m_hWaitBusy, m_hBreak };

	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, 0 ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_OBJECT_0 + 1:
			return 0;
		default:
			ReleaseSemaphore( m_hWaitBusy, 1, 0 );	return 0;
	}

	hObjects[ 0 ] = m_hConnectNew;
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, dwTimeout ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			ReleaseSemaphore( m_hWaitBusy, 1, 0 );
			return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
		default:
			ReleaseSemaphore( m_hWaitBusy, 1, 0 );
			return 0;
	}

	//////////////////////////////////////////////

	C_IPC_INFO client_ipci;
	C_IPC_INFO ipci;

	if ( Read( (BYTE *)&client_ipci, sizeof( client_ipci ) ) != sizeof( client_ipci ) ||
		 !ipci.Get() )
	{
		ReleaseSemaphore( m_hWaitBusy, 1, 0 );
		return 0;
	}

	C_IPC_CONNECTION *hConnection = new C_IPC_CONNECTION;
	if ( !hConnection )
	{
		ReleaseSemaphore( m_hWaitBusy, 1, 0 );
		return 0;
	}

	if ( !hConnection->Create( ipci.m_szGUID, client_ipci.m_szLinkName, pSA ) )
	{
		delete hConnection;
		ReleaseSemaphore( m_hWaitBusy, 1, 0 );
		return 0;
	}

	if ( Write( (BYTE *)&ipci, sizeof( ipci ) ) != sizeof( ipci ) ||
		!ReleaseSemaphore( m_hSend, 1, 0 ) )
	{
		hConnection->Close();
		delete hConnection;
		ReleaseSemaphore( m_hWaitBusy, 1, 0 );
		return 0;
	}

	//////////////////////////////////////////////

	hObjects[ 0 ] = m_hRecv;
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, dwTimeout ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			hConnection->Close();
			delete hConnection;
			ReleaseSemaphore( m_hWaitBusy, 1, 0 );
			return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
		default:
			hConnection->Close();
			delete hConnection;
			ReleaseSemaphore( m_hWaitBusy, 1, 0 );
			return 0;
	}

	//////////////////////////////////////////////

	ReleaseSemaphore( m_hWaitBusy, 1, 0 );
	return hConnection;
}

//////////////////////////////////////////////////////////////////////////////

C_IPC_CONNECTION * C_IPC_SERVER::Connect( char * pszServerName, DWORD dwTimeout, SECURITY_ATTRIBUTES * pSA )
{
	if ( pszServerName == 0 || *pszServerName == 0 ) return 0;

	//////////////////////////////////////////////

	DWORD dwTime1, dwTime2;
	if ( dwTimeout != INFINITE )	dwTime1 = GetTickCount();

	while ( !Open( pszServerName ) )
	{
		Sleep( 300 );
		if ( dwTimeout != INFINITE )
			if ( DATETIME_GetTicksOdds( dwTime1, GetTickCount() ) >= dwTimeout )
				return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
	}

	if ( dwTimeout != INFINITE )
	{
		dwTime2 = GetTickCount();
		if ( DATETIME_GetTicksOdds( dwTime1, dwTime2 ) >= dwTimeout )
		{
			Close();
			return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
		}
		else
			dwTimeout -= DATETIME_GetTicksOdds( dwTime1, dwTime2 );
	}

	//////////////////////////////////////////////

	HANDLE hObjects[ 2 ] = { m_hConnectBusy, m_hBreak };
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, dwTimeout ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			Close();
			return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
		default:
			ReleaseSemaphore( m_hConnectBusy, 1, 0 );
			Close();
			return 0;
	}

	if ( dwTimeout != INFINITE )
	{
		dwTime2 = GetTickCount();
		if ( DATETIME_GetTicksOdds( dwTime1, dwTime2 ) >= dwTimeout )
		{
			ReleaseSemaphore( m_hConnectBusy, 1, 0 );
			Close();
			return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
		}
		else
			dwTimeout -= DATETIME_GetTicksOdds( dwTime1, dwTime2 );
	}

	//////////////////////////////////////////////

	C_IPC_INFO ipci;
	if ( !ipci.Get() ||
		 Write( (BYTE *)&ipci, sizeof( ipci ) ) != sizeof( ipci ) ||
		 !ReleaseSemaphore( m_hConnectNew, 1, 0 ) )
	{
		ReleaseSemaphore( m_hConnectBusy, 1, 0 );
		Close();
		return 0;
	}

	hObjects[ 0 ] = m_hSend;
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, dwTimeout ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			ReleaseSemaphore( m_hConnectBusy, 1, 0 );
			Close();
			return (C_IPC_CONNECTION *)IPC_RC_TIMEOUT;
		default:
			ReleaseSemaphore( m_hConnectBusy, 1, 0 );
			Close();
			return 0;
	}

	//////////////////////////////////////////////

	C_IPC_CONNECTION *hConnection = new C_IPC_CONNECTION;
	if ( !hConnection )
	{
		ReleaseSemaphore( m_hConnectBusy, 1, 0 );
		Close();
		return 0;
	}

	C_IPC_INFO server_ipci;
	if ( Read( (BYTE *)&server_ipci, sizeof( server_ipci ) ) != sizeof( server_ipci ) ||
		!hConnection->Open( server_ipci.m_szGUID, server_ipci.m_szLinkName, pSA ) )
	{
		delete hConnection;
		ReleaseSemaphore( m_hConnectBusy, 1, 0 );
		Close();
		return 0;
	}

	if ( !ReleaseSemaphore( m_hRecv, 1, 0 ) )
	{
		hConnection->Close();
		delete hConnection;
		ReleaseSemaphore( m_hConnectBusy, 1, 0 );
		Close();
		return 0;
	}

	//////////////////////////////////////////////

	ReleaseSemaphore( m_hConnectBusy, 1, 0 );
	Close();
	return hConnection;
}

// eof ///////////////////////////////////////////////////////////////////////