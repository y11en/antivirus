/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	takeurl.cpp
*		\brief	выделение URI из сообщения
*		
*		\author Владислав Овчарик
*		\date	10.05.2005 11:28:05
*		
*		
*/		
#define NOMINMAX
#include <Prague/prague.h>

#include <windows.h>

#include "takeurl.h"
#include "message.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace
{
///////////////////////////////////////////////////////////////////////////////
char const http_head[] = "http://";
char const https_head[] = "https://";
///////////////////////////////////////////////////////////////////////////////
}//unnamed namesapce 
///////////////////////////////////////////////////////////////////////////////
http::take_url::take_url(http::message& msg, bool is_ssl)
					: uri_(new std::vector<char>), is_ssl_(is_ssl)
{
	msg.apply(*this);//! здесь двойная диспетчерезация.
}
///////////////////////////////////////////////////////////////////////////////
void http::take_url::do_action(http::request_message& msg)
{
	vector_ptr local_uri(msg.uri());
	vector_ptr method(msg.method());
	if(method.get() && strstr(_strupr(&(*method)[0]), _CONNECT) != 0)
	{
		vector_ptr host(msg.host());
		uri_->reserve(host->size() + sizeof(https_head));
		uri_->insert(uri_->end(), https_head, https_head + sizeof(https_head) - 1);//! exclude zero terminated characters
		uri_->insert(uri_->end(), &(*host)[0], &(*host)[0] + host->size() - 1);//! exclude zero terminated characters
		uri_->push_back('/');
		uri_->push_back(0);
	}
	else if(local_uri->size() && (*local_uri)[0] == '/')//! relative path found
	{
		vector_ptr host(msg.host());
		const char* prefix = is_ssl_ ? https_head : http_head;
		size_t prefix_size = is_ssl_ ? sizeof(https_head) : sizeof(http_head);
		uri_->reserve(host->size() + prefix_size + local_uri->size());
		uri_->insert(uri_->end(), prefix, prefix + prefix_size - 1);//! exclude zero terminated characters
		uri_->insert(uri_->end(), &(*host)[0], &(*host)[0] + host->size() - 1);//! exclude zero terminated characters
		uri_->insert(uri_->end(), &(*local_uri)[0], &(*local_uri)[0] + local_uri->size() - 1);//! exclude zero terminated characters
		uri_->push_back(0);
	}
	else
		uri_ = local_uri;
}
///////////////////////////////////////////////////////////////////////////////
std::vector<char> const& http::take_url::get() const
{
	return *uri_;
}
///////////////////////////////////////////////////////////////////////////////