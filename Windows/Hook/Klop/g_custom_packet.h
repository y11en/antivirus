#ifndef _CUSTOM_PACKET_H_
#define _CUSTOM_PACKET_H_

#include "klstatus.h"
#include "kldef.h"
#include "kl_lock.h"
#include "g_key.h"
#include "kl_list.h"
#include "..\hook\klop_api.h"

#ifndef KLOP_PLUGIN

#include "klopevent.h"
#include "g_klop_ndis.h"

#endif

#define     CP_PROTOCOL     139

KLSTATUS
CP_Init();

KLSTATUS
CP_Done();

KLSTATUS
CP_AddAdapter( CP_ADAPTER_INFO* pAI );

KLSTATUS
CP_SetBroadcast( CP_IFACE_INFO* pII, ULONG Count );

KLSTATUS
CP_RemoveAdapter(TCHAR*  AdapterName, ULONG AdapterNameSize );

KLSTATUS
CP_Send(char*  Packet, ULONG PacketSize);

KLSTATUS
CP_Receive(char*  Packet, ULONG* PacketSize);

KLSTATUS
OnPacketReceive( NDIS_HANDLE ProtocolBindingContext, PVOID HeaderBuffer, PVOID  IpPacket, ULONG PacketSize );

KLSTATUS
CP_SendLicBuffer(char* Buffer, ULONG BufferSize);

class CKl_FuncDebug
{
	char*	m_message;
public:
	
	CKl_FuncDebug(char* Message) : m_message(Message)
	{
#ifdef _DEBUG
		DbgPrint ( "==> %s\n", m_message );
#endif
	}
	CKl_FuncDebug::~CKl_FuncDebug()
	{
#ifdef _DEBUG
		DbgPrint ( "<== %s\n", m_message );
#endif
	}
};

#endif