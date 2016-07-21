/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwdr.h
 * \author	Andrew Kazachkov
 * \date	01.11.2004 11:00:06
 * \brief	Data protection API ("remote" version)
 * 
 */

#ifndef __KLCSPWDR_H__
#define __KLCSPWDR_H__

#include "./klcspwd_common.h"
#include "./klcspwd.h"
#include "./crypto.h"

namespace KLCSPWD
{
    typedef enum
    {
        KT_HOSTKEY = 1,
        KT_GLOBKEY = 2,
        KT_SRVRKEY = 3
    }key_type_t;

    const size_t    c_nPrefix = 4;

    const char      c_szHostPrefix[c_nPrefix + 1] = "HOST";
    const char      c_szGlobPrefix[c_nPrefix + 1] = "GLOB";
    const char      c_szSrvrPrefix[c_nPrefix + 1] = "SRVR";

    KLCSPWD_DECL void initialize(bool bOwnSSL);

    KLCSPWD_DECL void deinitialize();

    KLCSPWD_DECL bool IsDataProtectionInstalled();

    KLCSPWD_DECL int ProtectDataForKey(
                    const void*   pData,
                    size_t        nData,
                    const void*   pPublicKey,
                    size_t        nPublicKey,
                    key_type_t    nKeyType,
                    void*&        pProtectedData,
                    size_t&       nProtectedData);

    /*!
      \brief	Creates server key pair.

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int InstallServerKey();

    /*!
      \brief	Destroys server key pair.

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int UninstallServerKey();

    /*!
      \brief	Destroys server settings key pair.

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int InstallSettingsServerKey();

    /*!
      \brief	Destroys server settings key pair.

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int UninstallSettingsServerKey();
};

#endif //__KLCSPWDR_H__
