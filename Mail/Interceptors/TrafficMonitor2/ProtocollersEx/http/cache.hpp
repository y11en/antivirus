/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	cache.hpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	28.01.2006 18:58:50
*		
*		
*/		
#if !defined(_CAHE_HPP_INCLUDED)
#define _CAHE_HPP_INCLUDED
#include <Prague/wrappers/scoped_handle.hpp>
#include <boost/shared_ptr.hpp>
#include "version/ver_product.h"
///////////////////////////////////////////////////////////////////////////////
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ "(" TOSTRING(__LINE__) ")"
///////////////////////////////////////////////////////////////////////////////
namespace cache
{
///////////////////////////////////////////////////////////////////////////////
/** describe win32 class error*/
class win32_error
		: public std::runtime_error
{
public:
	win32_error(unsigned long err_code, char const* user_msg)
				: std::runtime_error(do_error_message(user_msg, err_code))
				, error_code_(err_code)
	{
	}
	unsigned long error() const { return error_code_; }
private:
	/** does make user friendly message*/
	static std::string do_error_message(char const* user_msg, unsigned long err_code)
	{
		typedef util::scoped_handle<HLOCAL, HLOCAL(WINAPI*)(HLOCAL), &::LocalFree, 1> hlocal;
		hlocal message;
		DWORD chars(::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER
									, 0, err_code, 0
									, reinterpret_cast<char*>(&message.get_ref()), 0, 0));
		if(chars)
		{
			std::string msg(user_msg);
			msg.append(": ");
			msg.append(reinterpret_cast<char const*>(message.get())
					, reinterpret_cast<char const*>(message.get()) + chars);
			return msg;
		}

		return std::string(user_msg);
	}
private:
	/** win32 error code*/
	unsigned long	error_code_;
};
///////////////////////////////////////////////////////////////////////////////
/** forward declaration*/
class filecache;
class mmfcache;
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
/** generate unique file in the %TEMP% folder*/
inline std::string generate_file_name()
{
	char fname[MAX_PATH] = { 0 };
	char const* prefix = "cch~";
	int const prefix_length = 4;
	DWORD sz = GetTempPathA(MAX_PATH, fname);
	fname[sz] = '\0';

	// if temp dir is absent, get application's trace root then
	if(INVALID_FILE_ATTRIBUTES == ::GetFileAttributesA(fname))
	{
		HKEY env_key;
		bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;
		static CHAR sDataRegPath[] = {VER_PRODUCT_REGISTRY_PATH "\\environment"};
		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, sDataRegPath, 0, KEY_READ | (bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0), &env_key))
		{
			DWORD type;
			sz = MAX_PATH;
			RegQueryValueExA(env_key, "TraceRoot", 0, &type, (LPBYTE)fname, &sz );
			RegCloseKey(env_key);
		}
	}

	strcat_s(fname, _countof(fname), prefix);
	LARGE_INTEGER u;
	QueryPerformanceCounter(&u);
	_i64toa(u.QuadPart, fname + sz + prefix_length, 16);
	strcat_s(fname, _countof(fname), ".htp");
	return std::string(fname, fname + strlen(fname));
}
///////////////////////////////////////////////////////////////////////////////
/** wrapper holder for Memory Mapped Object*/
typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &::CloseHandle, 0, 0> memmap_handle;
/** wrapper older for FIle Object*/
typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &::CloseHandle, 1, -1> file_handle;
/** wrapper holder for Memory Object*/
typedef util::scoped_handle<char*, BOOL (WINAPI*)(void const*), &::UnmapViewOfFile, 0, 0> memmap_ptr;
///////////////////////////////////////////////////////////////////////////////
/**
 * реализация буфера(кеша), построенного на файле.
 * поддерживает операции чтения/модификации и добавления в конец буфера
 */
class filecache_impl
		: private boost::noncopyable
{
	friend struct filecache_accessor;
	friend struct append_accessor;
public:
	explicit filecache_impl(std::string const& fname)
		: file_(::CreateFileA(fname.c_str()
							, GENERIC_READ | GENERIC_WRITE
							, FILE_SHARE_READ, 0, CREATE_ALWAYS
							, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY
							  /* FILE_FLAG_SEQUENTIAL_SCAN */ | FILE_FLAG_DELETE_ON_CLOSE
							, 0))
	{
		unsigned long error;
		if(file_.get() == INVALID_HANDLE_VALUE)
		{
			error = ::GetLastError();
			throw cache::win32_error(error, AT);
		}
		if(::SetFilePointer(file_.get(), 0, 0, FILE_END) == INVALID_SET_FILE_POINTER)
		{
			error = ::GetLastError();
			throw cache::win32_error(error, AT);
		}
	}
	~filecache_impl()
	{
		::SetEndOfFile(file_.get());
	}
	size_t capacity() const { return ::GetFileSize(file_.get(), 0); }
	size_t size() const { return this->capacity(); }
private:
	size_t append(char const* beg, char const* end)
	{
		unsigned long written(0);
		unsigned long error(0);
		if(!::WriteFile(file_.get(), beg, end - beg, &written, 0) && (error = ::GetLastError()))
			throw cache::win32_error(error, AT);
		return written;
	}
private:
	file_handle	file_;
};
///////////////////////////////////////////////////////////////////////////////
struct append_accessor
{
	friend class cache::filecache;
	friend class cache::mmfcache;

private:
	template<typename T>
	static size_t do_append(T& obj, char const* beg, char const* end)
	{
		return obj.append(beg, end);
	}
};
///////////////////////////////////////////////////////////////////////////////
struct erase_accessor
{
	friend class cache::filecache;
	friend class cache::mmfcache;

private:
	template<typename T>
	static void do_erase(T& obj, char* beg, char* end)
	{
		obj.erase(beg, end);
	}
	template<typename T>
	static void do_erase(T& obj, size_t beg, size_t end)
	{
		obj.erase(beg, end);
	}
};
struct resize_accessor
{
	friend class cache::filecache;
	friend class cache::mmfcache;
private:
	template<typename T>
	static void do_resize(T& obj, size_t size)
	{
		obj.resize(size);
	}
};
///////////////////////////////////////////////////////////////////////////////
struct filecache_accessor
{
	friend class mmfcache_impl;
private:
	static HANDLE do_handle(filecache_impl const& f) { return f.file_.get(); }
};
///////////////////////////////////////////////////////////////////////////////
class mmfcache_impl
		: private boost::noncopyable
{
	friend struct mmfcache_accessor;
	friend struct append_accessor;
	friend struct erase_accessor;
	friend struct resize_accessor;
	enum { grow_size = 0x1000 };
public:
	typedef char			value_type;
	typedef char&			reference;
	typedef char const&		const_reference;
	typedef char*			pointer;
	typedef char const*		const_pointer;
	typedef pointer			iterator;
	typedef pointer			iterator;
	typedef const_pointer	const_iterator;
//	typedef reverse_iterator<const_iterator, value_type, const_reference, const_pointer, difference_type> const_reverse_iterator;
//	typedef reverse_iterator<iterator, value_type, reference, pointer, difference_type> reverse_iterator;

	explicit mmfcache_impl(size_t size)
		: fcache_(new filecache_impl(detail::generate_file_name()))
	{
		unsigned long error;
		memmap_handle mem_map(::CreateFileMapping(filecache_accessor::do_handle(*fcache_), 0, PAGE_READWRITE, 0, size, 0));
		if(!mem_map.get())
		{
			error = ::GetLastError();
			throw cache::win32_error(error, AT);
		}
		base_ptr_.reset(static_cast<char*>(::MapViewOfFile(mem_map.get(), FILE_MAP_WRITE, 0, 0, size)));
		if(!base_ptr_.get())
		{
			error = ::GetLastError();
			throw cache::win32_error(error, AT);
		}
		first_ = base_ptr_.get();
		last_ = first_;
		end_ = first_ + size;
	}
	~mmfcache_impl()
	{
	}
	iterator begin() { return first_; }
	iterator end() { return last_; }
	const_iterator begin() const { return first_; }
	const_iterator end() const { return last_; }
	reference operator[](int index) { return first_[index]; }
	const_reference operator[](int index) const { return first_[index]; }
	size_t capacity() const { return end_ - first_; }
	size_t size() const { return last_ - first_; }
	bool empty() const { return this->size() == 0; }
private:
	/** создает в памяти отображение файла по смещениею offset и размером size*/
	mmfcache_impl(boost::shared_ptr<filecache_impl> fcache, size_t offset, size_t size)
		: fcache_(fcache)
	{
		unsigned long error;
		SYSTEM_INFO sinfo;
		GetSystemInfo(&sinfo);
		/** mapping offset*/
		size_t map_offset = (offset / sinfo.dwAllocationGranularity) * sinfo.dwAllocationGranularity;
		size_t map_size =  size + offset % sinfo.dwAllocationGranularity;
		/** create map object */
		memmap_handle mem_map(::CreateFileMapping(filecache_accessor::do_handle(*fcache_)
												, 0, PAGE_READWRITE
												, 0, offset + size, 0));
		if(!mem_map.get())
		{
			error = ::GetLastError();
			throw cache::win32_error(error, AT);
		}
		base_ptr_.reset(static_cast<char*>(::MapViewOfFile(mem_map.get()
														, FILE_MAP_WRITE
														, 0, map_offset, map_size)));
		if(!base_ptr_.get())
		{
			error = ::GetLastError();
			throw cache::win32_error(error, AT);
		}
		first_ = base_ptr_.get() + (offset - map_offset);
		last_ = first_ + size;
		end_ = first_ + size;
	}
	void resize(size_t size)
	{
		if((size_t)(end_ - first_) < size)
		{
			unsigned long error;
			base_ptr_.reset(0);
			size_t prev_size = this->size();
			size_t alloc_size = size + (grow_size -(size % grow_size));
			if(SetFilePointer(filecache_accessor::do_handle(*fcache_), alloc_size, 0, FILE_END) == INVALID_SET_FILE_POINTER)
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			if(!SetEndOfFile(filecache_accessor::do_handle(*fcache_)))
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			memmap_handle mem_map(::CreateFileMapping(filecache_accessor::do_handle(*fcache_), 0, PAGE_READWRITE, 0, fcache_->capacity(), 0));
			if(!mem_map.get())
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			base_ptr_.reset(static_cast<char*>(::MapViewOfFile(mem_map.get(), FILE_MAP_WRITE, 0, 0, fcache_->capacity())));
			if(!base_ptr_.get())
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			first_ = base_ptr_.get();
			last_ = first_ + size;
			end_ = first_ + fcache_->capacity();
		}
		else if(0 < size)
		{
			first_ = base_ptr_.get();
			last_ = first_ + size;
			end_ = first_ + fcache_->capacity();
		}
	}
	size_t append(char const* beg, char const* end)
	{
		size_t size = end - beg;
		if((size_t)(end_ - last_) < size)
		{
			unsigned long error;
			base_ptr_.reset(0);
			size_t prev_size = this->size();
			size_t alloc_size = size + (grow_size -(size % grow_size));
			if(SetFilePointer(filecache_accessor::do_handle(*fcache_), alloc_size, 0, FILE_END) == INVALID_SET_FILE_POINTER)
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			if(!SetEndOfFile(filecache_accessor::do_handle(*fcache_)))
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			memmap_handle mem_map(::CreateFileMapping(filecache_accessor::do_handle(*fcache_), 0, PAGE_READWRITE, 0, fcache_->capacity(), 0));
			if(!mem_map.get())
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			base_ptr_.reset(static_cast<char*>(::MapViewOfFile(mem_map.get(), FILE_MAP_WRITE, 0, 0, fcache_->capacity())));
			if(!base_ptr_.get())
			{
				error = ::GetLastError();
				throw cache::win32_error(error, AT);
			}
			first_ = base_ptr_.get();
			last_ = first_ + prev_size + size;
			end_ = first_ + fcache_->capacity();
			memcpy(first_ + prev_size, beg, size);
		}
		else if(0 < size)
		{
			memcpy(last_, beg, size);
			last_ += size;
		}
		return size;
	}
	void erase(iterator beg, iterator end)
	{
		if(0 < (last_ - end))
			memmove(beg, end, last_ - end);
		last_ -= (end - beg);
	}
private:
	boost::shared_ptr<filecache_impl>	fcache_;
	memmap_ptr							base_ptr_;
	char*								first_;
	char*								last_;
	char*								end_;
};
///////////////////////////////////////////////////////////////////////////////
struct mmfcache_accessor
{
	friend class filecache;
private:
	static boost::shared_ptr<mmfcache_impl> create(boost::shared_ptr<filecache_impl> f, size_t offset, size_t size)
	{
		return boost::shared_ptr<mmfcache_impl>(new mmfcache_impl(f, offset, size));
	}
};
}//namesapce detail
///////////////////////////////////////////////////////////////////////////////
class mmfcache
{
public:
	typedef detail::mmfcache_impl::value_type		value_type;
	typedef detail::mmfcache_impl::reference		reference;
	typedef detail::mmfcache_impl::const_reference	const_reference;
	typedef detail::mmfcache_impl::pointer			pointer;
	typedef detail::mmfcache_impl::const_pointer	const_pointer;
	typedef detail::mmfcache_impl::iterator			iterator;
	typedef detail::mmfcache_impl::const_iterator	const_iterator;

	explicit mmfcache(size_t size)
		: impl_(new detail::mmfcache_impl(size))
	{
	}
	explicit mmfcache(boost::shared_ptr<detail::mmfcache_impl> impl)
		: impl_(impl)
	{
	}
	iterator begin() { return impl_->begin(); }
	iterator end() { return impl_->end(); }
	const_iterator begin() const { return impl_->begin(); }
	const_iterator end() const { return impl_->end(); }
	reference operator[](int index) { return (*impl_)[index]; }
	const_reference operator[](int index) const { return (*impl_)[index]; }
	size_t capacity() const { return impl_->capacity(); }
	size_t size() const { return impl_->size(); }
	bool empty() const { return impl_->empty(); }
protected:
	size_t append(char const* beg, char const* end)
	{
		return detail::append_accessor::do_append(*impl_, beg, end);
	}
	void erase(char* beg, char* end)
	{
		detail::erase_accessor::do_erase(*impl_, beg, end);
	}
	void resize(size_t size)
	{
		detail::resize_accessor::do_resize(*impl_, size);
	}
private:
	boost::shared_ptr<detail::mmfcache_impl> impl_;
};
///////////////////////////////////////////////////////////////////////////////
/***/
class filecache
{
public:
	filecache()
		: impl_(new detail::filecache_impl(detail::generate_file_name()))
	{
	}
	explicit filecache(std::string const& fname)
		: impl_(new detail::filecache_impl(fname))
	{
	}
	mmfcache at_memory(size_t offset, size_t size)
	{
		return mmfcache(detail::mmfcache_accessor::create(impl_, offset, size));
	}
	size_t capacity() const { return impl_->capacity(); }
	size_t size() const { return impl_->size(); }
protected:
	size_t append(char const* beg, char const* end)
	{
		return detail::append_accessor::do_append(*impl_, beg, end);
	}
	void erase(size_t beg_pos, size_t end_pos)
	{
		throw std::runtime_error("filecache::erase() not implemented");
	}
private:
	boost::shared_ptr<detail::filecache_impl> impl_;
};
///////////////////////////////////////////////////////////////////////////////
/** расширяет открытый интерфейс кеша, открывая чать*/
template<typename Cache>
class growable
		: public Cache
{
public:

	template<typename P>
	growable(P const& p)
			: Cache(p)
	{
	}
	size_t append(char const* beg, char const* end)
	{
		return Cache::append(beg, end);
	}
	template<typename It>
	void erase(It beg, It end)
	{
		Cache::erase(beg, end);
	}
	void resize(size_t size)
	{
		Cache::resize(size);
	}
};
///////////////////////////////////////////////////////////////////////////////
}//namespace cache
///////////////////////////////////////////////////////////////////////////////
#endif