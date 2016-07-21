#if !defined (_REQUEST_H_)
#define _REQUEST_H_

#include <vector>

#include <nestednodelist.h>

#include "uuid.h"
#include "praguerpc.h"

namespace PragueRPC {

#define PA_IN       1<<0
#define PA_OUT      1<<1
/* #define PA_PTR      1<<2 */
/* #define PA_STRING   1<<3 */
/* #define PA_SIZE     1<<4 */

#define MAX_PARAMETERS_COUNT 30
  struct Method {
    unsigned int theMethodHash;
    void*        theMethod;
  };

  bool operator < ( const Interface& aLeft, const Interface& aRight );

  class Request;

  class RequestHeader {
    friend bool operator == ( const RequestHeader& aLeft, const RequestHeader& aRight ); 
  public:
    typedef int          Descriptor;
    enum Status { NoErrors, 
                  Processing, 
                  TerminateProcessing, 
                  InterfaceNotFound, 
                  MethodNotFound, 
                  NotEnoughMemory, 
                  ReceiveError, 
                  SendError,
                  ProcessingError };
    RequestHeader ();
    void setConnectionID ( ConnectionID aConnectionID ) { theRequestID.theConnectionID = aConnectionID; }
    ConnectionID getConnectionID () { return theRequestID.theConnectionID; }
    ConnectionID getClientID () { return theRequestID.theClientID; }
    void setUUID ( const UUID& aUUID ) { theUUID = aUUID; }
    UUID getUUID () const { return theUUID; }
    void setMethodHash ( unsigned int aMethodHash ) { theMethodHash = aMethodHash; }
    unsigned int getMethodHash () const { return theMethodHash; } 
    void setStatus ( Status aStatus ) { theStatus = aStatus; }
    Status getStatus () const { return theStatus; } 
    bool isLocal () const;
    bool read ( Descriptor aDescriptor );

  protected:
    static unsigned int theSID;
    struct RequestID {
      RequestID () : theClientID ( 0 ), theSessionID ( 0 ), theConnectionID ( 0 ) {}
      unsigned int theClientID;
      unsigned int theSessionID;
      ConnectionID theConnectionID;
    };

    RequestID    theRequestID;
    UUID         theUUID;
    unsigned int theMethodHash;
    Status       theStatus;
  };

  bool operator == ( const RequestHeader& aLeft, const RequestHeader& aRight );

  class Request : public RequestHeader {
  public:
    Request () : theParametersSize ( 0 ), theParametersPosition ( 0 ) {}
    Request ( const RequestHeader& aRequestHeader ) { RequestHeader& aHeader = *this; aHeader = aRequestHeader; }
    ~Request () {}

    template<typename T>
    bool addParameter ( T& aValue )
    {
      return insertParameter ( &aValue, sizeof ( T ) );
    }

/*     template<typename T> */
/*     bool addParameter ( T*& aValue ) */
/*     { */
/*       return insertParameter ( aValue, sizeof ( T ) ); */
/*     } */

    template<typename T>
    bool addParameter ( T* aValue, unsigned int aSize )
    {
      return insertParameter ( aValue, sizeof ( T ) * aSize );
    }

    template<typename T>
    bool getParameter ( T& aValue )
    {
      return fillParameter ( &aValue, sizeof ( T ) );
    }

/*     template<typename T> */
/*     bool getParameter ( T*& aValue ) */
/*     { */
/*       void** aPtr = reinterpret_cast <void**> ( &aValue ); */
/*       return fillParameter ( aPtr, sizeof ( T ) ); */
/*     } */

    template<typename T>
    bool getParameter ( T*& aValue, unsigned int aSize )
    {
      void** aPtr = reinterpret_cast <void**> ( &aValue );
      return fillParameter ( aPtr, sizeof ( T ) * aSize );
    }


    bool read ( Descriptor aDescriptor );
    bool write ( Descriptor aDescriptor );
    bool check () { return ( theParametersBuffer + theParametersSize ) == theParametersPosition ; }

  private:
    template<typename T>
    class ArrayAutoPointer {
    public:
      ArrayAutoPointer () : theValue ( 0 ) {}
      ArrayAutoPointer ( T* aValue ) : theValue ( aValue ) {}
      ArrayAutoPointer ( const ArrayAutoPointer& aValue ) 
      { theValue = aValue.theValue; const_cast<ArrayAutoPointer&> ( aValue ).theValue = 0; }
      ~ArrayAutoPointer () { reset (); }
      bool operator ! () { return !theValue; }
      T& operator [] ( int anIndex ) { return theValue [ anIndex ]; }
      operator T*() { return theValue; }
      T* operator = ( T* aValue ) { reset (); theValue = aValue; return aValue;}
      T* get () { return theValue; }
      void reset () { delete [] theValue; }
    private:
      T* theValue;
    };

    struct Parameter {
      Parameter () 
        : theValue ( 0 ), theSize ( 0 ) {} 
      Parameter ( const void* aValue, unsigned int aSize );

      ArrayAutoPointer<unsigned char> ( theValue );
      unsigned int     theSize;
    };

    bool insertParameter ( const void* aValue, unsigned int aSize );
    bool fillParameter ( void* aValue, unsigned int aSize );
    bool fillParameter ( void** aValue, unsigned int aSize );

    NESTED_NODE_LIST_ENTRY(Request);
    typedef std::vector<Parameter>          Parameters;
    Parameters                              theParameters;
    typedef ArrayAutoPointer<unsigned char> AutoPointer;
    typedef std::vector<AutoPointer>        AutoPointers;    
    AutoPointers                            theAutoPointers;
    unsigned int                            theParametersSize;
    AutoPointer                             theParametersBuffer;
    unsigned char*                          theParametersPosition;
  };

}


#endif //_REQUEST_H_
