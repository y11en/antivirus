// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  31 August 2004,  11:30 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- wheap.cpp
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end

#if defined (__unix__)

// AVP Prague stamp begin( Interface version,  )
#define Heap_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_win32loader.h>

#include <stdlib.h>
#include <string.h>
#include "heap.h"
// AVP Prague stamp end

#include <klsys/memorymanager/guardedmm.h>
#include <klsys/guards/posixguard.h>
#include <new>

typedef KLSysNS::PosixGuard Guard;

#define BLOCK_SIZE 0x100000 // 1Mb

// AVP Prague stamp begin( C++ class declaration,  )
struct HeapData : public cHeap /*cObjImpl*/ {
private:
  // Internal function declarations
  tERROR pr_call ObjectInitDone();
  tERROR pr_call ObjectClose();

  // Property function declarations
  tERROR pr_call heap_size( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
  tERROR check_heap ();
public:
  // External function declarations
  tERROR pr_call Alloc( tPTR* result, tDWORD p_size );
  tERROR pr_call Realloc( tPTR* result, tPTR p_mem, tDWORD p_size );
  tERROR pr_call Free( tPTR p_mem );
  tERROR pr_call Size( tDWORD* result, tPTR p_mem );
  tERROR pr_call Clear();

  // Data declaration
  typedef KLSysNS::GuardedMM<Guard> memorymanager_t;
  tCHAR  m_memorymanager_place [ sizeof ( memorymanager_t ) ];           // --
  memorymanager_t* m_memorymanager;
  tDWORD m_granularity;     // --
  tDWORD m_move_allowed;    // --
  tDWORD m_requested_size;  // --
  tINT   m_allocated_count; // --
  tBOOL  m_early_initialized; // //rus:инициализаци€ Windows heap во врем€ инициализации объекта
  Guard  m_lock;
  // AVP Prague stamp end



  // AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(HeapData)



    };
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Heap". Static(shared) property table variables
tDWORD g_alloc_all = 0; // must be READ_ONLY at runtime
// AVP Prague stamp end

//pthread_mutex_t theGlobalHeapProtector = PTHREAD_MUTEX_INITIALIZER;
Guard theGlobalHeapProtector;

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR HeapData::ObjectInitDone() 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter Heap::ObjectInitDone method" );

  // Place your code here
  if ( m_early_initialized ) {
    m_early_initialized = cFALSE;
    error = check_heap ();
  }

  PR_TRACE_A1( this, "Leave Heap::ObjectInitDone method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR HeapData::ObjectClose() 
{

  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter Heap::ObjectClose method" );

  // Place your code here
  if ( m_memorymanager ) {
    Guard::Lock aLock ( theGlobalHeapProtector );
    m_memorymanager->memorymanager_t::~memorymanager_t ();
    m_memorymanager = 0;
    g_alloc_all -= m_requested_size;
    m_requested_size = 0;
    m_allocated_count = 0;
  }
  PR_TRACE_A1( this, "Leave Heap::ObjectClose method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, heap_size )
// Interface "Heap". Method "Get" for property(s):
//  -- WIN32_HEAP_SIZE
tERROR HeapData::heap_size( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property plWIN32_HEAP_SIZE" );

  *out_size = sizeof(tDWORD);
  if ( size < *out_size )
    error = errBUFFER_TOO_SMALL;
  else
    if ( buffer )
      *(tDWORD*)buffer = m_requested_size;
    else
      error = errPARAMETER_INVALID;


  PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property plWIN32_HEAP_SIZE, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end


tERROR HeapData::check_heap ()
{
  if ( m_memorymanager )
    return errOK;

  if ( !m_early_initialized ) {  
    Guard::Lock aLock ( theGlobalHeapProtector );
    m_early_initialized = cTRUE;
    m_memorymanager = new ( m_memorymanager_place ) memorymanager_t ( BLOCK_SIZE );
  }
   
  if ( !m_memorymanager )
    return errOBJECT_NOT_INITIALIZED;
  return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, Alloc )
// Behaviour comment
//    //rus:возвращает указатель на распределенный регион пам€ти. ≈сли невозможно распределить запрошенное количество байт возвращает NULL в качестве указател€ и ошибку errNOT_ENOUGH_MEMORY.//eng:returns pointer to allocated memory. If this heap does not contain required amount of bytes returns NULL as pointer and errNOT_ENOUGH_MEMORY error code.
// Parameters are:
tERROR HeapData::Alloc( tPTR* result, tDWORD p_size ) 
{
  tERROR error = errOK;
  tPTR   ret_val = NULL;
  PR_TRACE_A0( this, "Enter Heap::Alloc method" );
  
  // Place your code here

  error = check_heap ();
  if ( PR_SUCC ( error ) ) {
#if defined (USE_STD_MANAGER)
    ret_val = calloc ( 1, p_size + sizeof ( tDWORD ) );
#else
    Guard::Lock aLock ( m_lock );
    ret_val = m_memorymanager->allocate ( p_size );
#endif
    if ( ret_val ) {
#if defined (USE_STD_MANAGER)
      *reinterpret_cast <tDWORD*> ( ret_val ) = p_size;
      ret_val = reinterpret_cast <char*> ( ret_val ) + sizeof( tDWORD );
#endif
      Guard::Lock aLock ( theGlobalHeapProtector );
      m_requested_size += p_size;
      g_alloc_all += p_size;
      ++m_allocated_count;
    }
    else
      error = errNOT_ENOUGH_MEMORY;
  }
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Heap::Alloc method, ret tPTR = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Realloc )
// Extended method comment
//    //rus:ѕри перераспределении пам€ти метод пытаетс€ сохранить указатель неизменным. ≈сли это не возможно, поведение зависит от значени€ свойства HEAP_MOVE_ALLOWED. ≈сли это cTRUE распредел€етс€ новый регион, и содержимое старого копируетс€. ≈сли cFALSE - метод завершаетс€ с ошибкой, возвращает NULL в качестве указател€ и errNOT_ENOUGH_MEMORY.
//
//    //eng:Method tries to preserve pointer to block. If it is not possible behaviour depends on HEAP_MOVE_ALLOWED property. If property is cTRUE method returns new pointer. All data in memory block will copied into new one, If move is disabled method fails and returns NULL as pointer and errNOT_ENOUGH_MEMORY as error code.
// Parameters are:
tERROR HeapData::Realloc( tPTR* result, tPTR p_mem, tDWORD p_size ) 
{
  tERROR error = errOK;
  tPTR   ret_val = NULL;
  PR_TRACE_A0( this, "Enter Heap::Realloc method" );

  // Place your code here
  error = check_heap ();

  if ( PR_SUCC ( error ) ) {
    tDWORD anOldSize = 0;
    if ( p_mem  ) {
#if defined (USE_STD_MANAGER)
      p_mem = reinterpret_cast<char*> ( p_mem ) - sizeof( tDWORD );  
      anOldSize = *reinterpret_cast<tDWORD*> ( p_mem );
#else
      anOldSize = m_memorymanager->size ( p_mem );
#endif
    }
#if defined (USE_STD_MANAGER)
    ret_val = realloc ( p_mem, p_size + sizeof( tDWORD ) );
#else
    Guard::Lock aLock ( m_lock );
    ret_val = m_memorymanager->reallocate ( p_mem, p_size );
#endif
    if ( ret_val ) {
#if defined (USE_STD_MANAGER)
      *reinterpret_cast <tDWORD*> ( ret_val ) = p_size;
      ret_val = reinterpret_cast <char*> ( ret_val ) + sizeof( tDWORD );
      if ( p_size > anOldSize ) 
        memset ( reinterpret_cast <char*> ( ret_val ) + anOldSize, 0, p_size - anOldSize ); 
#endif
      Guard::Lock aLock ( theGlobalHeapProtector );
      m_requested_size += p_size - anOldSize;
      g_alloc_all += p_size - anOldSize;
    }
    else
      error = errNOT_ENOUGH_MEMORY;
  }

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Heap::Realloc method, ret tPTR = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Free )
// Parameters are:
tERROR HeapData::Free( tPTR p_mem ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter Heap::Free method" );

  // Place your code here
  if ( !m_memorymanager )
    error = errOBJECT_NOT_INITIALIZED;

  tDWORD anOldSize = 0;
  if ( PR_SUCC ( error ) && p_mem  ) {
#if defined (USE_STD_MANAGER)
      p_mem = reinterpret_cast<char*> ( p_mem ) - sizeof( tDWORD );  
      anOldSize = *reinterpret_cast<tDWORD*> ( p_mem );
#else
      anOldSize = m_memorymanager->size ( p_mem );
#endif
  }

#if defined (USE_STD_MANAGER)
  if ( PR_SUCC ( error ) ) {
    free ( p_mem );
#else
  Guard::Lock aLock ( m_lock );
  if ( PR_SUCC ( error ) && m_memorymanager->deallocate ( p_mem ) ) {
#endif
    Guard::Lock aLock ( theGlobalHeapProtector );
    m_requested_size -=  anOldSize;
    g_alloc_all -= anOldSize;
    --m_allocated_count;
  }
    
  PR_TRACE_A1( this, "Leave Heap::Free method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Size )
// Result comment
//    size of allocated block. If pointer is not valid function will return 0 and error code will set.
// Parameters are:
tERROR HeapData::Size( tDWORD* result, tPTR p_mem ) 
{
  tERROR error = errOK;
  tDWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter Heap::Size method" );

  // Place your code here
  if ( !m_memorymanager )
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) &&  p_mem  ) {
#if defined (USE_STD_MANAGER)
    p_mem = reinterpret_cast<char*> ( p_mem ) - sizeof( tDWORD );  
    ret_val = *reinterpret_cast<tDWORD*> ( p_mem );
#else
    ret_val = m_memorymanager->size ( p_mem );
#endif
  }
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Heap::Size method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Clear )
// Parameters are:
tERROR HeapData::Clear() 
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter Heap::Clear method" );

  // Place your code here
  ObjectClose ();
  ObjectInitDone ();
  PR_TRACE_A1( this, "Leave Heap::Clear method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Heap". Register function
tERROR HeapData::Register( hROOT root ) 
{
  tERROR error;
  // AVP Prague stamp end



  // AVP Prague stamp begin( Property table,  )
  mpPROPERTY_TABLE(Heap_PropTable)
    mSHARED_PROPERTY( pgINTERFACE_VERSION, Heap_VERSION )
    mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Windows based Heap", 19 )
    mpLOCAL_PROPERTY_BUF( pgHEAP_GRANULARITY, HeapData, m_granularity, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
    mpLOCAL_PROPERTY_BUF( pgHEAP_MOVE_ALLOWED, HeapData, m_move_allowed, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
    mpLOCAL_PROPERTY_BUF( plREQUESTED_HEAP_SIZE, HeapData, m_requested_size, cPROP_BUFFER_READ )
    mpLOCAL_PROPERTY_FN( plWIN32_HEAP_SIZE, FN_CUST(heap_size), NULL )
    mpLOCAL_PROPERTY_BUF( plALLOCATED_POINTERS, HeapData, m_allocated_count, cPROP_BUFFER_READ )
    mpLOCAL_PROPERTY_BUF( plEARLY_INITIALIZED, HeapData, m_early_initialized, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
    mSHARED_PROPERTY_PTR( plGLOBAL_ALLOCATED, g_alloc_all, sizeof(g_alloc_all) )
    mpPROPERTY_TABLE_END(Heap_PropTable)
    // AVP Prague stamp end



    // AVP Prague stamp begin( Internal method table,  )
    mINTERNAL_TABLE_BEGIN(Heap)
    mINTERNAL_METHOD(ObjectInitDone)
    mINTERNAL_METHOD(ObjectClose)
    mINTERNAL_TABLE_END(Heap)
    // AVP Prague stamp end



    // AVP Prague stamp begin( External method table,  )
    mEXTERNAL_TABLE_BEGIN(Heap)
    mEXTERNAL_METHOD(Heap, Alloc)
    mEXTERNAL_METHOD(Heap, Realloc)
    mEXTERNAL_METHOD(Heap, Free)
    mEXTERNAL_METHOD(Heap, Size)
    mEXTERNAL_METHOD(Heap, Clear)
    mEXTERNAL_TABLE_END(Heap)
    // AVP Prague stamp end



    // AVP Prague stamp begin( Registration call,  )

    PR_TRACE_A0( root, "Enter Heap::Register method" );

  error = CALL_Root_RegisterIFace(
                                  root,                                   // root object
                                  IID_HEAP,                               // interface identifier
                                  PID_WIN32LOADER,                        // plugin identifier
                                  0x00000000,                             // subtype identifier
                                  Heap_VERSION,                           // interface version
                                  VID_PETROVITCH,                         // interface developer
                                  &i_Heap_vtbl,                           // internal(kernel called) function table
                                  (sizeof(i_Heap_vtbl)/sizeof(tPTR)),     // internal function table size
                                  &e_Heap_vtbl,                           // external function table
                                  (sizeof(e_Heap_vtbl)/sizeof(tPTR)),     // external function table size
                                  Heap_PropTable,                         // property table
                                  mPROPERTY_TABLE_SIZE(Heap_PropTable),   // property table size
                                  sizeof(HeapData)-sizeof(cObjImpl),      // memory size
                                  IFACE_SYSTEM                            // interface flags
                                  );

#ifdef _PRAGUE_TRACE_
  if ( PR_FAIL(error) )
    PR_TRACE( (root,prtDANGER,"Heap(IID_HEAP) registered [%terr]",error) );
#endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave Heap::Register method, ret %terr", error );
  // AVP Prague stamp end



  // AVP Prague stamp begin( C++ class regitration end,  )
  return error;
}

tERROR pr_call Heap_Register( hROOT root ) { return HeapData::Register(root); }
// AVP Prague stamp end

#endif //__unix__

