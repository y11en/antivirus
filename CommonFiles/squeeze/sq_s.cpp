#include "sq_s.h"
#include <memory.h>
#include <stdio.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

static BYTE outbuf [36];
static DWORD written_bytes;
static BYTE index_byte;
static BYTE index_bit;
static DWORD icount;

# define  put_byte(byte)  outbuf [index_byte++ + 2] = (BYTE)(byte);


static int put_bit (FILE *f, BYTE bit)
{
	try
	{
		*(WORD *)outbuf >>= 1;
		*(WORD *)outbuf |= (bit & 1) ? 0x8000 : 0;
		if (++index_bit == 0x10)
		{
			if(fwrite (outbuf, index_byte + 2, 1, f) != 1)
			{
				written_bytes = 0;
				return 1;
			}

			written_bytes += index_byte + 2;
			index_bit = index_byte = 0;
		}
	}
	catch(...)
	{
		written_bytes = 0;
		return 1;
	}

	return 0;
}

static int put_rest (FILE *f)
{
	try
	{
		*(WORD *)outbuf >>= 0x10 - index_bit;
		if(fwrite (outbuf, index_byte + 2, 1, f) != 1)
		{
			written_bytes = 0;
			return 1;
		}
		written_bytes += index_byte + 2;
	}
	catch(...)
	{
		written_bytes = 0;
		return 1;
	}

	return 0;
}


DWORD Squeeze::squeeze (BYTE *inf, FILE *outf, const DWORD length)
{
	try
	{
		int read_size, i;
		short int len, maxlen;
		short int buffer_top;
		
		short int span,	s_minimum, i_minimum, j;
		long bytes_total, bytes_now;

		bytes_total = length;
		
		written_bytes = 0L;
		index_bit = index_byte = 0;
		bytes_now = 0L;
		
		icount = 0;
		short int  *last_occurence = new short int[256];
		short int  *char_list = new short int[0x4500];
		BYTE *buffer = new BYTE[0x4500];
		
		read_size = (length < 0x4500) ? length : 0x4500;
		
		//read_size = fread (buffer, 1, read_size, inf);
		memcpy (buffer, inf, read_size);
		BYTE *inf_position = inf + read_size;

		icount += read_size;
		
		buffer_top = read_size;
		
		for ( i = 0; i < 256; i++ ) last_occurence[i] = -1;

		char_list[0] = -1;
		last_occurence[ (WORD) *buffer ] = 0;
		
		if (put_bit(outf,1)) goto ret;
		put_byte(*buffer);
		
		for (i = 1;i < buffer_top;i++)
		{
			BYTE *ptr = buffer + i;
			short int s;
			char_list[i] = last_occurence[(WORD)*ptr];
			last_occurence[(WORD)*ptr] = i;

			len = 0;
			maxlen = (buffer_top - i < 0x100 ? buffer_top - i : 0x100);
			s_minimum = -(i > 0x2000 ? 0x2000 : i);
			i_minimum = (i > 0x2000 ? i-0x2000 : 0);

			for (j = char_list[i] ; j >= i_minimum;	j = char_list[j] )
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
					if (put_bit (outf,len >> 1)) goto ret;
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
						put_byte (len-1);
					}
				}
			}
			else
			{
				if (put_bit(outf, 1)) goto ret;
				put_byte(buffer[i]);
			}

			if ( i > 0x4000 )
			{ 
				// Смещаем буфер на 0x2000 и читаем очередную порцию
				for (j = 0;j < 256;j++)
					last_occurence[j] = ( last_occurence[j] < 0x2000 ? -1 : last_occurence[j] - 0x2000 );

				for (j = 0x2000;j < buffer_top;j++)
				{
					buffer[j - 0x2000] = buffer[j];
					char_list[j - 0x2000] = char_list[j] < 0x2000 ? -1 : char_list[j] - 0x2000;
				}

				i -= 0x2000;
				buffer_top -= 0x2000;
				bytes_now += 0x2000;

				read_size = ((length - icount)<(DWORD)(0x4500 - buffer_top))?(length - icount):(0x4500 - buffer_top);
				//read_size = fread (buffer + buffer_top, 1, read_size, inf);
				read_size = min (read_size, inf + length - inf_position);
				memcpy (buffer + buffer_top, inf_position, read_size);
				
				inf_position += read_size;

				icount += read_size;

				buffer_top += read_size;
			}
		}
		if (put_bit(outf,0)) goto ret;
		if (put_bit(outf,1)) goto ret;
		put_byte (0xff);
		put_byte (0);
		put_byte (0);
		if (put_rest(outf))  goto ret;
		
ret:
		delete last_occurence;
		delete char_list;
		delete buffer;
	}
	catch(...)
	{
		written_bytes = 0;
	}
	
	return written_bytes;
}
