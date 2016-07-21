#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_seqio.h>

#include "i_cab.h"
#include "cab.h"
#include <m_utils.h>

tERROR ioCopyFile(tBYTE* Buffer, hIO Src, tQWORD qwSrcStartPos, tDWORD dwSize, hIO Dest, tQWORD qwDestStartPos)
{
tDWORD read;
tDWORD wrote;
tERROR error;
tQWORD qwPosRead;
tQWORD qwPosWrite;
  
  error = errOK;
  wrote = 0;
  
  if(error == errOK)
  {
    error = CALL_IO_SeekRead(Src, &read, qwSrcStartPos, Buffer, cCopyBufferSize > dwSize ? dwSize : cCopyBufferSize );
    qwPosRead = qwSrcStartPos;
    qwPosWrite = qwDestStartPos;
    while(read != 0 )
    {
      error = CALL_IO_SeekWrite(Dest,NULL, qwPosWrite, Buffer, read);
      if(errOK != error || wrote+read >=dwSize)
        break;
      
      qwPosWrite += read;
      qwPosRead += read;
      wrote += read;
      CALL_IO_SeekRead(Src, &read, qwPosRead, Buffer, cCopyBufferSize );
      if(wrote+read > dwSize)
        read = dwSize - wrote;
    }
  }
  return error;
}


tERROR SeqIoCopyFile(tBYTE* Buffer, hIO Src, tQWORD qwSrcStartPos, tDWORD dwSize, hSEQ_IO Dest)
{
tDWORD read=0;
tDWORD wrote;
tERROR error;
tQWORD qwPosRead;
  
  error = errOK;
  wrote = 0;
  
  if(error == errOK)
  {
    error = CALL_IO_SeekRead(Src, &read, qwSrcStartPos, Buffer, cCopyBufferSize > dwSize ? dwSize : cCopyBufferSize );
    qwPosRead = qwSrcStartPos;
    while(read != 0 )
    {
      error = CALL_SeqIO_Write(Dest,NULL, Buffer, read);
      if(errOK != error || wrote+read >=dwSize)
        break;
      
      qwPosRead += read;
      wrote += read;
      read = 0;
      CALL_IO_SeekRead(Src, &read, qwPosRead, Buffer, cCopyBufferSize );
      if(wrote+read > dwSize)
        read = dwSize - wrote;
    }
  }
  return error;
}

tDWORD SkipCABnameIo(hIO hArcIO, tCHAR* Buffer, tDWORD dwPos)
{
tDWORD dwRet;
hSEQ_IO hSeqIo;

  if(PR_FAIL(CALL_SYS_ObjectCreateQuick(hArcIO, &hSeqIo,IID_SEQIO, PID_ANY,0)))
  {
    return 0;
  }
  CALL_SeqIO_Seek(hSeqIo, NULL, dwPos, cSEEK_SET);
  dwRet = SkipCABname(hSeqIo, Buffer);
  CALL_SYS_ObjectClose(hSeqIo);
  return dwRet;
}

tDWORD SkipCABname(hSEQ_IO hIo, tCHAR* szBuffer)
{
  tDWORD dwRet;
  tCHAR  c;
  tINT   i;
  
  // Skip Name
  for ( i = 0, dwRet = 0, c = -1; c; )
  {
    if ( PR_FAIL(CALL_SeqIO_ReadByte(hIo, &c)) )
      break;
    
    dwRet++;
    if ( szBuffer && i < (cCopyBufferSize-4) )
    {
      szBuffer[i++] = c;
    }
  }

  //Skip Sequential Number
  for ( c = -1; c; )
  {
    if ( PR_FAIL(CALL_SeqIO_ReadByte(hIo, &c)) )
      break;

    dwRet++;
  }
  
  if ( szBuffer )
    szBuffer[i] = 0;

  return dwRet;
}


tDWORD checksum(tBYTE *buf, tDWORD len, tDWORD csum)
{
tDWORD i;
  
  i = len >> 2; // Do all the DWORDs first
  
  while(i--)
  {
    csum ^= *((tDWORD *)buf);
    buf += 4;
  }
  
  i = len & 3;  // Trailing bytes are not handled as you might expect!
  if (i == 3)
  {
    csum ^= (tDWORD)*buf++ << 16l;
    i--;
  }
  
  if (i == 2)
  {
    csum ^= (tDWORD)*buf++ << 8l;
    i--;
  }
  
  if (i == 1)
  {
    csum ^= (tDWORD)*buf++;
    i--;
  }
  return(csum);
}

tDWORD checksumIo(hSEQ_IO hSeqIo, tBYTE* Buffer, tDWORD BufferSize, tDWORD len, tDWORD csum)
{
tDWORD read;

  read = BufferSize;
  do
  {
    if(len < BufferSize)
    {
      read = len;
    }
    CALL_SeqIO_Read(hSeqIo, &read, Buffer, read);
    if(read == 0)
      break;
    len -= read;
    csum = checksum(Buffer, read, csum);
  }while(len != 0);

  return(csum);
}

tVOID FillCABHeader(PCAB_HEADER pch)
{
  memset((tBYTE *)pch,0, sizeof(CAB_HEADER));
  
  pch->sig = CAB_SIGNATURE;
    pch->coffFiles = sizeof(CAB_HEADER) + sizeof(CAB_FOLDER);
    pch->version = CAB_VERSION;
    pch->cFolders = 1;
}

