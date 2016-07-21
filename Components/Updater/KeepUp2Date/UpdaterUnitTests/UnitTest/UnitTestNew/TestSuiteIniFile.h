#ifndef TESTSUITE_INIFILE_H
#define TESTSUITE_INIFILE_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestSuiteIniFile: public TestSuiteBase
{
public:

	TestSuiteIniFile(): TestSuiteBase (L"IniFile"), s6c (&testLog), tCallback (&testLog) {};

	void TestCommon ();
	void TestIncorrectFormat ();
	void TestRandFormat ();

private:
	TestLog testLog;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};

#endif