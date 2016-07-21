#ifndef _G_KEY_H_
#define _G_KEY_H_

#pragma warning( disable : 4200 )

#include "kldef.h"
#include "klstatus.h"
#include "kl_list.h"
#include "..\hook\klop_api.h"

#define CP_HASH_SIZE	16

#pragma pack ( push, 1 )
class CKl_LicenceKey : public CKl_Object
{
public:
	ULONG	m_KeyBufferSize;
	char*	m_KeyBuffer;
	char	m_FullKeyHash[CP_HASH_SIZE];
	
	CKl_LicenceKey(void* Buffer, ULONG BufferSize);
	virtual ~CKl_LicenceKey();

	KLSTATUS GetData ( ULONG StartOffet, ULONG Count, char* Buffer );
};

class CKl_RemoteUserKey : public CKl_Object
{
public:
	QUEUE_ENTRY;
	char	m_MacAddr[6];			// мак адрес удаленного хоста
	char	m_Hash[CP_HASH_SIZE];   // хеш, который пришел от удаленного хоста в ответ.
	
	CKl_RemoteUserKey( char	MacAddr[6], char Hash[CP_HASH_SIZE] );
	virtual ~CKl_RemoteUserKey();
};
#pragma pack ( pop )

KLSTATUS
fn_hash(CKl_LicenceKey* Key, char* Hash);

KLSTATUS
fn_key(CKl_LicenceKey* Key, PVOID pData, ULONG DataSize, char* Hash );

ULONG
fn_crc(char* Data, ULONG DataSize);

KLSTATUS
MakeBuffer( 
		   IN	CKl_LicenceKey*		Key, 
		   IN	PCP_RANGE_LIST		RangeList, 
		   IN	ULONG				crc,
		   OUT	PCHAR*				Buffer,
		   OUT	ULONG*				BufferSize);

#endif // _G_KEY_H_