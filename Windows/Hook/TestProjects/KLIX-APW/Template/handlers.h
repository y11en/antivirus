$$IF(KLIN)
// KLIN start
extern unsigned long   KlinPluginID;
extern PVOID           Klin_Device;
// KLIN end
$$ENDIF

$$IF(KLICK)
// KLICK Start
extern unsigned long   KlickPluginID;
extern PVOID           Klick_Device;
// KLICK end
$$ENDIF

KLSTATUS	    
__cdecl
PluginInit(PPLUGIN_INIT_INFO   PlugInfo, PBASEDRV_INFO  pBaseDrvInfo);

KLSTATUS
MyIoctlHandler( ULONG nIoctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned);

$$IF(KLIN)
// KLIN Start
KLSTATUS
OnRegisterKlinPlugin(unsigned long ID, KL_PLUGIN_TYPE Type, IoctlHandler   PluginIoctl);

KLSTATUS
__cdecl
OnConnect (PVOID Handle, BOOLEAN isIncoming);
// KLIN end
$$ENDIF

$$IF(KLICK)
// KLICK start
KLSTATUS
OnRegisterKlickPlugin(unsigned long ID, KL_PLUGIN_TYPE Type, IoctlHandler   PluginIoctl);

KLSTATUS
__cdecl
OnReceive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize);

KLSTATUS
__cdecl
OnReceivePassive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize);
// KLICK end
$$ENDIF
