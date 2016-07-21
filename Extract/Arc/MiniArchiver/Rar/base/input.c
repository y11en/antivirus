#include <Prague/prague.h>
#include <Prague/iface/i_seqio.h>

#include "const.h"
#include "proto.h"

#ifndef MIN
# define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

RSTATIC tUINT RATTRIB DoReadBuff(tINPUT* input, tBYTE* Buff, tUINT Count)
{
  struct sRAR* RAR;
  tBYTE* BufPtr;
  tUINT  Rdt;
  tUINT  TotRdt;
  tUINT  ToRead;

  TotRdt = 0;
  BufPtr = Buff;
  RAR    = input->rardata;
  while( Count != 0 )
  {
    ToRead = MIN(Count, input->limit);
    Rdt = PackRead(RAR, BufPtr, ToRead);
    if ( RAR->Volume )
      input->rd_crc = CRC32(input->rd_crc, BufPtr, Rdt);
    input->limit -= Rdt;
    TotRdt += Rdt;
    BufPtr += Rdt;
    Count  -= Rdt;

    if ( Rdt == 0 )
    {
      if ( RAR->Volume != cFALSE )
      {
        tUINT  bitcnt;
        tUINT  bitbuf;
        tDWORD wr_crc;

         bitcnt = input->bitcnt;
         bitbuf = input->bitbuf;
         wr_crc = RAR->wr_crc32;

         if ( (RAR->MHD_Flags & MHD_SOLID) == 0 )
         {
           RAR->last_file = cTRUE;
         }

         if ( ~RAR->CurrentFile.file_crc )
         {
           // RAR 2.0 not support chunk CRC
           if ( input->rd_crc != ~RAR->CurrentFile.file_crc )
           {
             // Printf("Chunk CRC failed" crlf);
             RAR->LastError = (errOBJECT_DATA_CORRUPTED);
             input->eof = cTRUE;
             break;
           }
         }

         if ( PR_SUCC(ArchiveMerge(RAR, input, &RAR->CurrentFile)) )
         {
           input->limit  = input->rardata->CurrentFile.psize;
           input->bitcnt = bitcnt;
           input->bitbuf = bitbuf;
           input->rardata->wr_crc32 = wr_crc;
         }
         else if ( TotRdt != 0 )
         {
           break; /* something readed */
         }
         else
         {
           input->eof = cTRUE;
           break;
         }
      }
      else if ( TotRdt != 0 )
      {
        break; /* something readed */
      }
      else
      {
        input->eof = cTRUE;
        break;
      }
    }
  }

  return TotRdt;
}


RSTATIC rarbool RATTRIB FileBufferUpdate(tINPUT *input)
{
  struct sRAR* RAR;
  tQWORD needed;
  tUINT  curcan;
  tUINT  readed;
  tBYTE* Buff;
  tINT   Method;
  tUINT  Cnt, I;
  
  RAR = input->rardata;
  needed = RAR->TotPSize;
  if ( RAR->TotPSize == 0 )
  {
    input->eof = cTRUE;
    return(cFALSE);
  }

  curcan = (tUINT)MIN(needed, (tQWORD)input->bufsize);
  input->file_pos += input->inptr;
  input->inptr = 0;
  
  readed = DoReadBuff(input, input->inbuf, curcan);
  RAR->TotPSize -= readed;
  input->incnt   = readed;
  
  if ( readed && RAR->Decrypt )
  {
    Buff = input->inbuf;
    Cnt  = input->incnt;

    if ( RAR->Crypt.Version < 20 )
    {
      Method = (RAR->Crypt.Version == 15) ? NEW_CRYPT : OLD_DECODE;
      RAR_Crypt(&RAR->Crypt, Buff, Cnt, Method);
    }
    else
    {
      if ( RAR->Crypt.Version == 20 )
      {
        for ( I = 0; I < Cnt; I += 16 )
          RAR_DecryptBlock20(&RAR->Crypt, &Buff[I]);
      }
      else
      {
        Cnt = ALIGN(Cnt, 16);
        RAR_DecryptBlock(&RAR->Crypt, Buff, Cnt);
      }
    }
  }

  if ( errOK != RAR->LastError || Rotate(RAR) )
  {
    // user stop
    input->rardata->Suspended = TRUE;
    input->eof = cTRUE;
    input->incnt = 0;
    return(cFALSE);
  }

  return (rarbool)!input->eof;
}

RSTATIC byte RATTRIB ByteGetFileBuf(tINPUT *input)
{
    byte value;

    if ( input->inptr >= input->incnt )
    {
        value = 0;

        if ( input->type == cINPUT_FILE )
        {
            // update buffer
            if (FileBufferUpdate(input))
            {
                value = input->inbuf[input->inptr++];
            }
        }
        else
        {
            input->eof = TRUE;
        }
    }
    else
    {
        value = input->inbuf[input->inptr++];
    }
    return value;
}

// public constructors
RSTATIC void RATTRIB InputSetLimit(tINPUT *input, uint limit)
{
    input->limit   = limit;
    input->eof     = FALSE;
}

RSTATIC void RATTRIB InputInitSrc(tINPUT *input, tINPUT *source, byte (RATTRIB *ReadByte)(tINPUT *input))
{
    input->ByteGet = ReadByte;
    input->type    = cINPUT_SOURCE;
    input->limit   = FILE_SIZE_MAX;
    input->bitbuf  = 0;
    input->bitcnt  = 0;
    input->eof     = 0;
    input->input   = source;
    input->rardata = source->rardata;

    input->inbuf   = NULL;
    input->inptr   = 0;
    input->incnt   = 0;
    input->file_pos= 0;

//    input->hd_crc  = 0xFFFFFFFF;
//    input->rd_crc  = 0xFFFFFFFF;
}

RSTATIC void RATTRIB InputInitFile(tINPUT *input, tRAR *rardata, 
                                   tBYTE* Buff, tUINT BuffSize)
{
    input->ByteGet = ByteGetFileBuf;
    input->type    = cINPUT_FILE;
    input->limit   = FILE_SIZE_MAX;
    input->bitbuf  = 0;
    input->bitcnt  = 0;
    input->eof     = 0;
    input->input   = input;
    input->rardata = rardata;

    input->inbuf   = Buff;
    input->inptr   = 0;
    input->incnt   = 0;
    input->file_pos= (tDWORD)PackSeek(rardata, 0, cSEEK_CUR);

    input->hd_crc  = 0xFFFFFFFF;
    input->rd_crc  = 0xFFFFFFFF;

    input->bufsize = (BuffSize);
}

RSTATIC void RATTRIB InputInitBuf(tINPUT *input, void *buf, uint limit)
{
    input->ByteGet = ByteGetFileBuf;
    input->type    = cINPUT_BUFF;
    input->limit   = limit;

    input->bitbuf  = 0;

    input->bitcnt  = 0;
    input->eof     = 0;
    input->input   = NULL;

    input->inbuf   = buf;
    input->inptr   = 0;
    input->incnt   = limit;
    input->file_pos= 0;

    //input->hd_crc  = 0xFFFFFFFF;
    //input->rd_crc  = 0xFFFFFFFF;
}

// public methods

RSTATIC word RATTRIB BitGet(tINPUT *input)
{
    while ( input->bitcnt < 16 )
    {
        // calling own procedure
        input->bitbuf |= (uint)(input->ByteGet(input)) << (16-input->bitcnt);
        if ( input->eof )
        {
            input->eof = FALSE;
            break;
        }
        else
        {
            input->bitcnt += 8;
        }
    }
    return (word)((input->bitbuf) >> 8);
}

RSTATIC void RATTRIB BitAlign(tINPUT *input)
{
    BitAdd(input, input->bitcnt & 7);
}

RSTATIC void RATTRIB BitAdd(tINPUT *input, uint bits)
{
    if ( input->bitcnt < bits )
    {
        input->eof = TRUE;
        input->bitcnt = 0;
    }
    else
    {
        input->bitbuf <<= bits;
        input->bitcnt -= bits;
    }
}

RSTATIC byte RATTRIB RarReadByte(tINPUT *input)
{
    if ( input->bitcnt )
    {
        input->bitbuf <<= input->bitcnt & 7;
        input->bitcnt &= ~7;

        if ( input->bitcnt )
        {
            input->bitcnt -= 8;
            input->bitbuf <<= 8;
            return (byte)(input->bitbuf >> 24);
        }

    }

    if ( input->type == cINPUT_FILE )
    {
        return input->ByteGet(input);
    }
    else
    {
        if ( input->limit && !input->eof && !input->input->eof )
        {
            input->limit--;
            return input->ByteGet(input);
        }
        else
        {
            input->eof = TRUE;
            return 0;
        }
    }
}

RSTATIC byte RATTRIB RarReadByteCrc(tINPUT *input)
{
    byte value;

    value = RarReadByte(input);

    input->hd_crc = CRC32(input->hd_crc, &value, sizeof(value));

    return value;
}

RSTATIC word RATTRIB RarReadWordCrc(tINPUT *input)
{
    word value;

    value  = RarReadByteCrc(input);
    value |= RarReadByteCrc(input) << 8;
    return value;
}

RSTATIC word RATTRIB RarReadWord(tINPUT *input)
{
    word value;

    value  = input->ByteGet(input);
    value |= input->ByteGet(input) << 8;
    return value;
}

RSTATIC dword RATTRIB RarReadDwordCrc(tINPUT *input)
{
    dword value;

    value  = RarReadByteCrc(input);
    value |= RarReadByteCrc(input) << 8;
    value |= RarReadByteCrc(input) << 16;
    value |= RarReadByteCrc(input) << 24;
    return value;
}

RSTATIC dword RATTRIB RarReadDword(tINPUT *input)
{
    dword value;

    value  = RarReadByte(input);
    value |= RarReadByte(input) << 8;
    value |= RarReadByte(input) << 16;
    value |= RarReadByte(input) << 24;
    return value;
}


RSTATIC uint RATTRIB RarReadCrc(tINPUT *input, void *buf, uint count)
{
    uint mayread;
    byte *buffer;

    buffer = buf;
    if ( input->type == cINPUT_FILE )
    {
        while ( count )
        {
            mayread = input->incnt - input->inptr;
            if ( mayread )
            {
                if ( mayread > count ) mayread = count;
                memcpy(buffer,input->inbuf+input->inptr, mayread);
                input->hd_crc = CRC32(input->hd_crc, buffer, mayread);
                count -= mayread;
                buffer += mayread;
                input->inptr += mayread;
            }
            else
            {
                if ( !FileBufferUpdate(input) )
                  break;
            }
        }
    }

    return (uint)(buffer - (byte*)(buf));
}

RSTATIC filesize RATTRIB RarSkipCrc(tINPUT *input, uint count)
{
    uint mayread;
    uint read;

    read = 0;

    if ( input->type == cINPUT_FILE )
    {
        while ( count && !input->eof )
        {
            mayread = input->incnt - input->inptr;
            if ( mayread )
            {
                if ( mayread > count ) mayread = count;

                input->hd_crc = CRC32(input->hd_crc, input->inbuf+input->inptr, mayread);
                count -= mayread;
                input->inptr += mayread;
                read += mayread;
            }
            else
            {
                if ( !FileBufferUpdate(input) )
                  break;
            }
        }
    }
    return read;
}

RSTATIC filesize RATTRIB RarTell(tINPUT *input)
{
    return input->inptr + input->file_pos;
}

RSTATIC filesize RATTRIB RarSeek(tINPUT *input, filesize offset)
{
  if ( input->type == cINPUT_FILE )
  {
    input->file_pos = (tDWORD)PackSeek(input->rardata, offset, cSEEK_SET);
      
    if ( offset != input->file_pos )
    {
      input->eof = TRUE;
    }
      
    input->bitcnt  = 0;
    input->eof     = 0;
      
    input->inptr   = 0;
    input->incnt   = 0;
  }

  return input->file_pos;
}

RSTATIC void RATTRIB BufferUnpackStart(tINPUT *input, filesize limit )
{
    uint avail;

    if ( input->type == cINPUT_FILE )
    {
        avail = input->incnt-input->inptr;

        if (  avail > limit )
        {
            input->limit = 0;
            input->incnt = (uint)(input->inptr+limit);
        }
        else
        {
            input->limit = limit - avail;
        }

        // calculate CRC32 for current buffer
        input->rd_crc = CRC32(0xFFFFFFFF,input->inbuf+input->inptr, avail);

        // reset crc for written file
        input->rardata->wr_crc32 = 0xFFFFFFFF;
        input->bitcnt = 0;
        input->bitbuf = 0;
    }
}

RSTATIC void RATTRIB BufferUnpackStop(tINPUT *input)
{
    input->limit = FILE_SIZE_MAX;
}

RSTATIC void RATTRIB BufferCleanUp(tINPUT *input)
{
    if ( input->type == cINPUT_FILE )
    {
        input->file_pos += input->inptr;
        input->incnt = 0;
        input->inptr = 0;
        input->bitcnt = 0;
        input->eof = 0;
        input->limit = FILE_SIZE_MAX;
    }
}

RSTATIC byte RATTRIB RarReadByteStream(tINPUT *input)
{
    byte value;

    if ( input->limit && !input->input->eof )
    {
        input->limit--;
        value = (byte)(BitGet(input->input) >> 8);
        BitAdd(input->input,8);
        input->rd_crc = CRC32(input->rd_crc, &value, 1);
    }
    else
    {
        input->eof = TRUE;
        value = 0;
    }
    return value;
}

RSTATIC byte RATTRIB RarReadBytePPM(tINPUT *input)
{
    int value;
    byte val;

    if ( input->limit && !input->eof )
    {
        input->limit--;
        value = PPM_DecodeChar(input->rardata, input->input);
        if ( value == -1 )
        {
            input->eof = TRUE;
            val = 0;
        }
        else
        {
            val = (byte)(value);
            input->rd_crc = CRC32(input->rd_crc, &val, 1);
        }
    }
    else
    {
        input->eof = TRUE;
        val = 0;
    }

    return val;
}
