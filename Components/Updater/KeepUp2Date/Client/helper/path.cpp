#include "path.h"

#include "updaterStaff.h"

KLUPD::Path::Path()
{
}
KLUPD::Path::Path(const Path &input)
 : m_value(input.m_value)
{
}
KLUPD::Path::Path(const NoCaseString &input)
 : m_value(input)
{
}
KLUPD::Path::Path(const wchar_t *input)
 : m_value(input)
{
}

KLUPD::Path &KLUPD::Path::operator=(const Path &input)
{
    m_value = input.m_value;
    return *this;
}
KLUPD::Path &KLUPD::Path::operator=(const NoCaseString &input)
{
    m_value = input;
    return *this;
}
KLUPD::Path &KLUPD::Path::operator=(const wchar_t *input)
{
    m_value = input;
    return *this;
}
#ifndef WSTRING_SUPPORTED
KLUPD::Path &KLUPD::Path::operator=(const char *input)
{
    fromAscii(input);
    return *this;
}
KLUPD::Path::Path(const char *input)
{
    fromAscii(input);
}
#endif


KLUPD::Path KLUPD::Path::operator+(const wchar_t *right)const
{
    return m_value + right;
}
KLUPD::Path KLUPD::Path::operator+(const Path &right)const
{
    return m_value + right.m_value;
}
KLUPD::Path &KLUPD::Path::operator+=(const wchar_t *right)
{
    m_value += right;
    return *this;
}
KLUPD::Path &KLUPD::Path::operator+=(const Path &right)
{
    m_value += right.m_value;
    return *this;
}


bool KLUPD::Path::operator==(const Path &right)const
{
    return m_value == right.m_value;
}
bool KLUPD::Path::operator!=(const Path &right)const
{
    return !operator==(right);
}
bool KLUPD::Path::operator<(const Path &right)const
{
    return m_value < right.m_value;
}

KLUPD::NoCaseStringImplementation::const_reference KLUPD::Path::operator[](const size_t _Off)const
{
    return m_value[_Off];
}
KLUPD::NoCaseStringImplementation::reference KLUPD::Path::operator[](const size_t _Off)
{
    return m_value[_Off];
}

const KLUPD::NoCaseStringImplementation::value_type *KLUPD::Path::toWideChar()const
{
    return m_value.toWideChar();
}

std::string KLUPD::Path::toAscii()const
{
    return m_value.toAscii();
}
KLUPD::Path &KLUPD::Path::fromAscii(const std::string &input)
{
    m_value.fromAscii(input);
    return *this;
}


void KLUPD::Path::erase()
{
    m_value.erase();
}
bool KLUPD::Path::empty()const
{
    return m_value.empty();
}
size_t KLUPD::Path::size()const
{
    return m_value.size();
}
size_t KLUPD::Path::find(const Path &pattern)const
{
    return m_value.find(pattern.m_value);
}



bool KLUPD::Path::isFolder()const
{
    if(m_value.empty())
        return true;

    // it is folder in case trailing delimiter is present
    if(m_value[m_value.size() - 1] == '/' || m_value[m_value.size() - 1] == '\\')
        return true;

    #ifdef WSTRING_SUPPORTED
        if(unicodeFileOperationsSupported())
        {
            #if (defined (_WIN32)) && (_MSC_VER > VISUAL_STUDIO_6_COMPILER_VERSION)
                #ifdef _USE_32BIT_TIME_T
                    struct _stat32 statResult;
                #else
                    struct _stat64i32 statResult;
                #endif

                return (_wstat(toWideChar(), &statResult) == 0)
                    && (S_IFDIR & statResult.st_mode);
            #endif
        }
    #endif  // WSTRING_SUPPORTED

    struct stat statResult;
    return (stat(toAscii().c_str(), &statResult) == 0)
        && (S_IFDIR & statResult.st_mode);
}
bool KLUPD::Path::isPathAbsolute()const
{
    if(m_value.empty())
        return false;
    // UNIX paths starts with '\' symbol
    if((m_value[0] == L'\\') || (m_value[0] == L'/'))
        return true;

    // check against pattern "c:\", "d:\"
    if(m_value.size() < 2)
        return false;
    return m_value[1] == L':';
}
KLUPD::Path KLUPD::Path::getFileNameFromPath()const
{
    #ifdef WIN32
            if(unicodeFileOperationsSupported())
            {
                #ifdef WSTRING_SUPPORTED  // always defined in case unicodeFileOperationsSupported() return true
                    wchar_t drive[_MAX_DRIVE];
                    memset(drive, 0, _MAX_DRIVE);
                    wchar_t folder[_MAX_DIR];
                    memset(folder, 0, _MAX_DIR);
                    wchar_t fileName[_MAX_FNAME];
                    memset(fileName, 0, _MAX_FNAME);
                    wchar_t extention[_MAX_EXT];
                    memset(extention, 0, _MAX_EXT);
                
                    _wsplitpath(toWideChar(), drive, folder, fileName, extention);
                    return Path(fileName) + extention;
            #endif  // WSTRING_SUPPORTED
        }

        char drive[_MAX_DRIVE];
        memset(drive, 0, _MAX_DRIVE);
        char folder[_MAX_DIR];
        memset(folder, 0, _MAX_DIR);
        char fileName[_MAX_FNAME];
        memset(fileName, 0, _MAX_FNAME);
        char extention[_MAX_EXT];
        memset(extention, 0, _MAX_EXT);
    
        _splitpath(toAscii().c_str(), drive, folder, fileName, extention);
        return asciiToWideChar(fileName) + asciiToWideChar(extention);

    #else   // #ifdef WIN32
        return asciiToWideChar(basename(const_cast<char *>(toAscii().c_str()))).c_str();
    #endif
}

KLUPD::Path KLUPD::Path::getFolderFromPath()const
{
    Path result;
    result.m_value.assign(toWideChar(), m_value.size() - getFileNameFromPath().m_value.size());
    return result;
}

void KLUPD::Path::parseFileName(Path &fileNameWithoutExtension, Path &extension)const
{
    const size_t extensionOffset = m_value.find_last_of(L".");
    if(extensionOffset == NoCaseStringImplementation::npos)
    {
        fileNameWithoutExtension = m_value;
        extension.m_value.erase();
        return;
    }

    fileNameWithoutExtension.m_value.assign(toWideChar(), extensionOffset);
    extension.m_value.assign(toWideChar() + extensionOffset + 1);
}

void KLUPD::Path::convertPathToWindowsPlatformFormat()
{
    // on all Win9.x the path delimiter is '\' symbol
    for(NoCaseStringImplementation::iterator iter = m_value.begin(); iter != m_value.end(); ++iter)
    {
        if(*iter == L'/')
            *iter = L'\\';
    }
}

void KLUPD::Path::fixPathForWin9x()
{
// for UNIX the '/' symbol serves as path delimeter
#ifdef WIN32
    // on all non Win9.x the path delimiter symbol is '/'
    if(GetVersion() < VER_SERVER_NT)
        return;

    // on all Win9.x the path delimiter is '\' symbol
    convertPathToWindowsPlatformFormat();

#else
    return;
#endif
}

void KLUPD::Path::removeSingleLeadingDelimeter()
{
    if(!m_value.empty() && (m_value[0] == L'/'))
        m_value.erase(0, 1);
}


void KLUPD::Path::removeLeadingAndTrailingIndent()
{
    if(m_value.empty())
        return;

    // tabulation, space are to be removed
    const Path delimeterPatterns = L"\t ";


    // remove leading delimeters
    const size_t leadingOffset = m_value.find_first_not_of(delimeterPatterns.toWideChar());
    if(leadingOffset && (leadingOffset != NoCaseStringImplementation::npos))
        m_value.erase(0, leadingOffset);

    // remove trailing delimeters
    size_t trailingOffset = m_value.find_last_not_of(delimeterPatterns.toWideChar());
    if((trailingOffset != NoCaseStringImplementation::npos)
        && ((trailingOffset + 1) < m_value.size()))
    {
        m_value.erase(trailingOffset + 1);
    }
}

void KLUPD::Path::correctPathDelimiters()
{
    removeLeadingAndTrailingIndent();

    const wchar_t backSlash = L'\\';
    const wchar_t *pathDelimiter = L"/";
    const wchar_t networkProtocolDelimiter = L':';
    const Path doubleSlash(L"//");

    //////////////////////////////////////////////////////////////////////////
    /// replace all occurrences of '\' with '/' except \\ in the very beginning
    unsigned from = 0;
    if(m_value.size() > 1)
    {
        if((m_value[0] == backSlash) && (m_value[1] == backSlash))
            from = 2;
    }
    std::replace_if(m_value.begin() + from, m_value.end(), std::bind2nd(std::equal_to<char>(), backSlash), pathDelimiter[0]);


    //////////////////////////////////////////////////////////////////////////
    /// fix double "//" symbol, except for network protocol 'http://' and 'ftp://'
    for(size_t currentPosition = 0;;)
    {
        currentPosition = m_value.find(doubleSlash.m_value, currentPosition);
        if(currentPosition == NoCaseStringImplementation::npos)
            break;

        // skip network protocol delimiter
        if((currentPosition > 0)
            && (m_value[currentPosition - 1] == networkProtocolDelimiter))
        {
            ++currentPosition;
            continue;
        }

        m_value.replace(currentPosition, doubleSlash.m_value.size(), pathDelimiter);
    }

    //////////////////////////////////////////////////////////////////////////
    /// fix ".." in path
    while(true)
    {
        size_t doubleDotPosition = m_value.find(L"/..");
        if((doubleDotPosition == NoCaseStringImplementation::npos) || !doubleDotPosition)
            break;

        size_t previousDelimiterPosition = m_value.rfind(pathDelimiter[0], doubleDotPosition - 1);
        if(previousDelimiterPosition == NoCaseStringImplementation::npos || (doubleDotPosition < previousDelimiterPosition))
        {
            // if path starts with "/.."
            if(doubleDotPosition == 0)
                break;

            previousDelimiterPosition = 0;
            // erase extra "/" symbol
            doubleDotPosition += 1;
        }

        m_value.erase(previousDelimiterPosition, doubleDotPosition + 3 - previousDelimiterPosition);
    }

    //////////////////////////////////////////////////////////////////////////
    /// check that path last char is valid
    if(m_value.empty() || (m_value[m_value.size() - 1] != pathDelimiter[0]))
        m_value += pathDelimiter;
}

KLUPD::Path KLUPD::Path::emptyIfSingleSlash()const
{
    return ((m_value != L"/") && (m_value != L"\\"))
        ? m_value : L"";
}


bool KLUPD::Path::isAbsoluteUri()const
{
    const NoCaseString absoluteUriPrefix = L"http://";
    if(m_value.size() < absoluteUriPrefix.size())
        return false;


    const NoCaseString uriToCompare(m_value.toWideChar(), absoluteUriPrefix.size());
    return absoluteUriPrefix == uriToCompare;
}

