#ifndef __S_LICENSING_H__
#define __S_LICENSING_H__

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>

#include <PPP/plugin/p_licensing60.h>
#include <ProductCore/iface/i_cryptohelper.h>
#include <Updater/transport.h>

enum _ELic_IID
{
	lic_cKeySerialNumber = 0,
	lic_cLicensedObject = 1,
	lic_cComponentsLevel = 2,
	lic_cKeyInfo = 3,
	lic_cCheckInfo = 4,
	lic_cCheckInfoList = 5,
	lic_cUserData = 6,
	lic_cAuthInfo = 7,
	lic_cProxySettings = 8,
    lic_cLicInfo = 9,
	lic_cPostData = 10,
    lic_cProductIdentifer = 11,
};

//--------------------------- ESecureId ------------------------------

enum ESecureId {
    esiIsTrialKeyInstalled = 1,
	esiIsUpdatePerformed = 2,
	esiLicensingDataOld = 3,
	esiValidBasesDate = 4,
	esiValidBlackList = 5,
	esiSSLPrivkey = 6,

	esiLicensingData = CRHLP_PERSISTENT_DATA_ID + 0,
	esiLastTrialClearVer = CRHLP_PERSISTENT_DATA_ID + 1,
};

//--------------------------- EKeyType ------------------------------

enum EKeyType {
    ektUnknown = 0,
    ektBeta,
    ektTrial,
    ektTest,
    ektOEM,
    ektCommercial,
};

//--------------------------- EKeyInvalidReason ---------------------

enum EKeyInvalidReason {
    ekirUnknown = 0,
    ekirValid,
    ekirExpired,
    ekirCorrupted,
    ekirNotSigned,
    ekirWrongProduct,
    ekirNotFound,
    ekirBlackListed,
    ekirTrialAlredyUsed,
    ekirIllegalBaseUpdate,
    ekirInvalidBases,
    ekirKeyCreationDateInvalid,
    ekirTrialAlreadyInUse,
    ekirInvalidBlacklist,
    ekirInvalidUpdateDescription,
	ekirKeyInstallDateInvalid,
};

//--------------------------- EFunctionalityLevel -------------------

enum EFunctionalityLevel {
    eflUnknown = 0,
    eflNoFeatures,
    eflOnlyUpdates,
    eflFunctionWithoutUpdates,
    eflFullFunctionality,
};

//--------------------------- cKeySerialNumber  ---------------------

struct cKeySerialNumber : public cSerializable
{
    cKeySerialNumber()
        : m_dwMemberID(0),
          m_dwAppID(0),
          m_dwKeySerNum(0) {}

    DECLARE_IID(cKeySerialNumber, cSerializable, PID_LICENSING60, lic_cKeySerialNumber) ;

	int operator ==(const cKeySerialNumber& c) const
	{
		return m_dwMemberID  == c.m_dwMemberID &&
		       m_dwAppID     == c.m_dwAppID &&
		       m_dwKeySerNum == c.m_dwKeySerNum;
	}
	
	tDWORD  m_dwMemberID ;
	tDWORD  m_dwAppID ;
	tDWORD  m_dwKeySerNum ;
};


IMPLEMENT_SERIALIZABLE_BEGIN( cKeySerialNumber, "KeySerialNumber" )
	SER_VALUE( m_dwMemberID,  tid_DWORD, "MemberID"  )
	SER_VALUE( m_dwAppID,     tid_DWORD, "AppID"     )
	SER_VALUE( m_dwKeySerNum, tid_DWORD, "KeySerNum" )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cLicensedObject -----------------------

struct cLicensedObject : public cSerializable
{
    cLicensedObject()
        : m_dwKey(0),
        m_dwValue(0) {}

    DECLARE_IID(cLicensedObject, cSerializable, PID_LICENSING60, lic_cLicensedObject) ;
    
    tDWORD  m_dwKey ;
    tDWORD  m_dwValue ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cLicensedObject, "LicensedObject" )
	SER_VALUE( m_dwKey,   tid_DWORD, "Key"   )
	SER_VALUE( m_dwValue, tid_DWORD, "Value" )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cComponentsLevel ----------------------

struct cComponentsLevel : public cSerializable
{
    cComponentsLevel()
        : m_dwKey(0),
        m_dwValue(0) {}

    DECLARE_IID(cComponentsLevel, cSerializable, PID_LICENSING60, lic_cComponentsLevel) ;
    
    tDWORD  m_dwKey ;
    tDWORD  m_dwValue ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cComponentsLevel, "ComponentsLevel" )
	SER_VALUE( m_dwKey,   tid_DWORD, "Key"   )
	SER_VALUE( m_dwValue, tid_DWORD, "Value" )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cKeyInfo ------------------------------

struct cKeyInfo : public cSerializable
{
    cKeyInfo()
        : m_dwKeyVer(0),
          m_dwKeyType(ektUnknown),
          m_dwKeyLifeSpan(0),
          m_dwLicenseCount(0),
          m_dwAppID(0),
          m_dwProductID(0),
          m_dwMarketSector(0)
    {
        memset(&m_dtKeyCreationDate, 0, sizeof(m_dtKeyCreationDate)) ;
        memset(&m_dtKeyExpDate, 0, sizeof(m_dtKeyExpDate)) ;
    }

    DECLARE_IID(cKeyInfo, cSerializable, PID_LICENSING60, lic_cKeyInfo) ;

    tDWORD                      m_dwKeyVer ;
    tDT                         m_dtKeyCreationDate ;
    cKeySerialNumber            m_KeySerNum ;
    tDWORD                      m_dwKeyType ;
    tDWORD                      m_dwKeyLifeSpan ;
    tDT                         m_dtKeyExpDate ;
    tDWORD                      m_dwLicenseCount ;
    cStringObj                  m_strProductName ;
    tDWORD                      m_dwAppID ;
    tDWORD                      m_dwProductID ;
    cStringObj                  m_strProductVer ;
    cVector<cLicensedObject>    m_listLicensedObj ;
    cStringObj                  m_strLicenseInfo ;
    cStringObj                  m_strSupportInfo ;
    tDWORD                      m_dwMarketSector ;
    cVector<cComponentsLevel>   m_listCompFuncLevel ;
    cStringObj                  m_strCustomerInfo ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cKeyInfo, "KeyInfo" )
    SER_VALUE ( m_dwKeyVer,          tid_DWORD,              "KeyVer"                 )
    SER_VALUE ( m_dtKeyCreationDate, tid_DATETIME,           "KeyCreationDate"        )
    SER_VALUE ( m_KeySerNum,         cKeySerialNumber::eIID, "KeySerNum"              )
    SER_VALUE ( m_dwKeyType,         tid_DWORD,              "KeyType"                )
    SER_VALUE ( m_dwKeyLifeSpan,     tid_DWORD,              "KeyLifeSpan"            )
    SER_VALUE ( m_dtKeyExpDate,      tid_DATETIME,           "KeyExpDate"             )
    SER_VALUE ( m_dwLicenseCount,    tid_DWORD,              "LicenseCount"           )
    SER_VALUE ( m_strProductName,    tid_STRING_OBJ,         "ProductName"            )
    SER_VALUE ( m_dwAppID,           tid_DWORD,              "AppID"                  )
    SER_VALUE ( m_dwProductID,       tid_DWORD,              "ProductID"              )
    SER_VALUE ( m_strProductVer,     tid_STRING_OBJ,         "ProductVer"             )
    SER_VECTOR( m_listLicensedObj,   cLicensedObject::eIID,  "LicensedObj"            )
    SER_VALUE ( m_strLicenseInfo,    tid_STRING_OBJ,         "LicenseInfo"            )
    SER_VALUE ( m_strSupportInfo,    tid_STRING_OBJ,         "SupportInfo"            )
    SER_VALUE ( m_dwMarketSector,    tid_DWORD,              "MarketSector"           )
    SER_VECTOR( m_listCompFuncLevel, cComponentsLevel::eIID, "CompFuncLevel"          )
    SER_VALUE ( m_strCustomerInfo,   tid_STRING_OBJ,         "CustomerInfo"           )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cCheckInfo ----------------------------

struct cCheckInfo : public cSerializable
{
    cCheckInfo()
        : m_dwInvalidReason(ekirUnknown),
          m_dwDaysTillExpiration(0),
          m_dwFuncLevel(0),
          m_dwComponentFuncBitMask(0)
    {
        memset(&m_dtKeyExpirationDate, 0, sizeof(m_dtKeyExpirationDate)) ;
        memset(&m_dtKeyInstallDate,    0, sizeof(m_dtKeyInstallDate)) ;
        memset(&m_dtAppLicenseExpDate, 0, sizeof(m_dtAppLicenseExpDate)) ;
    }

    DECLARE_IID(cCheckInfo, cSerializable, PID_LICENSING60, lic_cCheckInfo) ;

    cStringObj  m_strKeyFileName ;
    cKeyInfo    m_KeyInfo ;
    tDWORD      m_dwInvalidReason ;
    tDT         m_dtKeyExpirationDate ;
    tDT         m_dtKeyInstallDate ;
    tDT         m_dtAppLicenseExpDate ;
    tDWORD      m_dwDaysTillExpiration ;
    tDWORD      m_dwFuncLevel ;
    tDWORD      m_dwComponentFuncBitMask ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cCheckInfo, "CheckInfo" )
	SER_VALUE ( m_strKeyFileName,         tid_STRING_OBJ,   "KeyFileName"              )
	SER_VALUE( m_KeyInfo,                 cKeyInfo::eIID,   "KeyInfo"                  )
    SER_VALUE ( m_dwInvalidReason,        tid_DWORD,        "InvalidReason"            )
    SER_VALUE ( m_dtKeyExpirationDate,    tid_DATETIME,     "KeyExpirationDate"        )
    SER_VALUE ( m_dtKeyInstallDate,       tid_DATETIME,     "KeyInstallDate"           )
    SER_VALUE ( m_dtAppLicenseExpDate,    tid_DATETIME,     "AppLicenseExpDate"        )
    SER_VALUE ( m_dwDaysTillExpiration,   tid_DWORD,        "DaysTillExpiration"       )
    SER_VALUE ( m_dwFuncLevel,            tid_DWORD,        "FuncLevel"                )
    SER_VALUE ( m_dwComponentFuncBitMask, tid_DWORD,        "ComponentFuncBitMask"     )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cCheckInfoList ------------------------

struct cCheckInfoList : public cSerializable
{
    DECLARE_IID(cCheckInfoList, cSerializable, PID_LICENSING60, lic_cCheckInfoList) ;

    cVector<cCheckInfo>     m_listCheckInfo ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cCheckInfoList, "CheckInfoList" )
	SER_VECTOR( m_listCheckInfo, cCheckInfo::eIID, "CheckInfo" )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cUserData ------------------------------

struct cUserData : public cSerializable
{
    cUserData()
        : m_nProdId(0), m_bTrial(cFALSE) {}

    DECLARE_IID(cUserData, cSerializable, PID_LICENSING60, lic_cUserData) ;

    tINT        m_nProdId ;         //!< идентификатор устанавливаемого продукта
    cStrObj     m_strProdVer ;      //!< версия устанавливаемого продукта
    cStrObj     m_strAgent ;        //!< идентификатор агента HTTP (User-Agent)
    cStrObj     m_strUrl ;          //!< URL сервера активации в формате http://server/path
    cStrObj     m_strKeyNum ;       //!< ключ в формате XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    cStrObj     m_strCompany ;      //!< название компании пользователя
    cStrObj     m_strName ;         //!< имя пользователя
    cStrObj     m_strCountry ;      //!< название страны пользователя
    cStrObj     m_strCity ;         //!< название города пользователя
    cStrObj     m_strAddress ;      //!< адрес пользователя
    cStrObj     m_strTelephone ;    //!< телефон пользователя
    cStrObj     m_strFax ;          //!< факс пользователя
    cStrObj     m_strEmail ;        //!< и-мейл пользователя
    cStrObj     m_strWww ;          //!< URL пользователя
	tBOOL       m_bTrial ;
	cProxySettings m_ProxySettings; //!< параметры прокси-сервера для On-Line активации
    cStrObj     m_strCustomerId ;   //!< идентификатор клиента
    cStrObj     m_strCustomerPwd ;  //!< пароль клиента
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUserData, "UserData" )
    SER_VALUE ( m_nProdId,          tid_INT,            "ProdId"     )
    SER_VALUE ( m_strProdVer,       tid_STRING_OBJ,     "ProdVer"    )
    SER_VALUE ( m_strAgent,         tid_STRING_OBJ,     "Agent"      )
    SER_VALUE ( m_strUrl,           tid_STRING_OBJ,     "Url"        )
    SER_VALUE ( m_strKeyNum,        tid_STRING_OBJ,     "KeyNum"     )
    SER_VALUE ( m_strCompany,       tid_STRING_OBJ,     "Company"    )
    SER_VALUE ( m_strName,          tid_STRING_OBJ,     "Name"       )
    SER_VALUE ( m_strCountry,       tid_STRING_OBJ,     "Country"    )
    SER_VALUE ( m_strCity,          tid_STRING_OBJ,     "City"       )
    SER_VALUE ( m_strAddress,       tid_STRING_OBJ,     "Address"    )
    SER_VALUE ( m_strTelephone,     tid_STRING_OBJ,     "Telephone"  )
    SER_VALUE ( m_strFax,           tid_STRING_OBJ,     "Fax"        )
    SER_VALUE ( m_strEmail,         tid_STRING_OBJ,     "Email"      )
    SER_VALUE ( m_strWww,           tid_STRING_OBJ,     "Www"        )
    SER_VALUE ( m_bTrial,           tid_BOOL,           "Trial"      )
	SER_VALUE ( m_ProxySettings,    cProxySettings::eIID, "ProxySettings")
    SER_VALUE ( m_strCustomerId,    tid_STRING_OBJ,     "CustomerId" )
    SER_VALUE ( m_strCustomerPwd,   tid_STRING_OBJ,     "CustomerPwd")
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cUserData ------------------------------

struct cPostData : public cSerializable
{
    DECLARE_IID(cPostData, cSerializable, PID_LICENSING60, lic_cPostData) ;
	
    cStrObj        m_header;
	cVector<tBYTE> m_postData;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPostData, "UserData" )
	SER_VALUE_M ( header,   tid_STRING_OBJ)
	SER_VECTOR_M( postData, tid_BYTE)
IMPLEMENT_SERIALIZABLE_END();


//--------------------------- cAuthInfo -----------------------------

struct cAuthInfo : public cSerializable
{
    DECLARE_IID(cAuthInfo, cSerializable, PID_LICENSING60, lic_cAuthInfo) ;

    cStrObj     m_strUserName ;
    cStrObj     m_strUserPassword ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAuthInfo, "AuthInfo" )
    SER_VALUE ( m_strUserName,       tid_STRING_OBJ,     "UserName"     )
    SER_VALUE ( m_strUserPassword,   tid_STRING_OBJ,     "UserPassword" )
IMPLEMENT_SERIALIZABLE_END();

//--------------------------- cLicInfo -----------------------------

struct cLicInfo : public cSerializable
{
	cLicInfo() :
		m_blTrialPeriodOver(cFALSE)
	{}
    
	DECLARE_IID(cLicInfo, cSerializable, PID_LICENSING60, lic_cLicInfo) ;

    tBOOL       m_blTrialPeriodOver ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cLicInfo, "LicInfo" )
    SER_VALUE ( m_blTrialPeriodOver,    tid_BOOL,   "TrialPeriodOver" )
IMPLEMENT_SERIALIZABLE_END();


//--------------------------- Product Identifier -----------------------------

struct cLicProductIdentifier : public cSerializable
{
    cLicProductIdentifier()
        : m_nProdId(0)
    {
    }
	DECLARE_IID(cLicProductIdentifier, cSerializable, PID_LICENSING60, lic_cProductIdentifer) ;

    // идентификатор устанавливаемого продукта
    tINT m_nProdId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cLicProductIdentifier, "ProductIdentifier" )
    SER_VALUE ( m_nProdId,          tid_INT,            "ProdId"     )
IMPLEMENT_SERIALIZABLE_END();

//-------------------------------------------------------------------

#endif//  __S_LICENSING_H__
