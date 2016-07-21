#include "kl_mm.h"

CKl_MManager::CKl_MManager()
{
    m_TotalSize = 0;
}


CKl_MManager::~CKl_MManager()
{
    FreeAll();
}

void*
CKl_MManager::Allocate(size_t AllocSize )
{
    MemChunk* Chunk;

    if ( Chunk = (MemChunk*)KL_MEM_ALLOC( AllocSize + sizeof ( MemChunk ) ) )
    {
        Chunk->Address = (char*)Chunk + sizeof ( MemChunk );
        Chunk->Size    = AllocSize;
        Chunk->Tag     = COMMON_TAG;

        m_TotalSize += Chunk->Size;

        m_ChunkList.InsertHead( Chunk );

        return Chunk->Address;
    }

    return NULL;
}


void
CKl_MManager::Free( void* Memory )
{
    if ( Memory )
    {
        MemChunk* Chunk = (MemChunk*)( (char*)Memory - sizeof ( MemChunk ) );
        
        m_ChunkList.InterlockedRemove( Chunk );

        m_TotalSize -= Chunk->Size;

        KL_MEM_FREE( Chunk );
    }
}

void
CKl_MManager::FreeAll()
{
    MemChunk* Chunk = NULL;
    
    while ( Chunk = m_ChunkList.InterlockedRemoveTail() )
    {
        KL_MEM_FREE( Chunk );
    }
}
