#if !defined (__LIC_DEFS_H__)
#define __LIC_DEFS_H__

#if defined (_MSC_VER)
  // disable warning 4786
  // 'identifier' : identifier was truncated to 'number' characters in the 
  // debug information
  #pragma warning (disable : 4786)
  #pragma once
#endif

//-----------------------------------------------------------------------------

#if defined (_WIN32)
  #include <tchar.h>
  #include <windows.h>
#endif

#include <string>
#include <vector>

#include "platform_compat.h"
#include "err_defs.h"
#include "date_t.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

namespace LicensingPolicy {

//-----------------------------------------------------------------------------

#if defined (__unix__)  || defined (__MWERKS__)
  typedef unsigned int		dword_t;
#else
  typedef unsigned long   dword_t;
#endif /* __unix__ */

#if defined (_WIN32)
  typedef DWORD sys_err_code_t;
#else
  typedef int sys_err_code_t;
#endif

typedef unsigned short             word_t;
typedef std::basic_string<_TCHAR>  string_t;
typedef std::vector<string_t>      string_list_t;

typedef struct 
{
  std::string image;
} file_image_t;

typedef long file_size_t;

enum functionality_level_t
{
  flUnknown = 0,
  flNoFeatures,
  flOnlyUpdates,
  flFunctionWithoutUpdates,
  flFullFunctionality
};

//-----------------------------------------------------------------------------
} // namespace LicensingPolicy

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
