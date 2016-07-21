#ifndef _KL_MEMORY_MANAGER_H_
#define _KL_MEMORY_MANAGER_H_

#include "kl_object.h"
#include "kldef.h"
#include "kl_list.h"

class CKl_Allocator : public CKl_Object
{
public:
    virtual void*   Allocate( size_t  AllocSize )   = 0;   // Выделяет память
    virtual void    Free    ( void*   Memory)       = 0;   // Освобождает память
};

/*
 *	Менеджер памяти, которых хранит информацию о выделенной памяти.
 *  Может подчищать за собой мусор
 */

struct MemChunk {
    QUEUE_ENTRY;
    PVOID   Address;
    size_t  Size;
    ULONG   Tag;
};
class CKl_MManager : public CKl_Allocator
{
public:
    CKl_List<MemChunk>  m_ChunkList;        // Список выделенной памяти.
    size_t              m_TotalSize;        // общее число выделенной памяти ( по мнению клиента )

    CKl_MManager();
    virtual ~CKl_MManager();

    void*   Allocate( size_t  AllocSize );
    void    Free    ( void*   Memory)    ;

    void    FreeAll();                                              // Освобождает всю память
};

#endif