// KLDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../klsdk/klstatus.h"
#include "../../../klsdk/PlugAPI.h"
#include "../../include/logger_api.h"
#include "logformat.h"
#include "Protocols.h"
#include "RangeTypes.h"
#include "command_line.h"
#include "install.h"
#include <shellapi.h>
#include <time.h>
#include <StuffIO/Ioutil.h>


static bool g_bRunning = true;

bool g_bProtoAll = true;
bool g_bProtoTCP = false;
bool g_bProtoUDP = false;
bool g_bProtoICMP = false;
int  g_Proto = -1;

bool g_bBcasts = false;

std::vector<Address> g_vecORRemoteList;
std::vector<Address> g_vecNEGRemoteList;

std::vector<Address> g_vecORLocalList;
std::vector<Address> g_vecNEGLocalList;

char* g_szFile2Store = NULL;

static BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
	switch( fdwCtrlType ) 
	{ 
    case CTRL_C_EVENT: 
    case CTRL_CLOSE_EVENT: 
    case CTRL_BREAK_EVENT: 
    case CTRL_LOGOFF_EVENT: 
    case CTRL_SHUTDOWN_EVENT:
		g_bRunning = false;	
		return TRUE; 
		
    default: 
		return FALSE; 
	} 
} 


/*
Return
true - match
false - don't match
*/
template <typename T> __inline bool match_value(T val, Range<T>&  range)
{
	if (range.type == RangeType::eSingle)
	{
		if (range.bval_any || val == range.val)
			return true;
	}
	else
	{
		if ((val >= range.start) && (range.bend_any || val <= range.end))
			return true;
	}
	
	return false;
}

/*
Return
true - match
false - don't match
*/
__inline bool match_ip_port(ULONG ip, USHORT port, Address addr_range)
{
	return match_value (ip, addr_range.ip)&&match_value (port, addr_range.port);
}

/*
Return
true - match
false - don't match
*/
__inline bool match_list(ULONG ip, USHORT port, std::vector<Address>& ORList, std::vector<Address>& NEGList)
{
	int nORListSize = ORList.size ();
	for (int i = 0; i < nORListSize; ++i)
	{
		if(match_ip_port (ip, port, ORList[i]))
			break;
	}

	if (nORListSize && i == nORListSize)
		return false;

	int nNEGListSize = NEGList.size ();
	for (i = 0; i < nNEGListSize; ++i)
	{
		if(match_ip_port (ip, port, NEGList[i]))
			return false;
	}

	return true;
}

bool IPv6_GetNextHdrOffset( IN PCHAR CurHdr, IN UCHAR CurHdrType, OUT ULONG* pOffset, OUT UCHAR* pNextType )
{
    if ( CurHdrType == 0  || // переходный заголовок
        CurHdrType == 43 || // маршрутизация         
        CurHdrType == 60 )  // параметры для получателя
    {
        *pNextType  = CurHdr[0];
        *pOffset    = ( CurHdr[1] + 1 ) * 8 ;

        return true;
    }

    if ( CurHdrType == 51 ) // аутентификация 
    {
        *pNextType  = CurHdr[0];
        *pOffset    = ( CurHdr[1] + 2 ) * 4;
    }

    if ( CurHdrType == 44 ) // фрагментация
    {
        *pNextType  = CurHdr[0];
        *pOffset    = 8;
        return true;
    }

    return false;
}

bool IPv6_HasNextHeader( UCHAR HeaderType )
{
    switch ( HeaderType )
    {
    case 0:
    case 43:
    case 44:
    case 51:
    case 60:
        return true;
    }

    return false;
}

__inline bool FilterIPPacket(bool isIncoming, ULONG local_ip, ULONG remote_ip, UCHAR Protocol, CHAR* Hdr)
{
	if ( (Protocol == IP_PROTO_TCP) && (g_bProtoTCP || g_bProtoAll) )
	{
		TCP_HEADER* pTCP = (TCP_HEADER*)Hdr;
		USHORT remote_port = isIncoming?ntohs(pTCP->srcPort):ntohs(pTCP->dstPort);
		USHORT local_port = isIncoming?ntohs(pTCP->dstPort):ntohs(pTCP->srcPort);

		return match_list (remote_ip, remote_port, g_vecORRemoteList, g_vecNEGRemoteList)&&
				match_list (local_ip, local_port, g_vecORLocalList, g_vecNEGLocalList);
	}
	else if ( (Protocol == IP_PROTO_UDP) && (g_bProtoUDP || g_bProtoAll) )
	{
		UDP_HEADER* pUDP = (UDP_HEADER*)Hdr;
		USHORT remote_port = isIncoming?ntohs(pUDP->srcPort):ntohs(pUDP->dstPort);
		USHORT local_port = isIncoming?ntohs(pUDP->dstPort):ntohs(pUDP->srcPort);
		return match_list (remote_ip, remote_port, g_vecORRemoteList, g_vecNEGRemoteList)&&
			match_list (local_ip, local_port, g_vecORLocalList, g_vecNEGLocalList);

	}
	else if ( (Protocol == IP_PROTO_ICMP) && (g_bProtoICMP || g_bProtoAll) )
	{
		USHORT remote_port = 0;
		USHORT local_port = 0;
		return match_list (remote_ip, remote_port, g_vecORRemoteList, g_vecNEGRemoteList)&&
			match_list (local_ip, local_port, g_vecORLocalList, g_vecNEGLocalList);
	}
    else if ((Protocol == g_Proto) || g_bProtoAll)
    {
		USHORT remote_port = 0;
		USHORT local_port = 0;
		return match_list (remote_ip, remote_port, g_vecORRemoteList, g_vecNEGRemoteList)&&
			match_list (local_ip, local_port, g_vecORLocalList, g_vecNEGLocalList);
    }
    return false;
}

__inline bool FilterPacket(LOGGER_PACKET* pPacket) 
{
	EthHdr* pEth = (EthHdr*)pPacket->PacketBuffer;

    if (pEth->h_dest[0] == 0xFF &&
        pEth->h_dest[1] == 0xFF &&
        pEth->h_dest[2] == 0xFF &&
        pEth->h_dest[3] == 0xFF &&
        pEth->h_dest[4] == 0xFF &&
        pEth->h_dest[5] == 0xFF &&
        !g_bBcasts)
    {
        return false;
    }

	//ip v.4
	if (pEth->h_proto == ETH_PROTO_IP)
	{
		IP_HEADER* pIP = (IP_HEADER*)pEth->data;
		if (pIP->Version == 4)
		{
			ULONG remote_ip	= pPacket->isIncoming?ntohl(pIP->srcIP):ntohl(pIP->dstIP);
			ULONG local_ip	= pPacket->isIncoming?ntohl(pIP->dstIP):ntohl(pIP->srcIP);

            return FilterIPPacket(pPacket->isIncoming ? true : false, local_ip, remote_ip, pIP->Protocol, (CHAR*)((ULONG*)pIP + pIP->Ihl));
		}
	}
	else if (pEth->h_proto == ETH_PROTO_IPV6) // IPv6
	{
        IPV6_HEADER* pIP = (IPV6_HEADER*)pEth->data;
        
        UCHAR CurType = pIP->NextHeader;
        ULONG CurOffset = sizeof ( IPV6_HEADER );
        ULONG TotalOffset = CurOffset;

        while ( IPv6_HasNextHeader(CurType) )
        {        
            // вычисляем следующий заголовок
            if ( !IPv6_GetNextHdrOffset( (CHAR*)pEth->data + TotalOffset, CurType, &CurOffset, &CurType ) )
                break;

            TotalOffset += CurOffset;
        }

        return FilterIPPacket(pPacket->isIncoming ? true : false, 0, 0, CurType, (CHAR*)pEth->data + TotalOffset);
    }

	return false;
}


int main(int argc, char* argv[])
{
	if (argc == 2 && stricmp(argv[1], "/uninstall") == 0)
	{
		UninstallKlick ();

		TCHAR InstallFolder[MAX_PATH];
		ExpandEnvironmentStrings( _T("%ProgramFiles%\\Kaspersky Lab\\Klick") , InstallFolder, sizeof(InstallFolder)/sizeof(InstallFolder[0]) );

		if(!DeleteFile((std::string(InstallFolder) + "\\kldump.exe").c_str ()))
		{
			Sleep(500);
			DeleteFile((std::string(InstallFolder) + "\\kldump.exe").c_str ());
		}

		RemoveDirectory (InstallFolder);
		RegDeleteKey (HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Klick"));

		return 0;
	}

	if (argc == 2 && stricmp(argv[1], "/remove_installer") == 0)
	{
		FreeConsole();

		if ( MessageBox(NULL, "Delete KLDUMP base driver?", "Kaspersky lab", MB_YESNO)!=IDYES )
			return 0;
		
		std::string ExePath;
		GetExeTempFileName (ExePath);
		
		TCHAR ModuleName[MAX_PATH];
		GetModuleFileName(NULL, ModuleName, sizeof(ModuleName)/sizeof(ModuleName[0]));

		CopyFile(ModuleName, ExePath.c_str(), FALSE);
		IOSMoveFileOnReboot(ExePath.c_str(), NULL);
		ShellExecute(NULL, NULL, ExePath.c_str(), _T("/uninstall"), NULL, SW_NORMAL);
		return 0;
	}


	HANDLE hKLICK = FindPluginDriver (_T("\\\\.\\klick"));

	if (hKLICK == INVALID_HANDLE_VALUE)
	{
		printf("Base driver \"klick\" not found. Install...\n");

		if (InstallKlick())
		{
			TCHAR InstallFolder[MAX_PATH];
			ExpandEnvironmentStrings( _T("%ProgramFiles%\\Kaspersky Lab") , InstallFolder, sizeof(InstallFolder)/sizeof(InstallFolder[0]) );
			CreateDirectory(InstallFolder, NULL);
			CreateDirectory((std::string(InstallFolder) + "\\Klick").c_str (), NULL);

			TCHAR CurrentPath[MAX_PATH];
			GetModuleFileName(NULL, CurrentPath, sizeof(CurrentPath)/sizeof(CurrentPath[0]));
			CopyFile(CurrentPath, (std::string(InstallFolder) + "\\Klick\\kldump.exe").c_str (), FALSE);
			

			HKEY hUninstallKey;
			DWORD dwDisposition;
			if ( ERROR_SUCCESS == RegCreateKeyEx (
				HKEY_LOCAL_MACHINE, 
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Klick"),
				0,
				NULL,
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				NULL,
				&hUninstallKey,
				&dwDisposition) )
			{
				TCHAR KeyData[MAX_PATH] = _T("Klick base driver(KLDump)");
				RegSetValueEx (
					hUninstallKey,    // subkey handle 
					_T("DisplayName"),				       // value name 
					0,						       // must be zero 
					REG_SZ,						   // value type 
					(LPBYTE) KeyData,				// pointer to value data 
					(DWORD) (_tcslen(KeyData)+1)*sizeof(TCHAR)  // length of value data 
					);

				_stprintf(KeyData, _T("%s\\Klick\\kldump.exe /remove_installer"), InstallFolder);
				RegSetValueEx (
					hUninstallKey,    // subkey handle 
					_T("UninstallString"),				       // value name 
					0,						       // must be zero 
					REG_SZ,						   // value type 
					(LPBYTE) KeyData,				// pointer to value data 
					(DWORD) (_tcslen(KeyData)+1)*sizeof(TCHAR)  // length of value data 
					);


				RegCloseKey (hUninstallKey);
			}
			

			printf("\"klick\" installation success. Need reboot.");
		}
		else
			printf("\"klick\" installation FAILED!");

		//always reboot
		return -1;
	}

	CloseHandle (hKLICK);

	if (!parse_commandline (argc, argv))
		return -1;

	if(g_szFile2Store == NULL)
	{
		printf("Filename to store not found, use /f file_name. Execution stopped.\n");
		return -1;
	}

	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );


	UnregLogger Logger;

	if (!Logger.Install())
	{
		printf("Cannot register plugin driver \"" LOGGER_NAME "\". Execution stopped.\n");
		return -1;
	}

	HANDLE hDriver = Logger.m_hLogger;


	HANDLE hStorage = CreateFile(g_szFile2Store, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hStorage == INVALID_HANDLE_VALUE)
	{
		printf("Cannot create file \"%s\". Execution stopped.\n", g_szFile2Store);
		return -1;
	}

    KLDUMP_LOG_HEADER Header;
    Header.sig = KLDUMP_LOG_SIG;
    Header.time = time(NULL);
    Header.reserved0 = 0;
    Header.reserved1 = 0;

	DWORD dwWritten = 0;
	if ( !WriteFile(hStorage, &Header, sizeof(Header), &dwWritten, NULL) || dwWritten != sizeof(Header) )
	{
		printf("\nWrite file error(%d). Execution stopped.\n", GetLastError());
		return -1;
	}

	ULONG uOut = 0;
	if (!DeviceIoControl(hDriver, PLUG_MOD_RESUME, NULL, 0, NULL, 0, &uOut, NULL))
	{
		printf("Cannot resume logger plugin\n");
		return -1;
	}


	PLUGIN_MESSAGE PluginMessage;
	RtlZeroMemory ( &PluginMessage, sizeof ( PluginMessage) );
	strcpy (PluginMessage.PluginName,  LOGGER_PLUGIN_NAME);
	PluginMessage.MessageID = MSG_GET_PACKET;

	LOGGER_PACKET Packet;
	ULONG uOutBuffSize = 0;

    printf("Writing captured traffic into %s...\n", g_szFile2Store);
    ULONG uPackets = 0;
    ULONG uSize = 0;

	bool bRunning = true;
	for(;;)
	{
		
		if (!g_bRunning)
		{
			if (!DeviceIoControl(hDriver, PLUG_MOD_PAUSE, NULL, 0, NULL, 0, &uOut, NULL))
			{
				printf("\nCannot pause logger plugin\n");
				return -1;
			}

			bRunning = false;

		}

		while (DeviceIoControl(hDriver, PLUG_IOCTL, &PluginMessage, sizeof(PLUGIN_MESSAGE), &Packet, sizeof (LOGGER_PACKET), &uOutBuffSize, NULL))
		{
			if ( !FilterPacket(&Packet) )
				continue;

			DWORD dwWritten = 0;
			
			if ( !WriteFile(hStorage, &Packet, Packet.PacketSize + 2 * sizeof(ULONG), &dwWritten, NULL) || dwWritten != Packet.PacketSize + 2 * sizeof(ULONG) )
			{
				printf("\nWrite file error(%d). Execution stopped.\n", GetLastError());
				return -1;
			}
			
			++uPackets;
            uSize += Packet.PacketSize;
        }

		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		printf("Packets/bytes captured: %d/%d", uPackets, uSize);

		if (bRunning)
		{
			Sleep(10);
		}
		else
        {
            printf("\nExecution stopped.");
			break;
        }
	}



	return 0;
}

