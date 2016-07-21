// Logger API

#define KLOG_PLUGIN_NAME "KLOG"

#pragma pack( 1 )

#define KLOG_PKT_SIZE         0x1     // ULONG
#define KLOG_PKT_DIRECTION    0x2     // ULONG
#define KLOG_PKT_DATA         0x3     // ULONG
#define KLOG_PKT_TIME         0x4     // KL_TIME
#define KLOG_PKT_STATUS       0x5     // ULONG

/*
    аттрибут пакета логгера. размер = 0 является допустимым
*/
typedef struct _KLOG_ATTRIBUTE {
    ULONG   size;
    ULONG   id;
    char    data[1];
}KLOG_ATTRIBUTE, *PKLOG_ATTRIBUTE;

// размер данных, потом 
typedef struct _KLOG_PACKET {
    ULONG   Size;
    KLOG_ATTRIBUTE Attrib[1];
} KLOG_PACKET, *PKLOG_PACKET;


// outBuffer = PKLOG_ATTRIBUTE, PKLOG_ATTRIBUTE, ....
#define MSG_KLOG_GET_PACKET      0x1


#pragma pack()
