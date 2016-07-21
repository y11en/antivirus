#ifndef TESTFTP_TRANSPORT_H
#define TESTFTP_TRANSPORT_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"

class TestFtpTransport: public TestSuiteBase
{
public:

	TestFtpTransport(): TestSuiteBase (L"FtpTransport"), s6c (&testLog), tCallback (&testLog) {};

	void TestWrongAuth ();
	void TestUserPassword ();
	void TestPassiveMode ();
	void TestActiveMode ();
	void TestErrorRetrFile ();
	void TestGettingFilePassive ();
	void TestGettingFileActive ();
	void TestGettingFileWithPath ();
	void TestGetting2Files ();
	void TestUsingMultilineReplies ();
	void TestUsingMultilineRepliesAdv ();
	void TestGettingFileWithRest ();
	void TestGettingFileWithoutRest ();
	void TestAbortTransferringClient ();
	void TestAbortTransferringServer ();
	void TestTimeouts ();
	void TestTimeoutAfterTransferringData ();

	void TestReusingSessionPassive ();
	void TestReusingSessionActive ();
	void TestReusingSessionDifferentServers ();
	void TestTryActiveIfPassiveFails ();

	void TestAuthorizationCases ();

	void TestExtendedModeIp4_Active ();
	void TestExtendedModeIp4_Passive ();

	void TestExtendedModeIp6_Active ();
	void TestExtendedModeIp6_Passive ();

	void TestIpV6_Resolving ();

	void Draft ();

private:
	TestLog testLog;
	TransactionCallback tCallback;
	Signature6Checker s6c;

};


#endif