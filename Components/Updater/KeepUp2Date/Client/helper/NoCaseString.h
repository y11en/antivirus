#ifndef NOCASESTRING_H_INCLUDED_38CEBD43_31A9_4f20_9118_D8715BCFB0C8
#define NOCASESTRING_H_INCLUDED_38CEBD43_31A9_4f20_9118_D8715BCFB0C8

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "comdefs.h"

namespace KLUPD    {

#ifdef WSTRING_SUPPORTED

    struct KAVUPDCORE_API NoCaseTraits : public std::char_traits<wchar_t>
    {
        static bool eq(const wchar_t &_Left, const wchar_t &_Right)
        {
            return ::_wcsnicmp(&_Left, &_Right, 1) == 0;
        }

        static int compare(const wchar_t *_First1, const wchar_t *_First2, size_t _Count)
        {
            return ::_wcsnicmp(_First1, _First2, _Count);
        }

	    static const wchar_t *find(const wchar_t *_First, size_t _Count, const wchar_t &_Ch)
		{
            for(size_t index = 0; index < _Count; ++index)
            {
                if(eq(*(_First + index), _Ch))
                    return _First + index;
            }
            return 0;
		}
    };

    // case-insensitive string implementation
    typedef std::basic_string<wchar_t, NoCaseTraits, std::allocator<wchar_t> > NoCaseStringImplementation;

#else // WSTRING_SUPPORTED

    struct NoCaseTraits : public std::char_traits<char>
    {
        static bool eq(const char &_Left, const char &_Right)
        {
            return strncasecmp(&_Left, &_Right, 1) == 0;
        }

        static int compare(const char *_First1, const char *_First2, size_t _Count)
        {
            return strncasecmp(_First1, _First2, _Count);
        }

	    static const char *find(const char *_First, size_t _Count, const char &_Ch)
		{
            for(size_t index = 0; index < _Count; ++index)
            {
                if(eq(*(_First + index), _Ch))
                    return _First + index;
            }
            return 0;
		}
    };

    typedef std::basic_string<char, NoCaseTraits, std::allocator<char> > NoCaseStringImplementation;

#endif  // WSTRING_SUPPORTED


    // class NoCaseString
    // Purpose: the main target of this class is to provide easy string manipulation as std::string class does.
    //  But at the same time provide support of Wide-Char string on those platforms that support std::wstring.
    // It should be transparent for client of the NoCaseString class to use class inspite support std::wstring type.
    //
    // Conceipt: class implements string manipulations similarly to std::string class, on those platforms
    //  that does not support std::wstring it performs hidden convertions to 'char' type
    //
    // More documentation: see std::basic_string<> class documentation in MSDN
    class KAVUPDCORE_API NoCaseString
    {
    public:
        typedef NoCaseStringImplementation::iterator iterator;
        typedef NoCaseStringImplementation::const_iterator const_iterator;

        #ifdef WSTRING_SUPPORTED
            NoCaseString(const std::wstring &, const size_t offset = 0);
            NoCaseString &operator=(const std::wstring &);
        #else
            NoCaseString(const std::string &, const size_t offset = 0);
            NoCaseString &operator=(const std::string &);

            // implicit conversion from 'char *' is only allowed on NON WSTRING_SUPPORTED platforms
            NoCaseString(const char *);
            NoCaseString(const char *, const size_t _Count);
            NoCaseString &operator=(const char *);
            NoCaseString &operator=(const char);

        #endif  // WSTRING_SUPPORTED

        const NoCaseStringImplementation::value_type *toWideChar()const;
        std::string toAscii()const;
        NoCaseString &fromAscii(const std::string &);
        bool toLong(size_t &result)const;
        static int dvalue(const char c);
        long hexStringToLong()const;

        bool toDouble(double &result)const;

        // converts string to time
        // returns true if conversion success, false otherwise
        bool KLUPD::NoCaseString::ToTime(tm &_time) const;
        // asserts the case that XML date tag has valid format
        bool checkDateFormat()const;

        NoCaseString();
		// required by gcc! removing it could cause double memory release.
        ~NoCaseString();
        NoCaseString(const wchar_t *);
        NoCaseString(const wchar_t *, const size_t _Count);
        NoCaseString(const NoCaseString &_Right, const size_t _Roff = 0, const size_t _Count = NoCaseStringImplementation::npos);

        NoCaseString &operator=(const wchar_t *);
        NoCaseString &operator=(const NoCaseString &);
        NoCaseString &operator=(const wchar_t);
        NoCaseString &assign(const NoCaseStringImplementation::value_type *);
        NoCaseString &assign(const NoCaseStringImplementation::value_type *, size_t _Count);
        NoCaseString &assign(const NoCaseString &_Str, const size_t off,  size_t _Count);



        NoCaseString operator+(const wchar_t *)const;
        NoCaseString operator+(const NoCaseString &)const;
        NoCaseString &operator+=(const wchar_t *);
        NoCaseString &operator+=(const NoCaseString &);
        NoCaseString &operator+=(const char);
        NoCaseString &operator+=(const wchar_t);

        bool operator==(const NoCaseString &)const;
        bool operator!=(const NoCaseString &)const;
        bool operator<(const NoCaseString &)const;

        NoCaseStringImplementation::const_reference operator[](const size_t _Off)const;
        NoCaseStringImplementation::reference operator[](const size_t _Off);

        iterator begin();
        const_iterator begin()const;
        iterator end();
        const_iterator end()const;

        NoCaseString &erase(const size_t _Pos = 0, const size_t _Count = NoCaseStringImplementation::npos);

        NoCaseString &replace(iterator _First0, iterator _Last0, const char *_Ptr);
        NoCaseString &replace(iterator _First0, iterator _Last0, const wchar_t *_Ptr);
        NoCaseString &replace(iterator _First0, iterator _Last0, NoCaseString &_Str);
        NoCaseString &replace(const size_t _Pos1, const size_t _Num1, const char *_Ptr);
        NoCaseString &replace(const size_t _Pos1, const size_t _Num1, const wchar_t *_Ptr);

        bool empty()const;
        size_t size()const;
        void resize(const size_t _Count, const wchar_t _Ch = char());
        NoCaseString substr(const size_t _Off = 0, const size_t _Count = NoCaseStringImplementation::npos)const;

        size_t find(const NoCaseString &, const size_t _Off = 0)const;
        size_t find(const char *, const size_t _Off, const size_t _Count)const;
        size_t find(const wchar_t *, const size_t _Off, const size_t _Count)const;

        size_t rfind(const char, const size_t _Off = NoCaseStringImplementation::npos)const;
        size_t rfind(const wchar_t, const size_t _Off = NoCaseStringImplementation::npos)const;
        size_t rfind(const char *, const size_t _Off = NoCaseStringImplementation::npos)const;
        size_t rfind(const wchar_t *, const size_t _Off = NoCaseStringImplementation::npos)const;

        size_t find_first_of(const char *, const size_t _Off = 0) const;
        size_t find_first_of(const wchar_t *, const size_t _Off = 0) const;
        size_t find_first_not_of(const char *, const size_t _Off = 0) const;
        size_t find_first_not_of(const wchar_t *, const size_t _Off = 0) const;
    	size_t find_last_of(const char *, const size_t _Off = NoCaseStringImplementation::npos) const;
    	size_t find_last_of(const wchar_t *, const size_t _Off = NoCaseStringImplementation::npos) const;
    	size_t find_last_not_of(const char *, const size_t _Off = NoCaseStringImplementation::npos) const;
    	size_t find_last_not_of(const wchar_t *, const size_t _Off = NoCaseStringImplementation::npos) const;

        NoCaseString &insert(const size_t _P0, const char *_Ptr);
        NoCaseString &insert(const size_t _P0, const wchar_t *_Ptr);
        NoCaseString &insert(const size_t _P0, const char *_Ptr, const size_t _Count);
        NoCaseString &insert(const size_t _P0, const wchar_t *_Ptr, const size_t _Count);
        NoCaseString &insert(const size_t _P0, const NoCaseString &_Str);
        NoCaseString &insert(const size_t _P0, const NoCaseString &_Str, const size_t _Off, const size_t _Count);
        NoCaseString &insert(const size_t _P0, const size_t _Count,  char _Ch);
        NoCaseString &insert(const size_t _P0, const size_t _Count,  wchar_t _Ch);
        iterator insert(iterator _It, char _Ch = char());
        iterator insert(iterator _It, wchar_t _Ch = wchar_t());
        template<class InputIterator> void insert(iterator _It, InputIterator _First, InputIterator _Last);
        void insert(iterator _It, const size_t _Count, char _Ch);
        void insert(iterator _It, const size_t _Count, wchar_t _Ch);

        bool isNumber()const;
        // trims whitespace from both sides of a string.
        void trim(const NoCaseString &whiteSpace = L" \t\n\r");

    private:
        NoCaseStringImplementation m_string;
    };


}   // namespace KLUPD

#endif  // NOCASESTRING_H_INCLUDED_38CEBD43_31A9_4f20_9118_D8715BCFB0C8
