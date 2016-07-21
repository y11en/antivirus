#define OUTGOING_PACKET			0x00000001
#define INCOMING_PACKET			0x00000002
#define WAS_NDIS_PACKET			0x00000004
#define PROCESSED_PACKET		0x00000008
#define DROP_PACKET				0x00000010
#define LOGGED_PACKET			0x00000020
#define HANDLER_SEND			0x00000040
#define HANDLER_SEND_PACKETS	0x00000080
#define HANDLER_RECEIVE			0x00000100
#define HANDLER_RECEIVE_PACKET	0x00000200
#define SELF_CONSTRUCTED		0x00000400

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

extern unsigned long   KlinPluginID;
extern unsigned long   KlickPluginID;

extern PVOID           Klin_Device;
extern PVOID           Klick_Device;
