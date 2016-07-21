/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	processor.h
*		\brief	Описание класса, предназначенного для обработки контента HTTP пакетов.
*				Обработка происходит после того, как пакет получен полностью если
*				в стратегии указан AVP3 "движок", если же в стратегии выбран потоковый
*				движок, то пакет не кешируется и обработка происходит помере поступления
*				данных
*		
*		\author Владислав Овчарик
*		\date	29.04.2005 13:22:10
*		
*		
*/		
#if !defined(_HTTP_PROCESSOR_INCLUDED_H_)
#define _HTTP_PROCESSOR_INCLUDED_H_
#include <vector>
#include <TrafficMonitor2/TrafficProtocollerTypes.h>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include "service_observer.h"
#include "preprocessing.h"
#include "proc_strategy.h"
#include <http/support_persistent.h>
#include <http/passthrough.h>
#include <AV/structs/s_avs.h>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
//! fwd declaration
class session;
class analyzer;
///////////////////////////////////////////////////////////////////////////////
namespace tp = TrafficProtocoller;
///////////////////////////////////////////////////////////////////////////////
/**
 * результаты детектирования входной последовательности, на принадлежность к 
 * HTTP протоколу
 */
enum detection_code
{
	detected = tp::dcDataDetected,			//! выделена служебная часть пакета
	need_more = tp::dcProtocolLikeDetected,	//! входной поток принадлежит протоколоу,
											//!	но служебная чать получена не полностью
	not_detected = tp::dcProtocolUnknown	//! входной поток не принадлежит протоколу 
};
///////////////////////////////////////////////////////////////////////////////
/**
 * обработчик потока данных протокола HTTP
 * 1. может детектировать входной поток на принадлежность к протоколу HTTP
 * 2. выделенное сообщение сообщение из трафика передает на обработку в
 *    антивирусный сервис на обработку.
 * 3. тип обработки определяется стратегией
 */
class processor
	: private boost::noncopyable
	, private service_observer
	, private preprocessing
	, private support_persistent
	, private passthrough
{
public:
	//! :-)
	processor(std::auto_ptr<processing_strategy> strategy
			, std::auto_ptr<session> proxy
			, std::auto_ptr<analyzer> analyzer
			, clock_t timeout);
	/**
	 * определяет, является ли входная последовательность протоколом HTTP
	 * SSL должен быть пропущен.
	 */
	detection_code detect(char const* begin, char const* end, msg_direction direction);
	/**
	 * помещает входную последовательность в очередь на обработку,
	 * а затем обрабатывает накопленную последовательность
	 */
	bool process(char const* begin, char const* end, msg_direction direction, bool is_ssl);
	/**
	 * принудительно сбрасывает на обработку частично выделенное HTTP сообщение
	 * длина данных в сообщении может определяться моментом разрыва соединения
	 */
	void flush();

	void lock() { sync::details::lock_ops<sync::local_mutex>::lock(mutex_); }
	void unlock() { sync::details::lock_ops<sync::local_mutex>::unlock(mutex_); }

	tERROR process_detection(cAskObjectAction *ask_action);

	//! ;-)
	~processor();
private:
	//! implementation service_observer
	virtual bool processing_yield(clock_t* start, int direction);
	//! implementation support_persistent
	virtual http::method_type request_method() const;
	virtual void request_method(http::method_type);
private:
	/**
	 * обоработка полученного сообщения
	 * 1. - выделить сообщение из полученных данных
	 * 2. - передать на обработку содержимое пакета в антивусный сервис
	 */
	bool do_process(msg_direction direction, bool is_ssl, bool force = false);
	//! implementation preprocessing
	virtual clock_t timeout() const
	{
		return timeout_;
	}
	virtual bool skip_that(char const* url) const;
	virtual bool skip_message(message const& msg) const;
	//! implementation passthrough
	virtual void passto(msg_direction dir, char const* beg, char const* end);
private:
	processing_strategy const& do_strategy(msg_direction direction) const { return *strategies_.at(direction); }
	processing_strategy& do_strategy(msg_direction direction) { return *strategies_.at(direction); }
	analyzer const& do_analyzer(msg_direction direction) const { return *analyzers_.at(direction); }
	analyzer& do_analyzer(msg_direction direction) { return *analyzers_.at(direction); }
private:
	/**
	 * стратегия+политики обработки полученного сообщения разделяются для request\response потоков
	 * необходимо разделять для поддержки pipelining
	 */
	std::vector<boost::shared_ptr<processing_strategy> > strategies_;
	/**
	 * анализатор протокола разбор+детектирование потока данных
	 * отдельно для response and request
	 */
	std::vector<boost::shared_ptr<analyzer> > analyzers_;
	/** socket layer proxy*/
	std::auto_ptr<session>				proxy_;
	/** last requested URI*/
	std::vector<char>					url_;	
	/** download timeout, -1 for infinite*/
	clock_t								timeout_;
	/** need to serialize process method*/
	sync::local_mutex					mutex_;
	/** request queue*/
	mutable std::vector<method_type>	queue_;
	/** av processing stage */
	enum processing_stage
	{
		stgNone,
		stgPreprocess,
		stgProcess
	};
	processing_stage					processing_stage_;
	std::vector<std::wstring>			detected_on_preprocessing_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_HTTP_PROCESSOR_INCLUDED_H_)
///////////////////////////////////////////////////////////////////////////////