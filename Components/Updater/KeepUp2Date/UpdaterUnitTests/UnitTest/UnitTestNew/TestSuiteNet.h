#ifndef TESTSUITE_NET_H
#define TESTSUITE_NET_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"

class TestSuiteNet: public TestSuiteBase
{
public:

	TestSuiteNet(): TestSuiteBase (L"Network Layer") {};

//Basic
	void TestBasicAuthorizationSuccess ();
	void TestBasicAuthorizationNotSupported ();
	void TestBasicAuthorizationWrongCredentials ();
	void TestBasicAuthorizationAccessDenied ();
//NTLM
	void TestNtlmAuthorizationSuccess ();
	void TestNtlmAuthorizationNotSupported ();
	void TestNtlmAuthorizationAccessDenied ();
//NTLM with creds	
	void TestNtlmAuthorizationWithCredsSuccess ();
	void TestNtlmAuthorizationWithCredsNotSupported ();
	void TestNtlmAuthorizationWithCredsWrongCredentials ();
	void TestNtlmAuthorizationWithCredsAccessDenied ();
//No auth
	void TestNoAuthAuthorizationSuccess ();
	void TestNoAuthAuthorizationNotSupported ();
	void TestNoAuthAuthorizationAccessDenied ();
//
	void TestOnlyNtlmAuthorization ();
	void TestOnlyNtlmAuthorizationWithCreds ();
	void TestOnlyBasicAuthorization ();
	void TestOnlyNoAuthAuthorization ();

//Http
	void TestHttpPortSpecification ();
	void TestHttpAuthorization ();
	void TestHttpConfigurationTest ();
	void TestHttpRelativePath ();
	void TestHttpRegetting ();

//Ftp
	void TestFtpPortSpecification ();
	void TestFtpAuthorization ();
	void TestFtpConfigurationTest ();
	void TestFtpOverHttp ();
	void TestFtpRelativePath ();
	void TestFtpRegetting ();

//Proxy
	void TestProxyConfigurationTest ();
	
//Misc
	void TestProxy_172_16_10_108 ();
	void TestFtpActivePassive ();
	void TestRegettingFailed ();
	void TestProxyCredentialsCache ();
	void TestBypassProxyForLocalAddresses ();
	void TestWpadProxyDetectResult ();

//IpV6

	void TestHttpIpV6 ();
	void TestFtpIpV6 ();
	void TestShareIpV6 ();
	void TestBypassProxyForLocalAddresses_IpV6 ();
	void TestWpadProxyDetectResult_IpV6 ();
};






#endif