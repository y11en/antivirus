#if !defined (_CPP_UTILS_H_)
#define _CPP_UTILS_H_

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_string.h>

#include <string.h>

template <typename T> 
tERROR getValue ( tPTR buffer, tDWORD size, tDWORD* out_size, T value )
{
  tDWORD aSize = sizeof ( T );  

  if ( out_size )
    *out_size = aSize;

  if ( !buffer )
    if ( !size ) 
      return errOK;
    else
      return errPARAMETER_INVALID;

  if ( size < aSize ) 
    return errBUFFER_TOO_SMALL;
  
  *(reinterpret_cast<T*> ( buffer ) ) = value;
  return errOK;
}

template <typename T> 
tERROR setValue ( tPTR buffer, tDWORD size, T& value, cObject* owner )
{
  if ( !buffer ) 
    return errPARAMETER_INVALID;

  tDWORD aSize = sizeof ( T );  

  if ( size < aSize ) 
    return errBUFFER_TOO_SMALL;

  value = *(reinterpret_cast<T*> ( buffer ) );
  return errOK;
}

template <> 
tERROR getValue <char*> ( tPTR buffer, tDWORD size, tDWORD* out_size, char* value )
{
  tDWORD aSize = (tDWORD)(( value ) ? strlen ( value ) + 1 : 0);

  if ( out_size )
    *out_size = aSize;
  
  if ( !buffer )
    if ( !size ) 
      return errOK;
    else
      return errPARAMETER_INVALID;

  if ( size < aSize ) 
    return errBUFFER_TOO_SMALL;

  memcpy ( buffer, value, aSize );
  return errOK;
}

template <> 
tERROR setValue <char*> ( tPTR buffer, tDWORD size, char*& value, cObject* owner )
{
  if ( !buffer || !owner ) 
    return errPARAMETER_INVALID;

  char* aTmp = value;

  tERROR anError = owner->heapAlloc ( reinterpret_cast<tPTR*> ( &value ), size + 1);

  if ( PR_FAIL ( anError ) ) 
    return anError;

  memcpy ( value, buffer, size );

  owner->heapFree ( aTmp );

  return errOK;
}

template <> 
tERROR getValue <const char*> ( tPTR buffer, tDWORD size, tDWORD* out_size, const char* value )
{
  tDWORD aSize = (tDWORD)(( value ) ? strlen ( value ) + 1 : 0);

  if ( out_size )
    *out_size = aSize;
  
  if ( !buffer )
    if ( !size ) 
      return errOK;
    else
      return errPARAMETER_INVALID;

  if ( size < aSize ) 
    return errBUFFER_TOO_SMALL;

  memcpy ( buffer, value, aSize );
  return errOK;
}

template <> 
tERROR getValue <cStringObj&> ( tPTR buffer, tDWORD size, tDWORD* out_size, cStringObj& value )
{
  tDWORD aSize = value.memsize ( cCP_UNICODE );

  if ( out_size )
    *out_size = aSize;
  
  if ( !buffer )
    if ( !size ) 
      return errOK;
    else
      return errPARAMETER_INVALID;

  if ( size < aSize ) 
    return errBUFFER_TOO_SMALL;

  return value.copy ( buffer, aSize, cCP_UNICODE );
}



#endif
