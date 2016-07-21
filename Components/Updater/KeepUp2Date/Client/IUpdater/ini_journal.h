#ifndef INI_JOURNAL_H_INCLUDED_19876263_741A_4185_9E4E_9EFE36F46D0D
#define INI_JOURNAL_H_INCLUDED_19876263_741A_4185_9E4E_9EFE36F46D0D

#include "../helper/stdinc.h"

/// interface for journaling events
class IniJournal : public KLUPD::JournalInterface
{
public:
    IniJournal();

    virtual void publishMessage(const KLUPD::CoreError &, const KLUPD::NoCaseString &parameter1 = KLUPD::NoCaseString(),
        const KLUPD::NoCaseString &parameter2 = KLUPD::NoCaseString());

    virtual void publishRetranslationResult(const KLUPD::CoreError &);
    virtual void publishUpdateResult(const KLUPD::CoreError &);
    virtual void publishRollbackResult(const KLUPD::CoreError &);

private:
    // disable copy operations
    IniJournal &operator=(const IniJournal &);
    IniJournal(const IniJournal &);
};

#endif  // #ifndef INI_JOURNAL_H_INCLUDED_19876263_741A_4185_9E4E_9EFE36F46D0D
