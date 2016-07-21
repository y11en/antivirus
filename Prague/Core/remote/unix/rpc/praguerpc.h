#if !defined (_PRAGUERPC_H_)
#define _PRAGUERPC_H_

#include "uuid.h"

namespace PragueRPC {

  typedef unsigned int ConnectionID;
  struct Interface;
  class RequestManager;
  extern RequestManager* theRequestManager;

  bool startRPCService ( const char* aServiceName, unsigned int aMaxClients );
  
  void stopRPCService ();
    
  bool registerInterface ( const Interface& anInterface );

  bool unregisterInterface ( const Interface& anInterface );

  bool connectToRPCService ( const char* aServiceName, ConnectionID& aConnectionID );

  bool disconnectFromRPCService ( ConnectionID aConnectionID );

  struct Method;

  struct Interface {
    UUID     theUUID;
    Method*  theMethodsTable;
  };

}

#define NOT_IMPLEMENTED { int* aCrash = 0;  *aCrash = 0x0;}

#endif // _PRAGUERPC_H_
