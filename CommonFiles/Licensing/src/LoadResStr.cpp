#include "../include/LoadResStr.h"

DWORD   loadResString 
    (HMODULE    hModule,
     UINT       stringId,
     WORD       langId,
     _TCHAR     *buf,
     DWORD      *bufSize
    )
{

    if (!bufSize) 
    {
        return (ERROR_INVALID_PARAMETER);
    }

    if (buf) 
    {
        *buf = 0;
    }

    HRSRC   _hRes;
    HGLOBAL _hGlobal;
    DWORD   _len;
    UINT    _blockNum;
    UINT    _blockOfs;
    wchar_t *_str;

    _blockNum = (stringId >> 4) + 1;
    _blockOfs = stringId & 0xf;

    SetLastError (0);
    
    _hRes = FindResourceEx (hModule, RT_STRING, MAKEINTRESOURCE (_blockNum), langId);

    if (!_hRes) 
    {
        return (GetLastError ());
    }

    SetLastError (0);

    _hGlobal = LoadResource (hModule, _hRes);

    if (!_hGlobal) 
    {
        return (GetLastError ());
    }
    
    SetLastError (0);

    _str = reinterpret_cast<wchar_t *> (LockResource (_hGlobal));

    if (!_str) 
    {
        return (GetLastError ());
    }

    for (UINT i = 0; i < _blockOfs; ++i)
    {
      _str += *_str + 1;
    }

    _len = *(reinterpret_cast<WORD *> (_str));

    if (!buf) 
    {
        *bufSize = _len + 1;
        return (ERROR_MORE_DATA);
    }

    if ((_len + 1) > *bufSize) 
    {
        *bufSize = _len + 1;
        *buf = 0;
        return (ERROR_MORE_DATA);
    }

#if !defined (UNICODE) && !defined (_UNICODE)
    WideCharToMultiByte (CP_ACP,
                         0,
                         _str + 1,
                         _len,
                         buf,
                         *bufSize,
                         0,
                         FALSE
                        );
#else
    wcsncpy (buf, _str + 1, _len);
#endif

    buf[_len] = 0;

    return (ERROR_SUCCESS);
    
}

//-----------------------------------------------------------------------------
