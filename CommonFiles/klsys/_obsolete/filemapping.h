#if !defined (_CPH_FILEMAP_H_)
#define _CPH_FILEMAP_H_

#if defined (__unix__)
#include <sys/types.h>
typedef off_t kl_off_t;
typedef size_t kl_size_t;
#elif defined (_WIN32)
#include <windows.h>
typedef __int64 kl_off_t;
typedef SIZE_T kl_size_t;
#endif

#define KL_FILEMAP_NONE             0
#define KL_FILEMAP_READ             1<<0
#define KL_FILEMAP_WRITE            1<<1
#define KL_FILEMAP_EXECUTE          1<<2
#define KL_FILEMAP_COPY_ON_WRITE    1<<3

enum AccessHints  { ahNormalAccess, ahSequentialAccess, ahRandomAccess };

struct FileMapping;

#if defined (__cplusplus)
extern "C" {
#endif // __cplusplus

/// function to create file mapping
/// @param aFileMapping - pointer to instance of filemapping
/// @param aFileName - name of file to be mapped
/// @param anAccessRights - bitwise OR of flags KL_FILEMAP_*
/// @param anAccessHints - hints for filemapping
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
int fm_create ( struct FileMapping* aFileMapping, const TCHAR* aFileName, int anAccessRights, enum AccessHints ahAccessHints );

/// function to destroy file mapping
/// @param aFileMapping - pointer to instance of filemapping
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
int fm_destroy ( struct FileMapping* aFileMapping );

/// function to create file mapping
/// @param aFileMapping - pointer to instance of filemapping
/// @param anOffset - starting offset in the file. Should be multiple of page size;
/// @param aNumberOfBytes - number of bytes to be mapped. If it is 0, then entire of the file will be mapped
/// @param aNumberOfMappedBytes - mandatory pointer to the variable that receives a number of mapped bytes
/// @result memory address if the function is succeeds, 0 or -1 otherwise
/// result is -1 in case of emply mapped file
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
void* fm_map ( struct FileMapping* aFileMapping, kl_off_t anOffset, kl_size_t aNumberOfBytes,  kl_size_t* aNumberOfMappedBytes );

/// function to flush changes back into the file
/// @param anAddress - mapped address
/// @param aNumberOfBytes - number of bytes to be flushed
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
int fm_flush ( void* anAddress, kl_size_t aNumberOfBytes );

/// function to unmap mapped address
/// @param anAddress - mapped address
/// @param aNumberOfBytes - number of bytes to be unmapped
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
int fm_unmap ( void* anAddress, kl_size_t aNumberOfBytes );

#if defined (__cplusplus)
}
#endif // __cplusplus

struct FileMapping {
#if defined (__unix__)
#if defined (__cplusplus)
public:
  FileMapping ()
    : theFile (-1), theMapFlags (0), theMapProt(0), theAdvise(0)
  {}
private:
#endif // (__cplusplus
  int theFile;
  int theMapFlags;
  int theMapProt;
  int theAdvise;
#endif // __unix__
#if defined (_WIN32)
#if defined (__cplusplus)
public:
  FileMapping ()
    : theFile (INVALID_HANDLE_VALUE), theMapping (INVALID_HANDLE_VALUE), theMapAccess(0)
  {}
private:
#endif // (__cplusplus
  HANDLE theFile;
  HANDLE theMapping;
  int theMapAccess;
#endif // _WIN32

#if defined (__cplusplus)
public:
  ~FileMapping () 
  { 
    fm_destroy ( this ); 
  }

/// function to create file mapping
/// @param aFileName - name of file to be mapped
/// @param anAccessRights - bitwise OR of flags KL_FILEMAP_*
/// @param anAccessHints - hints for filemapping
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
  int create ( const TCHAR* aFileName, int anAccessRights, AccessHints ahAccessHints = ahNormalAccess )
  {
    return fm_create ( this, aFileName, anAccessRights, ahAccessHints );
  }

/// function to destroy file mapping
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
  int destroy ()
  {
    return fm_destroy ( this );
  }

/// function to create file mapping
/// @param anOffset - starting offset in the file. Should be multiple of page size;
/// @param aNumberOfBytes - number of bytes to be mapped. If it is 0, then entire of the file will be mapped
/// @param aNumberOfMappedBytes - mandatory pointer to the variable that receives a number of mapped bytes
/// @result memory address if the function is succeeds, 0 or -1 otherwise
/// result is -1 in case of emply mapped file
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
  void* map ( kl_off_t anOffset, kl_size_t aNumberOfBytes,  kl_size_t* aNumberOfMappedBytes )
  {
    return fm_map ( this, anOffset, aNumberOfBytes, aNumberOfMappedBytes );
  }

/// function to flush changes back into the file
/// @param anAddress - mapped address
/// @param aNumberOfBytes - number of bytes to be flushed
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
  int flush ( void* anAddress, kl_size_t aNumberOfBytes )
  {
    return fm_flush ( anAddress, aNumberOfBytes );
  }

/// function to unmap mapped address
/// @param anAddress - mapped address
/// @param aNumberOfBytes - number of bytes to be unmapped
/// @result 0 if success, -1 otherwise
/// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
  int unmap ( void* anAddress, kl_size_t aNumberOfBytes )
  {
    return fm_unmap ( anAddress, aNumberOfBytes );
  }

  friend int fm_create ( FileMapping*, const TCHAR* , int AccessRights, enum AccessHints );
  friend int fm_destroy ( FileMapping* );
  friend void* fm_map ( FileMapping*, kl_off_t, kl_size_t,  kl_size_t* );
  friend int fm_flush ( void*, kl_size_t );
  friend int fm_unmap ( void*, kl_size_t );
#endif // __cplusplus
};


#endif
