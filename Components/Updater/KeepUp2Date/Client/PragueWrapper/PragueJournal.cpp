#include "PragueJournal.h"

#include "../../../SharedCode/PragueHelpers/LocalizeComponentName.h"

PragueJournal::PragueJournal(CUpdater2005 &updateTask, KLUPD::DownloadProgress &downloadProgress, PragueCallbacks &callbacks)
 : m_bytesDownloadedSave(0),
   m_downloadProgress(downloadProgress),
   m_callbacks(callbacks),
   m_updateTask(updateTask)
{
}

tQWORD PragueJournal::createMessageIdentifier(const size_t severity, const size_t eventIdentifier)
{
    return static_cast<tQWORD>(severity) + (static_cast<tQWORD>(eventIdentifier) << 32);
}

void PragueJournal::publishMessage(const KLUPD::CoreError &code, const KLUPD::NoCaseString &parameter1, const KLUPD::NoCaseString &parameter2)
{
    cUpdaterEvent evnt;
    evnt.m_qwEvent = createMessageIdentifier(getJournalSeverityLevel(code), pm_CORE_EVENT);
    evnt.m_tmTimeStamp = cDateTime::now_utc();
    evnt.m_defaultLocalization = KLUPD::toString(code).toWideChar();
    evnt.m_parameter1 = parameter1.toWideChar();
    evnt.m_parameter2 = parameter2.toWideChar();
    evnt.m_errError = PRAGUE_HELPERS::translateUpdaterResultCodeToPrague(code);

    switch(code)
    {
    case KLUPD::EVENT_DOWNLOAD_FILE_STARTED:
        {
            cAutoCS autoCSStatistics(m_updateTask.m_hCSStatistics, true);
            m_updateTask.m_Statistics.m_strFileName = parameter1.toWideChar();
        }
        break;

    case KLUPD::EVENT_SOURCE_SELECTED:
        {
            cAutoCS autoCSStatistics(m_updateTask.m_hCSStatistics, true);
            m_updateTask.m_Statistics.m_strHostPath = parameter1.toWideChar();
        }
        break;

    case KLUPD::EVENT_FILE_DOWNLOADED:
        evnt.m_dwFileSize = m_downloadProgress.bytesTransferred() - m_bytesDownloadedSave;
        m_bytesDownloadedSave = m_downloadProgress.bytesTransferred();
        break;

    case KLUPD::EVENT_StartInstallFilesForUpdate:
    case KLUPD::EVENT_StartInstallFilesForRetranslation:
    case KLUPD::EVENT_StartCopyFilesForRollback:
        // reset file name
        {
            cAutoCS autoCSStatistics(m_updateTask.m_hCSStatistics, true);
            m_updateTask.m_Statistics.m_strHostPath = "";
            m_updateTask.m_Statistics.m_strFileName = "";
        }
        break;

    case KLUPD::EVENT_NEW_FILE_INSTALLED:
        break;

    case KLUPD::EVENT_FILE_UPDATED:
        break;

    case KLUPD::EVENT_ComponentIsNotUpdated:
    case KLUPD::CORE_ComponentRejectedByProduct:
	case KLUPD::EVENT_ComponentIsNotRetranslated:
        {
            // adjusting task completion result
			if ( code != KLUPD::EVENT_ComponentIsNotRetranslated )
			{
				m_updateTask.m_failedToUpdateAllComponents = cTRUE;
			}

            // get localization file name
            const KLUPD::Path componentLocalizationFile = m_callbacks.m_updaterSettingsFile.getString(PragueCallbacks::s_componentLocalizationSection);
            PRAGUE_HELPERS::Components components;
            if(!componentLocalizationFile.empty() 
                && PRAGUE_HELPERS::getLocalizedComponentsInformationFromFile(componentLocalizationFile.toWideChar(), components, m_updateTask.pLog))
            {
                for(PRAGUE_HELPERS::Components::const_iterator componentIter = components.begin(); componentIter != components.end(); ++componentIter)
                {
                    if((parameter1 == componentIter->m_componentIdentificator.data())
                        && !componentIter->m_componentName.empty())
                    {
                        evnt.m_parameter1 = componentIter->m_componentName;
                        break;
                    }
                }
            }
                
            break;
        }
    default:
        // no specific actions is needed, only to suppress compilation warning
        break;
    }

    // write message into journal
    m_updateTask.sysSendMsg(getJournalSeverityLevel(code), pm_CORE_EVENT, 0, &evnt, SER_SENDMSG_PSIZE);
}

void PragueJournal::fileUpdated(const KLUPD::FileInfo &file, const ReplaceMechanism &replaceMechanism, const bool retranslationMode)
{
    // update index files does not require reboot
    if(file.m_type == KLUPD::FileInfo::index)
        return;

    // no reboot for retranslation
    if(retranslationMode)
        return;

    // reboot needed for product to use updated file
    m_callbacks.m_needReboot |= (replaceMechanism != odinary)
            || m_callbacks.needRebootIfComponentUpdated(file.m_componentIdSet);
}

void PragueJournal::fileAdded(const KLUPD::FileInfo &file, const bool retranslationMode)
{
    // update index files does not require reboot
    if(file.m_type == KLUPD::FileInfo::index)
        return;

    // no reboot for retranslation
    if(retranslationMode)
        return;

    m_callbacks.m_needReboot |= !!m_callbacks.needRebootIfComponentUpdated(file.m_componentIdSet);
}

size_t PragueJournal::getJournalSeverityLevel(const KLUPD::CoreError &code)
{
    switch(code)
    {
    // non-important events
    case KLUPD::CORE_NO_ERROR:
    case KLUPD::CORE_RETRANSLATION_SUCCESSFUL:
    case KLUPD::EVENT_DOWNLOAD_FILE_STARTED:
    case KLUPD::EVENT_DNS_NAME_RESOLVED:
        return pmc_UPDATER_REPORT;

    // all other are important events
    default:
        return pmc_UPDATER_CRITICAL_REPORT;
    }
}

