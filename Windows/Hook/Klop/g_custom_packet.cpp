#include "kldef.h"
#include "g_custom_packet.h"
#include "g_cp_proto.h"

// decimal
#define SIG_ETH_TYPE (USHORT)22978

ETH_HEADER Eth;

unsigned char BroadcastMac[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

unsigned short ip_sum(unsigned short *ptr,int nbytes) 
{
	register long           sum;
	USHORT                 oddbyte;
	register USHORT        answer;
	
	sum = 0;
	while (nbytes > 1)  
	{
		sum += *ptr++;
		nbytes -= 2;
	}
	
	if (nbytes == 1) 
	{
		oddbyte = 0;
		*((UCHAR *) &oddbyte) = *(UCHAR *)ptr;   /* one byte only */
		sum += oddbyte;
	}
	
	sum  = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);                 
	answer = (USHORT) ~sum;
	return(answer);
}

KLSTATUS
OnPacketReceive( NDIS_HANDLE ProtocolBindingContext, PVOID HeaderBuffer, PVOID  IpPacket, ULONG PacketSize )
{    
    CKl_OpenInstance*  OI = (CKl_OpenInstance*)ProtocolBindingContext;
    PIP_HEADER  ip = (PIP_HEADER)IpPacket;
    PCP_PACKET*	Packet;
	PETH_HEADER	eth = (PETH_HEADER)HeaderBuffer;

	if ( 0 == ( OI->Flags & OI_INITIALIZED ) )
		return KL_UNSUCCESSFUL;

	KL_TIME		CurrentTime;
	KlGetSystemTime( &CurrentTime );

	/*
	USHORT  FragmentOffset  = htons ( ip->Flg_FragOffs & 0xFF1F ) << 3;
	bool bMF       = !!( ip->Flg_FragOffs & 0x0020 );
    bool bDF       = !!( ip->Flg_FragOffs & 0x0040 );

	if ( (FragmentOffset == 0 && bMF == 0) || bDF )
    {        
        return KL_SUCCESS;
    }
	*/

    if ( ip->Protocol != CP_PROTOCOL )
        return KL_SUCCESS;

    Packet = (PCP_PACKET*)( (char*)IpPacket + ip->Ihl * sizeof ( PVOID ) );
	
	if ( Packet->version != PCP_VERSION )
		return KL_UNSUCCESSFUL;
	
	if ( Packet->type == PCP_REQUEST_PACKET )
		PCP_RecvRequest( (char*)OI->CurrentMacAddr, eth, Packet );

	if ( CurrentTime > g_StopTime )
		return KL_SUCCESS;

	if ( Packet->type == PCP_ANSWER_PACKET )
		PCP_RecvAnswer( (char*)OI->CurrentMacAddr, eth, Packet);

    return KL_SUCCESS;
}

/*
 *	Инициализируется Protocol
 */
KLSTATUS
CP_Init()
{
    NDIS_STATUS Status;

	if ( ProtocolHandle != NULL )
	{
		StopPacketProcessing = false;
		return KL_SUCCESS;	
	}

	OI_List		= new CKl_List<CKl_OpenInstance>();
	OI_BindList = new CKl_List<CKl_OpenInstance>();

#ifdef ISWIN9X    
	NDIS_STRING	aProtoName = NDIS_STRING_CONST( (PUCHAR)"klop" );
#else
	NDIS_STRING	aProtoName = NDIS_STRING_CONST( "klop" );
#endif

    NDIS_PCHARS PChars;
    char        rsvd[60];

    RtlZeroMemory ( &PChars, sizeof ( PChars ) );
	RtlZeroMemory ( &rsvd,   sizeof ( rsvd   ) );

    if ( !isWin98)
    {
		PChars.MajorNdisVersion            = 4;
		PChars.MinorNdisVersion            = 0;
		PChars.Reserved                    = 0;
		PChars.OpenAdapterCompleteHandler  = OnOpenAdapterComplete;
		PChars.CloseAdapterCompleteHandler = OnCloseAdapterComplete;
		PChars.SendCompleteHandler         = OnSendComplete;
		PChars.TransferDataCompleteHandler = OnTransferDataComplete;
		PChars.ResetCompleteHandler        = OnResetComplete;
		PChars.RequestCompleteHandler      = OnRequestComplete;
		PChars.ReceiveHandler              = OnReceive;
		PChars.ReceiveCompleteHandler      = OnReceiveComplete;
		PChars.StatusHandler               = OnStatus;
		PChars.StatusCompleteHandler       = OnStatusComplete;
		PChars.Name                        = aProtoName;
		
		// PChars.PnPEventHandler              = OnPnPEvent;
		PChars.BindAdapterHandler           = OnBindAdapter;
		PChars.UnbindAdapterHandler         = OnUnbindAdapter;    
    }
    else
    {
        PNDIS_PCHARS_9X MyPChars = (PNDIS_PCHARS_9X)&PChars;
        
        MyPChars->MajorNdisVersion            = 4;
        MyPChars->MinorNdisVersion            = 0;
        MyPChars->Reserved                    = 0;
        MyPChars->OpenAdapterCompleteHandler  = OnOpenAdapterComplete;
        MyPChars->CloseAdapterCompleteHandler = OnCloseAdapterComplete;
        MyPChars->SendCompleteHandler         = OnSendComplete;
        MyPChars->TransferDataCompleteHandler = OnTransferDataComplete;
        MyPChars->ResetCompleteHandler        = OnResetComplete;
        MyPChars->RequestCompleteHandler      = OnRequestComplete;
        MyPChars->ReceiveHandler              = OnReceive;
        MyPChars->ReceiveCompleteHandler      = OnReceiveComplete;
        MyPChars->StatusHandler               = OnStatus;
        MyPChars->StatusCompleteHandler       = OnStatusComplete;
        MyPChars->Name                        = aProtoName;
    }

    rNdisRegisterProtocol( &Status, &ProtocolHandle, &PChars, sizeof ( PChars )  );

    if ( NDIS_STATUS_SUCCESS != Status )
    {
        return KL_UNSUCCESSFUL;
    }

	StopPacketProcessing = false;
    
    return KL_SUCCESS;
}

/*
 *	Протокол открывает адаптер по имени, которое нам передается
 */
KLSTATUS
CP_AddAdapter( CP_ADAPTER_INFO* pAI )
{
	CKl_FuncDebug dd("CP_AddAdapter");
	
    NDIS_STATUS			Status, ErrorStatus;
    CKl_OpenInstance*   OI				= NULL;        
	bool				bFound			= false;    
    NDIS_MEDIUM			MediumArray[]	= { NdisMedium802_3 };
	
	OI = new CKl_OpenInstance( pAI->AdapterBuffer, pAI->AdapterNameSize );
	
	if ( OI == NULL )
		return KL_UNSUCCESSFUL;

#ifdef ISWIN9X
	NDIS_STRING AdapterName = { 
			(USHORT)pAI->AdapterNameSize, 
			(USHORT)pAI->AdapterNameSize, 
			(PUCHAR)OI->pAdapterName 
	};
#else
	NDIS_STRING AdapterName;
	NdisInitUnicodeString ( &AdapterName, (wchar_t*)OI->pAdapterName );
#endif

	OI->CurrentIP		= htonl ( pAI->LocalIp );

	rNdisOpenAdapter( 
		&Status,
		&ErrorStatus,
		&OI->AdapterHandle,
		&(OI->Medium),
		MediumArray,
		sizeof ( MediumArray ),
		ProtocolHandle,
		OI,
		&AdapterName,
		0,
		NULL
		);
		
	if ( Status == NDIS_STATUS_PENDING )
	{		
		OI->AdapterEvent.Wait();
		Status = OI->Status;
	}

	while ( Status == NDIS_STATUS_SUCCESS )
	{		
		OI->Flags |= OI_ADAPTER_OPENED; 
		OI_List->InsertTail( OI );

		SendRequest( 
			OI, 
			NdisRequestSetInformation,
			OID_GEN_PHYSICAL_MEDIUM,
			&OI->PhysicalMedium,
			sizeof ( OI->PhysicalMedium ) );

		if ( OI->PhysicalMedium == NdisPhysicalMedium1394 )
		{
			OI_List->InterlockedRemove( OI );
			break;
		}
		
		if ( NDIS_STATUS_SUCCESS !=  SendRequest( 
										OI, 
										NdisRequestSetInformation,
										OID_GEN_CURRENT_PACKET_FILTER,
										&OI->AdapterState,
										sizeof ( OI->AdapterState ) ) )
		{
			OI_List->InterlockedRemove( OI );
			break;
		}
		
		if ( NDIS_STATUS_SUCCESS != SendRequest( 
										OI,
										NdisRequestQueryInformation,
										OID_802_3_CURRENT_ADDRESS,
										OI->CurrentMacAddr,
										sizeof ( OI->CurrentMacAddr ) ) )
		{
			OI_List->InterlockedRemove( OI );
			break;
		}
		
		if ( NDIS_STATUS_SUCCESS != SendRequest( 
										OI,
										NdisRequestQueryInformation,
										OID_GEN_MAXIMUM_FRAME_SIZE,
										&OI->MaxFrameSize,
										sizeof ( OI->MaxFrameSize ) ) )
		{
			OI->MaxFrameSize = 1520;
		}

		OI->Flags |= OI_INITIALIZED;

		return KL_SUCCESS;
	}

	if ( OI->Flags & OI_ADAPTER_OPENED )
	{
		OI->AdapterEvent.Clear();
		rNdisCloseAdapter( &Status, OI->AdapterHandle );

		if ( Status == NDIS_STATUS_PENDING )
			OI->AdapterEvent.Wait();
	}
	
	delete OI;
	return KL_UNSUCCESSFUL;	
}

KLSTATUS
CP_SetBroadcast(CP_IFACE_INFO* pII, ULONG Count )
{
	CKl_FuncDebug dd("CP_SetBroadcast");

    CKl_OpenInstance* OI = NULL;
	
	CKl_AutoLock Lock ( &OI_List->m_Lock.Lock);

	while ( OI = OI_List->Next( OI ) )
	{
		for ( ULONG i = 0; i < Count; i++ )
		{
			if ( KlCompareMemory ( OI->CurrentMacAddr, pII[i].MacAddr, sizeof ( OI->CurrentMacAddr ) ) )
			{
				OI->Flags |= OI_HAS_MAC;
				OI->CurrentIP = pII[i].IpAddr;
			}
		}
	}	

	return KL_SUCCESS;
}

/*
 *	Закрываем адаптер по имени.
 */
KLSTATUS
CP_RemoveAdapter(TCHAR* AdapterName, ULONG AdapterNameSize)
{
	CKl_FuncDebug dd("CP_RemoveAdapter");

    NDIS_STATUS     Status;
	CKl_OpenInstance* OI = NULL;
    
	OI_List->Lock();

    while ( OI = OI_List->Next( OI ) )
    {
    	if ( KlCompareMemory ( OI->pAdapterName, AdapterName, AdapterNameSize ) )
        {
            OI_List->Remove( OI );
			OI->Flags &= ~OI_INITIALIZED;

			break;            
        }
    }

	OI_List->Unlock();
	
	if ( OI )
	{
		if ( OI->AdapterHandle )
		{			
			OI->AdapterEvent.Clear();

			rNdisCloseAdapter( &Status, OI->AdapterHandle );
			
			if ( Status == NDIS_STATUS_PENDING )
				OI->AdapterEvent.Wait();
		}
		
		delete OI;
		
		return KL_SUCCESS;
	}	
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
CP_Done()
{
	CKl_FuncDebug dd("CP_Done");

	NDIS_STATUS     Status;
    CKl_OpenInstance*  OI = NULL;

	StopPacketProcessing = true;

	if ( OI_List )
	{	
		while ( OI = OI_List->InterlockedRemoveHead() )
		{
			if ( OI->AdapterHandle )
			{				
				OI->AdapterEvent.Clear();
				rNdisCloseAdapter( &Status, OI->AdapterHandle );

				if ( Status == NDIS_STATUS_PENDING )					
					OI->AdapterEvent.Wait();
			}
			
			delete OI;
		}

		if ( OI_BindList )
		{
			while ( OI = OI_BindList->InterlockedRemoveHead() )
			{
				delete OI;
			}
		}		
	}
	
	return KL_SUCCESS;
}

KLSTATUS
CP_Send(char* Packet, ULONG PacketSize)
{
	CKl_FuncDebug dd("CP_Send");

    CKl_OpenInstance*  OI = NULL;

    CKl_AutoLock Lock ( &OI_List->m_Lock.Lock );

    while ( OI = OI_List->Next( OI ) )
	{
        
        NDIS_STATUS Status;
        PNDIS_BUFFER Buffer;
        PNDIS_PACKET NdisPacket;
        
        PVOID   pVA = KL_MEM_ALLOC ( PacketSize );
        
        NdisAllocateBuffer(&Status, &Buffer, OI->mBufferPoolH, pVA, PacketSize );
        
        if ( Status != NDIS_STATUS_SUCCESS )
        {
            return KL_UNSUCCESSFUL;
        }
        
        NdisAllocatePacket ( &Status, &NdisPacket, OI->mPacketPoolH );
        
        if ( Status != NDIS_STATUS_SUCCESS )
        {
            NdisFreeBuffer( Buffer );
            KL_MEM_FREE ( pVA );
            return KL_UNSUCCESSFUL;
        }
        
        RtlCopyMemory ( pVA, Packet, PacketSize );
        
        NdisChainBufferAtFront( NdisPacket, Buffer );
        
        // NdisRecalculatePacketCounts ( NdisPacket );
        
        NdisSend ( &Status, OI->AdapterHandle, NdisPacket );
        
        if ( Status != NDIS_STATUS_PENDING )
        {
            OnSendComplete( (NDIS_HANDLE)OI, NdisPacket, Status );
        }
        
    }
    
    return KL_SUCCESS;
}

KLSTATUS
CP_Receive(char* Packet, ULONG* PacketSize)
{
	CKl_FuncDebug dd("CP_Receive");

    return KL_SUCCESS;
}

KLSTATUS
CP_SendLicBuffer(char* Buffer, ULONG BufferSize)
{
	CKl_FuncDebug dd("CP_SendLicBuffer");

    CKl_OpenInstance*  OI = NULL ;
    
    CKl_AutoLock Lock ( &OI_List->m_Lock.Lock );

    while ( OI = OI_List->Next( OI ) )
	{
        if ( NULL == (OI->Flags & OI_INITIALIZED) )
			continue;

		if ( NULL == (OI->Flags & OI_HAS_MAC) )
			continue;

        NDIS_STATUS Status;
        PNDIS_BUFFER NdisBuffer;
        PNDIS_PACKET NdisPacket;
        
        ULONG   PacketSize = sizeof ( ETH_HEADER) + IP_HEADER_SIZE + BufferSize;
        PVOID   pVA = KL_MEM_ALLOC ( PacketSize + 100 );

        if ( pVA == NULL )
            return KL_UNSUCCESSFUL;

        PETH_HEADER eth  = (PETH_HEADER)pVA;
        PIP_HEADER  ip   = (PIP_HEADER) ( (char*)pVA + sizeof ( ETH_HEADER ) );
        PVOID       data = (PVOID)      ( (char*)ip  + IP_HEADER_SIZE );
        
        NdisAllocateBuffer(&Status, &NdisBuffer, OI->mBufferPoolH, pVA, PacketSize );
        
        if ( Status != NDIS_STATUS_SUCCESS )
        {
            return KL_UNSUCCESSFUL;
        }
        
        NdisAllocatePacket ( &Status, &NdisPacket, OI->mPacketPoolH );
        
        if ( Status != NDIS_STATUS_SUCCESS )
        {
            NdisFreeBuffer( NdisBuffer );
            KL_MEM_FREE ( pVA );
            return KL_UNSUCCESSFUL;
        }
        
        RtlCopyMemory ( eth->srcMac, OI->CurrentMacAddr, sizeof ( OI->CurrentMacAddr ) );
        RtlCopyMemory ( eth->dstMac, BroadcastMac , sizeof( BroadcastMac ) );
        eth->Type       = htons ( ETH_P_IP );

        ip->srcIP           = OI->CurrentIP;
        ip->dstIP           = 0xFFFFFFFF;
        ip->Ihl             = 5;
        ip->Version         = 4;
        ip->Protocol        = CP_PROTOCOL;
        ip->TimeToLive      = 255;
        ip->TypeOfService   = 0;
        ip->Flg_FragOffs    = 0;
        ip->TotalLength     = htons ( ((USHORT)( IP_HEADER_SIZE + BufferSize)) );
        ip->Identification  = htons ( dwLastID++ );
        ip->HeaderChecksum  = 0;
        ip->HeaderChecksum  = ip_sum( (USHORT*)ip, IP_HEADER_SIZE);        

        RtlCopyMemory ( data, Buffer, BufferSize );
        
        NdisChainBufferAtFront( NdisPacket, NdisBuffer );
        
        // NdisRecalculatePacketCounts ( NdisPacket );
        CKl_SendPkt* SendPkt = new CKl_SendPkt( NdisPacket );		
        NdisSend ( &Status, OI->AdapterHandle, NdisPacket );

		if ( Status != NDIS_STATUS_PENDING )
        {
            OnSendComplete( OI, NdisPacket, Status );
        }
    }

    return KL_SUCCESS;
}


CKl_OpenInstance::CKl_OpenInstance(PVOID	AdapterName, ULONG AdapterNameSize)
{
	CKl_FuncDebug dd("CKl_OpenInstance::CKl_OpenInstance");

	NDIS_STATUS	Status;
	Flags		= 0;
	MaxFrameSize = 1520;
	PhysicalMedium = NdisPhysicalMediumUnspecified;
	
	NdisAllocatePacketPool( &Status, &mPacketPoolH, 32, 4 * sizeof ( PVOID ) );
	
	if ( Status == NDIS_STATUS_SUCCESS )
	{		
		Flags |= OI_PACKET_POOL;
	}
	
	NdisAllocateBufferPool( &Status, &mBufferPoolH, 32 );        
	
	if ( Status == NDIS_STATUS_SUCCESS )
	{
		Flags |= OI_BUFFER_POOL;
	}
	
	InitializeListHead( &BufferList );
	BufferListLock = new CKl_Lock();
	
	pAdapterName = (wchar_t*)KL_MEM_ALLOC ( AdapterNameSize );
	if ( pAdapterName )
	{
		NameSize = AdapterNameSize;
		RtlCopyMemory ( pAdapterName, AdapterName, AdapterNameSize );
		Flags |= OI_ADAPTER_NAME;
	}

	AdapterState = NDIS_PACKET_TYPE_PROMISCUOUS;
}

CKl_OpenInstance::~CKl_OpenInstance()
{	
	CKl_FuncDebug dd("CKl_OpenInstance::~CKl_OpenInstance");
	
	if ( Flags & OI_BUFFER_POOL )
		NdisFreeBufferPool( mBufferPoolH );
	
	if ( Flags & OI_PACKET_POOL )
		NdisFreePacketPool( mPacketPoolH );
	
	if ( BufferListLock )
	{
		BufferListLock->Acquire();
		
		while ( !IsListEmpty( &BufferList ) )
		{
			PLIST_ENTRY pLE = (PLIST_ENTRY)RemoveHeadList( &BufferList );
			
			KL_MEM_FREE ( pLE );
		}
		
		BufferListLock->Release();
		
		delete BufferListLock;
	}
	
	if ( Flags & OI_ADAPTER_NAME )
		KL_MEM_FREE ( pAdapterName );
}