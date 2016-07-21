# if !defined( __expat_compat_h__ )
# define  __expat_compat_h__

#if defined(WIN32) || defined(_WIN32) || defined(OS_WIN32) || defined(_MSC_VER)
#   include <gnu/expat-1.95.2/lib/expat.h>
# else
#   include <expat.h>
# endif  // WIN32

# endif //__expat_compat_h__
