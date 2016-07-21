// --edkmapi.c------------------------------------------------------------------
// 
//  Module containing MAPI utility functions.
// 
// Copyright (C) Microsoft Corp. 1986 - 2000.  All Rights Reserved.
// -----------------------------------------------------------------------------
#include "stdafx.h"

#include "edk.h"
#include "MAPIedkDef.h"

#include "edkmapi.chk"

//$--HrMAPIGetFirstSRowSet-------------------------------------------------------
//  Gets the first SRowSet from a table
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetFirstSRowSet(           // RETURNS: return code
    IN LPMAPITABLE  lpTable,            // pointer to table address variable
    IN ULONG cRows,                     // count of number of rows in SRowSet
    IN LPSPropTagArray rgPropTags,      // array of property tags
    OUT LPSRowSet FAR *lppRows)         // pointer to address variable for
                                        // SRowSet
{
    HRESULT hr  = NOERROR;
    HRESULT hrT = NOERROR;

    DEBUGPUBLIC("HrMAPIGetFirstSRowSet()\n");

    hr = CHK_HrMAPIGetFirstSRowSet(
        lpTable,
        cRows,
        rgPropTags,
        lppRows);

    if(FAILED(hr))
        RETURN(hr);

    *lppRows = NULL;

    // Set the columns to return
    hrT = MAPICALL(lpTable)->SetColumns(/*lpTable,*/ rgPropTags, (ULONG)0);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Go to the beginning of the table
    hrT = MAPICALL(lpTable)->SeekRow(/*lpTable,*/ BOOKMARK_BEGINNING, 0, NULL);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get the next SRowSet
    hrT = MAPICALL(lpTable)->QueryRows(
        /*lpTable,*/
        cRows,
        (ULONG)0,
        lppRows);

    if(SUCCEEDED(hrT) && ((*lppRows) != NULL) && ((*lppRows)->cRows == 0))
    {
        FREEPROWS(*lppRows);
		
        hrT = EDK_E_NOT_FOUND;
    }

    if(FAILED(hrT) || ((*lppRows) == NULL))
    {
        if (hrT != EDK_E_NOT_FOUND)
        {
            hr = HR_LOG(E_FAIL);
        }
        else
        {
            hr = HR_LOG(EDK_E_END_OF_FILE);
        }
    }

cleanup:

    RETURN(hr);
}

//$--HrMAPIGetNextSRowSet--------------------------------------------------------
//  Gets the next SRowSet from a table
// -----------------------------------------------------------------------------
HRESULT HrMAPIGetNextSRowSet(            // RETURNS: return code
    IN LPMAPITABLE lpTable,             // pointer to table
    IN ULONG cRows,                     // count of number of rows in SRowSet
    IN LPSPropTagArray rgPropTags,      // array of property tags
    OUT LPSRowSet FAR *lppRows)         // pointer to address variable for
                                        // SRowSet
{
    HRESULT hr  = NOERROR;
    HRESULT hrT = NOERROR;

    DEBUGPUBLIC("HrMAPIGetNextSRowSet()\n");

    hr = CHK_HrMAPIGetNextSRowSet(
        lpTable,
        cRows,
        rgPropTags,
        lppRows);

    if(FAILED(hr))
        RETURN(hr);

    *lppRows = NULL;

    // Set the columns to return
    hrT = MAPICALL(lpTable)->SetColumns(/*lpTable,*/ rgPropTags, (ULONG)0);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    // Get the next SRowSet
    hrT = MAPICALL(lpTable)->QueryRows(
        /*lpTable,*/
        cRows,
        (ULONG)0,
        lppRows);

    if(SUCCEEDED(hrT) && ((*lppRows) != NULL) && ((*lppRows)->cRows == 0))
    {
        FREEPROWS(*lppRows);
		
        hrT = EDK_E_NOT_FOUND;
    }

    if(FAILED(hrT) || ((*lppRows) == NULL))
    {
        if (hrT != EDK_E_NOT_FOUND)
        {
            hr = HR_LOG(E_FAIL);
        }
        else
        {
            hr = HR_LOG(EDK_E_END_OF_FILE);
        }
    }

cleanup:

    RETURN(hr);
}

//$--HrMAPICreateEntryList-------------------------------------------------------
//  Creates an ENTRYLIST. 
// -----------------------------------------------------------------------------
HRESULT HrMAPICreateEntryList(           // RETURNS: return code
    IN ULONG cbeid,                     // count of bytes in Entry ID
    IN LPENTRYID lpeid,                 // pointer to Entry ID
    OUT LPENTRYLIST FAR *lppEntryList)  // pointer to address variable of Entry
                                        // list
{
    HRESULT hr              = NOERROR;
    SCODE   sc              = 0;
    LPVOID  lpvSBinaryArray = NULL;
    LPVOID  lpvSBinary      = NULL;
    LPVOID  lpv             = NULL;

    DEBUGPUBLIC("HrMAPICreateEntryList()\n");

    hr = CHK_HrMAPICreateEntryList(
        cbeid,
        lpeid,
        lppEntryList);

    if(FAILED(hr))
        RETURN(hr);

    *lppEntryList = NULL;

    sc = MAPIAllocateBuffer(cbeid, &lpv);                  

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Copy entry ID
    CopyMemory(lpv, lpeid, cbeid);

    sc = MAPIAllocateBuffer(sizeof(SBinary), &lpvSBinary);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Initialize SBinary structure
    ZeroMemory(lpvSBinary, sizeof(SBinary));

    ((LPSBinary)lpvSBinary)->cb = cbeid;
    ((LPSBinary)lpvSBinary)->lpb = (LPBYTE)lpv;

    sc = MAPIAllocateBuffer(sizeof(SBinaryArray), &lpvSBinaryArray);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Initialize SBinaryArray structure
    ZeroMemory(lpvSBinaryArray, sizeof(SBinaryArray));

    ((SBinaryArray *)lpvSBinaryArray)->cValues = 1;
    ((SBinaryArray *)lpvSBinaryArray)->lpbin = (LPSBinary)lpvSBinary;

    *lppEntryList = (LPENTRYLIST)lpvSBinaryArray;

cleanup:

    if(FAILED(hr))
    {
        MAPIFREEBUFFER(lpv);

        MAPIFREEBUFFER(lpvSBinary);

        MAPIFREEBUFFER(lpvSBinaryArray);
    }

    RETURN(hr);
}

//$--HrMAPIAppendEntryList-------------------------------------------------------
//  Appends to an ENTRYLIST.
// -----------------------------------------------------------------------------
HRESULT HrMAPIAppendEntryList(           // RETURNS: return code
    IN ULONG cbeid,                     // count of bytes in Entry ID
    IN LPENTRYID lpeid,                 // pointer to Entry ID
    OUT LPENTRYLIST FAR lpEntryList)    // pointer to address variable of Entry
                                        // list
{
    HRESULT hr          = NOERROR;
    SCODE   sc          = 0;
    LPVOID  lpvSBinary  = NULL;
    LPVOID  lpv         = NULL;
    ULONG   cValues     = 0;
    ULONG   cSBinary    = 0;
    ULONG   i           = 0;

    DEBUGPUBLIC("HrMAPIAppendEntryList()\n");

    hr = CHK_HrMAPIAppendEntryList(
        cbeid,
        lpeid,
        lpEntryList);

    if(FAILED(hr))
        RETURN(hr);

    sc = MAPIAllocateBuffer(cbeid, &lpv);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                              

    // Copy Entry ID
    CopyMemory(lpv, lpeid, cbeid);

    cValues = lpEntryList->cValues;

    cSBinary = sizeof(SBinary) * (cValues + 1);

    sc = MAPIAllocateBuffer(cSBinary, &lpvSBinary);

    if(FAILED(sc))                           
    {                                                   
        hr = HR_LOG(E_OUTOFMEMORY);                                 
        goto cleanup;
    }                                                   

    // Initialize SBinary structure
    ZeroMemory(lpvSBinary, cSBinary);

#define BINLIST ((LPSBinary)lpvSBinary)

    // Copy values from old SBinary list to new SBinary list
    for(i = 0; i < cValues; i++)
    {
        BINLIST[i].cb  = lpEntryList->lpbin[i].cb;
        BINLIST[i].lpb = lpEntryList->lpbin[i].lpb;
    }

    // Add new entry to the end of the list
    BINLIST[cValues].cb  = cbeid;
    BINLIST[cValues].lpb = (LPBYTE)lpv;

    // Free old SBinary list
    MAPIFREEBUFFER(lpEntryList->lpbin);

    // Increment number of cValues
    cValues++;

    // Update Entry list
    lpEntryList->cValues = cValues;
    lpEntryList->lpbin = BINLIST;

#undef BINLIST

cleanup:

    if(FAILED(hr))
    {
        MAPIFREEBUFFER(lpv);

        MAPIFREEBUFFER(lpvSBinary);
    }

    RETURN(hr);
}

//$--HrMAPIDestroyEntryList---------------------------------------------------------
//  Frees an ENTRYLIST. 
// -----------------------------------------------------------------------------
HRESULT HrMAPIDestroyEntryList(              // RETURNS: return code
    IN OUT LPENTRYLIST FAR *lppEntryList)// pointer to address variable of Entry
                                         // list
{
    HRESULT     hr          = NOERROR;
    ULONG       i           = 0;
    LPENTRYLIST lpEntryList = NULL;

    DEBUGPUBLIC("HrMAPIDestroyEntryList()\n");

    hr = CHK_HrMAPIDestroyEntryList(
        lppEntryList);

    if(FAILED(hr))
        RETURN(hr);

    if(lppEntryList != NULL)
    {
        lpEntryList = *lppEntryList;

        if(lpEntryList != NULL)
        {
            if(lpEntryList->lpbin != NULL)
            {
                // Free values in SBinary list
                for(i = 0; i < lpEntryList->cValues; i++)
                {
                    MAPIFREEBUFFER(lpEntryList->lpbin[i].lpb);
                }
    
                // Free SBinary list
                MAPIFREEBUFFER(lpEntryList->lpbin);
            }

            lpEntryList->cValues = 0;

            // Free SBinary array
            MAPIFREEBUFFER(lpEntryList);

            *lppEntryList = NULL;
        }
    }

    RETURN(hr);
}

//$--HrMAPIWriteStreamToFile----------------------------------------------------
//  Write stream to a file given a file handle.
// -----------------------------------------------------------------------------
HRESULT HrMAPIWriteStreamToFile(        // RETURNS: return code
    IN LPSTREAM lpStream,               // Pointer to stream
    OUT HANDLE hFile)                   // Handle to file
{
    HRESULT hr              = NOERROR;
    HRESULT hrT             = NOERROR;
    ULONG   cBytesRead      = 0;
    DWORD   cBytesWritten   = 0;
    BYTE    byteBuffer[128] = {0};
    BOOL    fWriteOk        = FALSE;

    DEBUGPUBLIC("HrMAPIWriteStreamToFile()\n");

    hr = CHK_HrMAPIWriteStreamToFile(
        lpStream,
        hFile);

    if(FAILED(hr))
        RETURN(hr);

    for(;;)
    {
        hrT = /*OLECALL*/(lpStream)->Read(
            /*lpStream,*/
            byteBuffer, 
            sizeof(byteBuffer),
            &cBytesRead);

        if(FAILED(hrT))
        {
            hr = HR_LOG(E_FAIL);
            break;
        }

        if (cBytesRead == 0)
        {
            hr = NOERROR;
            break ;    
        }

        fWriteOk = WriteFile(
            hFile,
            byteBuffer,
            cBytesRead,
            &cBytesWritten,
            NULL);

        if(fWriteOk == FALSE)
        {
            hr = HR_LOG(E_FAIL);
            break;
        }

        if(cBytesWritten != cBytesRead)
        {
            hr = HR_LOG(MAPI_E_NOT_ENOUGH_DISK);
            break;
        }
    }

    RETURN(hr);
}


//$--HrMAPIWriteFileToStream----------------------------------------------------
//  Write file to a stream given a stream pointer.
// -----------------------------------------------------------------------------
HRESULT HrMAPIWriteFileToStream(        // RETURNS: return code
    IN HANDLE hFile,                    // Handle to file
    OUT LPSTREAM lpStream)              // Pointer to stream
{
    HRESULT hr              = NOERROR;
    HRESULT hrT             = NOERROR;
    DWORD   cBytesRead      = 0;
    ULONG   cBytesWritten   = 0;
    BYTE    byteBuffer[128] = {0};
    BOOL    fReadOk         = FALSE;

    DEBUGPUBLIC("HrMAPIWriteFileToStream()\n");

    hr = CHK_HrMAPIWriteFileToStream(
        hFile,
        lpStream);

    if(FAILED(hr))
        RETURN(hr);

    for(;;)
    {
        fReadOk = ReadFile(
            hFile,
            byteBuffer,
            sizeof(byteBuffer),
            &cBytesRead,
            NULL);

        if(fReadOk == FALSE)
        {
            hr = HR_LOG(E_FAIL);
            break;
        }

        if(cBytesRead == 0)
        {
            hr = NOERROR;
            break;
        }

        hrT = /*OLECALL*/(lpStream)->Write(
            /*lpStream,*/
            byteBuffer, 
            cBytesRead,
            &cBytesWritten);

        if(FAILED(hrT))
        {
            hr = HR_LOG(E_FAIL);
            break;
        }
        
        if(cBytesWritten != cBytesRead)
        {
            hr = HR_LOG(MAPI_E_NOT_ENOUGH_DISK);
            break;
        }
    }

    RETURN(hr);
}


//$--HrMAPIWriteAttachmentToFile-----------------------------------------------
//  Write the identified message attachment to a file.
// -----------------------------------------------------------------------------
HRESULT HrMAPIWriteAttachmentToFile(   // RETURNS: return code
    IN LPMESSAGE lpMessage,             // message containing the attachments
    IN ULONG iAttach,                   // attachment identifier
    OUT HANDLE hFile)                   // handle to file
{
    HRESULT  hr          = NOERROR;
    HRESULT  hrT         = NOERROR;
    LPATTACH lpAttach    = NULL;
    LPSTREAM lpAttStream = NULL;

    DEBUGPUBLIC("HrMAPIWriteAttachmentToFile()\n");

    hr = CHK_HrMAPIWriteAttachmentToFile(
        lpMessage,
        iAttach,
        hFile);

    if(FAILED(hr))
        RETURN(hr);

    //
    //  Open the attachment.
    //

    hrT = MAPICALL(lpMessage)->OpenAttach(
        /*lpMessage,*/
        iAttach,
        NULL,
        MAPI_DEFERRED_ERRORS,
        &lpAttach);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    ASSERTERROR(lpAttach != NULL, "NULL lpAttach variable");

    //
    //  Get a stream interface on the attachment
    //

    hrT = MAPICALL(lpAttach)->OpenProperty(
        /*lpAttach,*/
        PR_ATTACH_DATA_BIN,
        (LPIID)&IID_IStream, 
        STGM_READ | STGM_DIRECT | STGM_SHARE_EXCLUSIVE,
        MAPI_DEFERRED_ERRORS,
        (LPUNKNOWN *)&lpAttStream);

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

        goto cleanup;
    }

    //
    //  Copy the attachment into a file.
    //

    hrT = HrMAPIWriteStreamToFile(lpAttStream, hFile);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    if(lpAttStream != NULL)
        /*OLECALL*/(lpAttStream)->Release(/*lpAttStream*/);

    if(lpAttach != NULL)
        MAPICALL(lpAttach)->Release(/*lpAttach*/);

    RETURN(hr);
}

//$--HrMAPIGotoSRow--------------------------------------------------------------
//  Goto the specified SRow in an SRowSet.
//------------------------------------------------------------------------------
HRESULT HrMAPIGotoSRow(                  // RETURNS: return code
    IN LPSRowSet FAR lpRows,            // pointer to SRowSet
    IN ULONG ulRow,                     // index of SRow in SRowSet
    OUT LPSRow *lppRow)                 // pointer to SRow
{
    HRESULT hr = NOERROR;

    DEBUGPUBLIC("HrMAPIGotoSRow()\n");

    hr = CHK_HrMAPIGotoSRow(
        lpRows,
        ulRow,
        lppRow);

    if(FAILED(hr))
        RETURN(hr);

    *lppRow = NULL;

    if(ulRow < lpRows->cRows)
    {
        *lppRow = &(lpRows->aRow[ulRow]);
    }
    else
    {
        hr = HR_LOG(EDK_E_NOT_FOUND);
    }

    RETURN(hr);
}

//$--HrMAPIGotoFirstSRow---------------------------------------------------------
//  Goto the first SRow in an SRowSet.
//------------------------------------------------------------------------------
HRESULT HrMAPIGotoFirstSRow(             // RETURNS: return code
    IN LPSRowSet FAR lpRows,            // pointer to SRowSet
    OUT ULONG *lpulRow,                 // index of SRow in SRowSet
    OUT LPSRow *lppRow)                 // pointer to SRow
{
    HRESULT hr = NOERROR;

    DEBUGPUBLIC("HrMAPIGotoFirstSRow()\n");

    hr = CHK_HrMAPIGotoFirstSRow(
        lpRows,
        lpulRow,
        lppRow);

    if(FAILED(hr))
        RETURN(hr);

    *lpulRow = 0;

    hr = HrMAPIGotoSRow(
        lpRows,
        0,
        lppRow);

    RETURN(hr);
}

//$--HrMAPIGotoNextSRow----------------------------------------------------------
//  Goto the next SRow in an SRowSet.
//------------------------------------------------------------------------------
HRESULT HrMAPIGotoNextSRow(              // RETURNS: return code
    IN LPSRowSet FAR lpRows,            // pointer to SRowSet
    IN OUT ULONG *lpulRow,              // index of SRow in SRowSet
    OUT LPSRow *lppRow)                 // pointer to SRow
{
    HRESULT hr    = NOERROR;
    ULONG   ulRow = 0;

    DEBUGPUBLIC("HrMAPIGotoNextSRow()\n");

    hr = CHK_HrMAPIGotoNextSRow(
        lpRows,
        lpulRow,
        lppRow);

    if(FAILED(hr))
        RETURN(hr);

    ulRow = *lpulRow;

    ulRow++;

    *lpulRow = ulRow;

    hr = HrMAPIGotoSRow(
        lpRows,
        ulRow,
        lppRow);

    RETURN(hr);
}

//$--HrMAPIWriteStreamToMemory----------------------------------------------------
//  Reads a given number of bytes from a stream to a block of memory.
// -----------------------------------------------------------------------------
HRESULT HrMAPIWriteStreamToMemory(        // RETURNS: return code
    IN LPSTREAM lpStream,               // pointer to stream
    IN ULONG cBytes,                    // count of bytes in memory
    IN LPBYTE lpbBytes,                 // pointer to memory
    OUT ULONG *lpcBytesRead)            // count of bytes read from stream
{
    HRESULT hr  = NOERROR;
    HRESULT hrT = NOERROR;

    DEBUGPUBLIC("HrMAPIWriteStreamToMemory()\n");

    hr = CHK_HrMAPIWriteStreamToMemory(
        lpStream,
        cBytes,
        lpbBytes,
        lpcBytesRead);

    if(FAILED(hr))
        RETURN(hr);

    *lpcBytesRead = 0;

    // Read from the stream
    hrT = /*OLECALL*/(lpStream)->Read(
        /*lpStream,*/
        lpbBytes,
        cBytes,
        lpcBytesRead);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrMAPIWriteMemoryToStream---------------------------------------------------
//  Writes a given number of bytes from a block of memory to a stream
// -----------------------------------------------------------------------------
HRESULT HrMAPIWriteMemoryToStream(       // RETURNS: return code
    IN LPSTREAM lpStream,               // pointer to stream
    IN ULONG cBytes,                    // count of bytes in memory
    IN LPBYTE lpbBytes,                 // pointer to memory
    OUT ULONG *lpcBytesWritten)         // count of bytes written from stream
{
    HRESULT hr  = NOERROR;
    HRESULT hrT = NOERROR;

    DEBUGPUBLIC("HrMAPIWriteMemoryToStream()\n");

    hr = CHK_HrMAPIWriteMemoryToStream(
        lpStream,
        cBytes,
        lpbBytes,
        lpcBytesWritten);

    if(FAILED(hr))
        RETURN(hr);

    *lpcBytesWritten = 0;

    // Write a block to the stream
    hrT = /*OLECALL*/(lpStream)->Write(
        /*lpStream,*/
        lpbBytes,
        cBytes,
        lpcBytesWritten);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

    if((*lpcBytesWritten) < cBytes)
    {
        hr = HR_LOG(MAPI_E_NOT_ENOUGH_DISK);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}

//$--HrMAPISetStreamSize---------------------------------------------------------
//  Sets the size of the given stream.
// -----------------------------------------------------------------------------
HRESULT HrMAPISetStreamSize(             // RETURNS: return code
    IN LPSTREAM lpStream,               // pointer to stream
    IN ULONG cBytes)                    // count of bytes in stream
{
    HRESULT        hr  = NOERROR;
    HRESULT        hrT = NOERROR;
    ULARGE_INTEGER ll  = {0};

    DEBUGPUBLIC("HrMAPISetStreamSize()\n");

    hr = CHK_HrMAPISetStreamSize(
        lpStream,
        cBytes);

    if(FAILED(hr))
        RETURN(hr);

    ll.LowPart  = cBytes;
    ll.HighPart = 0;

    hrT = /*OLECALL*/(lpStream)->SetSize(/*lpStream,*/ ll);

    if(FAILED(hrT))
    {
        hr = HR_LOG(E_FAIL);
        goto cleanup;
    }

cleanup:

    RETURN(hr);
}
