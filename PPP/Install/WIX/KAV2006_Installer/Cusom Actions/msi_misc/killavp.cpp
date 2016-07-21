#include <windows.h>
#include <map>
#include <vector>
#include <algorithm>
#include <locale>
#include <winperf.h>
#include <tlhelp32.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctype.h>
#include <iterator>
#include <mklif/fssync/fssync.h>
#include <msi.h>
#include "stdafx.h"
#include "msi_misc.h"

std::string lexical_cast(int i)
{
	char buf[128];
	_itoa(i, buf, 10);
	return buf;
}

std::string conv_to_char(wchar_t const* s)
{
	const size_t bytes = ::WideCharToMultiByte(CP_ACP, 0, s, -1, 0, 0, 0, 0);
	std::vector<char> v(1 + bytes);
	if (0 == bytes
		|| 0 == ::WideCharToMultiByte(CP_ACP, 0, s, -1, &v[0], v.size(), 0, 0))
		throw std::runtime_error("conv_to_char called wcstombs() and failed.");
	return &v[0];
}

///////////////////////////////////////////////////////////////////////////////
namespace util
{
///////////////////////////////////////////////////////////////////////////////
template<typename T>
struct static_array
{
	typedef T              value_type;
	typedef T*             iterator;
	typedef const T*       const_iterator;
	typedef T&             reference;
	typedef const T&       const_reference;
	typedef size_t         size_type;
	typedef ptrdiff_t      difference_type;

#if _MSC_VER > 1200
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#else
	typedef std::reverse_iterator<iterator,T> reverse_iterator;
	typedef std::reverse_iterator<const_iterator,T> const_reverse_iterator;
#endif

	static_array(T* p, size_t size)
		:	elems_(p)
		,	size_(size)
	{
	}

	iterator begin() { return elems_; }
	const_iterator begin() const { return elems_; }
	iterator end() { return elems_ + size_; }
	const_iterator end() const { return elems_ + size_; }
	
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}
	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	reference front() { return elems_[0]; }
	const_reference front() const { return elems_[0]; }
	reference back() { return elems_[size_ - 1]; }
	const_reference back() const { return elems_[size_ - 1]; }

	size_type size() { return size_; }
	bool empty() { return false; }
	size_type max_size() { return size_; }

private:
	T*			elems_;
	size_t		size_;
};

///////////////////////////////////////////////////////////////////////////////
/**
* \struct	scoped_handle
* \brief	...
*/
template
	<
	typename Handle,
	typename Pfn,
	Pfn close,
	int Unique,
	int null = 0
	>
	class scoped_handle
{
public:
	scoped_handle()
		:	h_((Handle)null)
	{}
	
	explicit scoped_handle(Handle h)
		:	h_(h)
	{}
	
	~scoped_handle()
	{
		if (h_ != (Handle)null)
			close(h_);
	}
	
	Handle& get_ref()
	{
		return h_;
	}
	
	Handle get() const
	{
		return h_;
	}
	
	void reset(Handle h)
	{
		scoped_handle(h).swap(*this);
	}
	
	Handle release()
	{
		Handle t(h_);
		h_ = (Handle)null;
		return t;
	}
	
	void swap(scoped_handle& other)
	{
		std::swap(h_, other.h_);
	}
	
private:
	typedef Handle(scoped_handle::*unspecified_bool_type)();
	scoped_handle(scoped_handle const&);
	scoped_handle& operator=(scoped_handle const&);
	
public:
	operator unspecified_bool_type() const
	{
		return (Handle)null == h_ ? false : &scoped_handle::release;
	}
	
private:
	Handle h_;
};
}// namespace util

///////////////////////////////////////////////////////////////////////////////
namespace winapi
{
typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &::CloseHandle, 0> handle_t;
typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &::CloseHandle, -1> filehandle_t;
typedef util::scoped_handle<HMODULE, BOOL (WINAPI*)(HMODULE), &::FreeLibrary, 0> modulehandle_t;
typedef util::scoped_handle<HKEY, long (WINAPI*)(HKEY),&::RegCloseKey, 0>  handle_key;
///////////////////////////////////////////////////////////////////////////////
enum open_mode
{
	read	= KEY_READ,
	write	= KEY_WRITE
};
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
inline void output_winapi_error(unsigned long error, char const* descr)
{
	char buf[10] = { 0 };
	//std::cerr << "error: winapi(" << error << ") '"	<< descr << "'\n";
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
struct match_key
{
	match_key(std::basic_string<_E> const& str)
		: str_(str)
	{
	}
	bool operator()(std::pair<_E const*, HKEY> const& key) const
	{
		return str_.find(key.first) != std::basic_string<_E>::npos;
	}
private:
	std::basic_string<_E>	const& str_;
};
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
struct regkey_defs
{
	typedef std::pair<_E const*, HKEY> regkey_value;
	typedef util::static_array<regkey_value> regkey_array;
};
///////////////////////////////////////////////////////////////////////////////
template<typename _E> struct regkey_traits;
///////////////////////////////////////////////////////////////////////////////
template<> struct regkey_traits<char>
{
	typedef regkey_defs<char>::regkey_value regkey_value;
	typedef regkey_defs<char>::regkey_array regkey_array;
	static char const* classes_root() { return "HKEY_CLASSES_ROOT"; }
	static char const* current_config() { return "HKEY_CURRENT_CONFIG"; }
	static char const* current_user() { return "HKEY_CURRENT_USER"; }
	static char const* local_machine() { return "HKEY_LOCAL_MACHINE"; }
	static char const* users() { return "HKEY_USERS"; }
	static char const* shared_dlls() { return "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"; }
};
///////////////////////////////////////////////////////////////////////////////
template<> struct regkey_traits<wchar_t>
{
	typedef regkey_defs<wchar_t>::regkey_value regkey_value;
	typedef regkey_defs<wchar_t>::regkey_array regkey_array;
	static wchar_t const* classes_root() { return L"HKEY_CLASSES_ROOT"; }
	static wchar_t const* current_config() { return L"HKEY_CURRENT_CONFIG"; }
	static wchar_t const* current_user() { return L"HKEY_CURRENT_USER"; }
	static wchar_t const* local_machine() { return L"HKEY_LOCAL_MACHINE"; }
	static wchar_t const* users() { return L"HKEY_USERS"; }
	static wchar_t const* shared_dlls() { return L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"; }
};
///////////////////////////////////////////////////////////////////////////////
inline long RegOpenKeyEx_helper(HKEY h, char const* key
								, unsigned long r, REGSAM s, HKEY* p, bool x64)
{
	return RegOpenKeyExA(h, key, r, s | (x64 ? KEY_WOW64_64KEY : 0), p);
}
///////////////////////////////////////////////////////////////////////////////
inline long RegOpenKeyEx_helper(HKEY h, wchar_t const* key
								, unsigned long r, REGSAM s, HKEY* p, bool x64)
{
	return RegOpenKeyExW(h, key, r, s | (x64 ? KEY_WOW64_64KEY : 0), p);
}
///////////////////////////////////////////////////////////////////////////////
inline LONG RegDeleteKey64(HKEY hKey, LPCSTR lpSubKey)
{
	typedef LONG (WINAPI *pfnRegDeleteKeyExA)(HKEY hKey, LPCSTR lpSubKey, REGSAM samDesired, DWORD Reserved);
	static pfnRegDeleteKeyExA fnRegDeleteKeyExA = NULL;
	if (!fnRegDeleteKeyExA)
	{
		HMODULE advapi32 = GetModuleHandle("Advapi32.dll");
		if (!advapi32)
			return ERROR_NOT_SUPPORTED;
		fnRegDeleteKeyExA = (pfnRegDeleteKeyExA) GetProcAddress(advapi32, "RegDeleteKeyExA");
		if (!fnRegDeleteKeyExA)
			return ERROR_NOT_SUPPORTED;
	}
	return fnRegDeleteKeyExA(hKey, lpSubKey, KEY_WOW64_64KEY, 0);
}
///////////////////////////////////////////////////////////////////////////////
inline LONG RegDeleteKey64(HKEY hKey, LPCWSTR lpSubKey)
{
	typedef LONG (WINAPI *pfnRegDeleteKeyExW)(HKEY hKey, LPCWSTR lpSubKey, REGSAM samDesired, DWORD Reserved);
	static pfnRegDeleteKeyExW fnRegDeleteKeyExW = NULL;
	if (!fnRegDeleteKeyExW)
	{
		HMODULE advapi32 = GetModuleHandle("Advapi32.dll");
		if (!advapi32)
			return ERROR_NOT_SUPPORTED;
		fnRegDeleteKeyExW = (pfnRegDeleteKeyExW) GetProcAddress(advapi32, "RegDeleteKeyExW");
		if (!fnRegDeleteKeyExW)
			return ERROR_NOT_SUPPORTED;
	}
	return fnRegDeleteKeyExW(hKey, lpSubKey, KEY_WOW64_64KEY, 0);
}
///////////////////////////////////////////////////////////////////////////////
inline LONG delete_regkey_helper(HKEY h, char const* key, bool x64)
{
	LONG nRet = ERROR_SUCCESS;
	handle_key hkey;
	if(ERROR_SUCCESS == RegOpenKeyExA(h, key, 0, KEY_READ | KEY_WRITE | (x64 ? KEY_WOW64_64KEY : 0), &hkey.get_ref()))
	{
		std::vector<char> key_name(MAX_PATH + 1, 0);
		int nIndex = 0;
		while(ERROR_SUCCESS == RegEnumKeyA(hkey.get(), nIndex, &key_name[0], MAX_PATH))
		{
			if (delete_regkey_helper(hkey.get(), &key_name[0], x64) != ERROR_SUCCESS)
				++nIndex;
		}

		hkey.reset(0);
		nRet = x64 ? RegDeleteKey64(h, key) : RegDeleteKeyA(h, key);
		if(ERROR_SUCCESS != nRet)	//may be need throw exception
			output_winapi_error(::GetLastError(), "delete_regkey_helper can not delete key");
	}

	return nRet;
}
///////////////////////////////////////////////////////////////////////////////
inline LONG delete_regkey_if_empty(HKEY h, char const* key, bool x64)
{
	handle_key hkey;
	LONG nRet = RegOpenKeyExA(h, key, 0, KEY_READ | KEY_WRITE | (x64 ? KEY_WOW64_64KEY : 0), &hkey.get_ref());
	if (ERROR_SUCCESS != nRet)
		return nRet;

	DWORD dwSubKeys, dwValuesCount;
	nRet = RegQueryInfoKey(hkey.get(), NULL, NULL, NULL, &dwSubKeys, NULL, NULL, &dwValuesCount, NULL, NULL, NULL, NULL);
	if (ERROR_SUCCESS != nRet)
		return nRet;

	if (dwSubKeys != 0 || dwValuesCount != 0)
		return ERROR_SUCCESS;

	hkey.reset(0);
	nRet = x64 ? RegDeleteKey64(h, key) : RegDeleteKeyA(h, key);	
	if (ERROR_SUCCESS != nRet)	//may be need throw exception
		output_winapi_error(::GetLastError(), "delete_regkey_if_empty can not delete key");
	
	return nRet;
}
///////////////////////////////////////////////////////////////////////////////
inline LONG delete_regkey_helper(HKEY h, wchar_t const* key, bool x64)
{
	LONG nRet = ERROR_SUCCESS;
	handle_key hkey;
	if(ERROR_SUCCESS == RegOpenKeyExW(h, key, 0, KEY_READ | KEY_WRITE | (x64 ? KEY_WOW64_64KEY : 0), &hkey.get_ref()))
	{
		std::vector<wchar_t> key_name(MAX_PATH + 1, 0);
		int nIndex = 0;
		while(ERROR_SUCCESS == RegEnumKeyW(hkey.get(), 0, &key_name[0], MAX_PATH))
		{
			if (delete_regkey_helper(hkey.get(), &key_name[0], x64) != ERROR_SUCCESS)
				++nIndex;
		}

		hkey.reset(0);
		nRet = x64 ? RegDeleteKey64(h, key) : RegDeleteKeyW(h, key);
		if(ERROR_SUCCESS != nRet)	//may be need throw exception
			output_winapi_error(::GetLastError(), "delete_regkey_helper can not delete key");
	}
}
///////////////////////////////////////////////////////////////////////////////
inline long RegQueryValueEx_helper(HKEY h, char const* n, unsigned long* r
								, unsigned long* t, unsigned char* b
								, unsigned long* sz)
{
	return RegQueryValueExA(h, n, r, t, b, sz);
}
///////////////////////////////////////////////////////////////////////////////
inline long RegQueryValueEx_helper(HKEY h, wchar_t const* n, unsigned long* r
								, unsigned long* t, unsigned char* b
								, unsigned long* sz)
{
	return RegQueryValueExW(h, n, r, t, b, sz);
}
///////////////////////////////////////////////////////////////////////////////
inline long RegDeleteValue_helper(HKEY h, char const* v)
{
	return RegDeleteValueA(h, v);
}
///////////////////////////////////////////////////////////////////////////////
inline long RegDeleteValue_helper(HKEY h, wchar_t const* v)
{
	return RegDeleteValueW(h, v);
}
///////////////////////////////////////////////////////////////////////////////
inline long RegSetValueEx_helper(HKEY h, char const* n
								 , unsigned long r
								 , unsigned long t
								 , const void* b
								 , size_t sz)
{
	return RegSetValueExA(h, n, r, t, (LPCBYTE)b, sz);
}
///////////////////////////////////////////////////////////////////////////////
inline long RegSetValueEx_helper(HKEY h, wchar_t const* n
								 , unsigned long r
								 , unsigned long t
								 , const void* b
								 , size_t sz)
{
	return RegSetValueExW(h, n, r, t, (LPCBYTE) b, sz);
}
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
//!return array of pair registry name & handle of registry
template<typename _E>
inline typename detail::regkey_defs<_E>::regkey_array
get_regkey_map()
{
	typedef typename detail::regkey_defs<_E>::regkey_value regkey_value;
	typedef typename detail::regkey_defs<_E>::regkey_array regkey_array;
	static regkey_value keys[] = 
	{
		std::make_pair(detail::regkey_traits<_E>::classes_root(), HKEY_CLASSES_ROOT),
		std::make_pair(detail::regkey_traits<_E>::current_config(), HKEY_CURRENT_CONFIG),
		std::make_pair(detail::regkey_traits<_E>::current_user(), HKEY_CURRENT_USER),
		std::make_pair(detail::regkey_traits<_E>::local_machine(), HKEY_LOCAL_MACHINE),
		std::make_pair(detail::regkey_traits<_E>::users(), HKEY_USERS)
	};
	return regkey_array(keys, sizeof(keys) /sizeof(regkey_value));
};
///////////////////////////////////////////////////////////////////////////////
//!verify key return true if key exists
template<typename _E>
inline bool verify_regkey(HKEY h, _E const* key, bool x64)
{
	handle_key hkey;
	return ERROR_SUCCESS == detail::RegOpenKeyEx_helper(h, key, 0, KEY_READ, &hkey.get_ref(), x64);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline bool verify_regkey(HKEY h, std::basic_string<_E> const& key, bool x64)
{
	return verify_regkey(h, key.c_str(), x64);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline void delete_regkey(HKEY h, _E const* key, bool empty_only, bool x64)
{
	if (empty_only)
		detail::delete_regkey_if_empty(h, key, x64);
	else
		detail::delete_regkey_helper(h, key, x64);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline void delete_regkey(HKEY h, std::basic_string<_E> const& key, bool empty_only, bool x64)
{
	delete_regkey(h, key.c_str(), empty_only, x64);
}
template<typename _E>
inline void delete_regkey_value(HKEY h, _E const* key, _E const* name, bool x64)
{
	handle_key hkey;
	if(ERROR_SUCCESS == RegOpenKeyExA(h, key, 0, KEY_READ | KEY_WRITE | (x64 ? KEY_WOW64_64KEY : 0), &hkey.get_ref()))
	{
		if(ERROR_SUCCESS != detail::RegDeleteValue_helper(hkey.get(), name))//may be need throw exception
			detail::output_winapi_error(::GetLastError(), "can not delete regkey value");
	}
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline void delete_regkey_value(HKEY h
								, std::basic_string<_E> const& key
								, std::basic_string<_E> const& name
								, bool x64)
{
	delete_regkey_value(h, key.c_str(), name.c_str(), x64);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline std::pair<HKEY, std::basic_string<_E> > transform_regkey(std::basic_string<_E> const& regkey)
{
	typedef detail::regkey_traits<_E>::regkey_array regkey_array;
	std::basic_string<_E> key(regkey);
	regkey_array const& reg_keys = get_regkey_map<_E>();
	regkey_array::const_iterator it(std::find_if(reg_keys.begin()
												, reg_keys.end()
												, detail::match_key<_E>(key)));
	if(it != reg_keys.end())//found registry key
	{
		key.erase(0, strlen(it->first) + 1);
		return std::make_pair(it->second, key);
	}
	return std::make_pair(reinterpret_cast<HKEY>(0), key);
}
///////////////////////////////////////////////////////////////////////////////
//! can throw exception
template<typename _E>
inline HKEY open_regkey(HKEY h, std::basic_string<_E> const& key, open_mode mode, bool x64)
{
	handle_key hkey;
	if(ERROR_SUCCESS != detail::RegOpenKeyEx_helper(h, key.c_str(), 0, mode, &hkey.get_ref(), x64))
		throw std::runtime_error("invalid registry key or insufficient access right");
	return hkey.release();
}
template<typename _E>
inline HKEY open_regkey(std::basic_string<_E> const& key, open_mode mode, bool x64)
{
	std::pair<HKEY, std::basic_string<_E> > const& tk = transform_regkey(key);
	return open_regkey(tk.first, tk.second, mode, x64);
}
///////////////////////////////////////////////////////////////////////////////
//!retun string value from registry key
//TODO: возможно нужно выбрасывать исключение !!!
template<typename _E>
inline std::basic_string<_E> query_regkey_value(handle_key const& h, _E const* name)
{
	unsigned long type(0);
	unsigned long size(0);
	std::basic_string<_E> result;
	if(ERROR_SUCCESS == detail::RegQueryValueEx_helper(h.get(), name, 0, &type, 0, &size))
	{
		std::vector<_E> buf(size + 1, 0);
		if(ERROR_SUCCESS == detail::RegQueryValueEx_helper(h.get(), name
														, 0, &type
														, reinterpret_cast<unsigned char*>(&buf[0])
														, &size))
			result.append(&buf[0]);
	}
	return result;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline std::basic_string<_E> query_regkey_value(handle_key const& h, std::basic_string<_E> const& name)
{
	return query_regkey_value(h, name.c_str());
}
///////////////////////////////////////////////////////////////////////////////
//!read any data to buffer
template<typename _E>
inline std::auto_ptr<std::vector<_E> > query_regkey_value_ex(handle_key const& h, _E const* name, unsigned long* vtype = 0)
{
	unsigned long type(0);
	unsigned long size(0);
	std::auto_ptr<std::vector<_E> > result(new std::vector<_E>);
	if(ERROR_SUCCESS == detail::RegQueryValueEx_helper(h.get(), name, 0, &type, 0, &size))
	{
		std::vector<_E> buf(size + 1, 0);
		if(ERROR_SUCCESS == detail::RegQueryValueEx_helper(h.get(), name
														, 0, &type
														, reinterpret_cast<unsigned char*>(&buf[0])
														, &size))
			result->swap(buf);
	}
	if(vtype)
		*vtype = type;
	return result;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline std::auto_ptr<std::vector<_E> > query_regkey_value_ex(handle_key const& h
															, std::basic_string<_E> const& name
															, unsigned long* vtype = 0)
{
	return query_regkey_value_ex(h, name.c_str(), vtype);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline size_t query_dll_refcount(_E const* name, bool x64)
{
	size_t refcount(1);
	handle_key hkey;
	if(ERROR_SUCCESS == detail::RegOpenKeyEx_helper(HKEY_LOCAL_MACHINE
												, detail::regkey_traits<_E>::shared_dlls()
												, 0
												, read
												, &hkey.get_ref()
												, x64))
	{
		std::auto_ptr<std::vector<_E> > buffer = query_regkey_value_ex(hkey, name);
		if(!buffer->empty())
			refcount = *reinterpret_cast<size_t*>(&(*buffer)[0]);
	}
	return refcount;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline size_t decriment_dll_refcount(_E const* name, bool x64)
{
	size_t refcount(1);
	handle_key hkey;
	if(ERROR_SUCCESS == detail::RegOpenKeyEx_helper(HKEY_LOCAL_MACHINE
												, detail::regkey_traits<_E>::shared_dlls()
												, 0
												, read | write
												, &hkey.get_ref()
												, x64))
	{
		std::auto_ptr<std::vector<_E> > buffer = query_regkey_value_ex(hkey, name);
		if(!buffer->empty())
			refcount = *reinterpret_cast<size_t*>(&(*buffer)[0]);
		--refcount;
		if(refcount == 0)
			detail::RegDeleteValue_helper(hkey.get(), name);
		else
		{
			detail::RegSetValueEx_helper(hkey.get()
										, name
										, 0
										, REG_DWORD
										, reinterpret_cast<unsigned char*>(&refcount)
										, sizeof(refcount));
		}
	}
	return refcount;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline bool write_regkey_value(HKEY key, _E const* name, _E const* value, _E const* data, int data_len, bool bx64)
{
	handle_key hkey;
	if(ERROR_SUCCESS != detail::RegOpenKeyEx_helper(key
		, name
		, 0
		, read | write
		, &hkey.get_ref()
		, bx64))
		return false;

	detail::RegSetValueEx_helper(hkey.get()
			, value
			, 0
			, REG_SZ
			, data
			, data_len);

	return true;
}
///////////////////////////////////////////////////////////////////////////////
}//namespace winapi
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace winapi
{
///////////////////////////////////////////////////////////////////////////////
inline bool is_Nt_platform()
{
	OSVERSIONINFO vi = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&vi);
	return VER_PLATFORM_WIN32_WINDOWS == vi.dwPlatformId
		? false
		: true;
}


///////////////////////////////////////////////////////////////////////////////
namespace perf
{
///////////////////////////////////////////////////////////////////////////////
struct _less
{
	bool operator()(std::string const& lhs, std::string const& rhs) const
	{
		return _strcmpi(lhs.c_str(), rhs.c_str()) < 0;
	}
};
///////////////////////////////////////////////////////////////////////////////
//!return all couters <counter name, counter id>
inline std::auto_ptr<std::map<std::string, unsigned long, _less> > counters_info()
{
	using namespace std;
	typedef std::map<std::string, unsigned long, _less> counter_map;
	std::auto_ptr<counter_map> result(new counter_map);
	stringstream s("software\\microsoft\\windows nt\\currentversion\\perflib\\");
	s.seekp(0, std::ios::end);
	s << std::setfill('0') << std::setw(3)
		<< std::hex << MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL);
	handle_key hkey(open_regkey(HKEY_LOCAL_MACHINE, s.str(), read, false));	
	auto_ptr<vector<char> > counters(query_regkey_value_ex(hkey, "Counters"));
	vector<char>::size_type size(counters->size());
	char const* p = &(*counters)[0];
	if(*p)//skip record with number of counters
	{
		p += strlen(p) + 1;
		if(*p)
			p += strlen(p) + 1;
	}
	while(*p)
	{
		char const* cid = p;//store pointer to id counter name
		p += strlen(p) + 1;//!take a pointer to counters name
		if(*p)
		{
			result->insert(make_pair(std::string(p), atol(cid)));
			p += strlen(p) + 1;
		}
	}
	return result;
}
template<typename _E>
inline std::auto_ptr<std::vector<char> > perfomance_data_block(std::basic_string<_E> const& counter_name)
{
	using namespace std;
	unsigned long error(ERROR_MORE_DATA);
	unsigned long type(0);
	unsigned long size(0);
	auto_ptr<vector<char> > block(new vector<char>(51200, 0));
	while(error != ERROR_SUCCESS)
	{
		size = block->size();
		error = winapi::detail::RegQueryValueEx_helper(HKEY_PERFORMANCE_DATA
							, counter_name.c_str()
							, 0, &type, reinterpret_cast<unsigned char*>(&(*block)[0]), &size);
		if(error == ERROR_MORE_DATA)
			block->resize(block->size() + block->size() / 2);
		else if(error != ERROR_SUCCESS)
			throw std::runtime_error("could not read perfomance counter block");
	}
	return block;
}
///////////////////////////////////////////////////////////////////////////////
}//namespace perf
///////////////////////////////////////////////////////////////////////////////
const int max_process_name = 32;
const int max_window_title = 64;
//! struct describes a processt 
struct task_item
{
	unsigned long	pid;				//process idetnifier
	HWND		hwnd;				//handle of the process main window
	char		pname[max_process_name];	//process name, only max_process_name bytes & single zero byte
	char		wtitle[max_window_title];	//title of the process main window
};
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI *fn_process_walk)(HANDLE h, LPPROCESSENTRY32 pe);
typedef HANDLE (WINAPI *fn_create_snapshot)(unsigned long flags, unsigned long pid);
///////////////////////////////////////////////////////////////////////////////
inline std::auto_ptr<std::vector<task_item> > task_win9x_list()
{
	fn_process_walk		_process_first = 0;
	fn_process_walk		_process_next = 0;
	fn_create_snapshot	_create_snapshot = 0;
	PROCESSENTRY32		pe = { sizeof(PROCESSENTRY32), 0 };
	HMODULE			kernel(::GetModuleHandleA("kernel32.dll"));
	
	_process_first =
		reinterpret_cast<fn_process_walk>(::GetProcAddress(kernel, "Process32First"));
	_process_next = 
		reinterpret_cast<fn_process_walk>(::GetProcAddress(kernel, "Process32Next"));
	_create_snapshot =
		reinterpret_cast<fn_create_snapshot>(::GetProcAddress(kernel, "CreateToolhelp32Snapshot"));
	if(!(_process_first && _process_next && _create_snapshot))
		throw std::runtime_error("entry point does not exist");
	std::auto_ptr<std::vector<task_item> > result(new std::vector<task_item>);
	filehandle_t	snapshot(_create_snapshot(TH32CS_SNAPPROCESS, 0));
	if(snapshot && _process_first(snapshot.get(), &pe))
		do
		{
			task_item task = { pe.th32ProcessID, 0 };
			util::static_array<char> name(pe.szExeFile, strlen(pe.szExeFile));
			util::static_array<char>::reverse_iterator it = 
									std::find(name.rbegin(), name.rend(), '\\');
			if(it != name.rend())
				strncpy(task.pname, it.base(), max_process_name);
			else
				strcpy(task.pname, "<foobar>");
			result->push_back(task);
		}while(_process_next(snapshot.get(), &pe));
	return result;
}
///////////////////////////////////////////////////////////////////////////////
//!set a new privilege for current process
inline void adjust_privilege(char const* privilege)
{
	handle_t token;
	if(!::OpenProcessToken(::GetCurrentProcess()
				, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
				, &token.get_ref()))
		throw std::runtime_error("OpenProcessToken failed");
	LUID luid;
	if (LookupPrivilegeValue(0, privilege, &luid)) 
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(token.get(), 0, &tp, sizeof(TOKEN_PRIVILEGES), 0, 0); 
		if(ERROR_SUCCESS != ::GetLastError())
			throw std::runtime_error("AdjustTokenPrivileges filed");
	}
}
//!return running processes list
//!¤«п в®Ј® зв®Ўл ­Ґ в йЁвм ­  NT4 psapi.dll зЁв Ґ¬ Їа®жҐббл Ё§ бзҐвзЁЄ®ў
//!для того чтобы не тащить на NT4 psapi.dll читаем процессы из счетчиков
inline std::auto_ptr<std::vector<task_item> > task_winnt_list()
{
	using namespace std;
	auto_ptr<vector<task_item> > result(new vector<task_item>);
	auto_ptr<map<std::string, unsigned long, perf::_less> > const& cinfo = perf::counters_info();
	typedef map<std::string, unsigned long, perf::_less>::const_iterator const_iterator;
	const_iterator it_pname(cinfo->find(string("process")));
	const_iterator it_pid(cinfo->find(string("id process")));
	if(it_pname != cinfo->end() && it_pid != cinfo->end())
	{
		auto_ptr<vector<char> > const& block = perf::perfomance_data_block(lexical_cast(it_pname->second));
		PERF_DATA_BLOCK* data_block = reinterpret_cast<PERF_DATA_BLOCK*>(&(*block)[0]);
		if(wcsncmp(data_block->Signature, L"PERF", 4) != 0)
			throw std::runtime_error("block has been read but it has not PERF signature");
		PERF_OBJECT_TYPE* object = reinterpret_cast<PERF_OBJECT_TYPE*>(reinterpret_cast<char*>(data_block)
										+ data_block->HeaderLength);
		PERF_COUNTER_DEFINITION* cdef = reinterpret_cast<PERF_COUNTER_DEFINITION*>(reinterpret_cast<char*>(object)
				+ object->HeaderLength);
		util::static_array<PERF_COUNTER_DEFINITION> cdef_array(cdef, object->NumCounters);
		util::static_array<PERF_COUNTER_DEFINITION>::iterator it;
		for (it = cdef_array.begin(); it != cdef_array.end(); ++it)
		{
			if (it->CounterNameTitleIndex == it_pid->second)
				break;
		}
		if(it != cdef_array.end())
		{
			unsigned long id_process_counter(it->CounterOffset);
			unsigned long ntask(object->NumInstances);
			result->reserve(ntask);
			PERF_INSTANCE_DEFINITION *inst = 
				reinterpret_cast<PERF_INSTANCE_DEFINITION*>(reinterpret_cast<char*>(object) + object->DefinitionLength);
			ctype<char> const& ct = use_facet<ctype<char> >(locale::classic());
			while(ntask--)
			{
				wchar_t *pname = reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(inst)
									+  inst->NameOffset);
				task_item task = { 0 };
				strncpy(task.pname, conv_to_char(pname).c_str(), max_process_name);
				ct.tolower(task.pname, task.pname + max_process_name);
				PERF_COUNTER_BLOCK* pcounter =
					reinterpret_cast<PERF_COUNTER_BLOCK*>(reinterpret_cast<char*>(inst) + inst->ByteLength);
				task.pid = *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(pcounter) + id_process_counter);
				if(!task.pid)
					task.pid = 0xfffffffe;//-2
				result->push_back(task);
				inst = reinterpret_cast<PERF_INSTANCE_DEFINITION*>(reinterpret_cast<char*>(pcounter) + pcounter->ByteLength);
			}
		}
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
//!return task list
inline std::auto_ptr<std::vector<task_item> > get_task_list()
{
	return is_Nt_platform()
	       ? detail::task_winnt_list()
	       : detail::task_win9x_list();
}
///////////////////////////////////////////////////////////////////////////////
inline bool murder_process(unsigned long pid)
{
	handle_t process(::OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, 0, pid));
	if(!process)
	{
//		DWORD dwErr = GetLastError();
//		std::cerr << "Can not open process with pid '" << pid << "' error = " << dwErr << "\n";
		return false;
	}
	if(!TerminateProcess(process.get(), 1))
	{
//		DWORD dwErr = GetLastError();
//		std::cerr << "Can not terminate process with pid '" << pid << "' error = " << dwErr << "\n";
		return false;
	}
    WaitForSingleObject(process.get(), 1000);
	return true;
}
}//namespace winapi
///////////////////////////////////////////////////////////////////////////////

class allow_terminate_process
{
public:
	allow_terminate_process () : m_ctx (0)
	{
		FSDrv_UniInit (& m_ctx);
		FSDrv_UniRegInv (m_ctx);
	}

	~allow_terminate_process ()
	{
		FSDrv_UniUnRegInv (m_ctx);
		FSDrv_UniDone (& m_ctx);
	}

private:
	void * m_ctx;
};

bool KillAvp(MSIHANDLE hInstall)
{
    installLog(hInstall, "KillAvp: entry point");
    try
	{
		if(winapi::is_Nt_platform())
			winapi::detail::adjust_privilege(SE_DEBUG_NAME);
		
        for (int pass = 0; pass < 50; ++pass)
        {
            bool avpfound = false;
		    std::auto_ptr<std::vector<winapi::task_item> > tasks = winapi::get_task_list();	
		    for (size_t i = 0; i < tasks->size(); ++i)
		    {
			    winapi::task_item& task = (*tasks)[i];
			    _strlwr(task.pname);

			    if (strcmp(task.pname, "avp") == 0
				    || strcmp(task.pname, "avp.exe") == 0
				    || strcmp(task.pname, "avp.com") == 0)
			    {
                    avpfound = true;
                    installLog(hInstall, "KillAvp: pass %d, terminating %s(%d)", pass, task.pname, task.pid);

				    allow_terminate_process guard;
				    if (! winapi::murder_process (task.pid))
                    {
                        installLog(hInstall, "KillAvp: unable to terminate %s: 0x%08X", task.pname, GetLastError());
					    return false;
                    }
			    }
		    }
            if (!avpfound)
            {
                installLog(hInstall, "KillAvp: no avp found, exiting");
                break;
            }
        }
        installLog(hInstall, "KillAvp: success");
		return true;
	}
	catch (std::exception&)
	{
        installLog(hInstall, "KillAvp: exception");
		return false;
	}
}

