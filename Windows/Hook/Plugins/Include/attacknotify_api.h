#ifndef __ATTACKNOTIFY_API_H__
#define __ATTACKNOTIFY_API_H__

// Attack Notify API

#define ATTACK_NOTIFICATOR_PLUGINNAME  "AttackNotificator"

// ID параметров, которые передаютс€ в klif при возникновении атаки
#define ID_ATTACK_ID            0x1   // ULONG
#define ID_ATTACK_PROTOCOL      0x2   // USHORT
#define ID_ATTACK_ATTACKER_IP   0x3   // ULONG
#define ID_ATTACK_LOCAL_PORT    0x4   // USHORT
#define ID_ATTACK_RESPOND_FLAGS 0x5   // ULONG  
#define ID_ATTACK_TIME          0x6   // BINARY (__int64)
#define ID_BAN_TIME             0x7   // ULONG ( в секундах )
#define ID_ATTACK_NAME          0x8   // BINARY
#define ID_ATTACK_ATTACKER_IPV6   0x9   // USHORT[8]
#define ID_ATTACK_ATTACKER_IPV6_ZONE   0xA   // ULONG

#define FLAG_ATTACK_DENY        0x1
#define FLAG_ATTACK_NOTIFY      0x2
#define FLAG_ATTACK_LOG         0x4
#define FLAG_ATTACK_BAN         0x8



typedef struct _AttackRespondItem {
    unsigned long   m_ID;       // attack ID. ¬ случае дефолтного - присылать 0
    unsigned long   m_Respond;  // смесь флагов, указанных выше
    unsigned long   m_BanTime;  // на сколько банить ( в секундах )
} AttackRespondItem, *pAttackRespondItem;
#define MSG_ATTACK_RESPOND  0x1


// message = ULONG (  in seconds )
#define MSG_SET_NOTIFY_PERIOD 0x2

#endif // __ATTACKNOTIFY_API_H__