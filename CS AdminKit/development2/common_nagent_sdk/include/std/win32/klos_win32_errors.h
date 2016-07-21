#ifndef __KLOS_WIN32_ERRORS_H__
#define __KLOS_WIN32_ERRORS_H__

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

#define KLSTD_LECHK(_x) KLSTD::klstd_chk_le((!(!(_x))), __FILE__, __LINE__)
#define KLSTD_HRCHK(_x) KLSTD::klstd_chk_hr((_x), __FILE__, __LINE__)
#define KLSTD_RGCHK(_x) KLSTD::klstd_chk_rg((_x), __FILE__, __LINE__)

#endif //_WIN32

#endif //__KLOS_WIN32_ERRORS_H__
