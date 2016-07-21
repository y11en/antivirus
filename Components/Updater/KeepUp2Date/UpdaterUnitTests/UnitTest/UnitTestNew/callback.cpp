#include "callback.h"
#include "main.h"
#include "CRTDBG.H"

#define SET_READY_EVENT(id)\
	SetEvent(cbs->hReadyEvents[Callbacks::id]);

#define WAIT_FOR_GOON_EVENT(id)\
	WaitForSingleObject(cbs->hGoOnEvents[Callbacks::id], INFINITE);

void CDownloadProgress::percentCallback(const size_t currentPercentValue)
{
	if (m_cbi) m_cbi->percentCallback (currentPercentValue);
}

bool CDownloadProgress::checkCancel()
{
	if (m_cbi) return m_cbi->checkCancel ();
	return false;
}

bool CDownloadProgress::treat_403_502_httpCodesAs407()
{
	return true;
}

bool CDownloadProgress::requestAuthorizationCredentials (Credentials& cred)
{
	std::wcout<<L"Requesting credentials...\n";
	std::wstring usrname, pwd;
	
	if (m_cbi)
	{
		bool result = m_cbi->requestCredentialsForAuthorizationOnProxy (usrname, pwd);
		if (result)
		{
			cred.userName(usrname);
			cred.password(pwd);
		}
		return result;
	}

	return false;
}

bool CDownloadProgress::authorizationTargetEnabled(const AuthorizationTarget &)
{
	return true;
}

UpdaterConfiguration& TestCallbackInterface::updaterConfiguration()
{
	return upd_config;
}

bool TestCallbackInterface::processReceivedFiles(const FileVector &files, const bool createRollbackNeeded, const Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString &)
{
	//std::wcout<<L"CALL ------------------------------- processReceivedFiles\n";
	_IF(processReceivedFiles)
		process_recd_files.fv = files;
		process_recd_files.createRollbackNeeded = createRollbackNeeded;
		process_recd_files.rollbackFolder = rollbackFolder.m_value.toWideChar();
		process_recd_files.retranslationMode = retranslationMode;
		
		if (createRollbackNeeded && pRbFile)
		{
			pRbFile->saveRollback (files, rollbackFolder, productFolder(retranslationMode), retranslationMode);
		}
		
		SET_READY_EVENT(processReceivedFiles)
		WAIT_FOR_GOON_EVENT(processReceivedFiles)
		return process_recd_files.result;
	_ELSE
		return true;
	_ENDIF
}
void TestCallbackInterface::processRollbackFileList(const FileVector &files, const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- processRollbackFileList\n";
}

bool TestCallbackInterface::removeRollbackSection(const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- removeRollbackSection\n";
	return false;
}

bool TestCallbackInterface::readRollbackFiles(FileVector & files, const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- readRollbackFiles\n";
	
	_IF(readRollbackFiles)
		SET_READY_EVENT(readRollbackFiles)
		WAIT_FOR_GOON_EVENT(readRollbackFiles)
		pRbFile->readRollback(files);
		return true;
	_ELSE
		return false;
	_ENDIF
}

void TestCallbackInterface::refreshIdentifier(const unsigned long installationIdentifier, const unsigned long sessionIdentifier)
{
	//std::wcout<<L"CALL ------------------------------- refreshIdentifier\n";
}

bool TestCallbackInterface::updateVariableFromIndex(const UpdatedSettings & updSetts, const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- updateVariableFromIndex\n";

	_IF(updateVariableFromIndex)
		arrUpdatedSettings.push_back(updSetts);
		SET_READY_EVENT(updateVariableFromIndex)
		WAIT_FOR_GOON_EVENT(updateVariableFromIndex)
		return true;
	_ELSE
		return false;
	_ENDIF
}

void TestCallbackInterface::updateAutomaticScheduleForUpdater(const UpdateSchedule & sched)
{
	//std::wcout<<L"CALL ------------------------------- updateAutomaticScheduleForUpdater\n";

	_IF(updateAutomaticScheduleForUpdater)
		updSchedule = sched;
		SET_READY_EVENT(updateAutomaticScheduleForUpdater)
		WAIT_FOR_GOON_EVENT(updateAutomaticScheduleForUpdater)
	_ENDIF
}

FileInfo TestCallbackInterface::getPrimaryIndex(const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- getPrimaryIndex\n";

	_IF(getPrimaryIndexWithEvent)

		gpi_mode = retranslationMode;
		SET_READY_EVENT(getPrimaryIndexWithEvent)
		WAIT_FOR_GOON_EVENT(getPrimaryIndexWithEvent)
		return primaryIndex;

	_ELSE

		_IF(getPrimaryIndex)
			return primaryIndex;
		_ELSE
			return FileInfo (L"u0607g.xml", L"index/6", FileInfo::index, true);
		_ENDIF
	
	_ENDIF
}

FileInfo TestCallbackInterface::getUpdaterConfigurationXml()
{
	//std::wcout<<L"CALL ------------------------------- getUpdaterConfigurationXml\n";

	_IF(getUpdaterConfigurationXml)
		return updcfgXml;
	_ELSE
		return FileInfo();
	_ENDIF
}

FileInfo TestCallbackInterface::getSitesFileXml()
{
	//std::wcout<<L"CALL ------------------------------- getSitesFileXml\n";

	_IF(getSitesFileXml)
		return sitesXml;
	_ELSE
		return FileInfo();
	_ENDIF
}

void TestCallbackInterface::percentCallback(const size_t currentPercentValue)
{
	//std::wcout<<L"CALL ------------------------------- percentCallback\n";
	
	_IF(percentCallback)
		SET_READY_EVENT(percentCallback)
		WAIT_FOR_GOON_EVENT(percentCallback)
	_ENDIF
}

bool TestCallbackInterface::checkCancel()
{
	//std::wcout<<L"CALL ------------------------------- checkCancel\n";
	
	_IF(checkCancel)
		//std::wcout<<L"return "<<bCancel<<L"\n";
		if (bCancel && !bytesDownloadedBeforeCancelling)
		{
			_ASSERTE (bytesDownloadedBeforeCancelling == 0);
			bytesDownloadedBeforeCancelling = m_dp.bytesTransferred();
		}
		return bCancel;
	_ELSE
		return false;
	_ENDIF
}

bool TestCallbackInterface::checkIfFileOptional(const FileInfo &absentFile, const bool retranslationMode)
{
	std::wcout<<L"CALL ------------------------------- checkIfFileOptional "<<absentFile.m_filename.toWideChar()<<L" retr_mode "<<retranslationMode<<L"\n";
	
	_IF(checkIfFileOptional)
		fiAbsentFile = absentFile;
		cfo_mode = retranslationMode;
		SET_READY_EVENT(checkIfFileOptional)
		WAIT_FOR_GOON_EVENT(checkIfFileOptional)
		return bFileOptionalAnswer;
	_ELSE
		return false;
	_ENDIF
}

void TestCallbackInterface::componentRemovedFromUpdate(const NoCaseString &component)
{
	//std::wcout<<L"CALL ------------------------------- componentRemovedFromUpdate\n";
	_IF(componentRemovedFromUpdate)
		strRemovedComponent = component;
		SET_READY_EVENT(componentRemovedFromUpdate)
		WAIT_FOR_GOON_EVENT(componentRemovedFromUpdate)
	_ENDIF
}

FileVector TestCallbackInterface::getSubstitutedIndex(const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- getSubstitutedIndex\n";
	
	_IF(getSubstitutedIndex)
		SET_READY_EVENT(getSubstitutedIndex)
		WAIT_FOR_GOON_EVENT(getSubstitutedIndex)
		return fvSubstitutedIndexes;
	_ELSE
		return FileVector();
	_ENDIF
}

bool TestCallbackInterface::requestCredentialsForAuthorizationOnProxy(std::wstring &userName, std::wstring &password)
{
	//std::wcout<<L"CALL ------------------------------- requestCredentialsForAuthorizationOnProxy\n";
	
	_IF(requestCredentialsForAuthorizationOnProxy)
		SET_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		WAIT_FOR_GOON_EVENT(requestCredentialsForAuthorizationOnProxy)
		userName = requestCreds.name.toWideChar();
		password = requestCreds.password.toWideChar();
		return requestCreds.result;
	_ELSE
		return false;
	_ENDIF
}


void TestCallbackInterface::processProxyAuthorizationSuccess(const NoCaseString &userName, const NoCaseString &password)
{
	//std::wcout<<L"CALL ------------------------------- processProxyAuthorizationSuccess\n";
/*
	_IF(processProxyAuthorizationSuccess)
		confirmCreds.name = userName;
		confirmCreds.password = password;
		SET_READY_EVENT(processProxyAuthorizationSuccess)
		WAIT_FOR_GOON_EVENT(processProxyAuthorizationSuccess)
	_ENDIF
*/
}

bool TestCallbackInterface::checkBlackListFile(const Path &fileName)
{
	//std::wcout<<L"CALL ------------------------------- checkBlackListFile\n";
	
	_IF(checkBlackListFile)
		return blacklistAnswer;
	_ELSE
		return true;
	_ENDIF
}

bool TestCallbackInterface::checkFilesToDownload(FileVector & fv, const NoCaseString &updateDate, const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- checkFilesToDownload\n";
	
	_IF(checkFilesToDownload)
		// проверить
		check_files_load.m_matchFileList = &fv;
		check_files_load.updateDate = updateDate;
		check_files_load.retranslationMode = retranslationMode;
		SET_READY_EVENT(checkFilesToDownload)
		WAIT_FOR_GOON_EVENT(checkFilesToDownload)
		return check_files_load.result;
	_ELSE
		//return KLUPD::CORE_NO_ERROR;
		return true;
	_ENDIF
		
}

bool TestCallbackInterface::expandEnvironmentString(const NoCaseString &input, NoCaseString &output, const StringParser::ExpandOrder&)
{
	//std::wcout<<L"CALL ------------------------------- expandEnvironmentString: "<<input.to_wstring().c_str()<<L"\n";

	intExpandStringCalls++;

	_IF(expandEnvironmentString)

		PathSubstitutionMap::const_iterator findIter = std::find_if(pathSubstitutions.begin(),
			pathSubstitutions.end(), PathSubstitutionFinderByKey(input));

		if(findIter != pathSubstitutions.end())
			output = findIter->second;
		
		if (output != L"") return true;
	_ENDIF
		return false;
}

/*
bool TestCallbackInterface::removeFileOnReboot(const FileInfo & fi, const NoCaseString &rollbackFolder)
{
	//std::wcout<<L"CALL ------------------------------- removeFileOnReboot\n";
	
	_IF(removeFileOnReboot)
		fiFileToRemove = fi;
		SET_READY_EVENT(removeFileOnReboot)
		WAIT_FOR_GOON_EVENT(removeFileOnReboot)
		return bRemoveFileOnRebootAnswer;
	_ELSE
		return false;
	_ENDIF
}

bool TestCallbackInterface::replaceFileOnReboot(const FileInfo & fi, const NoCaseString &rollbackFolder)
{
	//std::wcout<<L"CALL ------------------------------- replaceFileOnReboot\n";

	_IF(replaceFileOnReboot)
		fiFileToReplace = fi;
		SET_READY_EVENT(replaceFileOnReboot)
		WAIT_FOR_GOON_EVENT(replaceFileOnReboot)
		return bReplaceFileOnRebootAnswer;
	_ELSE
		return false;
	_ENDIF
}
*/

void TestCallbackInterface::removeLockedFileRequest(const Path &fileName)
{
	//std::wcout<<L"CALL ------------------------------- removeLockedFileRequest\n";

	_IF(removeLockedFileRequest)
		strRequestRemFile = fileName;
		SET_READY_EVENT(removeLockedFileRequest)
		WAIT_FOR_GOON_EVENT(removeLockedFileRequest)
	_ENDIF
}

//new callbacks
bool TestCallbackInterface::checkDownloadedFiles(const FileVector & fv, const bool retranslationMode)
{
	//std::wcout<<L"CALL ------------------------------- checkDownloadedFiles\n";
	_IF(checkDownloadedFiles)
		check_downloaded_files.fv = fv;
		check_downloaded_files.retranslationMode = retranslationMode;
		SET_READY_EVENT(checkDownloadedFiles)
		WAIT_FOR_GOON_EVENT(checkDownloadedFiles)
		return check_downloaded_files.result;
	_ELSE
		return true;
	_ENDIF
}

bool TestCallbackInterface::checkPreInstalledComponent(const Path &suffix, const NoCaseString &componentIdentifier, const FileVector & fv)
{
	//std::wcout<<L"CALL ------------------------------- checkPreInstalledComponent "<<componentIdentifier.c_str()<<L"\n";
	_IF(checkPreInstalledComponent)
		check_preinstalled_component.suffix = suffix;
		check_preinstalled_component.compId = componentIdentifier;
		check_preinstalled_component.fv = fv;
		SET_READY_EVENT(checkPreInstalledComponent)
		WAIT_FOR_GOON_EVENT(checkPreInstalledComponent)
		return check_preinstalled_component.result;
	_ELSE
		return true;
	_ENDIF
}

void TestCallbackInterface::lockComponentForUpdate(const NoCaseString &componentIdentifier)
{
	//std::wcout<<L"CALL ------------------------------- lockComponentForUpdate "<<componentIdentifier.c_str()<<L"\n";
	_IF(lockComponentForUpdate)
		comp2lock = componentIdentifier;
		SET_READY_EVENT(lockComponentForUpdate)
		WAIT_FOR_GOON_EVENT(lockComponentForUpdate)
	_ENDIF
}

void TestCallbackInterface::unlockComponentForUpdate(const NoCaseString &componentIdentifier)
{
	//std::wcout<<L"CALL ------------------------------- unlockComponentForUpdate "<<componentIdentifier.c_str()<<L"\n";
	_IF(unlockComponentForUpdate)
		comp2unlock = componentIdentifier;
		SET_READY_EVENT(unlockComponentForUpdate)
		WAIT_FOR_GOON_EVENT(unlockComponentForUpdate)
	_ENDIF
}

void TestCallbackInterface::switchToSystemContext()
{
	//std::wcout<<L"CALL ------------------------------- switchToSystemContext\n";
	_IF(switchToSystemContext)
		SET_READY_EVENT(switchToSystemContext)
		WAIT_FOR_GOON_EVENT(switchToSystemContext)
	_ENDIF
}

void TestCallbackInterface::switchToUserContext()
{
	//std::wcout<<L"CALL ------------------------------- switchToUserContext\n";
	_IF(switchToUserContext)
		SET_READY_EVENT(switchToUserContext)
		WAIT_FOR_GOON_EVENT(switchToUserContext)
	_ENDIF
}
////////////////////////////////////

const Path& TestCallbackInterface::GetRequestFileToRemove ()
{
	return strRequestRemFile;
}

void TestCallbackInterface::SetUpdaterConfiguration (const UpdaterConfiguration& _upd_config)
{
	upd_config = _upd_config;
}

void TestCallbackInterface::SetBlacklistAnswer (bool answer)
{
	blacklistAnswer = answer;
}

TestCallbackInterface::_checkFilesToDownload& TestCallbackInterface::GetCheckFilesToDownload ()
{
	return check_files_load;
}

const FileInfo& TestCallbackInterface::GetAbsentFile()
{
	return fiAbsentFile;
}

void TestCallbackInterface::SetOptionalFileAnswer (bool answer)
{
	bFileOptionalAnswer = answer;
}

const NoCaseString& TestCallbackInterface::GetRemovedComponent()
{
	return strRemovedComponent;
}

TestCallbackInterface::_processReceivedFiles& TestCallbackInterface::GetProcessReceivedFiles ()
{
	return process_recd_files;
}

TestCallbackInterface::_checkDownloadedFiles& TestCallbackInterface::GetCheckDownloadedFiles ()
{
	return check_downloaded_files;
}

const FileInfo& TestCallbackInterface::GetFileToReplace()
{
	return fiFileToReplace;
}

void TestCallbackInterface::SetReplaceFileResult (bool answer)
{
	bReplaceFileOnRebootAnswer = answer;
}

const FileInfo& TestCallbackInterface::GetFileToRemove()
{
	return fiFileToRemove;
}

void TestCallbackInterface::SetRemoveFileResult (bool answer)
{
	bRemoveFileOnRebootAnswer = answer;
}

void TestCallbackInterface::SetSubstitutedIndexes (const FileVector& indexes)
{
	fvSubstitutedIndexes = indexes;
}

void TestCallbackInterface::SetSitesXml (const FileInfo& fi)
{
	sitesXml = fi;
}

void TestCallbackInterface::SetUpdcfgXml (const FileInfo& fi)
{
	updcfgXml = fi;
}

void TestCallbackInterface::SetPrimaryIndex (const FileInfo& fi)
{
	primaryIndex = fi;
}

void TestCallbackInterface::SetRequestCreds (const _credentials& creds)
{
	requestCreds = creds;
}
const TestCallbackInterface::_credentials& TestCallbackInterface::GetConfirmCreds ()
{
	return confirmCreds;
}

const UpdateSchedule& TestCallbackInterface::GetUpdaterSchedule ()
{
	return updSchedule;
}

const std::vector<UpdatedSettings>& TestCallbackInterface::GetUpdatedSettings ()
{
	return arrUpdatedSettings;
}

void TestCallbackInterface::SetRollbackFile (IniBasedRollbackFile* pIrbf)
{
	pRbFile = pIrbf;
}

void TestCallbackInterface::SetRollbackFolder (const std::wstring& folder)
{
	rollbackFolder = folder;
}

bool TestCallbackInterface::GetGpiRetrMode ()
{
	return gpi_mode;
}

bool TestCallbackInterface::GetCfoRetrMode ()
{
	return cfo_mode;
}


void TestCallbackInterface::SetSubstitutionMap (const PathSubstitutionMap& _map)
{
	pathSubstitutions = _map;
}

int TestCallbackInterface::GetExpandStringsCalls ()
{
	return intExpandStringCalls;
}

size_t TestCallbackInterface::GetBytesDownloaded ()
{
	return btDownloaded;
}

void TestCallbackInterface::SetCancelUpdate (bool cl)
{
	bCancel = cl;
}

size_t TestCallbackInterface::GetBytesDownloadedBeforeCancelling ()
{
	return bytesDownloadedBeforeCancelling;
}

TestCallbackInterface::_checkPreInstalledComponent& TestCallbackInterface::GetCheckPreinstalledComponent ()
{
	return check_preinstalled_component;
}

NoCaseString TestCallbackInterface::GetComp2Lock ()
{
	return comp2lock;
}

NoCaseString TestCallbackInterface::GetComp2UnLock ()
{
	return comp2unlock;
}