#ifndef __BLACKLIST_H__
#define __BLACKLIST_H__

#if defined (_MSC_VER)
#pragma warning (disable : 4786)
#endif

#include "platform_compat.h"
#include <vector>
#include "lic_defs.h"
#include "keyfile.h"

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif


namespace LicensingPolicy {

//-----------------------------------------------------------------------------

typedef std::vector<key_serial_number_t> serial_number_list_t;

class CBlacklist
{
public:

  typedef enum
  {
    rcUnknown = 0,
    rcOK,
    rcInvalidParam,
    rcUnknownError,
    rcOutOfMemory,
    rcFileNotFound,
    rcCorruptedFile,
    rcReadError
  } ResultCodeT;

  //---------------------------------------------------------
   
                               CBlacklist    ();
                               ~CBlacklist   ();

  ResultCodeT                  loadBlackList (const _TCHAR* fileName);
  ResultCodeT                  loadBlackList (const char* buf,
                                              unsigned int size
                                             );

  const serial_number_list_t&  getSerNumList () const;
  bool                         findKey       (const key_serial_number_t&) const;
  
private:

  ResultCodeT          readFile         (const _TCHAR   *fileName,
                                         char           **buf,
                                         size_t         *bufSize
                                        );
  ResultCodeT          doLoadBlacklist  (const char *buf,
                                         size_t     bufSize
                                        );
  bool                 get              (const char   *buf,
                                         size_t bufSize,
                                         size_t pos,
                                         string_t* s, 
                                         size_t length
                                        );
  bool                 get              (const char       *buf,
                                         size_t     bufSize,
                                         size_t     pos, 
                                         dword_t    *value
                                        );
  bool                 get              (const char       *buf,
                                         size_t     bufSize,
                                         size_t     pos, 
                                         key_serial_number_t* value
                                        );

  bool                 haveEnoughData   (size_t     bufSize,
                                         size_t     pos, 
                                         size_t     size
                                        );
#if defined (WIN32) && defined (_UNICODE)
  void                 charToUnicode     (const char* s, std::wstring* ws);
#endif

  dword_t               m_blackLstFileVer;
  string_t              m_modificationDate;
  string_t              m_description;
  serial_number_list_t  m_serNumList;
 
};

//-----------------------------------------------------------------------------

inline CBlacklist::CBlacklist ()
{
}

//-----------------------------------------------------------------------------

inline CBlacklist::~CBlacklist ()
{
}

//-----------------------------------------------------------------------------

inline const serial_number_list_t& CBlacklist::getSerNumList () const
{
  return (m_serNumList);
}

//-----------------------------------------------------------------------------

} // namespace LicensingPolicy

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
#pragma warning (pop)
#endif

#endif
