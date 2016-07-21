#ifndef TEST_FILETRANSACTION_H
#define TEST_FILETRANSACTION_H

//#include "..\..\..\Client\helper\file_transaction.h"
//#include "Core/file_transaction.h"


#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"


class Test_FileTransaction
{
	public:
		Test_FileTransaction (const string& ini_file, bool log_enable, const string& log_name): config_ft (Configuration::Init (ini_file.c_str()) .GetFileTransactionConfig()),	
										ptr_log (new Ini_Log (log_enable)), ptr_journal (new Ini_Journal(ptr_log.get())) {};

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
		void TestReplacingFileDoesntExist_Obligate ();
		//void TestReplacingFileDoesntExist ();

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
		void TestCopyFileToItself ();

		//Others
		void TestPathDelimitters ();
		void TestEmptyTransaction ();

		//Interfaces
		shared_ptr<Log> ptr_log;
		shared_ptr<JournalInterface> ptr_journal;
		shared_ptr<CallbackInterface> ptr_callback;

	private:
		
		const Config_FileTransaction& config_ft;

};




class Suite_FileTransaction: public test_suite
{
	public:

		Suite_FileTransaction (const string& ini_file, const string& log_name);
		
};











#endif