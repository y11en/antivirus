//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetDetect.h"
#include "wininet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning ( disable : 4290 )

CPointer<CNetDetect> CSingleton<CNetDetect>::Insider;
int CSingleton<CNetDetect>::RefCount;

CNetDetect::CNetDetect() throw ( XNet )
:   hWs2Dll ( 0 ),
    WSAStartup ( 0 ),
    WSACleanup ( 0 ),
    gethostbyname ( 0 ),
    hWinInetDll ( 0 ),
    InternetGetConnectedState ( 0 ),
    wsaData ()

{try 
{
    hWs2Dll = LoadLibrary ( "ws2_32.dll" );

    if ( ! hWs2Dll ) throw XNet();
    
    WSAStartup = typeWSAStartup ( GetProcAddress ( hWs2Dll, "WSAStartup" ) );
    WSACleanup = typeWSACleanup ( GetProcAddress ( hWs2Dll, "WSACleanup" ) );
    gethostbyname = typeGethostbyname ( GetProcAddress ( hWs2Dll, "gethostbyname" ) );

    if ( ! WSAStartup || ! WSACleanup || ! gethostbyname ) throw XNet();

    memset ( & wsaData, 0, sizeof wsaData );
    if ( WSAStartup ( MAKEWORD(1,1), & wsaData ) != 0 ) throw XNet();

    hWinInetDll = LoadLibrary ( "wininet.dll" );

    if ( hWinInetDll == NULL ) throw XNet();

    InternetGetConnectedState = typeInternetGetConnectedState ( GetProcAddress ( hWinInetDll, 
                                                                 "InternetGetConnectedState" ) );

    if ( ! InternetGetConnectedState ) throw XNet();
}
catch ( ... )
{
    Cleanup();
    throw XNet();
}}

CNetDetect::~CNetDetect()
{
    Cleanup();
}

//////////////////////////////////////////////////////////////////////////

bool CNetDetect::Detect() throw ( XNet )
{
    DWORD dwFlags;
    bool bConnectionActive =  ( InternetGetConnectedState ( & dwFlags, 0 ) == TRUE );

    if ( ! bConnectionActive && dwFlags == 0 )
    {
        throw XNet();
    }
/*    
    if ( dwFlags & eINTERNET_CONNECTION_MODEM )
    {
        return bConnectionActive;
    }
    else if ( dwFlags & ( eINTERNET_CONNECTION_LAN | eINTERNET_CONNECTION_PROXY ) )
*/	
	if (true)
    {
        if (    gethostbyname ( "kaspersky.com" ) != NULL 
            || gethostbyname ( "kaspersky.ru"  ) != NULL )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return bConnectionActive;
    }
}

#pragma warning ( default : 4290 )

//////////////////////////////////////////////////////////////////////////

void CNetDetect::Cleanup()
{
    if ( hWinInetDll ) FreeLibrary ( hWinInetDll );
    if ( wsaData.wHighVersion || wsaData.wVersion ) WSACleanup();
    if ( hWs2Dll ) FreeLibrary ( hWs2Dll );
}
