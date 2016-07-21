#ifndef LOCAL_FILE_H_INCLUDED_4F5D275E_3FFA_4f55_8AD8_77CEA37CF802
#define LOCAL_FILE_H_INCLUDED_4F5D275E_3FFA_4f55_8AD8_77CEA37CF802

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000


#include "../include/stringParser.h"
#include "path.h"

namespace KLUPD {

class Log;

// Local files manipulation helper class
class KAVUPDCORE_API LocalFile
{
public:
    // name [in] - path to operate with
    explicit LocalFile(const Path &path, Log * = 0);
    
    // return true in case file exists
    bool exists()const;
    
    // return file size in bytes
    size_t size()const;
    
    // Unlinks local file
    // return true in case file was successfully unlinked
    CoreError unlink()const;

    // unlinks given folder content
    // path [in] - path to the folder to be erased
    // return true in case folder content was successfully unlinked
    static bool clearFolder(const Path &path, Log *);


    // Reads data from a given local file
    // return true in case file read successfully
    CoreError read(std::vector<unsigned char> &)const;
    CoreError write(const std::vector<unsigned char> &)const;
    CoreError rename(const Path &destination)const;

    /// sourceFileName [in] - point full or relative path including file name
    /// destinationFileName [in] - point full or relative path. File name may be omitted
    static CoreError copyFileImplementation(const Path &sourceFileName, const Path &destinationFileName, Log *);

    // copies file
    // destination [in] - destination for copy operation
    // return true in case file was copied
    CoreError copy(const Path &destination);

    // file name
    Path m_name;

private:
    Log *pLog;
};

// OS independent wrapper for mkdir() function which has different prototypes on different platforms
extern KAVUPDCORE_API bool mkdirWrapper(const Path &path);

// Creates specified folder
// path [in] - folder to create
// return true in case folder was successfully created
extern KAVUPDCORE_API bool createFolder(const Path &path, Log *);
extern bool isReadOnly(const Path &fileName);

// Enumerates all files in folder
// path [in] - folder to enumerate
// return true in case folder content was successfully read
extern bool enumerateFilesInFolder(const Path &path, std::vector<Path> &content, Log * = 0, const bool enumerateSubfolders = false);


// Scoped lock idiom to free file stream resources
class KAVUPDCORE_API AutoStream
{
public:
    AutoStream(Log *);
    ~AutoStream();

    // mode [in] - open file mode, the same for fopen()
    // Note: in case file is opened for write or append, then read-only attribute is cleared
    CoreError open(const Path &fileName, const NoCaseString &mode);
    // close file handle
    void close();

    // safe set file function
    AutoStream &operator=(FILE *);

    // Warning: you can not modify handle
    FILE *stream()const;

private:
    // disable copy operations
    AutoStream &operator=(const AutoStream &);
    AutoStream(const AutoStream &);

    FILE *m_stream;
    Log *pLog;
};

class KAVUPDCORE_API FileStream
{
public:
    FileStream(Log *);
    bool open(const Path &fileName, std::ios_base::openmode);
    void getLine(NoCaseString &);
    // writes the formatted output to the file stream, returning the number of bytes written
    void writeLine(const NoCaseString &buffer);

    // check eof(), bad(), fail()
    bool done();

private:
    Log *pLog;
    #ifdef WSTRING_SUPPORTED
        std::wfstream m_file;
    #else
        std::fstream m_file;
    #endif
};

}   // namespace KLUPD

#endif // LOCAL_FILE_H_INCLUDED_4F5D275E_3FFA_4f55_8AD8_77CEA37CF802
