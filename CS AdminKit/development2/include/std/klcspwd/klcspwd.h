/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwd.h
 * \author	Andrew Kazachkov
 * \date	27.10.2004 15:08:11
 * \brief	Data protection API
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __KLCSPWD_H__
#define __KLCSPWD_H__

#include "./klcspwd_common.h"

namespace KLCSPWD
{
/*KLCSAK_BEGIN_PRIVATE*/
    KLCSPWD_DECL void Initialize();
    
    KLCSPWD_DECL void Deinitialize();
/*KLCSAK_END_PRIVATE*/

    /*!
      \brief	Retrieves data which was protected by ProtectDataLocally 
                call. Caller must run under a member of Administrators
                group or local system account.

      \param	pProtectedData [in]  pointer to protected data block
      \param	nProtectedData [in]  size of protected data block in bytes
      \param	pData          [out] pointer to initial data. Caller is 
                                     responsible for freeing this data
                                     block with free()
      \param	nData          [out] size of initial data in bytes

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int UnprotectData(
                    const void*     pProtectedData, 
                    size_t          nProtectedData, 
                    void*&          pData,
                    size_t&         nData);


    /*!
      \brief	Protects data to store in SettingsStorage or local task.
                Caller must run under a member of Administrators group or
                local system account.

      \param	pData          [in]  pointer to initial data
      \param	nData          [in]  size of initial data in bytes
      \param	pProtectedData [out] pointer to protected data block. 
                                     Caller is responsible for freeing 
                                     this data block with free()
      \param	nProtectedData [out] size of protected data block in bytes

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int ProtectDataLocally(
                    const void*   pData,
                    size_t        nData,
                    void*&        pProtectedData,
                    size_t&       nProtectedData);
};

#endif //__KLCSPWD_H__
