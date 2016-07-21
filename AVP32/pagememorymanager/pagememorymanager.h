#if !defined (_PAGEMEMORYMANAGER_H_)
#define _PAGEMEMORYMANAGER_H_


namespace PageMemoryManagerNS {
  
#define KL_MEMORY_PROTECTION_NONE    0
#define KL_MEMORY_PROTECTION_READ    1<<0
#define KL_MEMORY_PROTECTION_WRITE   1<<1
#define KL_MEMORY_PROTECTION_EXECUTE 1<<2

  class PageMemoryManager {
  public:
    PageMemoryManager ( void* aPlacement = 0, 
                        unsigned int aBlockSize = 0, 
                        unsigned int aMemoryProtection = KL_MEMORY_PROTECTION_READ|KL_MEMORY_PROTECTION_WRITE );

    ~PageMemoryManager ();

    void* allocate ( unsigned int, unsigned int anAlignment = 2 );

    void setMemoryProtection ( unsigned int );

    unsigned int getMemoryProtection ();

	bool isAllocatedAddress ( void* );

    void deallocate ();

  private:
    struct PageControlBlock;

    unsigned int theMemoryProtection;
    unsigned int theSystemMemoryProtection;
    PageControlBlock* thePCB;
    bool thePlacement;
    unsigned int theBlockSize;
  };

}  //PageMemoryManagerNS


#endif // _PAGEMEMORYMANAGER_H_
