// Logger API

#define LOGGER_PLUGIN_NAME "Logger"

#pragma pack( 1 )

typedef struct _LOGGER_PACKET
{
    ULONG   isIncoming;
    ULONG   PacketSize;
    char    PacketBuffer[1600];
} LOGGER_PACKET, *PLOGGER_PACKET;

// outBuffer = PLOGGER_PACKET
#define MSG_GET_PACKET      0x1

// inBuffer = PluginMessage ( "C:\\Filename.log" )
#define MSG_SET_FILE_NAME   0x2

// *(ULONG*)OutBuffer = Count; // ( of queued packets )
#define MSG_GET_COUNT       0x3


#pragma pack()
