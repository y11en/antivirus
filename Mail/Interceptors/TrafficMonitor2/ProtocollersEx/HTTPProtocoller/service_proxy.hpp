/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	service_proxy.hpp
*		\brief	реализация класса, представляющего на стороне http монитора 
*				представителя, реализующего сервисы анитирирусной проверки,
*				анти-фишинга и т.д.
*		
*		\author Владислав Овчарик
*		\date	11.05.2005 16:41:40
*		
*		
*/		
#if !defined(_SERVICE_PROXY_INCLUDED_HPP_)
#define _SERVICE_PROXY_INCLUDED_HPP_
///////////////////////////////////////////////////////////////////////////////
#include <Mail/structs/s_httpscan.h>
#include <ParentalControl/structs/s_urlflt.h>

#include "service_proxy.h"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	представитель business logic. предоставляет сервисы анти-вирусной проверки
 *				сервисы анити-фишига и т.д.
 */
class generic_service_proxy
			: public service_proxy
{
public:
	explicit generic_service_proxy(hOBJECT service);
	/***/
	generic_service_proxy(generic_service_proxy const&);
	//! client_id	-	client identifier(process ID)
	//! url			-	name or description of data. it can be file name or url
	//! return true if URL is permitted
	virtual bool antiphishing(unsigned int client_id, char const* url) const;
	//! client_id	-	client identifier(process ID)
	//! url			-	name or description of data. it can be file name or url
	//! return true if URL is permitted
	virtual bool adblock(unsigned int client_id, char const* url) const;
	//! client_id	-	client identifier(process ID)
	//! url			-	name or description of data. it can be file name or url
	//! return true if URL is permitted
	virtual bool webav(unsigned int client_id, char const* url) const;
	///
	/// Parental control analyze
	///
	//! client_id	-	client identifier(process ID)
	//! url			-	name or description of data. it can be file name or url
	//! return true if URL is permitted
    virtual bool parctl
		( 
			unsigned int client_id,
            hIO data,
			const message* msg,
            char const* url,
            service_observer* observer = 0,
            int direction = 0 
		) const;
protected:
	virtual ~generic_service_proxy() { }
protected:
	hOBJECT service_;
};
///////////////////////////////////////////////////////////////////////////////
}//namesapce htpp
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_SERVICE_PROXY_INCLUDED_HPP_)