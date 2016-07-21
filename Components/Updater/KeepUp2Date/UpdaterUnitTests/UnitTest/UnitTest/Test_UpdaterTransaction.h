#ifndef TEST_UPDATERTRANSACTION_H
#define TEST_UPDATERTRANSACTION_H

#include "..\..\..\Client\helper\update_transaction.h"
#include "prod_config.h"
#include "upd_config.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"



class Test_UpdaterTransaction: public BaseTest
{
	public:
		Test_UpdaterTransaction (const string& ini_file, bool log_enable, const string& log_name):
									log (log_enable),
									updater (NULL, &log, true),
									prod_config (updater, &log, true),
									upd_sett (&log, true),
									config_ft (Configuration::Init (ini_file.c_str()) .GetFileTransactionConfig()),
									BaseTest ("UpdaterTransaction")
									{};
		
									
		//Tests - OK - Every action can be fulfilled
		void TestAddToEmpty ();
		void TestRemoveAll ();
		void TestChangeAll ();
		void TestMixedOperations ();

		//Test - Unknown operation - Crashed
		void TestNoopAll ();

		//Tests - Rollback - Replacing cannot be fullfilled
		void TestLockingReplacedFile ();
		void TestLockingBackupedFile ();
		void TestMissingCopiedFile ();

		//Tests - Rollback - Deleteing cannot be fulfilled
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

	private:

		Ini_Log logg;
		Ini_Updater updater;
		Ini_ProductConfiguration prod_config;
		Ini_UpdaterSettings upd_sett;
		Ini_Journal journal;
		IniCallbacks callback;

		const Config_FileTransaction& config_ft;
};


class Suite_UpdaterTransaction: public test_suite
{
	public:

		Suite_UpdaterTransaction (const string& ini_file, const string& log_name);
		
};


#endif