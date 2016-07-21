/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	http.h
*		\brief	describes basic structures for HTTP packet parser
*		
*		\author ¬ладислав ќвчарик
*		\date	14.02.2005 15:10:39
*		
*		
*/
#if !defined(_HTTP_INCLIDE_H_)
#define _HTTP_INCLIDE_H_
#pragma warning(disable:4761)
#include <utils/static_array.hpp>
#include <vector>
#include <map>
#include <string>
#include <list>
//TDOD: Ќеобходимо в будующем сделать эти структуры шаблонными,
//		что бы не зависить от типа итератора при синтаксическом разборе,
//		сейчас в качестве итератора выступает указатель на char
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
//! используем дабы избежать преждевременного копировани€
typedef util::static_array<char>			buffer_t;
//! используем дл€ возвращени€ плоских данных большого объема.
typedef std::auto_ptr<std::vector<char> > vector_ptr;
///////////////////////////////////////////////////////////////////////////////
template<typename It>
inline vector_ptr make_vector_ptr(It beg, It end)
{
	vector_ptr v(new std::vector<char>);
	v->reserve(std::distance(beg, end) + 1);
	v->insert(v->end(), beg, end);
	return v;
}
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
//! explicit specializatin
template<>
struct std::less<http::buffer_t>
			: public std::binary_function<http::buffer_t, http::buffer_t, bool>
{
	bool operator()(http::buffer_t const& lhs, http::buffer_t const& rhs) const
	{
		bool result(false);
		if(!lhs.empty() && !rhs.empty())
		{
			int cmpresult(_memicmp(&lhs[0], &rhs[0], lhs.size() > rhs.size() ? lhs.size() : rhs.size()));
			if(cmpresult == 0)
				result = lhs.size() < rhs.size() ? true : false;
			else if(cmpresult < 0)
				result = true;
		}
		return result;
	};
};
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
//! static constatnt
namespace
{
//! http command
#define _OPTIONS "OPTIONS"
#define _GET "GET"
#define _HEAD "HEAD"
#define _POST "POST"
#define _PUT "PUT"
#define _DELETE "DELETE"
#define _TRACE "TRACE"
#define _CONNECT "CONNECT"
//! http header fileds
#define _CONTENT_LENGTH "Content-Length"
#define _TRANSFER_ENCODING "Transfer-Encoding"
#define _CONTENT_TYPE "Content-Type"
#define _CONTENT_ENCODING "Content-Encoding"
#define _CACHE_CONTROL "Cache-Control"
#define _PRAGMA "Pragma"

#define _HOST "Host"
//! http header filedss' values
#define _CHUNKED "chunked"
///////////////////////////////////////////////////////////////////////////////
//! create buffer_t from null terminated string
inline http::buffer_t field_name(char const* fname)
{
	char * p = const_cast<char*>(fname);
	return http::buffer_t(p, p + strlen(p));
}
///////////////////////////////////////////////////////////////////////////////
template<typename T> inline T buffer_cast(http::buffer_t const& b, T* p = 0)
{
	return do_buffer_cast(b, p);
}
///////////////////////////////////////////////////////////////////////////////
//! cast buffer_t value to int value
inline int do_buffer_cast(http::buffer_t const& b, int* = 0)
{
	std::vector<char> buffer(b.size() + 1, 0);
	std::copy(b.begin(), b.end(), buffer.begin());
	return atoi(&buffer[0]);
}
///////////////////////////////////////////////////////////////////////////////
//! cast buffer_t value to std::string value
inline std::string do_buffer_cast(http::buffer_t const& b, std::string* = 0)
{
	return std::string(b.begin(), b.end());
}
//! cast buffer_t value to vector_ptr value
inline vector_ptr do_buffer_cast(http::buffer_t const& b, vector_ptr* = 0)
{
	return make_vector_ptr(b.begin(), b.end());
}
///////////////////////////////////////////////////////////////////////////////
}//unnamed namespace
//! enumerate request method
enum method_type
{
	_unknown			= 0,
	_options			= 1,
	_get				= 2,
	_head				= 3,
	_post				= 4,
	_put				= 5,
	_delete				= 6,
	_trace				= 7,
	_connect			= 8
};
///////////////////////////////////////////////////////////////////////////////
//! return numeric value of the request method
inline method_type request_method(buffer_t method)
{
	vector_ptr tmp(buffer_cast<vector_ptr>(method, 0));
	tmp->push_back(0);
	char* lmethod = &(*tmp)[0];
	if(_stricmp(lmethod, _OPTIONS) == 0)
		return _options;
	else if(_stricmp(lmethod, _GET) == 0)
		return _get;
	else if(_stricmp(lmethod, _HEAD) == 0)
		return _head;
	else if(_stricmp(lmethod, _POST) == 0)
		return _post;
	else if(_stricmp(lmethod, _PUT) == 0)
		return _put;
	else if(_stricmp(lmethod, _DELETE) == 0)
		return _delete;
	else if(_stricmp(lmethod, _CONNECT) == 0)
		return _connect;
	else if(_stricmp(lmethod, _TRACE) == 0)
		return _trace;

	return _unknown;
}
///////////////////////////////////////////////////////////////////////////////
typedef std::map<buffer_t, buffer_t>	header_t;
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	header data for request message
 */
struct request_data
{
	buffer_t		method;
	buffer_t		uri;
	buffer_t		ver;
	header_t		hdrs;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	header for response message
 */
struct response_data
{
	buffer_t		ver;
	int				scode;
	buffer_t		reason;
	header_t		hdrs;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	describes chunk in the chunked transfer encoding
 */
struct chunk_data
{
	int			size;		//! size of the chunk
	buffer_t	extension;	//! ; field-name=field-value
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	describes last chunk in the chunked transfer encoding
 */
struct last_chunk_data
{
	int			size;		//! must be a zero
	buffer_t	extension;	//! ; field-name=field-value
	header_t	trailer;
};
///////////////////////////////////////////////////////////////////////////////
//! return true if request method or url require SSL
//inline bool detect_ssl_handshake_request(char const* method, char const* url)
//{
//	return (url && strstr(url, "HTTPS:") != 0)
//			|| (method && strstr(method, _CONNECT) != 0);
//}
//! return  true if response is handshake SSL respone
inline bool detect_ssl_handshake_response(int scode, char const* reason)
{
	return scode == 200 && strstr(reason, "connection established") != 0;
}
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_HTTP_INCLIDE_H_)