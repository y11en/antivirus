#if !defined(update_TRANSACTION_H_INCLUDED)
#define update_TRANSACTION_H_INCLUDED

#include "stdinc.h"
#include "fileInfo.h"

namespace KLUPD {

// Updater files rotate class
class KAVUPDCORE_API UpdaterTransaction
{
public:
    explicit UpdaterTransaction(const Path &rollbackFolder, const bool retranslationMode,
        DownloadProgress &, CallbackInterface &, JournalInterface &, Log *);
    
    // launch file operation group (transaction)
     // Warning: there is no guarantee, that files in rollback
     //  folder are not damaged in case file transaction fails
    // Note: files status is adjusted to unchanged for all files failed to install
    CoreError run(FileVector &);

private:

    CoreError runImplementation(const FileVector &);

    // implements component installation algorithm
     // failIndex [out] - saves the position of file which is failed to be replaced
    bool processInstallComponent(const NoCaseString &componentName, const FileVector &, size_t &failIndex);

    // copy component files to rollback folder
    CoreError createRollbackFileCopiesForComponent(const FileVector &);
    // pre-install component: put files to pre-install destination
    CoreError preInstallComponent(const FileVector &, size_t &failIndex);

    // pre-replace algorithm selector
    CoreError preReplaceAlgorithmSelector(const FileInfo &);
    // pre-replace file implementaiton with suffix algorithm
    CoreError preReplaceFileWithSuffix(const FileInfo &);
    // try pre-replace file, in case failure 'replace-locked-file' implementation will be used
    bool preReplaceFileOrdinary(const FileInfo &);
    // pre-replace implementation for locked files
    CoreError preReplaceOrdinaryLockedFile(const FileInfo &);
    // pre-add file implemenation
    CoreError preAddFile(const FileInfo &);

    // post-install component step (after pre-intalled component is checked by product)
    CoreError postInstallComponent(const FileVector &);

    // in case some installation error, then whole component is rolled back
     // failIndex [in] - is hint which files were replaced
    void rollbackComponent(const NoCaseString &componentIdentifier, const FileVector &, const size_t failIndex);



    // generates suffix for 'replace-with-suffix' files installation algorithm
    void makeFileReplaceSuffix();
    // calculates file name with suffix for pre-install step
    Path getFilePreInstallLocation(const FileInfo &)const;

    // clean trash helper transaction files
    void cleanTransactionHelperFiles(const FileVector &);
    // helper to work with last transaction error
    void saveTransactionLastError(const CoreError &);



    // component must be installed atomically, so product and updater use mutex for component
    void lockComponent(const NoCaseString &);
    void unlockComponent(const NoCaseString &);

    // helper to switch context for Update operation,
     // because files in e.g. for Vista product folder may need permissions for re-write
    void switchToSystemContext(const std::string &traceText);
    void switchToUserContext(const std::string &traceText);

    Path m_fileReplaceSuffix;

    // indicate whether at least one component is installed to adjust integral transaction result
    bool m_anyComponentInstalled;
    // saved last error to adjust integral transaction result
    CoreError m_lastError;
    // a list of components not installed due to error
    std::vector<NoCaseString> m_failComponents;

    const Path m_rollbackFolder;

    const bool m_retranslationMode;

    DownloadProgress &m_downloadProgress;
    CallbackInterface &m_callbacks;
    JournalInterface &m_journal;
    Log *pLog;
};


}   // namespace KLUPD

#endif
