#include <memory.h>


void fillbitbuf(unsigned int &s_buf, const unsigned char *&s_fp, unsigned int &s_count)
{
    s_buf = (unsigned int)(s_fp[0]|(s_fp[1]<<8));
    s_count = 16;
    s_fp += 2;
}


void getbit(int &bit, unsigned int &s_buf, const unsigned char *&s_fp, unsigned int &s_count)
{
    bit = (s_buf & 1);
    if(--s_count)
        s_buf = (s_buf >> 1);
    else
        fillbitbuf(s_buf, s_fp, s_count);
}
  
unsigned int unsqu(const unsigned char *i_buf, unsigned char *o_buf)
{
    const unsigned char *s_fp  = i_buf;
    unsigned char *o_ptr = o_buf;
    unsigned int s_count = 0;
    unsigned int s_buf = 0;
    
    fillbitbuf(s_buf, s_fp, s_count);
    while(true)
    {
        int bit = 0;
        int len = 0L;
        getbit(bit, s_buf, s_fp, s_count);
        if(bit)
        {
            // get byte
            *o_ptr++ = (*s_fp++);
            continue;
        }

        signed short span = 0;
        getbit(bit, s_buf, s_fp, s_count);
        if(!bit)
        {
            getbit(len, s_buf, s_fp, s_count);
            len <<= 1;
            getbit(bit, s_buf, s_fp, s_count);
            len |= bit;
            len += 2;

            span
                = (*s_fp++) // get byte
                | 0xff00;
        }
        else
        {
            span = (*s_fp++);
            len  = (*s_fp++);
            span |= ((len & ~7) << 5)|0xe000;
            len  = (len & 7) + 2;
            if(len == 2)
            {
                len = (*s_fp++);
                // end mark of compressed load module
                if(!len)
                    return(o_ptr - (unsigned char*) o_buf);

                // segment change
                if(len == 1)
                    continue;
                ++len;
            }
        }
        
        while( len-- )
        {
            *o_ptr = *(o_ptr + span);
            ++o_ptr;
        }
    }
    
    // error
    return 0;
}

