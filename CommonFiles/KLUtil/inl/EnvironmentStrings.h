#if !defined(_ENVIRONMENTSTRINGS_H_)
#define _ENVIRONMENTSTRINGS_H_

#if _MSC_VER > 1000
#	pragma once
#endif // _MSC_VER > 1000

#include "../UnicodeConv.h"

namespace KLUTIL
{
	///////////////////////////////////////////////////////////////////////////////
	inline size_t ExpandEnvStrings(char const* s, char* d, size_t sz)
	{
		return ExpandEnvironmentStringsA(s, d, sz);
	}

	///////////////////////////////////////////////////////////////////////////////
	inline size_t ExpandEnvStrings(wchar_t const* s, wchar_t* d, size_t sz)
	{
		size_t size(ExpandEnvironmentStringsW(s, d, sz));
		if(!size && ERROR_CALL_NOT_IMPLEMENTED == GetLastError())
		{
			//! Эта ветка будет работать только под Win9x
			char const* tmp = __LPCSTR(s);
			size = ExpandEnvironmentStringsA(tmp, 0, 0);
			if(size)
			{
				std::vector<char> buf(size, 0);
				size = ExpandEnvironmentStringsA(tmp, &buf[0], buf.size());
				if(size <= sz)
				{
					wcscpy(d, __LPCWSTR(&buf[0]));
					d[size] = '\0';
				}
			}
		}
		return size;
	}

	///////////////////////////////////////////////////////////////////////////////
	template<typename _S>
	inline std::basic_string<_S> expand_env_strings(const _S* src)
	{
		std::basic_string<_S> result;
		size_t buf_size(0);
		if(src && (buf_size = ExpandEnvStrings(src, 0, 0)))
		{
			std::vector<_S> buf(buf_size + 1, 0);
			ExpandEnvStrings(src, &buf[0], buf.size());
			result.assign(&buf[0]);
		}
		return result;
	}

	///////////////////////////////////////////////////////////////////////////////
	template<typename _S>
	inline std::basic_string<_S> expand_env_strings(const std::basic_string<_S>& src)
	{
		return expand_env_strings(src.c_str());
	}


	///////////////////////////////////////////////////////////////////////////////
	typedef BOOL (WINAPI *fnPathUnExpandEnvStringsA)(LPCSTR pszPath, LPSTR pszBuf, UINT cchBuf);

	inline bool PathUnExpandEnvStrings(char const* s, char* d, size_t sz)
	{
		bool bResult = false;

		HMODULE hShlwapi32 = ::LoadLibrary (_T("Shlwapi.dll"));
		if (hShlwapi32)
		{
			fnPathUnExpandEnvStringsA pPathUnExpandEnvStrings = (fnPathUnExpandEnvStringsA)::GetProcAddress (hShlwapi32, "PathUnExpandEnvStringsA");

			if (pPathUnExpandEnvStrings)
				bResult = (pPathUnExpandEnvStrings(s, d, sz) == TRUE);

			::FreeLibrary (hShlwapi32);
		}

		return bResult;
	}

	///////////////////////////////////////////////////////////////////////////////
	typedef BOOL (WINAPI *fnPathUnExpandEnvStringsW)(LPCWSTR pszPath, LPWSTR pszBuf, UINT cchBuf);

	inline bool PathUnExpandEnvStrings(wchar_t const* s, wchar_t* d, size_t sz)
	{
		bool bResult = false;

		HMODULE hShlwapi32 = ::LoadLibrary (_T("Shlwapi.dll"));
		if (hShlwapi32)
		{
			fnPathUnExpandEnvStringsW pPathUnExpandEnvStrings = (fnPathUnExpandEnvStringsW)::GetProcAddress (hShlwapi32, "PathUnExpandEnvStringsW");

			if (pPathUnExpandEnvStrings)
				bResult = (pPathUnExpandEnvStrings(s, d, sz) == TRUE);

			::FreeLibrary (hShlwapi32);
		}

		return bResult;
	}

	///////////////////////////////////////////////////////////////////////////////
	template<typename _S>
	inline std::basic_string<_S> unexpand_env_strings(const _S* src)
	{
		std::basic_string<_S> result;

		_S buf[MAX_PATH];
		if (PathUnExpandEnvStrings(src, buffer, MAX_PATH))
			result.assign(&buf[0]);
		else
			result = src;

		return result;
	}

	///////////////////////////////////////////////////////////////////////////////
	template<typename _S>
	inline std::basic_string<_S> unexpand_env_strings(const std::basic_string<_S>& src)
	{
		return unexpand_env_strings(src.c_str());
	}

} // namespace KLUTIL

#endif //_ENVIRONMENTSTRINGS_H_
