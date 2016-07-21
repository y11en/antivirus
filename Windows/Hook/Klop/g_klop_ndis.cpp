#include "g_klop_ndis.h"

NDIS_HANDLE     ProtocolHandle = NULL;
NDIS_HANDLE		WrapperHandle = NULL;

CKl_List<CKl_OpenInstance>* OI_List = NULL;
CKl_List<CKl_OpenInstance>* OI_BindList = NULL; 

#define ETH_HDR_SIZE 0xE

USHORT	dwLastID = 1;

REGISTER_PROTOCOL    rNdisRegisterProtocol;
OPEN_ADAPTER	     rNdisOpenAdapter;
DEREGISTER_PROTOCOL	 rNdisDeregisterProtocol;
CLOSE_ADAPTER	     rNdisCloseAdapter;

bool				 StopPacketProcessing = true;

VOID
NDIS_API
HNdisRegisterProtocol(
				OUT   PNDIS_STATUS    Status,
				OUT   PNDIS_HANDLE    NdisProtocolHandle,
				IN    PNDIS_PCHARS    ProtocolCharacteristics,
				IN    UINT            CharacteristicsLength
				)
{
	CKl_FuncDebug dd("HNdisRegisterProtocol");

	rNdisRegisterProtocol( Status, NdisProtocolHandle, ProtocolCharacteristics, CharacteristicsLength);
}

VOID
NDIS_API
HNdisOpenAdapter(
				OUT PNDIS_STATUS	Status,
				OUT PNDIS_STATUS	OpenErrorStatus,
				OUT PNDIS_HANDLE	NdisBindingHandle,
				OUT PUINT			SelectedMediumIndex,
				IN PNDIS_MEDIUM		MediumArray,
				IN UINT				MediumArraySize,
				IN NDIS_HANDLE		NdisProtocolHandle,
				IN NDIS_HANDLE		ProtocolBindingContext,
				IN PNDIS_STRING		AdapterName,
				IN UINT				OpenOptions,
				IN PSTRING			AddressingInformation  OPTIONAL
				)
{
	CKl_FuncDebug dd("HNdisOpenAdapter");
	
	CKl_OpenInstance* OI = NULL;
	CP_ADAPTER_INFO* AI = NULL;
	
	if ( OI_BindList )
	{
		while ( OI = OI_BindList->InterlockedRemoveHead() )
		{
			AI = (CP_ADAPTER_INFO*)ExAllocatePoolWithTag ( NonPagedPool, sizeof( CP_ADAPTER_INFO ) + OI->NameSize, 'IA_G' );
			
			if ( AI )
			{
				RtlCopyMemory ( AI->AdapterBuffer, OI->pAdapterName, OI->NameSize );
				AI->AdapterNameSize = OI->NameSize;
				AI->LocalIp = OI->CurrentIP;
				AI->Version = KLOP_API_VERSION;
				CP_AddAdapter( AI );

				ExFreePool( AI );
			}
			
			delete OI;
		}
	}


	rNdisOpenAdapter(   Status, 
                        OpenErrorStatus,
                        NdisBindingHandle,
                        SelectedMediumIndex,
                        MediumArray,
                        MediumArraySize,
                        NdisProtocolHandle,
                        ProtocolBindingContext,
                        AdapterName,
                        OpenOptions,
                        AddressingInformation );
}

VOID
NDIS_API
HNdisCloseAdapter(
				OUT PNDIS_STATUS	Status,
				IN  NDIS_HANDLE		NdisBindingHandle
				)
{
	CKl_FuncDebug dd("HNdisCloseAdapter");

#ifdef ISWIN9X
	// Это хрень, которая проверяет : если мы терминируемся, то выгрузим все нафик.
	// Ибо нефик !
	DWORD SystemState = VMM_GetSystemInitState();

	if ( SystemState > SYSTEM_EXIT )
	{
		CP_Done();
	}
#endif

	rNdisCloseAdapter( Status, NdisBindingHandle );
}

VOID
NDIS_API
HNdisDeregisterProtocol(
				OUT PNDIS_STATUS	Status,
				IN  NDIS_HANDLE		NdisProtocolHandle
				)
{
	CKl_FuncDebug dd("HNdisDeregisterProtocol");

	rNdisDeregisterProtocol( Status , NdisProtocolHandle );
}

NDIS_STATUS
SendRequest(
                NDIS_HANDLE         ProtocolBindingContext, 
                NDIS_REQUEST_TYPE   Type,
                NDIS_OID            Oid,
                PVOID               Buffer, 
                ULONG               BufferSize)
{   
	CKl_FuncDebug dd("SendRequest");

    NDIS_STATUS     Status;
    CKl_OpenInstance*  OI     = (CKl_OpenInstance*)ProtocolBindingContext;    
    CKl_OI_Request	OiRequest(ProtocolBindingContext);
    PNDIS_REQUEST	Request     = &OiRequest.m_Request;


    Request->RequestType = Type;
    Request->DATA.SET_INFORMATION.Oid = Oid;
    Request->DATA.SET_INFORMATION.InformationBuffer = Buffer;
    Request->DATA.SET_INFORMATION.InformationBufferLength = BufferSize;
	
	OI->m_RequestList.InsertTail( &OiRequest );
        
	OiRequest.m_Event.Clear();
	NdisRequest( &Status, OI->AdapterHandle, Request );

	if ( Status == NDIS_STATUS_PENDING )
	{		
		OiRequest.m_Event.Wait();
		Status = OiRequest.m_FunalStatus;
	}

	OI->m_RequestList.InterlockedRemove( &OiRequest );
	//delete OiRequest;

    return Status;
}

VOID 
NDIS_API
OnRequestComplete( IN NDIS_HANDLE	ProtocolBindingContext, 
			   IN PNDIS_REQUEST NdisRequest, 
			   IN NDIS_STATUS	Status ) 
{
	CKl_FuncDebug dd("OnRequestComplete");

    CKl_OpenInstance*	OI			= (CKl_OpenInstance*)ProtocolBindingContext;
	CKl_OI_Request*		OiRequest	= NULL;
    
	OI->m_RequestList.Lock();

	while ( OiRequest = OI->m_RequestList.Next( OiRequest) )
	{
		if  ( &OiRequest->m_Request == NdisRequest )
		{			
			break;
		}
	}
	
	OI->m_RequestList.Unlock();

    if ( OiRequest )
    {
        OiRequest->m_FunalStatus = Status;        
		OiRequest->m_Event.Set();
    }    
}

NDIS_STATUS
GetMacAddr(NDIS_HANDLE ProtocolBindingContext)
{
    CKl_OpenInstance*  OI = (CKl_OpenInstance*)ProtocolBindingContext;
    
    return SendRequest( ProtocolBindingContext,
                        NdisRequestQueryInformation,
                        OID_802_3_CURRENT_ADDRESS,
                        OI->CurrentMacAddr,
                        sizeof ( OI->CurrentMacAddr ) );
}

VOID 
NDIS_API
OnOpenAdapterComplete( 
				IN NDIS_HANDLE ProtocolBindingContext, 
				IN NDIS_STATUS Status, 
				IN NDIS_STATUS OpenErrorStatus ) 
{
	CKl_FuncDebug dd("OnOpenAdapterComplete");
    CKl_OpenInstance*  OI = (CKl_OpenInstance*)ProtocolBindingContext;
    
    if ( OI )
    {
        OI->Status = Status;
		OI->AdapterEvent.Set();
    }
}

VOID 
NDIS_API
OnCloseAdapterComplete( 
				IN NDIS_HANDLE ProtocolBindingContext, 
				IN NDIS_STATUS Status )
{
	CKl_FuncDebug dd("OnCloseAdapterComplete");

    CKl_OpenInstance*  OI = (CKl_OpenInstance*)ProtocolBindingContext;

    if ( OI )
    {
        OI->Status = Status;
		OI->AdapterEvent.Set();
    }
}

VOID 
NDIS_API
OnSendComplete( 
				IN NDIS_HANDLE	ProtocolBindingContext, 
				IN PNDIS_PACKET pPacket, 
				IN NDIS_STATUS	Status ) 
{
	CKl_FuncDebug dd("OnSendComplete");

	CKl_OpenInstance*  OI = (CKl_OpenInstance*)ProtocolBindingContext;

    if ( OI )
    {
		CKl_SendPkt* SendPkt = NULL;
		
		SendPkt = (CKl_SendPkt*) ( *(PVOID*)pPacket->ProtocolReserved );

		if ( SendPkt )
		{
			SendPkt->m_Status = Status;
			
			PNDIS_BUFFER    Buffer;
			PVOID           pVA;
			UINT           Length;
			
			NdisUnchainBufferAtFront( pPacket, &Buffer );
			
			NdisQueryBuffer( Buffer, &pVA, &Length );
			
			NdisFreeBuffer(Buffer);
			KL_MEM_FREE ( pVA );
			
			NdisFreePacket ( pPacket );

			delete SendPkt;
		}        
    }
}

VOID 
NDIS_API
OnResetComplete( 
				IN NDIS_HANDLE	ProtocolBindingContext, 
				IN NDIS_STATUS	Status ) 
{
    CKl_FuncDebug dd("OnResetComplete");
}

NDIS_STATUS 
NDIS_API
OnReceiveSmallPacket( 
				IN NDIS_HANDLE	ProtocolBindingContext, 
				IN NDIS_HANDLE	MacReceiveContext, 
				IN PVOID		HeaderBuffer, 
				IN UINT			HeaderBufferSize, 
				IN PVOID		LookAheadBuffer, 
				IN UINT			LookaheadBufferSize, 
				IN UINT			PacketSize ) 
{
    CKl_OpenInstance*  OI = (CKl_OpenInstance*)ProtocolBindingContext;
    PNDIS_PACKET    NdisPacket;
    PNDIS_BUFFER    NdisBuffer;
    NDIS_STATUS     Status;
    UINT            BytesTransferred = 0;;
    
    PVOID           pVA;
    PLIST_ENTRY     pLE;
    
    NdisAllocatePacket ( &Status, &NdisPacket, OI->mPacketPoolH );
    if ( Status != NDIS_STATUS_SUCCESS )
        return NDIS_STATUS_FAILURE;
    
    OI->BufferListLock->Acquire();
    pLE = RemoveHeadList( &OI->BufferList );                    
    OI->BufferListLock->Release();
    
    if ( pLE == NULL )
    {
        pLE = (PLIST_ENTRY)KL_MEM_ALLOC( ETH_HDR_SIZE + PacketSize);
    }
    
    if ( pLE == NULL )
    {
        NdisFreePacket ( NdisPacket );
        return NDIS_STATUS_FAILURE;
    }      
    
    pVA = pLE;
    
    RtlCopyMemory ( pVA, HeaderBuffer, ETH_HDR_SIZE );
	NdisAllocateBuffer ( &Status, &NdisBuffer, OI->mBufferPoolH, (PCHAR)pVA + ETH_HDR_SIZE, PacketSize );
	NdisAdjustBufferLength( NdisBuffer, PacketSize );
    
    NdisChainBufferAtBack( NdisPacket, NdisBuffer );
    
    // трансфер
    NdisTransferData( 
        &Status, 
        OI->AdapterHandle, 
        MacReceiveContext, 
        0, 
        PacketSize, 
        NdisPacket, 
        &BytesTransferred);
    
    if ( NDIS_STATUS_PENDING != Status )
    {
        OnTransferDataComplete( ProtocolBindingContext, NdisPacket, Status, BytesTransferred );
    }
    
    return Status;
}

NDIS_STATUS 
NDIS_API
OnReceive( 
				IN NDIS_HANDLE	ProtocolBindingContext, 
				IN NDIS_HANDLE	MacReceiveContext, 
				IN PVOID		HeaderBuffer, 
				IN UINT			HeaderBufferSize, 
				IN PVOID		LookAheadBuffer, 
				IN UINT			LookaheadBufferSize, 
				IN UINT			PacketSize ) 
{
	
  	CKl_OpenInstance* OI = (CKl_OpenInstance*)ProtocolBindingContext;
    
    PETH_HEADER     eth = (PETH_HEADER)HeaderBuffer;
    PIP_HEADER      ip  = (PIP_HEADER)LookAheadBuffer;

	if ( ( OI->Flags & OI_INITIALIZED ) == NULL )
		return NDIS_STATUS_FAILURE;
	
	if ( StopPacketProcessing )
		return NDIS_STATUS_FAILURE;

    if ( eth->Type == htons ( ETH_P_IP ) )
    {
        if ( LookaheadBufferSize >= IP_HEADER_SIZE )
        {
            if ( ip->Protocol == CP_PROTOCOL )
            {
				if ( KlCompareMemory( eth->srcMac, OI->CurrentMacAddr, sizeof ( eth->srcMac ) ) )
				{	// свои собственные пакеты не рассматриваем !
					return KL_SUCCESS;
				}
				
                if ( LookaheadBufferSize < PacketSize )
                {
                    return OnReceiveSmallPacket( 
                                            ProtocolBindingContext,
                                            MacReceiveContext,
                                            HeaderBuffer,
                                            HeaderBufferSize,
                                            LookAheadBuffer,
                                            LookaheadBufferSize,
                                            PacketSize);
                }
                    
                OnPacketReceive( ProtocolBindingContext, HeaderBuffer, LookAheadBuffer, LookaheadBufferSize );
            }

            return NDIS_STATUS_SUCCESS;
        }
        
        return OnReceiveSmallPacket( 
            ProtocolBindingContext,
            MacReceiveContext,
            HeaderBuffer,
            HeaderBufferSize,
            LookAheadBuffer,
            LookaheadBufferSize,
            PacketSize);
    }

    
    return NDIS_STATUS_SUCCESS;
}

VOID 
NDIS_API
OnTransferDataComplete ( 
				IN NDIS_HANDLE     ProtocolBindingContext, 
				IN PNDIS_PACKET    NdisPacket, 
				IN NDIS_STATUS     Status, 
				IN UINT            BytesTransfered ) 
{
    PLIST_ENTRY     pLE;
    PVOID           pVA;
    UINT            TotalLength, Length;
    PNDIS_BUFFER    Buffer;
    CKl_OpenInstance*  OI = ( CKl_OpenInstance* )ProtocolBindingContext;
    
    // NdisGetFirstBufferFromPacket( NdisPacket, &Buffer, &pVA, &Length, &TotalLength );
	NdisQueryPacket( NdisPacket, NULL, NULL, &Buffer, &TotalLength );
	NdisQueryBuffer( Buffer, &pVA, &Length );
	
	pVA = (PCHAR)pVA - ETH_HDR_SIZE;
    
    if ( Status == NDIS_STATUS_SUCCESS )
    {
        OnPacketReceive( ProtocolBindingContext, pVA, (PCHAR)pVA + ETH_HDR_SIZE, BytesTransfered );		
    }
	
    NdisFreeBuffer(Buffer);
    
    OI->BufferListLock->Acquire();
    
    pLE = (PLIST_ENTRY)pVA;
    
    InsertTailList( &OI->BufferList, pLE );
    
    OI->BufferListLock->Release();
    
    NdisReinitializePacket( NdisPacket );
    
    NdisFreePacket ( NdisPacket );
}

VOID 
NDIS_API
OnReceiveComplete( 
				IN NDIS_HANDLE	ProtocolBindingContext
				) 
{	
    
}

VOID 
NDIS_API
OnStatus( 
				IN NDIS_HANDLE	ProtocolBindingContext, 
				IN NDIS_STATUS	Status, 
				IN PVOID		StatusBuffer, 
				IN UINT			StatusBufferSize 
				) 
{    
	CKl_FuncDebug dd("OnStatus");	
}


VOID 
NDIS_API
OnStatusComplete( 
				IN NDIS_HANDLE	ProtocolBindingContext 
				) 
{	
	CKl_FuncDebug dd("OnStatusComplete");    
}

VOID 
OnBindAdapter(
				OUT PNDIS_STATUS Status,
				IN  NDIS_HANDLE  BindContext,
				IN  PNDIS_STRING DeviceName,
				IN  PVOID		 SystemSpecific1,
				IN  PVOID		 SystemSpecific2
				)
{
	CKl_FuncDebug dd("OnBindAdapter");	
    *Status = NDIS_STATUS_SUCCESS;
	
    return;
}

VOID
OnUnbindAdapter(
				OUT PNDIS_STATUS Status,
				IN  NDIS_HANDLE  ProtocolBindingContext,
				IN  NDIS_HANDLE  UnbindContext
				)
{
	// короче ship happends
	// какая-то лажо выполняет unbind ... возможно это hibernater or something ...
	// в-общем адаптер придется закрыть ... а потом открыть, когда будет BindAdapter ...
	CKl_FuncDebug dd("OnUnbindAdapter");	
	
	NDIS_STATUS	MyStatus;
	CKl_OpenInstance* OI = (CKl_OpenInstance*)ProtocolBindingContext;
	CKl_OpenInstance* Temp = NULL;

	OI_List->Lock();
	while ( Temp = OI_List->Next( Temp ) )
	{
		if ( Temp == OI )
			break;
	}
	OI_List->Unlock();

	if ( Temp )
	{
		OI->Flags &= ~OI_INITIALIZED;
		
		OI_List->InterlockedRemove( OI );
		OI_BindList->InsertTail( OI );
		
		OI->AdapterEvent.Clear();
		rNdisCloseAdapter( &MyStatus, OI->AdapterHandle );
		
		if ( MyStatus == NDIS_STATUS_PENDING )
			OI->AdapterEvent.Wait();
	}

    *Status = NDIS_STATUS_SUCCESS;
	
    return;
}