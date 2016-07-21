/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srvinst_utils.h
 * \author	Andrew Kazachkov
 * \date	07.07.2005 16:28:18
 * \brief	Various helpers
 * 
 */

#include <server/db/dbconnection.h>
#include <common/locevents.h>
#include <std/err/error2string.h>
#include <climits>

#ifndef __KLSRVINST_UTILS_H__
#define __KLSRVINST_UTILS_H__

#define KLSRV_MAKEWORD(a, b)      ((AVP_word)(((AVP_byte)((AVP_dword)(a) & 0xff)) | ((AVP_word)((AVP_byte)((AVP_dword)(b) & 0xff))) << 8))
#define KLSRV_MAKELONG(a, b)      ((AVP_long)(((AVP_word)((AVP_dword)(a) & 0xffff)) | ((AVP_dword)((AVP_word)((AVP_dword)(b) & 0xffff))) << 16))
#define KLSRV_LOWORD(l)           ((AVP_word)((AVP_dword)(l) & 0xffff))
#define KLSRV_HIWORD(l)           ((AVP_word)((AVP_dword)(l) >> 16))
#define KLSRV_LOBYTE(w)           ((AVP_byte)((AVP_dword)(w) & 0xff))
#define KLSRV_HIBYTE(w)           ((AVP_byte)((AVP_dword)(w) >> 8))

#define KLSRV_ISGUID(x) (x && wcschr(x, L'-'))

#ifdef _DEBUG
#define KLSRV_CHKGUID(x)    KLSTD_ASSERT(KLSRV_ISGUID(x))
#else
#define KLSRV_CHKGUID(x)
#endif

#define KLSRV_TODO_MAKESTR(x)		#x
#define KLSRV_TODO_MAKESTR2(x)		KLSRV_TODO_MAKESTR(x)
#define KLSRV_TODO(msg) message(__FILE__"(" KLSRV_TODO_MAKESTR2(__LINE__) "): TODO: " msg)

#define KLSRV_REPORT(_p, _x)    \
        if(_p)  \
        {   \
            std::wstringstream os;   \
            os << _x;   \
            _p->ReportWarning((os.str().c_str()));    \
        }

#define KLSRV_INADDR_NONE   ULONG_MAX
unsigned long KLSRV_inet_addr(const char* p);
unsigned long KLSRV_htonl(unsigned long x);
unsigned long KLSRV_ntohl(unsigned long x);

namespace KLSRV
{    

    //! Convert string to uppercase according to the current language
    void ToUpperCase(const std::wstring& wstrString, std::wstring& wstrResult);

    //! Convert string to lowercase according to the current language
    void ToLowerCase(const std::wstring& wstrString, std::wstring& wstrResult);

    inline std::wstring ToUpperCase(const std::wstring& wstrString)
    {
        std::wstring wstrResult;
        ToUpperCase(wstrString, wstrResult);
        return wstrResult;
    };

    inline std::wstring ToLowerCase(const std::wstring& wstrString)
    {
        std::wstring wstrResult;
        ToLowerCase(wstrString, wstrResult);
        return wstrResult;
    };    

    KLCSSRV_DECL KLSTD_NOTHROW AVP_dword LoadServerFlag(const wchar_t* szName, AVP_dword dwDefaultValue) throw();
    KLCSSRV_DECL KLSTD_NOTHROW void SaveServerFlag(const wchar_t* szName, AVP_dword dwValue) throw();
        
    KLCSSRV_DECL KLSTD_NOTHROW bool LoadServerFlag(const wchar_t* szName, bool bDefaultValue) throw();
    KLCSSRV_DECL KLSTD_NOTHROW void SaveServerFlag(const wchar_t* szName, bool bValue) throw();
    
    KLCSSRV_DECL KLSTD_NOTHROW AVP_longlong LoadServerFlag(const wchar_t* szName, AVP_longlong llDefaultValue) throw();
    KLCSSRV_DECL KLSTD_NOTHROW void SaveServerFlag(const wchar_t* szName, AVP_longlong lValue) throw();

    inline bool LoadScannedFlag()
    {
        return KLSRV::LoadServerFlag(KLSRV_NETWORK_WAS_SCANNED, false);
    };

    inline void SaveScannedFlag(bool bScanned)
    {
        SaveServerFlag(KLSRV_NETWORK_WAS_SCANNED, bScanned);
    };


    KLSTD_INLINEONLY void LoadLocLibrary(const std::wstring& wstrPath)
    {
        KLLOC::LoadLocLibrary(wstrPath);
    };

    KLSTD_INLINEONLY std::wstring LoadLocString(int id, const wchar_t* szwDefault)
    {
        return KLLOC::LoadLocString(id, szwDefault);
    };

    KLSTD_INLINEONLY std::wstring FormatLocMessage(
                            int            idTemplate, 
                            const wchar_t* szwDefTemplate,
                            const wchar_t** pArgs)
    {
        return KLLOC::FormatLocMessage(idTemplate, szwDefTemplate, pArgs);
    };

    bool isIpAddress(const wchar_t* szName, unsigned long& ulIpAddress);
    bool isIpAddress(const char* szName, unsigned long& ulIpAddress);

    bool isDnsName(const wchar_t*   szName,
                    std::wstring&   wstrDnsHostName,
                    std::wstring&   wstrDnsDomainName);

    bool isNetBiosName(
                        const wchar_t*  szName,
                        std::wstring&   wstrHostName,
                        std::wstring&   wstrDomainName);

    void ConvertParamsVectorToArrayValue(
		const std::vector< KLSTD::KLAdapt< KLSTD::CAutoPtr< KLPAR::Params > > > & vectParams,
		KLPAR::ArrayValue** ppArray );

    void ConvertArrayValueToParamsVector(
		KLSTD::CAutoPtr<KLPAR::ArrayValue> pArrayValue,
		std::vector< KLSTD::KLAdapt< KLSTD::CAutoPtr< KLPAR::Params > > > & vectParams);
    
    /*!
      \brief	Converts params into blob suitable to save into DB as 
                IMAGE data type. Resulting blob may be converted back with
                Blob2Params function

      \param	pData IN Params to convert if pData is NULL function 
                also returns NULL 
      \param	bCompressed IN compressed params (recommended to save 
                space in DB as binary serialized params often may be 
                compressed up to 10 times)
      \return	resulting blob, NULL if pData is NULL
    */
    KLSTD::MemoryChunkPtr Params2Blob(KLPAR::ParamsPtr pData, bool bCompressed = true);
   

    /*!
      \brief	Converts blob generated with Params2Blob back into params.                

      \param	pChunk IN blob generated with Params2Blob
      \return	initial params, NULL if pChunk is NULL
    */
    KLPAR::ParamsPtr Blob2Params(KLSTD::MemoryChunkPtr pChunk);

    
    
}

#endif //__KLSRVINST_UTILS_H__
