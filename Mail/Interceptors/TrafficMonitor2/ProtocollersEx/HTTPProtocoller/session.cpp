/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	session.cpp
*		\brief	реализация сессии для связи с транспортным слоем (socket layer)
*		
*		\author Владислав Овчарик
*		\date	29.04.2005 15:02:39
*		
*		
*/		
#define NOMINMAX
#include <Prague/prague.h>
#include <Mail/structs/s_httpscan.h>

#include <windows.h>
#include <tchar.h>
#include "session.h"
#include <Prague/wrappers/io.hpp>
#include <Prague/wrappers/io_helpers.h>
#include <http/message.h>
#include <fstream>
#include <http/dump.hpp>
///////////////////////////////////////////////////////////////////////////////
http::session::session(hOBJECT monitor, void* ctx)
					: traffic_monitor_(monitor)
					, ctx_(ctx)
{
}
///////////////////////////////////////////////////////////////////////////////
http::session::~session()
{
}
///////////////////////////////////////////////////////////////////////////////
unsigned long http::session::client_id() const
{
	tp::session_t* s = static_cast<tp::session_t*>(ctx_);
	return s ? s->dwClientPID : 0;
}
///////////////////////////////////////////////////////////////////////////////
bool http::session::skip_that(char const* url, char const* content_type)
{
	tp::anybody_here_t skip_msg;
	skip_msg.assign(*static_cast<tp::session_t*>(ctx_), false);
	skip_msg.hProtocolType = tp::po_HTTP;
	skip_msg.szObjectName = url;
	if(content_type)
		skip_msg.szObjectType = content_type;
	tERROR error = traffic_monitor_->sysSendMsg(pmc_HTTPSCAN_PROCESS, 0, 0, &skip_msg, SER_SENDMSG_PSIZE);
	return error == errOK_DECIDED;
}
///////////////////////////////////////////////////////////////////////////////
size_t http::session::do_send(http::message_type type, hIO packet, state st)
{
	if(dump<2>::need_dump())//! сбрасываем пакет в дамп
	{
		prague::WeakIO<char> p(packet);
		dump<2>("!packet", this).write(p.io().begin(), p.io().end());
	}
	//! Обсылаем объект обратно в сессию
	tp::process_msg_t process_msg;
	process_msg.assign(*static_cast<tp::session_t*>(ctx_), false);
	process_msg.dsDataTarget = type == request_type ? tp::dsServer : tp::dsClient;
	process_msg.psStatus = static_cast<tp::process_status_t>(st);
	process_msg.hData = packet;
	tERROR error = traffic_monitor_->sysSendMsg(pmcTRAFFIC_PROTOCOLLER
									, pmPROCESS_FINISHED
									, 0, &process_msg, SER_SENDMSG_PSIZE);
	//todo: здесь нужно будет обработать ошибку и выбросить исключение
	return (size_t)prague::IO<char>::size(packet);
}
///////////////////////////////////////////////////////////////////////////////
size_t http::session::send(http::message_type type, hIO packet)
{
	return do_send(type, packet);
}
///////////////////////////////////////////////////////////////////////////////
size_t http::session::stop(http::message_type type, hIO packet)
{
	/**
	 * для того что бы trafficmonitor2 перешел в режим детектирования,
	 * установив статус обработки LikelyMine
	 */
	return do_send(type, packet, unknown_);
}
///////////////////////////////////////////////////////////////////////////////
size_t http::session::close(http::message_type type, hIO packet)
{
	return do_send(type, packet, close_);
}
///////////////////////////////////////////////////////////////////////////////
size_t http::session::break_off(http::message_type type)
{
	prague::IO<char> packet(prague::create_temp_io());
	return do_send(type, packet.get(), close_);
}
///////////////////////////////////////////////////////////////////////////////