#include "g_key.h"
#include "md5.h"



KLSTATUS 
fn_hash(CKl_LicenceKey* Key, char* Hash)
{
	md5_state_t			state;
	
	md5_init( &state );
				
	md5_append( &state, (UCHAR*)Key->m_KeyBuffer, Key->m_KeyBufferSize );
				
	md5_finish( &state, (UCHAR*)Hash );

	return KL_SUCCESS;
}

ULONG
fn_crc(char* Data, ULONG DataSize)
{
	return 0;
}

KLSTATUS
fn_key(CKl_LicenceKey* Key, PVOID pData, ULONG DataSize, char* Hash)
{
	md5_state_t			state;
	
	md5_init( &state );
				
	md5_append( &state, (UCHAR*)Key->m_KeyBuffer, Key->m_KeyBufferSize );
	md5_append( &state, (UCHAR*)pData, DataSize );
				
	md5_finish( &state, (UCHAR*)Hash );

	return KL_SUCCESS;
}

KLSTATUS
MakeBuffer(IN CKl_LicenceKey* Key, IN PCP_RANGE_LIST RangeList, IN ULONG crc, OUT PCHAR* Buffer, OUT ULONG* BufferSize)
{
	PCP_RANGE	pRange = NULL;
	CHAR		RangeBuffer[100];
	char	Hash[CP_HASH_SIZE];
	
	fn_hash( Key, Hash );

	for ( ULONG i = 0; i < RangeList->Count; i++ )
	{
		pRange = &RangeList->Range[i];
		Key->GetData(pRange->StartOffset, pRange->BytesCount, RangeBuffer );
	}

	return KL_SUCCESS;
}

CKl_LicenceKey::CKl_LicenceKey(void* Buffer, ULONG BufferSize)
{
	m_KeyBuffer		= NULL;
	m_KeyBufferSize = 0;

	if ( BufferSize && Buffer )
	{
		m_KeyBuffer = (char*)ExAllocatePoolWithTag( NonPagedPool, BufferSize, 'yekG');

		if ( m_KeyBuffer )
		{
			m_KeyBufferSize = BufferSize;
			RtlCopyMemory ( m_KeyBuffer, Buffer, BufferSize);
			fn_hash( this, m_FullKeyHash );
		}
	}
}

CKl_LicenceKey::~CKl_LicenceKey()
{
	if ( m_KeyBuffer )
	{
		ExFreePool ( m_KeyBuffer );
	}
}

KLSTATUS
CKl_LicenceKey::GetData(ULONG StartOffet, ULONG Count, char* Buffer )
{
	RtlCopyMemory ( (PCHAR)Buffer, (PCHAR)m_KeyBuffer + StartOffet, Count );

	return KL_SUCCESS;
}

//------------------------------------------------------------------------------------------------

CKl_RemoteUserKey::CKl_RemoteUserKey(char MacAddr[6], char Hash[CP_HASH_SIZE] )
{
	RtlCopyMemory( m_MacAddr, MacAddr, sizeof ( m_MacAddr ) );
	RtlCopyMemory( m_Hash, Hash, CP_HASH_SIZE );
}

CKl_RemoteUserKey::~CKl_RemoteUserKey()
{
}
