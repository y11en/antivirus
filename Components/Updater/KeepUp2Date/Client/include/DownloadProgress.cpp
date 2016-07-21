#include "DownloadProgress.h"


std::string KLUPD::DownloadProgress::toString(const AuthorizationTarget &authorizationTarget)
{
    if(authorizationTarget == server)
        return "authorization on server";
    if(authorizationTarget == proxyServer)
        return "authorization on proxy";

    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << "unknown authorization target " << static_cast<const int>(authorizationTarget);
    return stream.str();
}


// one percent tick per 200 files, do not want progress bar to run far while local files enumeration
const size_t KLUPD::DownloadProgress::s_filesNeededForOneTickForEnumeration = 200;

KLUPD::DownloadProgress::DownloadProgress()
 : m_expectedDownloadSize(0),
   m_bytesTransferred(0),
   m_timerStart(0),
   m_clockPassed(0),
   m_percentAdjustState(checkingLocalFiles),
   m_fileNumberEnumerated(0),
   m_percentRestForEnumeratingLocalFiles(7),
   m_percentRestForDownloadIndex(20),
   m_percentRestForReplacingFiles(10),
   m_maximumPercentValueReached(1),
   m_percentGetNotByDownloadedBytes(0)
{
}

KLUPD::DownloadProgress::~DownloadProgress()
{
}

bool KLUPD::DownloadProgress::requestAuthorizationCredentials(Credentials &)
{
    // can provide no credentials by default
    return false;
}

void KLUPD::DownloadProgress::percentCallback(const size_t currentPercentValue)
{
}

bool KLUPD::DownloadProgress::checkCancel()
{
    // by default continue update
    return false;
}


KLUPD::DownloadProgress::BytesPerSecond KLUPD::DownloadProgress::transferRate()const
{
    if(!m_clockPassed || !m_bytesTransferred)
        return 0.0;

    const double durationSeconds = static_cast<double>(m_clockPassed) / CLOCKS_PER_SEC;

    if(!durationSeconds)
        return 0.0;
    return static_cast<BytesPerSecond>(static_cast<double>(m_bytesTransferred) / durationSeconds);
}

size_t KLUPD::DownloadProgress::expectedDownloadSize()const
{
    return m_expectedDownloadSize;
}

size_t KLUPD::DownloadProgress::bytesTransferred()const
{
    return m_bytesTransferred;
}

void KLUPD::DownloadProgress::updateSpeedStartTimer()
{
    m_timerStart = clock();
}
void KLUPD::DownloadProgress::updateSpeedStopTimer()
{
    m_clockPassed += clock() - m_timerStart;
}



void KLUPD::DownloadProgress::bytesTransferred(const size_t bytes)
{
    m_bytesTransferred += bytes;

    // do not allow to change current percent in case update size is not calculated,
     // thus avoid reach incorrectly high percent in case switch to other source
    if(m_percentAdjustState != indexDownloaded)
        return;

    // leave 10% for helper traffic (like HTTP headers and not match difference files)
    const double expectedDownloadSize = 0.9 * m_expectedDownloadSize;
    // calculate percent
    const size_t percents = expectedDownloadSize
        ? static_cast<size_t>((100.0 * static_cast<double>(m_bytesTransferred)) / expectedDownloadSize)
        : 0;

    // normilize percent
    size_t normilizedPercent
        = (percents * (100 - m_percentRestForEnumeratingLocalFiles - m_percentRestForDownloadIndex - m_percentRestForReplacingFiles))
        / 100;

    normilizedPercent += m_percentGetNotByDownloadedBytes;
    
    // download started, move progress bar slightly
    if(!normilizedPercent)
        normilizedPercent = 1;


    // protection agains percent value > 100
    if(99 <= normilizedPercent)
        normilizedPercent = 99;

    // do not allow percent to go back
    if(normilizedPercent < m_maximumPercentValueReached)
        normilizedPercent = m_maximumPercentValueReached;
    else
        m_maximumPercentValueReached = normilizedPercent;

    percentCallback(normilizedPercent);
}

void KLUPD::DownloadProgress::percentAdjustState(const PercentAdjustState &state)
{
    percentCallback(m_maximumPercentValueReached);
    m_percentAdjustState = state;
}

void KLUPD::DownloadProgress::tickPercent()
{
    switch(m_percentAdjustState)
    {
    case checkingLocalFiles:
        {
            // one more files enumerated
            ++m_fileNumberEnumerated;

            // check if progress should be switched
            if((m_fileNumberEnumerated % s_filesNeededForOneTickForEnumeration) == 0)
            {
                // check if there is reserved percent for enumeration local rests
                if(m_percentRestForEnumeratingLocalFiles)
                {
                    --m_percentRestForEnumeratingLocalFiles;
                    if(m_maximumPercentValueReached < 100)
                    {
                        ++m_maximumPercentValueReached;
                        ++m_percentGetNotByDownloadedBytes;
                        percentCallback(m_maximumPercentValueReached);
                    }
                }
            }
        }
        return;
    case downloadingIndex:
        {
            // check if there is reserved percend for download index rests
            if(m_percentRestForDownloadIndex)
            {
                --m_percentRestForDownloadIndex;
                if(m_maximumPercentValueReached < 100)
                {
                    ++m_maximumPercentValueReached;
                    ++m_percentGetNotByDownloadedBytes;
                    percentCallback(m_maximumPercentValueReached);
                }
            }
        }
        return;

    case indexDownloaded:
        // no special progress ticks in index downloaded state
         // all progress is displayed by transferred bytes callbacks
        return;

    case replacingFiles:
        {
            if(m_percentRestForReplacingFiles)
            {
                --m_percentRestForReplacingFiles;
                if(m_maximumPercentValueReached < 100)
                {
                    ++m_maximumPercentValueReached;
                    ++m_percentGetNotByDownloadedBytes;
                    percentCallback(m_maximumPercentValueReached);
                }
            }
        }
        return;
    }
}


////////////////////////////////////////////////////////////////////////
//// EmptyDownloadProgress class implementation
////////////////////////////////////////////////////////////////////////

bool KLUPD::EmptyDownloadProgress::requestAuthorizationCredentials(Credentials &)
{
    return false;
}

bool KLUPD::EmptyDownloadProgress::authorizationTargetEnabled(const AuthorizationTarget &)
{
    return false;
}
