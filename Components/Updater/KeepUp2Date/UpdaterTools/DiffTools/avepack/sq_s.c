/* ------------------------------------------------------------------------- */
/* 21.01.2005 - Optimized by Dmitry Glavatskikh                              */
/* ------------------------------------------------------------------------- */

#include <stdlib.h>
#include "fakeoop.h"
#include "sq_su.h"

/* ------------------------------------------------------------------------- */

#define put_byte(bb) { outbuf[index_byte++ + 2] = (unsigned char)(bb); }

#define put_bit(memfile, bit_val)                     \
{                                                     \
  unsigned int oval = (outbuf[0]|(outbuf[1]<<8));     \
  oval = (oval >> 1)|((bit_val & 1) ? 0x8000 : 0);    \
  outbuf[0] = (unsigned char)(oval & 0xff);           \
  outbuf[1] = (unsigned char)((oval >> 8) & 0xff);    \
                                                      \
  if ( ++index_bit == 16 )                            \
  {                                                   \
    if ( xor_byte )                                   \
    {                                                 \
      int k, c;                                       \
      for( k = 0, c = index_byte+2; k < c; k++ )      \
        outbuf[k] ^= xor_byte;                        \
    }                                                 \
                                                      \
    MemFileWrite(memfile, outbuf, index_byte + 2);    \
    wrought_bytes += index_byte + 2;                  \
    index_bit = index_byte = 0;                       \
  }                                                   \
}

#define put_rest(memfile)                             \
{                                                     \
  unsigned int oval = (outbuf[0]|(outbuf[1]<<8));     \
  oval >>= (16 - index_bit);                          \
  outbuf[0] = (unsigned char)(oval & 0xff);           \
  outbuf[1] = (unsigned char)((oval >> 8) & 0xff);    \
                                                      \
  if ( xor_byte )                                     \
  {                                                   \
    int k, c;                                         \
    for( k = 0, c = index_byte+2; k < c; k++ )        \
      outbuf[k] ^= xor_byte;                          \
  }                                                   \
                                                      \
  MemFileWrite(memfile, outbuf, index_byte + 2);      \
  wrought_bytes += index_byte + 2;                    \
}

/* ------------------------------------------------------------------------- */

/* main compression function */
unsigned long squeeze(SMemFile* inf, SMemFile* outf, unsigned long length,
                      unsigned char xor_byte)
{
  int       readed_size,i;
  short int len, maxlen;
  short int buffer_top;
 
  short int span,
            s_minimum,
            i_minimum,
            j;
  long      bytes_total,
            bytes_now;

  short int *last_occurence;
  short int *char_list;
  unsigned char* buffer;
  unsigned char* wrkmem;
  unsigned long  wrkcnt;

  unsigned char  outbuf[36];
  unsigned long  wrought_bytes;
  unsigned char  index_byte;
  unsigned char  index_bit;
  unsigned long  icount;

  bytes_total = length;
  for ( i = 0; i < sizeof(outbuf); i++ )
    outbuf[i] = 0;

  wrought_bytes = 0L;
  index_bit = index_byte = 0;
  bytes_now = 0L;

  icount=0;

  /* alloc working memory */
  wrkcnt = (256 * sizeof(short int)) + 16 + (0x4500 * sizeof(short int)) + 16;
  wrkmem = (unsigned char*)malloc(wrkcnt + 0x4500 + 16);
  if ( NULL == wrkmem )
    return(0); /* error */

  /* setup wrk mem pointers */
  last_occurence = (short int *)(&wrkmem[0]);
  char_list      = (short int *)(&wrkmem[(256 * sizeof(short int)) + 16]);
  buffer         = (unsigned char*)(&wrkmem[(256 * sizeof(short int)) + 16 +
    (0x4500 * sizeof(short int)) + 16]);

  readed_size = (length < 0x4500) ? length : 0x4500;
  readed_size = MemFileRead(inf, buffer, readed_size);

  icount += readed_size;
  buffer_top = readed_size;

  /* fill index array */
  for ( i = 0; i < 256; i++ )
    last_occurence[i] = -1;

  char_list[0] = -1;
  last_occurence[buffer[0]] = 0;

  put_bit(outf, 1);
  put_byte(buffer[0]);

  for (i = 1; i < buffer_top; i++)
  {
    unsigned char* ptr = buffer + i;
    short int      s;

    char_list[i] = last_occurence[ptr[0]];
    last_occurence[ptr[0]] = i;

    len = 0;
    span = 0;
    maxlen = (buffer_top - i < 0x100 ? buffer_top - i : 0x100);
    s_minimum = -(i > 0x2000 ? 0x2000 : i);
    i_minimum = (i > 0x2000 ? i-0x2000 : 0);

    /* scan buffer backwards by pointers */
    for ( j = char_list[i]; j >= i_minimum; j = char_list[j] )
    {
      short int l;

      if ( j < i_minimum )
        break;

      s = j - i;
      for ( l = 1; l < maxlen && ptr[l] == ptr[s + l]; l++ );

      if (l > len)
      {
        len = l;
        span = s;
        if ( l >= maxlen )
          break;
      }
    }

    if( len > maxlen )
      len = maxlen;
    if ( span >= -0xff && len >= 2 || span < -0xff && len > 2 )
    {
      put_bit(outf,0);

      for ( j = 1; j < len; j++ )
      {
        i++;
        char_list[i] = last_occurence[ptr[j]];
        last_occurence[ptr[j]] = i;
      }

      if (len <= 5 && span >= -0xff)
      {
        len -= 2;
        put_bit(outf,0);
        put_bit(outf, (len>>1));
        put_bit (outf, len);
        put_byte(span & 0xff);
      }
      else
      {
        put_bit(outf, 1);
        put_byte(span & 0xff);
        if (len <= 9)
        {
          len = (len - 2)|(((unsigned short)span >> 5) & ~0x7);
          put_byte( len );
        }
        else
        {
          put_byte(((WORD)span >> 5) & ~0x7);
          put_byte(len-1); /* ! */
        }
      }
    }
    else
    {
      put_bit(outf, 1);
      put_byte(buffer[i]);
    }

    if ( i > 0x4000 )  /* move buffer on 0x2000 and read next block */
    {
      /* fix pointers */
      for ( j=0; j<256; j++ )
        last_occurence[j] = (last_occurence[j] < 0x2000 ? -1 :
          last_occurence[j] - 0x2000);
      for ( j=0x2000; j<buffer_top; j++ )
      {
        buffer[j-0x2000] = buffer[j];
        char_list[j-0x2000] = char_list[j] <0x2000 ? -1 : char_list[j] - 0x2000;
      }

      i -= 0x2000;
      buffer_top -= 0x2000;
      bytes_now += 0x2000;

      readed_size = ((length-icount)<(unsigned long)(0x4500-buffer_top)) ? 
        (length-icount) : (0x4500-buffer_top);

      readed_size = MemFileRead(inf, buffer+buffer_top, readed_size);
      icount+=readed_size;

      buffer_top += readed_size;
    }
  }

  put_bit(outf, 0);
  put_bit(outf, 1);

  put_byte (0xff);
  put_byte (0);
  put_byte (0);
  put_rest(outf);

  free(wrkmem);
  return(wrought_bytes);
}

/* ------------------------------------------------------------------------- */
