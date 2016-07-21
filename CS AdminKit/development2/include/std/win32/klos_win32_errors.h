#ifndef __KLOS_WIN32_ERRORS_H__
#define __KLOS_WIN32_ERRORS_H__
/*KLCSAK_PUBLIC_HEADER*/

#ifdef _WIN32

namespace KLSTD
{
    inline void KLSTD_FASTCALL klstd_chk_le(BOOL b, LPCSTR szFile, int nLine)
    {
        if(!b)
            KLSTD_ThrowLastError(   NULL,
                                    szFile,
                                    nLine,
                                    false);
    };

    inline void KLSTD_FASTCALL klstd_chk_hr(HRESULT hr, LPCSTR szFile, int nLine)
    {
        if(FAILED(hr))
            KLSTD_ThrowLastErrorCode(   NULL,
                                        hr,
                                        true,
                                        szFile,
                                        nLine,
                                        false);
    };

    inline void KLSTD_FASTCALL klstd_chk_rg(LONG res, LPCSTR szFile, int nLine)
    {
        if(res != NO_ERROR)
            KLSTD_ThrowLastErrorCode(   NULL,
                                        res,
                                        true,
                                        szFile,
                                        nLine,
                                        false);
    };
};

/*! 
    Simple processing win32 errors. 
    Use this macro for calls that return NULL or FALSE and use 
    SetLastError in case if error occured. 

    Example:

        KLSTD_LECHK(SetFileAttributes(
            szPath, 
            dwAttr & ~FILE_ATTRIBUTE_READONLY));

*/
#define KLSTD_LECHK(_x) KLSTD::klstd_chk_le((!(!(_x))), __FILE__, __LINE__)

/*! 
    Simple processing win32 errors. 
    Use this macro for calls that return negative HRESULT in case if
    error occured. 

    Example:

        KLSTD_HRCHK(m_pCommand.CreateInstance( __uuidof(ADODB::Command) ) );

*/
#define KLSTD_HRCHK(_x) KLSTD::klstd_chk_hr((_x), __FILE__, __LINE__)

/*! 
    Simple processing win32 errors. 
    Use this macro for calls that return non-zero win32 error code 
    in case if error occured (registry functions). 

    Example:

        KlRegKey key;
        KLSTD_RGCHK(key.Open(
                            HKEY_LOCAL_MACHINE,
                            wstrKey.c_str(),
                            KEY_READ));
*/
#define KLSTD_RGCHK(_x) KLSTD::klstd_chk_rg((_x), __FILE__, __LINE__)

#endif //_WIN32

#endif //__KLOS_WIN32_ERRORS_H__
