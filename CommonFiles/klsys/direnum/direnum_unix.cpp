#if defined (__unix__)

#include <string.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "direnum.h"


namespace KLSysNS {
  
  inline bool isAuxiliaryDirectory ( const char* aDirectory ) 
  {
    return ( aDirectory &&  
             ( ( ( aDirectory [ 0 ] == '.' ) && 
                 ( aDirectory [ 1 ] == 0 ) ) ||
               ( ( aDirectory [ 0 ] == '.' ) && 
                 ( aDirectory [ 1 ] == '.' ) && 
                 ( aDirectory [ 2 ] == 0 ) ) ) );
  }

  DirectoryEnumerator::DirectoryEnumerator ()
    : theDirectory ( 0 )
  {}
  

  DirectoryEnumerator::DirectoryEnumerator ( const char* aPath )
    : theDirectory ( 0 )
  {
    open ( aPath );
  }
  
  DirectoryEnumerator::~DirectoryEnumerator ()
  {
    if ( !theDirectory )
      return;
    closedir ( theDirectory );
  }
  
  bool DirectoryEnumerator::isOpened () const
  {
    return theDirectory;
  }

  bool DirectoryEnumerator::open ( const char* aPath )
  {
    close ();
    memset ( thePath, 0, sizeof ( thePath ) );
    strncpy ( thePath, aPath, sizeof ( thePath ) - 1 );
    theDirectory = opendir ( thePath );
    if ( !theDirectory )
      return false;
    memset ( &theDirEntry, 0, sizeof ( theDirEntry ) );
    return true;
  }  

  void DirectoryEnumerator::close ()
  {
    if ( !theDirectory )
      return;
    closedir ( theDirectory );
    theDirectory = 0;
  }
  
  bool DirectoryEnumerator::nextEntry ( const char* aPattern, bool aDirectoryPattern )
  {
    if ( !theDirectory  )
      return false;
    dirent* aDirEntry = 0;
    while ( !readdir_r ( theDirectory, &theDirEntry, &aDirEntry ) && aDirEntry ) {
      if ( isAuxiliaryDirectory ( theDirEntry.d_name ) )
        continue;
      if ( !aPattern || !fnmatch ( aPattern, theDirEntry.d_name, 0 ) )
        return true;
      if ( !aDirectoryPattern && isDirectory () ) 
        return true;
    }
    close ();
    return false;
  }  

  const char* DirectoryEnumerator::getFileName () const
  {
    if ( !theDirectory )
      return 0;
    return theDirEntry.d_name;
  }
    
  bool DirectoryEnumerator::isDirectory () const
  {
    if ( !theDirectory )
      return false;
    if ( !theDirEntry.d_name )
      return false;
#if defined (_DIRENT_HAVE_D_TYPE)
    if ( theDirEntry.d_type == DT_DIR )
      return true;
    if ( theDirEntry.d_type != DT_UNKNOWN )
      return false;
#endif
    char aFileName [ PATH_MAX ] = {0};
    strcat ( aFileName, thePath );
    strcat ( aFileName, "/" );
    strcat ( aFileName, getFileName () );
    struct stat aStatus;
    if ( stat ( aFileName, &aStatus ) )
      return false;
    
    return S_ISDIR ( aStatus.st_mode );
  }

}

#endif //__unix__
