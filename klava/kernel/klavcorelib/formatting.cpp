#include <kl_types.h>
#include <stdarg.h>
#include <limits>

/* size of conversion buffer (ANSI-specified minimum is 509) */
#define BUFFERSIZE    512


template <int Size>
struct CharBuffer
{
	char buf[Size];
	int currlen;
};

struct StringDesc
{
	StringDesc(): buf(0), currlen(0) 
	{}
	
	void assign(const char* b, int maxlen)
	{
		buf = const_cast<char*>(b);
		if (buf != NULL)
		{
            while (maxlen-- && *b)
                 ++b;
            currlen = (int)(b - buf);	// VS 2005 fix
		}
	}
	
	char* buf;
	int currlen;
};
/*
class weak_string
{
public:
	weak_string() : m_buff(0), m_size(0)
	{
	}
	
	weak_string(const char* buff, size_t size) : m_buff(buff), m_size(size)
	{
	}

	size_t size() const
	{
		return m_size; 
	}
	const char* buff() const
	{
		return m_buff;
	}
	protected:

	const char* m_buff;
	size_t m_size;
};*/

enum PrefixSignFlag
{
	PSF_none = 0,
	PSF_plus,
	PSF_space,
};

enum LengthModifier
{
	LM_none = 0,
	LM_hh,
	LM_h,
	LM_l,
	LM_ll,
	LM_j,
	LM_z,
	LM_t,
	LM_I32,
	LM_I64
};

enum STATE {
    S_NORMAL,          // normal state; outputting literal chars
    S_PERCENT,         // just read '%'
    S_FLAG,            // just read flag character
    S_WIDTH,           // just read width specifier
    S_DOT,             // just read '.'
    S_PRECIS,          // just read precision specifier
    S_SIZE,            // just read size specifier
    S_TYPE             // just read type specifier
};

const char* NULL_STRING = "(null)";  /* string to print on null ptr */

/*
				 S_NOR  S_PER  S_FLA  S_WID  S_DOT  S_PRE  S_SIZ  S_TYP
CH_OTHER         S_NOR, S_NOR, S_NOR, S_NOR, S_NOR, S_NOR, S_NOR, S_NOR,
CH_PERCENT       S_PER, S_NOR, S_NOR, S_NOR, S_NOR, S_NOR, S_NOR, S_PER,
CH_DOT           S_NOR, S_DOT, S_DOT, S_DOT, S_NOR, S_NOR, S_NOR, S_NOR,
CH_STAR          S_NOR, S_WID, S_WID, S_NOR, S_PRE, S_NOR, S_NOR, S_NOR,
CH_ZERO          S_NOR, S_FLA, S_FLA, S_WID, S_PRE, S_PRE, S_NOR, S_NOR,
CH_DIGIT         S_NOR, S_WID, S_WID, S_WID, S_PRE, S_PRE, S_NOR, S_NOR,
CH_FLAG          S_NOR, S_FLA, S_FLA, S_NOR, S_NOR, S_NOR, S_NOR, S_NOR,
CH_SIZE          S_NOR, S_SIZ, S_SIZ, S_SIZ, S_SIZ, S_SIZ, S_SIZ, S_NOR,
CH_TYPE          S_NOR, S_TYP, S_TYP, S_TYP, S_TYP, S_TYP, S_TYP, S_NOR,
*/


class Parser
{
enum CHARTYPE {
    CH_OTHER,           // character with no special meaning
    CH_PERCENT,         // '%' 
    CH_DOT,             // '.'
    CH_STAR,            // '*'
    CH_ZERO,            // '0'
    CH_DIGIT,           // '1'..'9'
    CH_FLAG,            // ' ', '+', '-', '#'
    CH_SIZE,            // 'h', 'l', 'j', 'z', 't', 'I'
    CH_TYPE             // 's', 'd', 'i', 'u', 'p', 'X', 'x', 'o'
};
public:
	Parser() : m_state(S_NORMAL) {}
	STATE NextToken(char c)
	{
		CHARTYPE cc = get_char_class(c);
		switch(m_state){
			case S_NORMAL:
				if (CH_PERCENT == cc)
					m_state = S_PERCENT;
				break;
			case S_PERCENT:
			case S_FLAG:
				if (CH_STAR == cc)
				{
					m_state = S_WIDTH;
					break;
				}
				if (CH_ZERO == cc || CH_FLAG == cc)
				{
					m_state = S_FLAG;
					break;
				}
			case S_WIDTH:
				if (CH_DOT == cc){
					m_state = S_DOT;
					break;
				}
				if (CH_STAR == cc)
				{
					m_state = S_NORMAL;
					break;
				}
				if (CH_ZERO == cc || CH_DIGIT == cc)
				{
					m_state = S_WIDTH;
					break;
				}

			case S_DOT:
				if (CH_STAR == cc)
				{
					m_state = S_PRECIS;
					break;
				}
			case S_PRECIS:
				if (CH_ZERO == cc || CH_DIGIT == cc)
				{
					m_state = S_PRECIS;
					break;
				}
			case S_SIZE:
				if (CH_PERCENT == cc)
				{
					m_state =  S_NORMAL;
					break;
				}
				if (CH_SIZE == cc)
				{
					m_state =  S_SIZE;
					break;
				}
				if (CH_TYPE == cc)
				{
					m_state =  S_TYPE;
					break;
				}
			case S_TYPE:
				if (CH_PERCENT == cc)
				{
					m_state =  S_PERCENT;
					break;
				}
				m_state = S_NORMAL;
		}
		return m_state;
	}
	CHARTYPE get_char_class(char c)
	{
		switch (c)
		{
		case '%':
			return CH_PERCENT;
		case '.':
			return CH_DOT;
		case '*':
			return CH_STAR;
		case '0':
			return CH_ZERO;
		case ' ':
		case '+':
		case '-':
		case '#':
			return CH_FLAG;
		case 'h':
		case 'l':
		case 'j':
		case 'z':
		case 't':
		case 'I':
			return CH_SIZE;
		case 's':
		case 'd':
		case 'i':
		case 'u':
		case 'p':
		case 'X':
		case 'x':
		case 'o':
			return CH_TYPE;
		}
		if  (c>='1' && c<='9')
			return CH_DIGIT;
		return CH_OTHER;
	}
protected:
	STATE m_state;
};

#if defined (_MSC_VER)
#define VA_COPY(dst,src) dst = src
#else
#define VA_COPY(dst,src) va_copy(dst,src)
#endif

struct va_list_wrapper{
  va_list_wrapper (va_list _list) { VA_COPY(argptr,_list);}
  va_list argptr;
};

template <bool nogreater32, bool is_signed>
struct get_argimpl
{
};

template<>
struct get_argimpl<true, true>
{
	static int32_t get(va_list_wrapper *pargptr)
	{
		return va_arg(pargptr->argptr, int32_t);
	}	
};

template<>
struct get_argimpl<true, false>
{
	static uint32_t get(va_list_wrapper *pargptr)
	{
		return va_arg(pargptr->argptr, uint32_t);
	}	
};

template<>
struct get_argimpl<false, true>
{
	static int64_t get(va_list_wrapper *pargptr)
	{
		return va_arg(pargptr->argptr, int64_t);
	}	
};

template<>
struct get_argimpl<false, false>
{
	static uint64_t get(va_list_wrapper *pargptr)
	{
		return va_arg(pargptr->argptr, uint64_t);
	}	
};

template<class T>
struct get_arg
{
	static T get(va_list_wrapper *pargptr)
	{
		return (T)get_argimpl<sizeof(T)<=4, std::numeric_limits<T>::is_signed>::get(pargptr);
	}
};

inline void* get_ptr(va_list_wrapper* pargptr)
{
	return va_arg(pargptr->argptr, void*);
}

template <class T>
int Format(
    T& output,
    const char *format,
    va_list_wrapper argptr
    )
{

	Parser parser;  //parser
	bool is_signed = false; //=true for i,d type characters
    LengthModifier lengthModifier; //see enum LengthModifier (0 - by default)
	PrefixSignFlag  prefixSignFlag;  //see enum PrefixSignFlag (0 - by default)
	StringDesc text;
    CharBuffer<2> prefix;
	bool is_left = false; 
	bool is_leadzero = false;
	bool is_alternate = false;    
	bool is_negative = false;
	bool is_forceoctal; //force leading '0' for octals 
    int charsout = 0; 
    int radix;
    int hexadd; 
    int precision;
    int fldwidth = 0;
    char c;
    
	CharBuffer<BUFFERSIZE> buffer;
    while ((c = *format++) != '\0' && charsout >= 0) {

        switch (parser.NextToken(c)) {

        case S_NORMAL:

            //normal state -- just write character
            output.write_char(c, &charsout);
            break;

        case S_PERCENT:
			//set def vals
            prefix.currlen = fldwidth = 0;
			is_signed = false;
            lengthModifier = LM_none;
			prefixSignFlag = PSF_none;
			precision = -1;
			is_left = false;
			is_leadzero = false;
			is_alternate = false;
			is_negative = false;
			is_forceoctal = false;
            break;

        case S_FLAG:
            switch (c) {
            case '-':
                is_left = true; 
                break;
            case '+':
				prefixSignFlag = PSF_plus;
                break;
            case ' ':
				prefixSignFlag = PSF_space;
                break;
            case '#':
                is_alternate = true;
                break;
            case '0':
                is_leadzero = true;
                break;
            }
            break;

        case S_WIDTH:
            if (c == '*') {
                fldwidth = get_arg<int>::get(&argptr);
                if (fldwidth < 0) {
                    is_left = true;
                    fldwidth = -fldwidth;
                }
            }
            else {
                fldwidth = fldwidth * 10 + (c - '0');
            }
            break;

        case S_DOT:
            precision = 0;
            break;

        case S_PRECIS:
            if (c == '*') {
                if ((precision = get_arg<int>::get(&argptr)) < 0)
                    precision = -1;
            }
            else {
                precision = precision * 10 + (c - '0');
            }
            break;

        case S_SIZE:
            switch (c) {

            case 'l':
				if (*format == 'l')
				{
					++format;
					lengthModifier = LM_ll;
				}else {
					lengthModifier = LM_l;
				}
                break;

			case 'h':
				if (*format == 'h')
				{
					++format;
					lengthModifier = LM_hh;
				}else {
					lengthModifier = LM_h;
				}
                break;

			case 'j':
				lengthModifier = LM_j;
				break;

			case 'z':
				lengthModifier = LM_z;
				break;

			case 't':
				lengthModifier = LM_t;
				break;


            case 'I':
                if ( (*format == '6') && (*(format + 1) == '4') ) {
                    format += 2;
                    lengthModifier = LM_I64;
                }
                if ( (*format == '3') && (*(format + 1) == '2') ) {
                    format += 2;
                    lengthModifier = LM_I32;
                }
                break;
            }
            break;

        case S_TYPE:
            switch (c) {
            case 's': {
				//string
                char *p =  reinterpret_cast<char*>(get_ptr(&argptr));
                text.assign(p?p:NULL_STRING, (precision == -1) ? INT_MAX : precision);
            }
            break;
			
            case 'd':
            case 'i':
                //signed decimal
                is_signed = true;
                radix = 10;
                goto COMMON_INT;

            case 'u':
                radix = 10;
                goto COMMON_INT;

            case 'p':
                //pointer
                precision = 2 * sizeof(void *);
				if (sizeof(void *)==4)
					lengthModifier = LM_I32;
				else
					lengthModifier = LM_I64;
				//to hex
            case 'X':
                hexadd = 'A' - '9' - 1;
                goto COMMON_HEX;

            case 'x':
                hexadd = 'a' - '9' - 1;     

            COMMON_HEX:
                radix = 16;
                if (is_alternate) {
                    prefix.buf[0] = '0';
                    prefix.buf[1] = (char)('x' - 'a' + '9' + 1 + hexadd);
                    prefix.currlen = 2;
                }
                goto COMMON_INT;

            case 'o':
                radix = 8;
                if (is_alternate) {
                    is_forceoctal = true;
                }

            COMMON_INT: {


                uint64_t number;		//number to convert
                int digit;				//ascii value of digit
                int64_t l;              //temp value 

				if (LM_I32 == lengthModifier)
				{
                    if (is_signed)
                        l = get_arg<int32_t>::get(&argptr);
                    else
                        l = get_arg<uint32_t>::get(&argptr);
				} else
				if (LM_I64 == lengthModifier)
				{
                    l = get_arg<int64_t>::get(&argptr);
				} else
				if (LM_ll == lengthModifier)
				{
					l = get_arg<int64_t>::get(&argptr);
				} else
				if (LM_l == lengthModifier)
				{
                    if (is_signed)
                        l = get_arg<long>::get(&argptr);
                    else
                        l = get_arg<unsigned long>::get(&argptr);
				} else
				if (LM_h == lengthModifier){
                    if (is_signed)
                        l = get_arg<short>::get(&argptr);
                    else
                        l = get_arg<unsigned short>::get(&argptr);
                } else
				if (LM_hh == lengthModifier)
				{
                    if (is_signed)
                        l = get_arg<signed char>::get(&argptr);
                    else
                        l = get_arg<unsigned char>::get(&argptr);
				} else
				if (LM_j == lengthModifier)
				{
                    if (is_signed)
						l = get_arg<intmax_t>::get(&argptr); 
                    else
						l = get_arg<uintmax_t>::get(&argptr); 

				} else
				if (LM_z == lengthModifier)
				{
					l = get_arg<size_t>::get(&argptr); 
				} else
				if (LM_t == lengthModifier)
				{
					l = get_arg<ptrdiff_t>::get(&argptr); 
				} else
                {
                    if (is_signed)
						l = get_arg<int>::get(&argptr); 
                    else
						l = get_arg<unsigned int>::get(&argptr); 
                }

                if ( is_signed && l < 0) {
                    number = -l;
                    is_negative = true; 
                } else {
                    number = l;
                }

                if (precision < 0)
                    precision = 1; 
                else
                    is_leadzero = false;

                if (number == 0)
                    prefix.currlen = 0;


                text.buf = buffer.buf + (BUFFERSIZE-1);    

                {
				while (precision-- > 0 || number != 0) {
                    digit = (int)(number % radix) + '0';
					number /= radix; //operator / for uint64 reqare aulldiv                
                    if (digit > '9') {
                        digit += hexadd;
                    }
                    *text.buf-- = (char)digit;       
                }
                }

                text.currlen = (int)(buffer.buf + (BUFFERSIZE-1) - text.buf);	// VS 2005 fix
                ++text.buf;          


                if ((is_forceoctal) && (text.buf[0] != '0' || text.currlen == 0)) {
                    *--text.buf = '0';
                    ++text.currlen;      
                }
            }
            break;
            }

                if (is_signed) {
                    if (is_negative) {
                        prefix.buf[0] = '-';
                        prefix.currlen = 1;
                    }
                    else if (PSF_plus == prefixSignFlag) {
                        prefix.buf[0] = '+';
                        prefix.currlen = 1;
                    }
                    else if (PSF_space == prefixSignFlag) {
                        prefix.buf[0] = ' ';
                        prefix.currlen = 1;
                    }
                }

                int padding = fldwidth - text.currlen - prefix.currlen;

                if (!(is_left||is_leadzero)) {
                    output.write_n_chars(' ', padding, &charsout);
                }

                output.write_string(prefix.buf, prefix.currlen, &charsout);

                if (is_leadzero && !is_left) {
                    output.write_n_chars('0', padding, &charsout);
                }

                output.write_string(text.buf, text.currlen, &charsout);

                if (is_left) {
                    output.write_n_chars(' ', padding, &charsout);
                }


            break;
        }
    }

    return charsout;       
}


class BuffOut
{
public:
	BuffOut(char* buff) : m_buff(buff)
	{
	}

	~BuffOut()
	{
	}
	
	void write_char(int ch, int *pnumwritten)
	{
		*(m_buff++) = (char)ch;
        ++(*pnumwritten);
	}

	void write_n_chars(int ch, int num, int *pnumwritten)
	{
	    while (num-- > 0) {
	        write_char(ch, pnumwritten);
	    }
	}

	void write_string(char *string, int len, int *pnumwritten)
	{
	    while (len-- > 0) {
	        write_char(*string++, pnumwritten);
		 }
	}
protected:
	char* m_buff;
};

struct NoOut
{
	void write_char(int ch, int *pnumwritten)
	{
        ++(*pnumwritten);
	}

	void write_n_chars(int ch, int num, int *pnumwritten)
	{
	    while (num-- > 0) {
	        write_char(ch, pnumwritten);
	    }
	}

	void write_string(char *string, int len, int *pnumwritten)
	{
	    while (len-- > 0) {
	        write_char(*string++, pnumwritten);
		 }
	}
};

class BuffNOut
{
public:
	BuffNOut(char* buff, size_t N) : m_buff(buff), m_failbit(false), m_N(N)
	{
	}
	~BuffNOut()
	{
		m_buff = 0;
		m_N = 0;
	}

	size_t count()
	{
		return m_N;
	}

	bool operator!()
	{
		return m_failbit;
	}
	
	void write_char(int ch, int *pnumwritten)
	{
		if (m_N){
			--m_N;
			if (m_buff)
				*(m_buff++) = (char)ch;
		} else {
			m_failbit = true;
		}
        ++(*pnumwritten);
	}

	void write_n_chars(int ch, int num, int *pnumwritten)
	{
	    while (num-- > 0) {
	        write_char(ch, pnumwritten);
	    }
	}

	void write_string(char *string, int len, int *pnumwritten)
	{
	    while (len-- > 0) {
	        write_char(*string++, pnumwritten);
		 }
	}
protected:
	char* m_buff;
	bool m_failbit;
	size_t m_N;
};

int kl_sprintf (char* buff, const char * format,...)
{
	va_list arglist;
	va_start(arglist, format);
	BuffOut b(buff);
        va_list_wrapper argptr ( arglist );
    int res = Format(b, format, argptr);
	if (res>=0){
		if (buff)
			buff[res]='\0';
	}
	return res;
}

int kl_vsprintf (char* buff, const char * format, va_list arglist)
{
	BuffOut b(buff);
        va_list_wrapper argptr ( arglist );
    int res = Format(b, format, argptr);
	if (res>=0){
		if (buff)
			buff[res]='\0';
	}
	return res;
}

int kl_scprintf (const char * format,...)
{
	va_list arglist;
	va_start(arglist, format);
	NoOut b;
        va_list_wrapper argptr ( arglist );
    return Format(b, format, argptr);
}

int kl_vscprintf (const char * format, va_list arglist)
{
	NoOut b;
        va_list_wrapper argptr ( arglist );
    return Format(b, format, argptr);
}


int kl_snprintf (char* buff, size_t count, const char * format,...)
{
	va_list arglist;
	va_start(arglist, format);
	BuffNOut b(buff, count-1);
        va_list_wrapper argptr ( arglist );
    int res = Format(b, format, argptr);
	if (res >= 0){
		if (buff)
		{
			if (!b)
				buff[count] = '\0';
			else
				buff[res]='\0';
		}
	}
	return res;
}

int kl_vsnprintf (char* buff, size_t count, const char * format, va_list arglist)
{
	BuffNOut b(buff, count-1);
        va_list_wrapper argptr ( arglist );
    int res = Format(b, format, argptr);
	if (res >= 0){
		if (buff)
		{
			if (!b)
				buff[count] = '\0';
			else
				buff[res]='\0';
		}
	}
	return res;
}
