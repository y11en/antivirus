#if !defined(SITES_INFO_H_INCLUDED_20599792_D0AD_4918_A4AE_CA554AC6CF1F)
#define SITES_INFO_H_INCLUDED_20599792_D0AD_4918_A4AE_CA554AC6CF1F

#include "map_wrapper.h"

#include "path.h"

#include "fileInfo.h"

#include "../include/updateSchedule.h"

namespace KLUPD {

// Update source information
class KAVUPDCORE_API SiteInfo
{
public:
    typedef NoCaseString Region;
    typedef std::vector<Region> RegionList;
    typedef PathSubstitutionMap Description;

    SiteInfo(const Region &region = Region(), const Path &address = L"", const double weight = 0, const Path &differenceFolder = L"diffs/");
    NoCaseString toString()const;

    // 2-letter ISO 3166 country code
    Region m_region;
    // server address
    Path m_address;
    // server weight. Is using to make a choice between servers
    double m_weight;
    // path where difference files are located on source
    Path m_differenceFolder;

    RegionList m_regions;


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    /// TODO: make members of class SitesVector

    // parses xml file with information about sources
    // oldFormat [in] - flag indicating if <site> tag is used while parsing, otherwise (new format) <size2> tag is used
    bool getSitesInfo(  std::vector<SiteInfo> &,
                        Description& description,
                        const Path &fileName,
                        const bool oldFormat,
                        Log *,
                        const bool setDefaultOnError,
                        const NoCaseString& strRootSectionName = NoCaseString())const;
    // sort sites list according to weight and region
    void sortSites(const std::vector<SiteInfo> &, const Region &region, std::vector<Path> &sortedSites, Log *)const;
    // appends randomly chosen site from hardcoded list to vector
    void appendRandomlyChosenHarcodedSite(std::vector<Path> &)const;
    void splitRegionList(const Region &source, RegionList &destination)const;
    bool parseSiteXML( std::vector<SiteInfo> &,
                       Description& description,
                       UpdateSchedule &automaticScheduleForUpdate,
                       const Path &fileName, const bool oldFormat, Log *,
                       const NoCaseString& strRootSectionName = NoCaseString())const;

private:
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    /// TODO: make members of class SitesVector
    void fillHardcodedList(std::vector<SiteInfo> &, Log *)const;
    static void sortWithWeightRandomization(std::vector<SiteInfo> &);
    bool isInList(const RegionList &, const Region &element)const;
};

struct HarcodedUpdateSource
{
    HarcodedUpdateSource();

    // hardcoded sources, which are used as Kaspersky Laboratory sources
     // in case configuraiton file is absent or damaged
    std::vector<SiteInfo> m_hardcodedSources;
};


}   // namespace KLUPD

#endif  // SITES_INFO_H_INCLUDED_20599792_D0AD_4918_A4AE_CA554AC6CF1F
