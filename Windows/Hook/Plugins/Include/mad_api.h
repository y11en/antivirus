// Make Attack Plugin API

#define MAD_PLUGINNAME  "Make Attack Plugin"

// ioctl message ids
#define MAD_MSGID_ATTACK          0x00000001

typedef struct _MAD_ATTACK {
	ULONG	AttackID;			// идентификатор атаки
	USHORT	Proto;				// протокол
	ULONG	AttackerIP;			// IP-адрес атакующего
	USHORT	LocalPort;			// порт
} MAD_ATTACK, *PMAD_ATTACK;
