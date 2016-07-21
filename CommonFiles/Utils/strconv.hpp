/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	strconv.hpp
*		\brief	
*		
*		\author ¬ладислав ќвчарик
*		\date	06.01.2004 17:34:00
*		
*		
*		
*/		

#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <ctype.h>
#ifndef _AFX_DLL
	#include <windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////
#if 1
namespace std { using ::isspace; using ::iswspace; }
#endif
///////////////////////////////////////////////////////////////////////////////
namespace conv
{
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
inline bool isspace_helper(int c) {	return std::isspace(c) == 1; }
inline bool isspace_helper(wint_t c) {	return std::iswspace(c) == 1; }
///////////////////////////////////////////////////////////////////////////////
inline std::string conv_to_char(wchar_t const* s)
{
	const int bytes = ::WideCharToMultiByte(CP_ACP, 0, s, -1, 0, 0, 0, 0);
	std::vector<char> v(1 + bytes);
	if (0 == bytes
		|| 0 == ::WideCharToMultiByte(CP_ACP, 0, s, -1, &v[0], static_cast<int>(v.size()), 0, 0))
		throw std::runtime_error("conv_to_char called wcstombs() and failed.");
	return &v[0];
}
///////////////////////////////////////////////////////////////////////////////
inline std::string conv_to_char(std::wstring const& s)
{
	return conv_to_char(s.c_str());
}
///////////////////////////////////////////////////////////////////////////////
inline std::wstring conv_to_wchar(char const* s)
{
	std::vector<wchar_t> v(1 + strlen(s));
	if(0 == MultiByteToWideChar(CP_ACP, 0, s, -1, &v[0], static_cast<int>(v.size())))
		throw std::runtime_error("conv_to_wchar called mbstowcs() failed.");
	return &v[0];
}
///////////////////////////////////////////////////////////////////////////////
inline std::wstring conv_to_wchar(char const* begin, char const* end)
{
	int len = static_cast<int>(end - begin);
	std::vector<wchar_t> v(1 + len);
	if(0 == MultiByteToWideChar(CP_ACP, 0, begin, len, &v[0], static_cast<int>(v.size())))
		throw std::runtime_error("conv_to_wchar called mbstowcs() failed.");
	return &v[0];
}
///////////////////////////////////////////////////////////////////////////////
inline std::wstring conv_to_wchar(std::string const& s)
{
	return conv_to_wchar(s.c_str());
}
///////////////////////////////////////////////////////////////////////////////
template<typename _C> struct string_conv_impl;
///////////////////////////////////////////////////////////////////////////////
template<>
struct string_conv_impl<char>
{
	static std::string do_conv(char const* s) {	return std::string(s); }
	static std::string do_conv(std::string const& s) { return s; }
	static std::string do_conv(wchar_t const* s) { return conv_to_char(s); }
	static std::string do_conv(std::wstring const& s) { return conv_to_char(s); }
	static std::string do_conv(char const* begin, char const* end) { return std::string(begin, end); }
};
///////////////////////////////////////////////////////////////////////////////
template<>
struct string_conv_impl<wchar_t>
{
	static std::wstring do_conv(char const* s) {	return conv_to_wchar(s); }
	static std::wstring do_conv(std::string const& s) { return conv_to_wchar(s); }
	static std::wstring do_conv(wchar_t const* s) { return std::wstring(s); }
	static std::wstring do_conv(std::wstring const& s) { return s; }
	static std::wstring do_conv(char const* begin, char const* end) { return conv_to_wchar(begin, end); }
	static std::wstring do_conv(wchar_t const* begin, wchar_t const* end) { return std::wstring(begin, end); }
};
///////////////////////////////////////////////////////////////////////////////
inline size_t ExpandEnvironmentStrings_helper(char const* s, char* d, size_t sz)
{
	return ExpandEnvironmentStringsA(s, d, static_cast<unsigned long>(sz));
}
///////////////////////////////////////////////////////////////////////////////
inline size_t ExpandEnvironmentStrings_helper(wchar_t const* s, wchar_t* d, size_t sz)
{
	size_t size(ExpandEnvironmentStringsW(s, d, static_cast<unsigned long>(sz)));
	if(!size && ERROR_CALL_NOT_IMPLEMENTED == GetLastError())
	{
		//! Ёта ветка будет работать только под Win9x
		std::string tmp(conv_to_char(s));
		size = ExpandEnvironmentStringsA(tmp.c_str(), 0, 0);
		if(size)
		{
			std::vector<char> buf(size, 0);
			size = ExpandEnvironmentStringsA(tmp.c_str(), &buf[0], static_cast<unsigned long>(buf.size()));
			if(size <= sz)
			{
				std::wstring wtmp(conv_to_wchar(&buf[0], &buf[0] + size));
				std::copy(wtmp.begin(), wtmp.end(), d);
				d[size] = '\0';
			}
		}
	}
	return size;
}
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
//! eiiaa?oaoey char -> wchar_t
template<typename _D, typename _S>
inline std::basic_string<_D> string_conv(_S const* src)
{
	return detail::string_conv_impl<_D>::do_conv(src);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _D, typename _S>
inline std::basic_string<_D> string_conv(std::basic_string<_S> const& src)
{
	return detail::string_conv_impl<_D>::do_conv(src.c_str());
}
///////////////////////////////////////////////////////////////////////////////
template<typename _D, typename _S>
inline std::basic_string<_D> string_conv(_S const* f, _S const* l)
{
	return detail::string_conv_impl<_D>::do_conv(f, l);
}
///////////////////////////////////////////////////////////////////////////////
// converts number to string
template<typename _D, typename _S>
inline std::basic_string<_D> ntos(_S src)
{
	std::basic_ostringstream<_D> os;
	if (!(os << src))
		throw std::runtime_error("ntos(): conversion failed");
	return os.str();
}
///////////////////////////////////////////////////////////////////////////////
// converts string to number
template<typename _D, typename _S>
inline _D ston(const _S* src, bool exact = true)
{
	_D dst; _S c;
	std::basic_istringstream<_S> is(src);
	if (!(is  >> dst) || (exact && is.get(c)))
		throw std::runtime_error("ston(): conversion failed");
	return dst;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _D, typename _S>
inline _D ston(const std::basic_string<_S>& src, bool exact = true)
{
	return ston<_D>(src.c_str(), exact);
}
///////////////////////////////////////////////////////////////////////////////
template<typename _S>
inline std::basic_string<_S> expand_env_strings(const _S* src)
{
	std::basic_string<_S> result;
	size_t buf_size(0);
	if(src && (buf_size = detail::ExpandEnvironmentStrings_helper(src, 0, 0)))
	{
		std::vector<_S> buf(buf_size + 1, 0);
		detail::ExpandEnvironmentStrings_helper(src, &buf[0], buf.size());
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
template<typename _E>
inline std::basic_string<_E>& trim_left(std::basic_string<_E>& str)
{
	if(!str.empty())
	{
		std::basic_string<_E>::size_type pos(0);
		for(;pos < str.length() && detail::isspace_helper(str[pos]); ++pos);
		str.erase(0, pos);
	}
	return str;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline std::basic_string<_E>& trim_right(std::basic_string<_E>& str)
{
	if(!str.empty())
	{
		std::basic_string<_E>::size_type pos(str.length());
		while(pos-- && detail::isspace_helper(str[pos]));
		if(pos != str.length() - 1)
			str.erase(pos + 1);
	}
	return str;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline std::basic_string<_E>& trim(std::basic_string<_E>& str)
{
	return trim_left(trim_right(str));
}
///////////////////////////////////////////////////////////////////////////////
}//namespace conv
///////////////////////////////////////////////////////////////////////////////