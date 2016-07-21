#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"

#include "TestSuite.h"
#include "TestSuiteNet.h"
#include "TestSuiteTransaction.h"
#include "TestSuiteSmallClasses.h"
#include "TestSuiteProxyDetector.h"
#include "TestSuiteAddress.h"
#include "TestSuiteSiteInfo.h"
#include "TestFtpTransport.h"
#include "TestHttpTransport.h"
#include "TestHttpProxy.h"
#include "TestHttpPost.h"
#include "TestSuiteIniFile.h"

#include "logger.h"

#define TEST_UPD true
#define TEST_NET true
#define TEST_TRANS true
#define TEST_OTHERS true
#define TEST_PROXY_DET true
#define TEST_ADDRESS true
#define TEST_SITEINFO true
#define TEST_TRANSPORT true
#define TEST_INIFILE true
#define TEST_HTTPPOST true



test_suite* init_unit_test_suite (int argc, char * argv[])
{
	test_suite* test = BOOST_TEST_SUITE ("Unit test Shell");
	
	if (TEST_UPD)
	{
		shared_ptr<TestSuite> ts (new TestSuite ());
		//Update
	/*
	
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckBlackList, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckFilesToDownload, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckIfFileOptional, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestProcessReceivedFiles, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestReplaceFileOnReboot, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestDoubleDownloading, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestLocalIndexes, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSourceIndexesDirectoried, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSourceIndexesPlanar, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSitesXml, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestUpdcfgXml, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRequestCredentials, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestUpdateSchedule, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestUpdateVariables, ts));
		//------------------no longer actual ---- test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRefreshIds, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRollback, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRollbackListCases, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestExpandStrings, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSubEnvrnmnt, ts));

		//Retranslation
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrCheckFilesToDownload, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrCheckIfFileOptional, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrProcessReceivedFiles, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrReplaceFileOnReboot, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrDoubleDownloading, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrLocalIndexes, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrRollback, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrRollbackListCases, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrSourceIndexesDirectoried, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrSourceIndexesPlanar, ts));

		
		//Both
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestBothCheckFilesToDownload, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestBothCheckIfFileOptional, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestDownloadTheSameFiles, ts));

		//other
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestFtpSourceUrlWithCreds, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestWhitespaces, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCaseTT17598, ts));

		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckCancel, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckCancelWhileDownloadingGroup, ts));

		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestUsingWPADProxy, ts));
		
		//new
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckDownloadedFiles, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestRetrCheckDownloadedFiles, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestBothCheckDownloadedFiles, ts));

		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestCheckPreInstall, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestLockComponents, ts));
		
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestAdjustLoginName, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestFileExistsInTempFolder, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestFileExistsInTargetFolder, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestFileExistsInTempTargetFolder, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSwitchingURLs, ts));
		test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestUnableToInit, ts));
*/		
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSwitchingURLsWhileDownloading, ts));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuite::TestSwitchingURLsInCaseOfTimeouts, ts));
	}
	

	if (TEST_NET)
	{

			shared_ptr<TestSuiteNet> tsn (new TestSuiteNet ());
/*		//BASIC
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestBasicAuthorizationSuccess, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestBasicAuthorizationNotSupported, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestBasicAuthorizationWrongCredentials, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestBasicAuthorizationAccessDenied, tsn));
		//NTLM
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationSuccess, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationNotSupported, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationAccessDenied, tsn));
		//NTLM with credentials
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationWithCredsSuccess, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationWithCredsNotSupported, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationWithCredsWrongCredentials, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNtlmAuthorizationWithCredsAccessDenied, tsn));
		//No auth
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNoAuthAuthorizationSuccess, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNoAuthAuthorizationNotSupported, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestNoAuthAuthorizationAccessDenied, tsn));
		//One auth type supported on proxy - several types provided to updater
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestOnlyNtlmAuthorization, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestOnlyNtlmAuthorizationWithCreds, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestOnlyBasicAuthorization, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestOnlyNoAuthAuthorization, tsn));

		//HTTP

			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestHttpPortSpecification, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestHttpAuthorization, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestHttpConfigurationTest, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestHttpRelativePath, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestHttpRegetting, tsn));

		//FTP

			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpPortSpecification, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpAuthorization, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpConfigurationTest, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpOverHttp, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpRelativePath, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpRegetting, tsn));

		//PROXY

			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestProxyConfigurationTest, tsn));
 
		//MISC	
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestProxy_172_16_10_108, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpActivePassive, tsn));
			//doesnot exist test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestRegettingFailed, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestProxyCredentialsCache, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestBypassProxyForLocalAddresses, tsn)); 			

			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestWpadProxyDetectResult, tsn));
		 
		//IpV6
*/			//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestHttpIpV6, tsn));
			//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestFtpIpV6, tsn));
/*			//не работает test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestShareIpV6, tsn));
			//бага, не работает для ipV6 -  //
			//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestBypassProxyForLocalAddresses_IpV6, tsn));
			test->add (BOOST_CLASS_TEST_CASE (&TestSuiteNet::TestWpadProxyDetectResult_IpV6, tsn));
	}

	if (TEST_TRANS)
	{
		shared_ptr<Test_UpdaterTransaction> tstrans (new Test_UpdaterTransaction ());
/*
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddToEmpty, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemoveAll, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestChangeAll, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestMixedOperations, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestNoopAll, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestLockingReplacedFile, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestLockingBackupedFile, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestMissingCopiedFile, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestLockingDeletedFile, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingFileExists, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingFileDoesnExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingFileDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingWhenSourceFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingWhenDestFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingWhenBackupFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingWhenSourceFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingWhenBackupFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingWhenDestFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingWhenSourceFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingWhenBackupFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingWhenDestFolderDoesntExist, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestPathDelimitters, tstrans));
		test->add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestEmptyTransaction, tstrans));
*/
	 } 

	if (TEST_OTHERS)
	{

		shared_ptr<TestSuiteSmallClasses> tsn (new TestSuiteSmallClasses ());

		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSmallClasses::TestLocalFile, tsn));
		//out-of-date - test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSmallClasses::TestIndexFileXMLParser, tsn));
	}

	if (TEST_PROXY_DET)
	{

		shared_ptr<TestSuiteProxyDetector> tsn (new TestSuiteProxyDetector ());

		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteProxyDetector::TestWPAD, tsn));
		/*ошибка разбора - исправлено */
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteProxyDetector::TestWPAD_IpV6, tsn));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteProxyDetector::TestWPADCorrupted, tsn));
		
	}

	if (TEST_ADDRESS)
	{

		shared_ptr<TestSuiteAddress> tsa (new TestSuiteAddress ());

		//TT31339-fixed
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteAddress::TestAddress, tsa));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteAddress::TestAddress_IpV6, tsa));

	}

	if (TEST_TRANSPORT)
	{

		shared_ptr<TestFtpTransport> tft (new TestFtpTransport ());
/*
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestWrongAuth, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestUserPassword, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestPassiveMode, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestActiveMode, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestErrorRetrFile, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestGettingFilePassive, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestGettingFileActive, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestGettingFileWithPath, tft));
//баг с постоянным запросом EPSV/EPRT
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestGetting2Files, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestUsingMultilineReplies, tft));

		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestUsingMultilineRepliesAdv, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestGettingFileWithRest, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestGettingFileWithoutRest, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestAbortTransferringClient, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestAbortTransferringServer, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestTimeouts, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestTimeoutAfterTransferringData, tft));

		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestReusingSessionPassive, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestReusingSessionActive, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestReusingSessionDifferentServers, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestTryActiveIfPassiveFails, tft));

		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestAuthorizationCases, tft));
		
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestExtendedModeIp4_Active, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestExtendedModeIp4_Passive, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestExtendedModeIp6_Active, tft));
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestExtendedModeIp6_Passive, tft));
		
		test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::TestIpV6_Resolving, tft));	
		///////test->add (BOOST_CLASS_TEST_CASE (&TestFtpTransport::Draft, tft));
*/	
		shared_ptr<TestHttpTransport> tht (new TestHttpTransport ());
/*
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFile, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFileWithRest, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFileWithRestCode200, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFileWithRestCode404, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFileWithRestCode416, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFileNoContentLength, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFileContentLength, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestConnection, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestPathes, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestConnectionClosingTimeout, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestReusingTheSameServer, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestReusingServers, tht));

		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestRedirectionCodes, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestRedirectionCycling, tht));
		//n/a test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestRedirectionUseProxy, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestRedirectionCases, tht));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpTransport::TestGetFile_IpV6, tht));
*/
		shared_ptr<TestHttpProxy> thp (new TestHttpProxy ());
/*
		*/test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestBasicAuth, thp));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestNoAuth, thp));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestNtlmAuth, thp));

		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestConnectionClosingTimeout, thp));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestFtpOverHttp, thp));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestReusingServersThroughProxy, thp));

		/*test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSwitchingURL_UseProxyCases, thp));//OK
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSpecialProxyCases_Redirect, thp));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSpecialProxyCases_404, thp));

		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSwitchingURL_UseWPAD, thp));
		
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestRequestingAuthInformation, thp));
		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestEmptyAuthListMode, thp));

		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestExhaustingAuthTypes, thp));

		test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestGetFile_IpV6, thp));
		*/

		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSpecialProxyCases, thp));
		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSpecialProxyCases_ConstCancel, thp));
		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSpecialProxyCases_CalculatedDiffs, thp));
		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpProxy::TestSpecialProxyCases_Code407, thp));
		
	}

	if (TEST_SITEINFO)
	{

		shared_ptr<TestSuiteSiteInfo> tssi (new TestSuiteSiteInfo ());

		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSiteInfo::TestWeight, tssi));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSiteInfo::TestUrl, tssi));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSiteInfo::TestDiff, tssi));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSiteInfo::TestRegion, tssi));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSiteInfo::TestTags, tssi));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteSiteInfo::TestSorting, tssi));

	}

	if (TEST_INIFILE)
	{

		shared_ptr<TestSuiteIniFile> tsif (new TestSuiteIniFile ());

		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteIniFile::TestCommon, tsif));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteIniFile::TestIncorrectFormat, tsif));
		//test->add (BOOST_CLASS_TEST_CASE (&TestSuiteIniFile::TestRandFormat, tsif));
		

	}

	if (TEST_HTTPPOST)
	{

		shared_ptr<TestHttpPost> tsif (new TestHttpPost ());

		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpPost::TestPost, tsif));
		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpPost::TestPostViaProxy, tsif));
		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpPost::TestServers, tsif));
		//test->add (BOOST_CLASS_TEST_CASE (&TestHttpPost::TestPostResponses, tsif));

	}

	return test;
}