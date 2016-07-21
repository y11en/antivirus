#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"
#include "helper/update_transaction.h"

#include "TestSuiteTransaction.h"
#include "main.h"
#include "util.h"

#define PREPARE_DIRS\
	dirs.push_back (ROOT);\
	dirs.push_back (TRANS_ROOT);\
	dirs.push_back (TRANS_DEST);\
	dirs.push_back (TRANS_DEST_ETALON);\
	dirs.push_back (TRANS_SOURCE);\
	dirs.push_back (TRANS_SOURCE_ETALON);\
	dirs.push_back (TRANS_BACKUP);\
	dirs.push_back (TRANS_BACKUP_ETALON);\
	_helper::RecreateDirectories (dirs);

using namespace KLUPD;
using namespace std;

class FileInfoWrapper
{
public:
	FileInfoWrapper (FileInfo::ChangeStatus status, const wstring& dest, const wstring& source, const wstring& backup, const wstring& file)
	{
		atom.m_transactionInformation.m_currentLocation = dest;
		atom.m_transactionInformation.m_newLocation = source;
		//atom.m_transactionInformation.m_rollbackLocation = backup;
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
	wstring reason;
	std::wcout<<L"Test adding files to empty directory\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddToEmpty");

	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	//FileVector _changedFiles;

	PREPARE_DIRS

	//prepare source directory
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	//prepare destination_etalon directory
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	//prepare backup etalon folder

	// check transaction
	
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);
	FileVector _changedFiles;
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));

	QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
	////QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	
	//Step (L"TestAddToEmpty", reason);
}

void Test_UpdaterTransaction::TestRemoveAll ()
{
	wstring reason;
	std::wcout<<L"Test removing all files from the directory\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRemoveAll");
/*	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	//FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	UpdaterTransaction updTrans (TRANS_BACKUP, testCallback, testJournal, false);
	FileVector _changedFiles;
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, L"", TRANS_BACKUP, L"\\unp002.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, L"", TRANS_BACKUP, L"\\unp003.avc"));


	QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
//	//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
*/	
}


void Test_UpdaterTransaction::TestChangeAll ()
{
	wstring reason;
	std::wcout<<L"Test changing all files in the directory\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestChangeAll");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	//FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);
	FileVector _changedFiles;
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));


	QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
	////QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	
}


void Test_UpdaterTransaction::TestMixedOperations ()
{
	wstring reason;
	std::wcout<<L"Testing all types of operations\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestMixedOperations");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	//_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	//_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));

	// _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));

	QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
	//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");

	//Step (L"TestMixedOperations", reason);
}

void Test_UpdaterTransaction::TestNoopAll ()
{
	wstring reason;
	std::wcout<<L"Test doing nothing with all files in the directory\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNoopAll");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, TRANS_DEST, L"", TRANS_BACKUP, L"\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::unchanged, TRANS_DEST, L"", TRANS_BACKUP, L"\\unp003.avc"));

	QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
	//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");

	//Step (L"TestNoopAll", reason);
}

void Test_UpdaterTransaction::TestLockingReplacedFile ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when a file, which is to be replaced is locked\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestLockingReplacedFile");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	{
		_helper::AutoFile hFile (wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NotEnoughPermissions, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestLockingReplacedFile", reason);
}


void Test_UpdaterTransaction::TestLockingBackupedFile ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when a file in the backup, which is to be replaced is locked\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestLockingBackupedFile");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	
	{
		_helper::AutoFile hFile  (wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NotEnoughPermissions, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}
	//Step (L"TestLockingBackupedFile", reason);
}

void Test_UpdaterTransaction::TestMissingCopiedFile ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when a file in the source folder, which is to be copied is missed\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestMissingCopiedFile");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	//UpdaterTransaction(const NoCaseString &rollbackFolder, const bool retranslationMode,
	//	DownloadProgress &, CallbackInterface &, JournalInterface &, Log *);

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp004.avc"));
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NoSuchFileOrDirectory, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestMissingCopiedFile", reason);
}

void Test_UpdaterTransaction::TestLockingDeletedFile ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when a file in the destination folder, which is to be deleted is locked\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestLockingDeletedFile");
	/*
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (TRANS_BACKUP, testCallback, testJournal, false);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	{
		_helper::AutoFile hFile (wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, OPEN_EXISTING);
	
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NotEnoughPermissions, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}
*/
	//Step (L"TestLockingDeletedFile", reason);
}

void Test_UpdaterTransaction::TestAddingFileExists ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when a file, which is to be added, already exists\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddingFileExists");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	
	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestAddingFileExists", reason);
}

void Test_UpdaterTransaction::TestRemovingFileDoesnExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when a file, which is to be deleted, doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRemovingFileDoesnExist");
/*	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	//_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	
	// check transaction
	UpdaterTransaction updTrans (TRANS_BACKUP, testCallback, testJournal, false);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NotEnoughPermissions, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}
*/
	//Step (L"TestRemovingFileDoesnExist", reason);
}

void Test_UpdaterTransaction::TestAddingWhenSourceFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the source folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddingWhenSourceFolderDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	
	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, wstring(TRANS_SOURCE).append(L"\\fake").c_str(), TRANS_BACKUP, L"unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp003.avc"));
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NoSuchFileOrDirectory, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestAddingWhenSourceFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestAddingWhenDestFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the destination folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddingWhenDestFolderDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	
	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, wstring(TRANS_DEST).append(L"\\fake").c_str(), TRANS_SOURCE, TRANS_BACKUP, L"unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp003.avc"));
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NoSuchFileOrDirectory, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestAddingWhenDestFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestAddingWhenBackupFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the backup folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddingWhenBackupFolderDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, wstring(TRANS_BACKUP).append(L"\\fake").c_str(), L"\\unp002.avc"));

	 _changedFiles.push_back (FileInfoWrapper(FileInfo::added, TRANS_DEST, TRANS_SOURCE, L"", L"\\unp003.avc"));

	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestAddingWhenBackupFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestRemovingWhenSourceFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the source folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRemovingWhenSourceFolderDoesntExist");
/*	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (TRANS_BACKUP, testCallback, testJournal, false);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, L"", TRANS_BACKUP, L"\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, wstring(TRANS_SOURCE).append(L"\\fake").c_str(), TRANS_BACKUP, L"\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}
*/
	//Step (L"TestRemovingWhenSourceFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestRemovingWhenBackupFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the source folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRemovingWhenBackupFolderDoesntExist");
/*	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (TRANS_BACKUP, testCallback, testJournal, false);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, L"", TRANS_BACKUP, L"\\unp001.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, wstring(TRANS_SOURCE).append(L"\\fake").c_str(), TRANS_BACKUP, L"\\unp002.avc"));
	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}
*/
	//Step (L"TestRemovingWhenBackupFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestRemovingWhenDestFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the destination folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRemovingWhenDestFolderDoesntExist");
	/*
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	UpdaterTransaction updTrans (TRANS_BACKUP, testCallback, testJournal, false);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, wstring(TRANS_DEST).append(L"\\fake").c_str(), TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::deleted, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NotEnoughPermissions, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}
*/
	//Step (L"TestRemovingWhenDestFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestReplacingWhenSourceFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the source folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReplacingWhenSourceFolderDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, wstring(TRANS_SOURCE).append(L"\\fake").c_str(), TRANS_BACKUP, L"unp002.avc"));
	
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NoSuchFileOrDirectory, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestReplacingWhenSourceFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestReplacingWhenDestFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the destination folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReplacingWhenDestFolderDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, wstring(TRANS_DEST).append(L"\\fake").c_str(), TRANS_SOURCE, TRANS_BACKUP, L"\\unp002.avc"));
	
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NoSuchFileOrDirectory, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestReplacingWhenDestFolderDoesntExist", reason);
}

void Test_UpdaterTransaction::TestReplacingWhenBackupFolderDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing behavior when the backup folder doesnt exist\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReplacingWhenBackupFolderDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp003.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\fake\\unp002.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp002.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp003.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp003.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp003.avc").c_str(), false);

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp002.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp002.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp003.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp003.avc").c_str(), false);

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp001.avc"));
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, wstring(TRANS_BACKUP).append(L"\\fake").c_str(), L"\\unp002.avc"));
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp003.avc"));

	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestReplacingWhenBackupFolderDoesntExist", reason);
}


void Test_UpdaterTransaction::TestPathDelimitters ()
{
	wstring reason;
	std::wcout<<L"Testing the placement of slash\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestPathDelimitters");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	
	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);
	
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	_changedFiles.push_back (FileInfoWrapper(FileInfo::modified, wstring(TRANS_DEST).append(L"\\").c_str(), wstring(TRANS_SOURCE).append(L"\\").c_str(), wstring(TRANS_BACKUP).append(L"\\").c_str(), L"\\unp001.avc"));

	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestPathDelimitters", reason);
}


void Test_UpdaterTransaction::TestEmptyTransaction ()
{
	wstring reason;
	std::wcout<<L"Testing an empty transaction\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestEmptyTransaction");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP_ETALON).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_BACKUP).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp001.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp001.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
	_helper::CloneFile (wstring(TRANS_STORAGE2).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
	
	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);	
	{
		QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NO_ERROR, L"Incorrect transaction result");
		//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");
	}

	//Step (L"TestEmptyTransaction", reason);
}

void Test_UpdaterTransaction::TestReplacingFileDoesntExist ()
{
	wstring reason;
	std::wcout<<L"Testing replacing missing file\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReplacingFileDoesntExist");
	
	vector<wstring> dirs;
	std::vector <pair<FileInfo,FileInfo::ChangeStatus> > _changedFilesEtalon;
	FileVector _changedFiles;

	PREPARE_DIRS

	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);
	
	_helper::CloneFile (wstring(TRANS_STORAGE3).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
	

	// check transaction
	CDownloadProgress dp;
	UpdaterTransaction updTrans (TRANS_BACKUP, false/*retr_mode*/, dp, testCallback, testJournal,&testLog);

	
	 _changedFiles.push_back (FileInfoWrapper(FileInfo::modified, TRANS_DEST, TRANS_SOURCE, TRANS_BACKUP, L"\\unp000.avc"));
	QC_BOOST_CHECK_MESSAGE (updTrans.run (_changedFiles) == CORE_NoSuchFileOrDirectory, L"Incorrect transaction result");
	//QC_BOOST_CHECK_MESSAGE (_changedFilesEtalon == _changedFiles, L"Incorrect changed files list");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");
	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_BACKUP_ETALON, TRANS_BACKUP), L"Backup folder");

	//Step (L"TestReplacingFileDoesntExist_Obligate", reason);
}

