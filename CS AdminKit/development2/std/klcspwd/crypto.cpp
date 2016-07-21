#include "std/base/klstd.h"

#include "openssl/rsa.h"
#include "openssl/rand.h"
#include "openssl/evp.h"

#include "openssl/rc5.h"


#include "std/klcspwd/crypto.h"
#include "std/klcspwd/errorcodes.h"


#ifdef _WIN32
#include <atlbase.h>
#include <wincrypt.h>


#endif

#include <avp/avp_data.h>
#include <avp/text2bin.h>
#include <avp/byteorder.h>

#include "boost/crc.hpp"

#include <vector>


#ifdef __unix
#define _ASSERTE    assert
#endif

namespace KLCSPWD
{

#ifdef _WIN32

    class CAutoCS
    {
    public:
        CAutoCS(CRITICAL_SECTION& cs)
            :   m_CS(cs)
        {
            EnterCriticalSection(&m_CS);
        };

        ~CAutoCS()
        {
            LeaveCriticalSection(&m_CS);
        };
    protected:
        CRITICAL_SECTION&   m_CS;
    };

    const RAND_METHOD*  g_pDefault = NULL;
    HCRYPTPROV          g_hProv = NULL;
    CRITICAL_SECTION    g_CS;
    bool                g_bOwnSSL;
    
    void Rand_seed(const void *buf, int num)
    {
        if(!g_hProv)
            g_pDefault->seed(buf, num);
    };

    int Rand_bytes(unsigned char *buf, int num)
    {
        if(!g_hProv)
            return g_pDefault->bytes(buf, num);
        CAutoCS acs(g_CS);
        return CryptGenRandom(g_hProv, num, buf);
    };

    void Rand_cleanup(void)
    {
        if(!g_hProv)
            g_pDefault->cleanup();
    };

    void Rand_add(const void *buf, int num, double entropy)
    {
        if(!g_hProv)
            g_pDefault->add(buf, num, entropy);
    };

    int Rand_pseudorand(unsigned char *buf, int num)
    {
        if(!g_hProv)
            return g_pDefault->pseudorand(buf, num);
        CAutoCS acs(g_CS);
        return CryptGenRandom(g_hProv, num, buf);
    };

    //returns 1
    int Rand_status(void)
    {
        if(!g_hProv)
            return g_pDefault->status();
        else
            return 1;
    };

    const RAND_METHOD g_Rm = 
    {
        Rand_seed,
        Rand_bytes,
        Rand_cleanup,
        Rand_add,
        Rand_pseudorand,
        Rand_status
    };
#endif
    
    KLCSPWD_DECL void initialize(bool bOwnSSL)
    {
#ifdef _WIN32
        if(bOwnSSL)
        {
            g_bOwnSSL = true;
            InitializeCriticalSection(&g_CS);
            if(!CryptAcquireContext(
                                &g_hProv,
                                NULL,
                                MS_DEF_PROV,
                                PROV_RSA_FULL,
                                CRYPT_VERIFYCONTEXT))
            {
                ;
            };
            g_pDefault = RAND_get_rand_method();
            RAND_set_rand_method(&const_cast<RAND_METHOD&>(g_Rm));
        };
#endif
    };

    KLCSPWD_DECL void deinitialize()
    {
#ifdef _WIN32
        if(g_bOwnSSL)
        {
            RAND_set_rand_method(g_pDefault);
            g_pDefault = NULL;
            if(g_hProv)
            {
                CryptReleaseContext(g_hProv, 0);
                g_hProv = NULL;
            };
            DeleteCriticalSection(&g_CS);
        };
#endif
    };

    int SerializePublicKey(RSA* pRsa, void*& pBuffer, size_t& nBuffer)
    {
        const size_t nData = i2d_RSAPublicKey(pRsa, NULL);
        void* pData = malloc(nData);
        if(!pData)
            return KLCSPWD_NOMEM;
        void* pTmp = pData;
        if(i2d_RSAPublicKey(pRsa, (unsigned char**)&pTmp) <= 0)
        {
            Free(pData, nData);
            return KLCSPWD_BAD_PUBKEY;
        };
        pBuffer = pData;
        nBuffer = nData;
        return 0;
    };

    int SerializePrivateKey(RSA* pRsa, void*& pBuffer, size_t& nBuffer)
    {
        const size_t nData = i2d_RSAPrivateKey(pRsa, NULL);
        void* pData = malloc(nData);
        if(!pData)
            return KLCSPWD_NOMEM;
        void* pTmp = pData;
        if(i2d_RSAPrivateKey(pRsa, (unsigned char**)&pTmp) <= 0)
        {
            Free(pData, nData);
            return KLCSPWD_BAD_PRVKEY;
        };
        pBuffer = pData;
        nBuffer = nData;
        return 0;
    };


    int GenRsaKey(
                void*&  pPublicKey,
                size_t& nPublicKey,
                void*&  pPrivateKey,
                size_t& nPrivateKey)
    {
        int nResult = 0;
        void*   pIPublicKey = NULL;
        size_t  nIPublicKey = 0;
        void*   pIPrivateKey = NULL;
        size_t  nIPrivateKey = 0;
        RSA*    pRsa = NULL;        
        {
            pRsa = RSA_generate_key(1024, RSA_F4, NULL, NULL);
            if(!pRsa)
            {
                nResult = KLCSPWD_KEYGEN_FAILED;
                goto onCleanup;
            };
            nResult = SerializePublicKey(pRsa, pIPublicKey, nIPublicKey);
            if(nResult)
                goto onCleanup;
            nResult = SerializePrivateKey(pRsa, pIPrivateKey, nIPrivateKey);
            if(nResult)
                goto onCleanup;
        };
        pPublicKey = pIPublicKey;
        nPublicKey = nIPublicKey;
        pIPublicKey = NULL;
        pPrivateKey = pIPrivateKey;
        pIPrivateKey = NULL;
        nPrivateKey = nIPrivateKey;
        nResult = 0;
    onCleanup:
        _ASSERTE(nResult == 0);
        Free(pIPublicKey, nIPublicKey);
        Free(pIPrivateKey, nIPrivateKey);
        if(pRsa)
            RSA_free(pRsa);        
        return nResult;
    };

    KLCSPWD_DECL int GenRandom(size_t nBuffer, void* pBuffer)
    {
        return RAND_bytes((unsigned char*)pBuffer, nBuffer) > 0
                ?   0
                :   KLCSPWD_RAND_FAILED;
    };

    RSA* DeserializePublicKey(void* pBuffer, size_t nBuffer)
    {
        return d2i_RSAPublicKey(NULL, (const unsigned char**)&pBuffer, nBuffer);
    };

    RSA* DeserializePrivateKey(void* pBuffer, size_t nBuffer)
    {
        return d2i_RSAPrivateKey(NULL, (const unsigned char**)&pBuffer, nBuffer);
    };

    class CDataWriter
    {
    public:
        CDataWriter(void* pBuffer, size_t nBuffer)
            :   m_pBuffer((unsigned char*)pBuffer)
            ,   m_nBuffer(nBuffer)
            ,   m_nOffset(0)
        {;};

        bool write(const void* pData, size_t nData)
        {
            if(m_pBuffer)
            {
                if(m_nOffset + nData > m_nBuffer)
                    return false;
                memcpy(&m_pBuffer[m_nOffset], pData, nData);
            };            
            m_nOffset += nData;
            return true;
        };

        bool writeObject(const void* pObject, size_t nObject)
        {
            AVP_dword dwSize = MachineToLittleEndian(AVP_dword(nObject));
            if(!write(&dwSize, sizeof(dwSize)))
                return false;
            return write(pObject, nObject);
        };
        size_t  GetSize()
        {
            return m_nOffset;
        };
    protected:
        unsigned char*  m_pBuffer;
        size_t          m_nBuffer;
        size_t          m_nOffset;
    };

    class CDataReader
    {
    public:
        CDataReader(const void* pBuffer, size_t nBuffer)
            :   m_pBuffer((const unsigned char*)pBuffer)
            ,   m_nBuffer(nBuffer)
            ,   m_nOffset(0)
        {;};
        bool read(void*& pData, size_t nData)
        {
            if(m_nOffset + nData > m_nBuffer)
                return false;
            pData = (void*)&m_pBuffer[m_nOffset];
            m_nOffset += nData;
            return true;
        };

        bool readObject(void*& pObject, size_t& nObject)
        {
            AVP_dword* pdwSize = NULL;
            if(!read((void*&)pdwSize, sizeof(AVP_dword)))
                return false;
            nObject = LittleEndianToMachine(*pdwSize);
            if(!read(pObject, nObject))
                return false;
            return true;
        };

    protected:
        const unsigned char*    m_pBuffer;
        size_t                  m_nBuffer;
        size_t                  m_nOffset;
    };


    /*!
        format
        [prefix]
        [encrypted_session_key_size]
        [encrypted_session_key]
        [iv_size]
        [iv]
        [ciphertext_size]
        [ciphertext]
    */

    int EncryptData(
                    const void* pPublicKey,
                    size_t      nPublicKey,
                    const void* pPrefix,
                    size_t      nPrefix,
                    const void* pPlainText,
                    size_t      nPlainText,
                    void*&      pCipherText,
                    size_t&     nCipherText)
    {
        int nResult = 0;
        RSA* pRsa = NULL;
        EVP_PKEY* pPKey = NULL;
        void*  pEncSessionKey = NULL;
        size_t nEncSessionKey = 0;
        void*  pProtectedData = NULL;
        size_t nProtectedData = 0;
        void*  pBuffer = NULL;
        size_t nBuffer = 0;
        const size_t nIv = EVP_MAX_IV_LENGTH;
        unsigned char iv[nIv];
        void* pIv = &iv[0];
        {
            nResult = GenRandom(EVP_MAX_IV_LENGTH, iv);
            if(nResult)
                goto onCleanup;
            pRsa = DeserializePublicKey(const_cast<void*>(pPublicKey), nPublicKey);
            if(!pRsa)
            {
                nResult = KLCSPWD_BAD_PUBKEY;
                goto onCleanup;
            };
            pPKey = EVP_PKEY_new();
            if(!pPKey)
            {
                nResult = KLCSPWD_NOMEM;
                goto onCleanup;
            };
            if(EVP_PKEY_set1_RSA(pPKey, pRsa) <= 0)
            {
                nResult = KLCSPWD_BAD_PUBKEY;
                goto onCleanup;
            };
            nEncSessionKey = EVP_PKEY_size(pPKey);
            pEncSessionKey = malloc(nEncSessionKey);
            if(!pEncSessionKey)
            {
                nResult = KLCSPWD_NOMEM;
                goto onCleanup;
            };
            unsigned char* pEk[]={(unsigned char*)pEncSessionKey};
            int pEkl[]={0};            
            EVP_CIPHER_CTX ctx;
            if(EVP_SealInit(
                            &ctx,
                            EVP_rc5_32_12_16_ofb(),
                            pEk,
                            pEkl,
                            (unsigned char*)pIv,
                            &pPKey,
                            1) <= 0)
            {
                nResult = KLCSPWD_SEAL_FAILED;
                goto onCleanup;
            };
            nEncSessionKey = pEkl[0];
            nProtectedData = nPlainText + EVP_CIPHER_CTX_block_size(&ctx);
            pProtectedData = malloc(nProtectedData);
            if(!pProtectedData)
            {
                nResult = KLCSPWD_NOMEM;
                goto onCleanup;
            };
            if(nPlainText > 0)
            {
                if(EVP_SealUpdate(
                                &ctx,
                                (unsigned char*)pProtectedData, 
                                (int*)&nProtectedData,
                                (unsigned char*)pPlainText,
                                nPlainText) <= 0)
                {
                    nResult = KLCSPWD_SEAL_FAILED;
                    goto onCleanup;
                };
            }
            else
                nProtectedData = 0;
            size_t nTmp = 0;
            EVP_SealFinal(
                        &ctx,
                        &((unsigned char*)pProtectedData)[nProtectedData],
                        (int*)&nTmp);
            nProtectedData += nTmp;
            ;
            boost::crc_32_type crc32;
            crc32.process_bytes(pPlainText, nPlainText);
            AVP_dword dwCrc32 = MachineToLittleEndian(AVP_dword(crc32.checksum()));
            ;
            {
                CDataWriter writer(NULL, 0);
                writer.write(pPrefix, nPrefix);
                writer.write(&dwCrc32, sizeof(dwCrc32));
                writer.writeObject(pEncSessionKey, nEncSessionKey);
                writer.writeObject(pIv, nIv);
                writer.writeObject(pProtectedData, nProtectedData);
                nBuffer = writer.GetSize();
                pBuffer = malloc(nBuffer);
                if(!pBuffer)
                {
                    nResult = KLCSPWD_NOMEM;
                    goto onCleanup;
                };
            };
            {
                CDataWriter writer(pBuffer, nBuffer);

                if(!writer.write(pPrefix, nPrefix))
                {
                    nResult = KLCSPWD_BUF_OVERFLOW;
                    goto onCleanup;
                };
                if(!writer.write(&dwCrc32, sizeof(dwCrc32)))
                {
                    nResult = KLCSPWD_BUF_OVERFLOW;
                    goto onCleanup;
                };
                if(!writer.writeObject(pEncSessionKey, nEncSessionKey))
                {
                    nResult = KLCSPWD_BUF_OVERFLOW;
                    goto onCleanup;
                };
                if(!writer.writeObject(pIv, nIv))
                {
                    nResult = KLCSPWD_BUF_OVERFLOW;
                    goto onCleanup;
                };
                if(!writer.writeObject(pProtectedData, nProtectedData))
                {
                    nResult = KLCSPWD_BUF_OVERFLOW;
                    goto onCleanup;
                };
            };
        };
        ;        
        pCipherText = pBuffer;
        pBuffer = NULL;
        nCipherText = nBuffer;
        nBuffer = 0;
    onCleanup:
        _ASSERTE(nResult == 0);
        Free(pBuffer, nBuffer);
        Free(pProtectedData, nProtectedData);
        Free(pEncSessionKey, nEncSessionKey);
        if(pPKey)
            EVP_PKEY_free(pPKey);
        if(pRsa)
            RSA_free(pRsa);
        return nResult;
    };

    int DecryptData(
                    const void* pPrivateKey,
                    size_t      nPrivateKey,
                    size_t      nPrefix,
                    const void* pCipherText,
                    size_t      nCipherText,
                    void*&      pPlainText,
                    size_t&     nPlainText)
    {
        int nResult = 0;
        RSA* pRsa = NULL;
        EVP_PKEY* pPKey = NULL;
        void*  pUnProtectedData = NULL;
        size_t nUnProtectedData = 0;
        {
            void*  pEncSessionKey = NULL;
            size_t nEncSessionKey = 0;
            void*  pProtectedData = NULL;
            size_t nProtectedData = 0;
            void*   pPrefix = NULL;
            void*   pIv = NULL;
            size_t  nIv = 0;

            CDataReader reader(pCipherText, nCipherText);
            if(!reader.read(pPrefix, nPrefix))
            {
                nResult = KLCSPWD_BAD_DATA_FMT;
                goto onCleanup;
            };
            AVP_dword* pCrc32 = NULL;
            if(!reader.read((void*&)pCrc32, sizeof(AVP_dword)))
            {
                nResult = KLCSPWD_BAD_DATA_FMT;
                goto onCleanup;
            };
            if(!reader.readObject(pEncSessionKey, nEncSessionKey))
            {
                nResult = KLCSPWD_BAD_DATA_FMT;
                goto onCleanup;
            };
            if(!reader.readObject(pIv, nIv))
            {
                nResult = KLCSPWD_BAD_DATA_FMT;
                goto onCleanup;
            };
            if(!reader.readObject(pProtectedData, nProtectedData))
            {
                nResult = KLCSPWD_BAD_DATA_FMT;
                goto onCleanup;
            };
            ;
            pRsa = DeserializePrivateKey(const_cast<void*>(pPrivateKey), nPrivateKey);
            if(!pRsa)
            {
                nResult = KLCSPWD_BAD_PRVKEY;
                goto onCleanup;
            };
            pPKey = EVP_PKEY_new();
            if(!pPKey)
            {
                nResult = KLCSPWD_NOMEM;
                goto onCleanup;
            };
            EVP_PKEY_set1_RSA(pPKey, pRsa);
            EVP_CIPHER_CTX ctx;
            if(EVP_OpenInit(
                        &ctx,
                        EVP_rc5_32_12_16_ofb(),
                        (unsigned char*)pEncSessionKey,
                        nEncSessionKey,
                        (unsigned char*)pIv,
                        pPKey) <= 0)
            {
                nResult = KLCSPWD_OPEN_FAILED;
                goto onCleanup;
            };
            nUnProtectedData = nProtectedData + EVP_CIPHER_CTX_block_size(&ctx);
            pUnProtectedData = malloc(nUnProtectedData);
            if(!pUnProtectedData)
            {
                nResult = KLCSPWD_NOMEM;
                goto onCleanup;
            };
            if(EVP_OpenUpdate(
                            &ctx,
                            (unsigned char*)pUnProtectedData, 
                            (int*)&nUnProtectedData,
                            (unsigned char*)pProtectedData,
                            nProtectedData) <= 0)
            {
                nResult = KLCSPWD_OPEN_FAILED;
                goto onCleanup;
            };
            size_t nTmp = 0;
            EVP_OpenFinal(
                        &ctx,
                        &((unsigned char*)pUnProtectedData)[nUnProtectedData],
                        (int*)&nTmp);
            nUnProtectedData += nTmp;
            ;
            //check CRC32
            boost::crc_32_type crc32;
            crc32.process_bytes(pUnProtectedData, nUnProtectedData);
            AVP_dword dwCrc32 = MachineToLittleEndian(AVP_dword(crc32.checksum()));
            _ASSERTE(dwCrc32 == *pCrc32);
            if(*pCrc32 != dwCrc32)
            {
                nResult = KLCSPWD_BAD_CRC;
                goto onCleanup;
            };
        };
        pPlainText = pUnProtectedData;
        nPlainText = nUnProtectedData;
        pUnProtectedData = NULL;
        nUnProtectedData = 0;
    onCleanup:
        _ASSERTE(nResult == 0);
        Free(pUnProtectedData, nUnProtectedData);
        if(pPKey)
            EVP_PKEY_free(pPKey);
        if(pRsa)
            RSA_free(pRsa);
        return nResult;

    };
    
    void Clean(void* pData, size_t nData)
    {
        if(pData && nData)
            memset(pData, 0, nData);
    };

    void Free(void* pData, size_t nData)
    {
        if(pData)
        {
            if(nData)
                memset(pData, 0, nData);
            free(pData);
        };
    };
    
    KLCSPWD_DECL int CreatePassword(
                            size_t  nLength, 
                            char*&  pData, 
                            size_t& nData)
    {
        if(!nLength || pData || nData)
            return -1;
        ;
        pData = NULL;
        nData = 0;
        int nResult  = 0;
        ;
        void*   pBytes = NULL;
        size_t  nBytes = 0;
        ;
        char*   pResBytes = NULL;
        size_t  nResBytes = 0;
        ;
        nBytes = nLength;
        pBytes = malloc(nBytes);
        memset(pBytes, 0xcd, nLength);
        if( RAND_bytes((unsigned char*)pBytes, nLength) <= 0 )
        {
            nResult = KLCSPWD_RAND_FAILED;
            goto onCleanup;
        };
        nResBytes = B2T_LENGTH(nBytes) + 1;
        pResBytes = (char*)malloc(nResBytes);
        BinToText(pBytes, nBytes, pResBytes, nResBytes);
        pResBytes[nLength] = 0;
        pData = pResBytes;
        nData = nResBytes;
        pResBytes = NULL;
        nResBytes = 0;
    onCleanup:
        Free(pBytes, nBytes);
        Free(pResBytes, nResBytes);
        return nResult;
    };

    int CreateRandomStringAI(
                size_t              nBuffer,
                std::vector<char>&  vecBufferA)
    {
        if(nBuffer < 2)
            return KLCSPWD_BADARG;
        std::vector<AVP_byte> vecData;
        vecData.resize(nBuffer);
        int nRes = KLCSPWD::GenRandom(vecData.size(), &vecData[0]);
        if( 0 == nRes)
        {
            const size_t c_nSize = B2T_LENGTH(vecData.size());
            vecBufferA.resize(c_nSize+1);
            KLSTD_ASSERT(c_nSize >= nBuffer-1);
            if(BinToText(&vecData[0], vecData.size(), &vecBufferA[0], c_nSize) < 0)
                nRes = -1;
        };
        return nRes;
    };

    KLCSPWD_DECL int FillRandomStringA(
                            char*       pBuffer, 
                            size_t      nBuffer)
    {
        std::vector<char> vecBufferA;
        int nRes = CreateRandomStringAI(nBuffer, vecBufferA);
        if( 0 == nRes )
        {
            strncpy(pBuffer, &vecBufferA[0], nBuffer-1);
            pBuffer[nBuffer-1] = 0;
        };
        return nRes;
    };

    KLCSPWD_DECL int FillRandomStringW(
                            wchar_t*    pBuffer, 
                            size_t      nBuffer)
    {
        std::vector<char> vecBufferA;
        int nRes = CreateRandomStringAI(nBuffer, vecBufferA);
        if( 0 == nRes )
        {
            KLSTD_A2CW2 wstrResult(&vecBufferA[0]);
            wcsncpy(pBuffer, wstrResult, nBuffer-1);
            pBuffer[nBuffer-1] = 0;
        };
        return nRes;
    };
};
