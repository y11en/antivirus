/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parbinser.cpp
 * \author	Andrew Kazachkov
 * \date	04.12.2003 15:21:58
 * \brief	
 * 
 */

#include "std/par/params.h"
#include "std/par/valenum.h"
#include "std/err/klerrors.h"
#include "std/par/parstreams.h"
#include "std/base/klbaseqi_imp.h"

#include <sstream>

#include "../../transport/zlib/zlib.h"

#define KLCS_MODULENAME L"KLPAR"

#define KLPAR_WRITER_BUFFER_SIZE    (1024*64)
#define KLPAR_READER_BUFFER_SIZE    (1024*64)
#define KLPAR_MIN_ZLIB_BUFFER_SIZE  (256)

//#ifdef _WIN32
//#pragma function(memcpy, memcmp)
//#endif

//#ifdef _WIN32
//#pragma intrinsic(memcpy)
//#endif

#define USE_FAST_MEMCPY

#define KLPAR_ZLIB_CHK(_code)   zlib_chk(_code, __FILE__, __LINE__)

namespace KLPAR
{
    #define KLPAR_ZLIB_SIGN 4
    static const char c_szSignZlib[KLPAR_ZLIB_SIGN]=
    {
        'Z','L','I','B'
    };

    #if defined(_WIN32) && defined(USE_FAST_MEMCPY)
        KLSTD_NOTHROW void * __fastcall my_memcpy (
                            void * dst,
                            const void * src,
                            size_t count) throw();
    #else
        #ifdef _WIN32
            #pragma function(memcpy)
        #endif
        KLSTD_NOTHROW KLSTD_INLINEONLY void * my_memcpy (
                            void * dst,
                            const void * src,
                            size_t count)throw()
        {
            return memcpy(dst, src, count);
        };

    #endif

    #define KLPAR_RESULT_FROM_ZLIB(x)    \
                (0xE0FE0000L | ((x) & 0x0000FFFF))


    void __fastcall zlib_chk(int code, const char* file, int line)
    {
        if(Z_OK != code && Z_STREAM_END != code)
        {
            std::wstringstream os;
            os << L"zlib error " << std::hex << code;
            KLERR_throwError(
                        KLCS_MODULENAME, 
                        KLSTD::STDE_SYSTEM, 
                        file, 
                        line, 
                        NULL, 
                        KLPAR_RESULT_FROM_ZLIB(code), 
                        os.str().c_str());
        };
    };

    inline int CBinWriter_MakeBuffeSize(int user)
    {
        if(user == -1)
            return KLPAR_WRITER_BUFFER_SIZE;
        if(user <= 1024)
            return 1024;
        return user;
    }

    CBinWriter::CBinWriter(
                        WriteStream*        pWriteStream,
                        AVP_dword           dwFlags,
                        boost::crc_32_type* pCrc32,
                        int                 nBufferSize)
        :   m_nBuffer(CBinWriter_MakeBuffeSize(nBufferSize))
        ,   m_nBufferPtr(0)
        ,   m_pStream(pWriteStream)
        ,   m_dwFlags(dwFlags)
        ,   m_pBuffer(NULL)
        ,   m_pCrc32(pCrc32)
        ,   m_pZStream( NULL )
    {
        m_pBuffer = (AVP_byte*)malloc(m_nBuffer);
        KLSTD_CHKMEM(m_pBuffer);
        if(dwFlags & KLPAR_SF_COMPRESS)
        {
            m_vecZBuffer.resize(m_nBuffer);
            z_stream * pStream = new z_stream;
            KLSTD_CHKMEM(pStream);
            KLSTD_ZEROSTRUCT(*pStream);
            KLERR_TRY
                KLPAR_ZLIB_CHK(deflateInit2(pStream, 3, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY));
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(4, pError);
                delete pStream;
                KLERR_RETHROW();
            KLERR_ENDTRY            
            m_pZStream = pStream;
            if(m_pZStream && pWriteStream)
                pWriteStream->Write(&c_szSignZlib[0], KLPAR_ZLIB_SIGN);
        };
    }

    CBinWriter::~CBinWriter()
    {
        if(m_pZStream)
        {
            deflateEnd((z_stream*)m_pZStream);
            delete (z_stream*)m_pZStream;
            m_pZStream = NULL;
        };
        free(m_pBuffer);
    };

    inline void CBinWriter::WriteWideString(const wchar_t* x, size_t n)
    {
        if(sizeof(wchar_t) == sizeof(unsigned short))
        {
            Write(x, n*sizeof(wchar_t));
        }
        else
        {
            for(size_t i =0; i < n; ++i)
            {
                unsigned short ch = (unsigned short)x[i];
                Write(&ch, sizeof(ch));
            };
        };
    };

    CBinWriter& CBinWriter::operator << (const std::wstring& x)
    {
        const AVP_dword dwSize=x.size();
		if ( dwSize!=0 )
		{
			Write(&dwSize, sizeof(dwSize));
            WriteWideString(&x[0], dwSize);
		}
        return *this;
    };

    CBinWriter& CBinWriter::operator << (const wchar_t* x)
    {
        const AVP_dword dwSize= (x!=NULL) ? wcslen(x) : 0;
        Write(&dwSize, sizeof(dwSize));
        WriteWideString(x, dwSize);
        return *this;
    };

    void CBinWriter::FlushBuffer(bool bLast)
    {
        if(m_pCrc32)
            m_pCrc32->process_bytes(m_pBuffer, m_nBufferPtr);
        if(m_pZStream && m_pStream)
        {
            z_stream& strm = *(z_stream*)m_pZStream;
            strm.next_in = m_pBuffer;
            strm.avail_in = m_nBufferPtr;
            int nFlush = bLast ? Z_FINISH : Z_NO_FLUSH;
            do{
                strm.next_out= &m_vecZBuffer[0];
                strm.avail_out = m_vecZBuffer.size();
                KLPAR_ZLIB_CHK(deflate(&strm, nFlush));
                int have = m_vecZBuffer.size() - strm.avail_out;
                if(have)
                    m_pStream->Write(&m_vecZBuffer[0], have);
            }while(strm.avail_out == 0);
        }
        else if(m_pStream)
            m_pStream->Write(m_pBuffer, m_nBufferPtr);
        m_nBufferPtr=0;
    };
    
    void CBinWriter::Flush()
    {
        FlushBuffer(true);
    };


    void CBinWriter::Write(const void* pDataToWrite, const int nDataToWrite)
    {
        KLSTD_ASSERT(nDataToWrite >= 0);
        if(nDataToWrite <= 0)
            return;
        AVP_byte*   pData=(AVP_byte*)pDataToWrite;
        int         nData = nDataToWrite;
        ;
        while(nData > 0)
        {
            int nSpaceInBuffer = m_nBuffer - m_nBufferPtr;
            KLSTD_ASSERT(nSpaceInBuffer >= 0);
            if(nSpaceInBuffer > 0)
            {
                int nWriteSize=nSpaceInBuffer < nData ? nSpaceInBuffer : nData;
                my_memcpy(&m_pBuffer[m_nBufferPtr], pData, nWriteSize);
                m_nBufferPtr+=nWriteSize;
                pData+=nWriteSize;
                nData-=nWriteSize;
                KLSTD_ASSERT(m_nBufferPtr <= m_nBuffer);
                KLSTD_ASSERT(nData >= 0);
            }
            else
                FlushBuffer();
        };
    };

    inline int CBinReader_MakeBuffeSize(int user)
    {
        if(user == -1)
            return KLPAR_READER_BUFFER_SIZE;
        if(user <= 2*4096)
            return 2*4096;
        return user;
    }

    const size_t c_nObjectSizeDefault = 32*1024*1024; // 32 Mb


    CBinReader::CBinReader(
                    ReadStream*         pReadStream,
                    boost::crc_32_type* pCrc32,
                    int                 nBufferSize,
                    size_t              nObjectSize)
        :   m_nBuffer(CBinReader_MakeBuffeSize(nBufferSize))
        ,   m_nBufferPtr(0)
        ,   m_nBufferSize(0)
        ,   m_pStream(pReadStream)
        ,   m_pBuffer(NULL)
        ,   m_bEOF(false)
        ,   m_pCrc32(pCrc32)
        ,   m_nObjectSize(nObjectSize ? nObjectSize : c_nObjectSizeDefault)
        ,   m_pZStream(NULL)
        ,   m_bDirectRead(false)
        ,   m_bFirstRead(true)
    {
        if(m_pStream->IfInMemory(m_pBuffer, const_cast<int&>(m_nBuffer)))
        {
            KLSTD_ASSERT(m_nBuffer > 0);
            //m_nBufferSize = m_nBuffer;
            m_bDirectRead = true;
        }
        else
        {
            KLSTD_ASSERT( m_nBuffer > 0);
            m_pBuffer = (AVP_byte*)malloc(m_nBuffer);
            KLSTD_CHKMEM(m_pBuffer);
            m_bDirectRead = false;
        };
    }

    CBinReader::~CBinReader()
    {
        if(m_pZStream)
        {
            inflateEnd((z_stream*)m_pZStream);
            delete (z_stream*)m_pZStream;
            m_pZStream = NULL;
        };
        if(!m_bDirectRead)
            free(m_pBuffer);
    };

    CBinReader& CBinReader::operator >> (std::wstring& x)
    {
        AVP_dword dwSize=0;
        Read(&dwSize, sizeof(dwSize));
        if(dwSize > m_nObjectSize)
            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
		if ( dwSize!=0 ) 
		{
            if(sizeof(wchar_t) == sizeof(unsigned short))
            {
                x.resize(dwSize);
                Read(&x[0], dwSize*sizeof(wchar_t));
            }
            else
            {
                x.reserve(dwSize);
                unsigned short ch = 0;
                for(size_t i = 0; i < dwSize; ++i)
                {
                    Read(&ch, sizeof(ch));
                    x.push_back((wchar_t)ch);
                };
            };
		}
		else 
		{
			x.clear();
		}
        return *this;
    };


    CBinReader& CBinReader::operator >> (std::string& x)
    {
        AVP_dword dwSize=0;
        Read(&dwSize, sizeof(dwSize));
        if(dwSize > m_nObjectSize)
            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
		if ( dwSize!=0 ) 
		{
			x.resize(dwSize);
			Read(&x[0], dwSize*sizeof(char));
		}
		else
		{
			x.clear();
		}
        return *this;
    };

    CBinReader& CBinReader::operator >> (KLPAR::string_t& x)
    {
        AVP_dword dwSize=0;
        Read(&dwSize, sizeof(dwSize));
        if(dwSize > m_nObjectSize)
            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
		if ( dwSize!=0 ) 
		{
            if(sizeof(wchar_t) == sizeof(unsigned short))
            {
                Read(x.LockBuffer(dwSize), dwSize*sizeof(wchar_t));
                x.UnlockBuffer();
            }
            else
            {
                wchar_t* pData = x.LockBuffer(dwSize);
                unsigned short ch = 0;
                for(size_t i = 0; i < dwSize; ++i)
                {
                    Read(&ch, sizeof(ch));
                    *pData = (wchar_t)ch;
                    ++pData ;
                };
                x.UnlockBuffer();
            };
		}
		else 
		{
			x.clear();
		}
        return *this;
    };

    //if m_bDirectRead (without compression) then first call initializes buffer and the second is always final
    void CBinReader::FillBuffer()
    {
        if(m_nBufferPtr > 0 && m_pCrc32 && m_nBufferSize > 0)
            m_pCrc32->process_bytes(m_pBuffer, m_nBufferPtr);
        if(m_bFirstRead)
        {
            KLSTD_ASSERT(!m_nBufferPtr);
            KLSTD_ASSERT(!m_nBufferSize);
            AVP_byte    buffer[KLPAR_ZLIB_SIGN] = {0};
            size_t      nWasRead = 0;
            if(m_bDirectRead)
            {
                nWasRead = std::min(m_nBuffer, (int)KLPAR_ZLIB_SIGN);
                memcpy(&buffer[0], m_pBuffer, nWasRead);
            }                
            else
                nWasRead = m_pStream->Read(buffer, KLPAR_ZLIB_SIGN);

            if( KLPAR_ZLIB_SIGN == nWasRead && 
                0 == memcmp(&buffer[0], c_szSignZlib, nWasRead))
            {                
                z_stream * pStream = new z_stream;
                KLSTD_CHKMEM(pStream);
                KLSTD_ZEROSTRUCT(*pStream);
                KLERR_TRY
                    KLPAR_ZLIB_CHK(inflateInit2(pStream, -MAX_WBITS));
                KLERR_CATCH(pError)
                    KLERR_SAY_FAILURE(4, pError);
                    delete pStream;
                    KLERR_RETHROW();
                KLERR_ENDTRY
                m_pZStream = pStream;
                // no direct reading
                if(m_bDirectRead)
                {                    
                    const_cast<int&>(m_nBuffer) = KLPAR_READER_BUFFER_SIZE;
                    m_pBuffer = (AVP_byte*)malloc(m_nBuffer);
                    KLSTD_CHKMEM(m_pBuffer);
                    m_bDirectRead = false;
                    
                    //fake call
                    nWasRead = m_pStream->Read(buffer, KLPAR_ZLIB_SIGN);
                };
                m_vecZBuffer.resize(CBinReader_MakeBuffeSize(m_nBuffer));
            }
            else
            {
                if(!m_bDirectRead)
                {
                    memcpy(m_pBuffer, &buffer[0], nWasRead);
                    m_nBufferSize = nWasRead;
                };
            };            
        }
        else
            m_nBufferSize = 0;
        ;
        if(m_pZStream)
        {
            KLSTD_ASSERT(!m_nBufferSize);
            z_stream& strm = *(z_stream*)m_pZStream;
            if(m_bFirstRead || strm.avail_out != 0)
            {
                strm.avail_in = m_pStream->Read(&m_vecZBuffer[0], m_vecZBuffer.size());
                strm.next_in = &m_vecZBuffer[0];
                if( 0 == strm.avail_in )
                    m_bEOF = true;
            };
            if(!m_bEOF)
            {
                //do{
                    strm.avail_out = m_nBuffer;
                    strm.next_out = m_pBuffer;
                    int ret = inflate(&strm, Z_NO_FLUSH);
                    if( Z_STREAM_END == ret)
                        m_bEOF = true;
                    KLPAR_ZLIB_CHK(ret);
                    int have = m_nBuffer - strm.avail_out;
                //} while (strm.avail_out == 0);

                m_nBufferSize = have;
            };
            //m_bEOF = (0 == strm.avail_in);
            m_nBufferPtr=0;
        }
        else
        {
            m_nBufferSize += m_bDirectRead ? m_nBuffer : m_pStream->Read(m_pBuffer+m_nBufferSize, m_nBuffer-m_nBufferSize);
            m_nBufferPtr=0;
            
            if(!m_bFirstRead && m_bDirectRead)
                m_nBufferSize = 0;
            m_bEOF = (!m_nBuffer || m_nBufferSize != m_nBuffer);
        };
        KLSTD_ASSERT_THROW(m_nBufferSize <= m_nBuffer);
        m_bFirstRead = false;
    };

    void CBinReader::Flush()
    {
        if(m_nBufferPtr > 0 && m_pCrc32 && m_nBufferSize > 0)
            m_pCrc32->process_bytes(m_pBuffer, m_nBufferPtr);        
    };

    #define CBinReader_WRITE(_dst, _src, _type) *((_type*)(void*)_dst) = *((_type*)(void*)_src)

    int CBinReader::Read(void* pBuffer, int nBytesToRead, bool bThrowIfEOF)
    {
        KLSTD_ASSERT(nBytesToRead >= 0);
        if(nBytesToRead <= 0)
            return 0;
        AVP_byte*   pData=(AVP_byte*)pBuffer;
        int         nData = nBytesToRead;
        ;
        while(nData > 0)
        {
            int nSpaceInBuffer = m_nBufferSize - m_nBufferPtr;
            KLSTD_ASSERT(nSpaceInBuffer >= 0);
            if(nSpaceInBuffer > 0)
            {
                int nReadSize = nSpaceInBuffer < nData ? nSpaceInBuffer : nData;
                switch(nReadSize)
                {
                case sizeof(AVP_byte):
                    CBinReader_WRITE(pData, &m_pBuffer[m_nBufferPtr], AVP_byte);
                    break;
                case sizeof(AVP_word):
                    CBinReader_WRITE(pData, &m_pBuffer[m_nBufferPtr], AVP_word);
                    break;
                case sizeof(AVP_dword):
                    CBinReader_WRITE(pData, &m_pBuffer[m_nBufferPtr], AVP_dword);
                    break;
                case sizeof(AVP_qword):
                    CBinReader_WRITE(pData, &m_pBuffer[m_nBufferPtr], AVP_qword);
                    break;
                default:
                    my_memcpy(pData, &m_pBuffer[m_nBufferPtr], nReadSize);
                    break;
                };                
                m_nBufferPtr+=nReadSize;
                pData += nReadSize;
                nData -= nReadSize;
                KLSTD_ASSERT(m_nBufferPtr <= m_nBufferSize);
                KLSTD_ASSERT(nData >= 0);
            }
            else
            {
                if(m_bEOF)
                {
                    if(bThrowIfEOF)
                        KLSTD_THROW(KLSTD::STDE_BADFORMAT);
                    return nBytesToRead-nData;
                };
                FillBuffer();
            };
        };
        return nBytesToRead;
    };

    void CBinReader::Unread(const void* pData, int nBytesToUnRead)
    {
        int nSpaceInBuffer = m_nBufferSize - m_nBufferPtr;
        KLSTD_ASSERT(nSpaceInBuffer >= 0);
        KLSTD_ASSERT_THROW(nBytesToUnRead <= m_nBufferPtr);
		if(!m_bDirectRead)
			memcpy(&m_pBuffer[m_nBufferPtr-nBytesToUnRead], pData, nBytesToUnRead);
        m_nBufferPtr-=nBytesToUnRead;
    };

#if defined(_WIN32) && defined(USE_FAST_MEMCPY)
    namespace
    {
        const size_t c_nBlockSize = 8*1024; //(not more than half of L1 cache size)
        const size_t BRUST_LEN = 32;

        __inline KLSTD_NOTHROW void * __fastcall my_memcpy__(
                        void*           dst,
                        const void*     src,
                        size_t          count) throw()
        {
            if(count > c_nBlockSize)
            {
                const int *p1 = (const int *)src;
                int *p2 = (int*)dst;
                int a, b, tmp=0;
                const int nCycles = count - c_nBlockSize;
		        for (a = 0; a < nCycles; a+= c_nBlockSize)
		        {
			        // load mem to cache
			        for(b = 0; b < c_nBlockSize; b += BRUST_LEN)
			        {
				        tmp += *(int *)((char *)p1 + a + b);
				        //*(int *)((char *)p2 + a + b)=tmp;
			        }

			        //copy memory
			        for(b = 0; b < c_nBlockSize; b += 32)
			        {
				        *(int *)((char *)p2 + a + b)= *(int *)((char *)p1 + a + b);
				        *(int *)((char *)p2 + a + b + 4)= *(int *)((char *)p1 + a + b + 4);
				        *(int *)((char *)p2 + a + b + 8)= *(int *)((char *)p1 + a + b + 8);
				        *(int *)((char *)p2 + a + b + 12)= *(int *)((char *)p1 + a + b + 12);
				        *(int *)((char *)p2 + a + b + 16)= *(int *)((char *)p1 + a + b + 16);
				        *(int *)((char *)p2 + a + b + 20)= *(int *)((char *)p1 + a + b + 20);
				        *(int *)((char *)p2 + a + b + 24)= *(int *)((char *)p1 + a + b + 24);
				        *(int *)((char *)p2 + a + b + 28)= *(int *)((char *)p1 + a + b + 28);
			        }
		        };
                // остаток
                const size_t nData = (a < count)
                        ?   count - a
                        :   0;
                if(nData)
                {
                    memcpy(((AVP_byte*)dst)+a, ((AVP_byte*)src)+a, nData);
                };
            }
            else
                memcpy(dst, src, count);
            return dst;
        };
    };

    KLSTD_NOTHROW void * __fastcall my_memcpy(
                        void * dst,
                        const void * src,
                        size_t count) throw()
    {
        const int nAlign = 4;
        const AVP_byte nMask = 0x3;
        const AVP_byte* pSrc = (const AVP_byte*)src;
        AVP_byte* pDst = (AVP_byte*)dst;
        const AVP_byte* p = pSrc;
        if(count > nAlign && (AVP_byte(p) & nMask) != 0)
        {                            
            int nDelta = nAlign + (AVP_byte(p) & ~nMask) - AVP_byte(p);
            for(int jk =0; jk < nDelta; ++jk)
                pDst[jk] = p[jk];
            my_memcpy__(pDst+nDelta, pSrc+nDelta, count-nDelta);
        }
        else
            my_memcpy__(pDst, pSrc, count);
        return dst;
    };
#endif
};
