#include "Test_FileTransaction.h"
#include "helper.h"

//#define addAtom addAtom

void Test_FileTransaction::TestAddToEmpty ()
{
	std::cout<<"Test adding files to empty directory\n";

	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	//prepare source directory
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	//prepare destination_etalon directory
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	//prepare backup etalon folder

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);

	BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}

void Test_FileTransaction::TestRemoveAll ()
{
	std::cout<<"Test removing all files from the directory\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp002.avc", config_ft.destination, "", config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, "", config_ft.backup);

	BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}

void Test_FileTransaction::TestChangeAll ()
{
	std::cout<<"Test changing all files in the directory\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);

	BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}

void Test_FileTransaction::TestMixedOperations ()
{
	std::cout<<"Testing all types of operations\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	//CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	//CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	//CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);

	BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}

void Test_FileTransaction::TestNoopAll ()
{
	std::cout<<"Test doing nothing with all files in the directory\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_UNCHANGED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_UNCHANGED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_UNCHANGED, "\\unp002.avc", config_ft.destination, "", config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_UNCHANGED, "\\unp003.avc", config_ft.destination, "", config_ft.backup);

	BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}

void Test_FileTransaction::TestLockingReplacedFile ()
{
	std::cout<<"Testing behavior when a file, which is to be replaced is locked\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		AutoFile hFile (string(config_ft.destination).append("\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}


void Test_FileTransaction::TestLockingBackupedFile ()
{
	std::cout<<"Testing behavior when a file in the backup, which is to be replaced is locked\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		AutoFile hFile  (string(config_ft.backup).append("\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestMissingCopiedFile ()
{
	std::cout<<"Testing behavior when a file in the source folder, which is to be copied is missed\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp004.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestLockingDeletedFile ()
{
	std::cout<<"Testing behavior when a file in the destination folder, which is to be deleted is locked\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		AutoFile hFile (string(config_ft.destination).append("\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, OPEN_EXISTING);
	
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestAddingFileExists ()
{
	std::cout<<"Testing behavior when a file, which is to be added, already exists\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestRemovingFileDoesnExist ()
{
	std::cout<<"Testing behavior when a file, which is to be deleted, doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	//CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	//CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp002.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestAddingWhenSourceFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp002.avc", config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestAddingWhenDestFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the destination folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp002.avc", string(config_ft.destination).append("\\fake").c_str(), config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestAddingWhenBackupFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the backup folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp002.avc", config_ft.destination, config_ft.source, string(config_ft.backup).append("\\fake").c_str());
	
	 fileTrans.addAtom (CHANGE_STATUS_ADDEN, "\\unp003.avc", config_ft.destination, config_ft.source, "");
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestRemovingWhenSourceFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp001.avc", config_ft.destination, "", config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp002.avc", config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestRemovingWhenBackupFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp001.avc", config_ft.destination, "", config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp002.avc", config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestRemovingWhenDestFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the destination folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp002.avc", string(config_ft.destination).append("\\fake").c_str(), config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_DELETED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestReplacingWhenSourceFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestReplacingWhenDestFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the destination folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", string(config_ft.destination).append("\\fake").c_str(), config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestReplacingWhenBackupFolderDoesntExist ()
{
	std::cout<<"Testing behavior when the backup folder doesnt exist\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);
	dirs.push_back (string(config_ft.backup_etalon).append("\\fake"));

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\fake\\unp002.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.source, string(config_ft.backup).append("\\fake").c_str());
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestCopyFileToItself ()
{
	std::cout<<"Testing copying a file to itself\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.destination, config_ft.destination);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp001.avc", config_ft.destination, config_ft.destination, config_ft.destination);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp002.avc", config_ft.destination, config_ft.destination, config_ft.destination);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp003.avc", config_ft.destination, config_ft.destination, config_ft.destination);
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestPathDelimitters ()
{
	std::cout<<"Testing the placement of slash\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);
	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "unp001.avc", string(config_ft.destination).append("\\").c_str(),
		string(config_ft.source).append("\\").c_str(), string(config_ft.backup).append("\\").c_str());
	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestEmptyTransaction ()
{
	std::cout<<"Testing an empty transaction\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	
	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());	
	{
		BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::Done);
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
}

void Test_FileTransaction::TestReplacingFileDoesntExist_Obligate ()
{
	std::cout<<"Testing replacing missing file when the obligate flag is set\n";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup);

	BOOST_REQUIRE (fileTrans.run () == FileTransaction::Result::RolledBack);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}
/*
void Test_FileTransaction::TestReplacingFileDoesntExist ()
{
	std::cout<<"Testing replacing missing file when the obligate flag is not set";
	
	vector<string> dirs;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	
	CopyFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	

	// check transaction
	FileTransaction fileTrans (ptr_log.get(), ptr_journal.get());

	
	 fileTrans.addAtom (CHANGE_STATUS_MODIFIED, "\\unp000.avc", config_ft.destination, config_ft.source, config_ft.backup)
	);

	BOOST_REQUIRE (fileTrans.run () (false) == FileTransaction::Result::Done);
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

}
*/
Suite_FileTransaction::Suite_FileTransaction (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_FileTransaction> instance (new Test_FileTransaction(ini_file, bLog, log_name));
	
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestAddToEmpty, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestRemoveAll, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestChangeAll, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestMixedOperations, instance));
	
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestNoopAll, instance));
	
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestLockingReplacedFile, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestLockingBackupedFile, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestMissingCopiedFile, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestLockingDeletedFile, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestAddingFileExists, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestRemovingFileDoesnExist, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestAddingWhenSourceFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestAddingWhenDestFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestAddingWhenBackupFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestRemovingWhenSourceFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestRemovingWhenDestFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestRemovingWhenBackupFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestReplacingWhenSourceFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestReplacingWhenDestFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestReplacingWhenBackupFolderDoesntExist, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestCopyFileToItself, instance));
			
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestPathDelimitters, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestEmptyTransaction, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestReplacingFileDoesntExist_Obligate, instance));
	
	//add (BOOST_CLASS_TEST_CASE (&Test_FileTransaction::TestReplacingFileDoesntExist, instance));

};