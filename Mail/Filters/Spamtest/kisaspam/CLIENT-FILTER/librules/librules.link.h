#if !defined( __librules_link_h__ )
#define __librules_link_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__LIB_LINK_H_PATH)
#pragma message( "Warning: __LIB_LINK_H_PATH was already defined." )
#undef __LIB_LINK_H_PATH
#endif

#ifndef _DEBUG
#define __LIB_LINK_H_PATH "client-filter/bin/librules"
#else
#define __LIB_LINK_H_PATH "client-filter/bind/librules"
#endif

#ifndef KIS_USAGE
    #include <lib.link.h>
#endif

#undef __LIB_LINK_H_PATH

#endif  // __librules_link_h__
