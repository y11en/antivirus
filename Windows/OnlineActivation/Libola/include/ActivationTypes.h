#ifndef _AG_ACTIVATIONTYPES_H_
#define _AG_ACTIVATIONTYPES_H_

#include <string>

namespace OnlineActivation
{

/**
 * Application identifier.
 */
struct ApplicationId
{
	/**
	 * Identifier of application.
	 */
    unsigned long	id;
};

/**
 * Customer identifier.
 */
struct CustomerId
{
	/**
	 * Customer support identifier.
	 */
    unsigned long	id;

	/**
	 * Customer password.
	 */
    char			password[8];
};

/**
 * Activation code.
 */
struct ActivationCode
{
	/**
	 * First code part.
	 */
	char part1[5];

	/**
	 * Second code part.
	 */
	char part2[5];

	/**
	 * Third code part.
	 */
	char part3[5];

	/**
	 * Fourth code part.
	 */
	char part4[5];
};

/**
 * Internet access configuration.
 */
struct InternetConfig
{
	enum AccessType
	{
		DIRECT,
		PRECONFIG,
		PROXY
	};

	/**
	 * Internet access type.
	 */
	AccessType access_type;

	/**
	 * Proxy list.
	 * NULL-terminated array of NULL-terminated proxy names or IP addresses.
	 */
	const char** proxy_list;

	/**
	 * Proxy bypass list.
	 * NULL-terminated array of NULL-terminated bypass names or IP addresses.
	 */
	const char** bypass_list;

	/**
	 * Bypass proxy for local addresses.
	 */
	bool bypass_local;
};

/**
 * Activation process configuration.
 */
struct ActivationConfig
{
	/**
	 * Activation URL list.
	 * NULL-terminated array of NULL-terminated activation URLs.
	 */
	const char** url_list;

	/**
	 * Internet configuration.
	 */
	InternetConfig inet_config;
};

} // namespace OnlineActivation

#endif //_AG_ACTIVATIONTYPES_H_
