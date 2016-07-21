/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	message_builder.cpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	24.06.2005 15:03:14
*		
*		
*/		
#define NOMINMAX
#include <windows.h>
#include <Prague/prague.h>
#include <Prague/wrappers/io_helpers.h>
#include <cassert>
#include "message_builder.h"
///////////////////////////////////////////////////////////////////////////////
http::message_builder::message_builder(size_t percent)
		: percent_(percent)
		, min_size_(0x4)//по умолчанию отправляем по 4 байта
{
}
///////////////////////////////////////////////////////////////////////////////
hIO http::message_builder::full_packet(http::message const& msg, http::message::build_data* bd) const
{
	if(bd)
		bd->length = unspecified_length;//! снимаем ограничения по длине, собираем максимально возможный пакет
	return msg.assemble(bd);
}
///////////////////////////////////////////////////////////////////////////////
hIO http::message_builder::partial_packet(http::message const& msg, http::message::build_data* bd) const
{
	if(bd)
	{
		size_t length((msg.max_size() - bd->position) * percent_ / 100);
		if(!length)
			length = min_size_;
		bd->length = length;
	}
	return msg.assemble(bd);
}

size_t http::message_builder::get_percent() const
{
	return percent_;
}
///////////////////////////////////////////////////////////////////////////////