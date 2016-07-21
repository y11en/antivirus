#include "stdinc.h"
#include "sourceDescription.h"
#include "updaterStaff.h"

KLUPD::Source::Source(const Path &URL, const EUpdateSourceType &updateSourceType, const bool useProxy,
               const Path &differenceFolder, const EUpdateStructure &structure)
 : m_url(URL),
   m_type(updateSourceType),
   m_useProxy(useProxy),
   m_differenceFolder(differenceFolder),
   m_structure(structure)
{
    m_differenceFolder.correctPathDelimiters();
}

bool KLUPD::Source::operator==(const Source &right)const
{
    return m_url == right.m_url
        && m_type == right.m_type
        && m_useProxy == right.m_useProxy
        && m_differenceFolder == right.m_differenceFolder
        && m_structure == right.m_structure;
}

bool KLUPD::Source::onlyDirectoriedStructureAllowed()const
{
    return m_type == UST_KLServer;
}
KLUPD::NoCaseString KLUPD::Source::toString()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    stream << ::toString(m_type).toAscii() << " " << m_url.toAscii();
    if(isFTPorHTTP(m_type, m_url))
        stream << (m_useProxy ? ", proxy enabled" : ", proxy disabled");
    stream << ", difference path '" << m_differenceFolder.toAscii() << "'";

    return asciiToWideChar(stream.str());
}

KLUPD::SourceList::SourceList()
 : m_currentIndex(0)
{
}

void KLUPD::SourceList::addSource(const EUpdateSourceType &type, const Path &URL, const bool useProxy, const Path &differenceFolder)
{
    Path url = URL;
    url.correctPathDelimiters();

    Source source(url, type, isFTPorHTTP(type, url) ? useProxy : false, differenceFolder);

    // check if source is not already in list
    if(std::find(m_sources.begin(), m_sources.end(), source) == m_sources.end())
        m_sources.push_back(source);
}

bool KLUPD::SourceList::getCurrentElement(Source &source)const
{
    if(!currentIndexInRange())
        return false;

    source = m_sources[m_currentIndex];
    return true;
}

EUpdateStructure KLUPD::SourceList::currentUpdateStructure()const
{
    if(currentIndexInRange())
        return m_sources[m_currentIndex].m_structure;

    return UPS_DIRS;
}
void KLUPD::SourceList::currentUpdateStructure(const EUpdateStructure &structure)
{
    if(currentIndexInRange())
        m_sources[m_currentIndex].m_structure = structure;
}

void KLUPD::SourceList::disableProxyForCurrentSource()
{
    if(currentIndexInRange())
        m_sources[m_currentIndex].m_useProxy = false;
}

bool KLUPD::SourceList::currentSourceIgnoreDates()const
{
    if(!currentIndexInRange())
        return false;
    return m_sources[m_currentIndex].m_type != UST_KLServer;
}

void KLUPD::SourceList::moveFirst()
{
    m_currentIndex = 0;
}
void KLUPD::SourceList::moveNext()
{
    ++m_currentIndex;
}

KLUPD::NoCaseString KLUPD::SourceList::toString(const char *delimiter)const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    for(std::vector<Source>::const_iterator sourceIter = m_sources.begin(); sourceIter != m_sources.end(); ++sourceIter)
    {
        stream << delimiter << sourceIter->toString().toAscii();

        // append line break for all sources but last one
        if(sourceIter + 1 != m_sources.end())
             stream << "\n";
    }
    return asciiToWideChar(stream.str());
}

bool KLUPD::SourceList::currentIndexInRange()const
{
    return (0 <= m_currentIndex)
        && (m_currentIndex < m_sources.size());
}
