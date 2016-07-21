#include <string.h>
#include <time.h>
#include <utime.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <prague.h>
#include <iface/i_objptr.h>
#include <iface/e_ktime.h>

extern hROOT g_root;
pthread_mutex_t theMutex = PTHREAD_MUTEX_INITIALIZER;

tERROR pr_call systemErrors2PragueErrors ( tDWORD unixErr ) {
  switch( unixErr ) {
  case ENOENT                      : return errOBJECT_NOT_FOUND;
    
  case EEXIST                      : return errOBJECT_ALREADY_EXISTS;
    
  case ETXTBSY                     : return errLOCKED;
    
  case EACCES                      :
  case EPERM                       : return errACCESS_DENIED;
    
  case EROFS                       : return errWRITE_PROTECT;

  case ENOSPC                      : return errOUT_OF_SPACE;

  default                          : return errUNEXPECTED;
  }
}

tERROR makeFullPath ( const tCHAR* aFilePath, const tCHAR* aFileName, tCHAR* aBuffer, tDWORD aBufferSize ) 
{
  if ( ( strlen ( aFilePath ) + strlen ( aFileName ) + 2 ) > aBufferSize )
    return errBUFFER_TOO_SMALL;

  strcat ( aBuffer, aFilePath );
  strcat ( aBuffer, "/" );
  strcat ( aBuffer, aFileName );

  return errOK;
}


tERROR _getFileAttributes( const tCHAR* aFileName, tDWORD& anAttributes, tDWORD& aSystemError ) 
{
  aSystemError = 0;
  struct stat aStatus;
  if ( stat ( aFileName, &aStatus ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );
  }

  anAttributes |= ( aStatus.st_mode & S_IFDIR ) ? fATTRIBUTE_DIRECTORY : 0;
  if ( access ( aFileName, W_OK ) ) {
    if ( errno == EACCES )
      anAttributes |= fATTRIBUTE_READONLY;
    else {
      aSystemError = errno;
      return systemErrors2PragueErrors ( aSystemError );
    }
  }
  else
    anAttributes |= fATTRIBUTE_NORMAL;
  return errOK;
}

tERROR _getFileAttributes( const tCHAR* aFilePath, const tCHAR* aFileName, tDWORD& anAttributes, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;
  return _getFileAttributes( aFullName, anAttributes, aSystemError );
}

tERROR _setFileAttributes( const tCHAR* aFileName, tDWORD anAttributes, tDWORD& aSystemError ) 
{
  aSystemError = 0;

  struct stat aStatus;
  
  if ( stat ( aFileName, &aStatus ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );
  }
  
  aStatus.st_mode &= ~(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

  mode_t aPermitions;

  if ( anAttributes & fATTRIBUTE_NORMAL ) 
    aPermitions = (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) & aStatus.st_mode;
  if  ( anAttributes & fATTRIBUTE_READONLY )
    aPermitions = (S_IRUSR|S_IRGRP) & aStatus.st_mode;
  
  if ( chmod ( aFileName, aPermitions ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );
  }
  return errOK;
}

tERROR _setFileAttributes( const tCHAR* aFilePath, const tCHAR* aFileName, tDWORD anAttributes, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;
  return _setFileAttributes( aFullName, anAttributes, aSystemError );
}


tERROR _getFileSize ( const tCHAR* aFileName, tQWORD& aFileSize, tDWORD& aSystemError )
{
  aSystemError  = 0;

  struct stat aStatus;
  if ( stat ( aFileName, &aStatus ) ) {
    aSystemError = errno;
    return  systemErrors2PragueErrors ( aSystemError );
  }

  aFileSize = aStatus.st_size;

  return errOK;
}

tERROR _getFileSize ( const tCHAR* aFilePath, const tCHAR* aFileName, tQWORD& aFileSize, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;
  return _getFileSize (  aFullName, aFileSize, aSystemError );
}

tERROR _setFileSize ( const tCHAR* aFileName, tQWORD aFileSize, tDWORD& aSystemError )
{
  aSystemError  = 0;

  if ( truncate ( aFileName, aFileSize ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );
  }
  
  return errOK;
}

tERROR _setFileSize ( const tCHAR* aFilePath, const tCHAR* aFileName, tQWORD aFileSize, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;
  return _setFileSize ( aFullName, aFileSize, aSystemError );
}

tERROR _getFileTime ( const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME& aDateTime, tDWORD& aSystemError )
{
  if (!DTSet)
    return errINTERFACE_NOT_LOADED;
  
  time_t aNeededTime;
  
  struct stat aStatus;
  if ( stat ( aFileName, &aStatus ) ) {
    aSystemError = errno;
    return  systemErrors2PragueErrors ( aSystemError );
  }

  switch ( aNeededTimeStamp ) {
    
  case pgOBJECT_CREATION_TIME:
    aNeededTime = aStatus.st_ctime;
    break;
    
  case pgOBJECT_LAST_WRITE_TIME:
    aNeededTime = aStatus.st_mtime;
    break;
    
  case pgOBJECT_LAST_ACCESS_TIME:
    aNeededTime = aStatus.st_atime;
    break;
    
  default:
    return errPARAMETER_INVALID;
    break;
  }
  
  tm aTm;    
  
  if ( !localtime_r ( &aNeededTime, &aTm ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );  
  }

  return DTSet ( &aDateTime, aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec, 0 );
}

tERROR _getFileTime ( const tCHAR* aFilePath, const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME& aDateTime, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;
  return _getFileTime ( aFullName, aNeededTimeStamp, aDateTime, aSystemError );
}


tERROR _setFileTime ( const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME aDateTime, tDWORD& aSystemError )
{
  if (!DTGet)
    return errINTERFACE_NOT_LOADED;

  tDWORD aYear, aMonth, aDay, anHour, aMinute, aSecond, aNanoseconds;

  tERROR error = errOK;

  error = DTGet ( reinterpret_cast<tDATETIME*> ( aDateTime ), &aYear, &aMonth, &aDay, &anHour, &aMinute, &aSecond, &aNanoseconds );

  if ( PR_FAIL ( error ) )
    return error;

  struct stat aStatus;
  if ( stat ( aFileName, &aStatus ) ) {
    aSystemError = errno;
    return  systemErrors2PragueErrors ( aSystemError );
  }

  time_t aNewTime;
  tm aTm;
  memset ( &aTm, 0, sizeof ( aTm ) );
  aTm.tm_sec = aSecond;
  aTm.tm_min = aMinute;
  aTm.tm_hour = anHour;
  aTm.tm_mday = aDay;
  aTm.tm_mon = aMonth - 1;
  aTm.tm_year = aYear - 1900;
  aNewTime = mktime ( &aTm );

  utimbuf aTimeBuf;
  aTimeBuf.modtime = aStatus.st_mtime;
  aTimeBuf.actime = aStatus.st_atime;

  switch ( aNeededTimeStamp ) {
    
  case pgOBJECT_CREATION_TIME:
    return errOK; // creation time is not changeable
    break;
    
  case pgOBJECT_LAST_WRITE_TIME:
    aTimeBuf.modtime = aNewTime;
    break;
    
  case pgOBJECT_LAST_ACCESS_TIME:
    aTimeBuf.actime = aNewTime;
    break;
    
  default:
    return errPARAMETER_INVALID;
    break;
  }

  if ( utime ( aFileName, &aTimeBuf ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );
  }
  return errOK;
}

tERROR _setFileTime ( const tCHAR* aFilePath, const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME aDateTime, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;
  return _setFileTime ( aFullName, aNeededTimeStamp, aDateTime, aSystemError );
}

tERROR pr_call _isFolder ( const tCHAR* aFilePath, const tCHAR* aFileName, tBOOL& aResult, tDWORD& aSystemError )
{
  tCHAR aFullName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aFilePath, aFileName, aFullName, sizeof ( aFullName ) );
  if ( PR_FAIL ( error ) )
    return error;

  struct stat aStatus;
  if ( stat ( aFullName, &aStatus ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );    
  }
  aResult = S_ISDIR ( aStatus.st_mode ) ? cTRUE : cFALSE;
  return errOK;
}

tERROR pr_call _copy ( const tCHAR* aSource, const tCHAR* aDestination, tBOOL anOverwrite, tDWORD& aSystemError )
{
  if ( !anOverwrite && ( !access ( aDestination, F_OK ) && ( errno == EEXIST ) ) )
    return errOBJECT_ALREADY_EXISTS;
  
  int aSourceFD = open ( aSource, O_RDONLY );
  if ( aSourceFD == -1 ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );    
  }  

  struct stat aStatus;
  
  if ( fstat ( aSourceFD, &aStatus ) ) {
    close ( aSourceFD );
    aSourceFD = -1;
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );    
  }    
  
  struct stat aDestStatus;
  if ( !stat (aDestination, &aDestStatus ) && ( aStatus.st_ino == aDestStatus.st_ino ) ) 
	  return errACCESS_DENIED;

  int aDestinationFD = open ( aDestination, 
                              ( ( anOverwrite ) ? O_CREAT | O_TRUNC : O_CREAT | O_EXCL ) | O_WRONLY, 
                              aStatus.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO) );
  
  if ( aDestinationFD == -1 ) {
    close ( aSourceFD );
    aSourceFD = -1;
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );    
  }    

  if ( !aStatus.st_size ) {
    close ( aSourceFD );
    aSourceFD = -1;
    close ( aDestinationFD );
    aDestinationFD = -1;
    return errOK;
  }

  tCHAR* anInputBuffer = reinterpret_cast <tCHAR*> ( mmap ( 0, aStatus.st_size, PROT_READ, MAP_SHARED, aSourceFD, 0 ) );
  if ( !anInputBuffer ) {
    aSystemError = errno;
    close ( aSourceFD );
    aSourceFD = -1;
    close ( aDestinationFD );
    aDestinationFD = -1;
    return systemErrors2PragueErrors ( aSystemError );    
  }    
  off_t aWriteCount = 0;
  while ( aWriteCount < aStatus.st_size ) {
    int aResult = write ( aDestinationFD, anInputBuffer + aWriteCount, aStatus.st_size - aWriteCount );
    if ( aResult != -1 ){ 
// 	    tCHAR* buf = (tCHAR*)malloc(aResult*3);
// 	    for(int i = 0; i < aResult; i++){
// 		    sprintf(buf+i*2, "%02x", *(anInputBuffer + aWriteCount + i));
// 	    }
// 	    PR_TRACE((g_root, prtSPAM, "nfio\tPrCopy: [%s]", buf)); 	    
// 	    free(buf);
	    aWriteCount += aResult;
    }
    else
      break;
  }
  
  aSystemError = errno;
  munmap ( anInputBuffer, aStatus.st_size );
  close ( aSourceFD );
  aSourceFD = -1;
  close ( aDestinationFD );
  aDestinationFD = -1;
  if ( aWriteCount < aStatus.st_size )
    return systemErrors2PragueErrors ( aSystemError );  
  return errOK;
}

tERROR pr_call _rename ( const tCHAR* aSource, const tCHAR* aDestination, tBOOL anOverwrite, tDWORD& aSystemError )
{
  if ( !anOverwrite && !access ( aDestination, F_OK ) )
    return errOBJECT_ALREADY_EXISTS;
  if ( rename ( aSource, aDestination ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );    
  }    
  return errOK;
}

tERROR pr_call _rename (  const tCHAR* aSourcePath, const tCHAR* aSourceName, 
                          const tCHAR* aDestinationPath, const tCHAR* aDestinationName, tBOOL anOverwrite, tDWORD& aSystemError )
{  
  tCHAR aFullSourceName [ PATH_MAX ] = {0};
  tERROR error =  makeFullPath ( aSourcePath, aSourceName, aFullSourceName, sizeof ( aFullSourceName ) );
  if ( PR_FAIL ( error ) )
    return error;

  tCHAR aFullDestinationName [ PATH_MAX ] = {0};
  error =  makeFullPath ( aDestinationPath, aDestinationName, aFullDestinationName, sizeof ( aFullDestinationName ) );
  if ( PR_FAIL ( error ) )
    return error;
  
  return _rename ( aFullSourceName, aFullDestinationName, anOverwrite, aSystemError );

}



tERROR pr_call _delete ( const tCHAR* aFileName, tDWORD& aSystemError )
{
  if ( unlink ( aFileName ) ) {
    aSystemError = errno;
    return systemErrors2PragueErrors ( aSystemError );    
  }    
  return errOK;
}

tERROR _absolutePath ( const tCHAR* aPath, tDWORD aSize, tCHAR* aBuffer, tDWORD aBufferSize )
{
  tCHAR aCurrentDir [ MAX_PATH ] = {0};
  pthread_mutex_lock ( &theMutex );
  if ( !getcwd ( aCurrentDir, sizeof ( aCurrentDir ) ) ) {
    pthread_mutex_unlock ( &theMutex );
    return errUNEXPECTED;
  }

  if ( chdir ( aPath ) ) {
    pthread_mutex_unlock ( &theMutex );
    return errPARAMETER_INVALID;
  }
  tERROR anError = ( !getcwd ( aBuffer, aBufferSize ) ) ? errUNEXPECTED : errOK;
  chdir ( aCurrentDir );  
  pthread_mutex_unlock ( &theMutex );
  return anError;
}


tERROR _dirName ( const tCHAR* aPath, tDWORD aSize, tCHAR* aBuffer, tDWORD aBufferSize, tDWORD& anOutSize )
{
  const tCHAR* aPosition = aPath + aSize - 1;
  while ( ( *aPosition != '/' ) && ( aPosition != aPath ) )
    --aPosition;
  tCHAR aRoot [] = "/";
  tCHAR aCurrentDir [] = "./";
  if ( aPosition == aPath ) 
    if ( *aPosition != '/' ) {
      aPath = aCurrentDir;
      anOutSize = sizeof ( aCurrentDir );
    }
    else {
      aPath = aRoot;
      anOutSize = sizeof ( aRoot );
    }

  else
    anOutSize = aPosition - aPath;
  if ( !aBufferSize )
    return errOK;
  if ( anOutSize > aBufferSize  )
    return errBUFFER_TOO_SMALL;
  strncpy ( aBuffer, aPath, anOutSize );
  aBuffer [ anOutSize ] = 0;
  return errOK;
}

tERROR _fileName ( const tCHAR* aPath, tDWORD aSize, tCHAR* aBuffer, tDWORD aBufferSize, tDWORD& anOutSize )
{
  const tCHAR* aPosition = aPath + aSize;
  while ( ( *( aPosition - 1 ) != '/' ) && ( aPosition != aPath ) )
    --aPosition;
  anOutSize = aPath + aSize - aPosition + 1;
  if ( !aBufferSize )
    return errOK;
  if ( anOutSize > aBufferSize  )
    return errBUFFER_TOO_SMALL;
  strncpy ( aBuffer, aPosition, anOutSize );
  return errOK;
}
