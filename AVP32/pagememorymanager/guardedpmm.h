#if !defined (_GUARDEDPMM_H_)
#define _GUARDEDPMM_H_

#include "pagememorymanager.h"

namespace PageMemoryManagerNS {

  template <typename Guard> 
  class GuardedPMM: public Guard, protected PageMemoryManager {
  public:
    GuardedPMM ( unsigned int aBlockSize = 0, 
                 int aMemoryProtection = KL_MEMORY_PROTECTION_READ|KL_MEMORY_PROTECTION_WRITE )
      : PageMemoryManager ( aBlockSize, aMemoryProtection )
      {}
    
    GuardedPMM ( void* aPlacement, unsigned int aSize )
      : PageMemoryManager ( aPlacement, aSize )
      {}
    
    void* allocate ( size_t aSize )
      {
        typename Guard::Lock aLock ( *this );
      return MemoryManager::allocate ( aSize );
    }
    
    void deallocate ()
    {
      typename Guard::Lock aLock ( *this );
      PageMemoryManager::deallocate ();
    }
    
    void setMemoryProtection ( memoryProtection aMemoryProtection )
    {
      typename Guard::Lock aLock ( *this );
      PageMemoryManager::setMemoryProtection ( aMemoryProtection );
    }
    
    memoryProtection getMemoryProtection ()
    {
      PageMemoryManager::getMemoryProtection ();  
    }
  }; 

}; // end of namespace PageMemoryManagerNS

#endif
