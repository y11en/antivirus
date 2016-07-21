#if !defined(JOURNAL_IFACE_H_INCLUDED)
#define JOURNAL_IFACE_H_INCLUDED

#include "../helper/fileInfo.h"

namespace KLUPD {

/// Journal interface class
class KAVUPDCORE_API JournalInterface
{
public:
    enum ReplaceMechanism
    {
        // odinary file replace mechanism
        odinary,
        // locked file was renamed first to be updated
        lockedFileReplaceMechanismUsed,
    };

    virtual ~JournalInterface()
    {
    }

    // publish update progress event to product
    virtual void publishMessage(const KLUPD::CoreError &, const NoCaseString &parameter = NoCaseString(),
        const NoCaseString &parameter2 = NoCaseString()) = 0;


    virtual void publishRetranslationResult(const KLUPD::CoreError &)
    {
    }
    virtual void publishUpdateResult(const KLUPD::CoreError &)
    {
    }
    virtual void publishRollbackResult(const KLUPD::CoreError &)
    {
    }

    // inform application that file is updated
    virtual void fileUpdated(const KLUPD::FileInfo &, const ReplaceMechanism &, const bool retranslationMode)
    {
    }
    // inform application that new file has been is added
    virtual void fileAdded(const KLUPD::FileInfo &, const bool retranslationMode)
    {
    }

protected:
    JournalInterface()
    {
    }
};

// this class does no output anything to journal
class KAVUPDCORE_API EmptyJournal : public JournalInterface
{
public:
    virtual void publishMessage(const KLUPD::CoreError &, const NoCaseString &, const NoCaseString &)
    {
    }
};


}   // namespace KLUPD

#endif
