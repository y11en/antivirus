#ifndef INI_BASED_ROLLBACK_H_INCLUDED_4E1249DA_7263_4ed5_9415_335BAF328AFF
#define INI_BASED_ROLLBACK_H_INCLUDED_4E1249DA_7263_4ed5_9415_335BAF328AFF

#include "../IniFile/IniFile.h"

namespace KLUPD {

// The IniBasedRollbackFile class provides implementation of updater rollback file based on ini-file
// The class may be re-used in different projects as shared code
class IniBasedRollbackFile
{
public:
    IniBasedRollbackFile(IniFile &, Log *);

    // wipes rollback file
    void clear();
    // read rollback files into input file vector
    bool readRollback(KLUPD::FileVector &);
    // save files into rollback ini-file
    // parameter rollbackComponentsFilter [in] - component filter, specifies components not to rollback
    // Some components can not be rolled back, as sample system files
    bool saveRollback(const KLUPD::FileVector &,
        const Path &rollbackFolder, const KLUPD::Path &retranslationFolder,
        const bool retranslationMode, const NoCaseString &rollbackComponentsFilter = NoCaseString());


private:
    void makeNewSectionName(const long index, const NoCaseString &name, NoCaseString &outputName);

    bool readSingleFileInfoRollback(const NoCaseString &sectionName, KLUPD::FileInfo &);
    bool saveRollbackListEntry(const NoCaseString &parentSectionName, const long orderNumber, const NoCaseString &newSectionName, const KLUPD::FileInfo &);
    bool saveRollbackInfo(const KLUPD::FileVector &);

    bool matchForRollback(const ComponentIdentefiers &filter, const ComponentIdentefiers &componentToCheck);


    IniFile &m_rollback;
    Log *pLog;
};


}   // namespace KLUPD

#endif  //  INI_BASED_ROLLBACK_H_INCLUDED_4E1249DA_7263_4ed5_9415_335BAF328AFF
