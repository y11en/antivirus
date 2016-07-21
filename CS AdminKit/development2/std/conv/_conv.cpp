// -*- C++ -*- Time-stamp: <03/05/13 14:39:43 ptr>

/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file _conv
 * \author Petr Ovchenkov
 * \date 2003
 * \brief 
 *
 */

#if defined(__unix) || defined(N_PLAT_NLM)
#	include <cwchar>
#	include <locale>
#endif

#if defined(__unix) 
#	include <boost/thread/mutex.hpp>
#endif

#include <algorithm>
#include <sstream>
// #include <iostreams>
#include <iterator>

#ifdef N_PLAT_NLM 		// Novell Netware
# include <unicode.h>
# include <nunicode.h>
#endif

#include "_conv.h"

using namespace std;

#if defined(__unix) 
boost::mutex _F_lock;
#define _F_locklock   boost::detail::thread::lock_ops<boost::mutex>::lock( _F_lock );
#define _F_lockunlock boost::detail::thread::lock_ops<boost::mutex>::unlock( _F_lock );
#endif

#ifdef N_PLAT_NLM
#define _F_locklock   ;
#define _F_lockunlock ;
#endif


static int Init_count = 0;
static char *Init_buf[32];
// int Thread::Init::_count = 0;
int& _conv::Init::_count( ::Init_count ); // trick to avoid friend declarations

_conv::Init::Init()
{
#if defined(__unix) || defined(N_PLAT_NLM)
  // This is performance problem, let's consider replace to atomic lock...
  _F_locklock

  if ( _count++ == 0 ) {
# ifdef __unix
    std::locale::global( std::locale() );
# endif
# ifdef N_PLAT_NLM
    NWUSStandardUnicodeInit();
# endif
  }

  _F_lockunlock
#endif
}

_conv::Init::~Init()
{
#if defined(__unix) || defined(N_PLAT_NLM)
  // This is performance problem, let's consider replace to atomic lock...
  _F_locklock

  if ( --_count == 0 ) {
    // nothing
# ifdef N_PLAT_NLM
//    NWUXUnloadConverter( sysconv );
    NWUSStandardUnicodeRelease();
# endif
  }

  _F_lockunlock
#endif
}

_conv::_conv()
{
  new( Init_buf ) Init();
}

_conv::~_conv()
{
  ((Init *)Init_buf)->~Init();
}

std::string _conv::narrow( const wchar_t *c )
{
#ifdef N_PLAT_NLM // temporary locale workaround
  nuint len;
  string::size_type l = wcslen( c );
  string ns( l, ' ' );
  NWUSUnicodeToByte( (nuint8 *)ns.data(), ns.length() + 1,
                     reinterpret_cast<const unicode *>(c), &len );
  return ns;
#else // !N_PLAT_NLM
# ifdef _STLPORT_VERSION
  ostringstream st;
  // locale l( "en_US.ISO8859-1" );
  // st1.imbue( l );

  ostream_iterator<char> oi( st );

  copy( c, c + ::wcslen(c), oi );

  return st.str();
# else
  string::size_tipe l = ::wcslen(c);
  string ns( l, L' ' );
 
  copy( c, c + l, ns.begin() );

  return ns;
# endif
#endif // !N_PLAT_NLM
}

std::string _conv::narrow( const wstring& s )
{
#ifdef N_PLAT_NLM // temporary locale workaround
  nuint len;
  string ns( s.length(), ' ' );
  NWUSUnicodeToByte( (nuint8 *)ns.data(), ns.length() + 1,
                     reinterpret_cast<const unicode *>(s.data()), &len );
  return ns;
#else // !N_PLAT_NLM
# ifdef _STLPORT_VERSION
  ostringstream st;
  // locale l( "en_US.ISO8859-1" );
  // st1.imbue( l );

  ostream_iterator<char> oi( st );

  copy( s.begin(), s.end(), oi );

  return st.str();
# else // !_STLPORT_VERSION
  string ns( s.length(), ' ' );

  copy( s.begin(), s.end(), ns.begin() );

  return ns;
# endif // !_STLPORT_VERSION
#endif // !N_PLAT_NLM
}

std::wstring _conv::widen( const string& s )
{
#ifdef N_PLAT_NLM // temporary locale workaround
  nuint len;
  wstring ws( s.length()+1, L'\0' );
  NWUSByteToUnicode( (punicode)ws.data(), ws.length()+1,
                     reinterpret_cast<const unsigned char *>(s.c_str()), &len );

  return ws;
#else // !N_PLAT_NLM
# ifdef _STLPORT_VERSION
  wostringstream st;
  // locale l( "en_US.ISO8859-1" );
  // st1.imbue( l );

  ostream_iterator<wchar_t,wchar_t> oi( st );

  copy( s.begin(), s.end(), oi );

  return st.str();
# else // !_STLPORT_VERSION
  wstring ws( s.length(), L' ' );

  copy( s.begin(), s.end(), ws.begin() );

  return ws;
# endif // !_STLPORT_VERSION
#endif // !N_PLAT_NLM
}
