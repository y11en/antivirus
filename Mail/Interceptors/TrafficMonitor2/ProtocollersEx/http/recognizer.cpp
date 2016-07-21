/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	recognizer.cpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	23.09.2005 20:06:33
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable : 4275 4786 4761)
#include "http.h"
#include "recognizer.hpp"

#include <algorithm>
#include <boost/spirit/core.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
/** возвращает статический массив базовых методов для HTTP*/
static const char *methods_[] =
{
	_OPTIONS,
	_GET,
	_HEAD,
	_POST,
	_PUT,
	_DELETE,
	_TRACE,
	_CONNECT
};

///////////////////////////////////////////////////////////////////////////////
struct check_method
{
	check_method(char const* beg, char const* end)
		: beg_(beg)
		, end_(end)
		, len_(std::distance(beg, end))
	{
	}
	bool operator ()(char const* s) const
	{
		return len_ && strlen(s) > len_ && std::equal(beg_, end_, s, &check_method::compare);
	}
private:
	static bool compare(char lhs, char rhs)
	{
		char l[] = { lhs, 0 };
		char r[] = { rhs, 0 };
		return _strlwr(l)[0] == _strlwr(r)[0];
	}
private:
	char const* beg_;
	char const* end_;
	size_t		len_;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * возвращает true, если есть хотя бы надежда на то, что в
 * в буфере есть необходимый нам метод
 **/
bool its_likely_mine(char const* begin, char const* end)
{
	const char **it =
			std::find_if(methods_, methods_ + _countof(methods_), check_method(begin, end));
	return it != methods_ + _countof(methods_);
}
///////////////////////////////////////////////////////////////////////////////
}//namespace
///////////////////////////////////////////////////////////////////////////////
 /**
 * data - данные результата парсинга по сути это заголовок пакета
 */
template<>
recognition recognizer<request_data>::detect(request_data const& data, bool hit
											, char const* stop, char const* begin
											, char const* end)
{
	namespace spirit = boost::spirit;

	if(hit)
		return http::its_mine;
	/** частично распарсили. получили несколько байт */
	if(!hit && data.method.empty() && detail::its_likely_mine(begin, end))
		return http::likely_mine;
	/** частично распарсили. проверяем что смогли выделить ТИП запроса */
	if(!hit && (end - stop) != (end - begin) && !data.method.empty())
	{
		if(data.uri.size() == 0 && stop != end)//веделили только метод пытаемся найти URL
		{
			spirit::rule<> white_space = +(spirit::ch_p(' ') | spirit::ch_p('\t'));
			spirit::parse_info<> info = spirit::parse(stop, end, white_space);
			if(!info.hit)//! если пробелы найдены, то надеемся на HTTP 1/1
				return http::not_mine;
			stop = info.stop;
			if(stop != end)//! ожидаем URL запроса
			{
				spirit::rule<> url = +(spirit::range<unsigned char>('\x20', '\xff') - spirit::space_p) >> +(spirit::ch_p(' ') | spirit::ch_p('\t'));
				spirit::parse_info<> info = spirit::parse(stop, end, url);
				if(!info.hit && info.stop != end)
					return http::not_mine;
				stop = info.stop;
			}
		}
		if(stop != end && data.ver.size() == 0)//! ожидаем версию
		{
			spirit::rule<> version = spirit::str_p("HTTP") >> spirit::ch_p('/') >> +(spirit::range<unsigned char>('\x20', '\xff') - spirit::space_p);
			spirit::parse_info<> info = spirit::parse(stop, end, version);
			if(!info.hit && info.stop != end)
				return http::not_mine;
			stop = info.stop;
		}
		return http::likely_mine;
	}
	/** ничего не нашли*/
	return http::not_mine;
}
///////////////////////////////////////////////////////////////////////////////
template<>
recognition recognizer<response_data>::detect(response_data const& data, bool hit
											, char const* stop, char const* begin
											, char const* end)
{
	namespace spirit = boost::spirit;

	if(hit)
		return http::its_mine;
	/** частично распарсили*/
	if(stop != begin)
	{
		/** версию получили - OK */
		if(data.ver.size() != 0)
			return http::likely_mine;
		/** если приняли меньше 6 байт, то stop должен совпадать с end */
		if(stop - begin < 6)
			return stop == end ? http::likely_mine : http::not_mine;
	}
	return http::not_mine;
}
///////////////////////////////////////////////////////////////////////////////
}//namesapce http
///////////////////////////////////////////////////////////////////////////////