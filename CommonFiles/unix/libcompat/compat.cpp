#if defined (HAVE_CONFIG_H)
#include <config-unix.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <dirent.h>
#include <libgen.h>
#include <fnmatch.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/time.h>
#if !defined (HAVE_FLOCK)
#include "../../../../compat/comp_flock.h"
#endif

#include <map>
#include <sstream>
#include <list>
#include <string>
#include <vector>

using std::string;

#include "compat.h"

//#define COMPAT_DEBUG
#ifdef COMPAT_DEBUG
#include <mcheck.h>
#define TRACE printf
#else // COMPAT_DEBUG
void printf_dummy (...) {}
#define TRACE printf_dummy
#endif // COMPAT_DEBUG

#define NOT_IMPLEMENTED int* not_implemented = 0; *not_implemented = 1;

#define CALL_ENTRY TlsSetValue ( lastErrorKey,  reinterpret_cast <void*> ( NO_ERROR ) );

#if defined (__linux__) && defined (THREADS)
	#define PTHREAD_MUTEX_RECURSIVE			PTHREAD_MUTEX_RECURSIVE_NP
#endif // __linux__

#if defined (__FreeBSD__) ||  defined (__OpenBSD__) || (__APPLE__) 
#define MAP_ANONYMOUS MAP_ANON
#endif // __FreeBSD__

const string MutexSuffix = "_mutex";
const string SemaphoreSuffix = "_semaphore";
const string EventSuffix = "_event";
string TempDirectory = "/tmp/";
const string MetafileExt = ".meta";
const string IPCPrefix = "libcompat_";
const string LockFile = "libcompat.lock";
const string AnonymousSharedMemory = "/dev/zero";


#if defined (__OpenBSD__) && !defined (RTLD_LOCAL)
#define RTLD_LOCAL RTLD_LAZY
#endif //__OpenBSD__

#if defined (USE_TMPFS)
#define Shm_open shm_open
#define Shm_unlink shm_unlink
#else // USE_TMPFS   
int Shm_open ( const char *name, int oflag, mode_t mode )
{
  char aName [ PATH_MAX ] = {0};
  strcat ( aName, TempDirectory.c_str () );
  strcat ( aName, IPCPrefix.c_str () );
  strncat ( aName, name, sizeof ( aName ) - TempDirectory.size () - IPCPrefix.size () ); 
  bool anAlreadyExists = !access ( aName, F_OK );
  if ( anAlreadyExists && ( oflag & O_EXCL ) ) {
    errno = EEXIST;
    return -1;
  }
  if ( !anAlreadyExists && !( oflag & O_CREAT ) ){
    return -1;
  }
  int aFD  = open ( aName, oflag | O_NOFOLLOW, mode );
  if ( aFD == -1 )
    return -1;
  if ( anAlreadyExists && !( oflag & O_CREAT ) ) { 
    if ( !flock ( aFD, LOCK_EX | LOCK_NB ) ) {
      unlink ( aName );
      flock ( aFD, LOCK_UN );
      close ( aFD );
      errno = ENOENT;
      return -1;
    }
  }
  return aFD;
}

int Shm_unlink ( const char *name )
{
  char aName [ PATH_MAX ] = {0};
  strcat ( aName, TempDirectory.c_str () );
  strcat ( aName, IPCPrefix.c_str () );
  strncat ( aName, name, sizeof ( aName ) - TempDirectory.size () - IPCPrefix.size () ); 
  return unlink ( aName );
}
#endif // USE_TMPFS   


#if defined (THREADS)
#if defined (__linux__)
typedef pthread_spinlock_t CompatLock_Fast;
#define COMPAT_INIT_LOCK_FAST(lock) pthread_spin_init ( lock, 0 )
#define COMPAT_DESTROY_LOCK_FAST(lock) pthread_spin_destroy ( lock )
#define COMPAT_LOCK_FAST(lock) pthread_spin_lock ( lock ) 
#define COMPAT_TRYLOCK_FAST(lock) ( pthread_spin_trylock ( lock )) ? FALSE : TRUE
#define COMPAT_UNLOCK_FAST(lock) pthread_spin_unlock ( lock )
#else // __linux__
typedef pthread_mutex_t CompatLock_Fast;
#define COMPAT_INIT_LOCK_FAST(lock) { pthread_mutexattr_t attr; \
                   pthread_mutexattr_init (&attr); \
                   pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE); \
                   pthread_mutex_init ( lock, &attr); } 
#define COMPAT_DESTROY_LOCK_FAST(lock) pthread_mutex_destroy ( lock )
#define COMPAT_LOCK_FAST(lock)   pthread_mutex_lock ( lock )
#define COMPAT_TRYLOCK_FAST(lock) ( pthread_mutex_trylock ( lock ) == EBUSY ) ? FALSE : TRUE
#define COMPAT_UNLOCK_FAST(lock)  pthread_mutex_unlock ( lock )
#endif // __linux__

typedef pthread_mutex_t CompatLock;
#define COMPAT_INIT_LOCK(lock) { pthread_mutexattr_t attr; \
                   pthread_mutexattr_init (&attr); \
                   pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE); \
                   pthread_mutex_init ( lock, &attr); } 
#define COMPAT_DESTROY_LOCK(lock) pthread_mutex_destroy ( lock )
#define COMPAT_LOCK(lock)   pthread_mutex_lock ( lock )
#define COMPAT_TRYLOCK(lock) ( pthread_mutex_trylock ( lock ) == EBUSY ) ? FALSE : TRUE
#define COMPAT_UNLOCK(lock)  pthread_mutex_unlock ( lock )
#else // THREADS
typedef char CompatLock_Fast;
typedef char CompatLock;
#define COMPAT_INIT_LOCK_FAST(lock)
#define COMPAT_DESTROY_LOCK_FAST(lock)
#define COMPAT_LOCK_FAST(lock)
#define COMPAT_TRYLOCK_FAST(lock) TRUE
#define COMPAT_UNLOCK_FAST(lock)
#define COMPAT_INIT_LOCK(lock)
#define COMPAT_DESTROY_LOCK(lock)
#define COMPAT_LOCK(lock)  
#define COMPAT_TRYLOCK(lock) TRUE
#define COMPAT_UNLOCK(lock)
#endif  // THREADS

char ApplicationName [PATH_MAX];
long theStartSeconds;

enum Tag { MUTEX, SEMAPHORE, EVENT, THREAD, FILES, FILEMAP } ;
DWORD lastErrorKey;

struct Mutex {
  int theFDs [ 2 ];
  CompatLock theMutex;
#if defined (THREADS)
  pthread_t theOwner;
#endif // THREADS  
  char* theName;
};

struct Event {
  int theFDs [ 2 ];
  BOOL theManualReset;
  CompatLock theMutex;
  char* theName;
};

struct File {
  int theFD;
};

struct Semaphore {
  int theFDs [ 2 ];
  CompatLock theMutex;
  long theMaxCount;
  char* theName;
};

struct FileMap {
  int theFD;
  char* theName;
};

struct Thread {
  int theFDs [ 2 ];
  CompatLock theMutex;
#if defined (THREADS)  
  pthread_t theThreadID;
#endif // THREADS   
  LPTHREAD_START_ROUTINE theStartAddress;
  void* theParameter;
  DWORD theExitStatus;
};
struct Handle {
  Tag theTag;
  union {
    Mutex            theMutex;
    Event            theEvent;
    File             theFile;
    Semaphore        theSemaphore;
    FileMap          theFileMap;
    Thread           theThread;
  };
  int theCount;
};

#define MAX_OBJECTS_NUMBER 1000
Handle* handles[MAX_OBJECTS_NUMBER];

#ifndef HEAP_SIZE
#define HEAP_SIZE               0x00100000 // 1 Mb
#endif // HEAP_SIZE

HANDLE GlobalHeap;


//**********************************************************************
// internal functions

typedef std::map <void*, DWORD> MapAddresses;

MapAddresses theMapAddresses;
CompatLock_Fast MapAddressesLock;
CompatLock_Fast Handles;

int theLockFD;

#if !defined (_NO_PRAGUE_)  
void initApplicationName ()
{
  memset ( ApplicationName, 0, sizeof ( ApplicationName ) );
#if defined (__linux__)
  std::stringstream aStream;
  aStream << "/proc/" << getpid () << "/exe";
  if ( readlink ( aStream.str ().c_str(), ApplicationName, sizeof ( ApplicationName ) ) == -1 ) 
    exit ( -1 );
#elif defined (__FreeBSD__) 
  std::stringstream aStream;
  aStream << "/proc/" << getpid () << "/file";
  if ( readlink ( aStream.str ().c_str(), ApplicationName, sizeof ( ApplicationName ) ) == -1 ) 
    exit ( -1 );
#elif defined (__OpenBSD__) || defined (__APPLE__)
#warning "Implementation is needed!" 
  return;
#else
#error "Implementation is needed!"  
#endif  
}
#endif

void lockFile ( int aFD, bool aWriteLock, bool isNeeded )
{
  if ( !isNeeded ) 
    return;
  struct flock aLock;
  if ( aWriteLock )
    aLock.l_type = F_WRLCK;
  else
    aLock.l_type = F_RDLCK;
  aLock.l_whence = SEEK_SET;
  aLock.l_start = 0;
  aLock.l_len = 0;
  int aResult = fcntl ( aFD, F_SETLKW, &aLock );
  (void)aResult;
  return;
}

bool tryLockFile ( int aFD, bool aWriteLock, bool isNeeded  )
{
  if ( !isNeeded )
    return true;
  struct flock aLock;
  if ( aWriteLock )
    aLock.l_type = F_WRLCK;
  else
    aLock.l_type = F_RDLCK;
  aLock.l_whence = SEEK_SET;
  aLock.l_start = 0;
  aLock.l_len = 0;
  if ( fcntl ( aFD, F_SETLK, &aLock ) == -1 )
    return false;
  return true;
}

void unlockFile ( int aFD, bool isNeeded )
{
  if ( !isNeeded )
    return;
  struct  flock aLock;
  aLock.l_type = F_UNLCK;
  aLock.l_whence = SEEK_SET;
  aLock.l_start = 0;
  aLock.l_len = 0;
  int aResult = fcntl ( aFD, F_SETLK, &aLock );
  (void)aResult;
  return;
}


void initialization () 
{   
#if defined (COMPAT_DEBUG)
  mtrace ();
#endif // COMPAT_DEBUG
  theLockFD = -1;
  lastErrorKey = TlsAlloc ();
#if !defined (_NO_PRAGUE_)  
  initApplicationName ();
#endif // _NO_PRAGUE_
  COMPAT_INIT_LOCK_FAST ( &MapAddressesLock );
  COMPAT_INIT_LOCK_FAST ( &Handles );
  GlobalHeap = HeapCreate ( 0, HEAP_SIZE, HEAP_SIZE );
  memset ( handles, 0, sizeof ( handles ) ); 
  theStartSeconds = 0;
  struct timeval aTimeVal;
  memset ( &aTimeVal, 0, sizeof ( aTimeVal ) );
  if ( !gettimeofday ( &aTimeVal, 0 ) )
    theStartSeconds = aTimeVal.tv_sec;
  char* aTempDir = getenv ( "TMP" );    
  if ( aTempDir && access ( aTempDir, R_OK|W_OK) == 0 )
    TempDirectory = string ( aTempDir ) + "/";
  else {
    char* aTempDir = getenv ( "TEMP" );    
    if ( aTempDir && access ( aTempDir, R_OK|W_OK) == 0 )
      TempDirectory = string ( aTempDir ) + "/";
    else
      TempDirectory= "/tmp/";
  }
}
  

void closeHandle ( int i )
{
  if ( !handles [ i ] )
    return;
  switch ( handles [ i ]->theTag ) {
  case MUTEX:
    COMPAT_DESTROY_LOCK  ( &handles [ i ]->theMutex.theMutex );
    if ( handles [ i ]->theMutex.theName ) {
      if ( !flock ( handles [ i ]->theMutex.theFDs [ 0 ], LOCK_EX | LOCK_NB ) ) 
        unlink ( handles [ i ]->theMutex.theName );
      free ( handles [ i ]->theMutex.theName);
      flock ( handles [ i ]->theMutex.theFDs [ 0 ], LOCK_UN );
    }
    close ( handles [ i ]->theMutex.theFDs [ 1 ] );
    close ( handles [ i ]->theMutex.theFDs [ 0 ] );
    handles [ i ]->theMutex.theFDs [ 1 ] = handles [ i ]->theMutex.theFDs [ 0 ] = -1;
    break;
  case SEMAPHORE:
    COMPAT_DESTROY_LOCK ( &handles [ i ]->theSemaphore.theMutex );
    if ( handles [ i ]->theSemaphore.theName ) { 
      if ( !flock (  handles [ i ]->theSemaphore.theFDs [ 0 ], LOCK_EX | LOCK_NB ) ) 
        unlink ( handles [ i ]->theSemaphore.theName );
      free ( handles [ i ]->theSemaphore.theName );
      flock ( handles [ i ]->theSemaphore.theFDs [ 0 ], LOCK_UN );
    }
    close ( handles [ i ]->theSemaphore.theFDs [ 1 ] );
    close ( handles [ i ]->theSemaphore.theFDs [ 0 ] );
    handles [ i ]->theSemaphore.theFDs [ 1 ] = handles [ i ]->theSemaphore.theFDs [ 0 ] = -1;
    break;
  case EVENT:
    COMPAT_DESTROY_LOCK ( &handles [ i ]->theEvent.theMutex );
    if ( handles [ i ]->theEvent.theName ) {
      if ( !flock (  handles [ i ]->theEvent.theFDs [ 0 ], LOCK_EX | LOCK_NB ) ) 
        unlink ( handles [ i ]->theEvent.theName );
      free ( handles [ i ]->theEvent.theName);
      flock ( handles [ i ]->theEvent.theFDs [ 0 ], LOCK_UN );
    }
    close ( handles [ i ]->theEvent.theFDs [ 1 ] );
    close ( handles [ i ]->theEvent.theFDs [ 0 ] );
    handles [ i ]->theEvent.theFDs [ 1 ] = handles [ i ]->theEvent.theFDs [ 0 ] = -1;
    break;
  case THREAD:
    close ( handles [ i ]->theThread.theFDs [ 1 ] );
    close ( handles [ i ]->theThread.theFDs [ 0 ] );
    handles [ i ]->theThread.theFDs [ 1 ] = handles [ i ]->theThread.theFDs [ 0 ] = -1;
    COMPAT_DESTROY_LOCK ( &handles [ i ]->theThread.theMutex );
    break;
  case FILES:
    close ( handles [ i ]->theFile.theFD );
    handles [ i ]->theFile.theFD = -1;
    break;
  case FILEMAP:
    if ( handles [ i ]->theFileMap.theName ) {
      if ( !flock (  handles [ i ]->theFileMap.theFD, LOCK_EX | LOCK_NB ) ) 
        Shm_unlink ( handles [ i ]->theFileMap.theName );
      flock (  handles [ i ]->theFileMap.theFD, LOCK_UN );
      free ( handles [ i ]->theFileMap.theName );
    }
    close ( handles [ i ]->theFileMap.theFD );
    handles [ i ]->theFileMap.theFD = -1;
    break;
  default:
    break;
  }
}

void deleteHandle ( int i )
{
  if ( !handles [ i ] )
    return;
  COMPAT_LOCK_FAST ( &Handles );
  //  HeapFree ( GetProcessHeap (), 0, handles [ i ] );
  free ( handles [ i ] );
  handles [ i ] = 0;
  COMPAT_UNLOCK_FAST ( &Handles );  
}

void finalization () 
{ 
  int i = 0;
  for ( i = 0; i < MAX_OBJECTS_NUMBER; i++ ) 
    if ( handles [ i ] ) {
      closeHandle ( i );
      deleteHandle ( i );
    }
  HeapDestroy ( GlobalHeap );
  COMPAT_DESTROY_LOCK_FAST ( &Handles );
  COMPAT_DESTROY_LOCK_FAST ( &MapAddressesLock );
  TlsFree ( lastErrorKey );
#if defined (COMPAT_DEBUG)
  //  muntrace ();
#endif // COMPAT_DEBUG
}

Handle* newHandle ()
{
  //Handle* aHandle = static_cast <Handle*> ( HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof ( Handle ) ) );
  Handle* aHandle = static_cast <Handle*> ( calloc ( sizeof ( Handle ), 1 ) );
  return aHandle;
}

int createHandle ( Tag aTag ) 
{
  int i = 0;
  COMPAT_LOCK_FAST ( &Handles );
  while ( handles [ i ] && ( i < MAX_OBJECTS_NUMBER ) )
    i++;
  if ( i == MAX_OBJECTS_NUMBER ) {
    COMPAT_UNLOCK_FAST ( &Handles ); 
    return -1;
  }
  handles [ i ] = newHandle ();
  if ( !handles [ i ] ){
    COMPAT_UNLOCK_FAST ( &Handles ); 
    return -1;
  }
  handles [ i ]->theCount = 1;
  handles [ i ]->theTag = aTag;
  COMPAT_UNLOCK_FAST ( &Handles ); 
  return i;
}

int getIndex ( HANDLE _Handle )
{
  Handle** aHandle = static_cast <Handle**> ( _Handle );
  int i = ( aHandle - handles );
  if ( ( i < 0 ) || 
       ( i > MAX_OBJECTS_NUMBER ) ||
       ( !handles [ i ] ) ) 
    return -1;
  return i;
}

int getIndex ( HANDLE _Handle, Tag aTag  )
{
  int i = getIndex ( _Handle );
  if ( i == -1 )
    return i;
  if ( handles [ i ]->theTag != aTag )
    return -1;
  return i;
}

string BaseName ( const char* aName )
{
  if ( !aName )
    return 0;
  char* aBaseName = strrchr ( aName, '/' );
  if ( aBaseName )
    if ( aBaseName == aName )
      return "/";
    else
      return aBaseName + 1;
  else
    return aName;
}

string DirName ( const char* aName )
{
  if ( !aName )
    return 0;
  char* aBaseName = strrchr ( aName, '/' );
  if ( aBaseName )
    if ( aBaseName == aName )
      return "/";
    else
      return string ( aName, aBaseName - aName );
  else
    return ".";
}

int findHandleByName ( const char* aName, Tag aTag )
{
  int i = 0;
  char aFileName [ PATH_MAX ] = {0};
  strcat ( aFileName, TempDirectory.c_str () );
  strcat ( aFileName, IPCPrefix.c_str () );
  int aLen = strlen ( aFileName );
  strncpy ( aFileName + aLen, BaseName ( aName ).c_str (), sizeof ( aFileName ) - aLen - 1 );
  aLen = strlen ( aFileName );
  switch ( aTag ) {
  case MUTEX:
    strncpy ( aFileName + aLen, MutexSuffix.c_str(), sizeof ( aFileName ) - aLen - 1 );  
    break;
  case SEMAPHORE:
    strncpy ( aFileName + aLen, SemaphoreSuffix.c_str(), sizeof ( aFileName ) - aLen - 1 );  
    break;
  case EVENT:
    strncpy ( aFileName + aLen, EventSuffix.c_str(), sizeof ( aFileName ) - aLen - 1 );  
    break;
  default:
    break;
  }
  COMPAT_LOCK_FAST ( &Handles );
  while ( i < MAX_OBJECTS_NUMBER ) {
    if ( handles [ i ] && ( aTag == handles [ i ]->theTag ) )
      switch ( aTag ) {
      case MUTEX:
        if ( handles [ i ]->theMutex.theName && !strcmp ( aFileName, handles [ i ]->theMutex.theName ) ) {
          COMPAT_UNLOCK_FAST ( &Handles );
          return i;
        }
        break;
      case SEMAPHORE:
        if ( handles [ i ]->theSemaphore.theName && !strcmp ( aFileName, handles [ i ]->theSemaphore.theName ) ) {
          COMPAT_UNLOCK_FAST ( &Handles );
          return i;
        }
        break;
      case EVENT:
        if ( handles [ i ]->theEvent.theName && !strcmp ( aFileName, handles [ i ]->theEvent.theName ) ) {
          COMPAT_UNLOCK_FAST ( &Handles );
          return i;
        }
        break;
      default:
        break;
      }
    i++;
  }
  COMPAT_UNLOCK_FAST ( &Handles );
  return -1;
}

void setError ()
{
  int lastError = 0;
  
  switch ( errno ) {
  case EPERM:      /* Operation not permitted */
    lastError = ERROR_ACCESS_DENIED;
    break;
  case ENOENT:      /* No such file or directory */
    lastError = ERROR_FILE_NOT_FOUND;
    break;
  case ESRCH:      /* No such process */
    break;
  case EINTR:      /* Interrupted system call */
    break;
  case EIO:      /* I/O error */
    break;
  case ENXIO:      /* No such device or address */
    break;
  case E2BIG:      /* Arg list too long */
    break;
  case ENOEXEC:      /* Exec format error */
    break;
  case EBADF:      /* Bad file number */
    break;
  case ECHILD:      /* No child processes */
    break;
  case EAGAIN:      /* Try again */
    break;
  case ENOMEM:      /* Out of memory */
    lastError =  ERROR_NOT_ENOUGH_MEMORY;
    break;
  case EACCES:      /* Permission denied */
    lastError = ERROR_ACCESS_DENIED;
    break;
  case EFAULT:      /* Bad address */
    break;
  case ENOTBLK:      /* Block device required */
    break;
  case EBUSY:      /* Device or resource busy */
    break;
  case EEXIST:      /* File exists */
    lastError = ERROR_FILE_EXISTS;
    break;
  case EXDEV:      /* Cross-device link */
    break;
  case ENODEV:      /* No such device */
    break;
  case ENOTDIR:      /* Not a directory */
    break;
  case EISDIR:      /* Is a directory */
    break;
  case EINVAL:      /* Invalid argument */
    break;
  case ENFILE:      /* File table overflow */
    lastError = ERROR_TOO_MANY_OPEN_FILES;
    break;
  case EMFILE:      /* Too many open files */
    lastError = ERROR_TOO_MANY_OPEN_FILES;
    break;
  case ENOTTY:      /* Not a typewriter */
    break;
  case ETXTBSY:      /* Text file busy */
    lastError = ERROR_ACCESS_DENIED;
    break;
  case EFBIG:      /* File too large */
    break;
  case ENOSPC:      /* No space left on device */
    lastError = ERROR_DISK_FULL;
    break;
  case ESPIPE:      /* Illegal seek */
    break;
  case EROFS:      /* Read-only file system */
    lastError = ERROR_ACCESS_DENIED;
    break;
  case EMLINK:      /* Too many links */
    break;
  case EPIPE:      /* Broken pipe */
    break;
  case EDOM:      /* Math argument out of domain of func */
    break;
  case ERANGE:      /* Math result not representable */
    break;
  case EDEADLK:      /* Resource deadlock would occur */
    break;
  case ENAMETOOLONG:      /* File name too long */
    break;
  case ENOLCK:      /* No record locks available */
    break;
  case ENOSYS:      /* Function not implemented */
    break;
  case ENOTEMPTY:      /* Directory not empty */
    break;
  case ELOOP:      /* Too many symbolic links encountered */
    break;
  default:
    break;
  }
  TlsSetValue ( lastErrorKey,  reinterpret_cast <void*> ( lastError ) );
}

void setError ( int anError )
{
  TlsSetValue ( lastErrorKey,  reinterpret_cast <void*> ( anError ) );
}



bool createSyncPair ( int* aFDs, const char* aSyncName, const char* aSuffix, char** aName )
{
  if ( !aSyncName ) {
    int error = socketpair ( PF_UNIX, SOCK_STREAM, 0, aFDs );
    if ( error ) {
      setError ();
      return false;
    }
    int aFDFlags = fcntl ( aFDs [ 0 ], F_GETFL );
    fcntl ( aFDs [ 0 ], F_SETFL, aFDFlags | O_NONBLOCK );
    return true;
  }
  char aFileName [ PATH_MAX ] = {0};
  strcat ( aFileName, TempDirectory.c_str () );
  strcat ( aFileName, IPCPrefix.c_str () );
  int aLen = strlen ( aFileName );
  strncpy ( aFileName + aLen, BaseName ( aSyncName ).c_str (), sizeof ( aFileName ) - aLen - 1 );
  aLen = strlen ( aFileName );
  if ( aSuffix )
    strncpy ( aFileName + aLen, aSuffix, sizeof ( aFileName ) - aLen - 1 );    
  if ( aName ) {
    *aName = (char*) malloc ( strlen ( aFileName ) + 1 );
    if ( !*aName ) {
      setError ( ERROR_NOT_ENOUGH_MEMORY );
      return false;
    }
    strcpy ( *aName, aFileName );
  }
  int error = mkfifo ( aFileName, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP );
  if ( error && ( errno != EEXIST ) ) {
    setError ();
    return false;
  }
  if ( error && ( errno == EEXIST ) ) 
    setError ( ERROR_ALREADY_EXISTS );
  aFDs [ 0 ] = open ( aFileName, O_RDONLY | O_NONBLOCK );
  if ( aFDs [ 0 ] == -1 ) {
    setError ();
    return false;
  } 
  aFDs [ 1 ] = open ( aFileName, O_WRONLY );
  if ( aFDs [ 1 ] == -1 ) {
    setError ();
    close ( aFDs [ 1 ] );
    aFDs [ 1 ] = -1;
    return false;
  } 
  if ( ( GetLastError () == ERROR_ALREADY_EXISTS ) && !flock ( aFDs [ 0 ], LOCK_EX | LOCK_NB ) ) {
    char tmp [ 100 ];
    int aResult = read ( aFDs [ 0 ],  tmp, sizeof ( tmp ) );    
    (void) aResult;
    setError ( NO_ERROR );
  }
  flock ( aFDs [ 0 ], LOCK_SH | LOCK_NB );
  return true;
}

bool connectSyncPair ( int* aFDs, const char* aSyncName, const char* aSuffix, char** aName ) 
{
  char aFileName [ PATH_MAX ] = {0};
  strcat ( aFileName, TempDirectory.c_str () );
  strcat ( aFileName, IPCPrefix.c_str () );
  int aLen = strlen ( aFileName );
  strncpy ( aFileName + aLen, aSyncName, sizeof ( aFileName ) - aLen - 1 );
  if ( aSuffix )
    strncpy ( aFileName + strlen ( aFileName ), aSuffix, sizeof ( aFileName ) - strlen ( aFileName )  - 1 );    
  aFDs [ 0 ] = open ( aFileName, O_RDONLY | O_NONBLOCK );
  if ( aFDs [ 0 ] == -1 ) {
     setError ();
     return false;
  } 
  aFDs [ 1 ] = open ( aFileName, O_WRONLY );
  if ( aFDs [ 1 ] == -1 ) {
     setError ();
     return false;
  } 
  if ( !flock ( aFDs [ 0 ], LOCK_EX | LOCK_NB ) ) {
    char tmp [ 100 ];
    int aResult = read ( aFDs [ 0 ],  tmp, sizeof ( tmp ) );    
    (void) aResult;
  }
  flock ( aFDs [ 0 ], LOCK_SH | LOCK_NB );
  if ( aName ) {
    *aName = (char*) malloc ( strlen ( aFileName ) + 1 );
    if ( !*aName ) {
      setError ( ERROR_NOT_ENOUGH_MEMORY );
      return false;
    }
    strcpy ( *aName, aFileName );
  }
  return true;
}


// initialization helper

class CompatInitHelper {
public:
  CompatInitHelper ();
  ~CompatInitHelper (); 

private:
  static int IsInitialized;
};

static CompatInitHelper CompatHelper;


CompatInitHelper::CompatInitHelper ()
{
  if ( !IsInitialized ) {
    initialization ();
  }
  ++IsInitialized;
}

CompatInitHelper::~CompatInitHelper () 
{
  --IsInitialized;
  if ( !IsInitialized ) {
     finalization ();
   }
};

int CompatInitHelper::IsInitialized = 0;

//**********************************************************************
// critical section functions

#if defined (THREADS)
void InitializeCriticalSection ( LPCRITICAL_SECTION lpCriticalSection  ) 
{
  CALL_ENTRY

  pthread_mutexattr_t attr; 
  pthread_mutexattr_init (&attr);
  pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE); 
  pthread_mutex_init ( lpCriticalSection, &attr); 
}

void DeleteCriticalSection ( LPCRITICAL_SECTION lpCriticalSection ) 
{
  CALL_ENTRY
  pthread_mutex_destroy ( lpCriticalSection );
}

void EnterCriticalSection ( LPCRITICAL_SECTION lpCriticalSection  ) 
{
  CALL_ENTRY
  pthread_mutex_lock ( lpCriticalSection );
}

BOOL TryEnterCriticalSection ( LPCRITICAL_SECTION lpCriticalSection  ) 
{
  CALL_ENTRY
  if ( pthread_mutex_trylock ( lpCriticalSection ) == EBUSY )
    return FALSE;
  return TRUE;
}

void LeaveCriticalSection ( LPCRITICAL_SECTION lpCriticalSection )
{
  CALL_ENTRY
  pthread_mutex_unlock ( lpCriticalSection );
}
#endif // THREADS
//**********************************************************************
// handle manipulation functions

BOOL CloseHandle ( HANDLE hObject )
{
  CALL_ENTRY
  int i = getIndex ( hObject );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  (handles [ i ]->theCount)--;
  if ( !handles [ i ]->theCount ) {
    closeHandle ( i );
    deleteHandle ( i );
  }
  return TRUE;
}

BOOL DuplicateHandle ( HANDLE hSourceProcessHandle,
                       HANDLE hSourceHandle,
                       HANDLE hTargetProcessHandle,
                       LPHANDLE lpTargetHandle,
                       DWORD dwDesiredAccess,
                       BOOL bInheritHandle,
                       DWORD dwOptions )
{
  CALL_ENTRY
  NOT_IMPLEMENTED
  return 0;
}

BOOL GetHandleInformation ( HANDLE hObject, LPDWORD lpdwFlags )
{
  CALL_ENTRY
  int i = getIndex ( hObject );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
   //  NOT_IMPLEMENTED
    
  return TRUE;
}

BOOL SetHandleInformation ( HANDLE hObject, 
                            DWORD dwMask, 
                            DWORD dwFlags )
{
  CALL_ENTRY
  NOT_IMPLEMENTED
  return 0;
}

//**********************************************************************
// Mutex functions  

HANDLE CreateMutex( void* lpMutexAttributes,
                    BOOL bInitialOwner,
                    const char* lpName )
{
  CALL_ENTRY
  int i = -1;
  if ( lpName )  
    i = findHandleByName ( lpName, MUTEX );
  if ( i != -1 ) {
    handles [ i ]->theCount++;
    setError ( ERROR_ALREADY_EXISTS );    
    return &handles [ i ];
  }
  else
    i = createHandle ( MUTEX );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !createSyncPair (  handles [ i ]->theMutex.theFDs, lpName, MutexSuffix.c_str () , &handles [ i ]->theMutex.theName ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  if ( !GetLastError () ) {
    COMPAT_INIT_LOCK ( &handles [ i ]->theMutex.theMutex );
    if ( !bInitialOwner ) {
      COMPAT_LOCK ( &handles [ i ]->theMutex.theMutex );    
      lockFile ( handles [ i ]->theMutex.theFDs [ 1 ], true, handles [ i ]->theMutex.theName != 0 );
      char tmp = 1;
      int aResult = write ( handles [ i ]->theMutex.theFDs [ 1 ],  &tmp, sizeof ( tmp ) );
      (void) aResult;
      unlockFile ( handles [ i ]->theMutex.theFDs [ 1 ], handles [ i ]->theMutex.theName != 0 );
      COMPAT_UNLOCK ( &handles [ i ]->theMutex.theMutex );
    }
#if defined (THREADS)
    else
      handles [ i ]->theMutex.theOwner = pthread_self ();
#endif // THREADS
  }
  return &handles [ i ];
}

HANDLE OpenMutex ( DWORD dwDesiredAccess,
                   BOOL bInheritHandle,
                   const char* lpName )
{
  CALL_ENTRY  
  int i = -1;
  if ( lpName )  
    i = findHandleByName ( lpName, MUTEX );
  if ( i != -1 ) {
    handles [ i ]->theCount++;
    return &handles [ i ];
  }
  else
    i = createHandle ( MUTEX );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !connectSyncPair ( handles [ i ]->theMutex.theFDs, lpName, MutexSuffix.c_str (), &handles [ i ]->theMutex.theName ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  
  return &handles [ i ];
}

BOOL ReleaseMutex ( HANDLE hMutex )
{
  CALL_ENTRY
  int i = getIndex ( hMutex, MUTEX );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  COMPAT_LOCK (  &handles [ i ]->theMutex.theMutex );
  lockFile ( handles [ i ]->theMutex.theFDs [ 1 ], true, handles [ i ]->theMutex.theName != 0  );
  char tmp = 1;
  int aResult = write ( handles [ i ]->theMutex.theFDs [ 1 ],  &tmp, sizeof ( tmp ) );
  (void) aResult;
#if defined (THREADS)  
  handles [ i ]->theMutex.theOwner = 0;
#endif // THREADS  
  unlockFile ( handles [ i ]->theMutex.theFDs [ 1 ], handles [ i ]->theMutex.theName != 0 );
  COMPAT_UNLOCK ( &handles [ i ]->theMutex.theMutex );  
  return TRUE;
}


//**********************************************************************
// event functions

HANDLE CreateEvent( void* lpEventAttributes,
                    BOOL bManualReset,
                    BOOL bInitialState,
                    const char* lpName )
{
  CALL_ENTRY
  int i = -1;
  if ( lpName )  
    i = findHandleByName ( lpName, EVENT );
  if ( i != -1 ) {
    handles [ i ]->theCount++;
    setError ( ERROR_ALREADY_EXISTS );    
    return &handles [ i ];
  }
  else
    i = createHandle ( EVENT );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !createSyncPair (  handles [ i ]->theEvent.theFDs, lpName, EventSuffix.c_str (), &handles [ i ]->theEvent.theName ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  if ( !GetLastError () ) { 
    COMPAT_INIT_LOCK ( &handles [ i ]->theEvent.theMutex ); 
    int result;
    if ( bInitialState ) {
      COMPAT_LOCK ( &handles [ i ]->theEvent.theMutex );     
      lockFile ( handles [ i ]->theEvent.theFDs [ 1 ], true, handles [ i ]->theEvent.theName != 0 );
      char tmp = 1;
      result = write ( handles [ i ]->theEvent.theFDs [ 1 ],  &tmp, sizeof ( tmp ) );
      unlockFile ( handles [ i ]->theEvent.theFDs [ 1 ], handles [ i ]->theEvent.theName != 0 );
      COMPAT_UNLOCK ( &handles [ i ]->theEvent.theMutex ); 
    }
  }
  handles [ i ]->theEvent.theManualReset = bManualReset;
  return &handles [ i ];
}

HANDLE OpenEvent ( DWORD dwDesiredAccess, 
                   BOOL bInheritHandle, 
                   const char* lpName )
{
  CALL_ENTRY
  int i = -1;
  if ( lpName )  
    i = findHandleByName ( lpName, EVENT );
  if ( i != -1 ) {
    handles [ i ]->theCount++;
    return &handles [ i ];
  }
  else
    i = createHandle ( EVENT );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !connectSyncPair ( handles [ i ]->theEvent.theFDs, lpName, EventSuffix.c_str (), &handles [ i ]->theEvent.theName ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  return &handles [ i ];
}

BOOL ResetEvent( HANDLE hEvent )
{
  CALL_ENTRY
  int i = getIndex ( hEvent, EVENT );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  COMPAT_LOCK (  &handles [ i ]->theEvent.theMutex );
  char tmp [ 100 ];
  lockFile ( handles [ i ]->theEvent.theFDs [ 0 ], false, handles [ i ]->theEvent.theName != 0 );
  int result = read ( handles [ i ]->theEvent.theFDs [ 0 ],  tmp, sizeof ( tmp ) );
  unlockFile ( handles [ i ]->theEvent.theFDs [ 0 ], handles [ i ]->theEvent.theName != 0 );
  COMPAT_UNLOCK ( &handles [ i ]->theEvent.theMutex );  
  return ( result == sizeof ( tmp ) ) ? TRUE : FALSE;
}

BOOL SetEvent ( HANDLE hEvent )
{
  CALL_ENTRY
  int i = getIndex ( hEvent, EVENT );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  COMPAT_LOCK (  &handles [ i ]->theEvent.theMutex );
  lockFile ( handles [ i ]->theEvent.theFDs [ 1 ], true, handles [ i ]->theEvent.theName != 0 );
  char tmp = 1;
  int result = write ( handles [ i ]->theEvent.theFDs [ 1 ],  &tmp, sizeof ( tmp ) );
  unlockFile ( handles [ i ]->theEvent.theFDs [ 1 ], handles [ i ]->theEvent.theName != 0 );
  COMPAT_UNLOCK ( &handles [ i ]->theEvent.theMutex );  
  return ( result == sizeof ( tmp ) ) ? TRUE : FALSE;
}

BOOL PulseEvent ( HANDLE hEvent )
{
  CALL_ENTRY
  return SetEvent ( hEvent ) && ResetEvent ( hEvent );
}

//**********************************************************************
// semaphores manupulation functions

HANDLE CreateSemaphore ( void* lpSemaphoreAttributes,
                         LONG lInitialCount,
                         LONG lMaximumCount,
                         const char* lpName )
{
  CALL_ENTRY 
  int i = -1;
  if ( lpName )  
    i = findHandleByName ( lpName, SEMAPHORE );
  if ( i != -1 ) {
    handles [ i ]->theCount++;
    setError ( ERROR_ALREADY_EXISTS );    
    return &handles [ i ];
  }
  else
    i = createHandle ( SEMAPHORE );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !createSyncPair (  handles [ i ]->theSemaphore.theFDs, lpName, SemaphoreSuffix.c_str (), &handles [ i ]->theSemaphore.theName ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  handles [ i ]->theSemaphore.theMaxCount = lMaximumCount;
  if ( !GetLastError () ) {  
    COMPAT_INIT_LOCK ( &handles [ i ]->theSemaphore.theMutex ); 
    if ( lInitialCount > 0 ) {
      COMPAT_LOCK ( &handles [ i ]->theSemaphore.theMutex ); 
      lockFile ( handles [ i ]->theSemaphore.theFDs [ 1 ], true, handles [ i ]->theSemaphore.theName != 0 );
      int aResult = write ( handles [ i ]->theSemaphore.theFDs [ 1 ],  &lInitialCount, sizeof ( lInitialCount ) );
      (void) aResult;
      unlockFile ( handles [ i ]->theSemaphore.theFDs [ 1 ], handles [ i ]->theSemaphore.theName != 0 );
      COMPAT_UNLOCK ( &handles [ i ]->theSemaphore.theMutex ); 
    }
  }
  return &handles [ i ];
}

HANDLE OpenSemaphore ( DWORD dwDesiredAccess,
                       BOOL bInheritHandle,
                       const char* lpName )
{
  CALL_ENTRY
  int i = -1;
  if ( lpName )  
    i = findHandleByName ( lpName, SEMAPHORE );
  if ( i != -1 ) {
    handles [ i ]->theCount++;
    return &handles [ i ];
  }
  else
    i = createHandle ( SEMAPHORE );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !connectSyncPair ( handles [ i ]->theSemaphore.theFDs, lpName, SemaphoreSuffix.c_str (), &handles [ i ]->theSemaphore.theName  ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  return &handles [ i ];
}

BOOL ReleaseSemaphore ( HANDLE hSemaphore,
                        LONG lReleaseCount,
                        LONG* lpPreviousCount )
{
  CALL_ENTRY
  int i = getIndex ( hSemaphore, SEMAPHORE );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  BOOL result = FALSE;
  COMPAT_LOCK (  &handles [ i ]->theSemaphore.theMutex );
  lockFile ( handles [ i ]->theSemaphore.theFDs [ 0 ], false, handles [ i ]->theSemaphore.theName != 0 );
  LONG aCount = 0;
  int aResult = read ( handles [ i ]->theSemaphore.theFDs [ 0 ],  &aCount, sizeof ( aCount ));  
  if ( lpPreviousCount) 
    *lpPreviousCount = aCount;
  if ( aCount < handles [ i ]->theSemaphore.theMaxCount )
    ++aCount;
  aResult = write ( handles [ i ]->theSemaphore.theFDs [ 1 ],  &aCount, sizeof ( aCount ));
  unlockFile ( handles [ i ]->theSemaphore.theFDs [ 0 ], handles [ i ]->theSemaphore.theName != 0 );
  COMPAT_UNLOCK ( &handles [ i ]->theSemaphore.theMutex );  
  return result;
}

//**********************************************************************
// waiting functions

DWORD WaitForAllObjects( DWORD nCount,
                         HANDLE *lpHandles,
                         DWORD dwMilliseconds )
{
  CALL_ENTRY
  timeval aTimeOut;
  memset ( &aTimeOut, 0, sizeof ( timeval ) );
  aTimeOut.tv_sec = dwMilliseconds / 1000;
  aTimeOut.tv_usec = ( dwMilliseconds % 1000 ) * 1000;
  
  int anUpperBound = 0;
  fd_set aReadFDs;
  FD_ZERO ( &aReadFDs );
  for ( unsigned int j = 0; j < nCount; j++ ) {  
    int i = getIndex ( lpHandles [ j ] );
    if ( i == -1 ) {
      setError ( ERROR_INVALID_HANDLE );
      TRACE ( "%s Invalid tag!\n", __FUNCTION__ );
      return WAIT_FAILED;
    }
    int aFD;
    switch ( handles [ i ]->theTag ) {
    case MUTEX:
      aFD = handles [ i ]->theMutex.theFDs [ 0 ] ;
      break;
    case EVENT:
      aFD = handles [ i ]->theEvent.theFDs [ 0 ] ;
      break;
    case SEMAPHORE:
      aFD = handles [ i ]->theSemaphore.theFDs [ 0 ] ;
      break;
    case THREAD:
      aFD = handles [ i ]->theThread.theFDs [ 0 ] ;
      break;
    default:
      TRACE ( "%s Invalid tag!\n", __FUNCTION__ );
      return WAIT_FAILED;
    }
    FD_SET ( aFD , &aReadFDs );
    anUpperBound = ( anUpperBound > aFD ) ? anUpperBound : aFD;
  }
  unsigned int aCurrentCount = nCount;
  int aCurrentUpperBound = anUpperBound;
  fd_set aTestFDs = aReadFDs;
  while (true){
    fd_set aLastTestFDs = aTestFDs;
    TRACE ( "%s\n", __FUNCTION__ );
    int result  = select ( aCurrentUpperBound + 1, &aTestFDs, 0 , 0, ( dwMilliseconds == INFINITE) ? 0 : &aTimeOut );
    if ( !result  )
      return WAIT_TIMEOUT;
    if ( result == -1  ) 
      if ( errno == EINTR)
        continue;
      else {
        setError ();
//        char anError [256] = {0};
//        TRACE ( "Unexpected error! %s \n", strerror_r ( errno, anError, sizeof ( anError ) ) );
        TRACE ( "Unexpected error! %s \n", strerror ( errno ));
        return WAIT_FAILED;
      }
    if ( static_cast <unsigned int> ( result ) != aCurrentCount ) {
      aCurrentUpperBound = 0;
      fd_set aTmpFDs;
      FD_ZERO ( &aTmpFDs );
      int aCount = 0;
      for ( unsigned int j = 0; j < nCount; j++ ) {  
        int i = getIndex ( lpHandles [ j ] );
        if ( i == -1 ) {
          setError ( ERROR_INVALID_HANDLE );
          TRACE ( "%s Invalid handle!\n", __FUNCTION__ );
          return WAIT_FAILED;
        }
        int aFD;
        switch ( handles [ i ]->theTag ) {
        case MUTEX:
          aFD = handles [ i ]->theMutex.theFDs [ 0 ] ;
          break;
        case EVENT:
          aFD = handles [ i ]->theEvent.theFDs [ 0 ] ;
          break;
        case SEMAPHORE:
          aFD = handles [ i ]->theSemaphore.theFDs [ 0 ] ;
          break;
        case THREAD:
          aFD = handles [ i ]->theThread.theFDs [ 0 ] ;
          break;
        default:
          TRACE ( "%s Invalid handle!\n", __FUNCTION__ );
          return WAIT_FAILED;
        }
        if ( !FD_ISSET ( aFD, &aTestFDs ) && FD_ISSET ( aFD, &aLastTestFDs ) ) {
          FD_SET ( aFD , &aTmpFDs );
          ++aCount; 
          aCurrentUpperBound = ( aCurrentUpperBound > aFD ) ? aCurrentUpperBound : aFD;
        }
      }
      if ( aCount ) {
        aTestFDs = aTmpFDs;
        aCurrentCount = aCount;
      }
      else {
        aTestFDs = aReadFDs;
        aCurrentUpperBound = anUpperBound;
        aCurrentCount = nCount;
      }
#if defined (THREADS)
      sched_yield ();
#endif // THREADS  
      continue;
    }
    else
      if ( aCurrentCount != nCount  ) {
        aTestFDs = aReadFDs;
        aCurrentUpperBound = anUpperBound;
        aCurrentCount = nCount;
#if defined (THREADS)
        sched_yield ();
#endif // THREADS  
        continue;
      }
    int anIndex = 0;
    // tring to enter into all critical section 
    for ( unsigned int j = 0; j < nCount; j++ ) {  
      int i = getIndex ( lpHandles [ j ] );
      if ( i == -1 ) {
        setError ( ERROR_INVALID_HANDLE );
        TRACE ( "%s Invalid handle!\n", __FUNCTION__ );
        return WAIT_FAILED;
      }
      switch ( handles [ i ]->theTag ) {
      case MUTEX:
        TRACE ( "%s Mutex ", __FUNCTION__ );
        if ( !COMPAT_TRYLOCK ( &handles [ i ]->theMutex.theMutex ) || 
             !tryLockFile ( handles [ i ]->theMutex.theFDs [ 0 ], false, handles [ i ]->theMutex.theName != 0 ) ) 
          anIndex = j;
        break;
      case EVENT:
        TRACE ( "%s Event ", __FUNCTION__ );
        if ( !COMPAT_TRYLOCK ( &handles [ i ]->theEvent.theMutex ) || 
             !tryLockFile ( handles [ i ]->theEvent.theFDs [ 0 ], false, handles [ i ]->theEvent.theName != 0  ))
          anIndex = j;
        break;
      case SEMAPHORE:
        TRACE ( "%s Semaphore ", __FUNCTION__);
        if ( !COMPAT_TRYLOCK ( &handles [ i ]->theSemaphore.theMutex ) || 
             !tryLockFile ( handles [ i ]->theSemaphore.theFDs [ 0 ], false, handles [ i ]->theSemaphore.theName != 0  ))
          anIndex = j;
        break;
      case THREAD:
        TRACE ( "%s Thread ", __FUNCTION__ );
        if ( !COMPAT_TRYLOCK ( &handles [ i ]->theThread.theMutex ) )
          anIndex = j;
        break;
      default:
        break;
      }
      if ( anIndex ) {
        TRACE ( "wasn't locked\n" );
        break;
      }
      TRACE ( "was locked," );
    }
    result = 0;
    char tmp [100];
    LONG aCount = 0;
    // reading data
    if ( !anIndex ) {
      for ( unsigned int j = 0; j < nCount; j++ ) {  
        int i = getIndex ( lpHandles [ j ] );
        if ( i == -1 ) {
          setError ( ERROR_INVALID_HANDLE );
          TRACE ( "Invalid handle!\n" );
          return WAIT_FAILED;
        }
        switch ( handles [ i ]->theTag ) {
        case MUTEX:
          result = read ( handles [ i ]->theMutex.theFDs [ 0 ],  tmp, sizeof ( tmp ) );
#if defined (THREADS)
          handles [ i ]->theMutex.theOwner = pthread_self ();
#endif // THREADS
          break;
        case EVENT:
          result = read ( handles [ i ]->theEvent.theFDs [ 0 ],  tmp, sizeof ( tmp ) );
          if ( ( result > 0) && handles [ i ]->theEvent.theManualReset ) 
            write ( handles [ i ]->theEvent.theFDs [ 1 ],  tmp, result );
          break;
        case SEMAPHORE:
          result = read ( handles [ i ]->theSemaphore.theFDs [ 0 ] ,  &aCount, sizeof ( aCount ) );
          if ( ( result > 0) && ( aCount > 1 ) ){
            --aCount;
            write ( handles [ i ]->theSemaphore.theFDs [ 1 ] ,  &aCount, sizeof ( aCount ) );
          }
          break;
        case THREAD:
          result = read ( handles [ i ]->theThread.theFDs [ 0 ],  tmp, sizeof ( tmp ) );
#if defined (THREADS)
          pthread_join ( handles [ i ]->theThread.theThreadID, 0 );
#endif // THREADS
          break;
        default:
          break;
        }
        if ( result <= 0 ) {
          TRACE ( "wasn't read ");
          if ( result == -1 ) {
//            char anError [256] = {0};
//            TRACE ( "due an error %s, ",  strerror_r ( errno, anError, sizeof ( anError ) ) );
            TRACE ( "due an error %s, ",  strerror ( errno ));
          }
          break;
        }
        TRACE ( "was read ");
      }
    }
    // exit from all entered critical section
    for ( unsigned int j = 0; j < ( ( anIndex ) ? anIndex : nCount ); j++ ) {  
      int i = getIndex ( lpHandles [ j ] );
      if ( i == -1 ) {
        setError ( ERROR_INVALID_HANDLE );
        return WAIT_FAILED;
      }
      switch ( handles [ i ]->theTag ) {
      case MUTEX:
        unlockFile ( handles [ i ]->theMutex.theFDs [ 0 ], handles [ i ]->theMutex.theName != 0  );
        COMPAT_UNLOCK ( &handles [ i ]->theMutex.theMutex );
        break;
      case EVENT:
        unlockFile ( handles [ i ]->theEvent.theFDs [ 0 ], handles [ i ]->theEvent.theName != 0 );
        COMPAT_UNLOCK ( &handles [ i ]->theEvent.theMutex );
        break;
      case SEMAPHORE:
        unlockFile ( handles [ i ]->theSemaphore.theFDs [ 0 ], handles [ i ]->theSemaphore.theName != 0 );
        COMPAT_UNLOCK ( &handles [ i ]->theSemaphore.theMutex );
        break;
      case THREAD:
        COMPAT_UNLOCK ( &handles [ i ]->theThread.theMutex );
        break;
      default:
        break;
      }
      TRACE ( "was unlocked\n");
    }
    if ( !anIndex && result )
      return WAIT_OBJECT_0;        
#if defined (THREADS)
    sched_yield ();
#endif // THREADS  
  }
}


DWORD WaitForAnyObjects( DWORD nCount,
                         HANDLE *lpHandles,
                         DWORD dwMilliseconds )
{
  CALL_ENTRY
  timeval aTimeOut;
  memset ( &aTimeOut, 0, sizeof ( timeval ) );
  aTimeOut.tv_sec = dwMilliseconds / 1000;
  aTimeOut.tv_usec = ( dwMilliseconds % 1000 ) * 1000;

  int anUpperBound = 0;
  fd_set aReadFDs;
  FD_ZERO ( &aReadFDs );
  for ( unsigned int j = 0; j < nCount; j++ ) {  
    int i = getIndex ( lpHandles [ j ] );
    if ( i == -1 ) {
      setError ( ERROR_INVALID_HANDLE );
      return WAIT_FAILED;
    }
    int aFD;
    switch ( handles [ i ]->theTag ) {
    case MUTEX:
#if defined (THREADS)
      if ( handles [ i ]->theMutex.theOwner == pthread_self () )
        return WAIT_OBJECT_0 + j;
#endif // THREADS
      aFD = handles [ i ]->theMutex.theFDs [ 0 ] ;
      break;
    case EVENT:
      aFD = handles [ i ]->theEvent.theFDs [ 0 ] ;
      break;
    case SEMAPHORE:
      aFD = handles [ i ]->theSemaphore.theFDs [ 0 ] ;
      break;
    case THREAD:
      aFD = handles [ i ]->theThread.theFDs [ 0 ] ;
      break;
    default:
      TRACE ( "%s Invalid handle!\n", __FUNCTION__ );
      return WAIT_FAILED;
    }
    FD_SET ( aFD , &aReadFDs );
    anUpperBound = ( anUpperBound > aFD ) ? anUpperBound : aFD;
  }
  
  while (true){
    fd_set aTestFDs = aReadFDs;
    int result  = select ( anUpperBound + 1, &aTestFDs, 0 , 0, ( dwMilliseconds == INFINITE) ? 0 : &aTimeOut );
    if ( !result  )
      return WAIT_TIMEOUT;
    if ( result == -1  ) 
      if ( errno == EINTR )
        continue;
      else {
        setError ();
        return WAIT_FAILED;
      }
    result = 0;
    char tmp [100]={0};
    LONG aCount = 0;
    for ( unsigned int j = 0; j < nCount; j++ ) {  
      int i = getIndex ( lpHandles [ j ] );
      if ( i == -1 ) {
        setError ( ERROR_INVALID_HANDLE );
        return WAIT_FAILED;
      }
      int aFD;
      switch ( handles [ i ]->theTag ) {
      case MUTEX:
        aFD = handles [ i ]->theMutex.theFDs [ 0 ] ;
        break;
      case EVENT:
        aFD = handles [ i ]->theEvent.theFDs [ 0 ] ;
        break;
      case SEMAPHORE:
        aFD = handles [ i ]->theSemaphore.theFDs [ 0 ] ;
        break;
      case THREAD:
        aFD = handles [ i ]->theThread.theFDs [ 0 ] ;
        break;
      default:
        TRACE ( "%s Invalid handle!\n", __FUNCTION__ );
        return WAIT_FAILED;
      }
      if ( FD_ISSET ( aFD,  &aTestFDs ) ) {
        switch ( handles [ i ]->theTag ) {
        case MUTEX:
          TRACE ( "%s Mutex ", __FUNCTION__ );
          if ( !COMPAT_TRYLOCK ( &handles [ i ]->theMutex.theMutex ) ) {
            TRACE ( "wasn't locked\n" );
            break;
          }
          if ( !tryLockFile ( aFD, false, handles [ i ]->theMutex.theName != 0 ) ) {
            COMPAT_UNLOCK ( &handles [ i ]->theMutex.theMutex );
            TRACE ( "wasn't locked\n" );
            break;
          }
          TRACE ( "was locked," );
          result = read ( aFD,  tmp, sizeof ( tmp ) );
#if defined (THREADS)
          handles [ i ]->theMutex.theOwner = pthread_self ();
#endif // THREADS
          unlockFile ( aFD, handles [ i ]->theMutex.theName != 0 );
          COMPAT_UNLOCK ( &handles [ i ]->theMutex.theMutex );
          TRACE ( "was unlocked," );
          break;
        case EVENT:
          TRACE ( "%s Event ", __FUNCTION__ );
          if ( !COMPAT_TRYLOCK ( &handles [ i ]->theEvent.theMutex ) ) {
            TRACE ( "wasn't locked\n" );
            break;
          }
          if ( !tryLockFile ( aFD, false, handles [ i ]->theEvent.theName != 0 ) ) {
            COMPAT_UNLOCK ( &handles [ i ]->theEvent.theMutex );
            TRACE ( "wasn't locked\n" );
            break;
          }
          TRACE ( "was locked," );
          result = read ( aFD,  tmp, sizeof ( tmp ) );
          if ( ( result > 0 ) && handles [ i ]->theEvent.theManualReset ) 
            write ( handles [ i ]->theEvent.theFDs [ 1 ],  tmp, result );
          unlockFile ( aFD, handles [ i ]->theEvent.theName != 0 );
          COMPAT_UNLOCK ( &handles [ i ]->theEvent.theMutex );
          TRACE ( "was unlocked," );
          break;
        case SEMAPHORE:
          TRACE ( "%s Semaphore ", __FUNCTION__ );
          if ( !COMPAT_TRYLOCK ( &handles [ i ]->theSemaphore.theMutex ) ) {
            TRACE ( "wasn't locked\n" );
            break;
          }
          if ( !tryLockFile ( aFD, false, handles [ i ]->theSemaphore.theName != 0 ) ) {
            COMPAT_UNLOCK ( &handles [ i ]->theSemaphore.theMutex );
            TRACE ( "wasn't locked\n" );
            break;
          }
          TRACE ( "was locked," );
          result = read ( handles [ i ]->theSemaphore.theFDs [ 0 ] ,  &aCount, sizeof ( aCount ) );
          if ( ( result > 0 ) && ( aCount > 1 ) ) {
            --aCount;
            write ( handles [ i ]->theSemaphore.theFDs [ 1 ] ,  &aCount, sizeof ( aCount ) );
          }
          unlockFile ( aFD, handles [ i ]->theSemaphore.theName != 0 );
          COMPAT_UNLOCK ( &handles [ i ]->theSemaphore.theMutex );
          TRACE ( "was unlocked," );
          break;
        case THREAD:
          TRACE ( "%s Thread ", __FUNCTION__ );
          if ( !COMPAT_TRYLOCK ( &handles [ i ]->theThread.theMutex ) ) {
            TRACE ( "wasn't locked\n" );
            break;
          }
          TRACE ( "was locked," );
          result = read ( aFD,  tmp, sizeof ( tmp ) );
          COMPAT_UNLOCK ( &handles [ i ]->theThread.theMutex );
          TRACE ( "was unlocked," );
          break;
        default:
          break;
        }
        if ( result > 0 ) {
          TRACE ( "was read\n");
          return WAIT_OBJECT_0 + j;
        }
        else {
          TRACE ( "wasn't read");
          if ( result == -1 ) {
//            char anError [256] = {0};
//            TRACE ( "due an error %s, ",  strerror_r ( errno, anError, sizeof ( anError ) ) );
            TRACE ( "due an error %s\n", strerror ( errno ) );
          }
	  else
            TRACE ( "\n" );
#if defined (THREADS)
          sched_yield ();
#endif // THREADS  
        }
      } 
    }
  }
}

DWORD WaitForSingleObject ( HANDLE hHandle,
                            DWORD dwMilliseconds )
{
  return WaitForAnyObjects ( 1, &hHandle, dwMilliseconds );
}

DWORD WaitForMultipleObjects( DWORD nCount,
                              HANDLE *lpHandles,
                              BOOL bWaitAll,
                              DWORD dwMilliseconds )
{
  if ( !nCount )
    return WAIT_OBJECT_0;
  if ( bWaitAll )
    return WaitForAllObjects ( nCount,
                               lpHandles,
                               dwMilliseconds );
  else
    return WaitForAnyObjects ( nCount,
                               lpHandles,
                               dwMilliseconds );

}

//**********************************************************************
// thread functions

#if defined (THREADS)

typedef void* (*threadProc) ( void* );
void* threadProcedure ( Thread* aThreadContext )
{
  TRACE ( "start %s %d\n", __FUNCTION__,  pthread_self () );
  aThreadContext->theExitStatus = aThreadContext->theStartAddress ( aThreadContext->theParameter );
  char tmp = 1;
  //  COMPAT_LOCK ( &aThreadContext->theMutex );
  write ( aThreadContext->theFDs [ 1 ],  &tmp, sizeof ( tmp ) );
  //  COMPAT_UNLOCK ( &aThreadContext->theMutex );
  TRACE ( "stop %s %d\n", __FUNCTION__,  pthread_self () );
  return 0;
}

HANDLE GetCurrentThread ( VOID )
{
  CALL_ENTRY
  NOT_IMPLEMENTED
  return 0;
}

DWORD GetCurrentThreadId( VOID )
{
  CALL_ENTRY
#if defined (__linux__) || defined (__sun)   
  return pthread_self ();
#elif defined (__FreeBSD__) || defined (__OpenBSD__) || (__APPLE__)
  return reinterpret_cast <DWORD> ( pthread_self () );
#else
#error "Implementation is needed"
#endif
}

HANDLE CreateThread ( void* lpThreadAttributes,
                      DWORD dwStackSize,
                      LPTHREAD_START_ROUTINE lpStartAddress,
                      LPVOID lpParameter,
                      DWORD dwCreationFlags,
                      LPDWORD lpThreadId )
{
  CALL_ENTRY
  int i = createHandle ( THREAD );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  if ( !createSyncPair (  handles [ i ]->theThread.theFDs, 0, 0, 0 ) ) {
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  COMPAT_INIT_LOCK ( &handles [ i ]->theThread.theMutex );
  handles [ i ]->theThread.theStartAddress = lpStartAddress;
  handles [ i ]->theThread.theParameter = lpParameter;
  if ( pthread_create (  &handles [ i ]->theThread.theThreadID, 0, reinterpret_cast<threadProc>(threadProcedure), &handles [ i ]->theThread ) ) {
    closeHandle ( i );
    deleteHandle ( i );
    return INVALID_HANDLE_VALUE;
  }
  if ( lpThreadId ) 
#if defined (__linux__) || defined (__sun)      
    *lpThreadId = handles [ i ]->theThread.theThreadID;
#elif defined (__FreeBSD__) || defined (__OpenBSD__) || (__APPLE__)
    *lpThreadId  = reinterpret_cast <DWORD> ( handles [ i ]->theThread.theThreadID );
#else
#error "Implementation is needed"
#endif
  return &handles [ i ];
}

#endif // THREADS

//**********************************************************************
// files manipulation finctions

struct qword {
  #if defined (MACHINE_IS_BIG_ENDIAN)
	  DWORD Hi;
	  DWORD Low;	
  #else	
	  DWORD Low;
	  DWORD Hi;
  #endif
}__attribute__((packed));

union Offset2QWord {
  off_t Offset;
  qword QWord;
};

HANDLE CreateFile ( const char* lpFileName,
                    DWORD dwDesiredAccess,
                    DWORD dwShareMode,
                    void* lpSecurityAttributes,
                    DWORD dwCreationDisposition,
                    DWORD dwFlagsAndAttributes,
                    HANDLE hTemplateFile )
{
  CALL_ENTRY
  if ( hTemplateFile ) {
    setError ( ERROR_NOT_SUPPORTED );
    return INVALID_HANDLE_VALUE;
  }
  struct stat aStat;
  if ( !stat ( lpFileName, &aStat ) && S_ISDIR ( aStat.st_mode ) && !( dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS ) ) {
    setError ();
    return INVALID_HANDLE_VALUE;
  }
  int i = createHandle ( FILES );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  int flags = 0;
  switch ( dwDesiredAccess ) {
  case GENERIC_READ: 
    flags = O_RDONLY;
    break;
  case GENERIC_WRITE: 
    flags = O_WRONLY;
    break;
  case (GENERIC_READ | GENERIC_WRITE):
    flags = O_RDWR;
    break;
  default:
    break;
  }
  switch (dwCreationDisposition ) {
  case CREATE_NEW:
    flags = flags | O_CREAT | O_EXCL;
    break;
  case CREATE_ALWAYS:
    flags = flags | O_CREAT | O_TRUNC;
    break;
  case OPEN_EXISTING:
    break;
  case OPEN_ALWAYS:
    flags = flags | O_CREAT;
    break;
  case TRUNCATE_EXISTING:
    flags = flags | O_TRUNC;
    break;
  default:
    break;
  }
  mode_t mode = 0;
  if (dwFlagsAndAttributes & FILE_ATTRIBUTE_READONLY )
    mode = S_IRUSR | S_IRGRP;
  else 
    mode = S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP;
  bool anAlreadyExists = !access ( lpFileName, F_OK );
  handles [ i ]->theFile.theFD = open ( lpFileName, flags, ( flags & O_CREAT ) ? mode : 0 );
  if ( handles [ i ]->theFile.theFD != -1 ) {
    if ( anAlreadyExists && ( ( dwCreationDisposition & CREATE_ALWAYS) || (dwCreationDisposition &  OPEN_ALWAYS ) ) )
      setError (ERROR_ALREADY_EXISTS);
    return &handles [ i ];
  }
  deleteHandle ( i );
  setError ();
  return INVALID_HANDLE_VALUE;
}

BOOL ReadFile ( HANDLE hFile,
                LPVOID lpBuffer,
                DWORD nNumberOfBytesToRead,
                LPDWORD lpNumberOfBytesRead,
                LPOVERLAPPED lpOverlapped )
{
  CALL_ENTRY
  if ( lpOverlapped ) {
    setError ( ERROR_NOT_SUPPORTED );
    return FALSE;
  }
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  int result = 0;
  result = read ( handles [ i ]->theFile.theFD, lpBuffer, nNumberOfBytesToRead );
  if ( result != -1 ) {
    *lpNumberOfBytesRead = result;
    return TRUE;
  }
  setError ();
  return FALSE;
}

DWORD SetFilePointer ( HANDLE hFile,
                       LONG lDistanceToMove,
                       PLONG lpDistanceToMoveHigh,
                       DWORD dwMoveMethod )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return INVALID_FILE_SIZE;
  }
  int whence = 0;
  switch ( dwMoveMethod ) {
  case FILE_BEGIN: 
    whence = SEEK_SET;
    break;
  case FILE_CURRENT:
    whence = SEEK_CUR;
    break;
  case FILE_END:
    whence = SEEK_END;
    break;
  default:
    return INVALID_FILE_SIZE;
  }
  Offset2QWord offset;
  memset ( &offset, 0, sizeof ( offset ) );
  offset.QWord.Low = lDistanceToMove;
  if ( lpDistanceToMoveHigh )
    offset.QWord.Hi = *lpDistanceToMoveHigh;
  Offset2QWord result;
  memset ( &result, 0, sizeof ( result ) );
  result.Offset = lseek( handles [ i ]->theFile.theFD, offset.Offset, whence);
  if ( result.Offset == static_cast <off_t> ( -1 ) ) {
    setError ();
    return INVALID_FILE_SIZE;
  }
  if ( lpDistanceToMoveHigh && result.QWord.Hi )
    *lpDistanceToMoveHigh = result.QWord.Hi;
  return result.QWord.Low;
}

BOOL WriteFile ( HANDLE hFile,
                 LPCVOID lpBuffer,
                 DWORD nNumberOfBytesToWrite,
                 LPDWORD lpNumberOfBytesWritten,
                 LPOVERLAPPED lpOverlapped )
{
  CALL_ENTRY
  if ( lpOverlapped ) {
    setError ( ERROR_NOT_SUPPORTED );
    return FALSE;
  }
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  int result = 0;
  result = write ( handles [ i ]->theFile.theFD, lpBuffer, nNumberOfBytesToWrite );
  if ( result != -1 ) {
    if ( lpNumberOfBytesWritten )
      *lpNumberOfBytesWritten = result;
    return TRUE;
  }
  setError ();
  return FALSE;
}

DWORD GetFileSize ( HANDLE hFile,
                    LPDWORD lpFileSizeHigh )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  struct stat aStat;
  if ( fstat ( handles [ i ]->theFile.theFD, &aStat ) ) {
    setError ();
    return FALSE;
  }
  Offset2QWord result;
  memset ( &result, 0, sizeof ( result ) );
  result.Offset = aStat.st_size;
  if ( lpFileSizeHigh && result.QWord.Hi )
    *lpFileSizeHigh = result.QWord.Hi;
  return result.QWord.Low;
}

BOOL SetEndOfFile ( HANDLE hFile )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  off_t offset = lseek ( handles [ i ]->theFile.theFD, 0, SEEK_CUR );
  if ( ftruncate (  handles [ i ]->theFile.theFD, offset ) ) {
    setError ();
    return FALSE;
  }
  return TRUE;
}


BOOL GetFileTime ( HANDLE hFile,
                   LPFILETIME lpCreationTime,
                   LPFILETIME lpLastAccessTime,
                   LPFILETIME lpLastWriteTime )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  struct stat aStat;
  if ( fstat ( handles [ i ]->theFile.theFD, &aStat ) ) {
    setError ();
    return FALSE;
  }
  long long tmp = aStat.st_ctime * 10000;
  memcpy ( lpCreationTime, &tmp, sizeof ( FILETIME ) );
  tmp = aStat.st_atime  * 10000;
  memcpy ( lpLastAccessTime, &tmp, sizeof ( FILETIME ) );
  tmp = aStat.st_mtime  * 10000;
  memcpy ( lpLastWriteTime, &tmp, sizeof ( FILETIME ) );
  return TRUE;
}


BOOL SetFileTime ( HANDLE hFile,
                   const FILETIME *lpCreationTime,
                   const FILETIME *lpLastAccessTime,
                   const FILETIME *lpLastWriteTime )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  //  NOT_IMPLEMENTED
  return TRUE;
}


BOOL GetFileInformationByHandle ( HANDLE hFile,
                                  LPBY_HANDLE_FILE_INFORMATION lpFileInformation )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  struct stat aStat;
  if ( fstat ( handles [ i ]->theFileMap.theFD, &aStat ) ) {
    setError ();
    return FALSE;
  }
  lpFileInformation->dwFileAttributes = 0;
  long long tmp = aStat.st_ctime * 10000;
  memcpy ( &lpFileInformation->ftCreationTime, &tmp, sizeof ( FILETIME ) );
  tmp = aStat.st_atime  * 10000;
  memcpy ( &lpFileInformation->ftLastAccessTime, &tmp, sizeof ( FILETIME ) );
  tmp = aStat.st_mtime  * 10000;
  memcpy ( &lpFileInformation->ftLastWriteTime, &tmp, sizeof ( FILETIME ) );
  lpFileInformation->dwVolumeSerialNumber = 0;
  Offset2QWord aSize;
  memset ( &aSize, 0, sizeof ( aSize ) );
  aSize.Offset = aStat.st_size;
  lpFileInformation->nFileSizeHigh = aSize.QWord.Hi;
  lpFileInformation->nFileSizeLow = aSize.QWord.Low;
  lpFileInformation->nNumberOfLinks = aStat.st_nlink;
  lpFileInformation->nFileIndexHigh = 0;
  lpFileInformation->nFileIndexLow = i;
  return TRUE;
}

BOOL DeleteFile ( const LPCSTR lpFileName )
{
  CALL_ENTRY
  if ( remove ( lpFileName ) ) {
    setError ();
    return FALSE;
  }
  return TRUE;
}


BOOL CopyFile ( LPCSTR lpExistingFileName,
                LPCSTR lpNewFileName,
                BOOL bFailIfExists )
{
  CALL_ENTRY
  int anInputFile = open ( lpExistingFileName, O_RDONLY );  
  if (  anInputFile == -1 ) {
    setError ();
    return FALSE;
  }
  struct stat aStat;
  if ( fstat ( anInputFile, &aStat ) ) {
    setError ();
    close ( anInputFile );
    return FALSE;
  }
  int flags = O_WRONLY | O_CREAT | O_TRUNC;
  if ( bFailIfExists ) 
    flags = flags | O_EXCL;
  int anOutputFile = open ( lpNewFileName,  flags, aStat.st_mode );
  if (  anOutputFile == -1 ) {
    setError ();
    close ( anInputFile );
    return FALSE;
  }
  char aBuffer [4096];
  int count = 0;
  do { 
    count = read ( anInputFile, aBuffer, sizeof ( aBuffer ) );
    if ( count > 0 )
      count = write ( anOutputFile, aBuffer, count );
  }
  while ( count > 0 );
  if ( count ) {
    setError ();
    close ( anInputFile );
    close ( anOutputFile );
    unlink ( lpNewFileName );
    return FALSE;
  }
  close ( anInputFile );
  close ( anOutputFile );
  return TRUE;
}


BOOL MoveFile ( LPCSTR lpExistingFileName,
                LPCSTR lpNewFileName )
{
  CALL_ENTRY
  if ( rename ( lpExistingFileName, lpNewFileName ) ) {
    setError ();
    return FALSE;
  }
  return TRUE;
}

// functins to find files

struct Directory {
  DIR* theDir;
  dirent theEntry;
  char theDirectory [PATH_MAX];
  char Pattern [PATH_MAX];
};

BOOL find_file ( Directory* pDir,
                 LPWIN32_FIND_DATA lpFindFileData )
{
  memset ( lpFindFileData, 0, sizeof ( WIN32_FIND_DATA ) );
  for (;;) {
    dirent* pDirEntry = 0;
    readdir_r ( pDir->theDir, &pDir->theEntry, &pDirEntry );
    if ( !pDirEntry ) {
      setError ( ERROR_NO_MORE_FILES );
      return FALSE;
    }
    if ( !fnmatch ( pDir->Pattern, pDirEntry->d_name, 0 ) ) {
      strcpy ( lpFindFileData->cFileName, pDirEntry->d_name );
      char aBuffer [PATH_MAX] = {0};
      strcat ( aBuffer,  pDir->theDirectory );
      strcat ( aBuffer,  "/" );
      strcat ( aBuffer, lpFindFileData->cFileName );
      struct stat aStat;
      if ( stat ( aBuffer, &aStat ) ) {
        setError ();
        return TRUE;//???????????????????????????????????
      }
      lpFindFileData->dwFileAttributes = 0;
      lpFindFileData->dwFileAttributes |= ( aStat.st_mode & S_IFDIR ) ? FILE_ATTRIBUTE_DIRECTORY : 0;
      lpFindFileData->dwFileAttributes |= ( ( aStat.st_mode & S_IWUSR ) ||
                                              ( aStat.st_mode & S_IWGRP ) ||
                                              ( aStat.st_mode & S_IWOTH ) ) ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_READONLY;
      if ( ( lpFindFileData->cFileName [ 0 ] == '.' ) && !( lpFindFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
        lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
      long long tmp = aStat.st_ctime * 10000;
      memcpy ( &lpFindFileData->ftCreationTime, &tmp, sizeof ( FILETIME ) );
      tmp = aStat.st_atime  * 10000;
      memcpy ( &lpFindFileData->ftLastAccessTime, &tmp, sizeof ( FILETIME ) );
      tmp = aStat.st_mtime  * 10000;
      memcpy ( &lpFindFileData->ftLastWriteTime, &tmp, sizeof ( FILETIME ) );  
      Offset2QWord aSize;
      memset ( &aSize, 0, sizeof ( aSize ) );
      aSize.Offset = aStat.st_size;
      lpFindFileData->nFileSizeLow = aSize.QWord.Low;
      lpFindFileData->nFileSizeHigh = aSize.QWord.Hi;
      return TRUE;
    }
  }
}


HANDLE FindFirstFile ( const char* lpFileName,
                       LPWIN32_FIND_DATA lpFindFileData )
{
  CALL_ENTRY
  char aBuffer [PATH_MAX] = {0};
  strncpy ( aBuffer, lpFileName, sizeof ( aBuffer ) - 1 );
  const char* aPtr = DirName ( aBuffer ).c_str ();
  DIR* tmp = opendir ( aPtr );
  if ( !tmp ) {
    setError ();
    return INVALID_HANDLE_VALUE;
  }
  //Directory* aDir = static_cast <Directory*> ( HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof ( Directory ) ) );
  Directory* aDir = static_cast <Directory*> ( calloc ( sizeof ( Directory ), 1 ) );
  if ( !aDir ) {
    closedir ( tmp );
    setError ();
    return INVALID_HANDLE_VALUE;
  }
  aDir->theDir = tmp;
  strncpy ( aDir->theDirectory, aPtr, sizeof ( aDir->theDirectory ) );
  strncpy ( aDir->Pattern, BaseName ( lpFileName ).c_str (), sizeof ( aDir->Pattern ) - 1 );
  if ( find_file ( aDir, lpFindFileData ) ) 
    return aDir;
  closedir ( tmp );
  //HeapFree ( GetProcessHeap (), 0, aDir );
  free ( aDir );
  setError ( ERROR_FILE_NOT_FOUND );
  return INVALID_HANDLE_VALUE;
}

BOOL FindNextFile ( HANDLE hFindFile,
                    LPWIN32_FIND_DATA lpFindFileData )
{
  CALL_ENTRY
  return find_file ( static_cast <Directory*> ( hFindFile ), lpFindFileData );
}

BOOL FindClose ( HANDLE hFindFile )
{
  CALL_ENTRY
  Directory* aDir = static_cast <Directory*> ( hFindFile );
  closedir ( aDir->theDir );
  //HeapFree ( GetProcessHeap (), 0, hFindFile );  
  free ( hFindFile );  
  return TRUE;
}

BOOL SystemTimeToFileTime ( const SYSTEMTIME *lpSystemTime,
                            LPFILETIME lpFileTime )
{
  CALL_ENTRY
  NOT_IMPLEMENTED
  return 0;
}

BOOL FileTimeToSystemTime ( const FILETIME *lpFileTime,
                            LPSYSTEMTIME lpSystemTime )
{
  CALL_ENTRY
  long long tmp;
  memcpy ( &tmp, lpFileTime, sizeof ( FILETIME ) );
  time_t aTime_t = tmp / 10000;
  tm aTm;
  if ( !gmtime_r ( &aTime_t, &aTm ) )
    return FALSE;
  lpSystemTime->wYear = aTm.tm_year + 1900;
  lpSystemTime->wMonth = aTm.tm_mon;
  lpSystemTime->wDayOfWeek = aTm.tm_wday;
  lpSystemTime->wDay = aTm.tm_mday;
  lpSystemTime->wHour = aTm.tm_hour;
  lpSystemTime->wMinute = aTm.tm_min;
  lpSystemTime->wSecond = aTm.tm_sec;
  lpSystemTime->wMilliseconds = 0;
  return TRUE;
}

HANDLE CreateFileMapping ( HANDLE hFile,
                           void* lpFileMappingAttributes,
                           DWORD flProtect,
                           DWORD dwMaximumSizeHigh,
                           DWORD dwMaximumSizeLow,
                           LPCSTR lpName )
{
  CALL_ENTRY
  int aFileIndex = -1;
  if ( hFile != INVALID_HANDLE_VALUE ) {
    aFileIndex = getIndex ( hFile, FILES );
    if ( aFileIndex == -1 ) {
      setError ( ERROR_INVALID_HANDLE );
      return 0;
    }
  }
  int i = createHandle ( FILEMAP );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return 0;
  }
  Offset2QWord aMaxSize;
  memset ( &aMaxSize, 0, sizeof ( aMaxSize ) );
  aMaxSize.QWord.Hi = dwMaximumSizeHigh;
  aMaxSize.QWord.Low = dwMaximumSizeLow;
  handles [ i ]->theFileMap.theFD = -1;
  if ( ( hFile == INVALID_HANDLE_VALUE ) && lpName ) {
    string aName = lpName;
#if defined (__FreeBSD__) && defined (USE_TMPFS)
    aName = TempDirectory + aName;
#endif
    handles [ i ]->theFileMap.theName = static_cast<char*> ( malloc ( aName.size () + 1 ) );
    if ( !handles [ i ]->theFileMap.theName ) {
      setError ( ERROR_NOT_ENOUGH_MEMORY );
      deleteHandle ( i );
      return 0;
    }
    memset ( handles [ i ]->theFileMap.theName, 0, aName.size () + 1 );
    strncpy ( handles [ i ]->theFileMap.theName, aName.c_str (), aName.size () );
    int flags = O_CREAT;
    if ( flProtect & PAGE_READONLY ) 
      flags |= O_RDONLY;
    if ( flProtect & PAGE_READWRITE ) 
      flags |= O_RDWR;
    int mode = S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP;
    while ( true ) {
      handles [ i ]->theFileMap.theFD = Shm_open ( aName.c_str (), flags | O_EXCL, mode );
      if ( ( handles [ i ]->theFileMap.theFD == -1 ) && ( errno == EEXIST ) ) {
        setError ( ERROR_ALREADY_EXISTS );
        handles [ i ]->theFileMap.theFD = Shm_open ( aName.c_str (), flags, mode ); 
      }
      //     else
      //       handles [ i ]->theFileMap.isOwner = true;
      if ( handles [ i ]->theFileMap.theFD == -1 ) {
        setError ();
        free (  handles [ i ]->theFileMap.theName );
        deleteHandle ( i );
        return 0;
      }
      if ( ( GetLastError () == ERROR_ALREADY_EXISTS ) && !flock ( handles [ i ]->theFileMap.theFD, LOCK_EX | LOCK_NB ) ) {
        Shm_unlink ( handles [ i ]->theFileMap.theName );
        flock ( handles [ i ]->theFileMap.theFD, LOCK_UN );
        setError ( NO_ERROR );
        continue;
      }  
      
      if ( GetLastError () != ERROR_ALREADY_EXISTS ) {
        if ( ftruncate ( handles [ i ]->theFileMap.theFD, aMaxSize.Offset ) ) {
          setError ( ERROR_DISK_FULL );
          Shm_unlink ( handles [ i ]->theFileMap.theName );
          flock ( handles [ i ]->theFileMap.theFD, LOCK_UN );
          free (  handles [ i ]->theFileMap.theName );
          deleteHandle ( i );
          return 0;
        }
      }
      flock ( handles [ i ]->theFileMap.theFD, LOCK_SH | LOCK_NB );
      return &handles [ i ];
    }
  }
//   if ( hFile == INVALID_HANDLE_VALUE ) {
//     handles [ i ]->theFileMap.theFD = open ( AnonymousSharedMemory.c_str (), O_RDWR );
//     if ( handles [ i ]->theFileMap.theFD == -1 ) {
//       setError ();
//       deleteHandle ( i );
//       return 0;
//     }
//   }
//   else
  handles [ i ]->theFileMap.theFD = dup ( handles [ aFileIndex ]->theFile.theFD );
  struct stat aStat;
  if ( fstat ( handles [ i ]->theFileMap.theFD, &aStat ) ) {
    setError ();
    deleteHandle ( i );
    return 0;
  }
  if ( aMaxSize.Offset > aStat.st_size ) {
    if ( ( flProtect & PAGE_READONLY ) ||
         ( flProtect & PAGE_WRITECOPY ) ) {
      setError ( INVALID_FILE_SIZE );
      deleteHandle ( i );
      return 0;
    }
    unsigned char tmp = 0x0;
    off_t aCurrentOffset = lseek ( handles [ i ]->theFileMap.theFD, 0, SEEK_CUR );
    lseek ( handles [ i ]->theFileMap.theFD, aMaxSize.Offset - 1, SEEK_SET );
    if ( write (  handles [ i ]->theFileMap.theFD, &tmp, sizeof ( tmp ) ) != sizeof ( tmp ) ) {
      setError ();
      lseek ( handles [ i ]->theFileMap.theFD, aCurrentOffset, SEEK_SET );
      ftruncate (  handles [ i ]->theFileMap.theFD, aStat.st_size );
      deleteHandle ( i );
      return 0;
    }
    fsync ( handles [ i ]->theFileMap.theFD );
    lseek ( handles [ i ]->theFileMap.theFD, aCurrentOffset, SEEK_SET );
  }
  return &handles [ i ];
}

HANDLE OpenFileMapping ( DWORD dwDesiredAccess,
                         BOOL bInheritHandle,
                         LPCSTR lpName )
{
  CALL_ENTRY
  int i = createHandle ( FILEMAP );
  if ( i == -1 ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return 0;
  }
  string aName = lpName;
#if defined (__FreeBSD__) && defined (USE_TMPFS)
  aName = TempDirectory + aName;
#endif
  handles [ i ]->theFileMap.theName = static_cast<char*> ( malloc ( aName.size () + 1 ) );
  if ( !handles [ i ]->theFileMap.theName ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    deleteHandle ( i );
    return 0;
  }
  memset ( handles [ i ]->theFileMap.theName, 0, aName.size () + 1 );
  strncpy ( handles [ i ]->theFileMap.theName, aName.c_str (), aName.size () );
  int flags = 0;
  switch ( dwDesiredAccess ){
  case FILE_MAP_ALL_ACCESS:
  case FILE_MAP_WRITE:
    flags |= O_RDWR;
    break;
  case FILE_MAP_READ:
    flags |= O_RDONLY;
    break;
  default:
    free (  handles [ i ]->theFileMap.theName );
    deleteHandle ( i );
    return 0;
  }
  handles [ i ]->theFileMap.theFD = Shm_open ( lpName, flags, 0 );
  if ( handles [ i ]->theFileMap.theFD == -1 ) {
    setError ();
    free (  handles [ i ]->theFileMap.theName );
    deleteHandle ( i );
    return 0;
  }
  flock ( handles [ i ]->theFileMap.theFD, LOCK_SH | LOCK_NB );
  return &handles [ i ];
}

LPVOID MapViewOfFile ( HANDLE hFileMappingObject,
                       DWORD dwDesiredAccess,
                       DWORD dwFileOffsetHigh,
                       DWORD dwFileOffsetLow,
                       DWORD dwNumberOfBytesToMap )
{
  CALL_ENTRY
  int i = getIndex ( hFileMappingObject, FILEMAP );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return 0;
  }
  Offset2QWord aFileOffset;
  memset ( &aFileOffset, 0, sizeof ( aFileOffset ) );
  aFileOffset.QWord.Hi = dwFileOffsetHigh;
  aFileOffset.QWord.Low = dwFileOffsetLow;
  int flags = 0;
  int prot = 0;
  switch ( dwDesiredAccess ){
  case FILE_MAP_ALL_ACCESS:
  case FILE_MAP_WRITE:
    prot = PROT_WRITE | PROT_READ;
    flags = MAP_SHARED;
    break;
  case FILE_MAP_READ:
    prot = PROT_READ;
    flags = MAP_SHARED;
    break;
  case FILE_MAP_COPY:
    prot = PROT_WRITE | PROT_READ;
    flags = MAP_PRIVATE;
    break;
  default:
    return 0;
  }
  if ( handles [ i ]->theFileMap.theFD == -1 )
    flags |= MAP_ANONYMOUS;
  int aPageSize = getpagesize ();
  off_t anOffset = ( aFileOffset.Offset % aPageSize ) ? ( aFileOffset.Offset / aPageSize + 1 ) * aPageSize : aFileOffset.Offset; 
  if ( !dwNumberOfBytesToMap ) {
    struct stat aStat;
    if ( fstat ( handles [ i ]->theFileMap.theFD, &aStat ) ) {
      setError ();
      return 0;
    }
    dwNumberOfBytesToMap = aStat.st_size;
  }
  off_t aNumberOfBytesToMap = dwNumberOfBytesToMap;//( dwNumberOfBytesToMap % aPageSize ) ? 
    //    ( dwNumberOfBytesToMap / aPageSize + 1 ) * aPageSize : dwNumberOfBytesToMap ; 
  void* aRegion = mmap ( 0, aNumberOfBytesToMap, prot, flags, handles [ i ]->theFileMap.theFD, anOffset );
  if ( aRegion == MAP_FAILED ) {
    setError ();
    return 0;
  }
  COMPAT_LOCK_FAST ( &MapAddressesLock );
  theMapAddresses [ aRegion ] = aNumberOfBytesToMap;
  COMPAT_UNLOCK_FAST ( &MapAddressesLock );
  return aRegion;
}

BOOL FlushViewOfFile ( LPVOID lpBaseAddress,
                       DWORD dwNumberOfBytesToFlush )
{
  CALL_ENTRY
  COMPAT_LOCK_FAST ( &MapAddressesLock );
  MapAddresses::iterator anIterator =  theMapAddresses.find ( lpBaseAddress );
  COMPAT_UNLOCK_FAST ( &MapAddressesLock );
  if ( anIterator != theMapAddresses.end () ) 
    if ( msync ( lpBaseAddress, dwNumberOfBytesToFlush ? dwNumberOfBytesToFlush : anIterator->second, MS_SYNC ) ) {  
      setError ();
      return FALSE;
    }
    else
      return TRUE;
  return FALSE;
}

BOOL UnmapViewOfFile ( LPVOID lpBaseAddress  )
{
  CALL_ENTRY
  COMPAT_LOCK_FAST ( &MapAddressesLock );
  MapAddresses::iterator anIterator =  theMapAddresses.find ( lpBaseAddress );
  COMPAT_UNLOCK_FAST ( &MapAddressesLock );
  if ( anIterator != theMapAddresses.end () ) {
    if ( msync ( lpBaseAddress, anIterator->second, MS_SYNC ) ) {
      setError ();
      return FALSE;
    }
    if ( munmap ( lpBaseAddress, anIterator->second ) ) {
      setError ();
      return FALSE;
    }   
    COMPAT_LOCK_FAST ( &MapAddressesLock );
    theMapAddresses.erase ( lpBaseAddress );
    COMPAT_UNLOCK_FAST ( &MapAddressesLock );
    return TRUE;
  }
  return FALSE;
}


UINT GetTempFileName ( LPCSTR lpPathName,
                       LPCSTR lpPrefixString,
                       UINT uUnique,
                       LPSTR lpTempFileName )
{
  CALL_ENTRY
  if ( !lpPathName )  
    return 0;
  int aFD;
  std::stringstream aStream;
  while (true) {
    aStream.clear ();
    aStream << lpPathName << '/' << lpPrefixString << std::hex << getpid();
    aStream.width ( 4 );
    unsigned int aSeed = getpid ();
    aStream << rand_r ( &aSeed );
    if ( ( aFD = open ( aStream.str ().c_str (), O_RDWR | O_CREAT | O_EXCL, S_IRWXU ) ) != -1 )
      break;
    if ( errno == EEXIST || errno == EAGAIN || errno == ETXTBSY )
      continue;
    return FALSE;
  };
  close ( aFD );
  unlink ( aStream.str ().c_str () );

  strcpy ( lpTempFileName,  aStream.str ().c_str () );
  return TRUE;
}


DWORD GetTempPath ( DWORD nBufferLength,
                    LPSTR lpBuffer )
{
  CALL_ENTRY
  memset ( lpBuffer, 0, nBufferLength );
  return strlen ( strncpy ( lpBuffer, TempDirectory.c_str (), nBufferLength ) );
}

BOOL FlushFileBuffers ( HANDLE hFile )
{
  CALL_ENTRY
  int i = getIndex ( hFile, FILES );
  if ( i == -1 ) {
    setError ( ERROR_INVALID_HANDLE );
    return FALSE;
  }
  if ( fsync ( handles [ i ]->theFile.theFD ) ) {
    setError ();
    return FALSE;
  }
  return TRUE;
}


BOOL SetFileAttributes ( LPCSTR lpFileName,
                         DWORD dwFileAttributes )
{
  CALL_ENTRY
  struct stat aStat;
  if ( stat ( lpFileName, &aStat ) ) {
    setError ();
    return FALSE;
  }
  if ( ( aStat.st_mode & S_IWUSR ) && 
       ( dwFileAttributes & FILE_ATTRIBUTE_READONLY ) && 
       chmod ( lpFileName, S_IRUSR ) ) {
    setError ();
    return FALSE;
  }
    
  if ( !( aStat.st_mode & S_IWUSR ) && 
       ( !( dwFileAttributes & FILE_ATTRIBUTE_READONLY ) || 
         ( dwFileAttributes & FILE_ATTRIBUTE_NORMAL ) ) && 
       chmod ( lpFileName, S_IWUSR | S_IRUSR ) ) {
    setError ();
    return FALSE;
  }
  return TRUE;
}


DWORD GetFileAttributes ( LPCSTR lpFileName )
{
  CALL_ENTRY
  struct stat aStat;
  if ( stat ( lpFileName, &aStat ) ) {
    setError ();
    return 0xFFFFFFFF;
  }
  DWORD aResult = 0;
  aResult |= ( aStat.st_mode & S_IFDIR ) ? FILE_ATTRIBUTE_DIRECTORY : 0;
  aResult |= ( aStat.st_mode & S_IWUSR ) ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_READONLY;
  if ( ( lpFileName [ 0 ] == '.' ) && !( aResult & FILE_ATTRIBUTE_DIRECTORY ) )
    aResult |= FILE_ATTRIBUTE_HIDDEN;
  return aResult;
}

BOOL GetFileAttributesEx ( LPCSTR lpFileName,
                           GET_FILEEX_INFO_LEVELS fInfoLevelId,
                           LPVOID lpFileInformation )
{
  CALL_ENTRY
  if ( fInfoLevelId != GetFileExInfoStandard )  
    return FALSE;
  struct stat aStat;
  if ( stat ( lpFileName, &aStat ) ) {
    setError ();
    return FALSE;
  }
  LPWIN32_FILE_ATTRIBUTE_DATA aFileInformation = static_cast <LPWIN32_FILE_ATTRIBUTE_DATA> ( lpFileInformation );
  aFileInformation->dwFileAttributes = 0;
  aFileInformation->dwFileAttributes |= ( aStat.st_mode & S_IFDIR ) ? FILE_ATTRIBUTE_DIRECTORY : 0;
  aFileInformation->dwFileAttributes |= ( ( aStat.st_mode & S_IWUSR ) ||
                                         ( aStat.st_mode & S_IWGRP ) ||
                                         ( aStat.st_mode & S_IWOTH ) ) ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_READONLY;
  if ( lpFileName [ 0 ] == '.' )
    aFileInformation->dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
  long long tmp = aStat.st_ctime * 10000;
  memcpy ( &aFileInformation->ftCreationTime, &tmp, sizeof ( FILETIME ) );
  tmp = aStat.st_atime  * 10000;
  memcpy ( &aFileInformation->ftLastAccessTime, &tmp, sizeof ( FILETIME ) );
  tmp = aStat.st_mtime  * 10000;
  memcpy ( &aFileInformation->ftLastWriteTime, &tmp, sizeof ( FILETIME ) );  
  Offset2QWord aSize;
  memset ( &aSize, 0, sizeof ( aSize ) );
  aSize.Offset = aStat.st_size;
  aFileInformation->nFileSizeLow = aSize.QWord.Low;
  aFileInformation->nFileSizeHigh = aSize.QWord.Hi;
  return TRUE;
}
//**********************************************************************
// heap functions

struct MemoryControlBlock {
  size_t theSizeOfBlock;
  MemoryControlBlock* thePrevBlock;
  MemoryControlBlock* theNextBlock;
}__attribute__((packed));

struct Heap {
  CompatLock_Fast HeapMutex;
  DWORD Flags;
  MemoryControlBlock* theLastAllocatedBlock;
};

HANDLE GetProcessHeap( VOID )
{
  return GlobalHeap;
}

HANDLE HeapCreate ( DWORD flOptions,
                    DWORD dwInitialSize,
                    DWORD dwMaximumSize )
{
  CALL_ENTRY
  // allocate new heap tag
  Heap* aHeap = static_cast <Heap*> ( malloc ( sizeof ( Heap ) ) );
  if ( !aHeap ) {
    setError ( ERROR_NOT_ENOUGH_MEMORY );
    return INVALID_HANDLE_VALUE;
  }
  memset ( aHeap, 0, sizeof ( Heap ) );
  aHeap->Flags = flOptions;
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) ) {
    COMPAT_INIT_LOCK_FAST ( &aHeap->HeapMutex );
  }
  return aHeap;
}

BOOL HeapDestroy ( HANDLE hHeap )
{
  CALL_ENTRY
  Heap* aHeap = static_cast <Heap*> ( hHeap );
  while ( aHeap->theLastAllocatedBlock ) {
    void* aTmp = aHeap->theLastAllocatedBlock;
    aHeap->theLastAllocatedBlock = aHeap->theLastAllocatedBlock->thePrevBlock;
    free ( aTmp );
  }
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) ) {
    COMPAT_DESTROY_LOCK_FAST ( &aHeap->HeapMutex );
  }
  free ( aHeap );
  return TRUE;
}

LPVOID HeapAlloc ( HANDLE hHeap,
                   DWORD dwFlags,
                   DWORD dwBytes )
{
  CALL_ENTRY
    if ( dwBytes > 10000000 ) {
      int i = 0;
      i++;
    }
      
  Heap* aHeap = static_cast<Heap*> ( hHeap );
  MemoryControlBlock* aResult = static_cast <MemoryControlBlock*> ( calloc ( sizeof ( char ), 
                                                                             dwBytes + sizeof ( MemoryControlBlock ) ) );
  if ( !aResult )
    return 0;
  aResult->theSizeOfBlock = dwBytes;
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) && !( dwFlags & HEAP_NO_SERIALIZE ) ) 
    COMPAT_LOCK_FAST ( &aHeap->HeapMutex );
  if ( aHeap->theLastAllocatedBlock ) {
    aResult->thePrevBlock = aHeap->theLastAllocatedBlock;
    aResult->thePrevBlock->theNextBlock = aResult;
  }
  aHeap->theLastAllocatedBlock = aResult;
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) && !( dwFlags & HEAP_NO_SERIALIZE ) ) 
    COMPAT_UNLOCK_FAST ( &aHeap->HeapMutex );
  return reinterpret_cast <char*> ( aResult ) + sizeof ( MemoryControlBlock );
}


BOOL HeapFree ( HANDLE hHeap,
                DWORD dwFlags,
                LPVOID lpMem )
{
  CALL_ENTRY
  Heap* aHeap = static_cast <Heap*> ( hHeap );
  MemoryControlBlock* aMCB = reinterpret_cast <MemoryControlBlock*> ( static_cast <char*> ( lpMem ) - 
                                                                      sizeof ( MemoryControlBlock ) );
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) && !( dwFlags & HEAP_NO_SERIALIZE ) ) 
    COMPAT_LOCK_FAST ( &aHeap->HeapMutex );
  if ( aHeap->theLastAllocatedBlock == aMCB ) 
    aHeap->theLastAllocatedBlock = aMCB->thePrevBlock;
  if ( aMCB->thePrevBlock )
    aMCB->thePrevBlock->theNextBlock = aMCB->theNextBlock;
  if ( aMCB->theNextBlock )
    aMCB->theNextBlock->thePrevBlock = aMCB->thePrevBlock;
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) && !( dwFlags & HEAP_NO_SERIALIZE ) ) 
    COMPAT_UNLOCK_FAST ( &aHeap->HeapMutex );
  //memset ( aMCB, 0, aMCB->theSizeOfBlock + sizeof ( MemoryControlBlock ));
  free ( aMCB );
  return TRUE;
}

LPVOID HeapReAlloc ( HANDLE hHeap,
                     DWORD dwFlags,
                     LPVOID lpMem,
                     DWORD dwBytes )
{
  CALL_ENTRY
  Heap* aHeap = static_cast <Heap*> ( hHeap );
  MemoryControlBlock* aMCB = reinterpret_cast <MemoryControlBlock*> ( static_cast <char*> ( lpMem ) - 
                                                                      sizeof ( MemoryControlBlock ) );
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) && !( dwFlags & HEAP_NO_SERIALIZE ) ) 
    COMPAT_LOCK_FAST ( &aHeap->HeapMutex );
  MemoryControlBlock aTmpMCB;
  memcpy ( &aTmpMCB, aMCB, sizeof ( MemoryControlBlock ) );
  MemoryControlBlock* aResult = static_cast <MemoryControlBlock*> ( realloc ( aMCB, dwBytes + sizeof ( MemoryControlBlock ) ) );
  if ( aResult > 0 ) {
    if ( ( dwBytes > aTmpMCB.theSizeOfBlock ) && ( dwFlags & HEAP_ZERO_MEMORY ) )
      memset ( reinterpret_cast<char*> (aResult) + 
               sizeof ( MemoryControlBlock )  + 
               aTmpMCB.theSizeOfBlock, 0, dwBytes - aTmpMCB.theSizeOfBlock ); 
    memcpy ( aResult, &aTmpMCB, sizeof ( MemoryControlBlock ) );
    aResult->theSizeOfBlock = dwBytes;
    if ( aHeap->theLastAllocatedBlock == aMCB ) 
      aHeap->theLastAllocatedBlock = aResult;
    if ( aResult->thePrevBlock ) 
      aResult->thePrevBlock->theNextBlock = aResult;
    if ( aResult->theNextBlock ) 
      aResult->theNextBlock->thePrevBlock = aResult;
  }
  if ( !( aHeap->Flags & HEAP_NO_SERIALIZE ) && !( dwFlags & HEAP_NO_SERIALIZE ) ) 
    COMPAT_UNLOCK_FAST ( &aHeap->HeapMutex );
  if ( aResult ) 
    return ( reinterpret_cast <char*> ( aResult ) + sizeof ( MemoryControlBlock ) );
  else
    return 0;
}


DWORD HeapSize ( HANDLE hHeap,
                 DWORD dwFlags,
                 LPVOID lpMem )
{
  CALL_ENTRY
  MemoryControlBlock* aMCB = reinterpret_cast <MemoryControlBlock*> ( static_cast <char*> ( lpMem ) - 
                                                                      sizeof ( MemoryControlBlock ) );
  return aMCB->theSizeOfBlock;
}

//**********************************************************************
// modules manipulation functions

#define LIBRARY      1
#define RESOURCES    2


struct Module {
  char theFileName [PATH_MAX];
  char theTag;
  union {
    void* theLibrary;
    struct{
      int theFD;
      void* theData;
    } theResource;
  };
};

DWORD GetModuleFileName ( HMODULE hModule,
                          char* lpFilename,
                          DWORD nSize )
{
  CALL_ENTRY
  if ( hModule ) {
    Module* aModule = static_cast <Module*> ( hModule );
    strcpy ( lpFilename, aModule->theFileName );
    return strlen ( lpFilename );
  }
  return strlen ( strncpy ( lpFilename, ApplicationName, nSize) );
}

typedef BOOL APIENTRY (*_DllMain)( HMODULE hModule, DWORD dwReason, LPVOID lpReserved ); 

// libraries manipulation functions
HMODULE LoadLibrary ( LPCSTR lpLibFileName )
{
  CALL_ENTRY
  //Module* aModule = static_cast <Module*> ( HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof ( Module ) ) );
  Module* aModule = static_cast <Module*> ( calloc ( sizeof ( Module ), 1 ) );
  if ( !aModule ) {
    setError ();
    return 0;
  }
  aModule->theLibrary = dlopen ( lpLibFileName, RTLD_LOCAL|RTLD_LAZY);
  if ( !aModule->theLibrary  ) {
    setError ();
    //HeapFree ( GetProcessHeap (), 0, aModule );
    free ( aModule );
    fprintf ( stderr, "%s\n", dlerror () );
    return 0;
  }
  if ( !lpLibFileName )
    lpLibFileName = ApplicationName;
  strncpy ( aModule->theFileName, lpLibFileName, sizeof ( aModule->theFileName ) - 1 );
  aModule->theTag = LIBRARY;
  _DllMain DllMain = _DllMain ( GetProcAddress ( aModule, "DllMain" ) );
  void* Reserved = 0;
  if ( DllMain && !DllMain ( aModule, DLL_PROCESS_ATTACH, Reserved ) ) {
    dlclose ( aModule->theLibrary );
    //   HeapFree ( GetProcessHeap (), 0, aModule );
    free ( aModule );
    return 0;
  }
  return aModule;
}

HMODULE loadLibraryAsResources ( LPCSTR lpLibFileName )
{
  char buffer [PATH_MAX] = {0};
  strncpy ( buffer, lpLibFileName, sizeof ( buffer ) - 1 );
  char* p;
  p = strrchr( buffer, '.' );
  if ( p )
    *p = 0;
  //  Module* aModule = static_cast <Module*> ( HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY,  sizeof ( Module ) ) );
  Module* aModule = static_cast <Module*> ( calloc ( sizeof ( Module ), 1 ) );
  if ( !aModule ) {
    setError ();
    return 0;
  }

  aModule->theTag = RESOURCES;
  aModule->theResource.theFD = -1;
  strcpy ( aModule->theFileName, buffer );
  return aModule;
}

HMODULE LoadLibraryEx ( LPCSTR lpLibFileName, 
                        HANDLE hFile, 
                        DWORD dwFlags )
{
  CALL_ENTRY
  switch ( dwFlags ) {
  case 0 : 
    return LoadLibrary ( lpLibFileName );
  case LOAD_LIBRARY_AS_DATAFILE:
    return loadLibraryAsResources ( lpLibFileName );
  default:
    setError ( ERROR_NOT_SUPPORTED );
    return 0;
  }
}

BOOL FreeLibrary ( HMODULE hLibModule )
{
  CALL_ENTRY
  if ( !hLibModule )
    return FALSE;
  Module* aModule = static_cast <Module*> ( hLibModule );
  int result = 0;
  _DllMain DllMain = 0;
  switch (aModule->theTag) {
  case LIBRARY:
    DllMain = _DllMain ( GetProcAddress ( aModule, "DllMain" ) );
    if ( DllMain ) {
      void* Reserved = 0;
      DllMain ( aModule, DLL_PROCESS_DETACH, Reserved );
    }
    result = dlclose ( aModule->theLibrary );
    break;
  case RESOURCES:
    close ( aModule->theResource.theFD );
    if ( aModule->theResource.theData ) {
      //      HeapFree ( GetProcessHeap (), 0, aModule->theResource.theData );
      free ( aModule->theResource.theData );
      aModule->theResource.theData = 0;
    }
    break;
  default:
    break;
  }
  //  HeapFree ( GetProcessHeap (), 0, aModule );
  free ( aModule );
  //  return !result;
  // FALSE is returned because instance count is yet not implemented
  return FALSE;
}

FARPROC GetProcAddress ( HMODULE hModule,
                         LPCSTR lpProcName )
{
  CALL_ENTRY
  Module* aModule = static_cast <Module*> ( hModule );
  if ( aModule->theLibrary )
    return dlsym ( aModule->theLibrary, lpProcName);
  else
    return 0;
}

//**********************************************************************
// resources manipulation functions


HRSRC FindResource ( HMODULE hModule,
                     LPCSTR lpName,
                     LPCSTR lpType )
{
  CALL_ENTRY
  if ( lpType != RT_RCDATA ) {
    setError ( ERROR_NOT_SUPPORTED );
    return 0;
  }
  Module* aModule = static_cast <Module*> ( hModule );
  if ( aModule->theTag != RESOURCES ) {
    setError ( ERROR_FILE_NOT_FOUND );
    return 0;
  }
  char buffer [PATH_MAX];
  strcpy ( buffer, aModule->theFileName );
  strcpy ( buffer + strlen ( buffer ), MetafileExt.c_str () );
  int fd =  open ( buffer, O_RDONLY );
  if ( fd == -1 ) {
    setError ();
    return 0;
  }
  aModule->theResource.theFD = fd;
  return hModule;
}


HGLOBAL LoadResource ( HMODULE hModule,
                       HRSRC hResInfo )
{
  CALL_ENTRY
  Module* aModule = static_cast <Module*> ( hModule );
  if ( ( aModule->theTag != RESOURCES ) || ( aModule->theResource.theFD == -1 ) ) {
    setError ( ERROR_FILE_NOT_FOUND );
    return 0;
  } 
  struct stat aStat;
  if ( fstat ( aModule->theResource.theFD, &aStat ) ) {
    setError ();
    return 0;
  }
  if ( !aStat.st_size ){
    setError ( ERROR_FILE_NOT_FOUND );
    return 0;
  } 
  if ( aModule->theResource.theData ) {
    //HeapFree ( GetProcessHeap (), 0, aModule->theResource.theData );
    free ( aModule->theResource.theData );
    aModule->theResource.theData = 0;
  }
  
  //aModule->theResource.theData = HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY, aStat.st_size );
  aModule->theResource.theData = calloc ( sizeof ( char ), aStat.st_size );
  if ( !aModule->theResource.theData ) {
    setError ();
    return 0;
  }

  if ( read ( aModule->theResource.theFD, aModule->theResource.theData, aStat.st_size ) != aStat.st_size ) {
    //    HeapFree ( GetProcessHeap (), 0, aModule->theResource.theData );
    free ( aModule->theResource.theData );
    aModule->theResource.theData = 0;
    setError ( ERROR_FILE_NOT_FOUND );
    return 0;    
  }
  return hModule;
}

BOOL FreeResource ( HGLOBAL hResData )
{
  CALL_ENTRY
  Module* aModule = static_cast <Module*> ( hResData );  
  //  HeapFree ( GetProcessHeap (), 0, aModule->theResource.theData );
  free ( aModule->theResource.theData );
  aModule->theResource.theData = 0;
  return TRUE;
}


DWORD SizeofResource ( HMODULE hModule,
                       HRSRC hResInfo )
{
  CALL_ENTRY
  Module* aModule = static_cast <Module*> ( hModule );
  if ( ( aModule->theTag != RESOURCES ) || ( aModule->theResource.theFD == -1 ) ) {
    setError ( ERROR_FILE_NOT_FOUND );
    return 0;
  }
  struct stat aStat;
  if ( fstat ( aModule->theResource.theFD, &aStat ) ) {
    setError ();
    return 0;
  }
  return aStat.st_size;
}

LPVOID LockResource ( HGLOBAL hResData )
{
  Module* aModule = static_cast <Module*> ( hResData );
  if ( ( aModule->theTag != RESOURCES ) || ( aModule->theResource.theFD == -1 ) ) {
    setError ( ERROR_FILE_NOT_FOUND );
    return 0;
  }
  if ( aModule->theResource.theData ) 
    return aModule->theResource.theData;
  return 0;
}


//**********************************************************************
// Thread Local Storage manipulation functions

#if defined (THREADS)

DWORD TlsAlloc ()
{
  pthread_key_t aKey;  
  if ( pthread_key_create ( &aKey, 0 ) )
    return TLS_OUT_OF_INDEXES;
  return aKey;
}


LPVOID TlsGetValue ( DWORD dwTlsIndex )
{
  void* tmp = pthread_getspecific ( static_cast <pthread_key_t> ( dwTlsIndex ) );
//   if ( !tmp ) {
//     return 0;//TLS_OUT_OF_INDEXES;
//   }
  return tmp;
}

BOOL TlsSetValue ( DWORD dwTlsIndex, LPVOID lpTlsValue )
{
  //  CALL_ENTRY
  if ( pthread_setspecific ( static_cast <pthread_key_t> ( dwTlsIndex ), lpTlsValue ) ) {
    //    setError ( TLS_OUT_OF_INDEXES );
    return FALSE;
  }
  return TRUE;
}


BOOL TlsFree( DWORD dwTlsIndex )
{
  if ( pthread_key_delete ( static_cast <pthread_key_t> (  dwTlsIndex ) ) ) 
    return FALSE;
  return TRUE;
}

#else // THREADS  

typedef std::vector<void*> TLSStubVector;

TLSStubVector theTLSStubVector;
DWORD TlsAlloc ()
{
  theTLSStubVector.push_back( 0 );
  return theTLSStubVector.size () - 1;
}


LPVOID TlsGetValue ( DWORD dwTlsIndex )
{
  if ( dwTlsIndex >= theTLSStubVector.size () ) 
    return 0;
  return theTLSStubVector [ dwTlsIndex ];
}

BOOL TlsSetValue ( DWORD dwTlsIndex, LPVOID lpTlsValue )
{
  if ( dwTlsIndex >= theTLSStubVector.size () ) 
    return FALSE;
  theTLSStubVector [ dwTlsIndex ] = lpTlsValue;
  return TRUE;
}


BOOL TlsFree( DWORD dwTlsIndex )
{
  if ( dwTlsIndex >= theTLSStubVector.size () ) 
    return FALSE; 
  return TRUE;
}

#endif // THREADS  

//**********************************************************************
// other functions

DWORD GetCurrentProcessId ()
{
  return getpid ();
}

VOID OutputDebugString ( const char* lpOutputString )
{
  CALL_ENTRY
      fprintf ( stderr, "%s\n", lpOutputString  );
}

DWORD GetLastError ( VOID )
{
  return reinterpret_cast <DWORD> ( TlsGetValue ( lastErrorKey ));
}

DWORD CoInitialize ( LPVOID reserved )
{
  CALL_ENTRY
  return 0;
}

void CoUninitialize ()
{
  CALL_ENTRY
  return;
}

#define UUID_STRING_MAX 37
#define UUID_ELEMENTS_NUM      11

DWORD IIDFromString ( const char* lpsz, LPIID lpiid )
{
  if ( !lpsz || (*lpsz == 0x0 ) ) {
    memset ( lpiid, 0, sizeof ( IID ) );
    return S_OK;
  }

  if ( strlen ( lpsz ) != ( UUID_STRING_MAX - 1 ) ) {
    return E_INVALIDARG;
  }

  long    aData1;
  int     aData2;
  int     aData3;
  int     aData4[8];
  if ( sscanf ( lpsz, "%8lx-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x",
                &aData1,
                &aData2,
                &aData3,
                &aData4[0],
                &aData4[1],
                &aData4[2],
                &aData4[3],
                &aData4[4],
                &aData4[5],
                &aData4[6],
                &aData4[7] ) != UUID_ELEMENTS_NUM ) 
    return E_INVALIDARG;

  lpiid->Data1 = aData1;
  lpiid->Data2 = aData2;
  lpiid->Data3 = aData3;
  lpiid->Data4[0] = aData4[0];
  lpiid->Data4[1] = aData4[1];
  lpiid->Data4[2] = aData4[2];
  lpiid->Data4[3] = aData4[3];
  lpiid->Data4[4] = aData4[4];
  lpiid->Data4[5] = aData4[5];
  lpiid->Data4[6] = aData4[6];
  lpiid->Data4[7] = aData4[7];
  return S_OK;
}


VOID GetLocalTime ( LPSYSTEMTIME lpSystemTime )
{
  CALL_ENTRY
  time_t aTime;
  if ( time ( &aTime ) == static_cast <time_t> ( -1 ) )
    return;
  tm aTm;
  if ( !gmtime_r ( &aTime, &aTm ) )
    return;
  lpSystemTime->wYear = aTm.tm_year + 1900;
  lpSystemTime->wMonth = aTm.tm_mon + 1;
  lpSystemTime->wDayOfWeek = aTm.tm_wday;
  lpSystemTime->wDay = aTm.tm_mday;
  lpSystemTime->wHour = aTm.tm_hour;
  lpSystemTime->wMinute = aTm.tm_min;
  lpSystemTime->wSecond = aTm.tm_sec;
  lpSystemTime->wMilliseconds = 0;
  return;  
}

VOID GetSystemTime( LPSYSTEMTIME lpSystemTime )
{
  CALL_ENTRY
  time_t aTime;
  if ( time ( &aTime ) == static_cast <time_t> ( -1 ) )
    return;
  tm aTm;
  if ( !gmtime_r ( &aTime, &aTm ) )
    return;
  lpSystemTime->wYear = aTm.tm_year + 1900;
  lpSystemTime->wMonth = aTm.tm_mon;
  lpSystemTime->wDayOfWeek = aTm.tm_wday;
  lpSystemTime->wDay = aTm.tm_mday;
  lpSystemTime->wHour = aTm.tm_hour;
  lpSystemTime->wMinute = aTm.tm_min;
  lpSystemTime->wSecond = aTm.tm_sec;
  lpSystemTime->wMilliseconds = 0;
  return;  
}

// error mode 
UINT SetErrorMode ( UINT uMode )
{
  CALL_ENTRY
  return 0;
}

DWORD GetVersion( VOID )
{
  CALL_ENTRY
  return 0x80000000;
}

BOOL GetVersionEx ( LPOSVERSIONINFO lpVersionInformation )
{
  CALL_ENTRY
  lpVersionInformation->dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  return TRUE;
}

VOID GetSystemInfo ( LPSYSTEM_INFO lpSystemInfo )
{
  CALL_ENTRY
  lpSystemInfo->dwPageSize = getpagesize ();  
  return;
}


BOOL QueryPerformanceCounter ( LARGE_INTEGER *lpPerformanceCount )
{
  CALL_ENTRY
  lpPerformanceCount->QuadPart = GetTickCount ();  
   //  NOT_IMPLEMENTED
  return 0;
}

BOOL QueryPerformanceFrequency ( LARGE_INTEGER *lpFrequency  )
{
  CALL_ENTRY
  NOT_IMPLEMENTED
  return 0;
}


DWORD GetCurrentDirectory ( DWORD nBufferLength,
                            LPSTR lpBuffer )
{
  CALL_ENTRY
  if ( getcwd ( lpBuffer, nBufferLength ) )
    return strlen ( lpBuffer );
  setError ();
  return 0;
}


DWORD GetTickCount ()
{
  CALL_ENTRY
  struct timeval aTimeVal;
  memset ( &aTimeVal, 0, sizeof ( aTimeVal ) );
  if ( gettimeofday ( &aTimeVal, 0 ) )
    return 0;
  return ( aTimeVal.tv_sec - theStartSeconds ) * 1000 + aTimeVal.tv_usec / 1000;
}
#if defined (__i386__)
#define LOCK_INSTRUCTION "lock;"

LONG InterlockedIncrement ( LONG* lpAddend )
{
  __asm__ __volatile__( LOCK_INSTRUCTION 
                        "incl %0"
                        :"=m" (*lpAddend)
                        :"m" (*lpAddend));
  return *lpAddend;
}

LONG InterlockedDecrement ( LONG* lpAddend )
{
  __asm__ __volatile__( LOCK_INSTRUCTION "decl %0"
                        :"=m" (*lpAddend)
                        :"m" (*lpAddend));
  return *lpAddend;
}

LONG InterlockedExchange ( LONG* Target,
                           LONG Value )
{
  LONG anInitialValue = *Target;
  __asm__ __volatile__( LOCK_INSTRUCTION 
                        "xchgl %0,%1"
                        :"=r" (Value)
                        :"m" (*Target)
                        :"memory");
  return anInitialValue; 
}

LONG InterlockedExchangeAdd ( LONG* lpAddend,
                              LONG Value )
{
  LONG anInitialValue = *lpAddend;
  __asm__ __volatile__( LOCK_INSTRUCTION 
                        "addl %1,%0"
                        :"=m" (*lpAddend)
                        :"ir" (Value), "m" (*lpAddend));
  return anInitialValue; 
}

#else
#error "Consideration is needed!"

LONG InterlockedIncrement ( LONG* lpAddend )
{
  (*lpAddend)++;
  return *lpAddend;
}

LONG InterlockedDecrement ( LONG* lpAddend )
{
  (*lpAddend)--;  
  return *lpAddend;  
}

LONG InterlockedExchange ( LONG* Target,
                           LONG Value )
{
  LONG anInitialValue = *Target;
  *Target = Value;  
  return anInitialValue; 
}

PVOID InterlockedCompareExchange ( PVOID *Destination,
                                   PVOID Exchange,
                                   PVOID Comperand )
{
  NOT_IMPLEMENTED
  return 0;
}

LONG InterlockedExchangeAdd ( LONG* Addend,
                                         LONG Value )
{
  LONG anInitialValue = *Addend;
  *Addend += Value;  
  return anInitialValue; 
}
#endif

// HMODULE _CreateModule ( const char* aModuleName )
// {
//   Module* aModule = static_cast <Module*> ( HeapAlloc ( GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof ( Module ) ) );
//   if ( !aModule ) {
//     setError ();
//     return 0;
//   }
//   char aFileName [PATH_MAX] = {0};
//   char* aPtr = 0;
//   strncpy ( aFileName, ApplicationName, sizeof ( aFileName ) );
//   aPtr = dirname ( aFileName );
//   int aLen = strlen ( aPtr );
//   strncpy ( aPtr + aLen, aModuleName, sizeof ( aPtr ) - aLen  );
//   return 

//   strncpy ( aModule->theFileName, aFileName, sizeof ( aModule->theFileName ) - 1 );
//   aModule->theTag = LIBRARY;
//   return aModule; 
// }

char* strlwr ( char* aSource )
{
  return aSource;
  int i = 0;
  while ( aSource [i] ) 
    aSource [ i ] = tolower ( aSource [i++]);
  return aSource;
}


DWORD ExpandEnvironmentStrings ( LPCSTR lpSrc, LPSTR lpDst, DWORD nSize )
{
  CALL_ENTRY
  NOT_IMPLEMENTED
  return 0;
}
