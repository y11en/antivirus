#include "ini_helper.h"

//#include "ini_updater_config.h"
#include "ini_updater_lists.h"


#define UNKNOWN_STRING "***UnknownString***"
#define MAX_STR_BUFF 1000

Ini_UpdaterLists::Ini_UpdaterLists(const bool retranslation, Log *log, const bool useCurrentFolder)
 : UpdaterLists(retranslation, log),
   m_useCurrentFolder(useCurrentFolder)
{
}

Ini_UpdaterLists::~Ini_UpdaterLists()
{
}

bool Ini_UpdaterLists::processLocalFiles(const FileVector &)
{
    // not implemented in this product, because not needed
    TRACE_MESSAGE("Process local update information is not implemented in this product");
    return true;
}

bool Ini_UpdaterLists::readSingleFileInfoRollback(const TCHAR *iniFileName, TCHAR *fileSectionName, FileInfo &file)
{
    if(!iniFileName || (*iniFileName == 0))
    {
        TRACE_MESSAGE("Read rollback file information error: configuration file name is not specified");
        return false;
    }
    if(!fileSectionName)
    {
        TRACE_MESSAGE("Read rollback file information error: configuration section is not specified");
        return false;
    }
    
    TCHAR buffer[MAX_STR_BUFF + 1] = _T("");
    
    // file name
    GetPrivateProfileString(fileSectionName, STRING(SS_KEY_RecentFileName).c_str(), _T(UNKNOWN_STRING), buffer, MAX_STR_BUFF, iniFileName);
    if(!_tcscmp(buffer, _T(UNKNOWN_STRING)))
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%s' value found", STRING(SS_KEY_RecentFileName).to_string().c_str());
        return false;
    }
    file.m_filename = buffer;

    // local path
    GetPrivateProfileString(fileSectionName, STRING(SS_KEY_RecentLocalPath).c_str(), _T(UNKNOWN_STRING), buffer, MAX_STR_BUFF, iniFileName);
    if(!_tcscmp(buffer, _T(UNKNOWN_STRING)))
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%s' value found", STRING(SS_KEY_RecentLocalPath).to_string().c_str());
        return false;
    }
    file.m_localPath = buffer;

    // change status
    GetPrivateProfileString(fileSectionName, STRING(SS_KEY_ChangeStatus).c_str(), _T(UNKNOWN_STRING), buffer, MAX_STR_BUFF, iniFileName);
    if(!_tcscmp(buffer, _T(UNKNOWN_STRING)))
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%s' value found", STRING(SS_KEY_ChangeStatus).to_string().c_str());
        return false;
    }
    file.m_rollbackChange = buffer;

    // md5 for consistency check
    GetPrivateProfileString(fileSectionName, STRING(kayw_MD5).c_str(), _T(UNKNOWN_STRING), buffer, MAX_STR_BUFF, iniFileName);
    if(_tcscmp(buffer, _T(UNKNOWN_STRING)))
        file.m_md5 = CBase64::decodeBuffer(STRING(buffer).to_string().c_str());

    TRACE_MESSAGE2("Rollback information read: '%s'", file.toString().c_str());
    return true;
}


bool Ini_UpdaterLists::readRollbackFileListFromSS(FileVector &changedFiles)
{
    const STRING section_name = SS_KEY_RollbackTree;
    
    STRING rollbackFileAndPath;
    if(!getProductFolder(m_useCurrentFolder, rollbackFileAndPath, pLog))
        return false;
    rollbackFileAndPath += UPDATER_LISTS_FILENEME;
    
    TCHAR childSectionName[MAX_PATH + 1];
    
    for(int _i = 0;; ++_i)
    {
        TCHAR szKeyName[MAX_PATH + 1];
        TCHAR fullSectionName[MAX_PATH + MAX_PATH + 1];
        
        memset(childSectionName, 0, (MAX_PATH + 1) * sizeof(TCHAR));
        memset(szKeyName, 0, (MAX_PATH + 1) * sizeof(TCHAR));
        memset(fullSectionName, 0, (MAX_PATH + MAX_PATH + 1) * sizeof(TCHAR));
        
        _stprintf(szKeyName, _T("entry%d"), _i);
        
        GetPrivateProfileString(section_name.c_str(), szKeyName, _T(""), childSectionName, MAX_PATH, rollbackFileAndPath.c_str());
        
        if(!childSectionName[0])
            break;
        
        _tcscpy(fullSectionName, section_name.c_str());
        _tcscat(fullSectionName, _T(":"));
        _tcscat(fullSectionName, childSectionName);
        
        FileInfo rec;
        if(!readSingleFileInfoRollback(rollbackFileAndPath.c_str(), fullSectionName, rec))
        {
            TRACE_MESSAGE2("Invalid content of '%s' section, skipping", fullSectionName);
            // ignore error, try next entry if any
            continue;
        }
        
        changedFiles.push_back(rec);
    }
    
    return true;
}

void Ini_UpdaterLists::makeNewSectionName(const long index, const STRING &name, STRING &outputName)
{
    char buffer[32];
    memset(buffer, 0, 32);
    ltoa(index, buffer, 10);
    outputName = STRING(buffer) + STRING(L"(") + name + STRING(L")");
}

bool Ini_UpdaterLists::saveRollbackListEntry(const TCHAR *iniFileName, const TCHAR *parentSectionName, const long orderNumber, const STRING &newSectionName, const FileInfo &file)
{
    if(!iniFileName || (*iniFileName == 0) || !parentSectionName || (*parentSectionName == 0))
    {
        TRACE_MESSAGE("Unable to save rollback entry information: invalid parameter");
        return false;
    }
    
    TCHAR szEntryKeyName[MAX_PATH+1];
    _stprintf(szEntryKeyName,_T("entry%d"), orderNumber);
    
    WritePrivateProfileString(parentSectionName, szEntryKeyName, STRING(newSectionName).c_str(), iniFileName);
    
    TCHAR fullSectionName[MAX_PATH+MAX_PATH+1];
    _tcscpy(fullSectionName, parentSectionName);
    _tcscat(fullSectionName, _T(":"));
    _tcscat(fullSectionName, STRING(newSectionName).c_str());
    
    // file name 
    WritePrivateProfileString(fullSectionName, STRING(SS_KEY_RecentFileName).c_str(), file.m_filename.c_str(), iniFileName);
    // local path
    WritePrivateProfileString(fullSectionName, STRING(SS_KEY_RecentLocalPath).c_str(), file.m_localPath.c_str(), iniFileName);
    // change status
    WritePrivateProfileString(fullSectionName, STRING(SS_KEY_ChangeStatus).c_str(), file.m_rollbackChange.c_str(), iniFileName);

    // md5 for consistency check
    if(!file.m_md5.empty())
    {
        std::vector<char> md5 = CBase64::encodeBuffer(&file.m_md5[0], file.m_md5.size());
        md5.insert(md5.end(), 0);
        WritePrivateProfileString(fullSectionName, STRING(kayw_MD5).c_str(), STRING(&md5[0]).c_str(), iniFileName);
    }
    
    return true;
}

bool Ini_UpdaterLists::saveRollbackInfo(const FileVector &rollbackList)
{
    STRING section_name = SS_KEY_RollbackTree;
    long k = 0;
    
    STRING rollbackFileAndPath;
    if(!getProductFolder(m_useCurrentFolder, rollbackFileAndPath, pLog))
    {
        TRACE_MESSAGE("Unable to save rollback information, because failed to get product folder");
        return false;
    }
    rollbackFileAndPath += UPDATER_LISTS_FILENEME;
    
    TRACE_MESSAGE2("Saving rollback list to '%s'", rollbackFileAndPath.to_string().c_str());

    removeRollbackSection();

    // for output saved entries in rows and save space in trace file
    std::string savedEntries;
    unsigned long logLineSplitIndex = 0;

    for(FileVector::const_iterator iter = rollbackList.begin(); iter != rollbackList.end(); ++iter)
    {
        if(iter->m_rollbackChange == STRING(SS_KEY_RecentStatusAdded)
            || iter->m_rollbackChange == STRING(SS_KEY_RecentStatusModified)
            || iter->m_rollbackChange == STRING(SS_KEY_RecentStatusDeleted))
        {
            STRING newSectionName;
            const long order_number = k++;
            makeNewSectionName(order_number, iter->m_filename, newSectionName);
            
            if(!saveRollbackListEntry(rollbackFileAndPath.c_str(), section_name.c_str(), order_number, newSectionName, *iter))
            {
                TRACE_MESSAGE2("Unable to save rollback information for file entry '%s'", (iter->m_localPath + iter->m_filename).to_string().c_str());
                return false;
            }


            // append comma before all items, but the very first one
            if(logLineSplitIndex++)
                savedEntries += ", ";
            // split line each 3 elements, but not before last element
            if(iter + 1 != rollbackList.end())
            {
                if(!(logLineSplitIndex % 3))
                    savedEntries += "\n\t";
            }
            savedEntries += (iter->m_localPath + iter->m_filename).to_string();
        }
    }

    TRACE_MESSAGE3("Rollback information saved successfully to %s, entries: %s", rollbackFileAndPath.to_string().c_str(), savedEntries.c_str());
    return true;
}

bool Ini_UpdaterLists::processChangedFiles(const STRING &rollbackFolder, const bool validLocalFilesForRollback)
{
    // Ini updater can not perform rollback in retranslation mode
    if(m_retranslation)
        return false;

    if(!validLocalFilesForRollback)
    {
        TRACE_MESSAGE("Rollback is not created, because inconsistent bases set was locally before update operation started");
        removeRollbackSection();
        return false;
    }
    
    TRACE_MESSAGE("Saving rollback file list");
    
    FileVector updateList;
    
    // set correct status for all entries that have changed
    std::string rollbackTrace;
    for(size_t index = 0; index < m_changedFiles.size(); ++index)
    {
        // entry copy, because size, signature, and hash tags are not valid here.
        //  There are values not for rollback, but values for new files
		FileInfo entryCopy;
        entryCopy.m_filename = m_changedFiles[index].m_filename;
        entryCopy.m_localPath = m_changedFiles[index].m_localPath;

		entryCopy.m_rollbackChange;
        switch(m_changedFiles[index].m_transactionInformation.m_changeStatus)
		{
		case FileInfo::added:
			entryCopy.m_rollbackChange = SS_KEY_RecentStatusAdded;
			break;

		case FileInfo::deleted:
			entryCopy.m_rollbackChange = SS_KEY_RecentStatusDeleted;
			break;
		
		case FileInfo::modified:
			entryCopy.m_rollbackChange = SS_KEY_RecentStatusModified;
			break;

		default:
            // do not write to rollback settings storage unchanged files
            continue;
		}

        // calculate MD5 for consistency check to restore changed and deleted files
        if((entryCopy.m_rollbackChange != SS_KEY_RecentStatusAdded)
            && !calcMD5Hash(STRING(rollbackFolder + entryCopy.m_filename).to_string().c_str(), entryCopy.m_md5))
        {
            TRACE_MESSAGE2("Rollback is not created, failed to calculate md5 for '%s'", (rollbackFolder + entryCopy.m_filename).to_string().c_str());
            return false;
        }
		updateList.push_back(entryCopy);

        if(index != 0)
            rollbackTrace += ", ";
        if(!(index % 15))
            rollbackTrace += "\n\t";
        rollbackTrace += entryCopy.m_filename.to_string() + " " + entryCopy.m_rollbackChange.to_string().c_str();
    }

    TRACE_MESSAGE2("Saving rollback files: %s", rollbackTrace.c_str());
    
    // saving rollback information
    if(!saveRollbackInfo(updateList))
    {
        TRACE_MESSAGE("Failed to save rollback file list");
        return false;
    }
    
    TRACE_MESSAGE("Rollback file list saved successfully");
    return true;
}

bool Ini_UpdaterLists::removeRollbackSection()
{
    STRING rollbackFileAndPath;
    if(!getProductFolder(m_useCurrentFolder, rollbackFileAndPath, pLog))
        return false;
    rollbackFileAndPath += UPDATER_LISTS_FILENEME;
    
    const STRING section_name = SS_KEY_RollbackTree;
    TCHAR childSectionName[MAX_PATH+1];
    
    for(int _i = 0; ; ++_i)
    {
        TCHAR szKeyName[MAX_PATH+1];
        TCHAR fullSectionName[MAX_PATH+MAX_PATH+1];
        
        memset(childSectionName, 0, (MAX_PATH+1) * sizeof(TCHAR));
        memset(szKeyName, 0, (MAX_PATH+1) * sizeof(TCHAR));
        memset(fullSectionName, 0, (MAX_PATH+MAX_PATH+1) * sizeof(TCHAR));
        
        _stprintf(szKeyName, _T("entry%d"), _i);
        
        GetPrivateProfileString(section_name.c_str(), szKeyName, _T(""), childSectionName, MAX_PATH, rollbackFileAndPath.c_str());
        
        if(!childSectionName[0])
            break;
        
        _tcscpy(fullSectionName, section_name.c_str());
        _tcscat(fullSectionName, _T(":"));
        _tcscat(fullSectionName, childSectionName);
        
        WritePrivateProfileString(fullSectionName, 0, 0, rollbackFileAndPath.c_str());
    }
    
    WritePrivateProfileString(STRING(SS_KEY_RollbackTree).c_str(), 0, 0, rollbackFileAndPath.c_str());
    return true;
}

