/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	message_builder.h
*		\brief	
*		
*		\author ¬ладислав ќвчарик
*		\date	24.06.2005 14:25:22
*		
*		
*/		
#if !defined(_MESSAGE_BUILDER_INCLUDED_H_)
#define _MESSAGE_BUILDER_INCLUDED_H_
#include <http/message.h>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
struct message_builder
{
	explicit message_builder(size_t percent);
	hIO full_packet(message const&, message::build_data*) const;
	hIO partial_packet(message const&, message::build_data*) const;
	size_t get_percent() const;
private:
	size_t	percent_;	//! какой процент пересобирать
	size_t	min_size_;	//! какой минимальный размер не должен быть пересобран
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_MESSAGE_BUILDER_INCLUDED_H_)