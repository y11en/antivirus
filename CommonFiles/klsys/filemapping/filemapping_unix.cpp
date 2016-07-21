#if defined (__unix__)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#include "filemapping.h"

namespace KLSysNS {

  FileMapping::FileMapping () 
    : theFile ( -1 ), 
      theMapFlags ( 0 ), 
      theMapProt ( 0 ), 
      theAdvise ( 0 )
  {}

  int FileMapping::create ( const char* aFileName,		// by ArtemK
                            int anAccessRights, 
                            AccessHints ahAccessHints )
  {
    destroy ();
    if ( !aFileName ) {
      errno = EINVAL;
      return -1;
    }

    int aFileFlags = 0;
    theMapProt = PROT_NONE;
    if ( anAccessRights & KL_FILEMAP_READ ) {
      aFileFlags = O_RDONLY;
      theMapProt |= PROT_READ;
      theMapFlags = MAP_SHARED;
    }
    if ( anAccessRights & KL_FILEMAP_WRITE) {
      aFileFlags = O_RDWR;
      theMapProt |= PROT_WRITE;
      theMapFlags = MAP_SHARED;
    }
    if ( anAccessRights & KL_FILEMAP_EXECUTE ){
      if ( !aFileFlags )
        aFileFlags = O_RDONLY;
      theMapProt |= PROT_EXEC;
      theMapFlags = MAP_SHARED;
    }
    if ( anAccessRights & KL_FILEMAP_COPY_ON_WRITE ) {
      aFileFlags = O_RDONLY;
      theMapProt |= PROT_READ|PROT_WRITE;
      theMapFlags = MAP_PRIVATE;
    }
#if defined (HAVE_MADVISE)
#if !defined (POSIX_MADV_NORMAL)
#define POSIX_MADV_NORMAL MADV_NORMAL
#endif // POSIX_MADV_NORMAL
#if !defined (POSIX_MADV_SEQUENTIAL)
#define POSIX_MADV_SEQUENTIAL MADV_SEQUENTIAL
#endif // POSIX_MADV_SEQUENTIAL
#if !defined (POSIX_MADV_RANDOM)
#define POSIX_MADV_RANDOM MADV_RANDOM
#endif // POSIX_MADV_RANDOM

    switch (ahAccessHints) {
    case ahNormalAccess:
      theAdvise = POSIX_MADV_NORMAL;
      break;
    case ahSequentialAccess:
      theAdvise = POSIX_MADV_SEQUENTIAL;
      break;
    case ahRandomAccess:
      theAdvise = POSIX_MADV_RANDOM;
      break;
    default:
      return -1;
    }
#endif // HAVE_MADVISE

    theFile = open ( aFileName, aFileFlags );
    if (theFile == -1)
      return -1;

    struct stat aStatus;
    if ( fstat ( theFile, &aStatus ) ) {
      close ( theFile );
      return -1;
    }
    
    if ( !S_ISREG ( aStatus.st_mode ) ) {
      close ( theFile );
      errno = EINVAL;
      return -1;
    }

    return 0;
  }

  int FileMapping::destroy ()
  {
    if ( theFile != -1 )
      close ( theFile );
    theFile = -1;
    return 0;
  }

  void* FileMapping::map ( kl_off_t anOffset, kl_size_t aNumberOfBytes,  kl_size_t* aNumberOfMappedBytes )
  {
    void* aResult = 0;
    if ( !aNumberOfMappedBytes ) {
      errno = EINVAL;
      return 0;
    }
    *aNumberOfMappedBytes = 0;
    struct stat aStat;
    if ( fstat ( theFile, &aStat ) )
      return 0;
    if ( !aStat.st_size ) 
      return reinterpret_cast<void*> ( -1 );

    if ( !aNumberOfBytes )
      aNumberOfBytes = aStat.st_size;

    aResult = mmap ( 0, aNumberOfBytes, theMapProt, theMapFlags, theFile, anOffset );
    if ( aResult == MAP_FAILED )
      aResult = 0;
#if defined (HAVE_MADVISE)
    if ( aResult )
      madvise ( aResult, aNumberOfBytes, theAdvise );
#endif // HAVE_MADVISE
    if ( aResult )
      *aNumberOfMappedBytes = aNumberOfBytes;
    return aResult;
  }

  int FileMapping::flush ( void* anAddress, kl_size_t aNumberOfBytes )
  {
    if ( msync ( anAddress, aNumberOfBytes, MS_SYNC ) )
      return -1;
    return 0;
  }
  
  int FileMapping::unmap ( void* anAddress, kl_size_t aNumberOfBytes )
  {
    if ( munmap ( anAddress, aNumberOfBytes ) )
      return -1;
    return 0;  
  }

}  

#endif // __unix__
