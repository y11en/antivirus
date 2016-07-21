#ifndef  _CRITSECT_H
#define  _CRITSECT_H

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief CriticalSection class. 
 *
 */


class CriticalSection
{
 // Constructor
 public:
  CriticalSection() { ::InitializeCriticalSection(&m_sect); }

 public:
  operator CRITICAL_SECTION*() { return &m_sect; }
  CRITICAL_SECTION m_sect;

 public:
  void Enter() { ::EnterCriticalSection(&m_sect); }
  void Leave() { ::LeaveCriticalSection(&m_sect); }

 public:
  ~CriticalSection() { ::DeleteCriticalSection(&m_sect); }
};

class LockCriticalSection
{
 // Constructor
 public:
  LockCriticalSection(CriticalSection *sect) : m_sect(sect) { m_sect->Enter(); }

 public:
  CriticalSection *m_sect;


 public:
  ~LockCriticalSection() { m_sect->Leave(); }
};


#endif // _CRITSECT_H
