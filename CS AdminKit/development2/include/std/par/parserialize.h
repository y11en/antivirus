/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parserialize.h
 * \author	Andrew Kazachkov
 * \date	05.12.2003 11:42:47
 * \brief	
 * 
 */

#ifndef __KL_PARSERIALIZE_H__
#define __KL_PARSERIALIZE_H__

//#include "./parstreams.h"
#include "./params.h"

const char c_RequestFlagTag[]	= "RequestFlag";
const char c_MessageIdTag[]		= "MessageId";

namespace KLPAR
{

    #define KLPAR_SIGN_SIZE 16

    //signature bytes
    static const char c_szSignSoap[KLPAR_SIGN_SIZE]=
    {
        '<','!','-','-','K','L','P','A','R','S','O','A','P','-','-','>'
    };

    static const char c_szSignBIN[KLPAR_SIGN_SIZE]=
    {
        '<','!','-','-','K','L','P','A','R','B','I','N',' ','-','-','>'
    };

    /*!
        Header for soap xml format
        1.  c_szSignSoap    16 bytes 
        2.  xml body
    */

    /*!
        Header for binary format
        1.  c_szSignBIN     16 bytes
        2.  offset of body  4  bytes
        3.  version         4  bytes
        4.  flags           4  bytes
        5.  body
    */

    struct SerParamsFormat
    {
        SerParamsFormat()
            :   m_dwStructSize(sizeof(SerParamsFormat))
            ,   m_dwVerson(1)
            ,   m_dwFlags(0)
        {;}
        AVP_dword   m_dwStructSize;
        AVP_dword   m_dwVerson;
        AVP_dword   m_dwFlags;  //KLPAR_SERFLAGS
    };

    class WriteStream;
    class ReadStream;

    //WriteStream and pCRC32 cannot be NULL simultaneously
    KLCSC_DECL void SerializeParams(
                        WriteStream*            pFs,
                        const SerParamsFormat*  pFormat,
                        KLPAR::Params*          pParams,
                        AVP_dword*              pCRC32 = NULL,
                        int                     nBufferSize = -1);

    //pParams cannot be NULL
    KLCSC_DECL void DeserializeParams(
                        ReadStream*         pFs,
                        SerParamsFormat*    pFormat,
                        KLPAR::Params**     ppParams,
                        AVP_dword*          pCRC32 = NULL,
                        int                 nBufferSize = -1,
                        size_t              nObjectSize = 0);

    inline AVP_dword CalcParamsCheckSum(KLPAR::Params* pParams)
    {
        AVP_dword dwResult = 0;
        SerParamsFormat spf;
        spf.m_dwFlags = KLPAR_SF_BINARY;
        SerializeParams(NULL, &spf, pParams, &dwResult);
        return dwResult;
    };
    
    const size_t c_Md5DigestSize=16;
    KLCSC_DECL void CalcParamsMd5Hash(KLPAR::Params* pParams, unsigned char digest[c_Md5DigestSize]);
    
    KLCSC_DECL void CalcParamsMd5Hash2Binary(KLPAR::Params* pParams, KLPAR::BinaryValue** ppResult);

    KLCSC_DECL void DeserializeFromMemory(
                                    const void*     pData,
                                    size_t          nData,
                                    AVP_dword&      lFlags,
                                    KLPAR::Params** ppParams,
                                    AVP_dword*      pCRC32 = NULL);
}
#endif //__KL_PARSERIALIZE_H__
