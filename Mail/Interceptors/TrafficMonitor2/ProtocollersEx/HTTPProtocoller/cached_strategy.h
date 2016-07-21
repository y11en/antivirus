/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	cached_strategy.h
*		\brief	определение класса реализующего кеширующую стратегию
*				для обработки на AVP3 движке
*		\author Vladislav Ovcharik
*		\date	28.09.2005 10:34:47
*		
*		
*/		
#if !defined(CACHED_STRATEGY_H_INCLUDED)
#define CACHED_STRATEGY_H_INCLUDED
#include "proc_strategy.h"
#include "message_builder.h"
#include <boost/utility.hpp>
#include "delayed_notifier.h"
#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
class avp3_engine_proxy;
class stream_engine_proxy;
class generic_strategy;
struct preprocessing;
///////////////////////////////////////////////////////////////////////////////
class cached_strategy
			: private boost::noncopyable
			, private http::message_policy
			, private http::content_policy
			, private http::assemble_policy
			, public http::processing_strategy
{
public:
	cached_strategy(preprocessing*, hOBJECT service, size_t percent_to_send, boost::shared_ptr<http::delayed_notifier> notifier);
	virtual ~cached_strategy();
	//! implementation processing_strategy interface
	virtual http::message *extract_message(analyzer& anlz, msg_direction direction, bool is_ssl, bool force);
	virtual bool can_process(const http::message &msg) const;
	virtual void reprocess();
	virtual preprocessing* set(preprocessing* preproc);
	virtual message const* msg() const;
	virtual service_proxy const& service() const;
	virtual service_proxy const& pre_av_service() const;
	virtual http::message_policy& msg_policy();
	virtual http::content_policy& cnt_policy();
	virtual http::assemble_policy const& asm_policy() const;
	virtual processing_strategy* clone() const;
private:
	//! implementation message_policy interface
	virtual message* replace(std::auto_ptr<message> msg);
	virtual bool can_complete(message const&) const;
	virtual bool need_processing(message const&) const;
	//! implementation content_policy interface
	virtual hIO build(http::message const& msg);
	virtual hIO replace(http::message& msg, hIO io);
	//! implementation assemble_policy interface
	virtual hIO assemble(message const& msg, bool full) const;
	virtual hIO pre_assemble(message const&) const;
	virtual bool has_assembled(message const&) const;
	cached_strategy(cached_strategy const&);
private:
	std::auto_ptr<generic_strategy>		generic_;	//! общее поведение для всех типов стратегий
	std::auto_ptr<avp3_engine_proxy>	service_;	//! антивирусный сервис
	std::auto_ptr<stream_engine_proxy>	stream_service_; //! антивирусный сервис
	std::auto_ptr<message::build_data>	data_;		//! processed & sent data
	std::auto_ptr<message::build_data>	stream_proc_data_; //! stream processed data
	message_builder						assemble_;	//! собирает сообщение в ASCII поток
	class event_sink
	{
		cached_strategy *p_;
		msg_direction direction_;
		bool is_ssl_;
	public:
		event_sink(cached_strategy *p, msg_direction direction, bool is_ssl) : 
		  p_(p), direction_(direction), is_ssl_(is_ssl){}
		void operator()();
	};
	boost::shared_ptr<http::delayed_notifier> notifier_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(CACHED_STRATEGY_H_INCLUDED)