#include <string.h>

#include <new>

#if defined (__unix__)
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#if !defined (MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#if defined (_WIN32)
#include <windows.h>
#endif

#include "pagememorymanager.h"

namespace PageMemoryManagerNS {

  inline unsigned int align ( unsigned int aSize, unsigned int anAlignment )
  {
    return ( aSize / anAlignment + ( ( aSize % anAlignment ) ? 1 : 0 ) ) * anAlignment;
  }

  struct PageMemoryManager::PageControlBlock {
    PageControlBlock () : theBlockSize ( 0 ), theNextPCB ( 0 ), theFirstUnusedBlock ( 0 ), theNumberOfFreeBytes ( 0 ) {};
    PageControlBlock ( unsigned int aBlockSize ) : 
      theBlockSize ( aBlockSize ), 
      theNextPCB ( 0 ), 
      theFirstUnusedBlock ( 0 ),
      theNumberOfFreeBytes ( theBlockSize - sizeof ( PageControlBlock ) ) 
    {
      theFirstUnusedBlock = reinterpret_cast <unsigned char*> ( this ) + sizeof ( PageControlBlock );
    }

    void * allocate ( unsigned int aSize, unsigned int anAlignment ) 
    {
      unsigned int anOffset = align ( reinterpret_cast <unsigned long> ( theFirstUnusedBlock ), anAlignment ) - 
        reinterpret_cast <unsigned long> ( theFirstUnusedBlock );
      if ( theNumberOfFreeBytes < ( aSize + anOffset ))  
        return 0;
      unsigned char* aResult = theFirstUnusedBlock + anOffset;
      theFirstUnusedBlock = aResult + aSize;
      theNumberOfFreeBytes -= aSize + anOffset;
      return aResult;
    }
    
    unsigned int theBlockSize;
    PageControlBlock* theNextPCB;
    unsigned char* theFirstUnusedBlock;
    unsigned int theNumberOfFreeBytes;
  };



  inline unsigned int getSystemMemoryProtection ( unsigned int aMemoryProtection )
  {
    unsigned int aProtection = 0;
#if defined (__unix__)
    aProtection = PROT_NONE;
    if ( aMemoryProtection & KL_MEMORY_PROTECTION_READ )
      aProtection |= PROT_READ;
    if ( aMemoryProtection & KL_MEMORY_PROTECTION_WRITE )
      aProtection |= PROT_READ|PROT_WRITE;
    if ( aMemoryProtection & KL_MEMORY_PROTECTION_EXECUTE )
      aProtection |= PROT_READ|PROT_EXEC;
#elif defined (_WIN32)
/*    if ( aMemoryProtection & KL_MEMORY_PROTECTION_EXECUTE ) {
      OSVERSIONINFOEX aOSInfo;	
      ZeroMemory ( &aOSInfo, sizeof (aOSInfo));
      aOSInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
      if ( !GetVersionEx ( (OSVERSIONINFO*)&aOSInfo ) ) {
        aOSInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if ( !GetVersionEx ( (OSVERSIONINFO*)&aOSInfo ) )
          return 0;
      }
      if ( ( aOSInfo.dwMajorVersion < 5 ) ||
           ( ( aOSInfo.dwMajorVersion == 5 ) && ( aOSInfo.dwMinorVersion < 1 ) ) ||
           ( ( aOSInfo.dwMajorVersion == 5 ) && ( aOSInfo.dwMinorVersion == 1 ) && ( aOSInfo.wServicePackMajor < 2 ) ) ) 
        aMemoryProtection &= ~KL_MEMORY_PROTECTION_EXECUTE;
    }*/
    aProtection = PAGE_READONLY;
    switch ( aMemoryProtection ) {
    case KL_MEMORY_PROTECTION_READ:
      aProtection = PAGE_READONLY;
      break;
    case (KL_MEMORY_PROTECTION_READ|KL_MEMORY_PROTECTION_WRITE) :
      aProtection = PAGE_READWRITE;     
      break;
    case KL_MEMORY_PROTECTION_EXECUTE:
    case (KL_MEMORY_PROTECTION_READ|KL_MEMORY_PROTECTION_EXECUTE):
      aProtection = PAGE_EXECUTE_READ;     
      break;
    case (KL_MEMORY_PROTECTION_READ|KL_MEMORY_PROTECTION_WRITE|KL_MEMORY_PROTECTION_EXECUTE):
      aProtection = PAGE_EXECUTE_READWRITE;     
      break;
    default:
      break;
    }
#else
#error "This platform is unknown."
#endif
    return aProtection;
  }

  inline unsigned int getSystemPageSize ()
  {
#if defined (__unix__)
    return sysconf(_SC_PAGESIZE);
#elif defined (_WIN32)
    SYSTEM_INFO aSystemInfo;
    GetSystemInfo ( &aSystemInfo );
    return aSystemInfo.dwPageSize;
#else
#error "This platform is unknown."
#endif
  }

  PageMemoryManager::PageMemoryManager ( void* aPlacement, 
                                         unsigned int aBlockSize, 
                                         unsigned int aMemoryProtection  )
    : theMemoryProtection ( aMemoryProtection ),
      theSystemMemoryProtection ( getSystemMemoryProtection ( aMemoryProtection ) ),
      thePCB ( ( aPlacement ) ? new ( aPlacement ) PageControlBlock ( aBlockSize ) : 0 ),
      thePlacement ( ( aPlacement ) ? true : false ),
      theBlockSize ( aBlockSize )
  {}
  
  PageMemoryManager::~PageMemoryManager ()
  {
    deallocate ();
  }

  void* PageMemoryManager::allocate ( unsigned int aSize, unsigned int anAlignment )
  {
    static unsigned int aPageSize = getSystemPageSize ();
    PageControlBlock* aPCB = thePCB;
    PageControlBlock* aPrevPCB = thePCB;
    void* aResult = 0;
    // trying to find a memory block with needed size

    while ( aPCB && aPCB->theNextPCB ) {
      if ( aPCB->theNextPCB->theNumberOfFreeBytes < ( aSize + anAlignment ) ) 
        break;
      aPrevPCB = aPCB;
      aPCB = aPCB->theNextPCB;
    }
    if ( aPCB )
      aResult = aPCB->allocate ( aSize, anAlignment );
    if ( !aResult && thePlacement )
      return 0;
    if ( !aResult ) {
      // if a block hasn't been found, allocate new page of memory
      unsigned int  aBlockSize = align ( ( ( theBlockSize > ( aSize + sizeof ( PageControlBlock ) ) ) ? 
                                           theBlockSize : ( aSize + sizeof ( PageControlBlock ) ) ), aPageSize );
#if defined (__unix__)
      void* aBlock = mmap (0, aBlockSize, theSystemMemoryProtection, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
      if ( aBlock == MAP_FAILED )
        aBlock = 0;
#elif defined (_WIN32)
      void* aBlock = VirtualAlloc ( 0, aBlockSize, MEM_RESERVE | MEM_COMMIT, theSystemMemoryProtection );
#else
#error "This platform is unknown."
#endif
      if ( !aBlock ) 
        return 0;
      aPCB = new ( aBlock ) PageControlBlock ( aBlockSize );
      if ( thePCB )
        aPCB->theNextPCB = thePCB;
      thePCB = aPCB;
      aResult = aPCB->allocate ( aSize, anAlignment );
      // move current PCB to a position according its number of free bytes
    }
    if ( !aResult ) 
      return 0;
    if ( !aPCB->theNextPCB || ( aPCB->theNumberOfFreeBytes >= aPCB->theNextPCB->theNumberOfFreeBytes ) )
      return aResult;
    if ( thePCB == aPCB )
      thePCB = aPCB->theNextPCB;
    else 
      aPrevPCB->theNextPCB = aPCB->theNextPCB;
    PageControlBlock* aTmpPCB = aPCB->theNextPCB;
    aPCB->theNextPCB = 0;
    while ( aTmpPCB && ( aPCB->theNumberOfFreeBytes < aTmpPCB->theNumberOfFreeBytes ) ) {
      aPrevPCB = aTmpPCB;
      aTmpPCB = aTmpPCB->theNextPCB;
    }
    aPrevPCB->theNextPCB = aPCB;
    aPCB->theNextPCB = aTmpPCB;
    return aResult;
  }

  void PageMemoryManager::deallocate () 
  {
    if ( thePlacement ) {
      memset ( thePCB, 0, theBlockSize );
      thePCB = new ( thePCB ) PageControlBlock ( theBlockSize );
      return;
    }
    PageControlBlock* aPCB = thePCB;
    while ( aPCB ) {
      PageControlBlock* aTmpPCB = aPCB->theNextPCB;
#if defined (__unix__)
      munmap ( aPCB, aPCB->theBlockSize );
#elif defined (_WIN32)
      VirtualFree ( aPCB, 0, MEM_RELEASE );
#else
#error "This platform is unknown."
#endif
      aPCB = aTmpPCB;
    }
    thePCB = 0;
  }
  
  void PageMemoryManager::setMemoryProtection ( unsigned int aMemoryProtection )
  {
    if ( thePlacement ) 
      return;

    unsigned int aProtection = getSystemMemoryProtection ( aMemoryProtection );

    PageControlBlock* aPCB = thePCB;
    while ( aPCB ) {
#if defined (__unix__)
      mprotect ( aPCB, aPCB->theBlockSize, aProtection );
#elif defined (_WIN32)
      DWORD anOldProtection;
      VirtualProtect ( aPCB, aPCB->theBlockSize, aProtection, &anOldProtection );
#else
#error "This platform is unknown."
#endif
      aPCB = aPCB->theNextPCB;
    }
    theMemoryProtection = aMemoryProtection;
    theSystemMemoryProtection = aProtection;
  }

  unsigned int PageMemoryManager::getMemoryProtection ()
  {
    return theMemoryProtection;
  }

  bool PageMemoryManager::isAllocatedAddress ( void* ptr )
  {
    PageControlBlock* aPCB = thePCB;
    while ( aPCB ) 
	{
		void* block_ptr = aPCB->theFirstUnusedBlock + aPCB->theNumberOfFreeBytes - aPCB->theBlockSize;
		if( ptr >= block_ptr && ptr < aPCB->theFirstUnusedBlock )
			return true;
		aPCB = aPCB->theNextPCB;
    }
	return false;
  }

} // PageMemoryManagerNS
