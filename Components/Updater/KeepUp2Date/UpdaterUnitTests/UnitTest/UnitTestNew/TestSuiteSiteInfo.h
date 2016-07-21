#ifndef TESTSUITE_SITEINFO_H
#define TESTSUITE_SITEINFO_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestSuiteSiteInfo: public TestSuiteBase
{
public:

	TestSuiteSiteInfo(): TestSuiteBase (L"SiteInfo"), s6c (&testLog), tCallback (&testLog) {};

	void TestWeight ();
	void TestUrl ();
	void TestDiff ();
	void TestRegion ();
	void TestTags ();
	void TestSorting ();

private:
	TestLog testLog;
	//TestJournalInterface testJournal;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};


#endif