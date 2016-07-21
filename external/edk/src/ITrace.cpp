// --itrace.c-------------------------------------------------------------------
// 
//  Module containing MAPI utility functions for message traces.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "itrace.chk"

//$--HrTraceGetEntryListSize--------------------------------------------------
//  Get the size of the hop trace information in bytes.
// -----------------------------------------------------------------------------
HRESULT HrTraceGetEntryListSize(      // RETURNS: return code
    IN LPTRACEINFO lpTraceInfo,         // Pointer to hop trace address variable
    OUT ULONG      *lpcbTraceInfo)      // Count of bytes in hop trace list
{
    HRESULT hr         = NOERROR;
    LPBYTE  lpbFirst   = NULL;
    LPBYTE  lpbLast    = NULL;
    ULONG   cBytes     = 0;
    ULONG   cbExpected = 0;
    ULONG   cEntries   = 0;

    DEBUGPUBLIC("HrTraceGetEntryListSize()");

    hr = CHK_HrTraceGetEntryListSize(
        lpTraceInfo,
        lpcbTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbTraceInfo = 0;

    lpbFirst = (LPBYTE)lpTraceInfo;

    cEntries = lpTraceInfo->cEntries;

    lpbLast  = (LPBYTE)&(lpTraceInfo->rgtraceentry[cEntries]) - 1;

    cBytes = ((lpbFirst <= lpbLast) ? (lpbLast-lpbFirst) : (lpbFirst-lpbLast));

    cBytes++;

    cbExpected = CbTRACEINFO(lpTraceInfo);

    if(cBytes != cbExpected)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    *lpcbTraceInfo = cBytes;

cleanup:

    RETURN(hr);
}

//$--HrTraceGetEntryList-----------------------------------------------------
//  Get the hop trace information for a given message.
// -----------------------------------------------------------------------------
HRESULT HrTraceGetEntryList(         // RETURNS: return code
    IN LPMESSAGE   lpMessage,           // Pointer to message.
    OUT LPTRACEINFO *lppTraceInfo)      // Pointer to hop trace address variable
{
    HRESULT hr         = NOERROR;
    ULONG   cBytes     = 0;
    ULONG   cbExpected = 0;

    DEBUGPUBLIC("HrTraceGetEntryList()");

    hr = CHK_HrTraceGetEntryList(
        lpMessage,
        lppTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    *lppTraceInfo = NULL;

    hr = HrMAPIGetPropBinary(
        (LPMAPIPROP)lpMessage,
        PR_TRACE_INFO,
        &cBytes,
        (LPVOID *)lppTraceInfo);

    if(FAILED(hr))
    {
        goto cleanup;
    }

    ASSERTERROR((*lppTraceInfo)->cEntries != 0, "ZERO cEntries variable");

    cbExpected = CbTRACEINFO((*lppTraceInfo));

    if(cBytes != cbExpected)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrTraceSetEntryList-----------------------------------------------------
//  Set the hop trace information for a given message.
// -----------------------------------------------------------------------------
HRESULT HrTraceSetEntryList(         // RETURNS: return code
    IN LPMESSAGE   lpMessage,           // Pointer to message.
    IN LPTRACEINFO lpTraceInfo)         // Pointer to hop trace address variable
{
    HRESULT hr         = NOERROR;
    ULONG   cBytes     = 0;
    ULONG   cbExpected = 0;

    DEBUGPUBLIC("HrTraceSetEntryList()");

    hr = CHK_HrTraceSetEntryList(
        lpMessage,
        lpTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    hr = HrTraceGetEntryListSize(lpTraceInfo, &cBytes);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cbExpected = CbTRACEINFO(lpTraceInfo);

    if(cBytes != cbExpected)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = HrMAPISetPropBinary(
        (LPMAPIPROP)lpMessage,
        PR_TRACE_INFO,
        cBytes,
        (LPVOID *)lpTraceInfo);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrTraceCopyEntry--------------------------------------------------------
//  Copy trace entry information to a trace entry structure.
// -----------------------------------------------------------------------------
HRESULT HrTraceCopyEntry(            // RETURNS: return code
    IN LONG     lAction,                // The routing action the tracing site
                                        // took.
    IN FILETIME ftArrivalTime,          // The time at which the communique
                                        // entered the tracing site.
    IN FILETIME ftDeferredTime,         // The time are which the tracing site
                                        // released the message.
    IN LPSTR    lpszADMDName,           // ADMD Name
    IN LPSTR    lpszCountryName,        // Country Name
    IN LPSTR    lpszPRMDId,             // PRMD Identifier
    IN LPSTR    lpszAttADMDName,        // Attempted ADMD Name
    IN LPSTR    lpszAttCountryName,     // Attempted Country Name
    IN LPSTR    lpszAttPRMDId,          // Attempted PRMD Identifier
    OUT LPTRACEENTRY lpTraceEntry)      // Pointer to trace entry address
                                        // variable.
{
    HRESULT hr = NOERROR;

    DEBUGPRIVATE("HrTraceCopyEntry()");

    hr = CHK_HrTraceCopyEntry(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lpTraceEntry);

    if(FAILED(hr))
        RETURN(hr);

    lpTraceEntry->lAction = lAction;

    lpTraceEntry->ftArrivalTime = ftArrivalTime;

    lpTraceEntry->ftDeferredTime = ftDeferredTime;

    if((lpszADMDName != NULL) && (strlen(lpszADMDName) > 0))
    {
        strncpy(lpTraceEntry->rgchADMDName, lpszADMDName, MAX_ADMD_NAME_SIZ);
        lpTraceEntry->rgchADMDName[MAX_ADMD_NAME_SIZ-1] = '\0';
    }

    if((lpszCountryName != NULL) && (strlen(lpszCountryName) > 0))
    {
        strncpy(lpTraceEntry->rgchCountryName, lpszCountryName, MAX_COUNTRY_NAME_SIZ);
        lpTraceEntry->rgchCountryName[MAX_COUNTRY_NAME_SIZ-1] = '\0';
    }

    if((lpszPRMDId != NULL) && (strlen(lpszPRMDId) > 0))
    {
        strncpy(lpTraceEntry->rgchPRMDId, lpszPRMDId, MAX_PRMD_NAME_SIZ);
        lpTraceEntry->rgchPRMDId[MAX_PRMD_NAME_SIZ-1] = '\0';
    }

    if((lpszAttADMDName != NULL) && (strlen(lpszAttADMDName) > 0))
    {
        strncpy(lpTraceEntry->rgchAttADMDName, lpszAttADMDName, MAX_ADMD_NAME_SIZ);
        lpTraceEntry->rgchAttADMDName[MAX_ADMD_NAME_SIZ-1] = '\0';
    }

    if((lpszAttCountryName != NULL) && (strlen(lpszAttCountryName) > 0))
    {
        strncpy(lpTraceEntry->rgchAttCountryName, lpszAttCountryName, MAX_COUNTRY_NAME_SIZ);
        lpTraceEntry->rgchAttCountryName[MAX_COUNTRY_NAME_SIZ-1] = '\0';
    }

    if((lpszAttPRMDId != NULL) && (strlen(lpszAttPRMDId) > 0))
    {
        strncpy(lpTraceEntry->rgchAttPRMDId, lpszAttPRMDId, MAX_PRMD_NAME_SIZ);
        lpTraceEntry->rgchAttPRMDId[MAX_PRMD_NAME_SIZ] = '\0';
    }

    RETURN(hr);
}

//$--HrTraceCreateEntryList--------------------------------------------------
//  Create a hop trace information list.
// -----------------------------------------------------------------------------
HRESULT HrTraceCreateEntryList(      // RETURNS: return code
    IN LONG     lAction,                // The routing action the tracing site
                                        // took.
    IN FILETIME ftArrivalTime,          // The time at which the communique
                                        // entered the tracing site.
    IN FILETIME ftDeferredTime,         // The time are which the tracing site
                                        // released the message.
    IN LPSTR    lpszADMDName,           // ADMD Name
    IN LPSTR    lpszCountryName,        // Country Name
    IN LPSTR    lpszPRMDId,             // PRMD Identifier
    IN LPSTR    lpszAttADMDName,        // Attempted ADMD Name
    IN LPSTR    lpszAttCountryName,     // Attempted Country Name
    IN LPSTR    lpszAttPRMDId,          // Attempted PRMD Identifier
    OUT LPTRACEINFO *lppTraceInfo)      // Pointer to hop trace address variable
{
    HRESULT     hr          = NOERROR;
    ULONG       cBytes      = 0;
    LPTRACEINFO lpTraceInfo = NULL;
    SCODE       sc          = 0;

    DEBUGPUBLIC("HrTraceCreateEntryList()");

    hr = CHK_HrTraceCreateEntryList(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lppTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    // Allocate a trace entry list

    cBytes = CbNewTRACEINFO(1);

    sc = MAPIAllocateBuffer(cBytes, (void **)lppTraceInfo);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;                                
    }                                                   

    // Initialize trace entry list
    ZeroMemory(*lppTraceInfo, cBytes);

    lpTraceInfo = *lppTraceInfo;

    lpTraceInfo->cEntries = 1;

    hr = HrTraceCopyEntry(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        &(lpTraceInfo->rgtraceentry[0]));

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrTraceAppendEntryList--------------------------------------------------
//  Append to an existing hop trace information list.
// -----------------------------------------------------------------------------
HRESULT HrTraceAppendEntryList(      // RETURNS: return code
    IN LONG     lAction,                // The routing action the tracing site
                                        // took.
    IN FILETIME ftArrivalTime,          // The time at which the communique
                                        // entered the tracing site.
    IN FILETIME ftDeferredTime,         // The time are which the tracing site
                                        // released the message.
    IN LPSTR    lpszADMDName,           // ADMD Name
    IN LPSTR    lpszCountryName,        // Country Name
    IN LPSTR    lpszPRMDId,             // PRMD Identifier
    IN LPSTR    lpszAttADMDName,        // Attempted ADMD Name
    IN LPSTR    lpszAttCountryName,     // Attempted Country Name
    IN LPSTR    lpszAttPRMDId,          // Attempted PRMD Identifier
    IN OUT LPTRACEINFO *lppTraceInfo)   // Pointer to hop trace address variable
{
    HRESULT      hr             = NOERROR;
    ULONG        i              = 0;
    ULONG        cBytes         = 0;
    ULONG        cEntries       = 0;
    LPTRACEINFO  lpTraceInfo    = NULL;
    LPTRACEENTRY lpTraceEntry   = NULL;
    LPTRACEINFO  lpNewTraceInfo = NULL;
    SCODE        sc             = 0;

    DEBUGPUBLIC("HrTraceAppendEntryList()");

    hr = CHK_HrTraceAppendEntryList(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lppTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    // Allocate a trace entry list

    lpTraceInfo = *lppTraceInfo;

    cEntries = lpTraceInfo->cEntries + 1;

    cBytes = CbNewTRACEINFO(cEntries);

    sc = MAPIAllocateBuffer(cBytes, (void **)&lpNewTraceInfo);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;                                
    }                                                   

    // Initialize trace entry list
    ZeroMemory(lpNewTraceInfo, cBytes);

    lpNewTraceInfo->cEntries = cEntries;

    cEntries--;

    for(i = 0; i < cEntries; i++)
    {
        lpTraceEntry = &(lpTraceInfo->rgtraceentry[i]);

        hr = HrTraceCopyEntry(
            lpTraceEntry->lAction,
            lpTraceEntry->ftArrivalTime,
            lpTraceEntry->ftDeferredTime,
            lpTraceEntry->rgchADMDName,
            lpTraceEntry->rgchCountryName,
            lpTraceEntry->rgchPRMDId,
            lpTraceEntry->rgchAttADMDName,
            lpTraceEntry->rgchAttCountryName,
            lpTraceEntry->rgchAttPRMDId,
            &(lpNewTraceInfo->rgtraceentry[i]));

        if(FAILED(hr))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    hr = HrTraceCopyEntry(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        &(lpNewTraceInfo->rgtraceentry[cEntries]));

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    MAPIFREEBUFFER(lpTraceInfo);

    *lppTraceInfo = lpNewTraceInfo;

cleanup:

    if(FAILED(hr))
    {
        MAPIFREEBUFFER(lpNewTraceInfo);
    }

    RETURN(hr);
}

//$--HrTraceGotoEntry--------------------------------------------------------
//  Goto the specified TRACEENTRY in a TRACEINFO
// -----------------------------------------------------------------------------
HRESULT HrTraceGotoEntry(            // RETURNS: return code
    IN LPTRACEINFO lpTraceInfo,         // pointer to TRACEINFO
    IN ULONG ulIndex,                   // index of TRACEENTRY in TRACEINFO
    OUT LPTRACEENTRY *lppTraceEntry)    // pointer to TRACEENTRY
{
    HRESULT hr = NOERROR;

    DEBUGPUBLIC("HrTraceGotoEntry()");

    hr = CHK_HrTraceGotoEntry(
        lpTraceInfo,
        ulIndex,
        lppTraceEntry);

    if(FAILED(hr))
        RETURN(hr);

    *lppTraceEntry = NULL;

    if(ulIndex < lpTraceInfo->cEntries)
    {
        *lppTraceEntry = &(lpTraceInfo->rgtraceentry[ulIndex]);
    }
    else
    {
        hr = HR_LOG(EDK_E_NOT_FOUND);
    }

    RETURN(hr);
}

//$--HrTraceGotoFirstEntry---------------------------------------------------
//  Goto the first TRACEENTRY in a TRACEINFO
// -----------------------------------------------------------------------------
HRESULT HrTraceGotoFirstEntry(       // RETURNS: return code
    IN LPTRACEINFO lpTraceInfo,         // pointer to TRACEINFO
    OUT ULONG *lpulIndex,               // index of TRACEENTRY in TRACEINFO
    OUT LPTRACEENTRY *lppTraceEntry)    // pointer to TRACEENTRY
{
    HRESULT hr = NOERROR;

    DEBUGPUBLIC("HrTraceGotoFirstEntry()");

    hr = CHK_HrTraceGotoFirstEntry(
        lpTraceInfo,
        lpulIndex,
        lppTraceEntry);

    if(FAILED(hr))
        RETURN(hr);

    *lpulIndex = 0;

    hr = HrTraceGotoEntry(
        lpTraceInfo,
        0,
        lppTraceEntry);

    RETURN(hr);
}

//$--HrTraceGotoNextEntry----------------------------------------------------
//  Goto the next TRACEENTRY in a TRACEINFO
// -----------------------------------------------------------------------------
HRESULT HrTraceGotoNextEntry(        // RETURNS: return code
    IN LPTRACEINFO lpTraceInfo,         // pointer to TRACEINFO
    IN OUT ULONG *lpulIndex,            // index of TRACEENTRY in TRACEINFO
    OUT LPTRACEENTRY *lppTraceEntry)    // pointer to TRACEENTRY
{
    HRESULT hr      = NOERROR;
    ULONG   ulIndex = 0;

    DEBUGPUBLIC("HrTraceGotoNextEntry()");

    hr = CHK_HrTraceGotoNextEntry(
        lpTraceInfo,
        lpulIndex,
        lppTraceEntry);

    if(FAILED(hr))
        RETURN(hr);

    ulIndex = *lpulIndex;

    ulIndex++;

    *lpulIndex = ulIndex;

    hr = HrTraceGotoEntry(
        lpTraceInfo,
        ulIndex,
        lppTraceEntry);

    RETURN(hr);
}

//$--HrTraceOpenEntry--------------------------------------------------------
//  Open a TRACEENTRY.
// -----------------------------------------------------------------------------
HRESULT HrTraceOpenEntry(            // RETURNS: return code
    IN LPTRACEENTRY lpTraceEntry,       // pointer to TRACEENTRY
    OUT LONG     *plAction,             // The routing action the tracing site
                                        // took.
    OUT FILETIME *pftArrivalTime,       // The time at which the communique
                                        // entered the tracing site.
    OUT FILETIME *pftDeferredTime,      // The time are which the tracing site
                                        // released the message.
    OUT LPSTR    *lppszADMDName,        // ADMD Name
    OUT LPSTR    *lppszCountryName,     // Country Name
    OUT LPSTR    *lppszPRMDId,          // PRMD Identifier
    OUT LPSTR    *lppszAttADMDName,     // Attempted ADMD Name
    OUT LPSTR    *lppszAttCountryName,  // Attempted Country Name
    OUT LPSTR    *lppszAttPRMDId)       // Attempted PRMD Identifier
{
    HRESULT hr = NOERROR;

    DEBUGPUBLIC("HrTraceOpenEntry()");

    hr = CHK_HrTraceOpenEntry(
        lpTraceEntry,
        plAction,
        pftArrivalTime,
        pftDeferredTime,
        lppszADMDName,
        lppszCountryName,
        lppszPRMDId,
        lppszAttADMDName,
        lppszAttCountryName,
        lppszAttPRMDId);

    if(FAILED(hr))
        RETURN(hr);

    *plAction               = lpTraceEntry->lAction;
    *pftArrivalTime         = lpTraceEntry->ftArrivalTime;
    *pftDeferredTime        = lpTraceEntry->ftDeferredTime;
    *lppszADMDName          = lpTraceEntry->rgchADMDName;
    *lppszCountryName       = lpTraceEntry->rgchCountryName;
    *lppszPRMDId            = lpTraceEntry->rgchPRMDId;
    *lppszAttADMDName       = lpTraceEntry->rgchAttADMDName;
    *lppszAttCountryName    = lpTraceEntry->rgchAttCountryName;
    *lppszAttPRMDId         = lpTraceEntry->rgchAttPRMDId;

    RETURN(hr);
}

//$--HrTraceSetInfo----------------------------------------------------------
//  Set the trace-info on a message.
// -----------------------------------------------------------------------------
HRESULT HrTraceSetInfo(              // RETURNS: return code
    IN LONG lAction,                    // pointer to action
    IN FILETIME *lpftArrivalTime,       // pointer to arrival time
    IN FILETIME *lpftDeferredTime,      // pointer to deferred time
    IN LPSTR lpszCountry,               // pointer to country
    IN LPSTR lpszADMD,                  // pointer to ADMD
    IN LPSTR lpszPRMD,                  // pointer to PRMD
    IN OUT LPMESSAGE lpMessage)         // pointer to message
{
    HRESULT     hr          = NOERROR;
    LPTRACEINFO lpTraceInfo = NULL;

    DEBUGPUBLIC("HrTraceSetInfo()");

    hr = CHK_HrTraceSetInfo(
        lAction,
        lpftArrivalTime,
        lpftDeferredTime,
        lpszCountry,
        lpszADMD,
        lpszPRMD,
        lpMessage);

    if(FAILED(hr))
        RETURN(hr);

    hr = HrTraceCreateEntryList(
        lAction,
        *lpftArrivalTime,
        *lpftDeferredTime,
        lpszADMD,
        lpszCountry,
        lpszPRMD,
        NULL,
        NULL,
        NULL,
        &lpTraceInfo);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = HrTraceSetEntryList(
        lpMessage,
        lpTraceInfo);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    MAPIFREEBUFFER(lpTraceInfo);

    RETURN(hr);
}

//$--HrTraceUpdateInfo-------------------------------------------------------
//  Update the trace-info on a message.
// -----------------------------------------------------------------------------
HRESULT HrTraceUpdateInfo(           // RETURNS: return code
    IN LONG lAction,                    // pointer to action
    IN FILETIME *lpftArrivalTime,       // pointer to arrival time
    IN FILETIME *lpftDeferredTime,      // pointer to deferred time
    IN LPSTR lpszCountry,               // pointer to country
    IN LPSTR lpszADMD,                  // pointer to ADMD
    IN LPSTR lpszPRMD,                  // pointer to PRMD
    IN OUT LPMESSAGE lpMessage)         // pointer to message
{
    HRESULT      hr                 = NOERROR;
    LPTRACEINFO  lpTraceInfo        = NULL;
    LPTRACEENTRY lpTraceEntry       = NULL;
    ULONG        index              = 0;
    LONG         lCurrAction        = 0;
    FILETIME     ftCurrArrivalTime  = {0};
    FILETIME     ftCurrDeferredTime = {0};
    LPSTR       lpszCurrCountry    = NULL;
    LPSTR       lpszCurrADMD       = NULL;
    LPSTR       lpszCurrPRMD       = NULL;
    LPSTR       lpszCurrAttCountry = NULL;
    LPSTR       lpszCurrAttADMD    = NULL;
    LPSTR       lpszCurrAttPRMD    = NULL;

    DEBUGPUBLIC("HrTraceUpdateInfo()");

    hr = CHK_HrTraceUpdateInfo(
        lAction,
        lpftArrivalTime,
        lpftDeferredTime,
        lpszCountry,
        lpszADMD,
        lpszPRMD,
        lpMessage);

    if(FAILED(hr))
        RETURN(hr);

    hr = HrTraceGetEntryList(
        lpMessage,
        &lpTraceInfo);

    if(FAILED(hr))
    {
        if(hr != EDK_E_NOT_FOUND)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    if(hr == EDK_E_NOT_FOUND)
    {
        hr = HrTraceCreateEntryList(
            lAction,
            *lpftArrivalTime,
            *lpftDeferredTime,
            lpszADMD,
            lpszCountry,
            lpszPRMD,
            NULL,
            NULL,
            NULL,
            &lpTraceInfo);

        if(FAILED(hr))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }
    else
    {
        hr = HrTraceGotoFirstEntry(lpTraceInfo, &index, &lpTraceEntry);

        while(SUCCEEDED(hr))
        {

            hr = HrTraceOpenEntry(
                lpTraceEntry,
                &lCurrAction,
                &ftCurrArrivalTime,
                &ftCurrDeferredTime,
                &lpszCurrADMD,
                &lpszCurrCountry,
                &lpszCurrPRMD,
                &lpszCurrAttADMD,
                &lpszCurrAttCountry,
                &lpszCurrAttPRMD);

            if(FAILED(hr))
            {
                hr = HR_LOG(E_FAIL);
                goto cleanup;
            }

            if( (lstrcmpi(lpszADMD, lpszCurrADMD) == 0) &&
                (lstrcmpi(lpszPRMD, lpszCurrPRMD) == 0) &&
                (lstrcmpi(lpszCountry, lpszCurrCountry) == 0))
            {
                hr = HR_LOG(EDK_E_ALREADY_EXISTS);
                goto cleanup;

            }

            hr = HrTraceGotoNextEntry(
                lpTraceInfo,
                &index,
                &lpTraceEntry);

            if((FAILED(hr)) && (hr != EDK_E_NOT_FOUND))
            {
                hr = HR_LOG(E_FAIL);
                goto cleanup;
            }
        }

        hr = HrTraceAppendEntryList(
            lAction,
            *lpftArrivalTime,
            *lpftDeferredTime,
            lpszADMD,
            lpszCountry,
            lpszPRMD,
            NULL,
            NULL,
            NULL,
            &lpTraceInfo);

        if(FAILED(hr))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    hr = HrTraceSetEntryList(
        lpMessage,
        lpTraceInfo);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    MAPIFREEBUFFER(lpTraceInfo);

    RETURN(hr);
}


//------------------------------------------------------------------------------
//
// INTERNAL TRACE INFORMATION FUNCTIONS
//
//------------------------------------------------------------------------------

//$--HrInternalTraceCopyEntry---------------------------------------------------
//  Copy internal trace entry information to an internal trace entry structure.
// -----------------------------------------------------------------------------
HRESULT HrInternalTraceCopyEntry(       // RETURNS: return code
    IN LONG     lAction,                // The routing action the tracing site
                                        // took.
    IN FILETIME ftArrivalTime,          // The time at which the communique
                                        // entered the tracing site.
    IN FILETIME ftDeferredTime,         // The time are which the tracing site
                                        // released the message.
    IN LPSTR    lpszADMDName,           // ADMD Name
    IN LPSTR    lpszCountryName,        // Country Name
    IN LPSTR    lpszPRMDId,             // PRMD Identifier
    IN LPSTR    lpszMTAName,            // MTA Name
    IN LPSTR    lpszAttADMDName,        // Attempted ADMD Name
    IN LPSTR    lpszAttCountryName,     // Attempted Country Name
    IN LPSTR    lpszAttPRMDId,          // Attempted PRMD Identifier
    IN LPSTR    lpszAttMTAName,         // Attempted MTA Name
    OUT PINTTRACEENTRY lpTraceEntry)    // Pointer to trace entry address
                                        // variable.
{
    HRESULT hr = NOERROR;

    DEBUGPRIVATE("HrInternalTraceCopyEntry()");

    hr = CHK_HrInternalTraceCopyEntry(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszMTAName,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lpszAttMTAName,
        lpTraceEntry);

    if(FAILED(hr))
        RETURN(hr);

    lpTraceEntry->lAction = lAction;

    lpTraceEntry->ftArrivalTime = ftArrivalTime;

    lpTraceEntry->ftDeferredTime = ftDeferredTime;

    if((lpszADMDName != NULL) && (strlen(lpszADMDName) > 0))
    {
        strncpy(lpTraceEntry->rgchADMDName, lpszADMDName, MAX_ADMD_NAME_SIZ);
        lpTraceEntry->rgchADMDName[MAX_ADMD_NAME_SIZ-1] = '\0';
    }

    if((lpszCountryName != NULL) && (strlen(lpszCountryName) > 0))
    {
        strncpy(lpTraceEntry->rgchCountryName, lpszCountryName, MAX_COUNTRY_NAME_SIZ);
        lpTraceEntry->rgchCountryName[MAX_COUNTRY_NAME_SIZ-1] = '\0';
    }

    if((lpszPRMDId != NULL) && (strlen(lpszPRMDId) > 0))
    {
        strncpy(lpTraceEntry->rgchPRMDId, lpszPRMDId, MAX_PRMD_NAME_SIZ);
        lpTraceEntry->rgchPRMDId[MAX_PRMD_NAME_SIZ-1] = '\0';
    }

    if((lpszMTAName != NULL) && (strlen(lpszMTAName) > 0))
    {
        strncpy(lpTraceEntry->rgchMTAName, lpszMTAName, MAX_MTA_NAME_SIZ);
        lpTraceEntry->rgchMTAName[MAX_MTA_NAME_SIZ-1] = '\0';
    }

    if((lpszAttADMDName != NULL) && (strlen(lpszAttADMDName) > 0))
    {
        strncpy(lpTraceEntry->rgchAttADMDName, lpszAttADMDName, MAX_ADMD_NAME_SIZ);
        lpTraceEntry->rgchAttADMDName[MAX_ADMD_NAME_SIZ-1] = '\0';
    }

    if((lpszAttCountryName != NULL) && (strlen(lpszAttCountryName) > 0))
    {
        strncpy(lpTraceEntry->rgchAttCountryName, lpszAttCountryName, MAX_COUNTRY_NAME_SIZ);
        lpTraceEntry->rgchAttCountryName[MAX_COUNTRY_NAME_SIZ-1] = '\0';
    }

    if((lpszAttPRMDId != NULL) && (strlen(lpszAttPRMDId) > 0))
    {
        strncpy(lpTraceEntry->rgchAttPRMDId, lpszAttPRMDId, MAX_PRMD_NAME_SIZ);
        lpTraceEntry->rgchAttPRMDId[MAX_PRMD_NAME_SIZ] = '\0';
    }

    if((lpszAttMTAName != NULL) && (strlen(lpszAttMTAName) > 0))
    {
        strncpy(lpTraceEntry->rgchAttMTAName, lpszAttMTAName, MAX_MTA_NAME_SIZ);
        lpTraceEntry->rgchAttMTAName[MAX_MTA_NAME_SIZ-1] = '\0';
    }

    RETURN(hr);
}

//$--HrInternalTraceCreateEntryList---------------------------------------------
//  Create a internal hop trace information list.
// -----------------------------------------------------------------------------
HRESULT HrInternalTraceCreateEntryList( // RETURNS: return code
    IN LONG     lAction,                // The routing action the tracing site
                                        // took.
    IN FILETIME ftArrivalTime,          // The time at which the communique
                                        // entered the tracing site.
    IN FILETIME ftDeferredTime,         // The time are which the tracing site
                                        // released the message.
    IN LPSTR    lpszADMDName,           // ADMD Name
    IN LPSTR    lpszCountryName,        // Country Name
    IN LPSTR    lpszPRMDId,             // PRMD Identifier
    IN LPSTR    lpszMTAName,            // MTA Name
    IN LPSTR    lpszAttADMDName,        // Attempted ADMD Name
    IN LPSTR    lpszAttCountryName,     // Attempted Country Name
    IN LPSTR    lpszAttPRMDId,          // Attempted PRMD Identifier
    IN LPSTR    lpszAttMTAName,         // Attempted MTA Name
    OUT PINTTRACEINFO *lppTraceInfo)    // Pointer to hop trace address variable
{
    HRESULT       hr          = NOERROR;
    ULONG         cBytes      = 0;
    PINTTRACEINFO lpTraceInfo = NULL;
    SCODE         sc          = 0;

    DEBUGPUBLIC("HrInternalTraceCreateEntryList()");

    hr = CHK_HrInternalTraceCreateEntryList(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszMTAName,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lpszAttMTAName,
        lppTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    // Allocate a trace entry list

    cBytes = CbNewINTTRACEINFO(1);

    sc = MAPIAllocateBuffer(cBytes, (void **)lppTraceInfo);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;                                
    }                                                   

    // Initialize trace entry list
    ZeroMemory(*lppTraceInfo, cBytes);

    lpTraceInfo = *lppTraceInfo;

    lpTraceInfo->cEntries = 1;

    hr = HrInternalTraceCopyEntry(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszMTAName,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lpszAttMTAName,
        &(lpTraceInfo->rgIntTraceEntry[0]));

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrInternalTraceAppendEntryList---------------------------------------------
//  Append to an existing internal hop trace information list.
// -----------------------------------------------------------------------------
HRESULT HrInternalTraceAppendEntryList( // RETURNS: return code
    IN LONG     lAction,                // The routing action the tracing site
                                        // took.
    IN FILETIME ftArrivalTime,          // The time at which the communique
                                        // entered the tracing site.
    IN FILETIME ftDeferredTime,         // The time are which the tracing site
                                        // released the message.
    IN LPSTR    lpszADMDName,           // ADMD Name
    IN LPSTR    lpszCountryName,        // Country Name
    IN LPSTR    lpszPRMDId,             // PRMD Identifier
    IN LPSTR    lpszMTAName,            // MTA Name
    IN LPSTR    lpszAttADMDName,        // Attempted ADMD Name
    IN LPSTR    lpszAttCountryName,     // Attempted Country Name
    IN LPSTR    lpszAttPRMDId,          // Attempted PRMD Identifier
    IN LPSTR    lpszAttMTAName,         // Attempted MTA Name
    IN OUT PINTTRACEINFO *lppTraceInfo) // Pointer to hop trace address variable
{
    HRESULT        hr             = NOERROR;
    ULONG          i              = 0;
    ULONG          cBytes         = 0;
    ULONG          cEntries       = 0;
    PINTTRACEINFO  lpTraceInfo    = NULL;
    PINTTRACEENTRY lpTraceEntry   = NULL;
    PINTTRACEINFO  lpNewTraceInfo = NULL;
    SCODE          sc             = 0;

    DEBUGPUBLIC("HrInternalTraceAppendEntryList()");

    hr = CHK_HrInternalTraceAppendEntryList(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszMTAName,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lpszAttMTAName,
        lppTraceInfo);

    if(FAILED(hr))
        RETURN(hr);

    // Allocate a trace entry list

    lpTraceInfo = *lppTraceInfo;

    cEntries = lpTraceInfo->cEntries + 1;

    cBytes = CbNewINTTRACEINFO(cEntries);

    sc = MAPIAllocateBuffer(cBytes, (void **)&lpNewTraceInfo);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;                                
    }                                                   

    // Initialize trace entry list
    ZeroMemory(lpNewTraceInfo, cBytes);

    lpNewTraceInfo->cEntries = cEntries;

    cEntries--;

    for(i = 0; i < cEntries; i++)
    {
        lpTraceEntry = &(lpTraceInfo->rgIntTraceEntry[i]);

        hr = HrInternalTraceCopyEntry(
            lpTraceEntry->lAction,
            lpTraceEntry->ftArrivalTime,
            lpTraceEntry->ftDeferredTime,
            lpTraceEntry->rgchADMDName,
            lpTraceEntry->rgchCountryName,
            lpTraceEntry->rgchPRMDId,
            lpTraceEntry->rgchMTAName,
            lpTraceEntry->rgchAttADMDName,
            lpTraceEntry->rgchAttCountryName,
            lpTraceEntry->rgchAttPRMDId,
            lpTraceEntry->rgchAttMTAName,
            &(lpNewTraceInfo->rgIntTraceEntry[i]));

        if(FAILED(hr))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    hr = HrInternalTraceCopyEntry(
        lAction,
        ftArrivalTime,
        ftDeferredTime,
        lpszADMDName,
        lpszCountryName,
        lpszPRMDId,
        lpszMTAName,
        lpszAttADMDName,
        lpszAttCountryName,
        lpszAttPRMDId,
        lpszAttMTAName,
        &(lpNewTraceInfo->rgIntTraceEntry[cEntries]));

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    MAPIFREEBUFFER(lpTraceInfo);

    *lppTraceInfo = lpNewTraceInfo;

cleanup:

    if(FAILED(hr))
    {
        MAPIFREEBUFFER(lpNewTraceInfo);
    }

    RETURN(hr);
}
