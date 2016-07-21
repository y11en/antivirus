
#if defined (__unix__)
	#if defined (HAVE_CONFIG_H)
		#include <config-unix.h>
	#endif

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <errno.h>
#endif // __unix__

#include "filemapping.h"

// by ArtemK
#ifdef _WIN32_WCE
#define OSVERSIONINFOEX		OSVERSIONINFO
#include <winbase.h>
#endif


#if defined (_WIN32)
#pragma pack(1)
struct qword {
  #if defined (MACHINE_IS_BIG_ENDIAN)
	  DWORD Hi;
	  DWORD Low;	
  #else	// MACHINE_IS_BIG_ENDIAN
	  DWORD Low;
	  DWORD Hi;
  #endif // MACHINE_IS_BIG_ENDIAN
};

union Offset2QWord {
  kl_off_t theOffset;
  qword theQWord;
};
#pragma pack ()

#if !defined (FILE_MAP_EXECUTE)
#define FILE_MAP_EXECUTE SECTION_MAP_WRITE
#endif // FILE_MAP_EXECUTE

#endif // _WIN32

int fm_create ( FileMapping* aFileMapping, const TCHAR* aFileName,		// by ArtemK
		int anAccessRights, AccessHints ahAccessHints )
{
#if defined (__unix__)  
  if ( !aFileName || !aFileMapping ) {
    errno = EINVAL;
    return -1;
  }
  int aFileFlags = 0;
  aFileMapping->theMapProt = PROT_NONE;
  if ( anAccessRights & KL_FILEMAP_READ ) {
    aFileFlags = O_RDONLY;
    aFileMapping->theMapProt |= PROT_READ;
    aFileMapping->theMapFlags = MAP_SHARED;
  }
  if ( anAccessRights & KL_FILEMAP_WRITE) {
    aFileFlags = O_RDWR;
    aFileMapping->theMapProt |= PROT_WRITE;
    aFileMapping->theMapFlags = MAP_SHARED;
  }
  if ( anAccessRights & KL_FILEMAP_EXECUTE ){
    if ( !aFileFlags )
      aFileFlags = O_RDONLY;
    aFileMapping->theMapProt |= PROT_EXEC;
    aFileMapping->theMapFlags = MAP_SHARED;
  }
  if ( anAccessRights & KL_FILEMAP_COPY_ON_WRITE ) {
    aFileFlags = O_RDONLY;
    aFileMapping->theMapProt |= PROT_READ|PROT_WRITE;
    aFileMapping->theMapFlags = MAP_PRIVATE;
  }

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
    aFileMapping->theAdvise = POSIX_MADV_NORMAL;
    break;
  case ahSequentialAccess:
    aFileMapping->theAdvise = POSIX_MADV_SEQUENTIAL;
    break;
  case ahRandomAccess:
    aFileMapping->theAdvise = POSIX_MADV_RANDOM;
    break;
  default:
    return -1;
  }
  aFileMapping->theFile = open ( aFileName, aFileFlags );
  if (aFileMapping->theFile == -1)
    return -1;
#elif defined (_WIN32)
  if ( !aFileName || !aFileMapping ) {
    SetLastError ( ERROR_BAD_ARGUMENTS );
    return -1;
  }
  int aFileFlags = 0;
  int aMapFlags = 0;
  int aFileAttrs = 0;
  
#ifndef _WIN32_WCE		// WCE does not support FILEMAP_EXECUTE		- by ArtemK
  if ( anAccessRights & KL_FILEMAP_EXECUTE ) {
    OSVERSIONINFOEX aOSInfo;
    ZeroMemory ( &aOSInfo, sizeof (aOSInfo));
    aOSInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
    if ( !GetVersionEx ( (OSVERSIONINFO*)&aOSInfo ) ) {
	  aOSInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	  if ( !GetVersionEx ( (OSVERSIONINFO*)&aOSInfo ) )
	    return -1;
	}
    if ( ( aOSInfo.dwMajorVersion < 5 ) ||
         ( aOSInfo.dwMajorVersion < 1 ) ||
	     ( ( aOSInfo.dwMinorVersion == 1 ) && ( aOSInfo.wServicePackMajor < 2 ) ) ) 
	  anAccessRights &= ~KL_FILEMAP_EXECUTE;
  }
#endif

  switch (anAccessRights) {

  case KL_FILEMAP_READ:
    aFileFlags = GENERIC_READ;
    aMapFlags = PAGE_READONLY;
    aFileMapping->theMapAccess = FILE_MAP_READ;
    break;
  case (KL_FILEMAP_READ|KL_FILEMAP_WRITE):
    aFileFlags =  GENERIC_READ | GENERIC_WRITE;
    aMapFlags = PAGE_READWRITE;
    aFileMapping->theMapAccess = FILE_MAP_WRITE;
    break;

#ifndef _WIN32_WCE							// by ArtemK
  case KL_FILEMAP_COPY_ON_WRITE:
    aFileFlags = GENERIC_READ|GENERIC_WRITE;
    aMapFlags = PAGE_WRITECOPY;
    aFileMapping->theMapAccess = FILE_MAP_COPY;
    break;
  case (KL_FILEMAP_READ|KL_FILEMAP_EXECUTE):
    aFileFlags =  GENERIC_READ|GENERIC_EXECUTE;
    aMapFlags = PAGE_EXECUTE_READ;
    aFileMapping->theMapAccess = FILE_MAP_READ|FILE_MAP_EXECUTE;
    break;
  case (KL_FILEMAP_READ|KL_FILEMAP_WRITE|KL_FILEMAP_EXECUTE):
    aFileFlags =  GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE;
    aMapFlags = PAGE_EXECUTE_READWRITE;
    aFileMapping->theMapAccess = FILE_MAP_READ|FILE_MAP_WRITE|FILE_MAP_EXECUTE;
    break;
  case (KL_FILEMAP_EXECUTE|KL_FILEMAP_COPY_ON_WRITE):
    aFileFlags =  GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE;
    aMapFlags = PAGE_EXECUTE_WRITECOPY;
    aFileMapping->theMapAccess = FILE_MAP_READ|FILE_MAP_WRITE|FILE_MAP_EXECUTE;
    break;
#endif

  default:
    SetLastError ( ERROR_BAD_ARGUMENTS );
    return -1;
  }

  switch (ahAccessHints) {
  case ahNormalAccess:
    break;
  case ahSequentialAccess:
    aFileAttrs |= FILE_FLAG_SEQUENTIAL_SCAN;
    break;
  case ahRandomAccess:
    aFileAttrs |= FILE_FLAG_RANDOM_ACCESS;
    break;
  default:
    SetLastError ( ERROR_BAD_ARGUMENTS );
    return -1;
  }

#ifdef _WIN32_WCE	// by ArtemK
  aFileMapping->theFile = CreateFileForMapping(aFileName, aFileFlags, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, aFileAttrs, 0);
#else
  aFileMapping->theFile = CreateFile ( aFileName, aFileFlags, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,OPEN_EXISTING,aFileAttrs,0 );
#endif

  if ( aFileMapping->theFile == INVALID_HANDLE_VALUE )
    return -1;	
  DWORD aFileSizeLow = 0;
  DWORD aFileSizeHigh = 0;
  aFileSizeLow = GetFileSize ( aFileMapping->theFile, &aFileSizeHigh );
  if ( !aFileSizeLow && !aFileSizeHigh ) {
    aFileMapping->theMapping = INVALID_HANDLE_VALUE;
    return 0;
  }
  aFileMapping->theMapping = CreateFileMapping ( aFileMapping->theFile, 0, aMapFlags, 0, 0, 0 );
  if ( !aFileMapping->theMapping ) {
    DWORD anError = GetLastError ();
    CloseHandle ( aFileMapping->theFile );
    SetLastError ( anError );
    return -1;
  }
#else 
#error "Implementation is needed!"  
#endif
  return 0;
}

int fm_destroy ( FileMapping* aFileMapping )
{
#if defined (__unix__)  
  if ( !aFileMapping ) {
    errno = EINVAL;
    return -1;
  }
  if ( aFileMapping->theFile != -1 )
    close ( aFileMapping->theFile );
  aFileMapping->theFile = -1;
#elif defined (_WIN32)
  if ( !aFileMapping ) {
    SetLastError ( ERROR_BAD_ARGUMENTS );
    return -1;
  }
  if ( aFileMapping->theMapping != INVALID_HANDLE_VALUE  )
    CloseHandle ( aFileMapping->theMapping );
  aFileMapping->theMapping = INVALID_HANDLE_VALUE;
  if ( aFileMapping->theFile != INVALID_HANDLE_VALUE )
    CloseHandle ( aFileMapping->theFile );
  aFileMapping->theFile = INVALID_HANDLE_VALUE;
#else 
#error "Implementation is needed!"  
#endif
  return 0;
}

void* fm_map ( FileMapping* aFileMapping, kl_off_t anOffset, kl_size_t aNumberOfBytes,  kl_size_t* aNumberOfMappedBytes )
{
  void* aResult = 0;
#if defined (__unix__)  
  if ( !aFileMapping ) {
    errno = EINVAL;
    return 0;
  }

  if ( !aNumberOfMappedBytes ) {
    errno = EINVAL;
    return 0;
  }
  struct stat aStat;
  if ( fstat ( aFileMapping->theFile, &aStat ) )
    return 0;
  if ( !aStat.st_size ) {
    *aNumberOfMappedBytes = 0;
    return (void*)-1;
  }
  if ( !aNumberOfBytes )
    aNumberOfBytes = aStat.st_size;

  aResult = mmap ( 0, aNumberOfBytes, aFileMapping->theMapProt, aFileMapping->theMapFlags, aFileMapping->theFile, anOffset );
#if defined (HAVE_MADVISE)
  if ( aResult )
    madvise ( aResult, aNumberOfBytes, aFileMapping->theAdvise );
#endif // HAVE_MADVISE
#elif defined (_WIN32)
  if ( !aFileMapping ) {
    SetLastError ( ERROR_BAD_ARGUMENTS );
    return 0;
  }
  if ( !aNumberOfMappedBytes ) {
    SetLastError ( ERROR_BAD_ARGUMENTS );
    return 0;
  }

  if ( aFileMapping->theMapping == INVALID_HANDLE_VALUE  ) {
    *aNumberOfMappedBytes = 0;
    return (void*)-1;
  }

  Offset2QWord aConvertor = {0};
  aConvertor.theOffset = anOffset;
  aResult = MapViewOfFile ( aFileMapping->theMapping,
                            aFileMapping->theMapAccess,
                            aConvertor.theQWord.Hi,
                            aConvertor.theQWord.Low,
                            aNumberOfBytes );
  if ( !aNumberOfBytes ) {
    aConvertor.theQWord.Low = GetFileSize ( aFileMapping->
		theFile, &aConvertor.theQWord.Hi );
    aNumberOfBytes = aConvertor.theQWord.Low;
  }
    
#else
#error "Implementation is needed!"  
#endif   
  if ( aResult && aNumberOfMappedBytes )
    *aNumberOfMappedBytes = aNumberOfBytes;
  return aResult;
}

int fm_flush ( void* anAddress, kl_size_t aNumberOfBytes )
{
#if defined (__unix__)  
  if ( msync ( anAddress, aNumberOfBytes, MS_SYNC ) )
    return -1;
#elif defined (_WIN32)
  if ( !FlushViewOfFile ( anAddress, aNumberOfBytes ) )
    return -1;
#else 
#error "Implementation is needed!"  
#endif  
  return 0;
}

int fm_unmap ( void* anAddress, kl_size_t aNumberOfBytes )
{
#if defined (__unix__)  
  if ( munmap ( anAddress, aNumberOfBytes ) )
    return -1;
#elif defined (_WIN32)
  if ( !UnmapViewOfFile ( anAddress ) )
    return -1;
#else
#error "Implementation is needed!"  
#endif  
  return 0;  
}

