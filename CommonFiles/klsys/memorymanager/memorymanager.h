#if !defined (_MEMORYMANAGER_H_)
#define _MEMORYMANAGER_H_


namespace KLSysNS {
  class MemoryManager {
  public:
    MemoryManager ( unsigned int aBlockSize = 0 );

    MemoryManager ( void*, unsigned int );

    ~MemoryManager ();

    void* allocate ( unsigned int );

    bool deallocate ( void* );

    void* reallocate ( void*, unsigned int );

    unsigned int size ( void* );

  private:
    struct PageControlBlock;

    PageControlBlock* thePCB;
    bool thePlacement;
    unsigned int theBlockSize;
  };

}  //KLSysNS


#endif // _MEMORYMANAGER_H_
