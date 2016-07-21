// NewsReader.cpp: implementation of the CNewsReader class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NewsReader.h"

//////////////////////////////////////////////////////////////////////
// News xml file support
#include <LAX.h>
#include <LAXi.h>

// Xml tags
#define XML_TAG_NEWS		"News"
#define XML_TAG_ITEM		"Item"
#define XML_TAG_MESSAGE		"Message"
#define XML_TAG_TITLE		"Title"
#define XML_TAG_ANNOUNCE	"Announcement"
#define XML_TAG_TEXT		"Text"
#define XML_TAG_LINK		"Link"

// Xml attributes
#define XML_ATTR_ID					L"Id"
#define XML_ATTR_SEVERITY			L"Severity"
#define XML_ATTR_PRODUCT_TYPE		L"ProductType"
#define XML_ATTR_PRODUCT_STATUS		L"ProductStatus"
#define XML_ATTR_PRODUCT_VER_MIN	L"ProductVersionMin"
#define XML_ATTR_PRODUCT_VER_MAX	L"ProductVersionMax"
#define XML_ATTR_LOCAlIZATION		L"Localization"
#define XML_ATTR_URL				L"URL"

#define XML_SEVERITY_ALARM			L"Alarm"
#define XML_SEVERITY_WARNING		L"Warning"
#define XML_SEVERITY_INFORMATION	L"Information"

typedef wchar_t XmlChar ;
typedef std::wstring XmlString;
typedef LAX::UTF8_WCHAR_T<LAX::STD_STRING_ADAPTER<XmlChar, XmlString> > XmlTransform ;
typedef LAX::XmlAttrMap<XmlTransform> XmlAttrMap ;
typedef LAX::XmlException XmlException ;
typedef LAX::XmlReader<XmlTransform, LAX::STD_ATTR_MAP_ADAPTER<char, XmlAttrMap>, LAX::XmlUseExceptions<XmlException> > XmlReader ;
typedef XmlReader::Element XmlElement ;
typedef std::vector<XmlString> ComponentIDArray ;
typedef std::vector<XmlString> XmlAttrArray ;
//////////////////////////////////////////////////////////////////////

#define ENV_STR_PRODUCT_TYPE	"%ProductType%"
#define ENV_STR_PRODUCT_STATUS	"%ProductStatus%"
#define ENV_STR_LOCALIZATION	"%Localization%"

static bool ReadNewsMessage(XmlReader& xmlReader, cGuiNewsItem& newsItem)
{
	bool bCommonLocFound = false;
	bool bProductLocFound = false;
	for(;;)
	{
		XmlElement xmlMessage;
		XmlAttrMap xmlMessageAttr;
		XmlString strMessageLocalization;

		if( xmlReader.readElement(xmlMessage, 0) == LAX::XML_END )
			break;

		if( !xmlMessage.compareXmlTag(XML_TAG_MESSAGE) )
		{
			xmlReader.skipElementBody(xmlMessage);
			xmlReader.endElement(xmlMessage);
			continue ;
		}

		xmlMessage.readAttrs(xmlMessageAttr);
		for( size_t i=0; i<xmlMessageAttr.size(); ++i )
		{
			if( xmlMessageAttr[i].name == XML_ATTR_LOCAlIZATION )
			{
				strMessageLocalization = xmlMessageAttr[i].value;
			}
		}

		bool bSkipMessage = true;
		if( strMessageLocalization == g_hGui->ExpandEnvironmentString(ENV_STR_LOCALIZATION).data() )
		{
			bProductLocFound = true;
			bSkipMessage = false;
		}
		else if( strMessageLocalization.empty() && !bProductLocFound )
		{
			bCommonLocFound = true;
			bSkipMessage = false;
		}

		if( !bSkipMessage )
		{
			for(;;)
			{
				XmlElement xmlMessageField;
				if( xmlReader.readElement(xmlMessageField, 0) == LAX::XML_END )
					break;

				XmlReader::string_t strText;
				XmlReader::str_t body;
				xmlReader.readElementBody(xmlMessageField, body);
				LAX::Xml<XmlTransform>::unescape(body.c_str(), body.length(), strText);

				if( strText.size() )
				{
					if( xmlMessageField.compareXmlTag(XML_TAG_TITLE) )
						newsItem.m_strTitle.assign(strText.c_str(), cCP_UNICODE, strText.size() * sizeof(tWCHAR));
					else if( xmlMessageField.compareXmlTag(XML_TAG_ANNOUNCE) )
						newsItem.m_strAnnouncement.assign(strText.c_str(), cCP_UNICODE, strText.size() * sizeof(tWCHAR));
					else if( xmlMessageField.compareXmlTag(XML_TAG_TEXT) )
					{
						XmlReader::string_t::size_type ind = strText.find(L"<![CDATA[");
						if( ind != strText.npos )
						{
							// remove <![CDATA[ prefix from the begin
							strText.erase(0, ind+9);

							ind = strText.rfind(L"]]>");
							if( ind != strText.npos )
							{
								// remove ]]> from the end
								strText.erase(ind);
							}
							newsItem.m_strText.assign(strText.c_str(), cCP_UNICODE, strText.size() * sizeof(tWCHAR));
						}
					}
					else if( xmlMessageField.compareXmlTag(XML_TAG_LINK) )
					{
						newsItem.m_strLink.assign(strText.c_str(), cCP_UNICODE, strText.size() * sizeof(tWCHAR));
					}
				}

				xmlReader.endElement(xmlMessageField);
			}
		}
		else
			xmlReader.skipElementBody(xmlMessage);

		xmlReader.endElement(xmlMessage);
	}

	return ( bCommonLocFound || bProductLocFound );
}

static bool ReadNewsItem(XmlReader& xmlReader, XmlElement& xmlItem, cGuiNewsItem& newsItem)
{
	XmlAttrMap xmlItemAttr;
	XmlString strItemProductType, strItemProductStatus, strItemProductVerMin, strItemProductVerMax;

	xmlItem.readAttrs(xmlItemAttr);
	for( size_t i=0; i<xmlItemAttr.size(); ++i )
	{
		if( xmlItemAttr[i].name == XML_ATTR_ID )
			newsItem.m_strId .assign(xmlItemAttr[i].value.c_str(), cCP_UNICODE, xmlItemAttr[i].value.size() * sizeof(tWCHAR));
		else if( xmlItemAttr[i].name == XML_ATTR_SEVERITY )
		{
			if( xmlItemAttr[i].value == XML_SEVERITY_ALARM )
				newsItem.m_Severity = cGuiNewsItem::eAlarm;
			else if( xmlItemAttr[i].value == XML_SEVERITY_WARNING )
				newsItem.m_Severity = cGuiNewsItem::eWarning;
			else
				newsItem.m_Severity = cGuiNewsItem::eInformation;
		}
		else if( xmlItemAttr[i].name == XML_ATTR_PRODUCT_TYPE )
				strItemProductType = xmlItemAttr[i].value;
		else if( xmlItemAttr[i].name == XML_ATTR_PRODUCT_STATUS )
			strItemProductStatus = xmlItemAttr[i].value;
		else if( xmlItemAttr[i].name == XML_ATTR_PRODUCT_VER_MIN )
			strItemProductVerMin = xmlItemAttr[i].value;
		else if( xmlItemAttr[i].name == XML_ATTR_PRODUCT_VER_MAX )
			strItemProductVerMax = xmlItemAttr[i].value;
	}
	
	if( !strItemProductType.empty() &&
		strItemProductType.find(g_hGui->ExpandEnvironmentString(ENV_STR_PRODUCT_TYPE).data()) == strItemProductType.npos )
	{
		return false;
	}

	if( !strItemProductStatus.empty() &&
		strItemProductStatus.find(g_hGui->ExpandEnvironmentString(ENV_STR_PRODUCT_STATUS).data()) == strItemProductStatus.npos )
	{
		return false;
	}

	if( !strItemProductVerMin.empty() || !strItemProductVerMax.empty() )
	{
		struct CVer { tDWORD v1, v2, v3, v4; };

		CVer curver;
		swscanf(g_hGui->ExpandEnvironmentString("%ProductVersion%").data(), L"%d.%d.%d.%d", 
			&curver.v1, &curver.v2, &curver.v3, &curver.v4);

		if( !strItemProductVerMin.empty() )
		{
			CVer minver = { 0, 0, 0, 0 };
			swscanf(strItemProductVerMin.c_str(), L"%d.%d.%d.%d", 
				&minver.v1, &minver.v2, &minver.v3, &minver.v4);
			if (curver.v1 < minver.v1
				|| (curver.v1 == minver.v1
				&& (curver.v2 < minver.v2 
					|| curver.v2 == minver.v2 
					&& (curver.v3 < minver.v3 || curver.v3 == minver.v3 && curver.v4 < minver.v4))))
			{
				return false;
			}
		}

		if( !strItemProductVerMax.empty() )
		{
			CVer maxver = { 0, 0, 0, 0 };
			swscanf(strItemProductVerMax.c_str(), L"%d.%d.%d.%d", 
				&maxver.v1, &maxver.v2, &maxver.v3, &maxver.v4);
			if (curver.v1 > maxver.v1
				|| (curver.v1 == maxver.v1
				&& (curver.v2 > maxver.v2 
				|| curver.v2 == maxver.v2 
				&& (curver.v3 > maxver.v3 || curver.v3 == maxver.v3 && curver.v4 > maxver.v4))))
			{
				return false;
			}
		}
	}

	return ReadNewsMessage(xmlReader, newsItem);
}

static void ReadNews(XmlReader& xmlReader, cGuiNews& News)
{
	for(;;)
	{
		cGuiNewsItem newsItem;
		XmlElement xmlItem;

		if( xmlReader.readElement(xmlItem, 0) == LAX::XML_END )
			break;

		if( !xmlItem.compareXmlTag(XML_TAG_ITEM) )
		{
			xmlReader.skipElementBody(xmlItem);
		}
		else
		{
			if( ReadNewsItem(xmlReader, xmlItem, newsItem) )
				News.push_back(newsItem);
			else
				xmlReader.skipElementBody(xmlItem);
		}

		xmlReader.endElement(xmlItem);
	}
}

bool CNewsReader::ReadFromBuffer(const char *xml, size_t len, cGuiNews& News)
{
	XmlReader xmlReader(xml, len);
	try
	{
		XmlElement xmlRoot;
		xmlReader.readElement(xmlRoot, LAX::XML_F_ELT_REQUIRED | LAX::XML_F_SKIP_ATTRS | LAX::XML_F_CHECK_NAME, XML_TAG_NEWS);
		if( !xmlRoot.hasBody() )
		{
			PR_TRACE((g_hGui, prtIMPORTANT, "gui\tNews xml parse error: root tag is absent"));
			return false;
		}

		ReadNews(xmlReader, News);

		xmlReader.endElement(xmlRoot) ;
	}
	catch (XmlException exp)
	{
		PR_TRACE((g_hGui, prtIMPORTANT, "gui\tNews xml parse error: %s", exp.what()));
		return false;
	}
	catch(...)
	{
		PR_TRACE((g_hGui, prtIMPORTANT, "gui\tNews xml parse unexpected error"));
		return false;
	}

	return true;
}

bool CNewsReader::ReadFromFile(const cStringObj& sFileName, cGuiNews& News)
{
	cIOObj hIo((cObject *)g_root, cAutoString(sFileName), fACCESS_READ);
	if( PR_FAIL(hIo.last_error()) )
		return false;

	tQWORD nFileSize;
	if( PR_FAIL(hIo->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT)) )
		return false;

	cVector<char> strData;
	strData.resize((tDWORD)nFileSize);

	tDWORD size = (tDWORD)nFileSize;
	if( hIo->SeekRead(&size, 0, &strData[0], size) )
		return false;

	return ReadFromBuffer(&strData[0], strData.size(), News);
}
