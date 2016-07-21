#include "sitesFileXMLParser.h"

#include "updaterStaff.h"

const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_rootSection = L"UpdaterSettingsSection";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_sectionSites = L"sites_list";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_tagSource = L"site2";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_tagSourceObsolete = L"site";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_attributeAddress = L"url";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_attributeRegion = L"region";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_attributeWeight = L"weight";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_attributeDifferenceFolder = L"diff_url";
const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_tagDescription = L"Description";

const KLUPD::NoCaseString KLUPD::SitesFileXMLParser::s_sectionUpdatePeriod = L"UpdatePeriod";


KLUPD::SitesFileXMLParser::SitesFileXMLParser(std::vector<SiteInfo> &sites,
                                              SiteInfo::Description& description,
                                              UpdateSchedule &automaticScheduleForUpdate,
                                              const bool oldFormat,
                                              Log* Log)
 : m_skipSectionLevel(0),
   m_sites(sites),
   m_description(description),
   m_automaticScheduleForUpdate(automaticScheduleForUpdate),
   m_oldFormat(oldFormat),
   pLog(Log),
   m_strRootSectionName(s_rootSection)
{
}

bool KLUPD::SitesFileXMLParser::Parse(XmlReader& xmlReader)
{
    return parseSiteXMLRecursively(xmlReader, 0);
}

bool KLUPD::SitesFileXMLParser::parseSiteXMLRecursively(XmlReader &xmlReader, int guardLevel)
{
    if(++guardLevel > 20)
    {
        TRACE_MESSAGE("Parse error: too deep sites XML structure");
        return false;
    }

    try
    {
        XmlElement elt;
        int tagParseResult = LAX::XML_OK;
        while((tagParseResult = xmlReader.readElement(elt, 0, 0)) == LAX::XML_OK)
        {
            std::string tag;
            elt.getTagName(tag);
            XmlAttrMap attrs;
            elt.readAttrs(attrs);

            onStartElement(asciiToWideChar(tag), attrs);
            if(elt.hasBody() && !parseSiteXMLRecursively(xmlReader, guardLevel))
                return false;

            onEndElement();
            tagParseResult = xmlReader.endElement(elt);
            if(tagParseResult != LAX::XML_OK)
                break;
        }
        if(tagParseResult < 0)
        {
            TRACE_MESSAGE2("Sites XML file parse error: '%d'", tagParseResult);
            return false;
        }
        return true;
    }
    catch(LAX::XmlException &xmlException)
    {
        TRACE_MESSAGE2("XML exception while parsing sites: '%s'", xmlException.what());
    }
    catch(const std::exception &error)
    {
        TRACE_MESSAGE2("XML exception while parsing sites: '%s'", error.what());
    }
    return false;
}


void KLUPD::SitesFileXMLParser::onStartElement(const NoCaseString &name, const XmlAttrMap &attributes)
{
    if(!m_currentSectionName.empty() && m_currentSectionName != name)
    {
        m_sectionHistory.push(m_currentSectionName);
        m_currentSectionName = name;
    }
    else if(m_currentSectionName.empty())
        m_currentSectionName = name;

    if(m_skipSectionLevel)
    {
        // skip unknown section
        ++m_skipSectionLevel;
    }
    else if(m_strRootSectionName == name)
    {
        // return and get all inner information when called again
    }
    else if(s_sectionSites == name)
    {
        // return and get all inner information when called again
    }
    else if(s_tagDescription == name)
    {
        parseDescription(attributes);
    }
    else if(name == (m_oldFormat ? s_tagSourceObsolete : s_tagSource))
        parseSite(attributes);
    else if(name == s_sectionUpdatePeriod)
        m_automaticScheduleForUpdate.fromXML(attributes);
    else
    {
		// skip unknown section
        ++m_skipSectionLevel;
    }
}

void KLUPD::SitesFileXMLParser::onEndElement()
{
    if(!m_sectionHistory.empty())
    {
        m_currentSectionName = m_sectionHistory.top();
        m_sectionHistory.pop();

        // Decrease unknown sections level counter
        if(m_skipSectionLevel)
            --m_skipSectionLevel;
    }
}

void KLUPD::SitesFileXMLParser::parseDescription(const XmlAttrMap &attributes)
{
    m_description.clear();
    for(XmlAttrMap::AttrBag::const_iterator attributesIter = attributes.m_attrs.begin(); attributesIter != attributes.m_attrs.end(); ++attributesIter)
    {
        NoCaseString strName = asciiToWideChar(attributesIter->name);
        NoCaseString strValue = asciiToWideChar(attributesIter->value);
        if( !strName.empty() )
            m_description.push_back( std::make_pair(strName, strValue) );
    }
}

void KLUPD::SitesFileXMLParser::parseSite(const XmlAttrMap &attributes)
{
    SiteInfo site;
    for(XmlAttrMap::AttrBag::const_iterator attributesIter = attributes.m_attrs.begin(); attributesIter != attributes.m_attrs.end(); ++attributesIter)
    {
        if(s_attributeAddress == asciiToWideChar(attributesIter->name))
            site.m_address.fromAscii(attributesIter->value);
        else if(s_attributeRegion == asciiToWideChar(attributesIter->name))
        {
            site.m_region.fromAscii(attributesIter->value);
            SiteInfo().splitRegionList(site.m_region, site.m_regions);
        }
        else if(s_attributeWeight == asciiToWideChar(attributesIter->name))
        {
            if(!NoCaseString(asciiToWideChar(attributesIter->value)).toDouble(site.m_weight))
                site.m_weight = 0;
        }
        else if(s_attributeDifferenceFolder == asciiToWideChar(attributesIter->name))
            site.m_differenceFolder.fromAscii(attributesIter->value);
        else
        {
            TRACE_MESSAGE3("Unexpected tag while parsed '%S' tag for site '%S'",
                m_oldFormat ? s_tagSourceObsolete.toWideChar() : s_tagSource.toWideChar(), site.toString().toWideChar());
        }
    }

    // Slightly validate parameters
    if(site.m_address.empty())
        throw (std::invalid_argument("No site URL specified while parsing site information"));

    if(!site.m_weight)
    {
        TRACE_MESSAGE2("No (or zero) site weight specified for site %S", site.toString().toWideChar());
        site.m_weight = 1;
    }

    TRACE_MESSAGE2("\tSource site found: %S", site.toString().toWideChar());
    m_sites.push_back(site);
}


#ifdef _WIN32
#include "../core/updater.h"

#define DESC_COMPONETS_ATTR     "components"
#define DESC_TIMEOUT_ATTR       "timeout"

bool GetBestUrl(const char* p_pszXmlBuffer,
                int         p_nXmlBufferSize,
                const char* p_pszRegion,
                char*       p_pszResult,
                int         p_nResultSize,
                char*       p_pszComponents,
                int         p_nComponentsSize,
                char*       p_pszTimeout,
                int         p_nTimeoutSize)
{
    bool blResult = true ;

    try {

        std::vector<KLUPD::SiteInfo> sites ;
        KLUPD::SiteInfo::Description description;
        KLUPD::UpdateSchedule        automaticScheduleForUpdate ;
        KLUPD::SitesFileXMLParser xmler(sites, description, automaticScheduleForUpdate, false, 0) ;
        XmlReader xmlReader(p_pszXmlBuffer, p_nXmlBufferSize) ;
        if (xmler.Parse(xmlReader) && sites.size())
        {
            WCHAR wcsRegion[MAX_PATH] ;
            memset(wcsRegion, 0, sizeof(wcsRegion)) ;
            if (p_pszRegion && strlen(p_pszRegion))
            {
                ::MultiByteToWideChar(CP_ACP, 0, p_pszRegion, (int)strlen(p_pszRegion) + 1, wcsRegion, MAX_PATH) ;
            }

            KLUPD::NoCaseString strRegion(wcsRegion) ;
            if (strRegion.size() == 0)
            {
                strRegion = KLUPD::getPreferredRegion(0) ;
            }

            std::vector<KLUPD::Path> sortedSites ;
            sites[0].sortSites(sites, strRegion, sortedSites, 0) ;
            if (sortedSites.size() && p_nResultSize > (int)sortedSites[0].size())
            {
                memcpy(p_pszResult, sortedSites[0].toAscii().c_str(), sortedSites[0].size() + 1) ;

                if (p_pszComponents)
                {
                    p_pszComponents[0] = '\0' ;
                }

                if (p_pszTimeout)
                {
                    p_pszTimeout[0] = '\0' ;
                }

                for (int nIndex = 0, nCount = description.size(); nIndex < nCount; ++nIndex)
                {
                    if (stricmp(description[nIndex].first.toAscii().c_str(), DESC_COMPONETS_ATTR) == 0 &&
                        (int)description[nIndex].second.size() < p_nComponentsSize)
                    {
                        memcpy(p_pszComponents, description[nIndex].second.toAscii().c_str(), description[nIndex].second.size() + 1) ;
                    }
                    else if (stricmp(description[nIndex].first.toAscii().c_str(), DESC_TIMEOUT_ATTR) == 0 &&
                        (int)description[nIndex].second.size() < p_nTimeoutSize)
                    {
                        memcpy(p_pszTimeout, description[nIndex].second.toAscii().c_str(), description[nIndex].second.size() + 1) ;
                    }
                }
            }
            else
            {
                blResult = false ;
            }
        }
        else 
        {
            blResult = false ;
        }

    } catch (...) {

        blResult = false ;

    }

    return blResult ;
}
#endif // _WIN32

