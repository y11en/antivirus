/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	generic_strategy.cpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	29.09.2005 10:24:45
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable : 4275 4786)
#include <windows.h>
#include <Prague/prague.h>
#include <http/analyzerimpl.h>
#include <Prague/wrappers/io_helpers.h>
#include <http/takeurl.h>
#include "generic_strategy.h"
#include "preprocessing.h"
///////////////////////////////////////////////////////////////////////////////
http::generic_strategy::generic_strategy(preprocessing* preproc)
								: preprocess_(preproc)
								, msg_()
								, url_(1, '\x0')
								, state_(http::unspecified)
								, forced_(false)

{
}
///////////////////////////////////////////////////////////////////////////////
http::generic_strategy::generic_strategy(http::generic_strategy const& other)
								: preprocess_(other.preprocess_)
								, msg_()
								, url_(1, '\x0')
								, state_(http::unspecified)
								, forced_(false)
{
}
///////////////////////////////////////////////////////////////////////////////
http::generic_strategy::~generic_strategy()
{
}
///////////////////////////////////////////////////////////////////////////////
http::message* http::generic_strategy::extract_message(analyzer& anlz, msg_direction direction, bool is_ssl, bool force)
{
	PR_ASSERT(!(msg_.get() && msg_->complete()));
	forced_ = force;
	bool no_msg(msg_.get() == 0);
	if(no_msg)
		msg_ = anlz.extract(force);
	else
		anlz.extract(*msg_.get());
	//! получаем URL
	if(no_msg && msg_.get() && msg_->type() == request_type)
	{
		take_url url(*msg_, is_ssl);
		if(!url.get().empty())
			std::vector<char>(url.get()).swap(url_);
	}
	//! устанавливаем нужно ли по запрашиваемому URL делать какие либо проверки
	//! или если пришел пакет, в теле которого потоковое видео или контент, который мы не
	//! не можем обработать, то пропускаем этот пакет без каких либо проверок и задержек.
	if(state_ == http::unspecified && msg_.get())
	{
		state_ = ((preprocess_ && (preprocess_->skip_that(&url_[0]) || preprocess_->skip_message(*msg_)))
					? http::skip_process : http::need_process);
	}

	return msg_.get();
}
///////////////////////////////////////////////////////////////////////////////
http::preprocessing* http::generic_strategy::pre_process() { return preprocess_; }
///////////////////////////////////////////////////////////////////////////////
void http::generic_strategy::pre_process(preprocessing* pre_proc) { preprocess_ = pre_proc;}
///////////////////////////////////////////////////////////////////////////////
http::message* http::generic_strategy::replace(std::auto_ptr<message> msg)
{
	if(msg.get())//! только не нулевые
		msg_ = msg;
	return msg_.get();
}
///////////////////////////////////////////////////////////////////////////////
void http::generic_strategy::reprocess()
{
	msg_ = std::auto_ptr<message>();
	state_ = http::unspecified;
	forced_ = false;
}
///////////////////////////////////////////////////////////////////////////////
bool http::generic_strategy::is_forced() const { return forced_; }
///////////////////////////////////////////////////////////////////////////////
bool http::generic_strategy::need_skip() const { return state_ == http::skip_process; }
///////////////////////////////////////////////////////////////////////////////
http::message const* http::generic_strategy::current_message() const { return msg_.get(); }
///////////////////////////////////////////////////////////////////////////////