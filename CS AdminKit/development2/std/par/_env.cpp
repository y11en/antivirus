// -*- C++ -*- Time-stamp: <03/05/06 18:22:10 ptr>

/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file _conv
 * \author Petr Ovchenkov
 * \date 2003
 * \brief 
 *
 */

#if defined(__unix) || defined(__MWERKS__)
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: _env.cpp,v 1.3 2003/05/08 09:06:08 ptr Exp $"
#  else
#ident "@(#)$Id: _env.cpp,v 1.3 2003/05/08 09:06:08 ptr Exp $"
#  endif
#endif

#ifdef __unix
#  include <unistd.h>
#  include <cstdlib>
#	include <boost/thread/recursive_mutex.hpp>
#endif

#ifdef N_PLAT_NLM
 #include <mt/xmt.h>
#endif

#include "_env.h"

#ifdef N_PLAT_NLM
 static __impl::Mutex _F_lock;
 #define _F_locklock   _F_lock.lock();
 #define _F_lockunlock _F_lock.unlock();
 #define _F_scopedlock   MT_REENTRANT( _F_lock, _x1);
#else
static boost::recursive_mutex _F_lock;
#define _F_scopedlock	boost::detail::thread::scoped_lock<boost::recursive_mutex> locker( _F_lock );
#define _F_locklock		boost::detail::thread::lock_ops<boost::recursive_mutex>::lock( _F_lock );
#define _F_lockunlock	boost::detail::thread::lock_ops<boost::recursive_mutex>::unlock( _F_lock );
#endif
static int Init_count_n = 0;
static int Init_count_w = 0;

static char *Init_buf[32];
int& Environment<std::string>::Init::_count( ::Init_count_n ); // trick to avoid friend declarations
int& Environment<std::wstring>::Init::_count( ::Init_count_w ); // trick to avoid friend declarations

Environment<std::string>::container_type Environment<std::string>::_env;
Environment<std::string>::_Compare Environment<std::string>::_eq;
_conv Environment<std::string>::_c;

Environment<std::wstring>::container_type Environment<std::wstring>::_env;
Environment<std::wstring>::_Compare Environment<std::wstring>::_eq;
_conv Environment<std::wstring>::_c;

static Environment<std::wstring> _my_env;

#ifdef __FreeBSD__
char *environ[] = { 0, 0, 0 };
#endif

Environment<std::string>::Init::Init()
{  
#ifdef __FreeBSD__
	environ[0] = 0;
#endif
	
  _F_scopedlock;

  if ( _count++ == 0 ) {
    Environment<std::string>::_take_list();
  }
}

Environment<std::string>::Init::~Init()
{
  _F_scopedlock;

  if ( --_count == 0 ) {
    // nothing
  }
}

Environment<std::string>::Environment()
{
  new( Init_buf ) Init();
}

Environment<std::string>::~Environment()
{
  ((Init *)Init_buf)->~Init();
}

Environment<std::wstring>::Init::Init()
{
  _F_scopedlock;

  if ( _count++ == 0 ) {
    Environment<std::wstring>::_take_list();
  }
}

Environment<std::wstring>::Init::~Init()
{
  _F_scopedlock;

  if ( --_count == 0 ) {
    // nothing
  }
}

Environment<std::wstring>::Environment()
{
  new( Init_buf ) Init();
}

Environment<std::wstring>::~Environment()
{
  ((Init *)Init_buf)->~Init();
}

Environment<std::string>::value_type Environment<std::string>::get( const value_type& key )
{
  value_type v;
  _F_scopedlock;
  v = getenv( key.c_str() ); //
  
  return v;
}

Environment<std::string>::value_type Environment<std::string>::get( const char *key )
{
  value_type v;
  _F_scopedlock;
  v = getenv( key ); // 

  return v;
}

void Environment<std::string>::set( const key_type& key, const value_type& value, bool over )
{
  _F_scopedlock;
  setenv( key.c_str(), value.c_str(), over ? 1 : 0 );
  iterator i = find_if( _env.begin(), _env.end(), std::bind2nd(_eq, key) );
  if ( i == _env.end() ) {
    _env.push_back( record_type( key, value ) );
  } else if ( over ) {
    (*i).second = value;
  }
}

void Environment<std::string>::unset( const key_type& key )
{
  _F_scopedlock;
  unsetenv( key.c_str() );
  iterator i = find_if( _env.begin(), _env.end(), std::bind2nd(_eq, key) );
  if ( i != _env.end() ) {
    _env.erase( i );
  }
}

void Environment<std::string>::_take_list()
{
#ifdef __unix
  for ( char **e = ::environ; int(*e) != int(0); ++e ) {
    std::string s( *e );
    std::string::size_type p = s.find( '=' );
    _env.push_back( record_type( s.substr( 0, p ), s.substr( p + 1 ) ) );
  }
#endif

#ifdef WIN32
#  ifdef _UNICODE
  wchar_t *e = GetEnvironmentStringsW();
  wchar_t *env = e;
  
  while ( wint_t(*e) != wint_t(0) ) {
    std::wstring s( e );
    std::wstring::size_type p = s.find( L'=' );
    _env.push_back( record_type( _c.narrow( s.substr( 0, p ) ), _c.narrow( s.substr( p + 1 ) ) ) );
    e += s.size() + 1;
  }
  FreeEnvironmentStrings( env );
#  else // !_UNICODE
  char *e = GetEnvironmentStringsA();
  char *env = e;
  
  while ( int(*e) != int(0) ) {
    std::string s( e );
    std::string::size_type p = s.find( '=' );
    _env.push_back( record_type( s.substr( 0, p ), s.substr( p + 1 ) ) );
    e += s.size() + 1;
  }
  free( static_cast<void *>(env) );
#  endif // _UNICODE
#endif // WIN32
}

// *************

Environment<std::wstring>::value_type Environment<std::wstring>::get( const value_type& key )
{
  value_type v;
  _F_scopedlock;
  v = _c.widen( getenv( _c.narrow( key ).c_str() ) );
  
  return v;
}

Environment<std::wstring>::value_type Environment<std::wstring>::get( const wchar_t *key )
{
  value_type v;
  _F_scopedlock;
  v = _c.widen( getenv( _c.narrow( key ).c_str() ) ); //

  return v;
}

void Environment<std::wstring>::set( const key_type& key, const value_type& value, bool over )
{
  _F_scopedlock;
  setenv( _c.narrow( key ).c_str(), _c.narrow( value ).c_str(), over ? 1 : 0 );
  iterator i = find_if( _env.begin(), _env.end(), std::bind2nd(_eq, key) );
  if ( i == _env.end() ) {
    _env.push_back( record_type( key, value ) );
  } else if ( over ) {
    (*i).second = value;
  }
}

void Environment<std::wstring>::unset( const key_type& key )
{
  _F_scopedlock;
  unsetenv( _c.narrow( key ).c_str() );
  iterator i = find_if( _env.begin(), _env.end(), std::bind2nd(_eq, key) );
  if ( i != _env.end() ) {
    _env.erase( i );
  }
}

void Environment<std::wstring>::_take_list()
{
#ifdef __unix
  for ( char **e = ::environ; int(*e) != int(0); ++e ) {  	
    std::string s( *e );
    std::string::size_type p = s.find( '=' );
    _env.push_back( record_type( _c.widen( s.substr( 0, p ) ), _c.widen( s.substr( p + 1 ) ) ) );
  }
#endif

#ifdef WIN32
#  ifdef _UNICODE
  wchar_t *e = GetEnvironmentStringsW();
  wchar_t *env = e;
  
  while ( wint_t(*e) != wint_t(0) ) {
    std::wstring s( e );
    std::wstring::size_type p = s.find( L'=' );
    _env.push_back( record_type( s.substr( 0, p ), s.substr( p + 1 ) ) );
    e += s.size() + 1;
  }
  FreeEnvironmentStrings( env );
#  else // !_UNICODE
  char *e = GetEnvironmentStringsA();
  char *env = e;
  
  while ( int(*e) != int(0) ) {
    std::string s( e );
    std::string::size_type p = s.find( '=' );
    _env.push_back( record_type( _c.widen( s.substr( 0, p ) ), _c.widen( s.substr( p + 1 ) ) ) );
    e += s.size() + 1;
  }
  free( static_cast<void *>(env) );
#  endif // _UNICODE
#endif // WIN32
}
