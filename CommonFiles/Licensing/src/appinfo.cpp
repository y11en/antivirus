
#include "../include/appinfo.h"

#if defined (_MSC_VER)
#pragma warning (push, 4)
#endif

using namespace LicensingPolicy;
using DataTree::makeAddr;

//-----------------------------------------------------------------------------

bool CAppInfo::loadAppInfo (const string_t& fileName)
{

  if (!m_dataTree.load (fileName))
    return (false);

  return (doLoading ());


} // CAppInfo::loadAppInfo (DataTree::CDataTree*)

//-----------------------------------------------------------------------------

bool CAppInfo::loadAppInfo (const char *buf, size_t size)
{

  if (!buf) 
  {
    return (false);
  }

  if (!m_dataTree.load (buf, (dword_t)size))
    return (false);

  return (doLoading ());


}

//-----------------------------------------------------------------------------

bool CAppInfo::doLoading ()
{
  return (extractDescription ()        &&
          extractVersion ()            &&
          extractCreationDate ()       &&
          extractAppId ()              &&
          extractAppName ()            &&
          extractAppVersion ()         &&
          extractIsRelease ()          &&
          extractProductList ()        &&
          extractComponentList ()      &&
          extractPublicKeyList ()
         );
}

//-----------------------------------------------------------------------------

bool CAppInfo::extractDescription ()
{
  return (0 != m_dataTree.get (makeAddr (KL_PID_APPINFO, 
    KL_PID_APPINFO_DESCRIPTION), &m_appInfo.description));
}

//-----------------------------------------------------------------------------

bool CAppInfo::extractVersion ()
{
  return (0 != m_dataTree.get (makeAddr (KL_PID_APPINFO, KL_PID_APPINFO_VERSION), 
    &m_appInfo.version));
}

//-----------------------------------------------------------------------------

bool CAppInfo::extractCreationDate ()
{
  DataTree::AVP_date_t d;

  if (!m_dataTree.get (makeAddr (KL_PID_APPINFO, KL_PID_APPINFO_DATECREATE),
      &d))
    return (0);

  m_appInfo.creationDate.m_year  = d.dateParts.year;
  m_appInfo.creationDate.m_month = d.dateParts.month;
  m_appInfo.creationDate.m_day   = d.dateParts.day;

  return (1);
}

//-----------------------------------------------------------------------------

bool CAppInfo::extractAppId ()
{
  return (0 != m_dataTree.get (makeAddr (KL_PID_APPINFO, KL_PID_APPINFO_APPID),
    &m_appInfo.id));
}


//-----------------------------------------------------------------------------

bool CAppInfo::extractAppName ()
{
  return (0 != m_dataTree.get (makeAddr (KL_PID_APPINFO, KL_PID_APPINFO_APPNAME), 
    &m_appInfo.name));
}

//-----------------------------------------------------------------------------

bool CAppInfo::extractAppVersion ()
{
  return (0 != m_dataTree.get (makeAddr (KL_PID_APPINFO, KL_PID_APPINFO_APPVERSION),
    &m_appInfo.ver));
}


//-----------------------------------------------------------------------------

bool CAppInfo::extractIsRelease ()
{
  m_appInfo.isRelease = 0 != m_dataTree.findData (makeAddr (KL_PID_APPINFO, 
    KL_PID_APPINFO_ISRELEASE));
    
  return (true);
}


//-----------------------------------------------------------------------------

bool CAppInfo::extractProductList  ()
{
  product_info_t pi;
  HDATA        hData;
  DataTree::CDataTree::iterator i 
    (m_dataTree.begin (m_dataTree.findData (makeAddr (KL_PID_PRODUCTS))));

  AVP_dword* ProductVerAddr = makeAddr (KL_PID_PRODUCT_VERSION);
  AVP_dword* ProductPubKeyId = makeAddr (KL_PID_PRODUCT_PUBLICKEY);

  m_appInfo.prodInfoLst.clear ();


  while (i != m_dataTree.end ()) 
  {
    
    // Extract product id
    pi.productId = GET_AVP_PID_ID (DATA_Get_Id (*i, 0));
    
    // Extract product name
    m_dataTree.get (*i, &pi.productName);

    // Extract product major version (if presented)
    hData = m_dataTree.findData (*i, ProductVerAddr);
    if (hData)
      m_dataTree.get (hData, &pi.majorVersion);

    // Extract product public key id (if presented)
    hData = m_dataTree.findData (*i, ProductPubKeyId);
    if (hData)
      m_dataTree.get (m_dataTree.findData (*i, ProductPubKeyId), &pi.pubKeyId);

    m_appInfo.prodInfoLst.insert (product_list_pair_t (pi.productId, pi));

    i++;
  }

  return (true);
}

//-----------------------------------------------------------------------------

bool CAppInfo::extractComponentList ()
{
  DataTree::CDataTree::iterator i 
    (m_dataTree.begin (m_dataTree.findData (makeAddr (KL_PID_COMPONENTS))));
  AVP_dword* idAddr = makeAddr (KL_PID_COMPONENTS_ID);
  HDATA        hData;
  dword_t      componentId;
  dword_t      funcLevel;

  m_appInfo.componentList.clear ();

  while (i != m_dataTree.end ()) 
  {
    funcLevel = 0;
    componentId = 0;

    m_dataTree.get (*i, &funcLevel);
    hData = m_dataTree.findData (idAddr);
    if (hData) 
    {
      m_dataTree.get (hData, &componentId);
    }

    m_appInfo.componentList.insert (component_list_pair_t (componentId, funcLevel));
    i++;
  }

  return (true);

}

//-----------------------------------------------------------------------------

bool CAppInfo::extractPublicKeyList ()
{
  public_key_t pk;
  DataTree::CDataTree::iterator i 
    (m_dataTree.begin (m_dataTree.findData (makeAddr (KL_PID_PUBLICKEYS))));

  m_appInfo.publicKeyList.clear ();

  while (i != m_dataTree.end ())
  {
    m_dataTree.get (*i, &pk.publicKey);
    m_appInfo.publicKeyList.push_back (pk);
    i++;
  }

  return (true);
}

//-----------------------------------------------------------------------------
