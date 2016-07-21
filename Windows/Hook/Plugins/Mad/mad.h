#ifndef _PACKET_LOGGER_H_
#define _PACKET_LOGGER_H_

#include "kldef.h"
#include "klickplugin.h"
#include "..\Include\mad_api.h"

class CKl_Mad   : public CKl_KlickPlugin
{   
public:
    CKl_Mad(char*   PluginName = NULL);
    virtual ~CKl_Mad();
    
    static KLSTATUS KL_CALLBACK ReceiveFrame(PVOID pktContext, void*   plContext);

    virtual 
    KLSTATUS    OnReceiveMessage(ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned );

    virtual 
    KLSTATUS    OnReceiveIOCTL(ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned );
};

#endif