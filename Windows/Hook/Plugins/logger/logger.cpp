#include "logger.h"
#include "klned\Ids_common.h"
#include "descriptions.h"

PLUGIN_CALLBACK PacketLoggerTable[] = {	    
    { CB_PACKET_RECEIVE, CKl_PacketLogger::ReceiveFrame, PRIORITY_HIGH + 2 },
    { CB_PACKET_SEND,    CKl_PacketLogger::SendFrame,    PRIORITY_HIGH + 2 }
};

CKl_PacketLogger::CKl_PacketLogger(char* PlugName /* = NULL */) : CKl_KlickPlugin( PlugName )
{
    AddTable( PacketLoggerTable, sizeof ( PacketLoggerTable ) / sizeof ( PLUGIN_CALLBACK ) );
	
	m_File = new CKl_File("C:\\packets.log");

    m_Log  = new CKl_Log( NULL );

    ExInitializeFastMutex( &m_LockMutex );
}

CKl_PacketLogger::~CKl_PacketLogger()
{
    if ( m_Registered )
    {
        Unregister();
    }
    
    if ( m_Log )
    {
        delete m_Log;
    }

    PagedPacket*    pgPkt;
    ExAcquireFastMutexUnsafe( &m_LockMutex );
    while ( pgPkt = m_PagedPacketList.RemoveHead() )
    {
        ExFreePool ( pgPkt );
    }
    ExReleaseFastMutexUnsafe( &m_LockMutex );
	if ( m_File )
		delete m_File;
}

KLSTATUS
CKl_PacketLogger::OnReceiveIOCTL(ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned )
{
    KLSTATUS klStatus = KL_UNSUCCESSFUL;
    PPLUGIN_MESSAGE pm = (PPLUGIN_MESSAGE)pInBuffer;

    switch( pm->MessageID ) 
    {
    case MSG_GET_PACKET:
        {
            if ( InBufferSize < sizeof ( PLUGIN_MESSAGE ) || OutBufferSize < sizeof ( LOGGER_PACKET) )
            {
                return KL_BUFFER_TOO_SMALL;
            }

            PLOGGER_PACKET logger_pkt = (PLOGGER_PACKET)pOutBuffer;
            
            PagedPacket*    pgPkt;
            ExAcquireFastMutexUnsafe( &m_LockMutex );
            pgPkt = m_PagedPacketList.RemoveHead();
            ExReleaseFastMutexUnsafe( &m_LockMutex );
            
            if ( pgPkt )
            {
                logger_pkt->isIncoming = pgPkt->isIncoming;
                logger_pkt->PacketSize = pgPkt->PacketSize;
                RtlCopyMemory ( logger_pkt->PacketBuffer, pgPkt->Buffer, pgPkt->PacketSize );
                ExFreePool ( pgPkt );
                
                *nReturned = sizeof ( LOGGER_PACKET );
                klStatus = KL_SUCCESS;
            }
        }        
        
    	break;
    case MSG_SET_FILE_NAME:
        klStatus = KL_SUCCESS;
    	break;
    default:
        break;
    }

    return klStatus;
}

KLSTATUS
KL_CALLBACK
CKl_PacketLogger::ReceiveFrame(PVOID pktContext, void*   plContext)
{
    char    PacketBuf[1600];
    ULONG   Copied;
	ULONG   Size = 0;
    
    CKl_PacketLogger*   _This = (CKl_PacketLogger*)plContext;

	Size = ((PKLICK_INFO)(_This->m_ImportTable))->GetPacketSize( pktContext );
	Copied = ((PKLICK_INFO)(_This->m_ImportTable))->GetLinearBuffer(pktContext, PacketBuf, 0, Size );
    
    CKl_BufferItem*  MyBuf = new CKl_BufferItem(_This->m_File);
    
    if ( MyBuf )
    {
        MyBuf->m_isIncoming = true;
        MyBuf->m_BufferSize = Copied;
        MyBuf->m_Buffer = (char*)KL_MEM_ALLOC ( Copied );
        MyBuf->m_PagedPacketList = &_This->m_PagedPacketList;
        MyBuf->m_LockMutex = &_This->m_LockMutex;
        RtlCopyMemory ( MyBuf->m_Buffer, PacketBuf, Copied);            
        
        _This->m_Log->AddItem( (CKl_LogItem*)MyBuf );
    }
    
        
    return KL_SUCCESS;
}

KLSTATUS
KL_CALLBACK
CKl_PacketLogger::SendFrame(PVOID pktContext, void*   plContext)
{
    char    PacketBuf[1600];
    ULONG   Copied;
    
    CKl_PacketLogger*   _This = (CKl_PacketLogger*)plContext;

	Copied = ((PKLICK_INFO)(_This->m_ImportTable))->GetLinearBuffer(pktContext, PacketBuf, 0, sizeof(PacketBuf) );
    
    
    CKl_BufferItem*  MyBuf = new CKl_BufferItem(_This->m_File);
    
    if ( MyBuf )
    {
        MyBuf->m_isIncoming = false;
        MyBuf->m_BufferSize = Copied;
        MyBuf->m_Buffer = (char*)KL_MEM_ALLOC ( Copied );
        MyBuf->m_PagedPacketList = &_This->m_PagedPacketList;
        MyBuf->m_LockMutex = &_This->m_LockMutex;
        RtlCopyMemory ( MyBuf->m_Buffer, PacketBuf, Copied);            
        
        _This->m_Log->AddItem( (CKl_LogItem*)MyBuf );
    }
    
        
    return KL_SUCCESS;
}

CKl_BufferItem::CKl_BufferItem(CKl_File* file) : CKl_LogItem()
{
    m_Buffer     = NULL;
    m_TargetFile = file;
}

CKl_BufferItem::~CKl_BufferItem()
{
    if ( m_Buffer )
        KL_MEM_FREE( m_Buffer );
}

// to copy packet from NonPagedPool to PagedPool
KLSTATUS CKl_BufferItem::Process()
{
    if ( m_TargetFile && m_Buffer && m_BufferSize )
    {
        // return KL_SUCCESS;

        PagedPacket* pgPacket = (PagedPacket*)ExAllocatePoolWithTag( PagedPool, sizeof ( PagedPacket ) + m_BufferSize, '1NeG' );
        if ( pgPacket )
        {
            pgPacket->isIncoming = m_isIncoming;
            pgPacket->PacketSize = m_BufferSize;
            RtlCopyMemory ( pgPacket->Buffer, m_Buffer, m_BufferSize );
            
            ExAcquireFastMutexUnsafe( m_LockMutex );
            m_PagedPacketList->InsertTailNoLock( pgPacket );
            ExReleaseFastMutexUnsafe ( m_LockMutex );
            return KL_SUCCESS;
        }

    }
	
	return KL_UNSUCCESSFUL;
}

