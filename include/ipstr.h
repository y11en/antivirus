#ifndef __IPSTR_H__
#define __IPSTR_H__

namespace IPStr
{

struct IPv6
{
    union
    {
        unsigned char  bytes [16];
        unsigned short words [8];
        unsigned int   dwords[4];
    };
};

template <int n>
int hex(int v) { return (v >> n*4) & 0xF; }

template <int n> struct power10    { enum { value = 10 * power10<n-1>::value }; };
template <>      struct power10<0> { enum { value = 1 }; };

template <int n>
int dec(int v) { return ( v / power10<n>::value ) % 10; }

inline size_t IPv4ToStr(unsigned int ipv4, char* str, size_t size)
{
    using namespace IPStr;
    char digits[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

    size_t left = size;
    size_t needed = 0;
    #define strput(c) (str && left ? ( *str++ = (c), left-- ) : 0, needed++)

    unsigned char *b = (unsigned char *)&ipv4;
    for (int i = 3; i >= 0; --i)
    {
        if (i != 3)
            strput('.');

        bool nzero = false;
        if (dec<2>(b[i]))          strput(digits[dec<2>(b[i])]), nzero = true;
        if (dec<1>(b[i]) || nzero) strput(digits[dec<1>(b[i])]);
                                   strput(digits[dec<0>(b[i])]);
    }
    strput('\0');

    #undef strput

    return needed;
}

inline size_t IPv6ToStr(const unsigned char* ipv6buff, unsigned int ipv6zone, char* str, size_t size)
{
    using namespace IPStr;
    char digits[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

    const IPv6*ipv6 = (const IPv6 *)ipv6buff;

    size_t left = size;
    size_t needed = 0;
    #define strput(c) (str && left ? ( *str++ = (c), left-- ) : 0, needed++)

    bool ipv4 = false;
    int minword = 0;

    if (ipv6->dwords[1] == 0x00005efe ||
        ipv6->dwords[3] == 0x00000000 && 
        ipv6->dwords[2] == 0x00000000 &&
            ( ipv6->dwords[1] == 0x0000ffff ||
              ipv6->dwords[1] == 0x00000000 && 
              ipv6->dwords[0] >  0xffff)
       )
    {
        ipv4 = true;
        minword = 2;
    }

    const unsigned short* p = ipv6->words;
    int zeros = 0;
    int start = 0;
    int maxzeros = 1;
    int maxstart = -1;
    int i = 0;
    for (i = 7; i >= minword; --i)
    {
        if (p[i] == 0) 
        {
            if (!start) start = i;
            zeros++;
        }
        else
        {
            if (zeros > maxzeros)
            {
                maxzeros = zeros,
                maxstart = start;
            }
            start = 0;
            zeros = 0;
        }
    }
    if (zeros > maxzeros)
    {
        maxzeros = zeros,
        maxstart = start;
    }
    
    bool ellipsis = false;
    for (i = 7; i >= minword; --i)
    {
        if (i > maxstart || i <= maxstart - maxzeros)
        {
            if (i != 7)
                strput(':');

            bool nzero = false;
            if (hex<3>(p[i]))          strput(digits[hex<3>(p[i])]), nzero = true;
            if (hex<2>(p[i]) || nzero) strput(digits[hex<2>(p[i])]), nzero = true;
            if (hex<1>(p[i]) || nzero) strput(digits[hex<1>(p[i])]);
                                       strput(digits[hex<0>(p[i])]);
        }
        else
        {
            if (!ellipsis)
                strput(':');
            ellipsis = true;
        }
    }
    if (ipv4)
    {
        const unsigned char *b = ipv6->bytes;
        for (int i = 3; i >= 0; --i)
        {
            if (i != 3)
                strput('.');
            else
                strput(':');

            bool nzero = false;
            if (dec<2>(b[i]))          strput(digits[dec<2>(b[i])]), nzero = true;
            if (dec<1>(b[i]) || nzero) strput(digits[dec<1>(b[i])]);
                                       strput(digits[dec<0>(b[i])]);
        }
    }
    if (maxstart != -1 &&
        maxstart - maxzeros < 0)
        strput(':');

    if (ipv6zone)
    {
        strput('%');

        bool nzero = false;
        if (dec<9>(ipv6zone))          strput(digits[dec<9>(ipv6zone)]), nzero = true;
        if (dec<8>(ipv6zone) || nzero) strput(digits[dec<8>(ipv6zone)]), nzero = true;
        if (dec<7>(ipv6zone) || nzero) strput(digits[dec<7>(ipv6zone)]), nzero = true;
        if (dec<6>(ipv6zone) || nzero) strput(digits[dec<6>(ipv6zone)]), nzero = true;
        if (dec<5>(ipv6zone) || nzero) strput(digits[dec<5>(ipv6zone)]), nzero = true;
        if (dec<4>(ipv6zone) || nzero) strput(digits[dec<4>(ipv6zone)]), nzero = true;
        if (dec<3>(ipv6zone) || nzero) strput(digits[dec<3>(ipv6zone)]), nzero = true;
        if (dec<2>(ipv6zone) || nzero) strput(digits[dec<2>(ipv6zone)]), nzero = true;
        if (dec<1>(ipv6zone) || nzero) strput(digits[dec<1>(ipv6zone)]);
                                       strput(digits[dec<0>(ipv6zone)]);
    }

    strput('\0');

    #undef strput

    return needed;
}

inline bool ishex(int c) { return c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';  }
inline int unhex(int c) 
{
    if (c >= '0' && c <= '9') return ((c - '0') & 0xF);
    if (c >= 'a' && c <= 'f') return ((c - 'a') & 0xF) + 0xa;
    if (c >= 'A' && c <= 'F') return ((c - 'A') & 0xF) + 0xa;
    return 0;
}
inline bool isdec(int c) { return c >= '0' && c <= '9';  }
inline int undec(int c) 
{
    if (c >= '0' && c <= '9') return (c - '0');
    return 0;
}
inline bool isoct(int c) { return c >= '0' && c <= '7';  }
inline int unoct(int c) 
{
    if (c >= '0' && c <= '7') return (c - '0');
    return 0;
}

inline bool IPv4FromStr(const char* str, unsigned int *ip)
{
    if (!str || !ip)
        return false;

    unsigned char*  bytes = (unsigned char*)ip;

    int state = 0;

    int nbyte = 3;              // current byte number
    unsigned int currbyte = 0;  // current byte value

    bool stop = false;
    while (!stop)
    {
        int c = *str++;
        if (c == 0) stop = true;

        switch (state)
        {
        case 0: // first digit
            if (c == '0')
                currbyte = 0,
                state = 2;
            else if (isdec(c))
                currbyte = undec(c),
                state = 1;
            else // error
                return false;
            break;
        case 1: // second+ decimal digit
            if (isdec(c))
                currbyte *= 10,
                currbyte += undec(c),
                state = 1;
            else if (c == '.')
            {
                if (currbyte > 255) return false;
                bytes[nbyte--] = currbyte;
                if (nbyte < 0) return false;
                state = 0;
            }
            else if (c == '\0')
            {
                if (nbyte == 3) 
                    *ip = currbyte;
                else if (nbyte == 0)
                {
                    if (currbyte > 255) return false;
                    bytes[nbyte] = currbyte;
                }
                else
                    return false;
            }
            else // error
                return false;
            break;
        case 2: // first octal or hexadecimal digit
            if (c == 'x' || c == 'X')
                state = 4;
            else if (isoct(c))
                currbyte = unoct(c),
                state = 3;
            else if (c == '.')
            {
                if (currbyte > 255) return false;
                bytes[nbyte--] = currbyte;
                if (nbyte < 0) return false;
                state = 0;
            }
            else if (c == '\0')
            {
                if (nbyte == 3)
                    *ip = currbyte;
                else if (nbyte == 0)
                {
                    if (currbyte > 255) return false;
                    bytes[nbyte] = currbyte;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case 3: //second+ octal digit
            if (isoct(c))
                currbyte *= 8,
                currbyte += unoct(c),
                state = 3;
            else if (c == '.')
            {
                if (currbyte > 255)return false;
                bytes[nbyte--] = currbyte;
                if (nbyte < 0) return false;
                state = 0;
            }
            else if (c == '\0')
            {
                if (nbyte == 3)         
                    *ip = currbyte;
                else if (nbyte == 0)
                {
                    if (currbyte > 255) return false;
                    bytes[nbyte] = currbyte;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        case 4: // first hexadecimal digit
            if (ishex(c))
                currbyte = unhex(c),
                state = 5;
            else
                return false;
            break;
        case 5: //second+ hexadecimal digit
            if (ishex(c)) 
                currbyte *= 16,
                currbyte += unhex(c),
                state = 5;
            else if (c == '.')
            {
                if (currbyte > 255) return false;
                bytes[nbyte--] = currbyte;
                if (nbyte < 0) return false;
                state = 0;
            }
            else if (c == '\0')
            {
                if (nbyte == 3)         
                    *ip = currbyte;
                else if (nbyte == 0)
                {
                    if (currbyte > 255) return false;
                    bytes[nbyte] = currbyte;
                }
                else
                    return false;
            }
            else
                return false;
            break;
        }
    }
    return true;
}

inline bool IPv6FromStr(const char* str, unsigned char *ip, unsigned int *zone)
{
    if (!str || !ip || !zone)
        return false;

    unsigned short* words = (unsigned short*)ip;
    unsigned char*  bytes = (unsigned char*)ip;

    *zone = 0;

    int state = 0;

    int digits = 0;
    int nword = 7; // current word number
    int eword = 7; // ellipsis begin word number
    int fword = 0; // finish word number
    int nbyte = 3; // current byte number (IPv4 mode)
    bool ellipsis = false;
    unsigned int currword = 0; // current word
    unsigned int currbyte = 0; // current byte (IPv4 mode)

    bool stop = false;
    while (!stop)
    {
        int c = *str++;
        if (c == 0) stop = true;

        switch (state)
        {
        case 0: // beginning of address
            if (ishex(c)) // word
                currword = unhex(c),
                digits = 1,
                state = 3;
            else if (c == ':') // ellipsis begin
                state = 4;
            else // error
                return false;
            break;
        case 1: // regular word, the first digit in the word expected
            if (ishex(c)) // word
                currword = unhex(c),
                digits = 1,
                state = 3;
            else if (c == ':') // ellipsis end
            {
                if (ellipsis) return false;
                ellipsis = true;
                eword = nword;
                state = 2;
            }
            else // error
                return false;
            break;
        case 2: // after ellipsis, the first digit in the word expected
            if (ishex(c)) // word
                currword = unhex(c),
                digits = 1,
                state = 3;
            else if (c == '%') // zone
                state = 7;
            else if (c == '\0') // end of address
            {}
            else // error
                return false;
            break;
        case 3: // the second+ digit in the word expected
            if (ishex(c)) // word
            {
                if (digits >= 4) return false;
                currword = currword << 4;
                currword |= unhex(c);
                digits++;
                state = 3;
            }
            else if (c == ':') // end of word
            {
                words[nword--] = currword;
                if (nword < (ellipsis ? 1 : 0))
                    return false;
                state = 1;
            }
            else if (c == '%') // zone
            {
                words[nword--] = currword;
                if (ellipsis) { if (nword < 0)   return false; }
                else          { if (nword != -1) return false; }
                state = 7;
            }
            else if (c == '\0') // end of address
            {
                words[nword--] = currword;
                if (ellipsis) { if (nword < 0)   return false; }
                else          { if (nword != -1) return false; }
            }
            else if (c == '.') // IPv4
            {
                if (ellipsis) { if (nword < 2)   return false; }
                else          { if (nword != 1)  return false; }

                // translate last word back to decimal byte
                if (digits > 3) return false;
                int d1 = (currword >> 8) & 0xF;
                int d2 = (currword >> 4) & 0xF;
                int d3 = (currword)      & 0xF;
                if (d1 > 9 || d2 > 9 || d3 > 9) return false;
                currbyte = d1*100 + d2*10 + d3;
                if (currbyte > 255) return false;
                bytes[nbyte--] = currbyte;
                fword = 2;
                state = 5;
            }
            else // error
                return false;
            break;
        case 4: // second character of leading :: expected
            if (c == ':') // ellipsis end
            {
                if (ellipsis) return false;
                ellipsis = true;
                eword = nword;
                state = 2;
            }
            else // error
                return false;
            break;
        case 5: // IPv4 first digit
            if (isdec(c))
                currbyte = undec(c),
                digits = 1,
                state = 6;
            else // error
                return false;
            break;
        case 6: // IPv4 second+ digit
            if (isdec(c))
            {
                if (digits >= 3) return false;
                currbyte *= 10;
                currbyte += undec(c);
                digits++;
                if (currbyte > 255) return false;
                state = 6;
            }
            else if (c == '.')
            {
                bytes[nbyte--] = currbyte;
                if (nbyte < 0) return false;
                state = 5;
            }
            else if (c == '%')
            {
                bytes[nbyte--] = currbyte;
                if (nbyte != -1) return false;
                state = 7;
            }
            else if (c == '\0')
            {
                bytes[nbyte--] = currbyte;
                if (nbyte != -1) return false;
            }
            else // error
                return false;
            break;
        case 7: // zone, first digit
            if (isdec(c))
                *zone *= 10,
                *zone += undec(c),
                digits = 1,
                state = 8;
            else // error
                return false;
            break;
        case 8: // zone, second digit
            if (isdec(c))
            {
                if (digits > 9) return false;
                *zone *= 10,
                *zone += undec(c);
            }
            else if (c == '\0') // end of address
            {}
            else // error
                return false;
            break;
        }
    }
    if (ellipsis)
    {
        int d = nword - fword + 1;
        int s = eword - nword;

        int n = fword;
        for (; n < fword + s; ++n) words[n] = words[n + d];
        for (; n < eword + 1; ++n) words[n] = 0x0000;
    }
    return true;
}

inline bool sscan_num(const char *str, unsigned int *num)
{
	*num = 0;
	for(; *str; str++)
	{
		char ch = *str - '0';
		if( ch < 0 || ch > 9 )
			return false;

		*num *= 10;
		*num += ch;
	}
	return true;
}

}

#endif // __IPSTR_H__


