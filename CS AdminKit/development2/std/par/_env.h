/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file _conv
 * \author Petr Ovchenkov
 * \date 2003
 * \brief 
 *
 */

#ifndef __STD__ENV_H
#define __STD__ENV_H

#if defined(__unix) || defined(__MWERKS__)
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: _env.h,v 1.4 2003/05/15 17:22:29 ptr Exp $"
#  else
#ident "@(#)$Id: _env.h,v 1.4 2003/05/15 17:22:29 ptr Exp $"
#  endif
#endif

#include <list>
#include <string>
#include <functional>
#include "../conv/_conv.h"


template <class string_type>
class Environment
{
  public:
    typedef string_type value_type;
    typedef string_type key_type;

    typedef std::pair<key_type,value_type> record_type;

    typedef std::list<record_type> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;

  private:
    struct k_equal :
        public std::binary_function<record_type, key_type, bool>
    {
        bool operator()( const Environment::record_type& r, const Environment::key_type& k ) const
          { return r.first == k; }
    };

    typedef k_equal _Compare;

  public:

    Environment();
    ~Environment();

    static value_type get( const key_type& key );
    static value_type get( const wchar_t *key );

    static void set( const key_type& key, const value_type& value, bool over = true );
    static void unset( const key_type& key );

    static const container_type& get() // MT unsafe!!!
      { return _env; }

  private:
    class Init
    {
      public:
        Init();
        ~Init();
      private:
        static int& _count;
    };

    static void _take_list();

    static container_type _env;
    static _Compare _eq;
    static _conv _c;
    friend class Environment::Init;
};

template<>
class Environment<std::string>
{
  public:
    typedef std::string value_type;
    typedef std::string key_type;

    typedef std::pair<key_type,value_type> record_type;

    typedef std::list<record_type> container_type;
    typedef container_type::iterator iterator;
    typedef const container_type::iterator const_iterator;

  private:
    struct k_equal :
        public std::binary_function<Environment::record_type,Environment::key_type,bool>
    {
        bool operator()( const Environment::record_type& r, const Environment::key_type& k ) const
          { return r.first == k; }
    };

    typedef Environment::k_equal _Compare;

  public:

    Environment();
    ~Environment();

    static value_type get( const key_type& key );
    static value_type get( const char *key );

    static void set( const key_type& key, const value_type& value, bool over = true );
    static void unset( const key_type& key );

    static const container_type& get() // MT unsafe!!!
      { return _env; }

  private:
    class Init
    {
      public:
        Init();
        ~Init();
      private:
        static int& _count;
    };

    static void _take_list();
   
    static container_type _env;
    static _Compare _eq;
    static _conv _c;
    friend class Environment<std::string>::Init;
};

template<>
class Environment<std::wstring>
{
  public:
    typedef std::wstring value_type;
    typedef std::wstring key_type;

    typedef std::pair<key_type,value_type> record_type;

    typedef std::list<record_type> container_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;

  private:
    struct k_equal :
        public std::binary_function<Environment::record_type,Environment::key_type,bool>
    {
        bool operator()( const Environment::record_type& r, const Environment::key_type& k ) const
          { return r.first == k; }
    };

    typedef Environment::k_equal _Compare;

  public:

    Environment();
    ~Environment();

    static value_type get( const key_type& key );
    static value_type get( const wchar_t *key );

    static void set( const key_type& key, const value_type& value, bool over = true );
    static void unset( const key_type& key );

    static const container_type& get() // MT unsafe!!!
      { return _env; }

  private:
    class Init
    {
      public:
        Init();
        ~Init();
      private:
        static int& _count;
    };

    static void _take_list();
   
    static container_type _env;
    static _Compare _eq;
    static _conv _c;
    friend class Environment<std::wstring>::Init;
};

typedef Environment<std::wstring> Environment_wide;

#endif // __STD__ENV_H

// Local Variables:
// mode: C++
// End:
