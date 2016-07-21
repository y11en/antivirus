#include "IniBasedRollback.h"

KLUPD::IniBasedRollbackFile::IniBasedRollbackFile(IniFile &rollback, Log *log)
 : m_rollback(rollback),
   pLog(log)
{
}

void KLUPD::IniBasedRollbackFile::clear()
{
    m_rollback.clear();
}

bool KLUPD::IniBasedRollbackFile::readRollback(FileVector &files)
{
    if(m_rollback.fileName().empty())
    {
        TRACE_MESSAGE("Failed to read rollback information, because file name is not specified");
        return false;
    }

    for(size_t fileIndex = 0;; ++fileIndex)
    {
        std::ostringstream keyNameStream;
        keyNameStream.imbue(std::locale::classic());
        keyNameStream << "entry" << fileIndex;

        const NoCaseString childSectionName = m_rollback.getString(KLUPD::asciiToWideChar(keyNameStream.str()), SS_KEY_RollbackTree);
        if(childSectionName.empty())
            break;
        
        FileInfo rec;
        const NoCaseString fullSectionName = NoCaseString(SS_KEY_RollbackTree) + L":" + childSectionName;
        if(!readSingleFileInfoRollback(fullSectionName, rec))
        {
            TRACE_MESSAGE2("Invalid content of '%S' section, skipping", fullSectionName.toWideChar());
            // ignore error, try next entry if any
            continue;
        }
        
        files.push_back(rec);
    }
    
    return true;
}

bool KLUPD::IniBasedRollbackFile::saveRollback(const FileVector &files,
                                               const KLUPD::Path &rollbackFolder,
                                               const KLUPD::Path &retranslationFolder,
                                               const bool retranslationMode, 
                                               const NoCaseString &rollbackComponentsFilterString)
{
    TRACE_MESSAGE3("Saving rollback to '%S', rollback folder '%S'",
        m_rollback.fileName().toWideChar(), rollbackFolder.toWideChar());

    if(m_rollback.fileName().empty())
    {
        TRACE_MESSAGE("Failed to create rollback, because file name is not specified");
        return false;
    }


    const ComponentIdentefiers rollbackComponentsFilter
        = StringParser::splitString(rollbackComponentsFilterString, COMPONENT_DELIMITER);

    // set correct status for all entries that have changed
    NoCaseString rollbackTrace;
    FileVector updateList;
    for(size_t changedFileIndex = 0; changedFileIndex < files.size(); ++changedFileIndex)
    {
        const FileInfo::ChangeStatus changeStatus = files[changedFileIndex].m_transactionInformation.m_changeStatus;
        // skip unchanged elements
        if(changeStatus == FileInfo::unchanged)
            continue;

        if(!matchForRollback(rollbackComponentsFilter, files[changedFileIndex].m_componentIdSet))
            continue;
        
        // entry copy, because size, signature, and hash tags are not valid here.
        //  There are values not for rollback, but values for new files
		FileInfo entryCopy;
        entryCopy.m_filename = files[changedFileIndex].m_filename;
        entryCopy.m_relativeURLPath = files[changedFileIndex].m_relativeURLPath;
        entryCopy.m_localPath = retranslationMode
            ? retranslationFolder + files[changedFileIndex].m_relativeURLPath
            : files[changedFileIndex].m_localPath;
        entryCopy.m_transactionInformation.m_changeStatus = changeStatus;
        entryCopy.m_componentIdSet = files[changedFileIndex].m_componentIdSet;
        entryCopy.m_transactionInformation.m_replaceMode = files[changedFileIndex].m_transactionInformation.m_replaceMode;

        if((changeStatus != FileInfo::added)
            // calculate MD5 for consistency check to restore changed and deleted files
            && !calcMD5Hash(rollbackFolder + entryCopy.m_relativeURLPath + entryCopy.m_filename, entryCopy.m_md5))
        {
            TRACE_MESSAGE2("Rollback is not created, failed to calculate md5 for '%S'",
                (rollbackFolder + entryCopy.m_relativeURLPath + entryCopy.m_filename).toWideChar());
            return false;
        }
		updateList.push_back(entryCopy);

        if(!rollbackTrace.empty())
            rollbackTrace += L", ";
        if(!(changedFileIndex % 15))
            rollbackTrace += L"\n\t";
        rollbackTrace += NoCaseString(entryCopy.m_filename.toWideChar())
            + L" " + FileInfo::toString(changeStatus);
    }

    TRACE_MESSAGE2("Saving rollback files: %S", rollbackTrace.toWideChar());
    
    // saving rollback information
    if(!saveRollbackInfo(updateList))
    {
        TRACE_MESSAGE("Failed to save rollback file list");
        return false;
    }
    
    TRACE_MESSAGE("Rollback file list saved successfully");
    return true;
}

void KLUPD::IniBasedRollbackFile::makeNewSectionName(const long index, const NoCaseString &name, NoCaseString &outputName)
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << index << "(" << name.toAscii() << ")";
    outputName.fromAscii(stream.str());
}

bool KLUPD::IniBasedRollbackFile::readSingleFileInfoRollback(const NoCaseString &sectionName, FileInfo &file)
{
    if(sectionName.empty())
    {
        TRACE_MESSAGE("Read rollback file information error: configuration section is not specified");
        return false;
    }
    
    // file name
    file.m_filename = m_rollback.getString(SS_KEY_RecentFileName, sectionName);
    if(file.m_filename.empty())
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%S' value found", SS_KEY_RecentFileName);
        return false;
    }

    // relative url path
    file.m_relativeURLPath = m_rollback.getString(SS_KEY_RecentRelativeSrvPath, sectionName);
    if(file.m_relativeURLPath.empty())
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%S' value found", SS_KEY_RecentRelativeSrvPath);
        return false;
    }

    // local path
    file.m_localPath = m_rollback.getString(SS_KEY_RecentLocalPath, sectionName);
    if(file.m_localPath.empty())
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%S' value found", SS_KEY_RecentLocalPath);
        return false;
    }

    // change status
    const NoCaseString changeStatus = m_rollback.getString(SS_KEY_ChangeStatus, sectionName);
    if(changeStatus.empty())
    {
        TRACE_MESSAGE2("Read rollback file information error: no '%S' value found", SS_KEY_ChangeStatus);
        return false;
    }
    file.m_transactionInformation.m_changeStatus = FileInfo::stringToChangeStatus(changeStatus);

    // component identifiers
    const NoCaseString componentIdSet = m_rollback.getString(SS_KEY_ComponentsSection, sectionName);
    file.m_componentIdSet = KLUPD::fromString(componentIdSet);

    // md5 for consistency check
    const NoCaseString md5 = m_rollback.getString(kayw_MD5, sectionName);
    // do not read rollback md5 for added files, because file needs to be deleted, and no md5 is needed to be known
    if(!md5.empty())
        file.m_md5 = CBase64::decodeBuffer(md5.toAscii().c_str());

    // file replace method
    file.m_transactionInformation.m_replaceMode
        = FileInfo::TransactionInformation::fromString(m_rollback.getString(keyw_ReplaceMode, sectionName));

    TRACE_MESSAGE2("Rollback information read: '%S'", file.toString().toWideChar());
    return true;
}

bool KLUPD::IniBasedRollbackFile::saveRollbackListEntry(const NoCaseString &parentSectionName, const long orderNumber, const NoCaseString &newSectionName, const FileInfo &file)
{
    std::ostringstream entryKeyNameStream;
    entryKeyNameStream.imbue(std::locale::classic());
    entryKeyNameStream << "entry" << orderNumber;

    //////////////////////////////////////////////////////////////////////////
    /// write file record to enumeration section "entry0=0(master.xml)"
    m_rollback.setValue(KLUPD::asciiToWideChar(entryKeyNameStream.str()), newSectionName, parentSectionName);

    //////////////////////////////////////////////////////////////////////////
    /// create new section with file description
    const NoCaseString sectionName = parentSectionName + L":" + newSectionName;

    m_rollback.createSection(sectionName);
    IniFile::Section *section = m_rollback.getSection(sectionName);
    if(!section)
    {
        TRACE_MESSAGE4("Unable to write rollback information for file '%S', failed to create section '%s' in file '%S'",
            file.m_filename.toWideChar(), sectionName.toWideChar(), m_rollback.fileName().toWideChar());
    }

    section->m_keys.push_back(IniFile::Key(SS_KEY_RecentFileName, file.m_filename.toWideChar()));
    section->m_keys.push_back(IniFile::Key(SS_KEY_RecentRelativeSrvPath, file.m_relativeURLPath.toWideChar()));
    section->m_keys.push_back(IniFile::Key(SS_KEY_RecentLocalPath, file.m_localPath.toWideChar()));
    section->m_keys.push_back(IniFile::Key(SS_KEY_ChangeStatus, FileInfo::toString(file.m_transactionInformation.m_changeStatus).toWideChar()));
    section->m_keys.push_back(IniFile::Key(SS_KEY_ComponentsSection, toString(file.m_componentIdSet).toWideChar()));

    // for newly added files md5 about previous version does not present
    if(!file.m_md5.empty())
    {
        std::vector<char> md5 = CBase64::encodeBuffer(&file.m_md5[0], file.m_md5.size());
        md5.push_back(0);
        section->m_keys.push_back(IniFile::Key(kayw_MD5, asciiToWideChar(&md5[0]).c_str()));
    }

    section->m_keys.push_back(IniFile::Key(keyw_ReplaceMode,
        FileInfo::TransactionInformation::toString(file.m_transactionInformation.m_replaceMode).toWideChar()));
    return true;
}

bool KLUPD::IniBasedRollbackFile::saveRollbackInfo(const FileVector &files)
{
    m_rollback.clear();

    // for output saved entries in rows and save space in trace file
    NoCaseString savedEntries;
    unsigned long logLineSplitIndex = 0;

    long k = 0;
    for(FileVector::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        NoCaseString newSectionName;
        const long order_number = k++;
        makeNewSectionName(order_number, iter->m_filename.toWideChar(), newSectionName);
        
        if(!saveRollbackListEntry(SS_KEY_RollbackTree, order_number, newSectionName, *iter))
        {
            TRACE_MESSAGE3("Unable to save rollback information for file entry '%S', relative URL path '%S'",
                (iter->m_localPath + iter->m_filename).toWideChar(), iter->m_relativeURLPath.toWideChar());
            return false;
        }

        // append comma before all items, but the very first one
        if(logLineSplitIndex++)
            savedEntries += L", ";
        // split line each 3 elements, but not before last element
        if(iter + 1 != files.end())
        {
            if(!(logLineSplitIndex % 3))
                savedEntries += L"\n\t";
        }
        savedEntries += (iter->m_localPath + iter->m_filename + L"(" + iter->m_relativeURLPath + L")").toWideChar();
    }

    TRACE_MESSAGE3("Rollback information saved successfully to %S, entries: %S",
        m_rollback.fileName().toWideChar(), savedEntries.toWideChar());
    return true;
}

bool KLUPD::IniBasedRollbackFile::matchForRollback(const ComponentIdentefiers &filter, const ComponentIdentefiers &componentToCheck)
{
    // filter is not set, any component matches
    if(filter.empty() || componentToCheck.empty())
        return true;

    return std::find_first_of(filter.begin(), filter.end(), componentToCheck.begin(), componentToCheck.end()) == filter.end();
}

