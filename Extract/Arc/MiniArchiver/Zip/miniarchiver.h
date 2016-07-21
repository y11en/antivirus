// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  14 March 2003,  14:45 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- MiniArchiver ZIP Plugin
// Author      -- Rubin
// File Name   -- miniarchiver.h
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __miniarchiver_h__297e8c4a_9de0_4585_a60e_569dc2104298 )
#define __miniarchiver_h__297e8c4a_9de0_4585_a60e_569dc2104298
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_os.h>

#include <Extract/iface/i_minarc.h>
#include <Extract/iface/i_uacall.h>
#include <Extract/plugin/p_minizip.h>

#include "ZIP.h"

#include "Hash/MD5/plugin_hash_md5.h"
#include <deflate/deflate_t.h>
#include <Explode/explode_t.h>
#include <Inflate/inflate_t.h>
#include <stored/stored_t.h>
#include <Unreduce/unreduce_t.h>
#include <Unshrink/unshrink_t.h>
#include <UnStored/unstored_t.h>

#include <Dmap/plugin_dmap.h>
#include <passdmap/dmap.h>
#include <SuperIO/plugin_superio.h>
#include <SuperIO/superio.h>
#include "hashutil/hashutil.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
    tERROR pr_call MiniArchiver_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface MiniArchiver. Inner data structure

typedef struct tag_ZIPData 
{
    tBYTE*        Window;
    tDWORD        WindowSize;
    tUINT         LastMethod;
    hTRANSFORMER  hTranformer;
    tDWORD        Keys[3];
    hSTRING       hPassword;
    tBOOL         Decrypt;
    tDWORD        WrtCrc32;
    tCHAR*        Password;
    tBYTE*        IoBuff;
    tINT          ResetMeth;

    hSEQ_IO       hSeqIO;
    hSEQ_IO       hCDirSeqIO;          /* special central dir reader */
    hHASH         hMD5;

    tBOOL     bIsReadOnly;                  // --
    tDWORD    dwOrigin;                     // --
    tDWORD    dwAvail;                      // --
    tBYTE     bFillChar;                    // --
    tWORD     wDelimiter;                   // --
    tDWORD    dwOpenMode;                   // --
    tDWORD    dwAccessMode;                 // --
    tBOOL     bMultiVolAsSingleSet;         // --
    tCHAR     szName[MAX_FILENAME_LEN];     // --
    tCHAR     szPath[MAX_FILENAME_LEN];     // --
    tBOOL     bIsFolder;                    // --
    tBOOL     bIsBrowsingDir;               // --
    tBOOL     bWarnCorrupted;               // --
    hIO       hArcIO;                       // --
    tQWORD    qwStartOffset;                // --
    tQWORD    qwCurPos;                     // --
    ZIPEntry* lpDir;                        // --
    tDWORD    dwFiles;                      // --
    hHEAP     hLocalHeap;                   // --
    tCHAR     szPassword[MAX_PASSWORD_LEN]; // --
} ZIPData, tZIP_DATA, *pZIP_DATA;


// to get pointer to the structure ZIPData from the hOBJECT obj
#define CUST_TO_ZIPData(object)  ((ZIPData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // miniarchiver_h
// AVP Prague stamp end



