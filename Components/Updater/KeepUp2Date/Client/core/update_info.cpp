#include "../helper/stdinc.h"

#include "update_info.h"

#include "../helper/indexFileXMLParser.h"
#include "../helper/updaterStaff.h"

KLUPD::UpdateInfo::UpdateInfo(const UpdaterConfiguration::ListFlags &filterFlags,
                              const ComponentIdentefiers &filterComponents,
                              Signature6Checker &signature6Checker,
                              const bool retranslationMode,
                              CallbackInterface &callbacks,
                              Log *log)
 : m_signature6Checker(signature6Checker),
   m_retranslationMode(retranslationMode),
   m_filterFlags(filterFlags),
   m_components(filterComponents),
   m_callbacks(callbacks),
   pLog(log)
{
}

KLUPD::CoreError KLUPD::UpdateInfo::parse(const Path &path,
                                          FileInfo &fileInfo,
                                          const ChangeStateCheck &changeStateCheck,
                                          const FileVector &localFilesToCheckAgainst,
                                          const bool suppressSuccessLogMessages)
{
    // index is already parsed, files already attached
    if(fileAlreadyParsed(fileInfo))
        return CORE_NO_ERROR;

    const Path fullFileName = path + fileInfo.m_filename;

    if(!suppressSuccessLogMessages)
        TRACE_MESSAGE2("Parsing XML index file '%S'", fullFileName.toWideChar());

    std::vector<unsigned char> buffer;
    const CoreError readFileResult = LocalFile(fullFileName, pLog).read(buffer);
    if(!KLUPD::isSuccess(readFileResult))
    {
        TRACE_MESSAGE3("Failed to read XML index file '%S', result '%S'",
            fullFileName.toWideChar(), KLUPD::toString(readFileResult).toWideChar());
        return readFileResult;
    }
    if(buffer.empty())
    {
        TRACE_MESSAGE3("Index XML file '%S' is corrupted, file size is %d", fullFileName.toWideChar(), buffer.size());
        return CORE_UPDATE_DESCRIPTION_DAMAGED;
    }

    // a file list obtained from XML
    FileVector fileList;
    IndexFileXMLParser xmler(fileInfo.m_filename.m_value, fileList, m_signature6Checker, fileInfo.m_relativeURLPath, m_retranslationMode, m_callbacks, pLog);

    XmlReader xmlReader(reinterpret_cast<const char *>(&buffer[0]), buffer.size());
    if(!xmler.parseXMLRecursively(xmlReader, 0)
        || fileList.empty())
    {
        TRACE_MESSAGE3("\tXML file parse error '%S', file number found %d",
            fullFileName.toWideChar(), fileList.size());
        return CORE_UPDATE_DESCRIPTION_DAMAGED;
    }
    m_parsedIndexCache.push_back(fileInfo);

    xmler.GetUpdateDate(fileInfo.m_strUpdateDate);
    xmler.GetBlackDate(fileInfo.m_strBlackDate);

    // save variables to write into settings storage
    std::vector<UpdatedSettings> updatedSettings = xmler.updatedSettings();
    m_updatedSettings.insert(m_updatedSettings.end(), updatedSettings.begin(), updatedSettings.end());

    // copy to match list only those items which suits download criteria
    for(FileVector::iterator fileIter = fileList.begin(); fileIter != fileList.end(); ++fileIter)
    {
        // check download filters criteria should be performed
        NoCaseString reasonNotMatch;
        if(!matchesSettings(reasonNotMatch, *fileIter))
        {
            if(!suppressSuccessLogMessages)
            {
                TRACE_MESSAGE3("\tFile filtered, download criteria does not matches: '%S', %S",
                    reasonNotMatch.toWideChar(), fileIter->toString().toWideChar());
            }
            continue;
        }

        // check download mandatory criteria
        reasonNotMatch.erase();
        if(!isEntryRequired(reasonNotMatch, *fileIter, m_filterFlags))
        {
            if(!suppressSuccessLogMessages)
            {
                TRACE_MESSAGE3("\tFile filtered, mandatory criteria does not matches: '%S', %S",
                    reasonNotMatch.toWideChar(), fileIter->toString().toWideChar());
            }
            continue;
        }

        // setup transaction folders
        fileIter->m_transactionInformation.m_currentLocation = m_callbacks.productFolder(*fileIter, m_retranslationMode);
        fileIter->m_transactionInformation.m_newLocation = m_callbacks.temporaryFolder(*fileIter);
        NoCaseString statusExplanations;
        fileIter->m_transactionInformation.m_changeStatus = getFileStatusAgainstLocal(*fileIter,
            changeStateCheck, localFilesToCheckAgainst, statusExplanations);

        // insert file with filtering duplicates
        bool fileDuplicate = false;

        // insert black list into the beginning of list, because it should be checked before downloading other bases
        if(fileIter->m_type == FileInfo::blackList)
            m_matchFileList.insertNewInTheBeginOfListOrUpdateTheSame(*fileIter, *fileIter, fileDuplicate, m_retranslationMode);
        else
            m_matchFileList.insertNewOrUpdateTheSame(*fileIter, *fileIter, fileDuplicate, m_retranslationMode);


        if(!suppressSuccessLogMessages)
        {
            // to avoid empty brackets output to trace
            if(!statusExplanations.empty())
                statusExplanations = NoCaseString(L"(") + statusExplanations + L")";

            if(fileDuplicate)
            {
                TRACE_MESSAGE3("\tDuplicate file information updated: %S %S",
                    fileIter->toString().toWideChar(), statusExplanations.toWideChar());
            }
            else
            {
                TRACE_MESSAGE3("\tFile matches download criteria: %S %S",
                    fileIter->toString().toWideChar(), statusExplanations.toWideChar());
            }
        }
    }

    return CORE_NO_ERROR;
}

KLUPD::FileInfo::ChangeStatus KLUPD::UpdateInfo::getFileStatusAgainstLocal(const FileInfo &etalon,
    const ChangeStateCheck &changeStateCheck, const FileVector &localFilesToCheckAgainst, NoCaseString &statusExplanations)const
{
    if(changeStateCheck == noStateChangeCheck)
        return FileInfo::unchanged;

    statusExplanations.erase();

    LocalFile fileInProductFolder(m_callbacks.productFolder(etalon, m_retranslationMode) + etalon.m_filename);

    //////////////////////////////////////////////////////////////////////////
    // check if file in product folder exists
    if(!fileInProductFolder.exists())
    {
        statusExplanations = L"original files does not exist";
        return FileInfo::added;
    }

    // inlined file is already downloaded together with index file, can be only compared if it changed
    if(etalon.isInlined())
    {
        std::vector<unsigned char> buffer;
        const CoreError readInlinedFileResult = fileInProductFolder.read(buffer);
        if(!isSuccess(readInlinedFileResult))
        {
            TRACE_MESSAGE3("Failed to detect change state for inlined file '%S', file operation error '%S'",
                fileInProductFolder.m_name.toWideChar(), toString(readInlinedFileResult).toWideChar());
            statusExplanations = L"inlined file can not be read";
            return FileInfo::added;
        }

        if(buffer == etalon.m_inlinedContent)
        {
            statusExplanations = L"inlined file is not changed";
            return FileInfo::unchanged;
        }
        else
        {
            statusExplanations = L"inlined file is not modified";
            return FileInfo::modified;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    // check against signature 5
    if(!etalon.m_signature5.empty())
    {
        if(checkInsideSignature5AgainstEtalon(fileInProductFolder.m_name, etalon.m_signature5, pLog))
        {
            statusExplanations = L"signature 5 is not changed";
            return FileInfo::unchanged;
        }
        else
        {
            statusExplanations = L"signature 5 is changed";
            return FileInfo::modified;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // check file against md5
    if(!etalon.m_md5.empty())
    {
        std::vector<unsigned char> md5;
        if(!calcMD5Hash(fileInProductFolder.m_name, md5))
        {
            statusExplanations = L"failed to calculate md5 hash";
            return FileInfo::added;
        }

        if(md5 == etalon.m_md5)
        {
            statusExplanations = L"md5 unchanged";
            return FileInfo::unchanged;
        }
        else
        {
            statusExplanations = L"md5 changed";
            return FileInfo::modified;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // check date

    statusExplanations += NoCaseString(L"date from index '") + etalon.m_dateFromIndex + L"', "
        + (localFilesToCheckAgainst.empty() ? L"local bases are not consistent" : L"local bases are consistent");

    if(!etalon.m_dateFromIndex.empty()
        // date check is only performed if file presents in consistent local file set,
        // otherwise file may belong to other Update set and must be updated
        && !localFilesToCheckAgainst.empty())
    {
        FileInfo::ChangeStatus modifiedIfDatesAreIgnored = FileInfo::unchanged;

        // check if index date is newer is only performed for Kaspersky sources
        if(changeStateCheck == checkStateButIgnoreDate)
        {
            modifiedIfDatesAreIgnored = FileInfo::modified;
            statusExplanations = L", dates are ignored for user-specified sources";
        }

        FileInfo localConsistentFile;
        if(localFilesToCheckAgainst.findTheSameFile(etalon, localConsistentFile, m_retranslationMode))
        {
            bool bIndex = etalon.isIndex();
            const NoCaseString& strLocalFileDate = bIndex ? localConsistentFile.m_strUpdateDate : localConsistentFile.m_dateFromIndex;
            NoCaseString strFileType(bIndex ? L"index" : L"non-index"); // for trace

            if(!strLocalFileDate.empty())
            {
                // if expected date on source is less, then file is skipped
                unsigned long nEtalonDate = updateDateStringToSequenceNumber(etalon.m_dateFromIndex);
                unsigned long nLocalFileDate = updateDateStringToSequenceNumber(strLocalFileDate);
                if(nEtalonDate < nLocalFileDate)
                {
                    statusExplanations += NoCaseString(L", ") + FileInfo::toString(modifiedIfDatesAreIgnored).toWideChar() +
                        L" " + strFileType + L" file is found in local consistent set with date '" + localConsistentFile.m_dateFromIndex + L"'";
                    return modifiedIfDatesAreIgnored;
                }
                else if( bIndex || nEtalonDate > nLocalFileDate)
                {
                    statusExplanations += NoCaseString(L", modified ") + strFileType + L" file is found in local consistent set, date from file '" + strLocalFileDate + L"'";
                    return FileInfo::modified;
                }
                else
                    statusExplanations +=  NoCaseString(L", ") + strFileType + L" file is found in local consistent set with same date";
            }
            else
                statusExplanations += NoCaseString(L", ") + strFileType + L" file is found in local consistent set, but there is no date in index specified";
        }
        else
            statusExplanations += L", file is not found in local consistent set";
    }

    //////////////////////////////////////////////////////////////////////////
    // check against signature 6

    bool insideSignature = false;
    NoCaseString errorMessage;
    if(!m_signature6Checker.checkSignature(fileInProductFolder.m_name, insideSignature, errorMessage))
    {
        statusExplanations += NoCaseString(L", signature 6 check changed (") + errorMessage + L")";
        return FileInfo::modified;
    }

    // file may contain inside signature6, then there is no way to determine whether
     //  file is different on source, and that is why must always be downloaded
    if(insideSignature)
    {
        statusExplanations += L", inside signature 6 check unchanged (file needs being downloaded)";
        return FileInfo::modified;
    }
    else
    {
        statusExplanations += L", signature 6 check unchanged";
        return FileInfo::unchanged;
    }
}

bool KLUPD::UpdateInfo::fileAlreadyParsed(const FileInfo &file)const
{
    return m_parsedIndexCache.theSameFilePresents(file, m_retranslationMode);
}

bool KLUPD::UpdateInfo::checkIfFileOptional(const FileInfo absentFile)
                    // absentFile is a copy, because update file set is modified
{
    // in case component identifier is not specified, then it is primary index or other helper file
    for(ComponentIdentefiers::const_iterator componentIter = absentFile.m_componentIdSet.begin();; ++componentIter)
    {
        if(componentIter == absentFile.m_componentIdSet.end())
        {
            TRACE_MESSAGE2("Error: component identifier is not specified '%S', file is mandatory and can not be removed from update",
                absentFile.toString().toWideChar());
            return false;
        }
        // non-empty component found
        if(!componentIter->empty())
            break;
    }

    // check if any of component's file is mandatory
    for(FileVector::const_iterator fileIter = m_matchFileList.begin(); fileIter != m_matchFileList.end(); ++fileIter)
    {
        // search if absent file has the same component identifier that absent file
        ComponentIdentefiers::const_iterator componentIter = std::find_first_of(absentFile.m_componentIdSet.begin(),
            absentFile.m_componentIdSet.end(), fileIter->m_componentIdSet.begin(), fileIter->m_componentIdSet.end());

        // not match by component
        if(componentIter == absentFile.m_componentIdSet.end())
            continue;

        if(!m_callbacks.checkIfFileOptional(*fileIter, m_retranslationMode))
        {
            TRACE_MESSAGE3("Error: missing file '%S' which is mandatory for '%S'",
                absentFile.toString().toWideChar(), fileIter->toString().toWideChar());
            return false;
        }
    }

    TRACE_MESSAGE2("File '%S' is optional for update",
        absentFile.toString().toWideChar());
    return true;
}

void KLUPD::UpdateInfo::removeComponentFromUpdate(
    const ComponentIdentefiers componentsToRemove,   // componentsToRemove - copy because when file is deleted iterator becomes invalid)
    JournalInterface &journal)
{
    // removing component from retranslation filter
    for(ComponentIdentefiers::const_iterator iter = componentsToRemove.begin(); iter != componentsToRemove.end(); ++iter)
    {
        TRACE_MESSAGE2("Warning: removing components from update %S", iter->toWideChar());

        // copy is needed, because iter will be invalid after remove from list
        const NoCaseString removingComponent = *iter;

        // removing from retranslation filter
        m_components.erase(std::remove_if(m_components.begin(), m_components.end(),
            std::bind2nd(std::equal_to<NoCaseString>(), removingComponent)), m_components.end());

        m_componentsRemovedFromUpdate.push_back(removingComponent);

		journal.publishMessage
			(
				m_retranslationMode ? EVENT_ComponentIsNotRetranslated : EVENT_ComponentIsNotUpdated, 
				removingComponent
			);
    }

    // removing file by component filter criteria
    if(!componentsToRemove.empty())
        filterByComponents(componentsToRemove);
}

void KLUPD::UpdateInfo::filterByComponents(// filter parameter is passed *by value*
                                    const ComponentIdentefiers componentIdSet)
{
    // filtering is not needed
    if(componentIdSet.empty())
        return;

    for(size_t i = 0; i < m_matchFileList.size();)
    {
        // do not filter in case component is not set
        FileVector::iterator removeIter = m_matchFileList.begin();
        std::advance(removeIter, i);
        
        // there is the only component with specified identifier in component set
        if(removeIter->m_componentIdSet.size() == 1)
        {
            // check if the only component in a set to remove
            if(std::find(componentIdSet.begin(), componentIdSet.end(), removeIter->m_componentIdSet[0]) != componentIdSet.end())
            {
                TRACE_MESSAGE3("\tRemoved optional file from download list with Component Identifier '%S': %S",
                    removeIter->m_componentIdSet[0].toWideChar(), removeIter->toString().toWideChar());
                m_matchFileList.erase(removeIter);
                continue;
            }
        }
        // the file matches for several component
        else if(!removeIter->m_componentIdSet.empty())
        {
            // removing all components in file component set
            for(ComponentIdentefiers::const_iterator iter = componentIdSet.begin(); iter != componentIdSet.end(); ++iter)
            {
                removeIter->m_componentIdSet.erase(std::remove_if(removeIter->m_componentIdSet.begin(), removeIter->m_componentIdSet.end(),
                    std::bind2nd(std::equal_to<NoCaseString>(), *iter)), removeIter->m_componentIdSet.end());
            }

            // no matched component rests, so removing file from list
            if(removeIter->m_componentIdSet.empty())
            {
                TRACE_MESSAGE2("\tRemoved optional file from download list, because all its Component Identifiers are optional: %S",
                    removeIter->toString().toWideChar());
                m_matchFileList.erase(removeIter);
                continue;
            }
        }

        ++i;
    }
}

bool KLUPD::UpdateInfo::isEntryRequired(NoCaseString &notMatchReason, const FileInfo &file, const UpdaterConfiguration::ListFlags &listFlags)
{
    // in retranslation mode inline files are not copied
     //  Note: inlined file must be parsed, and return is only here, because inline
     // signature 6 registry must be added to enable signature 6 check
    if(m_retranslationMode && !file.m_inlinedContent.empty())
    {
        notMatchReason = L"inlined files are not copied in retranslation mode";
        return false;
    }


    // mandatory files
    if(file.m_obligatoryEntry)
    {
        notMatchReason = L"mandatory element";
        switch(file.m_type)
        {
            case FileInfo::blackList:
                return true;
            case FileInfo::base:
                if(listFlags.update_bases)
                    return true;
                
                notMatchReason += NoCaseString(L", '") + FileInfo::toString(file.m_type) + L"' is not requested for update";
                return false;

            case FileInfo::patch:
                if(listFlags.automatically_apply_available_updates || listFlags.apply_urgent_updates)
                    return true;
                
                notMatchReason += NoCaseString(L", '") + FileInfo::toString(file.m_type) + L"' is not requested for update";
                return false;

            case FileInfo::index:
                if(listFlags.automatically_apply_available_updates || listFlags.apply_urgent_updates || listFlags.update_bases)
                   return true;
                
                notMatchReason += NoCaseString(L", '") + FileInfo::toString(file.m_type) + L"' is not requested for update";
                return false;

            default:
                // by default update needed
                return true;
        }
    }

    // optional files
    notMatchReason = L"optional element";
    switch(file.m_type)
    {
        case FileInfo::blackList:
            return true;
        case FileInfo::base:
            notMatchReason += NoCaseString(L", ") + FileInfo::toString(file.m_type) + L" is not requested for update";
            return false;

        case FileInfo::patch:
            if(listFlags.automatically_apply_available_updates)
                return true;
            
            notMatchReason +=  NoCaseString(L", '") + FileInfo::toString(file.m_type) + L"' is not requested for update";
            return false;

        case FileInfo::index:
            if(listFlags.automatically_apply_available_updates)
                return true;

            notMatchReason +=  NoCaseString(L", '") + FileInfo::toString(file.m_type) + L"' is not requested for update";
            return false;

        default:
            // by default update needed
            return true;
    }
}

bool KLUPD::UpdateInfo::isVersionValidForUpdate(const NoCaseString &product, const NoCaseString &update)
{
    // at first try to compare versions
    if(product == update)
        return true;

    int productVersion[4];  
    memset(productVersion,0, sizeof(productVersion));

    int updateVersion[4];  
    memset(updateVersion, 0, sizeof(updateVersion));

    // checked the most significant version digits
    #ifdef WSTRING_SUPPORTED
        int digitToCheck = swscanf(product.toWideChar(), L"%d.%d.%d.%d",
            &productVersion[0], &productVersion[1], &productVersion[2], &productVersion[3]);
        const int checkedDigits = swscanf(update.toWideChar(), L"%d.%d.%d.%d",
            &updateVersion[0], &updateVersion[1], &updateVersion[2], &updateVersion[3]);
    #else
        int digitToCheck = sscanf(product.toWideChar(), "%d.%d.%d.%d",
            &productVersion[0], &productVersion[1], &productVersion[2], &productVersion[3]);
        const int checkedDigits = sscanf(update.toWideChar(), "%d.%d.%d.%d",
            &updateVersion[0], &updateVersion[1], &updateVersion[2], &updateVersion[3]);
    #endif   // WSTRING_SUPPORTED

    if(digitToCheck < checkedDigits)
        digitToCheck = checkedDigits;

    
    // compare two passed versions digit by digit 
    bool _ret = true;
    for(size_t versionIndex = 0; versionIndex < static_cast<size_t>(digitToCheck); ++versionIndex)
    {
        if(productVersion[versionIndex] < updateVersion[versionIndex])
        {
            _ret = true;
            break;
        }
        else if(productVersion[versionIndex] > updateVersion[versionIndex])
        {
            _ret = false;
            break;
        }
    }
    
    return _ret;
}

bool KLUPD::UpdateInfo::matchesRetranslationList(NoCaseString &reasonNotMatch, const FileInfo &entry)
{
    if(!applicationMatchesForRetranslation(reasonNotMatch, entry, m_callbacks.updaterConfiguration().RetranslatedObjects.Applications_WhiteListMode,
        m_callbacks.updaterConfiguration().RetranslatedObjects.Applications))
    {
        return false;
    }

    if(!componentMatchesForRetranslation(reasonNotMatch, entry,
        m_callbacks.updaterConfiguration().RetranslatedObjects.Components_WhiteListMode, m_components))
    {
        return false;
    }

    return true;
}

bool KLUPD::UpdateInfo::applicationMatchesForRetranslation(NoCaseString &reasonNotMatch, const FileInfo &entry, const bool whiteListMode, const MapStringVStringWrapper &applicationList)
{
    // application ID is empty, application filter will be not applied
    if(entry.m_applicationFilter.matchesToAll())
        return true;


    // check if application presents in filter set
    bool elementPresents = false;
    std::vector<NoCaseString> versions;
    for(MapStringVStringWrapper::ConstIterator applicationIter = applicationList.m_data.begin(); applicationIter != applicationList.m_data.end(); ++applicationIter)
    {
        if(entry.m_applicationFilter.matches(applicationIter->first))
        {
            elementPresents = true;
            versions = applicationIter->second;
            break;
        }
    }

    if(!elementPresents)
    {
        if(whiteListMode)
        {
            reasonNotMatch += L"Not matches retranslation application filter (White List mode)";
            return false;
        }
        return true;
    }

    if(!whiteListMode)
    {
        // No other (version) checkings in Black List mode
        reasonNotMatch += L"Not matches retranslation application filter (Black List mode)";
        return false;
    }

    // the retranslation list does not specify versions, versions are not specified
    //  for this Application ID. White List mode, keep all versions, entry is not filtered out
    if(!versions.size())
        return true;

    // no applicaiton version specified for this entry, consider the entry as all-versions-matching, entry is not filtered out
    if(!entry.m_fromAppVersion.size())
        return true;

    bool entryMatches = false;
    for(size_t _ver = 0; _ver < entry.m_fromAppVersion.size(); ++_ver)
    {
        if(entryMatches)
            break;

        for(std::vector<NoCaseString>::const_iterator versionIterator = versions.begin(); versionIterator != versions.end(); ++versionIterator)
        {
            // Actually, checks "update_ver >= curr_ver"
            if(isVersionValidForUpdate(*versionIterator, entry.m_fromAppVersion[_ver]))
            {
                entryMatches = true;
                break;
            }
        }
    }

    if(!entryMatches)
    {
        reasonNotMatch += L"Not matches retranslation application version filter (While List mode)";
        return false;
    }

    return true;
}

bool KLUPD::UpdateInfo::componentMatchesForRetranslation(NoCaseString &reasonNotMatch, const FileInfo &entry,
        const bool whiteListMode, const ComponentIdentefiers &componentList)
{
    // component ID is empty, Component filter will be not applied
    if(entry.m_componentIdSet.empty())
        return true;

    const bool componentMatches = std::find_first_of(entry.m_componentIdSet.begin(), entry.m_componentIdSet.end(),
        componentList.begin(), componentList.end()) != entry.m_componentIdSet.end();

    if(!componentMatches)
    {
        if(whiteListMode)
        {
            reasonNotMatch += L"Matches retranslation list components failed: ComponentID not found in retranslation list, White List mode, entry is filtered out";
            return false;
        }
        return true;
    }

    if(!whiteListMode)
    {
        // no more filter checks in Black List mode
        reasonNotMatch += L"Matches retranslation list components failed: Black List mode, entry is filtered out";
        return false;
    }

    return true;
}

bool KLUPD::UpdateInfo::matchesSettings(NoCaseString &reasonNotMatch, const FileInfo &entry)
{
    if(m_retranslationMode)
    {
        if(m_callbacks.updaterConfiguration().retranslate_only_listed_apps)
        {
            // check if this entry matches the list from SS
            if(!matchesRetranslationList(reasonNotMatch, entry))
            {
                reasonNotMatch += L"does not match retranslation list";
                return false;
            }
        }
        return true;
    }
    
    // check language
    if(!entry.m_language.empty() && !m_callbacks.updaterConfiguration().product_lang.empty())
    {
        const std::vector<NoCaseString> productLanguage = m_callbacks.updaterConfiguration().product_lang;
        if(std::find_first_of(entry.m_language.begin(), entry.m_language.end(), productLanguage.begin(), productLanguage.end())
            == entry.m_language.end())
        {
            reasonNotMatch += L"filtered by language";
            return false;
        }
    } 
    
    // check whether Application ID matches current one 
    if(!entry.m_applicationFilter.matchesToAll())
    {
        if(!entry.m_applicationFilter.matches(m_callbacks.updaterConfiguration().pID))
        {
            reasonNotMatch += NoCaseString(L"Build number filter does not match (required: ")
                + m_callbacks.updaterConfiguration().pID + L")";
            return false;
        }
    }

    // check whether Component Identifier matches
    if(!entry.m_componentIdSet.empty())
    {
        // component identifier presents, searching for a component
        const bool componentMatches = std::find_first_of(entry.m_componentIdSet.begin(), entry.m_componentIdSet.end(),
            m_components.begin(), m_components.end()) != entry.m_componentIdSet.end();

        if(!componentMatches)
        {
            reasonNotMatch += L"component does not match";
            return false;
        }
    }

    // application versions check
    if(entry.m_fromAppVersion.size())
    {
        // given list of versions, we can update from
        bool versionMatches = false;

        for(std::vector<NoCaseString>::const_iterator correctVersionIter = entry.m_fromAppVersion.begin(); correctVersionIter != entry.m_fromAppVersion.end(); ++correctVersionIter)
        {
            if(isVersionValidForUpdate(m_callbacks.updaterConfiguration().pVer, *correctVersionIter))
            {
                versionMatches = true;
                break;
            }
        }
        if(!versionMatches)
        {
            reasonNotMatch += L"product version failed";
            return false;
        }
    }

    if(!doesOsMatch(entry.m_OSes))
    {
        reasonNotMatch += L"platform check failed";
        return false;
    }

    reasonNotMatch += L"matches";
    return true;
}

bool KLUPD::UpdateInfo::doesOsMatch(const std::vector<FileInfo::OS> &update)const
{
    // no OS version is specified, thus matches
    if(update.empty()) 
        return true;

    // OS version is specified, check if valid
    for(std::vector<FileInfo::OS>::const_iterator os = update.begin(); os != update.end(); ++os)
    {
        if(os->m_name == m_callbacks.updaterConfiguration().os.os)
        {
            // OS name matches
            if(os->m_version.empty())
                return true;

            if(m_callbacks.updaterConfiguration().os.version == os->m_version)
                return true;
        }
    }

    return false;
}
