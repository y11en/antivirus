#if !defined (_CONNECTIONMANAGER_CPP_)
#define _CONNECTIONMANAGER_CPP_

#include <pthread.h>
#include <sys/un.h>

#include <vector>
#include <map>
#include <string>

#include "praguerpc.h"

namespace PragueRPC {
  
  class ConnectionManager {
  public:
    typedef int Descriptor;

    ConnectionManager ( const char* aServiceName, unsigned int aMaxClients );
    ~ConnectionManager ();
    
    bool isInitialized () { return theIsInitialized; }
    void process ();
    void stop ();
    bool connectToService ( const char* aServiceName, ConnectionID& aConnectionID );
    bool disconnectFromService ( ConnectionID aConnectionID );
    Descriptor getConnectionForWriting ( ConnectionID aConnectionID );
    Descriptor getConnectionForReading ();
    void putConnection ( ConnectionID aConnectionID, Descriptor aDescriptor );
    void deleteConnection ( ConnectionID aConnectionID, Descriptor aDescriptor );
    void deleteConnection ( Descriptor aDescriptor );
    bool isConnectionAlive ( ConnectionID aConnectionID );
   
  private:
    bool addConnection ( ConnectionID aConnectionID,Descriptor aDescriptor, bool aFreeConnection = true );
    Descriptor getNewConnection ( ConnectionID aConnectionID );
    Descriptor makeNewConnection ( const std::string& aServiceName, bool theFreeConnection, ConnectionID* aConnectinID = 0 );

    typedef std::map<Descriptor,bool> Connections;

    struct Service {
      Service ( const std::string& aServiceName = "" );
      ~Service ();

      std::string           theServiceName;
      unsigned int          theWritingRequestCount;
      Descriptor            theWritingRequestPipe [ 2 ];
      Connections           theConnections;
    };

    Service& findService ( ConnectionID aConnectionID );

    typedef std::map<ConnectionID,Service*> Services;

    bool               theIsInitialized;
    std::string        theServiceName;
    unsigned int       theMaxClients;
    Descriptor         theListenSocket;
    Descriptor         theControlPipe [ 2 ];
    pthread_mutex_t    theServicesLock;
    Services           theServices;
    Descriptor         theReadingConnectionsPipe [ 2 ];
    unsigned int       theReadingRequestCount;
    Services::iterator thePositionIterator;
  };

} // PragueRPC

#endif // _CONNECTIONMANAGER_CPP_
