//#include <edk.h>
#include "MAPIHelper.h"

HRESULT HrCreateSearchKey(              // RETURNS: return code
    IN LPVOID lpObject,                 // pointer to object
    IN LPTSTR lpszAddressType,          // pointer to address type
    IN LPTSTR lpszAddress,              // pointer to address
    OUT LPTSTR *lppszSearchKey)         // pointer to search key
{
    HRESULT hr     = NOERROR;
    SCODE   sc     = 0;
    ULONG   cBytes = 0;

    *lppszSearchKey = NULL;

    cBytes =
        (lstrlen(lpszAddressType) +
         lstrlen(lpszAddress) + 2) * sizeof(TCHAR);

    sc = g_pMAPIEDK->pMAPIAllocateMore(cBytes, lpObject, (void **)lppszSearchKey);

    if(FAILED(sc))
    {
        hr = E_OUTOFMEMORY;
        goto cleanup;
    }

    lstrcpy(*lppszSearchKey, lpszAddressType);
    lstrcat(*lppszSearchKey, TEXT(":"));
    lstrcat(*lppszSearchKey, lpszAddress);
    CharUpperBuff(*lppszSearchKey, cBytes/sizeof(TCHAR));

cleanup:

    return hr;
}

//$--HrExpandMessageAddressList-------------------------------------------------
// Expand a list of addresses on the message.
// -----------------------------------------------------------------------------
HRESULT HrExpandMessageAddressList(     // RETURNS: return code
    IN LPMESSAGE  lpMessage,            // pointer to message
    OUT ULONG*    lpcMesgAddr,          // count of message addresses
    OUT ULONG*    lpcReplyAddr,         // count of reply recipient addresses
    OUT ULONG*    lpcRecipAddr,         // count of recipient addresses
    OUT LPADRLIST *lppRecipList,        // pointer to recipient address list
    OUT LPADRLIST *lppAdrList)          // pointer to address list
{
    HRESULT      hr                   = NOERROR;
    HRESULT      hrT                  = NOERROR;
    ULONG        i                    = 0;
    ULONG        cValues              = 0;
    LPSPropValue lpProps              = NULL;
    LPSPropValue lpPropT              = NULL;
    LPSRowSet    lpRows               = 0;

    LPMAPITABLE  lpTable              = NULL;

    SPropValue   rgProps[3]           = {0};

    ULONG        cEntries             = 0;

    ULONG        cAddr                = 0;

    ULONG        cMesgAddr            = 0;
    ULONG        cRecipAddr           = 0;
    ULONG        cReplyAddr           = 0;

#define IREPORTDEST 10

#define CADDREID    14

    SizedSPropTagArray(CADDREID, rgPropTags) =
    {
        CADDREID,
        {
            PR_SENDER_ENTRYID,
            PR_SENT_REPRESENTING_ENTRYID,
            PR_ORIGINAL_SENDER_ENTRYID,
            PR_ORIGINAL_SENT_REPRESENTING_ENTRYID,
            PR_RECEIVED_BY_ENTRYID,
            PR_RCVD_REPRESENTING_ENTRYID,
            PR_ORIGINAL_AUTHOR_ENTRYID,
            PR_REPORT_ENTRYID,
            PR_READ_RECEIPT_ENTRYID,
            PR_ORIGINATOR_ENTRYID,
            PR_REPORT_DESTINATION_ENTRYID,
            PR_CREATOR_ENTRYID,
            PR_LAST_MODIFIER_ENTRYID,
            //PR_ORIGINALLY_INTENDED_RECIP_ENTRYID,
            PR_REPLY_RECIPIENT_ENTRIES,
        }
    };

//    DEBUGPUBLIC("HrExpandMessageAddressList()");

    *lpcMesgAddr  = 0;
    *lpcReplyAddr = 0;
    *lpcRecipAddr = 0;
    *lppRecipList = NULL;
    *lppAdrList   = NULL;

    //
    // Get message addresses
    //

    hrT = MAPICALL(lpMessage)->GetProps(
        (SPropTagArray *)&rgPropTags,
        fMapiUnicode,
        &cValues,
        &lpProps);

    if(FAILED(hrT))
    {
        lpProps = NULL;

        hr = E_FAIL;
        goto cleanup;
    }

    //
    // Get the recipient table for the message
    //

    hrT = MAPICALL(lpMessage)->GetRecipientTable(
        MAPI_DEFERRED_ERRORS,
        &lpTable);

    if(FAILED(hrT))
    {
        hr = E_FAIL;
        goto cleanup;
    }

	//
	//  Query all the rows
	//

	hr = g_pMAPIEDK->pHrQueryAllRows(
	    lpTable,
		NULL,
		NULL,
		NULL,
		0,
		&lpRows);

    if(FAILED(hr))
    {
        hr = E_FAIL;
        goto cleanup;
    }

    //
    // Calculate the total number of addresses
    //

    if(lpProps[CADDREID-1].ulPropTag == PR_REPLY_RECIPIENT_ENTRIES)
    {
        cReplyAddr = lpProps[CADDREID-1].Value.MVbin.cValues;
    }
    else
    {
        cReplyAddr = 0;
    }

    cEntries = cValues - 1 + lpRows->cRows + cReplyAddr;

    for(i = 0; i < lpRows->cRows; i++)
    {
        lpPropT = g_pMAPIEDK->pLpValFindProp(
            PR_ORIGINAL_ENTRYID,
            lpRows->aRow[i].cValues,
            lpRows->aRow[i].lpProps);

        if((lpPropT != NULL) && (lpPropT->ulPropTag == PR_ORIGINAL_ENTRYID))
        {
            cEntries++;
        }
    }

    //
    // Create an ADRLIST
    //

    hr = g_pMAPIEDK->pHrMAPICreateSizedAddressList(
        cEntries,
        lppAdrList);

    if(FAILED(hr))
    {
        hr = E_FAIL;
        goto cleanup;
    }

    cAddr = 0;

    //
    // Add message addresses to the address list
    //

    cMesgAddr = --cValues;

    for(i = 0; i < cMesgAddr; i++, cAddr++)
    {
        if(PROP_TYPE(lpProps[i].ulPropTag) == PT_ERROR)
        {
            rgProps[0].ulPropTag     = PR_NULL;
            rgProps[0].Value.err     = MAPI_E_NOT_FOUND;
        }
        else
        {
            rgProps[0].ulPropTag     = PR_ENTRYID;
            rgProps[0].Value.bin.cb  = lpProps[i].Value.bin.cb;
            rgProps[0].Value.bin.lpb = lpProps[i].Value.bin.lpb;
        }

        rgProps[1].ulPropTag     = PR_RECIPIENT_TYPE;
        rgProps[1].Value.ul      = MAPI_TO;

        hr = g_pMAPIEDK->pHrMAPISetAddressList(
            cAddr,
            2,
            rgProps,
            *lppAdrList);

        if(FAILED(hr))
        {
            hr = E_FAIL;
            goto cleanup;
        }
    }

    //
    // Add reply recipient addresses to the address list
    //

    for(i = 0; i < cReplyAddr; i++, cAddr++)
    {
        rgProps[0].ulPropTag     = PR_ENTRYID;
        rgProps[0].Value.bin.cb  = lpProps[CADDREID-1].Value.MVbin.lpbin[i].cb;
        rgProps[0].Value.bin.lpb = lpProps[CADDREID-1].Value.MVbin.lpbin[i].lpb;
        rgProps[1].ulPropTag     = PR_RECIPIENT_TYPE;
        rgProps[1].Value.ul      = MAPI_TO;

        hr = g_pMAPIEDK->pHrMAPISetAddressList(
            cAddr,
            2,
            rgProps,
            *lppAdrList);

        if(FAILED(hr))
        {
            hr = E_FAIL;
            goto cleanup;
        }
    }

    //
    // Add recipient addresses to the address list
    //

    cRecipAddr = lpRows->cRows;

    for(i = 0; i < cRecipAddr; i++, cAddr++)
    {
        hr = g_pMAPIEDK->pHrMAPISetAddressList(
            cAddr,
            lpRows->aRow[i].cValues,
            lpRows->aRow[i].lpProps,
            *lppAdrList);

        if(FAILED(hr))
        {
            hr = E_FAIL;
            goto cleanup;
        }
    }

    for(i = 0; i < cRecipAddr; i++)
    {
        lpPropT = g_pMAPIEDK->pLpValFindProp(
            PR_ORIGINAL_ENTRYID,
            lpRows->aRow[i].cValues,
            lpRows->aRow[i].lpProps);

        if((lpPropT != NULL) && (lpPropT->ulPropTag == PR_ORIGINAL_ENTRYID))
        {
            rgProps[0].ulPropTag     = PR_ENTRYID;
            rgProps[0].Value.bin.cb  = lpPropT->Value.bin.cb;
            rgProps[0].Value.bin.lpb = lpPropT->Value.bin.lpb;
            rgProps[1].ulPropTag     = PR_RECIPIENT_NUMBER;
            rgProps[1].Value.ul      = i;
            rgProps[2].ulPropTag     = PR_RECIPIENT_TYPE;
            rgProps[2].Value.ul      = MAPI_TO;

            hr = g_pMAPIEDK->pHrMAPISetAddressList(
                cAddr,
                3,
                rgProps,
                *lppAdrList);

            cAddr++;

            if(FAILED(hr))
            {
                hr = E_FAIL;
                goto cleanup;
            }
        }
    }

    *lpcMesgAddr  = cMesgAddr;
    *lpcReplyAddr = cReplyAddr;
    *lpcRecipAddr = cRecipAddr;

    *lppRecipList = (LPADRLIST)lpRows;

cleanup:

    if(FAILED(hr))
    {
        *lpcMesgAddr  = 0;
        *lpcReplyAddr = 0;
        *lpcRecipAddr = 0;

        FREEPADRLIST(*lppRecipList);
    }

    // Free MAPI buffers
    MAPIFREEBUFFER(lpProps);

    // Release MAPI objects
    ULRELEASE(lpTable);

    return hr;

#undef IREPORTDEST
}
