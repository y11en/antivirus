#include "klim_pkt.h"

VOID
KlimPktSetStatus( PVOID pktContext, KLIM_PKT_STATUS Status )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    p->Status = Status;
}

BOOLEAN
KlimGetLocalMac( PVOID pktContext, char MacAddr[6] )
{
    PKLIM_PACKET p = (PKLIM_PACKET)pktContext;

    RtlCopyMemory( MacAddr, p->LocalMac, 6 );

    return TRUE;
}