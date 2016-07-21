#if !defined( __LibSigs_link_h__ )
#define __LibSigs_link_h__

#if defined(__LIB_LINK_H_PATH)
#pragma message( "Warning: __LIB_LINK_H_PATH was already defined." )
#undef __LIB_LINK_H_PATH
#endif

#define __LIB_LINK_H_PATH "smtp-filter/libsigs/libsigs"

#include <lib.link.h>

#undef __LIB_LINK_H_PATH

#endif  // __LibSigs_link_h__
