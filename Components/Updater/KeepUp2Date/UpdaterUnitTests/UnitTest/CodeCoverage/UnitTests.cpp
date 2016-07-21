#include "stdafx.h"

#include "TestFtpTransport.h"
#include "log.h"

#include "core/updater.h"
#include "helper/local_file.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace CodeCoverage
{
	[TestClass]
	public ref class UnitTest1
	{
	public:
		#pragma region Additional test attributes
		//
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		//[ClassInitialize()]
		//static void MyClassInitialize(TestContext^ testContext) {};
		//
		//Use ClassCleanup to run code after all tests in a class have run
		//[ClassCleanup()]
		//static void MyClassCleanup() {};
		//
		//Use TestInitialize to run code before running each test
		//[TestInitialize()]
		//void MyTestInitialize() {};
		//
		//Use TestCleanup to run code after each test has run
		//[TestCleanup()]
		//void MyTestCleanup() {};
		//
		#pragma endregion 

		[TestMethod]
		void TestMethodFtpTransport()
		{
			TestFtpTransport* tft = new TestFtpTransport ();
			tft->TestWrongAuth ();
			/*tft->TestUserPassword ();
			tft->TestPassiveMode ();
			tft->TestActiveMode ();
			tft->TestErrorRetrFile ();
			tft->TestGettingFilePassive ();
			tft->TestGettingFileActive ();
			tft->TestGettingFileWithPath ();
			tft->TestGetting2Files ();
			tft->TestUsingMultilineReplies ();
			tft->TestUsingMultilineRepliesAdv ();
			tft->TestGettingFileWithRest ();
			tft->TestGettingFileWithoutRest ();
			tft->TestAbortTransferringClient ();
			tft->TestAbortTransferringServer ();
			tft->TestTimeouts ();
			tft->TestTimeoutAfterTransferringData ();*/

		};
	};
}
