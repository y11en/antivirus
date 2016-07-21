// --addrlkup.c-----------------------------------------------------------------
//
//  Functions used in address mapping.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "addrlkup.chk"

//$--HrFindExchangeGlobalAddressList-------------------------------------------------
// Returns the entry ID of the global address list container in the address
// book.
// -----------------------------------------------------------------------------
HRESULT HrFindExchangeGlobalAddressList( // RETURNS: return code
    IN LPADRBOOK  lpAdrBook,        // address book pointer
    OUT ULONG *lpcbeid,             // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)          // pointer to entry ID pointer
{
    HRESULT         hr                  = NOERROR;
    ULONG           ulObjType           = 0;
    ULONG           i                   = 0;
    LPMAPIPROP      lpRootContainer     = NULL;
    LPMAPIPROP      lpContainer         = NULL;
    LPMAPITABLE     lpContainerTable    = NULL;
    LPSRowSet       lpRows              = NULL;
    ULONG           cbContainerEntryId  = 0;
    LPENTRYID       lpContainerEntryId  = NULL;
    LPSPropValue    lpCurrProp          = NULL;
    SRestriction    SRestrictAnd[2]     = {0};
    SRestriction    SRestrictGAL        = {0};
    SPropValue      SPropID             = {0};
    SPropValue      SPropProvider       = {0};
    BYTE            muid[]              = MUIDEMSAB;

    SizedSPropTagArray(1, rgPropTags) =
    {
        1, 
        {
            PR_ENTRYID,
        }
    };

    DEBUGPUBLIC("HrFindExchangeGlobalAddressList()");

    hr = CHK_HrFindExchangeGlobalAddressList(
        lpAdrBook,
        lpcbeid,
        lppeid);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbeid = 0;
    *lppeid  = NULL;

    // Open the root container of the address book
    hr = MAPICALL(lpAdrBook)->OpenEntry(
        /*lpAdrBook,*/ 
        0,
        NULL,
        NULL,
        MAPI_DEFERRED_ERRORS, 
        &ulObjType,
        (LPUNKNOWN FAR *)&lpRootContainer);

    if(FAILED(hr))
    {
        goto cleanup;
    }

    if(ulObjType != MAPI_ABCONT)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get the hierarchy table of the root container
    hr = MAPICALL(((LPABCONT)lpRootContainer))->GetHierarchyTable(
        /*(LPABCONT)lpRootContainer,*/
        MAPI_DEFERRED_ERRORS|CONVENIENT_DEPTH,
        &lpContainerTable);

    if(FAILED(hr))
    {
        goto cleanup;
    }

    // Restrict the table to the global address list (GAL)
    // ---------------------------------------------------

    // Initialize provider restriction to only Exchange providers

    SRestrictAnd[0].rt                          = RES_PROPERTY;
    SRestrictAnd[0].res.resProperty.relop       = RELOP_EQ;
    SRestrictAnd[0].res.resProperty.ulPropTag   = PR_AB_PROVIDER_ID;
    SPropProvider.ulPropTag                     = PR_AB_PROVIDER_ID;

    SPropProvider.Value.bin.cb                  = 16;
    SPropProvider.Value.bin.lpb                 = (LPBYTE)muid;
    SRestrictAnd[0].res.resProperty.lpProp      = &SPropProvider;

    // Initialize container ID restriction to only GAL container

    SRestrictAnd[1].rt                          = RES_PROPERTY;
    SRestrictAnd[1].res.resProperty.relop       = RELOP_EQ;
    SRestrictAnd[1].res.resProperty.ulPropTag   = PR_EMS_AB_CONTAINERID;
    SPropID.ulPropTag                           = PR_EMS_AB_CONTAINERID;
    SPropID.Value.l                             = 0;
    SRestrictAnd[1].res.resProperty.lpProp      = &SPropID;

    // Initialize AND restriction 
    
    SRestrictGAL.rt                             = RES_AND;
    SRestrictGAL.res.resAnd.cRes                = 2;
    SRestrictGAL.res.resAnd.lpRes               = &SRestrictAnd[0];

    // Restrict the table to the GAL - only a single row should remain

    // Get the row corresponding to the GAL

	//
	//  Query all the rows
	//

	hr = HrQueryAllRows(
	    lpContainerTable,
		(LPSPropTagArray)&rgPropTags,
		&SRestrictGAL,
		NULL,
		0,
		&lpRows);

    if(FAILED(hr) || (lpRows == NULL) || (lpRows->cRows != 1))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get the entry ID for the GAL

    lpCurrProp = &(lpRows->aRow[0].lpProps[0]);

    if(lpCurrProp->ulPropTag == PR_ENTRYID)
    {
        cbContainerEntryId = lpCurrProp->Value.bin.cb;
        lpContainerEntryId = (LPENTRYID)lpCurrProp->Value.bin.lpb;
    }
    else
    {
        hr = HR_LOG(EDK_E_NOT_FOUND);
        goto cleanup;
    }

    hr = MAPIAllocateBuffer(cbContainerEntryId, (LPVOID *)lppeid);

    if(FAILED(hr))
    {
        *lpcbeid = 0;
        *lppeid = NULL;
    }
    else
    {
        CopyMemory(
            *lppeid,
            lpContainerEntryId,
            cbContainerEntryId);

        *lpcbeid = cbContainerEntryId;
    }

cleanup:

    ULRELEASE(lpRootContainer);
    ULRELEASE(lpContainerTable);
    ULRELEASE(lpContainer);

    FREEPROWS(lpRows);
    
    if(FAILED(hr))
    {
        MAPIFREEBUFFER(*lppeid);

        *lpcbeid = 0;
        *lppeid = NULL;
    }
    
    RETURN(hr);
}

//$--HrGWResolveAddress@-------------------------------------------------------
// Returns the entry ID for the recipient identified by a given address.
// -----------------------------------------------------------------------------
HRESULT HrGWResolveAddressW(
    IN LPABCONT lpGalABCont,        // pointer to GAL container
    IN LPCWSTR lpszAddress,         // pointer to proxy address
    OUT BOOL *lpfMapiRecip,         // MAPI recipient
    OUT ULONG *lpcbEntryID,         // count of bytes in entry ID
    OUT LPENTRYID *lppEntryID)      // pointer to entry ID
{
    HRESULT     hr           = NOERROR;
    LPSTR       lpszAddressA = NULL;

    DEBUGPUBLIC( "HrGWResolveAddressW()");

    hr = CHK_HrGWResolveAddressW(
        lpGalABCont, lpszAddress, lpfMapiRecip,
        lpcbEntryID, lppEntryID);
    if( FAILED( hr))
        RETURN( hr);
    
    // MAPI doesn't officially support resolution of UNICODE
    // addresses.  Therefore, the address is converted to ANSI...

    hr = HrStrWToStrA( lpszAddress, &lpszAddressA);
    if( FAILED( hr))
        goto cleanup;
    
    hr = HrGWResolveAddressA(
        lpGalABCont, lpszAddressA, lpfMapiRecip,
        lpcbEntryID, lppEntryID);

cleanup:
    MAPIFREEBUFFER( lpszAddressA);

    RETURN(hr);
}

HRESULT HrGWResolveAddressA(
    IN LPABCONT lpGalABCont,        // pointer to GAL container
    IN LPCSTR lpszAddress,          // pointer to proxy address
    OUT BOOL *lpfMapiRecip,         // MAPI recipient
    OUT ULONG *lpcbEntryID,         // count of bytes in entry ID
    OUT LPENTRYID *lppEntryID)      // pointer to entry ID
{
    HRESULT     hr          = NOERROR;
    HRESULT     hrT         = 0;
    SCODE       sc          = 0;
    LPADRLIST   lpAdrList   = NULL;
    LPFlagList  lpFlagList  = NULL;
    SPropValue  prop[2]     = {0};
    ULONG       cbEntryID   = 0;
    LPENTRYID   lpEntryID   = NULL;

    static const SizedSPropTagArray(2, rgPropTags) =
    { 2, 
        {
            PR_ENTRYID,
            PR_SEND_RICH_INFO
        }
    };

    DEBUGPUBLIC("HrGWResolveAddressA()");

    hr = CHK_HrGWResolveAddressA(
        lpGalABCont,
        lpszAddress,
        lpfMapiRecip,
        lpcbEntryID,
        lppEntryID);

    if(FAILED(hr))
        RETURN(hr);

    *lpfMapiRecip = FALSE;
    *lpcbEntryID  = 0;
    *lppEntryID   = NULL;

    sc = MAPIAllocateBuffer( CbNewFlagList(1), (LPVOID*)&lpFlagList);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    lpFlagList->cFlags    = 1;
    lpFlagList->ulFlag[0] = MAPI_UNRESOLVED;

    hr = HrMAPICreateSizedAddressList(1, &lpAdrList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    prop[0].ulPropTag = PR_DISPLAY_NAME_A;
    prop[0].Value.lpszA = (LPSTR)lpszAddress;
    prop[1].ulPropTag = PR_RECIPIENT_TYPE;
    prop[1].Value.ul = MAPI_TO;

    hr = HrMAPISetAddressList(
        0,
        2,
        prop,
        lpAdrList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hrT = MAPICALL(lpGalABCont)->ResolveNames(
        /*lpGalABCont,*/
        (LPSPropTagArray)&rgPropTags,
        EMS_AB_ADDRESS_LOOKUP,
        lpAdrList,
        lpFlagList);

    if(lpFlagList->ulFlag[0] != MAPI_RESOLVED)
    {
        if(lpFlagList->ulFlag[0] == MAPI_AMBIGUOUS)
        {
            hrT = MAPI_E_AMBIGUOUS_RECIP;
        }
        else
        {
            hrT = MAPI_E_NOT_FOUND;
        }
    }

    if(FAILED(hrT))
    {
        if(hrT == MAPI_E_NOT_FOUND)
        {
            hr = HR_LOG(EDK_E_NOT_FOUND);
        }
        else
        {
            hr = HR_LOG(E_FAIL);
        };

        goto cleanup;
    }

    cbEntryID = lpAdrList->aEntries[0].rgPropVals[0].Value.bin.cb;
    lpEntryID = (LPENTRYID)lpAdrList->aEntries[0].rgPropVals[0].Value.bin.lpb;

    sc = MAPIAllocateBuffer( cbEntryID, (LPVOID*)lppEntryID);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    CopyMemory(*lppEntryID, lpEntryID, cbEntryID);
    *lpcbEntryID  = cbEntryID;
    *lpfMapiRecip = lpAdrList->aEntries[0].rgPropVals[1].Value.b;

cleanup:

    MAPIFREEBUFFER(lpFlagList);

    FREEPADRLIST(lpAdrList);

    RETURN(hr);
}

//$--HrCheckForType@------------------------------------------------------------
//  Check if the passed in string has the requested address type. If so, return
//  the address value, otherwise fail.
// -----------------------------------------------------------------------------
static HRESULT HrCheckForTypeW(  // RETURNS: return code
    IN  LPCWSTR lpszAddrType,    // pointer to address type
    IN  LPCWSTR lpszProxy,       // pointer to proxy address
    OUT LPWSTR * lppszAddress)   // pointer to address pointer
{
    HRESULT hr              = EDK_E_NOT_FOUND;
    LPCWSTR lpszProxyAddr   = NULL;
    ULONG   cbAddress       = 0;
    SCODE   sc              = 0;
    ULONG   cchProxy        = 0;
    ULONG   cchProxyType    = 0;

    DEBUGPRIVATE("HrCheckForTypeW()");

    hr = CHK_HrCheckForTypeW(
        lpszAddrType,
        lpszProxy,
        lppszAddress);

    if(FAILED(hr))
        RETURN(hr);

	// Initialize output parameter

	*lppszAddress = NULL;

    // find the ':' separator.

    cchProxy     = wcslen(lpszProxy);
    cchProxyType = wcscspn(lpszProxy, L":");

    if((cchProxyType == 0) || (cchProxyType >= cchProxy))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = EDK_E_NOT_FOUND;

    // does the address type match?
    if((cchProxyType == wcslen(lpszAddrType)) &&
       (_wcsnicmp(lpszProxy, lpszAddrType, cchProxyType) == 0))
    {   
        // specified address type found    
        lpszProxyAddr = lpszProxy + cchProxyType + 1;

        cbAddress = cbStrLenW(lpszProxyAddr);

        // make a buffer to hold it.
        sc = MAPIAllocateBuffer(cbAddress, (void **)lppszAddress);

        if(FAILED(sc))
        {
            hr = HR_LOG(E_OUTOFMEMORY);
        }
        else
        {
            CopyMemory(*lppszAddress, lpszProxyAddr, cbAddress);

            hr = NOERROR;
        }
    }

cleanup:

    RETURN(hr);
}

static HRESULT HrCheckForTypeA( // RETURNS: return code
    IN  LPCSTR lpszAddrType,    // pointer to address type
    IN  LPCSTR lpszProxy,       // pointer to proxy address
    OUT LPSTR * lppszAddress)   // pointer to address pointer
{
    HRESULT hr              = EDK_E_NOT_FOUND;
    LPCSTR  lpszProxyAddr   = NULL;
    ULONG   cbAddress       = 0;
    SCODE   sc              = 0;
    ULONG   cchProxy        = 0;
    ULONG   cchProxyType    = 0;

    DEBUGPRIVATE("HrCheckForTypeA()");

    hr = CHK_HrCheckForTypeA(
        lpszAddrType,
        lpszProxy,
        lppszAddress);

    if(FAILED(hr))
        RETURN(hr);

	// Initialize output parameter

	*lppszAddress = NULL;

    // find the ':' separator.

    cchProxy     = lstrlenA(lpszProxy);
    cchProxyType = strcspn(lpszProxy, ":");

    if((cchProxyType == 0) || (cchProxyType >= cchProxy))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = EDK_E_NOT_FOUND;

    // does the address type match?
    if((cchProxyType == (ULONG)lstrlenA(lpszAddrType)) &&
       (_strnicmp(lpszProxy, lpszAddrType, cchProxyType) == 0))
    {   
        // specified address type found    
        lpszProxyAddr = lpszProxy + cchProxyType + 1;

        cbAddress = cbStrLenA(lpszProxyAddr);

        // make a buffer to hold it.
        sc = MAPIAllocateBuffer(cbAddress, (void **)lppszAddress);

        if(FAILED(sc))
        {
            hr = HR_LOG(E_OUTOFMEMORY);
        }
        else
        {
            CopyMemory(*lppszAddress, lpszProxyAddr, cbAddress);

            hr = NOERROR;
        }
    }

cleanup:

    RETURN(hr);
}

//$--HrGWResolveProxy@---------------------------------------------------------
// Returns the address of a specified type for the recipient identified by
// a given entry ID.
// -----------------------------------------------------------------------------
#define IADDRTYPE  0
#define IEMAILADDR 1
#define IMAPIRECIP 2
#define IPROXYADDR 3

HRESULT HrGWResolveProxyW(         // RETURNS: return code
    IN  LPADRBOOK   lpAdrBook,      // pointer to address book
    IN  ULONG       cbeid,          // count of bytes in the entry ID
    IN  LPENTRYID   lpeid,          // pointer to the entry ID
    IN  LPCWSTR     lpszAddrType,   // pointer to the address type
    OUT BOOL *lpfMapiRecip,         // MAPI recipient
    OUT LPWSTR *    lppszAddress)   // pointer to the address pointer
{
    HRESULT         hr              = EDK_E_NOT_FOUND;
    HRESULT         hrT             = 0;
    SCODE           sc              = 0;
    ULONG           i               = 0;
    ULONG           cbAddress       = 0;
    ULONG           cProxy          = 0;
    LPSPropValue    lpProps         = NULL;
    LPADRLIST       lpAdrList       = NULL;
    SPropValue      prop[2]         = {0};

    SizedSPropTagArray(4, rgPropTags) =
    {
        4, 
        {
            PR_ADDRTYPE_W,
            PR_EMAIL_ADDRESS_W,
            PR_SEND_RICH_INFO,
            PR_EMS_AB_PROXY_ADDRESSES_W
        }
    };

    DEBUGPUBLIC("HrGWResolveProxyW()");

    hr = CHK_HrGWResolveProxyW(
        lpAdrBook,
        cbeid,
        lpeid,
        lpszAddrType,
        lpfMapiRecip,
        lppszAddress);

    if(FAILED(hr))
        RETURN(hr);

	// Initialize output parameters

    *lpfMapiRecip = FALSE;
	*lppszAddress = NULL;

    hr = HrMAPICreateSizedAddressList(1, &lpAdrList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    prop[0].ulPropTag       = PR_ENTRYID;
    prop[0].Value.bin.cb    = cbeid;
    prop[0].Value.bin.lpb   = (LPBYTE)lpeid;
    prop[1].ulPropTag       = PR_RECIPIENT_TYPE;
    prop[1].Value.ul        = MAPI_TO;

    hr = HrMAPISetAddressList(
        0,
        2,
        prop,
        lpAdrList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hrT = MAPICALL(lpAdrBook)->PrepareRecips( /*lpAdrBook,*/
        0,
        (LPSPropTagArray)&rgPropTags,
        lpAdrList);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    lpProps = lpAdrList->aEntries[0].rgPropVals;

    //
    //  Hack:  detect the case where prepare recips doesn't work correctly.
    //      This happens when trying to look up a recipient that is in
    //      a replicated directory but not in the local directory.
    //
    if (lpAdrList->aEntries[0].cValues == 3)
    {
        MODULE_ERROR("PrepareRecips() failed but did not declare.");

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // If the given address type matches the PR_ADDRTYPE value,
    // return the PR_EMAIL_ADDRESS value
    
    if((PROP_TYPE(lpProps[IADDRTYPE].ulPropTag) != PT_ERROR) &&
       (PROP_TYPE(lpProps[IEMAILADDR].ulPropTag) != PT_ERROR) &&
       (_wcsicmp(lpProps[IADDRTYPE].Value.lpszW, lpszAddrType) == 0))
    {
        cbAddress = cbStrLenW(lpProps[IEMAILADDR].Value.lpszW);

        sc = MAPIAllocateBuffer(cbAddress, (void **)lppszAddress);

        if(FAILED(sc))
        {
            hr = HR_LOG(E_OUTOFMEMORY);
        }
        else
        {
            CopyMemory(*lppszAddress, lpProps[IEMAILADDR].Value.lpszW, cbAddress);
            hr = NOERROR;
        }

        goto cleanup;
    }

    // Search for a PR_EMS_AB_PROXY_ADDRESSES of the given type if present.

    else if(PROP_TYPE(lpProps[IPROXYADDR].ulPropTag) != PT_ERROR)
    {
        // count of proxy addresses
        cProxy = lpAdrList->aEntries[0].rgPropVals[IPROXYADDR].Value.MVszW.cValues;

        for(i = 0; i < cProxy; i++)
        {
            hr = HrCheckForTypeW(
                lpszAddrType, 
                lpProps[IPROXYADDR].Value.MVszW.lppszW[i],
                lppszAddress);

            if(hr == EDK_E_NOT_FOUND)
            {
                continue;
            }
            else if(FAILED(hr))
            {
                goto cleanup;
            }
            else
            {
                //
                // Found a matching proxy address.
                //

                goto cleanup;
            }
        }
    }
    else
    {
        hr = HR_LOG(EDK_E_NOT_FOUND);
        goto cleanup;
    }    

cleanup:

    if(SUCCEEDED(hr))
    {
        *lpfMapiRecip = lpAdrList->aEntries[0].rgPropVals[IMAPIRECIP].Value.b;
    }

    FREEPADRLIST(lpAdrList);
    
    RETURN(hr);
}

HRESULT HrGWResolveProxyA(         // RETURNS: return code
    IN  LPADRBOOK   lpAdrBook,      // pointer to address book
    IN  ULONG       cbeid,          // count of bytes in the entry ID
    IN  LPENTRYID   lpeid,          // pointer to the entry ID
    IN  LPCSTR      lpszAddrType,   // pointer to the address type
    OUT BOOL *lpfMapiRecip,         // MAPI recipient
    OUT LPSTR *     lppszAddress)   // pointer to the address pointer
{
    HRESULT         hr              = EDK_E_NOT_FOUND;
    HRESULT         hrT             = 0;
    SCODE           sc              = 0;
    ULONG           i               = 0;
    ULONG           cbAddress       = 0;
    ULONG           cProxy          = 0;
    LPSPropValue    lpProps         = NULL;
    LPADRLIST       lpAdrList       = NULL;
    SPropValue      prop[2]         = {0};

    SizedSPropTagArray(4, rgPropTags) =
    {
        4, 
        {
            PR_ADDRTYPE_A,
            PR_EMAIL_ADDRESS_A,
            PR_SEND_RICH_INFO,
            PR_EMS_AB_PROXY_ADDRESSES_A
        }
    };

    DEBUGPUBLIC("HrGWResolveProxyA()");

    hr = CHK_HrGWResolveProxyA(
        lpAdrBook,
        cbeid,
        lpeid,
        lpszAddrType,
        lpfMapiRecip,
        lppszAddress);

    if(FAILED(hr))
        RETURN(hr);

	// Initialize output parameters

    *lpfMapiRecip = FALSE;
	*lppszAddress = NULL;

    hr = HrMAPICreateSizedAddressList(1, &lpAdrList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    prop[0].ulPropTag       = PR_ENTRYID;
    prop[0].Value.bin.cb    = cbeid;
    prop[0].Value.bin.lpb   = (LPBYTE)lpeid;
    prop[1].ulPropTag       = PR_RECIPIENT_TYPE;
    prop[1].Value.ul        = MAPI_TO;

    hr = HrMAPISetAddressList(
        0,
        2,
        prop,
        lpAdrList);

    if(FAILED(hr))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hrT = MAPICALL(lpAdrBook)->PrepareRecips( /*lpAdrBook,*/
        0,
        (LPSPropTagArray)&rgPropTags,
        lpAdrList);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    lpProps = lpAdrList->aEntries[0].rgPropVals;

    //
    //  Hack:  detect the case where prepare recips doesn't work correctly.
    //      This happens when trying to look up a recipient that is in
    //      a replicated directory but not in the local directory.
    //
    if (lpAdrList->aEntries[0].cValues == 3)
    {
        MODULE_ERROR("PrepareRecips() failed but did not declare.");

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // If the given address type matches the PR_ADDRTYPE value,
    // return the PR_EMAIL_ADDRESS value
    
    if((PROP_TYPE(lpProps[IADDRTYPE].ulPropTag) != PT_ERROR) &&
       (PROP_TYPE(lpProps[IEMAILADDR].ulPropTag) != PT_ERROR) &&
       (_strcmpi(lpProps[IADDRTYPE].Value.lpszA, lpszAddrType) == 0))
    {
        cbAddress = cbStrLenA(lpProps[IEMAILADDR].Value.lpszA);

        sc = MAPIAllocateBuffer(cbAddress, (void **)lppszAddress);

        if(FAILED(sc))
        {
            hr = HR_LOG(E_OUTOFMEMORY);
        }
        else
        {
            CopyMemory(*lppszAddress, lpProps[IEMAILADDR].Value.lpszW, cbAddress);
            hr = NOERROR;
        }

        goto cleanup;
    }

    // Search for a PR_EMS_AB_PROXY_ADDRESSES of the given type if present.

    else if(PROP_TYPE(lpProps[IPROXYADDR].ulPropTag) != PT_ERROR)
    {
        // count of proxy addresses
        cProxy = lpAdrList->aEntries[0].rgPropVals[IPROXYADDR].Value.MVszA.cValues;

        for(i = 0; i < cProxy; i++)
        {
            hr = HrCheckForTypeA(
                lpszAddrType, 
                lpProps[IPROXYADDR].Value.MVszA.lppszA[i],
                lppszAddress);

            if(hr == EDK_E_NOT_FOUND)
            {
                continue;
            }
            else if(FAILED(hr))
            {
                goto cleanup;
            }
            else
            {
                //
                // Found a matching proxy address.
                //

                goto cleanup;
            }
        }
    }
    else
    {
        hr = HR_LOG(EDK_E_NOT_FOUND);
        goto cleanup;
    }    

cleanup:

    if(SUCCEEDED(hr))
    {
        *lpfMapiRecip = lpAdrList->aEntries[0].rgPropVals[IMAPIRECIP].Value.b;
    }

    FREEPADRLIST(lpAdrList);
    
    RETURN(hr);
}
