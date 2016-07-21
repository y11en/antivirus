#ifndef __APPINFO_H__
#define __APPINFO_H__

#if defined (_MSC_VER)
#pragma warning (disable : 4786)
#endif

#include <vector>
#include <map>
#include "lic_defs.h"
#include "datatree.h"

#if defined (_MSC_VER)
#pragma warning (push, 4)
#endif

namespace LicensingPolicy
{

using DataTree::CDataTree;

//-----------------------------------------------------------------------------

//! Product info
struct product_info_t
{
                product_info_t () : productId (0), pubKeyId (0)
                {
                }
  int    operator < (const product_info_t& pi)
  {
    return (productId < pi.productId);
  }

  dword_t    productId;
  string_t   productName;
  string_t   majorVersion;
  dword_t    pubKeyId;
};


typedef std::pair<dword_t, product_info_t> product_list_pair_t;
typedef std::map<dword_t, product_info_t> product_list_t;

//! Component info (component id, functionality level)
typedef std::pair<dword_t, dword_t> component_list_pair_t;
typedef std::map<dword_t, dword_t> component_list_t;

struct public_key_t 
{
  string_t  publicKey;
};

typedef std::vector<public_key_t> publickey_list_t;

enum app_info_type_t
{
  itUnknown = 0,
  itApplicationInfo, 
  itComponentInfo
};

//! Component functionality level bit mask in particular app context
//! (application id, functionality level bit mask)
typedef std::pair<dword_t, dword_t> app_component_pair_t;
typedef std::map<dword_t, dword_t> app_component_list_t;

//! Legacy application compatibility list
//! (aplication id, application name)
typedef std::pair<dword_t, string_t> app_compat_pair_t;
typedef std::map<dword_t, string_t>  app_compat_list_t;

//! Application or component info
struct app_info_t
{
  string_t             description;
  dword_t              version;           //!< Structure version
  date_t               creationDate;      //!< Structure creattion date
  app_info_type_t      infoType;          //!< Information type (app or component)
  dword_t              id;                //!< App or component id (depends on infoType)
  string_t             name;              //!< App or component name 
  string_t             ver;               //!< App or component version
  bool                 isRelease;         //!< Release or beta 
  product_list_t       prodInfoLst;       //!< Products list
  component_list_t     componentList;     //!< Component list (app only) 
  app_component_list_t appList;           //!< Apps that might contain component (component only)
  publickey_list_t     publicKeyList;     //!< Public keys list
  app_compat_list_t    appCompatList;     //!< Legacy apps compatibility list
};

//-----------------------------------------------------------------------------

class CAppInfo
{
public:

                     CAppInfo        ();

  bool               loadAppInfo     (const string_t& fileName);
  bool               loadAppInfo     (const char *buf, size_t size);

  const app_info_t&  getAppInfo      () const;


private:
     
                     CAppInfo        (const CAppInfo&);
  CAppInfo&          operator =      (const CAppInfo&);

  bool               doLoading       ();

  bool               extractDescription   ();
  bool               extractVersion       ();
  bool               extractCreationDate  ();
  bool               extractAppId         ();
  bool               extractAppName       ();
  bool               extractAppVersion    ();
  bool               extractIsRelease     ();
  bool               extractProductList   ();
  bool               extractComponentList ();
  bool               extractPublicKeyList ();

  app_info_t         m_appInfo;
  CDataTree          m_dataTree;
};

//-----------------------------------------------------------------------------

inline CAppInfo::CAppInfo ()
{
}

//-----------------------------------------------------------------------------

inline const app_info_t& CAppInfo::getAppInfo () const
{
  return (m_appInfo);
}

//-----------------------------------------------------------------------------

}; // namespace LicensingPolicy

#if defined (_MSC_VER)
#pragma warning (pop)
#endif

#endif
