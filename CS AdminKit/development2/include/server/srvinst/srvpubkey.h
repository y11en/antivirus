/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srvpubkey.h
 * \author	Andrew Kazachkov
 * \date	01.11.2004 15:14:18
 * \brief	
 * 
 */

#ifndef __KLSRVPUBKEY_H__
#define __KLSRVPUBKEY_H__

void KLCSSRV_DECL KLSRV_InitializeKP();
void KLCSSRV_DECL KLSRV_DeinitializeKP();

void KLCSSRV_DECL KLSRV_GetServerKeyPair( KLPAR::BinaryValue**    ppPubKey,
                                        KLPAR::BinaryValue**    ppPrvKey,
                                        KLPAR::BinaryValue**    ppHash);

void KLCSSRV_DECL KLSRV_EncryptServerPrvKey(
                                        void*                   pHostPublicKey,
                                        size_t                  nHostPublicKey,
                                        void*                   pPrvKey,
                                        size_t                  nPrvKey,
                                        KLSTD::MemoryChunk**    ppEncPrvKey);



/*!
  \brief	Protects binary data with administration server 
            "global key". Use common function KLCSPWD::UnprotectData
            to unprotect. 
            Use KLCSPWD::Free to free protected data

  \param	pData           IN  data to protect
  \param	nData           IN  size of data to protect
  \param	pProtectedData  OUT protected data. Use KLCSPWD::Free to free this pointer. 
  \param	nProtectedData  OUT size of protected data
  \return	0 if success, error code otherwise
*/
int KLCSSRV_DECL KLSRV_ProtectDataGlobally(
                    const void*   pData,
                    size_t        nData,
                    void*&        pProtectedData,
                    size_t&       nProtectedData);


/*!
  \brief	Protects binary data with administration server 
            "global key". Use common function KLCSPWD::UnprotectData
            to unprotect. 
            Use KLCSPWD::Free to free protected data

  \param	pData    data to protect
  \param	nData    size of data to protect
  \retval	ppChunk  protected data. Use KLCSPWD::Free to free this pointer. 
  \return	0 if success, error code otherwise
*/
int KLCSSRV_DECL KLSRV_ProtectDataGlobally2(
                    const void*             pData,
                    size_t                  nData,
                    KLSTD::MemoryChunk**    ppChunk);


#endif //__KLSRVPUBKEY_H__