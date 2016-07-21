#if ! defined (_LINKSPMM_H_)
#define _LINKSPMM_H_

#include <pagememorymanager/pagememorymanager.h>

class LinksPMM {
public:
  LinksPMM ( void* anReadExecuteBlock = 0, unsigned int anReadExecuteBlockSize = 0, 
             void* aReadWriteBlock = 0, unsigned int aReadWriteBlockSize = 0,
             void* aReadOnlyBlock = 0, unsigned int aReadOnlyBlockSize = 0 )
    : theReadExecuteMemory ( anReadExecuteBlock, anReadExecuteBlockSize ), 
      theReadOnlyMemory ( aReadOnlyBlock, aReadOnlyBlockSize ), 
      theReadWriteMemory ( aReadWriteBlock, aReadWriteBlockSize )
  {}

  void* allocateReadExecuteMemory ( unsigned int aSize, unsigned int anAlignment )
  {
    return theReadExecuteMemory.allocate ( aSize, anAlignment );
  }

  void* allocateReadOnlyMemory ( unsigned int aSize, unsigned int anAlignment )
  {
    return theReadOnlyMemory.allocate ( aSize, anAlignment );
  }

  void* allocateReadWriteMemory ( unsigned int aSize, unsigned int anAlignment )
  {
    return theReadWriteMemory.allocate ( aSize, anAlignment );
  }

  void setMemoryProtection ()
  {
    theReadExecuteMemory.setMemoryProtection ( KL_MEMORY_PROTECTION_READ | KL_MEMORY_PROTECTION_EXECUTE );
    theReadOnlyMemory.setMemoryProtection ( KL_MEMORY_PROTECTION_READ );
    theReadWriteMemory.setMemoryProtection ( KL_MEMORY_PROTECTION_READ | KL_MEMORY_PROTECTION_WRITE );
  }

  void deallocate ()
  {
    theReadExecuteMemory.deallocate ();
    theReadOnlyMemory.deallocate ();
    theReadWriteMemory.deallocate ();
  }

private:
  typedef PageMemoryManagerNS::PageMemoryManager PageMemoryManager;
  PageMemoryManager theReadExecuteMemory;
  PageMemoryManager theReadOnlyMemory;
  PageMemoryManager theReadWriteMemory;
};


#endif // _LINKSPMM_H_
