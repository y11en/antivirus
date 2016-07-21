#if !defined (_REQUESTMANAGER_H_)
#define _REQUESTMANAGER_H_

#include <pthread.h>
#include <map>

#include <nestednodelist.h>
#include <thpimpl.h>

#include "connectionmanager.h"
#include "request.h"

namespace PragueRPC {

  class RequestManager {
  public:
    typedef ConnectionManager::Descriptor Descriptor;
    RequestManager ( const char* aServiceName, unsigned int aMaxClients );
    ~RequestManager ();
    
    bool isInitialized () { return theIsInitialized; }
    void process ();
    bool processRequest ( Request& aRequest );
    bool connectToService ( const char* aServiceName, ConnectionID& aConnectionID ) 
    { return theConnectionManager.connectToService ( aServiceName, aConnectionID ); }
    bool disconnectFromService ( ConnectionID aConnectionID )
    { return theConnectionManager.disconnectFromService ( aConnectionID ); }
    bool isConnectionAlive ( ConnectionID aConnectionID ) { return theConnectionManager.isConnectionAlive ( aConnectionID ); }

  private:
    bool initCommonTasks ();
    bool processRequest ( Descriptor aDescriptor );
    bool processLocalRequest ( RequestHeader& aRequestHeader, Descriptor aDescriptor );
    bool processRemoteRequest ( RequestHeader& aRequestHeader, Descriptor aDescriptor );
    bool processResponse ( Request& aRequest );
    
    static tERROR connectionTask ( tThreadCallType aCallType, 
                                   tPTR aThreadContext, 
                                   tPTR* aTaskContext, 
                                   tPTR aTaskData, 
                                   tDWORD aTaskDataLen );
    static tERROR processingTask ( tThreadCallType aCallType, 
                                   tPTR aThreadContext, 
                                   tPTR* aTaskContext, 
                                   tPTR aTaskData, 
                                   tDWORD aTaskDataLen );

    typedef NestedNodeListNS::NestedNodeList<Request> RequestsList;

    bool              theIsInitialized;
    ConnectionManager theConnectionManager;
    pthread_mutex_t   theResponseMutex; 
    pthread_cond_t    theResponseCondition;
    THPoolData        theThreadPool;
    RequestsList      theRequestsList;
  };

  bool registerInterface ( const Interface& anInterface );
  bool unregisterInterface ( const Interface& anInterface );

}

#endif // _REQUESTMANAGER_H_
