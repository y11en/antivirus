#include "StdAfx.h"
#include ".\keyinfoassembler.h"
#include <algorithm>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <serialize/KLDTSer.h>

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

namespace
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////////////////
		MSXML2::IXMLDOMNodePtr SerializeValueToXmlNode(
			const MSXML2::IXMLDOMDocumentPtr& spXmlDoc,
			const _bstr_t& bstrName, 
			const _bstr_t& bstrValue, 
			bool bCDATA = false
			)
		{
			MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(bstrName);
			if (bCDATA)
				spNode->appendChild(spXmlDoc->createCDATASection(bstrValue));
			else
				spNode->appendChild(spXmlDoc->createTextNode(bstrValue));
			return spNode;
		}
		////////////////////////////////////////////////////////////////////////////
		MSXML2::IXMLDOMNodePtr SerializeValueToXmlNode(
			const MSXML2::IXMLDOMDocumentPtr& spXmlDoc,
			const _bstr_t& bstrName, 
			DWORD dwValue,
			bool bCDATA = false
			)
		{
			std::wstringstream ss;
			ss << dwValue;
			return SerializeValueToXmlNode(spXmlDoc, bstrName, _bstr_t(ss.str().c_str()), bCDATA);
		}
		////////////////////////////////////////////////////////////////////////////
		MSXML2::IXMLDOMNodePtr SerializeValueToXmlNode(
			const MSXML2::IXMLDOMDocumentPtr& spXmlDoc,
			const _bstr_t& bstrName, 
			const SerializableDataBase::string_t& sValue, 
			bool bCDATA = false
			)
		{
			return SerializeValueToXmlNode(spXmlDoc, bstrName, _bstr_t(sValue.c_str()), bCDATA);
		}
		////////////////////////////////////////////////////////////////////////////
		MSXML2::IXMLDOMNodePtr SerializeValueToXmlNode(
			const MSXML2::IXMLDOMDocumentPtr& spXmlDoc,
			const _bstr_t& bstrName, 
			const SerializableDataBase::date_t& dtValue,
			bool bCDATA = false
			)
		{
			std::stringstream ss;
			ss << static_cast<unsigned int>(dtValue.dateParts.year)
			<< '-' << std::setw(2) << std::setfill('0') 
					<< static_cast<unsigned int>(dtValue.dateParts.month)
			<< '-' << std::setw(2) << std::setfill('0') 
					<< static_cast<unsigned int>(dtValue.dateParts.day);
			return SerializeValueToXmlNode(spXmlDoc, bstrName, _bstr_t(ss.str().c_str()), bCDATA);
		}
		////////////////////////////////////////////////////////////////////////////
		MSXML2::IXMLDOMNodePtr SerializeValueToXmlNode(
			const MSXML2::IXMLDOMDocumentPtr& spXmlDoc,
			const _bstr_t& bstrName, 
			const SerializableDataBase::serialnumber_t& snValue,
			bool bCDATA = false
			)
		{
			std::stringstream ss;
			ss << std::setw(4) << std::setfill('0') << snValue.parts.cust_id << '-'
			<< std::setw(6) << std::setfill('0') << snValue.parts.app_id << '-'
			<< std::setw(8) << std::setfill('0') << snValue.parts.key_num;
			return SerializeValueToXmlNode(spXmlDoc, bstrName, _bstr_t(ss.str().c_str()), bCDATA);
		}
		////////////////////////////////////////////////////////////////////////////
		template <typename T>
		inline MSXML2::IXMLDOMNodePtr SerializeValueToXmlNode(
			const MSXML2::IXMLDOMDocumentPtr& spXmlDoc,
			const _bstr_t& bstrName, 
			const value_wrapper<T>& value,
			bool bCDATA = false
			)
		{
			return detail::SerializeValueToXmlNode(spXmlDoc, bstrName, value.ref(), bCDATA);
		}
	}
	////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void SaveValueToXmlNode(
		const MSXML2::IXMLDOMNodePtr& spNode,
		const _bstr_t& bstrName, 
		const value_wrapper<T>& value,
		bool bCDATA = false
		)
	{
		if (value.exists())
			spNode->appendChild(detail::SerializeValueToXmlNode(spNode->ownerDocument, bstrName, value, bCDATA));
	}
	////////////////////////////////////////////////////////////////////////////
	MSXML2::IXMLDOMNodePtr RetrieveXmlNode(
		const MSXML2::IXMLDOMNodePtr& spParentNode, 
		const _bstr_t& bstrValueNodeName
		)
	{
		MSXML2::IXMLDOMNodePtr spResult;
		MSXML2::IXMLDOMNodeListPtr spChildrenList = spParentNode->childNodes;
		for (int i = 0; i < spChildrenList->length; ++i)
		{
			MSXML2::IXMLDOMNodePtr spChildNode = spChildrenList->item[i];
			if (spChildNode != NULL && spChildNode->nodeName == bstrValueNodeName)
			{
				spResult = spChildNode;
				break;
			}
		}
		return spResult;
	}
	namespace detail
	{
		////////////////////////////////////////////////////////////////////////////
		bool RetrieveXmlNodeValue(
			const MSXML2::IXMLDOMNodePtr& spNode, 
			SerializableDataBase& dest
			)
		{
			dest.DeserializeFromXmlNode(spNode);
			return true;
		}
		////////////////////////////////////////////////////////////////////////////
		bool RetrieveXmlNodeValue(
			const MSXML2::IXMLDOMNodePtr& spNode, 
			_bstr_t& bstrValue
			)
		{
			if (spNode != NULL && spNode->childNodes->length == 1)
			{
				MSXML2::IXMLDOMNodePtr spText = spNode->childNodes->item[0];
				if (spText->nodeType == MSXML2::NODE_TEXT || spText->nodeType == MSXML2::NODE_CDATA_SECTION)
				{
					bstrValue = spText->nodeValue; 
					return true;
				}
			}
			return false;
		}
		////////////////////////////////////////////////////////////////////////////
		bool RetrieveXmlNodeValue(
			const MSXML2::IXMLDOMNodePtr& spNode, 
			std::string& sValue
			)
		{
			_bstr_t bstrValue;
			if (RetrieveXmlNodeValue(spNode, bstrValue))
			{
				sValue = static_cast<const char*>(bstrValue);
				return true;
			}
			else
			{
				return false;
			}
		}
		////////////////////////////////////////////////////////////////////////////
		bool RetrieveXmlNodeValue(
			const MSXML2::IXMLDOMNodePtr& spNode, 
			DWORD& dwValue
			)
		{
			_bstr_t bstrValue;
			if (RetrieveXmlNodeValue(spNode, bstrValue))
			{
				dwValue = _wtol(static_cast<const wchar_t*>(bstrValue));
				return true;
			}
			else
			{
				return false;
			}
		}
		////////////////////////////////////////////////////////////////////////////
		bool RetrieveXmlNodeValue(
			const MSXML2::IXMLDOMNodePtr& spNode, 
			SerializableDataBase::date_t& dValue
			)
		{
			_bstr_t bstrValue;
			if (RetrieveXmlNodeValue(spNode, bstrValue))
			{
				if (bstrValue.length() == 10)
				{
					const wchar_t* pwszValue = static_cast<const wchar_t*>(bstrValue);
					if (pwszValue[4] == L'-' && pwszValue[7] == L'-')
					{
						struct tm tmValue;
						ZeroMemory(&tmValue, sizeof(tmValue));
						tmValue.tm_year = _wtol(std::wstring(pwszValue, pwszValue + 4).c_str()) - 1900;
						tmValue.tm_mon = _wtol(std::wstring(pwszValue + 5, pwszValue + 7).c_str()) - 1;
						tmValue.tm_mday = _wtol(std::wstring(pwszValue + 8, pwszValue + 10).c_str());
						mktime(&tmValue);
						dValue.dateParts.year		= tmValue.tm_year + 1900;
						dValue.dateParts.month		= tmValue.tm_mon + 1;
						dValue.dateParts.dayOfWeek	= tmValue.tm_wday;
						dValue.dateParts.day		= tmValue.tm_mday;
						return true;
					}
				}
			}
			return false;
		}
		////////////////////////////////////////////////////////////////////////////
		bool RetrieveXmlNodeValue(
			const MSXML2::IXMLDOMNodePtr& spNode, 
			SerializableDataBase::serialnumber_t& snValue
			)
		{
			_bstr_t bstrValue;
			if (RetrieveXmlNodeValue(spNode, bstrValue))
			{
				if (bstrValue.length() == 20)
				{
					const wchar_t* pwszValue = static_cast<const wchar_t*>(bstrValue);
					if (pwszValue[4] == L'-' && pwszValue[11] == L'-')
					{
						snValue.parts.cust_id = _wtol(std::wstring(pwszValue, pwszValue + 4).c_str());
						snValue.parts.app_id  = _wtol(std::wstring(pwszValue + 5, pwszValue + 11).c_str());
						snValue.parts.key_num = _wtol(std::wstring(pwszValue + 12, pwszValue + 20).c_str());
						return true;
					}
				}
			}
			return false;
		}
	}
	////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void RetrieveXmlNodeValue(
		const MSXML2::IXMLDOMNodePtr& spNode, 
		value_wrapper<T>& dest
		)
	{
		detail::RetrieveXmlNodeValue(spNode, dest.ref());
		dest.exists(true);
	}
	////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void RetrieveXmlNodeValue(
		const MSXML2::IXMLDOMNodePtr& spParentNode, 
		const _bstr_t& bstrValueNodeName,
		value_wrapper<T>& dest
		)
	{
		MSXML2::IXMLDOMNodePtr spChildNode = RetrieveXmlNode(spParentNode, bstrValueNodeName);
		if (spChildNode != NULL)
		{
			RetrieveXmlNodeValue(spChildNode, dest);
		}
		else
		{
			dest.zero_value();
			dest.exists(false);
		}
	}
	////////////////////////////////////////////////////////////////////////////
	inline HDATA DT_AddData(HDATA hData, AVP_dword pid)
	{
		return ::DATA_Add(hData, 0, pid, 0, 0);
	}
	////////////////////////////////////////////////////////////////////////////
	inline HDATA DT_AddData(HDATA hData, HDATA hInsert)
	{
		return ::DATA_Attach(hData, 0, hInsert, DATA_IF_ROOT_INCLUDE);
	}
	namespace detail
	{
		////////////////////////////////////////////////////////////////////////////
		inline HDATA DT_AddData(HDATA hData, AVP_dword pid, AVP_dword dwValue)
		{
			return ::DATA_Add(hData, 0, pid, dwValue, 0);
		}
		////////////////////////////////////////////////////////////////////////////
		inline HDATA DT_AddData(HDATA hData, AVP_dword pid, const SerializableDataBase::string_t& sValue)
		{
			return ::DATA_Add(hData, 0, pid, reinterpret_cast<AVP_dword>(const_cast<char*>(sValue.c_str())), 0);
		}
		////////////////////////////////////////////////////////////////////////////
		inline HDATA DT_AddData(HDATA hData, AVP_dword pid, const SerializableDataBase::date_t& dtValue)
		{
			return ::DATA_Add(hData, 0, pid, reinterpret_cast<AVP_dword>(&dtValue), sizeof(SerializableDataBase::date_t));
		}
		////////////////////////////////////////////////////////////////////////////
		inline HDATA DT_AddData(HDATA hData, AVP_dword pid, const SerializableDataBase::serialnumber_t& snValue)
		{
			return ::DATA_Add(hData, 0, pid, reinterpret_cast<AVP_dword>(&snValue), sizeof(SerializableDataBase::serialnumber_t));
		}
	}
	////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline HDATA DT_AddData(HDATA hData, AVP_dword pid, const value_wrapper<T>& value)
	{
		return (value.exists() ? detail::DT_AddData(hData, pid, value.ref()) : NULL);
	}
	////////////////////////////////////////////////////////////////////////////
	inline HDATA DT_GetChildNode(HDATA hData, AVP_dword pid)
	{
		MAKE_ADDR1(addr, pid);
		return ::DATA_Find(hData, addr);
	}
	////////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		////////////////////////////////////////////////////////////////////////////
		inline bool DT_GetData(HDATA hData, AVP_dword pid, AVP_dword& dwValue)
		{
			HDATA hValue = DT_GetChildNode(hData, pid);
			return (hValue && (::DATA_Get_Val(hValue, 0, 0, &dwValue, sizeof(AVP_dword)) != 0));
		}
		////////////////////////////////////////////////////////////////////////////
		bool DT_GetData(HDATA hData, AVP_dword pid, SerializableDataBase::date_t& dValue)
		{
			HDATA hValue = DT_GetChildNode(hData, pid);
			return (hValue && (::DATA_Get_Val(hValue, 0, 0, &(dValue.date), sizeof(dValue.date)) != 0));
		}
		////////////////////////////////////////////////////////////////////////////
		bool DT_GetData(HDATA hData, AVP_dword pid, SerializableDataBase::serialnumber_t& snValue)
		{
			HDATA hValue = DT_GetChildNode(hData, pid);
			return (hValue && (::DATA_Get_Val(hValue, 0, 0, &(snValue.sn), sizeof(snValue.sn)) != 0));
		}
		////////////////////////////////////////////////////////////////////////////
		bool DT_GetData(HDATA hData, AVP_dword pid, SerializableDataBase::string_t& sValue)
		{
			bool result = false;
			HDATA hValue = DT_GetChildNode(hData, pid);
			if (hValue)
			{
				AVP_dword size = ::DATA_Get_Val(hValue, 0, 0, 0, 0);
				if (size > 0)
				{
					std::vector<char> buf(size, 0);
					result = (::DATA_Get_Val(hValue, 0, 0, &buf[0], size) != 0);
					sValue.assign(buf.begin(), buf.end());
				}
			}
			return result;
		}
		////////////////////////////////////////////////////////////////////////////
		bool DT_GetData(HDATA hData, AVP_dword pid, SerializableDataBase& obj)
		{
			HDATA hChild = DT_GetChildNode(hData, pid);
			if (hChild)
			{
				obj.DeserializeFromPropertyNode(hChild);
				return true;
			}
			else
			{
				return false;
			}
		}
	}	
	////////////////////////////////////////////////////////////////////////////
	template <typename T>
	inline void DT_GetData(HDATA hData, AVP_dword pid, value_wrapper<T>& value)
	{
		value.exists(detail::DT_GetData(hData, pid, value.ref()));
	}
}

////////////////////////////////////////////////////////////////////////////
//class SerializableDataBase

////////////////////////////////////////////////////////////////////////////
void KeyProductInfo::Clear()
{
	m_nMarketSector = m_nSalesChannel = m_nLocalization = m_nPackage = 0;
}

////////////////////////////////////////////////////////////////////////////
void KeyProductInfo::InitializeTemplate()
{
	m_nMarketSector = m_nSalesChannel = m_nLocalization = m_nPackage = 1;
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyProductInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"MarketSector", m_nMarketSector);
	SaveValueToXmlNode(spNode, L"SalesChannel", m_nSalesChannel);
	SaveValueToXmlNode(spNode, L"Localization", m_nLocalization);
	SaveValueToXmlNode(spNode, L"Package", m_nPackage);
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyProductInfo::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"MarketSector", m_nMarketSector);
	RetrieveXmlNodeValue(spXmlNode, L"SalesChannel", m_nSalesChannel);
	RetrieveXmlNodeValue(spXmlNode, L"Localization", m_nLocalization);
	RetrieveXmlNodeValue(spXmlNode, L"Package", m_nPackage);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyProductInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEY_PRODUCT_MARKETSECTOR, m_nMarketSector);
	DT_AddData(hData, AVP_PID_KEY_PRODUCT_SALESCHANNEL, m_nSalesChannel);
	DT_AddData(hData, AVP_PID_KEY_PRODUCT_LOCALIZATION, m_nLocalization);
	DT_AddData(hData, AVP_PID_KEY_PACKAGE, m_nPackage);
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyProductInfo::DeserializeFromPropertyNode(HDATA hData)
{
	DT_GetData(hData, AVP_PID_KEY_PRODUCT_MARKETSECTOR, m_nMarketSector);
	DT_GetData(hData, AVP_PID_KEY_PRODUCT_SALESCHANNEL, m_nSalesChannel);
	DT_GetData(hData, AVP_PID_KEY_PRODUCT_LOCALIZATION, m_nLocalization);
	DT_GetData(hData, AVP_PID_KEY_PACKAGE, m_nPackage);
}

////////////////////////////////////////////////////////////////////////////
void KeyContactInfo::Clear()
{
	m_sName.ref().swap(string_t());
	m_sCountry.ref().swap(string_t());
	m_sCity.ref().swap(string_t());
	m_sAddress.ref().swap(string_t());
	m_sPhone.ref().swap(string_t());
	m_sFax.ref().swap(string_t());
	m_sEmail.ref().swap(string_t());
	m_sWWW.ref().swap(string_t());
}

////////////////////////////////////////////////////////////////////////////
void KeyContactInfo::InitializeTemplate()
{
	m_sName		= "Name";
	m_sCountry	= "Country";
	m_sCity		= "City";
	m_sAddress	= "Address";
	m_sPhone	= "Phone";
	m_sFax		= "Fax";
	m_sEmail	= "Email";
	m_sWWW		= "WWW";
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyContactInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"Name", m_sName);
	SaveValueToXmlNode(spNode, L"Country", m_sCountry);
	SaveValueToXmlNode(spNode, L"City", m_sCity);
	SaveValueToXmlNode(spNode, L"Address", m_sAddress);
	SaveValueToXmlNode(spNode, L"Phone", m_sPhone);
	SaveValueToXmlNode(spNode, L"Fax", m_sFax);
	SaveValueToXmlNode(spNode, L"Email", m_sEmail);
	SaveValueToXmlNode(spNode, L"WWW", m_sWWW);
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyContactInfo::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"Name", m_sName);
	RetrieveXmlNodeValue(spXmlNode, L"Country", m_sCountry);
	RetrieveXmlNodeValue(spXmlNode, L"City", m_sCity);
	RetrieveXmlNodeValue(spXmlNode, L"Address", m_sAddress);
	RetrieveXmlNodeValue(spXmlNode, L"Phone", m_sPhone);
	RetrieveXmlNodeValue(spXmlNode, L"Fax", m_sFax);
	RetrieveXmlNodeValue(spXmlNode, L"Email", m_sEmail);
	RetrieveXmlNodeValue(spXmlNode, L"WWW", m_sWWW);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyContactInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_NAME, m_sName);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_COUNTRY, m_sCountry);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_CITY, m_sCity);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_ADDRESS, m_sAddress);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_PHONE, m_sPhone);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_FAX, m_sFax);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_EMAIL, m_sEmail);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_WWW, m_sWWW);
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyContactInfo::DeserializeFromPropertyNode(HDATA hData)
{
	DT_GetData(hData, AVP_PID_KEY_CONTACT_NAME, m_sName);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_COUNTRY, m_sCountry);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_CITY, m_sCity);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_ADDRESS, m_sAddress);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_PHONE, m_sPhone);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_FAX, m_sFax);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_EMAIL, m_sEmail);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_WWW, m_sWWW);
}

////////////////////////////////////////////////////////////////////////////
void KeySupportInfo::Clear()
{
	m_sPhone.ref().swap(string_t());
	m_sFax.ref().swap(string_t());
	m_sEmail.ref().swap(string_t());
	m_sWWW.ref().swap(string_t());
}

////////////////////////////////////////////////////////////////////////////
void KeySupportInfo::InitializeTemplate()
{
	m_sPhone	= "Phone";
	m_sFax		= "Fax";
	m_sEmail	= "Email";
	m_sWWW		= "WWW";
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeySupportInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"Phone", m_sPhone);
	SaveValueToXmlNode(spNode, L"Fax", m_sFax);
	SaveValueToXmlNode(spNode, L"Email", m_sEmail);
	SaveValueToXmlNode(spNode, L"WWW", m_sWWW);
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeySupportInfo::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"Phone", m_sPhone);
	RetrieveXmlNodeValue(spXmlNode, L"Fax", m_sFax);
	RetrieveXmlNodeValue(spXmlNode, L"Email", m_sEmail);
	RetrieveXmlNodeValue(spXmlNode, L"WWW", m_sWWW);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeySupportInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_PHONE, m_sPhone);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_FAX, m_sFax);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_EMAIL, m_sEmail);
	DT_AddData(hData, AVP_PID_KEY_CONTACT_WWW, m_sWWW);
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeySupportInfo::DeserializeFromPropertyNode(HDATA hData)
{
	DT_GetData(hData, AVP_PID_KEY_CONTACT_PHONE, m_sPhone);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_FAX, m_sFax);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_EMAIL, m_sEmail);
	DT_GetData(hData, AVP_PID_KEY_CONTACT_WWW, m_sWWW);
}

////////////////////////////////////////////////////////////////////////////
void KeyCompanyInfo::Clear()
{
	m_ContactInfo.ref().Clear();
	m_SupportInfo.ref().Clear();
}

////////////////////////////////////////////////////////////////////////////
void KeyCompanyInfo::InitializeTemplate()
{
	m_ContactInfo.ref().InitializeTemplate();
	m_SupportInfo.ref().InitializeTemplate();
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyCompanyInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	spNode->appendChild(m_ContactInfo.ref().SerializeToXmlNode(spXmlDoc));
	spNode->appendChild(m_SupportInfo.ref().SerializeToXmlNode(spXmlDoc));
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyCompanyInfo::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"ContactInfo", m_ContactInfo);
	RetrieveXmlNodeValue(spXmlNode, L"SupportInfo", m_SupportInfo);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyCompanyInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, m_ContactInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, m_SupportInfo.ref().SerializeToPropertyNode());
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyCompanyInfo::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	DT_GetData(hData, m_ContactInfo.ref().GetDataTreePID(), m_ContactInfo);
	DT_GetData(hData, m_SupportInfo.ref().GetDataTreePID(), m_SupportInfo);
}

////////////////////////////////////////////////////////////////////////////
void KeyCustomerInfo::Clear()
{
	m_sCompanyName.ref().swap(string_t());
	m_sName.ref().swap(string_t());
	m_sCountry.ref().swap(string_t());
	m_sCity.ref().swap(string_t());
	m_sAddress.ref().swap(string_t());
	m_sPhone.ref().swap(string_t());
	m_sFax.ref().swap(string_t());
	m_sEmail.ref().swap(string_t());
	m_sWWW.ref().swap(string_t());
}

////////////////////////////////////////////////////////////////////////////
void KeyCustomerInfo::InitializeTemplate()
{
	m_sCompanyName = "CompanyName";
	m_sName		= "Name";
	m_sCountry	= "Country";
	m_sCity		= "City";
	m_sAddress	= "Address";
	m_sPhone	= "Phone";
	m_sFax		= "Fax";
	m_sEmail	= "Email";
	m_sWWW		= "WWW";
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyCustomerInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"CompanyName", m_sCompanyName);
	SaveValueToXmlNode(spNode, L"Name", m_sName);
	SaveValueToXmlNode(spNode, L"Country", m_sCountry);
	SaveValueToXmlNode(spNode, L"City", m_sCity);
	SaveValueToXmlNode(spNode, L"Address", m_sAddress);
	SaveValueToXmlNode(spNode, L"Phone", m_sPhone);
	SaveValueToXmlNode(spNode, L"Fax", m_sFax);
	SaveValueToXmlNode(spNode, L"Email", m_sEmail);
	SaveValueToXmlNode(spNode, L"WWW", m_sWWW);
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyCustomerInfo::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"CompanyName", m_sCompanyName);
	RetrieveXmlNodeValue(spXmlNode, L"Name", m_sName);
	RetrieveXmlNodeValue(spXmlNode, L"Country", m_sCountry);
	RetrieveXmlNodeValue(spXmlNode, L"City", m_sCity);
	RetrieveXmlNodeValue(spXmlNode, L"Address", m_sAddress);
	RetrieveXmlNodeValue(spXmlNode, L"Phone", m_sPhone);
	RetrieveXmlNodeValue(spXmlNode, L"Fax", m_sFax);
	RetrieveXmlNodeValue(spXmlNode, L"Email", m_sEmail);
	RetrieveXmlNodeValue(spXmlNode, L"WWW", m_sWWW);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyCustomerInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_COMPANYNAME, m_sCompanyName);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_NAME, m_sName);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_COUNTRY, m_sCountry);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_CITY, m_sCity);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_ADDRESS, m_sAddress);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_PHONE, m_sPhone);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_FAX, m_sFax);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_EMAIL, m_sEmail);
	DT_AddData(hData, AVP_PID_KEYCUSTOMER_WWW, m_sWWW);
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyCustomerInfo::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_COMPANYNAME, m_sCompanyName);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_NAME, m_sName);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_COUNTRY, m_sCountry);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_CITY, m_sCity);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_ADDRESS, m_sAddress);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_PHONE, m_sPhone);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_FAX, m_sFax);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_EMAIL, m_sEmail);
	DT_GetData(hData, AVP_PID_KEYCUSTOMER_WWW, m_sWWW);
}

////////////////////////////////////////////////////////////////////////////
void KeyCondition::Clear()
{
	m_sExpression.ref().swap(string_t());
	m_nNotify = 0;
}

////////////////////////////////////////////////////////////////////////////
void KeyCondition::InitializeTemplate()
{
	m_sExpression	= "1";
	m_nNotify		= 1;
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyCondition::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"Expression", m_sExpression, true);
	SaveValueToXmlNode(spNode, L"Notify", m_nNotify);
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyCondition::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"Expression", m_sExpression);
	RetrieveXmlNodeValue(spXmlNode, L"Notify", m_nNotify);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyCondition::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEY_CONDITION_EXPRESSION, m_sExpression);
	DT_AddData(hData, AVP_PID_KEY_CONDITION_NOTIFY, m_nNotify);
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyCondition::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	DT_GetData(hData, AVP_PID_KEY_CONDITION_EXPRESSION, m_sExpression);
	DT_GetData(hData, AVP_PID_KEY_CONDITION_NOTIFY, m_nNotify);
}

////////////////////////////////////////////////////////////////////////////
void KeyRestriction::Clear()
{
	m_sName.ref().swap(string_t());
	m_nValue = 0;
	m_sAdditiveCondition.ref().swap(string_t());
	m_vecConditions.clear();
	m_nDefaultNotify = 0;
}

////////////////////////////////////////////////////////////////////////////
void KeyRestriction::InitializeTemplate()
{
	m_sName = "Name";
	m_nValue = 1;
	m_sAdditiveCondition = "1";
	KeyConditionPtr pCond1(new KeyCondition(1)), pCond2(new KeyCondition(2));
	pCond1->InitializeTemplate();
	pCond2->InitializeTemplate();
	m_vecConditions.clear();
	m_vecConditions.push_back(pCond1);
	m_vecConditions.push_back(pCond2);
	m_nDefaultNotify = 1;
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyRestriction::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"Id", m_nId);
	SaveValueToXmlNode(spNode, L"Name", m_sName);
	SaveValueToXmlNode(spNode, L"Value", m_nValue);
	SaveValueToXmlNode(spNode, L"AdditiveCondition", m_sAdditiveCondition, true);
	SaveValueToXmlNode(spNode, L"DefaultNotify", m_nDefaultNotify);
	for (KeyConditionVec::const_iterator it = m_vecConditions.begin();
			it != m_vecConditions.end();
			++it)
	{
		spNode->appendChild((*it)->SerializeToXmlNode(spXmlDoc));
	}
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyRestriction::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"Id", m_nId);
	RetrieveXmlNodeValue(spXmlNode, L"Name", m_sName);
	RetrieveXmlNodeValue(spXmlNode, L"Value", m_nValue);
	RetrieveXmlNodeValue(spXmlNode, L"AdditiveCondition", m_sAdditiveCondition);
	RetrieveXmlNodeValue(spXmlNode, L"DefaultNotify", m_nDefaultNotify);
	{
		MSXML2::IXMLDOMNodeListPtr spChildrenList = spXmlNode->childNodes;
		int n = 0;
		for (int i = 0; i < spChildrenList->length; ++i)
		{
			MSXML2::IXMLDOMNodePtr spChildNode = spChildrenList->item[i];
			if (spChildNode != NULL && spChildNode->nodeName == _bstr_t(L"Condition"))
			{
				KeyConditionPtr pCond(new KeyCondition(n++));
				pCond->DeserializeFromXmlNode(spChildNode);
				m_vecConditions.push_back(pCond);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyRestriction::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEY_RESTRICTION_NAME, m_sName);
	DT_AddData(hData, AVP_PID_KEY_RESTRICTION_VALUE, m_nValue);
	DT_AddData(hData, AVP_PID_KEY_ADDITIVE_CONDITION, m_sAdditiveCondition);
	DT_AddData(hData, AVP_PID_KEY_RESTRICTION_DEFAULT_NOTIFY, m_nDefaultNotify);
	HDATA hCondList = DT_AddData(hData, AVP_PID_KEY_CONDITION_LIST);
	for (KeyConditionVec::const_iterator it = m_vecConditions.begin();
			it != m_vecConditions.end();
			++it)
	{
		DT_AddData(hCondList, (*it)->SerializeToPropertyNode());
	}
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyRestriction::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	DT_GetData(hData, AVP_PID_KEY_RESTRICTION_NAME, m_sName);
	DT_GetData(hData, AVP_PID_KEY_RESTRICTION_VALUE, m_nValue);
	DT_GetData(hData, AVP_PID_KEY_ADDITIVE_CONDITION, m_sAdditiveCondition);
	DT_GetData(hData, AVP_PID_KEY_RESTRICTION_DEFAULT_NOTIFY, m_nDefaultNotify);
	HDATA hCondList = DT_GetChildNode(hData, AVP_PID_KEY_CONDITION_LIST);
	if (hCondList)
	{
		HDATA hCurrent = ::DATA_Get_First(hCondList, 0);
		while (hCurrent)
		{
			KeyConditionPtr spCond(new KeyCondition(GET_AVP_PID_ID(::DATA_Get_Id(hCurrent, 0))));
			spCond->DeserializeFromPropertyNode(hCurrent);
			m_vecConditions.push_back(spCond);
			hCurrent = ::DATA_Get_Next(hCurrent, 0);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
LicenseObjectId::Type KeyLicenseObjectInfo::ConvertType(const string_t& type)
{
	if (type == "product")
		return LicenseObjectId::OBJ_TYPE_PRODUCT;
	else if (type == "application")
		return LicenseObjectId::OBJ_TYPE_APPLICATION;
	else if (type == "component")
		return LicenseObjectId::OBJ_TYPE_COMPONENT;
	else
		return LicenseObjectId::OBJ_TYPE_COMPONENT;
}

////////////////////////////////////////////////////////////////////////////
SerializableDataBase::string_t KeyLicenseObjectInfo::ConvertType(LicenseObjectId::Type type)
{
	switch (type)
	{
		case LicenseObjectId::OBJ_TYPE_PRODUCT:
			return "product";
		case LicenseObjectId::OBJ_TYPE_APPLICATION:
			return "application";
		case LicenseObjectId::OBJ_TYPE_COMPONENT:
			return "component";
		default:
			return "";
	}
}

////////////////////////////////////////////////////////////////////////////
void KeyLicenseObjectInfo::Clear()
{
	m_nId = 0;
	m_enumType = LicenseObjectId::OBJ_TYPE_PRODUCT;
	m_sName.ref().swap(string_t());
	m_nVersion = 0;
	m_vecPlatforms.clear();
	m_vecRestrictions.clear();
	m_vecChildrenLO.clear();
}

////////////////////////////////////////////////////////////////////////////
void KeyLicenseObjectInfo::InitializeTemplate()
{
	m_nId		= 1;
	m_enumType	= LicenseObjectId::OBJ_TYPE_PRODUCT;
	m_sName		= "Name";
	m_nVersion	= 1;

	m_vecPlatforms.clear();
	m_vecPlatforms.push_back(PT_UNKNOWN);
	m_vecPlatforms.push_back(PT_UNIX_FAMILY);
	m_vecPlatforms.push_back(PT_WIN_PERSONAL);
	m_vecPlatforms.push_back(PT_WIN_WORKSTATION);
	m_vecPlatforms.push_back(PT_WIN_SERVER_SMB);
	m_vecPlatforms.push_back(PT_WIN_SERVER_ENTERPRISE);

	KeyRestrictionPtr pRestr1(new KeyRestriction(1)), pRestr2(new KeyRestriction(2));
	pRestr1->InitializeTemplate();
	pRestr2->InitializeTemplate();
	m_vecRestrictions.clear();
	m_vecRestrictions.push_back(pRestr1);
	m_vecRestrictions.push_back(pRestr2);
	
//	KeyLicenseObjectInfoPtr pLO1(new KeyLicenseObjectInfo()), pLO2(new KeyLicenseObjectInfo());
//	pLO1->InitializeTemplate();
//	pLO2->InitializeTemplate();
//	m_vecChildrenLO.clear();
//	m_vecChildrenLO.push_back(pLO1);
//	m_vecChildrenLO.push_back(pLO2);
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyLicenseObjectInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"Id", m_nId);
	{
		value_wrapper<string_t> sType(ConvertType(m_enumType));
		sType.exists(m_enumType.exists());
		SaveValueToXmlNode(spNode, L"Type", sType);
	}
	SaveValueToXmlNode(spNode, L"Name", m_sName);
	SaveValueToXmlNode(spNode, L"Version", m_nVersion);
	{
		for (PlatformVec::const_iterator it = m_vecPlatforms.begin();
				it != m_vecPlatforms.end();
				++it)
		{
			string_t value;
			switch (*it)
			{
				case PT_UNKNOWN:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<string_t>("UNKNOWN", true));
					break;
				case PT_UNIX_FAMILY:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<string_t>("UNIX_FAMILY", true));
					break;
				case PT_WIN_PERSONAL:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<string_t>("WIN_PERSONAL", true));
					break;
				case PT_WIN_WORKSTATION:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<string_t>("WIN_WORKSTATION", true));
					break;
				case PT_WIN_SERVER_SMB:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<string_t>("WIN_SERVER_SMB", true));
					break;
				case PT_WIN_SERVER_ENTERPRISE:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<string_t>("WIN_SERVER_ENTERPRISE", true));
					break;
				default:
					SaveValueToXmlNode(spNode, L"Platform", value_wrapper<DWORD>(*it, true));
					break;
			}
		}
	}
	{
		for (KeyRestrictionVec::const_iterator it = m_vecRestrictions.begin();
				it != m_vecRestrictions.end();
				++it)
		{
			spNode->appendChild((*it)->SerializeToXmlNode(spXmlDoc));
		}
	}
	{
		for (KeyLicenseObjectInfoVec::const_iterator it = m_vecChildrenLO.begin();
				it != m_vecChildrenLO.end();
				++it)
		{
			spNode->appendChild((*it)->SerializeToXmlNode(spXmlDoc));
		}
	}
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyLicenseObjectInfo::DeserializeFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"Id", m_nId);
	{
		value_wrapper<string_t> sType;
		RetrieveXmlNodeValue(spXmlNode, L"Type", sType);
		m_enumType = ConvertType(sType);
		m_enumType.exists(sType.exists());
	}
	RetrieveXmlNodeValue(spXmlNode, L"Name", m_sName);
	RetrieveXmlNodeValue(spXmlNode, L"Version", m_nVersion);
	{
		MSXML2::IXMLDOMNodeListPtr spChildrenList = spXmlNode->childNodes;
		int n = 0;
		for (int i = 0; i < spChildrenList->length; ++i)
		{
			MSXML2::IXMLDOMNodePtr spChildNode = spChildrenList->item[i];
			if (spChildNode != NULL)
			{
				if (spChildNode->nodeName == _bstr_t(L"Restriction"))
				{
					KeyRestrictionPtr pRestr(new KeyRestriction(0));
					pRestr->DeserializeFromXmlNode(spChildNode);
					m_vecRestrictions.push_back(pRestr);
				}
				else if (spChildNode->nodeName == _bstr_t(L"LicenseObjectInfo"))
				{
					KeyLicenseObjectInfoPtr pLO(new KeyLicenseObjectInfo());
					pLO->DeserializeFromXmlNode(spChildNode);
					m_vecChildrenLO.push_back(pLO);
				}
				else if (spChildNode->nodeName == _bstr_t(L"Platform"))
				{ 
					value_wrapper<string_t> platform;
					RetrieveXmlNodeValue(spChildNode, platform);
					if (platform.exists())
					{
						if (strcmpi(platform.ref().c_str(), "UNKNOWN") == 0)
							m_vecPlatforms.push_back(PT_UNKNOWN);
						else if (strcmpi(platform.ref().c_str(), "UNIX_FAMILY") == 0)
							m_vecPlatforms.push_back(PT_UNIX_FAMILY);
						else if (strcmpi(platform.ref().c_str(), "WIN_PERSONAL") == 0)
							m_vecPlatforms.push_back(PT_WIN_PERSONAL);
						else if (strcmpi(platform.ref().c_str(), "WIN_WORKSTATION") == 0)
							m_vecPlatforms.push_back(PT_WIN_WORKSTATION);
						else if (strcmpi(platform.ref().c_str(), "WIN_SERVER_SMB") == 0)
							m_vecPlatforms.push_back(PT_WIN_SERVER_SMB);
						else if (strcmpi(platform.ref().c_str(), "WIN_SERVER_ENTERPRISE") == 0)
							m_vecPlatforms.push_back(PT_WIN_SERVER_ENTERPRISE);
						else
							m_vecPlatforms.push_back(static_cast<PlatformType>(atol(platform.ref().c_str())));
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyLicenseObjectInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEY_LO_NAME, m_sName);
	DT_AddData(hData, AVP_PID_KEY_LO_VERSION, m_nVersion);
	HDATA hPlatformList = DT_AddData(hData, AVP_PID_KEY_PRODUCT_PLATFORM_LIST);
	{
		for (PlatformVec::const_iterator it = m_vecPlatforms.begin();
				it != m_vecPlatforms.end();
				++it)
		{
			DT_AddData(hPlatformList, MAKE_AVP_PID(*it, 0, avpt_nothing, 0));
		}
	}
	HDATA hRestrList = DT_AddData(hData, AVP_PID_KEY_RESTRICTION_LIST);
	{
		for (KeyRestrictionVec::const_iterator it = m_vecRestrictions.begin();
				it != m_vecRestrictions.end();
				++it)
		{
			DT_AddData(hRestrList, (*it)->SerializeToPropertyNode());
		}
	}
	HDATA hChildrenLOList = DT_AddData(hData, AVP_PID_KEY_LO_LIST);
	{
		for (KeyLicenseObjectInfoVec::const_iterator it = m_vecChildrenLO.begin();
				it != m_vecChildrenLO.end();
				++it)
		{
			DT_AddData(hChildrenLOList, (*it)->SerializeToPropertyNode());
		}
	}
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyLicenseObjectInfo::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	m_nId = GET_AVP_PID_ID(::DATA_Get_Id(hData, 0));
	DT_GetData(hData, AVP_PID_KEY_LO_NAME, m_sName);
	DT_GetData(hData, AVP_PID_KEY_LO_VERSION, m_nVersion);
	{
		HDATA hPlatformList = DT_GetChildNode(hData, AVP_PID_KEY_PRODUCT_PLATFORM_LIST);
		if (hPlatformList)
		{
			HDATA hCurrent = ::DATA_Get_First(hPlatformList, 0);
			while (hCurrent)
			{
				m_vecPlatforms.push_back(static_cast<PlatformType>(GET_AVP_PID_ID(::DATA_Get_Id(hCurrent, 0))));
				hCurrent = ::DATA_Get_Next(hCurrent, 0);
			}
		}
	}
	{
		HDATA hRestrList = DT_GetChildNode(hData, AVP_PID_KEY_RESTRICTION_LIST);
		if (hRestrList)
		{
			HDATA hCurrent = ::DATA_Get_First(hRestrList, 0);
			while (hCurrent)
			{
				KeyRestrictionPtr spRestr(new KeyRestriction(GET_AVP_PID_ID(::DATA_Get_Id(hCurrent, 0))));
				spRestr->DeserializeFromPropertyNode(hCurrent);
				m_vecRestrictions.push_back(spRestr);
				hCurrent = ::DATA_Get_Next(hCurrent, 0);
			}
		}
	}
	{
		HDATA hLOList = DT_GetChildNode(hData, AVP_PID_KEY_LO_LIST);
		if (hLOList)
		{
			HDATA hCurrent = ::DATA_Get_First(hLOList, 0);
			while (hCurrent)
			{
				AVP_dword pid = ::DATA_Get_Id(hCurrent, 0);
				KeyLicenseObjectInfoPtr spLO(new KeyLicenseObjectInfo(
														GET_AVP_PID_ID(pid),
														static_cast<LicenseObjectId::Type>(GET_AVP_PID_APP(pid))
														));
				spLO->DeserializeFromPropertyNode(hCurrent);
				m_vecChildrenLO.push_back(spLO);
				hCurrent = ::DATA_Get_Next(hCurrent, 0);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
void KeyInfo::Clear()
{
	m_sKeyDescription.ref().swap(string_t());
	m_nKeyVersion = 0;

	m_dtKeyProdDate.ref().dateParts.year = 
		m_dtKeyProdDate.ref().dateParts.month =
		m_dtKeyProdDate.ref().dateParts.dayOfWeek =
		m_dtKeyProdDate.ref().dateParts.day = 0;

	m_KeySerialNumber.ref().parts.cust_id =
		m_KeySerialNumber.ref().parts.app_id =
		m_KeySerialNumber.ref().parts.key_num = 0;

	m_nKeyLicenseType	= 0;
	m_bKeyIsTrial		= false;
	m_nKeyLifeSpan		= 0;

	m_dtKeyExpDate.ref().dateParts.year =
		m_dtKeyExpDate.ref().dateParts.month =
		m_dtKeyExpDate.ref().dateParts.dayOfWeek =
		m_dtKeyExpDate.ref().dateParts.day = 0;

	m_vecKeyLicense.clear();
	m_nKeyLicenseCount = 0;
	m_KeyProductInfo.ref().Clear();
	m_KeyProductLicenseObjectInfo.ref().Clear();
	m_bKeyHasSupport = false;
	m_sKeyLicenseInfo.ref().swap(string_t());
	m_sKeySupportInfo.ref().swap(string_t());
	m_sKeyPLPosName.ref().swap(string_t());
	m_nKeyPLPos			= 0;
	m_nKeyProductId		= 0;
	m_sKeyProductVersion.ref().swap(string_t());
	m_nKeyMarketSector		= 0;
	m_nKeySalesChannel		= 0;
	m_DistributorInfo.ref().Clear();
	m_ResellerInfo.ref().Clear();
	m_LocalOfficeInfo.ref().Clear();
	m_HQInfo.ref().Clear();
	m_SupportCenterInfo.ref().Clear();
	m_nParthnerId			= 0;
	m_sParthnerName.ref().swap(string_t());
	m_sCompanyName.ref().swap(string_t());
	m_CustomerInfo.ref().Clear();
	m_sKeyRequestGuid.ref().swap(string_t());
	m_sKeyParentGuid.ref().swap(string_t());
	m_nKeyParent			= 0;
	m_nKeyCategory			= 0;
}

////////////////////////////////////////////////////////////////////////////
void KeyInfo::InitializeTemplate()
{
	time_t tCurrent = time(NULL);
	struct tm tmCurrent = *gmtime(&tCurrent);

	m_sKeyDescription	= "Kaspersky Lab key file";
	m_nKeyVersion		= 1;

	m_dtKeyProdDate.ref().dateParts.year = 1900 + tmCurrent.tm_year;
	m_dtKeyProdDate.ref().dateParts.month = tmCurrent.tm_mon + 1;
	m_dtKeyProdDate.ref().dateParts.dayOfWeek = tmCurrent.tm_wday;
	m_dtKeyProdDate.ref().dateParts.day = tmCurrent.tm_mday;

	m_KeySerialNumber.ref().parts.cust_id = 1;
	m_KeySerialNumber.ref().parts.app_id = 2;
	m_KeySerialNumber.ref().parts.key_num = 3;

	m_nKeyLicenseType		= 1;
	m_bKeyIsTrial			= true;
	m_nKeyLifeSpan			= 1;

	m_dtKeyExpDate.ref().dateParts.year = 1900 + tmCurrent.tm_year;
	m_dtKeyExpDate.ref().dateParts.month = tmCurrent.tm_mon + 1;
	m_dtKeyExpDate.ref().dateParts.dayOfWeek = tmCurrent.tm_wday;
	m_dtKeyExpDate.ref().dateParts.day = tmCurrent.tm_mday;

	m_vecKeyLicense.clear();
	m_vecKeyLicense.push_back(LicenseObjectVer4(1, 1));
	m_vecKeyLicense.push_back(LicenseObjectVer4(2, 2));
	m_nKeyLicenseCount = 1;
	m_KeyProductInfo.ref().InitializeTemplate();
	m_KeyProductLicenseObjectInfo.ref().InitializeTemplate();
	m_bKeyHasSupport = true;
	m_sKeyLicenseInfo		= "KeyLicenseInfo";
	m_sKeySupportInfo		= "KeySupportInfo";
	m_sKeyPLPosName			= "KeyPLPosName";
	m_nKeyPLPos				= 1;
	m_nKeyProductId			= 1;
	m_sKeyProductVersion	= "1";
	m_nKeyMarketSector		= 1;
	m_nKeySalesChannel		= 1;
	m_DistributorInfo.ref().InitializeTemplate();
	m_ResellerInfo.ref().InitializeTemplate();
	m_LocalOfficeInfo.ref().InitializeTemplate();
	m_HQInfo.ref().InitializeTemplate();
	m_SupportCenterInfo.ref().InitializeTemplate();
	m_nParthnerId			= 1;
	m_sParthnerName			= "ParthnerName";
	m_sCompanyName			= "CompanyName";
	m_CustomerInfo.ref().InitializeTemplate();
	m_sKeyRequestGuid		= "KeyRequestGuid";
	m_sKeyParentGuid		= "KeyParentGuid";
	m_nKeyParent			= 1;
	m_nKeyCategory			= 1;
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyInfo::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	SaveValueToXmlNode(spNode, L"Description", m_sKeyDescription);
	SaveValueToXmlNode(spNode, L"Version", m_nKeyVersion);
	SaveValueToXmlNode(spNode, L"ProdDate", m_dtKeyProdDate);
	SaveValueToXmlNode(spNode, L"SerialNumber", m_KeySerialNumber);
	SaveValueToXmlNode(spNode, L"LicenseType", m_nKeyLicenseType);
	SaveValueToXmlNode(spNode, L"IsTrial", value_wrapper<string_t>("", m_bKeyIsTrial));
	SaveValueToXmlNode(spNode, L"LifeSpan", m_nKeyLifeSpan);
	SaveValueToXmlNode(spNode, L"ExpDate", m_dtKeyExpDate);
	SaveValueToXmlNode(spNode, L"LicenseCount", m_nKeyLicenseCount);
	{
		for (LicenseObjectVer4Vec::const_iterator it = m_vecKeyLicense.begin();
				it != m_vecKeyLicense.end();
				++it)
		{
//			MSXML2::IXMLDOMNodePtr spLicense = spXmlDoc->createElement(L"License");
//			SaveValueToXmlNode(spLicense, L"ObjectId", value_wrapper<DWORD>(it->first, true));
//			SaveValueToXmlNode(spLicense, L"Count", value_wrapper<DWORD>(it->second, true));
//			spNode->appendChild(spLicense);
		}
	}
	spNode->appendChild(m_KeyProductInfo.ref().SerializeToXmlNode(spXmlDoc));
	spNode->appendChild(m_KeyProductLicenseObjectInfo.ref().SerializeToXmlNode(spXmlDoc));
	SaveValueToXmlNode(spNode, L"HasSupport", value_wrapper<string_t>("", m_bKeyHasSupport));
	SaveValueToXmlNode(spNode, L"LicenseInfo", m_sKeyLicenseInfo, true);
	SaveValueToXmlNode(spNode, L"SupportInfo", m_sKeySupportInfo, true);
	SaveValueToXmlNode(spNode, L"PLPosName", m_sKeyPLPosName);
	SaveValueToXmlNode(spNode, L"PLPos", m_nKeyPLPos);
	SaveValueToXmlNode(spNode, L"ProductId", m_nKeyProductId);
	SaveValueToXmlNode(spNode, L"ProductVersion", m_sKeyProductVersion);
	SaveValueToXmlNode(spNode, L"MarketSector", m_nKeyMarketSector);
	SaveValueToXmlNode(spNode, L"SalesChannel", m_nKeySalesChannel);
	spNode->appendChild(m_DistributorInfo.ref().SerializeToXmlNode(spXmlDoc));
	spNode->appendChild(m_ResellerInfo.ref().SerializeToXmlNode(spXmlDoc));
	spNode->appendChild(m_LocalOfficeInfo.ref().SerializeToXmlNode(spXmlDoc));
	spNode->appendChild(m_HQInfo.ref().SerializeToXmlNode(spXmlDoc));
	spNode->appendChild(m_SupportCenterInfo.ref().SerializeToXmlNode(spXmlDoc));
	SaveValueToXmlNode(spNode, L"ParthnerId", m_nParthnerId);
	SaveValueToXmlNode(spNode, L"ParthnerName", m_sParthnerName);
	SaveValueToXmlNode(spNode, L"CompanyName", m_sCompanyName);
	spNode->appendChild(m_CustomerInfo.ref().SerializeToXmlNode(spXmlDoc));
	SaveValueToXmlNode(spNode, L"RequestGuid", m_sKeyRequestGuid);
	SaveValueToXmlNode(spNode, L"ParentGuid", m_sKeyParentGuid);
	SaveValueToXmlNode(spNode, L"ParentKey", m_nKeyParent);
	SaveValueToXmlNode(spNode, L"KeyCategory", m_nKeyCategory);
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyInfo::DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode)
{
	Clear();
	RetrieveXmlNodeValue(spXmlNode, L"Description", m_sKeyDescription);
	RetrieveXmlNodeValue(spXmlNode, L"Version", m_nKeyVersion);
	RetrieveXmlNodeValue(spXmlNode, L"ProdDate", m_dtKeyProdDate);
	RetrieveXmlNodeValue(spXmlNode, L"SerialNumber", m_KeySerialNumber);
	RetrieveXmlNodeValue(spXmlNode, L"LicenseType", m_nKeyLicenseType);
	m_bKeyIsTrial = (RetrieveXmlNode(spXmlNode, L"IsTrial") != NULL);
	RetrieveXmlNodeValue(spXmlNode, L"LifeSpan", m_nKeyLifeSpan);
	RetrieveXmlNodeValue(spXmlNode, L"ExpDate", m_dtKeyExpDate);
	RetrieveXmlNodeValue(spXmlNode, L"LicenseCount", m_nKeyLicenseCount);
/*
	{
		for (LicenseObjectVer4Vec::const_iterator it = m_vecKeyLicense.begin();
				it != m_vecKeyLicense.end();
				++it)
		{
			MSXML2::IXMLDOMNodePtr spLicense = spXmlDoc->createElement(L"License");
			spLicense->appendChild(SerializeValueToXmlNode(spXmlDoc, L"ObjectId", it->first));
			spLicense->appendChild(SerializeValueToXmlNode(spXmlDoc, L"Count", it->second));
			spNode->appendChild(spLicense);
		}
	}
*/
	RetrieveXmlNodeValue(spXmlNode, L"ProductInfo", m_KeyProductInfo);
	RetrieveXmlNodeValue(spXmlNode, L"LicenseObjectInfo", m_KeyProductLicenseObjectInfo);
	m_bKeyHasSupport = (RetrieveXmlNode(spXmlNode, L"HasSupport") != NULL);
	RetrieveXmlNodeValue(spXmlNode, L"LicenseInfo", m_sKeyLicenseInfo);
	RetrieveXmlNodeValue(spXmlNode, L"SupportInfo", m_sKeySupportInfo);
	RetrieveXmlNodeValue(spXmlNode, L"PLPosName", m_sKeyPLPosName);
	RetrieveXmlNodeValue(spXmlNode, L"PLPos", m_nKeyPLPos);
	RetrieveXmlNodeValue(spXmlNode, L"ProductId", m_nKeyProductId);
	RetrieveXmlNodeValue(spXmlNode, L"ProductVersion", m_sKeyProductVersion);
	RetrieveXmlNodeValue(spXmlNode, L"MarketSector", m_nKeyMarketSector);
	RetrieveXmlNodeValue(spXmlNode, L"SalesChannel", m_nKeySalesChannel);
	RetrieveXmlNodeValue(spXmlNode, L"DistributorInfo", m_DistributorInfo);
	RetrieveXmlNodeValue(spXmlNode, L"ResellerInfo", m_ResellerInfo);
	RetrieveXmlNodeValue(spXmlNode, L"LocalOfficeInfo", m_LocalOfficeInfo);
	RetrieveXmlNodeValue(spXmlNode, L"HQInfo", m_HQInfo);
	RetrieveXmlNodeValue(spXmlNode, L"SupportCenterInfo", m_SupportCenterInfo);
	RetrieveXmlNodeValue(spXmlNode, L"ParthnerId", m_nParthnerId);
	RetrieveXmlNodeValue(spXmlNode, L"ParthnerName", m_sParthnerName);
	RetrieveXmlNodeValue(spXmlNode, L"CompanyName", m_sCompanyName);
	RetrieveXmlNodeValue(spXmlNode, L"Customer", m_CustomerInfo);
	RetrieveXmlNodeValue(spXmlNode, L"RequestGuid", m_sKeyRequestGuid);
	RetrieveXmlNodeValue(spXmlNode, L"ParentGuid", m_sKeyParentGuid);
	RetrieveXmlNodeValue(spXmlNode, L"ParentKey", m_nKeyParent);
	RetrieveXmlNodeValue(spXmlNode, L"KeyCategory", m_nKeyCategory);
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyInfo::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, AVP_PID_KEYDESCRIPTION, m_sKeyDescription);
	DT_AddData(hData, AVP_PID_KEYVERSION, m_nKeyVersion);
	DT_AddData(hData, AVP_PID_KEY_PROD_DATE, m_dtKeyProdDate);
	DT_AddData(hData, AVP_PID_KEYSERIALNUMBER, m_KeySerialNumber);
	DT_AddData(hData, AVP_PID_KEYLICENCETYPE, m_nKeyLicenseType);
	if (m_bKeyIsTrial)
		DT_AddData(hData, AVP_PID_KEYISTRIAL);
	DT_AddData(hData, AVP_PID_KEYLIFESPAN, m_nKeyLifeSpan);
	DT_AddData(hData, AVP_PID_KEYEXPDATE, m_dtKeyExpDate);
	DT_AddData(hData, AVP_PID_KEYLICENCECOUNT, m_nKeyLicenseCount);
	{
		HDATA hLicense = DT_AddData(hData, AVP_PID_KEYLICENCE);
//		for (LicenseObjectVer4Vec::const_iterator it = m_vecKeyLicense.begin();
//				it != m_vecKeyLicense.end();
//				++it)
//		{
//			DT_AddData(hLicense, it->first, it->second);
//		}
	}
	DT_AddData(hData, m_KeyProductInfo.ref().SerializeToPropertyNode());
	HDATA hLORoot = DT_AddData(hData, AVP_PID_KEY_LO_ROOT);
	DT_AddData(hLORoot, m_KeyProductLicenseObjectInfo.ref().SerializeToPropertyNode());
	if (m_bKeyHasSupport)
		DT_AddData(hData, AVP_PID_KEYHASSUPPORT);
	DT_AddData(hData, AVP_PID_KEYLICENCEINFO, m_sKeyLicenseInfo);
	DT_AddData(hData, AVP_PID_KEYSUPPORTINFO, m_sKeySupportInfo);
	DT_AddData(hData, AVP_PID_KEYPLPOSNAME, m_sKeyPLPosName);
	DT_AddData(hData, AVP_PID_KEYPLPOS, m_nKeyPLPos);
	DT_AddData(hData, AVP_PID_KEY_PRODUCTID, m_nKeyProductId);
	DT_AddData(hData, AVP_PID_KEY_PRODUCTVERSION, m_sKeyProductVersion);
	DT_AddData(hData, AVP_PID_KEY_MARCETSECTOR, m_nKeyMarketSector);
	DT_AddData(hData, AVP_PID_KEY_SALESCHANNEL, m_nKeySalesChannel);
	DT_AddData(hData, m_DistributorInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, m_ResellerInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, m_LocalOfficeInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, m_HQInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, m_SupportCenterInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, AVP_PID_KEY_PARTNERID, m_nParthnerId);
	DT_AddData(hData, AVP_PID_KEY_PARTNERNAME, m_sParthnerName);
	DT_AddData(hData, AVP_PID_KEYCOMPANYNAME, m_sCompanyName);
	DT_AddData(hData, m_CustomerInfo.ref().SerializeToPropertyNode());
	DT_AddData(hData, AVP_PID_KEYREQUESTGIUD, m_sKeyRequestGuid);
	DT_AddData(hData, AVP_PID_KEYPARENTGIUD, m_sKeyParentGuid);
	DT_AddData(hData, AVP_PID_KEY_PARENT, m_nKeyParent);
	DT_AddData(hData, AVP_PID_KEY_KEYCATEGORY, m_nKeyCategory);
	return hData;
}

////////////////////////////////////////////////////////////////////////////
void KeyInfo::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	DT_GetData(hData, AVP_PID_KEYDESCRIPTION, m_sKeyDescription);
	DT_GetData(hData, AVP_PID_KEYVERSION, m_nKeyVersion);
	DT_GetData(hData, AVP_PID_KEY_PROD_DATE, m_dtKeyProdDate);
	DT_GetData(hData, AVP_PID_KEYSERIALNUMBER, m_KeySerialNumber);
	DT_GetData(hData, AVP_PID_KEYLICENCETYPE, m_nKeyLicenseType);
	m_bKeyIsTrial = (DT_GetChildNode(hData, AVP_PID_KEYISTRIAL) != NULL);
	DT_GetData(hData, AVP_PID_KEYLIFESPAN, m_nKeyLifeSpan);
	DT_GetData(hData, AVP_PID_KEYEXPDATE, m_dtKeyExpDate);
	DT_GetData(hData, AVP_PID_KEYLICENCECOUNT, m_nKeyLicenseCount);
	{
		HDATA hLicense = DT_GetChildNode(hData, AVP_PID_KEYLICENCE);
//		for (LicenseObjectVer4Vec::const_iterator it = m_vecKeyLicense.begin();
//				it != m_vecKeyLicense.end();
//				++it)
//		{
//			DT_AddData(hLicense, it->first, it->second);
//		}
	}
	DT_GetData(hData, m_KeyProductInfo.ref().GetDataTreePID(), m_KeyProductInfo);
	{
		HDATA hLORoot = DT_GetChildNode(hData, AVP_PID_KEY_LO_ROOT);
		if (hLORoot)
		{
			HDATA hLOInfo = ::DATA_Get_First(hLORoot, 0);
			if (hLOInfo)
			{
				m_KeyProductLicenseObjectInfo.exists(true);
				m_KeyProductLicenseObjectInfo.ref().DeserializeFromPropertyNode(hLOInfo);
			}
		}
	}
	m_bKeyHasSupport = (DT_GetChildNode(hData, AVP_PID_KEYHASSUPPORT) != NULL);
	DT_GetData(hData, AVP_PID_KEYLICENCEINFO, m_sKeyLicenseInfo);
	DT_GetData(hData, AVP_PID_KEYSUPPORTINFO, m_sKeySupportInfo);
	DT_GetData(hData, AVP_PID_KEYPLPOSNAME, m_sKeyPLPosName);
	DT_GetData(hData, AVP_PID_KEYPLPOS, m_nKeyPLPos);
	DT_GetData(hData, AVP_PID_KEY_PRODUCTID, m_nKeyProductId);
	DT_GetData(hData, AVP_PID_KEY_PRODUCTVERSION, m_sKeyProductVersion);
	DT_GetData(hData, AVP_PID_KEY_MARCETSECTOR, m_nKeyMarketSector);
	DT_GetData(hData, AVP_PID_KEY_SALESCHANNEL, m_nKeySalesChannel);
	DT_GetData(hData, m_DistributorInfo.ref().GetDataTreePID(), m_DistributorInfo);
	DT_GetData(hData, m_ResellerInfo.ref().GetDataTreePID(), m_ResellerInfo);
	DT_GetData(hData, m_LocalOfficeInfo.ref().GetDataTreePID(), m_LocalOfficeInfo);
	DT_GetData(hData, m_HQInfo.ref().GetDataTreePID(), m_HQInfo);
	DT_GetData(hData, m_SupportCenterInfo.ref().GetDataTreePID(), m_SupportCenterInfo);
	DT_GetData(hData, AVP_PID_KEY_PARTNERID, m_nParthnerId);
	DT_GetData(hData, AVP_PID_KEY_PARTNERNAME, m_sParthnerName);
	DT_GetData(hData, AVP_PID_KEYCOMPANYNAME, m_sCompanyName);
	DT_GetData(hData, m_CustomerInfo.ref().GetDataTreePID(), m_CustomerInfo);
	DT_GetData(hData, AVP_PID_KEYREQUESTGIUD, m_sKeyRequestGuid);
	DT_GetData(hData, AVP_PID_KEYPARENTGIUD, m_sKeyParentGuid);
	DT_GetData(hData, AVP_PID_KEY_PARENT, m_nKeyParent);
	DT_GetData(hData, AVP_PID_KEY_KEYCATEGORY, m_nKeyCategory);
}

////////////////////////////////////////////////////////////////////////////
void KeyRoot::Clear()
{
	m_KeyInfo.ref().Clear();
	m_vecComponentLevels.clear();
}

////////////////////////////////////////////////////////////////////////////
void KeyRoot::InitializeTemplate()
{
	m_KeyInfo.ref().InitializeTemplate();
	m_vecComponentLevels.clear();
	m_vecComponentLevels.push_back(ComponentLevel(1, 1));
	m_vecComponentLevels.push_back(ComponentLevel(2, 2));
}

////////////////////////////////////////////////////////////////////////////
MSXML2::IXMLDOMNodePtr KeyRoot::SerializeToXmlNode(
	const MSXML2::IXMLDOMDocumentPtr& spXmlDoc
	) const
{
	MSXML2::IXMLDOMNodePtr spNode = spXmlDoc->createElement(_bstr_t(GetXmlTagName()));
	spNode->appendChild(m_KeyInfo.ref().SerializeToXmlNode(spXmlDoc));
	for (ComponentLevelVec::const_iterator it = m_vecComponentLevels.begin();
			it != m_vecComponentLevels.end();
			++it)
	{
//		MSXML2::IXMLDOMNodePtr spComponentLevel = spXmlDoc->createElement(L"ComponentLevel");
//		SaveValueToXmlNode(spComponentLevel, L"ComponentId", value_wrapper<DWORD>(it->first, true));
//		SaveValueToXmlNode(spComponentLevel, L"Level", value_wrapper<DWORD>(it->second, true));
//		spNode->appendChild(spComponentLevel);
	}
	return spNode;
}

////////////////////////////////////////////////////////////////////////////
void KeyRoot::DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode)
{
	Clear();
	MSXML2::IXMLDOMNodeListPtr spChildrenList = spXmlNode->childNodes;
	for (int i = 0; i < spChildrenList->length; ++i)
	{
		MSXML2::IXMLDOMNodePtr spChildNode = spChildrenList->item[i];
		if (spChildNode)
		{
			if (spChildNode->nodeName == _bstr_t(L"KeyInfo"))
			{
				m_KeyInfo.ref().DeserializeFromXmlNode(spChildNode);
			}
			else if (spChildNode->nodeName == _bstr_t(L"ComponentLevel"))
			{
				m_vecComponentLevels.push_back(DeserializeComponentLevelFromXmlNode(spChildNode));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
KeyRoot::ComponentLevel KeyRoot::DeserializeComponentLevelFromXmlNode(
	const MSXML2::IXMLDOMNodePtr& spXmlNode
	)
{
	value_wrapper<DWORD> component_id, level;
	RetrieveXmlNodeValue(spXmlNode, L"ComponentId", component_id);
	RetrieveXmlNodeValue(spXmlNode, L"Level", level);
	return KeyRoot::ComponentLevel(component_id.ref(), level.ref());
}

////////////////////////////////////////////////////////////////////////////
HDATA KeyRoot::SerializeToPropertyNode() const
{
	HDATA hData = ::DATA_Add(0, 0, GetDataTreePID(), 0, 0);
	DT_AddData(hData, m_KeyInfo.ref().SerializeToPropertyNode());
	// !!! нет информации о формировании ID(i)
	for (ComponentLevelVec::const_iterator it = m_vecComponentLevels.begin();
			it != m_vecComponentLevels.end();
			++it)
	{
//		DT_AddData(hData, MAKE_AVP_PID(it->first, AVP_DC_KEY, avpt_dword, 0), value_wrapper<DWORD>(it->second, true));
	}
	return hData;
}


////////////////////////////////////////////////////////////////////////////
void KeyRoot::DeserializeFromPropertyNode(HDATA hData)
{
	Clear();
	DT_GetData(hData, m_KeyInfo.ref().GetDataTreePID(), m_KeyInfo);
	// !!! нет информации о формировании ID(i)
}

////////////////////////////////////////////////////////////////////////////
void KeyInfoAssembler::ReadFromKeyFile(
	const char* szFileName, 
	KeyRoot& key_root
	)
{
	HDATA hData = 0;
	::KLDT_DeserializeUsingSWM(szFileName, &hData);
	if (hData)
	{
		key_root.DeserializeFromPropertyNode(hData);
		::DATA_Remove(hData, 0);
	}
}

////////////////////////////////////////////////////////////////////////////
void KeyInfoAssembler::WriteToKeyFile(
	const char* szFileName, 
	const KeyRoot& key_root
	)
{
	HDATA hData = key_root.SerializeToPropertyNode();
	if (hData)
	{
		::KLDT_SerializeUsingSWM(szFileName, hData, "");
		::DATA_Remove(hData, 0);
	}
}

////////////////////////////////////////////////////////////////////////////
void KeyInfoAssembler::ReadFromXmlFile(
	const char* szFileName, 
	KeyRoot& key_root
	)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocumentPtr spXmlDoc;
	hr = spXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument/*FreeThreadedDOMDocument40*/));
	if (FAILED(hr))
		throw std::runtime_error("Can't create MSXML.Document object.");

	spXmlDoc->load(_bstr_t(szFileName));
	
	key_root.DeserializeFromXmlNode(spXmlDoc->documentElement);
	
}

////////////////////////////////////////////////////////////////////////////
void KeyInfoAssembler::WriteToXmlFile(
	const char* szFileName, 
	const KeyRoot& key_root
	)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocumentPtr spXmlDoc;
	hr = spXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument/*FreeThreadedDOMDocument40*/));
	if (FAILED(hr))
		throw std::runtime_error("Can't create MSXML.Document object.");
		
//	spXmlDoc->preserveWhiteSpace = VARIANT_TRUE;

	spXmlDoc->appendChild(key_root.SerializeToXmlNode(spXmlDoc));
	spXmlDoc->save(_bstr_t(szFileName));
}
