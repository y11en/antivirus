
#if defined (_WIN32)
#include <windows.h>
#endif

#include <stdlib.h>
#include "../../serialize/kldtser.h"
#include "../../_avpio.h"
#include "../include/datatree.h"

#pragma pack ()

#if defined (_MSC_VER)
#pragma warning (push, 4)
#endif

#if defined (__MWERKS__)
  #include "../include/novell_io_redir.h"
#endif

using namespace DataTree;

size_t CDataTree::m_instanceCounter = 0;

//-----------------------------------------------------------------------------

CAddr<2> DataTree::makeAddr (dword_t id0)
{
  CAddr<2> a;

  a.addr[0] = id0;
  a.addr[1] = 0;

  return (a);
}

CAddr<3> DataTree::makeAddr (dword_t id0, dword_t id1)
{
  CAddr<3> a;

  a.addr[0] = id0;
  a.addr[1] = id1;
  a.addr[2] = 0;

  return (a);
}

CAddr<4> DataTree::makeAddr (dword_t id0, dword_t id1, dword_t id2)
{
  CAddr<4> a;

  a.addr[0] = id0;
  a.addr[1] = id1;
  a.addr[2] = id2;
  a.addr[3] = 0;

  return (a);
}

CAddr<5> DataTree::makeAddr (dword_t id0, dword_t id1, dword_t id2, dword_t id3)
{
  CAddr<5> a;

  a.addr[0] = id0;
  a.addr[1] = id1;
  a.addr[2] = id2;
  a.addr[3] = id3;
  a.addr[4] = 0;

  return (a);
}

//-----------------------------------------------------------------------------

CDataTree::CDataTree () : m_root (0)
{
  if (!m_instanceCounter)
  {
    ::KLDT_Init_Library (malloc, free);

#if defined (_WIN32)

    AvpCloseHandle = CloseHandle;
    AvpWriteFile   = WriteFile;
    AvpCreateFile  = CreateFile;
    AvpGetFileSize = GetFileSize;
    AvpReadFile    = ReadFile;
    
#else

  #if defined (__MWERKS__)
  
    AvpCloseHandle = nlmAvpCloseHandle;
    AvpWriteFile   = nlmAvpWriteFile;
    AvpCreateFile  = nlmAvpCreateFile;
    AvpGetFileSize = nlmAvpGetFileSize;
    AvpReadFile    = nlmAvpReadFile;
    
  #endif
    
#endif

  }
  m_instanceCounter++;
}

//-----------------------------------------------------------------------------

CDataTree::~CDataTree ()
{
  destroyTree ();
  if (m_instanceCounter > 0)
    m_instanceCounter--;
  if (!m_instanceCounter) 
    ::KLDT_Deinit_Library (FALSE);
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::load (const string_t& fileName) 
{
  destroyTree ();

  m_fileName = fileName;

  if (!m_fileName.empty ())
  {
    return (0 != ::KLDT_Deserialize (m_fileName.c_str (), &m_root));
  }
  else
    return (0);
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::load (const char *buf, dword_t bufSize) 
{

  destroyTree ();

  return (0 != ::KLDT_DeserializeFromMemoryUsingSWM (const_cast<char *> (buf), 
                                                     bufSize, 
                                                     &m_root
                                                    ));
  return (0);
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::save (const string_t& fileName) 
{

  m_fileName = fileName;

  if (m_root && !m_fileName.empty ())
    return (0 != ::KLDT_Serialize (fileName.c_str (), m_root, _T ("")));
  else
    return (0);
  
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::save (char **buf, dword_t *bufSize) 
{

  if (!buf || !bufSize ||!m_root)
    return (0);

  *buf = 0;

  if (!(*bufSize = ::KLDT_SerializeToMemoryUsingSWM (0, 0, m_root, _T (""))))
    return (0);

  if (!*bufSize)
    return (0);

  unsigned int r = 0;

  try
  {
    *buf = new char[*bufSize];

    if (!*buf)
    {
      *bufSize = 0;
      r  = 0;
    }
    else
    {
      if (! (r = ::KLDT_SerializeToMemoryUsingSWM (*buf, *bufSize, m_root, _T (""))))
      {
        delete *buf;
        *buf = 0;
        *bufSize = 0;
      }
    }

  }
  catch (std::bad_alloc&) 
  {
    *buf = 0;
    *bufSize = 0;
    r = 0;
  }

  return (r);

}

//-----------------------------------------------------------------------------

void CDataTree::setFileName (const string_t&  fileName) 
{
  destroyTree ();
  m_fileName = fileName;
}

//-----------------------------------------------------------------------------

string_t& CDataTree::getFileName () 
{
  return (m_fileName);
}


//-----------------------------------------------------------------------------

HPROP CDataTree::findProperty (dword_t* addr) 
{
  return (::DATA_Find_Prop (m_root, addr, 0));
}

//-----------------------------------------------------------------------------

HDATA CDataTree::findData (dword_t* addr) 
{
  return (::DATA_Find (m_root, addr));
}

//-----------------------------------------------------------------------------

HDATA CDataTree::findData (HDATA hData, dword_t* addr) 
{
  return (::DATA_Find (hData, addr));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (dword_t* addr, dword_t* val) 
{
  if ( ::DATA_Find ( m_root, addr ) )
    return (get (::DATA_Find_Prop (m_root, addr, 0), val));
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (dword_t* addr, string_t* val) 
{
  if ( ::DATA_Find ( m_root, addr ) )
    return (get (::DATA_Find_Prop (m_root, addr, 0), val));
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (dword_t* addr, AVP_date_t* val) 
{
  if ( ::DATA_Find ( m_root, addr ) )
    return (get (::DATA_Find_Prop (m_root, addr, 0), val));
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (dword_t* addr, void* val, dword_t size) 
{
  if ( ::DATA_Find ( m_root, addr ) )
    return (get (::DATA_Find_Prop (m_root, addr, 0), val, size));
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (dword_t* addr, void** val, dword_t *size)
{
  if ( ::DATA_Find ( m_root, addr ) )
    return (get (::DATA_Find_Prop (m_root, addr, 0), val, size));
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HDATA handle, dword_t* val) 
{
  return (get (::DATA_Find_Prop (handle, 0, 0), val));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HDATA handle, string_t* val, dword_t id) 
{
  return (get (::DATA_Find_Prop (handle, 0, id), val));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HDATA handle, AVP_date_t* val) 
{
  return (get (::DATA_Find_Prop (handle, 0, 0), val));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HDATA handle, void* val, dword_t size) 
{
  return (get (::DATA_Find_Prop (handle, 0, 0), val, size));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HDATA handle, void** val, dword_t* size)
{
  return (get (::DATA_Find_Prop (handle, 0, 0), val, size));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HPROP hProp, dword_t* val) 
{
  if (!hProp || ! val)
    return (0);

  return (0 != ::PROP_Get_Val (hProp, val, sizeof(dword_t)));
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HPROP hProp, string_t* val) 
{
  
  if (!hProp || !val)
    return (0);

  val->resize (0);

  size_t size = ::PROP_Get_Val (hProp, 0, 0);
  AVP_dword r = 0;

  if (size) 
  {
    try
    {
      char *buf = new char[(size + 1) * sizeof(char)];

      if (!buf) 
      {
        return (0);
      }
        
      r = ::PROP_Get_Val (hProp, buf, (AVP_dword)size);
      if (r)
      {
        buf[size] = 0;

#if defined (_WIN32) && defined (_UNICODE)
        charToUnicode (buf, val);
#else
        *val = buf;
#endif

      }

      delete [] buf;

    }
    catch (std::bad_alloc&) 
    {
    }

  } // if (size)

  return (r);

}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HPROP hProp, AVP_date_t* val) 
{
  if (!hProp || !val)
    return (0);

  return (0 != ::PROP_Get_Val (hProp, &val->date, sizeof (AVP_date)));
  
}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HPROP hProp, void* val, dword_t size) 
{
  if (!hProp)
    return (0);

  if (val)
  {
    return (::PROP_Get_Val (hProp, val, size));
  }
  else
  {
    // Just return property data size
    return (::PROP_Get_Val (hProp, 0, 0));
  }

}

//-----------------------------------------------------------------------------

unsigned int CDataTree::get (HPROP propHandle, void** val, dword_t *size)
{
  
  if (!propHandle || !val || !size)
  {
    return (0);
  }

  *val = 0;
  *size = ::PROP_Get_Val (propHandle, 0, 0);

  if (!*size) 
  {
    return (0);
  }

  try
  {
    *val = new char[*size];
    if (*val) 
    {
      return (::PROP_Get_Val (propHandle, *val, *size));
    }
    else
    {
      return (0);
    }
  }

  catch (const std::bad_alloc&) 
  {
    return (0);
  }


}

//-----------------------------------------------------------------------------

void CDataTree::destroyTree () 
{
  if (m_root) 
  {
    ::DATA_Remove (m_root, 0);
    m_root = 0;
  }
}

//-----------------------------------------------------------------------------

#if defined (_WIN32) && defined (_UNICODE)

void CDataTree::charToUnicode (const char *s, std::wstring *ws) 
{
  
  if (!s || ! ws)
    return;
  
  wchar_t    *buf;
  int      length; 

  length = (int)strlen (s);

  if (!length) 
  {
    return;
  }

  try
  {

    // MS VC++ 6.0 new operator does not throw std::bad_alloc, but 
    // all might be...
    buf = new wchar_t[(length + 1) * sizeof (wchar_t)];

    if (!buf)
    {
      return;
    }

    if (MultiByteToWideChar (1251,
                             0,
                             s,
                             length,
                             buf,
                             length
                            ) == length)
    {
      buf[length] = 0;
      *ws = buf;
    }

    delete [] buf;

  }
  catch (std::bad_alloc&) 
  {
  }

}

#endif // #if defined (_WIN32) && defined (_UNICODE)

//-----------------------------------------------------------------------------

HDATA CDataTree::createTree (dword_t rootId) 
{

  destroyTree ();

  m_root = ::DATA_Add (0, 0, rootId, 0, 0);

  return (m_root);

}

//-----------------------------------------------------------------------------

HDATA CDataTree::addNode (HDATA node, dword_t nodeId) 
{

  return (::DATA_Add (node, 0, nodeId, 0, 0));

}

//-----------------------------------------------------------------------------

HDATA CDataTree::addData (HDATA node, dword_t nodeId, const string_t& value) 
{
  return (::DATA_Add (node, 
                      0, 
                      nodeId, 
                      reinterpret_cast <AVP_size_t> (const_cast <_TCHAR*> (value.c_str ())), 
                      0
                     ));
}

//-----------------------------------------------------------------------------

HDATA CDataTree::addData (HDATA node, dword_t nodeId, dword_t value) 
{
  return (::DATA_Add (node, 
                      0, 
                      nodeId, 
                      value, 
                      0
                     ));
}

//-----------------------------------------------------------------------------

HDATA CDataTree::addData (HDATA node, dword_t nodeId, const void *value, dword_t size)
{
  return (::DATA_Add (node, 0, nodeId, reinterpret_cast<AVP_size_t>(value), size));
}

//-----------------------------------------------------------------------------

void CDataTree::clear ()
{
  destroyTree ();
}

//-----------------------------------------------------------------------------
