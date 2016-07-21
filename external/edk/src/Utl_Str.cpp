// --UTL_STR.C------------------------------------------------------------------
//
// String conversion helper functions.
//
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
//
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "utl_str.chk"


//$--HrStrAToStrW---------------------------------------------------------------
//  Convert a byte string to a word string.  The resulting string is placed in 
//  a buffer allocated using MAPIAllocateBuffer.
// -----------------------------------------------------------------------------
HRESULT HrStrAToStrW(
    IN  LPCSTR          lpszSource,     // source string
    OUT LPWSTR *        lppwszDest)     // destination string
{
    HRESULT             hr              = NOERROR;
    LONG                cchDest         = 0;
    LPWSTR              lpwszDest       = NULL;

    DEBUGPUBLIC("HrStrAToStrW()");

    hr = CHK_HrStrAToStrW(lpszSource, lppwszDest);
    if (FAILED(hr))
        RETURN(hr);

    // Compute the number of word characters needed for the destination buffer.

    if (*lpszSource)
    {
        cchDest = MultiByteToWideChar(
            CP_ACP, 
            MB_PRECOMPOSED, 
            lpszSource, 
            strlen(lpszSource), 
            NULL, 
            0);
        if (cchDest == 0)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    // Allocate the destination string buffer.

    hr = MAPIAllocateBuffer((ULONG)((cchDest + 1) * sizeof(WCHAR)), (LPVOID*)&lpwszDest);
    if (FAILED(hr))
        goto cleanup;

    // Convert from byte string to word string.

    if (*lpszSource)
    {
        cchDest = MultiByteToWideChar(
            CP_ACP, 
            MB_PRECOMPOSED, 
            lpszSource, 
            strlen(lpszSource), 
            lpwszDest, 
            cchDest);
        if (cchDest == 0)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }
    lpwszDest[cchDest] = 0;

    // Store in output parameter.

    *lppwszDest = lpwszDest;

cleanup:
    if (FAILED(hr))
    {
        MAPIFREEBUFFER(lpwszDest);
    }

    RETURN(hr);
}


//$--HrStrWToStrA---------------------------------------------------------------
//  Convert a word string to a byte string.  The resulting string is placed in 
//  a buffer allocated using MAPIAllocateBuffer.
// -----------------------------------------------------------------------------
HRESULT HrStrWToStrA(
    IN  LPCWSTR         lpwszSource,    // source string
    OUT LPSTR *         lppszDest)      // destination string
{
    HRESULT             hr              = NOERROR;
    LONG                cchDest         = 0;
    LPSTR               lpszDest        = NULL;

    DEBUGPUBLIC("HrStrWToStrA()");

    hr = CHK_HrStrWToStrA(lpwszSource, lppszDest);
    if (FAILED(hr))
        RETURN(hr);

    // Compute the number of byte characters needed for the destination buffer.

    if (*lpwszSource)
    {
        cchDest = WideCharToMultiByte(
            CP_ACP, 
            WC_COMPOSITECHECK | WC_SEPCHARS, 
            lpwszSource, 
            wcslen(lpwszSource), 
            NULL, 
            0, 
            NULL, 
            NULL);
        if (cchDest == 0)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    // Allocate the destination string buffer.

    hr = MAPIAllocateBuffer((ULONG)(cchDest + 1), (LPVOID*)&lpszDest);
    if (FAILED(hr))
        goto cleanup;

    // Convert from word string to byte string.

    if (*lpwszSource)
    {
        cchDest = WideCharToMultiByte(
            CP_ACP, 
            WC_COMPOSITECHECK | WC_SEPCHARS, 
            lpwszSource, 
            wcslen(lpwszSource), 
            lpszDest, 
            cchDest, 
            NULL, 
            NULL);
        if (cchDest == 0)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }
    lpszDest[cchDest] = 0;

    // Store in output parameter.

    *lppszDest = lpszDest;

cleanup:
    if (FAILED(hr))
    {
        MAPIFREEBUFFER(lpszDest);
    }

    RETURN(hr);
}


//$--HrStrAToStrA---------------------------------------------------------------
//  Create an allocated copy of a byte string using MAPIAllocateBuffer.
//  This is useful for creating macros involving CHAR strings.
// -----------------------------------------------------------------------------
HRESULT HrStrAToStrA(
    IN  LPCSTR          lpszSource,     // source string
    OUT LPSTR *         lppszDest)      // destination string
{
    HRESULT             hr              = NOERROR;
    LPSTR               lpszDest        = NULL;

    DEBUGPUBLIC("HrStrAToStrA()");

    hr = CHK_HrStrAToStrA(lpszSource, lppszDest);
    if (FAILED(hr))
        RETURN(hr);

    // Allocate the destination buffer and copy the string.

    hr = MAPIAllocateBuffer((ULONG)(strlen(lpszSource) + 1), (LPVOID*)&lpszDest);
    if (FAILED(hr))
        goto cleanup;

    strcpy(lpszDest, lpszSource);

    // Store in output parameter.

    *lppszDest = lpszDest;

cleanup:
    if (FAILED(hr))
    {
        MAPIFREEBUFFER(lpszDest);
    }

    RETURN(hr);
}


//$--HrStrWToStrW---------------------------------------------------------------
//  Create an allocated copy of a word string using MAPIAllocateBuffer.
//  This is useful for creating macros involving CHAR strings.
// -----------------------------------------------------------------------------
HRESULT HrStrWToStrW(
    IN  LPCWSTR         lpwszSource,    // source string
    OUT LPWSTR *        lppwszDest)     // destination string
{
    HRESULT             hr              = NOERROR;
    LPWSTR              lpwszDest       = NULL;

    DEBUGPUBLIC("HrStrWToStrW()");

    hr = CHK_HrStrWToStrW(lpwszSource, lppwszDest);
    if (FAILED(hr))
        RETURN(hr);

    // Allocate the destination buffer and copy the string.

    hr = MAPIAllocateBuffer(
        (ULONG)((wcslen(lpwszSource) + 1) * sizeof(WCHAR)), (LPVOID*)&lpwszDest);
    if (FAILED(hr))
        goto cleanup;

    wcscpy(lpwszDest, lpwszSource);

    // Store in output parameter.

    *lppwszDest = lpwszDest;

cleanup:
    if (FAILED(hr))
    {
        MAPIFREEBUFFER(lpwszDest);
    }

    RETURN(hr);
}
