// klopik.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

HANDLE	g_DrvHandle = NULL;

struct ADAPTER_INFO {
    ULONG			Version;    // Версия AdapterInfo.
	ULONG			LocalIp;
	ULONG			AdapterNameSize;
	char			AdapterBuffer[MAX_PATH];
};

std::list<ADAPTER_INFO>	AdapterList;

char	Key[12] = "Hello World";

bool
isWinNT()
{
	DWORD dwVersion = GetVersion();
	
	if ( dwVersion < 0x80000000 )
		return true;

	return false;
}

BOOL
IK_GetAdapters()
{
	ADAPTER_INFO	AI;	
	HKEY			hKey;
	FILETIME		ftLastWriteTime;      // last write time 


	if ( isWinNT() )
	{
		if ( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Adapters", &hKey ) )
		{
			DWORD retCode;
			CHAR  achKey[MAX_PATH]; 
			ULONG	KeyLength;
			int i;

			for (i = 0, retCode = ERROR_SUCCESS; 
						retCode == ERROR_SUCCESS; 
						i++								)
			{ 
				KeyLength = MAX_PATH;
				retCode = RegEnumKeyExW(hKey, 
										i, 
										(wchar_t*)achKey, 
										&KeyLength, 
										NULL, 
										NULL, 
										NULL, 
										&ftLastWriteTime); 

				if ( retCode == ERROR_SUCCESS )
				{
					memset ( &AI , 0, sizeof ( AI ) );
					wcscpy ( (wchar_t*)AI.AdapterBuffer, L"\\Device\\");
					wcscat ( (wchar_t*)AI.AdapterBuffer, (wchar_t*)achKey );
					AI.AdapterNameSize = ( wcslen ( (wchar_t*)achKey ) + 1 ) << 1;
					AI.LocalIp = 0;

					AdapterList.push_back( AI );
				}
				
			} 

		}
	}
	else
	{

		if ( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, "Enum\\Network\\MSTCP", &hKey ) )
		{
			DWORD retCode;
			CHAR  achKey[MAX_PATH]; 
			wchar_t wchkey[MAX_PATH];
			ULONG	KeyLength;
			int i;

			for (i = 0, retCode = ERROR_SUCCESS; 
						retCode == ERROR_SUCCESS; 
						i++								)
			{ 
				KeyLength = MAX_PATH;
				retCode = RegEnumKeyExA(hKey, 
										i, 
										achKey, 
										&KeyLength, 
										NULL, 
										NULL, 
										NULL, 
										&ftLastWriteTime); 

				if ( retCode == ERROR_SUCCESS )
				{
					//MultiByteToWideChar ( CP_ACP, 0, achKey, strlen ( achKey ) + 1, wchkey, sizeof ( wchkey ) / sizeof ( wchar_t ) );    

					memset ( &AI , 0, sizeof ( AI ) );					
					strcpy ( AI.AdapterBuffer, achKey );
					AI.AdapterNameSize = strlen ( achKey ) + 1;
					AI.LocalIp = 0;

					AdapterList.push_back( AI );
				}
				
			} 

		}
	}

	
	return TRUE;
}

BOOL
IK_OpenKlop()
{
	BOOL	Result;
	ULONG	Version = 0;
	ULONG	dwRet;

	g_DrvHandle = CreateFile(
		"\\\\.\\klop",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if ( g_DrvHandle == INVALID_HANDLE_VALUE )
	{
		printf ( "Unable to Open KLOP Driver\n" );
		return FALSE;
	}

	if ( Result = DeviceIoControl( g_DrvHandle,
		KLOP_GET_VERSION,
		NULL,
		0,
		&Version,
		sizeof ( Version ),
		&dwRet,
		NULL) )
	{	
		printf( "Klopik Version = %x\n", KLOP_API_VERSION );
		printf( "Driver Version = %x\n", Version );

		if ( Version != KLOP_API_VERSION )
			return FALSE;
	}

	return TRUE;
}

BOOL
IK_CloseKlop()
{
	if ( g_DrvHandle )
		CloseHandle( g_DrvHandle );

	return TRUE;
}

BOOL
IK_Init()
{
	BOOL	Result = FALSE;
	DWORD	lpReturned;
	
	ULONG			AllocSize = sizeof ( CP_INIT_INFO ) + sizeof( Key );
	CP_INIT_INFO* ii = (CP_INIT_INFO*)malloc ( AllocSize );
	
	if ( ii )
	{
		ii->KeySize = sizeof ( Key );
		memcpy ( ii->KeyBuffer, Key, sizeof ( Key ) );
		
		Result = DeviceIoControl( 
							g_DrvHandle,
							KLOP_CP_INIT,
							ii,
							AllocSize,
							NULL,
							0,
							&lpReturned,
							NULL);

		free ( ii );
	}
	
	return Result;
}

BOOL
IK_AddAdapter(ADAPTER_INFO* pAI)
{	
	BOOL	Result;
	DWORD	lpReturned;
	
	Result = DeviceIoControl( g_DrvHandle,
		KLOP_CP_ADD_ADAPTER,
		pAI,
		sizeof ( ADAPTER_INFO ),
		NULL,
		0,
		&lpReturned,
		NULL);
	
	return Result;
}

BOOL
IK_SetBroadcastAddr(  )
{
	BOOL	Result;
	DWORD	lpReturned;
	PMIB_IPADDRTABLE  IpTable = NULL;
	ULONG	dwSize = 0;
	CP_IFACE_INFO* pII = NULL;
	DWORD	err;

	if ( ERROR_INSUFFICIENT_BUFFER == GetIpAddrTable(IpTable, &dwSize, TRUE) )
	{
		IpTable = (PMIB_IPADDRTABLE)malloc ( dwSize );
	}

	if ( IpTable )
	{
		if ( NO_ERROR == (err = GetIpAddrTable(IpTable, &dwSize, TRUE)) )
		{
			pII = (CP_IFACE_INFO*)malloc ( sizeof ( CP_IFACE_INFO ) * IpTable->dwNumEntries );
			
			for ( int i = 0; i < IpTable->dwNumEntries; i++ )
			{
				MIB_IFROW row;
				memset ( &row, 0, sizeof ( MIB_IFROW ) );
				row.dwIndex = IpTable->table[i].dwIndex;
				
				if ( NO_ERROR == (err = GetIfEntry(&row) ) )
				{
					pII[i].IpAddr = IpTable->table[i].dwAddr | ~(IpTable->table[i].dwMask);
					RtlCopyMemory ( pII[i].MacAddr, row.bPhysAddr, 6 );
				}
			}
		}
		
		if ( pII )
		{
			Result = DeviceIoControl( g_DrvHandle,
				KLOP_CP_SET_BROADCAST,
				pII,
				IpTable->dwNumEntries * sizeof(CP_IFACE_INFO),
				NULL,
				0,
				&lpReturned,
				NULL);
			
			free ( pII );
		}	

		free ( IpTable );
	}
	
	
	return Result;
}

BOOL
IK_Done()
{
	BOOL	Result;
	DWORD	lpReturned;
	
	Result = DeviceIoControl( g_DrvHandle,
		KLOP_CP_DONE,
		NULL,
		0,
		NULL,
		0,
		&lpReturned,
		NULL);
	
	return Result;
}

BOOL
IK_Activate()
{
	BOOL	Result;
	DWORD	lpReturned;
	
	Result = DeviceIoControl( g_DrvHandle,
		KLOP_CP_ACTIVATE,
		NULL,
		0,
		NULL,
		0,
		&lpReturned,
		NULL);
	
	return Result;
}


int main(int argc, char* argv[])
{	
	std::list<ADAPTER_INFO>::iterator ii;	

	IK_SetBroadcastAddr();

	IK_OpenKlop();

	IK_Init();
	
	IK_GetAdapters();

	for ( ii = AdapterList.begin(); ii != AdapterList.end(); ii++ )
	{		
		IK_AddAdapter( &(ADAPTER_INFO)*ii );		
	}

	IK_SetBroadcastAddr();

	IK_Activate();

	IK_CloseKlop();

	return 0;
}
