#include "Test_LocalFile.h"
#include "helper.h"


void Test_LocalFile::TestFileCopyOK ()
{
	string reason;

	std::cout<<"Test copying a file\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);
	dirs.push_back (config_lf.destination_etalon);
	dirs.push_back (config_lf.source);
	dirs.push_back (config_lf.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.source_etalon).append("\\unp000.avc").c_str(), false);

	{
		//LocalFile locFile (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		LocalFile locFile_src (string(config_lf.source).append("\\unp000.avc"), ptr_log.get());

		//TD_BOOST_CHECK_MESSAGE (locFile.Create (LocalFile::Mode::LOCAL_MODE_CREATE), "File creation");
//		TD_BOOST_CHECK_MESSAGE (locFile_src.Open (LocalFile::Mode::LOCAL_MODE_READ), "File opening");
/*
		//vector<BYTE> data;
		//data.reserve (locFile_src.Size());
		//data.reserve (100000);
		char data[100000];

		int _ret, _read = 0;
		do
		{
			_ret = locFile_src.Read (&data[_read], locFile_src.Size() - _read);
			_read += _ret;
			TD_BOOST_CHECK_MESSAGE (_ret != -1, "Reading file");
		} while (_ret !=  0);
		
		TD_BOOST_CHECK_MESSAGE (_read == 10769, "Reading file");
		*/
		
		TD_BOOST_CHECK_MESSAGE (locFile_src.copy ( string(config_lf.destination).append("\\unp000.avc") ), "Copying file");
	}

	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.destination_etalon, config_lf.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.source_etalon, config_lf.source), "Source folder");	

	Step ("TestFileCopyOK", reason);
};

void Test_LocalFile::TestFileCopyWhenExists ()
{
	string reason;
	std::cout<<"Test copying a file when the target one already exists\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);
	dirs.push_back (config_lf.destination_etalon);
	dirs.push_back (config_lf.source);
	dirs.push_back (config_lf.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp001.avc").c_str(), string(config_lf.destination).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.source).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.source_etalon).append("\\unp000.avc").c_str(), false);

	{
		LocalFile locFile_src (string(config_lf.source).append("\\unp000.avc"), ptr_log.get());

//		TD_BOOST_CHECK_MESSAGE (locFile_src.Open (LocalFile::Mode::LOCAL_MODE_READ), "File opening");
		TD_BOOST_CHECK_MESSAGE (locFile_src.copy ( string(config_lf.destination).append("\\unp000.avc") ), "Copying file");
	}

	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.destination_etalon, config_lf.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.source_etalon, config_lf.source), "Source folder");	

	Step ("TestFileCopyWhenExists", reason);
};

void Test_LocalFile::TestFileUnlinkOK ()
{
	string reason;
	std::cout<<"Test deleting a file successfully\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);
	dirs.push_back (config_lf.destination_etalon);
	dirs.push_back (config_lf.source);
	dirs.push_back (config_lf.source_etalon);

	helper::RecreateDirectories (dirs);

	//CopyFile (string(config_lf.storage).append("\\unp001.avc").c_str(), string(config_lf.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination).append("\\unp000.avc").c_str(), false);
	
	{
		LocalFile locFile_src (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		TD_BOOST_CHECK_MESSAGE (locFile_src.unlink(), "Deleting file");
	}

	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.destination_etalon, config_lf.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.source_etalon, config_lf.source), "Source folder");	

	Step ("TestFileUnlinkOK", reason);
};

void Test_LocalFile::TestFileUnlinkFail ()
{
	string reason;

	std::cout<<"Test deleteting a file when it is locked\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);
	dirs.push_back (config_lf.destination_etalon);
	dirs.push_back (config_lf.source);
	dirs.push_back (config_lf.source_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination).append("\\unp000.avc").c_str(), false);
	
	{
		LocalFile locFile_src (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		
		AutoFile hFile (string(config_lf.destination).append("\\unp000.avc").c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
		
		TD_BOOST_CHECK_MESSAGE (!locFile_src.unlink(), "Deleting file");
	}

	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.destination_etalon, config_lf.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.source_etalon, config_lf.source), "Source folder");	

	Step ("TestFileUnlinkFail", reason);
};

void Test_LocalFile::TestFileRenameOK ()
{
	string reason;

	std::cout<<"Test renaming a file sucessfully\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);
	dirs.push_back (config_lf.destination_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination_etalon).append("\\unp001.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination).append("\\unp000.avc").c_str(), false);
	
	{
		LocalFile locFile_src (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		
		TD_BOOST_CHECK_MESSAGE (locFile_src.rename (string(config_lf.destination).append("\\unp001.avc")), "Renaming file");
	}

	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.destination_etalon, config_lf.destination), "Destination folder");
	
	Step ("TestFileRenameOK", reason);
};

void Test_LocalFile::TestFileRenameFail ()
{
	string reason;

	std::cout<<"Test renaming a file when it is locked\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);
	dirs.push_back (config_lf.destination_etalon);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination_etalon).append("\\unp000.avc").c_str(), false);
	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination).append("\\unp000.avc").c_str(), false);
	
	{
		LocalFile locFile_src (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		AutoFile hFile  (string(config_lf.destination).append("\\unp000.avc").c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
		
		TD_BOOST_CHECK_MESSAGE (!locFile_src.rename (string(config_lf.destination).append("\\unp001.avc")), "Renaming file");
	}

	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_lf.difftool, config_lf.destination_etalon, config_lf.destination), "Destination folder");

	Step ("TestFileRenameFail", reason);
};

void Test_LocalFile::TestFileExistenceOK ()
{
	string reason;

	std::cout<<"Test existence of a file when it exists\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);

	helper::RecreateDirectories (dirs);

	CopyFile (string(config_lf.storage).append("\\unp000.avc").c_str(), string(config_lf.destination).append("\\unp000.avc").c_str(), false);
	
	LocalFile locFile_src (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		
	TD_BOOST_CHECK_MESSAGE (locFile_src.exists(), "Checking existence of a file");

	Step ("TestFileExistenceOK", reason);

};

void Test_LocalFile::TestFileExistenceFail ()
{
	string reason;

	std::cout<<"Test existence of a file when it is missed\n";

	vector<string> dirs;

	dirs.push_back (config_lf.destination);

	helper::RecreateDirectories (dirs);
	
	LocalFile locFile_src (string(config_lf.destination).append("\\unp000.avc"), ptr_log.get());
		
	TD_BOOST_CHECK_MESSAGE (!locFile_src.exists(), "Checking existence of a file");

	Step ("TestFileExistenceFail", reason);
};


Suite_LocalFile::Suite_LocalFile (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_LocalFile> instance (new Test_LocalFile(ini_file, bLog, log_name));

	add (boost::unit_test::make_test_case<Test_LocalFile>(&Test_LocalFile::Start, "Test_LocalFile::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileCopyOK, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileCopyWhenExists, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileUnlinkOK, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileUnlinkFail, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileRenameOK, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileRenameFail, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileExistenceOK, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_LocalFile::TestFileExistenceFail, instance), 0);
	add (boost::unit_test::make_test_case<Test_LocalFile>(&Test_LocalFile::End, "Test_LocalFile::End", instance));
	
	

}
	