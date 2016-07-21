// --iaddress.c-----------------------------------------------------------------
// 
//  Module containing MAPI utility functions for addressing.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "iaddress.chk"

//$--HrMAPICreateAddressList-----------------------------------------------------
//  Create an address list.
// -----------------------------------------------------------------------------
HRESULT HrMAPICreateAddressList(             // RETURNS: return code
    IN ULONG cProps,                        // count of values in address list
                                            // entry
    IN LPSPropValue lpPropValues,           // pointer to address list entry
    OUT LPADRLIST *lppAdrList)              // pointer to address list pointer
{
    HRESULT         hr              = NOERROR;
    SCODE           sc              = 0;
    LPSPropValue    lpNewPropValues = NULL;
    ULONG           cBytes          = 0;

    DEBUGPUBLIC("HrMAPICreateAddressList()\n");

    hr = CHK_HrMAPICreateAddressList(
        cProps,
        lpPropValues,
        lppAdrList);

    if(FAILED(hr))
        RETURN(hr);

    *lppAdrList = NULL;

    sc = ScDupPropset(
        cProps,
        lpPropValues,
		MAPIAllocateBuffer,
		&lpNewPropValues);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cBytes = CbNewADRLIST(1);

    sc = MAPIAllocateBuffer(cBytes, (LPVOID*)lppAdrList);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Initialize ADRLIST structure
    ZeroMemory(*lppAdrList, cBytes);

    (*lppAdrList)->cEntries = 1;
    (*lppAdrList)->aEntries[0].cValues = cProps;
    (*lppAdrList)->aEntries[0].rgPropVals = lpNewPropValues;

cleanup:

    if(FAILED(hr))
    {
        if(lppAdrList != NULL)
        {
            MAPIFREEBUFFER(*lppAdrList);
            *lppAdrList = NULL;
        }
        MAPIFREEBUFFER(lpNewPropValues);
    }

    RETURN(hr);
}

//$--HrMAPIAppendAddressList-----------------------------------------------------
//  Append to an address list.
// -----------------------------------------------------------------------------
HRESULT HrMAPIAppendAddressList(             // RETURNS: return code
    IN ULONG cProps,                        // count of values in address list
                                            // entry
    IN LPSPropValue lpPropValues,           // pointer to address list entry
    IN OUT LPADRLIST *lppAdrList)           // pointer to address list pointer
{
    HRESULT         hr              = NOERROR;
    SCODE           sc              = 0;
    LPADRLIST       lpAdrList       = NULL;
    LPADRENTRY      lpAdrEntry      = NULL;
    LPSPropValue    lpNewPropValues = NULL;
    ULONG           i               = 0;
    ULONG           cBytes          = 0;
    ULONG           cEntries        = 0;

    DEBUGPUBLIC("HrMAPIAppendAddressList()\n");

    hr = CHK_HrMAPIAppendAddressList(
        cProps,
        lpPropValues,
        lppAdrList);

    if(FAILED(hr))
        RETURN(hr);

    sc = ScDupPropset(
        cProps,
        lpPropValues,
		MAPIAllocateBuffer,
		&lpNewPropValues);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cEntries = ((*lppAdrList)->cEntries + 1);

    cBytes = CbNewADRLIST(cEntries);

    sc = MAPIAllocateBuffer(cBytes, (void **)&lpAdrList);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Initialize ADRLIST
    ZeroMemory(lpAdrList, cBytes);

    lpAdrEntry = lpAdrList->aEntries;

    // Copy old ADRENTRY values to array
    for(i = 0; i < (*lppAdrList)->cEntries; i++)
    {
        lpAdrEntry[i].cValues =
            (*lppAdrList)->aEntries[i].cValues;
        lpAdrEntry[i].rgPropVals =
            (*lppAdrList)->aEntries[i].rgPropVals;
    }

    // Copy new ADRENTRY values to array
    lpAdrEntry[i].cValues = cProps;
    lpAdrEntry[i].rgPropVals = lpNewPropValues;

    lpAdrList->cEntries = (*lppAdrList)->cEntries + 1;

    MAPIFREEBUFFER(*lppAdrList);

    *lppAdrList = lpAdrList;

cleanup:

    if(FAILED(hr))
    {
        MAPIFREEBUFFER(lpNewPropValues);
    }

    RETURN(hr);
}

//$--HrMAPICreateSizedAddressList------------------------------------------------
//  Create a sized address list.
// -----------------------------------------------------------------------------
HRESULT HrMAPICreateSizedAddressList(        // RETURNS: return code
    IN ULONG cEntries,                      // count of entries in address list
    OUT LPADRLIST *lppAdrList)              // pointer to address list pointer
{
    HRESULT         hr              = NOERROR;
    SCODE           sc              = 0;
    ULONG           cBytes          = 0;

    DEBUGPUBLIC("HrMAPICreateAddressList()\n");

    hr = CHK_HrMAPICreateSizedAddressList(
        cEntries,
        lppAdrList);

    if(FAILED(hr))
        RETURN(hr);

    *lppAdrList = NULL;

    cBytes = CbNewADRLIST(cEntries);

    sc = MAPIAllocateBuffer(cBytes, (LPVOID*)lppAdrList);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Initialize ADRLIST structure
    ZeroMemory(*lppAdrList, cBytes);

    (*lppAdrList)->cEntries = cEntries;

cleanup:

    RETURN(hr);
}

//$--HrMAPISetAddressList--------------------------------------------------------
//  Set an address list.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetAddressList(                // RETURNS: return code
    IN ULONG iEntry,                        // index of address list entry
    IN ULONG cProps,                        // count of values in address list
                                            // entry
    IN LPSPropValue lpPropValues,           // pointer to address list entry
    IN OUT LPADRLIST lpAdrList)             // pointer to address list pointer
{
    HRESULT         hr              = NOERROR;
    SCODE           sc              = 0;
    LPSPropValue    lpNewPropValues = NULL;
    ULONG           cBytes          = 0;

    DEBUGPUBLIC("HrMAPISetAddressList()\n");

    hr = CHK_HrMAPISetAddressList(
        iEntry,
        cProps,
        lpPropValues,
        lpAdrList);

    if(FAILED(hr))
        RETURN(hr);

    if(iEntry >= lpAdrList->cEntries)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    sc = ScDupPropset(
        cProps,
        lpPropValues,
		MAPIAllocateBuffer,
		&lpNewPropValues);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    if(lpAdrList->aEntries[iEntry].rgPropVals != NULL)
    {
        MAPIFREEBUFFER(lpAdrList->aEntries[iEntry].rgPropVals);
    }

    lpAdrList->aEntries[iEntry].cValues = cProps;
    lpAdrList->aEntries[iEntry].rgPropVals = lpNewPropValues;

cleanup:

    RETURN(hr);
}
