#if !defined (__SECURE_DATA_H__)
#define __SECURE_DATA_H__

#if defined (_MSC_VER)
  // disable warning 4786
  // 'identifier' : identifier was truncated to 'number' characters in the
  // debug information
  #pragma warning (disable : 4786)
  #pragma once
#endif

//-----------------------------------------------------------------------------

#include <map>
#include "lic_defs.h"
#include "err_defs.h"
#include "datatree.h"
#include "keyfile.h"

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif


namespace LicensingPolicy {

  namespace SecureData {

    using DataTree::CDataTree;

//-----------------------------------------------------------------------------

typedef struct
{
  string_t              keyFileName;        //!< key file name
  key_serial_number_t   serialNumber;       //!< key serial number
  date_t                installDate;        //!< key installation date
  date_t                expirationDate;     //!< installed key expiration date
  bool                  usedToBeActiveKey;  //!< did the key installed as active one?
} installed_key_info_t;

typedef std::pair<key_serial_number_t, installed_key_info_t> installed_keys_pair_t;
typedef std::map<key_serial_number_t, installed_key_info_t>  installed_keys_list_t;

typedef struct
{
  key_serial_number_t            activeKeySerNum;
  file_image_t                   activeKey;
  file_image_t                   reserveKey;
  key_serial_number_t            reserveKeySerNum;
  key_serial_number_t            trialKeySerNum;
  installed_keys_list_t          keysList;
  date_t                         lastKnownDate;
  date_t                         trialLimitDate;
} secure_data_t;


class CSecureData
{

public:

                      CSecureData            ();

  HRESULT             storeActiveKey         (const installed_key_info_t& keyInfo,
                                              const file_image_t&         keyFile
                                             );
  HRESULT             getActiveKey           (installed_key_info_t *keyInfo,
                                              file_image_t         *keyFile
                                             );
  HRESULT             getActiveKeyInfo       (installed_key_info_t *keyInfo);
  HRESULT             clearActiveKey         ();


  HRESULT             storeReserveKey        (const installed_key_info_t& keyInfo,
                                              const file_image_t&         keyFile
                                             );
  HRESULT             getReserveKey          (installed_key_info_t *keyInfo,
                                              file_image_t         *keyFile
                                             );
  HRESULT             getReserveKeyInfo      (installed_key_info_t *keyInfo);
  HRESULT             clearReserveKey        ();

  HRESULT             storeTrialKeyInfo      (const installed_key_info_t& key_info);
  HRESULT             getTrialKeyInfo        (installed_key_info_t *keyInfo);

  HRESULT             deleteInstalledKeyInfo (const key_serial_number_t& keyNumber);

  HRESULT             findKey                (const key_serial_number_t&  keyNumber,
                                              installed_key_info_t       *keyInfo
                                             );

  HRESULT             isActiveOrReserveKey   (const key_serial_number_t& serNum);

  HRESULT             moveReserveKeyToActive ();


  HRESULT             getKeysList            (installed_keys_list_t * keysList);


  HRESULT             serialize              (std::string *buf);
  HRESULT             deserialize            (const char *buf,
                                              size_t      size
                                             );

  HRESULT             getLastKnownDate       (date_t *);
  HRESULT             setLastKnownDate       (const date_t&);

  HRESULT             getTrialLimitDate      (date_t *);
  HRESULT             setTrialLimitDate      (const date_t&);

  HRESULT			  clear					 ();

  bool				  hasTrialKeyInvo		 () const;

private:

                      CSecureData            (const CSecureData&);
  CSecureData&        operator =             (const CSecureData&);

  installed_key_info_t*
                      findKeyInfo            (const key_serial_number_t& keyNumber);

  //  Serialization/deserialization procedures

  HRESULT             serializeKeyInfo       (HDATA node,
                                              const installed_key_info_t& keyInfo
                                             );
  HRESULT             deserializeKeyInfo     (DataTree::CDataTree::iterator& i,
                                              installed_key_info_t *keyInfo
                                             );

  HRESULT             serializeKeySerNumAndImage
                                             (HDATA node,
                                              const key_serial_number_t& sn,
                                              const file_image_t&        image
                                             );
  HRESULT             deserializeKeySerNumAndImage
                                             (DataTree::CDataTree::iterator& i,
                                              key_serial_number_t *sn,
                                              file_image_t *fileImage
                                             );
  HRESULT             serializeKeysList      ();
  HRESULT             deserializeKeysList    ();

  HRESULT             checkDataConsistency   ();


  CDataTree        m_dataTree;
  secure_data_t    m_data;

};

//-----------------------------------------------------------------------------

  }; // namespace SecureData

}; // namespace LicensingPolicy

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
