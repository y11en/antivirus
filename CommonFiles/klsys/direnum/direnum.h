#if !defined (_DIRENUM_H_)
#define _DIRENUM_H_

#if defined (_WIN32)
#include <windows.h>
#elif defined (__unix__)
#include <stddef.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#ifndef MAX_PATH
#include <limits.h>
#if defined(__OpenBSD__) || defined(__FreeBSD__)
#include <sys/syslimits.h> 
#endif
#define MAX_PATH PATH_MAX
#endif
#else
#error "Unknown platform"
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

namespace KLSysNS {
  
  class DirectoryEnumerator {
  public:
    DirectoryEnumerator ();
    
    DirectoryEnumerator ( const char* aPath );

    ~DirectoryEnumerator ();

    bool isOpened () const;

    bool open ( const char* aPath );

    void close ();

    /// function to move file pointer to a next entry
    /// @param aPattern - pattern to be matched in pathnames
    /// @param aDirectoryPattern -  true, if pattern has to be applied to directories 
    ///    ( function will move file pointer ONLY to files and directories with the pattern in the name )
    ///                             false otherwise 
    ///    ( function will move file pointer to files with the pattern in the name and ALL directories )
    /// @result true if file pointer has been moved
    ///          false if no more files left
    bool nextEntry ( const char* aPattern = 0, bool aDirectoryPattern = true );

    const char* getFileName () const;
    
    bool isDirectory () const;

#if defined (_WIN32)    
    DirectoryEnumerator ( const wchar_t* aPath );
    bool open ( const wchar_t* aPath );
    bool nextEntry ( const wchar_t* aPattern = 0, bool aDirectoryPattern = true );
    const wchar_t* getFileNameW () const;
#endif

  private:
#if defined (_WIN32)
    HANDLE theDirectory;
    unsigned char theDataPlace [ sizeof ( WIN32_FIND_DATAW ) ];
    bool isFirstEntry;
    bool isUnicode; 
#elif defined (__unix__)
    DIR* theDirectory;
    union {
      dirent theDirEntry;
#if defined (__sun__)
      char aDummy [ offsetof(struct dirent,d_name) + PATH_MAX + 1 ];
#endif
    };
    char thePath [PATH_MAX];
#else
#error "Unknown platform"
#endif
	
  };

} // KLSysNS

#endif //_DIRENUM_H_
