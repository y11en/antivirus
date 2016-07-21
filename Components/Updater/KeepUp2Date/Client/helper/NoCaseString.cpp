#include "NoCaseString.h"


#ifdef WSTRING_SUPPORTED

    KLUPD::NoCaseString::NoCaseString(const std::wstring &input, const size_t offset)
     : m_string(input.c_str() + offset)
    {
    }
    KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const std::wstring &right)
    {
        m_string = right.c_str();
        return *this;
    }

#else // WSTRING_SUPPORTED

    KLUPD::NoCaseString::NoCaseString(const std::string &input, const size_t offset)
     : m_string(input.c_str() + offset)
    {
    }
    KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const std::string &right)
    {
        fromAscii(right);
        return *this;
    }

    KLUPD::NoCaseString::NoCaseString(const char *right)
        : m_string(right)
    {
    }
    KLUPD::NoCaseString::NoCaseString(const char *right, const size_t _Count)
        : m_string(right, _Count)
    {
    }
    KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const char *right)
    {
        fromAscii(right);
        return *this;
    }

    KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const char right)
    {
        m_string = right;
        return *this;
    }

#endif   // WSTRING_SUPPORTED

const KLUPD::NoCaseStringImplementation::value_type *KLUPD::NoCaseString::toWideChar()const
{
    return m_string.c_str();
}

std::string KLUPD::NoCaseString::toAscii()const
{
#ifdef WSTRING_SUPPORTED
    return wideCharToAscii(m_string.c_str());
#else
    return m_string.c_str();
#endif
}

KLUPD::NoCaseString &KLUPD::NoCaseString::fromAscii(const std::string &input)
{
#ifdef WSTRING_SUPPORTED
    m_string = asciiToWideChar(input).c_str();
#else
    m_string = input.c_str();
#endif
    return *this;
}


bool KLUPD::NoCaseString::toLong(size_t &result)const
{
    // No obligatory integer value specified
    if(m_string.empty())
        return false;

    const std::string value = toAscii();

    // check for digital characters only
    for(unsigned long i = 0; i < value.length(); ++i)
    {
        // invalid integer value specified
        if(!isdigit(value.c_str()[i]))
            return false;
    }

    unsigned long _ret = strtol(value.c_str(), 0, 10);
  
    // invalid integer value specified
    if(errno == ERANGE)
        return false;

    result = _ret;
    return true;
}

int KLUPD::NoCaseString::dvalue(const char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';

    if(c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    if(c >= 'a' && c <= 'f')
        return 10 + c - 'a';

    return -1;
}

long KLUPD::NoCaseString::hexStringToLong()const
{
    long res = 0;
    for(size_t index = 0; index < m_string.size(); ++index)
    {
        const int d = dvalue(NoCaseString(m_string.c_str()).toAscii()[index]);
        if(d == -1)
            break;
        res = res * 16 + d;
    }

    return res;
}

bool KLUPD::NoCaseString::toDouble(double &resultDouble)const
{
    resultDouble = 0.0;

    size_t result = 0;
// TODO: here is data loss, due to conversion to lower size
    if(!toLong(result))
        return false;
    resultDouble = static_cast<double>(result);
    return true;
}

bool KLUPD::NoCaseString::ToTime(tm &_time) const
{
	if( m_string.empty() )
        return false;

	memset(&_time, 0, sizeof(struct tm));
	
	if( !strptime(toAscii().c_str(), "%d%m%Y %H%M", &_time) )
        return false;

	return true;
}
bool KLUPD::NoCaseString::checkDateFormat()const
{
	struct tm _time;
    return ToTime(_time);
}

KLUPD::NoCaseString::NoCaseString()
{
}

KLUPD::NoCaseString::~NoCaseString()
{
}

KLUPD::NoCaseString::NoCaseString(const wchar_t *input)
    #ifdef WSTRING_SUPPORTED
        : m_string(input)
    #else
        : m_string(wideCharToAscii(input).c_str())
    #endif
{
}

KLUPD::NoCaseString::NoCaseString(const wchar_t *input, const size_t _Count)
    #ifdef WSTRING_SUPPORTED
        : m_string(input, _Count)
    #else
        : m_string(wideCharToAscii(input).c_str(), _Count)
    #endif
{
}

KLUPD::NoCaseString::NoCaseString(const NoCaseString &_Right, const size_t _Roff, const size_t _Count)
 : m_string(_Right.m_string, _Roff, _Count)
{
}

KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const wchar_t *right)
{
    #ifdef WSTRING_SUPPORTED
        m_string = right;
    #else
        m_string = wideCharToAscii(right).c_str();
    #endif
    return *this;
}

KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const NoCaseString &right)
{
    m_string = right.m_string;
    return *this;
}

KLUPD::NoCaseString &KLUPD::NoCaseString::operator=(const wchar_t right)
{
    #ifdef WSTRING_SUPPORTED
        m_string = right;
    #else
        m_string = wideCharToAscii(right);
    #endif
    return *this;
}


KLUPD::NoCaseString &KLUPD::NoCaseString::assign(const NoCaseStringImplementation::value_type *input)
{
    m_string.assign(input);
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::assign(const NoCaseStringImplementation::value_type *input, size_t _Count)
{
    m_string.assign(input, _Count);
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::assign(const NoCaseString &_Str, const size_t off,  size_t _Count)
{
    m_string.assign(_Str.m_string, off,  _Count);
    return *this;
}



KLUPD::NoCaseString KLUPD::NoCaseString::operator+(const wchar_t *right)const
{
    #ifdef WSTRING_SUPPORTED
        return (m_string + right).c_str();
    #else
        return (m_string + wideCharToAscii(right).c_str()).c_str();
    #endif
}
KLUPD::NoCaseString KLUPD::NoCaseString::operator+(const NoCaseString &right)const
{
    return (m_string + right.m_string).c_str();
}

KLUPD::NoCaseString &KLUPD::NoCaseString::operator+=(const wchar_t *right)
{
    m_string
        #ifdef WSTRING_SUPPORTED
            += right;
        #else
            += wideCharToAscii(right).c_str();
        #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::operator+=(const NoCaseString &right)
{
    m_string += right.m_string;
    return *this;
}

KLUPD::NoCaseString &KLUPD::NoCaseString::operator+=(const char _Ch)
{
    #ifdef WSTRING_SUPPORTED
        m_string += asciiToWideChar(_Ch);
    #else
        m_string += _Ch;
    #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::operator+=(const wchar_t _Ch)
{
    #ifdef WSTRING_SUPPORTED
        m_string += _Ch;
    #else
        m_string += wideCharToAscii(_Ch);
    #endif
    return *this;
}

bool KLUPD::NoCaseString::operator==(const NoCaseString &right)const
{
    return m_string == right.m_string;
}
bool KLUPD::NoCaseString::operator!=(const NoCaseString &right)const
{
    return !operator==(right);
}
bool KLUPD::NoCaseString::operator<(const NoCaseString &right)const
{
    return m_string < right.m_string;
}

KLUPD::NoCaseStringImplementation::const_reference KLUPD::NoCaseString::operator[](size_t _Off)const
{
    return m_string[_Off];
}
KLUPD::NoCaseStringImplementation::reference KLUPD::NoCaseString::operator[](const size_t _Off)
{
    return m_string[_Off];
}

KLUPD::NoCaseString::iterator KLUPD::NoCaseString::begin()
{
    return m_string.begin();
}
KLUPD::NoCaseString::const_iterator KLUPD::NoCaseString::begin()const
{
    return m_string.begin();
}
KLUPD::NoCaseString::iterator KLUPD::NoCaseString::end()
{
    return m_string.end();
}
KLUPD::NoCaseString::const_iterator KLUPD::NoCaseString::end()const
{
    return m_string.end();
}

KLUPD::NoCaseString &KLUPD::NoCaseString::erase(const size_t _Pos, const size_t _Count)
{
    m_string.erase(_Pos, _Count);
    return *this;
}

KLUPD::NoCaseString &KLUPD::NoCaseString::replace(iterator _First0, iterator _Last0, const char *_Ptr)
{
    #ifdef WSTRING_SUPPORTED
        m_string.replace(_First0, _Last0, asciiToWideChar(_Ptr).c_str());
    #else
        m_string.replace(_First0, _Last0, _Ptr);
    #endif

    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::replace(iterator _First0, iterator _Last0, const wchar_t *_Ptr)
{
    #ifdef WSTRING_SUPPORTED
        m_string.replace(_First0, _Last0, _Ptr);
    #else
        m_string.replace(_First0, _Last0, wideCharToAscii(_Ptr).c_str());
    #endif

    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::replace(iterator _First0, iterator _Last0, NoCaseString &_Str)
{
    m_string.replace(_First0, _Last0, _Str.m_string);
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::replace(const size_t _Pos1, const size_t _Num1, const char *_Ptr)
{
    #ifdef WSTRING_SUPPORTED
        m_string.replace(_Pos1, _Num1, asciiToWideChar(_Ptr).c_str());
    #else
        m_string.replace(_Pos1, _Num1, _Ptr);
    #endif

    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::replace(const size_t _Pos1, const size_t _Num1, const wchar_t *_Ptr)
{
    #ifdef WSTRING_SUPPORTED
        m_string.replace(_Pos1, _Num1, _Ptr);
    #else
        m_string.replace(_Pos1, _Num1, wideCharToAscii(_Ptr).c_str());
    #endif

    return *this;
}


bool KLUPD::NoCaseString::empty()const
{
    return m_string.empty();
}

size_t KLUPD::NoCaseString::size()const
{
    return m_string.size();
}

void KLUPD::NoCaseString::resize(const size_t _Count, const wchar_t _Ch)
{
    #ifdef WSTRING_SUPPORTED
        return m_string.resize(_Count, _Ch);
    #else
        return m_string.resize(_Count, wideCharToAscii(_Ch));
    #endif
}

KLUPD::NoCaseString KLUPD::NoCaseString::substr(const size_t _Off, const size_t _Count)const
{
    return m_string.substr(_Off, _Count).c_str();
}

size_t KLUPD::NoCaseString::find(const NoCaseString &right, const size_t _Off)const
{
    return m_string.find(right.m_string.c_str(), _Off);
}

size_t KLUPD::NoCaseString::find(const char *_Ptr, const size_t _Off, const size_t _Count)const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find(asciiToWideChar(_Ptr).c_str(), _Off, _Count);
    #else
        return m_string.find(_Ptr, _Off, _Count);
    #endif
}
size_t KLUPD::NoCaseString::find(const wchar_t *_Ptr, const size_t _Off, const size_t _Count)const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find(_Ptr, _Off, _Count);
    #else
        return m_string.find(wideCharToAscii(_Ptr).c_str(), _Off, _Count);
    #endif
}

size_t KLUPD::NoCaseString::rfind(const char _Ch, const size_t _Off)const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.rfind(asciiToWideChar(_Ch), _Off);
    #else
        return m_string.rfind(_Ch, _Off);
    #endif
}
size_t KLUPD::NoCaseString::rfind(const wchar_t _Ch, const size_t _Off)const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.rfind(_Ch, _Off);
    #else
        return m_string.rfind(wideCharToAscii(_Ch), _Off);
    #endif
}
size_t KLUPD::NoCaseString::rfind(const char *_Ptr, const size_t _Off)const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.rfind(asciiToWideChar(_Ptr).c_str(), _Off);
    #else
        return m_string.rfind(_Ptr, _Off);
    #endif
}
size_t KLUPD::NoCaseString::rfind(const wchar_t *_Ptr, const size_t _Off)const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.rfind(_Ptr, _Off);
    #else
        return m_string.rfind(wideCharToAscii(_Ptr).c_str(), _Off);
    #endif
}

size_t KLUPD::NoCaseString::find_first_of(const char *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_first_of(asciiToWideChar(_Ptr).c_str(), _Off);
    #else
        return m_string.find_first_of(_Ptr, _Off);
    #endif
}
size_t KLUPD::NoCaseString::find_first_of(const wchar_t *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_first_of(_Ptr, _Off);
    #else
        return m_string.find_first_of(wideCharToAscii(_Ptr).c_str(), _Off);
    #endif
}

size_t KLUPD::NoCaseString::find_first_not_of(const char *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_first_not_of(asciiToWideChar(_Ptr).c_str(), _Off);
    #else
        return m_string.find_first_not_of(_Ptr, _Off);
    #endif
}
size_t KLUPD::NoCaseString::find_first_not_of(const wchar_t *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_first_not_of(_Ptr, _Off);
    #else
        return m_string.find_first_not_of(wideCharToAscii(_Ptr).c_str(), _Off);
    #endif
}

size_t KLUPD::NoCaseString::find_last_of(const char *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_last_of(asciiToWideChar(_Ptr).c_str(), _Off);
    #else
        return m_string.find_last_of(_Ptr, _Off);
    #endif
}
size_t KLUPD::NoCaseString::find_last_of(const wchar_t *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_last_of(_Ptr, _Off);
    #else
        return m_string.find_last_of(wideCharToAscii(_Ptr).c_str(), _Off);
    #endif
}
size_t KLUPD::NoCaseString::find_last_not_of(const char *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_last_not_of(asciiToWideChar(_Ptr).c_str(), _Off);
    #else
        return m_string.find_last_not_of(_Ptr, _Off);
    #endif
}
size_t KLUPD::NoCaseString::find_last_not_of(const wchar_t *_Ptr, const size_t _Off) const
{
    #ifdef WSTRING_SUPPORTED
        return m_string.find_last_not_of(_Ptr, _Off);
    #else
        return m_string.find_last_not_of(wideCharToAscii(_Ptr).c_str(), _Off);
    #endif
}

KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const char *_Ptr)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_P0, asciiToWideChar(_Ptr).c_str());
    #else
        m_string.insert(_P0, _Ptr);
    #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const wchar_t *_Ptr)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_P0, _Ptr);
    #else
        m_string.insert(_P0, wideCharToAscii(_Ptr).c_str());
    #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const char *_Ptr, const size_t _Count)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_P0, asciiToWideChar(_Ptr).c_str(), _Count);
    #else
        m_string.insert(_P0, _Ptr, _Count);
    #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const wchar_t *_Ptr, const size_t _Count)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_P0, _Ptr, _Count);
    #else
        m_string.insert(_P0, wideCharToAscii(_Ptr).c_str(), _Count);
    #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const NoCaseString &_Str)
{
    m_string.insert(_P0, _Str.m_string);
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const NoCaseString &_Str, const size_t _Off, const size_t _Count)
{
    m_string.insert(_P0, _Str.m_string, _Off, _Count);
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const size_t _Count,  char _Ch)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_P0, _Count,  asciiToWideChar(_Ch));
    #else
        m_string.insert(_P0, _Count,  _Ch);
    #endif
    return *this;
}
KLUPD::NoCaseString &KLUPD::NoCaseString::insert(const size_t _P0, const size_t _Count,  wchar_t _Ch)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_P0, _Count,  _Ch);
    #else
        m_string.insert(_P0, _Count,  wideCharToAscii(_Ch));
    #endif
    return *this;
}
KLUPD::NoCaseString::iterator KLUPD::NoCaseString::insert(iterator _It, char _Ch)
{
    #ifdef WSTRING_SUPPORTED
        return m_string.insert(_It, asciiToWideChar(_Ch));
    #else
        return m_string.insert(_It, _Ch);
    #endif
}
KLUPD::NoCaseString::iterator KLUPD::NoCaseString::insert(iterator _It, wchar_t _Ch)
{
    #ifdef WSTRING_SUPPORTED
        return m_string.insert(_It, _Ch);
    #else
        return m_string.insert(_It, wideCharToAscii(_Ch));
    #endif
}
template<class InputIterator> void KLUPD::NoCaseString::insert(iterator _It, InputIterator _First, InputIterator _Last)
{
    return m_string.insert(_It, _First, _Last);
}
void KLUPD::NoCaseString::insert(iterator _It, const size_t _Count, char _Ch)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_It, _Count, asciiToWideChar(_Ch));
    #else
        m_string.insert(_It, _Count, _Ch);
    #endif
}
void KLUPD::NoCaseString::insert(iterator _It, const size_t _Count, wchar_t _Ch)
{
    #ifdef WSTRING_SUPPORTED
        m_string.insert(_It, _Count, _Ch);
    #else
        m_string.insert(_It, _Count, wideCharToAscii(_Ch));
    #endif
}


bool KLUPD::NoCaseString::isNumber()const
{
    return std::find_if(m_string.begin(), m_string.end(), std::not1(std::ptr_fun(isdigit)))
        == m_string.end();
}


void KLUPD::NoCaseString::trim(const NoCaseString &whiteSpace)
{
    // trim left
    const size_t leftOffset = m_string.find_first_not_of(whiteSpace.toWideChar());
    if(leftOffset != NoCaseStringImplementation::npos)
        m_string.erase(0, leftOffset);

    // trim right
    const size_t firstRightWhiteSpaceOffset = m_string.find_last_not_of(whiteSpace.toWideChar());
    if(firstRightWhiteSpaceOffset == NoCaseStringImplementation::npos)
        return;
    const size_t lastRightWhiteSpaceOffset = m_string.find_last_of(whiteSpace.toWideChar());
    if(lastRightWhiteSpaceOffset == NoCaseStringImplementation::npos)
        return;

    if(firstRightWhiteSpaceOffset < lastRightWhiteSpaceOffset)
        m_string.erase(firstRightWhiteSpaceOffset + 1);
}
