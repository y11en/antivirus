// -*- C++ -*- Time-stamp: <03/01/22 13:40:04 ptr>

/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file _conv
 * \author Petr Ovchenkov
 * \date 2003
 * \brief 
 *
 */

#ifndef __KL_STD__CONV_H
#define __KL_STD__CONV_H

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: _conv.h,v 1.1 2003/01/22 11:18:41 ptr Exp $"
#  else
#ident "@(#)$Id: _conv.h,v 1.1 2003/01/22 11:18:41 ptr Exp $"
#  endif
#endif

// #include <cwchar>
#include <string> // wchar included here

// Experimental conversion

class _conv
{
  public:
    _conv();
    ~_conv();


    std::string narrow( const wchar_t * );
    // std::string narrow( const wchar_t *, const std::locale& );
    std::string narrow( const std::wstring& );
    // std::string narrow( const std::wstring&, const std::locale& );
    // std::wstring widen( const char * );
    std::wstring widen( const std::string& );

  private:
    class Init
    {
      public:
        Init();
        ~Init();
      private:
        static int& _count;
    };
};



#endif // __KL_STD__CONV_H
