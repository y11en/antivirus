/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	mscompress.hpp
*		\brief	С++ compression library using MS Cabinet SDK
*		
*		\author Владислав Овчарик
*		\date	18.02.2004 16:02:27
*		
*		
*		
*/		
#pragma once
#include <stdexcept>
#include <limits>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <boost/shared_ptr.hpp>
#include "fci.h"
#include "fdi.h"

#include <std/trc/trace.h>
#include <avp/text2bin.h>
#include <std/base/klbase.h>

static std::string WideString2BinaryString( const std::wstring &str )
{
	const size_t	nSizeText = str.length() * sizeof(wchar_t);
	const size_t	nBlock = B2T_LENGTH(nSizeText);

	KLSTD::CArrayPointer<char> pBlock;
    pBlock = (char *)malloc( (nBlock + 1) );
    if(!(char*)pBlock) return std::string();

    memset((char*)pBlock, 0, nBlock + 1);
    if( !BinToText(str.c_str(), nSizeText, pBlock, nBlock)) return std::string();

	return std::string( (char *)pBlock, nBlock );

	
}

static std::wstring BinaryString2WideString( const std::string &binStr )
{
	if ( binStr.empty() ) return std::wstring();

	const size_t	nSizeText = binStr.length();
	size_t			nBlock = T2B_LENGTH(nSizeText);

    KLSTD::CArrayPointer<char> pBlock;
    pBlock = (char *)malloc( (nBlock+ 1 ) );
    if(!(char*)pBlock) return std::wstring();
	int nData = 0;
    memset((char*)pBlock, 0, nBlock + 1);
    if(!(nBlock = TextToBin(binStr.c_str(), nSizeText, pBlock, nBlock)))
		return std::wstring();

	return std::wstring( (wchar_t *)(char *)pBlock, nBlock / sizeof(wchar_t) );
}

///////////////////////////////////////////////////////////////////////////////
namespace mszip
{
///////////////////////////////////////////////////////////////////////////////
enum compression_type
{
	MASK_TYPE		= tcompMASK_TYPE,
	NONE_TYPE		= tcompTYPE_NONE,
	MSZIP_TYPE		= tcompTYPE_MSZIP,
	QUANTUM_TYPE	= tcompTYPE_QUANTUM,
	LZX_TYPE		= tcompTYPE_LZX,
	BAD_TYPE		= tcompBAD
};
///////////////////////////////////////////////////////////////////////////////
class library_not_found
:	public std::runtime_error
{
public:
	library_not_found()
		:	std::runtime_error("cabinet.dll not found")
	{
	}
};
///////////////////////////////////////////////////////////////////////////////
class entrypoint_not_found
:	public std::runtime_error
{
public:
	entrypoint_not_found(std::string const& entry_name)
		:	std::runtime_error(entry_name)
	{
	}
};
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
typedef HFCI (FAR DIAMONDAPI *fn_fci_create_type)(PERF	perf
								, PFNFCIFILEPLACED	pfnfiledest
								, PFNFCIALLOC		pfnalloc
								, PFNFCIFREE		pfnfree
								, PFNFCIOPEN		pfnopen
								, PFNFCIREAD		pfnread
								, PFNFCIWRITE		pfnwrite
								, PFNFCICLOSE		pfnclose
								, PFNFCISEEK		pfnseek
								, PFNFCIDELETE		pfndelete
								, PFNFCIGETTEMPFILE	pfntemp
								, PCCAB				pccab
								, void FAR *		pv);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fci_add_file_type)(HFCI		hfci
								, char					*pszSourceFile
								, char					*pszFileName
								, BOOL					fExecute
								, PFNFCIGETNEXTCABINET	GetNextCab
								, PFNFCISTATUS			pfnProgress
								, PFNFCIGETOPENINFO		pfnOpenInfo
								, TCOMP					typeCompress);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fci_flush_cabinet_type)(HFCI	hfci
								, BOOL						fGetNextCab
								, PFNFCIGETNEXTCABINET		GetNextCab
								, PFNFCISTATUS				pfnProgress);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fci_flush_folder_type)(HFCI		hfci
								, PFNFCIGETNEXTCABINET		GetNextCab
								, PFNFCISTATUS				pfnProgress);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fci_destroy_type)(HFCI			hfci);
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef HFDI (FAR DIAMONDAPI *fn_fdi_create_type)( 
								  PFNALLOC			pfnalloc
								, PFNFREE			pfnfree
								, PFNOPEN			pfnopen
								, PFNREAD			pfnread
								, PFNWRITE			pfnwrite
								, PFNCLOSE			pfnclose
								, PFNSEEK			pfnseek
								, int				cpuType
								, PERF     perf );
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fdi_destroy_type)(HFDI			hfci);
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fdi_copy_type)( HFDI   hfdi
								, char FAR				*pszCabinet
								, char FAR				*pszCabPath
								, int					flags
								, PFNFDINOTIFY			pfnfdin
								, PFNFDIDECRYPT			pfnfdid
								, void FAR				*pvUser );
///////////////////////////////////////////////////////////////////////////////
inline __declspec(noreturn) void _fci_issue_error(int error_code)
{
	switch (error_code)
	{
		case FCIERR_NONE:
			throw std::runtime_error("No error");
		case FCIERR_OPEN_SRC:
			throw std::runtime_error("Failure opening file to be stored in cabinet");
		case FCIERR_READ_SRC:
			throw std::runtime_error("Failure reading file to be stored in cabinet");
		case FCIERR_ALLOC_FAIL:
			throw std::runtime_error("Insufficient memory in FCI");
		case FCIERR_TEMP_FILE:
			throw std::runtime_error("Could not create a temporary file");
		case FCIERR_BAD_COMPR_TYPE:
			throw std::runtime_error("Unknown compression type");
		case FCIERR_CAB_FILE:
			throw std::runtime_error("Could not create cabinet file");
		case FCIERR_USER_ABORT:
			throw std::runtime_error("Client requested abort");
		case FCIERR_MCI_FAIL:
			throw std::runtime_error("Failure compressing data");
		default:
			throw std::runtime_error("Unknown error");
	}
}
inline __declspec(noreturn) void _fdi_issue_error(int error_code)
{
	switch (error_code)
	{
		case FDIERROR_NONE:
			throw std::runtime_error("No error");
		case FDIERROR_CABINET_NOT_FOUND:
			throw std::runtime_error("Cabinet not found");
		case FDIERROR_NOT_A_CABINET:
			throw std::runtime_error("Wrong format of cabinet file");
		case FDIERROR_UNKNOWN_CABINET_VERSION:
			throw std::runtime_error("Unknown cabinet version");
		case FDIERROR_CORRUPT_CABINET:
		case FDIERROR_WRONG_CABINET:
			throw std::runtime_error("Cabinet file is corrupt");
		case FDIERROR_ALLOC_FAIL:
			throw std::runtime_error("Insufficient memory in FCI");
		case FDIERROR_BAD_COMPR_TYPE:
			throw std::runtime_error("Unknown compression type");
		case FDIERROR_MDI_FAIL:
			throw std::runtime_error("Failure compressing data");
		case FDIERROR_TARGET_FILE:
			throw std::runtime_error("Failure writing target file");
		case FDIERROR_RESERVE_MISMATCH:
			throw std::runtime_error("Reserve mismatch");
		case FDIERROR_USER_ABORT:
			throw std::runtime_error("Client requested abort");			
		default:
			throw std::runtime_error("Unknown error");
	}
}
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////

class cab_helper
{
public:

	cab_helper() { hfci_ = NULL; hfdi_ = NULL; }
	/** ctor */	
	void Init(compression_type type
				, long size
				, std::string const& folder
				, std::string const& name
				, bool compress )		
		
	{
		dll_module_ = boost::shared_ptr<void>(::LoadLibrary(TEXT("cabinet.dll")), ::FreeLibrary);
		type_ = type;
		size_ = size;
		folder_ = folder;
		name_ = name;
		hfci_ = 0;
		hfdi_ = 0;
		using namespace detail;
		if(!dll_module_)
			throw library_not_found();

		if ( compress )
		{
			if(!(fn_fci_create_ = 
				reinterpret_cast<fn_fci_create_type>(GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FCICreate"))))
																throw entrypoint_not_found("FCICreate not found");
			if(!(fn_fci_destroy_ =
				reinterpret_cast<fn_fci_destroy_type>(GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FCIDestroy"))))
																throw entrypoint_not_found("FCIDestroy not found");
			if(!(fn_fci_flush_cabinet_ =
				reinterpret_cast<fn_fci_flush_cabinet_type>(GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FCIFlushCabinet"))))
																throw entrypoint_not_found("FCIFlushCabinet not found");	
			if(!(fn_fci_flush_folder_ =
				reinterpret_cast<fn_fci_flush_folder_type>(GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FCIFlushFolder"))))
																throw entrypoint_not_found("FCIFlushFolder not found");
			if(!(fn_fci_add_file_ =
				reinterpret_cast<fn_fci_add_file_type>(GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FCIAddFile"))))
																throw entrypoint_not_found("FCIAddFile not found");
			memset(&cab_params_, 0, sizeof(cab_params_));
			cab_params_.cb = size;
			cab_params_.cbFolderThresh = size;
			cab_params_.iCab = 0;
			cab_params_.iDisk = 0;
			cab_params_.setID = 0x4C4B;
			strcpy(cab_params_.szCabPath, folder.c_str());
			strcpy(cab_params_.szCab, name.c_str());		
			
			hfci_ = fn_fci_create_(&error_, file_placed, mem_alloc, mem_free
				, fci_open, fci_read, fci_write, fci_close
				, fci_seek, fci_delete, get_temp_file
				, &cab_params_, this);
			if(!hfci_)
				detail::_fci_issue_error(error_.erfOper);
		}
		else
		{
			if(!(fn_fdi_create_ = 
				reinterpret_cast<fn_fdi_create_type>(GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FDICreate"))))
																throw entrypoint_not_found("FDICreate not found");
			if(!(fn_fdi_destroy_ =
				reinterpret_cast<fn_fdi_destroy_type>(GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FDIDestroy"))))
																throw entrypoint_not_found("FDIDestroy not found");
			if(!(fn_fdi_copy_ =
				reinterpret_cast<fn_fdi_copy_type>(GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FDICopy"))))
																throw entrypoint_not_found("FDICopy not found");

			hfdi_ = fn_fdi_create_( mem_alloc, mem_free
				, fdi_open, fdi_read, fdi_write, fdi_close
				, fdi_seek, cpu80386, &error_);
			if(!hfdi_)
				detail::_fdi_issue_error(error_.erfOper);
			
		}
	}
	~cab_helper()
	{
		if(hfci_)
		{
			fn_fci_flush_cabinet_(hfci_, FALSE, get_next_cabinet, progress);
			fn_fci_destroy_(hfci_);
		}
		if (hfdi_)
		{			
			fn_fdi_destroy_(hfdi_);
		}
	}
protected:
	/** definition of cabinet callback functions */
	static FNFCIALLOC(mem_alloc) { return malloc(cb); }
	static FNFCIFREE(mem_free) { free(memory); }
	static FNFCIOPEN(fci_open)
	{
		int result = _open(
			pszFile, 
			oflag, pmode);
		if (result == -1)
			*err = errno;
		return result;
	}
	static FNFCIREAD(fci_read)
	{
		unsigned int result = _read(hf, memory, cb);
		if (result != cb)
			*err = errno;
		return result;
	}
	static FNFCIWRITE(fci_write)
	{
		unsigned int result = _write(hf, memory, cb);
		if (result != cb)
			*err = errno;
		return result;
	}
	static FNFCICLOSE(fci_close)
	{
		int result = _close(hf);
		if (result)
			*err = errno;
		return result;
	}
	static FNFCISEEK(fci_seek)
	{
		long result = _lseek(hf, dist, seektype);
		if (result == -1)
			*err = errno;
		return result;
	}
	static FNOPEN(fdi_open)
	{
		int result = _open(pszFile, oflag, pmode);		
		return result;
	}
	static FNREAD(fdi_read)
	{
		unsigned int result = _read(hf, pv, cb);		
		return result;
	}
	static FNWRITE(fdi_write)
	{
		unsigned int result = _write(hf, pv, cb);		
		return result;
	}
	static FNCLOSE(fdi_close)
	{
		int result = _close(hf);		
		return result;
	}
	static FNSEEK(fdi_seek)
	{
		long result = _lseek(hf, dist, seektype);		
		return result;
	}
	static FNFCIDELETE(fci_delete)
	{
		int result = remove(pszFile);
		if (result != 0)
			*err = errno;
		return result;
	}
	static FNFCIFILEPLACED(file_placed) { return 0; }
	static FNFCIGETTEMPFILE(get_temp_file)
	{
		char	*psz = _tempnam("", "xx");
		if (psz && (strlen(psz) < static_cast<size_t>(cbTempName)))
		{
			strcpy(pszTempName, psz);
			return free(psz), TRUE;
		}
		return psz ? (free(psz), FALSE) : FALSE;
	}
	static FNFCISTATUS(progress) { return 0; }
	static FNFCIGETNEXTCABINET(get_next_cabinet)
	{
		char buffer[16] = { 0 };
		cab_helper const* encoder = static_cast<cab_helper const*>(pv);
		strcpy(pccab->szCab, encoder->name_.c_str());
		strcat(pccab->szCab, itoa(pccab->iCab, buffer, 10));
		strcat(pccab->szCab, ".cab");
		return TRUE;
	}
	static FNFCIGETOPENINFO(get_open_info)
	{
		std::wstring wideStrFile = BinaryString2WideString( pszName );
		if ( wideStrFile.empty() ) wideStrFile = KLSTD_A2W2( pszName );

		HANDLE handle = CreateFileW(
								wideStrFile.c_str()
								, 0
								, FILE_SHARE_READ
								, 0
								, OPEN_EXISTING
								, FILE_ATTRIBUTE_NORMAL, 0);
		
		if(handle == INVALID_HANDLE_VALUE)
		{
			KLTRACE3( L"FCI", L"compress_file (CreateFileA error ) pszName - '%ls'.\n", wideStrFile.c_str() );
			return -1;
		}

		BY_HANDLE_FILE_INFORMATION bhfi;
		if(!GetFileInformationByHandle(handle, &bhfi))
		{
			KLTRACE3( L"FCI", L"compress_file (GetFileInformationByHandle error) pszName - '%ls'.\n", wideStrFile.c_str() );
			return CloseHandle(handle), -1;
		}

		FILETIME filetime;
		FileTimeToLocalFileTime(&bhfi.ftLastWriteTime, &filetime);
		FileTimeToDosDateTime(&filetime, pdate, ptime);

		*pattribs = (int)(bhfi.dwFileAttributes & (_A_RDONLY | _A_SYSTEM | _A_HIDDEN | _A_ARCH));

		CloseHandle(handle);

		return _wopen(
			wideStrFile.c_str(),
			_O_RDONLY | _O_BINARY);;
	}
	static FNFDINOTIFY(fdi_notify)
	{
		if ( fdint==fdintCOPY_FILE )
		{
			std::string folderName( (char *)pfdin->pv );
			int err = 0;
			int fHandle = fci_open( (char *)std::string(folderName + std::string(pfdin->psz1)).c_str(), 
				_O_CREAT | _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE, &err,
				NULL );
			if ( err ) return (-1);
			return fHandle;
		}
		if ( fdint==fdintCLOSE_FILE_INFO )
		{
			int err = 0;
			fci_close( pfdin->hf, &err, NULL );
			if ( err ) return (-1);
			SetFileAttributesA( pfdin->psz1, pfdin->attribs );
			return 1;
		}
		return 0;
	}
public:
	/** handle to cabinet dll */
	boost::shared_ptr<void>				dll_module_;
	/** pointer to function FCICreate */
	detail::fn_fci_create_type			fn_fci_create_;
	/** pointer to function FDICreate */
	detail::fn_fdi_create_type			fn_fdi_create_;
	/** pointer to function FCIDestroy */
	detail::fn_fci_destroy_type			fn_fci_destroy_;
	/** pointer to function FDIDestroy */
	detail::fn_fdi_destroy_type			fn_fdi_destroy_;
	/** pointer to function FCIFlushCabinet */
	detail::fn_fci_flush_cabinet_type	fn_fci_flush_cabinet_;
	/** pointer to function FCIFolder */
	detail::fn_fci_flush_folder_type	fn_fci_flush_folder_;
	/** pointer to function FCIAddFile */
	detail::fn_fci_add_file_type		fn_fci_add_file_;
	/** pointer to function FDICopy */
	detail::fn_fdi_copy_type			fn_fdi_copy_;
	/** cab compression type */
	compression_type					type_;
	/** size of cab */
	unsigned long						size_;
	/** destination folder */
	std::string							folder_;
	/** cab name with out .cab extention */
	std::string							name_;
	HFCI								hfci_;	
	HFDI								hfdi_;	
	ERF									error_;
	CCAB								cab_params_;
};

/** create cabs files and add files */
class cab_compressor : public cab_helper
{
public:
	bool bInited;

	cab_compressor()
	{
		bInited = false;
	}
	/** ctor */
	cab_compressor(compression_type type
					, long size
					, std::string const& folder
					, std::string const& name)
	{
		cab_helper::Init( type, size, folder, name, true );
		bInited = true;
	}

	void Init(compression_type type
					, long size
					, std::string const& folder
					, std::string const& name)
	{
		cab_helper::Init( type, size, folder, name, true );
		bInited = true;
	}
	void InitWide(compression_type type
					, long size
					, std::wstring const& folder
					, std::wstring const& name)
	{
		cab_helper::Init( type, size, (const char *)KLSTD_W2A2(folder.c_str()), 
			(const char *)KLSTD_W2A2(name.c_str()), true );
		bInited = true;
	}
	~cab_compressor()
	{
	}	
	void compress_file(std::string const& full_name)
	{
		std::string::size_type pos = full_name.find_last_of('\\');
		std::string::size_type lpos = full_name.find_last_of('/');
		if(std::string::npos != lpos && lpos>pos ) pos = lpos;
		if(std::string::npos == pos)
			throw std::invalid_argument(full_name);

		char const *file_name = full_name.c_str() + pos + 1;
		if(!fn_fci_add_file_(hfci_
							, const_cast<char*>(full_name.c_str())
							, const_cast<char*>(file_name)
							, FALSE
							, get_next_cabinet
							, progress
							, get_open_info, type_))
			detail::_fci_issue_error(error_.erfOper);
	}
	void compress_file(std::string const& full_name,std::string const& file_name)
	{
		if(!fn_fci_add_file_(hfci_
							, const_cast<char*>(full_name.c_str())
							, const_cast<char*>(file_name.c_str())
							, FALSE
							, get_next_cabinet
							, progress
							, get_open_info, type_))
			detail::_fci_issue_error(error_.erfOper);
	}
	void compress_file_wide(std::wstring const& full_name)
	{
		std::wstring::size_type pos = full_name.find_last_of(L'\\');
		std::string::size_type lpos = full_name.find_last_of(L'/');
		if(std::string::npos != lpos && lpos>pos ) pos = lpos;
		if(std::wstring::npos == pos)
			throw std::invalid_argument(std::string());

		wchar_t const *file_name = full_name.c_str() + pos + 1;		
		std::string binStr_full_name = WideString2BinaryString( full_name );
		if(!fn_fci_add_file_(hfci_
							, (char *)binStr_full_name.c_str()
							, (char*)(KLSTD_W2A2(file_name))
							, FALSE
							, get_next_cabinet
							, progress
							, get_open_info, type_))
			detail::_fci_issue_error(error_.erfOper);
	}
	void compress_file_wide(std::wstring const& full_name,std::wstring const& file_name)
	{
		std::string binStr_full_name = WideString2BinaryString( full_name );
		if(!fn_fci_add_file_(hfci_
							, (char *)binStr_full_name.c_str()
							, (char*)(KLSTD_W2A2(file_name.c_str()))
							, FALSE
							, get_next_cabinet
							, progress
							, get_open_info, type_))
			detail::_fci_issue_error(error_.erfOper);
	}
};
///////////////////////////////////////////////////////////////////////////////
/** extract files from cabs */
class cab_decompressor : public cab_helper
{
public:
	/** ctor */
	cab_decompressor(compression_type type
					, long size
					, std::string const& folder
					, std::string const& name)
	{
		cab_helper::Init( type, size, folder, name, false );
	}
	~cab_decompressor()
	{
	}
	void extract( const std::string &extractPath )
	{
		if(!fn_fdi_copy_(hfdi_
							, const_cast<char*>(name_.c_str())
							, const_cast<char*>(folder_.c_str())
							, 0
							, fdi_notify
							, NULL
							, const_cast<char*>(extractPath.c_str()) ))
			detail::_fci_issue_error(error_.erfOper);
	}
};
///////////////////////////////////////////////////////////////////////////////
}//namespace mszip
///////////////////////////////////////////////////////////////////////////////