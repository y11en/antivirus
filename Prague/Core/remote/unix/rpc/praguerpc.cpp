#include "praguerpc.h"
#include "requestmanager.h"
#include "debugtrace.h"

namespace PragueRPC {

  RequestManager* theRequestManager;  

  bool startRPCService ( const char* aServiceName, 
                         unsigned int aMaxClients )
  {
    ENTER;
    if ( theRequestManager )
      return true;
    theRequestManager = new ( std::nothrow ) RequestManager ( aServiceName, aMaxClients );
    if ( !theRequestManager || !theRequestManager->isInitialized () )
      return false;
    theRequestManager->process ();
    return true;
  }
  
  void stopRPCService ()
  {
    ENTER;
    delete theRequestManager;
    theRequestManager = 0;
  }

  bool connectToRPCService ( const char* aServiceName, ConnectionID& aConnectionID )
  {
    ENTER;
    if ( !theRequestManager )
      return false; 
    return theRequestManager->connectToService ( aServiceName, aConnectionID );
  }

  bool disconnectFromRPCService ( ConnectionID aConnectionID )
  {
    ENTER;
    if ( !theRequestManager )
      return false; 
    return theRequestManager->disconnectFromService ( aConnectionID );
  }
}
