#ifndef _SUBST_HELPER__INCLUDE_
#define _SUBST_HELPER__INCLUDE_

#include "defs.h"

namespace KLUTIL
{
KLUTIL_API bool Subst2Orig( LPCWSTR szwSubstPath, OUT std::wstring& orig );
KLUTIL_API bool LocalPathToUNC( const TCHAR * szLocalPath, OUT std::basic_string<TCHAR>& szUNCPath );
}

#endif // !defined(_SUBST_HELPER__INCLUDE_)
