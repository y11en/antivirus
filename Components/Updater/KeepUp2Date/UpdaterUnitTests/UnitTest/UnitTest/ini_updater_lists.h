#if !defined(INI_UPDATER_LISTS_H_INCLUDED)
#define INI_UPDATER_LISTS_H_INCLUDED

#include <include/cfg_lists.h>

class Ini_UpdaterLists : public UpdaterLists
{
public:
	Ini_UpdaterLists(const bool retranslation, Log *, const bool useCurrentFolder);
	virtual ~Ini_UpdaterLists();

	virtual bool processLocalFiles(const FileVector &);
    virtual bool processChangedFiles(const STRING &rollbackFolder, const bool validLocalFilesForRollback);

    bool Ini_UpdaterLists::saveRollbackInfo(const FileVector &);

    virtual bool removeRollbackSection();
    
private:
    // disable copy operations
    Ini_UpdaterLists &operator=(const Ini_UpdaterLists &);
    Ini_UpdaterLists(const Ini_UpdaterLists &);

    bool readSingleFileInfoRollback(const TCHAR *iniFileName, TCHAR *fileSectionName, FileInfo &);
    bool readRollbackFileListFromSS(FileVector &changedFiles);
    void makeNewSectionName(const long index, const STRING &name, STRING &outputName);
    bool saveRollbackListEntry(const TCHAR *iniFileName, const TCHAR *parentSectionName, const long orderNumber, const STRING &newSectionName, const FileInfo &);

    const bool m_useCurrentFolder;
};

#endif
