// --istore.c-------------------------------------------------------------------
// 
//  Module containing MAPI utility functions for message stores.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "istore.chk"

// $--HrMAPIFindStore@----------------------------------------------------------
//
// DESCRIPTION:	Find an MDB store based on a string.
//
// INPUT:
//
//  [lpSession]		-- MAPI session ptr.
//  [lpszStore]		-- Store name (PR_DISPLAY_NAME).
//
// OUTPUT:
//
//  [lpcbentryid]	-- Number of bytes in store's entry identifier.
//  [lppentryid]	-- Ptr to store entry identifier.
//
// RETURNS:     NOERROR				if successful;
//				E_INVALIDARG		if bad input;
//				E_OUTOFMEMORY		if not enough memory;
//				EDK_E_NOT_FOUND		if specified store not found;
//              E_FAIL				otherwise.
//
//---------------------------------------------------------------------------
HRESULT HrMAPIFindStoreW(
	IN		LPMAPISESSION	lpSession,  	// MAPI session ptr
	IN		LPCWSTR			lpszStore,  	// store name
	OUT		ULONG FAR *		lpcbentryid,    // ptr to # byte in entry ID
	OUT		LPENTRYID FAR *	lppentryid)		// entry ID buffer ptr
{
    HRESULT			hr           = NOERROR;
    LPSTR           lpszStoreA   = NULL;

	DEBUGPUBLIC( "HrMAPIFindStoreW()");

    hr = CHK_HrMAPIFindStoreW( lpSession, lpszStore, lpcbentryid, lppentryid);
    if (FAILED(hr))
        RETURN(hr);

	// Because MAPI doesn't officially support UNICODE strings, we are forced
    // to do a string conversion and call the other function...
	hr = HrStrWToStrA( lpszStore, &lpszStoreA);
    if( FAILED( hr))
        goto cleanup;

    hr = HrMAPIFindStoreA( lpSession, lpszStoreA, lpcbentryid, lppentryid);

cleanup:
	MAPIFREEBUFFER( lpszStoreA);

    RETURN(hr);
}

HRESULT HrMAPIFindStoreA(
	IN		LPMAPISESSION	lpSession,  	// MAPI session ptr
	IN		LPCSTR			lpszStore,  	// store name
	OUT		ULONG FAR *		lpcbentryid,    // ptr to # byte in entry ID
	OUT		LPENTRYID FAR *	lppentryid)		// entry ID buffer ptr
{
    HRESULT			hr           = NOERROR;
	LPMAPITABLE		lpTable      = NULL;
	LPSRowSet		lpRow        = NULL;
    LPSPropValue    lpProp       = NULL;

    static SizedSPropTagArray( 1, EntryID) = { 1, { PR_ENTRYID}};
    static SPropValue RestrictProp = { PR_DISPLAY_NAME_A, 0L, { 0}};
    static SRestriction Restriction = { RES_PROPERTY, { RELOP_EQ, PR_DISPLAY_NAME_A, (ULONG)&RestrictProp}};
    
	DEBUGPUBLIC( "HrMAPIFindStoreA()");

    hr = CHK_HrMAPIFindStoreA( lpSession, lpszStore, lpcbentryid, lppentryid);
    if (FAILED(hr))
        RETURN(hr);

    // Set name of the store name to find.
    RestrictProp.Value.lpszA = (LPSTR)lpszStore;

	hr = MAPICALL( lpSession)->GetMsgStoresTable( /*lpSession,*/
        0L, &lpTable);
	if (FAILED(hr))
    {
        hr = HR_LOG( E_FAIL);
		goto cleanup;
    }

    hr = HrQueryAllRows(
        lpTable, (LPSPropTagArray)&EntryID, 
        &Restriction, NULL, 0, &lpRow);
	if( FAILED( hr))
    {
        if( hr == MAPI_E_NOT_FOUND)
            hr = HR_LOG( EDK_E_NOT_FOUND);
        else
            hr = HR_LOG( E_FAIL);

        goto cleanup;
    }

    if (lpRow == NULL || lpRow->cRows != 1)
    {
        hr = HR_LOG( E_FAIL);
        goto cleanup;
    }

    lpProp = &lpRow->aRow[0].lpProps[0];

	hr = MAPIAllocateBuffer(lpProp->Value.bin.cb, (LPVOID FAR *)lppentryid);
	if (FAILED(hr))
	{
		hr = HR_LOG(E_OUTOFMEMORY);
		goto cleanup;
	}

	memcpy(*lppentryid, lpProp->Value.bin.lpb, lpProp->Value.bin.cb);
	*lpcbentryid = lpProp->Value.bin.cb;

cleanup:
	FREEPROWS(lpRow);
	ULRELEASE(lpTable);

    RETURN(hr);
}

//$--HrMAPIFindDefaultMsgStore----------------------------------------------------
//  Get the entry ID of the default message store.
// -----------------------------------------------------------------------------
HRESULT HrMAPIFindDefaultMsgStore(    // RETURNS: return code
    IN LPMAPISESSION lplhSession,   // session pointer
    OUT ULONG *lpcbeid,             // count of bytes in entry ID
    OUT LPENTRYID *lppeid)          // entry ID of default store
{   
    HRESULT     hr      = NOERROR;
    HRESULT     hrT     = NOERROR;
    SCODE       sc      = 0;
    LPMAPITABLE lpTable = NULL;
    LPSRowSet   lpRows  = NULL;
    LPENTRYID   lpeid   = NULL;
    ULONG       cbeid   = 0;
    ULONG       cRows   = 0;
    ULONG       i       = 0;

    SizedSPropTagArray(2, rgPropTagArray) =
    {
        2,
        {
            PR_DEFAULT_STORE,
            PR_ENTRYID
        }
    };

    DEBUGPUBLIC("HrMAPIFindDefaultMsgStore()\n");

    hr = CHK_HrMAPIFindDefaultMsgStore(
        lplhSession,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    // Get the list of available message stores from MAPI

    hrT = MAPICALL(lplhSession)->GetMsgStoresTable(/*lplhSession,*/ 0, &lpTable);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get the row count for the message recipient table
    hrT = MAPICALL(lpTable)->GetRowCount(/*lpTable,*/ 0, &cRows);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Set the columns to return
    hrT = MAPICALL(lpTable)->SetColumns(/*lpTable,*/ (LPSPropTagArray)&rgPropTagArray, 0);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Go to the beginning of the recipient table for the envelope
    hrT = MAPICALL(lpTable)->SeekRow(/*lpTable,*/ BOOKMARK_BEGINNING, 0, NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Read all the rows of the table
    hrT = MAPICALL(lpTable)->QueryRows(/*lpTable,*/ cRows, 0, &lpRows);

    if(SUCCEEDED(hrT) && (lpRows != NULL) && (lpRows->cRows == 0))
    {
        FREEPROWS(lpRows);

        hrT = EDK_E_NOT_FOUND;
    }

    if(FAILED(hrT) || (lpRows == NULL))
    {
        if(hrT != EDK_E_NOT_FOUND)
        {
            hr = HR_LOG(E_FAIL);
        }
        else
        {
            hr = HR_LOG(EDK_E_NOT_FOUND);
        }

        goto cleanup;
    }


    for(i = 0; i < cRows; i++)
    {
        if(lpRows->aRow[i].lpProps[0].Value.b == TRUE)
        {
            cbeid = lpRows->aRow[i].lpProps[1].Value.bin.cb;

            sc = MAPIAllocateBuffer(cbeid, (void **)&lpeid);

            if(FAILED(sc))
            {
                cbeid = 0;
                lpeid = NULL;

                hr = HR_LOG(E_OUTOFMEMORY);
                goto cleanup;
            }

            // Copy entry ID of message store
            CopyMemory(
                lpeid,
                lpRows->aRow[i].lpProps[1].Value.bin.lpb,
                cbeid);

            break;
        }
    }

    if(lpeid == NULL)
    {
        hr = HR_LOG(E_FAIL);
    }

    ASSERTERROR(cbeid != 0, "ZERO cbeid variable");

    ASSERT_READ_PTR(lpeid, cbeid, "INVALID lpeid variable");

cleanup:

    if(lpRows != NULL)
    {
        FREEPROWS(lpRows);
    }

    ULRELEASE(lpTable);

    *lpcbeid = cbeid;
    *lppeid = lpeid;

    RETURN(hr);
}


//$--FIsPublicStore------------------------------------------------------------
//  Returns TRUE if the MDB is a public store.
// -----------------------------------------------------------------------------
BOOL FIsPublicStore(
    IN LPMDB lpmdb)                     // pointer to message store
    
{
    BOOL          bIsPublic   = FALSE;
    HRESULT       hrT         = NOERROR;
    ULONG         cValues     = 0;
    LPSPropValue  lpPropValue = NULL;
    ULONG         cbeid       = 0;

    static SPropTagArray rgPropTag = { 1, { PR_MDB_PROVIDER } };
    
    DEBUGPUBLIC( "FIsPublicStore()");

    hrT = CHK_FIsPublicStore( lpmdb);
    if(FAILED(hrT))
        return(FALSE);

    // Get the property. 
    hrT = MAPICALL(lpmdb)->GetProps( /*lpmdb,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);

    if(FAILED(hrT) || hrT == MAPI_W_ERRORS_RETURNED)
    {
        HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_MDB_PROVIDER)
    {
        HR_LOG(E_FAIL);
        goto cleanup;
    }

    // See if PR_MDB_PROVIDER == pbExchangeProviderPublicGuid

    if ( (lpPropValue->Value.bin.cb == sizeof(GUID)) &&
         (memcmp(lpPropValue->Value.bin.lpb,
            pbExchangeProviderPublicGuid, sizeof(GUID)) == 0) )
    {
         bIsPublic = TRUE;
    }

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    return(bIsPublic);
}

//$--_HrOpenStoreFromGuid-------------------------------------------------------
//  Locates the store provider based on GUID and returns open interface pointer
//------------------------------------------------------------------------------
HRESULT _HrOpenStoreFromGuid (
    IN  LPMAPISESSION lphSession,
    IN  LPGUID        pbStoreProviderGuid,
    OUT LPMDB        *lppMDB)
{
    HRESULT       hr         = NOERROR;
    HRESULT       hrT        = NOERROR;
    LPMAPITABLE   lpTable    = NULL;
    LPSRowSet     lpRows     = NULL;

    static SPropTagArray aptEntryID = { 1, { PR_ENTRYID } };
    static SPropValue    SProp = { PR_MDB_PROVIDER, 0L, { 0}};
    static SRestriction  SRestrict = { RES_PROPERTY,
        { RELOP_EQ, PR_MDB_PROVIDER, (LONG)&SProp } };
    
    DEBUGPRIVATE("_HrOpenStoreFromGuid()");

    hr = CHK_HrOpenStoreFromGuid( lphSession, pbStoreProviderGuid, lppMDB);
    if(FAILED(hr))
        RETURN(hr);

    // Clear return value

    *lppMDB = NULL;

    // Get table of message stores

    hrT = MAPICALL(lphSession)->GetMsgStoresTable(/*lphSession,*/ 0, &lpTable);
    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Restrict the table to the appropriate Exchange store
    SProp.Value.bin.cb  = sizeof(GUID);
    SProp.Value.bin.lpb = (LPBYTE)pbStoreProviderGuid;

    hrT = HrQueryAllRows( lpTable, &aptEntryID, &SRestrict, NULL, 0L, &lpRows);
    if(FAILED(hrT) || (lpRows->cRows == 0))
    {
        if((hrT == MAPI_E_NOT_FOUND) ||
           ((hrT == SUCCESS_SUCCESS) && (lpRows->cRows == 0)))
        {
            hr = HR_LOG(EDK_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        }

        goto cleanup;
    }

    if((lpRows->aRow->cValues != 1) || 
        (lpRows->aRow->lpProps[0].ulPropTag != PR_ENTRYID))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Open the message store 

    hrT = MAPICALL(lphSession)->OpenMsgStore(
        /*lphSession,*/
        (ULONG)0,
        lpRows->aRow->lpProps[0].Value.bin.cb,
        (LPENTRYID)lpRows->aRow->lpProps[0].Value.bin.lpb,
        NULL,
        MAPI_BEST_ACCESS | MDB_NO_DIALOG,
        lppMDB);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    FREEPROWS(lpRows);

    ULRELEASE(lpTable);

    RETURN(hr);
}

