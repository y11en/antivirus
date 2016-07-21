#include "StdAfx.h"
#include "..\..\Windows\Hook\Plugins/Include/klstat_api.h"

CKAHUM::OpResult  CKAHSTAT::GetPassDropPktStat( 
                            OUT __int64* nInDropped, 
                            OUT __int64* nInPassed, 
                            OUT __int64* nOutDropped,                             
                            OUT __int64* nOutPassed )
{
    CKAHUM::OpResult Res;
    BLOCKPASS_STAT  Stat;

    Res = CKAHSTAT_SendPluginMessage( 
                            PLUG_IOCTL,
                            CKAHSTAT::bdKlick,
                            KLICK_STATISTICS_PLUGIN,
                            MSG_GET_BLOCKPASS_STAT,
                            NULL, 0,
                            &Stat, sizeof ( Stat ),
                            NULL);

    if ( nInDropped )
        *nInDropped = Stat.dwInBlock;

    if ( nInPassed )
        *nInPassed = Stat.dwInPass;

    if ( nOutDropped )
        *nOutDropped = Stat.dwOutBlock;

    if ( nOutPassed )
        *nOutPassed = Stat.dwOutPass;

    return Res;
}

CKAHUM::OpResult CKAHSTAT::GetTrafficInfoLastDay( 
                            OUT __int64* nInBytes,
                            OUT __int64* nOutBytes )
{
    CKAHUM::OpResult Res;

    LAST_DAY_TRAFFIC_INFO Stat;

    Res = CKAHSTAT_SendPluginMessage( 
                            PLUG_IOCTL,
                            CKAHSTAT::bdKlick,
                            KLICK_STATISTICS_PLUGIN,
                            MSG_GET_LAST_DAY_STAT,
                            NULL, 0,
                            &Stat, sizeof ( Stat ),
                            NULL);

    if ( nInBytes )
        *nInBytes = Stat.InBytes;

    if ( nOutBytes )
        *nOutBytes = Stat.OutBytes;

    return Res;
}

CKAHUM::OpResult CKAHSTAT::GetConnectionCount(ULONG* pCount)
{
    CKAHUM::OpResult OpRes;    

    OpRes = CKAHSTAT_SendPluginMessage( 
                            PLUG_IOCTL,
                            CKAHSTAT::bdKlin,
                            KLIN_STATISTICS_PLUGIN,
                            MSG_GET_CONNECTION_COUNT,
                            NULL, 0,
                            pCount, sizeof ( ULONG ),
                            NULL);

    return OpRes;
}

CKAHUM::OpResult CKAHSTAT::GetDroppedConnectionCount(ULONG* pCount)
{
    CKAHUM::OpResult OpRes;    

    OpRes = CKAHSTAT_SendPluginMessage( 
                            PLUG_IOCTL,
                            CKAHSTAT::bdKlin,
                            KLIN_STATISTICS_PLUGIN,
                            MSG_GET_DROPPED_CONNECTION_COUNT,
                            NULL, 0,
                            pCount, sizeof ( ULONG ),
                            NULL);

    return OpRes;
}