#ifndef PATH_H_INCLUDED_BD764D7E_B774_4bea_8EAD_00B82D2B8F2D
#define PATH_H_INCLUDED_BD764D7E_B774_4bea_8EAD_00B82D2B8F2D

#include "comdefs.h"

namespace KLUPD {

// helper which encapsulate path operations. The main objective of the class is to provide easy method
// in case we need to switch Path implementaiton to other representation (wide-char, UTF8, 0-terminated strings, or other)
class KAVUPDCORE_API Path
{
public:
    Path();
    Path(const Path &);
    Path(const NoCaseString &);
    Path(const wchar_t *);
    Path &operator=(const Path &);
    Path &operator=(const NoCaseString &);
    Path &operator=(const wchar_t *);
    #ifndef WSTRING_SUPPORTED
        // implicit conversion from 'char *' is only allowed on NON WSTRING_SUPPORTED platforms
        Path(const char *);
        Path &operator=(const char *);
    #endif

    Path operator+(const wchar_t *)const;
    Path operator+(const Path &)const;
    Path &operator+=(const wchar_t *);
    Path &operator+=(const Path &);

    bool operator==(const Path &)const;
    bool operator!=(const Path &)const;
    bool operator<(const Path &)const;

    NoCaseStringImplementation::const_reference operator[](const size_t _Off)const;
    NoCaseStringImplementation::reference operator[](const size_t _Off);

    const NoCaseStringImplementation::value_type *toWideChar()const;
    std::string toAscii()const;
    Path &fromAscii(const std::string &);

    void erase();
    bool empty()const;
    size_t size()const;
    size_t find(const Path &)const;


    bool isFolder()const;
    bool isPathAbsolute()const;
    Path getFileNameFromPath()const;
    Path getFolderFromPath()const;
    void parseFileName(Path &fileNameWithoutExtension, Path &extension)const;

    // transform path to Windows format.
    //  Restrinctions: this function can not be used with network paths
    void convertPathToWindowsPlatformFormat();

    // change path delimiter OS for appropriate for Windows 9.x
    void fixPathForWin9x();

    void removeSingleLeadingDelimeter();
    // removes leading and trailing path delimiters
    void removeLeadingAndTrailingIndent();

    // fixes path to have correct and non-duplicate delimiters
    void correctPathDelimiters();

    // returns empty string in case string contains only single symbol which is path delimiter
    Path emptyIfSingleSlash()const;


    // check whether path is absolute Uri
    bool isAbsoluteUri()const;

    // current implementation is based on wide-char no-case string
     NoCaseString m_value;
};

}   // namespace KLUPD

#endif   // PATH_H_INCLUDED_BD764D7E_B774_4bea_8EAD_00B82D2B8F2D
