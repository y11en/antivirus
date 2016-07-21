#include "comdefs.h"

#include "local_file.h"
#include "updaterStaff.h"
#include "path.h"

#ifndef _access
    #define _access access
#endif

#ifndef WIN32
    #define _rmdir rmdir
    #define _unlink unlink
    #define _chmod chmod

    #define _S_IREAD S_IREAD
    #define _S_IWRITE S_IWRITE
#endif

#define FLAG_READ_ACCESS_CHECK 4

#ifndef F_OK
    #define F_OK	0
#endif

#ifndef R_OK
    #define R_OK	2
#endif

KLUPD::LocalFile::LocalFile(const Path &name, Log *log)
 : m_name(name),
   pLog(log)
{
}

bool KLUPD::LocalFile::exists()const
{
    bool exist = false;
    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            exist = _waccess(m_name.toWideChar(), FLAG_READ_ACCESS_CHECK) == 0;
        #endif
    }
    else
        exist = _access(m_name.toAscii().c_str(), FLAG_READ_ACCESS_CHECK) == 0;

    if(exist)
        return true;

    const int lastError = errno;
    // the 'path not found' result is expected, trace only possible problems with access permissions, etc
    if(lastError != ENOENT)
    {
        TRACE_MESSAGE3("Failed to check if file exists '%S', last error '%S'",
            m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
    }
    return false;
}

KLUPD::CoreError KLUPD::LocalFile::read(std::vector<unsigned char> &buffer)const
{
    // Note: STL is not used for read file, because STL stream reads
     //  byte by byte thus slowing performance, and code is simple

    AutoStream readStream(pLog);
    const CoreError openResult = readStream.open(m_name, L"rb");
    if(!isSuccess(openResult))
    {
        TRACE_MESSAGE3("Read from file failed, because unable to open '%S', result '%S'",
            m_name.toWideChar(), toString(openResult).toWideChar());
        return openResult;
    }

    // getting file size
    if(fseek(readStream.stream(), 0, SEEK_END) != 0)
    {
        const int lastError = errno;
        TRACE_MESSAGE3("Failed to get file size, unable seek to the end of the file '%S', last error '%S'",
            m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }


    // resize buffer to fit file content
    buffer.resize(ftell(readStream.stream()));
    // file is empty
    if(buffer.empty())
        return CORE_NO_ERROR;


    // seek back to the file begin
    if(fseek(readStream.stream(), 0, SEEK_SET) != 0)
    {
        const int lastError = errno;
        TRACE_MESSAGE3("Failed to read file content, unable seek to the start of file '%S', last error '%S'",
            m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }

    if(fread(reinterpret_cast<void *>(&buffer[0]), buffer.size(), 1, readStream.stream())  != 1)
    {
        const int lastError = errno;
        TRACE_MESSAGE3("Failed to read file content '%S', last error '%S'",
            m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::LocalFile::write(const std::vector<unsigned char> &buffer)const
{
    // Note: STL is not used for read file, because STL stream reads
     //  byte by byte thus slowing performance, and code is simple

    AutoStream writeStream(pLog);
    const CoreError openResult = writeStream.open(m_name, L"wb");
    if(!isSuccess(openResult))
    {
        TRACE_MESSAGE3("Write to file failed, because unable to open file '%S', result '%S'",
            m_name.toWideChar(), toString(openResult).toWideChar());
        return openResult;
    }

    if(!buffer.empty())
    {
        if(fwrite(reinterpret_cast<const void *>(&buffer[0]), 1, buffer.size(), writeStream.stream()) != buffer.size())
        {
            const int lastError = errno;
            TRACE_MESSAGE3("Failed to write file content '%S', last error '%S'",
                m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
            return lastErrorToUpdaterFileErrorCode(lastError);
        }
    }
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::LocalFile::rename(const Path &destination)const
{
    const CoreError unlinkResult = LocalFile(destination).unlink();
    if(!isSuccess(unlinkResult))
    {
        TRACE_MESSAGE4("Failed to moved file '%S' to '%S', because failed to unlink destination file, result '%S'",
            m_name.toWideChar(), destination.toWideChar(), toString(unlinkResult).toWideChar());
        return unlinkResult;
    }

    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            if(::_wrename(m_name.toWideChar(), destination.toWideChar()) == 0)
            {
                TRACE_MESSAGE3("Successfully moved file '%S' to '%S'",
                    m_name.toWideChar(), destination.toWideChar());
                return CORE_NO_ERROR;
            }
        #endif
    }
    else if(::rename(m_name.toAscii().c_str(), destination.toAscii().c_str()) == 0)
    {
        TRACE_MESSAGE3("Successfully moved file '%S' to '%S'",
            m_name.toWideChar(), destination.toWideChar());
        return CORE_NO_ERROR;
    }

    const int lastError = errno;
    TRACE_MESSAGE4("Failed to move file: '%S' to '%S', last error '%S'",
        m_name.toWideChar(), destination.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
    return lastErrorToUpdaterFileErrorCode(lastError);
}

size_t KLUPD::LocalFile::size()const
{
    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            std::wifstream file;
            file.open(m_name.toWideChar(), std::ios::binary);
            if(!file.is_open())
            {
                const int lastError = errno;
                TRACE_MESSAGE3("Failed get file size '%S', last error '%S'",
                    m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
                return 0;
            }
            // seek to the end of file, and return end file position
            file.seekg(0, std::ios_base::end);
            return file.tellg();
        #endif
    }

    std::ifstream file;
    file.open(m_name.toAscii().c_str(), std::ios::binary);
    if(!file.is_open())
    {
        const int lastError = errno;
        TRACE_MESSAGE3("Failed get file size '%S', last error '%S'",
            m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return 0;
    }
    // seek to the end of file, and return end file position
    file.seekg(0, std::ios_base::end);
    return file.tellg();

}

KLUPD::CoreError KLUPD::LocalFile::unlink()const
{
    if(!exists())
        return CORE_NO_ERROR;

    // removing read-only attribute
    if(isReadOnly(m_name))
    {
        TRACE_MESSAGE2("File to unlink '%S' is read-only, changing file attributes", m_name.toWideChar());

        if(unicodeFileOperationsSupported())
        {
            // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
            #if defined(WIN32) && defined(WSTRING_SUPPORTED)
                _wchmod(m_name.toWideChar(), _S_IREAD | _S_IWRITE);
            #endif
        }
        else
            _chmod(m_name.toAscii().c_str(), _S_IREAD | _S_IWRITE);
    }

    // removing 'name' as folder
    if(m_name.isFolder())
    {
        if(unicodeFileOperationsSupported())
        {
            // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
            #if defined(WIN32) && defined(WSTRING_SUPPORTED)
                if(::_wrmdir(m_name.toWideChar()) == 0)
                {
                    TRACE_MESSAGE2("Folder unlinked successfully '%S'", m_name.toWideChar());
                    return CORE_NO_ERROR;
                }
            #endif
        }
        else if(::_rmdir(m_name.toAscii().c_str()) == 0)
        {
            TRACE_MESSAGE2("Folder unlinked successfully '%S'", m_name.toWideChar());
            return CORE_NO_ERROR;
        }

        const int lastError = errno;
        TRACE_MESSAGE3("Failed to remove folder '%S', last error '%S'",
            m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }

    // removing 'name' as file
    bool unlinked = false;
    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            unlinked = ::_wunlink(m_name.toWideChar()) == 0;
        #endif
    }
    else
        unlinked = ::_unlink(m_name.toAscii().c_str()) == 0;

    const int lastError = errno;

    // re-check, because unlink may return success, but locked file rests
    if(!exists())
    {
        TRACE_MESSAGE2("File unlinked successfully '%S'", m_name.toWideChar());
        return CORE_NO_ERROR;
    }

    TRACE_MESSAGE4("Failed to unlink file '%S', last error '%S' (unlinked flag %d)",
        m_name.toWideChar(), errnoToString(lastError, posixStyle).toWideChar(), unlinked);
    return lastErrorToUpdaterFileErrorCode(lastError);
}

KLUPD::CoreError KLUPD::LocalFile::copyFileImplementation(const Path &sourceFileName, const Path &destinationFileName, Log *pLog)
{
    std::vector<unsigned char> buffer;
    const CoreError readFileResult = LocalFile(sourceFileName, pLog).read(buffer);
    if(!isSuccess(readFileResult))
    {
        TRACE_MESSAGE3("Failed to copy file, because can not read source file '%S', result '%S'",
            sourceFileName.toWideChar(), toString(readFileResult).toWideChar());
        return readFileResult;
    }

    const CoreError writeFileResult = LocalFile(destinationFileName, pLog).write(buffer);
    if(!isSuccess(writeFileResult))
    {
        TRACE_MESSAGE3("Failed to copy file, because can not write to destination file '%S', result '%S'",
            sourceFileName.toWideChar(), toString(writeFileResult).toWideChar());
        return writeFileResult;
    }
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::LocalFile::copy(const Path &dest)
{
    // check file existence
    if(!exists())
    {
        TRACE_MESSAGE3("Failed to copy file, because source file does not exist '%S' (destination '%S')",
            m_name.toWideChar(), dest.toWideChar());
        return CORE_NoSuchFileOrDirectory;
    }

    // extract file name
    const Path fileName = m_name.getFileNameFromPath();
    if(fileName.empty())
    {
        TRACE_MESSAGE3("Failed to copy file, because target is not file name, but folder. Target '%S', destination '%S'",
            m_name.toWideChar(), dest.toWideChar());
        return CORE_GenericFileOperationFailure;
    }

    Path destination(dest);
    // append file name in case folder specified as destination
    if(dest.isFolder())
    {
        destination.correctPathDelimiters();
        destination = destination.emptyIfSingleSlash() + fileName;
    }


    const CoreError copyFileResult = copyFileImplementation(m_name, destination, pLog);
    if(!isSuccess(copyFileResult))
    {
        TRACE_MESSAGE4("Failed to copy file '%S' to '%S', result '%S'",
            m_name.toWideChar(), destination.toWideChar(), toString(copyFileResult).toWideChar());
        return copyFileResult;
    }

    TRACE_MESSAGE3("File copied: '%S' => '%S'", m_name.toWideChar(), destination.toWideChar());
    return CORE_NO_ERROR;
}

bool KLUPD::LocalFile::clearFolder(const Path &path, Log *pLog)
{
    // check folder existence
    if(!LocalFile(path).exists())
        return true;

    std::vector<Path> content;
    if(!enumerateFilesInFolder(path, content, pLog, true))
    {
        TRACE_MESSAGE2("Cleaning folder: error reading content of folder '%S'", path.toWideChar());
        return false;
    }
    // no more files in folder, so removing folder itself
    if(content.empty())
        return isSuccess(LocalFile(path, pLog).unlink());

    for(unsigned long index = 0; index < content.size(); ++index)
    {
        Path currentFileSystemItem = content[index];

        // recursively removing content of nested folders
        if(currentFileSystemItem.isFolder())
        {
            // appending trailing '/' path delimiter
            currentFileSystemItem.correctPathDelimiters();
            
            // try one more time with original folder
            //   to avoid recursion return
            return clearFolder(currentFileSystemItem, pLog)
                && clearFolder(path, pLog);
        }

        if(!isSuccess(LocalFile(currentFileSystemItem, pLog).unlink()))
        {
            TRACE_MESSAGE3("Error removing path '%S', element '%S'",
                path.toWideChar(), currentFileSystemItem.toWideChar());
            return false;
        }
    }

    // unlink folder itself after all files in folder are removed
    // Note: there were no subfolders, otherwise function would return before
    return isSuccess(LocalFile(path, pLog).unlink());
}

bool KLUPD::mkdirWrapper(const Path &path)
{
    #ifdef WIN32
        if(unicodeFileOperationsSupported())
        {
            // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
            #if defined(WIN32) && defined(WSTRING_SUPPORTED)
                return CreateDirectoryW(path.toWideChar(), 0) == TRUE;
            #endif
        }
        return CreateDirectoryA(path.toAscii().c_str(), 0) == TRUE;
    #else   // UNIX
        return mkdir(path.toAscii().c_str(), 0777) == 0;
    #endif
}
    

bool KLUPD::createFolder(const Path &path, Log *pLog)
{
    if(LocalFile(path).exists())
        return true;
    
    if(mkdirWrapper(path))
        return true;

    // recursively create directory
    size_t lastDelimeterOffset = path.m_value.rfind(L'/');
    if(lastDelimeterOffset == NoCaseStringImplementation::npos)
    {
        lastDelimeterOffset = path.m_value.rfind(L'\\');
        if(lastDelimeterOffset == NoCaseStringImplementation::npos)
        {
            TRACE_MESSAGE2("Failed to create path recursively '%S'", path.toWideChar());
            return false;
        }
    }

    Path _s(path);
    _s.m_value.erase(lastDelimeterOffset, NoCaseStringImplementation::npos);
    if(!createFolder(_s, pLog))
        return false;

    if(!LocalFile(path).exists() && !mkdirWrapper(path))
    {
        TRACE_MESSAGE2("Failed to create folder '%S'", path.toWideChar());
        return false;
    }

    return true;
}

bool KLUPD::isReadOnly(const Path &fileName)
{
    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            if(_waccess(fileName.toWideChar(), F_OK) == -1)
                return false;   // file not found

            if(_waccess(fileName.toWideChar(), R_OK) != -1)
                return false;   // writable
        #endif
    }
    else
    {
        if(_access(fileName.toAscii().c_str(), F_OK) == -1)
            return false;   // file not found

        if(_access(fileName.toAscii().c_str(), R_OK) != -1)
            return false;   // writable
    }

    return true;        // not writable
}

bool KLUPD::enumerateFilesInFolder(const Path &path, std::vector<Path> &content, Log *pLog, const bool enumerateSubfolders)
{
    #ifdef WIN32

        #if defined(WSTRING_SUPPORTED)
            WIN32_FIND_DATAW findFileDataW;
            memset(&findFileDataW, 0, sizeof(findFileDataW));
            WIN32_FIND_DATAA findFileDataA;
            memset(&findFileDataA, 0, sizeof(findFileDataA));

            HANDLE findHandle = unicodeFileOperationsSupported()
                ? FindFirstFileW((path + L"*").toWideChar(), &findFileDataW)
                : FindFirstFileA((path + L"*").toAscii().c_str(), &findFileDataA);

        #else
            WIN32_FIND_DATAA findFileData;
            HANDLE findHandle = FindFirstFileA((path + L"*").toAscii().c_str(), &findFileData);
        #endif

        if(findHandle == INVALID_HANDLE_VALUE) 
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE3("Failed to enumerate folder '%S', last error '%S'",
                path.toWideChar(), errnoToString(lastError, windowsStyle).toWideChar());
            return false;
        }

        #if defined(WSTRING_SUPPORTED)
            const Path cFileName = unicodeFileOperationsSupported()
                ? findFileDataW.cFileName : asciiToWideChar(findFileDataA.cFileName);
        #else
            const Path cFileName = asciiToWideChar(findFileData.cFileName);
        #endif

        if((cFileName != L".") && (cFileName != L".."))
            content.push_back(path + cFileName);

        bool enumerateSubfolderResult = true;
        while(
                    #if defined(WSTRING_SUPPORTED)
                        unicodeFileOperationsSupported()
                        ? FindNextFileW(findHandle, &findFileDataW)
                        : FindNextFileA(findHandle, &findFileDataA)
                    #else
                        FindNextFileA(findHandle, &findFileData)
                    #endif
            )
        {

            #if defined(WSTRING_SUPPORTED)
                const Path cFileName = unicodeFileOperationsSupported()
                    ? findFileDataW.cFileName : asciiToWideChar(findFileDataA.cFileName);
            #else
                const Path cFileName = asciiToWideChar(findFileData.cFileName);
            #endif

            if((cFileName == L".") || (cFileName == L".."))
                continue;

            #if defined(WSTRING_SUPPORTED)
                const BOOL isDirectory = FILE_ATTRIBUTE_DIRECTORY
                    & (unicodeFileOperationsSupported() ? findFileDataW.dwFileAttributes :  findFileDataA.dwFileAttributes);
            #else
                const BOOL isDirectory = findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            #endif

            if(isDirectory)
            {
                if(enumerateSubfolders)
                    content.push_back(path + cFileName);
                enumerateSubfolderResult = enumerateFilesInFolder(path + cFileName + L"/",
                    content, pLog, enumerateSubfolders);
                if(!enumerateSubfolderResult)
                    break;
            }
            else
                content.push_back(path + cFileName);
        }

        const int lastError = GetLastError();
        const bool noMoreFiles = (lastError == ERROR_NO_MORE_FILES);
        FindClose(findHandle);

        return noMoreFiles && enumerateSubfolderResult;

    #else // WIN32

        DIR *dir = opendir(path.toAscii().c_str());
        if(!dir)
        {
            const int lastError = GetLastError();
            TRACE_MESSAGE3("Failed to enumerate folder '%S', last error '%S'",
                path.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
            return lastErrorToUpdaterFileErrorCode(lastError);
        }
    
        bool enumerateSubfolderResult = true;
        while(true)
        {
            struct dirent *dirEntry = readdir(dir);
            if(!dirEntry)
                break;
        
            if(std::string(dirEntry->d_name) == "." )
                continue;
            if(std::string(dirEntry->d_name) == "..")
                continue;
        
            struct stat _st;
            if(stat((path.toAscii() + dirEntry->d_name).c_str(), &_st))
            {
                TRACE_MESSAGE3("Failed to stat '%S%s'", path.toWideChar(), dirEntry->d_name);
                continue;
            }
            if(S_ISDIR(_st.st_mode))
            {
                if(enumerateSubfolders)
                    content.push_back(path + asciiToWideChar(dirEntry->d_name).c_str());
                enumerateSubfolderResult = enumerateFilesInFolder( asciiToWideChar(path.toAscii() + dirEntry->d_name + "/").c_str() ,
                    content, pLog, enumerateSubfolders);
                if(!enumerateSubfolderResult)
                    break;
            }
            else
                content.push_back((path + asciiToWideChar(dirEntry->d_name).c_str()));
        }
        closedir(dir);
        return enumerateSubfolderResult;
    #endif
}


KLUPD::AutoStream::AutoStream(Log *log)
 : m_stream(0),
   pLog(log)
{
}
KLUPD::AutoStream::~AutoStream()
{
    close();
}

KLUPD::AutoStream &KLUPD::AutoStream::operator=(FILE *stream)
{
    if(stream == m_stream)
        return *this;

    close();
    m_stream = stream;
    return *this;
}

FILE *KLUPD::AutoStream::stream()const
{
    return m_stream;
}

KLUPD::CoreError KLUPD::AutoStream::open(const Path &fileName, const NoCaseString &mode)
{
    close();

    const bool writeMode
        = (mode.find_first_of(L"aw") != NoCaseStringImplementation::npos)
        || (mode.find(L"a+") != NoCaseStringImplementation::npos);

    // removing read-only attribute
    if(writeMode && isReadOnly(fileName))
    {
        TRACE_MESSAGE2("File stream '%S' is read-only, changing file attributes", fileName.toWideChar());
        if(unicodeFileOperationsSupported())
        {
            // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
            #if defined(WIN32) && defined(WSTRING_SUPPORTED)
                _wchmod(fileName.toWideChar(), _S_IREAD | _S_IWRITE);
            #endif
        }
        else
            _chmod(fileName.toAscii().c_str(), _S_IREAD | _S_IWRITE);
    }

    if(unicodeFileOperationsSupported())
    {
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
            m_stream = _wfopen(fileName.toWideChar(), mode.toWideChar());
        #endif
    }
    else
        m_stream = fopen(fileName.toAscii().c_str(), mode.toAscii().c_str());

    if(!m_stream)
    {
        const int lastError = errno;
        TRACE_MESSAGE4("Failed to open file stream '%S' in '%S' mode, result '%S'",
            fileName.toWideChar(), mode.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }

    return CORE_NO_ERROR;
}

void KLUPD::AutoStream::close()
{
    if(!m_stream)
        return;

    fclose(m_stream);
    m_stream = 0;
}



KLUPD::FileStream::FileStream(Log *log)
 : pLog(log)
{
}

bool KLUPD::FileStream::open(const Path &fileName, std::ios_base::openmode mode)
{
    m_file.open(
        // both _WIN32 and WSTRING_SUPPORTED macros are always defined in case unicodeFileOperationsSupported() returns true
        #if defined(WIN32) && defined(WSTRING_SUPPORTED)
	        fileName.toWideChar(),
        #else
	        fileName.toAscii().c_str(),
        #endif	
            mode);

    const int lastError = errno;

    if(m_file.is_open())
        return true;

    TRACE_MESSAGE3("Unable to open file '%S', last error '%S'",
        fileName.toWideChar(), KLUPD::errnoToString(lastError, KLUPD::posixStyle).toWideChar());
    return false;
}

void KLUPD::FileStream::getLine(NoCaseString &output)
{
    const size_t bufferSize = 2 * 1024;

    #ifdef WSTRING_SUPPORTED
        wchar_t buffer[bufferSize];
    #else
        char buffer[bufferSize];
    #endif
    m_file.getline(buffer, bufferSize);
    output = buffer;
}

void KLUPD::FileStream::writeLine(const NoCaseString &bufferIn)
{
    NoCaseString buffer = bufferIn;

    if(buffer[buffer.size()] != L'\n' && buffer[buffer.size()] != L'\r')
        buffer += L"\n";

    m_file.write(buffer.toWideChar(), buffer.size());
}

bool KLUPD::FileStream::done()
{
    return m_file.eof() || m_file.bad() || m_file.fail();
}

