// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  14 April 2005,  18:08 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sychev
// File Name   -- simpleobject.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define SimpleObject_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_vmarea.h>

#include <klsys/virtualmemory/virtualmemory.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable VMArea : public cSimpleObject
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call setProtection( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations

// Data declaration
  tPTR   m_theAddress;  // --
  tDWORD m_theSize;     // --
  tDWORD m_theProtection; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(VMArea)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "SimpleObject". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR VMArea::ObjectInit()
{
  tERROR error = errOK;
  PR_TRACE_FUNC_FRAME( "SimpleObject::ObjectInit method" );

  // Place your code here
  m_theProtection = PROTECTION_READ | PROTECTION_WRITE;
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR VMArea::ObjectInitDone()
{
  tERROR error = errOK;
  PR_TRACE_FUNC_FRAME( "SimpleObject::ObjectInitDone method" );

  // Place your code here
  KLSysNS::Protection aProtection = KL_VIRTMEM_NONE;
  if ( m_theProtection & PROTECTION_READ )
    aProtection |= KL_VIRTMEM_READ;
  if ( m_theProtection & PROTECTION_WRITE )
    aProtection |= KL_VIRTMEM_WRITE;
  if ( m_theProtection & PROTECTION_EXECUTE )
    aProtection |= KL_VIRTMEM_EXECUTE;
  m_theAddress = KLSysNS::vm_allocate ( m_theSize, aProtection, m_theAddress );
  if ( !m_theAddress )
    error = errNOT_ENOUGH_MEMORY;
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR VMArea::ObjectClose()
{
  tERROR error = errOK;
  PR_TRACE_FUNC_FRAME( "SimpleObject::ObjectClose method" );

  // Place your code here
  if ( m_theAddress )
    KLSysNS::vm_deallocate ( m_theAddress, m_theSize );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setProtection )
// Interface "SimpleObject". Method "Set" for property(s):
//  -- PROTECTION
tERROR VMArea::setProtection( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method setProtection for property plPROTECTION" );

  if ( !buffer || size != sizeof ( tDWORD ) )
    error = errPARAMETER_INVALID;

  tDWORD aNewProtection = *( reinterpret_cast <tDWORD*> ( buffer ) );
  *out_size = 0;
  if ( PR_SUCC ( error ) ) {
    KLSysNS::Protection aProtection = KL_VIRTMEM_NONE;
    if ( aNewProtection & PROTECTION_READ )
      aProtection |= KL_VIRTMEM_READ;
    if ( aNewProtection & PROTECTION_WRITE )
      aProtection |= KL_VIRTMEM_WRITE;
    if ( aNewProtection & PROTECTION_EXECUTE )
      aProtection |= KL_VIRTMEM_EXECUTE;
    if ( !KLSysNS::vm_protect ( m_theAddress, m_theSize, aProtection ) )
      error = errUNEXPECTED;
    else
      m_theProtection = aNewProtection;
  }
  PR_TRACE_A2( this, "Leave *SET* method setProtection for property plPROTECTION, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "SimpleObject". Register function
tERROR VMArea::Register( hROOT root ) 
{
  tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(SimpleObject_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, SimpleObject_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "managing of an area of virtual memory", 38 )
  mpLOCAL_PROPERTY_BUF( plSIZE, VMArea, m_theSize, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
  mpLOCAL_PROPERTY( plPROTECTION, VMArea, m_theProtection, cPROP_BUFFER_READ, NULL, FN_CUST(setProtection) )
  mpLOCAL_PROPERTY_BUF( plADDRESS, VMArea, m_theAddress, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(SimpleObject_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(SimpleObject)
  mINTERNAL_METHOD(ObjectInit)
  mINTERNAL_METHOD(ObjectInitDone)
  mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(SimpleObject)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

  PR_TRACE_FUNC_FRAME_( *root, "SimpleObject::Register method", &error );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_SIMPLEOBJECT,                       // interface identifier
    PID_VMAREA,                             // plugin identifier
    0x00000000,                             // subtype identifier
    SimpleObject_VERSION,                   // interface version
    VID_SYCHEV,                             // interface developer
    &i_SimpleObject_vtbl,                   // internal(kernel called) function table
    (sizeof(i_SimpleObject_vtbl)/sizeof(tPTR)),// internal function table size
    NULL,                                   // external function table
    0,                                      // external function table size
    SimpleObject_PropTable,                 // property table
    mPROPERTY_TABLE_SIZE(SimpleObject_PropTable),// property table size
    sizeof(VMArea)-sizeof(cObjImpl),        // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"SimpleObject(IID_SIMPLEOBJECT) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
  return error;
}

tERROR pr_call SimpleObject_Register( hROOT root ) { return VMArea::Register(root); }
// AVP Prague stamp end



