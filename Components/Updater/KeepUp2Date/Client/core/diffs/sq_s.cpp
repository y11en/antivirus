#include <memory.h>

#include "sq_su.h"

#define put_byte(bb) { outbuf[index_byte++ + 2] = (unsigned char)(bb); }

#define put_bit(memfile, bit_val)                     \
{                                                     \
    unsigned int oval = (outbuf[0]|(outbuf[1]<<8));     \
    oval = (oval >> 1)|((bit_val & 1) ? 0x8000 : 0);    \
    outbuf[0] = (unsigned char)(oval & 0xff);           \
    outbuf[1] = (unsigned char)((oval >> 8) & 0xff);    \
    if( ++index_bit == 16 )                            \
      {                                                   \
          memfile.insert(memfile.end(), outbuf, outbuf + index_byte + 2);   \
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
    memfile.insert(memfile.end(), outbuf, outbuf + index_byte + 2);   \
    wrought_bytes += index_byte + 2;                    \
}


unsigned int squeeze(const unsigned char *inf, std::vector<unsigned char> &outf, const unsigned int length)
{
    unsigned char index_byte = 0;
    unsigned char index_bit = 0;
    unsigned long wrought_bytes = 0L;

    unsigned char outbuf[36];
    memset(outbuf, 0, sizeof(outbuf));

    // alloc working buffer
    const unsigned long wrkcnt = (256 * sizeof(short int)) + 16 + (0x4500 * sizeof(short int)) + 16;
    unsigned char wrkmem[wrkcnt + 0x4500 + 16];

    // setup wrk mem pointers
    short int *last_occurence = reinterpret_cast<short int *>(wrkmem);
    short int *char_list = reinterpret_cast<short int *>(&wrkmem[(256 * sizeof(short int)) + 16]);
    unsigned char *buffer = reinterpret_cast<unsigned char *>(&wrkmem[(256 * sizeof(short int)) + 16 + (0x4500 * sizeof(short int)) + 16]);

    int readed_size = (length < 0x4500) ? length : 0x4500;
    // - readed_size = MemFileRead(inf, buffer, readed_size);
    // TODO: check it
    unsigned long bytes_read = 0;
    memcpy(buffer, inf + bytes_read, readed_size);
    bytes_read += readed_size;

    unsigned long icount = readed_size;
    int buffer_top = readed_size;

    // fill index array
    for(int indexArray = 0; indexArray < 256; ++indexArray)
        last_occurence[indexArray] = -1;

    char_list[0] = -1;
    last_occurence[buffer[0]] = 0;

    put_bit(outf, 1);
    put_byte(buffer[0]);

    for(int i = 1; i < buffer_top; ++i)
    {
        unsigned char *ptr = buffer + i;

        char_list[i] = last_occurence[ptr[0]];
        last_occurence[ptr[0]] = static_cast<short>(i);

        short int len = 0;
        short int span = 0;
        const short int maxlen = (buffer_top - i < 0x100 ? buffer_top - i : 0x100);
        const short int i_minimum = (i > 0x2000 ? i-0x2000 : 0);

        // scan buffer backwards by pointers
        for(short int charListIndex = char_list[i]; charListIndex >= i_minimum; charListIndex = char_list[charListIndex])
        {
            if(charListIndex < i_minimum)
                break;

            const short int s = charListIndex - i;
	        short int l = 0;
            for(l = 1; l < maxlen && ptr[l] == ptr[s + l]; ++l)
                ;

            if(l > len)
            {
                len = l;
                span = s;
                if(l >= maxlen)
                    break;
            }
        }

        if(len > maxlen)
            len = maxlen;

        if(span >= -0xff && len >= 2 || span < -0xff && len > 2)
        {
            put_bit(outf, 0);
            
            for(short int indexPtr = 1; indexPtr < len; ++indexPtr)
            {
                ++i;
                char_list[i] = last_occurence[ptr[indexPtr]];
                last_occurence[ptr[indexPtr]] = i;
            }

            if (len <= 5 && span >= -0xff)
            {
                len -= 2;
                put_bit(outf, 0);
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
                    put_byte(((unsigned short)span >> 5) & ~0x7);
                    put_byte(len-1); /* ! */
                }
            }
        }
        else
        {
            put_bit(outf, 1);
            put_byte(buffer[i]);
        }

        // move buffer on 0x2000 and read next block
        if(i > 0x4000)
        {
            /* fix pointers */
            short int lastOccurenceIndex = 0;
            for(; lastOccurenceIndex < 256; ++lastOccurenceIndex)
                last_occurence[lastOccurenceIndex] = (last_occurence[lastOccurenceIndex] < 0x2000 ? -1 :
            last_occurence[lastOccurenceIndex] - 0x2000);
            for(short int bufferIndex = 0x2000; bufferIndex < buffer_top; ++bufferIndex)
            {
                buffer[bufferIndex - 0x2000] = buffer[bufferIndex];
                char_list[bufferIndex - 0x2000] = char_list[bufferIndex] < 0x2000 ? -1 : char_list[bufferIndex] - 0x2000;
            }

            i -= 0x2000;
            buffer_top -= 0x2000;

            readed_size = ((length - icount) < (unsigned long)(0x4500 - buffer_top))
                ? (length - icount) : (0x4500 - buffer_top);

            memcpy(buffer+buffer_top, inf + bytes_read, readed_size);
            bytes_read += readed_size;

            icount += readed_size;

            buffer_top += readed_size;
        }
    }

    put_bit(outf, 0);
    put_bit(outf, 1);

    put_byte (0xff);
    put_byte (0);
    put_byte (0);
    put_rest(outf);

    return wrought_bytes;
}
