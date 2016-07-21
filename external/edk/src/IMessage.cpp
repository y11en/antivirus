// --imessage.c-----------------------------------------------------------------
// 
//  Module containing MAPI utility functions.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "imessage.chk"

//$--HrEDKSearchTableByProp-----------------------------------------------------
//  Find the entry IDs matching the restriction.
// -----------------------------------------------------------------------------
static HRESULT HrEDKSearchTableByProp(  // RETURNS: return code
    IN LPMAPITABLE lpTable,             // pointer to table
    IN LPSPropValue lpSPropValue,       // property value
    OUT LPENTRYLIST *lppMsgList)        // list of matching entry IDs
{
    HRESULT       hr         = NOERROR;
    HRESULT       hrT        = NOERROR;
    LPSRowSet     lpRows     = NULL;
    LPENTRYLIST   lpMsgList  = NULL;
    ULONG         i          = 0;
    ULONG         cbeid      = 0;
    LPENTRYID     lpeid      = NULL;
    ULONG         cRows      = 0;
    SPropTagArray rgPropTags = { 1, { PR_ENTRYID } };
    SRestriction  sres       = { 0 };
    LPSPropValue  lpProp     = NULL;

    DEBUGPRIVATE("HrEDKSearchTableByProp()");

    hr = CHK_HrEDKSearchTableByProp(
        lpTable,
        lpSPropValue,
        lppMsgList);

    if(FAILED(hr))
        RETURN(hr);

    *lppMsgList = NULL;

    // Set the columns to return
    hrT = MAPICALL(lpTable)->SetColumns(/*lpTable,*/ &rgPropTags, TBL_BATCH);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Restrict rows to those that match criteria

    sres.rt                         = RES_PROPERTY;
    sres.res.resProperty.relop      = RELOP_EQ;
    sres.res.resProperty.ulPropTag  = lpSPropValue->ulPropTag;
    sres.res.resProperty.lpProp     = lpSPropValue;

    hrT = MAPICALL(lpTable)->Restrict(/*lpTable,*/ &sres, 0);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Search the rows of the table, EDK_MAX_QUERY_ROWS at a time
    
    for(;;)
    {
    
        // Get a row set

        hrT = MAPICALL(lpTable)->QueryRows(
            /*lpTable,*/
            EDK_MAX_QUERY_ROWS,
            0,
            &lpRows);

        if(FAILED(hrT))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }

        cRows = lpRows->cRows;

        // If table is empty then no message(s)

        if((cRows == 0) && (lpMsgList == NULL))
        {
            hr = HR_LOG(EDK_E_NOT_FOUND);
            goto cleanup;
        }

        // Create list of entry IDs

        for(i = 0; i < cRows; i++)
        {
            lpProp = lpRows->aRow[i].lpProps;

            if((lpProp[0].ulPropTag == PR_ENTRYID) && (lpRows->aRow[i].cValues > 0))
            {
                cbeid = lpProp[0].Value.bin.cb;

                lpeid = (LPENTRYID)lpProp[0].Value.bin.lpb;

                if(lpMsgList == NULL)
                {
                    hrT = HrMAPICreateEntryList(cbeid,lpeid,&lpMsgList);

                    if(FAILED(hrT))
                    {
                        hr = HR_LOG(E_FAIL);
                        goto cleanup;
                    }
                }
                else
                {
                    hrT = HrMAPIAppendEntryList(cbeid,lpeid,lpMsgList);

                    if(FAILED(hrT))
                    {
                        hr = HR_LOG(E_FAIL);
                        goto cleanup;
                    }
                }
            }
            else
            {
                ASSERTERROR(
                    (lpProp[0].ulPropTag == PR_ENTRYID),"INVALID property tag");

                ASSERTERROR((lpRows->aRow[i].cValues > 0),"INVALID row entry");
            }
        }

        FREEPROWS(lpRows);
    }

cleanup:

    FREEPROWS(lpRows);

    if(FAILED(hr))
    {
        hrT = HrMAPIDestroyEntryList(&lpMsgList);

        if(FAILED(hrT))
        {
            hr = HR_LOG(E_FAIL);
        }

        *lppMsgList = NULL;
    }
    else
    {
        *lppMsgList = lpMsgList;
    }

    RETURN(hr);
}

//$--HrMAPIFindMsgByProp---------------------------------------------------------
//  Find the entry ID of a message given a property.
// -----------------------------------------------------------------------------
HRESULT HrMAPIFindMsgByProp(         // RETURNS: return code
    IN LPMAPIFOLDER lpFolder,       // pointer to folder
    IN LPSPropValue lpSPropValue,   // property value
    OUT LPENTRYLIST *lppMsgList)    // list of matching messages
{
    HRESULT       hr         = NOERROR;
    HRESULT       hrT        = NOERROR;
    LPMAPITABLE   lpTable    = NULL;

    DEBUGPUBLIC("HrMAPIFindMsgByProp()");

    hr = CHK_HrMAPIFindMsgByProp(
        lpFolder,
        lpSPropValue,
        lppMsgList);

    if(FAILED(hr))
        RETURN(hr);

    // Get the contents table for the folder
    hrT = MAPICALL(lpFolder)->GetContentsTable(
        /*lpFolder,*/
        MAPI_DEFERRED_ERRORS,
        &lpTable);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = HrEDKSearchTableByProp(
        lpTable,
        lpSPropValue,
        lppMsgList);

cleanup:

    ULRELEASE(lpTable);

    RETURN(hr);
}

//$--HrMAPIMoveMessage--------------------------------------------------------
//  Move one message from one folder to another.
// -----------------------------------------------------------------------------
HRESULT HrMAPIMoveMessage(            // RETURNS: return code
    IN LPMAPIFOLDER lpSrcFolder,        // pointer to source folder
    IN LPMAPIFOLDER lpDstFolder,        // pointer to destination folder
    IN ULONG cbeid,                     // count of bytes in entry ID
    IN LPENTRYID lpeid)                 // pointer to entry ID
{
    HRESULT     hr           = NOERROR;
    HRESULT     hrT          = NOERROR;
    LPENTRYLIST lpMsgList    = NULL;

    DEBUGPUBLIC("HrMAPIMoveMessage()");

    hr = CHK_HrMAPIMoveMessage(
        lpSrcFolder,
        lpDstFolder,
        cbeid,
        lpeid);

    if(FAILED(hr))
        RETURN(hr);

    hr = HrMAPICreateEntryList(
        cbeid,
        lpeid,
        &lpMsgList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Move the message

    hrT = MAPICALL(lpSrcFolder)->CopyMessages(
        /*lpSrcFolder,*/
        lpMsgList,
        NULL,
        lpDstFolder,
        (ULONG)0,
        NULL,
        MESSAGE_MOVE);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    hrT = HrMAPIDestroyEntryList(
        &lpMsgList);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPICopyMessage--------------------------------------------------------
//  Copy one message from one folder to another.
// -----------------------------------------------------------------------------
HRESULT HrMAPICopyMessage(            // RETURNS: return code
    IN LPMAPIFOLDER lpSrcFolder,        // pointer to source folder
    IN LPMAPIFOLDER lpDstFolder,        // pointer to destination folder
    IN ULONG cbeid,                     // count of bytes in entry ID
    IN LPENTRYID lpeid)                 // pointer to entry ID
{
    HRESULT     hr           = NOERROR;
    HRESULT     hrT          = NOERROR;
    LPENTRYLIST lpMsgList    = NULL;

    DEBUGPUBLIC("HrMAPICopyMessage()");

    hr = CHK_HrMAPICopyMessage(
        lpSrcFolder,
        lpDstFolder,
        cbeid,
        lpeid);

    if(FAILED(hr))
        RETURN(hr);

    hr = HrMAPICreateEntryList(
        cbeid,
        lpeid,
        &lpMsgList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Copy the message

    hrT = MAPICALL(lpSrcFolder)->CopyMessages(
        /*lpSrcFolder,*/
        lpMsgList,
        NULL,
        lpDstFolder,
        (ULONG)0,
        NULL,
        0);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    hrT = HrMAPIDestroyEntryList(
        &lpMsgList);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPIDeleteMessage------------------------------------------------------
//  Delete one message from one folder to another.
// -----------------------------------------------------------------------------
HRESULT HrMAPIDeleteMessage(          // RETURNS: return code
    IN LPMAPIFOLDER lpFolder,           // pointer to folder
    IN ULONG cbeid,                     // count of bytes in entry ID
    IN LPENTRYID lpeid)                 // pointer to entry ID
{
    HRESULT     hr           = NOERROR;
    HRESULT     hrT          = NOERROR;
    LPENTRYLIST lpMsgList    = NULL;

    DEBUGPUBLIC("HrMAPIDeleteMessage()");

    hr = CHK_HrMAPIDeleteMessage(
        lpFolder,
        cbeid,
        lpeid);

    if(FAILED(hr))
        RETURN(hr);

    hr = HrMAPICreateEntryList(
        cbeid,
        lpeid,
        &lpMsgList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Remove the message

    hrT = MAPICALL(lpFolder)->DeleteMessages(
        /*lpFolder,*/
        lpMsgList,
        (ULONG)0,
        NULL,
        (ULONG)0);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    hrT = HrMAPIDestroyEntryList(
        &lpMsgList);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

