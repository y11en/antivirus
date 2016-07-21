#ifndef __CPATH__H
#define __CPATH__H

// forward declarations
class cPath; 
class cFile; 
class cEnvironmentHelper; 

#include "types.h"
#include <time.h>

#include "_winnt.h"


#define FILE_ATTRIBUTE_UNDEFINED            0xFFFFFFFE
#define FILE_ATTRIBUTE_INVALID              0xFFFFFFFF

#define INVALID_HANDLE ((tHANDLE)(-1))



class cPath
{
public:
	cPath();
	cPath(cEnvironmentHelper* pEnvironmentHelper, tcstring path, tcstring fullpath = NULL, uint32_t attributes = FILE_ATTRIBUTE_UNDEFINED, tcstring default_dir = NULL);
	cPath(cPath& path);
	~cPath();
	void assign(cEnvironmentHelper* pEnvironmentHelper, tcstring path, tcstring fullpath = NULL, uint32_t attributes = FILE_ATTRIBUTE_UNDEFINED, tcstring default_dir = NULL);
	tcstring get();
	tcstring getFull();
	tcstring getFilename();
	tcstring getExtension();
	tcstring appendSlash();
	uint32_t getAttributes();
	size_t getFullLen();
	bool isExist();
	bool compare(tcstring path);

	cPath& operator = (tcstring path);
	cPath& operator = (const cPath& path);

	void   clear();

protected:
	cEnvironmentHelper* env;

private:
	tstring m_path;
	tstring m_fullpath;
	tcstring m_filename;
	tcstring m_extension;
	size_t   m_fullpath_len;
	uint32_t m_attributes;
	tcstring m_default_dir;
	void init();
};


typedef enum enumExecutableType {
	eUnknown = 0, // not analyzed yet
	eNotExecutable,
	eExecutableImage,
	eExecutableLibrary,
	eExecutableScript,
} eExecutableType;

typedef enum enumFileFormats {
	eFormatUnknown = 0,
	eFormatMZ,
	eFormatPE,
	eFormatOLE,
	eFormatText,
} eFileFormat;

#ifndef IMAGE_SCN_MEM_EXECUTE
#define IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define IMAGE_SCN_MEM_READ                   0x40000000  // Section is readable.
#define IMAGE_SCN_MEM_WRITE                  0x80000000  // Section is writeable.
#endif

typedef struct tag_EXECUTABLE_SECT_INFO {
	uint32_t section_raw_offset;
	uint32_t section_raw_size;
	uint32_t section_rva_offset;
//	uint32_t section_rva_size;
	uint32_t characteristics;
} EXECUTABLE_SECT_INFO, *PEXECUTABLE_SECT_INFO;

typedef struct tag_FILE_FORMAT_INFO {
	eFileFormat format;
	eExecutableType executable_type;
	bool     data_analysed;
	bool     ext_analysed;
	uint64_t filesize;
	time_t   timestamp;
	uint32_t checksum;
	uint64_t image_size;
	uint64_t overlay_size;
	uint64_t pe_offset;
	uint64_t entry_point;
	char     entry_point_data[0x40];
	uint32_t sect_count;
	EXECUTABLE_SECT_INFO sections[0x10];
} FILE_FORMAT_INFO, *PFILE_FORMAT_INFO;

class cFile : public cPath
{
public:
	cFile(cEnvironmentHelper* pEnvironmentHelper, tcstring path, tcstring fullpath = NULL, uint32_t attributes = FILE_ATTRIBUTE_UNDEFINED, tHANDLE handle = INVALID_HANDLE, uint64_t uniq = 0, FILE_FORMAT_INFO* pfi = NULL);
	cFile(cPath& path, FILE_FORMAT_INFO* pfi = NULL);
	~cFile();

	uint64_t getUniq() { return m_uniq; };
	bool     Read(uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read = NULL);
	bool     ReadLine(uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read, uint32_t* delim_len = NULL);
	uint64_t Size();
	bool     isReadable();
	bool     isExecutable();
	FILE_FORMAT_INFO fi;

private:
	tHANDLE m_handle;
	bool    m_bTryOpenDone;
	bool    m_bOpened;
	bool    m_bReadTry;
	bool    m_bReadable;
	const uint64_t m_uniq;
};


#endif // __CPATH__H
