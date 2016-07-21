#include "../../loadkeys/avpkeyid.h"
#include "../include/keyfile.h"

#if defined (_MSC_VER)
#pragma warning (push, 4)
#endif

using namespace LicensingPolicy;
using DataTree::makeAddr;

enum raw_key_type_t
{
  rktUnknown        = 0,
  rktCommercial     = 1,
  rktBeta           = 2,
  rktTrial          = 3,
  rktTest           = 4, 
  rktOEM            = 5
};


//-----------------------------------------------------------------------------
/*
key_serial_number_t::key_serial_number_t ()
{
  number.parts.memberId = 0;
  number.parts.appId = 0;
  number.parts.keySerNum = 0;
}

//-----------------------------------------------------------------------------

key_info_t::key_info_t () :
  keyVer (0),
  keyType (ktUnknown),
  keyLifeSpan (0),
  licenseCount (0),
  appId (0),
  productId (0),
  marketSector (0)
  
{
  keySerNum.clear ();
  keyCreationDate.clear ();
  keyExpDate.clear ();
}

//-----------------------------------------------------------------------------

key_info_t& key_info_t::operator = (const key_info_t& ki)
{
  
  keyVer                    = ki.keyVer;
  keyCreationDate           = ki.keyCreationDate;
  keySerNum                 = ki.keySerNum;
  keyType                   = ki.keyType;
  keyLifeSpan               = ki.keyLifeSpan;
  keyExpDate                = ki.keyExpDate;
  licenseCount              = ki.licenseCount;
  productName               = ki.productName;
  appId                     = ki.appId;
  productId                 = ki.productId;
  productVer                = ki.productVer;
  licensedObjLst            = ki.licensedObjLst;
  licenseInfo               = ki.licenseInfo;
  supportInfo               = ki.supportInfo;
  marketSector              = ki.marketSector;
  compFuncLevel             = ki.compFuncLevel;
  customer_info             = ki.customer_info;

  return (*this);

}
*/
//-----------------------------------------------------------------------------

bool CKeyInfo::loadKeyInfo (const string_t& fileName)
{
	bool	bRet;

	try
	{
		if (!m_dataTree.load (fileName)) return (false);
		bRet = doLoading ();
	}
	catch (...) 
	{
		bRet = false;
	}

	return (bRet);

}

//-----------------------------------------------------------------------------

bool CKeyInfo::loadKeyInfo (const char *s, size_t size)
{

	if (!s) return (false);

	bool	bRet;

	try
	{
		if (!m_dataTree.load (s, (dword_t)size)) return (false);
		bRet = doLoading ();
	}
	catch (...) 
	{
		bRet = false;
	}

	return (bRet);

}

//-----------------------------------------------------------------------------

bool CKeyInfo::doLoading ()
{
  return (extractKeyVersion ()        && 
          extractKeyCreationDate ()   &&
          extractKeySerialNumber ()   &&
          extractKeyType ()           &&
          extractLifeSpan ()          &&
          extractExpDate ()           &&
          extractLicenseCount ()      &&
          extractLicenseInfo ()       &&
          extractCustomerInfo ()      &&
          extractSupportInfo ()       &&
          extractProductName ()       &&
          extractAppId ()             &&
          extractProductId ()         &&
          extractProductMajorVer ()   &&
          extractLicensedObjList ()
         );
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractKeyVersion ()
{
  return (0 != m_dataTree.get (makeAddr (AVP_PID_KEYINFO, AVP_PID_KEYVERSION),
    &m_keyInfo.keyVer));
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractKeyCreationDate ()
{
  DataTree::AVP_date_t d;

  if (!m_dataTree.get (makeAddr (AVP_PID_KEYINFO, AVP_PID_KEY_PROD_DATE), &d))
    return (0);

  m_keyInfo.keyCreationDate.m_year  = d.dateParts.year;
  m_keyInfo.keyCreationDate.m_month = d.dateParts.month;
  m_keyInfo.keyCreationDate.m_day   = d.dateParts.day;

  return (1);

}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractKeySerialNumber ()
{

  return (0 != m_dataTree.get (makeAddr (AVP_PID_KEYINFO, AVP_PID_KEYSERIALNUMBER),
    &m_keyInfo.keySerNum, sizeof (m_keyInfo.keySerNum)));
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractKeyType ()
{
  dword_t kt;
  dword_t r = m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYLICENCETYPE),
                               &kt);

  if (!r)
  {
    // Old key format. Check AVP_PID_KEYISTRIAL node. Key is trial if node is
    // presented and commercial otherwise.
    if (!m_dataTree.findData (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYISTRIAL))) 
    {
      m_keyInfo.keyType = ktCommercial;
    } 
    else
    {
      m_keyInfo.keyType = ktTrial;
    }
    return (true);
  }

  switch (kt)
  {
    case rktCommercial : m_keyInfo.keyType = ktCommercial; break;
    case rktBeta       : m_keyInfo.keyType = ktBeta; break;
    case rktTrial      : m_keyInfo.keyType = ktTrial; break;
    case rktTest       : m_keyInfo.keyType = ktTest; break;
    case rktOEM        : m_keyInfo.keyType = ktOEM; break;
    default            : m_keyInfo.keyType = ktUnknown; break;
  }
  return (true);
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractLifeSpan ()
{
  return (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYLIFESPAN), 
    &m_keyInfo.keyLifeSpan));
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractExpDate ()
{

  DataTree::AVP_date_t d;

  if (!m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYEXPDATE), &d))
    return (0);

  m_keyInfo.keyExpDate.m_year  = d.dateParts.year;
  m_keyInfo.keyExpDate.m_month = d.dateParts.month;
  m_keyInfo.keyExpDate.m_day   = d.dateParts.day;

  return (1);

}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractLicenseCount ()
{
  return (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYLICENCECOUNT), 
    &m_keyInfo.licenseCount));
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractProductName ()
{
  int r = m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYPLPOSNAME),
    &m_keyInfo.productName);

  if (!r) 
  {
    m_keyInfo.productName = _T ("");
  }
  return (true);
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractProductMajorVer ()
{
  int r = m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEY_PRODUCTVERSION),
    &m_keyInfo.productVer);

  if (!r) 
  {
    m_keyInfo.productVer = _T ("");
  }
  return (true);
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractAppId ()
{
  return (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYPLPOS),
    &m_keyInfo.appId));
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractProductId ()
{
  int r = m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEY_PRODUCTID),
    &m_keyInfo.productId);

  if (!r) 
  {
    m_keyInfo.productId = 0;
  }
  return (true);
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractLicensedObjList ()
{
  DataTree::CDataTree::iterator i
    (m_dataTree.begin (m_dataTree.findData (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYLICENCE))));
  dword_t licObjId;
  dword_t licenseNumber;

  while (i != m_dataTree.end ()) 
  {
    licObjId = GET_AVP_PID_ID (DATA_Get_Id (*i, 0));
    m_dataTree.get (*i, &licenseNumber);

    m_keyInfo.licensedObjLst.insert (licensed_object_pair_t (licObjId, licenseNumber));
    i++;
  }

  return (true);
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractLicenseInfo ()
{
  return (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYLICENCEINFO),
    &m_keyInfo.licenseInfo));
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractCustomerInfo ()
{

  string_t s;
  string_t().swap(m_keyInfo.customer_info);

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_COMPANYNAME), &s))
    m_keyInfo.customer_info = s;

  m_keyInfo.customer_info += _TCHAR('\n');

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_NAME), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_COUNTRY), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');
 
  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_CITY), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_ADDRESS), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_PHONE), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_FAX), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');

  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_EMAIL), &s))
    m_keyInfo.customer_info += s;

  m_keyInfo.customer_info += _TCHAR('\n');
  
  if (0 != m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYCUSTOMER, AVP_PID_KEYCUSTOMER_WWW), &s))
    m_keyInfo.customer_info += s;
	
  m_keyInfo.customer_info += _TCHAR('\n');

  return (true);
  
}

//-----------------------------------------------------------------------------

bool CKeyInfo::extractSupportInfo ()
{
  m_dataTree.get (makeAddr (AVP_PID_KEYROOT, AVP_PID_KEYSUPPORTINFO),
      &m_keyInfo.supportInfo);
  return (true);
};

//-----------------------------------------------------------------------------

bool CKeyInfo::extractCompFuncLevel ()
{
  return (false);
}

//-----------------------------------------------------------------------------
