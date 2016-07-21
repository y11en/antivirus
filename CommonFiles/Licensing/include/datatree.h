#ifndef __DATATREE_H__
#define  __DATATREE_H__

#if defined (_MSC_VER)
#pragma warning (disable : 4786)
#endif

#if defined (_WIN32)
  #include <tchar.h>
  #include <windows.h>
#endif

#include "platform_compat.h"

#include <string>

#include "../../property/property.h"
#include "appinfoid.h"
#pragma pack ()

#include "lic_defs.h"

#if defined (_MSC_VER)
#pragma once
#pragma warning (push, 4)
#endif

#if defined (__MWERKS__)
  #include <novell_io_redir.h>
#endif


namespace DataTree 
{

//-----------------------------------------------------------------------------

typedef AVP_dword dword_t;
typedef std::basic_string<_TCHAR> string_t;

template <unsigned int dim>
struct CAddr
{
  dword_t addr[dim];
  operator dword_t* ()
  {
    return (addr);
  }
};

CAddr<2> makeAddr (dword_t id0);
CAddr<3> makeAddr (dword_t id0, dword_t id1);
CAddr<4> makeAddr (dword_t id0, dword_t id1, dword_t id2);
CAddr<5> makeAddr (dword_t id0, dword_t id1, dword_t id2, dword_t id3);


typedef union  
{
  AVP_date date;
  struct
  {
    unsigned short year;
    unsigned short month;
    unsigned short dayOfWeek;
    unsigned short day;
  } dateParts;
} AVP_date_t;


class CDataTree
{
public:

  class iterator;
  
                 CDataTree ();
                 ~CDataTree ();

  unsigned int   load         (const string_t& fileName);
  unsigned int   load         (const char *buf, dword_t bufSize);
  unsigned int   save         (const string_t& fileName);
  unsigned int   save         (char **buf, dword_t *bufSize);

  void           setFileName  (const string_t& fileName);
  string_t&      getFileName  ();

  HPROP          findProperty (dword_t* addr);
  HDATA          findData     (dword_t* addr);
  HDATA          findData     (HDATA hData, dword_t* addr);

  unsigned int   get          (dword_t* addr, dword_t* val);
  unsigned int   get          (dword_t* addr, string_t* val);
  unsigned int   get          (dword_t* addr, AVP_date_t* val);
  unsigned int   get          (dword_t* addr, void* val, dword_t size);
  unsigned int   get          (dword_t* addr, void** val, dword_t *size);

  unsigned int   get          (HDATA handle, dword_t* val);
  unsigned int   get          (HDATA handle, string_t* val, dword_t id = 0);
  unsigned int   get          (HDATA handle, AVP_date_t* val);
  unsigned int   get          (HDATA handle, void* val, dword_t size);
  unsigned int   get          (HDATA handle, void** val, dword_t* size);

  unsigned int   get          (HPROP propHandle, dword_t* val);
  unsigned int   get          (HPROP propHandle, string_t* val);
  unsigned int   get          (HPROP propHandle, AVP_date_t* val);
  unsigned int   get          (HPROP propHandle, void* val, dword_t size);
  unsigned int   get          (HPROP propHandle, void** val, dword_t *size);

  HDATA          getRoot      ();

  HDATA          createTree   (dword_t rootId);
  HDATA          addNode      (HDATA node, dword_t nodeId);
  HDATA          addData      (HDATA node, dword_t nodeId, const string_t& value);
  HDATA          addData      (HDATA node, dword_t nodeId, dword_t value);
  HDATA          addData      (HDATA node, dword_t nodeId, const void *value, dword_t size);

  void           clear        ();

  iterator       begin        (HDATA startNode);
  iterator       end          ();

private:

   void          destroyTree  ();

#if defined (_WIN32) && defined (_UNICODE)
   void          charToUnicode (const char *s, std::wstring *ws);
#endif

   string_t      m_fileName;
   HDATA         m_root;
   
   static size_t m_instanceCounter;

}; // class CDataTree

//-----------------------------------------------------------------------------

class CDataTree::iterator
{

  friend class CDataTree;

public:
                          iterator ();
                          iterator (const iterator&);
                          ~iterator () {}

  iterator&               operator = (const iterator&);

  int                     operator == (const iterator&);
  int                     operator != (const iterator&);               

  CDataTree::iterator&    operator ++ (int);
  HDATA                   operator *  ();

private:
     
                          iterator (HDATA, CDataTree*);
                            
  HDATA             m_currPtr;
  CDataTree* m_dataTree;

};

//-----------------------------------------------------------------------------

inline HDATA CDataTree::getRoot () 
{
  return (m_root);
}

//-----------------------------------------------------------------------------

inline CDataTree::iterator CDataTree::begin (HDATA startNode) 
{


  if (!startNode)  
  {
    return (end ());
  }

  HDATA hd = ::DATA_Get_First (startNode, 0);

  return (hd ? iterator (hd, this) : end ());

}

//-----------------------------------------------------------------------------

inline CDataTree::iterator CDataTree::end () 
{
  return (iterator (0, this));
}

//-----------------------------------------------------------------------------
//
//  CDataTree::iterator
//
//-----------------------------------------------------------------------------

inline CDataTree::iterator::iterator ()  : m_currPtr (0), 
  m_dataTree (0)
{
}

//-----------------------------------------------------------------------------

inline CDataTree::iterator::iterator (HDATA addr, 
  CDataTree* dataTree)  : m_currPtr (addr), m_dataTree (dataTree) 
{
}

//-----------------------------------------------------------------------------

inline CDataTree::iterator::iterator (const iterator& i)  : 
  m_currPtr (i.m_currPtr), m_dataTree (i.m_dataTree)
{
}

//-----------------------------------------------------------------------------

inline CDataTree::iterator& 
  CDataTree::iterator::operator = (const iterator& i) 
{
  m_currPtr = i.m_currPtr;
  m_dataTree = i.m_dataTree;
  return (*this);
}

//-----------------------------------------------------------------------------

inline int CDataTree::iterator::operator == (const iterator& i) 
{
  return (m_currPtr == i.m_currPtr && m_dataTree == i.m_dataTree);
}

//-----------------------------------------------------------------------------

inline int CDataTree::iterator::operator != (const iterator& i) 
{
  return (!(*this == i));
}

//-----------------------------------------------------------------------------

inline CDataTree::iterator& CDataTree::iterator::operator ++ (int) 
{
  m_currPtr = ::DATA_Get_Next (m_currPtr, 0);
  return (*this);
}

//-----------------------------------------------------------------------------

inline  HDATA CDataTree::iterator::operator * () 
{
  return (m_currPtr);
}

//-----------------------------------------------------------------------------

} // namespace DataTree

#if defined (_MSC_VER)
#pragma warning (pop)
#endif

#endif
