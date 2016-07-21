#ifndef CALLBACK_H
#define CALLBACK_H

#pragma warning (disable: 4355)

#include "stdafx.h"

#include "include/callback_iface.h"
#include "inifile/inibasedrollback.h"
#include "include/DownloadProgress.h"

using namespace KLUPD;

struct Callbacks;

class TestCallbackInterface;
class CDownloadProgress: public KLUPD::DownloadProgress
{
public:

	CDownloadProgress  (TestCallbackInterface* cbi): m_cbi (cbi) {};
	CDownloadProgress (): m_cbi (NULL) {};

	virtual void percentCallback(const size_t currentPercentValue);
	virtual bool checkCancel();
	virtual bool requestAuthorizationCredentials(Credentials &);
	virtual bool authorizationTargetEnabled(const AuthorizationTarget &);
	virtual bool treat_403_502_httpCodesAs407 ();


private:

	TestCallbackInterface* m_cbi;

};

class TestCallbackInterface: public CallbackInterface
{
public:

	TestCallbackInterface (Log* pLog, Callbacks* _cbs): CallbackInterface (/*pLog*/), cbs(_cbs), upd_config (1, 1, L"1"),
		blacklistAnswer (true), bFileOptionalAnswer (false), pRbFile (NULL), intExpandStringCalls (0),
		btDownloaded (0), bCancel (false), bytesDownloadedBeforeCancelling(0), m_dp (this) {};

	CDownloadProgress m_dp;

	virtual UpdaterConfiguration &updaterConfiguration();

    virtual bool processReceivedFiles(const FileVector &, const bool createRollbackNeeded, const Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString &);
    virtual void processRollbackFileList(const FileVector &, const bool retranslationMode);

	virtual bool removeRollbackSection(const bool retranslationMode);
	virtual bool readRollbackFiles(FileVector &, const bool retranslationMode);

	virtual void refreshIdentifier(const unsigned long installationIdentifier, const unsigned long sessionIdentifier);
    virtual bool updateVariableFromIndex(const UpdatedSettings &, const bool retranslationMode);
    virtual void updateAutomaticScheduleForUpdater(const UpdateSchedule &);

    virtual FileInfo getPrimaryIndex(const bool retranslationMode);
    virtual FileInfo getUpdaterConfigurationXml();
    virtual FileInfo getSitesFileXml();


    virtual void percentCallback(const size_t currentPercentValue);
    virtual bool checkCancel();

    virtual bool checkIfFileOptional(const FileInfo &absentFile, const bool retranslationMode);
    virtual void componentRemovedFromUpdate(const NoCaseString &component);
    virtual FileVector getSubstitutedIndex(const bool retranslationMode);
	virtual bool requestCredentialsForAuthorizationOnProxy(std::wstring &userName, std::wstring &password);
    virtual void processProxyAuthorizationSuccess(const NoCaseString &userName, const NoCaseString &password);

    virtual bool checkBlackListFile(const Path &fileName);
    virtual bool checkFilesToDownload(FileVector &, const NoCaseString &updateDate, const bool retranslationMode);
    virtual bool expandEnvironmentString(const NoCaseString &input, NoCaseString &output, const StringParser::ExpandOrder&);

    //virtual bool removeFileOnReboot(const FileInfo &, const NoCaseString &rollbackFolder);
    //virtual bool replaceFileOnReboot(const FileInfo &, const NoCaseString &rollbackFolder);

	virtual void removeLockedFileRequest(const Path &fileName);


	//new callbacks for testing

	virtual bool checkDownloadedFiles(const FileVector &, const bool retranslationMode);
	virtual bool checkPreInstalledComponent(const Path &suffix, const NoCaseString &componentIdentifier, const FileVector &);
	virtual void lockComponentForUpdate(const NoCaseString &componentIdentifier);
	virtual void unlockComponentForUpdate(const NoCaseString &componentIdentifier);
	virtual void switchToSystemContext();
	virtual void switchToUserContext();

	//
	struct _checkFilesToDownload
	{
		_checkFilesToDownload (): result (/*KLUPD::CORE_NO_ERROR*/true) {};
		//UpdateInfo* updInfo;
		FileVector* m_matchFileList;
		NoCaseString updateDate;
		bool retranslationMode;
		/*KLUPD::CoreError*/bool result;
	};

	struct _processReceivedFiles
	{
		_processReceivedFiles (): result (true) {};
		
		FileVector fv;
		bool createRollbackNeeded;
		NoCaseString rollbackFolder;
		bool retranslationMode;
		bool result;
	};

	struct _checkDownloadedFiles
	{
		_checkDownloadedFiles (): result (true) {};

		FileVector fv;
		bool retranslationMode;
		bool result;
	};

	struct _checkPreInstalledComponent
	{
		_checkPreInstalledComponent (): result (true) {};

		Path suffix;
		FileVector fv;
		NoCaseString compId;
		bool result;
	};

	struct _credentials
	{
		_credentials (): result (true) {};
		NoCaseString name;
		NoCaseString password;
		bool result;
	};

	//
	void SetUpdaterConfiguration (const UpdaterConfiguration& _upd_config);
	void SetBlacklistAnswer (bool answer);
	_checkFilesToDownload& GetCheckFilesToDownload ();
	_checkDownloadedFiles& GetCheckDownloadedFiles ();
	_checkPreInstalledComponent& GetCheckPreinstalledComponent ();
	const FileInfo& GetAbsentFile ();
	void SetOptionalFileAnswer (bool answer);
	const NoCaseString& GetRemovedComponent ();
	_processReceivedFiles& GetProcessReceivedFiles ();
	const FileInfo& GetFileToReplace();
	void SetReplaceFileResult (bool answer);
	const FileInfo& GetFileToRemove();
	void SetRemoveFileResult (bool answer);
	void SetSubstitutedIndexes (const FileVector& indexes);
	const Path& GetRequestFileToRemove ();
	void SetSitesXml (const FileInfo& fi);
	void SetUpdcfgXml (const FileInfo& fi);
	void SetPrimaryIndex (const FileInfo& fi);

	void SetRequestCreds (const _credentials& creds);
	const _credentials& GetConfirmCreds ();
	const UpdateSchedule& GetUpdaterSchedule ();
	const std::vector<UpdatedSettings>& GetUpdatedSettings ();
	void SetRollbackFile (IniBasedRollbackFile* pIrbf);
	void SetRollbackFolder (const std::wstring& folder);
	bool GetGpiRetrMode ();
	bool GetCfoRetrMode ();
	void SetSubstitutionMap (const PathSubstitutionMap& _map);
	int GetExpandStringsCalls ();
	size_t GetBytesDownloaded ();
	void SetCancelUpdate (bool cl);
	size_t GetBytesDownloadedBeforeCancelling ();
	NoCaseString GetComp2Lock ();
	NoCaseString GetComp2UnLock ();

	//just for compiling
	int m_bytesDownloaded_depr;
private:

	Callbacks* cbs;
	UpdaterConfiguration upd_config;
	bool blacklistAnswer;
	_checkFilesToDownload check_files_load;
	_checkDownloadedFiles check_downloaded_files;
	_checkPreInstalledComponent check_preinstalled_component;
	FileInfo fiAbsentFile;
	bool bFileOptionalAnswer;
	NoCaseString strRemovedComponent;
	_processReceivedFiles process_recd_files;
	FileInfo fiFileToReplace;
	bool bReplaceFileOnRebootAnswer;
	FileInfo fiFileToRemove;
	bool bRemoveFileOnRebootAnswer;
	FileVector fvSubstitutedIndexes;
	FileInfo sitesXml;
	FileInfo updcfgXml;
	FileInfo primaryIndex;
	_credentials requestCreds;
	_credentials confirmCreds;
	UpdateSchedule updSchedule;
	std::vector<UpdatedSettings> arrUpdatedSettings;
	IniBasedRollbackFile* pRbFile;
	std::wstring rollbackFolder;
	bool gpi_mode;
	bool cfo_mode;
	PathSubstitutionMap pathSubstitutions;
	int intExpandStringCalls;
	size_t btDownloaded;
	Path strRequestRemFile;
	bool bCancel;
	size_t bytesDownloadedBeforeCancelling;
	NoCaseString comp2lock;
	NoCaseString comp2unlock;
	
};

struct Callbacks
{
	Callbacks ()
	{
		for (int i = 0; i < CALLS_NAME_ENUM_SIZE; ++i)
		{
			bFlags[i] = false;
			hReadyEvents[i] = INVALID_HANDLE_VALUE;
			hGoOnEvents[i] = INVALID_HANDLE_VALUE;
		}
	}

	~Callbacks()
	{
		for (int i = 0; i < CALLS_NAME_ENUM_SIZE; ++i)
		{
			if (hReadyEvents[i] != INVALID_HANDLE_VALUE) CloseHandle (hReadyEvents[i]);
			if (hGoOnEvents[i] != INVALID_HANDLE_VALUE) CloseHandle (hGoOnEvents[i]);
		}

	}

	enum callsName
	{
		updaterConfiguration = 0,
		processReceivedFiles,
		processRollbackFileList,
		removeRollbackSection,
		readRollbackFiles,
		refreshIdentifier,
		updateVariableFromIndex,
		updateAutomaticScheduleForUpdater,
		getPrimaryIndex,
		getPrimaryIndexWithEvent,
		getUpdaterConfigurationXml,
		getSitesFileXml,
		percentCallback,
		checkCancel,
		checkIfFileOptional,
		componentRemovedFromUpdate,
		getSubstitutedIndex,
		requestCredentialsForAuthorizationOnProxy,
		//processProxyAuthorizationSuccess,
		checkBlackListFile,
		checkFilesToDownload,
		expandEnvironmentString,
		bytesDownloaded,
		//removeFileOnReboot,
		//replaceFileOnReboot,
		removeLockedFileRequest,
		checkDownloadedFiles,
		checkPreInstalledComponent,
		lockComponentForUpdate,
		unlockComponentForUpdate,
		switchToSystemContext,
		switchToUserContext,

		CALLS_NAME_ENUM_SIZE
	};

	
	bool bFlags[CALLS_NAME_ENUM_SIZE];
	HANDLE hReadyEvents[CALLS_NAME_ENUM_SIZE];
	HANDLE hGoOnEvents[CALLS_NAME_ENUM_SIZE];
	

};

#define _IF(func)\
if (cbs->bFlags[Callbacks::func]) {

#define _ELSE }	else {

#define _ENDIF }


















#endif