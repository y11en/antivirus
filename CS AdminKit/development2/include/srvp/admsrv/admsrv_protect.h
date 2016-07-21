/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	admsrv_protect.h
 * \author	Andrew Kazachkov
 * \date	29.10.2004 15:54:20
 * \brief	
 * 
 */

#ifndef __KLADMSRV_CRYPTO_H__
#define __KLADMSRV_CRYPTO_H__

namespace KLADMSRV
{
    class KLSTD_NOVTABLE DataProtection : public KLSTD::KLBaseQI
    {
    public:
    /*!
      \brief	Protects sensitive data to store in SettingsStorage
                or local task.

      \param	szwHostId  [in]   host name
      \param	pData      [in]   pointer to data
      \param	nData      [in]   size of data in bytes
      \param	ppChunk    [out]  pointer to protected data block. 

      \exception KLSTD::STDE_NOTPERM host has no public key (possibly
                 it doesn't support data protection or nagent isn't
                 installed there)
      \exception KLSTD::STDE_NOFUNC server doesn't support data protection
      \exception *
    */
        virtual void ProtectDataForHost(
                                const wchar_t*          szwHostId,
                                const void*             pData,
                                size_t                  nData,
                                KLSTD::MemoryChunk**    ppChunk) = 0;

    /*!
      \brief	Protects sensitive data to store in policy or
                global/group task.

      \param	pData   [in]  pointer to data
      \param	nData   [in]  size of data in bytes
      \param	ppChunk [out] pointer to protected data block. 

      \exception KLSTD::STDE_NOFUNC server doesn't support data protection
      \exception *
    */
        virtual void ProtectDataGlobally(
                                const void*             pData,
                                size_t                  nData,
                                KLSTD::MemoryChunk**    ppChunk) = 0;
    };
};

#endif //__KLADMSRV_CRYPTO_H__