#ifndef TESTHTTP_TRANSPORT_H
#define TESTHTTP_TRANSPORT_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestHttpTransport: public TestSuiteBase
{
public:

	TestHttpTransport(): TestSuiteBase (L"HttpTransport"), s6c (&testLog), tCallback (&testLog) {};

	void TestGetFile ();
	void TestConnection ();
	void TestGetFileWithRest ();
	void TestGetFileWithRestCode200 ();
	void TestGetFileWithRestCode404 ();
	void TestGetFileWithRestCode416 ();
	void TestGetFileNoContentLength ();
	void TestPathes ();
	void TestConnectionClosingTimeout ();
	void TestGetFileContentLength ();

	void TestReusingTheSameServer ();
	void TestReusingServers ();

	void TestRedirectionCodes ();
	void TestRedirectionCycling ();
	void TestRedirectionCases ();

	void TestGetFile_IpV6 ();
private:

	void TestRedirectionCodes_impl (const int code);

	TestLog testLog;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};


#endif