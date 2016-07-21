// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  10 August 2004,  11:37 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sychev
// File Name   -- filemap.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Buffer_VERSION ((tVERSION)0)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_tracer.h>
#include <Prague/plugin/p_filemapping.h>

#include <string.h>
#include <klsys/errors/errors.h>
// AVP Prague stamp end

#include <klsys/filemapping/filemapping.h>

using namespace KLSysNS;

// AVP Prague stamp begin( C++ class declaration,  )
struct FileMap : public cBuffer /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call ReadProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations

//  IO declared methods
// //rus:методы декларированные в интерфейсе "IO"
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

//  buffer declared methods
// //rus:методы декларированные в интерфейсе "buffer"
	tERROR pr_call Lock( tPTR* result );
	tERROR pr_call AddLock( tPTR* result, tDWORD p_size_to_add );
	tERROR pr_call SizeLock( tPTR* result, tDWORD p_size );
	tERROR pr_call Unlock();

// Data declaration
  hOBJECT     m_BasedOn;             // --
  tDWORD      m_AccessMode;          // --
  tDWORD      m_Availability;        // --
  tCHAR       m_FullName[MAX_PATH];  // --
  tORIG_ID    m_Origin;              // --
  FileMapping m_FileMap;             // --
  tPTR        m_Address;             // --
  FileMapping::kl_size_t   m_NumberOfMappedBytes; // --
  tDWORD      m_AccessHint;          // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(FileMap)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Buffer". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR FileMap::ObjectInit()
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter Buffer::ObjectInit method" );

  // Place your code here
  m_AccessHint = fHINT_NORMAL_ACCESS;
  error = errOK;	
  PR_TRACE_A1( this, "Leave Buffer::ObjectInit method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
//! tERROR FileMap::ObjectInitDone()
tERROR FileMap::ObjectInitDone()
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter Buffer::ObjectInitDone method" );

  // Place your code here
  error = errOK;
  int anAccessRights = KL_FILEMAP_NONE;
  if ( m_AccessMode & fACCESS_READ )
    anAccessRights |= KL_FILEMAP_READ;
  if ( m_AccessMode & fACCESS_WRITE )
    anAccessRights |= KL_FILEMAP_WRITE;
  if ( m_AccessMode & fACCESS_EXECUTE )
    anAccessRights |= KL_FILEMAP_EXECUTE;
  if ( m_AccessMode & fACCESS_COPY_ON_WRITE )
    anAccessRights |= KL_FILEMAP_COPY_ON_WRITE;
  
  FileMapping::AccessHints anAccessHints;

  switch ( m_AccessHint )
  {
  case fHINT_NORMAL_ACCESS:
	anAccessHints = FileMapping::ahNormalAccess;
	break;
  case fHINT_SEQUENTIAL_ACCESS:
	anAccessHints = FileMapping::ahSequentialAccess;
	break;
  case fHINT_RANDOM_ACCESS:
	anAccessHints = FileMapping::ahRandomAccess;
	break;
  default:
	error = errOBJECT_CANNOT_BE_INITIALIZED;
  }	
  if ( PR_SUCC (error) )
    if ( m_FileMap.create ( m_FullName, anAccessRights, anAccessHints ) ) {
      int anError = getError ();
      if ( isInvalidArgument ( anError ) )
        error = errPARAMETER_INVALID ;
      else
        if ( isFileNotFound ( anError ) )
          error = errOBJECT_NOT_FOUND;
        else
          if ( isAccessDenied ( anError ) )
            error = errACCESS_DENIED;
          else
            error = errOBJECT_CANNOT_BE_INITIALIZED;
    }
  if ( PR_SUCC (error) ) {
    m_Address =  m_FileMap.map ( 0, 0, &m_NumberOfMappedBytes );
    if ( !m_Address ) {
      int anError = getError ();
      if ( isNotEnoughMemory ( anError ) )
        error = errNOT_ENOUGH_MEMORY;
      else
        error = errOBJECT_CANNOT_BE_INITIALIZED;
      m_FileMap.destroy ();
    }
  }
  PR_TRACE_A1( this, "Leave Buffer::ObjectInitDone method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
//! tERROR FileMap::ObjectClose()
tERROR FileMap::ObjectClose()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter Buffer::ObjectClose method" );

  // Place your code here
  if ( m_Address )
     m_FileMap.unmap ( m_Address, m_NumberOfMappedBytes );
  m_FileMap.destroy ();

  PR_TRACE_A1( this, "Leave Buffer::ObjectClose method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReadProperties )
// Interface "Buffer". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//! tERROR FileMap::ReadProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
tERROR FileMap::ReadProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* multyproperty method ObjectClose" );
  char* aPtr = strrchr ( m_FullName, '/' );
#if defined (_WIN32)
  char* aPtr1 = strrchr ( m_FullName, '\\' );
  aPtr = ( aPtr1 > aPtr ) ? aPtr1 : aPtr;
#endif

  switch ( prop ) {
  case pgOBJECT_NAME:
    ++aPtr;
    *out_size = (tDWORD)strlen ( aPtr );
    if ( buffer != NULL ) {
      if ( *out_size > size )
        error = errBUFFER_TOO_SMALL;
      else
        strncpy ( buffer, aPtr, *out_size );
    }
    break;

  case pgOBJECT_PATH:
    ++aPtr;
    *out_size = (tDWORD)strlen(m_FullName) - (tDWORD)strlen(aPtr);
    if ( buffer != NULL ) {
      if ( *out_size > size )
        error = errBUFFER_TOO_SMALL;
      else
        strncpy ( buffer, m_FullName, *out_size );
    }
    break;

    default:
      *out_size = 0;
      break;
  }

  PR_TRACE_A2( this, "Leave *GET* multyproperty method ObjectClose, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //rus:В буфер вычитывается содержимое объекта начиная со смещения заданного параметром "offset". Возвращает количество байт реально помещенное в буфер.
//    //eng:Fills buffer with the object contents begins with the offset indicated by "offset" parameter. Returns real count of bytes read
// Behaviour comment
//    //rus:Метод должен вернуть errOUT_OF_OBJECT, если параметр "offset" находится за последним байтом объекта. При этом возвращаемое количество байт должно быть 0. В противном случае метод возвращает errOK и реальное количество байт считанное в буфер, пусть даже 0 байт.
// Parameters are:
//! tERROR FileMap::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
tERROR FileMap::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
  tERROR error = errNOT_IMPLEMENTED;
  tDWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter Buffer::SeekRead method" );

  // Place your code here
  error = errOK;
  tDWORD anOffset = static_cast <tDWORD>(p_offset);
  if ( m_NumberOfMappedBytes < p_offset )
    error =  errOBJECT_SEEK;
  ret_val = ( p_size > ( m_NumberOfMappedBytes - anOffset ) ) ? (tDWORD)m_NumberOfMappedBytes - anOffset : p_size;
  memmove ( p_buffer, reinterpret_cast <void*> ( reinterpret_cast <tDWORD> ( m_Address ) + anOffset ), ret_val ) ;

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Buffer::SeekRead method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    //rus:зкопирует буфер в регион, начиная, с указанного с в праметре "offset" смещения.
//    //eng:writes buffer to the object begins with "offset". Returns real count of bytes written
// Behaviour comment
//    //rus:возвращаемые ошибки:
//       errOBJECT_
// Parameters are:
//! tERROR FileMap::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
tERROR FileMap::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
  tERROR error = errNOT_IMPLEMENTED;
  tDWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter Buffer::SeekWrite method" );

  // Place your code here
  error = errOK;
  if ( !( m_AccessMode & fACCESS_WRITE ) && !( m_AccessMode & fACCESS_COPY_ON_WRITE ) )
    error = errACCESS_DENIED;
  else {
    tDWORD anOffset = static_cast <tDWORD>(p_offset);
    if ( m_NumberOfMappedBytes < p_offset )
      error = errOBJECT_SEEK;
    ret_val = ( p_size > ( m_NumberOfMappedBytes - anOffset ) ) ? (tDWORD)m_NumberOfMappedBytes - anOffset : p_size;
    memmove ( reinterpret_cast <void*> ( reinterpret_cast <tDWORD> ( m_Address ) + anOffset ), p_buffer, ret_val ) ;
  }
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Buffer::SeekWrite method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    //rus:возвращает одинаковое значение для любого типа размера (см.описание параметра "type")
//    //eng:returns the same size for any size type (see description of the "type" parameter).
// Parameters are:
//   "type" : //rus:Тип размера - одна из констант:
//              IO_SIZE_TYPE_EXPLICIT
//              IO_SIZE_TYPE_APPROXIMATE
//              IO_SIZE_TYPE_ESTIMATE
//            //eng:Size type is one of the following:
//              IO_SIZE_TYPE_EXPLICIT
//              IO_SIZE_TYPE_APPROXIMATE
//              IO_SIZE_TYPE_ESTIMATE
//
//! tERROR FileMap::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
tERROR FileMap::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
  tERROR error = errNOT_IMPLEMENTED;
  tQWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter Buffer::GetSize method" );

  // Place your code here
  ret_val = m_NumberOfMappedBytes;
  error = errOK;
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Buffer::GetSize method, ret tQWORD = %I64u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
//! tERROR FileMap::SetSize( tQWORD p_new_size )
tERROR FileMap::SetSize( tQWORD p_new_size )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter Buffer::SetSize method" );

  // Place your code here

  PR_TRACE_A1( this, "Leave Buffer::SetSize method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Parameters are:
//! tERROR FileMap::Flush()
tERROR FileMap::Flush()
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter Buffer::Flush method" );

  // Place your code here
  if ( m_FileMap.flush ( m_Address, m_NumberOfMappedBytes ) )
    error = errUNEXPECTED;
  else
    error = errOK;
  PR_TRACE_A1( this, "Leave Buffer::Flush method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Lock )
// Extended method comment
//    //rus:регион памяти не может быть переписан методом "SeekWrite" и размер его не может быть изменен пока объект заблокирован
//    //eng:Memory cannot be overwritten while object is locked
// Parameters are:
//! tERROR FileMap::Lock( tPTR* result )
tERROR FileMap::Lock( tPTR* result )
{
  tERROR error = errNOT_IMPLEMENTED;
  tPTR   ret_val = NULL;
  PR_TRACE_A0( this, "Enter Buffer::Lock method" );

  // Place your code here
  ret_val = m_Address;
  error = errOK;
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Buffer::Lock method, ret tPTR = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddLock )
// Parameters are:
//! tERROR FileMap::AddLock( tPTR* result, tDWORD p_size_to_add )
tERROR FileMap::AddLock( tPTR* result, tDWORD p_size_to_add )
{
  tERROR error = errNOT_IMPLEMENTED;
  tPTR   ret_val = NULL;
  PR_TRACE_A0( this, "Enter Buffer::AddLock method" );

  // Place your code here

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Buffer::AddLock method, ret tPTR = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SizeLock )
// Parameters are:
//! tERROR FileMap::SizeLock( tPTR* result, tDWORD p_size )
tERROR FileMap::SizeLock( tPTR* result, tDWORD p_size )
{
  tERROR error = errNOT_IMPLEMENTED;
  tPTR   ret_val = 0;
  PR_TRACE_A0( this, "Enter Buffer::SizeLock method" );

  // Place your code here

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave Buffer::SizeLock method, ret tPTR = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Unlock )
// Extended method comment
//    //rus:Разблокирует регион и возвращает счетчик блокировок. Если сетчик равен 0, то возможно писать в регион методом "SeekWrite" и перераспределять регион
//    //eng:Unlocks memory and returns lock count. If lock count is zero client can reallock attached memory and use SeekWrite method to write to memory region.
// Parameters are:
//! tERROR FileMap::Unlock()
tERROR FileMap::Unlock()
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter Buffer::Unlock method" );

  // Place your code here
  error = errOK;
  PR_TRACE_A1( this, "Leave Buffer::Unlock method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Buffer". Register function
tERROR FileMap::Register( hROOT root ) 
{
  tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Buffer_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, Buffer_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "file mapping", 13 )

// IO inherited properties
// Предполагается, что ВСЕ свойства этого интерфейса будут реализованы статическими, за исключением, возможно, pgOBJECT_OPEN_MODE, pgOBJECT_ACCESS_MODE и pgOBJECT_BASED_ON.
  mpLOCAL_PROPERTY_FN( pgOBJECT_NAME, FN_CUST(ReadProperties), NULL )
  mpLOCAL_PROPERTY_FN( pgOBJECT_PATH, FN_CUST(ReadProperties), NULL )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, FileMap, m_FullName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
  mSHARED_PROPERTY( pgOBJECT_OPEN_MODE, ((tDWORD)(fOMODE_OPEN_IF_EXIST)) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, FileMap, m_AccessMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
  mSHARED_PROPERTY( pgOBJECT_DELETE_ON_CLOSE, ((tBOOL)(cFALSE)) )
  mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_SHARE)) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, FileMap, m_BasedOn, cPROP_BUFFER_READ )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, FileMap, m_Origin, cPROP_BUFFER_READ )
  mSHARED_PROPERTY( pgLOCK_COUNT, ((tDWORD)(0)) )
  mpLOCAL_PROPERTY_BUF( plACCESS_HINT, FileMap, m_AccessHint, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Buffer_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Buffer)
  mINTERNAL_METHOD(ObjectInit)
  mINTERNAL_METHOD(ObjectInitDone)
  mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(Buffer)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Buffer)
  mEXTERNAL_METHOD(Buffer, SeekRead)
  mEXTERNAL_METHOD(Buffer, SeekWrite)
  mEXTERNAL_METHOD(Buffer, GetSize)
  mEXTERNAL_METHOD(Buffer, SetSize)
  mEXTERNAL_METHOD(Buffer, Flush)
  mEXTERNAL_METHOD(Buffer, Lock)
  mEXTERNAL_METHOD(Buffer, AddLock)
  mEXTERNAL_METHOD(Buffer, SizeLock)
  mEXTERNAL_METHOD(Buffer, Unlock)
mEXTERNAL_TABLE_END(Buffer)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

  PR_TRACE_A0( root, "Enter Buffer::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_BUFFER,                             // interface identifier
    PID_FILEMAPPING,                        // plugin identifier
    0x00000000,                             // subtype identifier
    Buffer_VERSION,                         // interface version
    VID_SYCHEV,                             // interface developer
    &i_Buffer_vtbl,                         // internal(kernel called) function table
    (sizeof(i_Buffer_vtbl)/sizeof(tPTR)),   // internal function table size
    &e_Buffer_vtbl,                         // external function table
    (sizeof(e_Buffer_vtbl)/sizeof(tPTR)),   // external function table size
    Buffer_PropTable,                       // property table
    mPROPERTY_TABLE_SIZE(Buffer_PropTable), // property table size
    sizeof(FileMap)-sizeof(cObjImpl),       // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"Buffer(IID_BUFFER) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave Buffer::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
  return error;
}

tERROR pr_call Buffer_Register( hROOT root ) { return FileMap::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_FILL_CHAR
// You have to implement propetry: pgINTERFACE_COMPATIBILITY
// AVP Prague stamp end



