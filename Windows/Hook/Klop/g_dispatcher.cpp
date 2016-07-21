#include "g_dispatcher.h"
#include "kl_ioctl.h"

UCHAR	isWin98 = FALSE;
HANDLE	ClientPID = INVALID_HANDLE_VALUE;

PKLIM_TABLE ext_KlimTable;

KLSTATUS
KlopGetVersion(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{
	CHECK_OUT_BUFFER_SIZE( sizeof ( ULONG ) );

	*nReturned = sizeof ( ULONG );
	*(ULONG*)pOutBuffer = KLOP_API_VERSION;
	
	return KL_SUCCESS;
}

KLSTATUS
KlopInit(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{		
	PCP_INIT_INFO InitInfo = (PCP_INIT_INFO)pInBuffer;

	if ( InBufferSize < sizeof ( CP_INIT_INFO ) )
		return KL_BUFFER_TOO_SMALL;
	if ( InBufferSize < sizeof ( CP_INIT_INFO ) + InitInfo->KeySize )
		return KL_BUFFER_TOO_SMALL;

    ext_KlimTable = NULL;

#ifdef KLOP_PLUGIN

    CKl_Device Klim(L"\\device\\klim6");

    if ( Klim.m_DevicePtr == NULL )
        Klim.m_DevicePtr = Klim.FindDeviceByName( L"\\device\\klim5" );

    if ( KL_SUCCESS != Klim.SendIoctl( KLIM_GET_TABLE, NULL, 0, &ext_KlimTable, sizeof ( PVOID ), NULL ) )
    {
        return KL_UNSUCCESSFUL;
    }

#endif

	if ( KL_SUCCESS == PCP_Init(InitInfo->KeyBuffer, InitInfo->KeySize) )
	{
		if ( KL_SUCCESS == CP_Init() )
		{
            // запоминаем клиента. Если он отвалится, то мы все деинициализируем.
			ClientPID = PsGetCurrentProcessId();
			return KL_SUCCESS;
		}

		PCP_InterlockedDone();
	}	
	
	return KL_UNSUCCESSFUL;
}

KLSTATUS
KlopAddAdapter(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{	
	CP_ADAPTER_INFO* pAI = ( CP_ADAPTER_INFO* )pInBuffer;
	if ( InBufferSize < sizeof ( CP_ADAPTER_INFO ) )
		return KL_BUFFER_TOO_SMALL;
	if ( InBufferSize < sizeof ( CP_ADAPTER_INFO ) + pAI->AdapterNameSize )
		return KL_BUFFER_TOO_SMALL;

	return CP_AddAdapter( pAI );
}

KLSTATUS
KlopSetBroadcastAddr(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{	
	PCP_IFACE_INFO  ii = (PCP_IFACE_INFO)pInBuffer;
	ULONG			Count = InBufferSize / sizeof ( CP_IFACE_INFO );
	
	if ( InBufferSize < sizeof( CP_IFACE_INFO ) )
		return KL_BUFFER_TOO_SMALL;

	return CP_SetBroadcast( ii, Count );
}

KLSTATUS
KlopDone(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{
	ClientPID = INVALID_HANDLE_VALUE;
	
	PCP_InterlockedDone();
	CP_Done();
	
	return KL_SUCCESS;
}

KLSTATUS
KlopSendPacket(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{
    if ( InBufferSize < 0xE + 0x14 )
        return KL_BUFFER_TOO_SMALL;

	return CP_Send( (char*)pInBuffer, InBufferSize );
}

KLSTATUS
KlopActivateKey(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{
	// InBuffer = TimeToWait;
	// PCP_SendRequest( RangeList );
	if ( InBufferSize < sizeof ( ULONG ) )
		return KL_BUFFER_TOO_SMALL;

	ULONG	TimeToWait = *(ULONG*)pInBuffer;
	KL_TIME	CurrentTime;
	KlGetSystemTime( &CurrentTime );
	
	g_StopTime = CurrentTime + ( ONE_SECOND_TIME / 1000 ) * TimeToWait;

	PCP_SendRequest();

	return KL_SUCCESS;
}

KLSTATUS
KlopGetMacs( 
                IN  PVOID  pInBuffer, 
                IN  ULONG  InBufferSize, 
                IN  PVOID  pOutBuffer, 
                IN  ULONG  OutBufferSize, 
                OUT ULONG* nReturned )
{
    ULONG KeyCount = PCP_GetKeyItems( (char*)pOutBuffer, OutBufferSize );

    if ( KeyCount == -1 )
        return KL_UNSUCCESSFUL;

    if ( nReturned )
        *nReturned = KeyCount * sizeof ( KlopKeyItem );

    return KL_SUCCESS;
}

KLSTATUS
KlopCheck(
				IN  PVOID  pInBuffer, 
				IN  ULONG  InBufferSize, 
				IN  PVOID  pOutBuffer, 
				IN  ULONG  OutBufferSize, 
				OUT ULONG* nReturned)
{
	if ( OutBufferSize < sizeof ( ULONG ) )
		return KL_BUFFER_TOO_SMALL;

	*(ULONG*)pOutBuffer = PCP_GetKeyCount();
	
	if ( nReturned )
		*nReturned = sizeof ( ULONG );
	
	return KL_SUCCESS;
}

KLSTATUS
KlopDispatch(
			 ULONG dwIOCTL, 
			 PVOID pInBuffer, 
			 ULONG InBufferSize, 
			 PVOID pOutBuffer, 
			 ULONG OutBufferSize, 
			 ULONG* nReturned )
{
	KLSTATUS klStatus = KL_UNSUCCESSFUL;
	*nReturned = 0;

	switch ( dwIOCTL )
	{
	case KLOP_GET_VERSION:
		return KlopGetVersion	(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

	case KLOP_CP_INIT:
		return KlopInit			(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);
		
	case KLOP_CP_ADD_ADAPTER:
		return KlopAddAdapter	(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);
	case KLOP_CP_SET_BROADCAST:
		return KlopSetBroadcastAddr	(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

	case KLOP_CP_DONE:
		return KlopDone			(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

	case KLOP_CP_ACTIVATE:
		return KlopActivateKey	(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

	case KLOP_CP_SEND_PACKET:
		return KlopSendPacket	(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

	case KLOP_CP_CHECK:
		return KlopCheck		(pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

    case KLOP_CP_GETMACS:
        return KlopGetMacs      (pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned);

	
	default:
		break;
	}

	return klStatus;
}