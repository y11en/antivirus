#include "g_precomp.h"
#pragma hdrstop

#define PLUGIN_DESCRIPTION  "KLPF plugin"

IS_NEED_FILTER_EVENT	IsNeedFilterEvent;
FILTER_EVENT_FUNC		FilterEvent;

QueryHandleHandler      QueryAddr;
QueryHandleHandler      QueryConn;
IoctlHandler            CallBaseDriver;

LIST_ENTRY              AcceptList;
SPIN_LOCK               AcceptListLock;

unsigned long   KlinPluginID;
unsigned long   KlickPluginID;

PVOID           Klin_Device     = NULL;
PVOID           Klick_Device    = NULL;

#define CALLBACK_COUNT  7

PLUGIN_CALLBACK KlinCallbackTable[CALLBACK_COUNT] = {
    { CB_CONNECT,               OnConnect,          PRIORITY_NORMAL },
    { CB_POST_CONNECT,          OnPostConnect,      PRIORITY_NORMAL },
    { CB_POST_CONNECT_PASSIVE,  OnConnectPassive,   PRIORITY_NORMAL },
    { CB_RECEIVE_DG,            OnRecvDG,           PRIORITY_NORMAL },
    { CB_DELAYED_RECEIVE_DG,    OnDelayedRecvDG,    PRIORITY_NORMAL },
    { CB_SEND_DG,               OnSendDG,           PRIORITY_NORMAL },
    { CB_DELAYED_SEND_DG,       OnDelayedSendDG,    PRIORITY_NORMAL }
};

ULONG DoFiltering = 0;

ULONG
Klin2KlpfTaConvert(PVOID    KlinBuffer, ULONG   KlinBufferSize, PVOID KlpfBuffer, ULONG KlpfBufferSize);

ULONG
Klin2KlpfConnConvert( PVOID    KlinBuffer, ULONG   KlinBufferSize, PVOID KlpfBuffer, ULONG KlpfBufferSize );

KLSTATUS
MyIoctlHandler(ULONG nIoctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned)
{
    *nReturned = 0;

    switch ( nIoctl )
    {
    case KLPF_GET_VERSION:
        {			
            ULONG	Version;            
            Version = KLPF_BUILDNUM;
            RtlCopyMemory (pOutBuffer, &Version, sizeof(Version));				
            *nReturned = sizeof(Version);            
            DbgPrint ("KLPF : Version = %x\n", Version);
        }
        break;    
    case KLPF_START_FILTER:	
        {			
            PEXT_START_FILTERING	pExt = (PEXT_START_FILTERING)pInBuffer;
            if ( pExt != NULL && InBufferSize >= sizeof(EXT_START_FILTERING) )
            {
                FilterEvent			= pExt->m_pFilterEvent;
                IsNeedFilterEvent	= pExt->m_pIsNeedFilterEvent;
                DbgPrint ("KLPF : Start filtering\n");
            }

            if ( !DoFiltering &&  ( Klin_Device = KlFindBaseDriver( KLIN_BASE_DRV ) ) )
            {        
                REGISTER_KLIN_PLUGIN( Klin_Device, PluginInit );
            }
            ++DoFiltering;
        }			        
        break;
    case KLPF_STOP_FILTER:        
        DbgPrint ("KLPF : Stop filtering");
        --DoFiltering;
        
        if ( !DoFiltering )
        {
            if ( Klin_Device )
                DEREGISTER_KLIN_PLUGIN( Klin_Device, KlinPluginID );
        }
        break;
    case KLPF_GET_LOCAL_TA:
        {
            /*
             *	Здесь есть ограничение на список портов. ( nBufferSize !!!)
             */
            KLSTATUS klStatus;

            ULONG   nBufferSize = 0xFFFF;
            PVOID   pBuffer     = KL_MEM_ALLOC(nBufferSize);            

            DbgPrint ("KLPF : Get TA List\n");
            
            klStatus = CallBaseDriver(KLIN_GET_ADDR_LIST, NULL, 0, pBuffer, nBufferSize, &nBufferSize);

            if ( klStatus == KL_BUFFER_TOO_SMALL )
            {
                KL_MEM_FREE(pBuffer);
                pBuffer     = KL_MEM_ALLOC(nBufferSize);
                klStatus = CallBaseDriver(KLIN_GET_ADDR_LIST, NULL, 0, pBuffer, nBufferSize, &nBufferSize);
            }

            if ( klStatus == KL_SUCCESS )
            {                
                *nReturned = Klin2KlpfTaConvert(pBuffer, nBufferSize, pOutBuffer, OutBufferSize);
            }

            KL_MEM_FREE(pBuffer);
		}
        break;
    case KLPF_GET_CONNECTIONS:
        {
            /*
             *	Здесь есть ограничение на список соединений. ( nBufferSize !!!)
             */

            KLSTATUS klStatus;

            ULONG   nBufferSize = 0xFFFF;
            PVOID   pBuffer     = KL_MEM_ALLOC(nBufferSize);

            DbgPrint ("KLPF : Get CONN List\n");

            klStatus = CallBaseDriver(KLIN_GET_CONN_LIST, NULL, 0, pBuffer, nBufferSize, &nBufferSize);

            if ( klStatus == KL_BUFFER_TOO_SMALL )
            {
                KL_MEM_FREE(pBuffer);
                pBuffer     = KL_MEM_ALLOC(nBufferSize);
                klStatus = CallBaseDriver(KLIN_GET_CONN_LIST, NULL, 0, pBuffer, nBufferSize, &nBufferSize);
            }
            
            if ( klStatus == KL_SUCCESS )
            {                
                *nReturned = Klin2KlpfConnConvert(pBuffer, nBufferSize, pOutBuffer, OutBufferSize);
            }

            KL_MEM_FREE(pBuffer);
		}
        break;
    case KLPF_MAKE_DISCONNECT:
        {
            KLSTATUS klStatus;
            
            klStatus = CallBaseDriver(KLIN_MAKE_DISCONNECT, pInBuffer, InBufferSize, NULL, 0, nReturned);
        }
        break;
    default:
        break;
    }

    return KL_SUCCESS;
}

KLSTATUS
__cdecl
OnOpenAddr(PVOID    Context)
{
    USHORT  Port = 0;
    CHAR    ProcessName[16];

    QueryAddr(Context, QueryLocalPort, (PCHAR)&Port, sizeof (USHORT));
    QueryAddr(Context, QueryProcess,     (PCHAR)&ProcessName, sizeof(ProcessName));
    
    return KL_SUCCESS;
}

KLSTATUS
__cdecl
OnOpenConn(PVOID Handle, PVOID Context)
{    
    return KL_SUCCESS;
}

KLSTATUS
__cdecl
OnConnect (PVOID Handle, BOOLEAN isIncoming)
{   
    return KL_MORE_PROCESSING;
}

KLSTATUS
__cdecl
OnPostConnect (PVOID Handle, BOOLEAN isIncoming)
{   
    return KL_MORE_PROCESSING;
}

KLSTATUS
__cdecl
OnConnectPassive (PVOID Handle, BOOLEAN isIncoming)
{   
    KLSTATUS    klStatus = KL_SUCCESS;
    ULONG   rIp     = 0, 
            lIp     = 0;

    USHORT  rPort   = 0, 
            lPort   = 0;

    ULONG   Pid     = 0;
    
    CHAR    ProcessName[KL_PROCNAMELEN];
    CHAR    ImageName[KL_USERAPPNAMELEN];
    CHAR    Hash[KL_HASH_SIZE];

    ULONG     bInitiator =  isIncoming ? ID_CONNECT_INCOMING : ID_CONNECT_OUTGOING;
    
    QueryConn(Handle, QueryLocalIp,     (PCHAR)&lIp,         sizeof (ULONG));
    QueryConn(Handle, QueryLocalPort,   (PCHAR)&lPort,       sizeof (USHORT));
    QueryConn(Handle, QueryRemoteIp,    (PCHAR)&rIp,         sizeof (ULONG));
    QueryConn(Handle, QueryRemotePort,  (PCHAR)&rPort,       sizeof (USHORT));
    QueryConn(Handle, QueryPID,         (PCHAR)&Pid,         sizeof (ULONG));
    QueryConn(Handle, QueryProcess,     (PCHAR)&ProcessName, sizeof (ProcessName));
    QueryConn(Handle, QueryImage,       (PCHAR)&ImageName,   sizeof (ImageName));
    QueryConn(Handle, QueryHash,        (PCHAR)&Hash,        sizeof (Hash));
    

    //---------------------------------------------------------------------------------------------------------					
	if ( FilterEvent && ( rIp != lIp ) ) 
    {
        VERDICT	Verdict;
        
        do
        {
            ULONG	FilterState = 0;
            {					
                START_NDIS_FILTERS(MJ_PACKET_OUT,FLTTYPE_PID, ProcessName)
                    
                ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_DST_ADDRESS,		htonl(rIp)		);
                ADD_USHORT_PARAM (pSingleParam, pParam, ID_TCP_DST_PORT,		htons(rPort)	);
                ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_SRC_ADDRESS,		htonl(lIp)		);
                ADD_USHORT_PARAM (pSingleParam, pParam, ID_TCP_SRC_PORT,		htons(lPort)    );
                
                ADD_UCHAR_PARAM	 (pSingleParam, pParam, ID_IP_PROTOCOL,         0x6); // this is TCP protocol ...
                
                Verdict = FilterEvent(pParam, NULL);
                
                END_NDIS_FILTERS
            }
            
            if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
            {                
                Verdict = Verdict_Discard;
                break;
            }
            
            if (Verdict == Verdict_Default || Verdict == Verdict_Pass || Verdict== Verdict_WDog || Verdict== Verdict_Continue)
                FilterState = 1;
            
            {					
                START_NDIS_FILTERS(MJ_PACKET_IN,FLTTYPE_PID, ProcessName)
                    
                ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_SRC_ADDRESS,		htonl(rIp)		);
                ADD_USHORT_PARAM (pSingleParam, pParam, ID_TCP_SRC_PORT,		htons(rPort)	);
                ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_DST_ADDRESS,		htonl(lIp)		);
                ADD_USHORT_PARAM (pSingleParam, pParam, ID_TCP_DST_PORT,		htons(lPort)	);
                
                ADD_UCHAR_PARAM	 (pSingleParam, pParam, ID_IP_PROTOCOL,         0x6); // this is TCP protocol ...
                
                Verdict = FilterEvent(pParam, NULL);
                
                END_NDIS_FILTERS
            }
            
            if ( Verdict == Verdict_Discard || Verdict == Verdict_Kill )
            {                
                Verdict = Verdict_Discard;
                break;
            }
            
            if ( Verdict == Verdict_Default || Verdict==Verdict_Pass || Verdict==Verdict_WDog || Verdict==Verdict_Continue )
                FilterState = (FilterState << 1) + 1;
            
            // Здесь проверить вердикт 
            if (FilterState == 3)
            {                
                Verdict = Verdict_Pass;
                break;
            }            
            
            {					
                START_TDI_FILTERS(EVENT_CONNECT, FLTTYPE_KLPF, Pid, ProcessName)
                    
                ADD_ULONG_PARAM  (pSingleParam, pParam, ID_REMOTE_IP,		htonl(rIp)		);
                ADD_USHORT_PARAM (pSingleParam, pParam, ID_REMOTE_PORT,		htons(rPort)	);
                ADD_ULONG_PARAM  (pSingleParam, pParam, ID_LOCAL_IP,		htonl(lIp)		);
                ADD_USHORT_PARAM (pSingleParam, pParam, ID_LOCAL_PORT,		htons(lPort)	);
                
                ADD_ULONG_PARAM	 (pSingleParam, pParam, ID_INITIATOR,		bInitiator);
                
                ADD_UCHAR_PARAM	 (pSingleParam, pParam, ID_IP_PROTOCOL, 0x6); // this is TCP protocol ...
                
                COPY_BINARY_PARAM(pSingleParam, pParam, ID_FULL_APP_PATH,	ImageName, USERAPPNAMELEN);
                COPY_BINARY_PARAM(pSingleParam, pParam, ID_FILE_CHECKSUM,	Hash, HASH_SIZE);

                Verdict = FilterEvent(pParam, NULL);
                
                END_TDI_FILTERS
            }

        } while (FALSE);

        if ( Verdict == Verdict_Discard )
            klStatus = KL_DENY_REQUEST;
    }
    //-----------------------------------------------------------------------------------------------------    
/*
    if ( KL_CODE ( klStatus ) != F_BLOCK )
    {
        if ( isIncoming == FALSE )
        {
            PACCEPT_INFO    Info;
            
            RefreshAcceptList();
            
            RememberConnInfo( lIp, lPort, rIp, rPort );
        }
        else
        {
            
        }
    }
*/
    return klStatus;
}

KLSTATUS
__cdecl
OnDisconnect (PVOID Handle)
{   
    return KL_SUCCESS;
}

KLSTATUS
__cdecl
OnSendDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr)
{   
    return KL_DELAY_REQUEST;
}

KLSTATUS
__cdecl
OnDelayedSendDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr)
{  
    KLSTATUS    klStatus = KL_SUCCESS;
    ULONG   rIp     = RemoteAddr->Address[0].Address[0].in_addr, 
            lIp     = 0;

    USHORT  rPort   = RemoteAddr->Address[0].Address[0].sin_port, 
            lPort   = 0;

    ULONG   Pid     = 0;
    
    CHAR    ProcessName[KL_PROCNAMELEN];
    CHAR    ImageName[KL_USERAPPNAMELEN];
    CHAR    Hash[KL_HASH_SIZE];
    
    QueryAddr(Handle, QueryLocalIp,     (PCHAR)&lIp,         sizeof (ULONG));
    QueryAddr(Handle, QueryLocalPort,   (PCHAR)&lPort,       sizeof (USHORT));    
    QueryAddr(Handle, QueryPID,         (PCHAR)&Pid,         sizeof (ULONG));
    QueryAddr(Handle, QueryProcess,     (PCHAR)&ProcessName, sizeof(ProcessName));
    QueryAddr(Handle, QueryImage,       (PCHAR)&ImageName,   sizeof(ImageName));
    QueryAddr(Handle, QueryHash,        (PCHAR)&Hash,        sizeof(Hash));

    //---------------------------------------------------------------------------------------------------------					
	if (FilterEvent && lIp != rIp) 
	{
		VERDICT	Verdict;
        
		do 
		{				
			{					
				START_NDIS_FILTERS(MJ_PACKET_OUT,FLTTYPE_PID, ProcessName)
					
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_DST_ADDRESS,		htonl(rIp)	    );
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_UDP_DST_PORT,		htons(rPort)	);
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_SRC_ADDRESS,		htonl(lIp)		);
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_UDP_SRC_PORT,		htons(lPort)	);
				
				ADD_UCHAR_PARAM	 (pSingleParam, pParam, ID_IP_PROTOCOL, 0x11); // this is UDP protocol ...
				
				Verdict = FilterEvent(pParam, NULL);
				
				END_NDIS_FILTERS
			}
			
			if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
			{				
				Verdict = Verdict_Discard;
				break;
			}
			
			if (Verdict == Verdict_Default || Verdict == Verdict_Pass || Verdict== Verdict_WDog || Verdict== Verdict_Continue)
			{
				break;
			}
			
			{					
				START_TDI_FILTERS(EVENT_SEND, FLTTYPE_KLPF, Pid, ProcessName)
					
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_REMOTE_IP,		htonl(rIp)	    );
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_REMOTE_PORT,		htons(rPort)	);
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_LOCAL_IP,		htonl(lIp)		);
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_LOCAL_PORT,		htons(lPort)	);
				
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_LOCAL_IP,		htonl(lIp)		); // см. комментарий для Udp-Receive.
				
				ADD_UCHAR_PARAM  (pSingleParam, pParam,  ID_IP_PROTOCOL,	0x11); // this is UDP protocol ...
				
				COPY_BINARY_PARAM(pSingleParam, pParam, ID_FULL_APP_PATH,	ImageName, USERAPPNAMELEN);						
				COPY_BINARY_PARAM(pSingleParam, pParam, ID_FILE_CHECKSUM,	Hash, HASH_SIZE);
				
				Verdict = FilterEvent(pParam, NULL);					
				
				END_TDI_FILTERS	
			}
			
		} while (FALSE);

		if(Verdict == Verdict_Discard)
		{					
			klStatus = KL_DENY_REQUEST;			
		}			
	}
	//-----------------------------------------------------------------------------------------------------
        
    return klStatus;    
}

KLSTATUS
__cdecl
OnRecvDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr)
{       
    return KL_DELAY_REQUEST;
}

KLSTATUS
__cdecl
OnDelayedRecvDG (PVOID Handle, PTA_IP_ADDRESS RemoteAddr)
{    
    KLSTATUS    klStatus = KL_SUCCESS;
    
    ULONG   rIp     = RemoteAddr->Address[0].Address[0].in_addr, 
            lIp     = 0;

    USHORT  rPort   = RemoteAddr->Address[0].Address[0].sin_port, 
            lPort   = 0;

    ULONG   Pid     = 0;
    
    CHAR    ProcessName[KL_PROCNAMELEN];
    CHAR    ImageName[KL_USERAPPNAMELEN];
    CHAR    Hash[KL_HASH_SIZE];
    
    QueryAddr(Handle, QueryLocalIp,     (PCHAR)&lIp,         sizeof (ULONG));
    QueryAddr(Handle, QueryLocalPort,   (PCHAR)&lPort,       sizeof (USHORT));    
    QueryAddr(Handle, QueryPID,         (PCHAR)&Pid,         sizeof (ULONG));
    QueryAddr(Handle, QueryProcess,     (PCHAR)&ProcessName, sizeof(ProcessName));
    QueryAddr(Handle, QueryImage,       (PCHAR)&ImageName,   sizeof(ImageName));
    QueryAddr(Handle, QueryHash,        (PCHAR)&Hash,        sizeof(Hash));

    //---------------------------------------------------------------------------------------------------------					
	if ( FilterEvent && lIp != rIp ) 
	{	
		VERDICT	Verdict;					
		
		do 
		{					
			{
				START_NDIS_FILTERS(MJ_PACKET_IN,FLTTYPE_PID, ProcessName)
					
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_SRC_ADDRESS,		htonl(rIp)	    );
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_UDP_SRC_PORT,		htons(rPort)	);
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_IP_DST_ADDRESS,		htonl(lIp)		);
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_UDP_DST_PORT,		htons(lPort)	);
				
				ADD_UCHAR_PARAM	 (pSingleParam, pParam, ID_IP_PROTOCOL, 0x11); // this is UDP protocol ...
										
				Verdict = FilterEvent(pParam, NULL);
				
				END_NDIS_FILTERS
			}

			if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
			{				
				Verdict = Verdict_Discard;
				break;
			}
			
			if (Verdict == Verdict_Default || Verdict == Verdict_Pass || Verdict== Verdict_WDog || Verdict== Verdict_Continue)
			{				
				break;					
			}

			{					
				START_TDI_FILTERS(EVENT_RECEIVE, FLTTYPE_KLPF, Pid, ProcessName)
					
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_REMOTE_IP,		htonl(rIp)	    );
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_REMOTE_PORT,		htons(rPort)	);
				ADD_ULONG_PARAM  (pSingleParam, pParam, ID_LOCAL_IP,		htonl(lIp)		);
				ADD_USHORT_PARAM (pSingleParam, pParam, ID_LOCAL_PORT,		htons(lPort)	);
				
				ADD_ULONG_PARAM	 (pSingleParam, pParam, ID_LOCAL_IP,		htonl(lIp)			); // Это все потому, что INITIATOR поставить нельзя. Борька по этому параметру поределяет TCP-коннект
				
				ADD_UCHAR_PARAM  (pSingleParam, pParam,  ID_IP_PROTOCOL,	0x11); // this is UDP protocol ...
				
				COPY_BINARY_PARAM(pSingleParam, pParam, ID_FULL_APP_PATH,	ImageName, USERAPPNAMELEN);						
				COPY_BINARY_PARAM(pSingleParam, pParam, ID_FILE_CHECKSUM,	Hash, HASH_SIZE);
				
				Verdict = FilterEvent(pParam, NULL);					
				
				END_TDI_FILTERS	
			}

		} while (FALSE);
		

		if( Verdict == Verdict_Discard  )
		{
			klStatus = KL_DENY_REQUEST;
		}	
					
	}
	//-----------------------------------------------------------------------------------------------------		
        
    return klStatus;
}

/*
 *	Инициализация плагина
 */
KLSTATUS
__cdecl
PluginInit(PPLUGIN_INIT_INFO   PlugInfo, PBASEDRV_INFO  pBaseDrvInfo)
{      
    ULONG       i               = 0;    
    ULONG       CallbackCount   = 0; 
    PLUGIN_CALLBACK* Table;
    
    switch( pBaseDrvInfo->BaseDrvID ) 
    {
    case KLIN_BASE_DRIVER:
        {
            PKLIN_INFO  KlinInfo  = (PKLIN_INFO)pBaseDrvInfo;
            KlinPluginID          = PlugInfo->PluginID;
            CallbackCount         = sizeof ( KlinCallbackTable  ) / sizeof (PLUGIN_CALLBACK);
            Table                 = KlinCallbackTable;
            
            CallBaseDriver  = KlinInfo->IOCTL_Handler;
            QueryConn       = KlinInfo->QueryConn;
            QueryAddr       = KlinInfo->QueryAddr;

            InitializeListHead(&AcceptList);
            AllocateSpinLock(&AcceptListLock);
        }
    	break;    
    default:
        return KL_UNSUCCESSFUL;        
        break;
    }    

    // Заполняем плагинную структуру
    memcpy ( PlugInfo->PluginDescription, PLUGIN_DESCRIPTION, sizeof(PLUGIN_DESCRIPTION) );
    
    PlugInfo->IOCTL_Handler         = MyIoctlHandler;
    PlugInfo->CallbackCount         = CallbackCount;

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

ULONG
Klin2KlpfTaConvert( PVOID    KlinBuffer, ULONG   KlinBufferSize, PVOID KlpfBuffer, ULONG KlpfBufferSize )
{
    ULONG               Count     = *(ULONG*)KlinBuffer;
    PADDR_LIST_ITEM     KlinItem    = (PADDR_LIST_ITEM)((PCHAR)KlinBuffer + sizeof (ULONG));
    PIO_LOCAL_TA_INFO   KlpfItem    = KlpfBuffer;
    
    while ( ( (PCHAR)KlpfItem < (PCHAR)KlpfBuffer + KlpfBufferSize )  && Count )
    {
        RtlCopyMemory ( KlpfItem, KlinItem, sizeof ( ADDR_LIST_ITEM ) );
        KlpfItem++;
        KlinItem++;
        Count--;
    }

    return ((ULONG)KlpfItem - (ULONG)KlpfBuffer);
}

ULONG
Klin2KlpfConnConvert( PVOID    KlinBuffer, ULONG   KlinBufferSize, PVOID KlpfBuffer, ULONG KlpfBufferSize )
{
    ULONG               Count     = *(ULONG*)KlinBuffer;
    PCONN_LIST_ITEM     KlinItem    = (PCONN_LIST_ITEM)((PCHAR)KlinBuffer + sizeof (ULONG));
    PIO_CONN_INFO       KlpfItem    = KlpfBuffer;
    
    while ( ( (PCHAR)KlpfItem < (PCHAR)KlpfBuffer + KlpfBufferSize )  && Count )
    {
        KlpfItem->Handle        = KlinItem->Handle;
        KlpfItem->isIncoming    = KlinItem->isIncoming;
        KlpfItem->LocalIp       = KlinItem->LocalIp;
        KlpfItem->LocalPort     = KlinItem->LocalPort;
        KlpfItem->PID           = KlinItem->PID;
        KlpfItem->RemoteIp      = KlinItem->RemoteIp;
        KlpfItem->RemotePort    = KlinItem->RemotePort;
        KlpfItem->Protocol      = KlinItem->Protocol;

        RtlCopyMemory ( KlpfItem->ProcessName, KlinItem->ProcessName, KL_PROCNAMELEN);
        RtlCopyMemory ( KlpfItem->UserAppName, KlinItem->UserAppName, KL_USERAPPNAMELEN);
        

        KlpfItem++;
        KlinItem++;
        Count--;
    }

    return ((ULONG)KlpfItem - (ULONG)KlpfBuffer);
}

VOID    RememberConnInfo(unsigned long lIp, unsigned short lPort, unsigned long rIp, unsigned short rPort)
{
    IRQL            Irql;
    PACCEPT_INFO    ConnInfo;

    ConnInfo = KL_MEM_ALLOC ( sizeof ( ACCEPT_INFO ) );

    if ( ConnInfo )
    {
        ConnInfo->lIp   = lIp;
        ConnInfo->lPort = lPort;
        ConnInfo->rIp   = rIp;
        ConnInfo->rPort = rPort;

        KlGetSystemTime( &ConnInfo->TimeStamp );

        AcquireSpinLock( &AcceptListLock, &Irql );

        InsertTailList ( &AcceptList, &(ConnInfo->m_ListEntry) );

        ReleaseSpinLock( &AcceptListLock, Irql  );
    }
}

/*
 *	Функция удаляет из листа устаревшие элементы.
 *  Устаревает элемент через 20 секунд.
 */
VOID	RefreshAcceptList()
{	
	KL_TIME	        CurrentTime;	
	IRQL	 		Irql;
    PACCEPT_INFO	ConnInfo, Temp;

    AcquireSpinLock( &AcceptListLock, &Irql );

    KlGetSystemTime(&CurrentTime);

    ConnInfo = (PACCEPT_INFO)AcceptList.Flink;

    if ( IsListEmpty ( &AcceptList ) )
    {
        ReleaseSpinLock(&AcceptListLock, Irql);
        return;
    }

	while( ConnInfo != (PACCEPT_INFO)&AcceptList )
	{
        if ( ConnInfo == NULL )
        {   // случилась лажа            
            InitializeListHead(&AcceptList);
            break;
        }
        
		if ( CurrentTime - ConnInfo->TimeStamp > ONE_SECOND_TIME * 20 )
		{
            Temp        = ConnInfo;
            ConnInfo    = (PACCEPT_INFO)ConnInfo->m_ListEntry.Flink;

			RemoveEntryList( &(Temp->m_ListEntry) );
			ExFreePool( Temp );
			continue;
		}
		
		ConnInfo = (PACCEPT_INFO)ConnInfo->m_ListEntry.Flink;
	}
    
    ReleaseSpinLock(&AcceptListLock, Irql);
}

/*
 *	Если в AcceptList есть структура с такими параметрами - вернуть TRUE, иначе вернуть FALSE
 */
BOOLEAN	CheckAcceptList(unsigned long lIp, unsigned short lPort, unsigned long rIp, unsigned short rPort)
{	
	IRQL			Irql;
    PACCEPT_INFO	ConnInfo;
    BOOLEAN         RetVal = FALSE;

	AcquireSpinLock(&AcceptListLock, &Irql);

    ConnInfo = (PACCEPT_INFO)AcceptList.Flink;

    if ( IsListEmpty ( &AcceptList ) )
    {
        ReleaseSpinLock(&AcceptListLock, Irql);
        return RetVal;
    }
	
    while( ConnInfo != (PACCEPT_INFO)&AcceptList )
	{
        if ( ConnInfo == NULL )
        {   // случилась лажа            
            InitializeListHead(&AcceptList);
            break;
        }
		
		if ( ConnInfo->lIp	  == lIp	 &&
			 ConnInfo->lPort  == lPort   &&
			 ConnInfo->rIp    == rIp     &&
			 ConnInfo->rPort  == rPort )
		{
            RetVal = TRUE;
			break;
		}

		ConnInfo = (PACCEPT_INFO)ConnInfo->m_ListEntry.Flink;
	}
    
	ReleaseSpinLock(&AcceptListLock, Irql);

	return RetVal;
}