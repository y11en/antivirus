#if !defined(AFX_SITESFILEXMLPARSER_H__CA2DA138_A532_4941_B672_B4EE2D3A9679__INCLUDED_)
#define AFX_SITESFILEXMLPARSER_H__CA2DA138_A532_4941_B672_B4EE2D3A9679__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "stdinc.h"
#include "sites_info.h"


namespace KLUPD {

/// This class parses sites XML files (like updcfg.xml). Sites file
///  contains sources set for Updater and information about primary index
class KAVUPDCORE_API SitesFileXMLParser
{
public:
    SitesFileXMLParser(std::vector<SiteInfo>&, SiteInfo::Description&, UpdateSchedule &automaticScheduleForUpdate, const bool oldFormat, Log *);

    bool Parse(XmlReader&);

    void SetRootSectionName(const NoCaseString& strRootSectionName)
    {
        m_strRootSectionName = strRootSectionName;
    }

private:
    // disable copy operations
    SitesFileXMLParser &operator=(const SitesFileXMLParser &);
    SitesFileXMLParser(const SitesFileXMLParser &);

    void onStartElement(const NoCaseString &name, const XmlAttrMap &);
    void onEndElement();
    
    bool parseSiteXMLRecursively(XmlReader &, int guardLevel);
    void parseSite(const XmlAttrMap &);
    void parseDescription(const XmlAttrMap &);

    std::stack<NoCaseString> m_sectionHistory;
    NoCaseString m_currentSectionName;
    size_t m_skipSectionLevel;
    std::vector<SiteInfo> &m_sites;
    SiteInfo::Description &m_description;
    UpdateSchedule &m_automaticScheduleForUpdate;

    const bool m_oldFormat;
    Log *pLog;

    NoCaseString m_strRootSectionName;

    ///////// a document (root) section name in sites XML file /////////
    static const NoCaseString s_rootSection;

    ////// section with all update sources //////
    static const NoCaseString s_sectionSites;

    // XML tag with update source description
    static const NoCaseString s_tagSource;
    // XML tag with update source description (in obsolete format)
    static const NoCaseString s_tagSourceObsolete;
    // address attribute
    static const NoCaseString s_attributeAddress;
    // 2-letter ISO 3166 Country Code transformed to lower case (in old format Country Name)
    static const NoCaseString s_attributeRegion;
    // source weight
    static const NoCaseString s_attributeWeight;
    // XML attribute which specifies where difference files should be obtained from
    static const NoCaseString s_attributeDifferenceFolder;
    // XML tag with file description
    static const NoCaseString s_tagDescription;

    ////// Information about scheduling //////
    static const NoCaseString s_sectionUpdatePeriod;
};


}   // namespace KLUPD

#ifdef _WIN32
extern "C" {
    bool KAVUPDCORE_API GetBestUrl(const char* p_pszXmlBuffer,
                                   int         p_nXmlBufferSize,
                                   const char* p_pszRegion,
                                   char*       p_pszResult,
                                   int         p_nResultSize,
                                   char*       p_pszComponents,
                                   int         p_nComponentsSize,
                                   char*       p_pszTimeout,
                                   int         p_nTimeoutSize) ;
};
#endif // _WIN32

#endif // !defined(AFX_SITESFILEXMLPARSER_H__CA2DA138_A532_4941_B672_B4EE2D3A9679__INCLUDED_)
