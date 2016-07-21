#include "stdafx.h"
#include "klop_util.h"
#include "iphlpapi.h"

#define NT_ADAPTERS_KEY "System\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Adapters"
#define W9X_ADAPTER_KEY "Enum\\Network\\MSTCP"


bool
KLOP_isWinNT()
{
	DWORD dwVersion = GetVersion();
	
	if ( dwVersion < 0x80000000 )
		return true;

	return false;
}

// функция, которая сканирует адаптер.
// Появилась из-за того, что падала 4.0 с модемом.
bool scan( wchar_t *device_name)
{
	bool Result = false;
        HANDLE hdevice ;
        wchar_t device_file[ MAX_PATH ] ;

        swprintf( device_file, L"\\\\.\\%s", device_name) ;

        hdevice = CreateFileW(   device_file,
                                GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL) ;

        if( hdevice == INVALID_HANDLE_VALUE)
        {
                // printf( "Error: Device not available\n\n") ;
        }
        else
        {
                // ...
			CloseHandle( hdevice) ;
			Result = true;
        }


		return Result;
}

bool
IsHidden(HKEY hkey)
{	
	DWORD type = REG_DWORD ;
    DWORD size = sizeof ( type ) ;
	DWORD Hidden = 0;

	RegQueryValueEx(    
				hkey,
                "Hidden",
                NULL,
                &type,
                (PUCHAR)&Hidden,
                &size);

	return Hidden != 0;
}


std::list<ADAPTER_INFO>
KLOP_GetAdapterList()
{
	std::list<ADAPTER_INFO> RetList;
	ADAPTER_INFO	AI;	
	HKEY			hKey = NULL;
	FILETIME		ftLastWriteTime;      // last write time 
	CHAR			achKey[MAX_PATH]; 
	wchar_t			wchKey[MAX_PATH];
	ULONG			KeyLength;
	int				i;
	DWORD			retCode;

	if ( KLOP_isWinNT() )
	{
		/*
		if ( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, NT_ADAPTERS_KEY, &hKey ) )
		{
			for (i = 0, retCode = ERROR_SUCCESS; retCode == ERROR_SUCCESS; i++ )
			{ 
				KeyLength = MAX_PATH;

				retCode = RegEnumKeyExW(hKey, i, wchKey, &KeyLength, NULL, NULL, NULL, &ftLastWriteTime );

				if ( ERROR_SUCCESS == retCode )
				{
					memset ( &AI , 0, sizeof ( AI ) );
					wcscpy ( (wchar_t*)AI.AdapterBuffer, L"\\Device\\");
					wcscat ( (wchar_t*)AI.AdapterBuffer, wchKey );
					AI.AdapterNameSize = ( wcslen ( (wchar_t*)AI.AdapterBuffer ) + 1 ) << 1;
					AI.LocalIp = 0;

					RetList.push_back( AI );
				}
			} 
		}
		else
		*/
		{
			// Winnt4.0
			if ( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards", &hKey ) )
			{
				for ( i = 0, retCode = ERROR_SUCCESS; retCode == ERROR_SUCCESS; i++ )
				{
					HKEY hSubKey = NULL;
					KeyLength = MAX_PATH;
					retCode = RegEnumKeyExW(hKey, i, wchKey, &KeyLength, NULL, NULL, NULL, &ftLastWriteTime );
					
					if ( retCode == ERROR_SUCCESS )
					{
						retCode = RegOpenKeyW(hKey, wchKey, &hSubKey );
					}

					if ( retCode == ERROR_SUCCESS )
					{			
						KeyLength = MAX_PATH;
						DWORD	type = REG_SZ;

						if ( ERROR_SUCCESS == RegQueryValueExW( 
							hSubKey, 
							L"ServiceName", 
							NULL,
							&type,
							(LPBYTE)wchKey, 
							(ULONG*)&KeyLength ) )
						{
							memset ( &AI , 0, sizeof ( AI ) );
							wcscpy ( (wchar_t*)AI.AdapterBuffer, L"\\Device\\");
							wcscat ( (wchar_t*)AI.AdapterBuffer, wchKey );
							AI.AdapterNameSize = ( wcslen ( (wchar_t*)AI.AdapterBuffer ) + 1 ) << 1;
							AI.LocalIp = 0;
							
							// Если открывается и не Hidden, тогда добавляем для проверки.
							if ( scan( (wchar_t*)wchKey ) && !IsHidden(hSubKey) )
								RetList.push_back( AI );
						}
					}

					if ( hSubKey )
						RegCloseKey( hSubKey );
				}
			}			
		}
	}
	else
	{
		if ( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, W9X_ADAPTER_KEY, &hKey ) )
		{			
			for (i = 0, retCode = ERROR_SUCCESS; retCode == ERROR_SUCCESS; i++ )
			{ 
				KeyLength = MAX_PATH;

				retCode = RegEnumKeyExA(hKey, i, achKey, &KeyLength, NULL, NULL, NULL, &ftLastWriteTime);
				
				if ( ERROR_SUCCESS == retCode )
				{
					memset ( &AI , 0, sizeof ( AI ) );					
					strcpy ( AI.AdapterBuffer, achKey );
					AI.AdapterNameSize = strlen ( achKey ) + 1;
					AI.LocalIp = 0;

					RetList.push_back( AI );
				}				
			} 
		}
	}

	if ( hKey )
	{
		RegCloseKey( hKey );
	}

	return RetList;
}

BOOL
KLOP_Init( const char* Key, ULONG KeySize)
{
	BOOL	Result = FALSE;
	DWORD	lpReturned;
	
	ULONG		  AllocSize = sizeof ( CP_INIT_INFO ) + KeySize;
	CP_INIT_INFO* InitInfo	= (CP_INIT_INFO*)malloc ( AllocSize );
	
	if ( InitInfo )
	{
		InitInfo->KeySize = KeySize;
		
		memcpy ( InitInfo->KeyBuffer, Key, KeySize );
		
		Result = DeviceIoControl( 
							g_DrvHandle,
							KLOP_CP_INIT,
							InitInfo,
							AllocSize,
							NULL,
							0,
							&lpReturned,
							NULL);

		free ( InitInfo );
	}
	
	return Result;
}

BOOL
KLOP_AddAdapter(ADAPTER_INFO* pAI)
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
KLOP_SetBroadcastAddr(  )
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
			
			for ( DWORD i = 0; i < IpTable->dwNumEntries; i++ )
			{
				MIB_IFROW row;
				memset ( &row, 0, sizeof ( MIB_IFROW ) );				
				
				if ( KLOP_isWinNT() )
				{
					row.dwIndex = IpTable->table[i].dwIndex;

					if ( NO_ERROR == (err = GetIfEntry(&row) ) )
					{
						pII[i].IpAddr = IpTable->table[i].dwAddr | ~(IpTable->table[i].dwMask);
						RtlCopyMemory ( pII[i].MacAddr, row.bPhysAddr, 6 );
					}
				}
				else
				{
					row.dwIndex = IpTable->table[i].dwIndex;
					if ( NO_ERROR == (err = GetIfEntry(&row) ) )
					{
						pII[i].IpAddr = IpTable->table[i].dwAddr | ~(IpTable->table[i].dwMask);
						RtlCopyMemory ( pII[i].MacAddr, row.bPhysAddr, 6 );
					}
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