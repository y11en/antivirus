#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <property/property.h>
#include <Licensing2/src/LicenseKeyPid.h>
#include <Licensing2/include/LicensingInterface.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////
template <typename T>
class value_wrapper
{
public:
	value_wrapper() : m_value(), m_exists(true)  {}
	value_wrapper(const T& value, bool exists = true) : m_value(value), m_exists(exists)  {}
	T& operator = (const T& value)  { return (m_value = value); }
	operator const T& () const  { return m_value; }
	operator T& ()  { return m_value; }
	const T& ref() const  { return m_value; }
	T& ref()  { return m_value; }
	void zero_value()  { m_value = T(); }
	bool exists() const  { return m_exists; }
	void exists(bool exists)  { m_exists = exists; }
private:
	T m_value;
	bool m_exists;
};

////////////////////////////////////////////////////////////////////////////
class SerializableDataBase
{
public:
	typedef std::string string_t;
	typedef union  
	{
		AVP_date date;
		struct
		{
			unsigned short year;
			unsigned short month;
			unsigned short dayOfWeek;
			unsigned short day;
		} dateParts;
	} date_t;
	typedef union  
	{
		DWORD sn[3];
		struct
		{
			DWORD cust_id;
			DWORD app_id;
			DWORD key_num;
		} parts;
	} serialnumber_t;
public:
	virtual ~SerializableDataBase()  {}
	virtual void Clear() = 0;
	virtual void InitializeTemplate() = 0;
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const = 0;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode) = 0;
	virtual HDATA SerializeToPropertyNode() const = 0;
	virtual void DeserializeFromPropertyNode(HDATA hData)  {}
};

////////////////////////////////////////////////////////////////////////////
class KeyProductInfo : public SerializableDataBase
{
public:
	KeyProductInfo()
		: m_nMarketSector(0)
		, m_nSalesChannel(0)
		, m_nLocalization(0)
		, m_nPackage(0)
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"ProductInfo"; }
	static AVP_dword GetDataTreePID()  { return KasperskyLicensing::Implementation::AVP_PID_KEY_PRODUCT_INFO; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	value_wrapper<DWORD> m_nMarketSector;
	value_wrapper<DWORD> m_nSalesChannel;
	value_wrapper<DWORD> m_nLocalization;
	value_wrapper<DWORD> m_nPackage;
};

////////////////////////////////////////////////////////////////////////////
class KeyContactInfo : public SerializableDataBase
{
public:
	KeyContactInfo()
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"ContactInfo"; }
	static AVP_dword GetDataTreePID()  { return KasperskyLicensing::Implementation::AVP_PID_KEY_CONTACT_INFO; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
protected:
	value_wrapper<string_t> m_sName;
	value_wrapper<string_t> m_sCountry;
	value_wrapper<string_t> m_sCity;
	value_wrapper<string_t> m_sAddress;
	value_wrapper<string_t> m_sPhone;
	value_wrapper<string_t> m_sFax;
	value_wrapper<string_t> m_sEmail;
	value_wrapper<string_t> m_sWWW;
};

////////////////////////////////////////////////////////////////////////////
class KeySupportInfo : public SerializableDataBase
{
public:
	KeySupportInfo()  {}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"SupportInfo"; }
	static AVP_dword GetDataTreePID()  { return KasperskyLicensing::Implementation::AVP_PID_KEY_SUPPORT_INFO; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	value_wrapper<string_t> m_sPhone;
	value_wrapper<string_t> m_sFax;
	value_wrapper<string_t> m_sEmail;
	value_wrapper<string_t> m_sWWW;
};

////////////////////////////////////////////////////////////////////////////
class KeyCompanyInfo : public SerializableDataBase
{
public:
	void Clear();
	void InitializeTemplate();
	virtual const wchar_t* GetXmlTagName() const = 0;
	MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual AVP_dword GetDataTreePID() const = 0;
	HDATA SerializeToPropertyNode() const;
	void DeserializeFromPropertyNode(HDATA hData);
protected:
	KeyCompanyInfo()  {}
private:
	value_wrapper<KeyContactInfo> m_ContactInfo;
	value_wrapper<KeySupportInfo> m_SupportInfo;
};

////////////////////////////////////////////////////////////////////////////
class KeyDistributorInfo : public KeyCompanyInfo
{
public:
	virtual const wchar_t* GetXmlTagName() const  { return L"DistributorInfo"; }
	virtual AVP_dword GetDataTreePID() const  { return KasperskyLicensing::Implementation::AVP_PID_KEY_DISTRIBUTOR_INFO; }
};

////////////////////////////////////////////////////////////////////////////
class KeyResellerInfo : public KeyCompanyInfo
{
public:
	virtual const wchar_t* GetXmlTagName() const  { return L"ResellerInfo"; }
	virtual AVP_dword GetDataTreePID() const  { return KasperskyLicensing::Implementation::AVP_PID_KEY_RESELLER_INFO; }
};

////////////////////////////////////////////////////////////////////////////
class KeyLocalOfficeInfo : public KeyCompanyInfo
{
public:
	virtual const wchar_t* GetXmlTagName() const  { return L"LocalOfficeInfo"; }
	virtual AVP_dword GetDataTreePID() const  { return KasperskyLicensing::Implementation::AVP_PID_KEY_LOCALOFFICE_INFO; }
};

////////////////////////////////////////////////////////////////////////////
class KeyHQInfo : public KeyCompanyInfo
{
public:
	virtual const wchar_t* GetXmlTagName() const  { return L"HQInfo"; }
	virtual AVP_dword GetDataTreePID() const  { return KasperskyLicensing::Implementation::AVP_PID_KEY_HQ_INFO; }
};

////////////////////////////////////////////////////////////////////////////
class KeySupportCenterInfo : public KeyCompanyInfo
{
public:
	virtual const wchar_t* GetXmlTagName() const  { return L"SupportCenterInfo"; }
	virtual AVP_dword GetDataTreePID() const  { return KasperskyLicensing::Implementation::AVP_PID_KEY_SUPPORTCENTER_INFO; }
};

////////////////////////////////////////////////////////////////////////////
class KeyCustomerInfo : public SerializableDataBase
{
public:
	KeyCustomerInfo()
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"Customer"; }
	static AVP_dword GetDataTreePID()  { return AVP_PID_KEYCUSTOMER; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
protected:
	value_wrapper<string_t> m_sCompanyName;
	value_wrapper<string_t> m_sName;
	value_wrapper<string_t> m_sCountry;
	value_wrapper<string_t> m_sCity;
	value_wrapper<string_t> m_sAddress;
	value_wrapper<string_t> m_sPhone;
	value_wrapper<string_t> m_sFax;
	value_wrapper<string_t> m_sEmail;
	value_wrapper<string_t> m_sWWW;
};

////////////////////////////////////////////////////////////////////////////
class KeyCondition : public SerializableDataBase
{
public:
	KeyCondition(DWORD nNumber)
		: m_nNumber(nNumber)
		, m_nNotify(0)
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"Condition"; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	AVP_dword GetDataTreePID() const  { return MAKE_AVP_PID(m_nNumber, 0, avpt_nothing, 0); }
private:
	DWORD m_nNumber;
	value_wrapper<string_t> m_sExpression;
	value_wrapper<DWORD>	m_nNotify;
//	bool	 m_bNotifyExists;
};

////////////////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<KeyCondition> KeyConditionPtr;
typedef std::vector<KeyConditionPtr> KeyConditionVec;

////////////////////////////////////////////////////////////////////////////
class KeyRestriction : public SerializableDataBase
{
public:
	KeyRestriction(DWORD nId)
		: m_nId(nId)
		, m_nValue(0)
		, m_nDefaultNotify(0)
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"Restriction"; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	AVP_dword GetDataTreePID() const  { return MAKE_AVP_PID(m_nId, 0, avpt_nothing, 0); }
private:
	value_wrapper<AVP_dword> m_nId;						
	value_wrapper<string_t>	 m_sName;
	value_wrapper<AVP_dword> m_nValue;
	value_wrapper<string_t>	 m_sAdditiveCondition;
	KeyConditionVec			 m_vecConditions;
	value_wrapper<AVP_dword> m_nDefaultNotify;
};

////////////////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<KeyRestriction> KeyRestrictionPtr;
typedef std::vector<KeyRestrictionPtr> KeyRestrictionVec;

////////////////////////////////////////////////////////////////////////////
class KeyLicenseObjectInfo;
typedef boost::shared_ptr<KeyLicenseObjectInfo> KeyLicenseObjectInfoPtr;
typedef std::vector<KeyLicenseObjectInfoPtr> KeyLicenseObjectInfoVec;

////////////////////////////////////////////////////////////////////////////
class KeyLicenseObjectInfo : public SerializableDataBase
{
public:
	KeyLicenseObjectInfo(
		AVP_dword id = 0, 
		KasperskyLicensing::LicenseObjectId::Type type = KasperskyLicensing::LicenseObjectId::OBJ_TYPE_PRODUCT
		) 
		: m_nId(id)
		, m_enumType(type)
		, m_nVersion(0)
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	static const wchar_t* GetXmlTagName()  { return L"LicenseObjectInfo"; }
	AVP_dword GetDataTreePID() const  { return MAKE_AVP_PID(m_nId, static_cast<int>(m_enumType), avpt_nothing, 0); }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	static KasperskyLicensing::LicenseObjectId::Type ConvertType(const string_t& type);
	static string_t ConvertType(KasperskyLicensing::LicenseObjectId::Type type);
private:
	typedef std::vector<KasperskyLicensing::PlatformType> PlatformVec;
	value_wrapper<AVP_dword>									m_nId;
	value_wrapper<KasperskyLicensing::LicenseObjectId::Type>	m_enumType;
	value_wrapper<string_t>										m_sName;
	value_wrapper<AVP_dword>									m_nVersion;
	PlatformVec													m_vecPlatforms;
	KeyRestrictionVec											m_vecRestrictions;
	KeyLicenseObjectInfoVec										m_vecChildrenLO;
};

////////////////////////////////////////////////////////////////////////////
class KeyInfo : public SerializableDataBase
{
public:
	KeyInfo()
		: m_nKeyVersion(0)
		, m_bKeyIsTrial(false)
		, m_nKeyLifeSpan(0)
		, m_nKeyLicenseCount(0)
		, m_bKeyHasSupport(false)
		, m_nKeyPLPos(0)
		, m_nKeyProductId(0)
		, m_nKeyMarketSector(0)
		, m_nKeySalesChannel(0)
	{
		m_dtKeyProdDate.zero_value();
		m_dtKeyExpDate.zero_value();
		m_KeySerialNumber.zero_value();
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	// xml serialize/deserialize methods
	static const wchar_t* GetXmlTagName()  { return L"KeyInfo"; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	// property serialize/deserialize methods
	static AVP_dword GetDataTreePID()  { return AVP_PID_KEYINFO; }
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	typedef std::pair<DWORD, DWORD> LicenseObjectVer4;
	typedef std::vector<LicenseObjectVer4> LicenseObjectVer4Vec;
	value_wrapper<string_t>				m_sKeyDescription;
	value_wrapper<DWORD>				m_nKeyVersion;
	value_wrapper<date_t>				m_dtKeyProdDate;
	value_wrapper<serialnumber_t>		m_KeySerialNumber;
	value_wrapper<DWORD>				m_nKeyLicenseType;
	bool								m_bKeyIsTrial;
	value_wrapper<DWORD>				m_nKeyLifeSpan;
	value_wrapper<date_t>				m_dtKeyExpDate;
	value_wrapper<DWORD>				m_nKeyLicenseCount;
	LicenseObjectVer4Vec				m_vecKeyLicense;
	value_wrapper<KeyProductInfo>		m_KeyProductInfo;
	value_wrapper<KeyLicenseObjectInfo>	m_KeyProductLicenseObjectInfo;
	bool								m_bKeyHasSupport;
	value_wrapper<string_t>				m_sKeyLicenseInfo;
	value_wrapper<string_t>				m_sKeySupportInfo;
	value_wrapper<string_t>				m_sKeyPLPosName;
	value_wrapper<DWORD>				m_nKeyPLPos;
	value_wrapper<DWORD>				m_nKeyProductId;
	value_wrapper<string_t>				m_sKeyProductVersion;
	value_wrapper<DWORD>				m_nKeyMarketSector;
	value_wrapper<DWORD>				m_nKeySalesChannel;
	value_wrapper<KeyDistributorInfo>	m_DistributorInfo;
	value_wrapper<KeyResellerInfo>		m_ResellerInfo;
	value_wrapper<KeyLocalOfficeInfo>	m_LocalOfficeInfo;
	value_wrapper<KeyHQInfo>			m_HQInfo;
	value_wrapper<KeySupportCenterInfo>	m_SupportCenterInfo;
	value_wrapper<DWORD>				m_nParthnerId;
	value_wrapper<string_t>				m_sParthnerName;
	value_wrapper<string_t>				m_sCompanyName;
	value_wrapper<KeyCustomerInfo>		m_CustomerInfo;
	value_wrapper<string_t>				m_sKeyRequestGuid;
	value_wrapper<string_t>				m_sKeyParentGuid;
	value_wrapper<DWORD>				m_nKeyParent;
	value_wrapper<DWORD>				m_nKeyCategory;
};

////////////////////////////////////////////////////////////////////////////
class KeyRoot : public SerializableDataBase
{
public:
	typedef std::pair<DWORD, DWORD> ComponentLevel;
	typedef std::vector<ComponentLevel> ComponentLevelVec;
public:
	KeyRoot()
	{
	}
	virtual void Clear();
	virtual void InitializeTemplate();
	// xml serialize/deserialize methods
	static const wchar_t* GetXmlTagName()  { return L"KeyRoot"; }
	virtual MSXML2::IXMLDOMNodePtr SerializeToXmlNode(const MSXML2::IXMLDOMDocumentPtr& spXmlDoc) const;
	virtual void DeserializeFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
	// property serialize/deserialize methods
	static AVP_dword GetDataTreePID()  { return AVP_PID_KEYROOT; }
	virtual HDATA SerializeToPropertyNode() const;
	virtual void DeserializeFromPropertyNode(HDATA hData);
private:
	static ComponentLevel DeserializeComponentLevelFromXmlNode(const MSXML2::IXMLDOMNodePtr& spXmlNode);
private:
	value_wrapper<KeyInfo> m_KeyInfo;
	ComponentLevelVec m_vecComponentLevels;
};

////////////////////////////////////////////////////////////////////////////
class KeyInfoAssembler
{
public:
	static void ReadFromKeyFile(const char* szFileName, KeyRoot& key_root);
	static void WriteToKeyFile(const char* szFileName, const KeyRoot& key_root);
	static void ReadFromXmlFile(const char* szFileName, KeyRoot& key_root);
	static void WriteToXmlFile(const char* szFileName, const KeyRoot& key_root);
private:
	KeyInfoAssembler();
	~KeyInfoAssembler();
};
