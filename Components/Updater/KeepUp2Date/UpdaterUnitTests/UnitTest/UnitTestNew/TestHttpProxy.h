#ifndef TESTHTTP_PROXY_H
#define TESTHTTP_PROXY_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestHttpProxy: public TestSuiteBase
{
public:

	TestHttpProxy(): TestSuiteBase (L"HttpProxy"), s6c (&testLog), tCallback (&testLog) {};

	void TestBasicAuth ();
	void TestNoAuth ();
	void TestNtlmAuth ();

	void TestConnectionClosingTimeout ();
	void TestFtpOverHttp ();

	void TestReusingServersThroughProxy ();
	void TestSwitchingURL_UseProxyCases ();
	void TestSwitchingURL_UseWPAD ();

	void TestRequestingAuthInformation ();
	void TestEmptyAuthListMode ();

	void TestSpecialProxyCases_Redirect ();
	void TestSpecialProxyCases_404 ();

	void TestExhaustingAuthTypes ();

	void TestGetFile_IpV6 ();

	void TestSpecialProxyCases ();
	void TestSpecialProxyCases_ConstCancel ();
	void TestSpecialProxyCases_CalculatedDiffs ();
	void TestSpecialProxyCases_Code407 ();

private:



	TestLog testLog;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};




#endif