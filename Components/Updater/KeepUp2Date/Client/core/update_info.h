#if !defined(UPDATE_INFO_H_INCLUDED_50E995DD_3FF2_412e_9B35_897CCB58BA57)
#define UPDATE_INFO_H_INCLUDED_50E995DD_3FF2_412e_9B35_897CCB58BA57

#include "../include/cfg_updater.h"

namespace KLUPD {

/// the UpdateInfo class deals with update description file
class KAVUPDCORE_API UpdateInfo
{
public:
    enum ChangeStateCheck
    {
        // it is granted then in case enumeration local files we always get 'unchanged'
         // file vs itself. Use this for optimization in local files enumeration mode
        noStateChangeCheck,
        // file change state is calculated by comparing downloaded file agains to local one
        checkStateAgainstLocalFile,
        // check file state, but ignores date check
        checkStateButIgnoreDate,
    };


    UpdateInfo(const UpdaterConfiguration::ListFlags &,
        const ComponentIdentefiers &filterComponents,
        Signature6Checker &, const bool retranslationMode,
        CallbackInterface &, Log *);


    // Ask product if file is optional for download.
    //  In case file is optional update can continue, otherwise update must stop
    // absentFile [in] - absent file to check if optional and can be removed from update
    bool checkIfFileOptional(const FileInfo absentFile);

    // componentsToRemove - copy because when file is deleted iterator becomes invalid)
    void removeComponentFromUpdate(const ComponentIdentefiers componentsToRemove, JournalInterface &);

    // filter all files from match set with specified components match criteria
     // Note: the filter value is passed by value, because it may become invalid in remove operation
    void filterByComponents(const ComponentIdentefiers componentIdSet);


    // parses specified index file and appends links from index file to Update information structure
    // suppressSuccessLogMessages [in] - suppress successful trace message to make trace less size
    // return true has been parsed successfully
    KLUPD::CoreError parse(const Path &path, FileInfo &, const ChangeStateCheck &,
        const FileVector &localFilesToCheckAgainst, const bool suppressSuccessLogMessages);


    // allows to check if file is already parsed
    bool fileAlreadyParsed(const FileInfo &)const;
    

    /// list of file which matches download criteria
    FileVector m_matchFileList;

    /// set of values to write into KCA product settings storage obtained while parsing index XML files from 'UpdateSettings' attributes
    std::vector<KLUPD::UpdatedSettings> m_updatedSettings;  

    Signature6Checker &m_signature6Checker;

    // component set which present locally, but was removed from update, because damaged or absent on source
    //  the files of these components should not be removed
    ComponentIdentefiers m_componentsRemovedFromUpdate;

    const bool m_retranslationMode;

private:
    // disable copy operations
    UpdateInfo &operator=(const UpdateInfo &);
    UpdateInfo(const UpdateInfo &);

    // return file status against local file
    //  statusExplanations [in] - details to output to trace with explanation of status
    FileInfo::ChangeStatus getFileStatusAgainstLocal(const FileInfo &etalon, const ChangeStateCheck &,
        const FileVector &localFilesToCheckAgainst, NoCaseString &statusExplanations)const;


    // check whether entry should be filtered or not
    bool isEntryRequired(NoCaseString &notMatchReason, const FileInfo &, const KLUPD::UpdaterConfiguration::ListFlags &);

    // Compare versions of product and update description
    // product [in] Product version
    // update [in] Update version
    // return true if record is valid for update
    bool isVersionValidForUpdate(const NoCaseString &product, const NoCaseString &update);

	// Check whether entry belongs to retranslation list
	// const FileInfo & [in] - an entry from update description file
	// return true if matches
	bool matchesRetranslationList(NoCaseString &reasonNotMatch, const FileInfo &);
    bool applicationMatchesForRetranslation(NoCaseString &reasonNotMatch, const FileInfo &,
        const bool whiteListMode, const MapStringVStringWrapper &applicationList);
	bool componentMatchesForRetranslation(NoCaseString &reasonNotMatch, const FileInfo &,
        const bool whiteListMode, const ComponentIdentefiers &componentList);

	// Check whether entry fits current settings
	// const FileInfo & [in] -f an entry from update description file
	// return true if matches
    bool matchesSettings(NoCaseString &reasonNotMatch, const FileInfo &);

    // Check whether update description file record matches host OS
    // return true when record is valid for update
    bool doesOsMatch(const std::vector<FileInfo::OS> &update)const;


    // save already attached index to avoid re-parse it
    FileVector m_parsedIndexCache;

    const UpdaterConfiguration::ListFlags &m_filterFlags;
    // component list for filtering. Copy, because component set can be modified while filtering
    ComponentIdentefiers m_components;
    CallbackInterface &m_callbacks;
    Log *pLog;
};

}   // namespace KLUPD

#endif  // UPDATE_INFO_H_INCLUDED_50E995DD_3FF2_412e_9B35_897CCB58BA57
