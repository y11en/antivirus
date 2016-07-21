// --ifind.c------------------------------------------------------------------
// 
//  Module containing FIND functions for various folders.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "ifind.chk"

//$--HrMAPIFindInbox----------------------------------------------------------
//  Find IPM inbox folder.
// -----------------------------------------------------------------------------
HRESULT HrMAPIFindInbox(              // RETURNS: return code
    IN LPMDB lpMdb,                     // pointer to message store
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // Entry ID of IPM inbox
{
    HRESULT hr  = NOERROR;
    HRESULT hrT = NOERROR;
    SCODE   sc  = 0;

    DEBUGPUBLIC("HrMAPIFindInbox()");

    hr = CHK_HrMAPIFindInbox(
        lpMdb,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Get the entry ID of the Inbox from the message store
    hrT = MAPICALL(lpMdb)->GetReceiveFolder(
        /*lpMdb,*/
        TEXT("IPM"),
        fMapiUnicode,
        lpcbeid,
        lppeid,
        NULL);

    if(FAILED(hrT))
    {
        if(hrT == MAPI_E_NOT_FOUND)
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }
    }

    ASSERTERROR(*lpcbeid != 0, "ZERO length entry ID");

    ASSERTERROR(*lppeid != NULL, "NULL entry ID");

    RETURN(hr);
}

//$--HrMAPIFindOutbox---------------------------------------------------------
//  Find IPM outbox folder.
// -----------------------------------------------------------------------------
HRESULT HrMAPIFindOutbox(             // RETURNS: return code
    IN LPMDB lpMdb,                     // pointer to message store
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // Entry ID of IPM outbox
{
    HRESULT       hr          = NOERROR;
    HRESULT       hrT         = NOERROR;
    SCODE         sc          = 0;
    ULONG         cValues     = 0;
    LPSPropValue  lpPropValue = NULL;
    ULONG         cbeid       = 0;
    SPropTagArray rgPropTag   = { 1, { PR_IPM_OUTBOX_ENTRYID } };
    
    DEBUGPUBLIC("HrMAPIFindOutbox()");

    hr = CHK_HrMAPIFindOutbox(
        lpMdb,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Get the outbox entry ID property. 
    hrT = MAPICALL(lpMdb)->GetProps(
        /*lpMdb,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);

    if(hrT == MAPI_W_ERRORS_RETURNED)
    {
        if((lpPropValue != NULL) && (lpPropValue->Value.ul == MAPI_E_NOT_FOUND))
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }
        goto cleanup;
    }

    if(FAILED(hrT))
    {
        lpPropValue = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_IPM_OUTBOX_ENTRYID)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cbeid = lpPropValue->Value.bin.cb;

    sc = MAPIAllocateBuffer(cbeid, (void **)lppeid);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    // Copy outbox Entry ID
    CopyMemory(
        *lppeid,
        lpPropValue->Value.bin.lpb,
        cbeid);

    *lpcbeid = cbeid;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrMAPIFindIPMSubtree--------------------------------------------------------
//  Find IPM subtree folder.
// -----------------------------------------------------------------------------
HRESULT HrMAPIFindIPMSubtree(            // RETURNS: return code
    IN LPMDB lpMdb,                     // pointer to message store
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // entry ID of IPM subtree
{
    HRESULT       hr          = NOERROR;
    HRESULT       hrT         = NOERROR;
    SCODE         sc          = 0;
    ULONG         cValues     = 0;
    LPSPropValue  lpPropValue = NULL;
    ULONG         cbeid       = 0;
    SPropTagArray rgPropTag   = { 1, { PR_IPM_SUBTREE_ENTRYID } };
    
    DEBUGPUBLIC("HrMAPIFindIPMSubtree()");

    hr = CHK_HrMAPIFindIPMSubtree(
        lpMdb,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Get the subtree entry ID property. 
    hrT = MAPICALL(lpMdb)->GetProps(
        /*lpMdb,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);

    if(hrT == MAPI_W_ERRORS_RETURNED)
    {
        if((lpPropValue != NULL) && (lpPropValue->Value.ul == MAPI_E_NOT_FOUND))
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }
        goto cleanup;
    }

    if(FAILED(hrT))
    {
        lpPropValue = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_IPM_SUBTREE_ENTRYID)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cbeid = lpPropValue->Value.bin.cb;

    sc = MAPIAllocateBuffer(cbeid, (void **)lppeid);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    // Copy subtree entry ID
    CopyMemory(
        *lppeid,
        lpPropValue->Value.bin.lpb,
        cbeid);

    *lpcbeid = cbeid;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrGWFindMtsOutFolder------------------------------------------------------------
//  Find MTS-OUT folder.
// -----------------------------------------------------------------------------
HRESULT HrGWFindMtsOutFolder(                // RETURNS: return code
    IN LPMDB lpMdb,                     // pointer to message store
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // entry ID of MTS-OUT
{
    HRESULT       hr          = NOERROR;
    HRESULT       hrT         = NOERROR;
    SCODE         sc          = 0;
    ULONG         cValues     = 0;
    LPSPropValue  lpPropValue = NULL;
    ULONG         cbeid       = 0;
    SPropTagArray rgPropTag   = { 1, { PR_GW_MTSOUT_ENTRYID } };
    
    DEBUGPUBLIC("HrGWFindMtsOutFolder()");

    hr = CHK_HrGWFindMtsOutFolder(
        lpMdb,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Get the entry ID property. 
    hrT = MAPICALL(lpMdb)->GetProps(
        /*lpMdb,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);

    if(hrT == MAPI_W_ERRORS_RETURNED)
    {
        if((lpPropValue != NULL) && (lpPropValue->Value.ul == MAPI_E_NOT_FOUND))
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }
        goto cleanup;
    }

    if(FAILED(hrT))
    {
        lpPropValue = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_GW_MTSOUT_ENTRYID)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cbeid = lpPropValue->Value.bin.cb;

    sc = MAPIAllocateBuffer(cbeid, (void **)lppeid);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    // Copy entry ID
    CopyMemory(
        *lppeid,
        lpPropValue->Value.bin.lpb,
        cbeid);

    *lpcbeid = cbeid;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrGWFindMtsInFolder-------------------------------------------------------------
//  Find MTS-IN folder.
// -----------------------------------------------------------------------------
HRESULT HrGWFindMtsInFolder(                 // RETURNS: return code
    IN LPMDB lpMdb,                     // pointer to message store
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // entry ID of MTS-IN
{
    HRESULT       hr          = NOERROR;
    HRESULT       hrT         = NOERROR;
    SCODE         sc          = 0;
    ULONG         cValues     = 0;
    LPSPropValue  lpPropValue = NULL;
    ULONG         cbeid       = 0;
    SPropTagArray rgPropTag   = { 1, { PR_GW_MTSIN_ENTRYID } };
    
    DEBUGPUBLIC("HrGWFindMtsInFolder()");

    hr = CHK_HrGWFindMtsInFolder(
        lpMdb,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Get the entry ID property. 
    hrT = MAPICALL(lpMdb)->GetProps(
		/*lpMdb,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);

    if(hrT == MAPI_W_ERRORS_RETURNED)
    {
        if((lpPropValue != NULL) && (lpPropValue->Value.ul == MAPI_E_NOT_FOUND))
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }
        goto cleanup;
    }

    if(FAILED(hrT))
    {
        lpPropValue = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_GW_MTSIN_ENTRYID)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cbeid = lpPropValue->Value.bin.cb;

    sc = MAPIAllocateBuffer(cbeid, (void **)lppeid);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    // Copy entry ID
    CopyMemory(
        *lppeid,
        lpPropValue->Value.bin.lpb,
        cbeid);

    *lpcbeid = cbeid;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrFindExchangePublicStore-------------------------------------------------------
//  Find public store root folder.
// -----------------------------------------------------------------------------
HRESULT HrFindExchangePublicStore(           // RETURNS: return code
    IN LPMDB lpMdb,                     // pointer to message store
    OUT ULONG *lpcbeid,                 // count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // entry ID of public store
{
    HRESULT       hr          = NOERROR;
    HRESULT       hrT         = NOERROR;
    SCODE         sc          = 0;
    ULONG         cValues     = 0;
    LPSPropValue  lpPropValue = NULL;
    ULONG         cbeid       = 0;
    SPropTagArray rgPropTag   = { 1, { PR_NON_IPM_SUBTREE_ENTRYID } };
    
    DEBUGPUBLIC("HrFindExchangePublicStore()");

    hr = CHK_HrFindExchangePublicStore(
        lpMdb,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Get the entry ID property. 
    hrT = MAPICALL(lpMdb)->GetProps(
        /*lpMdb,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);

    if(hrT == MAPI_W_ERRORS_RETURNED)
    {
        if((lpPropValue != NULL) && (lpPropValue->Value.ul == MAPI_E_NOT_FOUND))
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }
        goto cleanup;
    }

    if(FAILED(hrT))
    {
        lpPropValue = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_NON_IPM_SUBTREE_ENTRYID)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    cbeid = lpPropValue->Value.bin.cb;

    sc = MAPIAllocateBuffer(cbeid, (void **)lppeid);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    // Copy entry ID
    CopyMemory(
        *lppeid,
        lpPropValue->Value.bin.lpb,
        cbeid);

    *lpcbeid = cbeid;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

