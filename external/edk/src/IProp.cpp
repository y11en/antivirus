// --iprop.c--------------------------------------------------------------------
// 
//  Module containing MAPI utility functions for properties.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"
#include "iprop.chk"

//$--HrMAPIOpenCachedProp--------------------------------------------------------
//
//  DESCRIPTION: Create a new (local) IPropData object in which the original object
//  properties are cached.  The local cached can be created for
//  reading (for use with GetProp calls) for for writing (for use with
//  SetProp calls).  The purpose of this function and HrMAPICloseCachedProp
//  is to reduce the number of remote procedure calls made by code
//  which performs many GetProp or SetProp calls on an object.
//
//  INPUT:  lpObj   --  property object to cache
//          lpPropList  --  list of properties to cache (for reading)
//                          defaults to all properties if NULL.
//          ulFlags --  read OR write access flag (EDK_CACHE_READ
//                      or EDK_CACHE_WRITE)
//          
//  OUTPUT: lppCachedObj    --  cached property object
//
//  RETURNS:    HRESULT --  NOERROR if successful,
//                          E_INVALIDARG if bad input
//                          E_FAIL otherwise.
//
//  NOTE:   This function creates a cached object for reading only
//          or for writing only.  It does not support and object
//          for both reading and writing.
//
// -----------------------------------------------------------------------------
HRESULT HrMAPIOpenCachedProp(            // RETURNS: return code
    IN LPMAPIPROP lpObj,                // source object
    IN LPSPropTagArray lpPropList,      // list of properties to cache
    IN ULONG ulFlags,                   // open for reading only or for writing only
	OUT LPPROPDATA FAR * lppCachedObj)  // cached version of source object
{
	HRESULT             hr         = NOERROR;
	LPSPropProblemArray lpProblems = NULL;
    LPSPropValue        lpPropVals = NULL;
    ULONG               ulPropCount= 0;     // number of properties

    DEBUGPUBLIC("HrMAPIOpenCachedProp()\n");

    hr = CHK_HrMAPIOpenCachedProp(
        lpObj,
        lpPropList,
        ulFlags,
    	lppCachedObj);

    if(FAILED(hr))
        RETURN(hr);

    *lppCachedObj = NULL;

    //
	// Call CreateIProp() to create a new IPropData object.
	// Since IPropData inherits from IMAPIProp, we can use it as our
	// IMAPIProp cache pointer.
    //

	hr = CreateIProp(
	    &IID_IMAPIPropData, // interface type
		MAPIAllocateBuffer,	// allocation routine
	    MAPIAllocateMore,   // allocate more routine
	    MAPIFreeBuffer,     // deallocation routine
	    0,                  // reserved
	    lppCachedObj);      // address of pointer to new IPropData object

   	if(FAILED(hr))
   	{
		hr = HR_LOG(E_FAIL);

        goto cleanup;
	}

    // If we are creating the cache for reading, then
    // we must populate the new object with properties. 
    // Otherwise, we pass an "empty" object back to the user
    // for property writing.
    if ( ulFlags == EDK_CACHE_READ )
    {
        // Get properties from remote object and set these same properties
        // on the local object.  
        hr = MAPICALL(lpObj)->GetProps(
			/*lpObj,*/                  // for C to C++ vtbl resolution
            lpPropList,             // Get all properties
            fMapiUnicode,           // flags
            &ulPropCount,           // number of properties retrieved
            &lpPropVals);           // property values structure pointer

        // handle errors
        // If there is an error, release the cached object
        if ( FAILED(hr) || (hr == MAPI_W_ERRORS_RETURNED) )
        {
            hr = HR_LOG(E_FAIL);
                    
            goto cleanup;
        }

        hr = MAPICALL(*lppCachedObj)->SetProps(/**lppCachedObj,*/
                                               ulPropCount,
                                               lpPropVals,
                                               &lpProblems);
      
        // handle errors
        if ( FAILED(hr) )
        {
            hr = HR_LOG(E_FAIL);

            goto cleanup;    
        }

        // Check to see if there were any problems setting the
        // properties
        if ( lpProblems != NULL )
        {
            // We have an error
            hr = HR_LOG(E_FAIL);

            goto cleanup;
        }
    }   // end if creating cache for reading

    // We have been successful.
	
cleanup:

    // Handle error case
    if ( FAILED(hr) )
    {
        ULRELEASE(*lppCachedObj);
    }

    // Free MAPI buffers
    MAPIFREEBUFFER(lpProblems);
    MAPIFREEBUFFER(lpPropVals);

    RETURN(hr);
}

//$--HrMAPICloseCachedProp-------------------------------------------------------
//
//  DESCRIPTION: If object was created as a write cache,
//               copy properties in local cached object
//               back to original remote object.
//
//  INPUT:  lpCachedObj --  cached property object
//          lpOriginalObj   --  original property object
//          ulFlags --  read cache or write cache flag (EDK_CACHE_READ
//                      or EDK_CACHE_WRITE)
//
//  OUTPUT: lppProblems --  set to the property problem array returned
//          by if there were problems setting properties on the original
//          object
//
//  NOTES:  lppProblems:  It may be set, even though overall call
//          is successful.  This is because all of the SetProps have been "deferred" on the
//          original object until this call, the user will need to evaluate
//          the contents of the lppProblems buffer pointer based on which
//          properties he/or she actually tried to set.  
//
//  RETURNS:    HRESULT --  NOERROR if successful,
//                          E_INVALIDARG if bad input
//                          E_FAIL otherwise
//
//                          lppProblems will only be valid if return code
//                          is NOERROR.
//
// -----------------------------------------------------------------------------
HRESULT HrMAPICloseCachedProp(           // RETURNS: return code
    IN LPPROPDATA lpCachedObj,          // cached property object
    IN LPMAPIPROP lpOriginalObj,        // original object
    IN ULONG ulFlags,                   // cache type (EDK_CACHE_READ or EDK_CACHE_WRITE)
    OUT LPSPropProblemArray FAR * lppProblems) // pointer to property problems array if problems setting properties
{
	HRESULT             hr          = NOERROR;	
    ULONG               ulPropCount = 0;        // number of properties
    LPSPropValue        lpPropVals  = NULL;     // property values

    DEBUGPUBLIC("HrMAPICloseCachedProp()\n");

    hr = CHK_HrMAPICloseCachedProp(
        lpCachedObj,
        lpOriginalObj,
        ulFlags,
        lppProblems);

    if(FAILED(hr))
        RETURN(hr);

    *lppProblems = NULL;    // initialize output

    // If the cache was opened for writing, copy all of the
    // properties from the cached object back to the original
    // object.
    if ( ulFlags == EDK_CACHE_WRITE )
    {
        // Get properties from local object and set these same properties
        // on the remote object.  CopyTo() is not sufficient, because it
        // won't copy read-only properties.
        hr = MAPICALL(lpCachedObj)->GetProps(
			/*lpCachedObj,*/            // for C to C++ vtbl resolution
            NULL,                   // get all properties
            fMapiUnicode,           // flags
            &ulPropCount,           // number of properties retrieved
            &lpPropVals);           // property values structure pointer

        // handle errors
        if ( FAILED(hr) || (hr == MAPI_W_ERRORS_RETURNED) )
        {
            hr = HR_LOG(E_FAIL);

            goto cleanup;
        }

        // Set all properties retrieved in the remote object        
        hr = MAPICALL(lpOriginalObj)->SetProps(/*lpOriginalObj,*/
                                               ulPropCount,
                                               lpPropVals,
                                               lppProblems);
      
        // handle errors
        if ( FAILED(hr) )
        {
            hr = HR_LOG(E_FAIL);

            goto cleanup;

        }
    }   // end if cache created for writing

    // The user will need to evaluate why the set property
    // call has failed (if *lppProblems has been set
    // by the SetProps call) based on what properties (if any)
    // they have set in the cached object.

    // Overall, we have been successful.

cleanup:

    // Free MAPI buffers
    MAPIFREEBUFFER(lpPropVals);

    RETURN(hr);
}

//$--HrMAPIGetPropString---------------------------------------------------------
//  Get a string property. 
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetPropString(             // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    OUT ULONG *lpcbProp,                // count of bytes in property
    OUT LPVOID *lppvProp)               // pointer to property address variable
{
    HRESULT      hr          = NOERROR;
    HRESULT      hrT         = NOERROR;
    ULONG        cValues     = 0;
    LPSPropValue lpPropValue = NULL;
    ULONG        cbProp      = 0;
    SCODE        sc          = 0;

    SizedSPropTagArray(1, rgPropTag) =
    {
        1,
        {
            0
        }
    };

    DEBUGPUBLIC("HrMAPIGetPropString()\n");

    hr = CHK_HrMAPIGetPropString(
        lpObj,
        ulPropTag,
        lpcbProp,
        lppvProp);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbProp = 0;
    *lppvProp = NULL;

    rgPropTag.cValues = 1;
    rgPropTag.aulPropTag[0] = ulPropTag;

    hrT = MAPICALL(lpObj)->GetProps(
         /*lpObj,*/
         (LPSPropTagArray)&rgPropTag,
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

    if(PROP_TYPE(ulPropTag) == PT_STRING8)
    {
        cbProp = ((lpPropValue->Value.lpszA == NULL) ?
            sizeof(CHAR) :
            cbStrLenA(lpPropValue->Value.lpszA));
    }
    else
    {
        cbProp = ((lpPropValue->Value.lpszW == NULL) ?
            sizeof(WCHAR) :
            cbStrLenW(lpPropValue->Value.lpszW));
    }

    sc = MAPIAllocateBuffer(cbProp, lppvProp);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    ASSERTERROR(*lppvProp != NULL, "NULL *lppvProp pointer");

    if(PROP_TYPE(ulPropTag) == PT_STRING8)
    {
        memcpy(*lppvProp, lpPropValue->Value.lpszA, cbProp);
    }
    else
    {
        memcpy(*lppvProp, lpPropValue->Value.lpszW, cbProp);
    }

    *lpcbProp = cbProp;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrMAPISetPropString---------------------------------------------------------
//  Set a string property. 
// -----------------------------------------------------------------------------
HRESULT HrMAPISetPropString(             // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN LPVOID lpvProp)                  // pointer to property
{
    HRESULT    hr        = NOERROR;
    HRESULT    hrT       = NOERROR;
    ULONG      cValues   = 1;
    SPropValue PropValue = {0};

    DEBUGPUBLIC("HrMAPISetPropString()\n");

    hr = CHK_HrMAPISetPropString(
        lpObj,
        ulPropTag,
        lpvProp);

    if(FAILED(hr))
        RETURN(hr);

    // Initialize SPropValue structure
    memset(&PropValue, 0, sizeof(PropValue));

    PropValue.ulPropTag     = ulPropTag;

    if(PROP_TYPE(ulPropTag) == PT_STRING8)
    {
        PropValue.Value.lpszA = (LPSTR)lpvProp;
    }
    else
    {
        PropValue.Value.lpszW = (LPWSTR)lpvProp;
    }

    hrT = MAPICALL(lpObj)->SetProps(
        /*lpObj,*/
        cValues,
        &PropValue,
        NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPIGetPropBinary---------------------------------------------------------
//  Get a binary property.
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetPropBinary(             // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    OUT ULONG *lpcbProp,                // count of bytes in property
    OUT LPVOID *lppvProp)               // pointer to property address variable
{
    HRESULT      hr          = NOERROR;
    HRESULT      hrT         = NOERROR;
    ULONG        cValues     = 0;
    LPSPropValue lpPropValue = NULL;
    ULONG        cbProp      = 0;
    SCODE        sc          = 0;

    SizedSPropTagArray(1, rgPropTag) =
    {
        1,
        {
            0
        }
    };

    DEBUGPUBLIC("HrMAPIGetPropBinary()\n");

    hr = CHK_HrMAPIGetPropBinary(
        lpObj,
        ulPropTag,
        lpcbProp,
        lppvProp);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbProp = 0L;
    *lppvProp = NULL;

    rgPropTag.cValues = 1;
    rgPropTag.aulPropTag[0] = ulPropTag;

    hrT = MAPICALL(lpObj)->GetProps(
         /*lpObj,*/
         (LPSPropTagArray)&rgPropTag,
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

    cbProp = lpPropValue->Value.bin.cb;

    sc = MAPIAllocateBuffer(cbProp, lppvProp);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_OUTOFMEMORY);
        goto cleanup;
    }

    ASSERTERROR(*lppvProp != NULL, "NULL *lppvProp pointer");

    // Copy property value
    memcpy(*lppvProp, lpPropValue->Value.bin.lpb, cbProp);

    *lpcbProp = cbProp;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrMAPISetPropBinary---------------------------------------------------------
//  Set a binary property.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetPropBinary(             // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN ULONG cbProp,                    // count of bytes in property
    IN LPVOID lpvProp)                  // pointer to property
{
    HRESULT    hr        = NOERROR;
    HRESULT    hrT       = NOERROR;
    ULONG      cValues   = 1;
    SPropValue PropValue = {0};

    DEBUGPUBLIC("HrMAPISetPropBinary()\n");

    hr = CHK_HrMAPISetPropBinary(
        lpObj,
        ulPropTag,
        cbProp,
        lpvProp);

    if(FAILED(hr))
        RETURN(hr);

    // Initialize SPropValue structure
    memset(&PropValue, 0, sizeof(PropValue));

    PropValue.ulPropTag     = ulPropTag;
    PropValue.Value.bin.cb  = cbProp;
    PropValue.Value.bin.lpb = (LPBYTE)lpvProp;

    hrT = MAPICALL(lpObj)->SetProps(
		/*lpObj,*/
        cValues,
        &PropValue,
        NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPIGetPropBoolean--------------------------------------------------------
//  Get a boolean property.
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetPropBoolean(            // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    OUT BOOL *lpfProp)                  // pointer to property variable
{
    HRESULT      hr          = NOERROR;
    HRESULT      hrT         = NOERROR;
    ULONG        cValues     = 0;
    LPSPropValue lpPropValue = NULL;
    SCODE        sc          = 0;

    SizedSPropTagArray(1, rgPropTag) =
    {
        1,
        {
            0
        }
    };

    DEBUGPUBLIC("HrMAPIGetPropBoolean()\n");

    hr = CHK_HrMAPIGetPropBoolean(
        lpObj,
        ulPropTag,
        lpfProp);

    if(FAILED(hr))
        RETURN(hr);

    *lpfProp = FALSE;

    rgPropTag.cValues = 1;
    rgPropTag.aulPropTag[0] = ulPropTag;

    hrT = MAPICALL(lpObj)->GetProps(
         /*lpObj,*/
         (LPSPropTagArray)&rgPropTag,
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

    *lpfProp = (BOOL)(lpPropValue->Value.b);

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrMAPISetPropBoolean--------------------------------------------------------
//  Set a boolean property.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetPropBoolean(            // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN BOOL fProp)                      // property
{
    HRESULT    hr        = NOERROR;
    HRESULT    hrT       = NOERROR;
    ULONG      cValues   = 1;
    SPropValue PropValue = {0};

    DEBUGPUBLIC("HrMAPISetPropBoolean()\n");

    hr = CHK_HrMAPISetPropBoolean(
        lpObj,
        ulPropTag,
        fProp);

    if(FAILED(hr))
        RETURN(hr);

    // Initialize SPropValue structure
    ZeroMemory(&PropValue, sizeof(PropValue));

    PropValue.ulPropTag = ulPropTag;
    PropValue.Value.b   = fProp;

    hrT = MAPICALL(lpObj)->SetProps(
        /*lpObj,*/
        cValues,
        &PropValue,
        NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPIGetPropLong-----------------------------------------------------------
//  Get a long property.
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetPropLong(               // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    OUT ULONG *lpulProp)                // pointer to property variable
{
    HRESULT      hr          = NOERROR;
    HRESULT      hrT         = NOERROR;
    ULONG        cValues     = 0;
    LPSPropValue lpPropValue = NULL;
    SCODE        sc          = 0;

    SizedSPropTagArray(1, rgPropTag) =
    {
        1,
        {
            0
        }
    };

    DEBUGPUBLIC("HrMAPIGetPropLong()\n");

    hr = CHK_HrMAPIGetPropLong(
        lpObj,
        ulPropTag,
        lpulProp);

    if(FAILED(hr))
        RETURN(hr);

    *lpulProp = 0;

    rgPropTag.cValues = 1;
    rgPropTag.aulPropTag[0] = ulPropTag;

    hrT = MAPICALL(lpObj)->GetProps(
         /*lpObj,*/
         (LPSPropTagArray)&rgPropTag,
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

    *lpulProp = lpPropValue->Value.ul;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrMAPISetPropLong-----------------------------------------------------------
//  Set a long property.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetPropLong(               // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN ULONG ulProp)                    // property
{
    HRESULT    hr        = NOERROR;
    HRESULT    hrT       = NOERROR;
    ULONG      cValues   = 1;
    SPropValue PropValue = {0};

    DEBUGPUBLIC("HrMAPISetPropLong()\n");

    hr = CHK_HrMAPISetPropLong(
        lpObj,
        ulPropTag,
        ulProp);

    if(FAILED(hr))
        RETURN(hr);

    // Initialize SPropValue structure
    ZeroMemory(&PropValue, sizeof(PropValue));

    PropValue.ulPropTag = ulPropTag;
    PropValue.Value.ul  = ulProp;

    hrT = MAPICALL(lpObj)->SetProps(
        /*lpObj,*/
        cValues,
        &PropValue,
        NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPIGetPropSystime--------------------------------------------------------
//  Get a systime property.
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetPropSystime(            // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    OUT LPFILETIME lpSystime)           // pointer to property variable
{
    HRESULT      hr          = NOERROR;
    HRESULT      hrT         = NOERROR;
    ULONG        cValues     = 0;
    LPSPropValue lpPropValue = NULL;
    SCODE        sc          = 0;

    SizedSPropTagArray(1, rgPropTag) =
    {
        1,
        {
            0
        }
    };

    DEBUGPUBLIC("HrMAPIGetPropSystime()\n");

    hr = CHK_HrMAPIGetPropSystime(
        lpObj,
        ulPropTag,
        lpSystime);

    if(FAILED(hr))
        RETURN(hr);

    // Initialize FILETIME structure
    ZeroMemory(lpSystime, sizeof(FILETIME));

    rgPropTag.cValues = 1;
    rgPropTag.aulPropTag[0] = ulPropTag;

    hrT = MAPICALL(lpObj)->GetProps(
         /*lpObj,*/
         (LPSPropTagArray)&rgPropTag,
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

    lpSystime->dwLowDateTime = lpPropValue->Value.ft.dwLowDateTime;
    lpSystime->dwHighDateTime = lpPropValue->Value.ft.dwHighDateTime;

cleanup:

    MAPIFREEBUFFER(lpPropValue);

    RETURN(hr);
}

//$--HrMAPISetPropSystime--------------------------------------------------------
//  Set a systime property.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetPropSystime(            // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN LPFILETIME lpSystime)            // pointer to property
{
    HRESULT    hr        = NOERROR;
    HRESULT    hrT       = NOERROR;
    ULONG      cValues   = 1;
    SPropValue PropValue = {0};

    DEBUGPUBLIC("HrMAPISetPropSystime()\n");

    hr = CHK_HrMAPISetPropSystime(
        lpObj,
        ulPropTag,
        lpSystime);

    if(FAILED(hr))
        RETURN(hr);

    // Initialize SPropValue structure
    ZeroMemory(&PropValue, sizeof(PropValue));

    PropValue.ulPropTag                 = ulPropTag;
    PropValue.Value.ft.dwLowDateTime    = lpSystime->dwLowDateTime;
    PropValue.Value.ft.dwHighDateTime   = lpSystime->dwHighDateTime;

    hrT = MAPICALL(lpObj)->SetProps(
        /*lpObj,*/
        cValues,
        &PropValue,
        NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
    }

    RETURN(hr);
}

//$--HrMAPIGetPropToFile---------------------------------------------------------
//  Get a property and put in a given file.
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetPropToFile(             // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN LPSTR lpszFilename,             // pointer to destination file name
    OUT ULONG *lpcbProp)                // pointer to count of bytes address
                                        // variable
{
    HRESULT  hr             = NOERROR;
    HRESULT  hrT            = NOERROR;
    SCODE    sc             = 0;
    LPSTREAM lpStream       = NULL;
    HANDLE   hFile          = NULL;
    ULONG    ulBytesRead    = 0;
    LPBYTE   lpbBlock       = NULL;
    DWORD    dwBytesWritten = 0;

    DEBUGPUBLIC("HrMAPIGetPropToFile()\n");

    hr = CHK_HrMAPIGetPropToFile(
        lpObj,
        ulPropTag,
        lpszFilename,
        lpcbProp);

    if(FAILED(hr))
        RETURN(hr);

    // Open a stream on the property
    hrT = MAPICALL(lpObj)->OpenProperty(
        /*lpObj,*/
        ulPropTag,
        (LPIID)&IID_IStream,
        STGM_READ,
        MAPI_DEFERRED_ERRORS,
        (LPUNKNOWN *)&lpStream);

    if(FAILED(hrT))
    {
        // Streams are not supported by provider
        if((hrT == MAPI_E_NO_SUPPORT) || (hrT == MAPI_E_INTERFACE_NOT_SUPPORTED))
        {
            ULONG PropType = 0;

            lpStream = NULL;

            MODULE_WARNING1("Streams are not supported by provider [%08lx]",hrT);

            PropType = PROP_TYPE(ulPropTag);

            // Read property into memory            
            switch(PropType)
            {
            case PT_BINARY:
                hr = HrMAPIGetPropBinary(
                    lpObj,
                    ulPropTag,
                    &ulBytesRead,
                    (void **)&lpbBlock);
                break;
            default:
                hr = HrMAPIGetPropString(
                    lpObj,
                    ulPropTag,
                    &ulBytesRead,
                    (void **)&lpbBlock);
            }
        }
        else
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }

        if(FAILED(hr))
        {
            goto cleanup;
        }
    }

    hFile = CreateFile(
        lpszFilename,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    //  Copy propery value to the file
    if(lpStream != NULL)
    {
        sc = MAPIAllocateBuffer(EDK_CBTRANSFER, (void **)&lpbBlock);

        // An error occured allocating the block buffer
        if(FAILED(sc))
        {
            hr = HR_LOG(E_OUTOFMEMORY);
            goto cleanup;
        }

        for (;;)
        {
            // Read a block from the stream
            hrT = /*OLECALL*/(lpStream)->Read(
                /*lpStream,*/
                lpbBlock,
                EDK_CBTRANSFER,
                &ulBytesRead);

            if(FAILED(hrT))
            {
                hr = HR_LOG(E_FAIL);
                goto cleanup;
            }

            if(ulBytesRead == 0L)
                break;

            // Write the block to the file
            hr = _HrWriteFile(hFile, ulBytesRead, lpbBlock);

            if(FAILED(hr))
            {
                goto cleanup;
            }
        }
    }
    else
    {
        // Write the block to the file
        hr = _HrWriteFile(hFile, ulBytesRead, lpbBlock);

        if(FAILED(hr))
        {
            goto cleanup;
        }
    }


cleanup:

    // Close the file
    if(hFile != NULL)
    {
        if(CloseHandle(hFile) == FALSE)
        {
            hr = HR_LOG(E_FAIL);
        }
    }

    // Release the stream
    //ULOLERELEASE(lpStream);
	if (lpStream != NULL)
	{
		lpStream->Release();
	}
	lpStream = NULL;

    MAPIFREEBUFFER(lpbBlock);

    RETURN(hr);
}

//$--HrMAPISetPropFromFile-------------------------------------------------------
//  Set a property from a given file.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetPropFromFile(           // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN LPSTR lpszFilename,             // pointer to source file name
    OUT ULONG *lpcbProp)                // pointer to count of bytes address
                                        // variable
{
    HRESULT        hr             = NOERROR;
    HRESULT        hrT            = NOERROR;
    SCODE          sc             = 0;
    LPSTREAM       lpStream       = NULL;
    HFILE          hFile          = HFILE_ERROR;
    OFSTRUCT       ofStruct       = {0};
    DWORD          dwBytesRead    = 0;
    LPBYTE         lpbBlock       = NULL;
    ULONG          ulBytesWritten = 0;
    ULARGE_INTEGER ll             = {0,0};
    ULONG          ulFileSize     = 0;
    BYTE           bLastByte      = 0xFF;
    ULONG          cbProp         = 0;

    DEBUGPUBLIC("HrMAPISetPropFromFile()\n");

    hr = CHK_HrMAPISetPropFromFile(
        lpObj,
        ulPropTag,
        lpszFilename,
        lpcbProp);

    if(FAILED(hr))
        RETURN(hr);

    *lpcbProp = 0;

    // Open a stream on the property
    hrT = MAPICALL(lpObj)->OpenProperty(
        /*lpObj,*/
        ulPropTag,
        (LPIID)&IID_IStream,
        STGM_DIRECT | STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
        MAPI_CREATE | MAPI_MODIFY | MAPI_DEFERRED_ERRORS,
        (LPUNKNOWN *)&lpStream);

    if(FAILED(hrT))
    {
        // Streams are not supported by provider
        if((hrT == MAPI_E_NO_SUPPORT) || (hrT == MAPI_E_INTERFACE_NOT_SUPPORTED))
        {
            lpStream = NULL;
        }
        else
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    hFile = OpenFile(
        lpszFilename,
        &ofStruct,
        OF_READ);

    if(hFile == HFILE_ERROR)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get file size
    if((ulFileSize = GetFileSize((HANDLE)hFile, NULL)) == (DWORD)HFILE_ERROR)
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    if(PROP_TYPE(ulPropTag) == PT_UNICODE)
    {
        if((ulFileSize % sizeof(wchar_t)) != 0)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }
    }

    cbProp = ulFileSize;

    //  Copy propery value to the file
    if(lpStream != NULL)
    {
        // Allocate memory for the block buffer
        sc = MAPIAllocateBuffer(EDK_CBTRANSFER, (void **)&lpbBlock);

        // An error occured allocating the block buffer
        if(FAILED(sc))
        {
            hr = HR_LOG(E_OUTOFMEMORY);
            goto cleanup;
        }

        ll.LowPart  = ulFileSize;
        ll.HighPart = 0L;

        hrT = /*OLECALL*/(lpStream)->SetSize(/*lpStream,*/ ll);

        if(FAILED(hrT))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }

        for (;;)
        {
            BOOL    fStatus;

            // Read a block from the file
            fStatus = ReadFile(
                (HANDLE)hFile,
                lpbBlock,
                EDK_CBTRANSFER,
                &dwBytesRead,
                NULL);

            if(fStatus == FALSE)
            {
                hr = HR_LOG(E_FAIL);
                goto cleanup;
            }

            if(dwBytesRead == 0L)
                break;

            bLastByte = lpbBlock[dwBytesRead - 1L];

            // Write a block to the stream
            hrT = /*OLECALL*/(lpStream)->Write(
                /*lpStream,*/
                lpbBlock,
                dwBytesRead,
                &ulBytesWritten);

            if(FAILED(hrT))
            {
                hr = HR_LOG(E_FAIL);
                goto cleanup;
            }

            if(ulBytesWritten < dwBytesRead)
            {
                hr = HR_LOG(MAPI_E_NOT_ENOUGH_DISK);
                goto cleanup;
            }
        }

        if((PROP_TYPE(ulPropTag) == PT_STRING8) ||
           (PROP_TYPE(ulPropTag) == PT_UNICODE))
        {
            // NULL terminate if not already
            if(bLastByte != 0)
            {
                // Initialize with enough zeroes for a NULL character
                ZeroMemory(lpbBlock, sizeof(wchar_t));

                if(PROP_TYPE(ulPropTag) == PT_UNICODE)
                {
                    dwBytesRead = sizeof(wchar_t);
                }
                else
                {
                    dwBytesRead = 1L;
                }

                ulBytesWritten = 0L;

                // Write a block to the stream
                hrT = /*OLECALL*/(lpStream)->Write(
                    /*lpStream,*/
                    lpbBlock,
                    dwBytesRead,
                    &ulBytesWritten);

                if(FAILED(hrT))
                {
                    hr = HR_LOG(E_FAIL);
                    goto cleanup;
                }

                if(ulBytesWritten < dwBytesRead)
                {
                    hr = HR_LOG(MAPI_E_NOT_ENOUGH_DISK);
                    goto cleanup;
                }

                cbProp += ulBytesWritten;
            }
        }
    }
    else
    {
        BOOL  fStatus  = FALSE;
        ULONG PropType = 0;

        // Allocate the memory for the property value
        sc = MAPIAllocateBuffer(
            ulFileSize + 2 * sizeof(wchar_t),
            (void **)&lpbBlock);

        // An error occured allocating the block buffer
        if(FAILED(sc))
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }

        // Read the property value into memory
        fStatus = ReadFile(
            (HANDLE)hFile,
            lpbBlock,
            ulFileSize,
            &dwBytesRead,
            NULL);

        if(fStatus == FALSE)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }

        // Check if the entire file was read
        if(dwBytesRead != ulFileSize)
        {
            hr = HR_LOG(E_FAIL);
            goto cleanup;
        }

        if((PROP_TYPE(ulPropTag) == PT_STRING8) ||
           (PROP_TYPE(ulPropTag) == PT_UNICODE))
        {
            // NULL terminate if not already
            bLastByte = lpbBlock[dwBytesRead - 1L];

            if(bLastByte != 0)
            {
                if(PROP_TYPE(ulPropTag) == PT_UNICODE)
                {
                    ((wchar_t *)lpbBlock)[dwBytesRead/sizeof(wchar_t)] = '\0';
                    ulFileSize += sizeof(wchar_t);
                }
                else
                {
                    lpbBlock[dwBytesRead] = 0;
                    ulFileSize++;
                }

                cbProp = ulFileSize;
            }
        }

        PropType = PROP_TYPE(ulPropTag);

        // Set property
        switch(PropType)
        {
        case PT_BINARY:
            hr = HrMAPISetPropBinary(
                lpObj,
                ulPropTag,
                ulFileSize,
                &lpbBlock);
            break;
        default:
            hr = HrMAPISetPropString(
                lpObj,
                ulPropTag,
                &lpbBlock);
        }
    }


cleanup:

    // Close the file
    if(hFile != HFILE_ERROR)
    {
        if(CloseHandle((HANDLE)hFile) == FALSE)
        {
            hr = HR_LOG(E_FAIL);
        }
    }

    // Release the stream
    //ULOLERELEASE(lpStream);
	if(lpStream != NULL)
	{
		lpStream->Release();
	}
	lpStream = NULL;  

    MAPIFREEBUFFER(lpbBlock);

    if(SUCCEEDED(hr))
    {
        *lpcbProp = cbProp;
    }

    RETURN(hr);
}

//$--HrMAPIOpenStreamOnProperty-------------------------------------------------
//  Open a stream on a given property.
// -----------------------------------------------------------------------------
HRESULT HrMAPIOpenStreamOnProperty(      // RETURNS: return code
    IN LPMAPIPROP lpObj,                // pointer to object
    IN ULONG ulPropTag,                 // property tag
    IN ULONG ulFlags,                   // flags (MAPI_CREATE and/or MAPI_MODIFY)
    OUT LPSTREAM *lppStream)            // pointer to stream address variable
{
    HRESULT  hr          = NOERROR;
    HRESULT  hrT         = NOERROR;
    LPSTREAM lpStream    = NULL;
    ULONG    ulStgmFlags = STGM_READ;

    DEBUGPUBLIC("HrMAPIOpenStreamOnProperty()\n");

    hr = CHK_HrMAPIOpenStreamOnProperty(
        lpObj,
        ulPropTag,
        ulFlags,
        lppStream);

    if(FAILED(hr))
        RETURN(hr);

    *lppStream = NULL;

    if(ulFlags & MAPI_CREATE)
    {
        ulStgmFlags |= STGM_CREATE;
    }

    if(ulFlags & MAPI_MODIFY)
    {
        ulStgmFlags |= STGM_WRITE;
    }

    hrT = MAPICALL(lpObj)->OpenProperty(
        /*lpObj,*/
        ulPropTag,
        (LPIID)&IID_IStream,
        STGM_DIRECT | STGM_SHARE_EXCLUSIVE | ulStgmFlags,
        MAPI_DEFERRED_ERRORS | ulFlags,
        (LPUNKNOWN *)&lpStream);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    *lppStream = lpStream;

cleanup:

    RETURN(hr);
}

//$--HrMAPIAppendSPropValues-----------------------------------------------------
//  Append one set of SPropValue's to another.
// -----------------------------------------------------------------------------
HRESULT HrMAPIAppendSPropValues(         // RETURNS: return code
    IN ULONG cHeadProps,                // count of property values in head
    IN LPSPropValue lpHeadProps,        // pointer to property values in
                                        // head
    IN ULONG cTailProps,                // count of property values in tail
    IN LPSPropValue lpTailProps,        // pointer to property values in
                                        // tail
    OUT ULONG *lpcNewProps,             // pointer to count of property
                                        // values
    OUT LPSPropValue *lppNewProps)      // pointer to property values 
{
    HRESULT         hr                  = NOERROR;
    SCODE           sc                  = 0;
    ULONG           cNewProps           = 0;
    LPSPropValue    lpTmpProps          = NULL;
    LPSPropValue    lpNewProps          = NULL;
    ULONG           cBytes              = 0;
    ULONG           i                   = 0;
    ULONG           j                   = 0;

    DEBUGPUBLIC("HrMAPIAppendSPropValues()\n");

    hr = CHK_HrMAPIAppendSPropValues(
        cHeadProps,
        lpHeadProps,
        cTailProps,
        lpTailProps,
        lpcNewProps,
        lppNewProps);

    if(FAILED(hr))
        RETURN(hr);

    *lpcNewProps = 0;
    *lppNewProps = NULL;

    cNewProps = cHeadProps + cTailProps;

    cBytes = CbSPropValue(cNewProps);

    sc = MAPIAllocateBuffer(cBytes, (void **)&lpTmpProps);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Copy existing property values
    for(i = 0; i < cHeadProps; i++)
    {
        lpTmpProps[i] = lpHeadProps[i];
    }

    for(i = cHeadProps, j = 0; i < cNewProps; i++, j++)
    {
        lpTmpProps[i] = lpTailProps[j];
    }

    sc = ScDupPropset(
        cNewProps,
        lpTmpProps,
		MAPIAllocateBuffer,
		&lpNewProps);

    if(FAILED(sc))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    *lpcNewProps = cNewProps;
    *lppNewProps = lpNewProps;

cleanup:

    MAPIFREEBUFFER(lpTmpProps);

    RETURN(hr);
}

//$--HrMAPIMoveOneProp-----------------------------------------------------------
//  Move one property from a source object to a destination object.
// -----------------------------------------------------------------------------
HRESULT HrMAPIMoveOneProp(               // RETURNS: return code
    IN LPMAPIPROP lpSrcObj,             // pointer to source object
    IN ULONG ulSrcPropTag,              // source property tag
    IN ULONG ulDstPropTag,              // destination property tag
    IN BOOL IsMust,                     // TRUE if a required property
    IN BOOL IsReplace,                  // TRUE if existing destination
                                        // property can be replaced
    IN OUT LPMAPIPROP lpDstObj)         // pointer to destination object
{
    HRESULT         hr                  = NOERROR;
    HRESULT         hrT                 = NOERROR;
    SCODE           sc                  = 0;
    ULONG           cProps              = 0;
    LPSPropValue    lpProps             = NULL;
    SizedSPropTagArray(1, rgPropTag)    = { 1, 0 };

    DEBUGPUBLIC("HrMAPIMoveOneProp()\n");

    hr = CHK_HrMAPIMoveOneProp(
        lpSrcObj,
        ulSrcPropTag,
        ulDstPropTag,
        IsMust,
        IsReplace,
        lpDstObj);

    if(FAILED(hr))
        RETURN(hr);

    if(PROP_TYPE(ulSrcPropTag) != PROP_TYPE(ulDstPropTag))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    if((IsReplace == FALSE) && FPropExists(lpDstObj, ulDstPropTag))
    {
        MODULE_WARNING("Destination property exists and not overwritten.");

        goto cleanup;
    }

    rgPropTag.cValues = 1;
    rgPropTag.aulPropTag[0] = ulSrcPropTag;

    hrT = MAPICALL(lpSrcObj)->GetProps(
        /*lpSrcObj,*/
        (LPSPropTagArray)&rgPropTag,
        fMapiUnicode,
        &cProps,
        &lpProps);

    if(hrT == MAPI_W_ERRORS_RETURNED)
    {
        hrT = lpProps->Value.ul;

        if(hrT != MAPI_E_NOT_FOUND)
        {
            hr = HR_LOG(E_FAIL);
        }
        else if(IsMust == TRUE)
        {
            hr = HR_LOG(MAPI_E_NOT_FOUND);
        }

        goto cleanup;
    }

    if(FAILED(hrT))
    {
        lpProps = NULL;

        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(cProps != 0, "ZERO cProps variable");

    ASSERTERROR(lpProps != NULL, "NULL lpProps variable");

    lpProps->ulPropTag = ulDstPropTag;

    hrT = MAPICALL(lpDstObj)->SetProps(
        /*lpDstObj,*/
        cProps,
        lpProps,
        NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }
         
cleanup:

    MAPIFREEBUFFER(lpProps);

    RETURN(hr);
}
