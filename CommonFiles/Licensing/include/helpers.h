#ifndef __HELPERS_H__
#define __HELPERS_H__

#if defined(_MSC_VER)
  #pragma once
#endif

#if defined (_WIN32)
  #include <io.h>
#else
  
  #if defined (__unix__)
    #include <unistd.h>
  #endif

#endif

#include <platform_compat.h>

//-----------------------------------------------------------------------------

#if defined(_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

class CFILE
{

public:

            CFILE ();
            ~CFILE ();

  CFILE&    operator = (int);

  int       get ();

private:
        
          CFILE (const CFILE&);
  CFILE&  operator = (const CFILE&);
  
  int   m_fileHandle;

};

//-----------------------------------------------------------------------------


inline CFILE::CFILE () : m_fileHandle (-1)
{
}

//-----------------------------------------------------------------------------

inline CFILE::~CFILE ()
{
  if (-1 != m_fileHandle)
  {
    _close (m_fileHandle);
  }
}

//-----------------------------------------------------------------------------

inline CFILE& CFILE::operator = (int file)
{
  if (-1 != m_fileHandle)
  {
    _close (m_fileHandle);
  }
  m_fileHandle = file;
  return (*this);
}

//-----------------------------------------------------------------------------

inline int CFILE::get ()
{
  return (m_fileHandle);
}


//-----------------------------------------------------------------------------

#if defined(_MSC_VER)
  #pragma warning (pop)
#endif

#endif
