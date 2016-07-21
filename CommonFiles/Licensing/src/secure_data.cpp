#include "../include/err_defs.h"
#include "../include/secure_data.h"
#include "../../sign/sign.h"

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif


using namespace LicensingPolicy;
using namespace LicensingPolicy::SecureData;

const dword_t ROOT                  = MAKE_AVP_PID (1, 8, avpt_nothing, 0);
const dword_t ACTIVE_KEY            = MAKE_AVP_PID (1, 1, avpt_nothing, 0);
const dword_t RESERVE_KEY           = MAKE_AVP_PID (2, 1, avpt_nothing, 0);
const dword_t TRIAL_KEY             = MAKE_AVP_PID (3, 1, avpt_bin, 0);
const dword_t KEYS_LIST             = MAKE_AVP_PID (4, 1, avpt_dword, 0);
const dword_t LAST_KNOWN_DATE       = MAKE_AVP_PID (5, 1, avpt_bin, 0);
const dword_t TRIAL_LIMIT_DATE      = MAKE_AVP_PID (6, 1, avpt_bin, 0);

const dword_t KEY_FILE_NAME         = MAKE_AVP_PID (1, 1, avpt_bin, 0);
const dword_t KEY_SERIAL_NUMBER     = MAKE_AVP_PID (2, 1, avpt_bin, 0);
const dword_t KEY_INSTALL_DATE      = MAKE_AVP_PID (3, 1, avpt_bin, 0);
const dword_t KEY_EXP_DATE          = MAKE_AVP_PID (4, 1, avpt_bin, 0);
const dword_t KEY_USED_TO_BE_ACTIVE = MAKE_AVP_PID (5, 1, avpt_bin, 0);
const dword_t KEY_LAST_VIEW_DATE    = MAKE_AVP_PID (6, 1, avpt_bin, 0);
const dword_t APP_ID                = MAKE_AVP_PID (7, 1, avpt_dword, 0);

//-----------------------------------------------------------------------------

CSecureData::CSecureData ()
{
  m_data.activeKeySerNum.clear ();
  m_data.reserveKeySerNum.clear ();
  m_data.trialKeySerNum.clear ();
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::serialize (std::string *buf)
{

  if (!buf)
  {
    return (E_INVALIDARG);
  }

// Write data to tree

  if (!m_dataTree.createTree (ROOT))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  HDATA node;

  // Active key info

  if (! m_data.activeKeySerNum.empty ())
  {
    if (!(node = m_dataTree.addData (m_dataTree.getRoot (),
                                      ACTIVE_KEY, 
                                      0
                                     )))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

    if (FAILED (serializeKeySerNumAndImage (node, m_data.activeKeySerNum, m_data.activeKey))) 
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

  }

  // Reserve key info

  if (! m_data.reserveKeySerNum.empty ())
  {
    if (!(node = m_dataTree.addData (m_dataTree.getRoot (), 
                                      RESERVE_KEY, 
                                      0
                                     )))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

    if (FAILED (serializeKeySerNumAndImage (node, m_data.reserveKeySerNum, m_data.reserveKey))) 
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }
   
  }

  // Trial key info

  if (! m_data.trialKeySerNum.empty ())
  {
    if (!(node = m_dataTree.addData (m_dataTree.getRoot (), 
                                      TRIAL_KEY,
                                      &m_data.trialKeySerNum,
                                      sizeof (m_data.trialKeySerNum)
                                     )))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

  }

  // Keys info list

  if (FAILED (serializeKeysList ())) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Last known date

  if (! (node = m_dataTree.addData (m_dataTree.getRoot (), 
                                    LAST_KNOWN_DATE,
                                    &m_data.lastKnownDate,
                                    sizeof (m_data.lastKnownDate)
                                   ))) 
  {
      return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Trial limit date

  if (! (node = m_dataTree.addData (m_dataTree.getRoot (), 
                                    TRIAL_LIMIT_DATE,
                                    &m_data.trialLimitDate,
                                    sizeof (m_data.trialLimitDate)
                                   ))) 
  {
      return (LIC_E_SECURE_DATA_CORRUPT);
  }
  
// Serialize tree to memory buffer

  char              *s;
  DataTree::dword_t  size;


 if (m_dataTree.save (&s, &size))
 {
   try
   {
     buf->assign (s, size);
     delete [] s;
     return (S_OK);
   }
   catch (const std::bad_alloc&)
   {
     delete [] s;
     return (E_OUTOFMEMORY);
   }

 }
 else
 {
   return (LIC_E_SECURE_DATA_CORRUPT);
 }

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::deserialize (const char *buf,
                                  size_t      size
                                 )
{

  if (!buf || !size) 
  {
    return (E_INVALIDARG);
  }

  if (! m_dataTree.load (buf, (dword_t)size)) 
  {
	m_dataTree.clear ();
	return (LIC_E_SECURE_DATA_CORRUPT);
  }

  DataTree::CDataTree::iterator i (m_dataTree.begin (
    m_dataTree.findData (DataTree::makeAddr (ACTIVE_KEY))));

  if (i != m_dataTree.end ())
  {
    if (S_OK != deserializeKeySerNumAndImage (i, &m_data.activeKeySerNum, &m_data.activeKey))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }
  }
  else
  {
    // No active key
    m_data.activeKey.image.resize (0);
    m_data.activeKeySerNum.clear ();
  }

  // Load reserve key

  if (!m_data.activeKeySerNum.empty ())
  {
    // Reserve key can exist only if there is active one
    i = m_dataTree.begin (m_dataTree.findData (DataTree::makeAddr (RESERVE_KEY)));

    if (i != m_dataTree.end()) 
    {
      if (S_OK != deserializeKeySerNumAndImage (i, &m_data.reserveKeySerNum, &m_data.reserveKey))
      {
        return (LIC_E_SECURE_DATA_CORRUPT);
      }
    }
    else
    {
      // No reserve key
      m_data.reserveKey.image.resize (0);
      m_data.reserveKeySerNum.clear ();
    }

  }
  else
  {
    // No reserve key
    m_data.reserveKey.image.resize (0);
    m_data.reserveKeySerNum.clear ();
  }

  // Load trial key serial number

  HDATA hData = m_dataTree.findData (DataTree::makeAddr (TRIAL_KEY));

  if (hData) 
  {
    if (!m_dataTree.get (hData, &m_data.trialKeySerNum, sizeof (m_data.trialKeySerNum))) 
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }
  }

  if (FAILED (deserializeKeysList ())) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Last known date
  hData = m_dataTree.findData (DataTree::makeAddr (LAST_KNOWN_DATE));
  if (hData) 
  {
      if (!m_dataTree.get (hData, &m_data.lastKnownDate, sizeof (m_data.lastKnownDate)))
      {
          return (LIC_E_SECURE_DATA_CORRUPT);
      }
  }
  else
  {
      m_data.lastKnownDate.clear ();
  }

  // Trial limit date
  hData = m_dataTree.findData (DataTree::makeAddr (TRIAL_LIMIT_DATE));
  if (hData) 
  {
      if (!m_dataTree.get (hData, &m_data.trialLimitDate, sizeof (m_data.trialLimitDate)))
      {
          return (LIC_E_SECURE_DATA_CORRUPT);
      }
  }
  else
  {
      m_data.trialLimitDate.clear ();
  }

  if (FAILED (checkDataConsistency ()))
    return (LIC_E_SECURE_DATA_CORRUPT);
  
  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::clear ()
{
	m_dataTree.clear ();
	m_data.activeKey.image.erase (m_data.activeKey.image.begin (), m_data.activeKey.image.end ());
	m_data.activeKeySerNum.clear ();
	m_data.keysList.erase (m_data.keysList.begin (), m_data.keysList.end ());
	m_data.lastKnownDate.clear ();
	m_data.reserveKey.image.erase (m_data.reserveKey.image.begin (), m_data.reserveKey.image.end ());
	m_data.reserveKeySerNum.clear ();
	m_data.trialKeySerNum.clear ();
	m_data.trialLimitDate.clear ();
	return (S_OK);
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::serializeKeyInfo (HDATA node, 
                                       const installed_key_info_t& keyInfo
                                      )
{

  if (! m_dataTree.addData (node, 
                            KEY_FILE_NAME,
                            keyInfo.keyFileName.c_str (),
                            (dword_t)keyInfo.keyFileName.size () * sizeof (_TCHAR)
                           ))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  if (! m_dataTree.addData (node,
                            KEY_SERIAL_NUMBER,
                            &keyInfo.serialNumber,
                            sizeof (keyInfo.serialNumber)
                           ))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  if (! m_dataTree.addData (node, 
                            KEY_INSTALL_DATE, 
                            &keyInfo.installDate,
                            sizeof (keyInfo.installDate)
                           ))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  if (! m_dataTree.addData (node,
                            KEY_EXP_DATE,
                            &keyInfo.expirationDate,
                            sizeof (keyInfo.expirationDate)
                           ))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  if (! m_dataTree.addData (node,
                            KEY_USED_TO_BE_ACTIVE,
                            &keyInfo.usedToBeActiveKey,
                            sizeof (keyInfo.usedToBeActiveKey)
                           )) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::deserializeKeyInfo (DataTree::CDataTree::iterator& i, 
                                         installed_key_info_t *keyInfo
                                        )
{

  if (!keyInfo)
  {
    return (E_INVALIDARG);
  }


  if (i == m_dataTree.end ()) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Extract key file name

  _TCHAR    *s;
  dword_t    size;

  if (! m_dataTree.get (*i, reinterpret_cast<void**> (&s), &size))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  try
  {

    keyInfo->keyFileName.assign (s, size / sizeof (_TCHAR));
    delete [] s;
  }

  catch (const std::bad_alloc&) 
  {
    delete [] s;
    return (E_OUTOFMEMORY);
  }

  // Extract key serial number

  i++;

  if (! m_dataTree.get (*i, 
                         &keyInfo->serialNumber, 
                         sizeof (key_serial_number_t)))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Extract key installation date

  i++;

  if (! m_dataTree.get (*i, &keyInfo->installDate, sizeof (keyInfo->installDate))) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Extract key expiration date

  i++;

  if (! m_dataTree.get (*i, &keyInfo->expirationDate, sizeof (keyInfo->expirationDate))) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  // Extract boolean flag indicating whether the key used to be active key or not

  i++;

  if (i == m_dataTree.end ())
  {
      // Assume that licensing info we are processing is of older version
      // with no "usedToBeActiveKey" flag defined.
      keyInfo->usedToBeActiveKey = false;
  }
  else
  {
      if (! m_dataTree.get (*i, &keyInfo->usedToBeActiveKey, sizeof (keyInfo->usedToBeActiveKey))) 
      {
        return (LIC_E_SECURE_DATA_CORRUPT);
      }
  }

  return (S_OK);
 
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getKeysList (installed_keys_list_t *keysList)
{
  if (!keysList)
  {
    return (E_INVALIDARG);
  }

  *keysList = m_data.keysList;

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::storeActiveKey (const installed_key_info_t& keyInfo,
                                     const file_image_t&         keyFile
                                    )
{

  m_data.activeKey = keyFile;
  m_data.activeKeySerNum = keyInfo.serialNumber;

  std::pair<installed_keys_list_t::iterator, bool> r = 
    m_data.keysList.insert (installed_keys_pair_t (keyInfo.serialNumber, keyInfo));

  if (m_data.keysList.end () != r.first)
  {
      r.first->second.usedToBeActiveKey = true;
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getActiveKey (installed_key_info_t *keyInfo,
                                   file_image_t         *keyFile
                                  )
{

  if (!keyFile || !keyInfo) 
  {
    return (E_INVALIDARG);
  }

  if (m_data.activeKey.image.empty ()) 
  {
    return (S_FALSE);
  }

  if (S_OK != findKey (m_data.activeKeySerNum, keyInfo)) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  *keyFile = m_data.activeKey;

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getActiveKeyInfo (installed_key_info_t *keyInfo)
{
  return (findKey (m_data.activeKeySerNum, keyInfo));
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::clearActiveKey ()
{

  m_data.activeKeySerNum.clear ();
  m_data.activeKey.image.resize (0);
 
  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::storeReserveKey (const installed_key_info_t&  keyInfo,
                                      const file_image_t&          keyFile
                                     )
{

  m_data.reserveKey = keyFile;
  m_data.reserveKeySerNum = keyInfo.serialNumber;

  m_data.keysList.insert (installed_keys_pair_t (keyInfo.serialNumber, keyInfo));

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getReserveKey (installed_key_info_t *keyInfo,
                                    file_image_t         *keyFile
                                   )
{

  if (!keyInfo || !keyFile)
  {
    return (E_INVALIDARG);
  }

  keyInfo->serialNumber.clear ();
    
  if (m_data.reserveKey.image.empty ()) 
  {
    return (S_FALSE);
  }

  if (S_OK != findKey (m_data.reserveKeySerNum, keyInfo))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  *keyFile = m_data.reserveKey;

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getReserveKeyInfo (installed_key_info_t *keyInfo)
{
  return (findKey (m_data.reserveKeySerNum, keyInfo));
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::clearReserveKey ()
{

  installed_keys_list_t::iterator   it (m_data.keysList.find (m_data.reserveKeySerNum));

  if (m_data.keysList.end () != it) 
  {
	  if (!m_data.trialKeySerNum.empty ()) 
	  {
		  if ( (m_data.trialKeySerNum == it->second.serialNumber) && (!it->second.usedToBeActiveKey) )
		  {
			  m_data.trialKeySerNum.clear ();
		  }
	  }
      m_data.keysList.erase (it);
  }

  m_data.reserveKeySerNum.clear ();
  m_data.reserveKey.image.resize (0);

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::storeTrialKeyInfo  (const installed_key_info_t& keyInfo)
{

  m_data.trialKeySerNum = keyInfo.serialNumber;
  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getTrialKeyInfo (installed_key_info_t *keyInfo)
{

  if (!keyInfo) 
  {
    return (E_INVALIDARG);
  }

  keyInfo->serialNumber.clear ();

  if (m_data.trialKeySerNum.empty ())
    return (S_FALSE);

  return (S_OK == findKey (m_data.trialKeySerNum, keyInfo) ? S_OK : LIC_E_SECURE_DATA_CORRUPT);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::findKey (const key_serial_number_t&  keyNumber,
                              installed_key_info_t       *keyInfo
                             )
{

  if (!keyInfo) 
  {
    return (E_INVALIDARG);
  }

  installed_keys_list_t::iterator i = m_data.keysList.find (keyNumber);

  if (i != m_data.keysList.end ()) 
  {
    *keyInfo = i->second;
    return (S_OK);
  }
  else
  {
    keyInfo->serialNumber.clear ();
    return (S_FALSE);
  }

}

//-----------------------------------------------------------------------------

installed_key_info_t* CSecureData::findKeyInfo (const key_serial_number_t& keyNumber)
{

  installed_keys_list_t::iterator i = m_data.keysList.find (keyNumber);

  return (m_data.keysList.end () != i ? &i->second : 0);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::deleteInstalledKeyInfo (const key_serial_number_t& keyNumber)
{
  m_data.keysList.erase (keyNumber);
  return (S_OK);
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::serializeKeySerNumAndImage (HDATA node,
                                                 const key_serial_number_t& sn,
                                                 const file_image_t&        image
                                                )
{

  if (! m_dataTree.addData (node, 
                            MAKE_AVP_PID (1, 1, avpt_bin, 0), 
                            &sn, 
                            sizeof (sn))) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  if (! m_dataTree.addData (node, 
                            MAKE_AVP_PID (2, 1, avpt_bin, 0), 
                            image.image.c_str (),
                            (dword_t)(sizeof (char) * image.image.size ()))) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::deserializeKeySerNumAndImage  (DataTree::CDataTree::iterator& i,
                                                    key_serial_number_t *sn,
                                                    file_image_t *fileImage
                                                   )
{

  if (!sn || !fileImage) 
  {
    return (E_INVALIDARG);
  }

  if (i == m_dataTree.end ()) 
  {
    return (S_FALSE);
  }

  // Load key serial number
  if (! m_dataTree.get (*i, sn, sizeof (*sn)))
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  i++;

  if (i == m_dataTree.end ()) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }


  char              *s;
  DataTree::dword_t  size;

  // Load active key file image
  if (! m_dataTree.get (*i, reinterpret_cast<void**> (&s), &size)) 
  {
    return (false);
  }

  try
  {
    fileImage->image.assign (s, size);
    delete [] s;
  }

  catch (const std::bad_alloc&) 
  {
    delete [] s;
    return (E_OUTOFMEMORY);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::serializeKeysList ()
{


  if (m_data.keysList.size ())
  {
  
    HDATA node;
    
    if (! (node = m_dataTree.addData (m_dataTree.getRoot (),
                                       KEYS_LIST,
                                       (dword_t)m_data.keysList.size ()
                                      )))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

    installed_keys_list_t::iterator i (m_data.keysList.begin ());
    HDATA                           currNode;
    size_t                          counter = 1;

    while (i != m_data.keysList.end ()) 
    {
      if (! (currNode = m_dataTree.addNode (node, 
                                            MAKE_AVP_PID (counter, 1, avpt_nothing, 0)))) 
      {
        return (LIC_E_SECURE_DATA_CORRUPT);
      }
      if (FAILED (serializeKeyInfo (currNode, i->second)))
      {
        return (LIC_E_SECURE_DATA_CORRUPT);
      };

      i++;
      counter++;
    }

  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::deserializeKeysList ()
{

  DataTree::CDataTree::iterator i (m_dataTree.begin (
    m_dataTree.findData (DataTree::makeAddr (KEYS_LIST))));

  if (i == m_dataTree.end ()) 
  {
    return (S_OK);
  }

  installed_key_info_t ki;
  size_t               counter = 1;
  HDATA                node = m_dataTree.findData (DataTree::makeAddr (KEYS_LIST));
  
  while (i != m_dataTree.end ()) 
  {
  
    DataTree::CDataTree::iterator j (m_dataTree.begin (
      m_dataTree.findData (node, DataTree::makeAddr (MAKE_AVP_PID (counter, 1, avpt_nothing, 0)))));

    if (FAILED (deserializeKeyInfo (j, &ki)))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }
    m_data.keysList.insert (installed_keys_pair_t (ki.serialNumber, ki));

    i++;
    counter++;
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::checkDataConsistency ()
{

  if (! m_data.activeKeySerNum.empty ())
  {
    // Check if installation info presented for active key

    if (! findKeyInfo (m_data.activeKeySerNum)) 
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

	// Verify if active key installation info has correct value of usedToBeActiveKey flag.
	// It can be false if we use license storage created by earlier licensing library
	// version
	
	installed_keys_list_t::iterator i = m_data.keysList.find (m_data.activeKeySerNum);
	
	if (m_data.keysList.end () != i)
	{
		i->second.usedToBeActiveKey = true;
	}
	else
	{
		return (LIC_E_SECURE_DATA_CORRUPT);
	}


    if (! m_data.reserveKeySerNum.empty ())
    {

      // Check if installation info presented for reserve key

      if (! findKeyInfo (m_data.reserveKeySerNum)) 
      {
        return (LIC_E_SECURE_DATA_CORRUPT);
      }
    }

  }

  if (! m_data.trialKeySerNum.empty ()) 
  {
    // Check if installation info presented for trial key

    if (! findKeyInfo (m_data.trialKeySerNum))
    {
      return (LIC_E_SECURE_DATA_CORRUPT);
    }

  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::isActiveOrReserveKey  (const key_serial_number_t& serNum)
{
  return ( (serNum == m_data.activeKeySerNum) || 
           (serNum == m_data.reserveKeySerNum) ? S_OK : S_FALSE);
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::moveReserveKeyToActive ()
{

	installed_keys_list_t::iterator i = m_data.keysList.find (m_data.reserveKeySerNum);
	
	if (m_data.keysList.end () != i)
	{
		i->second.usedToBeActiveKey = true;
	}
	else
	{
		return (E_FAIL);
	}

	m_data.activeKey = m_data.reserveKey;
	m_data.activeKeySerNum = m_data.reserveKeySerNum;
	m_data.reserveKey.image.resize (0);
	m_data.reserveKeySerNum.number.parts.keySerNum = 0;


	return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getLastKnownDate (date_t *lastKnownDate)
{
    if (!lastKnownDate) 
    {
        return (E_INVALIDARG);
    }
    *lastKnownDate = m_data.lastKnownDate;
    return (S_OK);
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::setLastKnownDate (const date_t& lastKnownDate)
{
    m_data.lastKnownDate = lastKnownDate;
    return (S_OK);
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::getTrialLimitDate (date_t *trialLimitDate)
{
    if(!trialLimitDate)
    {
        return (E_INVALIDARG);
    }
    *trialLimitDate = m_data.trialLimitDate;
    return (S_OK);
}

//-----------------------------------------------------------------------------

HRESULT CSecureData::setTrialLimitDate (const date_t& trialLimitDate)
{
    m_data.trialLimitDate = trialLimitDate;
    return (S_OK);
}

//-----------------------------------------------------------------------------

bool CSecureData::hasTrialKeyInvo() const
{
	return !m_data.trialKeySerNum.empty();
}

//-----------------------------------------------------------------------------
