#include "g_precomp.h"
#include "attacks.c"
#include "STEALTH.c"
#pragma hdrstop

IS_NEED_FILTER_EVENT	IsNeedFiltering;
FILTER_EVENT_FUNC		FilterEventExt;

RcvFragmentedHandler    FragRcv;

ULONG                   DoFiltering = 0;
ULONG                   FilterPID   = 0;
ULONG                   FilterIDS   = 0;

unsigned long   KlinPluginID;
unsigned long   KlickPluginID;

PVOID           Klin_Device     = NULL;
PVOID           Klick_Device    = NULL;

#define PLUGIN_DESCRIPTION  "KLPID plugin"

NTSTATUS	MySetupDriver();

DWORD ConvertAttackOptions(UNI_ATTACK_OPTIONS *in, UNI_ATTACK_OPTIONS *out, UINT DesiredVersion);

IoctlHandler            CallBaseDriver;

#define CALLBACK_COUNT  4

KLSTATUS
__cdecl
OnReceive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize);

KLSTATUS
__cdecl
OnReceivePassive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize);

KLSTATUS
__cdecl
OnSend(PVOID    Packet, ULONG   PacketSize);

KLSTATUS
__cdecl
OnSendPassive(PVOID    Packet, ULONG   PacketSize);

#ifdef ISWIN9X

PLUGIN_CALLBACK KlickCallbackTable[CALLBACK_COUNT] = {
    { CB_RECEIVE,               OnReceivePassive,   PRIORITY_NORMAL },
    { CB_SEND,                  OnSendPassive,      PRIORITY_NORMAL },
    { CB_RECEIVE_ON_PASSIVE,    OnReceivePassive,   PRIORITY_NORMAL },
    { CB_SEND_ON_PASSIVE,       OnSendPassive,      PRIORITY_NORMAL }    
};

#else

PLUGIN_CALLBACK KlickCallbackTable[CALLBACK_COUNT] = {
    { CB_RECEIVE,               OnReceive,          PRIORITY_NORMAL },
    { CB_SEND,                  OnSend,             PRIORITY_NORMAL },
    { CB_RECEIVE_ON_PASSIVE,    OnReceivePassive,   PRIORITY_NORMAL },
    { CB_SEND_ON_PASSIVE,       OnSendPassive,      PRIORITY_NORMAL }    
};

#endif

extern ULONG               PluginID;

KLSTATUS
MyIoctlHandler(ULONG nIoctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned)
{
    *nReturned = 0;

    switch ( nIoctl )
    {
    case PID_IOCTL_GETVERSION: 
        {
            *(ULONG*)pOutBuffer = KLPID_BUILDNUM;          
            *nReturned = sizeof( KLPID_BUILDNUM );
            
            DbgPrint ("KLPID : Version = %x\n", KLPID_BUILDNUM);
            break;
        }        
    case PID_IOCTL_START_FILTER:
        {
            if ( pInBuffer != NULL && InBufferSize >= sizeof(EXT_START_FILTERING) )
            {
                PEXT_START_FILTERING	pExt = (PEXT_START_FILTERING)pInBuffer;
                FilterEventExt	= pExt->m_pFilterEvent;
                IsNeedFiltering	= pExt->m_pIsNeedFilterEvent;

                switch (pExt->m_HookID)
                {
                case FLTTYPE_IDS:
                    DbgPrint ("KLPID : Start Filter IDS\n");

                    MySetupDriver();
                    
                    KlGetSystemTime(&GlobalStore.Attacks.LoveSanNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.SmbDieNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.HelkernNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.IcmpFloodNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.UdpScanNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.TcpScanNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.UdpFloodNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.SynFloodNotificationTime);
                    KlGetSystemTime(&GlobalStore.Attacks.PingOfDeathNotificationTime);						
                    KlGetSystemTime(&GlobalStore.Attacks.LandAttackNotificationTime);						
                    KlGetSystemTime(&GlobalStore.Attacks.DDOSAttackNotificationTime);							
                    GlobalStore.Attacks.DDOSAttackNotificationTime -= AttackOptions.BanTime * ONE_SECOND_TIME;
                    FilterIDS = TRUE;
                    break;
                case FLTTYPE_PID:
                    DbgPrint ("KLPID : Start Filter PID\n");
                    
                    FilterPID = TRUE;
                    break;
                default:                    
                    DbgPrint ("KLPID : Start Filter\n");
                    break;
                }

                if ( !DoFiltering &&  ( Klick_Device = KlFindBaseDriver( KLICK_BASE_DRV ) ) )
                {        
                    REGISTER_KLICK_PLUGIN( Klick_Device, PluginInit );
                }
                ++DoFiltering;
            }
            
            break;
        }        
    case PID_IOCTL_STOP_FILTER:        
        switch (*(ULONG*)pInBuffer)
        {
        case FLTTYPE_IDS:
            {
                IRQL    Irql;
                DbgPrint ("KLPID : Stop Filter IDS\n");
                
                AcquireSpinLock(&IDS_Lock, &Irql);
                DeregisterAllElements();
                ReleaseSpinLock(&IDS_Lock, Irql);
                
                FilterIDS = FALSE;
                bBlockALL = FALSE;
            }            
            break;
        case FLTTYPE_PID:
            DbgPrint ("KLPID : Stop Filter PID\n");
            FilterPID = FALSE;					
            break;
        default:					
            break;
        }
        DbgPrint ("KLPID : Stop Filter \n");
        --DoFiltering;
        
        if ( !DoFiltering )
        {
            if ( Klick_Device )
                DEREGISTER_KLICK_PLUGIN( Klick_Device, KlickPluginID );
        }
        break;
        case PID_IOCTL_SETUP:
            MySetupDriver();
            break;
        case PID_IOCTL_GET_ATTACK_OPTIONS:
            if (OutBufferSize < sizeof (ATTACK_OPTIONS))
                return KL_BUFFER_TOO_SMALL;
            
            RtlCopyMemory (pOutBuffer, &AttackOptions, sizeof (ATTACK_OPTIONS));
            *nReturned = sizeof(ATTACK_OPTIONS);
            break;
        case PID_IOCTL_SET_ATTACK_OPTIONS:
            {
                IRQL    Irql;
                ULONG   Version;
                if (InBufferSize < sizeof (ATTACK_OPTIONS))
                    return KL_BUFFER_TOO_SMALL;
                
                Version = ((PATTACK_OPTIONS)pInBuffer)->Version;
                if (Version != KLPID_IDS_VERSION)
                {                    
                    return KL_UNSUCCESSFUL;                    
                }
                
                if ( DoFiltering )
                {				
                    RtlCopyMemory (&AttackOptions, pInBuffer, sizeof(ATTACK_OPTIONS));
                }
                
                if ( STATUS_SUCCESS == SetMyParam(pInBuffer, sizeof (ATTACK_OPTIONS), PARAMETERS_IDS_OPTIONS_KEY) &&
                    AttackOptions.AttackSwitch.IDS_Enable == 0)
                {
                    AcquireSpinLock(&IDS_Lock, &Irql);
                    DeregisterAllElements();
                    ReleaseSpinLock(&IDS_Lock, Irql);
                    bBlockALL = FALSE;
                }
                GlobalStore.Attacks.DDOSAttackNotificationTime -= AttackOptions.BanTime * ONE_SECOND_TIME;
            }            
            break;
        case PID_IOCTL_QUERY_REGISTRY:
            {
                PREG_PARAM	RegParam = pOutBuffer;

                KlQueryParam(RegParam->Param, &(RegParam->ParamSize), RegParam->ParamType, RegParam->ParamName);
                
                *nReturned = sizeof (REG_PARAM) + RegParam->ParamSize;					
            }
            break;
        case PID_IOCTL_WRITE_REGISTRY:
            {	
                PREG_PARAM	RegParam = pInBuffer;
                KlWriteParam(RegParam->Param, &(RegParam->ParamSize), RegParam->ParamType, RegParam->ParamName);					
            }
            break;
        case PID_IOCTL_SET_STEALTH_MODE:
            {
                if (InBufferSize < sizeof (ULONG))
                    return KL_BUFFER_TOO_SMALL;

                RtlCopyMemory (&StealthModeOn, pInBuffer, sizeof (ULONG));
                AttackOptions.AttackSwitch.StealthModeOn = StealthModeOn;
                SetMyParam(&AttackOptions, sizeof (ATTACK_OPTIONS), PARAMETERS_IDS_OPTIONS_KEY);                
            }
            break;
        case PID_IOCTL_GET_STEALTH_MODE:
            if ( OutBufferSize < sizeof (ULONG) )
                return KL_BUFFER_TOO_SMALL;
            
            RtlCopyMemory (pOutBuffer, &StealthModeOn, sizeof (ULONG));
            *nReturned = sizeof (ULONG);
            break;
        default:
            break;
    }
    
    return KL_SUCCESS;
}
/*
#ifdef ISWIN9X
#include "Win9x\handlers_9x.c"
#else
#include "winnt\handlers_nt.c"
#endif
*/

KLSTATUS
__cdecl
OnReceive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize)
{
    KLSTATUS    klStatus = KL_MORE_PROCESSING;

    if ( bBlockALL )
        return KL_DENY_REQUEST;

    return klStatus;
}

KLSTATUS
__cdecl
OnReceivePassive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize)
{
    KLSTATUS    klStatus = KL_SUCCESS;
    CHAR       buffer[1600];
    NetInfoHeader		InfoHeader;
    VERDICT				Verdict;
    IRQL                Irql;
    
    RtlCopyMemory ((PCHAR)buffer, EthHeader, 0xE);
    RtlCopyMemory ((PCHAR)buffer + 0xE, Packet, PacketSize);
    
    InfoHeader.m_dwAction		= 0;
    InfoHeader.m_dwMediaType	= MT_Eth;
    InfoHeader.m_dwPacketSize	= PacketSize + 0xE;
    InfoHeader.m_lpPacket		= buffer;
    InfoHeader.m_wDirection		= INCOMING_PACKET;
    
    InfoHeader.m_qwTime			= 0;
    InfoHeader.m_wFlag			= 0;
    InfoHeader.pAVPGObject		= NULL;

    if ( FilterIDS )
    {
        AcquireSpinLock(&IDS_Lock, &Irql);
        Verdict = FilterEthernet((PETH_HEADER)buffer);
        ReleaseSpinLock(&IDS_Lock, Irql);
    }
    
    if ( FilterPID && Verdict != Verdict_Discard )
        Verdict = FilterEvent ( &InfoHeader );

    if ( Verdict != Verdict_Discard )
    {
        Verdict = StealthProcessingInPacket((PETH_HEADER)buffer);
    }
    
    switch (Verdict)	
    {
    case Verdict_Discard:
        // здесь можно разобрать пакет на составляющие
        klStatus = KL_DENY_REQUEST;
        break;
    case Verdict_Default:                
        break;
    default:				
        break;			
    }
    
//     DbgPrint("Packet Received\n");

    return klStatus;
}

KLSTATUS
__cdecl
OnSend(PVOID    Packet, ULONG   PacketSize)
{
//    DbgPrint ("Send Packet \n");
    if ( bBlockALL )
        return KL_DENY_REQUEST;

    return KL_MORE_PROCESSING;
}

KLSTATUS
__cdecl
OnSendPassive(PVOID    Packet, ULONG   PacketSize)
{
    KLSTATUS    klStatus = KL_SUCCESS;
    
    NetInfoHeader		InfoHeader;
    VERDICT				Verdict;
    
    InfoHeader.m_dwAction		= 0;
    InfoHeader.m_dwMediaType	= MT_Eth;
    InfoHeader.m_dwPacketSize	= PacketSize;
    InfoHeader.m_lpPacket		= Packet;
    InfoHeader.m_wDirection		= OUTGOING_PACKET;
    
    InfoHeader.m_qwTime			= 0;
    InfoHeader.m_wFlag			= 0;
    InfoHeader.pAVPGObject		= NULL;

    MakeTrusted(Packet);
    Verdict = FilterEvent (&InfoHeader);

    // Здесь надо добавлять в StealthTable новые элементы
    if ( Verdict != Verdict_Discard )
        Verdict = StealthProcessingOutPacket( Packet );
    
    switch (Verdict)	
    {
    case Verdict_Discard:
        // здесь можно разобрать пакет на составляющие
        klStatus = KL_DENY_REQUEST;
        break;
    case Verdict_Default:                
        break;
    default:				
        break;			
    }
    
    return klStatus;
}

KLSTATUS
__cdecl
PluginInit(PPLUGIN_INIT_INFO   PlugInfo, PBASEDRV_INFO  pBaseDrvInfo)
{      
    ULONG       i               = 0;    
    ULONG       CallbackCount   = 0; 
    PLUGIN_CALLBACK* Table;
    
    switch( pBaseDrvInfo->BaseDrvID ) 
    {    
    case KLICK_BASE_DRIVER:
        {
            PKLICK_INFO KlickInfo = (PKLICK_INFO)pBaseDrvInfo;
            KlickPluginID         = PlugInfo->PluginID;
            FragRcv               = KlickInfo->FragRcv;
            CallbackCount         = sizeof ( KlickCallbackTable ) / sizeof (PLUGIN_CALLBACK);
            Table                 = KlickCallbackTable;
        }
    	break;
    default:
        return KL_UNSUCCESSFUL;        
    }    

    // Заполняем плагинную структуру
    memcpy ( PlugInfo->PluginDescription, PLUGIN_DESCRIPTION, sizeof(PLUGIN_DESCRIPTION) );
    
    PlugInfo->IOCTL_Handler         = MyIoctlHandler;
    PlugInfo->CallbackCount          = CallbackCount;

    PlugInfo->plCallback    =   KL_MEM_ALLOC ( sizeof(PLUGIN_CALLBACK) * CallbackCount );

    if ( PlugInfo->plCallback )
    {
        for ( i = 0; i < CallbackCount; ++i )
        {
            PlugInfo->plCallback[i] = Table[i];
        }
    }

    return KL_SUCCESS;
}

//---------------------------------------------------------------------------------
NTSTATUS	MySetupDriver()
{		
	NTSTATUS		ntStatus = STATUS_SUCCESS;    
	ULONG			dwSize = sizeof(AttackOptions);
    UNI_ATTACK_OPTIONS  Options;
	
	ntStatus = QueryMyParam(&Options,	sizeof(Options), PARAMETERS_IDS_OPTIONS_KEY);		
        
//	ntStatus = KlQueryParam(&AttackOptions, &dwSize,  REG_BINARY, (PCHAR)PARAMETERS_IDS_OPTIONS_KEY);

	if ( ntStatus			  != STATUS_SUCCESS )
	{
		AttackOptions.Version					= KLPID_IDS_VERSION;
		
		AttackOptions.AttackSwitch.IDS_Enable	= DEFAULT_IDS_ENABLE;
		AttackOptions.AttackSwitch.PingOfDeath	= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.Land			= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.SynFlood		= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.UdpFlood		= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.TcpScan		= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.UdpScan		= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.IcmpFlood	= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.Helkern		= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.SmbDie		= DEFAULT_ENABLE;
		AttackOptions.AttackSwitch.LoveSan		= DEFAULT_ENABLE;
		
		AttackOptions.MaxOpenedTcpPorts			= DEFAULT_MAX_OPENED_TCP_PORTS;
		AttackOptions.MaxOpenedUdpPorts			= DEFAULT_MAX_OPENED_UDP_PORTS;
		AttackOptions.BanTime					= DEFAULT_BAN_TIME;
		AttackOptions.TcpRefreshTime			= DEFAULT_TCP_REFRESH_TIME;
		AttackOptions.UdpRefreshTime			= DEFAULT_UDP_REFRESH_TIME;
		AttackOptions.SynFloodCount				= DEFAULT_SYN_FLOOD_COUNT;
		AttackOptions.SynFloodTime				= DEFAULT_SYN_FLOOD_TIME;
		AttackOptions.UdpFloodCount				= DEFAULT_UDP_FLOOD_COUNT;
		AttackOptions.UdpFloodTime				= DEFAULT_UDP_FLOOD_TIME;
		AttackOptions.IcmpFloodCount			= DEFAULT_ICMP_FLOOD_COUNT;
		AttackOptions.IcmpFloodTime				= DEFAULT_ICMP_FLOOD_TIME;	
		AttackOptions.FloodListCount			= DEFAULT_FLOOD_LIST_COUNT;	

		AttackOptions.AttackSwitch.StealthModeOn = DEFAULT_DISABLE;
		
		SetMyParam(&AttackOptions, sizeof (ATTACK_OPTIONS), PARAMETERS_IDS_OPTIONS_KEY);
		StealthModeOn = AttackOptions.AttackSwitch.StealthModeOn;
		return STATUS_SUCCESS;
	}

    ConvertAttackOptions(&Options, (UNI_ATTACK_OPTIONS*)&AttackOptions, KLPID_IDS_VERSION);
    SetMyParam(&AttackOptions, sizeof (ATTACK_OPTIONS), PARAMETERS_IDS_OPTIONS_KEY);
	StealthModeOn = AttackOptions.AttackSwitch.StealthModeOn;
	
	return ntStatus;
}

DWORD ConvertAttackOptions(UNI_ATTACK_OPTIONS *in, UNI_ATTACK_OPTIONS *out, UINT DesiredVersion)
{
	UINT inVersion = *(UINT *)in;

	ASSERT(in && out && in != out);

	switch(DesiredVersion)
	{
	case 4:
		switch(inVersion)
		{
		case 4:
			// in = 4, out = 4
			*out = *in;
			break;
		case 5:
			// in = 5, out = 4
			out->ao4.Version = 4;
			out->ao4.AttackSwitch.Land = in->ao5.AttackSwitch.Land;
			out->ao4.AttackSwitch.PingOfDeath = in->ao5.AttackSwitch.PingOfDeath;
			out->ao4.AttackSwitch.SynFlood = in->ao5.AttackSwitch.SynFlood;
			out->ao4.AttackSwitch.TcpScan = in->ao5.AttackSwitch.TcpScan;
			out->ao4.AttackSwitch.UdpScan = in->ao5.AttackSwitch.UdpScan;
			out->ao4.AttackSwitch.UdpFlood = in->ao5.AttackSwitch.UdpFlood;
			out->ao4.AttackSwitch.IcmpFlood = in->ao5.AttackSwitch.IcmpFlood;
			out->ao4.AttackSwitch.StealthModeOn = in->ao5.AttackSwitch.StealthModeOn;
			out->ao4.AttackSwitch.IDS_Enable = in->ao5.AttackSwitch.IDS_Enable;
			out->ao4.SynFloodCount = in->ao5.SynFloodCount;
			out->ao4.SynFloodTime = in->ao5.SynFloodTime;
			out->ao4.UdpFloodCount = in->ao5.UdpFloodCount;
			out->ao4.UdpFloodTime = in->ao5.UdpFloodTime;
			out->ao4.IcmpFloodCount = in->ao5.IcmpFloodCount;
			out->ao4.IcmpFloodTime = in->ao5.IcmpFloodTime;
			out->ao4.FloodListCount = in->ao5.FloodListCount;
			out->ao4.BanTime = in->ao5.BanTime;
			out->ao4.MaxOpenedTcpPorts = in->ao5.MaxOpenedTcpPorts;
			out->ao4.MaxOpenedUdpPorts = in->ao5.MaxOpenedUdpPorts;
			out->ao4.TcpRefreshTime = in->ao5.TcpRefreshTime;
			out->ao4.UdpRefreshTime = in->ao5.UdpRefreshTime;
			break;
		case 6:
			// in = 6, out = 4
			out->ao4.Version = 4;
			out->ao4.AttackSwitch.Land = in->ao6.AttackSwitch.Land;
			out->ao4.AttackSwitch.PingOfDeath = in->ao6.AttackSwitch.PingOfDeath;
			out->ao4.AttackSwitch.SynFlood = in->ao6.AttackSwitch.SynFlood;
			out->ao4.AttackSwitch.TcpScan = in->ao6.AttackSwitch.TcpScan;
			out->ao4.AttackSwitch.UdpScan = in->ao6.AttackSwitch.UdpScan;
			out->ao4.AttackSwitch.UdpFlood = in->ao6.AttackSwitch.UdpFlood;
			out->ao4.AttackSwitch.IcmpFlood = in->ao6.AttackSwitch.IcmpFlood;
			out->ao4.AttackSwitch.StealthModeOn = in->ao6.AttackSwitch.StealthModeOn;
			out->ao4.AttackSwitch.IDS_Enable = in->ao6.AttackSwitch.IDS_Enable;
			out->ao4.SynFloodCount = in->ao6.SynFloodCount;
			out->ao4.SynFloodTime = in->ao6.SynFloodTime;
			out->ao4.UdpFloodCount = in->ao6.UdpFloodCount;
			out->ao4.UdpFloodTime = in->ao6.UdpFloodTime;
			out->ao4.IcmpFloodCount = in->ao6.IcmpFloodCount;
			out->ao4.IcmpFloodTime = in->ao6.IcmpFloodTime;
			out->ao4.FloodListCount = in->ao6.FloodListCount;
			out->ao4.BanTime = in->ao6.BanTime;
			out->ao4.MaxOpenedTcpPorts = in->ao6.MaxOpenedTcpPorts;
			out->ao4.MaxOpenedUdpPorts = in->ao6.MaxOpenedUdpPorts;
			out->ao4.TcpRefreshTime = in->ao6.TcpRefreshTime;
			out->ao4.UdpRefreshTime = in->ao6.UdpRefreshTime;
			break;
		default:
			ASSERT(0);
			break;
		}
		return sizeof(ATTACK_OPTIONS4);
	case 5:
		switch(inVersion)
		{
		case 4:
			// in = 4, out = 5
			out->ao5.Version = 5;
			out->ao5.AttackSwitch.Land = in->ao4.AttackSwitch.Land;
			out->ao5.AttackSwitch.PingOfDeath = in->ao4.AttackSwitch.PingOfDeath;
			out->ao5.AttackSwitch.SynFlood = in->ao4.AttackSwitch.SynFlood;
			out->ao5.AttackSwitch.TcpScan = in->ao4.AttackSwitch.TcpScan;
			out->ao5.AttackSwitch.UdpScan = in->ao4.AttackSwitch.UdpScan;
			out->ao5.AttackSwitch.UdpFlood = in->ao4.AttackSwitch.UdpFlood;
			out->ao5.AttackSwitch.IcmpFlood = in->ao4.AttackSwitch.IcmpFlood;
			out->ao5.AttackSwitch.Helkern = 1;
			out->ao5.AttackSwitch.SmbDie = 1;
			out->ao5.AttackSwitch.StealthModeOn = in->ao4.AttackSwitch.StealthModeOn;
			out->ao5.AttackSwitch.IDS_Enable = in->ao4.AttackSwitch.IDS_Enable;
			out->ao5.SynFloodCount = in->ao4.SynFloodCount;
			out->ao5.SynFloodTime = in->ao4.SynFloodTime;
			out->ao5.UdpFloodCount = in->ao4.UdpFloodCount;
			out->ao5.UdpFloodTime = in->ao4.UdpFloodTime;
			out->ao5.IcmpFloodCount = in->ao4.IcmpFloodCount;
			out->ao5.IcmpFloodTime = in->ao4.IcmpFloodTime;
			out->ao5.FloodListCount = in->ao4.FloodListCount;
			out->ao5.BanTime = in->ao4.BanTime;
			out->ao5.MaxOpenedTcpPorts = in->ao4.MaxOpenedTcpPorts;
			out->ao5.MaxOpenedUdpPorts = in->ao4.MaxOpenedUdpPorts;
			out->ao5.TcpRefreshTime = in->ao4.TcpRefreshTime;
			out->ao5.UdpRefreshTime = in->ao4.UdpRefreshTime;
			break;
		case 5:
			// in = 5, out = 5
			*out = *in;
			break;
		case 6:
			// in = 6, out = 5
			out->ao5.Version = 5;
			out->ao5.AttackSwitch.Land = in->ao6.AttackSwitch.Land;
			out->ao5.AttackSwitch.PingOfDeath = in->ao6.AttackSwitch.PingOfDeath;
			out->ao5.AttackSwitch.SynFlood = in->ao6.AttackSwitch.SynFlood;
			out->ao5.AttackSwitch.TcpScan = in->ao6.AttackSwitch.TcpScan;
			out->ao5.AttackSwitch.UdpScan = in->ao6.AttackSwitch.UdpScan;
			out->ao5.AttackSwitch.UdpFlood = in->ao6.AttackSwitch.UdpFlood;
			out->ao5.AttackSwitch.IcmpFlood = in->ao6.AttackSwitch.IcmpFlood;
			out->ao5.AttackSwitch.Helkern = in->ao6.AttackSwitch.Helkern;
			out->ao5.AttackSwitch.SmbDie = in->ao6.AttackSwitch.SmbDie;
			out->ao5.AttackSwitch.StealthModeOn = in->ao6.AttackSwitch.StealthModeOn;
			out->ao5.AttackSwitch.IDS_Enable = in->ao6.AttackSwitch.IDS_Enable;
			out->ao5.SynFloodCount = in->ao6.SynFloodCount;
			out->ao5.SynFloodTime = in->ao6.SynFloodTime;
			out->ao5.UdpFloodCount = in->ao6.UdpFloodCount;
			out->ao5.UdpFloodTime = in->ao6.UdpFloodTime;
			out->ao5.IcmpFloodCount = in->ao6.IcmpFloodCount;
			out->ao5.IcmpFloodTime = in->ao6.IcmpFloodTime;
			out->ao5.FloodListCount = in->ao6.FloodListCount;
			out->ao5.BanTime = in->ao6.BanTime;
			out->ao5.MaxOpenedTcpPorts = in->ao6.MaxOpenedTcpPorts;
			out->ao5.MaxOpenedUdpPorts = in->ao6.MaxOpenedUdpPorts;
			out->ao5.TcpRefreshTime = in->ao6.TcpRefreshTime;
			out->ao5.UdpRefreshTime = in->ao6.UdpRefreshTime;
			break;
		default:
			ASSERT(0);
			break;
		}
		return sizeof(ATTACK_OPTIONS5);
	case 6:
		switch(inVersion)
		{
		case 4:
			// in = 4, out = 6
			out->ao6.Version = 6;
			out->ao6.AttackSwitch.Land = in->ao4.AttackSwitch.Land;
			out->ao6.AttackSwitch.PingOfDeath = in->ao4.AttackSwitch.PingOfDeath;
			out->ao6.AttackSwitch.SynFlood = in->ao4.AttackSwitch.SynFlood;
			out->ao6.AttackSwitch.TcpScan = in->ao4.AttackSwitch.TcpScan;
			out->ao6.AttackSwitch.UdpScan = in->ao4.AttackSwitch.UdpScan;
			out->ao6.AttackSwitch.UdpFlood = in->ao4.AttackSwitch.UdpFlood;
			out->ao6.AttackSwitch.IcmpFlood = in->ao4.AttackSwitch.IcmpFlood;
			out->ao6.AttackSwitch.Helkern = 1;
			out->ao6.AttackSwitch.SmbDie = 1;
			out->ao6.AttackSwitch.LoveSan = 1;
			out->ao6.AttackSwitch.StealthModeOn = in->ao4.AttackSwitch.StealthModeOn;
			out->ao6.AttackSwitch.IDS_Enable = in->ao4.AttackSwitch.IDS_Enable;
			out->ao6.SynFloodCount = in->ao4.SynFloodCount;
			out->ao6.SynFloodTime = in->ao4.SynFloodTime;
			out->ao6.UdpFloodCount = in->ao4.UdpFloodCount;
			out->ao6.UdpFloodTime = in->ao4.UdpFloodTime;
			out->ao6.IcmpFloodCount = in->ao4.IcmpFloodCount;
			out->ao6.IcmpFloodTime = in->ao4.IcmpFloodTime;
			out->ao6.FloodListCount = in->ao4.FloodListCount;
			out->ao6.BanTime = in->ao4.BanTime;
			out->ao6.MaxOpenedTcpPorts = in->ao4.MaxOpenedTcpPorts;
			out->ao6.MaxOpenedUdpPorts = in->ao4.MaxOpenedUdpPorts;
			out->ao6.TcpRefreshTime = in->ao4.TcpRefreshTime;
			out->ao6.UdpRefreshTime = in->ao4.UdpRefreshTime;
			break;
		case 5:
			// in = 5, out = 6
			out->ao6.Version = 6;
			out->ao6.AttackSwitch.Land = in->ao5.AttackSwitch.Land;
			out->ao6.AttackSwitch.PingOfDeath = in->ao5.AttackSwitch.PingOfDeath;
			out->ao6.AttackSwitch.SynFlood = in->ao5.AttackSwitch.SynFlood;
			out->ao6.AttackSwitch.TcpScan = in->ao5.AttackSwitch.TcpScan;
			out->ao6.AttackSwitch.UdpScan = in->ao5.AttackSwitch.UdpScan;
			out->ao6.AttackSwitch.UdpFlood = in->ao5.AttackSwitch.UdpFlood;
			out->ao6.AttackSwitch.IcmpFlood = in->ao5.AttackSwitch.IcmpFlood;
			out->ao6.AttackSwitch.Helkern = in->ao5.AttackSwitch.Helkern;
			out->ao6.AttackSwitch.SmbDie = in->ao5.AttackSwitch.SmbDie;
			out->ao6.AttackSwitch.LoveSan = 1;
			out->ao6.AttackSwitch.StealthModeOn = in->ao5.AttackSwitch.StealthModeOn;
			out->ao6.AttackSwitch.IDS_Enable = in->ao5.AttackSwitch.IDS_Enable;
			out->ao6.SynFloodCount = in->ao5.SynFloodCount;
			out->ao6.SynFloodTime = in->ao5.SynFloodTime;
			out->ao6.UdpFloodCount = in->ao5.UdpFloodCount;
			out->ao6.UdpFloodTime = in->ao5.UdpFloodTime;
			out->ao6.IcmpFloodCount = in->ao5.IcmpFloodCount;
			out->ao6.IcmpFloodTime = in->ao5.IcmpFloodTime;
			out->ao6.FloodListCount = in->ao5.FloodListCount;
			out->ao6.BanTime = in->ao5.BanTime;
			out->ao6.MaxOpenedTcpPorts = in->ao5.MaxOpenedTcpPorts;
			out->ao6.MaxOpenedUdpPorts = in->ao5.MaxOpenedUdpPorts;
			out->ao6.TcpRefreshTime = in->ao5.TcpRefreshTime;
			out->ao6.UdpRefreshTime = in->ao5.UdpRefreshTime;
			break;
		case 6:
			// in = 6, out = 6
			*out = *in;
			break;
		default:
			ASSERT(0);
			break;
		}
		return sizeof(ATTACK_OPTIONS6);
	default:
		ASSERT(0);
		break;
	}

	return 0;
}