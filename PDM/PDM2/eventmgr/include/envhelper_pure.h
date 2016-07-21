#ifndef _cEnvironmentHelper_H
#define _cEnvironmentHelper_H

#if !defined(FILE_ATTRIBUTE_READONLY)
#define FILE_ATTRIBUTE_READONLY             0x00000001  
#define FILE_ATTRIBUTE_HIDDEN               0x00000002  
#define FILE_ATTRIBUTE_SYSTEM               0x00000004  
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020  
#define FILE_ATTRIBUTE_DEVICE               0x00000040  
#define FILE_ATTRIBUTE_NORMAL               0x00000080  
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800  
#define FILE_ATTRIBUTE_OFFLINE              0x00001000  
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000  
#endif

#define FILE_ATTRIBUTE_UNDEFINED            0xFFFFFFFE
#define FILE_ATTRIBUTE_INVALID              0xFFFFFFFF

#define INVALID_HANDLE ((handle_t)(-1))

#if !defined(HKEY_LOCAL_MACHINE)
// Reserved Key Handles.
#define HKEY_CLASSES_ROOT           (( regkey_t ) 0x80000000 )
#define HKEY_CURRENT_USER           (( regkey_t ) 0x80000001 )
#define HKEY_LOCAL_MACHINE          (( regkey_t ) 0x80000002 )
#define HKEY_USERS                  (( regkey_t ) 0x80000003 )
#define HKEY_PERFORMANCE_DATA       (( regkey_t ) 0x80000004 )
#define HKEY_CURRENT_CONFIG         (( regkey_t ) 0x80000005 )
#define HKEY_DYN_DATA               (( regkey_t ) 0x80000006 )
// Predefined Value Types.
#define REG_NONE                    ( 0 )   // No value type
#define REG_SZ                      ( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
#define REG_BINARY                  ( 3 )   // Free form binary
#define REG_DWORD                   ( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
#define REG_LINK                    ( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
#define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )
#endif

#include "types.h"
#include "cpath.h"

class cEventHandler;

class cEnvironmentHelper
{
public:
	cEnvironmentHelper() : m_pRootEventHandler(0) { };
	~cEnvironmentHelper() {};

	// environment
	virtual tstring ExpandEnvironmentStr(const tstring value) = 0;
	
	// Interlocked Variable Access
	virtual long    InterlockedIncrement(long* addend) = 0;
	virtual long    InterlockedDecrement(long* addend) = 0;
	virtual long    InterlockedExchangeAdd(long* addend, long value) = 0;

	// Path 
	virtual tstring PathGetLong(const tstring path) = 0;
	virtual bool    PathIsNetwork(const tstring path) = 0;
//	virtual const tstring PathSkipUnicodePrefix(const tstring path) = 0;
	virtual tstring PathFindExecutable(const tstring file, const tstring default_dir) = 0;

	// File operations
	virtual uint32  FileAttr(const tstring fullpath) = 0;
	virtual handle_t  FileOpen(const tstring filename) = 0; // read shared access
	virtual bool    FileClose(handle_t hFile) = 0;
	virtual bool    FileRead(handle_t hFile, uint64 pos, void* buff, uint32 buff_size, uint32* bytes_read) = 0;
	virtual uint64  FileSize(handle_t hFile) = 0;
	virtual uint64  FileSize(const tstring filename) = 0;

	// Registry
	virtual bool    Reg_OpenKey(regkey_t hKey, const tstring lpSubKey, regkey_t* phkResult) = 0;
	virtual bool    Reg_EnumValue(regkey_t hKey, uint32 dwIndex, tstring* lpValueName, uint32* lpType) = 0;
	virtual tstring Reg_QueryValueStr(regkey_t hKey, const tstring lpValueName) = 0;
	virtual bool    Reg_CloseKey(regkey_t hKey) = 0;

public:
	cEventHandler* m_pRootEventHandler;
};


#endif // _cEnvironmentHelper_H