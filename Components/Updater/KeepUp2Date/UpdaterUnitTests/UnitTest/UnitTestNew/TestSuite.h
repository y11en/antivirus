#ifndef TESTSUITE_H
#define TESTSUITE_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"

#include "HttpServer.h"

#define CREATE_EVENTS(id)\
	cbacks.hReadyEvents[Callbacks::id] = CreateEvent (NULL, false, false, NULL);\
	cbacks.hGoOnEvents[Callbacks::id] = CreateEvent (NULL, false, false, NULL);

#define CLOSE_EVENTS(id)\
	CloseHandle ( cbacks.hReadyEvents[Callbacks::id] );\
	CloseHandle ( cbacks.hGoOnEvents[Callbacks::id] );\
	cbacks.hReadyEvents[Callbacks::id] = INVALID_HANDLE_VALUE;\
	cbacks.hGoOnEvents[Callbacks::id] = INVALID_HANDLE_VALUE;

#define SET_GOON_EVENT(id)\
	SetEvent(cbacks.hGoOnEvents[Callbacks::id]);

#define WAIT_FOR_READY_EVENT(id)\
	WaitForSingleObject(cbacks.hReadyEvents[Callbacks::id], INFINITE);

class TestSuite: public TestSuiteBase
{
public:
	
	TestSuite(): TestSuiteBase (L"Updater") {};

	void Test ();

	//Update
	void TestCheckBlackList ();
	void TestCheckFilesToDownload ();
	void TestCheckIfFileOptional ();
	void TestProcessReceivedFiles ();
	void TestDoubleDownloading ();
	void TestReplaceFileOnReboot ();
	void TestLocalIndexes ();
	void TestSourceIndexesDirectoried ();
	void TestSourceIndexesPlanar ();
	void TestUpdcfgXml ();
	void TestSitesXml ();
	void TestRequestCredentials ();
	void TestUpdateSchedule ();
	void TestUpdateVariables ();
	void TestRefreshIds ();
	void TestRollback ();
	void TestRollbackListCases ();
	void TestExpandStrings ();
	void TestSubEnvrnmnt ();
	void TestCheckCancel ();
	void TestCheckCancelWhileDownloadingGroup ();
	void TestUsingWPADProxy ();
	

	//Retranslation
	void TestRetrCheckFilesToDownload ();
	void TestRetrCheckIfFileOptional ();
	void TestRetrProcessReceivedFiles ();
	void TestRetrDoubleDownloading ();
	void TestRetrReplaceFileOnReboot ();
	void TestRetrLocalIndexes ();
	void TestRetrRollback ();
	void TestRetrRollbackListCases ();
	void TestRetrSourceIndexesDirectoried ();
	void TestRetrSourceIndexesPlanar ();

	//Both tasks
	void TestBothCheckFilesToDownload ();
	void TestBothCheckIfFileOptional ();
	void TestBothReplaceFileOnReboot ();

	void TestDownloadTheSameFiles ();

	//Other
	void TestFtpSourceUrlWithCreds ();
	void TestCaseTT17598 ();
	void TestWhitespaces ();

	//new
	void TestCheckDownloadedFiles ();
	void TestRetrCheckDownloadedFiles ();
	void TestBothCheckDownloadedFiles ();

	void TestCheckPreInstall ();
	void TestLockComponents ();

	void TestAdjustLoginName ();
	void TestFileExistsInTempFolder ();
	void TestFileExistsInTargetFolder ();
	void TestFileExistsInTempTargetFolder ();
	void TestSwitchingURLs ();
	void TestUnableToInit ();
	void TestSwitchingURLsWhileDownloading ();
	void TestSwitchingURLsInCaseOfTimeouts ();

private:
	enum NetError
	{
		TRANSFER_ERROR = 0,
		INTERNAL_ERROR_500,
		TIMEOUT_EXCEEDS,
		ERRORS_COUNT
	};

	void TestCheckCancelWhileDownloading (const wchar_t*, const wchar_t*);
	void TestSwitchingURLsWhileDownloading_Implementation (const NetError& reason);
	void EmulateNetError (HttpServer& hs, const NetError& error, const wstring& filename, const string& reqname, ...);
	void TransferFile (HttpServer& hs, const wstring& filename, const string& reqname);
	
};


















#endif