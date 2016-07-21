#ifndef TESTSUITE_UPDATERTRANSACTION_H
#define TESTSUITE_UPDATERTRANSACTION_H

#include "boost.h"
#include "TestSuiteBase.h"

#include "callback.h"
#include "journal.h"

#include "log.h"

class TransactionCallback: public CallbackInterface
{
public:
	TransactionCallback (Log* pLog): CallbackInterface (/*pLog*/) {};
	virtual bool removeFileOnReboot(const FileInfo & fi, const Path &rollbackFolder)
	{
		return false;
	}

	virtual bool replaceFileOnReboot(const FileInfo & fi, const Path &rollbackFolder)
	{
		return false;
	}

	virtual bool checkFilesToDownload(FileVector &, const NoCaseString &updateDate, const bool retranslationMode)
	{
		return true;
	}

	virtual FileInfo getPrimaryIndex(const bool retranslationMode)
	{
		return FileInfo();
	}

	virtual FileInfo getUpdaterConfigurationXml()
	{
		return FileInfo();
	}

	virtual FileInfo getSitesFileXml()
	{
		return FileInfo();
	}
	
	 virtual UpdaterConfiguration &updaterConfiguration()
	{
		static TestLog testLog;
		static UpdaterConfiguration upd_config (1, 1, L"1");
		return upd_config;
	}

	 //now abstract in CallbackInterface

	 virtual bool removeRollbackSection(const bool retranslationMode)
	 {
		 return true;
	 }
	 virtual bool readRollbackFiles(FileVector &, const bool retranslationMode)
	 {
		 return false;
	 }

	 virtual bool processReceivedFiles(const FileVector &, const bool createRollbackNeeded, const Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString &)
	 {
		 return true;
	 }

	 virtual bool expandEnvironmentString(const NoCaseString &input, NoCaseString &output, const StringParser::ExpandOrder &)
	 {
		 return false;
	 }



};

class Test_UpdaterTransaction: public TestSuiteBase
{
	public:

		Test_UpdaterTransaction (): TestSuiteBase (L"Updater Transaction"), testCallback (&testLog) {};
		  
										
		//Tests - OK - Every action can be fulfilled
		void TestAddToEmpty ();
		void TestRemoveAll ();
		void TestChangeAll ();
		void TestMixedOperations ();

		//Test - Unknown operation - Crashed
		void TestNoopAll ();

		//Tests - Rollback - Replacing cannot be fulfilled
		void TestLockingReplacedFile ();
		void TestLockingBackupedFile ();
		void TestMissingCopiedFile ();

		//Tests - Rollback - Deleting cannot be fulfilled
		void TestLockingDeletedFile ();

		//Tests
		void TestAddingFileExists ();
		void TestRemovingFileDoesnExist ();
		void TestReplacingFileDoesntExist ();

		//Folders
		void TestAddingWhenSourceFolderDoesntExist ();
		void TestAddingWhenDestFolderDoesntExist ();
		void TestAddingWhenBackupFolderDoesntExist ();
		void TestRemovingWhenSourceFolderDoesntExist ();
		void TestRemovingWhenBackupFolderDoesntExist ();
		void TestRemovingWhenDestFolderDoesntExist ();
		void TestReplacingWhenSourceFolderDoesntExist ();
		void TestReplacingWhenBackupFolderDoesntExist ();
		void TestReplacingWhenDestFolderDoesntExist ();

		//Cycle
		//void TestCopyFileToItself ();

		//Others
		void TestPathDelimitters ();
		void TestEmptyTransaction ();
/*
	private:

		Ini_Log log;
		Ini_Updater updater;
		Ini_ProductConfiguration prod_config;
		Ini_UpdaterSettings upd_sett;
		Ini_Journal journal;
		IniCallbacks callback;

		const Config_FileTransaction& config_ft;
		*/
	private:
		TestLog testLog;
		TestJournalInterface testJournal;
		TransactionCallback testCallback;
		
};


#endif