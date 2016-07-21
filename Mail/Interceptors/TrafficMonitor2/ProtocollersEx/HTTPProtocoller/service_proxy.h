/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	service_proxy.h
*		\brief	abstract interface for anti-virus, anti-fishing, anti-advertisement
*		
*		\author Vladislav Ovcharik
*		\date	28.09.2005 9:54:52
*		
*		
*/		
#if !defined(SERVICE_PROXY_H_INCLUDED)
#define SERVICE_PROXY_H_INCLUDED
#include <memory>
#include <vector>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
struct service_observer;
class message;

///////////////////////////////////////////////////////////////////////////////
enum avs_result
{
	object_is_ok		= 0x000,
	object_detected		= 0x001,
	object_detected_sure= 0x002,
	object_was_deleted	= 0x004,
	object_disinfected	= 0x008,
	object_some_skipped	= 0x010,
	object_some_changes	= 0x020,
	object_sfdb_known	= 0x040,
	object_ntfs_known	= 0x080,
	object_pre_processed= 0x100,
	object_canceled		= 0x200,
	object_unchanged	= 0x400,
	object_external_detect = 0x800,
	object_some_allowed = 0x1000
};
///////////////////////////////////////////////////////////////////////////////
/**
 * использовать только для передачи ответсвенности из вызываемого кода в вызывающий
 */
struct antivirus_result
{
	antivirus_result()
		: result(object_is_ok)
	{
	}
	explicit antivirus_result(avs_result r)
						: result(r)
	{
	}
	avs_result result;
	std::auto_ptr<std::vector<char> > vir_name;
};
///////////////////////////////////////////////////////////////////////////////
struct __declspec(novtable) service_proxy
{
	/**
	 * return true if url is not blocked
	 */
	virtual bool antiphishing(unsigned int client_id, char const* url) const = 0;
	/**
	 *
	 */
	virtual bool adblock(unsigned int client_id, char const* url) const = 0;
	/**
	 *
	 */
	virtual bool webav(unsigned int client_id, char const* url) const = 0;
	/**
	*
	*/
	///
	/// Parental control analyze
	///
	virtual bool parctl
		(
			unsigned int client_id, 
			hIO data,
			const message* msg,
            char const* url, 
			service_observer* observer = 0,
            int direction = 0
		) const = 0;
	/**
	*
	*/
	virtual antivirus_result antivirus(unsigned int client_id, hIO data
									, char const* url, service_observer* observer = 0
									, int direction = 0, bool is_gzip = false) const = 0;
	/***/
	virtual ~service_proxy() {}
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(SERVICE_PROXY_H_INCLUDED)