#if !defined (_GUARDEDMM_H_)
#define _GUARDEDMM_H_

#include "memorymanager.h"

namespace KLSysNS {

  template <typename Guard> 
  class GuardedMM: public Guard, protected MemoryManager {
  public:
    GuardedMM ( unsigned int aBlockSize = 0 )
      : MemoryManager ( aBlockSize )
    {}
    
    GuardedMM ( void* aPlacement, unsigned int aSize )
      : MemoryManager ( aPlacement, aSize )
    {}

    void* allocate ( size_t aSize )
    {
      typename Guard::Lock aLock ( *this );
      return MemoryManager::allocate ( (unsigned)aSize );
    }
    
    bool deallocate ( void* anAddress )
    {
      typename Guard::Lock aLock ( *this );
      return MemoryManager::deallocate ( anAddress );
    }

    void* reallocate ( void* anAddress, size_t aSize )
    {
      typename Guard::Lock aLock ( *this );
      return MemoryManager::reallocate ( anAddress, aSize );
    }

    unsigned int size ( void* anAddress ) 
    {
      typename Guard::Lock aLock ( *this );
      return MemoryManager::size ( anAddress );
    }
  };
} // MemoryManagerNS

#endif // _GUARDEDMM_H_
