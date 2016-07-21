/*
 *  elfbasesunpack.cpp,v 1.2 2002/10/17 09:55:01 tim 
 */

static BYTE	*	s_fp = NULL;
static BYTE	*	o_ptr = NULL;
static WORD		s_buf = 0;
static BYTE		s_count = 0;


static inline WORD GETWORD (WORD *ptr)
{
	register WORD value = 0;
	value |= ((unsigned char *) ptr)[0];
	value <<= 8;
	value |= ((unsigned char *) ptr)[1];
	return value;
}


static void unpack_fillbitbuf()
{
	s_buf = GETWORD((WORD*)s_fp);
	s_fp += 2;
	s_count = 0x10;
}

// Try to avoid multiline defines
#define getbit(b) { b = s_buf & 1; if(--s_count) s_buf >>= 1; else unpack_fillbitbuf(); }
#define getbyte() (*(s_fp++))


static void unpack_init()
{
	s_buf = GETWORD ((WORD*)s_fp);
	s_fp += 2;
	s_count = 0x10;
}

static int unpack_process()
{
	DWORD len, bit;
    short int span;

	while (1)
	{
		getbit(bit);
        
        if ( bit )
        {
			*o_ptr++ = getbyte();
			continue;
        }

		getbit(bit);
        
        if ( !bit )
        {
			getbit (len);
			len <<= 1;
			getbit (bit);
			len |= bit;
			len += 2;
			span = getbyte() | 0xff00;
        } 
        else 
        {
			span = getbyte();
			len = getbyte();
			span |= ((len & ~0x07)<<5) | 0xe000;
			len = (len & 0x07)+2;
			if (len == 2)
			{
				len = getbyte();

				if ( len == 0 )
					return 0;// end mark of compreesed load module

				if ( len == 1 )
					continue;// segment change
				else
					len++;
			}
        }

        for ( ; len > 0; len--, o_ptr++ )
        	*o_ptr = *(o_ptr+span);
	}

	return 1;
}


static DWORD unpack_unsqu (BYTE *i_buf, BYTE *o_buf)
{
    o_ptr = o_buf;
    s_fp = i_buf;
    unpack_init();
    unpack_process();
    return (o_ptr - o_buf);
}
