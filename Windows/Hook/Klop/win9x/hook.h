#ifndef _G_KLOP_HOOK_H__
#define _G_KLOP_HOOK_H__

#include "kldef.h"
#include "klstatus.h"
#include "..\g_custom_packet.h"

ULONG   HookNdis();

typedef 
LRESULT 
(__cdecl *t_PELDR_AddExportTable)   ( PHPEEXPORTTABLE, PSTR, ULONG,ULONG,ULONG,PVOID,PUSHORT,PVOID,PHLIST);


LRESULT 
__cdecl
HPELDR_AddExportTable(
					  PHPEEXPORTTABLE phExportTable,
					  PSTR pszModuleName,
					  ULONG cExportedFunctions,
					  ULONG cExportedNames,
					  ULONG ulOrdinalBase,
					  PVOID *pExportNameList,
					  PUSHORT pExportOrdinals,
					  PVOID *pExportAddrs,
					  PHLIST phList
					  );

extern t_PELDR_AddExportTable   rPELDR_AddExportTable;

//---------- NDIS stuff  ---------------------------------------------------------
#define Ndis_Device_ID	0x00028
#define Ndis_DEVICE_ID	0x00028

#define	Ndis_Service	Declare_Service
Begin_Service_Table(NDIS)

    Ndis_Service (NDISGetVersion)

    Ndis_Service (NDISAllocateSpinLock)
    Ndis_Service (NDISFreeSpinLock)
    Ndis_Service (NDISAcquireSpinLock)
    Ndis_Service (NDISReleaseSpinLock)

    Ndis_Service (NDISOpenConfiguration)
    Ndis_Service (NDISReadConfiguration)
    Ndis_Service (NDISCloseConfiguration)
    Ndis_Service (NDISReadEisaSlotInformation)
    Ndis_Service (NDISReadMcaPosInformation)

    Ndis_Service (NDISAllocateMemory)
    Ndis_Service (NDISFreeMemory)
    Ndis_Service (NDISSetTimer)
    Ndis_Service (NDISCancelTimer)
    Ndis_Service (NDISStallExecution)
    Ndis_Service (NDISInitializeInterrupt)
    Ndis_Service (NDISRemoveInterrupt)
    Ndis_Service (NDISSynchronizeWithInterrupt)
    Ndis_Service (NDISOpenFile)
    Ndis_Service (NDISMapFile)
    Ndis_Service (NDISUnmapFile)
    Ndis_Service (NDISCloseFile)

    Ndis_Service (NDISAllocatePacketPool)
    Ndis_Service (NDISFreePacketPool)
    Ndis_Service (NDISAllocatePacket)
    Ndis_Service (NDISReinitializePacket)
    Ndis_Service (NDISFreePacket)
    Ndis_Service (NDISQueryPacket)

    Ndis_Service (NDISAllocateBufferPool)
    Ndis_Service (NDISFreeBufferPool)
    Ndis_Service (NDISAllocateBuffer)
    Ndis_Service (NDISCopyBuffer)
    Ndis_Service (NDISFreeBuffer)
    Ndis_Service (NDISQueryBuffer)
    Ndis_Service (NDISGetBufferPhysicalAddress)
    Ndis_Service (NDISChainBufferAtFront)
    Ndis_Service (NDISChainBufferAtBack)
    Ndis_Service (NDISUnchainBufferAtFront)
    Ndis_Service (NDISUnchainBufferAtBack)
    Ndis_Service (NDISGetNextBuffer)
    Ndis_Service (NDISCopyFromPacketToPacket)

    Ndis_Service (NDISRegisterProtocol)
    Ndis_Service (NDISDeregisterProtocol)
    Ndis_Service (NDISOpenAdapter)
    Ndis_Service (NDISCloseAdapter)
    Ndis_Service (NDISSend)
    Ndis_Service (NDISTransferData)
    Ndis_Service (NDISReset)
    Ndis_Service (NDISRequest)

    Ndis_Service (NDISInitializeWrapper)
    Ndis_Service (NDISTerminateWrapper)
    Ndis_Service (NDISRegisterMac)
    Ndis_Service (NDISDeregisterMac)
    Ndis_Service (NDISRegisterAdapter)
    Ndis_Service (NDISDeregisterAdapter)
    Ndis_Service (NDISCompleteOpenAdapter)
    Ndis_Service (NDISCompleteCloseAdapter)
    Ndis_Service (NDISCompleteSend)
    Ndis_Service (NDISCompleteTransferData)
    Ndis_Service (NDISCompleteReset)
    Ndis_Service (NDISCompleteRequest)
    Ndis_Service (NDISIndicateReceive)
    Ndis_Service (NDISIndicateReceiveComplete)
    Ndis_Service (NDISIndicateStatus)
    Ndis_Service (NDISIndicateStatusComplete)
    Ndis_Service (NDISCompleteQueryStatistics)

    Ndis_Service (NDISEqualString)
    Ndis_Service (NDISRegAdaptShutdown)
    Ndis_Service (NDISReadNetworkAddress)

    Ndis_Service (NDISWriteErrorLogEntry)

    Ndis_Service (NDISMapIoSpace)
    Ndis_Service (NDISDeregAdaptShutdown)

    Ndis_Service (NDISAllocateSharedMemory)
    Ndis_Service (NDISFreeSharedMemory)

    Ndis_Service (NDISAllocateDmaChannel)
    Ndis_Service (NDISSetupDmaTransfer)
    Ndis_Service (NDISCompleteDmaTransfer)
    Ndis_Service (NDISReadDmaCounter)
    Ndis_Service (NDISFreeDmaChannel)
    Ndis_Service (NDISReleaseAdapterResources)
    Ndis_Service (NDISQueryGlobalStatistics)

    Ndis_Service (NDISOpenProtocolConfiguration)
    Ndis_Service (NDISCompleteBindAdapter)
    Ndis_Service (NDISCompleteUnbindAdapter)
    Ndis_Service (WrapperStartNet)
    Ndis_Service (WrapperGetComponentList)
    Ndis_Service (WrapperQueryAdapterResources)
    Ndis_Service (WrapperDelayBinding)
    Ndis_Service (WrapperResumeBinding)
    Ndis_Service (WrapperRemoveChildren)
    Ndis_Service (NDISImmediateReadPciSlotInformation)
    Ndis_Service (NDISImmediateWritePciSlotInformation)
    Ndis_Service (NDISReadPciSlotInformation)
    Ndis_Service (NDISWritePciSlotInformation)
    Ndis_Service (NDISPciAssignResources)
    Ndis_Service (NDISQueryBufferOffset)

    Ndis_Service (NDISMWanSend)
//    Ndis_Service (DbgPrint)

    Ndis_Service (NDISInitializeEvent)
    Ndis_Service (NDISSetEvent)
    Ndis_Service (NDISResetEvent)
    Ndis_Service (NDISWaitEvent)
End_Service_Table(NDIS)




extern REGISTER_PROTOCOL    rNdisRegisterProtocol;
extern OPEN_ADAPTER	        rNdisOpenAdapter;
extern DEREGISTER_PROTOCOL	rNdisDeregisterProtocol;
extern CLOSE_ADAPTER	    rNdisCloseAdapter;

extern RETURN_PACKET	    rNdisReturnPackets;

extern MY_REQUEST_HANDLER   rNdisRequest;
extern MY_RESET_HANDLER     rNdisReset;

#endif