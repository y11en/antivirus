#if !defined(UPDATER_H_INCLUDED_1493FB54_323C_4fd4_9F0F_1622013213F1)
#define UPDATER_H_INCLUDED_1493FB54_323C_4fd4_9F0F_1622013213F1

#include "../helper/updaterStaff.h"

#include "update_info.h"
#include "HttpProtocolImplementation.h"
#include "NetFacAK.h"
#include "../net/netTransfer.h"

// including proxyDetector functionality. This header should be included only on Windows platform
#define PROXYDETECTOR_EXPORTS
#include "../../proxyDetector/proxyDetector.h"

namespace KLUPD {

class KAVUPDCORE_API Updater : public Socket::Initializer
{
public:
    explicit Updater(DownloadProgress &, CallbackInterface &, JournalInterface &, Log *);

    // build a list of local files and check consistency and files signature
    // FileVector & [out] - a local files list
    // bVerify flag is set when database consistency verification must be performed
    // return true in case local files are consistent
    bool EnumerateLocalFiles(FileVector& vecFiles, bool bRetr, bool bVerify = true);
    // earns update date from file list given
    // returns string containing update date or empty stirng on error
    NoCaseString GetUpdateDate(const FileVector& vecFiles);

	// perform update and retranslation operation, depends on configuration
    // return code related to Update operation.
    //  Note: to get code related to retranslation use the getRetranslationOperationResult() function
	CoreError doUpdate();
    CoreError getRetranslationOperationResult()const;
    
	// perform rollback operation
	CoreError doRollback(const bool retranslationMode = false);

	JournalInterface &m_journal;
    DownloadProgress &m_downloadProgress;
	CallbackInterface &m_callback;
	Log *pLog;

private:

    // enumerates local files
    // vecFiles [out] is a list to store files descriptions (method cleans it before processing)
    // bRetr flag is set in case of retranslation process
    // returns true if enumeration success, false otherwise
    bool _EnumerateLocalFiles(UpdateInfo &info, bool bRetr = false);

    // verifies files in list given
    // vecFiles [in] is a files list
    // bRetr flag is set in case of retranslation process
    // returns true if verification success, false otherwise
    bool _VerifyFiles(UpdateInfo &info, bool bRetr = false);
private:
    friend class UpdateInfo;

    // disable copy operations
    Updater &operator=(const Updater &);
    Updater(const Updater &);


	// download remote file with FTP (no-proxy) or File Transfer protocol
	CoreError DoGetRemoteFile_SelfNet(
        const Path &fileName,
        const Path &localPath,
        const Path &relativeUrlPath,
        const Path &url);

	CoreError downloadFileWithRegetting(const FileInfo &, const Path &downloadFolder, Signature6Checker &);

	// download bases
	CoreError downloadBases(UpdateInfo &);


    // construct source list:
    //  1. from sorted sites.xml file
    //  2. then sources from sorted updcfg.xml file are appended
    //  3. finally randomly chosen hard-coded site is appended
    // sites are sorted according to it Region (most considerable). In case
    //  equal regions weight and random mixing is used for sort sources
	bool loadSitesList(std::vector<Path> &sitesList);

	CoreError downloadEntry(const FileInfo &, const Path &targetFolder,
        // difference files are not obligatory for Update, because plain file will be downloaded,
         //  difference files may have different extensions and be not allowed on proxies.
         // It is not needed to tread 403 and 502 codes as 407 code in this case
        const bool fileWithVariableExtensionNotObligatoryForUpdate = false);
	
    // calculate size of update and retranslation files which need to be downloaded intersection
    void calculatingDownloadSize(const UpdateInfo &update, const UpdateInfo &retranslation);
    

	// get primary index file information
	FileInfo &getPrimaryIndexFileInfo(const bool checkIfChangedAgainstFileInTemporaryFolder, const bool retranslationMode);

    // transform Kaspersky Laboratory source to set of sources obtained from configuration files
    // input [in] - source list which needs to be expanded with Kaspersky Laboratory sources
    // result [out] - result source list which has been expanded
	bool constructSourceList(SourceList &input, SourceList &result);

    // targetFileIsValid [out] - specifies if file needs being downloaded
    // fileToCheck [in] - file to check with full path
    // etalon [in] - etalon file against which is checked whether file can be downloaded with regetting
    // return true if file can be downloaded with regetting feature. Regetting may fail in case
    //  file has the same size, but different signature. Then HTTP transport will fail on such occasion.
    bool canBeDownloadedWithRegetting(bool &targetFileIsValid, const Path &fileToCheck, const FileInfo &etalon)const;

    // compare file dates
    bool downloadedIndexNewerThenLocal(const FileInfo &, const Path &downloadedPath, const Path &localPath, Signature6Checker &, const bool retranslationMode)const;
    // parses primary index
    CoreError parsePrimaryIndex(UpdateInfo &);

    // enumerates secondary index files located locally
    CoreError enumerateLocalSecondaryIndexes(UpdateInfo &);
    // recursively downloads secondary indexes
    CoreError downloadSecondaryIndexes(UpdateInfo &);
    // check each index file in update set agaings Black Limit Date
    bool checkBlackDateLimit(const FileVector &, const unsigned long blackListLimit, const bool retranslationMode);


    // check if file has already been downloaded in current session, otherwise downloaded it
    CoreError downloadFileUsingCache(const FileInfo &fileEntry, UpdateInfo &);
    // download file using different compression mechanisms
    CoreError downloadFileWithAllCompressionMechanisms(const FileInfo &fileEntry, UpdateInfo &);

    CoreError updateWithDifferenceFileForIndex(const Path &productFullFileName, const FileInfo &, const Path &targetTemporaryFolder, Signature6Checker &);
	CoreError updateWithDifferenceFile(const Path &productFullFileName, const FileInfo &, const Path &targetTemporaryFolder, Signature6Checker &);

    // generate difference file extension based on file context
	bool makeDifferenceFileExtension(const Path &sourceFileFullPath, Path &differenceFileExtension)const;

	CoreError updateFileWithKlzCompression(const FileInfo &, const Path &targetFolder, Signature6Checker &);

	void makeKlzCompressionFileName(const Path &relativeURLPath,    // server path (relative to base url)
		const Path &baseFileName, Path &KLZPath, Path &KLZFile)const;


    void outputUpdaterVersionToTrace()const;

    // report source selected event
    void reportSourceSelected();
    // reports FATAL folder access problems to journal, logs and makes callback to product about 
    void reportCanNotCreateFolder(const Path &folder, const std::string &additionalInfo)const;
    // report network error, adjust parameter if needed
    void reportNetworkEvent(const KLUPD::CoreError &code, const NoCaseString &parameter)const;


    // replace files
    CoreError replaceFiles(UpdateInfo &);

    // send file set obtained while update operation to application
    void sendLocalFilesToProduct(const UpdateInfo &);

    // prepare Updater and check input configuration
    CoreError initialize();

    // download primary index
    bool downloadPrimaryIndex(UpdateInfo &retranslationInfo, UpdateInfo &updateInfo);

    // implements download from one source
    void downloadFromSource(bool &primaryIndexDownloadedAndParsed);
    // this function does update, it implements main update and retranslation loop which tries all sources
    void downloadFromAllSources();
    // enumerate files in rollback folder and checks against files passed by product via readRollbackFiles() callback
    CoreError checkFilesInRollbackFolder(const FileVector &fileList, FileVector &filesToAffect, const bool retranslationMode);

    // rollback implementation function, introduced for convenience
    CoreError doRollbackImplementation(const bool retranslationMode);


    // to reduce download number search for local file
    bool tryFindLocalFileAndCopyToTemporaryFolder(const FileInfo &, Signature6Checker &, const bool retranslationMode)const;


    const FileVector localConsistentFiles(const bool retranslationMode)const;


    // flag indicates whether local files before update operation are consistent
    bool m_localFilesEnumerated;
    bool m_retranslationLocalBasesAreConsistent;
    bool m_updateLocalBasesAreConsistent;
    // cache of parsed local files
    FileVector m_retranslationLocalFiles;
    FileVector m_updateLocalFiles;

    // information of all downloaded files in current session with results from current update source,
     // to avoid download files twice during single update session thus save traffic
    // Warning: do not use outside dynamic library scope
    std::map<Path, CoreError> m_downloadedFilesInCurrentSession;

    // local signature checks
    Signature6Checker m_retranslationLocalSignature6Checker;
    Signature6Checker m_updateLocalSignature6Checker;

    // HTTP protocol implementation and session state
    HttpProtocolFileImplementation m_httpProtocol;
    // FTP protocol implementation and current state
    FtpProtocol m_ftpProtocol;
    // Administration Kit protocol implementation and session state
    AdministrationKitProtocol m_administrationKitProtocol;
    // file protocol implementation and current state
    FileTransferProtocol m_fileProtocol;

    // source list constructed from product source list. This parameter is needed, because e.g. Kaspersky
    //  Laboratory source stands for several source URLs, thus product parameter is translated
    SourceList m_sourceList;

    // primary index file information
	FileInfo m_retranslationPrimaryIndexInformation;
	FileInfo m_updatePrimaryIndexInformation;

    // helper variable for all sources loop (to avoid passing local variables as function parameters)
    CoreError m_retranslationResult;
    CoreError m_updateResult;
    bool m_retranslationDone;
    bool m_updateDone;


    // Search duplicates for retranslation by update criteria:
      //  duplicate criteria: (fileName == fileName) && (localPath == localPath) && (relativeURL != relativeURL)
     // Note: duplicates for update by retranslation criteria does not exist
     // Force not using duplicates (incorrect files) in search for retranslation files
    void calculateDuplicatesForRetranslation(const FileVector &retranslationFiles);
    void cleanDuplicatesForRetranslation();
    bool checkIfDuplicate(const FileInfo &)const;
    FileVector m_duplicatesForRetranslation;


    // helper variable to fix cases if update is performed while source changes
    //  in this case second update attempt is going to be performed
    NoCaseString m_basesDateAtStartUpdate;

    // this date is used to check if licence expired
        // TODO: make different dates for retranslation and update operations
    NoCaseString m_basesDateCurrentIteration;

    unsigned long m_retranslationBlackListLimit;
    unsigned long m_updateBlackListLimit;

    // automatic proxy server address detection instance
    ProxyDetectorNamespace::ProxyDetector m_proxyDetector;

    // proxy address for current source
    Address m_currentProxyAddress;
};


NoCaseString KAVUPDCORE_API getPreferredRegion(Log *);

}   // namespace KLUPD

#endif // UPDATER_H_INCLUDED_1493FB54_323C_4fd4_9F0F_1622013213F1
