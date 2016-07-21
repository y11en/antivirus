#ifndef __KLUT_LOGFILE_H__
#define __KLUT_LOGFILE_H__

#include <std/base/klstd.h>
#include <std/err/error2string.h>
#include <common/locevents.h>
#include <std/err/errloc_intervals.h>
#include "../../apps/klbackup/errlocids.h"

__inline std::wostream& operator << (std::wostream& os, const KLSTD_CW2CW& x)
{
    os << ((const wchar_t*)x);
    return os;
}

namespace KLBKP
{
    inline std::wstring FormatString(
                    int             nCode,
                    const wchar_t*  szwPar1 = NULL,
                    const wchar_t*  szwPar2 = NULL,
                    const wchar_t*  szwPar3 = NULL,
                    const wchar_t*  szwPar4 = NULL,
                    const wchar_t*  szwPar5 = NULL,
                    const wchar_t*  szwPar6 = NULL,
                    const wchar_t*  szwPar7 = NULL,
                    const wchar_t*  szwPar8 = NULL,
                    const wchar_t*  szwPar9 = NULL)
    {
        KLSTD::klwstr_t wstrRes;
        KLERR_LocFormatModuleString(
                    KLERR_LOCMOD_KLBACKUP, 
                    nCode,
                    wstrRes.outref(),
                    szwPar1,
                    szwPar2,
                    szwPar3,
                    szwPar4,
                    szwPar5,
                    szwPar6,
                    szwPar7,
                    szwPar8,
                    szwPar9);
        ;
        return wstrRes;
    };
};

namespace KLUT
{
    class KlUtLogFile
    {
    public:
        KlUtLogFile();
        virtual ~KlUtLogFile();
        /*!
          \brief	Creates 
          \param	szwFile IN filename, NULL means stdout
        */
        void Create(const wchar_t* szwFile);
        void Destroy();
        void WriteLog(const char* szBuffer);
        void WriteLog(const wchar_t* szBuffer);
        int m_nLocOK, m_nLocFailure, m_nLocFailureCode;
    protected:
        void WriteLogI(const wchar_t* szBuffer);
    protected:        
        FILE*   m_pLog;
        bool    m_bConvertLogOut, m_bIsConsole;
        bool    m_bClose;
    };

    extern KlUtLogFile g_oKlUtLOgFile;

    inline std::wstring Int2Str(unsigned x, bool bDec = true)
    {
        wchar_t szBuffer[64];
        _ultow(x, szBuffer, bDec?10:16);
        return szBuffer;
    };

    inline std::wstring Int2Str(int x, bool bDec = true)
    {
        wchar_t szBuffer[64];
        _ltow(x, szBuffer, bDec?10:16);
        return szBuffer;
    };

    template <class T>
        void WriteLog(const T* szBuffer)
    {
        g_oKlUtLOgFile.WriteLog(szBuffer);
    };

    #define KLUT_LOG(_x)    \
            {   \
                std::wstringstream _klbkos;   \
                _klbkos << _x;   \
                KLUT::WriteLog(  _klbkos.str().c_str()  );    \
            }

    #define KLUT_LOGOK()            KLUT_LOG( KLLOC::LoadLocString(KLUT::g_oKlUtLOgFile.m_nLocOK, L"OK") << std::endl)
    #define KLUT_LOGOK2(_x)            KLUT_LOG(KLLOC::LoadLocString(KLUT::g_oKlUtLOgFile.m_nLocOK, L"OK") << std::endl << _x << std::endl)

    #define KLUT_LOGFAILED(_code)   \
                    KLUT_LOG(KLBKP::FormatString(KLBACKUP::KLBKEL_FAILED) << L" - 0x" \
                    << KLUT::Int2Str(unsigned(_code), false) << std::endl)

    #define KLUT_LOGFAILED_STR(_str)   \
                    KLUT_LOG(KLBKP::FormatString(KLBACKUP::KLBKEL_FAILED) << L" - " \
                    << _str << std::endl)

    #define KLUT_LOGEXCPT(_pError)   \
        if(KLERR_IsErrorLocalized(_pError)) \
        {   \
            KLSTD::klwstr_t wstrStr;    \
            KLERR_LocFormatErrorString(_pError, wstrStr.outref());   \
            KLUT_LOG( KLBKP::FormatString(KLBACKUP::KLBKEL_FAILED) \
                        << L" - " << ((const wchar_t*)wstrStr) << std::endl );  \
        }   \
        else    \
        {   \
            KLUT_LOG(KLBKP::FormatString(KLBACKUP::KLBKEL_FAILED) << L" - " \
            << KLUT::Int2Str(unsigned(_pError->GetId()), true)   \
            << L" ('" << _pError->GetMsg() << L"')"  \
            << std::endl);  \
        };
};

#endif //__KLUT_LOGFILE_H__
