#if !defined (_C_MEMORYMANAGER_H_)
#define _C_MEMORYMANAGER_H_

#define MEMORY_PROTECTION_NONE    0
#define MEMORY_PROTECTION_READ    1<<0
#define MEMORY_PROTECTION_WRITE   1<<1
#define MEMORY_PROTECTION_EXECUTE 1<<2

#if defined (__cplusplus)
extern "C" {
#endif

  void* allocate ( unsigned int /* size */, unsigned int /* memory protection */ );

  void deallocate ( void* /* address */, unsigned int /* size */ );

  int protect ( void* /* address */,  unsigned int /*size*/, unsigned int /* memory protection */ );

#if defined (__cplusplus)
}
#endif

#endif // _C_MEMORYMANAGER_H_
