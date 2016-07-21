/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	crypto.h
 * \author	Andrew Kazachkov
 * \date	28.10.2004 11:10:07
 * \brief	
 * 
 */

#ifndef __KLCRYPTO_H__
#define __KLCRYPTO_H__

#include "./klcspwd_common.h"

namespace KLCSPWD
{
    
    KLCSPWD_DECL void Clean(void* pData, size_t nData);
    
    KLCSPWD_DECL void Free(void* pData, size_t nData);
    

/*!
  \brief	Generates password

  \param	nLength     IN  password length in characters, not including terminated zero
  \param	pData       OUT password data
  \param	nData       OUT password data size in bytes
  \return	0 if success
*/
    KLCSPWD_DECL int CreatePassword(
                            size_t  nLength, 
                            char*&  pData, 
                            size_t& nData);


    /*!
      \brief	Fills buffer with random chars

      \param	pBuffer
      \param	nBuffer 
      \return	0 if success
    */
    KLCSPWD_DECL int FillRandomStringW(
                            wchar_t*    pBuffer, 
                            size_t      nBuffer);

    KLCSPWD_DECL int FillRandomStringA(
                            char*       pBuffer, 
                            size_t      nBuffer);
};

#endif //__KLCRYPTO_H__
