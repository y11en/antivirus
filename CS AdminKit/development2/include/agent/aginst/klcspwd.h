/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwd.h
 * \author	Andrew Kazachkov
 * \date	11.11.2004 17:01:29
 * \brief	see klcspwd/klcspwd.h
 * 
 */

#ifndef __AGINST_KLCSPWD_H__
#define __AGINST_KLCSPWD_H__

int KLCSA_DECL KLCSPWD_UnprotectData(
                    const void*     pProtectedData, 
                    size_t          nProtectedData, 
                    void*&          pData,
                    size_t&         nData);

int KLCSA_DECL KLCSPWD_ProtectDataLocally(
                    const void*   pData,
                    size_t        nData,
                    void*&        pProtectedData,
                    size_t&       nProtectedData);

#endif //__AGINST_KLCSPWD_H__
