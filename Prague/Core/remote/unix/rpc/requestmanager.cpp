#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#include <utility>

//#define DEBUG_TRACE
#include "debugtrace.h"
#include "requestmanager.h"
#include "utils.h"

namespace PragueRPC {

  extern unsigned int thePID;

  unsigned int theWaitTimeOut = 1000;

  typedef bool (* ProxyMethod) ( Request& );

  typedef std::map<UUID,Method*> Interfaces;
  Interfaces        theInterfaces;

  RequestManager::RequestManager ( const char* aServerName, 
                                   unsigned int aMaxClients )
    : theIsInitialized ( false ),
      theConnectionManager ( aServerName, aMaxClients )
    
  {
    ENTER;
    if ( !theConnectionManager.isInitialized () )
      return;
    THP_InitData ( &theThreadPool );
    theThreadPool.m_MaxThreads = aMaxClients * 2;
    theThreadPool.m_YieldTimeout = 1000;
    if ( pthread_mutex_init ( &theResponseMutex, 0 ) || 
         pthread_cond_init ( &theResponseCondition, 0 ) || 
         !initCommonTasks () ) {
      pthread_mutex_destroy ( &theResponseMutex );
      pthread_cond_destroy ( &theResponseCondition );
      return;
    }
    theIsInitialized = true;
  }

  RequestManager::~RequestManager ()
  {
    ENTER;
    if ( !theIsInitialized )
      return;
    theConnectionManager.stop ();
    THP_Stop ( &theThreadPool );
    THP_DoneData ( &theThreadPool );
    pthread_mutex_destroy ( &theResponseMutex );
    pthread_cond_destroy ( &theResponseCondition );
  }
  
  void RequestManager::process ()
  {
    ENTER;
    if ( !theIsInitialized )
      return;
    THP_Start ( &theThreadPool );
  }

  bool RequestManager::processRequest ( Request& aRequest )
  {
    ENTER;
    if ( !theIsInitialized )
      return false;
    Descriptor aDescriptor = theConnectionManager.getConnectionForWriting ( aRequest.getConnectionID () ); 
    if ( aDescriptor == -1 ) {
      TRACE ( "Error: can't get a descriptor for writing '%s'\n", strerror ( errno ) );
      return false;
    }
    bool aResult = aRequest.write ( aDescriptor );
    if ( !aResult ) {
      theConnectionManager.deleteConnection ( aRequest.getConnectionID (), aDescriptor );
      return false;
    }
    aRequest.setStatus ( RequestHeader::Processing );
    Lock aLock ( &theResponseMutex );
    theRequestsList.push_back ( aRequest );
    aLock.unlock ();
    theConnectionManager.putConnection ( aRequest.getConnectionID (), aDescriptor );
    ++theThreadPool.m_MaxThreads;
    do {
      struct timeval aNow;
      gettimeofday(&aNow, 0 );
      struct timespec aTimeOut;
      aTimeOut.tv_sec = aNow.tv_sec + theWaitTimeOut / 1000 ;
      aTimeOut.tv_nsec = aNow.tv_usec * 1000 + theWaitTimeOut % 1000 * 1000000;
      aLock.lock ();
      if ( ( pthread_cond_timedwait (  &theResponseCondition, &theResponseMutex, &aTimeOut ) == ETIMEDOUT ) &&
           !isConnectionAlive ( aRequest.getConnectionID () ) )
        aRequest.setStatus ( RequestHeader::TerminateProcessing );  
    }  
    while ( aRequest.getStatus () == RequestHeader::Processing );
    TRACE ( "Notice: the response has been got\n" );
    theRequestsList.remove ( aRequest );
    --theThreadPool.m_MaxThreads;
    return aResult;
  }

  bool RequestManager::initCommonTasks ()
  {
    ENTER;
    if ( PR_FAIL ( THP_CreateSeparateThread ( &theThreadPool, connectionTask, reinterpret_cast<tDWORD> ( this ) ) ) )    
      return false;
    RequestManager* aRequestManager = this;
    if ( PR_FAIL ( THP_AddTask ( &theThreadPool, 0, processingTask, &aRequestManager, sizeof ( aRequestManager ), 0 ) ) )    
      return false;    
    return true;
  }

  bool RequestManager::processRequest ( Descriptor aDescriptor )
  {
    ENTER;
    RequestHeader aRequestHeader;
    if ( !aRequestHeader.read ( aDescriptor ) ) {
      theConnectionManager.deleteConnection ( aDescriptor );
      return false;
    }
    if ( aRequestHeader.isLocal () ) 
      return processLocalRequest ( aRequestHeader, aDescriptor );
    else
      return processRemoteRequest ( aRequestHeader, aDescriptor );
  }

  bool RequestManager::processLocalRequest ( RequestHeader& aRequestHeader, Descriptor aDescriptor )
  {
    ENTER;
    Lock aLock ( &theResponseMutex );
    for ( RequestsList::iterator anIterator = theRequestsList.begin ();
          anIterator != theRequestsList.end (); 
          ++anIterator ) 
      if ( *anIterator == aRequestHeader ) {
        Request& aRequest = *anIterator;
        aRequest.setStatus ( aRequestHeader.getStatus () );
        bool aResult = aRequest.read ( aDescriptor );
        if ( aResult ) 
          theConnectionManager.putConnection ( aRequestHeader.getConnectionID (), aDescriptor );
        else
          theConnectionManager.deleteConnection ( aRequestHeader.getConnectionID (), aDescriptor );      
        pthread_cond_broadcast ( &theResponseCondition );
        return aResult;
      }
    Request aRequest ( aRequestHeader );
    bool aResult = aRequest.read ( aDescriptor );
    if ( aResult )
      theConnectionManager.putConnection ( aRequestHeader.getConnectionID (), aDescriptor );
    else
      theConnectionManager.deleteConnection ( aRequestHeader.getConnectionID (), aDescriptor ); 
    return aResult;
  }
  
  bool RequestManager::processRemoteRequest ( RequestHeader& aRequestHeader, Descriptor aDescriptor )
  {
    ENTER;
    Request aRequest ( aRequestHeader );
    if ( !aRequest.read ( aDescriptor ) ) {
      theConnectionManager.deleteConnection ( aRequestHeader.getClientID (), aDescriptor );      
      return false;
    }
    theConnectionManager.putConnection ( aRequestHeader.getClientID (), aDescriptor );
    Interfaces::iterator anIterator = theInterfaces.find ( aRequestHeader.getUUID () );
    if ( anIterator == theInterfaces.end () ) {
      TRACE ( "Error: the interface hasn't found\n" );
      aRequest.setStatus ( RequestHeader::InterfaceNotFound );
      return processResponse ( aRequest );
    }
    Method* aMethodTable = anIterator->second;
    unsigned int anIndex = 0;
    while ( aMethodTable [ anIndex ].theMethodHash && aMethodTable [ anIndex ].theMethod ) {
      if ( aMethodTable [ anIndex ].theMethodHash == aRequest.getMethodHash () )
        break;
      ++anIndex;
    }
    if ( aMethodTable [ anIndex ].theMethodHash && aMethodTable [ anIndex ].theMethod ) {
      ProxyMethod aMethod = (ProxyMethod) aMethodTable [ anIndex ].theMethod;
      if ( !aMethod ( aRequest ) )
        aRequest.setStatus ( RequestHeader::ProcessingError );
      
    }
    else 
      aRequest.setStatus ( RequestHeader::MethodNotFound );
    return processResponse ( aRequest );
  }

  bool RequestManager::processResponse ( Request& aRequest )
  {
    ENTER;
    Descriptor aDescriptor = theConnectionManager.getConnectionForWriting ( aRequest.getClientID () ); 
    if ( aDescriptor == -1 )
      return false;
    bool aResult = aRequest.write ( aDescriptor );
    if ( aResult )
      theConnectionManager.putConnection ( aRequest.getClientID (), aDescriptor );
    else
      theConnectionManager.deleteConnection ( aRequest.getClientID (), aDescriptor );
    return aResult;
  }

  tERROR RequestManager::connectionTask ( tThreadCallType aCallType, 
                                          tPTR aThreadContext, 
                                          tPTR* aTaskContext, 
                                          tPTR aTaskData, 
                                          tDWORD aTaskDataLen )
  {
    ENTER;
    TRACE ( "Notice: the connection task has started\n" );
    RequestManager* aRequestManager = reinterpret_cast<RequestManager*> ( aTaskData );
    aRequestManager->theConnectionManager.process ();
    Lock aLock ( &aRequestManager->theResponseMutex );
    for ( RequestManager::RequestsList::iterator anIterator = aRequestManager->theRequestsList.begin ();
          anIterator != aRequestManager->theRequestsList.end ();
          ++anIterator ) 
      anIterator->setStatus ( RequestHeader::TerminateProcessing );
    pthread_cond_broadcast ( &aRequestManager->theResponseCondition );
    TRACE ( "Notice: the connection task has done\n" );
    return errOK;
  }


  tERROR RequestManager::processingTask ( tThreadCallType aCallType, 
                                          tPTR aThreadContext, 
                                          tPTR* aTaskContext, 
                                          tPTR aTaskData, 
                                          tDWORD aTaskDataLen )
  {
    ENTER;
    if ( aCallType != TP_CBTYPE_TASK_PROCESS )
      return errOK;
    TRACE ( "Notice: the processing task has started\n" );
    RequestManager* aRequestManager = * ( reinterpret_cast<RequestManager**> ( aTaskData ) );
    Descriptor aDescriptor = aRequestManager->theConnectionManager.getConnectionForReading ();
    if ( aDescriptor == -1 ) {
      TRACE ( "Error: can't get a connection. Stop the processing task\n" );      
      return errUNEXPECTED;
    }
    tERROR anError = THP_AddTask ( &aRequestManager->theThreadPool, 0, processingTask, aTaskData, aTaskDataLen, 0 );    
    if ( PR_FAIL ( anError ) ) {
      TRACE ( "Error: can't add a new task\n" );
      return anError;
    }
    aRequestManager->processRequest ( aDescriptor );
    TRACE ( "Notice: the processing task has done\n" );
    return errOK;
  }

  bool registerInterface ( const Interface& anInterface )
  {    
    ENTER;
    std::pair<Interfaces::const_iterator,bool> aResult = theInterfaces.insert ( std::make_pair ( anInterface.theUUID,
                                                                                                 anInterface.theMethodsTable ) );
    return aResult.second;
  }

  bool unregisterInterface ( const Interface& anInterface )
  {    
    ENTER;
    return theInterfaces.erase ( anInterface.theUUID );
  }

}
