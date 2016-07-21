#include "ini_updater.h"

#include "TestInterface.h"

Ini_Updater::Ini_Updater(CallbackInterface *callback, Log *log, const bool useCurrentFolder)
 : Updater(STRING(), log),
   m_useCurrentFolder(useCurrentFolder)
{
    callbacks = callback;
}

JournalInterface *Ini_Updater::createJournal()
{
    return new Ini_Journal(pLog);
}

CallbackInterface *Ini_Updater::setCallbacks()
{
    return callbacks;
}

KLUPD::CoreError Ini_Updater::getProductConfiguration()
{
    pConfig = new Ini_ProductConfiguration(*this, pLog, m_useCurrentFolder);

    try
    {
        pConfig->GetData();
    }
    catch(const std::exception &error)
    {
        TRACE_MESSAGE2("Exception while getting product configuration '%s'", error.what());
        return KLUPD::CORE_ADMKIT_FAILURE;
    }

    return KLUPD::CORE_NO_ERROR;
}


KLUPD::CoreError Ini_Updater::getUpdaterConfiguration()
{
    uSettings = new Ini_UpdaterSettings(pLog, m_useCurrentFolder);

    try
    {
        uSettings->GetData();
    }
    catch(const std::exception &error)
    {
        TRACE_MESSAGE2("Exception while getting updater configuration '%s'", error.what());
        return KLUPD::CORE_ADMKIT_FAILURE;
    }

    return KLUPD::CORE_NO_ERROR;
}

KLUPD::CoreError Ini_Updater::getUpdaterLists()
{
    TRACE_MESSAGE2("Getting updater lists for product '%s'", pID.to_string().c_str());
    mLists = new Ini_UpdaterLists(true, pLog, m_useCurrentFolder);
    uLists = new Ini_UpdaterLists(false, pLog, m_useCurrentFolder);

    return KLUPD::CORE_NO_ERROR;
}

