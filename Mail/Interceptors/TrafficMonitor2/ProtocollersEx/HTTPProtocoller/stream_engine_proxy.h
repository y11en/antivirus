/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	stream_engine_proxy.h
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	29.09.2005 19:19:50
*		
*		
*/		
#if !defined(_STREAM_ENGINE_PROXY_H_INCLUDED)
#define _STREAM_ENGINE_PROXY_H_INCLUDED
#include "service_proxy.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
class stream_engine_proxy
			: public generic_service_proxy
{
public:
	explicit stream_engine_proxy(hOBJECT service);
	stream_engine_proxy(stream_engine_proxy const& other);
	virtual ~stream_engine_proxy();
	//! send data to antivirus processing
	virtual antivirus_result antivirus(unsigned int client_id, hIO data
									, char const* url, service_observer* observer = 0
									, int direction = 0, bool is_gzip = false) const;
	//! begin antivirus processing
	void begin_antivirus();
	//! end antivirus processing
	void end_antivirus();
private:
	void*	stm_ctx_;	//! контекст потокового движка
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_STREAM_ENGINE_PROXY_H_INCLUDED)