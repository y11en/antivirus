#pragma once

#if !defined(AFX_NETDETECT_H__6808151E_1AC9_4F8A_BE91_1709D36E1738__INCLUDED_)
#define AFX_NETDETECT_H__6808151E_1AC9_4F8A_BE91_1709D36E1738__INCLUDED_

#include <winsock2.h>

#pragma warning ( disable : 4290 )

class CNetDetect  
{
public:
    class XNet {};

    CNetDetect() throw ( XNet );
    virtual ~CNetDetect();

	bool Detect() throw ( XNet );

private:
	void Cleanup();
    // Flags for InternetGetConnectedState
    enum EInternetConnectedState
    {
        eINTERNET_CONNECTION_MODEM =        1,
        eINTERNET_CONNECTION_LAN =          2,
        eINTERNET_CONNECTION_PROXY =        4,
        eINTERNET_CONNECTION_MODEM_BUSY =   8
    };
    
    typedef BOOL (WINAPI *typeInternetGetConnectedState)(LPDWORD lpdwFlags, DWORD dwReserved);
    typedef int (PASCAL FAR *typeWSAStartup)(IN WORD wVersionRequired, OUT LPWSADATA lpWSAData);
    typedef int (PASCAL FAR *typeWSACleanup)(void);
    typedef struct hostent FAR * ( PASCAL FAR * typeGethostbyname)(IN const char FAR * name);
    
    typeInternetGetConnectedState InternetGetConnectedState;
    typeWSAStartup WSAStartup;
    typeWSACleanup WSACleanup;
    typeGethostbyname gethostbyname;
    
    WSADATA wsaData;
    HMODULE hWinInetDll, hWs2Dll;
};

#pragma warning ( default : 4290 )

#include <stuff/cpointer.h>
typedef CSingleton<CNetDetect> NetDetect;

#endif // !defined(AFX_NETDETECT_H__6808151E_1AC9_4F8A_BE91_1709D36E1738__INCLUDED_)
