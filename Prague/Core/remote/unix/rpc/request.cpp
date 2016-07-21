#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

//#define DEBUG_TRACE
#include "debugtrace.h"
#include "request.h"
#include "utils.h"

namespace PragueRPC {
  
  extern unsigned int thePID;

  bool operator < ( const Interface& aLeft, const Interface& aRight ) 
  { 
    ENTER;
    return aLeft.theUUID < aRight.theUUID; 
  }

  bool recvAll ( Request::Descriptor aDescriptor, void* aBuffer, unsigned int aSize )
  {
    ENTER;
    unsigned char* aCharBuffer = reinterpret_cast <unsigned char*> ( aBuffer );
    TRACE ( "Notice: try to receive %d bytes of 0x%08x\n", aSize, aCharBuffer );
    TRACE_HEX ( aCharBuffer, aSize );
    do {
      int aResult = recv ( aDescriptor, aCharBuffer, aSize, 0 /*MSG_WAITALL*/ );
      if ( aResult <= 0 ) {
        TRACE ( "Error: can't receive a data '%s'\n", strerror ( errno ) );
        return false;
      }
      aCharBuffer += aResult;
      aSize -= aResult;
    }
    while ( aSize );
    return true;
  }
    
  bool sendAll ( Request::Descriptor aDescriptor, const void* aBuffer, unsigned int aSize )
  {
    ENTER;
    const unsigned char* aCharBuffer = reinterpret_cast <const unsigned char*> ( aBuffer );
    TRACE ( "Notice: try to send %d bytes of 0x%08x\n", aSize, aCharBuffer );
    TRACE_HEX ( aCharBuffer, aSize );
    do {
      int aResult  = send ( aDescriptor, aCharBuffer, aSize, MSG_NOSIGNAL );
      if ( aResult <= 0 ) {
        TRACE ( "Error: can't send a data '%s'\n", strerror ( errno ) );
        return false;
      }
      aCharBuffer += aResult;
      aSize -= aResult;
    }
    while ( aSize );
    return true;
  }


  //
  // RequestHeader
  //

  pthread_mutex_t theSIDMutex = PTHREAD_MUTEX_INITIALIZER;
  unsigned int RequestHeader::theSID = 0;


  RequestHeader::RequestHeader ()
    : theMethodHash ( 0 ),
      theStatus ( NoErrors )
  {
    ENTER;
    theRequestID.theClientID = thePID;
    Lock aLock ( &theSIDMutex );
    theRequestID.theSessionID = theSID++;
    memset ( &theUUID, 0, sizeof ( theUUID ) );
  }

  bool RequestHeader::isLocal () const 
  { 
    ENTER; 
    return theRequestID.theClientID == thePID; 
  }

  bool operator == ( const RequestHeader& aLeft, const RequestHeader& aRight )
  {
    ENTER;
    return ( aLeft.theRequestID.theClientID == aRight.theRequestID.theClientID ) &&
      ( aLeft.theRequestID.theSessionID == aRight.theRequestID.theSessionID ) &&
      ( aLeft.theRequestID.theConnectionID == aRight.theRequestID.theConnectionID ) &&
      ( aLeft.theUUID == aRight.theUUID ) &&
      ( aLeft.theMethodHash == aRight.theMethodHash );
  }

  bool RequestHeader::read ( Descriptor aDescriptor )
  {
    ENTER;
    if ( !recvAll ( aDescriptor, this, sizeof ( RequestHeader ) ) ) {
      theStatus = ReceiveError;
      TRACE ( "Error: can't receive a message '%s'\n", strerror ( errno ) );
      return false;
    }
    TRACE ( "Notice: a request entry has been received ID=%d-%d-%d Status=%d\n", theRequestID.theClientID, theRequestID.theSessionID, theRequestID.theConnectionID, theStatus );
    return true;
  }

  //
  //  Request
  //

  Request::Parameter::Parameter ( const void* aValue, unsigned int aSize ) 
    : theValue ( 0 ), theSize ( aSize ) 
  {
    ENTER;
    if ( aSize )
      theValue = new unsigned char [ aSize ];
    if ( aValue && theValue ) 
      memcpy ( theValue, aValue, aSize );
  } 

  bool Request::read ( Descriptor aDescriptor )
  {
    ENTER;
    if ( theStatus != NoErrors )
      return true;

    if ( !recvAll ( aDescriptor, &theParametersSize, sizeof ( theParametersSize ) ) ) {
      theStatus = ReceiveError;
      TRACE ( "Error: can't receive a message '%s'\n", strerror ( errno ) );
      return false;
    }
    
    theParametersBuffer = new ( std::nothrow ) unsigned char [ theParametersSize ];
    theParametersPosition = theParametersBuffer;
    if ( !theParametersBuffer ) {
      theStatus = NotEnoughMemory;
      return false;
    }
    memset ( theParametersBuffer.get (), 0, theParametersSize );
    if ( !recvAll ( aDescriptor, theParametersBuffer.get (), theParametersSize ) ) {
      theStatus = ReceiveError;
      TRACE ( "Error: can't receive a message '%s'\n", strerror ( errno ) );
      return false;
    } 

    TRACE ( "Notice: a request length %d bytes has been received ID=%d-%d-%d Status=%d\n", theParametersSize, theRequestID.theClientID, theRequestID.theSessionID, theRequestID.theConnectionID, theStatus );
    TRACE_HEX ( theParametersBuffer.get (), theParametersSize );
    return true;
  }

  bool Request::write ( Descriptor aDescriptor )
  {
    ENTER;
    if ( theStatus != NoErrors ) {
      if ( !sendAll ( aDescriptor, this, sizeof ( RequestHeader ) ) ) {
        theStatus = SendError;
        TRACE ( "Error: can't send a message '%s'\n", strerror ( errno ) );
        return false;
      }
      TRACE ( "Notice: a request has been sent ID=%d-%d-%d Status=%d\n", theRequestID.theClientID, theRequestID.theSessionID, theRequestID.theConnectionID, theStatus );
      return true;
    }
    unsigned int aParametersCount = theParameters.size ();
    theParametersSize = 0;
    for ( unsigned int i = 0; i < aParametersCount; i++ )
      theParametersSize += theParameters [ i ].theSize + sizeof ( theParameters [ i ].theSize );
    unsigned int aCommonSize = sizeof ( theParametersSize ) + theParametersSize + sizeof ( RequestHeader );
    ArrayAutoPointer<unsigned char> aBuffer = new ( std::nothrow ) unsigned char [ aCommonSize ];
    if ( !aBuffer ) {
      theStatus = NotEnoughMemory;
      return false;
    }
    memset ( aBuffer.get (), 0,  aCommonSize );
    unsigned char* aPosition = aBuffer.get ();
    memcpy ( aPosition, this, sizeof ( RequestHeader ) );
    aPosition += sizeof ( RequestHeader );
    memcpy ( aPosition, &theParametersSize, sizeof ( theParametersSize ) );
    aPosition += sizeof ( theParametersSize );
    for ( unsigned int i = 0; i < aParametersCount; i++ ) {
      memcpy ( aPosition, &theParameters [ i ].theSize,  sizeof ( theParameters [ i ].theSize ) );
      aPosition += sizeof ( theParameters [ i ].theSize );
      if ( theParameters [ i ].theSize ) 
        memcpy ( aPosition, theParameters [ i ].theValue, theParameters [ i ].theSize );
      aPosition += theParameters [ i ].theSize;
    }
    if ( !sendAll ( aDescriptor, aBuffer.get (), aCommonSize ) ) {
      theStatus = SendError;
      TRACE ( "Error: can't send a message '%s'\n", strerror ( errno ) );
      return false;
    }
    
    TRACE ( "Notice: a request %d bytes has been sent ID=%d-%d-%d Status=%d\n", 
            aCommonSize , //sizeof ( theParametersSize ) + sizeof ( RequestHeader ) + theParametersSize, 
            theRequestID.theClientID, theRequestID.theSessionID, theRequestID.theConnectionID, theStatus );
    TRACE_HEX ( aBuffer.get () + sizeof ( RequestHeader ) + sizeof ( theParametersSize ), theParametersSize );
    return true;
  }

  bool Request::insertParameter ( const void* aValue, unsigned int aSize )
  {
    ENTER;
    TRACE ( "inserted %d bytes\n", aSize );
    Parameter aParameter ( aValue, aSize );
    theParameters.push_back ( aParameter );    
    return true;
  }
  
  bool Request::fillParameter ( void* aValue, unsigned int aSize )
  {
    ENTER;
    if ( !theParametersBuffer )
      return false;
    unsigned int aParameterSize = 0;
    if ( ( theParametersPosition + sizeof ( aParameterSize ) ) > ( theParametersBuffer + theParametersSize ) ) {
      TRACE ( "Error: position is out of bounds\n" );
      return false;
    }
    memcpy ( &aParameterSize, theParametersPosition, sizeof ( aParameterSize ) );
    TRACE ( "parameter size: %d\n", aParameterSize );
    theParametersPosition += sizeof ( aParameterSize );
    if ( ( theParametersPosition + aParameterSize ) > ( theParametersBuffer + theParametersSize ) ) {
      TRACE ( "Error: parameter size is out of bounds\n" );
      return false;
    }
    if ( aParameterSize > aSize ) {
      TRACE ( "Error: parameter size (%d) is more that input size (%d)\n", aParameterSize, aSize );
      return false;
    }
    TRACE ( "get %d bytes\n", aParameterSize );
    memcpy ( aValue, theParametersPosition, aParameterSize );
    theParametersPosition += aParameterSize;
    return true;
  }

  bool Request::fillParameter ( void** aValue, unsigned int aSize )
  {
    ENTER;
    if ( !theParametersBuffer )
      return false;
    unsigned int aParameterSize = 0;
    if ( ( theParametersPosition + sizeof ( aParameterSize ) ) > ( theParametersBuffer + theParametersSize ) ) {
      TRACE ( "Error: position is out of bounds\n" );
      return false;
    }
    memcpy ( &aParameterSize, theParametersPosition, sizeof ( aParameterSize ) );
    TRACE ( "parameter size: %d\n", aParameterSize );
    theParametersPosition += sizeof ( aParameterSize );
    if ( ( theParametersPosition + aParameterSize ) > ( theParametersBuffer + theParametersSize ) ) {
      TRACE ( "Error: parameter size is out of bounds\n" );
      return false;
    }
    if ( !aSize && aParameterSize )
      aSize = aParameterSize;
    if ( aParameterSize > aSize ) {
      TRACE ( "Error: parameter size (%d) is more that input size (%d)\n", aParameterSize, aSize );
      return false;
    }
    if ( aParameterSize && !*aValue ) {
      unsigned char* aPointer = new ( std::nothrow ) unsigned char [ aSize ];
      if ( !aPointer ){
        theStatus = NotEnoughMemory;
        return false;
      }
      memset ( aPointer, 0, aSize );
      theAutoPointers.push_back ( aPointer );    
      *aValue = aPointer;
    }
    TRACE ( "get %d bytes\n", aParameterSize );
    memcpy ( *aValue, theParametersPosition, aParameterSize );
    theParametersPosition += aParameterSize;
    return true;
  }

} // PragueRPC
 
