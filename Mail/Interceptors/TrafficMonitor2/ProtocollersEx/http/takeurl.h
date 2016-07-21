/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	takeurl.h
*		\brief	выделяет URI и делает его абсолютным
*		
*		\author Владислав Овчарик
*		\date	10.05.2005 11:20:48
*		
*		
*/		
#if !defined(_TAKE_URL_INCLUDED_H_)
#define _TAKE_URL_INCLUDED_H_
///////////////////////////////////////////////////////////////////////////////
#include "action.h"
#include <boost/utility.hpp>
#include <vector>
#include <memory>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	выделяет и абсолютиризует URI из объекта типа message
 *				ОГРАНИЧЕНИЕ ДАННЫЙ КЛАСС НЕ МОЖЕТ БЫТЬ БАЗОВЫМ, ТАК КАК
 *				В КОНСТРУКТОРЕ ВЫЗЫВАЕТСЯ ВИРТУАЛЬНЫЕ МЕТОДЫ, ЧЕРЕЗ МЕХАНИЗМ
 *				ДВОЙНОЙ ДИСПЕТЧЕРЕЗАЦИИ
 */
class take_url
		: private boost::noncopyable
		, public action_hierarchy<action_list>
{
public:
	explicit take_url(message& msg, bool is_ssl);
	virtual void do_action(http::request_message&);
	DEFINE_DEFAULT_ACTION(response_message)
	DEFINE_DEFAULT_ACTION(message)
	std::vector<char> const& get() const;
private:
	std::auto_ptr<std::vector<char> >	uri_;
	bool is_ssl_;
};
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_TAKE_URL_INCLUDED_H_)