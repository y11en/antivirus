/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	cached_strategy.cpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	28.09.2005 10:45:44
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable : 4275)
#include <windows.h>
#include <Prague/prague.h>
#include <Prague/wrappers/io_helpers.h>
#include "cached_strategy.h"
#include "generic_strategy.h"
#include "preprocessing.h"
#include <http/http.h>
#include <http/message.h>
#include "avp3_engine_proxy.h"
#include "stream_engine_proxy.h"
#include "processor.h"

///////////////////////////////////////////////////////////////////////////////
http::cached_strategy::cached_strategy(preprocessing* pre_proc, hOBJECT service, size_t percent_to_send, boost::shared_ptr<http::delayed_notifier> notifier)
							: generic_(new generic_strategy(pre_proc))
							, service_(new http::avp3_engine_proxy(service))
							, stream_service_(new http::stream_engine_proxy(service))
							, assemble_(percent_to_send)
							, notifier_(notifier)
{
	notifier_->add_ref();
}
///////////////////////////////////////////////////////////////////////////////
http::cached_strategy::cached_strategy(cached_strategy const& other)
							: generic_(new generic_strategy(*other.generic_))
							, service_(new http::avp3_engine_proxy(*other.service_))
							, assemble_(other.assemble_.get_percent())
							, stream_service_(new http::stream_engine_proxy(*other.stream_service_))
							, notifier_(other.notifier_)
{
	notifier_->add_ref();
}
///////////////////////////////////////////////////////////////////////////////
http::cached_strategy::~cached_strategy()
{
	notifier_->remove_events_by_key(this, true);
	notifier_->release();
}
///////////////////////////////////////////////////////////////////////////////
http::message *http::cached_strategy::extract_message(http::analyzer& anlz, msg_direction direction, bool is_ssl, bool force)
{
	//!	assert(generic_->pre_process());
	bool no_msg = msg() == 0;	
	message* msg = generic_->extract_message(anlz, direction, is_ssl, force);
	//! сообщение только что появилось, заряжаем отложенную нотификацию
	if(no_msg && msg)
		notifier_->add_event(generic_->pre_process()->timeout() / 1000, this, event_sink(this, direction, is_ssl));
	if(msg && !data_.get())
		data_ = std::auto_ptr<message::build_data>(new message::build_data);
	//! данные для стримового движка
	if(msg && !stream_proc_data_.get())
	{
		stream_service_->begin_antivirus();
		stream_proc_data_ = std::auto_ptr<message::build_data>(new message::build_data);
		stream_proc_data_->length = http::unspecified_length;
		stream_proc_data_->position = msg->max_size() - msg->size();
	}
	if(msg && !generic_->need_skip())
	{
		size_t offset(msg->max_size() - msg->size()); 
		stream_proc_data_->position = (offset > stream_proc_data_->position)
			? offset
			: stream_proc_data_->position;
	}
	return msg;
}
///////////////////////////////////////////////////////////////////////////////
bool http::cached_strategy::can_process(const http::message &msg) const
{
	//! определяем не наступил ли конец кеширования данных
	return msg.complete() || msg.ready_to_deliver() || generic_->need_skip();
}
///////////////////////////////////////////////////////////////////////////////
void http::cached_strategy::event_sink::operator()()
{
	//! сработала отложенная нотификация
	message const *msg = p_->msg();
	//! выставляем флаг ready_to_deliver на сообщении
	if(msg)
		const_cast<message *>(msg)->set_ready_to_deliver(true);
	//! проталкиваем данные
	http::processor *proc = static_cast<http::processor *>(p_->generic_->pre_process());
	if(proc)
		proc->process(0, 0, direction_, is_ssl_);
}
///////////////////////////////////////////////////////////////////////////////
void http::cached_strategy::reprocess()
{
	notifier_->remove_events_by_key(this);
	generic_->reprocess();
	data_ = std::auto_ptr<message::build_data>();
	stream_proc_data_ = std::auto_ptr<message::build_data>();
	stream_service_->end_antivirus();
}
///////////////////////////////////////////////////////////////////////////////
http::preprocessing* http::cached_strategy::set(http::preprocessing* preproc)
{
	http::preprocessing* tmp = generic_->pre_process();
	generic_->pre_process(preproc);
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////
http::message const* http::cached_strategy::msg() const { return generic_->current_message(); }
///////////////////////////////////////////////////////////////////////////////
http::service_proxy const& http::cached_strategy::service() const { return *service_; }
///////////////////////////////////////////////////////////////////////////////
http::service_proxy const& http::cached_strategy::pre_av_service() const { return *stream_service_; }
///////////////////////////////////////////////////////////////////////////////
http::message_policy& http::cached_strategy::msg_policy() { return *this; }
///////////////////////////////////////////////////////////////////////////////
http::content_policy& http::cached_strategy::cnt_policy() { return *this; }
///////////////////////////////////////////////////////////////////////////////
http::assemble_policy const& http::cached_strategy::asm_policy() const { return *this; }
///////////////////////////////////////////////////////////////////////////////
http::message* http::cached_strategy::replace(std::auto_ptr<http::message> msg)
{
	if(msg.get())
		notifier_->remove_events_by_key(this);
	return generic_->replace(msg);
}
///////////////////////////////////////////////////////////////////////////////
bool http::cached_strategy::can_complete(http::message const& msg) const
{
	return msg.complete() || generic_->is_forced();
}
///////////////////////////////////////////////////////////////////////////////
bool http::cached_strategy::need_processing(message const& msg) const
{
	return cached_strategy::can_complete(msg) && !generic_->need_skip();
}
///////////////////////////////////////////////////////////////////////////////
hIO http::cached_strategy::build(http::message const& msg)
{
	return msg.content();
}
///////////////////////////////////////////////////////////////////////////////
hIO http::cached_strategy::replace(http::message& msg, hIO io)
{
	return msg.replace(io);
}
///////////////////////////////////////////////////////////////////////////////
hIO http::cached_strategy::assemble(http::message const& msg, bool full) const
{
	hIO packet(full || generic_->need_skip() 
		? assemble_.full_packet(msg, data_.get())
		: assemble_.partial_packet(msg, data_.get()));
	return packet;
}
///////////////////////////////////////////////////////////////////////////////
hIO http::cached_strategy::pre_assemble(message const& msg) const
{
	return assemble_.full_packet(msg, stream_proc_data_.get());
}
///////////////////////////////////////////////////////////////////////////////
bool http::cached_strategy::has_assembled(message const&) const
{
	return message::build_data::has_built(data_.get());
}
///////////////////////////////////////////////////////////////////////////////
http::processing_strategy* http::cached_strategy::clone() const { return new cached_strategy(*this); }
///////////////////////////////////////////////////////////////////////////////