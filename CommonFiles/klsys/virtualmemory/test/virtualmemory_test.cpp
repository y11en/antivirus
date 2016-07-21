#if defined (__unix__)
#include <signal.h>
#include <setjmp.h>
#endif

#if defined (_WIN32)
#include <windows.h>

#define TRY     __try
#define EXCEPT  __except (EXCEPTION_EXECUTE_HANDLER) 
#endif

#include <iostream>
#include <kl_platform.h>
#include "../virtualmemory.h"

using namespace std;
using namespace KLSysNS;

#if defined (__unix__)

jmp_buf theJumpBuffer;

void SIGSEGVhandler ( int )
{
  siglongjmp ( theJumpBuffer, -1 );
}

#define TRY if ( !sigsetjmp ( theJumpBuffer, 1 ) )
#define EXCEPT else
#endif




typedef void (*dummy) (void);

void testBlock ( void* anAddress, unsigned int aSize, bool testExecute = false )
{
  unsigned char aChar = 0;
  cerr << "try to read:"; 
  TRY {
        aChar = *( reinterpret_cast <unsigned char*> ( anAddress ) + aSize / 2 );
        cerr << "OK" << endl;
      }
  EXCEPT {
        cerr << "Error" << endl;
      }

  TRY {
        cerr << "try to write:";
        *( reinterpret_cast <unsigned char*> ( anAddress ) + aSize / 2 ) = aChar;
        cerr << "OK" << endl;
      }
  EXCEPT {
        cerr << "Error" << endl;
      }

  if ( testExecute ) {
    TRY {
          cerr << "try to execute:";
          dummy aDummy = (dummy)anAddress;
          aDummy ();
          cerr << "OK" << endl;
        }
    EXCEPT {
          cerr << "Error" << endl;
        }
  }
  if ( vm_lock ( anAddress, aSize ) ) {
    vm_unlock ( anAddress, aSize );
    cerr << "The memory block has been locked and unlocked successfully" << endl;
  }
  else
    cerr << "Can't lock the memory block" << endl;
  cerr << "----------------------------------------------" << endl;
}


void testProtection () 
{
  cerr << "***** Test of protections of an allocated memory block" << endl;
  unsigned int aSize = 100;
  void* anAddress = vm_allocate ( aSize );
  cerr << "Allocated a block of memory with size " << aSize << " bytes: " << anAddress << endl;
#if defined (KL_ARCH_X86)  
  *( reinterpret_cast <unsigned char*> ( anAddress ) ) = 0xC3;
#elif defined (KL_ARCH_PPC)  
  *( reinterpret_cast <unsigned int*> ( anAddress ) ) = 0x4e800020;
#else
#error "Consideration is needed!"
#endif
  if ( vm_protect ( anAddress, aSize, KL_VIRTMEM_READ ) )
    cerr << "The memory block has been protected successfully for READ" << endl;
  else
    cerr << "Can't protect the memory block for READ" << endl;
  
  testBlock ( anAddress, aSize, true );

  if ( vm_protect ( anAddress, aSize, KL_VIRTMEM_WRITE ) )
    cerr << "The memory block has been protected successfully for WRITE" << endl;
  else
    cerr << "Can't protect the memory block for WRITE" << endl;

  testBlock ( anAddress, aSize, true );

  if ( vm_protect ( anAddress, aSize, KL_VIRTMEM_EXECUTE ) )
    cerr << "The memory block has been protected successfully for EXECUTE" << endl;
  else
    cerr << "Can't protect the memory block for EXECUTE" << endl;

  testBlock ( anAddress, aSize, true );

  if ( vm_protect ( anAddress, aSize, KL_VIRTMEM_READ|KL_VIRTMEM_WRITE ) )
    cerr << "The memory block has been protected successfully for READ|WRITE" << endl;
  else
    cerr << "Can't protect the memory block for READ|WRITE" << endl;

  testBlock ( anAddress, aSize, true );  

  if ( vm_protect ( anAddress, aSize, KL_VIRTMEM_READ|KL_VIRTMEM_EXECUTE ) )
    cerr << "The memory block has been protected successfully for READ|EXECUTE" << endl;
  else
    cerr << "Can't protect the memory block for READ|EXECUTE" << endl;

  testBlock ( anAddress, aSize, true );  

  if ( vm_protect ( anAddress, aSize, KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE ) )
    cerr << "The memory block has been protected successfully for WRITE|EXECUTE" << endl;
  else
    cerr << "Can't protect the memory block for WRITE|EXECUTE" << endl;

  testBlock ( anAddress, aSize, true ); 

  if ( vm_protect ( anAddress, aSize,  KL_VIRTMEM_READ|KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE ) )
    cerr << "The memory block has been protected successfully for READ|WRITE|EXECUTE" << endl;
  else
    cerr << "Can't protect the memory block for READ|WRITE|EXECUTE" << endl;

  testBlock ( anAddress, aSize, true );

  vm_deallocate ( anAddress, aSize );
}


void testAllocation () 
{
  cerr << "***** Test of allocating memory blocks with different protections" << endl;
  unsigned int aSize = 100;
  void* anAddress = 0;
  if ( ( anAddress = vm_allocate ( aSize, KL_VIRTMEM_READ ) ) != 0  )
    cerr << "The memory block has been allocated successfully for READ" << endl;
  else
    cerr << "Can't allocate the memory block for READ" << endl;
  
  testBlock ( anAddress, aSize );

  vm_deallocate ( anAddress, aSize );

  if ( ( anAddress = vm_allocate ( aSize, KL_VIRTMEM_WRITE ) ) != 0 )
    cerr << "The memory block has been allocated successfully for WRITE" << endl;
  else
    cerr << "Can't allocate the memory block for WRITE" << endl;

  testBlock ( anAddress, aSize );

  vm_deallocate ( anAddress, aSize );

  if ( ( anAddress = vm_allocate ( aSize, KL_VIRTMEM_EXECUTE ) ) != 0 )
    cerr << "The memory block has been allocated successfully for EXECUTE" << endl;
  else
    cerr << "Can't allocate the memory block for EXECUTE" << endl;

  testBlock ( anAddress, aSize );

  vm_deallocate ( anAddress, aSize );

  if ( ( anAddress = vm_allocate ( aSize, KL_VIRTMEM_READ|KL_VIRTMEM_WRITE ) ) != 0 )
    cerr << "The memory block has been allocated successfully for READ|WRITE" << endl;
  else
    cerr << "Can't allocate the memory block for READ|WRITE" << endl;

  testBlock ( anAddress, aSize );  

  vm_deallocate ( anAddress, aSize );

  if ( ( anAddress = vm_allocate ( aSize, KL_VIRTMEM_READ|KL_VIRTMEM_EXECUTE ) ) != 0 )
    cerr << "The memory block has been allocated successfully for READ|EXECUTE" << endl;
  else
    cerr << "Can't allocate the memory block for READ|EXECUTE" << endl;

  testBlock ( anAddress, aSize );  

  vm_deallocate ( anAddress, aSize );

  if ( ( anAddress = vm_allocate ( aSize, KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE ) ) != 0 )
    cerr << "The memory block has been allocated successfully for WRITE|EXECUTE" << endl;
  else
    cerr << "Can't allocate the memory block for WRITE|EXECUTE" << endl;

  testBlock ( anAddress, aSize ); 

  vm_deallocate ( anAddress, aSize );

  if ( ( anAddress = vm_allocate ( aSize,  KL_VIRTMEM_READ|KL_VIRTMEM_WRITE|KL_VIRTMEM_EXECUTE ) ) != 0 )
    cerr << "The memory block has been allocated successfully for READ|WRITE|EXECUTE" << endl;
  else
    cerr << "Can't allocate the memory block for READ|WRITE|EXECUTE" << endl;

  testBlock ( anAddress, aSize );

  vm_deallocate ( anAddress, aSize );
}



int main ()
{
#if defined (__unix__)
  signal ( SIGSEGV, SIGSEGVhandler );
#endif
  cerr << "Pagesize: " << vm_pagesize () << endl;
  testAllocation ();
  testProtection ();
  return 0;
}

