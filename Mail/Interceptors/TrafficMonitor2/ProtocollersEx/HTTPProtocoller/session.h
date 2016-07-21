/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	session.h
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	29.04.2005 14:57:48
*		
*		
*/		
#if !defined(_SESSION_PROXY_INCLUDED_H_)
#define _SESSION_PROXY_INCLUDED_H_
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include <boost/utility.hpp>
#include <http/message.h>
#include <TrafficMonitor2/TrafficProtocoller.h>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace tp = TrafficProtocoller;
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	- прокси класс, представляющий сокетную сессию на уровне агализатора
 **/
class session
		: private boost::noncopyable
{
public:
	/**
	 * \breif возможные состояния сессии
	 */
	enum state
	{
		alive_		= tp::psKeepAlive,		//! продолжение обработки пакетов
		stop_		= tp::psProcessFinished,//! приостановка обработки пакетов, переход в режим детектирования пакетов
		close_		= tp::psCloseConnection,//! отказ от обработки пакетов на текущей сессии
		unknown_	= tp::psUnknown,		//! последующие данные не принадлежат протоколу HTPP, переход в режим детектирования пакетов
		terminate_
	};
	session(hOBJECT monitor, void* ctx);
	//! return client process id
	unsigned long client_id() const;
	//! отправка обработанного пакета. возвращает количество отправленных байт
	size_t send(http::message_type type, hIO packet);
	//! отправить пакет и остановить обработку пакетов
	size_t stop(http::message_type type, hIO packet);
	//! разорвать сессию
	size_t break_off(http::message_type type);
	//! отправить пакет и прекратить обработку пакетов на текущем соединение
	size_t close(http::message_type type, hIO packet);
	//! проверить нужно ли пропускать этот пакет в текущей сессии
	bool skip_that(char const* url, char const* content_type = 0);

	~session();
private:
	size_t do_send(http::message_type type, hIO packet, state st = alive_);
private:
	hOBJECT				traffic_monitor_;	
	void*				ctx_;							//! объект типа session_t
};
///////////////////////////////////////////////////////////////////////////////
}//namesapce http
#endif//!defined(_SESSION_PROXY_INCLUDED_H_)