#ifndef _KL_STAT_API_H__
#define _KL_STAT_API_H__

#define KLICK_STATISTICS_PLUGIN "KlickStat"
#define KLIN_STATISTICS_PLUGIN "KlinStat"

// MSG_STAT_RESET ---------------------------------------
#define MSG_STAT_RESET                                      0x1
//-------------------------------------------------------

// MSG_GET_BLOCKPASS_STAT -------------------------------

typedef struct _BLOCKPASS_STAT {
    __int64 dwInPass;
    __int64 dwInBlock;
    __int64 dwOutPass;
    __int64 dwOutBlock;
} BLOCKPASS_STAT, *PBLOCKPASS_STAT;

#define MSG_GET_BLOCKPASS_STAT                              0x2
//-------------------------------------------------------

// MSG_GET_CONNECTION_COUNT -----------------------------
// OutBuffer = ULONG
#define MSG_GET_CONNECTION_COUNT                            0x3
//-------------------------------------------------------

// MSG_GET_OPEN_PORT_COUNT ------------------------------
// OutBuffer = ULONG
#define MSG_GET_OPEN_PORT_COUNT                             0x4
//-------------------------------------------------------


// MSG_GET_DROPPED_CONNECTIONS --------------------------
// OutBuffer = ULONG
#define MSG_GET_DROPPED_CONNECTION_COUNT                    0x5
//-------------------------------------------------------


// MSG_GET_LAST_HOUR_STAT--------------------------------

// пакетная статистика, собираемая на RIP
// та же статистика сбрасывается периодически в реестр для сохранения
typedef struct _RIP_PKT_STAT {
    ULONG   rIp;
    
    __int64 m_InSize;   // скачано байт, включая заголовки
    __int64 m_OutSize;  // отправлено байт, включая заголовки.
    
} RIP_PKT_STAT, *PRIP_PKT_STAT;

#define STAT_VERSION 0x1

typedef struct _PKT_STAT {
    ULONG   Version;
    KL_TIME StartTime;

    ULONG Count;
    RIP_PKT_STAT rIpStat[0];
} PKT_STAT, *PPKT_STAT;
#define MSG_GET_LAST_HOUR_STAT                              0x6

//-------------------------------------------------------


// MSG_GET_LAST_DAY_STAT --------------------------------
typedef struct _LAST_DAY_TRAFFIC_INFO {
    __int64 InBytes;
    __int64 OutBytes;
} LAST_DAY_TRAFFIC_INFO, *PLAST_DAY_TRAFFIC_INFO;
#define MSG_GET_LAST_DAY_STAT                               0x7

//-------------------------------------------------------



#endif