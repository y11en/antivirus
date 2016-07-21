#if defined (_WIN32)

#include "virtualmemory.h"

namespace KLSysNS {
  void* vm_allocate ( kl_size_t aSize, Protection aProtection, void* aStartAddress )
  {
    DWORD aProtectionFlag = PAGE_NOACCESS;
    switch ( aProtection ) {
    case KL_VIRTMEM_READ:
      aProtectionFlag = PAGE_READONLY;
      break;
    case KL_VIRTMEM_WRITE:
    case (KL_VIRTMEM_READ|KL_VIRTMEM_WRITE) :
      aProtectionFlag = PAGE_READWRITE;     
      break;
    case KL_VIRTMEM_EXECUTE:
      aProtectionFlag = PAGE_EXECUTE;
      break;
    case (KL_VIRTMEM_READ|KL_VIRTMEM_EXECUTE):
      aProtectionFlag = PAGE_EXECUTE_READ;     
      break;
    case (KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE):
    case (KL_VIRTMEM_READ|KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE):
      aProtectionFlag = PAGE_EXECUTE_READWRITE;     
      break;
    }
    return VirtualAlloc ( aStartAddress, aSize, MEM_COMMIT, aProtectionFlag );
  }

  void vm_deallocate ( void* anAddress, kl_size_t aSize )
  {
    VirtualFree ( anAddress, 0, MEM_RELEASE );
  }
  
  bool vm_protect ( void* anAddress, kl_size_t aSize, Protection aProtection )
  {
    DWORD aProtectionFlag = PAGE_NOACCESS;
    switch ( aProtection ) {
    case KL_VIRTMEM_READ:
      aProtectionFlag = PAGE_READONLY;
      break;
    case KL_VIRTMEM_WRITE:
    case (KL_VIRTMEM_READ|KL_VIRTMEM_WRITE) :
      aProtectionFlag = PAGE_READWRITE;     
      break;
    case KL_VIRTMEM_EXECUTE:
      aProtectionFlag = PAGE_EXECUTE;
      break;
    case (KL_VIRTMEM_READ|KL_VIRTMEM_EXECUTE):
      aProtectionFlag = PAGE_EXECUTE_READ;     
      break;
    case (KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE):
    case (KL_VIRTMEM_READ|KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE):
      aProtectionFlag = PAGE_EXECUTE_READWRITE;     
      break;
    }
    return VirtualProtect ( anAddress, aSize, aProtectionFlag, &aProtectionFlag ) == TRUE;
  }
  
  bool vm_lock ( void* anAddress, kl_size_t aSize )
  {
#ifndef _WIN32_WCE	// by ArtemK
    return VirtualLock ( anAddress, aSize ) == TRUE;
#else
	return TRUE;
#endif
  }
  
  void vm_unlock ( void* anAddress, kl_size_t aSize )
  {
#ifndef _WIN32_WCE
    VirtualUnlock ( anAddress, aSize );
#endif
  }
  
  inline int getSystemPageSize ()
  {
    SYSTEM_INFO aSystemInfo;
    GetSystemInfo ( &aSystemInfo );
    return aSystemInfo.dwPageSize;
  }

  unsigned int vm_pagesize ()
  {
    static unsigned int aPageSize = getSystemPageSize ();
    return aPageSize;
  }
}

#endif // _WIN32
