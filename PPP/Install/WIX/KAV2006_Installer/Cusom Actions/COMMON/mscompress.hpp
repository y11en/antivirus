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
#if !defined(MS_COMPRESS_CAB_HPP_INCLUDED)
#define MS_COMPRESS_CAB_HPP_INCLUDED
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#undef NOMINMAX
#include <stdexcept>
#include <limits>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "scoped_handle.hpp"
#include "fci.h"
#include "fdi.h"

///////////////////////////////////////////////////////////////////////////////
namespace mszip
{
///////////////////////////////////////////////////////////////////////////////
enum compress_type
{
	mask_type		= tcompMASK_TYPE,
	none_type		= tcompTYPE_NONE,
	mszip_type		= tcompTYPE_MSZIP,
	quantum_type	= tcompTYPE_QUANTUM,
	lzx_type		= tcompTYPE_LZX,
	bad_type		= tcompBAD
};
///////////////////////////////////////////////////////////////////////////////
class library_not_found
		: public std::runtime_error
{
public:
	library_not_found()
		:	std::runtime_error("cabinet.dll not found")
	{
	}
};
///////////////////////////////////////////////////////////////////////////////
class entrypoint_not_found
		: public std::runtime_error
{
public:
	entrypoint_not_found(std::string const& entry_name)
		:	std::runtime_error(entry_name)
	{
	}
};
///////////////////////////////////////////////////////////////////////////////
class compress_error
		: public std::runtime_error
{
public:
	explicit compress_error(std::string const& msg)
		: std::runtime_error(msg)
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
typedef BOOL (FAR DIAMONDAPI *fn_fci_flush_cabinet_type)(HFCI hfci
								, BOOL						fGetNextCab
								, PFNFCIGETNEXTCABINET		GetNextCab
								, PFNFCISTATUS				pfnProgress);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fci_flush_folder_type)(HFCI hfci
								, PFNFCIGETNEXTCABINET		GetNextCab
								, PFNFCISTATUS				pfnProgress);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fci_destroy_type)(HFCI hfci);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fdi_is_cabinet)(HFDI hfdi
								, INT_PTR hf
								, PFDICABINETINFO pfdici);
///////////////////////////////////////////////////////////////////////////////
typedef HFDI (FAR DIAMONDAPI *fn_fdi_create)(PFNALLOC pfnalloc
								, PFNFREE  pfnfree
								, PFNOPEN  pfnopen
								, PFNREAD  pfnread
								, PFNWRITE pfnwrite
								, PFNCLOSE pfnclose
								, PFNSEEK  pfnseek
								, int      cpuType
								, PERF     perf);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fdi_copy)(HFDI hfdi
								, char FAR *pszCabinet
								, char FAR *pszCabPath
								, int flags
								, PFNFDINOTIFY pfnfdin
								, PFNFDIDECRYPT pfnfdid
								, void FAR *pvUser);
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (FAR DIAMONDAPI *fn_fdi_destroy)(HFDI hfdi);
///////////////////////////////////////////////////////////////////////////////
inline __declspec(noreturn) void _fci_issue_error(int error_code)
{
	switch (error_code)
	{
		case FCIERR_NONE:
			throw mszip::compress_error("no error");
		case FCIERR_OPEN_SRC:
			throw mszip::compress_error("failure opening file to be stored in cabinet");
		case FCIERR_READ_SRC:
			throw mszip::compress_error("failure reading file to be stored in cabinet");
		case FCIERR_ALLOC_FAIL:
			throw mszip::compress_error("insufficient memory in fci");
		case FCIERR_TEMP_FILE:
			throw mszip::compress_error("could not create a temporary file");
		case FCIERR_BAD_COMPR_TYPE:
			throw mszip::compress_error("unknown compression type");
		case FCIERR_CAB_FILE:
			throw mszip::compress_error("could not create cabinet file");
		case FCIERR_USER_ABORT:
			throw mszip::compress_error("client requested abort");
		case FCIERR_MCI_FAIL:
			throw mszip::compress_error("failure compressing data");
		default:
			throw mszip::compress_error("unknown error");
	}
}
///////////////////////////////////////////////////////////////////////////////
inline __declspec(noreturn) void _fdi_issue_error(int error_code)
{
	switch(error_code)
	{
	case FDIERROR_NONE:
		throw mszip::compress_error("no error");
	case FDIERROR_CABINET_NOT_FOUND:
		throw mszip::compress_error("cabinet not found");
	case FDIERROR_NOT_A_CABINET:
		throw mszip::compress_error("cabinet file does not have the correct format");
	case FDIERROR_UNKNOWN_CABINET_VERSION:
		throw mszip::compress_error("cabinet file has an unknown version number");
	case FDIERROR_CORRUPT_CABINET:
		throw mszip::compress_error("cabinet file is corrupt");
	case FDIERROR_ALLOC_FAIL:
		throw mszip::compress_error("could not allocate enough memory");
	case FDIERROR_BAD_COMPR_TYPE:
		throw mszip::compress_error("unknown compression type in a cabinet folder");
	case FDIERROR_MDI_FAIL:
		throw mszip::compress_error("failure decompressing data from a cabinet file");
	case FDIERROR_TARGET_FILE:
		throw mszip::compress_error("failure writing to target file");
	case FDIERROR_RESERVE_MISMATCH:
		throw mszip::compress_error("cabinets in a set do not have the same RESERVE sizes");
	case FDIERROR_WRONG_CABINET:
		throw mszip::compress_error("cabinet returned on fdintNEXT_CABINET is incorrect");
	case FDIERROR_USER_ABORT:
		throw mszip::compress_error("fdi aborted");
	default:
		throw mszip::compress_error("unknown error");
	}
}
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
/**
 * \brief create cabs files and add files
 */
class compressor
			: private boost::noncopyable
{
public:
	/** ctor */
	compressor(std::string const& folder
					, std::string const& name
					, compress_type type = mszip::mszip_type
					, unsigned long size = std::numeric_limits<long>::max())
		: dll_module_(::LoadLibrary(TEXT("cabinet.dll")))
		, type_(type)
		, size_(size)
		, folder_(folder)
		, name_(name)
		, hfci_(0)
	{
		using namespace detail;

		if(!dll_module_)
			throw library_not_found();
		if(!(fn_fci_create_ = 
			reinterpret_cast<fn_fci_create_type>(::GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FCICreate"))))
			throw entrypoint_not_found("FCICreate not found");
		if(!(fn_fci_destroy_ =
			reinterpret_cast<fn_fci_destroy_type>(::GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FCIDestroy"))))
			throw entrypoint_not_found("FCIDestroy not found");
		if(!(fn_fci_flush_cabinet_ =
			reinterpret_cast<fn_fci_flush_cabinet_type>(::GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FCIFlushCabinet"))))
			throw entrypoint_not_found("FCIFlushCabinet not found");	
		if(!(fn_fci_flush_folder_ =
			reinterpret_cast<fn_fci_flush_folder_type>(::GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FCIFlushFolder"))))
			throw entrypoint_not_found("FCIFlushFolder not found");
		if(!(fn_fci_add_file_ =
			reinterpret_cast<fn_fci_add_file_type>(::GetProcAddress(static_cast<HMODULE>(dll_module_.get())
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
		strcat(cab_params_.szCab, ".cab");
		// create cab file
		hfci_ = fn_fci_create_(&error_, file_placed, mem_alloc, mem_free
							, fci_open, fci_read, fci_write, fci_close
							, fci_seek, fci_delete, get_temp_file
							, &cab_params_, this);
		if(!hfci_)
			detail::_fci_issue_error(error_.erfOper);
	}
	// dctor
	~compressor()
	{
		if(hfci_)
		{
			// flush all changes tot cabinet file
			fn_fci_flush_cabinet_(hfci_, FALSE, get_next_cabinet, progress);
			// close cabinet file
			fn_fci_destroy_(hfci_);
		}
	}
	//! add file to cabinet file
	void compress(std::string const& full_name)
	{
		std::string::size_type pos = full_name.find_last_of('\\');
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
private:
	/** definition of cabinet callback functions */
	static FNFCIALLOC(mem_alloc) { return malloc(cb); }
	static FNFCIFREE(mem_free) { free(memory); }
	static FNFCIOPEN(fci_open)
	{
		int result = _open(pszFile, oflag, pmode);
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
		if (!result)
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
		compressor const* encoder = static_cast<compressor const*>(pv);
		strcpy(pccab->szCab, encoder->name_.c_str());
		strcat(pccab->szCab, _itoa(pccab->iCab, buffer, 10));
		strcat(pccab->szCab, ".cab");
		return TRUE;
	}
	static FNFCIGETOPENINFO(get_open_info)
	{
		HANDLE handle = CreateFileA(pszName
								, 0
								, FILE_SHARE_READ
								, 0
								, OPEN_EXISTING
								, FILE_ATTRIBUTE_NORMAL, 0);
		if(handle == INVALID_HANDLE_VALUE)
			return -1;
		
		BY_HANDLE_FILE_INFORMATION finfo;
		if(!GetFileInformationByHandle(handle, &finfo))
			return CloseHandle(handle), -1;
		CloseHandle(handle);

		FILETIME filetime;
		FileTimeToLocalFileTime(&finfo.ftLastWriteTime, &filetime);
		FileTimeToDosDateTime(&filetime, pdate, ptime);
		*pattribs = static_cast<int>(GetFileAttributesA(pszName) & (_A_RDONLY
																	| _A_SYSTEM
																	| _A_HIDDEN
																	| _A_ARCH));
		return _open(pszName, _O_RDONLY | _O_BINARY);
	}
private:
	typedef util::scoped_handle<HMODULE, BOOL (WINAPI*)(HMODULE), &::FreeLibrary, 0, 0> mudule_handle_t;
	/** handle to cabinet dll */
	mudule_handle_t						dll_module_;
	/** pointer to function FCICreate */
	detail::fn_fci_create_type			fn_fci_create_;
	/** pointer to function FCIDestroy */
	detail::fn_fci_destroy_type			fn_fci_destroy_;
	/** pointer to function FCIFlushCabinet */
	detail::fn_fci_flush_cabinet_type	fn_fci_flush_cabinet_;
	/** pointer to function FCIFolder */
	detail::fn_fci_flush_folder_type	fn_fci_flush_folder_;
	/** pointer to function FCIAddFile */
	detail::fn_fci_add_file_type		fn_fci_add_file_;
	/** cab compression type */
	compress_type						type_;
	/** size of cab */
	unsigned long						size_;
	/** destination folder */
	std::string							folder_;
	/** cab name with out .cab extention */
	std::string							name_;
	HFCI								hfci_;
	ERF									error_;
	CCAB								cab_params_;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \brief extract files from cabs
 */
class decompressor
		: private boost::noncopyable
{
public:

    class name_resolver
    {
    public:
        virtual ~name_resolver() {}
        virtual std::string resolve(std::string a) = 0;
    };

	explicit decompressor(std::string const& cab_name, name_resolver* resolver = 0)
		: cab_name_(cab_name)
		, dll_module_(::LoadLibrary(TEXT("cabinet.dll")))
        , resolver_(resolver)
	{
		using namespace detail;

		if(!dll_module_)
			throw library_not_found();

		if(!(fn_is_cabinet_ = 
			reinterpret_cast<fn_fdi_is_cabinet>(::GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FDIIsCabinet"))))
			throw entrypoint_not_found("FDIIsCabinet not found");
		if(!(fn_destroy_ =
			reinterpret_cast<fn_fdi_destroy>(::GetProcAddress(reinterpret_cast<HMODULE>(dll_module_.get())
																, "FDIDestroy"))))
			throw entrypoint_not_found("FDIDestroy not found");
		if(!(fn_copy_ =
			reinterpret_cast<fn_fdi_copy>(::GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FDICopy"))))
			throw entrypoint_not_found("FDICopy not found");	
		if(!(fn_create_ =
			reinterpret_cast<fn_fdi_create>(::GetProcAddress(static_cast<HMODULE>(dll_module_.get())
																, "FDICreate"))))
			throw entrypoint_not_found("FDICreate not found");
		// create cab file
		hfdi_ = fn_create_(mem_alloc, mem_free
							, fdi_open, fdi_read, fdi_write, fdi_close
							, fdi_seek, cpu80386, &error_);
		if(!hfdi_)
			detail::_fci_issue_error(error_.erfOper);
	}
	~decompressor()
	{
		if(hfdi_)
			fn_destroy_(hfdi_);
	}
	bool decompress(std::string const& to)
	{
		std::string::size_type pos = cab_name_.find_last_of("\\/");
		if(std::string::npos == pos)
			throw std::invalid_argument(cab_name_);
		std::string name(cab_name_.substr(pos + 1));
		std::string folder(cab_name_.substr(0, pos + 1));
		destination_ = to;
		if(destination_.empty())
			throw std::invalid_argument("argument is empty");
		if(destination_[destination_.length() - 1] != '\\'
			&& destination_[destination_.length() - 1] != '/')
			destination_.append(1, '\\');
		return fn_copy_(hfdi_
						, const_cast<char*>(name.c_str())
						, const_cast<char*>(folder.c_str())
						, 0, fdi_notification_function
						, 0, this) != 0;
	}
	std::string const& path() const { return destination_; }
private:
	static FNALLOC(mem_alloc) {	return malloc(cb); }
	static FNFREE(mem_free) { free(pv); }
	static FNOPEN(fdi_open) { return _open(pszFile, oflag, pmode); }
	static FNREAD(fdi_read) { return _read(hf, pv, cb); }
	static FNWRITE(fdi_write) { return _write(hf, pv, cb); }
	static FNCLOSE(fdi_close) { return _close(hf); }
	static FNSEEK(fdi_seek) { return _lseek(hf, dist, seektype); }
	static FNFDINOTIFY(fdi_notification_function)
	{
		switch(fdint)
		{
			case fdintCABINET_INFO: // general information about the cabinet
				return 0;
			case fdintPARTIAL_FILE: // first file in cabinet is continuation
				return 0;
			case fdintCOPY_FILE: // file to be copied
			{
				decompressor *_this = static_cast<decompressor *>(pfdin->pv);
				std::string destination(_this->path());
				destination.append(pfdin->psz1);
                if (_this->resolver_.get()) destination = _this->resolver_->resolve(destination);
				return fdi_open(const_cast<char*>(destination.c_str())
									, _O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL
									, _S_IREAD | _S_IWRITE);

			}
		case fdintCLOSE_FILE_INFO:	// close the file, set relevant info
        {
			decompressor *_this = static_cast<decompressor *>(pfdin->pv);
			std::string destination(_this->path());
			destination.append(pfdin->psz1);

			fdi_close(pfdin->hf);
			typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &::CloseHandle, 0, -1> file_handle_t;
			//Set date/time
            file_handle_t handle(CreateFileA(destination.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0));
            if(handle)
            {
                FILETIME datetime = { 0 };
                if(::DosDateTimeToFileTime(pfdin->date, pfdin->time, &datetime))
                {
                    FILETIME local_filetime = { 0 };
                    if (::LocalFileTimeToFileTime(&datetime, &local_filetime))
                        ::SetFileTime(handle.get(), &local_filetime, 0, &local_filetime);
                }
            }
            DWORD   attrs(pfdin->attribs);
            attrs &= _A_ARCH;
            ::SetFileAttributes(destination.c_str(), attrs);
			return 1;
        }
		case fdintNEXT_CABINET:
			return 0;
		}
		return 0;
	}
private:
	typedef util::scoped_handle<HMODULE, BOOL (WINAPI*)(HMODULE), &::FreeLibrary, 0, 0> mudule_handle_t;
	mudule_handle_t				dll_module_;
	detail::fn_fdi_is_cabinet	fn_is_cabinet_;
	detail::fn_fdi_create		fn_create_;
	detail::fn_fdi_copy			fn_copy_;
	detail::fn_fdi_destroy		fn_destroy_;
	ERF							error_;
	HFDI						hfdi_;
	FDICABINETINFO				fdici_;
	std::string					cab_name_;
	std::string					destination_;
    std::auto_ptr<name_resolver>    resolver_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace mszip
///////////////////////////////////////////////////////////////////////////////
#endif// !defined(MS_COMPRESS_CAB_HPP_INCLUDED)