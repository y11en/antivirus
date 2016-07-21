#ifndef __KEYFILE_H__
#define __KEYFILE_H__

#if defined (_MSC_VER)
  #pragma once
  #pragma warning (disable : 4786)
#endif

#include "lic_defs.h"
#include "datatree.h"
#include <memory>
#include <vector>
#include <map>

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

namespace LicensingPolicy
{

using DataTree::CDataTree;

enum restriction_type_t
{
  rtUnknown          = 0,
  rtNodes            = 1,
  rtEmails           = 2,
  rtDailyTraffic     = 3,
  rtConnections      = 4,
  rtCheckingThreads  = 5,
  rtTotalTraffic     = 6
};

//-----------------------------------------------------------------------------

/*!
 * \brief Kaspersky lab license key serial number consist from three parts:
 *        Customer Id- Application Id - Key serial number. Nevertheless, 
 *        Key serial number uniqely identifies license key
*/
struct key_serial_number_t
{
 
      key_serial_number_t ();
    
  bool operator == (const key_serial_number_t& sn) const;
  bool operator != (const key_serial_number_t& sn) const;
  bool operator < (const key_serial_number_t& sn) const;
  bool empty () const;
  void clear ();

  union 
  {
    struct
    {
      dword_t memberId;
      dword_t appId;
      dword_t keySerNum;
    } parts;

    dword_t number[3];
    
  } number;

}; // struct key_serial_number_t

typedef std::pair<dword_t, dword_t>       licensed_object_pair_t;
typedef std::map<dword_t, dword_t>        licensed_object_list_t;
typedef std::pair<dword_t, dword_t>       component_level_pair_t;
typedef std::map<dword_t, dword_t>        components_level_list_t;

enum key_type_t
{
  ktUnknown = 0,
  ktBeta,
  ktTrial,
  ktTest,
  ktOEM,
  ktCommercial
};

struct key_info_t
{

                    key_info_t ();
//  key_info_t&       operator = (const key_info_t&);

  dword_t                    keyVer;            //!< key file structure version      
  date_t                     keyCreationDate;   //!< key file creation date  
  key_serial_number_t        keySerNum;         //!< key file serial number 
  key_type_t                 keyType;           //!< key type (commercial, trial, ...) 
  dword_t                    keyLifeSpan;       //!< key validity period since installation (days)
  date_t                     keyExpDate;        //!< key limit validity period (days)
  dword_t                    licenseCount;      //!< license number (key ver 1-3)
  string_t                   productName;       //!< product name
  dword_t                    appId;             //!< application name
  dword_t                    productId;         //!< product id
  string_t                   productVer;        //!< major product version
  licensed_object_list_t     licensedObjLst;    //!< licensed objects with license count
  string_t                   licenseInfo;       //!< descriptional license info
  string_t                   supportInfo;       //!< support company properties
  dword_t                    marketSector;      //!< market sector 
  components_level_list_t    compFuncLevel;     //!< components functionality levels list
  string_t                   customer_info;     //!< customer info
};

class CKeyInfo
{
public:

                          CKeyInfo    ();
                          ~CKeyInfo   ();

  bool                    loadKeyInfo (const string_t& fileName);
  bool                    loadKeyInfo (const char *s, size_t size);

  const key_info_t&       getKeyInfo  () const;       

private:

                          CKeyInfo     (const CKeyInfo&);
  CKeyInfo&               operator =   (const CKeyInfo&);

  bool                    doLoading    ();

  bool                    extractDescription     ();
  bool                    extractKeyVersion      ();
  bool                    extractKeyCreationDate ();
  bool                    extractKeySerialNumber ();
  bool                    extractKeyType         ();
  bool                    extractLifeSpan        ();
  bool                    extractExpDate         ();
  bool                    extractLicenseCount    ();
  bool                    extractProductName     ();
  bool                    extractAppId           ();
  bool                    extractProductId       ();
  bool                    extractProductMajorVer ();
  bool                    extractLicensedObjList ();
  bool                    extractLicenseInfo     ();
  bool                    extractSupportInfo     ();
  bool                    extractCustomerInfo    ();
  bool                    extractCompFuncLevel   ();
    
  CDataTree               m_dataTree;
  key_info_t              m_keyInfo;

};

//-----------------------------------------------------------------------------

inline key_serial_number_t::key_serial_number_t ()
{
  number.parts.memberId = 0;
  number.parts.appId = 0;
  number.parts.keySerNum = 0;
}

//-----------------------------------------------------------------------------
inline key_info_t::key_info_t () :
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

inline CKeyInfo::CKeyInfo ()
{
}

//-----------------------------------------------------------------------------

inline CKeyInfo::~CKeyInfo ()
{
}

//-----------------------------------------------------------------------------

inline const key_info_t& CKeyInfo::getKeyInfo () const
{
  return (m_keyInfo);
}

//=============================================================================

inline bool key_serial_number_t::operator == (const key_serial_number_t& sn) const
{
  return (number.parts.keySerNum  == sn.number.parts.keySerNum);
}

//-----------------------------------------------------------------------------

inline bool key_serial_number_t::operator != (const key_serial_number_t& sn) const
{
  return (! (*this == sn));
}

//-----------------------------------------------------------------------------

inline bool key_serial_number_t::operator < (const key_serial_number_t& sn) const
{
  return (number.parts.keySerNum < sn.number.parts.keySerNum);
}

//-----------------------------------------------------------------------------

inline bool key_serial_number_t::empty () const
{
  return (0 == number.parts.keySerNum);
}

//-----------------------------------------------------------------------------

inline void key_serial_number_t::clear ()
{
  number.parts.appId = 0;
  number.parts.memberId = 0;
  number.parts.keySerNum = 0;
}

//-----------------------------------------------------------------------------

} // namespace LicensingPolicy

#if defined (_MSC_VER)
#pragma warning (pop)
#endif

#endif
