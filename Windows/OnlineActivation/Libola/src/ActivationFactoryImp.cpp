#include "stdafx.h"

#include <sstream>
#include <boost/shared_ptr.hpp>

#include "../include/Request.h"
#include "../include/ActivationTypes.h"
#include "../include/ActivationErrors.h"
#include "../include/ActivationFactory.h"

#include "InternetSession.h"
#include "RequestData.h"
#include "RecurrentRequest.h"
#include "ActivationFactoryImp.h"

namespace OnlineActivation
{
///////////////////////////////////////////////////////////////////////////////
namespace
{
	const char* REQUEST_ID_FIELD		= "REQUEST_ID";
	const char* APP_ID_FIELD			= "APP_ID";
	const char* CUSTOMER_ID_FIELD		= "CUSTOMER_ID";
	const char* CUSTOMER_PASS_FIELD		= "CUSTOMER_PASS";
	const char* ACT_CODE_FIELD			= "ACT_CODE";
	const char* EQUAL_SIGN				= "=";
	const char* DASH_SIGN				= "-";
	const char* FIELD_DELIMITER			= "&";

	const char* USER_AGENT_ACTIVATION	= "Online Activation Library v2.0";
	const char* POST_HEADERS			= "Content-Type: application/x-www-form-urlencoded";
}
///////////////////////////////////////////////////////////////////////////////
std::string GenerateGUID()
{
	class rpc_string
	{
	public:
		rpc_string()
			: p(0) {}
		~rpc_string()
			{ p ? RpcStringFreeA(&p) : void(0); }
		unsigned char* get() const
			{ return p; }
		unsigned char** operator& ()
			{ return &p; }
	private:
		unsigned char* p;
	};

	UUID uid = { 0 };
	UuidCreate(&uid);
	rpc_string suid;
	if (UuidToStringA(&uid, &suid) != RPC_S_OK)
		throw SystemError(ERROR_OUTOFMEMORY);
	return std::string(reinterpret_cast<char*>(suid.get()));
}
///////////////////////////////////////////////////////////////////////////////
std::string SerializeActivationCode(const ActivationCode& code)
{
	std::string s;
	s.append(code.part1, sizeof(code.part1));
	s.append(DASH_SIGN);
	s.append(code.part2, sizeof(code.part2));
	s.append(DASH_SIGN);
	s.append(code.part3, sizeof(code.part3));
	s.append(DASH_SIGN);
	s.append(code.part4, sizeof(code.part4));
	return s;
}
///////////////////////////////////////////////////////////////////////////////
void VerifyActivationCode(const std::string& code)
{
	_ASSERTE(!code.empty() && "Activation code must be filled");
	// the alphabet
	static const char* ABC = "123456789ABCDEFGHJKMNPQRSTUVWXYZ";
	// the last symbol is a checksum
	std::string::const_iterator 
		begin = code.begin(), end = code.end() - 1;
	unsigned long S = 0;
	// calculate the checksum
	for (std::string::const_iterator it = begin; it != end; ++it)
	{
		if (const char* jt = strchr(ABC, toupper(*it)))
		{
			// offset in the alphabet
			const size_t O = jt - ABC + 1;
			// coefficient 1 for odd, 3 for even
			const unsigned long C = ((it - begin) % 2) ? 3 : 1;
			// add to sum
			S += O * C;
		}
	}
	// modulus
	const size_t M = strlen(ABC);
	// checksum symbol must be equal to checksum
	if (ABC[(M - S % M) % M] != toupper(*end))
		throw LogicalError(LogicalError::INVALID_ACTIVATION_CODE);
}
///////////////////////////////////////////////////////////////////////////////
void VerifyCustomerId(const CustomerId& id)
{
	// customer id must be greater than 1000
	if (id.id < 1000)
		throw LogicalError(LogicalError::INVALID_CUSTOMER_ID);
	// check for valid symbols in the password
	for (size_t i = 0; i < sizeof(id.password); ++i)
	{
		const char c = toupper(id.password[i]);
		// password char must be in [A..Za..z_] range
		if (c >= 'A' || c <= 'Z' || c == '_')
			continue;
		throw LogicalError(LogicalError::INVALID_CUSTOMER_PASSWORD);
	}
}
///////////////////////////////////////////////////////////////////////////////
ActivationFactoryImp::ActivationFactoryImp(const ActivationConfig& config)
{
	// url list must be filled
	if (!(config.url_list && config.url_list[0]))
		throw LogicalError(LogicalError::INVALID_PARAMETER);
	URL_COMPONENTSA urlComp = { 0 };
	// fill activation url list
	for(const char** it = config.url_list; *it; ++it)
	{
		memset(&urlComp, 0, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(URL_COMPONENTS);
		urlComp.dwHostNameLength = TRUE;
		urlComp.dwUrlPathLength = TRUE;
		// parse activation URL
		if(!InternetCrackUrlA(*it, strlen(*it), 0, &urlComp))
			throw LogicalError(LogicalError::INVALID_PARAMETER);
		m_urls.push_back(*it);
	}
	// analyze internet configuration
	switch (config.inet_config.access_type)
	{
	// direct access to internet
	case InternetConfig::DIRECT:
		m_pSession = boost::shared_ptr<InternetSession>
			(new InternetSession(USER_AGENT_ACTIVATION, INTERNET_OPEN_TYPE_DIRECT, 0, 0));
		break;
	// configuration retrieved from the registry
	case InternetConfig::PRECONFIG:
		m_pSession = boost::shared_ptr<InternetSession>
			(new InternetSession(USER_AGENT_ACTIVATION, INTERNET_OPEN_TYPE_PRECONFIG, 0, 0));
		break;
	case InternetConfig::PROXY:
		// proxy list must be filled
		if (config.inet_config.proxy_list && config.inet_config.proxy_list[0])
		{
			// proxy list
			std::string proxies;
			// fill proxy list
			for (const char** it = config.inet_config.proxy_list; *it; ++it)
			{
				proxies.append(*it);
				// space delimited list
				proxies.push_back(' ');
			}
			// proxy bypass list
			std::string bypass;
			// if proxy bypass list specified
			if (config.inet_config.bypass_list && config.inet_config.bypass_list[0])
			{
				// fill proxy bypass list
				for (const char** it = config.inet_config.proxy_list; *it; ++it)
				{
					bypass.append(*it);
					// space delimited list
					bypass.push_back(' ');
				}
			}
			// if bypass proxy for local addresses is specified
			if (config.inet_config.bypass_local)
				bypass.append("<local>");
			_ASSERTE(!proxies.empty() && "Proxy list must contain at least one entry");
			// create internet session object
			m_pSession = boost::shared_ptr<InternetSession>
				(new InternetSession(USER_AGENT_ACTIVATION, INTERNET_OPEN_TYPE_PROXY, 
										proxies.c_str(), bypass.empty() ? 0 : bypass.c_str()));
		}
		// proxy list is empty
		else
			throw LogicalError(LogicalError::INVALID_PARAMETER);
		break;
	default:
		throw LogicalError(LogicalError::INVALID_PARAMETER);
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Request> ActivationFactoryImp::ComposeRequest
(
	const ApplicationId& appId,
	const ActivationCode& actCode
)
{
	const std::string& sActivationCode = SerializeActivationCode(actCode);
	// verify checksum of activation code
	VerifyActivationCode(sActivationCode);
	std::ostringstream os;
	// serialize request fields
	os	<< REQUEST_ID_FIELD	<< EQUAL_SIGN	<< GenerateGUID()	<< FIELD_DELIMITER 
		<< APP_ID_FIELD		<< EQUAL_SIGN	<< appId.id			<< FIELD_DELIMITER 
		<< ACT_CODE_FIELD	<< EQUAL_SIGN	<< sActivationCode;
	// compose request data
	boost::shared_ptr<RequestData> pData
		(new RequestData(POST_HEADERS, os.str().c_str(), os.str().length()));
	// create initial activation request
	return RecurrentRequest::CreateInitialRequest(m_pSession, pData, m_urls);
}
///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Request> ActivationFactoryImp::ComposeRequest
(
	const ApplicationId& appId,
	const ActivationCode& actCode,
	const CustomerId& custId
)
{
	const std::string& sActivationCode = SerializeActivationCode(actCode);
	// verify checksum of activation code
	VerifyActivationCode(sActivationCode);
	// verify customer identifier
	VerifyCustomerId(custId);
	std::ostringstream os;
	// serialize request fields
	os	<< REQUEST_ID_FIELD		<< EQUAL_SIGN	<< GenerateGUID()	<< FIELD_DELIMITER 
		<< APP_ID_FIELD			<< EQUAL_SIGN	<< appId.id			<< FIELD_DELIMITER 
		<< ACT_CODE_FIELD		<< EQUAL_SIGN	<< sActivationCode	<< FIELD_DELIMITER
		<< CUSTOMER_ID_FIELD	<< EQUAL_SIGN	<< custId.id		<< FIELD_DELIMITER
		<< CUSTOMER_PASS_FIELD	<< EQUAL_SIGN	<< std::string(custId.password, sizeof(custId.password));
	// compose request data
	boost::shared_ptr<RequestData> pData
		(new RequestData(POST_HEADERS, os.str().c_str(), os.str().length()));
	// create initial activation request
	return RecurrentRequest::CreateInitialRequest(m_pSession, pData, m_urls);
}
///////////////////////////////////////////////////////////////////////////////
} // namespace OnlineActivation
///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<OnlineActivation::ActivationFactory> 
	CreateActivationFactory(const OnlineActivation::ActivationConfig& config)
{
	using namespace OnlineActivation;
	boost::shared_ptr<ActivationFactory> pFactory(new ActivationFactoryImp(config));
	return pFactory;
}
///////////////////////////////////////////////////////////////////////////////
