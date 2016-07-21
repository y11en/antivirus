/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	stream_strategy.cpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	28.09.2005 10:45:44
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable : 4275)
#include <Prague/prague.h>

#include <windows.h>
#include <Prague/wrappers/io_helpers.h>
#include "stream_strategy.h"
#include "generic_strategy.h"
#include "preprocessing.h"
#include <http/http.h>
#include <http/message.h>
#include "stream_engine_proxy.h"
///////////////////////////////////////////////////////////////////////////////
struct http::stream_strategy::content_data
{
	content_data()
		: data(0)
	{
	}
	prague::IO<char>	data;
};
///////////////////////////////////////////////////////////////////////////////
http::stream_strategy::stream_strategy(preprocessing* pre_proc, hOBJECT service)
							: generic_(new generic_strategy(pre_proc))
							, service_(new http::stream_engine_proxy(service))
							, assemble_(100)//! отправляем всегда все
							, content_(new content_data())
{
}
///////////////////////////////////////////////////////////////////////////////
http::stream_strategy::stream_strategy(stream_strategy const& other)
							: generic_(new generic_strategy(*other.generic_))
							, service_(new http::stream_engine_proxy(*other.service_))
							, assemble_(100)//! отправляем всегда все
							, content_(new content_data())
{
}
///////////////////////////////////////////////////////////////////////////////
http::stream_strategy::~stream_strategy()
{
	delete content_;
}
///////////////////////////////////////////////////////////////////////////////
http::message *http::stream_strategy::extract_message(http::analyzer& anlz, msg_direction direction, bool is_ssl, bool force)
{
	//!	assert(generic_->pre_process());
	message* msg = generic_->extract_message(anlz, direction, is_ssl, force);
	if(msg && !data_.get())
	{
		data_ = std::auto_ptr<message::build_data>(new message::build_data);
		service_->begin_antivirus();
	}
	if(msg && !proc_data_.get())
	{
		proc_data_ = std::auto_ptr<message::build_data>(new message::build_data);
		proc_data_->length = http::unspecified_length;
		proc_data_->position = msg->max_size() - msg->size();
	}
	//! момент для начала процессирования всегда наступает для потокового движка
	return msg;
}
///////////////////////////////////////////////////////////////////////////////
bool http::stream_strategy::can_process(const http::message &msg) const
{
	return true;
}
///////////////////////////////////////////////////////////////////////////////
void http::stream_strategy::reprocess()
{
	generic_->reprocess();
	data_ = std::auto_ptr<message::build_data>();
	proc_data_ = std::auto_ptr<message::build_data>();
	content_->data.reset(0);
	service_->end_antivirus();
}
///////////////////////////////////////////////////////////////////////////////
http::preprocessing* http::stream_strategy::set(http::preprocessing* preproc)
{
	http::preprocessing* tmp = generic_->pre_process();
	generic_->pre_process(preproc);
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////
http::message const* http::stream_strategy::msg() const { return generic_->current_message(); }
///////////////////////////////////////////////////////////////////////////////
http::service_proxy const& http::stream_strategy::service() const { return *service_; }
///////////////////////////////////////////////////////////////////////////////
http::service_proxy const& http::stream_strategy::pre_av_service() const { return *service_; }
///////////////////////////////////////////////////////////////////////////////
http::message_policy& http::stream_strategy::msg_policy() { return *this; }
///////////////////////////////////////////////////////////////////////////////
http::content_policy& http::stream_strategy::cnt_policy() { return *this; }
///////////////////////////////////////////////////////////////////////////////
http::assemble_policy const& http::stream_strategy::asm_policy() const { return *this; }
///////////////////////////////////////////////////////////////////////////////
http::message* http::stream_strategy::replace(std::auto_ptr<http::message> msg)
{
	return generic_->replace(msg);
}
///////////////////////////////////////////////////////////////////////////////
bool http::stream_strategy::can_complete(http::message const& msg) const
{
	return msg.complete() || generic_->is_forced();
}
///////////////////////////////////////////////////////////////////////////////
bool http::stream_strategy::need_processing(message const& msg) const
{
	return !generic_->need_skip();
}
///////////////////////////////////////////////////////////////////////////////
hIO http::stream_strategy::build(http::message const& msg)
{
	size_t offset(msg.max_size() - msg.size()); 
	proc_data_->position = (offset > data_->position)
							? offset
							: data_->position;
	content_->data.reset(assemble_.full_packet(msg, proc_data_.get()));
	return content_->data.get();
}
///////////////////////////////////////////////////////////////////////////////
hIO http::stream_strategy::replace(http::message& msg, hIO io)
{
	hIO prev = content_->data.release();
	prague::IO<char> tmp(msg.replace(io));
	return prev;
}
///////////////////////////////////////////////////////////////////////////////
hIO http::stream_strategy::assemble(http::message const& msg, bool full) const
{
	return assemble_.full_packet(msg, data_.get());
}
///////////////////////////////////////////////////////////////////////////////
hIO http::stream_strategy::pre_assemble(message const&) const
{
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
bool http::stream_strategy::has_assembled(message const&) const
{
	return message::build_data::has_built(data_.get());
}
///////////////////////////////////////////////////////////////////////////////
http::processing_strategy* http::stream_strategy::clone() const { return new stream_strategy(*this); }
///////////////////////////////////////////////////////////////////////////////