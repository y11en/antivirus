// --ifolder.c------------------------------------------------------------------
// 
//  Module containing MAPI utility functions for folders.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "ifolder.chk"

typedef struct {
    ULONG   cbRootFolderA;
    LPSTR   lpszRootFolderA;
    ULONG   cbRootFolderW;
    LPWSTR  lpszRootFolderW;
    ULONG   ulPropTag;
} ROOT_LOOKUP;

#define RLENTRY(str,ul) {sizeof(str), str, sizeof(L##str), L##str, ul}

static const ROOT_LOOKUP rgRootLookup[] = {
    RLENTRY( "@PR_IPM_SUBTREE_ENTRYID",              PR_IPM_SUBTREE_ENTRYID),
    RLENTRY( "@PR_IPM_OUTBOX_ENTRYID",               PR_IPM_OUTBOX_ENTRYID),
    RLENTRY( "@PR_IPM_WASTEBASKET_ENTRYID",          PR_IPM_WASTEBASKET_ENTRYID),
    RLENTRY( "@PR_IPM_SENTMAIL_ENTRYID",             PR_IPM_SENTMAIL_ENTRYID),
    RLENTRY( "@PR_IPM_PUBLIC_FOLDERS_ENTRYID",       PR_IPM_PUBLIC_FOLDERS_ENTRYID),
    RLENTRY( "@PR_IPM_FAVORITES_ENTRYID",            PR_IPM_FAVORITES_ENTRYID),
    RLENTRY( "@PR_NON_IPM_SUBTREE_ENTRYID",          PR_NON_IPM_SUBTREE_ENTRYID),
    RLENTRY( "@PR_GW_MTSIN_ENTRYID",                 PR_GW_MTSIN_ENTRYID),
    RLENTRY( "@PR_GW_MTSOUT_ENTRYID",                PR_GW_MTSOUT_ENTRYID),
    RLENTRY( "@PR_VIEWS_ENTRYID",                    PR_VIEWS_ENTRYID),
    RLENTRY( "@PR_DEFAULT_VIEW_ENTRYID",             PR_DEFAULT_VIEW_ENTRYID),
    RLENTRY( "@PR_COMMON_VIEWS_ENTRYID",             PR_COMMON_VIEWS_ENTRYID),
    RLENTRY( "@PR_FINDER_ENTRYID",                   PR_FINDER_ENTRYID),
    RLENTRY( "@PR_HEADER_FOLDER_ENTRYID",            PR_HEADER_FOLDER_ENTRYID),
    RLENTRY( "@PR_SCHEDULE_FOLDER_ENTRYID",          PR_SCHEDULE_FOLDER_ENTRYID),
    RLENTRY( "@PR_IPM_DAF_ENTRYID",                  PR_IPM_DAF_ENTRYID),
    RLENTRY( "@PR_EFORMS_REGISTRY_ENTRYID",          PR_EFORMS_REGISTRY_ENTRYID),
    RLENTRY( "@PR_SPLUS_FREE_BUSY_ENTRYID",          PR_SPLUS_FREE_BUSY_ENTRYID),
    RLENTRY( "@PR_OFFLINE_ADDRBOOK_ENTRYID",         PR_OFFLINE_ADDRBOOK_ENTRYID),
    RLENTRY( "@PR_EFORMS_FOR_LOCALE_ENTRYID",        PR_EFORMS_FOR_LOCALE_ENTRYID),
    RLENTRY( "@PR_FREE_BUSY_FOR_LOCAL_SITE_ENTRYID", PR_FREE_BUSY_FOR_LOCAL_SITE_ENTRYID),
    RLENTRY( "@PR_ADDRBOOK_FOR_LOCAL_SITE_ENTRYID",  PR_ADDRBOOK_FOR_LOCAL_SITE_ENTRYID),
};

static const ULONG ulRootLookupCount = ARRAY_CNT(rgRootLookup);

//$--_HrLookupRootFolder@------------------------------------------------------
//  Compare folder name to known root folder ENTRYID strings.  Return ENTRYID,
//  if matched.
// -----------------------------------------------------------------------------
static HRESULT _HrLookupRootFolderW(
    IN  LPMDB lpMdb,                // pointer to open message store
    IN  LPCWSTR lpszRootFolder,     // root folder name only (no separators)
    OUT ULONG *lpcbeid,             // size of entryid
    OUT LPENTRYID *lppeid)          // pointer to entryid
{
    HRESULT      hr           = NOERROR;

    DEBUGPRIVATE( "HrMAPIFindRootFolderW()");

    hr = CHK_HrMAPIFindRootFolderW( lpMdb, lpszRootFolder, lpcbeid, lppeid);
    if( FAILED( hr))
        RETURN( hr);

    *lpcbeid = 0L;
    *lppeid  = NULL;

    // Since there are no ENTRYID properties for "Inbox"...
    if( _wcsicmp( lpszRootFolder, L"@PR_IPM_INBOX_ENTRYID") == 0)
    {
        hr = MAPICALL( lpMdb)->GetReceiveFolder( /*lpMdb,*/
            NULL,       // default message class ("IPM")
            MAPI_UNICODE,
            lpcbeid,
            lppeid,
            NULL);
        if( FAILED( hr))
        {
            if( hr == MAPI_E_NOT_FOUND)
                hr = HR_LOG( EDK_E_NOT_FOUND);
            else
                hr = HR_LOG( E_FAIL);

            goto cleanup;
        }
    }
    else
    {
        ULONG cbRootFolder = 0L;
        ULONG i            = 0L;

        // Calculate length of folder name to speed up search somewhat
        cbRootFolder = cbStrLenW( lpszRootFolder);

        for( i = 0L; i < ulRootLookupCount; i++)
        {
            if( cbRootFolder == rgRootLookup[i].cbRootFolderW &&
                _wcsicmp( rgRootLookup[i].lpszRootFolderW, lpszRootFolder) == 0 )
            {
                SPropTagArray rgPropTag    = { 1, { rgRootLookup[i].ulPropTag}};
                LPSPropValue  lpPropValue  = NULL;
                ULONG         cValues      = 0L;

                // Get the outbox entry ID property. 
                hr = MAPICALL(lpMdb)->GetProps( /*lpMdb,*/
                    &rgPropTag,
                    MAPI_UNICODE,
                    &cValues,
                    &lpPropValue);
                if( FAILED( hr))
                {
                    hr = HR_LOG(E_FAIL);
                    goto cleanup;
                }
        
                if( hr == MAPI_W_ERRORS_RETURNED)
                {
                    if( lpPropValue && lpPropValue->Value.ul == MAPI_E_NOT_FOUND)
                        hr = HR_LOG( MAPI_E_NOT_FOUND);
                    else
                        hr = HR_LOG( E_FAIL);

                    goto cleanup;
                }
        
                ASSERTERROR(cValues != 0, "ZERO properties returned");
                ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");
        
                // Check to make sure we got the right property.
                if (lpPropValue->ulPropTag == rgRootLookup[i].ulPropTag)
                {
                    *lpcbeid = lpPropValue->Value.bin.cb;
    
                    hr = MAPIAllocateBuffer( *lpcbeid, (PVOID*)lppeid);
                    if( SUCCEEDED( hr))
                    {
                        CopyMemory( *lppeid, lpPropValue->Value.bin.lpb, *lpcbeid);
                    }
                    else
                    {
                        hr = HR_LOG( E_OUTOFMEMORY);
                    }
                }
                else
                {
                    hr = HR_LOG(E_FAIL);
                }

                MAPIFREEBUFFER( lpPropValue);
                goto cleanup;
            }
        } 

        // Not a recognized root folder
        hr = HR_LOG( EDK_E_NOT_FOUND);
    }

cleanup:
    RETURN( hr);
}

static HRESULT _HrLookupRootFolderA(
    IN  LPMDB lpMdb,                // pointer to open message store
    IN  LPCSTR lpszRootFolder,      // root folder name only (no separators)
    OUT ULONG *lpcbeid,             // size of entryid
    OUT LPENTRYID *lppeid)          // pointer to entryid
{
    HRESULT      hr           = NOERROR;

    DEBUGPUBLIC( "HrMAPIFindRootFolderA()");

    hr = CHK_HrMAPIFindRootFolderA( lpMdb, lpszRootFolder, lpcbeid, lppeid);
    if( FAILED( hr))
        RETURN( hr);

    *lpcbeid = 0L;
    *lppeid  = NULL;

    // Since there are no ENTRYID properties for "Inbox"...
    if( _strcmpi( lpszRootFolder, "@PR_IPM_INBOX_ENTRYID") == 0)
    {
        hr = MAPICALL( lpMdb)->GetReceiveFolder( /*lpMdb,*/
            NULL,       // default message class ("IPM")
            0L,
            lpcbeid,
            lppeid,
            NULL);
        if( FAILED( hr))
        {
            if( hr == MAPI_E_NOT_FOUND)
                hr = HR_LOG( EDK_E_NOT_FOUND);
            else
                hr = HR_LOG( E_FAIL);

            goto cleanup;
        }
    }
    else
    {
        ULONG cbRootFolder = 0L;
        ULONG i            = 0L;

        // Calculate length of folder name to speed up search somewhat
        cbRootFolder = cbStrLenA( lpszRootFolder);

        for( i = 0L; i < ulRootLookupCount; i++)
        {
            if( cbRootFolder == rgRootLookup[i].cbRootFolderA &&
                _strcmpi( rgRootLookup[i].lpszRootFolderA, lpszRootFolder) == 0 )
            {
                SPropTagArray rgPropTag    = { 1, { rgRootLookup[i].ulPropTag}};
                LPSPropValue  lpPropValue  = NULL;
                ULONG         cValues      = 0L;

                // Get the outbox entry ID property. 
                hr = MAPICALL(lpMdb)->GetProps( /*lpMdb,*/
                    &rgPropTag,
                    0L,
                    &cValues,
                    &lpPropValue);
                if( FAILED( hr))
                {
                    hr = HR_LOG(E_FAIL);
                    goto cleanup;
                }
        
                if( hr == MAPI_W_ERRORS_RETURNED)
                {
                    if( lpPropValue && lpPropValue->Value.ul == MAPI_E_NOT_FOUND)
                        hr = HR_LOG( MAPI_E_NOT_FOUND);
                    else
                        hr = HR_LOG( E_FAIL);

                    goto cleanup;
                }
        
                ASSERTERROR(cValues != 0, "ZERO properties returned");
                ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");
        
                // Check to make sure we got the right property.
                if (lpPropValue->ulPropTag == rgRootLookup[i].ulPropTag)
                {
                    *lpcbeid = lpPropValue->Value.bin.cb;
    
                    hr = MAPIAllocateBuffer( *lpcbeid, (PVOID*)lppeid);
                    if( SUCCEEDED( hr))
                    {
                        CopyMemory( *lppeid, lpPropValue->Value.bin.lpb, *lpcbeid);
                    }
                    else
                    {
                        hr = HR_LOG( E_OUTOFMEMORY);
                    }
                }
                else
                {
                    hr = HR_LOG(E_FAIL);
                }

                MAPIFREEBUFFER( lpPropValue);
                goto cleanup;
            }
        } 

        // Not a recognized root folder
        hr = HR_LOG( EDK_E_NOT_FOUND);
    }

cleanup:
    RETURN( hr);
}

//$--HrMAPIFindFolder@-----------------------------------------------------
//  Search folder for entry ID of child folder by name.
// -----------------------------------------------------------------------------
HRESULT HrMAPIFindFolderW(         // RETURNS: return code
    IN LPMAPIFOLDER lpFolder,           // pointer to folder
    IN LPCWSTR lpszName,                // name of folder to find
    OUT ULONG *lpcbeid,                 // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID pointer
{
    HRESULT			hr           = NOERROR;
	LPMAPITABLE		lpTable      = NULL;
	LPSRowSet		lpRow        = NULL;
    LPSPropValue    lpRowProp    = NULL;
    ULONG           i            = 0L;

    static const enum { IDISPNAME, IENTRYID };
    static const SizedSPropTagArray( 2, rgColProps) = { 2, { PR_DISPLAY_NAME_W, PR_ENTRYID}};
    
	DEBUGPUBLIC( "HrMAPIFindFolderW()");

    hr = CHK_HrMAPIFindFolderW( lpFolder, lpszName, lpcbeid, lppeid);
    if (FAILED(hr))
        RETURN(hr);

    // Avoid dereferencing a NULL in retail builds
    if( !lpcbeid || !lppeid)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lpcbeid = 0;
    *lppeid  = NULL;

    if( !lpFolder)
    {
        hr = HR_LOG( E_FAIL);
        goto cleanup;
    }

	hr = MAPICALL( lpFolder)->GetHierarchyTable( /*lpFolder,*/
        MAPI_UNICODE | MAPI_DEFERRED_ERRORS, &lpTable);
	if (FAILED(hr))
    {
        hr = HR_LOG( E_FAIL);
		goto cleanup;
    }

    hr = HrQueryAllRows( lpTable, (LPSPropTagArray)&rgColProps, NULL, NULL, 0L, &lpRow);
	if( FAILED( hr))
    {
        if( hr == MAPI_E_NOT_FOUND)
            hr = HR_LOG( EDK_E_NOT_FOUND);
        else
            hr = HR_LOG( E_FAIL);

        goto cleanup;
    }

    ASSERTERROR( lpRow != NULL, "NULL lpRow returned from QueryRows!");
    
    // Search rows for the folder in question
    for(i = 0; i < lpRow->cRows; i++)
    {
        ASSERTERROR(
            lpRow->aRow[i].cValues > 0,
            "Row is empty of properties.");

        lpRowProp = lpRow->aRow[i].lpProps;

        ASSERTERROR(
            lpRowProp[IENTRYID].ulPropTag == PR_ENTRYID,
            "Row does not contain entry id");
        ASSERTERROR(
            lpRowProp[IDISPNAME].ulPropTag == PR_DISPLAY_NAME_A,
            "Row does not contain display name");

        if(_wcsicmp(lpRowProp[IDISPNAME].Value.lpszW, lpszName) == 0)
        {
            *lpcbeid = lpRowProp[IENTRYID].Value.bin.cb;

            hr = MAPIAllocateBuffer(*lpcbeid, (PVOID*)lppeid);
            if(FAILED(hr))
            {
                hr = HR_LOG(E_OUTOFMEMORY);
                goto cleanup;
            }

            // Copy entry ID
            CopyMemory( *lppeid, lpRowProp[IENTRYID].Value.bin.lpb, *lpcbeid);
            goto cleanup;
        }

    }  // for

    hr = HR_LOG( EDK_E_NOT_FOUND);

cleanup:
	FREEPROWS(lpRow);
	ULRELEASE(lpTable);

    RETURN(hr);
}

HRESULT HrMAPIFindFolderA(         // RETURNS: return code
    IN LPMAPIFOLDER lpFolder,           // pointer to folder
    IN LPCSTR  lpszName,                // name of folder to find
    OUT ULONG *lpcbeid,                 // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID pointer
{
    HRESULT			hr           = NOERROR;
	LPMAPITABLE		lpTable      = NULL;
	LPSRowSet		lpRow        = NULL;
    LPSPropValue    lpRowProp    = NULL;
    ULONG           i            = 0L;

    static const enum { IDISPNAME, IENTRYID };
    static SizedSPropTagArray( 2, rgColProps) = { 2, { PR_DISPLAY_NAME_A, PR_ENTRYID}};
    
	DEBUGPUBLIC( "HrMAPIFindFolderA()");

    hr = CHK_HrMAPIFindFolderA( lpFolder, lpszName, lpcbeid, lppeid);
    if (FAILED(hr))
        RETURN(hr);

    // Avoid dereferencing a NULL in retail builds
    if( !lpcbeid || !lppeid)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lpcbeid = 0;
    *lppeid  = NULL;

    if( !lpFolder)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

	hr = MAPICALL( lpFolder)->GetHierarchyTable( /*lpFolder,*/
        MAPI_DEFERRED_ERRORS, &lpTable);
	if (FAILED(hr))
    {
        hr = HR_LOG( E_FAIL);
		goto cleanup;
    }

    hr = HrQueryAllRows( lpTable, (LPSPropTagArray)&rgColProps, NULL, NULL, 0L, &lpRow);
	if( FAILED( hr))
    {
        if( hr == MAPI_E_NOT_FOUND)
            hr = HR_LOG( EDK_E_NOT_FOUND);
        else
            hr = HR_LOG( E_FAIL);

        goto cleanup;
    }

    ASSERTERROR( lpRow != NULL, "NULL lpRow returned from QueryRows!");
    
    // Search rows for the folder in question
    for(i = 0; i < lpRow->cRows; i++)
    {
        ASSERTERROR(
            lpRow->aRow[i].cValues > 0,
            "Row is empty of properties.");

        lpRowProp = lpRow->aRow[i].lpProps;

        ASSERTERROR(
            lpRowProp[IENTRYID].ulPropTag == PR_ENTRYID,
            "Row does not contain entry id");
        ASSERTERROR(
            lpRowProp[IDISPNAME].ulPropTag == PR_DISPLAY_NAME_A,
            "Row does not contain display name");

        if(_strcmpi(lpRowProp[IDISPNAME].Value.lpszA, lpszName) == 0)
        {
            *lpcbeid = lpRowProp[IENTRYID].Value.bin.cb;

            hr = MAPIAllocateBuffer(*lpcbeid, (PVOID*)lppeid);
            if(FAILED(hr))
            {
                hr = HR_LOG(E_OUTOFMEMORY);
                goto cleanup;
            }

            // Copy entry ID
            CopyMemory( *lppeid, lpRowProp[IENTRYID].Value.bin.lpb, *lpcbeid);
            goto cleanup;
        }

    }  // for

    hr = HR_LOG( EDK_E_NOT_FOUND);

cleanup:
	FREEPROWS(lpRow);
	ULRELEASE(lpTable);

    RETURN(hr);
}

//$--HrMAPIFindSubfolderEx@---------------------------------------------------------
//  Finds an arbitrarily nested folder in the indicated folder given it's 
//  path name.
//------------------------------------------------------------------------------
HRESULT HrMAPIFindSubfolderExW(
    IN LPMAPIFOLDER lpRootFolder,       // open root folder
    IN WCHAR chSep,                     // folder path separator character
    IN LPCWSTR lpszFolderPath,          // folder path
    OUT ULONG *lpcbeid,                 // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID pointer
{
    HRESULT         hr              = NOERROR;
    LPMAPIFOLDER    lpParentFolder  = lpRootFolder;
    LPMAPIFOLDER    lpChildFolder   = NULL;
	ULONG           cbeid           = 0L;
    LPENTRYID       lpeid           = NULL;
    LPWSTR         *lppszFolderList = NULL;
    WCHAR           rgchSep[2]      = L"\\";
    ULONG           ulFolderCount   = 0L;
	ULONG			ulObjType	    = 0L;
    ULONG           i               = 0L;

    DEBUGPUBLIC( "HrMAPIFindSubfolderExW()");

    hr = CHK_HrMAPIFindSubfolderExW(
        lpRootFolder, chSep, lpszFolderPath, lpcbeid, lppeid);
    if( FAILED( hr))
        RETURN( hr);

    if( !lpcbeid || !lppeid)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    // Set the user defined separator character
    rgchSep[0] = chSep;

    hr = HrStrTokAllW( lpszFolderPath, rgchSep, &ulFolderCount, &lppszFolderList);
    if( FAILED( hr))
        goto cleanup;

    for(i = 0L; i < ulFolderCount; i++)
    {
        // Free entryid before re-use.
        MAPIFREEBUFFER( lpeid);

        hr = HrMAPIFindFolderW( lpParentFolder, lppszFolderList[i], &cbeid, &lpeid); 
        if( FAILED( hr))
            goto cleanup;
        
        // Only OpenEntry if needed for next tier of folder path.
        if( lppszFolderList[i+1] != NULL)
        {
            hr = MAPICALL( lpParentFolder)->OpenEntry( /*lpParentFolder,*/
                cbeid,
                lpeid,
                NULL,
                MAPI_DEFERRED_ERRORS,
                &ulObjType,
                (LPUNKNOWN*)&lpChildFolder);
            if( FAILED( hr) || ulObjType != MAPI_FOLDER)
            {
                MAPIFREEBUFFER( lpeid);

                hr = HR_LOG( E_FAIL);
                goto cleanup;
            }
        }

        // No longer need the parent folder
        // (Don't release the folder that was passed!)
        if( i > 0L)
            ULRELEASE( lpParentFolder);
        
        lpParentFolder = lpChildFolder;
        lpChildFolder  = NULL;
    }

    // Success!
    *lpcbeid = cbeid;
    *lppeid  = lpeid;

cleanup:
    MAPIFREEBUFFER( lppszFolderList);

    RETURN( hr);
}

HRESULT HrMAPIFindSubfolderExA(
    IN LPMAPIFOLDER lpRootFolder,       // open root folder
    IN CHAR chSep,                      // folder path separator character
    IN LPCSTR lpszFolderPath,           // folder path
    OUT ULONG *lpcbeid,                 // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID pointer
{
    HRESULT         hr              = NOERROR;
    LPMAPIFOLDER    lpParentFolder  = lpRootFolder;
    LPMAPIFOLDER    lpChildFolder   = NULL;
	ULONG           cbeid           = 0L;
    LPENTRYID       lpeid           = NULL;
    LPSTR          *lppszFolderList = NULL;
    CHAR            rgchSep[2]      = "\\";
    ULONG           ulFolderCount   = 0L;
	ULONG			ulObjType	    = 0L;
    ULONG           i               = 0L;

    DEBUGPUBLIC( "HrMAPIFindSubfolderExA()");

    hr = CHK_HrMAPIFindSubfolderExA(
        lpRootFolder, chSep, lpszFolderPath, lpcbeid, lppeid);
    if( FAILED( hr))
        RETURN( hr);

    if( !lpcbeid || !lppeid)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    // Set the user defined separator character
    rgchSep[0] = chSep;

    hr = HrStrTokAllA( lpszFolderPath, rgchSep, &ulFolderCount, &lppszFolderList);
    if( FAILED( hr))
        goto cleanup;

    for(i = 0L; i < ulFolderCount; i++)
    {
        // Free entryid before re-use.
        MAPIFREEBUFFER( lpeid);

        hr = HrMAPIFindFolderA( lpParentFolder, lppszFolderList[i], &cbeid, &lpeid); 
        if( FAILED( hr))
            goto cleanup;
        
        // Only OpenEntry if needed for next tier of folder path.
        if( lppszFolderList[i+1] != NULL)
        {
            hr = MAPICALL( lpParentFolder)->OpenEntry( /*lpParentFolder,*/
                cbeid,
                lpeid,
                NULL,
                MAPI_DEFERRED_ERRORS,
                &ulObjType,
                (LPUNKNOWN*)&lpChildFolder);
            if( FAILED( hr) || ulObjType != MAPI_FOLDER)
            {
                MAPIFREEBUFFER( lpeid);

                hr = HR_LOG( E_FAIL);
                goto cleanup;
            }
        }

        // No longer need the parent folder
        // (Don't release the folder that was passed!)
        if( i > 0L)
            ULRELEASE( lpParentFolder);
        
        lpParentFolder = lpChildFolder;
        lpChildFolder  = NULL;
    }

    // Success!
    *lpcbeid = cbeid;
    *lppeid  = lpeid;

cleanup:
    MAPIFREEBUFFER( lppszFolderList);

    RETURN( hr);
}
    
//$--HrMAPIFindFolderEx@-----------------------------------------------
//  Finds an arbitrarily nested folder in the indicated store given it's 
//  path name.
//------------------------------------------------------------------------------
HRESULT HrMAPIFindFolderExW(
    IN LPMDB lpMdb,                     // Open message store
    IN WCHAR  chSep,                    // folder path separator character
    IN LPCWSTR lpszFolderPath,          // folder path
    OUT ULONG *lpcbeid,                 // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID pointer
{
    HRESULT      hr              = NOERROR;
    LPMAPIFOLDER lpRootFolder    = NULL;
    const WCHAR *cpPath          = lpszFolderPath;
    ULONG        cbeid           = 0L;
    LPENTRYID    lpeid           = NULL;

    DEBUGPUBLIC("HrMAPIFindFolderExW()");
            
    hr = CHK_HrMAPIFindFolderExW(
        lpMdb, chSep, lpszFolderPath, lpcbeid, lppeid);
    if( FAILED(hr))
        RETURN(hr);

    // Avoid dereferencing a NULL in retail builds
    if( !lpcbeid || !lppeid)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lpcbeid = 0L;
    *lppeid  = NULL;

    if( !lpMdb)
    {
        hr = HR_LOG( E_FAIL);
        goto cleanup;
    }

    // Move forward past any initial path separators
    while( *cpPath && *cpPath == chSep) cpPath++;

    // Check for literal property name
    if( *cpPath == L'@')
    {
        WCHAR  szRootFolder[MAX_PATH + 1] = {0};
        WCHAR *cpFolder        = szRootFolder;

        // Copy root folder to string buffer
        while( *cpPath && *cpPath != chSep) *cpFolder++ = *cpPath++;
    
        *cpFolder = 0;
    
        hr = _HrLookupRootFolderW( lpMdb, szRootFolder, &cbeid, &lpeid);
        if( hr == EDK_E_NOT_FOUND)
        {   // Folder was unrecognized!  Try opening the REAL root folder,
            // and pass the FULL path to HrMAPIFindSubfolderEx().
    
            cpPath = lpszFolderPath;
            cbeid  = 0L;      // force REAL root folder
            lpeid  = NULL;
        }
        else if( FAILED( hr))
            goto cleanup;
        else
        {   // Folder found! If path continues, open the returned ENTRYID
            // and pass the REMAINING path to HrMAPIFindSubfolderEx().
    
            // Skip past next separators, if necessary
            while( *cpPath && *cpPath == chSep) cpPath++;
        }
    }

    if( *cpPath)
    {   // The path continues!

        ULONG ulObjType = 0;

        hr = MAPICALL( lpMdb)->OpenEntry( /*lpMdb,*/
            cbeid,
            lpeid,
            NULL,
            MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
            &ulObjType,
            (LPUNKNOWN*)&lpRootFolder);
        if( FAILED( hr))
            goto cleanup;

        if( ulObjType != MAPI_FOLDER)
        {
            hr = HR_LOG( E_FAIL);
            goto cleanup;
        }

        // Free before re-use
        MAPIFREEBUFFER( lpeid);

        // Find the subfolder in question
        hr = HrMAPIFindSubfolderExW(
            lpRootFolder, chSep, cpPath, &cbeid, &lpeid);
        if(FAILED(hr))
            goto cleanup;
    }

    // Success!
    *lpcbeid = cbeid;
    *lppeid  = lpeid;

cleanup:
    ULRELEASE(lpRootFolder);

    RETURN(hr);
}

HRESULT HrMAPIFindFolderExA(
    IN LPMDB lpMdb,                     // Open message store
    IN CHAR  chSep,                     // folder path separator character
    IN LPCSTR lpszFolderPath,           // folder path
    OUT ULONG *lpcbeid,                 // pointer to count of bytes in entry ID
    OUT LPENTRYID *lppeid)              // pointer to entry ID pointer
{
    HRESULT      hr              = NOERROR;
    LPMAPIFOLDER lpRootFolder    = NULL;
    const CHAR  *cpPath          = lpszFolderPath;
    ULONG        cbeid           = 0L;
    LPENTRYID    lpeid           = NULL;

    DEBUGPUBLIC("HrMAPIFindFolderExA()");
            
    hr = CHK_HrMAPIFindFolderExA(
        lpMdb, chSep, lpszFolderPath, lpcbeid, lppeid);
    if( FAILED(hr))
        RETURN(hr);

    // Avoid dereferencing a NULL in retail builds
    if( !lpcbeid || !lppeid)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lpcbeid = 0L;
    *lppeid  = NULL;

    if( !lpMdb)
    {
        hr = HR_LOG( E_FAIL);
        goto cleanup;
    }

    // Move forward past any initial path separators
    while( *cpPath && *cpPath == chSep) cpPath++;

    // Check for literal property name
    if( *cpPath == '@')
    {
        CHAR  szRootFolder[MAX_PATH + 1] = {0};
        CHAR *cpFolder        = szRootFolder;
    
        // Copy root folder to string buffer
        while( *cpPath && *cpPath != chSep) *cpFolder++ = *cpPath++;
    
        *cpFolder = 0;
    
        hr = _HrLookupRootFolderA( lpMdb, szRootFolder, &cbeid, &lpeid);
        if( hr == EDK_E_NOT_FOUND)
        {   // Folder was unrecognized!  Try opening the REAL root folder,
            // and pass the FULL path to HrMAPIFindSubfolderEx().
    
            cpPath = lpszFolderPath;
            cbeid  = 0L;      // force REAL root folder
            lpeid  = NULL;
        }
        else if( FAILED( hr))
            goto cleanup;
        else
        {   // Folder found! If path continues, open the returned ENTRYID
            // and pass the REMAINING path to HrMAPIFindSubfolderEx().
    
            // Skip past next separators, if necessary
            while( *cpPath && *cpPath == chSep) cpPath++;
        }
    }
    
    if( *cpPath)
    {   // The path continues!

        ULONG ulObjType = 0;

        hr = MAPICALL( lpMdb)->OpenEntry( /*lpMdb,*/
            cbeid,
            lpeid,
            NULL,
            MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
            &ulObjType,
            (LPUNKNOWN*)&lpRootFolder);
        if( FAILED( hr))
            goto cleanup;

        if( ulObjType != MAPI_FOLDER)
        {
            hr = HR_LOG( E_FAIL);
            goto cleanup;
        }

        // Free before re-use
        MAPIFREEBUFFER( lpeid);

        // Find the subfolder in question
        hr = HrMAPIFindSubfolderExA(
            lpRootFolder, chSep, cpPath, &cbeid, &lpeid);
        if(FAILED(hr))
            goto cleanup;
    }

    // Success!
    *lpcbeid = cbeid;
    *lppeid  = lpeid;

cleanup:
    ULRELEASE(lpRootFolder);

    RETURN(hr);
}

//$--HrMAPIOpenFolderEx@-----------------------------------------------
//  Opens an arbitrarily nested folder in the indicated store given it's 
//  path name.
//------------------------------------------------------------------------------
HRESULT HrMAPIOpenFolderExW(
    IN LPMDB lpMdb,                     // Open message store
    IN WCHAR chSep,                     // folder path separator character
    IN LPCWSTR lpszFolderPath,          // folder path
    OUT LPMAPIFOLDER * lppFolder)       // pointer to folder opened
{
    HRESULT   hr        = NOERROR;
    LPENTRYID lpeid     = NULL;
    ULONG     cbeid     = 0;
    ULONG     ulObjType = 0;

    DEBUGPUBLIC( "HrMAPIOpenFolderExW()");
            
    hr = CHK_HrMAPIOpenFolderExW(
        lpMdb, chSep, lpszFolderPath, lppFolder);
    if(FAILED(hr))
        RETURN(hr);

    if( !lppFolder)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lppFolder = NULL;

    hr = HrMAPIFindFolderExW(
        lpMdb, chSep, lpszFolderPath,
        &cbeid, &lpeid);
    if(FAILED(hr))
        goto cleanup;

    hr = MAPICALL( lpMdb)->OpenEntry( /*lpMdb,*/
        cbeid,
        lpeid,
        NULL,
        MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
        &ulObjType,
        (LPUNKNOWN*)lppFolder);
    if( FAILED( hr) || ulObjType != MAPI_FOLDER)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERT_IUNKNOWN_PTR( *lppFolder, "INVALID *lppFolder pointer");

cleanup:
    MAPIFREEBUFFER(lpeid);

    RETURN(hr);
}

HRESULT HrMAPIOpenFolderExA(
    IN LPMDB lpMdb,                     // Open message store
    IN CHAR chSep,                      // folder path separator character
    IN LPCSTR lpszFolderPath,           // folder path
    OUT LPMAPIFOLDER * lppFolder)       // pointer to folder opened
{
    HRESULT   hr        = NOERROR;
    LPENTRYID lpeid     = NULL;
    ULONG     cbeid     = 0;
    ULONG     ulObjType = 0;

    DEBUGPUBLIC( "HrMAPIOpenFolderExA()");
            
    hr = CHK_HrMAPIOpenFolderExA(
        lpMdb, chSep, lpszFolderPath, lppFolder);
    if(FAILED(hr))
        RETURN(hr);

    if( !lppFolder)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lppFolder = NULL;

    hr = HrMAPIFindFolderExA(
        lpMdb, chSep, lpszFolderPath,
        &cbeid, &lpeid);
    if(FAILED(hr))
        goto cleanup;

    hr = MAPICALL(lpMdb)->OpenEntry( /*lpMdb,*/
        cbeid,
        lpeid,
        NULL,
        MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
        &ulObjType,
        (LPUNKNOWN*)lppFolder);
    if( FAILED( hr) || ulObjType != MAPI_FOLDER)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERT_IUNKNOWN_PTR(*lppFolder, "INVALID *lppFolder pointer");

cleanup:
    MAPIFREEBUFFER(lpeid);

    RETURN(hr);
}

//$--HrMAPIOpenSubfolderEx@---------------------------------------------------
//  Opens an aribtrarily nested folder in the indicated folder given
//  its path name. 
//------------------------------------------------------------------------
HRESULT HrMAPIOpenSubfolderExW(
    IN LPMAPIFOLDER lpRootFolder,       // open root folder
    IN WCHAR chSep,                     // character used as path separator
    IN LPCWSTR lpszFolderPath,          // folder path
    OUT LPMAPIFOLDER *lppFolder)        // pointer to open child folder
{
    HRESULT   hr        = NOERROR;
    LPENTRYID lpeid     = NULL;
    ULONG     cbeid     = 0;
    ULONG     ulObjType = 0;

    DEBUGPUBLIC( "HrMAPIOpenSubfolderExW()");
            
    hr = CHK_HrMAPIOpenSubfolderExW(
        lpRootFolder, chSep, lpszFolderPath, lppFolder);
    if(FAILED(hr))
        RETURN(hr);

    if( !lppFolder)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lppFolder = NULL;

    hr = HrMAPIFindSubfolderExW(
        lpRootFolder, chSep, lpszFolderPath,
        &cbeid, &lpeid);
    if(FAILED(hr))
        goto cleanup;

    hr = MAPICALL( lpRootFolder)->OpenEntry( /*lpRootFolder,*/
        cbeid,
        lpeid,
        NULL,
        MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
        &ulObjType,
        (LPUNKNOWN*)lppFolder);
    if( FAILED( hr) || ulObjType != MAPI_FOLDER)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERT_IUNKNOWN_PTR( *lppFolder, "INVALID Folder!");

cleanup:
    MAPIFREEBUFFER(lpeid);

    RETURN(hr);
}

HRESULT HrMAPIOpenSubfolderExA(
    IN LPMAPIFOLDER lpRootFolder,       // open root folder
    IN CHAR chSep,                      // character used as path separator
    IN LPCSTR lpszFolderPath,           // folder path
    OUT LPMAPIFOLDER *lppFolder)        // pointer to open child folder
{
    HRESULT   hr        = NOERROR;
    LPENTRYID lpeid     = NULL;
    ULONG     cbeid     = 0;
    ULONG     ulObjType = 0;

    DEBUGPUBLIC( "HrMAPIOpenSubfolderExA()");
            
    hr = CHK_HrMAPIOpenSubfolderExA(
        lpRootFolder, chSep, lpszFolderPath, lppFolder);
    if(FAILED(hr))
        RETURN(hr);

    if( !lppFolder)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    *lppFolder = NULL;

    hr = HrMAPIFindSubfolderExA(
        lpRootFolder, chSep, lpszFolderPath,
        &cbeid, &lpeid);
    if(FAILED(hr))
        goto cleanup;

    hr = MAPICALL( lpRootFolder)->OpenEntry( /*lpRootFolder,*/
        cbeid,
        lpeid,
        NULL,
        MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
        &ulObjType,
        (LPUNKNOWN*)lppFolder);
    if( FAILED( hr) || ulObjType != MAPI_FOLDER)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERT_IUNKNOWN_PTR( *lppFolder, "INVALID Folder!");

cleanup:
    MAPIFREEBUFFER(lpeid);

    RETURN(hr);
}

//$--HrOpenExchangePublicFolders-----------------------------------------------
//  Opens the root of the public folder heirarchy.
//------------------------------------------------------------------------
HRESULT HrOpenExchangePublicFolders(
    IN LPMDB lpPubStore,                // pointer to public message store
    OUT LPMAPIFOLDER *lppRootFolder)    // return ptr to public folder root
{
    HRESULT       hr           = NOERROR;
    ULONG         cValues      = 0L;
    LPSPropValue  lpPropValue  = NULL;
    SPropTagArray rgPropTag    = { 1, { PR_IPM_PUBLIC_FOLDERS_ENTRYID } };
    ULONG         ulObjType    = 0L;
    LPMAPIFOLDER  lpRootFolder = NULL;
    
    DEBUGPUBLIC( "HrOpenExchangePublicFolders()");

    hr = CHK_HrOpenExchangePublicFolders( lpPubStore, lppRootFolder);
    if( FAILED( hr))
        RETURN( hr);

    // Avoid dereferencing a NULL in retail builds
    if( !lpPubStore)
    {
        hr = HR_LOG( E_INVALIDARG);
        goto cleanup;
    }

    // Get the outbox entry ID property. 
    hr = MAPICALL(lpPubStore)->GetProps( /*lpPubStore,*/
        &rgPropTag,
        fMapiUnicode,
        &cValues,
        &lpPropValue);
    if( FAILED( hr))
    {
        lpPropValue = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    if( hr == MAPI_W_ERRORS_RETURNED)
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

    ASSERTERROR(cValues != 0, "ZERO cValues variable");

    ASSERTERROR(lpPropValue != NULL, "NULL lpPropValue variable");

    // Check to make sure we got the right property.
    if (lpPropValue->ulPropTag != PR_IPM_PUBLIC_FOLDERS_ENTRYID)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    hr = MAPICALL( lpPubStore)->OpenEntry( /*lpPubStore,*/
        lpPropValue->Value.bin.cb,
        (LPENTRYID)lpPropValue->Value.bin.lpb,
        NULL,
        MAPI_BEST_ACCESS | MAPI_DEFERRED_ERRORS,
        &ulObjType,
        (LPUNKNOWN*)&lpRootFolder);
    if( FAILED( hr) || ulObjType != MAPI_FOLDER)
        goto cleanup;

    *lppRootFolder = lpRootFolder;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

