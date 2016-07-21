////////////////////////////////////////////////////////////////////
//
//  SQ_U.CPP
//  Unpack proc for squish
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
//  20.01.2005 - Optmized by Dmitry Glavatskikh
//
////////////////////////////////////////////////////////////////////

#include <memory.h>
#include "fakeoop.h"

#define fillbitbuf() {                          \
  s_buf = (unsigned int)(s_fp[0]|(s_fp[1]<<8)); \
  s_count = 16;                                 \
  s_fp += 2;                                    \
}

#define getbit(b) {       \
  b = (s_buf & 1);        \
  if( --s_count )         \
    s_buf = (s_buf >> 1); \
  else fillbitbuf();      \
}
  
#define getbyte() (*s_fp++)

static unsigned long process(unsigned char* i_buf, unsigned char* o_buf)
{
  unsigned char*  s_fp  = i_buf;
  unsigned char*  o_ptr = o_buf;
  unsigned int    s_count = 0;
  unsigned int    y_count = 0;
  unsigned int    s_buf;
  unsigned long   len;
  signed   short  span;
  int             bit;

  fillbitbuf();
  while(1)
  {
    getbit(bit);
    if(bit)
    {
      *o_ptr++ = getbyte();
      continue;
    }
    getbit(bit);
    if(!bit)
    {
      getbit(len);
      len <<= 1;
      getbit(bit);
      len |= bit;
      len += 2;
      span = getbyte()|0xff00;
    }
    else
    {
      span = getbyte();
      len  = getbyte();
      span |= ((len & ~7) << 5)|0xe000;
      len  = (len & 7) + 2;
      if ( len == 2 )
      {
        len = getbyte();
        if( len == 0 )
          return(o_ptr - o_buf); /* end mark of compreesed load module */
        else if( len == 1 )
          continue; /* segment change */
        else
          len++;
      }
    }
    
    while( len-- )
    {
      *o_ptr = *(o_ptr + span);
       o_ptr++;
    }
  }

  return(0); /* error */
}

unsigned long unsqu(unsigned char* i_buf, unsigned char* o_buf)
{
  return process(i_buf, o_buf);
}
