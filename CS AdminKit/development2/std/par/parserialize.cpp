/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parserialize.cpp
 * \author	Andrew Kazachkov
 * \date	04.12.2003 12:18:13
 * \brief	
 * 
 */

#define KLPAR_GSOAP_SUPPORTED

#include "std/par/params.h"
#include "std/par/valenum.h"
#include "std/io/klio.h"
#include "std/base/klbaseqi_imp.h"
#include "./parbinser.h"

#include "openssl/md5.h"
#include "common/measurer.h"


/*#if defined(_WIN32) && !(defined(KLSSINST_EXPORTS) && defined(_LIB))
#ifdef DEBUG
	#pragma comment(lib, "../OPENSSL/LIB/DEBUG/ssleay32.lib")
	#pragma comment(lib, "../OPENSSL/LIB/DEBUG/libeay32.lib")	
#else
	#pragma comment(lib, "../OPENSSL/LIB/RELEASE/ssleay32.lib")
	#pragma comment(lib, "../OPENSSL/LIB/RELEASE/libeay32.lib")	
#endif
#endif*/

#ifdef __unix__

	#include <sys/param.h>
    #include <sys/mount.h>
    #include <sys/stat.h>

#endif


#ifdef KLPAR_GSOAP_SUPPORTED
#include "std/par/par_gsoap.h"
#endif

#include <list>

#define KLCS_MODULENAME L"KLPAR"

#define KLPAR_SER_MEMWRITE_BUFFER_SIZE  (1024 * 4)
#define KLPAR_SER_MEMREAD_BUFFER_SIZE   0L

#ifdef _WIN32
#define KL_GET_FILE_LEN_BY_FILE_ID(id,len) len = _filelength(id);
#else
#define KL_GET_FILE_LEN_BY_FILE_ID(id,len)\
	{\
	struct stat st;\
	if ( fstat(id,&st)==0 )\
		len = st.st_size;\
	else\
		len = (-1);\
	}
#endif

namespace KLPAR
{

    class CMd5Stream : public WriteStream
    {
    public:
        CMd5Stream()
        {
            MD5_Init(&m_Md5Context);
        };
        void Write(const void* pData, int nDataToWrite)
        {
            MD5_Update(&m_Md5Context, pData, (unsigned long)nDataToWrite);
        };
        void GetDigest(unsigned char digest[c_Md5DigestSize])
        {
            MD5_Final(digest, &m_Md5Context);
        }
    protected:
        MD5_CTX m_Md5Context;
    };

    class CFileWriteStream : public WriteStream
    {
    public:
        CFileWriteStream(KLSTD::File* pFile)
            :   m_pFile(pFile)
        {;};
        void Write(const void* pData, int nDataToWrite)
        {
            m_pFile->Write(pData, nDataToWrite);
        };
    protected:
        KLSTD::CAutoPtr<KLSTD::File>    m_pFile;
    };

    class CFileIdWriteStream : public WriteStream
    {
    public:
        CFileIdWriteStream(int id)
            :   m_id(id)
        {;}
        void Write(const void* pData, int nDataToWrite)
        {
            int nWritten=write(m_id, pData, nDataToWrite);
            if(nWritten != nDataToWrite)
            {
                if(nWritten == -1)
                    KLSTD_THROW_ERRNO();
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            };
        };
    protected:
        int m_id;
    };

    class Allocator
    {
    public:
        virtual void* Allocate(size_t size) = 0;
    };

    class CMemoryWriteStream : public WriteStream
    {
    public:
        CMemoryWriteStream()
            :   m_nLength(0)
        {;}

        void Write(const void* pData, int nDataToWrite)
        {
            KLSTD_ASSERT(nDataToWrite >= 0);
            if(nDataToWrite > 0)
            {
                KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
                KLSTD_AllocMemoryChunk(nDataToWrite, &pChunk);
                memcpy(pChunk->GetDataPtr(), pData, nDataToWrite);
                m_lstChunks.push_back(pChunk);
                m_nLength+=nDataToWrite;
            };
        };

        void MakeMemoryChunk(KLSTD::MemoryChunk** ppChunk)
        {
            KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
            KLSTD_AllocMemoryChunk(m_nLength, &pChunk);
            ;
            AVP_byte* pBuffer=(AVP_byte*)pChunk->GetDataPtr();
            const int nBuffer=m_nLength;
            FillFiniteData(pBuffer, nBuffer);
            ;
            pChunk.CopyTo(ppChunk);
        };
        
        void MakeMemoryChunk(Allocator* pAllocator, void* &pData, int& nData)
        {
            AVP_byte* pBuffer=(AVP_byte*)pAllocator->Allocate(m_nLength);
            const int nBuffer=m_nLength;
            FillFiniteData(pBuffer, nBuffer);
            ;
            pData = pBuffer;
            nData = nBuffer;
        };
    protected:
        void FillFiniteData(AVP_byte* pBuffer, int nBuffer)
        {
            int nBufferPtr=0;
            for(chunklist_t::iterator it=m_lstChunks.begin(); it != m_lstChunks.end(); ++it)
            {
                const int nSize=(*it).m_T->GetDataSize();
                memcpy(&pBuffer[nBufferPtr], (*it).m_T->GetDataPtr(), nSize);
                nBufferPtr+=nSize;
                KLSTD_ASSERT(nBufferPtr <= m_nLength);
            };
        };
        typedef std::list<KLSTD::KLAdapt<KLSTD::CAutoPtr<KLSTD::MemoryChunk> > > chunklist_t;
        chunklist_t m_lstChunks;
        int         m_nLength;
    };

#ifdef KLPAR_GSOAP_SUPPORTED
    class CGsoapAdapterW
    {
    public:
        CGsoapAdapterW(CBinWriter& writer)
            :   m_Writer(writer)
            ,   m_bError(false)
        {;};
        void Flush()
        {
            m_Writer.Flush();
        }
        static int SOAP_FMAC2 fwrite(struct soap *soap, const char* pData, size_t nData)
        {
            CGsoapAdapterW* This=(CGsoapAdapterW*)soap->user;
            int nResult=SOAP_OK;
            KLERR_TRY
                This->m_Writer.Write(pData, nData);
            KLERR_CATCH(pError)
                nResult=SOAP_EOF;
                This->m_bError=true;
            KLERR_ENDTRY;
            return nResult;
        };
        bool            m_bError;
    protected:
        CBinWriter& m_Writer;
    };

    class CGsoapAdapterR
    {
    public:
        CGsoapAdapterR(CBinReader& reader)
            :   m_Reader(reader)
            ,   m_bError(false)
        {;};
        void Flush()
        {
            m_Reader.Flush();
        }
        static size_t SOAP_FMAC2 fread(struct soap *soap, char* pData, size_t nData)        
        {
            CGsoapAdapterR* This=(CGsoapAdapterR*)soap->user;
            int nResult=0;
            KLERR_TRY
                nResult=This->m_Reader.Read(pData, nData, false);
            KLERR_CATCH(pError)                
                This->m_bError=true;
            KLERR_ENDTRY;
            return nResult;
        };
        bool            m_bError;
    protected:
        CBinReader& m_Reader;
    };

#endif

    class CFileReadStream : public ReadStream
    {
    public:
        CFileReadStream(KLSTD::File* pFile)
            :   m_pFile(pFile)
        {;};
        int Read(void* pBuffer, int nBuffer)
        {
#ifdef _DEBUG
        KL_TMEASURE_BEGIN(L"CFileReadStream::Read", 4)
#endif
            return (int)m_pFile->Read(pBuffer, nBuffer);
#ifdef _DEBUG
        KL_TMEASURE_END();
#endif
        };
    protected:
        KLSTD::CAutoPtr<KLSTD::File>    m_pFile;
    };

    class CFileIdReadStream : public ReadStream
    {
    public:
        CFileIdReadStream(int id)
            :   m_id(id)
        {;}
        int Read(void* pBuffer, int nBuffer)
        {
#ifdef _DEBUG
        KL_TMEASURE_BEGIN(L"CFileIdReadStream::Read", 4)
#endif
            int nWasRead=read(m_id, pBuffer, nBuffer);
            if(nWasRead == -1)
                KLSTD_THROW_ERRNO();
            return nWasRead;
#ifdef _DEBUG
        KL_TMEASURE_END();
#endif
        };
    protected:
        int m_id;
    };

    class CMemoryReadStream : public ReadStream
    {
    public:
        CMemoryReadStream(const void* pData, int nData)
            :   m_pData((AVP_byte*)pData)
            ,   m_nData(nData)
            ,   m_nBufferPtr(0)
        {;}

        virtual int Read(void* pBuffer, int nBuffer)
        {
            KLSTD_ASSERT(nBuffer >= 0);
            const int nSpace=m_nData-m_nBufferPtr;
            KLSTD_ASSERT(nSpace >= 0);
            const int nDataToRead=nSpace < nBuffer ? nSpace : nBuffer;
            memcpy(pBuffer, &m_pData[m_nBufferPtr], nDataToRead);
            m_nBufferPtr+=nDataToRead;
            KLSTD_ASSERT(m_nBufferPtr <= m_nData);
            return nDataToRead;
        };

        virtual bool IfInMemory(AVP_byte*& pBuffer, int& nBuffer)
        {
            pBuffer = const_cast<AVP_byte*>(m_pData);
            nBuffer = m_nData;
            return true;
        };
    protected:
        const AVP_byte* m_pData;
        const int       m_nData;
        int             m_nBufferPtr;
    };

};

//ser.cpp

using namespace KLPAR;

namespace KLPAR
{
    class CAutoSoap
    {
    public:
        CAutoSoap()
            :   m_pSoap(NULL)
        {
            m_pSoap = (struct soap*)malloc(sizeof(struct soap));
            KLSTD_CHKMEM(m_pSoap);
            memset(m_pSoap, 0, sizeof(struct soap));
        }
        
        ~CAutoSoap()
        {
            if(m_pSoap)
            {
                free(m_pSoap);
                m_pSoap = NULL;
            };
        }
        KLSTD_INLINEONLY struct soap* get()
        {
            return m_pSoap;
        };
    protected:
        struct soap*    m_pSoap;
    };
    
    void SerializeParams(
                        WriteStream*            pFs,
                        const SerParamsFormat*  pFormat,
                        KLPAR::Params*          pParams,
                        AVP_dword*              pCRC32,
                        int                     nBufferSize)
    {
    //KL_TMEASURE_BEGIN(L"SerializeParams", 4)
        KLSTD_CHK(pFs, pFs || pCRC32);
        KLSTD_CHKINPTR(pFormat);
        //KLSTD_CHKINPTR(pParams);
        if( 
            pFormat->m_dwVerson > 1 ||
            pFormat->m_dwStructSize != sizeof(SerParamsFormat))
        {
            KLSTD_NOTIMP();
        };

        KLSTD::CPointer<boost::crc_32_type> crc32;
        if(pCRC32)
        {
            crc32=new boost::crc_32_type;
            KLSTD_CHKMEM(crc32);
        };
        CBinWriter strm(pFs, pFormat->m_dwFlags, crc32, nBufferSize);

        if(pFormat->m_dwFlags & KLPAR_SF_BIN)
        {            
            CParamsBinWriter writer(strm, pFormat);
            strm.Write(c_szSignBIN, KLPAR_SIGN_SIZE);
            AVP_dword dwOffset=KLPAR_SIGN_SIZE + sizeof(AVP_dword)*3;
            strm << dwOffset << pFormat->m_dwVerson << pFormat->m_dwFlags;
            writer.Start(pParams);
        }
        else
        {
    #ifdef KLPAR_GSOAP_SUPPORTED
            //pFs->Write(c_szSignSoap, KLPAR_SIGN_SIZE);
            CGsoapAdapterW saw(strm);
		    CAutoSoap soap;
		    soap_init(soap.get());
		    param__params par;
		    par.soap_default(soap.get());
		    ParamsForSoap(soap.get(), (Params*)pParams, par, false);
		    ;
		    soap.get()->sendfd = 0;
		    soap.get()->user=&saw;
		    soap.get()->fsend=saw.fwrite;
		    soap.get()->frecv=NULL;
            ;
		    soap_begin(soap.get());
//			    soap.get()->buffering=true;
			    par.soap_serialize(soap.get());
			    soap_begin_send(soap.get());		
			    KLERR_TRY
				    par.soap_put(soap.get(), "root", "param:params");
                    if(saw.m_bError)
                        KLSTD_THROW(KLSTD::STDE_NOSPC);
			    KLERR_CATCH(pError)
				    soap_end_send(soap.get());
				    soap_destroy(soap.get());
				    soap_end(soap.get());	
				    KLERR_RETHROW();
                KLERR_ENDTRY
			    soap_end_send(soap.get());
			    soap_destroy(soap.get());
		    soap_end(soap.get());
            saw.Flush();
        
    #else
            KLSTD_NOTIMP();
    #endif
        };
        if(pCRC32 && crc32)
            *pCRC32 = crc32->checksum();
    //KL_TMEASURE_END()
    }
};

KLCSC_DECL void KLPAR_SerializeToFileID2(
                                int                     id,
                                AVP_dword               lFlags,
                                KLPAR::Params*          pParams)
{
    CFileIdWriteStream fs(id);
    SerParamsFormat pf;
    pf.m_dwFlags = lFlags;
    SerializeParams(&fs, &pf, pParams);
};

KLCSC_DECL void KLPAR_SerializeToFileName2(
                                const std::wstring&    wstrName,
                                AVP_dword               lFlags,
                                KLPAR::Params*         pParams)
{
    KLSTD::CAutoPtr<KLSTD::File> pFile;
    KLSTD_CreateFile(
                    wstrName,
                    0,
                    KLSTD::CF_CREATE_ALWAYS,
                    KLSTD::AF_WRITE,
                    KLSTD::EF_SEQUENTIAL_SCAN,
                    &pFile);
    CFileWriteStream fs(pFile);
    SerParamsFormat pf;
    pf.m_dwFlags = lFlags;
    SerializeParams(&fs, &pf, pParams);
}

KLCSC_DECL void KLPAR_SerializeToMemory2(                                
                                AVP_dword               lFlags,
                                KLPAR::Params*          pParams,
                                KLSTD::MemoryChunk**    ppChunk)
{
    CMemoryWriteStream ms;
    SerParamsFormat pf;
    pf.m_dwFlags = lFlags;
    SerializeParams(&ms, &pf, pParams, NULL, KLPAR_SER_MEMWRITE_BUFFER_SIZE);
    ms.MakeMemoryChunk(ppChunk);
}

#ifdef KLPAR_GSOAP_SUPPORTED

class CAllocator : public Allocator
{
public:
    CAllocator(struct soap* pSoap)
        :   m_pSoap(pSoap)
    {
        KLSTD_ASSERT_THROW(m_pSoap != NULL);
    };
    void* Allocate(size_t size)
    {
        void* pResult=soap_malloc(m_pSoap, size);
        KLSTD_CHKMEM(pResult);
        return pResult;
    };
protected:
    struct soap*    m_pSoap;
};


void KLPAR_SerializeToMemoryForSoap(
                                AVP_dword               lFlags,
                                KLPAR::Params*          pParams,
                                struct soap*            pSoap,
                                void*&                  pData,
                                int&                    nData,
                                AVP_dword*              pCRC32)
{
//KL_TMEASURE_BEGIN(L"KLPAR_SerializeToMemoryForSoap", 4)
    KLSTD_CHKINPTR(pSoap);
    CAllocator al(pSoap);
    CMemoryWriteStream ms;
    SerParamsFormat pf;
    pf.m_dwFlags = lFlags;
    SerializeParams(&ms, &pf, pParams, pCRC32, KLPAR_SER_MEMWRITE_BUFFER_SIZE);
    ms.MakeMemoryChunk(&al, pData, nData);
//KL_TMEASURE_END()
}

#endif

namespace KLPAR
{
    void DeserializeParams(
                        ReadStream*             pFs,
                        SerParamsFormat*        pFormat,
                        KLPAR::Params**         ppParams,
                        AVP_dword*              pCRC32,
                        int                     nBufferSize,
                        size_t                  nObjectSize)
    {
    //KL_TMEASURE_BEGIN(L"DeserializeParams", 1)
        KLSTD_CHKINPTR(pFs);
        KLSTD_CHKINPTR(pFormat);
        KLSTD_CHKOUTPTR(ppParams);

        if( pFormat->m_dwStructSize != sizeof(SerParamsFormat) )
            KLSTD_NOTIMP();

        KLSTD::CPointer<boost::crc_32_type> crc32;
        if(pCRC32)
        {
            crc32=new boost::crc_32_type;
            KLSTD_CHKMEM(crc32);
        };

        CBinReader strm(pFs, crc32, nBufferSize, nObjectSize);
        char pBuffer[KLPAR_SIGN_SIZE];
        strm.Read(pBuffer, KLPAR_SIGN_SIZE);
        if(memcmp(pBuffer, c_szSignBIN, KLPAR_SIGN_SIZE) == 0)//binary
        {
            AVP_dword dwOffset=0, dwVersion = 0, dwFlags = 0;
            strm >> dwOffset >> dwVersion >> dwFlags;
            pFormat->m_dwFlags = dwFlags;
            pFormat->m_dwVerson = dwVersion;
            CParamsBinReader reader(strm, pFormat);
            reader.Start(ppParams);
        }
        else//xmlsoap
        {
            strm.Unread(pBuffer, KLPAR_SIGN_SIZE);
    #ifdef KLPAR_GSOAP_SUPPORTED
            CGsoapAdapterR sar(strm);
		    CAutoSoap soap;
		    soap_init(soap.get());
		    param__params par;
		    par.soap_default(soap.get());
		    ;
		    soap.get()->recvfd = 0;
            soap.get()->user = &sar;
            soap.get()->frecv = sar.fread;
		    soap_begin(soap.get());
//			    soap.get()->buffering=true;
			    soap_begin_recv(soap.get());
			    KLERR_TRY
				    if(!par.soap_get(soap.get(), "root", "param:params"))
                        KLSTD_THROW(KLSTD::STDE_BADFORMAT);
                    if(sar.m_bError)
                        KLSTD_THROW(KLSTD::STDE_GENERAL);
			    KLERR_CATCH(pError)
				    soap_end_recv(soap.get());
				    soap_destroy(soap.get());
				    soap_end(soap.get());
				    KLERR_RETHROW();
			    KLERR_ENDTRY
			    soap_end_recv(soap.get());
			    ParamsFromSoap(par, ppParams);
		    soap_destroy(soap.get());
		    soap_end(soap.get());
            sar.Flush();
    #else
            KLSTD_NOTIMP();
    #endif
        };
        if(pCRC32 && crc32)
            *pCRC32 = crc32->checksum();
    //KL_TMEASURE_END()
    }
};

KLCSC_DECL void KLPAR_DeserializeFromFileID2(
                                int             id,
                                AVP_dword&      lFlags,
                                KLPAR::Params** ppParams)
{
    CFileIdReadStream fs(id);
    SerParamsFormat pf;
    long lLength;
	KL_GET_FILE_LEN_BY_FILE_ID(id, lLength );
    if(lLength < 0)
        lLength = 0;
    DeserializeParams(&fs, &pf, ppParams, NULL, -1, (size_t)lLength);
    lFlags=pf.m_dwFlags;
}

KLCSC_DECL void KLPAR_DeserializeFromFileName2(
                                const std::wstring& wstrName,
                                AVP_dword&          lFlags,
                                KLPAR::Params**     ppParams)
{
    KLSTD::CAutoPtr<KLSTD::File> pFile;
    KLSTD_CreateFile(
                    wstrName,
                    KLSTD::SF_READ,
                    KLSTD::CF_OPEN_EXISTING,
                    KLSTD::AF_READ,
                    KLSTD::EF_SEQUENTIAL_SCAN,
                    &pFile);
    CFileReadStream fs(pFile);
    SerParamsFormat pf;
    DeserializeParams(&fs, &pf, ppParams, NULL, -1, (size_t)pFile->GetSize());
    lFlags=pf.m_dwFlags;
}

namespace KLPAR
{
    void DeserializeFromMemory(
                                    const void*     pData,
                                    size_t          nData,
                                    AVP_dword&      lFlags,
                                    KLPAR::Params** ppParams,
                                    AVP_dword*      pCRC32)
    {
    //KL_TMEASURE_BEGIN(L"DeserializeFromMemory", 4)
        CMemoryReadStream ms(pData, nData);
        SerParamsFormat pf;
        pf.m_dwFlags = lFlags;
        DeserializeParams(&ms, &pf, ppParams, pCRC32, KLPAR_SER_MEMREAD_BUFFER_SIZE, nData);
        lFlags=pf.m_dwFlags;
    //KL_TMEASURE_END()
    };


    void CalcParamsMd5Hash(KLPAR::Params* pParams, unsigned char digest[c_Md5DigestSize])
    {
    //KL_TMEASURE_BEGIN(L"CalcParamsMd5Hash", 4)
        CMd5Stream md5s;
        SerParamsFormat pf;
        pf.m_dwFlags = KLPAR_SF_BINARY;
        KLPAR::SerializeParams(&md5s, &pf, pParams, NULL, KLPAR_SER_MEMREAD_BUFFER_SIZE);
        md5s.GetDigest(digest);
    //KL_TMEASURE_END()
    };
    
    KLCSC_DECL void CalcParamsMd5Hash2Binary(KLPAR::Params* pParams, KLPAR::BinaryValue** ppResult)
    {
        KLSTD_CHKOUTPTR(ppResult);
        unsigned char digest[c_Md5DigestSize];
        CalcParamsMd5Hash(pParams, digest);
        KLPAR::BinaryValuePtr pBinaryValue;
        KLPAR::CreateValue(KLPAR::binary_wrapper_t(digest, c_Md5DigestSize), &pBinaryValue);
        pBinaryValue.CopyTo(ppResult);
    };
}

KLCSC_DECL void KLPAR_DeserializeFromMemory2(
                                const void*     pData,
                                size_t          nData,
                                AVP_dword&      lFlags,
                                KLPAR::Params** ppParams)
{
    KLSTD_CHKINPTR(pData);
    KLSTD_CHK(nData, nData > 0);
    KLSTD_CHKOUTPTR(ppParams);

    DeserializeFromMemory(
                        pData,
                        nData,
                        lFlags,
                        ppParams);
}

KLCSC_DECL void KLPAR_SerializeToFileID(int id, const KLPAR::Params* pParams)
{
    KLSTD_CHKINPTR(pParams);
    KLPAR_SerializeToFileID2(id, 0, (KLPAR::Params*)pParams);
}

KLCSC_DECL void KLPAR_SerializeToFileName(const std::wstring& wstrName, const KLPAR::Params* pParams)
{
    KLSTD_CHKINPTR(pParams);
    KLPAR_SerializeToFileName2(wstrName, 0, (KLPAR::Params*)pParams);
}

KLCSC_DECL void KLPAR_SerializeToMemory(const KLPAR::Params* pParams, KLSTD::MemoryChunk** ppChunk)
{
    KLSTD_CHKINPTR(pParams);
    KLSTD_CHKOUTPTR(ppChunk);
    KLPAR_SerializeToMemory2(0, (KLPAR::Params*)pParams, ppChunk);
}

KLCSC_DECL void KLPAR_DeserializeFromFileID(int id, KLPAR::Params** ppParams)
{
    KLSTD_CHKOUTPTR(ppParams);
    AVP_dword dwFlags=0;
    KLPAR_DeserializeFromFileID2(id, dwFlags, ppParams);
}

KLCSC_DECL void KLPAR_DeserializeFromFileName(const std::wstring& wstrName, KLPAR::Params** ppParams)
{
    KLSTD_CHKOUTPTR(ppParams);
    AVP_dword dwFlags=0;
    KLPAR_DeserializeFromFileName2(wstrName, dwFlags, ppParams);
}

KLCSC_DECL void KLPAR_DeserializeFromMemory(const void* pData, size_t nData, KLPAR::Params** ppParams)
{
    KLSTD_CHKINPTR(pData);
    KLSTD_CHK(nData, nData > 0);
    KLSTD_CHKOUTPTR(ppParams);
    
    AVP_dword dwFlags=0;
    KLPAR_DeserializeFromMemory2(pData, nData, dwFlags, ppParams);
}

KLCSC_DECL void KLPAR_SerializeToFile(
                    const wchar_t*          szwName, 
                    const KLPAR::Params*    pParams)
{
    KLSTD_CHKINPTR(pParams);
    KLSTD_CHK(szwName, szwName && szwName[0]);
    ;
    KLPAR_SerializeToFileName2(szwName, 0, (KLPAR::Params*)pParams);
}

KLCSC_DECL void KLPAR_DeserializeFromFile(
                    const wchar_t* szwName, 
                    KLPAR::Params** ppParams)
{
    KLSTD_CHKOUTPTR(ppParams);
    KLSTD_CHK(szwName, szwName && szwName[0]);
    ;
    AVP_dword dwFlags = 0;
    KLPAR_DeserializeFromFileName2(szwName, dwFlags, ppParams);
}
