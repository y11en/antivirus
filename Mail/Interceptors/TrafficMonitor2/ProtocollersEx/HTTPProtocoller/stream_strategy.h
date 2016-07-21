/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	stream_strategy.h
*		\brief	определение класса реализующего стратегию
*				для обработки на потоковом движке
*		\author Vladislav Ovcharik
*		\date	28.09.2005 10:34:47
*		
*		
*/		
#if !defined(STREAM_STRATEGY_H_INCLUDED)
#define STREAM_STRATEGY_H_INCLUDED
#include "proc_strategy.h"
#include "message_builder.h"
#include <boost/utility.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
class stream_engine_proxy;
class generic_strategy;
struct preprocessing;
///////////////////////////////////////////////////////////////////////////////
class stream_strategy
			: private boost::noncopyable
			, private http::message_policy
			, private http::content_policy
			, private http::assemble_policy
			, public http::processing_strategy
{
	struct content_data;
public:
	stream_strategy(preprocessing*, hOBJECT service);
	virtual ~stream_strategy();
	//! implementation processing_strategy interface
	virtual message *extract_message(analyzer& anlz, msg_direction direction, bool is_ssl, bool force);
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
	stream_strategy(stream_strategy const&);
private:
	std::auto_ptr<generic_strategy>		generic_;	//! общее поведение для всех типов стратегий
	std::auto_ptr<stream_engine_proxy>	service_;	//! антивирусный сервис
	std::auto_ptr<message::build_data>	data_;		//! sent data
	std::auto_ptr<message::build_data>	proc_data_;	//! processed data
	message_builder						assemble_;	//! собирает сообщение в ASCII поток
	content_data*						content_;	//! контент отданный на проверку
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(STREAM_STRATEGY_H_INCLUDED)