#if defined (_WIN32)

#include "filemapping.h"

// by ArtemK
#ifdef _WIN32_WCE
#define OSVERSIONINFOEX		OSVERSIONINFO
#include <winbase.h>
#endif

#include <malloc.h>

namespace KLSysNS {

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
	FileMapping::kl_off_t theOffset;
    qword theQWord;
  };
#pragma pack ()

#if !defined (FILE_MAP_EXECUTE)
#define FILE_MAP_EXECUTE SECTION_MAP_WRITE
#endif // FILE_MAP_EXECUTE

  FileMapping::FileMapping ()
    : theFile (INVALID_HANDLE_VALUE), theMapping (0), theMapAccess(0)
  {}	

 
  int FileMapping::create ( const char* aFileName,		
                            int anAccessRights, 
                            AccessHints ahAccessHints )
  {
    destroy ();
    return create_impl ( aFileName, anAccessRights, ahAccessHints, false );
  }

  int FileMapping::create ( const wchar_t* aFileName,		
                            int anAccessRights, 
                            AccessHints ahAccessHints )
  {
    destroy ();
    return create_impl ( aFileName, anAccessRights, ahAccessHints, true );
  }
 
  
  int FileMapping::create_impl ( const void* aFileName,
                                 int anAccessRights, 
                                 AccessHints ahAccessHints,
                                 bool anUnicodeFileName )
  {
    if ( !aFileName ) {
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
           ( ( aOSInfo.dwMajorVersion == 5 ) && ( aOSInfo.dwMinorVersion < 1 ) ) ||
           ( ( aOSInfo.dwMajorVersion == 5 ) && ( aOSInfo.dwMinorVersion == 1 ) && ( aOSInfo.wServicePackMajor < 2 ) ) ||
           ( ( aOSInfo.dwMajorVersion == 5 ) && ( aOSInfo.dwMinorVersion == 2 ) && ( aOSInfo.wServicePackMajor < 1 ) ) ) { 
        anAccessRights &= ~KL_FILEMAP_EXECUTE;
        anAccessRights |= KL_FILEMAP_READ;
      }
    }
#endif

    switch (anAccessRights) {

    case KL_FILEMAP_READ:
      aFileFlags = GENERIC_READ;
      aMapFlags = PAGE_READONLY;
      theMapAccess = FILE_MAP_READ;
      break;
    case KL_FILEMAP_WRITE:
    case (KL_FILEMAP_READ|KL_FILEMAP_WRITE):
      aFileFlags =  GENERIC_READ | GENERIC_WRITE;
      aMapFlags = PAGE_READWRITE;
      theMapAccess = FILE_MAP_READ|FILE_MAP_WRITE;
      break;

#ifndef _WIN32_WCE							// by ArtemK
    case KL_FILEMAP_COPY_ON_WRITE:
    case (KL_FILEMAP_READ|KL_FILEMAP_COPY_ON_WRITE):  
      aFileFlags = GENERIC_READ;
      aMapFlags = PAGE_WRITECOPY;
      theMapAccess = FILE_MAP_COPY;
      break;
    case KL_FILEMAP_EXECUTE:
    case (KL_FILEMAP_READ|KL_FILEMAP_EXECUTE):
      aFileFlags =  GENERIC_READ|GENERIC_EXECUTE;
      aMapFlags = PAGE_EXECUTE_READ;
      theMapAccess = FILE_MAP_READ|FILE_MAP_EXECUTE;
      break;
    case (KL_FILEMAP_READ|KL_FILEMAP_WRITE|KL_FILEMAP_EXECUTE):
      aFileFlags =  GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE;
      aMapFlags = PAGE_EXECUTE_READWRITE;
      theMapAccess = FILE_MAP_READ|FILE_MAP_WRITE|FILE_MAP_EXECUTE;
      break;
    case (KL_FILEMAP_EXECUTE|KL_FILEMAP_COPY_ON_WRITE):
      aFileFlags = GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE;
      aMapFlags = PAGE_EXECUTE_WRITECOPY;
      theMapAccess = FILE_MAP_READ|FILE_MAP_WRITE|FILE_MAP_EXECUTE;
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

#ifdef _WIN32_WCE
	theFile = CreateFileForMapping ( reinterpret_cast <const wchar_t*> ( aFileName ),
                                     aFileFlags, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, aFileAttrs, 0);

#else
	static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);
	if ( anUnicodeFileName && g_bUnicodePlatform )
	{
		theFile = CreateFileW ( reinterpret_cast <const wchar_t*> ( aFileName ),
						aFileFlags, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, aFileAttrs, 0);
	}
	else
	{
		if(anUnicodeFileName)
		{
			const wchar_t *_lpw = (const wchar_t *)aFileName;
			int nChars = lstrlenW(_lpw) + 1;
			char *_lpa = (char *)alloca(nChars*sizeof(WCHAR));
			if(!::WideCharToMultiByte(CP_ACP, 0, (wchar_t *)aFileName, -1, _lpa, nChars*sizeof(WCHAR), NULL, NULL))
				return -1;
			theFile = CreateFileA ( _lpa,
				aFileFlags, FILE_SHARE_READ | FILE_SHARE_WRITE | (g_bUnicodePlatform ? FILE_SHARE_DELETE : 0), 0, OPEN_EXISTING, aFileAttrs, 0);
		}
		else
		{
			theFile = CreateFileA ( reinterpret_cast <const char*> ( aFileName ),
				aFileFlags, FILE_SHARE_READ | FILE_SHARE_WRITE | (g_bUnicodePlatform ? FILE_SHARE_DELETE : 0), 0, OPEN_EXISTING, aFileAttrs, 0);
		}
	}
#endif

    if ( theFile == INVALID_HANDLE_VALUE )
      return -1;	
    DWORD aFileSizeLow = 0;
    DWORD aFileSizeHigh = 0;
    aFileSizeLow = GetFileSize ( theFile, &aFileSizeHigh );
    if ( !aFileSizeLow && !aFileSizeHigh ) {
      theMapping = 0;
      return 0;
    }
    theMapping = CreateFileMapping ( theFile, 0, aMapFlags, 0, 0, 0 );
    if ( !theMapping ) {
      DWORD anError = GetLastError ();
      CloseHandle ( theFile );
      theFile = INVALID_HANDLE_VALUE;
      SetLastError ( anError );
      return -1;
    }
    return 0;
  }

  int FileMapping::destroy ()
  {
    if ( theMapping )
      CloseHandle ( theMapping );
    theMapping = 0;
    if ( theFile != INVALID_HANDLE_VALUE )
      CloseHandle ( theFile );
    theFile = INVALID_HANDLE_VALUE;
    return 0;
  }

  void* FileMapping::map ( kl_off_t anOffset, kl_size_t aNumberOfBytes,  kl_size_t* aNumberOfMappedBytes )
  {
    void* aResult = 0;
    if ( !aNumberOfMappedBytes ) {
      SetLastError ( ERROR_BAD_ARGUMENTS );
      return 0;
    }
    *aNumberOfMappedBytes = 0;

    if ( !theMapping )
      return (void*)-1;

    Offset2QWord aConvertor = {0};
    aConvertor.theOffset = anOffset;
    aResult = MapViewOfFile ( theMapping,
                              theMapAccess,
                              aConvertor.theQWord.Hi,
                              aConvertor.theQWord.Low,
                              aNumberOfBytes );
    if ( !aNumberOfBytes ) {
      aConvertor.theQWord.Low = GetFileSize ( theFile, &aConvertor.theQWord.Hi );
      aNumberOfBytes = aConvertor.theQWord.Low;
    }
    if ( aResult )
      *aNumberOfMappedBytes = aNumberOfBytes;
    return aResult;
  }

  int FileMapping::flush ( void* anAddress, kl_size_t aNumberOfBytes )
  {
    if ( !FlushViewOfFile ( anAddress, aNumberOfBytes ) )
      return -1;
    return 0;
  }

  int FileMapping::unmap ( void* anAddress, kl_size_t aNumberOfBytes )
  {
    if ( !UnmapViewOfFile ( anAddress ) )
      return -1;
    return 0;  
  }

}  

#endif // _WIN32
