#ifndef TESTHTTP_POST_H
#define TESTHTTP_POST_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestHttpPost: public TestSuiteBase
{
public:

	TestHttpPost(): TestSuiteBase (L"HttpPost"), s6c (&testLog), tCallback (&testLog) {};

	void TestPost ();
	void TestPostViaProxy ();
	void TestServers ();
	void TestPostResponses ();

private:



	TestLog testLog;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};




#endif