/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	stream_error.h
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	28.02.2006 10:02:36
*		
*		
*/		
#if !defined(_STREAM_ERROR_H_INCLUDED)
#define _STREAM_ERROR_H_INCLUDED
#include "exceptions.hpp"
#include "passthrough.h"
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace stream_error
{
///////////////////////////////////////////////////////////////////////////////
/** passthrough or to server or to client*/
class passthough_stream
{
public:
	passthough_stream(http::msg_direction dir, http::passthrough* pass)
		: direction_(dir)
		, pass_(pass)
	{
	}
	passthough_stream(passthough_stream const& other)
		: direction_(other.direction_)
		, pass_(other.pass_)
	{
	}
	void operator()(char const* beg, char const* end)
	{
		if(pass_)
			pass_->passto(direction_, beg, end);
	}
private:
	http::msg_direction	direction_;
	http::passthrough*	pass_;
};
///////////////////////////////////////////////////////////////////////////////
class throw_exception
{
public:
	explicit throw_exception(http::msg_direction dir)
		: direction_(dir)
	{
	}
	void operator()(char const* beg, char const* end)
	{
		throw http::extract_error(std::string(beg, end));
	}
private:
	http::msg_direction direction_;
};
///////////////////////////////////////////////////////////////////////////////
class nothing_todo
{
public:
	void operator()(char const* beg, char const* end)
	{
		0;
	}
};
///////////////////////////////////////////////////////////////////////////////
}//namesapce stream_error
///////////////////////////////////////////////////////////////////////////////
}//namespace http
#endif//!defined(_STREAM_ERROR_H_INCLUDED)