/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	strconv.hpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	06.01.2004 17:34:00
*		
*		
*		
*/		

#pragma once
#include <vector>
#include <string>
#include <ctype.h>

///////////////////////////////////////////////////////////////////////////////
#if _MSC_VER < 1300
namespace std { using ::isspace; using ::iswspace; }
#endif
///////////////////////////////////////////////////////////////////////////////
namespace conv
{
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
inline std::string conv_to_char(wchar_t const* s)
{
	const size_t bytes = wcstombs(0, s, 0);
	std::vector<char> v(1 + bytes);
	if (-1 == bytes || -1 == wcstombs(&v[0], s, v.size()))
		throw std::runtime_error("conv_to_char called wcstombs() and failed.");
	return &v[0];
}
///////////////////////////////////////////////////////////////////////////////
inline std::string conv_to_char(wchar_t const* begin, wchar_t const* end)
{
	size_t len(end - begin);
	std::vector<char> v(1 + len);
	if (-1 == wcstombs(&v[0], begin, len))
		throw std::runtime_error("conv_to_char called wcstombs() failed.");
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
	if (-1 == mbstowcs(&v[0], s, strlen(s)))
		throw std::runtime_error("conv_to_wchar called mbstowcs() failed.");
	return &v[0];
}
///////////////////////////////////////////////////////////////////////////////
inline std::wstring conv_to_wchar(char const* begin, char const* end)
{
	size_t len(end - begin);
	std::vector<wchar_t> v(1 + len);
	if (-1 == mbstowcs(&v[0], begin, len))
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
	static std::string do_conv(char const* begin, char const* end) { return std::string(begin, end); }
	static std::string do_conv(wchar_t const* s) { return conv_to_char(s); }
	static std::string do_conv(std::wstring const& s) { return conv_to_char(s); }
	static std::string do_conv(wchar_t const* begin, wchar_t const* end) { return conv_to_char(begin, end); }
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
};
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
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
}//namespace conv
///////////////////////////////////////////////////////////////////////////////