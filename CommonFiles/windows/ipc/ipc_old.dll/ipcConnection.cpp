// ipcConnection.cpp /////////////////////////////////////////////////////////

// Interprocess Communication Connection
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "IPCConnection.h"
#include "ipc_def.h"
#include "ipc_err.h"
#include "DateTime.h"

//////////////////////////////////////////////////////////////////////////////

const char *const C_IPC_CONNECTION::IPC_ADD_NAME_MAPFILE_BUFFER		= "__CONNECTION_MAPFILE_BUFFER";

const char *const C_IPC_CONNECTION::IPC_ADD_NAME_BREAK				= "__CONNECTION_BREAK";

const char *const C_IPC_CONNECTION::IPC_ADD_NAME_SERVER_SEND_BUSY	= "__CONNECTION_SERVER_SEND_BUSY";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_SERVER_SEND_NEW	= "__CONNECTION_SERVER_SEND_NEW";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_SERVER_SEND		= "__CONNECTION_SERVER_SEND";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_SERVER_RECV_BUSY	= "__CONNECTION_SERVER_RECV_BUSY";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_SERVER_RECV		= "__CONNECTION_SERVER_RECV";

const char *const C_IPC_CONNECTION::IPC_ADD_NAME_CLIENT_SEND_BUSY	= "__CONNECTION_CLIENT_SEND_BUSY";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_CLIENT_SEND_NEW	= "__CONNECTION_CLIENT_SEND_NEW";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_CLIENT_SEND		= "__CONNECTION_CLIENT_SEND";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_CLIENT_RECV_BUSY	= "__CONNECTION_CLIENT_RECV_BUSY";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_CLIENT_RECV		= "__CONNECTION_CLIENT_RECV";

const char *const C_IPC_CONNECTION::IPC_ADD_NAME_SERVER_LINK		= "__CONNECTION_SERVER_LINK";
const char *const C_IPC_CONNECTION::IPC_ADD_NAME_CLIENT_LINK		= "__CONNECTION_CLIENT_LINK";

//////////////////////////////////////////////////

const DWORD C_IPC_CONNECTION::IPC_MAX_SENR_AND_RECV_BUF_SIZE		= ( 512 * 1024 * 1024 ); // 512 Mb

//////////////////////////////////////////////////////////////////////////////

C_IPC_CONNECTION::C_IPC_CONNECTION()
{
	m_boInited			= false;

	*m_szConnectionName	= 0;

	m_hBreak			= 0;

	m_hServerSendBusy	= 0;
	m_hServerSendNew	= 0;
	m_hServerSend		= 0;
	m_hServerRecvBusy	= 0;
	m_hServerRecv		= 0;

	m_hClientSendBusy	= 0;
	m_hClientSendNew	= 0;
	m_hClientSend		= 0;
	m_hClientRecvBusy	= 0;
	m_hClientRecv		= 0;

	m_hUserEventBusy	= 0;
	m_hUserEvent		= 0;
	m_dwUserEventRes	= 0xffffffff;

	m_hLink				= 0;

	InitializeCriticalSection( &m_csLastError );
	m_dwLastError		= IPC_ERR_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////

C_IPC_CONNECTION::~C_IPC_CONNECTION()
{
	DeleteCriticalSection( &m_csLastError );
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::Create( char *pszConnectionName, char *pszLinkName, SECURITY_ATTRIBUTES *pSA )
{
	if ( m_boInited || !IsValidConnectionName( pszConnectionName ) ) return false;
	char szFirstPartOfName[ _MAX_PATH ];
	strcpy( szFirstPartOfName, FrefixName() ); strcat( szFirstPartOfName, pszConnectionName );
	char szName[ _MAX_PATH ];

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_MAPFILE_BUFFER );
	if( !C_IPC_BUFFER::Create( szName, pSA ) ) return false;

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_BREAK );
	HANDLE hBreak = CreateEvent( pSA, TRUE, FALSE, szName ); // manually, nonsignaled
	if( !hBreak || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hBreak ) CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_SEND_BUSY );
	HANDLE hServerSendBusy = CreateSemaphore( pSA, 1, 1, szName );
	if( !hServerSendBusy || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hServerSendBusy ) CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_SEND_NEW );
	HANDLE hServerSendNew = CreateSemaphore( pSA, 0, 1, szName );
	if( !hServerSendNew || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hServerSendNew ) CloseHandle( hServerSendNew );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}
	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_SEND );
	HANDLE hServerSend = CreateSemaphore( pSA, 0, 1, szName );
	if( !hServerSend || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hServerSend ) CloseHandle( hServerSend );

		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_RECV_BUSY );
	HANDLE hServerRecvBusy = CreateSemaphore( pSA, 1, 1, szName );
	if( !hServerRecvBusy || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hServerRecvBusy ) CloseHandle( hServerRecvBusy );

		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_RECV );
	HANDLE hServerRecv = CreateSemaphore( pSA, 0, 1, szName );
	if( !hServerRecv || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hServerRecv ) CloseHandle( hServerRecv );

		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_SEND_BUSY );
	HANDLE hClientSendBusy = CreateSemaphore( pSA, 1, 1, szName );
	if ( !hClientSendBusy || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hClientSendBusy ) CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_SEND_NEW );
	HANDLE hClientSendNew = CreateSemaphore( pSA, 0, 1, szName );
	if( !hClientSendNew || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hClientSendNew ) CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_SEND );
	HANDLE hClientSend = CreateSemaphore( pSA, 0, 1, szName );
	if( !hClientSend || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hClientSend ) CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_RECV_BUSY );
	HANDLE hClientRecvBusy = CreateSemaphore( pSA, 1, 1, szName );
	if( !hClientRecvBusy || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hClientRecvBusy ) CloseHandle( hClientRecvBusy );
		CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_RECV );
	HANDLE hClientRecv = CreateSemaphore( pSA, 0, 1, szName );
	if( !hClientRecv || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if( hClientRecv ) CloseHandle( hClientRecv );
		CloseHandle( hClientRecvBusy );
		CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	HANDLE hUserEventBusy = CreateSemaphore( 0, 1, 1, 0 );
	if ( !hUserEventBusy )
	{
		CloseHandle( hClientRecv );
		CloseHandle( hClientRecvBusy );
		CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	HANDLE hLink = ::OpenMutex( MUTEX_ALL_ACCESS, FALSE, pszLinkName );
	if ( !hLink )
	{
		if ( hLink ) ::CloseHandle( hLink );

		::CloseHandle( hUserEventBusy );

		::CloseHandle( hClientRecv );
		::CloseHandle( hClientRecvBusy );
		::CloseHandle( hClientSend );
		::CloseHandle( hClientSendNew );
		::CloseHandle( hClientSendBusy );

		::CloseHandle( hServerRecv );
		::CloseHandle( hServerRecvBusy );
		::CloseHandle( hServerSend );
		::CloseHandle( hServerSendNew );
		::CloseHandle( hServerSendBusy );

		::CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	
	}

	//////////////////////////////////////////////

			m_boInited			=	true;

	strcpy(	m_szConnectionName,		pszConnectionName );

			m_hBreak			=	hBreak;

			m_hServerSendBusy	=	hServerSendBusy;
			m_hServerSendNew	=	hServerSendNew;
			m_hServerSend		=	hServerSend;
			m_hServerRecvBusy	=	hServerRecvBusy;
			m_hServerRecv		=	hServerRecv;

			m_hClientSendBusy	=	hClientSendBusy;
			m_hClientSendNew	=	hClientSendNew;
			m_hClientSend		=	hClientSend;
			m_hClientRecvBusy	=	hClientRecvBusy;
			m_hClientRecv		=	hClientRecv;

			m_hUserEventBusy	=	hUserEventBusy;

			m_hLink				=	hLink;

	//////////////////////////////////////////////

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::Open( char *pszConnectionName, char *pszLinkName, SECURITY_ATTRIBUTES *pSA )
{
	if ( m_boInited || !IsValidConnectionName( pszConnectionName ) ) return false;
	char szFirstPartOfName[ _MAX_PATH ];
	::strcpy( szFirstPartOfName, FrefixName() ); ::strcat( szFirstPartOfName, pszConnectionName );
	char szName[ _MAX_PATH ];

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_MAPFILE_BUFFER );
	if ( !C_IPC_BUFFER::Open( szName ) ) return false;

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_BREAK );
	HANDLE hBreak = OpenEvent( EVENT_ALL_ACCESS, FALSE, szName );
	if ( !hBreak )
	{
		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_SEND_BUSY );
	HANDLE hServerSendBusy = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hServerSendBusy )
	{
		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_SEND_NEW );
	HANDLE hServerSendNew = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hServerSendNew )
	{
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_SEND );
	HANDLE hServerSend = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hServerSend )
	{
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_RECV_BUSY );
	HANDLE hServerRecvBusy = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hServerRecvBusy )
	{
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_SERVER_RECV );
	HANDLE hServerRecv = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hServerRecv )
	{
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_SEND_BUSY );
	HANDLE hClientSendBusy = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hClientSendBusy )
	{
		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_SEND_NEW );
	HANDLE hClientSendNew = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hClientSendNew )
	{
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_SEND );
	HANDLE hClientSend = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hClientSend )
	{
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_RECV_BUSY );
	HANDLE hClientRecvBusy = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hClientRecvBusy )
	{
		CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	strcpy( szName, szFirstPartOfName ); strcat( szName, IPC_ADD_NAME_CLIENT_RECV );
	HANDLE hClientRecv = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, szName );
	if( !hClientRecv )
	{
		CloseHandle( hClientRecvBusy );
		CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	HANDLE hUserEventBusy = CreateSemaphore( 0, 1, 1, 0 );
	if( !hUserEventBusy )
	{
		CloseHandle( hClientRecv );
		CloseHandle( hClientRecvBusy );
		CloseHandle( hClientSend );
		CloseHandle( hClientSendNew );
		CloseHandle( hClientSendBusy );

		CloseHandle( hServerRecv );
		CloseHandle( hServerRecvBusy );
		CloseHandle( hServerSend );
		CloseHandle( hServerSendNew );
		CloseHandle( hServerSendBusy );

		CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

	HANDLE hLink = ::OpenMutex( MUTEX_ALL_ACCESS, FALSE, pszLinkName );
	if ( !hLink )
	{
		::CloseHandle( hUserEventBusy );

		::CloseHandle( hClientRecv );
		::CloseHandle( hClientRecvBusy );
		::CloseHandle( hClientSend );
		::CloseHandle( hClientSendNew );
		::CloseHandle( hClientSendBusy );

		::CloseHandle( hServerRecv );
		::CloseHandle( hServerRecvBusy );
		::CloseHandle( hServerSend );
		::CloseHandle( hServerSendNew );
		::CloseHandle( hServerSendBusy );

		::CloseHandle( hBreak );

		C_IPC_BUFFER::Close();
		return false;
	}

	//////////////////////////////////////////////

			m_boInited			=	true;

	strcpy(	m_szConnectionName,		pszConnectionName );

			m_hBreak			=	hBreak;

			m_hServerSendBusy	=	hServerSendBusy;
			m_hServerSendNew	=	hServerSendNew;
			m_hServerSend		=	hServerSend;
			m_hServerRecvBusy	=	hServerRecvBusy;
			m_hServerRecv		=	hServerRecv;

			m_hClientSendBusy	=	hClientSendBusy;
			m_hClientSendNew	=	hClientSendNew;
			m_hClientSend		=	hClientSend;
			m_hClientRecvBusy	=	hClientRecvBusy;
			m_hClientRecv		=	hClientRecv;

			m_hUserEventBusy	=	hUserEventBusy;

			m_hLink				=	hLink;

	//////////////////////////////////////////////

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::Close()
{
	if ( ! m_boInited || ! ::SetEvent( m_hBreak ) ) return false;

	HANDLE hObjects[ 3 ];
	switch ( m_enSide )
	{
		case IPC_SIDE_SERVER:
			hObjects[ 0 ] = m_hServerSendBusy;
			hObjects[ 1 ] = m_hClientRecvBusy;
			break;
		case IPC_SIDE_CLIENT:
			hObjects[ 0 ] = m_hClientSendBusy;
			hObjects[ 1 ] = m_hServerRecvBusy;
			break;
		default:
			return false;
	}
	hObjects[ 2 ] = m_hUserEventBusy;

	//////////////////////////////////////////////

	if ( ::WaitForMultipleObjects( 3, hObjects, TRUE, INFINITE ) != WAIT_OBJECT_0 ||

		 !::CloseHandle( m_hLink ) ||

		 !::CloseHandle( m_hUserEventBusy ) ||
		 !::CloseHandle( m_hBreak ) ||

		 !::CloseHandle( m_hServerSendBusy ) ||
		 !::CloseHandle( m_hServerSendNew ) ||
		 !::CloseHandle( m_hServerSend ) ||
		 !::CloseHandle( m_hServerRecvBusy ) ||
		 !::CloseHandle( m_hServerRecv ) ||

		 !::CloseHandle( m_hClientSendBusy ) ||
		 !::CloseHandle( m_hClientSendNew ) ||
		 !::CloseHandle( m_hClientSend ) ||
		 !::CloseHandle( m_hClientRecvBusy ) ||
		 !::CloseHandle( m_hClientRecv ) ||

		 !C_IPC_BUFFER::Close()
	)
		return false;

	//////////////////////////////////////////////

	m_boInited			= false;

	*m_szConnectionName	= 0;

	m_hBreak			= 0;

	m_hServerSendBusy	= 0;
	m_hServerSendNew	= 0;
	m_hServerSend		= 0;
	m_hServerRecvBusy	= 0;
	m_hServerRecv		= 0;

	m_hClientSendBusy	= 0;
	m_hClientSendNew	= 0;
	m_hClientSend		= 0;
	m_hClientRecvBusy	= 0;
	m_hClientRecv		= 0;

	m_hUserEventBusy	= 0;
	m_hUserEvent		= 0;
	m_dwUserEventRes	= 0xffffffff;

	m_hLink				= 0;

	//////////////////////////////////////////////

	return true;
}

//////////////////////////////////////////////////////////////////////////////

DWORD C_IPC_CONNECTION::Recv( BYTE *pbyBuf, DWORD dwBufSize, DWORD dwTimeout )
{
	if ( pbyBuf == 0 || dwBufSize > IPC_MAX_SENR_AND_RECV_BUF_SIZE )
	{	SetLastErr( IPC_ERR_INVALID_ARG );
		return IPC_RC_ERROR;
	}

	//////////////////////////////////////////////

	HANDLE	hRecvBusy;
	HANDLE	hSendNew;
	HANDLE	hRecv;
	HANDLE	hSend;

	switch ( m_enSide )
	{	case IPC_SIDE_SERVER:
			hRecvBusy	= m_hClientRecvBusy;
			hSendNew	= m_hClientSendNew;
			hRecv		= m_hClientRecv;
			hSend		= m_hClientSend;
			break;
		case IPC_SIDE_CLIENT:
			hRecvBusy	= m_hServerRecvBusy;
			hSendNew	= m_hServerSendNew;
			hRecv		= m_hServerRecv;
			hSend		= m_hServerSend;
			break;
		default:
			SetLastErr( IPC_ERR_UNKNOWN );
			return IPC_RC_ERROR;
	}

	//////////////////////////////////////////////

	HANDLE hObjects[ 4 ] = { hRecvBusy, m_hBreak, m_hLink };
	hObjects[ 3 ] = GetUserEventHandle();
	DWORD dwObjectsCount;
	if ( hObjects[ 3 ] ) dwObjectsCount = 4; else dwObjectsCount = 3;

	//////////////////////////////////////////////

	DWORD dwTime1, dwTime2;
	if ( dwTimeout != INFINITE ) dwTime1 = GetTickCount();

	switch ( WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, dwTimeout ) )
	{	case WAIT_OBJECT_0:			break;
		case WAIT_OBJECT_0 + 1:		SetLastErr( IPC_ERR_CLOSED ); return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 2:		SetLastErr( IPC_ERR_BROKEN ); ReleaseMutex( m_hLink ); return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 3:		SetLastErr( IPC_ERR_USER_EVENT_SET ); SetUserEventRes( WAIT_OBJECT_0 ); return IPC_RC_ERROR;
		case WAIT_ABANDONED_0 + 2:	SetLastErr( IPC_ERR_BROKEN ); return IPC_RC_ERROR;
		case WAIT_TIMEOUT:			SetLastErr( IPC_ERR_TIMEOUT ); return IPC_RC_TIMEOUT;
		default:					SetLastErr( IPC_ERR_UNKNOWN ); return IPC_RC_ERROR;
	}

	if ( dwTimeout != INFINITE )
	{	dwTime2 = GetTickCount();
		if ( DATETIME_GetTicksOdds( dwTime1, dwTime2 ) >= dwTimeout )
		{
			SetLastErr( IPC_ERR_TIMEOUT );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_TIMEOUT;
		}
		else dwTimeout -= DATETIME_GetTicksOdds( dwTime1, dwTime2 );
	}

	//////////////////////////////////////////////

	hObjects[ 0 ] = hSendNew;
	switch ( WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, dwTimeout ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_OBJECT_0 + 1:
			SetLastErr( IPC_ERR_CLOSED );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 2:
			SetLastErr( IPC_ERR_BROKEN );
			ReleaseMutex( m_hLink );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 3:
			SetLastErr( IPC_ERR_USER_EVENT_SET );
			SetUserEventRes( WAIT_OBJECT_0 );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_ABANDONED_0 + 2:
			SetLastErr( IPC_ERR_BROKEN );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_TIMEOUT:
			SetLastErr( IPC_ERR_TIMEOUT );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_TIMEOUT;
		default:
			SetLastErr( IPC_ERR_UNKNOWN );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_ERROR;
	}

	DWORD dwRecvSize;
	if ( Read( (BYTE *)&dwRecvSize, sizeof( dwRecvSize ) ) != sizeof( DWORD ) ||
		 dwRecvSize > dwBufSize ||
		 ! ReleaseSemaphore( hRecv, 1, 0 ) )
	{
		SetLastErr( IPC_ERR_UNKNOWN );
		ReleaseSemaphore( hRecvBusy, 1, 0 );
		return IPC_RC_ERROR;
	}
	DWORD dwBufCount = dwRecvSize / IPC_BUF_SIZE;
	DWORD dwBufPart  = dwRecvSize % IPC_BUF_SIZE;
	DWORD dwBytes    = 0;

	//////////////////////////////////////////////

	DWORD dwRes;

	hObjects[ 0 ] = hSend;
	if ( dwBufCount )
		for ( DWORD i = 1; i <= dwBufCount; ++i )
			if ( ( dwRes = WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, INFINITE ) ) != WAIT_OBJECT_0 ||
				 Read( pbyBuf, IPC_BUF_SIZE ) != IPC_BUF_SIZE ||
				 ! ReleaseSemaphore( hRecv, 1, 0 ) )
			{
				if ( dwRes == WAIT_OBJECT_0 + 2 ) ReleaseMutex( m_hLink );
				else if ( dwRes == WAIT_OBJECT_0 + 3 ) SetUserEventRes( WAIT_OBJECT_0 );

				SetLastErr( IPC_ERR_UNKNOWN );
				ReleaseSemaphore( hRecvBusy, 1, 0 );
				return IPC_RC_ERROR;
			}
			else
			{	dwBytes	+= IPC_BUF_SIZE;
				pbyBuf	+= IPC_BUF_SIZE;
			}

	if ( dwBufPart )
		if ( ( dwRes = WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, INFINITE ) ) != WAIT_OBJECT_0 ||
			 Read( pbyBuf, dwBufPart ) != dwBufPart ||
			 ! ReleaseSemaphore( hRecv, 1, 0 ) )
		{
			if ( dwRes == WAIT_OBJECT_0 + 2 ) ReleaseMutex( m_hLink );
			else if ( dwRes == WAIT_OBJECT_0 + 3 ) SetUserEventRes( WAIT_OBJECT_0 );

			SetLastErr( IPC_ERR_UNKNOWN );
			ReleaseSemaphore( hRecvBusy, 1, 0 );
			return IPC_RC_ERROR;
		}
		else
			dwBytes += dwBufPart;

	//////////////////////////////////////////////

	SetLastErr( IPC_ERR_NO_ERROR );
	ReleaseSemaphore( hRecvBusy, 1, 0 );
	return dwBytes;
}

//////////////////////////////////////////////////////////////////////////////

DWORD C_IPC_CONNECTION::Send( BYTE *pbyBuf, DWORD dwBufSize, DWORD dwTimeout )
{
	if ( pbyBuf == 0 || dwBufSize > IPC_MAX_SENR_AND_RECV_BUF_SIZE )
	{	SetLastErr( IPC_ERR_INVALID_ARG );
		return IPC_RC_ERROR;
	}

	//////////////////////////////////////////////	

	HANDLE	hSendBusy;
	HANDLE	hSendNew;
	HANDLE	hSend;
	HANDLE	hRecv;

	switch ( m_enSide )
	{	case IPC_SIDE_SERVER:
			hSendBusy	= m_hServerSendBusy;
			hSendNew	= m_hServerSendNew;
			hSend		= m_hServerSend;
			hRecv		= m_hServerRecv;
			break;
		case IPC_SIDE_CLIENT:
			hSendBusy	= m_hClientSendBusy;
			hSendNew	= m_hClientSendNew;
			hSend		= m_hClientSend;
			hRecv		= m_hClientRecv;
			break;
		default:
			SetLastErr( IPC_ERR_UNKNOWN );
			return IPC_RC_ERROR;
	}

	//////////////////////////////////////////////

	HANDLE hObjects[ 4 ] = { hSendBusy, m_hBreak, m_hLink };
	hObjects[ 3 ] = GetUserEventHandle();
	DWORD dwObjectsCount;
	if ( hObjects[ 3 ] ) dwObjectsCount = 4; else dwObjectsCount = 3;

	//////////////////////////////////////////////

	DWORD dwTime1, dwTime2;
	if ( dwTimeout != INFINITE ) dwTime1 = GetTickCount();

	switch ( WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, dwTimeout ) )
	{	case WAIT_OBJECT_0:			break;
		case WAIT_OBJECT_0 + 1:		SetLastErr( IPC_ERR_CLOSED ); return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 2:		SetLastErr( IPC_ERR_BROKEN ); ReleaseMutex( m_hLink ); return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 3:		SetLastErr( IPC_ERR_USER_EVENT_SET ); SetUserEventRes( WAIT_OBJECT_0 ); return IPC_RC_ERROR;
		case WAIT_ABANDONED_0 + 2:	SetLastErr( IPC_ERR_BROKEN ); return IPC_RC_ERROR;
		case WAIT_TIMEOUT:			SetLastErr( IPC_ERR_TIMEOUT ); return IPC_RC_TIMEOUT;
		default:					SetLastErr( IPC_ERR_UNKNOWN ); return IPC_RC_ERROR;
	}

	if ( dwTimeout != INFINITE )
	{	dwTime2 = GetTickCount();
		if ( DATETIME_GetTicksOdds( dwTime1, dwTime2 ) >= dwTimeout )
		{	SetLastErr( IPC_ERR_TIMEOUT );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_TIMEOUT;
		}
		else dwTimeout -= DATETIME_GetTicksOdds( dwTime1, dwTime2 );
	}

	//////////////////////////////////////////////

	hObjects[ 0 ] = hRecv;
	if ( Write( (BYTE *)&dwBufSize, sizeof( dwBufSize ) ) != sizeof( DWORD ) ||
		 ! ReleaseSemaphore( hSendNew, 1, 0 ) )
	{	SetLastErr( IPC_ERR_UNKNOWN );
		ReleaseSemaphore( hSendBusy, 1, 0 );
		return IPC_RC_ERROR;
	}

	switch ( WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, dwTimeout ) )
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_OBJECT_0 + 1:
			SetLastErr( IPC_ERR_CLOSED );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 2:
			SetLastErr( IPC_ERR_BROKEN );
			ReleaseMutex( m_hLink );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_OBJECT_0 + 3:
			SetLastErr( IPC_ERR_USER_EVENT_SET );
			SetUserEventRes( WAIT_OBJECT_0 );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_ABANDONED_0 + 2:
			SetLastErr( IPC_ERR_BROKEN );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_ERROR;
		case WAIT_TIMEOUT:
			SetLastErr( IPC_ERR_TIMEOUT );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_TIMEOUT;
		default:
			SetLastErr( IPC_ERR_UNKNOWN );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_ERROR;
	}

	DWORD dwBufCount = dwBufSize / IPC_BUF_SIZE;
	DWORD dwBufPart  = dwBufSize % IPC_BUF_SIZE;
	DWORD dwBytes    = 0;

	//////////////////////////////////////////////

	DWORD dwRes;

	if ( dwBufCount )
		for ( DWORD i = 1; i <= dwBufCount; i++ )
			if ( Write( pbyBuf, IPC_BUF_SIZE ) != IPC_BUF_SIZE ||
				 ! ReleaseSemaphore( hSend, 1, 0 ) || 
				 ( dwRes = WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, INFINITE ) ) != WAIT_OBJECT_0 )
			{
				if ( dwRes == WAIT_OBJECT_0 + 2 ) ReleaseMutex( m_hLink );
				else if ( dwRes == WAIT_OBJECT_0 + 3 ) SetUserEventRes( WAIT_OBJECT_0 );

				SetLastErr( IPC_ERR_UNKNOWN );
				ReleaseSemaphore( hSendBusy, 1, 0 );
				return IPC_RC_ERROR;
			}
			else
			{
				pbyBuf	+= IPC_BUF_SIZE;
				dwBytes	+= IPC_BUF_SIZE;
			}

	if ( dwBufPart )
		if ( Write( pbyBuf, dwBufPart ) != dwBufPart ||
			 ! ReleaseSemaphore( hSend, 1, 0 ) || 
			 ( dwRes = WaitForMultipleObjects( dwObjectsCount, hObjects, FALSE, INFINITE ) ) != WAIT_OBJECT_0 )
		{
			if ( dwRes == WAIT_OBJECT_0 + 2 ) ReleaseMutex( m_hLink );
			else if ( dwRes == WAIT_OBJECT_0 + 3 ) SetUserEventRes( WAIT_OBJECT_0 );

			SetLastErr( IPC_ERR_UNKNOWN );
			ReleaseSemaphore( hSendBusy, 1, 0 );
			return IPC_RC_ERROR;
		}
		else
			dwBytes	+= dwBufPart;

	//////////////////////////////////////////////

	SetLastErr( IPC_ERR_NO_ERROR );
	ReleaseSemaphore( hSendBusy, 1, 0 );
	return dwBytes;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::SetUserEvent( HANDLE *pUserEvent )
{
	if ( !pUserEvent || !*pUserEvent ) return false;

	HANDLE hObjects[ 2 ] = { m_hUserEventBusy, m_hBreak };
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, INFINITE ) )
	{
		case WAIT_OBJECT_0:
			break;
		default:
			ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
			return false;
	}

	m_hUserEvent = *pUserEvent;
	m_dwUserEventRes = WAIT_TIMEOUT;
	ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::GetUserEventRes( DWORD *pdwUserEvent ) const
{
	if ( !pdwUserEvent || !*pdwUserEvent ) return false;

	HANDLE hObjects[ 2 ] = { m_hUserEventBusy, m_hBreak };
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, INFINITE ) )
	{
		case WAIT_OBJECT_0:
			break;
		default:
			ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
			return false;
	}

	*pdwUserEvent = m_dwUserEventRes;
	ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::ResetUserEvent() const
{
	HANDLE hObjects[ 2 ] = { m_hUserEventBusy, m_hBreak };
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, INFINITE ) )
	{
		case WAIT_OBJECT_0:
			break;
		default:
			ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
			return false;
	}

	if ( !ResetEvent( m_hUserEvent ) )
	{
		ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
		return false;
	}

	ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::IsValidConnectionName( char * pszConnectionName ) const
{
	if ( ! pszConnectionName || ! *pszConnectionName ) return false;

	DWORD dwPartOfFullLen = ::strlen( FrefixName() ) + ::strlen( pszConnectionName );

	if ( dwPartOfFullLen + ::strlen( IPC_ADD_NAME_MAPFILE_BUFFER )		>= _MAX_PATH ||

		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_BREAK )				>= _MAX_PATH ||

		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_SERVER_SEND_BUSY )	>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_SERVER_SEND_NEW )		>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_SERVER_SEND )			>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_SERVER_RECV_BUSY )	>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_SERVER_RECV )			>= _MAX_PATH ||

		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_CLIENT_SEND_BUSY )	>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_CLIENT_SEND_NEW )		>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_CLIENT_SEND )			>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_CLIENT_RECV_BUSY )	>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_CLIENT_RECV )			>= _MAX_PATH ||

		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_SERVER_LINK )			>= _MAX_PATH ||
		 dwPartOfFullLen + ::strlen( IPC_ADD_NAME_CLIENT_LINK )			>= _MAX_PATH )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

HANDLE C_IPC_CONNECTION::GetUserEventHandle() const
{
	HANDLE hObjects[ 2 ] = { m_hUserEventBusy, m_hBreak };
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, INFINITE ) )
	{
		case WAIT_OBJECT_0:
			break;
		default:
			ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
			return 0;
	}

	HANDLE h = m_hUserEvent;

	ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
	return h;
}

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_CONNECTION::SetUserEventRes( DWORD dwUserEventRes )
{
	HANDLE hObjects[ 2 ] = { m_hUserEventBusy, m_hBreak };
	switch ( WaitForMultipleObjects( 2, hObjects, FALSE, INFINITE ) )
	{
		case WAIT_OBJECT_0:
			break;
		default:
			ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
			return false;
	}

	m_dwUserEventRes = dwUserEventRes;

	ReleaseSemaphore( m_hUserEventBusy, 1, 0 );
	return true;
}

//////////////////////////////////////////////////////////////////////////////

DWORD C_IPC_CONNECTION::GetLastErr()
{
	EnterCriticalSection( &m_csLastError );
	DWORD dwLastError = m_dwLastError;
	LeaveCriticalSection( &m_csLastError );
	return dwLastError;
}

//////////////////////////////////////////////////////////////////////////////

void C_IPC_CONNECTION::SetLastErr( DWORD dwError )
{
	EnterCriticalSection( &m_csLastError );
	m_dwLastError = dwError;
	LeaveCriticalSection( &m_csLastError );
}

// eof ///////////////////////////////////////////////////////////////////////