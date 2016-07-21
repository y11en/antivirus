/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	verifyssl.cpp
*		\brief	реализация проверки пакета на принадлежность к SSL
*		
*		\author Владислав Овчарик
*		\date	12.05.2005 9:44:45
*		
*		
*/		
#define NOMINMAX
#include <Prague/prague.h>

#include <windows.h>

#include "verifyssl.h"
#include "message.hpp"
///////////////////////////////////////////////////////////////////////////////
http::verify_ssl::verify_ssl(http::message& msg)
					: is_ssl_(false)
{
	msg.apply(*this);//! здесь двойная диспетчерезация
}
///////////////////////////////////////////////////////////////////////////////
void http::verify_ssl::do_action(http::response_message& msg)
{
	vector_ptr reason(msg.reason());
	is_ssl_ = detect_ssl_handshake_response(msg.status_code(), _strlwr(&(*reason)[0]));
	is_ssl_ |= (msg.status_code() == 200 && msg.request_method() == _connect);
}
///////////////////////////////////////////////////////////////////////////////