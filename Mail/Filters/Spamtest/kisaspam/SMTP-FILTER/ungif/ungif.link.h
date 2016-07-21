#if !defined( __ungif_link_h__ )
#define __ungif_link_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__LIB_LINK_H_PATH)
#pragma message( "Warning: __LIB_LINK_H_PATH was already defined." )
#undef __LIB_LINK_H_PATH
#endif

#define __LIB_LINK_H_PATH "smtp-filter/ungif/ungif"

#include <lib.link.h>

#undef __LIB_LINK_H_PATH

#endif  // __ungif_link_h__
