#include "stdafx.h"

TCHAR*
DrvName( CKAHSTAT::BaseDrvType DrvType )
{
    switch ( DrvType )
    {
    case CKAHSTAT::bdKlick:
        return _T("\\\\.\\klick");
    case CKAHSTAT::bdKlin:
        return _T("\\\\.\\klin");
    }
    
    return NULL;
}

CKAHUM::OpResult CKAHSTAT_SendPluginMessage (
                                      DWORD dwIoControlCode, 
                                      CKAHSTAT::BaseDrvType BaseDrv,
                                      CHAR *szPluginName, 
                                      ULONG nMessageID,
                                      PVOID pInBuffer, 
                                      ULONG nInBufferSize,
                                      PVOID pOutBuffer, 
                                      ULONG nOutBufferSize, 
                                      ULONG *nRetBufferSize)
{
    BOOL    DevCtlRes;
    CKAHUM::OpResult opRes = CKAHUM::srOK;
    TCHAR*  szName = DrvName( BaseDrv );
    HANDLE hPlugin;

    if ( szName == NULL )
        return CKAHUM::srOpFailed;
    
    std::vector<BYTE> vec_buf ( sizeof (BASE_PLUGIN_MESSAGE) + nInBufferSize, 0);

    BASE_PLUGIN_MESSAGE *pBasePluginMessage = (BASE_PLUGIN_MESSAGE *)&vec_buf[0];

    strcpy_s (pBasePluginMessage->message.PluginName, _countof(pBasePluginMessage->message.PluginName), szPluginName);
	pBasePluginMessage->message.MessageID   = nMessageID;
	pBasePluginMessage->message.MessageSize = nInBufferSize;
    pBasePluginMessage->dwIoctl = dwIoControlCode;

    if (pInBuffer && nInBufferSize > 0)
		memcpy (&vec_buf [sizeof (BASE_PLUGIN_MESSAGE)], pInBuffer, nInBufferSize);

    if ( g_bIsNT )
		hPlugin = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hPlugin = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

    if ( hPlugin == INVALID_HANDLE_VALUE )
        return CKAHUM::srOpFailed;

    DWORD dwBytesReturned = 0;
	
    DevCtlRes = DeviceIoControl( 
                            hPlugin, 
                            BASE_RECEIVE_PLUGIN_REQUEST, 
                            pBasePluginMessage, 
                            sizeof (BASE_PLUGIN_MESSAGE) + nInBufferSize, 
                            pOutBuffer, 
                            nOutBufferSize, 
                            &dwBytesReturned, 
                            NULL);

    if ( !DevCtlRes )
    {
        opRes = CKAHUM::srOpFailed;
        // error
    }
    else
    {
        opRes = CKAHUM::srOK;
    }

    if ( nRetBufferSize )
        *nRetBufferSize = dwBytesReturned;

    CloseHandle( hPlugin );

    return opRes;
}