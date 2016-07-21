#ifndef TESTSUITE_ADDRESS_H
#define TESTSUITE_ADDRESS_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestSuiteAddress: public TestSuiteBase
{
public:

	TestSuiteAddress(): TestSuiteBase (L"Address"), s6c (&testLog), tCallback (&testLog) {};

	void TestAddress ();
	void TestAddress_IpV6 ();

private:
	TestLog testLog;
	//TestJournalInterface testJournal;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};


#endif