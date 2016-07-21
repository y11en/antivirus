#ifndef __KL_MSI_WRAPPERS_H__
#define __KL_MSI_WRAPPERS_H__

#ifdef _WIN32

#ifdef _WIN32_MSI
    #define _WIN32_MSI   200
#endif

#include <std/base/klstd.h>
#include <std/win32/klos_win32v50.h>
#include <Msi.h>
#include <msiquery.h>
#include <common/kldelayload.h>
#include <std/win32/klos_win32_errors.h>


const wchar_t c_szwKLMSIWRAPPER_ModuleName[] = L"KLMSIWRAPPER";

inline UINT KLMSIWRAPPER_MsiEnumRelatedProducts(
	LPCTSTR  lpUpgradeCode,    // upgrade code of products to enumerate
	DWORD     dwReserved,       // reserved, must be 0
	DWORD     iProductIndex,    // 0-based index into registered products
	LPTSTR   lpProductBuf)    // buffer of char count: 39 (size of string GUID)
{
    UINT uResult = 0;
    __try
    {
        uResult = MsiEnumRelatedProducts(   lpUpgradeCode, 
                                            dwReserved, 
                                            iProductIndex, 
                                            lpProductBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiOpenProduct(
      LPCTSTR szProduct,   // product code
      MSIHANDLE *hProduct) // returned product handle, must be closed
{
    UINT uResult = 0;
    __try
    {
        uResult = MsiOpenProduct(szProduct, hProduct);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiGetProductProperty(
	MSIHANDLE   hProduct,       // product handle obtained from MsiOpenProduct
	LPCTSTR    szProperty,     // property name, case-sensitive
	LPTSTR     lpValueBuf,     // returned value, NULL if not desired
	DWORD      *pcchValueBuf)  // in/out buffer character count
{
    UINT uResult = 0;
    __try
    {
        uResult = MsiGetProductProperty(hProduct, szProperty, lpValueBuf, pcchValueBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiGetProductInfo(
  LPCTSTR szProduct,   // product code
  LPCTSTR szProperty,  // product property
  LPTSTR lpValueBuf,   // buffer to return property value
  DWORD *pcchValueBuf  // buffer character count
)
{
    UINT uResult = 0;
    __try
    {
        uResult = MsiGetProductInfo(szProduct, szProperty, lpValueBuf, pcchValueBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiEnumProducts(
  DWORD iProductIndex, // 0-based index into registered products
  LPTSTR lpProductBuf  // buffer to receive GUID
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiEnumProducts(iProductIndex, lpProductBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiEnumPatches(
  LPCTSTR szProduct,       // product code, string GUID
  DWORD iPatchIndex,       // 0-based index into patches
  LPTSTR lpPatchBuf,       // buffer for patch
  LPTSTR lpTransformsBuf,  // buffer for transforms
  DWORD *pcchTransformsBuf // transforms buffer character count
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiEnumPatches(szProduct, iPatchIndex, lpPatchBuf, lpTransformsBuf, pcchTransformsBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiGetPatchInfo(
  LPCTSTR szPatch,      // patch code
  LPCTSTR szAttribute,  // attribute name, case-sensitive
  LPTSTR lpValueBuf,    // returned value, NULL if not desired
  DWORD *pcchValueBuf   // buffer character count, 
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiGetPatchInfo(szPatch, szAttribute, lpValueBuf, pcchValueBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiOpenDatabase(
  LPCTSTR szDatabasePath,  // path to database
  LPCTSTR szPersist,       // output database or predefined persist mode
  MSIHANDLE *phDatabase    // location to return database handle
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiOpenDatabase(szDatabasePath, szPersist, phDatabase);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_OPEN_FAILED;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiDatabaseOpenView(
  MSIHANDLE hDatabase,   // database handle
  LPCTSTR szQuery,       // SQL query to be prepared
  MSIHANDLE *phView      // returned view if TRUE
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiDatabaseOpenView(hDatabase, szQuery, phView);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_INVALID_HANDLE;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiViewExecute(
  MSIHANDLE hView,    // view handle
  MSIHANDLE hRecord   // parameter record
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiViewExecute(hView, hRecord);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_FUNCTION_FAILED;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiViewFetch(
  MSIHANDLE hView,      // view handle
  MSIHANDLE *phRecord   // handle for fetched record
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiViewFetch(hView, phRecord);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_NO_MORE_ITEMS;
    };
    return uResult;
};

inline UINT KLMSIWRAPPER_MsiRecordGetString(
  MSIHANDLE hRecord,     // record handle
  unsigned int iField,   // field requested
  LPTSTR szValueBuf,     // buffer to hold string
  DWORD *pcchValueBuf    // pointer to size of buffer
)
{
	UINT uResult = 0;
    __try
    {
        uResult = MsiRecordGetString(hRecord, iField, szValueBuf, pcchValueBuf);
    }
    __except( KLDELAYLOAD_DL_CHECK())
    {
        KLSTD::Trace(1, c_szwKLMSIWRAPPER_ModuleName, L"MSI is not installed !!!\n");
        uResult = ERROR_INVALID_HANDLE;
    };
    return uResult;
};


#endif //_WIN32

#endif //__KL_MSI_WRAPPERS_H__
