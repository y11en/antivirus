/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	skip_app.cpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	28.06.2005 15:11:24
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable : 4275 4786)
#include <Prague/prague.h>

#include <windows.h>

#include "skip_app.h"
#include "message.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace
{
///////////////////////////////////////////////////////////////////////////////
char const mplayer_radio[] = "vnd.ms.wms-hdr.asfv1";//application/vnd.ms.wms-hdr.asfv1
char const mplayer_stream[] = "x-mms-framed";//application/x-mms-framed
char const mplayer_asf[] = "x-ms-asf";//video/x-ms-asf
char const qtplayer_stream[] = "x-rtsp-tunnelled";//application/x-rtsp-tunnelled
char const vvtk_tunnel[] = "x-vvtk-tunnelled";//application/x-vvtk-tunnelled
///////////////////////////////////////////////////////////////////////////////
char winamp_fld[] = "Icy-MetaData";
///////////////////////////////////////////////////////////////////////////////
}//namespace unnamed
///////////////////////////////////////////////////////////////////////////////
http::skip_application::skip_application(http::message& msg)
									: skipped_(false)
{
	msg.apply(*this);//! здесь двойная диспетчерезация
}
///////////////////////////////////////////////////////////////////////////////
void http::skip_application::do_action(http::response_message& msg)
{
	vector_ptr const& subtype = msg.media_subtype();
	char *media_sub_type = _strlwr(&(*subtype)[0]);
	skipped_ = !msg.is_chunked_message() && msg.has_unknown_content_size() || //! не будем копить ответы неизвестного размера,
																			  //! только память без толку расходовать
				(_stricmp(media_sub_type, mplayer_radio) == 0
				|| _stricmp(media_sub_type, mplayer_stream) == 0
				|| _stricmp(media_sub_type, mplayer_asf) == 0
				|| _stricmp(media_sub_type, qtplayer_stream) == 0
				|| _stricmp(media_sub_type, vvtk_tunnel) == 0);
}
///////////////////////////////////////////////////////////////////////////////
void http::skip_application::do_action(http::request_message& msg)
{
	message_headers const& hdrs = msg.headers();
	//! проверяем, что это не запрос от winamp-a
	http::buffer_t fld(&winamp_fld[0], sizeof(winamp_fld) - 1);
	http::vector_ptr val = hdrs.get(fld);
	skipped_ = val.get() != 0;
	//! проверяем, что запрос не есть тунелированное потоковое видиео
	//! Real Time Streaming Protocol (RTSP) RFC2326
	if(!skipped_)
	{
		vector_ptr const& subtype = msg.media_subtype();
		char *media_sub_type = _strlwr(&(*subtype)[0]);
		skipped_ = (_stricmp(media_sub_type, qtplayer_stream) == 0 ||
					_stricmp(media_sub_type, vvtk_tunnel) == 0);
	}
}
///////////////////////////////////////////////////////////////////////////////