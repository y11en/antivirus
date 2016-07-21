// ipc32.h ///////////////////////////////////////////////////////////////////

// Interprocess Communication (IPC)
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC32_H
#define IPC32_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "ipc_err.h"
#include "sa.h"
#include "HandlesTable.h"
#include "CommonHeader.h"
#include "version.h"
#include "ipc_def.h"
#include "IPCServer.h"
#include "IPCConnection.h"

//////////////////////////////////////////////////////////////////////////////

class C_IPC32
{
public:
	C_IPC32();
	~C_IPC32();
	bool IsValid() const;

	DWORD GetVersion() const;

	HIPCSERVER ServerStart( char * pszServerName ) const;
	BOOL ServerStop( HIPCSERVER hServer ) const;

	HIPCCONNECTION ServerWaitForConnection( HIPCSERVER hServer, DWORD dwTimeout ) const;
	HIPCCONNECTION Connect( char * pszServerName, DWORD dwTimeout ) const;
	BOOL CloseConnection( HIPCCONNECTION hConnection ) const;

	DWORD Recv( HIPCCONNECTION hConnection, void * pvBuf, DWORD dwBufSizeB, DWORD dwTimeout ) const;
	DWORD Send( HIPCCONNECTION hConnection, void * pvBuf, DWORD dwBufSizeB, DWORD dwTimeout ) const;

	BOOL SetUserEvent( HIPCCONNECTION hConnection, HANDLE * pUserEvent ) const;
	BOOL GetUserEvent( HIPCCONNECTION hConnection, DWORD * pdwUserEvent ) const;
	BOOL ResetUserEvent( HIPCCONNECTION hConnection ) const;

	DWORD GetConnectionLastErr( HIPCCONNECTION hConnection ) const;

private:
	bool				m_boValid;

	HSA					m_hSA;
	SECURITY_ATTRIBUTES	*m_pSA;
	DWORD				m_dwDesiredAccess;

	C_HANDLES_TABLE		*m_pServerHandles;
	C_HANDLES_TABLE		*m_pConnectionHandles;

	char				m_szLinkName[ _MAX_PATH ];
	HANDLE				m_hLink;

	static const DWORD	MAX_SERVERS;
	static const DWORD	MAX_CONNECTIONS;
};

//////////////////////////////////////////////////////////////////////////////

inline C_IPC32::C_IPC32()
{
	m_boValid = false;

	//////////////////////////////////////////////

	OSVERSIONINFO Info;
	Info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	if ( ! GetVersionEx( &Info ) ) return;
	if ( Info.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		m_hSA				= SA_Create( SECURITY_WORLD_RID, SA_ACCESS_MASK_ALL );
		if ( m_hSA == 0 ) return;
		m_pSA				= SA_Get( m_hSA );
		m_dwDesiredAccess	= SYNCHRONIZE;
	}
	else
	{
		m_hSA				= 0;
		m_pSA				= 0;
		m_dwDesiredAccess	= PROCESS_ALL_ACCESS;
	}

	//////////////////////////////////////////////

	m_pServerHandles = new C_HANDLES_TABLE( MAX_SERVERS );
	if ( ! m_pServerHandles )
	{
		SA_Destroy( m_hSA );
		return;
	}
	if ( ! m_pServerHandles->IsValid() )
	{
		delete m_pServerHandles;
		SA_Destroy( m_hSA );
		return;
	}

	//////////////////////////////////////////////

	m_pConnectionHandles = new C_HANDLES_TABLE( MAX_CONNECTIONS );
	if ( ! m_pConnectionHandles )
	{
		delete m_pServerHandles;
		SA_Destroy( m_hSA );
		return;
	}
	if ( ! m_pConnectionHandles->IsValid() )
	{
		delete m_pConnectionHandles;
		delete m_pServerHandles;
		SA_Destroy( m_hSA );
		return;
	}

	//////////////////////////////////////////////

	strcpy( m_szLinkName, C_IPC_BUFFER::FrefixName() );
	sprintf( m_szLinkName + strlen( m_szLinkName ),"%#010x", ::GetCurrentProcessId() );
	m_hLink = ::CreateMutex( m_pSA, TRUE, m_szLinkName );
	if ( ! m_hLink || ::GetLastError() == ERROR_ALREADY_EXISTS )
	{
		if ( m_hLink ) { CloseHandle( m_hLink ); m_hLink = 0; }
		delete m_pConnectionHandles;
		delete m_pServerHandles;
		SA_Destroy( m_hSA );
		return;
	}

	//////////////////////////////////////////////

	m_boValid = true;
}

//////////////////////////////////////////////////////////////////////////////

inline C_IPC32::~C_IPC32()
{
	if ( ! m_boValid ) return;

	DWORD i;
	for ( i = 1; i <= MAX_SERVERS; ++i ) ServerStop( i );
	for ( i = 1; i <= MAX_CONNECTIONS; ++i ) CloseConnection( i );

	CloseHandle( m_hLink );
	delete m_pConnectionHandles;
	delete m_pServerHandles;
	SA_Destroy( m_hSA );
}

//////////////////////////////////////////////////////////////////////////////

inline bool C_IPC32::IsValid() const
{
	return m_boValid;
};

//////////////////////////////////////////////////////////////////////////////

inline DWORD C_IPC32::GetVersion() const
{
	return chPackBytes( MODULE_VERSION_A, MODULE_VERSION_B, MODULE_VERSION_C, MODULE_VERSION_D );
}

//////////////////////////////////////////////////////////////////////////////

inline HIPCSERVER C_IPC32::ServerStart( char * pszServerName ) const
{
	HANDLES_TABLE_ID id = m_pServerHandles->Reserve();
	if ( ! id ) return 0;

	C_IPC_SERVER * h = new C_IPC_SERVER();
	if ( ! h )
	{
		m_pServerHandles->UnReserve( id );
		return 0;
	}

	if ( ! h->Create( pszServerName, m_pSA ) )
	{
		delete h;
		m_pServerHandles->UnReserve( id );
		return 0;
	}

	if ( ! m_pServerHandles->UnReserveAndAdd( id, (HANDLES_TABLE_HANDLE)h ) )
	{
		h->Close();
		delete h ;
		m_pServerHandles->UnReserve( id );
		return 0;
	}

	return id;
}

//////////////////////////////////////////////////////////////////////////////

inline BOOL C_IPC32::ServerStop( HIPCSERVER hServer ) const
{
	C_IPC_SERVER * h = (C_IPC_SERVER *)m_pServerHandles->Remove( hServer );
	if ( ! h || ! h->Close() ) return FALSE;

	delete h;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

inline HIPCCONNECTION C_IPC32::ServerWaitForConnection( HIPCSERVER hServer, DWORD dwTimeout ) const
{
	C_IPC_SERVER * h = (C_IPC_SERVER *)m_pServerHandles->Get( hServer );
	if ( ! h ) return 0;

	HANDLES_TABLE_ID id = m_pConnectionHandles->Reserve();
	if ( ! id ) return 0;

	C_IPC_CONNECTION * hConnection = h->WaitForConnection( dwTimeout, m_pSA );
	switch( (DWORD)hConnection )
	{
		case 0:
		case IPC_RC_TIMEOUT:
			m_pConnectionHandles->UnReserve( id );
			return (HIPCCONNECTION)hConnection;
		default:
			if ( ! m_pConnectionHandles->UnReserveAndAdd( id, (HANDLES_TABLE_HANDLE)hConnection ) )
			{
				m_pConnectionHandles->UnReserve( id );
				hConnection->Close();
				return 0;
			}
			return id;
	}
}

//////////////////////////////////////////////////////////////////////////////

inline HIPCCONNECTION C_IPC32::Connect( char * pszServerName, DWORD dwTimeout ) const
{
	HANDLES_TABLE_ID id = m_pConnectionHandles->Reserve();
	if ( ! id ) return 0;

	C_IPC_SERVER h;
	C_IPC_CONNECTION * hConnection = h.Connect( pszServerName, dwTimeout, m_pSA );
	switch( (DWORD)hConnection )
	{
		case 0:
		case IPC_RC_TIMEOUT:
			m_pConnectionHandles->UnReserve( id );
			return (HIPCCONNECTION)hConnection;
		default:
			if ( ! m_pConnectionHandles->UnReserveAndAdd( id, (HANDLES_TABLE_HANDLE)hConnection ) )
			{
				m_pConnectionHandles->UnReserve( id );
				hConnection->Close();
				return 0;
			}
			return id;
	}
}

//////////////////////////////////////////////////////////////////////////////

inline BOOL C_IPC32::CloseConnection( HIPCCONNECTION hConnection ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Remove( hConnection );
	if ( ! h || ! h->Close() ) return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

inline DWORD C_IPC32::Recv( HIPCCONNECTION	hConnection, void * pvBuf, DWORD dwBufSizeB, DWORD dwTimeout ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Get( hConnection );
	if ( ! h ) return IPC_RC_ERROR;
	return h->Recv( (BYTE *)pvBuf, dwBufSizeB, dwTimeout );
}

//////////////////////////////////////////////////////////////////////////////

inline DWORD C_IPC32::Send( HIPCCONNECTION hConnection, void * pvBuf, DWORD dwBufSizeB, DWORD dwTimeout ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Get( hConnection );
	if ( ! h ) return IPC_RC_ERROR;
	return h->Send( (BYTE *)pvBuf, dwBufSizeB, dwTimeout );
}

//////////////////////////////////////////////////////////////////////////////

inline BOOL C_IPC32::SetUserEvent( HIPCCONNECTION hConnection, HANDLE * pUserEvent ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Get( hConnection );
	if ( ! h || ! h->SetUserEvent( pUserEvent ) ) return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

inline BOOL C_IPC32::GetUserEvent( HIPCCONNECTION hConnection, DWORD * pdwUserEvent ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Get( hConnection );
	if ( ! h || ! h->GetUserEventRes( pdwUserEvent ) ) return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

inline BOOL C_IPC32::ResetUserEvent( HIPCCONNECTION hConnection ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Get( hConnection );
	if ( ! h || !h->ResetUserEvent() ) return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

inline DWORD C_IPC32::GetConnectionLastErr( HIPCCONNECTION hConnection ) const
{
	C_IPC_CONNECTION * h = (C_IPC_CONNECTION *)m_pConnectionHandles->Get( hConnection );
	if ( ! h ) return IPC_RC_ERROR;
	return h->GetLastErr();
}

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC32_H

// eof ///////////////////////////////////////////////////////////////////////