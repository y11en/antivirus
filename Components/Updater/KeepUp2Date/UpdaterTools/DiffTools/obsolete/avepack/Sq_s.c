#include <stdio.h>
#include <stdlib.h>
#include "fakeoop.h"

static BYTE  outbuf[36];
static DWORD wrought_bytes;
static BYTE  index_byte;
static BYTE  index_bit;
static DWORD icount;
static BYTE  xor;

//static WORD ocount;

#define  put_byte(byte)  outbuf[index_byte++ + 2] = (BYTE)(byte);

static int put_bit (SMemFile *f, BYTE bit)
{
  *(WORD *)outbuf >>= 1;
  *(WORD *)outbuf |= (bit & 1) ? 0x8000 : 0;
  if (++index_bit == 0x10)
  {
/*	if (memcpy (f+ocount,outbuf,index_byte + 2) == NULL)
	  return -1;
	ocount += index_byte + 2;
*/
    if ( xor )
    {
      int k, c;
      for( k=0,c=index_byte+2; k<c; k++ )
        outbuf[k] ^= xor;
    }
	  MemFileWrite(f, outbuf, index_byte + 2);
	  wrought_bytes += index_byte + 2;
	  index_bit = index_byte = 0;
  }
  return 0;
}


static int put_rest (SMemFile *f)
{
	*(WORD *)outbuf >>= 0x10 - index_bit;
/*
	if (memcpy (f+ocount,outbuf,index_byte + 2) == NULL)
	return -1;
	ocount += index_byte + 2;
*/
  if ( xor )
  {
    int k, c;
    for( k=0,c=index_byte+2; k<c; k++ )
      outbuf[k] ^= xor;
  }
	MemFileWrite(f, outbuf, index_byte + 2);
	wrought_bytes += index_byte + 2;
	return 0;
}


/*  Процедура упаковки файла  */
DWORD squeeze ( SMemFile *inf, SMemFile *outf, DWORD length, BYTE xor_byte )
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
  BYTE *buffer;
 //long       perc, perc_old;
/*
  if(length>0x8000){
	printf("\n:ERROR: Paragraph length exeedes 0x8000");
	exit(1);
  }
*/
  xor = xor_byte;
  bytes_total = length;

  wrought_bytes = 0L;
  index_bit = index_byte = 0;
  bytes_now = 0L;

  icount=0;
/*
  ocount=0;
*/
  last_occurence = (short int *)malloc(256*sizeof(short int));
  char_list = (short int *)malloc(0x4500*sizeof(short int));
  buffer = (BYTE *)malloc(0x4500);

  readed_size = length<0x4500?length:0x4500;
/*
  memcpy (buffer,inf+icount,readed_size);
*/
  
  readed_size = MemFileRead(inf, buffer, readed_size);

  icount+=readed_size;

  buffer_top = readed_size;

  /* Заполнение массивов индексов */
  for ( i = 0; i < 256; i++ ) last_occurence[i] = -1;
  char_list[0] = -1;
  last_occurence[ (WORD) *buffer ] = 0;

  if (put_bit (outf,1)) goto ret;
  put_byte (*buffer);

//  perc_old = -1L;

  for (i = 1; i < buffer_top; i++)
  {
   BYTE  *ptr = buffer + i;
   short int    s;
/*
	perc = ( 10000L * i + 10000L * bytes_now )/ ( bytes_total );
	if (  perc - perc_old > 499 )
	{
//	  printf( "\rsq: %3ld.%02ld%% ", perc/100L, perc%100L );
	  perc_old = perc;
	}
*/
	char_list[i] = last_occurence[ (WORD) *ptr ];
	last_occurence[ (WORD) *ptr ] = i;

	len = 0;
  span = 0;
	maxlen = (buffer_top - i < 0x100 ? buffer_top - i : 0x100);
	s_minimum = -(i > 0x2000 ? 0x2000 : i);
	i_minimum = (i > 0x2000 ? i-0x2000 : 0);

	/* Просмотр буфера назад по указателям */
	for (j = char_list[i] ; j >= i_minimum;
	j = char_list[j] )
	{
	 short int l;

	  if ( j < i_minimum ) break;
	  s = j - i;

	  for (l = 1; l < maxlen && *(ptr + l) == *(ptr + s + l); l++) ;

	  if (l > len)
	  {

		len = l;
		span = s;
		if (l >= maxlen) break;
	  }
	}
	if( len > maxlen ) len = maxlen;
	if (span >= -0xff && len >= 2 || span < -0xff && len > 2)
	{
	if (put_bit (outf,0)) goto ret;

	for ( j = 1; j < len; j++ )
	{
	  i++;
	  char_list[i] = last_occurence[ (WORD) *(ptr+j) ];
	  last_occurence[ (WORD) *(ptr+j) ] = i;
	}


	  if (len <= 5 && span >= -0xff)
	  {
		len -= 2;
		if (put_bit (outf,0)) goto ret;
		if (put_bit (outf,(BYTE)(len>>1))) goto ret;
		if (put_bit (outf,(BYTE)len)) goto ret;
		put_byte (span & 0xff);
	  }
	  else
	  {
		if (put_bit (outf,1)) goto ret;
		put_byte (span & 0xff);
		if (len <= 9) {
		  len = (len - 2) | (((WORD)span >> 5) & ~0x7);
		  put_byte( len );
		}
		else
		{
		  put_byte (((WORD)span >> 5) & ~0x7);
		  put_byte (len-1); /* ! */
		}
	  }
	}
	else
	{
	  if (put_bit (outf,1))   goto ret;
	  put_byte (buffer [i]);
	}
	if ( i > 0x4000 )
	{ /* Смещаем буфер на 0x2000 и читаем очередную порцию */

	  /* Коррекция указателей */
	  for ( j=0; j<256; j++ )
	  last_occurence[j] = ( last_occurence[j] < 0x2000 ? -1 :
							last_occurence[j] - 0x2000 );
	  for ( j=0x2000; j<buffer_top; j++ )
	  {
		  buffer[j-0x2000] = buffer[j];
		  char_list[j-0x2000] = char_list[j] <0x2000 ? -1 : char_list[j] - 0x2000;
	  }
	  i -= 0x2000;
	  buffer_top -= 0x2000;
	  bytes_now += 0x2000;

	  readed_size = ((length-icount)<(DWORD)(0x4500-buffer_top))?(length-icount):(0x4500-buffer_top);


//	  memcpy (buffer+buffer_top,inf+icount,readed_size);
	  readed_size = MemFileRead(inf, buffer+buffer_top, readed_size);
	  icount+=readed_size;

	  buffer_top += readed_size;
	}
  }
  if (put_bit (outf,0)) goto ret;
  if (put_bit (outf,1)) goto ret;
//  put_byte (-0xff);
  put_byte (0xff);
  put_byte (0);
  put_byte (0);
  if (put_rest (outf))  goto ret;

//  printf( "\rsq: 100.00%% ", perc/100L, perc%100L );
ret:
  if ( last_occurence )
    free( last_occurence );
  if ( char_list )
    free( char_list );
  if ( buffer )
    free( buffer );
  return wrought_bytes;
}


