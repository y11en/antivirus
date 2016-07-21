#include "g_cp_proto.h"

CKl_LicenceKey*				 g_MyLicence		= NULL;		
CKl_List<CKl_RemoteUserKey>* g_RemoteKeyList	= NULL;		
SPIN_LOCK				     g_PCP_Lock;

KL_TIME						 g_StopTime;	// время, когда нужно перестать принимать пакеты.

KLSTATUS
PCP_Init(char* Buffer, ULONG BufferSize)
{
	CKl_AutoLock Lock ( &g_PCP_Lock );
	
	PCP_Done();
	
	g_MyLicence = new CKl_LicenceKey( Buffer, BufferSize );
	g_RemoteKeyList = new CKl_List<CKl_RemoteUserKey>();


	return g_MyLicence ? KL_SUCCESS : KL_UNSUCCESSFUL;
}

void
PCP_Done()
{
	if ( g_MyLicence )
	{
		delete g_MyLicence;
		g_MyLicence = NULL;
	}

	if ( g_RemoteKeyList )
	{
		CKl_RemoteUserKey* RemoteKey = NULL;
		
		while ( RemoteKey = g_RemoteKeyList->InterlockedRemoveHead() )
			delete RemoteKey;

		delete g_RemoteKeyList;
		g_RemoteKeyList = NULL;
	}
}

void
PCP_InterlockedDone()
{
	CKl_AutoLock Lock ( &g_PCP_Lock );

	PCP_Done();
}

void
PCP_SendRequest( )
{
	// CKl_AutoLock Lock ( &g_PCP_Lock );

	KLSTATUS	klStatus;
	PCP_PACKET	Packet;

	RtlZeroMemory ( &Packet, sizeof( PCP_PACKET ) );
	Packet.version	= PCP_VERSION;
	Packet.type		= PCP_REQUEST_PACKET;

	// считаем хеш по ключу, переданному нам при инициализации.
	fn_hash( g_MyLicence, Packet.hash );
	
	// отправляем пакет с этим хешом.
	klStatus = CP_SendLicBuffer( (char*)&Packet, sizeof ( Packet ) );
}

void
PCP_RecvAnswer( char MyMacAddr[6], PETH_HEADER eth, PCP_PACKET* Packet )
{
	// CKl_AutoLock Lock ( &g_PCP_Lock );
	
	// нам пришел ответ от юзера на наш запрос.
	// Packet.hash - это хеш, посчитанный по ключу + MacAddr.
	// Пересчитаем на свой манер. Если хеши совпадут, то добавить в список ключей
	// данный мак-адрес.
	char	hash[CP_HASH_SIZE];

	if ( Packet->type != PCP_ANSWER_PACKET )
		return;

	if ( g_MyLicence )
	{
		fn_key( g_MyLicence, eth->srcMac, sizeof ( eth->srcMac ), hash );
		
		if ( KlCompareMemory ( hash, Packet->hash, CP_HASH_SIZE ) )
		{	
			CKl_RemoteUserKey*	RemoteKey = NULL;
			
			CKl_AutoLock Lock1( &g_RemoteKeyList->m_Lock.Lock );
			
			while ( RemoteKey = g_RemoteKeyList->Next( RemoteKey ) ) 
			{
				if ( KlCompareMemory( eth->srcMac, RemoteKey->m_MacAddr, sizeof ( eth->srcMac ) ) )
				{
					// такой Mac уже есть в списке.
					return;
				}
			}
			
			RemoteKey = new CKl_RemoteUserKey( (CHAR*)eth->srcMac, Packet->hash );
			g_RemoteKeyList->InsertTailNoLock( RemoteKey );
		}
	}
}

void
PCP_RecvRequest( char MyMacAddr[6], PETH_HEADER eth, PCP_PACKET* Packet )
{
	// CKl_AutoLock Lock ( &g_PCP_Lock );

	// Packet->hash содержит хеш ключа.
	if ( Packet->type != PCP_REQUEST_PACKET )
		return;
	
	if ( g_MyLicence )
	{
		// eth - это eth_header отправителя. 
		if ( KlCompareMemory( Packet->hash, g_MyLicence->m_FullKeyHash, CP_HASH_SIZE ) )
		{
			PCP_SendAnswer( MyMacAddr );
		}
	}
}

void
PCP_SendAnswer( char MyMacAddr[6] )
{
	// CKl_AutoLock Lock ( &g_PCP_Lock );

	KLSTATUS	klStatus;
	PCP_PACKET	Packet;

	RtlZeroMemory ( &Packet, sizeof( PCP_PACKET ) );

	Packet.version	= PCP_VERSION;
	Packet.type		= PCP_ANSWER_PACKET;

	fn_key( g_MyLicence, MyMacAddr, 0x6, Packet.hash );
	
	klStatus = CP_SendLicBuffer( (char*)&Packet, sizeof ( Packet ) );
}

#ifndef KLOP_PLUGIN

bool
IsLocalMac( char Mac[6] )
{
	BOOLEAN	bRes;
	CKl_OpenInstance* OI = NULL;
	CKl_AutoLock Lock( &OI_List->m_Lock.Lock );
	
	while ( OI = OI_List->Next( OI ) )
	{
		ETH_COMPARE_NETWORK_ADDRESSES_EQ( OI->CurrentMacAddr, Mac, &bRes );

		if ( bRes == 0 )
			return true;
	}

	return false;
}

#endif

ULONG
PCP_GetKeyCount()
{
	CKl_AutoLock Lock ( &g_PCP_Lock );
	CKl_RemoteUserKey*	RemoteKey = NULL;

	ULONG Count = 0;

	while ( RemoteKey = g_RemoteKeyList->Next(RemoteKey) )
    {
        if ( !IsLocalMac( RemoteKey->m_MacAddr) )
            Count++;
    }		
	
	return Count;
}

ULONG
PCP_GetKeyItems( char* Buffer, ULONG BufferSize )
{
    CKl_AutoLock Lock ( &g_PCP_Lock );
    CKl_RemoteUserKey*	RemoteKey = NULL;

    KlopKeyItem* Item = (KlopKeyItem*)Buffer;

    ULONG Count = 0;

    if ( g_RemoteKeyList == NULL )
        return -1;

    while ( RemoteKey = g_RemoteKeyList->Next(RemoteKey) )
    {   
        if ( !IsLocalMac( RemoteKey->m_MacAddr) )
        {
            if ( ( Count + 1 ) * sizeof ( KlopKeyItem ) > BufferSize )
                return -1;

            RtlCopyMemory( Item->MacAddr, RemoteKey->m_MacAddr, 6 );
            Count++;
            Item++;
        }
    }		

    return Count;
}

void
PCP_ClearRemoteKeys()
{
	CKl_AutoLock Lock ( &g_PCP_Lock );
	CKl_RemoteUserKey*	RemoteKey = NULL;

	if ( g_RemoteKeyList )
	{
		CKl_RemoteUserKey* RemoteKey = NULL;
		while ( RemoteKey = g_RemoteKeyList->InterlockedRemoveHead() )
			delete RemoteKey;
	}
}