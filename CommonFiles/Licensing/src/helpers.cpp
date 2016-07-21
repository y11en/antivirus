#include "stdafx.h"
#include <helpers.h>

//-----------------------------------------------------------------------------

CFileLock::CFileLock (const _TCHAR* fileName, 
                      file_lock_mode_t lockMode) 
  : m_fileName (fileName),
    m_lockMode (lockMode),
    m_fileHandle (INVALID_HANDLE_VALUE)
{
}

//-----------------------------------------------------------------------------

CFileLock::CFileLock (const STRING& fileName,
                      file_lock_mode_t lockMode) 
  : m_fileName (fileName),
    m_lockMode (lockMode),
    m_fileHandle (INVALID_HANDLE_VALUE)
{
}

//-----------------------------------------------------------------------------

CFileLock::~CFileLock ()
{
  unlock ();
}

//-----------------------------------------------------------------------------

DWORD CFileLock::lock ()
{
  if (INVALID_HANDLE_VALUE != m_fileHandle)
    return (true); // file is already locked

  if (m_fileName.empty ())
    return (false);

  m_fileHandle = ::CreateFile (m_fileName.c_str (),
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ,
                               0,
                               OPEN_EXISTING,
                               0,
                               0
                               );
  return (INVALID_HANDLE_VALUE != m_fileHandle ? ERROR_SUCCESS : GetLastError ());
}

//-----------------------------------------------------------------------------

DWORD CFileLock::unlock ()
{
  if (INVALID_HANDLE_VALUE == m_fileHandle)
    return (ERROR_SUCCESS);
  return (::CloseHandle (m_fileHandle) ? ERROR_SUCCESS : GetLastError ());
}

//-----------------------------------------------------------------------------
