#if defined (__unix__)

#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include "virtualmemory.h"

#if !defined (MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace KLSysNS {
  void* vm_allocate ( kl_size_t aSize, Protection aProtection, void* aStartAddress )
  {
    int aProtectionFlag = PROT_NONE;
    if ( aProtection & KL_VIRTMEM_READ )
      aProtectionFlag |= PROT_READ;
    if ( aProtection & KL_VIRTMEM_WRITE) 
      aProtectionFlag |= PROT_WRITE;
    if ( aProtection & KL_VIRTMEM_EXECUTE )
      aProtectionFlag |= PROT_EXEC;
    return mmap ( aStartAddress, aSize, aProtectionFlag, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
  }

  void vm_deallocate ( void* anAddress, kl_size_t aSize )
  {
    munmap ( anAddress, aSize );
  }
  
  bool vm_protect ( void* anAddress, kl_size_t aSize, Protection aProtection )
  {
    int aProtectionFlag = PROT_NONE;
    if ( aProtection & KL_VIRTMEM_READ )
      aProtectionFlag |= PROT_READ;
    if ( aProtection & KL_VIRTMEM_WRITE) 
      aProtectionFlag |= PROT_WRITE;
    if ( aProtection & KL_VIRTMEM_EXECUTE )
      aProtectionFlag |= PROT_EXEC;
    return mprotect ( anAddress, aSize, aProtectionFlag ) != -1;
  }
  
  bool vm_lock ( void* anAddress, kl_size_t aSize )
  {
    return mlock ( anAddress, aSize ) == 0;
  }
  
  void vm_unlock ( void* anAddress, kl_size_t aSize )
  {
    munlock ( anAddress, aSize );
  }
  
  inline int getSystemPageSize ()
  {
    return sysconf(_SC_PAGESIZE);
  }

  unsigned int vm_pagesize ()
  {
    static unsigned int aPageSize = getSystemPageSize ();
    return aPageSize;
  }
}

#endif // __unix__
