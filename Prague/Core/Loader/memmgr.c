#include "memmgr.h"

#if defined (_WIN32)

#include <windows.h>

void* allocate ( unsigned int aSize, unsigned int aProtection )
{
  int aProtectionFlag = PAGE_NOACCESS;
  switch ( aProtection ) {
  case MEMORY_PROTECTION_READ:
    aProtectionFlag = PAGE_READONLY;
    break;
  case (MEMORY_PROTECTION_READ|MEMORY_PROTECTION_WRITE) :
    aProtectionFlag = PAGE_READWRITE;     
    break;
  case MEMORY_PROTECTION_EXECUTE:
    aProtectionFlag = PAGE_EXECUTE;     
    break;
  case (MEMORY_PROTECTION_READ|MEMORY_PROTECTION_EXECUTE):
    aProtectionFlag = PAGE_EXECUTE_READ;     
    break;
  case (MEMORY_PROTECTION_READ|MEMORY_PROTECTION_WRITE|MEMORY_PROTECTION_EXECUTE):
    aProtectionFlag = PAGE_EXECUTE_READWRITE;     
    break;
  default:
    break;
  }
  return VirtualAlloc ( 0, aSize, MEM_RESERVE | MEM_COMMIT, aProtectionFlag );
}

void deallocate ( void* aPointer, unsigned int dummy)
{
  VirtualFree ( aPointer, 0, MEM_RELEASE );  
}

int protect ( void* aPointer, unsigned int aSize, unsigned int aProtection )
{
  DWORD anOldProtection = 0;
  int aProtectionFlag = PAGE_NOACCESS;
  switch ( aProtection ) {
  case MEMORY_PROTECTION_READ:
    aProtectionFlag = PAGE_READONLY;
    break;
  case (MEMORY_PROTECTION_READ|MEMORY_PROTECTION_WRITE) :
    aProtectionFlag = PAGE_READWRITE;     
    break;
  case MEMORY_PROTECTION_EXECUTE:
    aProtectionFlag = PAGE_EXECUTE;     
    break;
  case (MEMORY_PROTECTION_READ|MEMORY_PROTECTION_EXECUTE):
    aProtectionFlag = PAGE_EXECUTE_READ;     
    break;
  case (MEMORY_PROTECTION_READ|MEMORY_PROTECTION_WRITE|MEMORY_PROTECTION_EXECUTE):
    aProtectionFlag = PAGE_EXECUTE_READWRITE;     
    break;
  default:
    break;
  }
  return !VirtualProtect ( aPointer, aSize, aProtectionFlag, &anOldProtection );
}

#else

#include <sys/types.h>
#include <sys/mman.h>

#if !defined (MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

void* allocate ( unsigned int aSize, unsigned int aProtection )
{
  int aProtectionFlag = PROT_NONE;
  if ( aProtection & MEMORY_PROTECTION_READ )
    aProtectionFlag |= PROT_READ;
  if ( aProtection & MEMORY_PROTECTION_WRITE )
    aProtectionFlag |= PROT_WRITE;
  if ( aProtection & MEMORY_PROTECTION_EXECUTE )
    aProtectionFlag |= PROT_EXEC;
  void* aBuffer = mmap (0, aSize, aProtectionFlag, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
  if ( aBuffer == MAP_FAILED )
    return 0;
  return aBuffer;
  
}

void deallocate ( void* aPointer, unsigned int aSize )
{
  munmap ( aPointer, aSize );
}

int protect ( void* aPointer, unsigned int aSize, unsigned int aProtection )
{
  int aProtectionFlag = PROT_NONE;
  if ( aProtection & MEMORY_PROTECTION_READ )
    aProtectionFlag |= PROT_READ;
  if ( aProtection & MEMORY_PROTECTION_WRITE )
    aProtectionFlag |= PROT_WRITE;
  if ( aProtection & MEMORY_PROTECTION_EXECUTE )
    aProtectionFlag |= PROT_EXEC;
  return mprotect ( aPointer, aSize, aProtection );
}
#endif

