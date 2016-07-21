#include "stdinc.h"

#include "sites_info.h"
#include "sitesFileXMLParser.h"
#include "updaterStaff.h"

static const wchar_t *g_anyRegionString = L"*";
static const wchar_t *g_regionDelimiter = L",";

KLUPD::SiteInfo::SiteInfo(const Region &region, const Path &address, const double weight, const Path &differenceFolder)
 : m_region(region),
   m_address(address),
   m_weight(weight),
   m_differenceFolder(differenceFolder)
{
}

KLUPD::NoCaseString KLUPD::SiteInfo::toString()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << "region " << m_region.toAscii()
        << ", url '" << m_address.toAscii()
        << "', weight " << m_weight
        << ", difference folder '" << m_differenceFolder.toAscii() << "'";

    NoCaseString regions;
    for(RegionList::const_iterator iter = m_regions.begin(); iter != m_regions.end(); ++iter)
        regions += *iter + g_regionDelimiter;
    // removing one extra comma
    if(!regions.empty())
        regions.resize(regions.size() - 1);

    if(m_region != regions)
        stream << ", parsed regions " << regions.toAscii();
    return asciiToWideChar(stream.str());
}

void KLUPD::SiteInfo::fillHardcodedList(std::vector<SiteInfo> &inf, Log *pLog)const
{
    TRACE_MESSAGE("Using hardcoded source list");

    HarcodedUpdateSource harcodedUpdateSource;
    for(std::vector<SiteInfo>::const_iterator sourceIter = harcodedUpdateSource.m_hardcodedSources.begin(); sourceIter != harcodedUpdateSource.m_hardcodedSources.end(); ++sourceIter)
    {
        SiteInfo nonConstCopy = *sourceIter;
        splitRegionList(nonConstCopy.m_region, nonConstCopy.m_regions);
        inf.push_back(nonConstCopy);
    }
}

bool KLUPD::SiteInfo::parseSiteXML(std::vector<SiteInfo> &sites,
                                   Description& description,
                                   UpdateSchedule &automaticScheduleForUpdate,
                                   const Path &fileName,
                                   const bool oldFormat,
                                   Log *pLog,
                                   const NoCaseString& strRootSectionName)const
{
    std::vector<unsigned char> buffer;
    const CoreError readSitesFileResult = LocalFile(fileName, pLog).read(buffer);
    if(!isSuccess(readSitesFileResult))
    {
        TRACE_MESSAGE3("Failed to read XML sites configuration file '%S', result '%S'",
            fileName.toWideChar(), KLUPD::toString(readSitesFileResult).toWideChar());
        return false;
    }
    if(buffer.empty())
    {
        TRACE_MESSAGE3("XML sites configuration file '%S' is corrupted, file size is %d",
            fileName.toWideChar(), buffer.size());
        return false;
    }

    #ifdef KLUPD_ENABLE_SIGNATURE_5
        if(!checkInsideSignature5(fileName))
        {
            TRACE_MESSAGE2("XML sites file has wrong signature '%S'", fileName.toWideChar());
            return false;
        }
    #else
        TRACE_MESSAGE2("Signature 5 checks are disabled. Can not check signature for '%S'", fileName.toWideChar());
    #endif

    SitesFileXMLParser xmler(sites, description, automaticScheduleForUpdate, oldFormat, pLog);
    if( !strRootSectionName.empty() )
        xmler.SetRootSectionName(strRootSectionName);
    XmlReader xmlReader(reinterpret_cast<const char *>(&buffer[0]), buffer.size());
    return xmler.Parse(xmlReader);
}

bool KLUPD::SiteInfo::getSitesInfo(std::vector<SiteInfo> &sites,
                                   Description& description,
                                   const Path &filname,
                                   const bool oldFormat,
                                   Log *pLog,
                                   const bool setDefaultOnError,
                                   const NoCaseString& strRootSectionName
                                   )const
{
    std::vector<SiteInfo> sitesCopy = sites;

    UpdateSchedule unusedUpdateSchedule;
    if(parseSiteXML(sites, description, unusedUpdateSchedule, filname, oldFormat, pLog, strRootSectionName))
        return true;

    // restore initial state
    sites.swap(sitesCopy);

    if(setDefaultOnError)
    {
        fillHardcodedList(sites, pLog);
        return true;
    }
    return false;
}

void KLUPD::SiteInfo::sortWithWeightRandomization(std::vector<SiteInfo> &sites)
{
    std::vector<SiteInfo> tempSites = sites;
    sites.clear();
    
    // make an extra call to rand() in order to compensate fully predictable generated digit under Win.*
    srand(static_cast<unsigned int>(time(0)));
    // figthing against bad random number generator in standard library
    rand(); rand(); rand(); rand(); rand(); rand();
    
    while(tempSites.size())
    {
        // calculate normalizing parameters
        double weightsSum = 0;
        for(size_t siteIndex = 0; siteIndex < tempSites.size(); ++siteIndex)
            weightsSum += tempSites[siteIndex].m_weight;
        
        // generate normalized value

        const double randValue = static_cast<double>(rand());
        const double minimumAcceptedWeight = randValue * weightsSum / static_cast<double>(RAND_MAX);
        
        // look what entry has been affected
        weightsSum = 0;
        for(unsigned j = 0; j < tempSites.size(); ++j)
        {
            weightsSum += tempSites[j].m_weight;
            if((minimumAcceptedWeight <= weightsSum) || (j == tempSites.size() - 1))
            {
                // this is the entry we need
                sites.push_back(tempSites[j]); 
                tempSites.erase(tempSites.begin() + j);
                break;
            }
        }
    }
}

bool KLUPD::SiteInfo::isInList(const RegionList &sites, const Region &element)const
{
    return std::find(sites.begin(), sites.end(), element) != sites.end();
}

void KLUPD::SiteInfo::sortSites(const std::vector<SiteInfo> &sites, const Region &region,
                                std::vector<Path> &sortedSites, Log *pLog)const
{
    sortedSites.clear();

    if(region.empty())
        TRACE_MESSAGE("No region specified, going default\n");

    // First sort those sites, that have specified region
    std::vector<SiteInfo> regionFilteredSites;
    std::vector<SiteInfo> default_sites;

    for(size_t defaultSitesIndex = 0; defaultSitesIndex < sites.size(); ++defaultSitesIndex)
    {
        if(isInList(sites[defaultSitesIndex].m_regions, region))
            regionFilteredSites.push_back(sites[defaultSitesIndex]);

        if(isInList(sites[defaultSitesIndex].m_regions, Region(g_anyRegionString)))
            default_sites.push_back(sites[defaultSitesIndex]);
    }

    bool using_default_region = false;
    if(regionFilteredSites.empty())
    {
        regionFilteredSites = default_sites;
        using_default_region = true;
    }
    
    sortWithWeightRandomization(regionFilteredSites);
    // Copy ready names to resulting vector
    for(size_t sortedSitesIndex = 0; sortedSitesIndex < regionFilteredSites.size(); ++sortedSitesIndex)
        sortedSites.push_back(regionFilteredSites[sortedSitesIndex].m_address);

    // Copy the rest sites
    std::vector<SiteInfo> sitesOfNotMatchRegions;
    for(size_t temporarySitesIndex = 0; temporarySitesIndex < sites.size(); ++temporarySitesIndex)
    {
        if(using_default_region)
        {
            if(!(isInList(sites[temporarySitesIndex].m_regions, region)
                || isInList(sites[temporarySitesIndex].m_regions, Region(g_anyRegionString))))
            {
                sitesOfNotMatchRegions.push_back(sites[temporarySitesIndex]);
            }
        }
        else if(!isInList(sites[temporarySitesIndex].m_regions, region))
            sitesOfNotMatchRegions.push_back(sites[temporarySitesIndex]);
    }
    
    sortWithWeightRandomization(sitesOfNotMatchRegions);
    // copy ready names to resulting vector
    for(size_t resultSortedSitesIndex = 0; resultSortedSitesIndex < sitesOfNotMatchRegions.size(); ++resultSortedSitesIndex)
        sortedSites.push_back(sitesOfNotMatchRegions[resultSortedSitesIndex].m_address);
}

void KLUPD::SiteInfo::appendRandomlyChosenHarcodedSite(std::vector<Path> &sites)const
{
    HarcodedUpdateSource harcodedUpdateSource;
    const size_t idx = static_cast<size_t>(time(0)) % harcodedUpdateSource.m_hardcodedSources.size();
    sites.push_back(harcodedUpdateSource.m_hardcodedSources[idx].m_address);
}

void KLUPD::SiteInfo::splitRegionList(const Region &source, RegionList &destination)const
{
    destination.clear();
    if(source.empty())
        return;

    size_t k = 0;
    size_t start = 0;
    for(k = 0; k < source.size(); ++k)
    {
        if (source[k] == L',' || source[k] == L' ')
        {
            if(k > start)
                destination.push_back(source.substr(start, k - start));
            start = k + 1;
        }
    }
    if(k > start)
        destination.push_back(source.substr(start, k - start));
}


KLUPD::HarcodedUpdateSource::HarcodedUpdateSource()
{
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"http://downloads1.kaspersky-labs.com", 30000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"http://downloads2.kaspersky-labs.com", 40000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"http://downloads3.kaspersky-labs.com", 40000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"http://downloads4.kaspersky-labs.com", 10000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"http://downloads5.kaspersky-labs.com", 40000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"ftp://downloads1.kaspersky-labs.com", 10000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"ftp://downloads2.kaspersky-labs.com", 20000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"ftp://downloads3.kaspersky-labs.com", 30000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"ftp://downloads4.kaspersky-labs.com", 30000.0, L"diffs/"));
    m_hardcodedSources.push_back(KLUPD::SiteInfo(g_anyRegionString, L"ftp://downloads5.kaspersky-labs.com", 30000.0, L"diffs/"));
}

