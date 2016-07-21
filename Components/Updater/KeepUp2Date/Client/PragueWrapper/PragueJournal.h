#if !defined(AFX_PRAGUEJOURNAL_H__086B2BCD_3F91_49A1_A094_513A422316EB__INCLUDED_)
#define AFX_PRAGUEJOURNAL_H__086B2BCD_3F91_49A1_A094_513A422316EB__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "comdefs.h"
#include "UpdaterTask/task.h"

class PragueCallbacks;

class PragueJournal : public KLUPD::JournalInterface
{
public:
    PragueJournal(CUpdater2005 &, KLUPD::DownloadProgress &, PragueCallbacks &);

    // create message identifier for Prague
    static tQWORD createMessageIdentifier(const size_t severity, const size_t eventIdentifier);

    virtual void publishMessage(const KLUPD::CoreError &, const KLUPD::NoCaseString &parameter1 = KLUPD::NoCaseString(),
        const KLUPD::NoCaseString &parameter2 = KLUPD::NoCaseString());

    // inform application that file is updated
    virtual void fileUpdated(const KLUPD::FileInfo &, const ReplaceMechanism &, const bool retranslationMode);

    // inform application that new file has been is added
    virtual void fileAdded(const KLUPD::FileInfo &, const bool retranslationMode);

private:
    // disable copy operations
    PragueJournal &operator=(const PragueJournal &);
    PragueJournal(const PragueJournal &);

    // selector if message is important and should be written to generic journal, or journal with critical events
    size_t getJournalSeverityLevel(const KLUPD::CoreError &);


    // current download size to fill file size field for each file in Journal
    tDWORD m_bytesDownloadedSave;

    KLUPD::DownloadProgress &m_downloadProgress;
    PragueCallbacks &m_callbacks;
    CUpdater2005 &m_updateTask;

    // current update source
    KLUPD::Path m_currentSource;
};

#endif // !defined(AFX_PRAGUEJOURNAL_H__086B2BCD_3F91_49A1_A094_513A422316EB__INCLUDED_)
