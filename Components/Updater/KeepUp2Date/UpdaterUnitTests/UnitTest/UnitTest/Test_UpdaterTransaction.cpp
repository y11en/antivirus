#include "Test_UpdaterTransaction.h"
#include "helper.h"

class FileInfoWrapper
{
public:
	FileInfoWrapper (FileInfo::ChangeStatus status, const string& dest, const string& source, const string& backup, const string& file)
	{
		atom.m_transactionInformation.m_currentLocation = dest;
		atom.m_transactionInformation.m_newLocation = source;
		atom.m_transactionInformation.m_rollbackLocation = backup;
		atom.m_filename = file;
		atom.m_transactionInformation.m_changeStatus = status;
		//ch_status = status;
	}
	operator FileInfo()
	{
		return atom;
	}

private:
	FileInfo atom;
	FileInfo::ChangeStatus ch_status;
};

void Test_UpdaterTransaction::TestAddToEmpty ()
{
	string reason;
	std::cout<<"Test adding files to empty directory\n";

	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	//FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	//prepare source directory
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	//prepare destination_etalon directory
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	//prepare backup etalon folder

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);
	FileVector _changedFiles;
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
/*
	 updTrans.addAtom (FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, FileInfo ("\\unp000.avc"));
	 _changedFilesEtalon.push_back (make_pair (FileInfo ("\\unp000.avc"), FileInfo::added));
	 updTrans.addAtom (FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, FileInfo ("\\unp001.avc"));
	 _changedFilesEtalon.push_back (make_pair (FileInfo ("\\unp001.avc"), FileInfo::added));
	 updTrans.addAtom (FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, FileInfo ("\\unp002.avc"));
	 _changedFilesEtalon.push_back (make_pair (FileInfo ("\\unp002.avc"), FileInfo::added));
	 updTrans.addAtom (FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, FileInfo ("\\unp003.avc"));
	_changedFilesEtalon.push_back (make_pair (FileInfo ("\\unp003.avc"), FileInfo::added));
*/
	TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
	////TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	
	Step ("TestAddToEmpty", reason);
}

void Test_UpdaterTransaction::TestRemoveAll ()
{
	string reason;
	std::cout<<"Test removing all files from the directory\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	//FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);
	FileVector _changedFiles;
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, "", config_ft.backup, "\\unp002.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, "", config_ft.backup, "\\unp003.avc"));

/*	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, "", config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, "", config_ft.backup, "\\unp003.avc"));
*/

	TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
//	//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	
	Step ("TestRemoveAll", reason);
}


void Test_UpdaterTransaction::TestChangeAll ()
{
	string reason;
	std::cout<<"Test changing all files in the directory\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	//FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);
	FileVector _changedFiles;
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));


/*	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
*/

	TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
	////TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	
	Step ("TestChangeAll", reason);
}


void Test_UpdaterTransaction::TestMixedOperations ()
{
	string reason;
	std::cout<<"Testing all types of operations\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	//helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	//helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	//helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));

	TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
	//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

	Step ("TestMixedOperations", reason);
}

void Test_UpdaterTransaction::TestNoopAll ()
{
	string reason;
	std::cout<<"Test doing nothing with all files in the directory\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, config_ft.destination, "", config_ft.backup, "\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, config_ft.destination, "", config_ft.backup, "\\unp003.avc"));

	TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
	//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

	Step ("TestNoopAll", reason);
}

void Test_UpdaterTransaction::TestLockingReplacedFile ()
{
	string reason;
	std::cout<<"Testing behavior when a file, which is to be replaced is locked\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	{
		AutoFile hFile (string(config_ft.destination).append("\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestLockingReplacedFile", reason);
}


void Test_UpdaterTransaction::TestLockingBackupedFile ()
{
	string reason;
	std::cout<<"Testing behavior when a file in the backup, which is to be replaced is locked\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	
	{
		AutoFile hFile  (string(config_ft.backup).append("\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}
	Step ("TestLockingBackupedFile", reason);
}

void Test_UpdaterTransaction::TestMissingCopiedFile ()
{
	string reason;
	std::cout<<"Testing behavior when a file in the source folder, which is to be copied is missed\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp004.avc"));
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestMissingCopiedFile", reason);
}

void Test_UpdaterTransaction::TestLockingDeletedFile ()
{
	string reason;
	std::cout<<"Testing behavior when a file in the destination folder, which is to be deleted is locked\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	{
		AutoFile hFile (string(config_ft.destination).append("\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, OPEN_EXISTING);
	
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestLockingDeletedFile", reason);
}

void Test_UpdaterTransaction::TestAddingFileExists ()
{
	string reason;
	std::cout<<"Testing behavior when a file, which is to be added, already exists\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestAddingFileExists", reason);
}

void Test_UpdaterTransaction::TestRemovingFileDoesnExist ()
{
	string reason;
	std::cout<<"Testing behavior when a file, which is to be deleted, doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	//helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	//helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestRemovingFileDoesnExist", reason);
}

void Test_UpdaterTransaction::TestAddingWhenSourceFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup, "unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "unp003.avc"));
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestAddingWhenSourceFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestAddingWhenDestFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the destination folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, string(config_ft.destination).append("\\fake").c_str(), config_ft.source, config_ft.backup, "unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "unp003.avc"));
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestAddingWhenDestFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestAddingWhenBackupFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the backup folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, string(config_ft.backup).append("\\fake").c_str(), "\\unp002.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, config_ft.destination, config_ft.source, "", "\\unp003.avc"));

	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestAddingWhenBackupFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestRemovingWhenSourceFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, "", config_ft.backup, "\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup, "\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestRemovingWhenSourceFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestRemovingWhenBackupFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, "", config_ft.backup, "\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup, "\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestRemovingWhenBackupFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestRemovingWhenDestFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the destination folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, string(config_ft.destination).append("\\fake").c_str(), config_ft.source, config_ft.backup, "\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestRemovingWhenDestFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestReplacingWhenSourceFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the source folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, string(config_ft.source).append("\\fake").c_str(), config_ft.backup, "\\unp002.avc"));
	
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestReplacingWhenSourceFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestReplacingWhenDestFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the destination folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, string(config_ft.destination).append("\\fake").c_str(), config_ft.source, config_ft.backup, "\\unp002.avc"));
	
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestReplacingWhenDestFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestReplacingWhenBackupFolderDoesntExist ()
{
	string reason;
	std::cout<<"Testing behavior when the backup folder doesnt exist\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);
	dirs.push_back (string(config_ft.backup_etalon).append("\\fake"));

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.backup_etalon).append("\\unp003.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.backup_etalon).append("\\fake\\unp002.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp002.avc").c_str(), string(config_ft.backup).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp003.avc").c_str(), string(config_ft.backup).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source_etalon).append("\\unp003.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.source).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.source).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp002.avc").c_str(), string(config_ft.destination).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp003.avc").c_str(), string(config_ft.destination).append("\\unp003.avc").c_str(), false);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp002.avc").c_str(), string(config_ft.destination_etalon).append("\\unp002.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp003.avc").c_str(), string(config_ft.destination_etalon).append("\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp001.avc"));
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, string(config_ft.backup).append("\\fake").c_str(), "\\unp002.avc"));
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp003.avc"));

	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestReplacingWhenBackupFolderDoesntExist", reason);
}


void Test_UpdaterTransaction::TestPathDelimitters ()
{
	string reason;
	std::cout<<"Testing the placement of slash\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, string(config_ft.destination).append("\\").c_str(), string(config_ft.source).append("\\").c_str(), string(config_ft.backup).append("\\").c_str(), "\\unp001.avc"));

	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestPathDelimitters", reason);
}


void Test_UpdaterTransaction::TestEmptyTransaction ()
{
	string reason;
	std::cout<<"Testing an empty transaction\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup_etalon).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage).append("\\unp000.avc").c_str(), string(config_ft.backup).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage).append("\\unp001.avc").c_str(), string(config_ft.backup).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source_etalon).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage3).append("\\unp001.avc").c_str(), string(config_ft.source).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination).append("\\unp001.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage2).append("\\unp000.avc").c_str(), string(config_ft.destination_etalon).append("\\unp000.avc").c_str(), false);
	helper::CloneFile (string(config_ft.storage2).append("\\unp001.avc").c_str(), string(config_ft.destination_etalon).append("\\unp001.avc").c_str(), false);
	
	// check transaction
	UpdaterTransaction updTrans (&log, &journal);	
	{
		TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::Done, "Incorrect transaction result");
		//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");
	}

	Step ("TestEmptyTransaction", reason);
}

void Test_UpdaterTransaction::TestReplacingFileDoesntExist ()
{
	string reason;
	std::cout<<"Testing replacing missing file\n";
	
	vector<string> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	dirs.push_back (config_ft.destination);
	dirs.push_back (config_ft.destination_etalon);
	dirs.push_back (config_ft.backup);
	dirs.push_back (config_ft.backup_etalon);
	dirs.push_back (config_ft.source);
	dirs.push_back (config_ft.source_etalon);

	helper::RecreateDirectories (dirs);

	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source_etalon).append("\\unp000.avc").c_str(), false);
	
	helper::CloneFile (string(config_ft.storage3).append("\\unp000.avc").c_str(), string(config_ft.source).append("\\unp000.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (&log, &journal);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, config_ft.destination, config_ft.source, config_ft.backup, "\\unp000.avc"));
	TD_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == UpdaterTransaction::Result::RolledBack, "Incorrect transaction result");
	//TD_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, "Incorrect changed files list");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.destination_etalon, config_ft.destination), "Destination folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.source_etalon, config_ft.source), "Source folder");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_ft.difftool, config_ft.backup_etalon, config_ft.backup), "Backup folder");

	Step ("TestReplacingFileDoesntExist_Obligate", reason);
}


Suite_UpdaterTransaction::Suite_UpdaterTransaction (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_UpdaterTransaction> instance (new Test_UpdaterTransaction(ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_UpdaterTransaction>(&Test_UpdaterTransaction::Start, "Test_UpdaterTransaction::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddToEmpty, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemoveAll, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestChangeAll, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestMixedOperations, instance));
	
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestNoopAll, instance));
	
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestLockingReplacedFile, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestLockingBackupedFile, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestMissingCopiedFile, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestLockingDeletedFile, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingFileExists, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingFileDoesnExist, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingWhenSourceFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingWhenDestFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestAddingWhenBackupFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingWhenSourceFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingWhenDestFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestRemovingWhenBackupFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingWhenSourceFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingWhenDestFolderDoesntExist, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingWhenBackupFolderDoesntExist, instance));
			
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestPathDelimitters, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestEmptyTransaction, instance));

	add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingFileDoesntExist, instance));
	
	add (boost::unit_test::make_test_case<Test_UpdaterTransaction>(&Test_UpdaterTransaction::End, "Test_UpdaterTransaction::End", instance));
	
	////add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestReplacingFileDoesntExist, instance));
	////add (BOOST_CLASS_TEST_CASE (&Test_UpdaterTransaction::TestCopyFileToItself, instance));

  };
