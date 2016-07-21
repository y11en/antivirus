/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	analyzerimpl.h
*		\brief	реализация анализатора протокола HTTP
*				класс analyzer определяет принадлежность поступающих
*				данных протоколу HTTP, выделяет законченные пакеты для обработки
*				Под законченным пакетом(сообщение) подразумевается сообщение, для которого
*				можно выделить заголовок пакета - request/response line & all entity headers.
*				
*		
*		\author Владислав Овчарик
*		\date	25.04.2005 16:29:17
*		
*		
*/		
#if !defined(_HTTP_ANALYZER_IMPL_INCLUDE_H_)
#define _HTTP_ANALYZER_IMPL_INCLUDE_H_
//////////////////////////////////////////////////////////////////////////////
#include <TrafficMonitor2/TrafficProtocollerTypes.h>
#include <deque>
#include <utils/local_mutex.hpp>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include "message.h"
#include "recognizer.hpp"
#include "cache.hpp"
#include "support_persistent.h"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - реализация анализатора HTTP
 * */
class analyzer
		: private boost::noncopyable
{
public:
	//! ;-)
	analyzer();
	explicit analyzer(support_persistent*);
	//! detect http traffic from input sequence
	virtual http::recognition recognize(const char* begin, char const* end) const;
	//! extract message from cached http traffic
	virtual std::auto_ptr<message> extract(bool force = false);
	//! extract message from cached http traffic
	virtual void extract(http::message& msg);
	//! enqueue input sequence
	void enqueue(char const* begin, char const* end);
	//! return true if cached traffic not empty
	bool can_extract() const { return !cache_.empty(); }
	//! return cached data and clear cache
	std::auto_ptr<std::deque<char> > flush();
	//! ;-)
	virtual ~analyzer();
	virtual analyzer* clone() const;
	virtual support_persistent* support_keep_alive(support_persistent*);
	/** set a new error recovery strategy*/
	template<typename T> void error_strategy(T const& fn)
	{
		error_ = fn;
	}
private:
	//! return recognition code for the input sequence
	//! need to detect the input seques is a http traffic
	template<typename D>
	static recognition do_recognize(D const& data, bool hit
									, char const* stop
									, char const* begin, char const* end)
	{
		return http::recognizer<D>::detect(data, hit, stop, begin, end);
	}
private:
	//! recovery input http stream from error(litter)
	size_t recovery_error(char const* begin, char const* end);
	/**
	  * create a http message from the input buffer. return number of bytes
	  * extructed from input buffer
	  *	force - заставляем анализатор выделить максимально доступный заголовок пакета
	  *			данный параметр необходим для востановления пакетов с ошибками.
	  *			Microsoft-IIS/6.0 может присылать сообщение без финального CRLF,
	  *			если в этом сообщение отсутсвует message-body
	  * msg   - smart ptr to a message may be NULL
	  * [begin, end) - input sequence
	  */ 
	std::pair<size_t, bool> do_extract(std::auto_ptr<message>& msg, char const* begin, char const* end, bool force);
	/**
	  * create a message from cached http traffic
	  * return a new message if msg is NULL, if msg is not NULL will return is NULL
	  * force - заставляет анализатор выделить максимально допустимый протоколом
	  *			заголовок сообщения. Это необходимо для востановления ошибочных пакетом
	  *			Microsoft-IIS/6.0 может присылать сообщение без финального CRLF,
	  *			если в таком сообщение отсутсвует message-body
	  */
	std::auto_ptr<http::message> do_extract(http::message* msg, bool force);
	//! create http message
	/**
	 * type			- type of message (request or response)
	 * real_size	- size of the message-body + message-entity
	 * hdr_begin	- begin of the message-header + (request/status line)
	 * hdr_end		- end of the message-header + (request/status line)
	 * begin		- begin of the message-body (content data)
	 * end			- end of the message-body (content data)
	 */
	std::auto_ptr<message> create_message(message_type type, size_t real_size
										, char const* hdr_begin, char const* hdr_end
										, char const* begin, char const* end
										, method_type request_method);
private:
	typedef cache::mmfcache::iterator			cache_iterator;
	typedef cache::mmfcache::const_iterator		const_cache_iterator;
	/** cache to parse input request\response*/
	cache::growable<cache::mmfcache>					cache_;
	support_persistent*									supp_;
	/** what does if error occurred? this is strategy to process error*/
	boost::function2<void, char const*, char const*>	error_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_HTTP_ANALYZER_IMPL_INCLUDE_H_)