#ifndef FILEINFO_INCLUDED_C53D0909_BBE7_44d2_B0D4_D78DFA32ECD8
#define FILEINFO_INCLUDED_C53D0909_BBE7_44d2_B0D4_D78DFA32ECD8


#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000


#include "../include/cfg_updater.h"
#include "path.h"

namespace KLUPD {

// Structure to collect file info from kavset.xml file
struct KAVUPDCORE_API FileInfo
{
    // file status
    enum ChangeStatus
    {
        // file is not going to be downloaded from source
        unchanged,
        // newer file is found on update source. Local file will be replaced
        modified,
        // new update set contains file which does not present in current set. File will be added to local files set
        added,
    };
    static NoCaseString toString(const ChangeStatus &);
    static ChangeStatus stringToChangeStatus(const NoCaseString &);

    // update item type
    enum Type
    {
        // data file for product, Anti-Virus base
        base,
        // update module, usually executable
        patch,
        // index file. Used by updater to get a set of files to update
        index,
        // list of revoked licences
        blackList,
        // signature 6 registry
        registry,
        // signature 6 public key
        key
    };
    // humanReadable [in] - outputs in human readable format, otherwise for in KCA settings storage format
    static NoCaseString toString(const Type &, const bool humanReadable = true);
    static Type fromStringInIndexFormat(const NoCaseString &);

    // check string if item is obligatory
    static bool fromStringObligatoryInIndexFormat(const NoCaseString &);


    // OS description
    struct OS
    {
        // output OS in human-readable way
        NoCaseString toString()const;

        // operating system name
        NoCaseString m_name;
        // operating system version
        NoCaseString m_version;
    };


    // the ApplicationFilter class represents filter by 'Build' or 'ApplicationID' XML tag
    struct KAVUPDCORE_API ApplicationFilter
    {
        typedef size_t BuildNumber;

        struct KAVUPDCORE_API Range
        {
            Range(const BuildNumber &min = 0, const BuildNumber &max = 0);

            BuildNumber minBuild()const;
            BuildNumber maxBuild()const;

            // checks whether range overlap exists
            bool intersects(const Range &)const;

            // convert to human-readable string
            NoCaseString toString(const bool humanReadableFormat)const;
            // initialization from string
            bool fromString(const NoCaseString &);

        private:
            BuildNumber m_min;
            BuildNumber m_max;
        };

        ApplicationFilter();

        std::vector<Range> numericRanges()const;
        std::vector<NoCaseString> stringBuilds()const;

        // obsolete applicaiton tag is used (in case update from masterv2.xml
          // for Administration Kit instead 'Build' the obsolete 'ApplicationID' tag may be used
         // The m_obsoleteApplicationTagUsed member may be removed on Administration Kit switch to u0607g.xml index.
        bool obsoleteApplicationTagUsed()const;
        void obsoleteApplicationTagUsed(const bool);

        // check if matches filter
        bool matches(const ApplicationFilter &)const;
        bool matches(const NoCaseString &)const;

        // return true in case filter matches for all items
        bool matchesToAll()const;

        // parse filter string and add to internal representation
         // Format:
         //  a set of builds are delimiter with semicolon ';' symbol
         //  a range is specified with ',' symbol
        void parseAndAddRange(const NoCaseString &rangeToParseString);

        // represent application filter in human-readable format
        NoCaseString toString()const;

    private:
        // application can be string or number
        std::vector<Range> m_numericRanges;
        std::vector<NoCaseString> m_stringBuilds;

        bool m_obsoleteApplicationTagUsed;
    };

    // data for file transaction operations
    struct KAVUPDCORE_API TransactionInformation
    {
        // specifies update algorithm
        enum ReplaceMode
        {
            // file placed on new location in ordinary way,
             // locked file replaced by rename original file
            ordinary,
            // file placed to original destination with suffix,
             // locked files are unlocked by product
            suffix,
        };
        static ReplaceMode fromString(const NoCaseString &);
        static NoCaseString toString(const ReplaceMode &);

        TransactionInformation(const ChangeStatus & = unchanged, const ReplaceMode & = ordinary,
            const Path &currentLocation = Path(), const Path &newLocation = Path());

        NoCaseString toString()const;

        // identifies file change status
        ChangeStatus m_changeStatus;
        // update protocol
        ReplaceMode m_replaceMode;

        // current file location
        Path m_currentLocation;
        // location of a new version of file
        Path m_newLocation;
    };

    explicit FileInfo(const Path &fileName = Path(),
        const Path &relativeURLPath = Path(L"/"),
        const Type & = base,
        const bool primaryIndex = false);

    // converts structure to readable form
    NoCaseString toString(const bool withTransactionInformation = false)const;

    // comparison
    bool isTheSame(const FileInfo &, const bool retranslationMode)const;

    bool isIndex()const;
    bool isInlined()const;
    bool isBlackList()const;
    // asserts file status, and return true in case file download from source needed
    bool downloadNeeded()const;
    // return true in case Klz file compression mechanism can be used for file download
    const bool canBeDownloadedWithKlz()const;


    // application identifier specifies application that match update criteria
    ApplicationFilter m_applicationFilter;
    // component identifier set specifies components that match
    ComponentIdentefiers m_componentIdSet;
    // file type
    Type m_type;
    // name of file being updated (not including path)
    Path m_filename;
    // relative path to file on update source
    Path m_relativeURLPath;
    // path to file on local system.
    //  Note: the path is valid only in update mode. To get retranslation mode path use productFolder(file, false)
    Path m_localPath;
    // original local path, before environment variables preporcession was performed
    Path m_originalLocalPath;


    // product application versions from which update is suitable
    //
    // Format: the product version is represented by 4 optional
    //  digits string [d1[.d2[.d3[.d4]]]]
    // Examples: "5.0.1.2", "5.0.1"
    // Filter: the greater version of patch matches to the lower version
    //   of application. Major number is most significant and comparison
    //  is performed until next minor version digit is different.
    // Note: comparison is performed by comparison each digit. In case product's
    //   version is represented with less digit number, then comparison is performed
    //  only with leading product version digits and trailing digits are not involved
    //  into comparison.
    //
    // E.g.: patch with version "5.1" matches to Application
    //   with version "5.0" and for "5.0.1" and for "5.1"
    std::vector<NoCaseString> m_fromAppVersion;


    // product versions to which update is being done
    NoCaseString m_toAppVersion;
    // indicates if entry is mandatory for update
    bool m_obligatoryEntry;
    // Kaspersky Laboratory signature (5th format)
    NoCaseString m_signature5;
    // md5 file hash is introduced in addition to signature, because for some files
    //  the content must not be changed, and signature is appended to the end of file
    std::vector<unsigned char> m_md5;
    // list of operating system this file matches
    std::vector<OS> m_OSes;
    // language of a product being processed
    std::vector<NoCaseString> m_language;

// TODO: dummy convention, that m_size member undefined value is -1
    // expected file size
    size_t m_size;
    // date of file expected to be on update source (may differ to real file size)
    NoCaseString m_dateFromIndex;
    NoCaseString m_strUpdateDate;   // actual file date (used only for indexes)
    NoCaseString m_strBlackDate;    // black list date actual on index issue (used only for indexes)
    NoCaseString m_comment;

    // localization set is used by AdminKit to pick locale
    NoCaseString m_localizationSet6;
    // localization set is used by AdminKit to pick locale
    NoCaseString m_localizationSetAK;
    // fix name for patch to be passed to the product
    NoCaseString m_fixName;

    // lug for black list to solve problem with file for Kaspersky Administration Team
      // black list may be out-dated but solve period gives ability to solve period
     // and specifies period to solve problem
    size_t m_solvePeriodDays;

    // the content of inlined file read from 'Body' XML tag
    std::vector<unsigned char> m_inlinedContent;
    

    // indicates if file is primary index
    bool m_primaryIndex;


    TransactionInformation m_transactionInformation;

private:
    // disable standard comparison operation, because it depends on retranslation mode
     // use FileVector::isTheSamePredicate structure instead
    bool operator==(const FileInfo &)const;
};

struct FileVector : public std::vector<FileInfo>
{
    // class to enable std::find() algorithm with FileVector
    // Usage:
     //  FileVector vector = ...;
     //  FileInfo file = ...;
     //  const bool retranslationMode = ...;
     //  FileVector::iterator removeIter = std::find_if(vector.begin(), vector.end(),
     //          KLUPD::FileVector::IsTheSamePredicate(file, retranslationMode));
    struct IsTheSamePredicate
    {
        IsTheSamePredicate(const FileInfo &, const bool retranslationMode);
        bool operator()(const FileInfo &)const;

        const FileInfo m_etalon;
        const bool m_retranslationMode;
    };

    typedef std::map<NoCaseString, FileVector> FilesByComponent;


    // returns true in case the same file presents in the set
    bool theSameFilePresents(const FileInfo &, const bool retranslationMode)const;
    bool findTheSameFile(const FileInfo &etalon, FileInfo &result, const bool retranslationMode)const;
    void insertNewOrUpdateTheSame(const FileInfo &sameElement, const FileInfo &newValue, bool &fileDuplicate, const bool retranslationMode);
    // insert new on top of vector, or replace existent file
    void insertNewInTheBeginOfListOrUpdateTheSame(const FileInfo &sameElement, const FileInfo &newValue, bool &fileDuplicate, const bool retranslationMode);

    // return true in case at least one of file's status is not 'unchanged'
    bool anyFileChanged()const;
    // split file vector by components
    void splitByComponents(FilesByComponent &, const bool filterEmptyTransactions)const;
};


}   // namespace KLUPD

#endif  // #ifndef FILEINFO_INCLUDED_C53D0909_BBE7_44d2_B0D4_D78DFA32ECD8
