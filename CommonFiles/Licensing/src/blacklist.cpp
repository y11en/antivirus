#include <algorithm>
#include "../include/blacklist.h"

#if defined (_WIN32)
  #include <windows.h>
#endif

#include <fcntl.h>
#include "../include/helpers.h"

using namespace LicensingPolicy;

#if defined (_MSC_VER)
#pragma warning (push, 4)
#endif


//-----------------------------------------------------------------------------

const _TCHAR Signature[] = _T ("KLBL");

const unsigned int SignatureOffset      = 0;
const unsigned int HeaderSizeOffset     = 4;

const unsigned int SerNumListOffset     = 148;

const unsigned int SignatureSize        = 4;
const unsigned int ModificationDateSize = 8;
const unsigned int DescriptionSize      = 128;

const file_size_t  MaxBlacklistFileSize = 1024 * 200; 

//-----------------------------------------------------------------------------

CBlacklist::ResultCodeT CBlacklist::loadBlackList (const _TCHAR* fileName)
{

  if (!fileName) 
  {
    return (rcInvalidParam);
  }

  char      *_buf = 0;
  size_t    _bufSize;

  ResultCodeT r = readFile (fileName, &_buf, &_bufSize);

  if (rcOK != r)
  {
    delete [] _buf;
    return (r);
  }

  try
  {
	  r = doLoadBlacklist (_buf, _bufSize);
  }
  catch (...) 
  {
	r = rcCorruptedFile;
  }

  delete [] _buf;

  return (r);
 
}

//-----------------------------------------------------------------------------

CBlacklist::ResultCodeT CBlacklist::loadBlackList (const char   *buf,
                                                   unsigned int size
                                                  )
{

  if (!buf || !size) 
  {
    return (rcInvalidParam);
  }

  ResultCodeT	r;

  try
  {
	  r = doLoadBlacklist (buf, size);
  }
  catch (...) 
  {
	  r = rcCorruptedFile;
  }

  return (r);

}

//-----------------------------------------------------------------------------

CBlacklist::ResultCodeT CBlacklist::doLoadBlacklist 
    (const char     *buf,
     size_t         bufSize
    )
{

  string_t s;
  dword_t  headerSize;
  dword_t  currBuffPos = SignatureOffset;
  dword_t  serNumCount;

  // Extract signature
  if (!get (buf, bufSize, currBuffPos, &s, SignatureSize) || (0 != s.compare (Signature))) 
    return (rcCorruptedFile);

  currBuffPos += SignatureSize;

  // Extract header size

  if (!get (buf, bufSize, currBuffPos, &headerSize))
    return (rcCorruptedFile);

  currBuffPos += sizeof (dword_t);

  // Extract black list file version

   if (!get (buf, bufSize, currBuffPos, &m_blackLstFileVer))
     return (rcCorruptedFile);

  currBuffPos += sizeof (dword_t);

  // Exract file modification date

  if (!get (buf, bufSize, currBuffPos, &m_modificationDate, ModificationDateSize))
    return (rcCorruptedFile);

  currBuffPos += ModificationDateSize;

  // Extract description

  if (!get (buf, bufSize, currBuffPos, &m_description, DescriptionSize))
    return (rcCorruptedFile);

  currBuffPos += DescriptionSize;

  // Move to serial numbers list head

  currBuffPos = SerNumListOffset; 

  // Extract serial numbers count

  if (!get (buf, bufSize, currBuffPos, &serNumCount))
    return (rcCorruptedFile);

  currBuffPos += sizeof (dword_t);

  size_t              counter = 0;
  key_serial_number_t sn; 

  m_serNumList.reserve (serNumCount);

  while ((counter < serNumCount) && get (buf, bufSize, currBuffPos, &sn)) 
  {
    m_serNumList.push_back (sn);
    currBuffPos += sizeof (dword_t);
    counter++;
  }

  return (counter < serNumCount ? rcCorruptedFile : rcOK);

}

//-----------------------------------------------------------------------------

CBlacklist::ResultCodeT CBlacklist::readFile 
    (const _TCHAR   *fileName,
     char           **buf,
     size_t         *bufSize
    )
{

  if (!fileName || !buf || !bufSize) 
  {
      return (rcInvalidParam);
  }
    
  *buf = 0;
  *bufSize = 0;

  file_size_t fileSize;

  CFILE fHandle;
  
  fHandle =  _topen (fileName, O_RDONLY);

  if (-1 == fHandle.get ()) 
    return (rcReadError);

  if (static_cast<file_size_t> (-1) == (fileSize = _lseek (fHandle.get (), 0L, SEEK_END)))
    return (rcReadError);

//  if (MaxBlacklistFileSize < fileSize)
//    return (rcCorruptedFile);
//  else
    *bufSize = static_cast <unsigned int> (fileSize);

  if (static_cast<file_size_t> (-1) == _lseek (fHandle.get (), 0L, SEEK_SET))
    return (rcReadError);

  *buf = new char[*bufSize];

  if (!buf)
    return (rcOutOfMemory);

  if (-1 == (_read (fHandle.get (), *buf, (unsigned int)*bufSize)))
  {
    delete [] *buf;
    *buf = 0;
    *bufSize = 0;
    return (rcReadError);
  }

  return (rcOK);
  
}

//-----------------------------------------------------------------------------

bool CBlacklist::get 
    (const char     *buf,
     size_t         bufSize,
     size_t         pos,
     string_t       *s,
     size_t         length
    )
{

  if (!haveEnoughData (bufSize, pos, length))
    return (false);

#if defined (_WIN32) && defined (_UNICODE)

  std::string tmp;

  tmp.assign (buf + pos, length);

  charToUnicode (tmp.c_str (), s);

#else

  s->assign (buf + pos, length);

#endif

  return (true);
}

//-----------------------------------------------------------------------------

bool CBlacklist::get
    (const char *buf,
     size_t     bufSize,
     size_t     pos,
     dword_t    *value
    )
{
  if (!haveEnoughData (bufSize, pos, sizeof (dword_t)))
    return (false);

  memcpy (value, buf + pos, sizeof (dword_t));

  return (true);
}

//-----------------------------------------------------------------------------

bool CBlacklist::get 
    (const char             *buf,
     size_t                 bufSize,
     size_t                 pos, 
     key_serial_number_t    *value
    )
{

  memset (value, 0, sizeof (key_serial_number_t));
  if (!haveEnoughData (bufSize, pos, sizeof (dword_t)))
    return (false);

  memcpy (&value->number.parts.keySerNum, buf + pos, sizeof (dword_t));

  return (true);

}

//-----------------------------------------------------------------------------

#if defined (WIN32) && defined (_UNICODE)

void CBlacklist::charToUnicode (const char* s, 
                                std::wstring* ws
                               )
{

  if (!s || !ws)
  {
    return;
  }

  int length = (int)strlen (s);

  if (!length)
    return;

  try
  {

    wchar_t *buf = new wchar_t[(length + 1) * sizeof (wchar_t)];

    if (!buf)
    {
      return;
    }

    if (MultiByteToWideChar (1251, 
                             0, 
                             s, 
                             length, 
                             buf, 
                             length + 1
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

#endif // #if defined (WIN32) && defined (_UNICODE)

//-----------------------------------------------------------------------------

bool CBlacklist::haveEnoughData 
    (size_t bufSize,
     size_t pos, 
     size_t size
    )
{
  return ((bufSize - pos + 1) >= size);
}

//-----------------------------------------------------------------------------

bool CBlacklist::findKey (const key_serial_number_t& sn) const
{
  return (m_serNumList.end () != std::find (m_serNumList.begin (), 
    m_serNumList.end (), sn));
}

//-----------------------------------------------------------------------------
