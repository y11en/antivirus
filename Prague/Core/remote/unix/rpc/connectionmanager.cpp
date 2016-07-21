#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <glob.h>

#include <sstream>
//#define DEBUG_TRACE
#include "debugtrace.h"
#include "connectionmanager.h"
#include "utils.h"


namespace PragueRPC {

#define RequestForReading 1<<1
#define RequestForUpdate  1<<2

  unsigned int thePID = getpid ();
  

  // 
  // ConnectionManager::Services
  //
  ConnectionManager::Service::Service ( const std::string& aServiceName )
    : theServiceName ( aServiceName ),
      theWritingRequestCount ( 0 )
  {
    ENTER;
    pipe ( theWritingRequestPipe );
  }

  ConnectionManager::Service::~Service ()
  {
    ENTER;
    close ( theWritingRequestPipe [ 1 ] );
    theWritingRequestPipe [ 1 ] = -1;
    sched_yield ();
    close ( theWritingRequestPipe [ 0 ] );
    theWritingRequestPipe [ 0 ] = -1;
    Connections::iterator anIterator = theConnections.begin ();
    while ( anIterator != theConnections.end () ) {
      close ( anIterator->first );
      theConnections.erase ( anIterator++ );
    }
  }

  //
  // ConnectionManager
  //

  ConnectionManager::ConnectionManager ( const char* aServiceName, unsigned int aMaxClients )
    : theIsInitialized ( true ),
      theMaxClients ( aMaxClients ),
      theListenSocket ( -1 ),
      theReadingRequestCount ( 0 ),
      thePositionIterator ( theServices.end () )
  {
    ENTER;
    if ( pthread_mutex_init ( &theServicesLock, 0 ) ) {
      TRACE ( "Error: can't create mutex\n");
      theIsInitialized = false;
    }
    theControlPipe [ 0 ] = theControlPipe [ 1 ] = -1;
    theReadingConnectionsPipe [ 0 ] = theReadingConnectionsPipe [ 1 ] = -1;
    if ( pipe ( theControlPipe ) || 
         pipe ( theReadingConnectionsPipe ) ) {
      TRACE ( "Error: can't create pipe\n");
      theIsInitialized = false;
    }    
    theListenSocket = socket ( AF_UNIX, SOCK_STREAM, 0 );
    if ( theListenSocket == -1 ) {
      TRACE ( "Error: can't create listener socket '%s'\n", strerror ( errno ) );
      theIsInitialized = false;
    }
    std::stringstream aStream;
    aStream << aServiceName << thePID;
    unlink ( aStream.str().c_str () );
    struct sockaddr_un anAddress;
    anAddress.sun_family = AF_UNIX;
    memset ( anAddress.sun_path, 0, sizeof ( anAddress.sun_path ) );
    strncpy ( anAddress.sun_path, aStream.str().c_str (), sizeof ( anAddress.sun_path ) - 1 ); 

	// set newly created sockets access rights to 0666 (read and write everyone)
	mode_t oldMode = umask( 0111 );
    if ( bind ( theListenSocket, reinterpret_cast<sockaddr*> ( &anAddress ), sizeof ( anAddress ) ) ) {
      TRACE ( "Error: can't bind listener socket '%s'\n", strerror ( errno ) );
      theIsInitialized = false;
    }
	// restore previous socket access rights
	umask( oldMode );

//	RM - commented out A. Sychev's way to share priviligies, would consider it later
//    chmod(anAddress.sun_path, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
	if ( ::listen ( theListenSocket, 10 ) ) {
      TRACE ( "Error: can't listen socket '%s'\n", strerror ( errno ) );
      theIsInitialized = false;
    }
    theServiceName = aStream.str().c_str ();
    if ( !theIsInitialized ) {
      pthread_mutex_destroy ( &theServicesLock );
      close ( theControlPipe [ 1 ] );
      theControlPipe [ 1 ] = -1;
      close ( theControlPipe [ 0 ] );
      theControlPipe [ 0 ] = -1;
      close ( theReadingConnectionsPipe [ 1 ] );
      theReadingConnectionsPipe [ 1 ] = -1;
      close ( theReadingConnectionsPipe [ 0 ] );
      theReadingConnectionsPipe [ 0 ] = -1;
      close ( theListenSocket );
      theListenSocket = -1;
      return;
    }
  }

  ConnectionManager::~ConnectionManager ()
  {
    ENTER;
    if (  theControlPipe [ 1 ] != -1 )
      stop ();
    close ( theControlPipe [ 0 ] );
    theControlPipe [ 0 ] = -1;
    close ( theReadingConnectionsPipe [ 0 ] );
    theReadingConnectionsPipe [ 0 ] = -1;
    pthread_mutex_destroy ( &theServicesLock );
  }

  void ConnectionManager::process ()
  {
    ENTER;
    if ( !theIsInitialized )
      return;
    while ( true ) {
      fd_set aReadingSet, aWritingSet;
      FD_ZERO ( &aReadingSet );
      FD_ZERO ( &aWritingSet );
      FD_SET ( theControlPipe [ 0 ], &aReadingSet );
      FD_SET ( theListenSocket, &aReadingSet );
      Descriptor aMaxDescriptor = ( theControlPipe [ 0 ] > theListenSocket ) ? theControlPipe [ 0 ] : theListenSocket;
      Lock aLock ( &theServicesLock );
      bool aWritingSetIsNotEmpty = false;
      for ( Services::const_iterator aServiceIterator = theServices.begin ();
            aServiceIterator != theServices.end ();
            ++aServiceIterator ) {
        for ( Connections::const_iterator anIterator = aServiceIterator->second->theConnections.begin ();
              anIterator !=  aServiceIterator->second->theConnections.end ();
              ++anIterator ) {
          if ( anIterator->second  ) {
            if ( theReadingRequestCount )
              FD_SET ( anIterator->first, &aReadingSet );
            
            if ( aServiceIterator->second->theWritingRequestCount ) {
              FD_SET ( anIterator->first, &aWritingSet );
              aWritingSetIsNotEmpty = true;
            }
            if ( anIterator->first > aMaxDescriptor )
              aMaxDescriptor = anIterator->first; 
          }
        }
      }
      aLock.unlock ();
      int aCount = select ( aMaxDescriptor + 1, &aReadingSet, aWritingSetIsNotEmpty ? &aWritingSet : 0, 0, 0 );
      if ( aCount < 0 ) {
        TRACE ( "Error: something wrong with select '%s'\n", strerror ( errno ) );
        continue;
      }
      if ( FD_ISSET ( theControlPipe [ 0 ], &aReadingSet ) ) {
        TRACE ( "Notice: there is a message in the control pipe\n" );
        int aRequest = 0;
        int aResult = read ( theControlPipe [ 0 ], &aRequest, sizeof ( aRequest ) );
        if ( !aResult ) {
          TRACE ( "Notice: the control pipe has been closed - a time to stop\n" );
          break;
        }
        switch ( aRequest ){
        case RequestForReading:
          theReadingRequestCount++;
	  TRACE ( "Notice: the message in the control pipe - request for reading\n" );        
          break;
        case RequestForUpdate:
	  TRACE ( "Notice: the message in the control pipe - request for update\n" );        
          break;
        default:
	  TRACE ( "Notice: the message in the control pipe - %d\n", aRequest );        
          aLock.lock ();
          Services::iterator anIterator =  theServices.find ( aRequest );
          if ( anIterator != theServices.end () )
            anIterator->second->theWritingRequestCount += 1;
          aLock.unlock ();
          break;
        };
        if ( aCount-- == 1 ) 
          continue;
      }
      if ( FD_ISSET ( theListenSocket, &aReadingSet ) ) {
        TRACE ( "Notice: there is a connection request\n" );
        Descriptor aDescriptor = accept ( theListenSocket, 0, 0 );
        if (  aDescriptor == -1 ) {
          TRACE ( "Error: can't accept a connection '%s'\n", strerror ( errno ) );
        }
        else {
          ConnectionID aConnectionID = 0;
          aLock.lock ();
          if ( ( recv ( aDescriptor, &aConnectionID, sizeof ( aConnectionID ), MSG_WAITALL ) == sizeof ( aConnectionID ) ) &&
               ( ( theServices.find ( aConnectionID ) != theServices.end () ) || 
                 ( !theMaxClients || ( theServices.size () < theMaxClients ) ) ) &&
               ( send ( aDescriptor, &thePID, sizeof ( thePID ), MSG_NOSIGNAL ) == sizeof ( thePID ) ) ) {
            TRACE ( "Notice: the handshaking with %d has complited\n", aConnectionID );        
	    aLock.unlock ();
            if ( !addConnection ( aConnectionID, aDescriptor ) ) {
              close ( aDescriptor );
              aDescriptor = -1;
              TRACE ( "Error: can't add connection\n" );        
            }
              
          }
          else {
            TRACE ( "Error: the handshaking has broken '%s'\n", strerror ( errno )  );        
            close ( aDescriptor );
            aDescriptor = -1;
          }
          aLock.unlock ();
        }
        if ( aCount-- == 1 ) 
          continue;
      }
      aLock.lock ();

//       for ( Services::iterator aServiceIterator = theServices.begin ();
//             aServiceIterator != theServices.end ();
//             ++aServiceIterator ) {

      if ( thePositionIterator == theServices.end () )
        thePositionIterator = theServices.begin ();
      else
        ++thePositionIterator;
      Services::iterator aServiceIterator = thePositionIterator;
      while ( aServiceIterator != theServices.end () ) {
        for ( Connections::iterator anIterator = aServiceIterator->second->theConnections.begin ();
              anIterator != aServiceIterator->second->theConnections.end ();
              ++anIterator ) { 
          if ( !anIterator->second )
            continue;
          if ( theReadingRequestCount && FD_ISSET ( anIterator->first, &aReadingSet ) ) {
            TRACE ( "Notice: there is a ready for reading connection - %d\n", anIterator->first );
            if ( write ( theReadingConnectionsPipe [ 1 ], 
                         &anIterator->first, 
                         sizeof ( anIterator->first ) ) == sizeof ( anIterator->first ) ) {
              TRACE ( "Notice: a ready for reading connection has been gave - %d\n", anIterator->first );
              anIterator->second = false;
              --theReadingRequestCount;
            }
            continue;
          }
          if ( aServiceIterator->second->theWritingRequestCount && FD_ISSET ( anIterator->first, &aWritingSet ) ) {
            TRACE ( "Notice: there is a ready for writing connection -%d\n", anIterator->first );
            if ( write ( aServiceIterator->second->theWritingRequestPipe [ 1 ], 
                         &anIterator->first, 
                         sizeof ( anIterator->first ) ) == sizeof ( anIterator->first ) ) {
              TRACE ( "Notice: a ready for writing connection has been gave - %d \n", anIterator->first );
              anIterator->second = false;
              aServiceIterator->second->theWritingRequestCount -= 1;
            }
          }
        }
        if ( ++aServiceIterator == theServices.end () )
          aServiceIterator = theServices.begin ();
        if ( aServiceIterator == thePositionIterator )
          break;
      }
    }
  }


  void ConnectionManager::stop ()
  {
    ENTER;
    if ( !theIsInitialized )
      return;
    close ( theControlPipe [ 1 ] );
    theControlPipe [ 1 ] = -1;
    sched_yield ();
    close ( theReadingConnectionsPipe [ 1 ] );
    theReadingConnectionsPipe [ 1 ] = -1;
    close ( theListenSocket );
    theListenSocket = -1;
    Lock aLock ( &theServicesLock );
    unlink ( theServiceName.c_str () );
    for ( Services::iterator anIterator = theServices.begin ();
          anIterator != theServices.end ();
          ++anIterator )
      delete anIterator->second;
    theServices.clear ();
  }

  bool ConnectionManager::connectToService ( const char* aServiceName, ConnectionID& aConnectionID )
  {
    ENTER;
    if ( !theIsInitialized )
      return false;
    std::string aServicePattern ( aServiceName );
    aServicePattern += "[0-9]*";
    glob_t aGlob;
    if ( glob ( aServicePattern.c_str (), GLOB_ERR, 0, &aGlob ) ) {
      globfree ( &aGlob );
      return false;
    }
    Descriptor aDescriptor = -1;
    for ( size_t i = 0; i < aGlob.gl_pathc; ++i ) {
      struct stat aStatus;
      if ( stat ( aGlob.gl_pathv [ i ], &aStatus ) || !S_ISSOCK(aStatus.st_mode) )
        continue;
      Lock aLock ( &theServicesLock );
      for ( Services::const_iterator anIterator = theServices.begin ();
            anIterator != theServices.end ();
            ++anIterator )
        if ( anIterator->second->theServiceName == aGlob.gl_pathv [ i ] ) {
          TRACE ( "The connection is already active\n");
          aConnectionID = anIterator->first;
          return true;
        }
      aLock.unlock();
      aDescriptor = makeNewConnection ( aGlob.gl_pathv [ i ], true, &aConnectionID );
      if ( aDescriptor != -1 )
        break;
    }
    globfree ( &aGlob );    
    return aDescriptor != -1;
  }

  bool ConnectionManager::disconnectFromService ( ConnectionID aConnectionID )
  {
    ENTER;
    if ( !theIsInitialized )
      return false;
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    if ( anIterator == theServices.end () )
      return false;
    Service* aService = anIterator->second;
    theServices.erase ( anIterator );
    thePositionIterator = theServices.begin ();
    delete aService;
    return true;
  }

  ConnectionManager::Descriptor ConnectionManager::getConnectionForReading ()
  {
    ENTER;
    if ( !theIsInitialized )
      return -1;
    int aTmp = RequestForReading;
    //    if ( theReadingRequestCount > 1 )
      write ( theControlPipe [ 1 ], &aTmp, sizeof ( aTmp ) );
    Descriptor aDescriptor = -1;
    int aResult = read ( theReadingConnectionsPipe [ 0 ], &aDescriptor, sizeof ( aDescriptor ) );
    TRACE ( "Notice: a connection for reading has been got %d\n", aDescriptor );
    return ( aResult == sizeof ( aDescriptor ) ) ? aDescriptor : -1;
  }

  ConnectionManager::Descriptor ConnectionManager::getConnectionForWriting ( ConnectionID aConnectionID )
  {
    ENTER;
    if ( !theIsInitialized )
      return -1;
    TRACE ( "connection %d\n", aConnectionID );
    Descriptor aDescriptor = getNewConnection ( aConnectionID );
    if ( aDescriptor != -1 ) {
      TRACE ( "Notice: a connection for writing has been got %d\n", aDescriptor );
      return aDescriptor;
    }
    if ( !isConnectionAlive ( aConnectionID ) ) 
      return -1;
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    if ( anIterator == theServices.end () )
      return -1;
    Service* aService = anIterator->second;
    aLock.unlock ();
    int aTmp = aConnectionID;
    write ( theControlPipe [ 1 ], &aTmp, sizeof ( aTmp ) );
    int aResult = read ( aService->theWritingRequestPipe [ 0 ], &aDescriptor, sizeof ( aDescriptor ) );
    TRACE ( "Notice: a connection for writing has been got %d\n", aDescriptor );
    return ( aResult == sizeof ( aDescriptor ) ) ? aDescriptor : -1;
  }

  void ConnectionManager::putConnection ( ConnectionID aConnectionID, Descriptor aDescriptor )
  {
    ENTER;
    if ( !theIsInitialized )
      return;
    TRACE ( "connection %d\n", aConnectionID );
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    if ( anIterator == theServices.end () )
      return;
    Service* aService = anIterator->second;
    aService->theConnections [ aDescriptor ] = true;
    aLock.unlock ();
    int aTmp = RequestForUpdate;
    write ( theControlPipe [ 1 ], &aTmp, sizeof ( aTmp ) );
    TRACE ( "Notice: a connections has been put to the connections list - %d\n", aDescriptor );
    sched_yield ();
  }

  bool ConnectionManager::addConnection ( ConnectionID aConnectionID, Descriptor aDescriptor, bool aFreeConnection ) 
  { 
    ENTER;
    TRACE ( "connection %d\n", aConnectionID );
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    Service* aService = 0;
    if ( anIterator == theServices.end () ) {
      if ( ( aService = new (std::nothrow) Service ) == 0 )
        return false;
      theServices [ aConnectionID ] = aService;
    }
    else
      aService = anIterator->second;
    aService->theConnections [ aDescriptor ] = aFreeConnection;
    aLock.unlock ();
    int aTmp = RequestForUpdate;
    write ( theControlPipe [ 1 ], &aTmp, sizeof ( aTmp ) );
    TRACE ( "Notice: a new conection has been added to the connections list - %d\n", aDescriptor );
    return true;
  }
  
  void ConnectionManager::deleteConnection ( ConnectionID aConnectionID, Descriptor aDescriptor ) 
  { 
    ENTER;
    if ( !theIsInitialized )
      return;
    TRACE ( "connection %d\n", aConnectionID );
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    if ( anIterator == theServices.end () )
      return;
    Service* aService = anIterator->second;
    aService->theConnections.erase ( aDescriptor );
    if ( ( makeNewConnection ( aService->theServiceName, true ) == -1 ) &&
         !aService->theConnections.size () ) {
      TRACE ( "Notice: it seems the service %s is down\n", aService->theServiceName.c_str () );
      theServices.erase ( aConnectionID );
      thePositionIterator = theServices.begin ();
      delete aService;
    }
    aLock.unlock ();
    int aTmp = RequestForUpdate;
    write ( theControlPipe [ 1 ], &aTmp, sizeof ( aTmp ) );
    close ( aDescriptor ); 
  }

  void ConnectionManager::deleteConnection ( Descriptor aDescriptor ) 
  { 
    ENTER;
    if ( !theIsInitialized )
      return;
    Lock aLock ( &theServicesLock );
    ConnectionID aConnectionID = 0;
    for ( Services::const_iterator aServiceIterator = theServices.begin ();
          aServiceIterator != theServices.end ();
          ++aServiceIterator ) 
      if ( aServiceIterator->second->theConnections.find ( aDescriptor) != aServiceIterator->second->theConnections.end () ) {
        aConnectionID = aServiceIterator->first;
        break;
      }
    if ( aConnectionID == 0)
      return;
    TRACE ( "connection %d\n", aConnectionID );
    aLock.unlock ();
    deleteConnection ( aConnectionID, aDescriptor ); 
  }

  bool ConnectionManager::isConnectionAlive ( ConnectionID aConnectionID )
  {
    ENTER;
    if ( !theIsInitialized )
      return false;
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    if ( anIterator == theServices.end () ) {
      TRACE ( "Notice: the connection %d is not found \n", aConnectionID );
      return false;
    }
    return anIterator->second->theConnections.size () != 0;
  }
  
  ConnectionManager::Descriptor ConnectionManager::getNewConnection ( ConnectionID aConnectionID )
  {
    ENTER;
    Lock aLock ( &theServicesLock );
    Services::iterator anIterator =  theServices.find ( aConnectionID );
    if ( anIterator == theServices.end () )
      return -1;
    Service* aService = anIterator->second;
    for ( Connections::const_iterator anIterator = aService->theConnections.begin ();
          anIterator !=  aService->theConnections.end ();
          ++anIterator )
      if ( anIterator->second )
        return -1;
    return makeNewConnection ( aService->theServiceName, false );
  }
  
  ConnectionManager::Descriptor ConnectionManager::makeNewConnection ( const std::string& aServiceName, 
                                                                       bool aFreeConnection, 
                                                                       ConnectionID* aConnectinID )
  {
    ENTER;
    if ( aServiceName.empty () )
      return -1;
    Descriptor aDescriptor = socket ( AF_UNIX, SOCK_STREAM, 0 );
    if ( aDescriptor == -1 )
      return -1;
    struct sockaddr_un anAddress;
    anAddress.sun_family = AF_UNIX;
    memset ( anAddress.sun_path, 0, sizeof ( anAddress.sun_path ) );
    strncpy ( anAddress.sun_path, aServiceName.c_str(), sizeof ( anAddress.sun_path ) - 1 );
    if ( connect ( aDescriptor, reinterpret_cast<sockaddr*> ( &anAddress ), sizeof ( anAddress ) ) ) { 
      TRACE ( "Error: can't connect to the server %s '%s'\n", aServiceName.c_str(), strerror ( errno ) );
      return -1;
    }
    ConnectionID aCID = thePID;
    if ( ( send ( aDescriptor, &aCID, sizeof ( aCID ), MSG_NOSIGNAL ) == sizeof ( aCID ) ) && 
         ( recv ( aDescriptor, &aCID, sizeof ( aCID ), MSG_WAITALL ) == sizeof ( aCID ) ) ) {
      TRACE ( "Notice: the handshaking with %d has complited\n", aCID );        
      if ( !addConnection ( aCID, aDescriptor, aFreeConnection ) ) {
        close ( aDescriptor );
        return -1;
      }
      Lock aLock ( &theServicesLock );      
      theServices [ aCID ]->theServiceName = aServiceName;
      thePositionIterator = theServices.begin ();
      if ( aConnectinID  )
        *aConnectinID = aCID;
    }
    else {
      TRACE ( "Error: the handshaking has broken\n" );        
      close ( aDescriptor );
      return -1;
    }
    TRACE ( "Notice: a new connection has been got - %d\n", aDescriptor );
    return aDescriptor;
  }

} // PragueRPC
