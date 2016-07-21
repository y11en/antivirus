#define __SID_LENGTH	32
#define HASH_SIZE       16

#define htonl(_x_) ((ULONG)(_x_<<24) | (((_x_)<<8)&0x00FF0000)|(((_x_)>>8)&0x0000FF00)|(((_x_)>>24)&0x000000FF))
#define htons(_x_) ((USHORT)(((_x_)<<8)|((_x_)>>8)))

extern unsigned long   KlinPluginID;
extern unsigned long   KlickPluginID;

extern PVOID           Klin_Device;
extern PVOID           Klick_Device;

KLSTATUS
__cdecl
PluginInit(PPLUGIN_INIT_INFO   PlugInfo, PBASEDRV_INFO  pBaseDrvInfo);

KLSTATUS
MyIoctlHandler(
                ULONG nIoctl, 
                PVOID pInBuffer, 
                ULONG InBufferSize, 
                PVOID pOutBuffer, 
                ULONG OutBufferSize, 
                ULONG* nReturned);


KLSTATUS
__cdecl
OnConnectPassive (PVOID Handle, BOOLEAN isIncoming);

KLSTATUS
__cdecl
OnConnect (PVOID Handle, BOOLEAN isIncoming);

KLSTATUS
__cdecl
OnPostConnect (PVOID Handle, BOOLEAN isIncoming);

KLSTATUS
__cdecl
OnSendDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr);

KLSTATUS
__cdecl
OnDelayedSendDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr);

KLSTATUS
__cdecl
OnRecvDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr);

KLSTATUS
__cdecl
OnDelayedRecvDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr);

typedef struct _ACCEPT_INFO {
    LIST_ENTRY      m_ListEntry;
    KL_TIME         TimeStamp;
    
    unsigned long   lIp;
    unsigned short  lPort;
    unsigned long   rIp;
    unsigned short  rPort;
} ACCEPT_INFO, *PACCEPT_INFO;

VOID	RefreshAcceptList();

BOOLEAN	CheckAcceptList(unsigned long lIp, unsigned short lPort, unsigned long rIp, unsigned short rPort);
