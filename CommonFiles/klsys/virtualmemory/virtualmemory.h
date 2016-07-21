#if !defined (_VIRTUALMEMORY_H_)
#define _VIRTUALMEMORY_H_

#if defined (__unix__)
#include <sys/types.h>
#elif defined (_WIN32)
#include <windows.h>
#elif
#endif

namespace KLSysNS {

#define KL_VIRTMEM_NONE             0
#define KL_VIRTMEM_READ             1<<0
#define KL_VIRTMEM_WRITE            1<<1
#define KL_VIRTMEM_EXECUTE          1<<2

#if defined (__unix__)
    typedef size_t kl_size_t;
#elif defined (_WIN32)
    typedef SIZE_T kl_size_t;
#endif
  typedef unsigned int Protection;
    
  void* vm_allocate ( kl_size_t aSize, Protection aProtection = KL_VIRTMEM_READ|KL_VIRTMEM_WRITE, void* aStartAddress = 0 );
  void vm_deallocate ( void* anAddress, kl_size_t aSize );
  bool vm_protect ( void* anAddress, kl_size_t aSize, Protection aProtection );
  bool vm_lock ( void* anAddress, kl_size_t aSize );
  void vm_unlock ( void* anAddress, kl_size_t aSize );
  unsigned int vm_pagesize ();

  class VMArea {
  public:
    VMArea ( kl_size_t aSize, Protection aProtection, void* aStartAddress = 0 )
      : theAddress ( vm_allocate ( aSize, aProtection, aStartAddress ) ),
        theSize ( aSize ),
        theProtection ( aProtection )
    {}

    ~VMArea ()
    {
      vm_deallocate ( theAddress, theSize );
    }

    bool setProtection ( Protection aProtection )
    {
      bool aResult = vm_protect ( theAddress, theSize, aProtection );
      if ( aResult )
        theProtection = aProtection;
      return aResult;
    }

    Protection getProtection () { return theProtection; }
    kl_size_t  getSize () { return theSize; }
    void*      getAddress () { return theAddress; }

  private:
    void*      theAddress;
    kl_size_t  theSize;
    Protection theProtection;
  };

}


#endif // _VIRTUALMEMORY_H_
