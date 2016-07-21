// “естирование списка открытых портов

#include "stdafx.h"

HANDLE  OpenDriver(char*    drv)
{
    HANDLE	hFile;		
	
	hFile = CreateFile(
		drv,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

    if ( hFile == (void*)-1 )
        printf ( "Error Open %s\n", drv );

    return hFile;
}

void
CloseDriver(HANDLE  hDrv)
{
    CloseHandle( hDrv );
}

BOOL
SendPM(HANDLE drv, char*    PluginName, ULONG  MessageID, PVOID Message, ULONG   MessageSize, PVOID retBuf, ULONG RetBufSize, ULONG* BytesNeeded)
{
    BOOL    res;
    ULONG   ret;
    
    PPLUGIN_MESSAGE pm = (PPLUGIN_MESSAGE)malloc ( sizeof (PLUGIN_MESSAGE) + MessageSize );
    
    if ( PluginName )
        strcpy( pm->PluginName, PluginName );
    pm->plContext = NULL;
    pm->SenderID = 0;
    pm->MessageID = MessageID;
    pm->MessageSize = MessageSize;
    memcpy( pm->Message, Message, MessageSize );

    res = DeviceIoControl( drv, PLUG_IOCTL, pm, sizeof (PLUGIN_MESSAGE) + MessageSize, retBuf, RetBufSize, &ret, NULL);

    *BytesNeeded = ret;

    if ( pm )
        free ( pm );

    return res;
}

ULONG
GetPlugModVersion(HANDLE    drv)
{
    ULONG   Version = 0,
            ret     = 0;

    DeviceIoControl( drv, PLUG_GET_MOD_VERSION, NULL, 0, &Version, sizeof(Version), &ret, NULL );

    printf ("Ret = %x\n", ret);

    return Version;
}

ULONG
GetPortList(HANDLE  drv,char*   List, ULONG ListSize, ULONG* BytesRet )
{
    ULONG   error = ERROR_INSUFFICIENT_BUFFER;
    
    if ( 0 == SendPM(drv, FIREWALL_PLUGIN_NAME, MSG_GET_PORT_LIST, NULL, 0,  List, ListSize, BytesRet) )
    {
        error = GetLastError();        
    }
    else
    {
        error = ERROR_SUCCESS;
    }
    printf ("BytesRet = %x\n", *BytesRet);

    return error;
}

void
PrintPortList(char* List, int ListSize)
{
    PFW_PORT_LIST_ITEM  Item = (PFW_PORT_LIST_ITEM)List;

    while ( (int)Item - (int)List < (int)ListSize )
    {
        printf ("%ls (PID = %x) %x:%d \n", Item->UserAppName, Item->PID, Item->LocalIp, Item->LocalPort);
        ++Item;
    }
}


int main(int argc, char* argv[])
{
    HANDLE  hFW   = OpenDriver("\\\\.\\klfw");
    ULONG   bRet  = 0;

    if ( hFW == (void*)-1 )
    {
        return -1;
    }

    printf ( "Plugin Module Version = %x\n", GetPlugModVersion(hFW) );

    char*    PortList;
    int      ListSize = 100000;

    if ( PortList = (char*)malloc ( ListSize ) )
    {
        while ( ERROR_INSUFFICIENT_BUFFER == GetPortList( hFW, PortList, ListSize, &bRet) )
        {
            free ( PortList );
            ListSize += 10000;

            PortList = (char*)malloc ( ListSize );
        }
        printf ("Bytes ret = %x\n", bRet);
        
        if ( PortList )
        {
            PrintPortList( PortList, bRet );
            free( PortList );
        }
    }

    GetPlugModVersion(hFW);

    CloseDriver( hFW );
        
	return 0;
}
