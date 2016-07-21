#ifndef _PARSER_H
#define _PARSER_H

#include "klickplugin.h"
#include "Ids_common.h"

class CKl_PacketParcer : public CKl_KlickPlugin
{    
public:
    
    CKl_PacketParcer();
    virtual ~CKl_PacketParcer();

    static KLSTATUS KL_CALLBACK ReceiveFrame (PVOID pktContext, PVOID  plContext);
    static KLSTATUS KL_CALLBACK ReceiveFrameDispatch (PVOID pktContext, PVOID  plContext);

    static KLSTATUS KL_CALLBACK ReceiveIP  (PVOID     plContext, ULONG    DataSize, PVOID pData);

    static KLSTATUS KL_CALLBACK ReceiveTCP (PVOID     plContext, ULONG    DataSize, PVOID pData);
    static KLSTATUS KL_CALLBACK ReceiveUDP (PVOID     plContext, ULONG    DataSize, PVOID pData);
    static KLSTATUS KL_CALLBACK ReceiveICMP(PVOID     plContext, ULONG    DataSize, PVOID pData);
};

#endif