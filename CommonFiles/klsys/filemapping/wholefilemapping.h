#if !defined (_WHOLEFILEMAPPING_H_)
#define _WHOLEFILEMAPPING_H_

#include "filemapping.h"

namespace KLSysNS {

  class WholeFileMapping : private FileMapping {
  public:
    typedef FileMapping::AccessHints AccessHints;
    typedef FileMapping::kl_off_t kl_off_t;
    typedef FileMapping::kl_size_t kl_size_t;

    WholeFileMapping ()
      : theAddress ( 0 ),
        theNumberOfMappedBytes ( 0 ) {}
  
    ~WholeFileMapping () { unmap (); }


    /// function to create file mapping
    /// @param aFileName - name of file to be mapped
    /// @param anAccessRights - bitwise OR of flags KL_FILEMAP_*
    /// @param anAccessHints - hints for filemapping
    /// @param aNumberOfMappedBytes - optional pointer to the variable that receives a number of mapped bytes
    /// @result memory address if the function is succeeds, 0 or -1 otherwise
    /// result is -1 in case of emply mapped file
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    void* map ( const char* aFileName, 
                int anAccessRights,  
                kl_size_t* aNumberOfMappedBytes = 0, 
                AccessHints ahAccessHints = ahNormalAccess )
    {
      if ( theAddress && ( theAddress != reinterpret_cast <void*> ( -1 ) ) )
        unmap ();
      if ( FileMapping::create ( aFileName, anAccessRights, ahAccessHints ) )
        return 0;
      theAddress  = FileMapping::map ( 0, 0, &theNumberOfMappedBytes );
      if ( aNumberOfMappedBytes )
        *aNumberOfMappedBytes = theNumberOfMappedBytes;
      return theAddress;
    }

#if defined (_WIN32)
    void* map ( const wchar_t* aFileName, 
                int anAccessRights,  
                kl_size_t* aNumberOfMappedBytes = 0, 
                AccessHints ahAccessHints = ahNormalAccess )
    {
      if ( theAddress && ( theAddress != reinterpret_cast <void*> ( -1 ) ) )
        unmap ();
      if ( FileMapping::create ( aFileName, anAccessRights, ahAccessHints ) )
        return 0;
      theAddress  = FileMapping::map ( 0, 0, &theNumberOfMappedBytes );
      if ( aNumberOfMappedBytes )
        *aNumberOfMappedBytes = theNumberOfMappedBytes;
      return theAddress;
    }
#endif
  
    /// function to flush changes back into the file
    /// @result 0 if success, -1 otherwise
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    int flush () { return FileMapping::flush ( theAddress, theNumberOfMappedBytes ); };

    /// function to unmap mapped address
    /// @result 0 if success, -1 otherwise
    /// to get a reason of an error see system error ( errno under Unix-like OS, GetLastError () under Windows )
    int unmap () 
    {  
      int aResult = FileMapping::unmap ( theAddress, theNumberOfMappedBytes ); 
      if ( aResult )
        return aResult;
      theAddress = 0;
      theNumberOfMappedBytes = 0;
      aResult = FileMapping::destroy ();
      return aResult;
    }
    /// function to get the mapped area
    void* data () const { return theAddress; }
    
    /// function to get a number of mapped bytes
    kl_size_t size () const { return theNumberOfMappedBytes; }

  private:
    void* theAddress;
    kl_size_t theNumberOfMappedBytes;
  };

} // KLSysNS

#endif // _WHOLEFILEMAPPING_H_
