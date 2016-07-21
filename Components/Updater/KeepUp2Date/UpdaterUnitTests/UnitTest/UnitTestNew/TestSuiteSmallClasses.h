#ifndef TESTSUITE_SMALL_CLASSES_H
#define TESTSUITE_SMALL_CLASSES_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestSuiteSmallClasses: public TestSuiteBase
{
public:

	TestSuiteSmallClasses(): TestSuiteBase (L"Others"), s6c (&testLog), tCallback (&testLog) {};

	void TestLocalFile ();
	void TestIndexFileXMLParser ();

private:
	TestLog testLog;
	//TestJournalInterface testJournal;
	TransactionCallback tCallback;
	Signature6Checker s6c;

	FileVector m_files;

};

#endif