// Common/C_FileIO.h

#include "C_FileIO.h"

#include <fcntl.h>
//#include <unistd.h>
#include <io.h>

namespace NC {
namespace NFile {
namespace NIO {

bool CFileBase::OpenBinary(const char *name, int flags)
{
  #ifdef O_BINARY
  flags |= O_BINARY;
  #endif
  Close();
  _handle = ::_open(name, flags, 0666);
  return _handle != -1;
}

bool CFileBase::Close()
{
  if(_handle == -1)
    return true;
  if (_close(_handle) != 0)
    return false;
  _handle = -1;
  return true;
}

bool CFileBase::GetLength(UInt64 &length) const
{
  off_t curPos = Seek(0, SEEK_CUR);
  off_t lengthTemp = Seek(0, SEEK_END);
  Seek(curPos, SEEK_SET);
  length = (UInt64)lengthTemp;
  return true;
}

off_t CFileBase::Seek(off_t distanceToMove, int moveMethod) const
{
  return ::_lseek(_handle, distanceToMove, moveMethod);
}

/////////////////////////
// CInFile

bool CInFile::Open(const char *name)
{
  return CFileBase::OpenBinary(name, O_RDONLY);
}

ssize_t CInFile::Read(void *data, size_t size)
{
  return _read(_handle, data, size);
}

/////////////////////////
// COutFile

bool COutFile::Create(const char *name, bool createAlways)
{
  if (createAlways)
  {
    Close();
    _handle = ::_creat(name, 0666);
    return _handle != -1;
  }
  return OpenBinary(name, O_CREAT | O_EXCL | O_WRONLY);
}

ssize_t COutFile::Write(const void *data, size_t size)
{
  return _write(_handle, data, size);
}

}}}
