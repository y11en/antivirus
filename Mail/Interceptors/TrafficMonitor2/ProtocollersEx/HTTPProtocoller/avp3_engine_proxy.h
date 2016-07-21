/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	avp3_engine_proxy.h
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	29.09.2005 19:20:01
*		
*		
*/		
#if !defined(_AVP3_ENGINE_PROXY_H_INCLUDED)
#define _AVP3_ENGINE_PROXY_H_INCLUDED
#include "service_proxy.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
class avp3_engine_proxy
			: public generic_service_proxy
{
public:
	explicit avp3_engine_proxy(hOBJECT service);
	avp3_engine_proxy(avp3_engine_proxy const&);
	virtual ~avp3_engine_proxy();
	/**
	 * запросить antivirus-ный сервис и передать данные на обработку
	 * client_id	-	client identifier(process ID)
	 * data		-	data for process on the requested service
	 * url			-	name or description of data. it can be file name or url
	 */
	virtual antivirus_result antivirus(unsigned int client_id, hIO data
									, char const* url, service_observer* observer = 0
									, int direction = 0, bool is_gzip = false) const;
private:
	/** callback need to process YIELD from av-engine*/
	static unsigned long scan_processing(hOBJECT _this, unsigned long msg_cls
										, unsigned long msg_id, hOBJECT obj, hOBJECT ctx
										, hOBJECT receive_point, void* pbuff
										, unsigned long* blen);
private:
	hOBJECT receiver_;	//! need to realize callback mechanism
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_AVP3_ENGINE_PROXY_H_INCLUDED)
