/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parstreams.h
 * \author	Andrew Kazachkov
 * \date	05.12.2003 12:32:29
 * \brief	
 * 
 */


#ifndef __KL_PARSTREAMS_H__
#define __KL_PARSTREAMS_H__

#include <boost/crc.hpp>
#include <std/par/value.h>
#include <avp/byteorder.h>
#include <limits.h>
#include <std/par/parstrdata.h>

namespace KLPAR
{
    #define KLPAR_SER_MAX_OBJECT_SIZE   (size_t(INT_MAX/8))

    class KLSTD_NOVTABLE WriteStream
    {
    public:
        virtual void Write(const void* pData, int nDataToWrite) = 0;
    };

    class KLSTD_NOVTABLE ReadStream
    {
    public:
        virtual int Read(void* pBuffer, int nBuffer) = 0;
        
        virtual bool IfInMemory(AVP_byte*& pBuffer, int& nBuffer)
        {
            return false;
        };
    };

    #define KLPAR_PREPARE_W(_x) _x=MachineToLittleEndian(_x)
    #define KLPAR_PREPARE_R(_x) _x=LittleEndianToMachine(_x)

    class CBinWriter
    {
    public:
        typedef enum{
            BWF_UTF8STRINGS=0x1
        }flags_t;

        CBinWriter(
                    WriteStream*        pWriteStream,
                    AVP_dword           dwFlags,
                    boost::crc_32_type* pCrc32,
                    int                 nBufferSize = -1);

        ~CBinWriter();

        inline CBinWriter& operator << (AVP_byte x)
        {
            Write(&x, sizeof(x));
            return *this;
        };

        inline CBinWriter& operator << (AVP_dword x)
        {
            Write(&x, sizeof(x));
            return *this;
        };

        inline CBinWriter& operator << (AVP_qword x)
        {
            Write(&x, sizeof(x));
            return *this;
        };

        inline CBinWriter& operator << (float x)
        {
            Write(&x, sizeof(x));
            return *this;
        };

        inline CBinWriter& operator << (double x)
        {
            Write(&x, sizeof(x));
            return *this;
        };

        inline CBinWriter& operator << (bool x)
        {
            AVP_byte y=x?1:0;
            Write(&y, sizeof(y));
            return *this;
        };

        CBinWriter& operator << (const std::wstring& x);

        CBinWriter& operator << (const wchar_t* x);

        inline CBinWriter& operator << (const std::string& x)
        {
            const AVP_dword dwSize=x.size();
			if ( dwSize!=0 )
			{
				Write(&dwSize, sizeof(dwSize));
				Write(&x[0], dwSize*sizeof(char));
			}
            return *this;
        };

        inline CBinWriter& operator << (const char* x)
        {
            const AVP_dword dwSize= (x!=NULL) ? strlen(x) : 0;
            Write(&dwSize, sizeof(dwSize));
            Write(x, dwSize*sizeof(char));
            return *this;
        };

        void Write(const void* pDataToWrite, const int nDataToWrite);

        void Flush();
    protected:
        void WriteWideString(const wchar_t* x, size_t n);
        void FlushBuffer(bool bLast = false);
    protected:
        AVP_dword           m_dwFlags;
        AVP_byte*           m_pBuffer;
        const int           m_nBuffer;
        int                 m_nBufferPtr;
        WriteStream*        m_pStream;
        boost::crc_32_type* m_pCrc32;
        void*                   m_pZStream;
        std::vector<AVP_byte>   m_vecZBuffer;
    };


    class CBinReader
    {
    public:
        CBinReader(
                ReadStream* pReadStream,
                boost::crc_32_type* pCrc32,
                int                 nBufferSize = -1,
                size_t              nObjectSize = 0);
        ~CBinReader();
        KLSTD_INLINEONLY CBinReader& operator >> (AVP_byte& x)
        {
            Read(&x, sizeof(x));
            return *this;
        };

        KLSTD_INLINEONLY CBinReader& operator >> (AVP_dword& x)
        {
            Read(&x, sizeof(x));
            return *this;
        };

        KLSTD_INLINEONLY CBinReader& operator >> (AVP_qword& x)
        {
            Read(&x, sizeof(x));
            return *this;
        };

        KLSTD_INLINEONLY CBinReader& operator >> (float& x)
        {
            Read(&x, sizeof(x));
            return *this;
        };

        KLSTD_INLINEONLY CBinReader& operator >> (double& x)
        {
            Read(&x, sizeof(x));
            return *this;
        };

        KLSTD_INLINEONLY CBinReader& operator >> (bool& x)
        {
            AVP_byte y;
            Read(&y, sizeof(y));
            x=(y!=0);
            return *this;
        };

        CBinReader& operator >> (std::wstring& x);

        CBinReader& operator >> (std::string& x);

        CBinReader& operator >> (KLPAR::string_t& x);

        int Read(void* pBuffer, int nBytesToRead, bool bThrowIfEOF=true);        
        void Unread(const void* pData, int nBytesToUnRead);
        void Flush();
        
        // doesn't returns real size !!!
        KLSTD_INLINEONLY size_t GetObjectSize() const
        {
            return m_pZStream ? KLPAR_SER_MAX_OBJECT_SIZE : m_nObjectSize;
        };
    protected:
        void FillBuffer();
    protected:
        AVP_byte*           m_pBuffer;
        const int           m_nBuffer;
        int                 m_nBufferSize;
        int                 m_nBufferPtr;
        ReadStream*         m_pStream;
        bool                m_bEOF;
        boost::crc_32_type* m_pCrc32;
        size_t              m_nObjectSize;
        void*               m_pZStream;
        std::vector<AVP_byte>   m_vecZBuffer;
        bool                m_bDirectRead;
        bool                m_bFirstRead;
    };

};
#endif //__KL_PARSTREAMS_H__
