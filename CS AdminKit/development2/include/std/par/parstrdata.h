/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parstrdata.h
 * \author	Andrew Kazachkov
 * \date	10.09.2004 10:01:18
 * \brief	
 * 
 */

#ifndef __KLPARSTRDATA_H__
#define __KLPARSTRDATA_H__

KLCSC_DECL int KLSTD_FASTCALL klstd_wcscmp (
        const wchar_t * src,
        const wchar_t * dst);

KLCSC_DECL size_t __fastcall my_wcslen (
        const wchar_t * wcs);


namespace KLPAR
{
    struct strdata_stat_t
    {
        strdata_stat_t(const wchar_t* szwString)
        {
            assign(szwString);
        };
        
        void assign(const wchar_t* szwString)
        {
            m_szwString = szwString?szwString:L"";
            m_nLength = my_wcslen(m_szwString);
        };

        const wchar_t*  m_szwString;
        size_t          m_nLength;
    };

    struct KLCSC_DECL string_t
    {

    protected:
        struct _string_data_t
        {
            volatile long   m_lCntRef;//! reference counter
            size_t          m_lLength;//! length of string without terminating 0
        };

        struct string_data_t : public _string_data_t
        {
            wchar_t m_data[1];//! string data terminated with 0
        };

        static string_data_t* AllocateString(const wchar_t* pData, size_t nSize = -1);
        static void ReleaseString(string_data_t* pData);

    public:
        string_t();

        string_t(const wchar_t* pData);

        string_t(const strdata_stat_t* pData);

        string_t(const string_t& data);

        ~string_t();

        void clear();

        void assign(const string_t& data);

        inline string_t& operator = (const string_t& data)
        {
            assign(data);
            return *this;
        };
                
        inline const wchar_t* KLSTD_FASTCALL get() const
        {
            return (m_pData) 
                    ?   &m_pData->m_data[0] 
                    :   (m_pStaticData ? m_pStaticData->m_szwString : L"");
        };

        inline bool KLSTD_FASTCALL empty() const 
        {
            return !get()[0];
        };

        inline size_t KLSTD_FASTCALL getLength() const
        {
            return m_pData 
                    ?   (size_t)m_pData->m_lLength 
                    :   (m_pStaticData ? m_pStaticData->m_nLength : 0);
        };

        wchar_t* LockBuffer(size_t nCharsWithoutNull);

        void UnlockBuffer();

        inline int KLSTD_FASTCALL compare(const string_t& data) const
        {
            return klstd_wcscmp(get(), data.get());
        }

        inline bool KLSTD_FASTCALL operator < (const string_t& data) const
        {
            return compare(data) < 0;
        }

        inline bool KLSTD_FASTCALL operator == (const string_t& data) const
        {
            return compare(data) == 0;
        }

        inline bool KLSTD_FASTCALL operator > (const string_t& data) const
        {
            return compare(data) > 0;
        }

    protected:
        string_data_t*          m_pData;
        const strdata_stat_t*   m_pStaticData;
    };
};
#endif //__KLPARSTRDATA_H__

