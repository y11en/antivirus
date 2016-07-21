#ifndef _PACKET_LOGGER_H_
#define _PACKET_LOGGER_H_

#include "klned\klickplugin.h"
#include "kl_buffer.h"
#include "kl_log.h"
#include "kl_file.h"
#include "../include/logger_api.h"

typedef struct _PagedPacket
{
    QUEUE_ENTRY;
    ULONG   PacketSize; // sizeof ( Buffer )
    ULONG   isIncoming;
    char    Buffer[0];
} PagedPacket;

class CKl_BufferItem : public CKl_LogItem
{
public:
    CKl_File*   m_TargetFile;
    CKl_List<PagedPacket>* m_PagedPacketList;
    FAST_MUTEX  *m_LockMutex;
    
    char*       m_Buffer;
    ULONG       m_BufferSize;
    ULONG       m_isIncoming;

	virtual KLSTATUS	Process();
    
    CKl_BufferItem(CKl_File* file);
	virtual ~CKl_BufferItem();
};

class CKl_PacketLogger   : public CKl_KlickPlugin
{   
public:
    CKl_Log*    m_Log;
	CKl_File*	m_File;
    CKl_List<PagedPacket> m_PagedPacketList;
    FAST_MUTEX  m_LockMutex;
    
    CKl_PacketLogger(char*   PlugName = NULL);
    virtual ~CKl_PacketLogger();
    
    static KLSTATUS KL_CALLBACK ReceiveFrame(PVOID pktContext, void*   plContext);
    static KLSTATUS KL_CALLBACK SendFrame(PVOID pktContext, void*   plContext);
    
    virtual 
    KLSTATUS    OnReceiveIOCTL(ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned );
};

#endif