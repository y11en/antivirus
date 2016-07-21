/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	action.h
*		\brief	»нтерфейс, позвол€ет расшир€ть действи€ над http сообщение, т.е.
*				добавл€ть новые 
*		
*		\author ¬ладислав ќвчарик
*		\date	07.05.2005 13:33:11
*		
*		
*/		
#if !defined(_MESSAGE_ACTION_INCLUDED_H_)
#define _MESSAGE_ACTION_INCLUDED_H_
#include <utils/slist.hpp>
#include <utils/inherit_linearly.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace mpl = boost::mpl;
///////////////////////////////////////////////////////////////////////////////
//FWD объ€влени€
class message;
class request_message;
class response_message;
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	интерфейс дл€ pattern visitor
 */
template<typename T>
struct __declspec(novtable) action
{
	virtual void do_action(T&) = 0;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	метакласс возвращающий необходимый узел дл€ генератора иерархий
 */
template<typename T>
struct meta_action
{
	typedef action<T> type;

	BOOST_MPL_AUX_LAMBDA_SUPPORT(1, meta_action, (T))
};
///////////////////////////////////////////////////////////////////////////////
//! declaration the list of messages
typedef util::slist<message,
					util::slist<request_message,
								util::slist<response_message, mpl::empty_base>
								>
					> action_list;
///////////////////////////////////////////////////////////////////////////////
/**
 * \brif	-	
 */
template<typename alist>
struct action_hierarchy
		: util::inherit_linearly<alist, meta_action<mpl::_1> >::type
{
	template<typename MSG> void action(MSG& msg)
	{
		http::action<MSG>& act = *this;
		act.do_action(msg);
	}
};
///////////////////////////////////////////////////////////////////////////////
//! 
typedef action_hierarchy<action_list> base_action;
///////////////////////////////////////////////////////////////////////////////
//! макрос размещаетс€ в каждом конечном классе, который будет поддерживать
//	расширение по функционалу(добавление новых виртуальных функций в иерархию)
#define DEFINE_APPLY_ACTION(some_action) \
    virtual void apply(some_action& act) \
    { act.action(*this); }
//! макрос размещаетс€ в action классах дл€ генерации поведени€ по-умолчанию
#define DEFINE_DEFAULT_ACTION(msg) \
	virtual void do_action(msg&) \
	{ 0;/* todo: поведение по-умолчанию */ }
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_MESSAGE_ACTION_INCLUDED_H_)