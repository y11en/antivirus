// main.cpp //////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "..\ipc.dll\load_ipc.h"
#include "DateTime.h"

//////////////////////////////////////////////////

int main( int argc, char * * argv )
{
	if ( !IPC_LoadDLL() ) return 1;

	HIPCSERVER hServer = IPC_ServerStart(__TEXT("ipc_test_server"));
	if ( !hServer ) return 1;
	printf("\n    Server started\n");
	printf("\n    Server wait for connection...\n");
	HIPCCONNECTION hConnection1 = IPC_ServerWaitForConnection( hServer, IPC_TIMEOUT_INFINITE );
	if( hConnection1 == 0 || hConnection1 == IPC_RC_TIMEOUT )
	{
		printf( "    Can't connect with Client\n" );
		IPC_ServerStop( hServer );
		IPC_FreeDLL();
		return 1;
	}
	printf("    Client connected\n");

	//////////////////////////////////////////////
//	bool boRes = IPC_FreeDLL();
//	return 1;


	char * p = new char[ 64 * 1024 ];
	if ( ! p )
	{
		IPC_CloseConnection( hConnection1 );
		IPC_ServerStop( hServer );
		IPC_FreeDLL();
		return 1;
	}

	//////////////////////////////////////////////

	HANDLE h = CreateEvent( 0, TRUE, FALSE, 0 );
	IPC_SetEvents( hConnection1, &h, 1, 0 );

	DWORD dwTickCount1 =  GetTickCount();
	for ( DWORD i = 0; i <= 30000; i++ )
	{
		DWORD rc = IPC_Recv( hConnection1, p, 64 * 1024, INFINITE );
 		if ( rc == -1 || rc == -2 ) printf( "\nerror\n" );
//		SetEvent( h );
//		strcpy( p, "ServerSendData" );
//		rc = IPC_Send( hConnection1, p, 64 * 1024, INFINITE );
//		if ( rc == -1 || rc == -2 ) printf( "\nerror\n" );
//		DWORD dwRes = 0;
//		BOOL boRes = IPC_GetEvents( hConnection1, &dwRes, 0 );
	}
	DWORD dwTickCount2 = GetTickCount();
	printf("\nTickCount=%ld", dwTickCount2 - dwTickCount1);

	//////////////////////////////////////////////

	delete[] p;

	BOOL r1 = IPC_CloseConnection( hConnection1 );

	IPC_ServerStop( hServer );
	IPC_FreeDLL();
	return 0;
}	

// eof ///////////////////////////////////////////