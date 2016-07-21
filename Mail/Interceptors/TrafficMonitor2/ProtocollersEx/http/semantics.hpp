/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	semantics.hpp
*		\brief	семантические действия
*		
*		\author Владислав Овчарик
*		\date	03.05.2005 14:36:06
*		
*		
*/		
#if !defined(_SEMANTICS_INCLUDED_H_)
#define _SEMANTICS_INCLUDED_H_
#pragma warning(disable:4761)
#include "http.h"
#include <cassert>
#include <algorithm>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
/**
 * semantic action convert parsed data to a integer
 */
struct digits
{
	explicit digits(int* val)
		: val_(val)
	{
	}
	void operator()(char const* f, char const* l) const
	{
		if(val_)
		{
			char buffer[0x20] = { 0 };
			assert(0x20 > (l - f));
			std::copy(f, l, buffer);
			*val_ = atoi(buffer);
		}
	}
private:
	int* val_;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * semantic action copies parsed data to a buffer
 */
struct sequence
{
	sequence(buffer_t* seq)
		: seq_(seq)
	{
	}
	void operator()(char const* f, char const* l) const
	{
		if(seq_)
			seq_->reset(const_cast<char*>(f), const_cast<char*>(l));
	}
private:
	buffer_t* seq_;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * semantic action fills up header-message map with tuple (field-name, field-value)
 */
struct header
{
	header(header_t* p)
		: hdrs_(p)
	{
	}
	void operator()(char const* f, char const* l) const
	{
		if(hdrs_)
		{
			char const* sep = std::find(f, l, ':');
			if(sep != l)
			{
				char const* val = sep + 1;
				for(;val != l && *val == ' '; ++val);
				hdrs_->insert(std::make_pair(buffer_t(const_cast<char*>(f), const_cast<char*>(sep))
											, buffer_t(const_cast<char*>(val), const_cast<char*>(l))));
			}
		}
	}
private:
	header_t*	hdrs_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_SEMANTICS_INCLUDED_H_)