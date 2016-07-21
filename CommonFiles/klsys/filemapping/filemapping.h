#if !defined (_CPH_FILEMAP_H_)
#define _CPH_FILEMAP_H_

#if defined (__unix__)
#include <sys/types.h>
#elif defined (_WIN32)
#include <windows.h>
#endif

namespace KLSysNS {

#define KL_FILEMAP_NONE             0
#define KL_FILEMAP_READ             1<<0
#define KL_FILEMAP_WRITE            1<<1
#define KL_FILEMAP_EXECUTE          1<<2
#define KL_FILEMAP_COPY_ON_WRITE    1<<3

  class FileMapping {
  public:
    enum AccessHints  { ahNormalAccess, ahSequentialAccess, ahRandomAccess };

#if defined (__unix__)
    typedef off_t kl_off_t;
    typedef size_t kl_size_t;
#elif defined (_WIN32)
    typedef __int64 kl_off_t;
    typedef SIZE_T kl_size_t;
#endif

    FileMapping ();

    ~FileMapping () 
    { 
      destroy ();
    }

    /// function to create file mapping
    /// @param aFileName - name of file to be mapped
    /// @param anAccessRights - bitwise OR of flags KL_FILEMAP_*
    /// @param anAccessHints - hints for filemapping
    /// @result 0 if success, -1 otherwise
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    int create ( const char* aFileName, int anAccessRights, AccessHints ahAccessHints = ahNormalAccess );

#if defined (_WIN32)
    int create ( const wchar_t* aFileName, int anAccessRights, AccessHints ahAccessHints = ahNormalAccess );
#endif

    /// function to destroy file mapping
    /// @result 0 if success, -1 otherwise
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    int destroy ();

    /// function to create file mapping
    /// @param anOffset - starting offset in the file. Should be multiple of page size;
    /// @param aNumberOfBytes - number of bytes to be mapped. If it is 0, then entire of the file will be mapped
    /// @param aNumberOfMappedBytes - mandatory pointer to the variable that receives a number of mapped bytes
    /// @result memory address if the function is succeeds, 0 or -1 otherwise
    /// result is -1 in case of emply mapped file
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    void* map ( kl_off_t anOffset, kl_size_t aNumberOfBytes,  kl_size_t* aNumberOfMappedBytes );

    /// function to flush changes back into the file
    /// @param anAddress - mapped address
    /// @param aNumberOfBytes - number of bytes to be flushed
    /// @result 0 if success, -1 otherwise
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    int flush ( void* anAddress, kl_size_t aNumberOfBytes );

    /// function to unmap mapped address
    /// @param anAddress - mapped address
    /// @param aNumberOfBytes - number of bytes to be unmapped
    /// @result 0 if success, -1 otherwise
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    int unmap ( void* anAddress, kl_size_t aNumberOfBytes );

  private:
#if defined (_WIN32)
    int create_impl ( const void* aFileName,
                      int anAccessRights, 
                      AccessHints ahAccessHints,
		 			  bool anUnicodeFileName );
    HANDLE theFile;
    HANDLE theMapping;
    int theMapAccess;
#elif defined (__unix__)
    int theFile;
    int theMapFlags;
    int theMapProt;
    int theAdvise;
#else
#error "This platform is unknown."
#endif
  };
  
} // KLSysNS
#endif
