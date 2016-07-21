// ipcInfo.cpp ///////////////////////////////////////////////////////////////

// Interprocess Communication Information to Client and Server
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "IPCInfo.h"
#include "IPCBuffer.h"

//////////////////////////////////////////////////////////////////////////////

bool C_IPC_INFO::Get()
{
	if ( GetGUID( m_szGUID, _MAX_PATH ) == 0xffffffff ) return false;

	::strcpy( m_szLinkName, C_IPC_BUFFER::FrefixName() );
	::sprintf( m_szLinkName + ::strlen( m_szLinkName ),"%#010x", ::GetCurrentProcessId() );

	return true;
}

//////////////////////////////////////////////////////////////////////////////

DWORD C_IPC_INFO::GetGUID( char *pszBuf, DWORD dwBufSize )
{
	if ( pszBuf )
	{
		UUID Uuid;
		RPC_STATUS stat = ::UuidCreate( &Uuid );
		if ( stat == RPC_S_OK || stat == RPC_S_UUID_LOCAL_ONLY )
		{
			unsigned char *StringUuid = 0;
			stat = ::UuidToString( &Uuid, &StringUuid );
			if ( stat == RPC_S_OK )
			{
				DWORD dwBufLen = ::strlen( (const char *)StringUuid );
				if ( dwBufLen < dwBufSize )
				{
					::strcpy( pszBuf, (const char *)StringUuid );
					if ( ::RpcStringFree( &StringUuid ) == RPC_S_OK )
						return dwBufLen + 1;
				}
				::RpcStringFree( &StringUuid );
			}
		}
	}
	return 0xffffffff;
}

// eof ///////////////////////////////////////////////////////////////////////