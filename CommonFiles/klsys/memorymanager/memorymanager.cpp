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

#include "memorymanager.h"

const unsigned int theAlignment = 4; 

namespace KLSysNS {

  inline unsigned int align ( unsigned int aSize, unsigned int anAlignment )
  {
    return (aSize +(anAlignment-1)) & ~(anAlignment-1);
  }

  inline unsigned char* align ( void* anAddress, unsigned int anAlignment )
  {
    return reinterpret_cast<unsigned char*>((reinterpret_cast<unsigned long>(anAddress) + (anAlignment - 1)) & ~(anAlignment - 1));
  }

  struct MemoryControlBlock {
    MemoryControlBlock ( MemoryControlBlock* aNextMCB = 0,
                         MemoryControlBlock* aNextFreeMCB = 0,
                         MemoryControlBlock* aPrevFreeMCB = 0 ) 
      : theNextMCB ( aNextMCB ),
        theNextFreeMCB ( aNextFreeMCB ), 
        thePrevFreeMCB ( aPrevFreeMCB )
    {}



    MemoryControlBlock ( unsigned int aBlockSize  ) 
      : theNextMCB ( 0 ),
        theNextFreeMCB ( 0 ), 
        thePrevFreeMCB ( 0 )
        
    {
      static unsigned int aMCBSize = align ( sizeof ( MemoryControlBlock ), theAlignment );
      theNextMCB = new ( reinterpret_cast <unsigned char*> ( this ) +  aBlockSize - aMCBSize ) MemoryControlBlock;
    }

    unsigned int size () 
    {
      if ( !theNextMCB )
        return 0;
      return static_cast<unsigned int>(reinterpret_cast <unsigned char*> ( theNextMCB ) - align ( &theNextFreeMCB, theAlignment ));
    }
    
    unsigned char* getBlockAddress ()
    {
      return align ( &theNextFreeMCB, theAlignment );
    }

    MemoryControlBlock* theNextMCB;     
    MemoryControlBlock* theNextFreeMCB;
    MemoryControlBlock* thePrevFreeMCB;
  };


  struct MemoryManager::PageControlBlock {
    PageControlBlock ( unsigned int aBlockSize ) 
      : theBlockSize ( aBlockSize ),
        theNextPCB ( 0 )
    {
      static unsigned int aPCBSize = align ( sizeof ( PageControlBlock ), theAlignment );
      theFirstUnusedBlock = new ( reinterpret_cast<unsigned char*> ( this ) + aPCBSize ) MemoryControlBlock ( theBlockSize - aPCBSize );
    }

    void* allocate ( unsigned int aSize )
    {
      checkConsistensy ();
      MemoryControlBlock* aMCB = theFirstUnusedBlock;
      MemoryControlBlock* aPrevFreeMCB = 0;
      aSize = align ( ( aSize > ( sizeof ( MemoryControlBlock ) - sizeof ( aMCB->theNextMCB ) ) ) ? aSize : 
                      ( sizeof ( MemoryControlBlock ) - sizeof ( aMCB->theNextMCB ) ), 
                      theAlignment );
      while ( aMCB ) {
        if ( aMCB->size () >= aSize )
          break;
        aPrevFreeMCB = aMCB;
        aMCB = aMCB->theNextFreeMCB;
      }
      if ( !aMCB )
        return 0;
      unsigned char* aResult = aMCB->getBlockAddress ();
      static unsigned int aMCBSize = align ( sizeof ( MemoryControlBlock ), theAlignment );
      if ( ( aMCB->size () - aSize ) > aMCBSize ) {
        aMCB->theNextMCB = new ( aResult + aSize ) MemoryControlBlock ( aMCB->theNextMCB, aMCB->theNextFreeMCB, aPrevFreeMCB );
        if ( aPrevFreeMCB ) 
          aPrevFreeMCB->theNextFreeMCB = aMCB->theNextMCB;
        if ( theFirstUnusedBlock == aMCB )
          theFirstUnusedBlock = aMCB->theNextMCB;
      }
      else {
        if ( aPrevFreeMCB ) {
          aPrevFreeMCB->theNextFreeMCB = aMCB->theNextFreeMCB;
          if ( aPrevFreeMCB->theNextFreeMCB )
            aPrevFreeMCB->theNextFreeMCB->thePrevFreeMCB = aPrevFreeMCB;
        }
        if ( theFirstUnusedBlock == aMCB )
          theFirstUnusedBlock = aMCB->theNextFreeMCB;
      }
      aMCB->theNextFreeMCB = aMCB->thePrevFreeMCB = 0;
      checkConsistensy ();
      return aResult;
    }


    bool deallocate ( void* anAddress )
    {
      checkConsistensy ();
      if ( !isOwner ( anAddress ) )
        return false;
      static unsigned int aMCBSize = align ( sizeof ( MemoryControlBlock* ), theAlignment );
      MemoryControlBlock* aMCB = reinterpret_cast<MemoryControlBlock*> ( reinterpret_cast <unsigned char*> ( anAddress ) - aMCBSize  );
      memset ( aMCB->getBlockAddress (), 0, aMCB->size () );
      if ( ( theFirstUnusedBlock > aMCB ) || !theFirstUnusedBlock )  {
        if ( theFirstUnusedBlock ) {
          aMCB->theNextFreeMCB = theFirstUnusedBlock;
          theFirstUnusedBlock->thePrevFreeMCB = aMCB;
        }
        theFirstUnusedBlock = aMCB;
      }
      else {
        MemoryControlBlock* aTmpMCB = theFirstUnusedBlock;
        while ( aTmpMCB->theNextFreeMCB && ( aTmpMCB->theNextFreeMCB < aMCB ) )
          aTmpMCB = aTmpMCB->theNextFreeMCB;
        if ( aTmpMCB->theNextMCB == aMCB ) {
          aTmpMCB->theNextMCB = aMCB->theNextMCB;
          aMCB->theNextMCB = 0;
          aMCB = aTmpMCB;
        }
        else {
          aMCB->theNextFreeMCB = aTmpMCB->theNextFreeMCB;
          aMCB->thePrevFreeMCB = aTmpMCB;
          if ( aTmpMCB->theNextFreeMCB )
            aTmpMCB->theNextFreeMCB->thePrevFreeMCB = aMCB;
          aTmpMCB->theNextFreeMCB = aMCB;
        }

      }
      if ( aMCB->theNextFreeMCB && ( aMCB->theNextFreeMCB == aMCB->theNextMCB ) ) {
        MemoryControlBlock* aTmpMCB = aMCB->theNextMCB;
        aMCB->theNextMCB = aTmpMCB->theNextMCB;
        aMCB->theNextFreeMCB = aTmpMCB->theNextFreeMCB;
        if ( aMCB->theNextFreeMCB )
          aMCB->theNextFreeMCB->thePrevFreeMCB = aMCB;
        aTmpMCB->theNextMCB = aTmpMCB->theNextFreeMCB = aTmpMCB->thePrevFreeMCB = 0;
        
      }
      checkConsistensy ();
      return true;
    }
    
    inline bool isOwner ( void* anAddress )
    {
      if ( reinterpret_cast <unsigned char*> ( anAddress ) < reinterpret_cast <unsigned char*> ( this ) ||
           reinterpret_cast <unsigned char*> ( anAddress ) > ( reinterpret_cast <unsigned char*> ( this ) + theBlockSize ) )
        return false;
      return true;
    }
    
    inline bool isEmpty () 
    {
      static unsigned int aPCBSize = align ( sizeof ( PageControlBlock ), theAlignment );
      static unsigned int aMCBSize = align ( sizeof ( MemoryControlBlock ), theAlignment );
      if ( theFirstUnusedBlock &&  
           ( reinterpret_cast<unsigned char*> ( theFirstUnusedBlock ) == ( reinterpret_cast<unsigned char*> ( this ) + 
                                                                           aPCBSize ) ) &&
           ( reinterpret_cast<unsigned char*> ( theFirstUnusedBlock->theNextMCB ) == ( reinterpret_cast<unsigned char*> ( this ) + 
                                                                                       theBlockSize - aMCBSize ) ) )
        return true;
      return false;
    }

    inline void checkConsistensy ()
    {
#if defined (CHECK_CONSISTENSY)
      static unsigned int aPCBSize = align ( sizeof ( PageControlBlock ), theAlignment );
      static unsigned int aMCBSize = align ( sizeof ( MemoryControlBlock ), theAlignment );
      MemoryControlBlock* aMCB = reinterpret_cast<MemoryControlBlock*>( reinterpret_cast<unsigned char*> ( this ) + aPCBSize );
      MemoryControlBlock* aPrevMCB = 0;
      MemoryControlBlock* aFreeMCB = theFirstUnusedBlock;
      MemoryControlBlock* aPrevFreeMCB = 0;      
      while ( aMCB ) {
        if ( aMCB == aFreeMCB ) {
          aPrevFreeMCB = aFreeMCB;
          aFreeMCB = aFreeMCB->theNextFreeMCB;
        }
        aPrevMCB = aMCB;
        aMCB = aMCB->theNextMCB;
      }
      if ( aPrevMCB != reinterpret_cast<MemoryControlBlock*>(reinterpret_cast<unsigned char*> ( this ) +  theBlockSize - aMCBSize ) )
        throw -1;
      if ( aFreeMCB )
        throw -1;
#endif
    }

    unsigned int theBlockSize;
    PageControlBlock* theNextPCB;
    MemoryControlBlock* theFirstUnusedBlock;
  };

  inline int getSystemPageSize ()
  {
#if defined (__unix__)
    return sysconf(_SC_PAGESIZE);
#endif
#if defined (_WIN32)
    SYSTEM_INFO aSystemInfo;
    GetSystemInfo ( &aSystemInfo );
    return aSystemInfo.dwPageSize;
#endif
  }


  MemoryManager::MemoryManager ( unsigned int aBlockSize )
    : thePCB ( 0 ),
      thePlacement ( false ),
      theBlockSize ( align ( aBlockSize, theAlignment ) )
  {}

  MemoryManager::MemoryManager ( void* aPlacement, unsigned int aSize )
    : thePCB ( new ( aPlacement ) PageControlBlock ( aSize ) ),
      thePlacement ( true ),
      theBlockSize ( aSize )
  {}
  
  MemoryManager::~MemoryManager ()
  {
    if ( thePlacement )
      return;
    PageControlBlock* aPCB = thePCB;
    while ( aPCB ) {
      PageControlBlock* aTmpPCB = aPCB->theNextPCB;
#if defined (__unix__)
      munmap ( aPCB, aPCB->theBlockSize );
#endif      
#if defined (_WIN32)
      VirtualFree ( aPCB, 0, MEM_RELEASE );
#endif
      aPCB = aTmpPCB;
    }
  }
  
  void* MemoryManager::allocate ( unsigned int aSize )
  {
    static unsigned int aPageSize = getSystemPageSize ();
    PageControlBlock* aPCB = thePCB;
    PageControlBlock* aPrevPCB = thePCB;
    // trying to find a memory block with needed size
    while ( aPCB ) {
      void* aResult = aPCB->allocate ( aSize );
      if ( aResult )
        return aResult;
      aPrevPCB = aPCB;
      aPCB = aPCB->theNextPCB;
    }
    if ( !aPCB && thePlacement )
      return 0;
    // if the block hasn't been found, allocate new pages of memory
    aSize = align ( aSize, theAlignment ); 
    unsigned int  aBlockSize = align ( ( ( theBlockSize > ( aSize + sizeof ( PageControlBlock ) ) ) ? 
                                         theBlockSize : ( aSize + sizeof ( PageControlBlock ) ) ), aPageSize );
#if defined (__unix__)
    void * aBlock = mmap ( 0, aBlockSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
    if ( aBlock == MAP_FAILED )
      aBlock = 0;
#endif
#if defined (_WIN32)
    void * aBlock = VirtualAlloc ( 0, aBlockSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
#endif
    if ( !aBlock ) 
      return 0;
    aPCB = new ( aBlock ) PageControlBlock ( aBlockSize );
    if ( !thePCB )
      thePCB = aPCB;
    else
      aPrevPCB->theNextPCB = aPCB;
    return aPCB->allocate ( aSize );
  }
  
  bool MemoryManager::deallocate ( void* anAddress ) 
  {
    PageControlBlock* aPCB = thePCB;
    PageControlBlock* aPrevPCB = 0;
    while ( aPCB ) {
      if ( aPCB->deallocate ( anAddress ) )
        break;
      aPrevPCB = aPCB;
      aPCB = aPCB->theNextPCB;
    }
    if ( !aPCB )
      return false;
    if ( !aPCB->isEmpty () )
      return true;
    if ( aPrevPCB )
      aPrevPCB->theNextPCB = aPCB->theNextPCB;
    else
      thePCB = aPCB->theNextPCB;
#if defined (__unix__)
    munmap ( aPCB, aPCB->theBlockSize );
#endif      
#if defined (_WIN32)
    VirtualFree ( aPCB, 0, MEM_RELEASE );
#endif
    return true;
  }


  void* MemoryManager::reallocate ( void* anAddress, unsigned int aSize )
  {
    void* aResult = 0;
    if ( aSize && ( ( aResult = allocate ( aSize ) ) != 0 ) && anAddress ) {
      unsigned int anOldSize = size ( anAddress );
      memmove ( aResult, anAddress, ( ( aSize > anOldSize ) ? anOldSize : aSize ) );
      deallocate ( anAddress );
    }
    return aResult;
  }
    
  unsigned int MemoryManager::size ( void* anAddress )
  {
    PageControlBlock* aPCB = thePCB;
    while ( aPCB ) {
      if ( aPCB->isOwner ( anAddress ) ) {
        static unsigned int anAlignment = align ( sizeof ( MemoryControlBlock* ), theAlignment );
        MemoryControlBlock* aMCB = reinterpret_cast<MemoryControlBlock*> ( reinterpret_cast <unsigned char*> ( anAddress ) - 
                                                                           anAlignment );
        return aMCB->size ();
      }
      aPCB = aPCB->theNextPCB;
    }
    return 0; 
  }
  

} // KLSysNS
