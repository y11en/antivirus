#ifndef __port_novell_h_
#define __port_novell_h_

#ifndef _WCHAR_T_DEFINED
  #define _WCHAR_T_DEFINED
  typedef unsigned short wchar_t;
#endif


#if !defined (_SIZE_T_DEFINED_)
  #define _SIZE_T_DEFINED_

  #if !defined(_SIZE_T)
    #define _SIZE_T
    typedef unsigned size_t;
  #endif
#endif


#ifndef USHRT_MAX
  #define USHRT_MAX ((unsigned short)-1)
#endif

#endif