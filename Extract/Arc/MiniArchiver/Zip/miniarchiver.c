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
// File Name   -- miniarchiver.c
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define MiniArchiver_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>

#include "miniarchiver.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure "ZIPData" is described in "O:\Prague\MiniArchiver\Zip\miniarchiver.h" header file
// Interface MiniArchiver. Inner data structure
//! } ZIPData;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_MiniArchiver 
{
    const iMiniArchiverVtbl* vtbl; // pointer to the "MiniArchiver" virtual table
    const iSYSTEMVTBL*       sys;  // pointer to the "SYSTEM" virtual table
    ZIPData*                 data; // pointer to the "MiniArchiver" data structure
} *hi_MiniArchiver;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call MiniArchiver_Recognize( hOBJECT _that );
tERROR pr_call MiniArchiver_ObjectInit( hi_MiniArchiver _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_MiniArchiver_vtbl = 
{
    (tIntFnRecognize)        MiniArchiver_Recognize,
    (tIntFnObjectNew)        NULL,
    (tIntFnObjectInit)       MiniArchiver_ObjectInit,
    (tIntFnObjectInitDone)   NULL,
    (tIntFnObjectCheck)      NULL,
    (tIntFnObjectPreClose)   NULL,
    (tIntFnObjectClose)      NULL,
    (tIntFnChildNew)         NULL,
    (tIntFnChildInitDone)    NULL,
    (tIntFnChildClose)       NULL,
    (tIntFnMsgReceive)       NULL,
    (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpMiniArchiver_Reset)          ( hi_MiniArchiver _this );             // -- First call of Next method after Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_Next)           ( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId ); // -- First call of Next method after creating or Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_ObjCreate)      ( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO ); // -- Create IO from current enumerated object;
typedef   tERROR (pr_call *fnpMiniArchiver_SetAsCurrent)   ( hi_MiniArchiver _this, tQWORD qwObjectId ); // -- Set Object (qwObjectId) as currently enumerated object;
typedef   tERROR (pr_call *fnpMiniArchiver_RebuildArchive) ( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo ); // -- ;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iMiniArchiverVtbl 
{
    fnpMiniArchiver_Reset           Reset;
    fnpMiniArchiver_Next            Next;
    fnpMiniArchiver_ObjCreate       ObjCreate;
    fnpMiniArchiver_SetAsCurrent    SetAsCurrent;
    fnpMiniArchiver_RebuildArchive  RebuildArchive;
}; // "MiniArchiver" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call MiniArchiver_Reset( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId );
tERROR pr_call MiniArchiver_ObjCreate( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO );
tERROR pr_call MiniArchiver_SetAsCurrent( hi_MiniArchiver _this, tQWORD qwObjectId );
tERROR pr_call MiniArchiver_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iMiniArchiverVtbl e_MiniArchiver_vtbl = 
{
    MiniArchiver_Reset,
    MiniArchiver_Next,
    MiniArchiver_ObjCreate,
    MiniArchiver_SetAsCurrent,
    MiniArchiver_RebuildArchive
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call MiniArchiver_PROP_Get( hi_MiniArchiver _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call MiniArchiver_PROP_Set( hi_MiniArchiver _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MiniArchiver". Static(shared) property table variables
const tVERSION Version = 1; // must be READ_ONLY at runtime
const tSTRING szComment = "ZIP"; // must be READ_ONLY at runtime
const tBOOL IsSensitive = cFALSE; // must be READ_ONLY at runtime
const tDWORD dwMaxLen = MAX_FILENAME_LEN; // must be READ_ONLY at runtime
const tBOOL IsSolid = cFALSE; // must be READ_ONLY at runtime
// AVP Prague stamp end

#define RAND_HEAD_LEN  12  /* decryption salt size */

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(MiniArchiver_PropTable)
    mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, Version, sizeof(Version) )
    mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 4 )
    mSHARED_PROPERTY_PTR( pgNAME_CASE_SENSITIVE, IsSensitive, sizeof(IsSensitive) )
    mLOCAL_PROPERTY_FN( pgOBJECT_NAME, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_PATH, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgIS_FOLDER, MiniArchiver_PROP_Get, NULL )
    mSHARED_PROPERTY_PTR( pgNAME_MAX_LEN, dwMaxLen, sizeof(dwMaxLen) )
    mSHARED_PROPERTY_PTR( pgIS_SOLID, IsSolid, sizeof(IsSolid) )
    mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
    mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_HASH, MiniArchiver_PROP_Get, NULL )
//!     mLOCAL_PROPERTY_BUF( pgMINIARC_PROP_OBJECT_ID, ZIPData, qwCurPos, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY( pgMINIARC_PROP_OBJECT_ID, ZIPData, qwCurPos, cPROP_BUFFER_READ, MiniArchiver_PROP_Get, MiniArchiver_PROP_Set )
//!     mLOCAL_PROPERTY_BUF( pgMULTIVOL_AS_SINGLE_SET, ZIPData, bMultiVolAsSingleSet, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgMULTIVOL_AS_SINGLE_SET, ZIPData, bMultiVolAsSingleSet, cPROP_BUFFER_READ_WRITE )
//!     mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, ZIPData, dwAccessMode, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, ZIPData, dwAccessMode, cPROP_BUFFER_READ_WRITE )
//!     mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, ZIPData, dwOpenMode, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, ZIPData, dwOpenMode, cPROP_BUFFER_READ_WRITE )
//!     mLOCAL_PROPERTY_BUF( pgNAME_DELIMITER, ZIPData, wDelimiter, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgNAME_DELIMITER, ZIPData, wDelimiter, cPROP_BUFFER_READ_WRITE )
    mLOCAL_PROPERTY_BUF( pgOBJECT_FILL_CHAR, ZIPData, bFillChar, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgOBJECT_AVAILABILITY, ZIPData, dwAvail, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, ZIPData, dwOrigin, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgIS_READONLY, ZIPData, bIsReadOnly, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_FN( pgOBJECT_COMPRESSED_SIZE, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_SIGNATURE, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_COMPRESSION_METHOD, MiniArchiver_PROP_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_PASSWORD_PROTECTED, MiniArchiver_PROP_Get, NULL )
    mSHARED_PROPERTY( pgOBJECT_NAME_CP, ((tCODEPAGE)(cCP_OEM)) )
    mSHARED_PROPERTY( pgOBJECT_PATH_CP, ((tCODEPAGE)(cCP_OEM)) )
    mSHARED_PROPERTY( pgOBJECT_FULL_NAME_CP, ((tCODEPAGE)(cCP_OEM)) )
mPROPERTY_TABLE_END(MiniArchiver_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "MiniArchiver". Register function
tERROR pr_call MiniArchiver_Register( hROOT root ) 
{
    tERROR error;

    PR_TRACE_A0( root, "Enter MiniArchiver::Register method" );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_MINIARCHIVER,                       // interface identifier
        PID_MINIZIP,                            // plugin identifier
        0x00000000,                             // subtype identifier
        Version,                                // interface version
        VID_RUBIN,                              // interface developer
        &i_MiniArchiver_vtbl,                   // internal(kernel called) function table
        (sizeof(i_MiniArchiver_vtbl)/sizeof(tPTR)),// internal function table size
        &e_MiniArchiver_vtbl,                   // external function table
        (sizeof(e_MiniArchiver_vtbl)/sizeof(tPTR)),// external function table size
        MiniArchiver_PropTable,                 // property table
        mPROPERTY_TABLE_SIZE(MiniArchiver_PropTable),// property table size
        sizeof(ZIPData),                        // memory size
        0                                       // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"MiniArchiver(IID_MINIARCHIVER) registered [error=0x%08x]",error) );
    #endif // _PRAGUE_TRACE_

    PR_TRACE_A1( root, "Leave MiniArchiver::Register method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, Get )
// Interface "MiniArchiver". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- IS_FOLDER
//  -- OBJECT_SIZE_Q
//  -- OBJECT_HASH
//  -- OBJECT_COMPRESSED_SIZE
tERROR pr_call MiniArchiver_PROP_Get( hi_MiniArchiver _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    ZIPData* data = _this->data;
    tBYTE  Hash[16];
    tDWORD dwSize;
    tDWORD dwPos;

    PR_TRACE_A0( _this, "Enter *GET* multyproperty method MiniArchiver_PROP_Get" );

    switch ( prop )
    {
    case pgOBJECT_NAME:
    case pgOBJECT_PATH:
    case pgOBJECT_FULL_NAME:

        if (buffer && size)
        {
            tUINT ns;

            if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
            {
                error = errEND_OF_THE_LIST;
                *out_size = 0;
                break;
            }

            ns = _toui32(strlen( data->lpDir[data->qwCurPos].szName ));

            if ( ns+1 > size )
            {
                error = errBUFFER_TOO_SMALL;
                size = ns + 1;
            }
            else
            {
                memcpy( buffer, data->lpDir[data->qwCurPos].szName, ns+1 );
                size = ns + 1;
            }
        }
        else
        {
            size = MAX_FILENAME_LEN;
        }

        *out_size = size;
        break;

    case pgIS_FOLDER:
        if (buffer && size)
        {
            if ( size < sizeof(tBOOL) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }

                *(tBOOL*)buffer = data->lpDir[data->qwCurPos].bIsFolder;
            }
        }
        *out_size = sizeof(tBOOL);
        break;

    case pgOBJECT_SIZE_Q:
        if (buffer && size)
        {
            if ( size < sizeof(tQWORD) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }

                *(tQWORD*)buffer = (tQWORD)data->lpDir[data->qwCurPos].Header.uncompressed_size;
            }
        }
        *out_size = sizeof(tQWORD);
        break;

    case pgOBJECT_HASH:
        if (buffer && size)
        {
            if ( size < sizeof(tQWORD) )
            {
                error = errBUFFER_TOO_SMALL;
                *out_size = sizeof(tQWORD);
            }
            else
            {
                tBYTE RBuf[32];
                tUINT YeldCount;
                tUINT PBlkLen;

                if ( NULL == data->hMD5 )
                {
                  /* create hash */
                  error = CALL_SYS_ObjectCreateQuick(_this, &data->hMD5, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY);
                  if(PR_FAIL(error))
                  {
                    *out_size = 0;
                    return error;
                  }
                }
                if ( NULL == data->hSeqIO )
                {
                  /* create seq_io */
                  error = CALL_SYS_ObjectCreateQuick(data->hArcIO, &data->hSeqIO, IID_SEQIO, PID_ANY, SUBTYPE_ANY) ;
                  if(PR_FAIL(error))
                  {
                    *out_size = 0;
                    return error;
                  }
                }

                CALL_Hash_Reset(data->hMD5);
                for ( YeldCount = 0, dwPos = 0; dwPos < data->dwFiles; dwPos++ )
                {
                  /* send yeld message 1/32 */
                  if ( 0 == (YeldCount++ & 0x1f) )
                  {
                     if ( PR_FAIL(error = CALL_SYS_SendMsg(data->hArcIO, pmc_PROCESSING,
                        pm_PROCESSING_YIELD, _this, NULL, NULL)) )
                     {
                       return(error); /* stopped */
                     }
                  }

                  CALL_Hash_Update(data->hMD5, (tBYTE*)&data->lpDir[dwPos].Header, sizeof(LocalFileHeader));

                  memset(&RBuf[0], 0, sizeof(RBuf));
                  PBlkLen = data->lpDir[dwPos].Header.compressed_size;
                  dwSize  = PBlkLen < (sizeof(RBuf)/2) ? PBlkLen : (sizeof(RBuf)/2);

                  if ( dwSize > 0 )
                  {
                    /* read up to 16 bytes from stream start */
                    error = CALL_SeqIO_SeekRead(data->hSeqIO, NULL, data->lpDir[dwPos].qwPos, &RBuf[0], dwSize);
                    if (PR_FAIL(error))
                    {
                      *out_size = 0;
                      return error;
                    }
                  }
                  if ( PBlkLen > (sizeof(RBuf)/2) )
                  {
                    /* read up to 16 bytes from stream end */
                    error = CALL_SeqIO_SeekRead(data->hSeqIO, NULL, data->lpDir[dwPos].qwPos + PBlkLen - (sizeof(RBuf)/2), &RBuf[sizeof(RBuf)/2], (sizeof(RBuf)/2));
                    if (PR_FAIL(error))
                    {
                      *out_size = 0;
                      return error;
                    }
                  }

                  /* update hash context by buffer */
                  CALL_Hash_Update(data->hMD5, &RBuf[0], sizeof(RBuf));
                }

                *out_size = sizeof(tQWORD);
                CALL_Hash_GetHash(data->hMD5, &Hash[0], sizeof(Hash));
                CALL_SYS_ObjectClose(data->hMD5); data->hMD5 = NULL;
                *((tQWORD*)buffer)  = *(tQWORD*)(&Hash[0]);
                *((tQWORD*)buffer) ^= *(tQWORD*)(&Hash[8]);
            }
        }
        else
        {
            *out_size = sizeof(tQWORD);
        }
        break;
    case pgOBJECT_COMPRESSED_SIZE:
        if (buffer && size)
        {
            if ( size < sizeof(tDWORD) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }

                *(tDWORD*)buffer = (tDWORD)data->lpDir[data->qwCurPos].dwSizeOfRawData;
            }
        }
        *out_size = sizeof(tDWORD);
        break;

    case pgOBJECT_SIGNATURE:
#if 1 // by Dmitry Glavatskikh, 24.09.2004
        *out_size = 16;
        if (buffer && size)
        {
            if ( size < 16 )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }
                if ( NULL != data->hArcIO )
                {
                  // read encryption rnd salt
                  CALL_IO_SeekRead(data->hArcIO, out_size,
                    data->lpDir[data->qwCurPos].qwPos, buffer, 12);
                }
                *(tDWORD*)(&buffer[12]) = (tDWORD)data->lpDir[data->qwCurPos].Header.crc32;
            }
        }
        break;
#else
        if (buffer && size)
        {
            if ( size < sizeof(tDWORD) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }

                *(tDWORD*)buffer = (tDWORD)data->lpDir[data->qwCurPos].Header.crc32;
            }
        }
        *out_size = sizeof(tDWORD);
        break;
#endif
    case pgOBJECT_COMPRESSION_METHOD:
        if (buffer && size)
        {
            if ( size < sizeof(tDWORD) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }

                *(tDWORD*)buffer = (tDWORD)data->lpDir[data->qwCurPos].Header.compression_method;
            }
        }
        *out_size = sizeof(tDWORD);
        break;

    case pgOBJECT_PASSWORD_PROTECTED:
        if (buffer && size)
        {
            if ( size < sizeof(tBOOL) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                if (data->qwCurPos < 0 || data->qwCurPos >= data->dwFiles)
                {
                    error = errEND_OF_THE_LIST;
                    *out_size = 0;
                    break;
                }

                *(tBOOL*)buffer = data->lpDir[data->qwCurPos].Header.general_purpose_bit_flag & 1 ? cTRUE : cFALSE;
            }
        }
        *out_size = sizeof(tBOOL);
        break;

    case pgMINIARC_PROP_OBJECT_ID:
        if (buffer && size)
        {
            if ( size < sizeof(tQWORD) )
            {
                error = errBUFFER_TOO_SMALL;
            }
            else
            {
                *(tQWORD*)buffer = (tQWORD)data->qwCurPos + 1;
            }
        }
        *out_size = sizeof(tQWORD);
        break;

    default:
        *out_size = 0;
        break;
    }

    PR_TRACE_A2( _this, "Leave *GET* multyproperty method MiniArchiver_PROP_Get, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, Set )
// Interface "MiniArchiver". Method "Set" for property(s):
//  -- MINIARC_PROP_OBJECT_ID
tERROR pr_call MiniArchiver_PROP_Set( hi_MiniArchiver _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *SET* method MiniArchiver_PROP_Set for property pgMINIARC_PROP_OBJECT_ID" );

    _this->data->qwCurPos = *(tQWORD*)buffer - 1;

    *out_size = 0;

    PR_TRACE_A2( _this, "Leave *SET* method MiniArchiver_PROP_Set for property pgMINIARC_PROP_OBJECT_ID, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, Recognize )
// Extended method comment
//    Static method. Kernel calls this method wihtout any created objects of this interface.
//    Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//    Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//    It must be quick (preliminary) analysis
// Parameters are:
tERROR pr_call MiniArchiver_Recognize( hOBJECT _that )
{
    tERROR error = errINTERFACE_INCOMPATIBLE;
    LocalFileHeader lfh;
    tDWORD dwRead;

    PR_TRACE_A2( 0, "Enter MiniArchiver::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

    if(PR_SUCC(CALL_IO_SeekRead((hIO)_that, &dwRead, 0, &lfh, sizeof(lfh))))
    {
        if((tWORD)lfh.sig == 0x4b50)
        {
            if (dwRead == sizeof(lfh))
            {
                if (lfh.version_needed_to_extract <= 0x15)
                {
                    error = errOK;
                }
            }
            else if (dwRead == sizeof(EndRecord))
            {
                error = errOK;
            }
        }
    }


    PR_TRACE_A2( 0, "Leave MiniArchiver::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call MiniArchiver_ObjectInit( hi_MiniArchiver _this )
{
    tERROR error = errOBJECT_NOT_CREATED;
    ZIPData* data = _this->data;
    tBOOL  bIsBrowsingDir = cFALSE;
    tDWORD dwPosOfLocalHeader = 0;
    tQWORD qwCurPos = 0;
    tDWORD dwCurPosInEnum = 0;
    tDWORD dwCurDirSize = 16;
    tDWORD dwPos;
    tDWORD dwTmp;
    tDWORD dwSizeOfRawData;
    tDWORD dwSizeOfRawDataWithHeader;
    FileHeader DirHeader;
    LocalFileHeader LocalHeader;
    tCHAR szName[MAX_FILENAME_LEN];
    tDWORD dwTotalFiles = 0;
    tBOOL  bIsFolder = cFALSE;
    tUINT  YeldCount = 0;

    PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectInit method" );
    memset(data, 0, sizeof(ZIPData));

    data->bIsReadOnly = cFALSE;
    data->dwAvail = fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE | fAVAIL_CHANGE_MODE | fAVAIL_DELETE_ON_CLOSE;

    error = CALL_SYS_ObjectCreate((hOBJECT)_this, &data->hLocalHeap, IID_HEAP, PID_ANY,0);
    if(PR_SUCC(error))
    {
        CALL_SYS_PropertySetDWord(data->hLocalHeap, pgHEAP_GRANULARITY, 10*1024);
        if(errOK!=CALL_SYS_ObjectCreateDone(data->hLocalHeap))
        {
            error = errOBJECT_NOT_CREATED;
        }
    }

    if (PR_FAIL(error))
    {
        return error;
    }

    dwTmp = 0;
    data->hArcIO = (hIO)CALL_SYS_ParentGet(_this,IID_IO);
    if(data->hArcIO)
    {
        data->dwOpenMode   = CALL_SYS_PropertyGetDWord( (hOBJECT)data->hArcIO, pgOBJECT_OPEN_MODE);
        data->dwAccessMode = CALL_SYS_PropertyGetDWord( (hOBJECT)data->hArcIO, pgOBJECT_ACCESS_MODE);

        if ((tPID)CALL_SYS_PropertyGetDWord((hOBJECT)data->hArcIO, pgPLUGIN_ID) == DMAP_ID)
        {
            error = CALL_SYS_PropertyGet( (hOBJECT)data->hArcIO, NULL, ppMAP_AREA_START, &data->qwStartOffset, sizeof(tQWORD) );
        }

        error = (tERROR)CALL_SYS_ObjectCreateQuick((hOBJECT)data->hArcIO,&data->hSeqIO,IID_SEQIO,PID_ANY,0);
        if ( PR_FAIL(error) )
          return(error);
        error = (tERROR)CALL_SYS_ObjectCreateQuick((hOBJECT)data->hArcIO,&data->hCDirSeqIO,IID_SEQIO,PID_ANY,0);
        if ( PR_FAIL(error) )
          return(error);
    }

    error = CALL_Heap_Alloc(data->hLocalHeap, (tPTR*)&data->lpDir, dwCurDirSize*sizeof(ZIPEntry));
    if (PR_FAIL(error))
    {
        return error;
    }

    if (PR_SUCC(iFindEndHeader(data->hLocalHeap, data->hCDirSeqIO, data->qwStartOffset, &dwPos, &dwTmp)))
    {
        if (dwTmp == 0)
        {
            error = errEND_OF_THE_LIST;
        }
        else
        {
            qwCurPos = (tQWORD)dwPos;
            bIsBrowsingDir = cTRUE;
        }
    }


    while ( PR_SUCC(error) )
    {
      /* send yeld message 1/32 */
      if ( 0 == (YeldCount++ & 0x1f) )
      {
        if ( PR_FAIL(error = CALL_SYS_SendMsg(data->hArcIO, pmc_PROCESSING,
          pm_PROCESSING_YIELD, _this, NULL, NULL)) )
        {
          return(error); /* stopped */
        }
      }

        if(bIsBrowsingDir)
        {
            /* read central directory headers */
            error = CALL_SeqIO_Seek(data->hCDirSeqIO,NULL, qwCurPos, cSEEK_SET);
            error = CALL_SeqIO_Read(data->hCDirSeqIO, &dwTmp, &DirHeader,sizeof(FileHeader));
            if(PR_SUCC(error) && sizeof(FileHeader) == dwTmp)
            {
                if(DirHeader.entral_file_header_signature == HEADER_FILE_SIG)
                {
                    if(DirHeader.filename_length < sizeof(szName))
                    {
                        error = CALL_SeqIO_Read(data->hCDirSeqIO,&dwTmp, &szName, DirHeader.filename_length);
                        szName[DirHeader.filename_length] = 0;
                    
                        dwPosOfLocalHeader = (tDWORD)(DirHeader.relative_offset_of_local_header - data->qwStartOffset);//data->dwLastPos = (tDWORD)data->qwCurPos;

                        error = CALL_SeqIO_SeekRead(data->hSeqIO, &dwTmp, dwPosOfLocalHeader, &LocalHeader,sizeof(LocalFileHeader));
                        if(sizeof(LocalFileHeader) != dwTmp || PR_FAIL(error))
                        {
                            break;
                        }

                        if (LocalHeader.sig == LOCAL_HDR_SIG)
                        {
                            LocalHeader.uncompressed_size = DirHeader.uncompressed_size;
                            LocalHeader.compressed_size = DirHeader.compressed_size;
                            LocalHeader.general_purpose_bit_flag = DirHeader.general_purpose_bit_flag;
                            LocalHeader.compression_method = DirHeader.compression_method;
                            LocalHeader.crc32 = DirHeader.crc32;
                        }
                        else
                        {
                            LocalHeader.sig                       = LOCAL_HDR_SIG;
                            LocalHeader.version_needed_to_extract = DirHeader.version_needed_to_extract;
                            LocalHeader.general_purpose_bit_flag  = DirHeader.general_purpose_bit_flag;
                            LocalHeader.compression_method        = DirHeader.compression_method;
                            LocalHeader.last_mod_file_time        = DirHeader.last_mod_file_time;
                            LocalHeader.last_mod_file_date        = DirHeader.last_mod_file_date;
                            LocalHeader.crc32                     = DirHeader.crc32;
                            LocalHeader.compressed_size           = DirHeader.compressed_size;
                            LocalHeader.uncompressed_size         = DirHeader.uncompressed_size;
                            LocalHeader.filename_length           = DirHeader.filename_length;
                            LocalHeader.extra_field_length        = DirHeader.extra_field_length;
                        }

                        dwSizeOfRawDataWithHeader = LocalHeader.compressed_size+
                            sizeof(LocalFileHeader)+LocalHeader.filename_length+
                            LocalHeader.extra_field_length;

                        dwSizeOfRawData = LocalHeader.compressed_size;

                        dwPos = dwPosOfLocalHeader + sizeof(LocalFileHeader)+LocalHeader.filename_length+
                            LocalHeader.extra_field_length;
                    
                        if((DirHeader.uncompressed_size == 0 ||
                            DirHeader.compressed_size == 0) &&
                            szName[DirHeader.filename_length-1]=='/')
                        {
                            qwCurPos += DirHeader.filename_length+DirHeader.file_comment_length+
                                DirHeader.extra_field_length+sizeof(FileHeader);
                            dwTotalFiles++;
                            continue;
                        }

                        bIsFolder = cFALSE;

                        qwCurPos += DirHeader.filename_length+DirHeader.file_comment_length+
                            DirHeader.extra_field_length+sizeof(FileHeader);
                        dwTotalFiles++;
    //Exit
                    }
                    else
                    {
                        error = errOBJECT_READ;
                        break;
                    }
                }
                else if(DirHeader.entral_file_header_signature == END_RECORD_SIG)
                {
                    error = errEND_OF_THE_LIST;
                    break;
                }
                else
                {
                    data->bWarnCorrupted = cTRUE;
                    error = errEND_OF_THE_LIST;
                    break;
                }
            }
            else
            {
                error = errEND_OF_THE_LIST;
                break;
            }
        }
        else
        {
            /* read local headers */
            error = CALL_SeqIO_Seek(data->hSeqIO, NULL, qwCurPos, cSEEK_SET);
            error = CALL_SeqIO_Read(data->hSeqIO, &dwTmp, &LocalHeader,sizeof(LocalFileHeader));
            if(PR_SUCC(error) && sizeof(LocalFileHeader) == dwTmp)
            {
                if(LocalHeader.sig == 0)    // Empty ZIP
                {
                    error = errEND_OF_THE_LIST;
                    break;
                }
                else if(LocalHeader.sig == UFO_HDR_SIG)
                {
                    CALL_SeqIO_Seek(data->hSeqIO, &qwCurPos, sizeof(LocalFileHeader)-0x10,cSEEK_BACK);
                    continue;
                }
                else if(LocalHeader.sig == LOCAL_HDR_SIG)
                {
                    if(LocalHeader.filename_length<sizeof(szName))
                    {
                        error = CALL_SeqIO_Read(data->hSeqIO,&dwTmp, &szName,LocalHeader.filename_length);

                        if((LocalHeader.uncompressed_size == 0 ||
                            LocalHeader.compressed_size == 0) &&
                            szName[LocalHeader.filename_length-1]=='/')
                        {
                            dwPosOfLocalHeader = /*dwLastPos = */(tDWORD)qwCurPos;
                            dwSizeOfRawDataWithHeader = LocalHeader.compressed_size+sizeof(LocalFileHeader)+
                                LocalHeader.filename_length+LocalHeader.extra_field_length;
                            dwSizeOfRawData = LocalHeader.compressed_size;
                            dwPos = dwPosOfLocalHeader + sizeof(LocalFileHeader)+LocalHeader.filename_length+
                                LocalHeader.extra_field_length;

                            qwCurPos += LocalHeader.compressed_size+LocalHeader.filename_length+
                                LocalHeader.extra_field_length+sizeof(LocalFileHeader);
                            dwTotalFiles++;
                            continue;
                        }
                        szName[LocalHeader.filename_length]=0;
                        if((LocalHeader.general_purpose_bit_flag & 0x8) == 0x8 && LocalHeader.compressed_size == 0)
                        {
                            // Try To Check Dirs
                            if (PR_SUCC(iFindEndHeader(data->hLocalHeap, data->hCDirSeqIO, data->qwStartOffset, &dwTmp, 0)))
                            {
                                if(dwTmp == 0)
                                {
                                    error = errEND_OF_THE_LIST;
                                    break;
                                }
                                if((dwTmp = iSkipNumberOfFile(data->hSeqIO, dwTmp, dwTotalFiles))!=0)
                                {
                                    bIsBrowsingDir = cTRUE;
                                    qwCurPos = (tQWORD)dwTmp;
                                    continue;
                                }
                            }
                            else
                            {
                                error = errOBJECT_READ;
                                break;
                            }

                        }
                        bIsFolder = cFALSE;
                        dwPosOfLocalHeader = /*dwLastPos =*/ (tDWORD)qwCurPos;
                        dwSizeOfRawDataWithHeader = LocalHeader.compressed_size+
                            sizeof(LocalFileHeader)+LocalHeader.filename_length+
                            LocalHeader.extra_field_length;

                        dwSizeOfRawData = LocalHeader.compressed_size;

                        dwPos = dwPosOfLocalHeader + sizeof(LocalFileHeader)+LocalHeader.filename_length+
                            LocalHeader.extra_field_length;

                        qwCurPos += LocalHeader.compressed_size+LocalHeader.filename_length+
                            LocalHeader.extra_field_length+sizeof(LocalFileHeader);
                        dwTotalFiles++;
                        //Exit
                    }
                    else
                    {
                        error = errOBJECT_READ;
                        break;
                    }
                }
                else if(LocalHeader.sig == HEADER_FILE_SIG)
                {
                    error = errEND_OF_THE_LIST;
                    break;
                }
                else 
                {
                    data->bWarnCorrupted = cTRUE;
                    error = errEND_OF_THE_LIST;
                    break;
                }
            }
            else
            {
                error = errEND_OF_THE_LIST;
                break;
            }
        }

        if (dwCurPosInEnum >= dwCurDirSize)
        {
            dwCurDirSize += 16;

            error = CALL_Heap_Realloc(data->hLocalHeap, (tPTR*)&data->lpDir, data->lpDir, dwCurDirSize*sizeof(ZIPEntry));
            if (PR_FAIL(error))
            {
                break;
            }
        }

        data->lpDir[dwCurPosInEnum].Header = LocalHeader;

        // petrovitch 27.05.2004 
        // it is a SINGLE!!!  OS dependent call in plugin
        // just to be binary portable!!!
        // it was:  strncpy(data->lpDir[dwCurPosInEnum].szName, szName, MAX_FILENAME_LEN);
        // now   :
        {
          tUINT str_length = _toui32(strlen( szName ));
          if ( str_length > (MAX_FILENAME_LEN-1) )
            str_length = MAX_FILENAME_LEN-1;
          memcpy(data->lpDir[dwCurPosInEnum].szName, szName, str_length);
          data->lpDir[dwCurPosInEnum].szName[str_length] = 0;
        }

        data->lpDir[dwCurPosInEnum].bIsFolder = bIsFolder;
        data->lpDir[dwCurPosInEnum].dwSizeOfRawData = dwSizeOfRawData;
        data->lpDir[dwCurPosInEnum].dwSizeOfRawDataWithHeader = dwSizeOfRawDataWithHeader;
        data->lpDir[dwCurPosInEnum].qwHeaderPos = dwPosOfLocalHeader;
        data->lpDir[dwCurPosInEnum].qwPos = dwPos;

        dwCurPosInEnum++;
    }

    data->dwFiles = dwCurPosInEnum;

    if (error == errEND_OF_THE_LIST)
    {
        error = errOK;
    }

    data->qwCurPos = 0xFFFFFFFFFFFFFFFF;

    if (PR_SUCC(error) && data->bWarnCorrupted)
    {
        // object is corrupted therefore becomes read only
        data->bIsReadOnly = cTRUE;
        data->dwAvail = fAVAIL_READ;
        error = warnOBJECT_DATA_CORRUPTED;
    }

    PR_TRACE_A1( _this, "Leave MiniArchiver::ObjectInit method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Behaviour comment
//    Reset All internal data (variables) to default or initial values. After this call enumerator must start from the very beginning
// Parameters are:
tERROR pr_call MiniArchiver_Reset( hi_MiniArchiver _this )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter MiniArchiver::Reset method" );

    _this->data->qwCurPos = 0xFFFFFFFFFFFFFFFF;

    PR_TRACE_A1( _this, "Leave MiniArchiver::Reset method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Parameters are:
tERROR pr_call MiniArchiver_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId )
{
    tERROR error = errOK;
    tQWORD ret_val = 0;
    ZIPData* data =_this->data;
    tQWORD qwCurPos;

    (void)ret_val;
    PR_TRACE_A0( _this, "Enter MiniArchiver::Next method" );

    qwCurPos = qwLastObjectId - 1;

    qwCurPos++;

    if (qwCurPos >= data->dwFiles)
    {
        error = errEND_OF_THE_LIST;
    }

    if ( result )
        *result = qwCurPos + 1;
    PR_TRACE_A2( _this, "Leave MiniArchiver::Next method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end

/* ------------------------------------------------------------------------- */

/* CRC-32 polynom table */
static const tDWORD Crc32Tbl[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

#ifndef MIN
# define MIN(a,b)  ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b)  ((a)>(b)?(a):(b))
#endif

#define DO_CRC32(c,b) (Crc32Tbl[((tINT)(c) ^ (b)) & 0xff] ^ ((c) >> 8))
#define CRC32_DO1(buf)  Seed = DO_CRC32(Seed, *buf++)
#define CRC32_DO2(buf)  CRC32_DO1(buf); CRC32_DO1(buf)
#define CRC32_DO4(buf)  CRC32_DO2(buf); CRC32_DO2(buf)
#define CRC32_DO8(buf)  CRC32_DO4(buf); CRC32_DO4(buf)
#define CRC32_DO16(buf) CRC32_DO8(buf); CRC32_DO8(buf)

/* fast CRC-32 implementation */
static tDWORD ZipCrc32(tBYTE* Buff, tUINT Size, tDWORD Seed)
{
  while( Size >= 16 )
  {
    CRC32_DO16(Buff);
    Size -= 16;
  }
  while( Size-- )
  {
    CRC32_DO1(Buff);
  }

  return(Seed);
}

/* ------------------------------------------------------------------------- */

#define ZIP_KEY_CRC32(c, b) (Crc32Tbl[((tINT)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

#define ZIP_DECRYPT_BYTE(Ctx, Res)                    \
{                                                     \
  tDWORD Tmp;                                         \
                                                      \
  Tmp = ((tDWORD)Ctx->Keys[2] & 0xffff) | 2;          \
  Res = (tINT)(((Tmp * (Tmp ^ 1)) >> 8) & 0xff);      \
}

#define ZIP_UPDATE_KEYS(Ctx, C)                       \
{                                                     \
  tINT SKey;                                          \
                                                      \
  Ctx->Keys[0]  = ZIP_KEY_CRC32(Ctx->Keys[0], C);     \
  Ctx->Keys[1] += Ctx->Keys[0] & 0xff;                \
  Ctx->Keys[1]  = Ctx->Keys[1] * 134775813L + 1;      \
  SKey = (tINT)(Ctx->Keys[1] >> 24);                  \
  Ctx->Keys[2] = ZIP_KEY_CRC32(Ctx->Keys[2], SKey);   \
}

#define ZIP_INIT_KEYS(Ctx, Passwd)                    \
{                                                     \
  tCHAR* Pwd;                                         \
  tINT   PB;                                          \
                                                      \
  Pwd = Passwd;                                       \
  Ctx->Keys[0] = 305419896L;                          \
  Ctx->Keys[1] = 591751049L;                          \
  Ctx->Keys[2] = 878082192L;                          \
  while ( '\0' != (PB = *Pwd) )                       \
  {                                                   \
    ZIP_UPDATE_KEYS(Ctx, PB);                         \
    Pwd++;                                            \
  }                                                   \
}

/* ------------------------------------------------------------------------- */

/* prosess yeld message */
static tERROR Zip_Rotate(hi_MiniArchiver _this, hIO IO)
{
  tERROR Error;

  if ( PR_FAIL(Error = CALL_SYS_SendMsg(IO, pmc_PROCESSING,
    pm_PROCESSING_YIELD, _this, 0 ,0)) )
  {
    return(Error); /* stopped */
  }

  return(errOK);
}

/* ------------------------------------------------------------------------- */

/* block read callback */
static tDWORD pr_call Zip_ReadCB(tPTR User, tBYTE* Buff, tDWORD Size)
{
  tZIP_DATA* Data;
  tBYTE  CB, B, RB, R;
  tUINT  I;

  /* check params */
  if ( NULL == (Data = (tZIP_DATA*)(User)) || NULL == Buff || 0 == Size )
    return(0); /* error */

  /* decrypt block */
  if ( Data->Decrypt )
  {
    for ( I = 0; I < (tUINT)(Size); I++ )
    {
      CB = Buff[I];
      ZIP_DECRYPT_BYTE(Data, R);
      RB = B = (tBYTE)(CB ^ R);
      ZIP_UPDATE_KEYS(Data, B);
      Buff[I] = RB;
    }
  }

  return(Size);
}

/* ------------------------------------------------------------------------- */

/* block write callback */
static tDWORD pr_call Zip_WriteCB(tPTR User, tBYTE* Buff, tDWORD Size)
{
  tZIP_DATA* Data;

  /* check params */
  if ( NULL == (Data = (tZIP_DATA*)(User)) || NULL == Buff || 0 == Size )
    return(0); /* error */

  /* calc crc-32 */
  if ( Data->Decrypt )
  {
    Data->WrtCrc32 = ZipCrc32(Buff, Size, Data->WrtCrc32);
  }

  return(Size);
}

/* ------------------------------------------------------------------------- */

/* old function for extract object */
static tERROR Zip_ObjCreate(hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO)
{
    tERROR error;
    tINT iTmp;
    hTRANSFORMER hCompressor;
    hSEQ_IO hInputSeqIo, hOutputSeqIo;
    tQWORD qwSize;
    hIO hDmapIO;
    hIO hSuperIo;
    ZIPData * data = _this->data;
    tQWORD qwTmp;
    ZIPEntry * entry;
    hSTRING hPasswordString=0;
    tQWORD qwCurPos;
    tDWORD dwFlag;

    PR_TRACE_A0( _this, "Enter MiniArchiver::ObjCreate method" );

    error = errOBJECT_NOT_CREATED;
    //TempOutputIO = NULL;
    hOutputSeqIo = NULL;
    hInputSeqIo = NULL;
    hCompressor = NULL;
    hSuperIo = NULL;

    qwCurPos = qwObjectIdToCreate - 1;

    entry = &data->lpDir[qwCurPos];

loc_retry:

    error = CALL_SYS_ObjectCreate(_this, &hSuperIo, IID_IO, PID_SUPERIO, SUBTYPE_ANY);
    if(PR_FAIL(error))
    {
        return error;
    }

    switch(entry->Header.compression_method)
    {
        case STORED:
            error = CALL_SYS_ObjectCreate((hOBJECT)hSuperIo, (hOBJECT*)&hCompressor, IID_TRANSFORMER, PID_UNSTORE,0);
            break;
        case REDUCED1 :
        case REDUCED2 :
        case REDUCED3 :
        case REDUCED4 :
            error = CALL_SYS_ObjectCreate((hOBJECT)hSuperIo, (hOBJECT*)&hCompressor, IID_TRANSFORMER, PID_UNREDUCE,0);
            if(PR_SUCC(error))
            {
                dwFlag = entry->Header.compression_method;
                error = CALL_SYS_PropertySet(hCompressor, NULL, plEXPLODE_COMPRESSION_FLAG, &dwFlag, sizeof(dwFlag));
            }
            break;

        case DEFLATED:
            error = CALL_SYS_ObjectCreate((hOBJECT)hSuperIo, (hOBJECT*)&hCompressor,IID_TRANSFORMER, PID_INFLATE,0);
            if(PR_SUCC(error))
            {
                CALL_SYS_PropertySetDWord(hCompressor, plINFLATE_PETITE_FLAG, 0);
                CALL_SYS_PropertySetDWord(hCompressor, plINFLATE_WINDOW_SIZE,32*1024);

                iTmp=-15;
                CALL_SYS_PropertySet(hCompressor, NULL, plINFLATE_WINDOW_MULTIPIER, &iTmp, sizeof(iTmp));
            }
            break;
        case SHRUNK:
            error = CALL_SYS_ObjectCreate((hOBJECT)hSuperIo,(hOBJECT*)&hCompressor,IID_TRANSFORMER, PID_UNSHRINK,0);
            break;
        case IMPLODED:
            error = CALL_SYS_ObjectCreate((hOBJECT)hSuperIo,(hOBJECT*)&hCompressor,IID_TRANSFORMER, PID_EXPLODE,0);
            if(PR_SUCC(error))
            {
                dwFlag = entry->Header.general_purpose_bit_flag;
                CALL_SYS_PropertySet(hCompressor, NULL, plEXPLODE_BIT_FLAG, &dwFlag, sizeof(dwFlag));
            }
            break;
        case DEFLATED64:
// TODO Implement Deflate 64 decompression
            return errNOT_IMPLEMENTED;
        default:
            return errOBJECT_DATA_CORRUPTED;
    }
    if(PR_FAIL(error))
    {
        CALL_SYS_ObjectClose(hSuperIo);
        return error;
    }

    if(entry->Header.general_purpose_bit_flag & 1)//Password Protected file!
    {
        error=(tERROR)CALL_SYS_ObjectCreate(hCompressor,(hOBJECT*)&hDmapIO, IID_IO, PID_ZIPPASSWORDMAP,0);

        if(hDmapIO)
        {
//#define pm_IO_PASSWORD_REQUEST 0x00000003 // (3) -- запрос пароля для доступа к физическому объекту
//    объект типа hSTRING.
// context comment
// обработчик сообщения может вернуть ошибку. Это означает, что обработку текущего объекта
// нужно прекратить с соответсвующим кодом ошибки (? errOBJECT_PASSWORD_PROTECTED ?)
// Cледует учитывать ситуацию, когда ни одного обработчика сообщения не было.
// В этом случае kernel вернет warning errOK_NO_DECIDERS

            if (!hPasswordString)
            {
                error = CALL_SYS_ObjectCreateQuick( _this, &hPasswordString, IID_STRING, PID_ANY, SUBTYPE_ANY );
                if(PR_FAIL(error))
                {
                    CALL_SYS_ObjectClose(hSuperIo);
                    return error;
                }
            }
            
            if(data->szPassword[0])
                CALL_SYS_PropertySet( hDmapIO, NULL, ppMAP_PROP_PASSWORD, data->szPassword, _toui32(strlen(data->szPassword)));
            else
            {
                error = errNOT_OK;
                if(data->hArcIO)
                    error = CALL_SYS_SendMsg (hUniArcIO, pmc_IO, pm_IO_PASSWORD_REQUEST, hPasswordString, NULL, NULL);
                if( error == errOK || error == errOK_DECIDED)
                {
                    CALL_String_ExportToBuff( hPasswordString, NULL, cSTRING_WHOLE, data->szPassword, MAX_PASSWORD_LEN, cCP_ANSI, cSTRING_Z);
                }
                else
                {
                    CALL_SYS_ObjectClose(hPasswordString);
                    CALL_SYS_ObjectClose(hSuperIo);
                    //error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this,(hOBJECT*)&hDmapIO, IID_IO, DMAP_ID,0);
                    return errOBJECT_PASSWORD_PROTECTED;
                }
            }
            CALL_SYS_PropertySetDWord(hDmapIO, ppMAP_PROP_CRC32, entry->Header.crc32);
            CALL_SYS_PropertySetWord(hDmapIO, ppMAP_PROP_DATE, entry->Header.last_mod_file_date);
            CALL_SYS_PropertySetWord(hDmapIO, ppMAP_PROP_TIME, entry->Header.last_mod_file_time);
            CALL_SYS_PropertySetWord(hDmapIO, ppMAP_PROP_FLAG, entry->Header.general_purpose_bit_flag);
            CALL_SYS_PropertySet( hDmapIO, NULL, ppMAP_PROP_PASSWORD, data->szPassword, _toui32(strlen(data->szPassword)));
        }
    }
    else
    {
        error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)hCompressor,(hOBJECT*)&hDmapIO, IID_IO, DMAP_ID,0);
    }

    if(PR_FAIL(error))
    {
        if (hPasswordString)
            CALL_SYS_ObjectClose(hPasswordString);
        CALL_SYS_ObjectClose(hSuperIo);
        return error;
    }

    CALL_SYS_PropertySet( hDmapIO, NULL,  ppMAP_AREA_ORIGIN, &data->hArcIO, sizeof(hIO) );
    CALL_SYS_PropertySetStr( hDmapIO, NULL, pgOBJECT_NAME, &entry->szName, _toui32(strlen(entry->szName)), cCP_ANSI );

    CALL_SYS_PropertySetDWord( hDmapIO, pgOBJECT_OPEN_MODE, data->dwOpenMode);
    CALL_SYS_PropertySetDWord( hDmapIO, pgOBJECT_ACCESS_MODE, data->dwAccessMode);

    qwTmp = entry->qwPos;
    CALL_SYS_PropertySet( hDmapIO, NULL, ppMAP_AREA_START, &qwTmp, sizeof(qwTmp));

    qwTmp = entry->Header.compressed_size;
    CALL_SYS_PropertySet( hDmapIO, NULL, ppMAP_AREA_SIZE, &qwTmp , sizeof(qwTmp ) );

    error = CALL_SYS_ObjectCreateDone( hDmapIO );

    if(PR_FAIL(error))
    {
        error = errOBJECT_DATA_CORRUPTED;
    }
    else
    {
        if(entry->Header.general_purpose_bit_flag & 1)
        {
            while(errOK != CALL_SYS_PropertyGetDWord(hDmapIO, ppMAP_PROP_ERRORCODE))
            {
                error = CALL_SYS_SendMsg (hUniArcIO, pmc_IO, pm_IO_PASSWORD_REQUEST, hPasswordString, NULL, NULL);
                if( error == errOK || error == errOK_DECIDED)
                {
                    CALL_String_ExportToBuff( hPasswordString, NULL, cSTRING_WHOLE, data->szPassword, MAX_PASSWORD_LEN, cCP_ANSI, cSTRING_Z);
                    CALL_SYS_PropertySet( hDmapIO, NULL, ppMAP_PROP_PASSWORD, data->szPassword, _toui32(strlen(data->szPassword)));

                }
                else
                {
                    if(hPasswordString)
                        CALL_SYS_ObjectClose(hPasswordString);
                    CALL_SYS_ObjectClose(hSuperIo);
                    return errOBJECT_PASSWORD_PROTECTED;
                }
            }
        }
    }

    if(PR_SUCC(error))
    {
        qwSize = entry->Header.uncompressed_size;
        CALL_SYS_PropertySet(hCompressor, NULL, pgTRANSFORM_SIZE, &qwSize, sizeof(qwSize ));
        //plREAD_CHUCK_SIZE

        error = CALL_SYS_ObjectCreateQuick(hDmapIO, &hInputSeqIo, IID_SEQIO, PID_ANY,0);
        if(PR_FAIL(error ))
        {
            if (hPasswordString)
                CALL_SYS_ObjectClose(hPasswordString);
            CALL_SYS_ObjectClose(hSuperIo);
            return error;
        }
        CALL_SYS_PropertySetObj(hCompressor, pgINPUT_SEQ_IO, (hOBJECT)hInputSeqIo);


        CALL_SYS_PropertySetObj(hSuperIo, plINPUT_CONVERTER, (hOBJECT)hCompressor);
        CALL_SYS_PropertySetObj( (hOBJECT)hSuperIo, pgOBJECT_BASED_ON, NULL);
        CALL_SYS_PropertySetDWord( (hOBJECT)hSuperIo, pgOBJECT_ACCESS_MODE, fACCESS_RW);//fACCESS_READ);data->dwAccessMode);
        CALL_SYS_PropertySetDWord( (hOBJECT)hSuperIo, pgOBJECT_OPEN_MODE,   fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
        error = CALL_SYS_ObjectCreateDone(hSuperIo);
        if(PR_FAIL(error))
        {
            if (hPasswordString)
                CALL_SYS_ObjectClose(hPasswordString);
            CALL_SYS_ObjectClose(hSuperIo);
            return error;
        }

        if(entry->Header.general_purpose_bit_flag & 1) // password protected
        {
            tBYTE* Buffer = 0;
            tDWORD cSizeOfBuffer = 0x1000;
            tDWORD CRC32;
            error = CALL_Heap_Alloc(data->hLocalHeap, (tPTR*)&Buffer, cSizeOfBuffer+16);
            // we should unpack the whole file and check CRC32
            // otherwise there could be a "good" password which casuses wrong content

            CRC32 = CountCRC32Io(hSuperIo, Buffer, cSizeOfBuffer);

            CALL_Heap_Free(data->hLocalHeap, (tPTR)Buffer);

            if (CRC32 != entry->Header.crc32 )
            {
                //if (hPasswordString)
                //    CALL_SYS_ObjectClose(hPasswordString);
                //hPasswordString = NULL;
                CALL_SYS_ObjectClose(hSuperIo);
                hSuperIo = NULL;

                data->szPassword[0] = '\0';

                goto loc_retry;
            }
        }

    }

    //CALL_SYS_ObjectClose(hDmapIO);

    if (hPasswordString)
        CALL_SYS_ObjectClose(hPasswordString);
    hPasswordString = NULL;

    if ( result )
        *result = hSuperIo;
    PR_TRACE_A2( _this, "Leave MiniArchiver::ObjCreate method, ret hIO = %p, error = 0x%08x", hSuperIo, error );
    return error;
}

/* ------------------------------------------------------------------------- */

#ifdef _DEBUG
# if 0
#  define DEBUG_DUMP
# endif
#endif

#ifdef DEBUG_DUMP
#include <stdio.h>
static tVOID DG_DumpSeqIO(hSEQ_IO hSeqIO, tCHAR* FName)
{
  tBYTE  Buff[2048];
  FILE*  F;
  tUINT  Rdt;
  tCHAR* Name;

  Name = &FName[strlen(FName)];
  while( Name >= FName && *Name != '\\' && *Name != '/' )
    Name--;

  Name++;
  if ( NULL == (F = fopen(Name, "wb")) )
    return; 

  fseek(F, 0, SEEK_SET);
  CALL_SeqIO_Seek(hSeqIO, NULL, 0, cSEEK_SET);

  while(1)
  {
    Rdt = 0;
    if ( !PR_SUCC(CALL_SeqIO_Read(hSeqIO, &Rdt, &Buff[0], sizeof(Buff))) )
      break;
    if ( Rdt == 0 )
      break;
    fwrite(&Buff[0], Rdt, 1, F);
  }

  fclose(F);
}
#else
# define DG_DumpSeqIO(a,b)
#endif

/* ------------------------------------------------------------------------- */

#define ZIP_RBUF_SIZE  (8*1024)
#define ZIP_WBUF_SIZE  (8*1024)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ObjCreate )
// Parameters are:
tERROR pr_call MiniArchiver_ObjCreate(hi_MiniArchiver _this, hIO* Result,
                                      tQWORD qwObjId, hIO hUniArcIO)
{
  hTRANSFORMER hTrans;
  tBOOL      HasSalt;
  tBYTE      Salt[RAND_HEAD_LEN];
  tERROR     Error = errOK;
  tZIP_DATA* Data; 
  ZIPEntry*  Entry;
  tUINT      CurId;
  tUINT      Method;
  hIO        hDstIO = NULL;
  hSEQ_IO    hSrcSeqIO = NULL;
  hSEQ_IO    hDstSeqIO = NULL;
  tBOOL      Decrypt;
  tQWORD     PDataOfs;
  tQWORD     PSize;
  tQWORD     USize;
  tUINT      RotCnt;

  Data  = (tZIP_DATA*)(_this->data);
  CurId = (tUINT)(qwObjId - 1);
  Entry = &Data->lpDir[CurId];

  Method = Entry->Header.compression_method;
  switch( Method )
  {
  case STORED:
  case DEFLATED:
  case DEFLATED64:
    break;
  case 99: /* AES 128/192/256 encrypted */
    return(errOBJECT_PASSWORD_PROTECTED);
  default: /* use old shitty unpacker */
    return Zip_ObjCreate(_this, Result, qwObjId, hUniArcIO);
  }

  /* check last transformer type */
  if ( Data->LastMethod != Method && NULL != Data->hTranformer )
  {
    CALL_SYS_ObjectClose(Data->hTranformer);
    Data->hTranformer = NULL;
  }

  /* create transformer if needed */
  if ( NULL == (hTrans = Data->hTranformer) )
  {
    tBYTE* Wnd;
    tDWORD WndSize;
    tINT   WndMul;

    switch( Method )
    {
    case STORED:
      WndSize = (32*1024);
      Error = CALL_SYS_ObjectCreateQuick((hOBJECT)_this, &hTrans,
        IID_TRANSFORMER, PID_UNSTORE, SUBTYPE_ANY);
      Data->ResetMeth = UNSTORED_RESET_STATE;
      break;
    case DEFLATED:
    case DEFLATED64:
      WndSize = (Method == DEFLATED) ? (32*1024) : (64*1024);
      Error = CALL_SYS_ObjectCreateQuick((hOBJECT)_this, &hTrans,
        IID_TRANSFORMER, PID_INFLATE, SUBTYPE_ANY);
      Data->ResetMeth = INFLATE_RESET_STATE;
      break;
    default:
      return(errNOT_OK);
    }

    /* check for error */
    if ( PR_FAIL(Error) )
      return(Error);

    Data->LastMethod  = Method;
    Data->hTranformer = hTrans;

    /* init window */
    if ( NULL != (Wnd = Data->Window) && Data->WindowSize < WndSize )
    {
      CALL_Heap_Free(Data->hLocalHeap, Wnd);
      Data->Window = Wnd = NULL;
    }
    if ( NULL == Wnd )
    {
      if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hLocalHeap, &Wnd, WndSize)) )
        goto loc_exit;
      
      Data->Window = Wnd;
      Data->WindowSize = WndSize;
    }

    /* init transformer window */
    CALL_SYS_PropertySet(hTrans, NULL, pgTRANSFORM_WINDOW, &Wnd, sizeof(Wnd));
    CALL_SYS_PropertySet(hTrans, NULL, pgTRANSFORM_WINDOW_SIZE, &WndSize,
      sizeof(WndSize));

    /* init io buffers */
    if ( Method != STORED )
    {
      tDWORD Size;
      tBYTE* Buf;

      /* alloc buffer */
      if ( NULL == (Buf = Data->IoBuff) )
      {
        Size  = (ZIP_RBUF_SIZE + ZIP_WBUF_SIZE);
        if ( PR_FAIL(Error = CALL_Heap_Alloc(Data->hLocalHeap, &Buf, Size)) )
          goto loc_exit;

        Data->IoBuff = Buf;
      }
      
      Size = ZIP_RBUF_SIZE;
      CALL_SYS_PropertySet(hTrans, NULL, pgINPUT_BUFF, &Buf, sizeof(Buf));
      CALL_SYS_PropertySet(hTrans, NULL, pgINPUT_BUFF_SIZE, &Size, sizeof(Size));

      Buf += ZIP_RBUF_SIZE;
      Size = ZIP_WBUF_SIZE;
      CALL_SYS_PropertySet(hTrans, NULL, pgOUTPUT_BUFF, &Buf, sizeof(Buf));
      CALL_SYS_PropertySet(hTrans, NULL, pgOUTPUT_BUFF_SIZE, &Size, sizeof(Size));
    }

    /* config transformer */
    switch( Method )
    {
    case STORED:
      break; /* not needed */
    case DEFLATED:
      WndMul = -15;
      CALL_SYS_PropertySet(hTrans, NULL, plINFLATE_WINDOW_MULTIPIER, &WndMul, sizeof(tINT));
      break;
    case DEFLATED64:
      WndMul = -16;
      CALL_SYS_PropertySet(hTrans, NULL, plINFLATE_WINDOW_MULTIPIER, &WndMul, sizeof(tINT));
      break;
    default:
      return(errNOT_OK);
    }
  }

  Data->Decrypt = Decrypt = (Entry->Header.general_purpose_bit_flag & 1) != 0;

  if ( Decrypt )
  {
    tFUNC_PTR FPtr;

    /* setup callbacks */
    FPtr = (tFUNC_PTR)(Zip_ReadCB);
    CALL_SYS_PropertySet(hTrans, NULL, pgINPUT_CALLBACK, &FPtr, sizeof(FPtr));
    FPtr = (tFUNC_PTR)(Zip_WriteCB);
    CALL_SYS_PropertySet(hTrans, NULL, pgOUTPUT_CALLBACK,&FPtr, sizeof(FPtr));
    CALL_SYS_PropertySet(hTrans, NULL, pgCALLBACK_USER,  &Data, sizeof(Data));

    HasSalt = cFALSE; /* no salt readed yet */
  }

  /* create output io */
  Error = CALL_SYS_ObjectCreate(_this,&hDstIO,IID_IO,PID_TMPFILE,SUBTYPE_ANY);
  if ( PR_FAIL(Error) )
    goto loc_exit;
  CALL_SYS_PropertySetDWord((hOBJECT)(hDstIO),pgOBJECT_ACCESS_MODE,fACCESS_RW);
  CALL_SYS_PropertySetDWord((hOBJECT)(hDstIO),pgOBJECT_OPEN_MODE,
      fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_READ);
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateDone(hDstIO)) )
    goto loc_exit;

  /* create output seq-io */
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(hDstIO, &hDstSeqIO,
    IID_SEQIO, PID_ANY, SUBTYPE_ANY)) )
  {
    goto loc_exit;
  }

  /* get input seq-io */
  if ( NULL == (hSrcSeqIO = Data->hSeqIO) )
  {
    Error = errOBJECT_NOT_INITIALIZED;
    goto loc_exit;
  }

  /* setup transormer io */
  CALL_SYS_PropertySet(hTrans, NULL, pgINPUT_SEQ_IO,  &hSrcSeqIO, sizeof(hSrcSeqIO));
  CALL_SYS_PropertySet(hTrans, NULL, pgOUTPUT_SEQ_IO, &hDstSeqIO, sizeof(hDstSeqIO));

  /* calc sizes */
  switch( Method )
  {
  case STORED:
    PSize = Entry->Header.uncompressed_size;
    PSize = MAX(PSize, Entry->Header.compressed_size);
    USize = PSize;
    break;
  case DEFLATED:
  case DEFLATED64:
    PSize = Entry->Header.compressed_size;
    USize = 0xffffffffffffffff;
    break;
  default:
    Error = errNOT_OK;
    goto loc_exit;
  }

  PDataOfs = Entry->qwPos;
  RotCnt = 0;

psw_retry:
  
  /* crypt init */
  Error = errOK;
  if ( Decrypt )
  {
    tCHAR* Password;
    tBYTE  hh[RAND_HEAD_LEN];
    tBOOL  BadPas;
    tBYTE  B;
    tUINT  I;

    if ( NULL == Data->hPassword )
    {
      /* create password container */
      if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(_this, &Data->hPassword,
        IID_STRING, PID_ANY, SUBTYPE_ANY)) )
      {
        goto loc_exit;
      }

      /* create password buffer */
      Error = CALL_Heap_Alloc(Data->hLocalHeap,&Password,MAX_PASSWORD_LEN+4);
      if ( PR_FAIL(Error) )
        goto loc_exit;

      Data->Password = Password;
    }
    else
    {
      Password = Data->Password;
    }

    /* check current password */
    if ( Password[0] == '\0' )
    {
      /* send pswd request */
      Error = CALL_SYS_SendMsg(hUniArcIO, pmc_IO, pm_IO_PASSWORD_REQUEST,
        Data->hPassword, NULL, NULL);
      if ( Error != errOK_DECIDED )
      {
        Error = errOBJECT_PASSWORD_PROTECTED;
        goto loc_exit; /* no passwords */
      }
    
      /* convert */
      Password[MAX_PASSWORD_LEN+0] = '\0';
      Password[MAX_PASSWORD_LEN+1] = '\0';
      if ( PR_FAIL(Error = CALL_String_ExportToBuff(Data->hPassword, NULL, cSTRING_WHOLE,
          &Password[0], MAX_PASSWORD_LEN, cCP_OEM, cSTRING_Z)) )
      {
        goto loc_exit;
      }
      if ( Password[0] == '\0' )
      {
        Error = errOBJECT_PASSWORD_PROTECTED;
        goto loc_exit; /* no passwords */
      }
    }

    /* read salt */
    if ( !HasSalt )
    {
      tQWORD Offs = 0;
      tDWORD Size = 0;

      CALL_SeqIO_Seek(hSrcSeqIO, &Offs, PDataOfs, cSEEK_SET);
      if ( Offs != PDataOfs )
      {
        Error = errOBJECT_DATA_CORRUPTED;
        goto loc_exit;
      }

      PDataOfs += sizeof(Salt);
      Error = CALL_SeqIO_Read(hSrcSeqIO, &Size, &Salt[0], sizeof(Salt));
      if ( Size != sizeof(Salt) )
      {
        Error = errOBJECT_DATA_CORRUPTED;
        goto loc_exit;
      }

      /* update sizes */
      PSize -= sizeof(Salt);
      if ( Method == STORED )
        USize -= sizeof(Salt);

      HasSalt = cTRUE; /* salt readed */
    }

    /* init keys */
    Data->WrtCrc32 = 0xffffffff;
    ZIP_INIT_KEYS(Data, Password);

    /* decrypt salt */
    for ( I = 0; I < RAND_HEAD_LEN; I++ )
    {
      tBYTE CB, B, RB, R;

      CB = Salt[I];
      ZIP_DECRYPT_BYTE(Data, R);
      RB = B = (tBYTE)(CB ^ R);
      ZIP_UPDATE_KEYS(Data, B);
      hh[I] = RB;
    }

    /* check key */
    BadPas = cFALSE;
    B = hh[RAND_HEAD_LEN-1];
    if( Entry->Header.general_purpose_bit_flag & 8 )
    {
      if ( B != ((Entry->Header.last_mod_file_time >> 8) & 0xff) )
        BadPas = cTRUE;
    }
    else
    {
      if ( B != ((Entry->Header.crc32 >> 24) & 0xff) )
         BadPas = cTRUE;
    }

    if ( BadPas )
    {
      if ( !(++RotCnt & 0x1f) )
      {
        if ( errOK != (Error = Zip_Rotate(_this, hUniArcIO)) )
          goto loc_exit; /* stopped */
      }

      Data->Password[0] = '\0';
      goto psw_retry; /* try other password */
    }
  }

  /* re-seek files */
  CALL_SeqIO_Seek(hDstSeqIO, NULL, 0, cSEEK_SET);
  CALL_SeqIO_Seek(hSrcSeqIO, NULL, PDataOfs, cSEEK_SET);

  /* reset transformer */
  CALL_Transformer_Reset(hTrans, Data->ResetMeth);

  /* setup transformer sizes */
  CALL_SYS_PropertySet(hTrans, NULL, pgTRANSFORM_INPUT_SIZE, &PSize, sizeof(tQWORD));
  CALL_SYS_PropertySet(hTrans, NULL, pgTRANSFORM_SIZE, &USize, sizeof(tQWORD));

  /* transform object */
  Error = CALL_Transformer_Decode(hTrans, NULL);
  DG_DumpSeqIO(hDstSeqIO, Entry->szName);

  /* check result */
  if ( Error == errOPERATION_CANCELED )
    goto loc_exit; /* stopped */
  if ( (PR_FAIL(Error) && Decrypt) || (Decrypt && Entry->Header.crc32 &&
    Entry->Header.crc32 != ~Data->WrtCrc32) )
  {
    /* send yeld message */
    Data->Password[0] = '\0';
    if ( errOK == (Error = Zip_Rotate(_this, hUniArcIO)) )
      goto psw_retry; /* try another password */
  }

loc_exit:
  if ( NULL != hDstSeqIO )
  {
    CALL_SYS_ObjectClose(hDstSeqIO);
    hDstSeqIO = NULL;
  }
  if ( PR_FAIL(Error) )
  {
    if ( NULL != hDstIO )
    {
      CALL_SYS_ObjectClose(hDstIO);
      hDstIO = NULL;
    }
  }

  if ( NULL != Result )
    *Result = hDstIO;

  return(Error); /* done */
}
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetAsCurrent )
// Parameters are:
tERROR pr_call MiniArchiver_SetAsCurrent( hi_MiniArchiver _this, tQWORD qwObjectId )
{
    tERROR error = errNOT_IMPLEMENTED;
    PR_TRACE_A0( _this, "Enter MiniArchiver::SetAsCurrent method" );

    // Place your code here

    PR_TRACE_A1( _this, "Leave MiniArchiver::SetAsCurrent method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RebuildArchive )
// Parameters are:
tERROR pr_call MiniArchiver_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo )
{
    tERROR error = errOK;
    LocalFileHeader LocalHeader;
    FileHeader DirHeader;
    hIO hMainDirIO;
    hSEQ_IO hMainDirSeqIO;
    hSEQ_IO hNewZipSeqIO;
    hSEQ_IO hTempSeqIo;
    hIO io;
    tBOOL bChanged = cFALSE;
    tQWORD qwTmp;
    tDWORD dwPos;
    tBYTE* Buffer;
    tDWORD cSizeOfBuffer = 0x1000;
    ZIPData * data = _this->data;
    ZIPEntry * entry = NULL;
    tQWORD qwDataPosition;
    tDWORD dwTotalRecords = 0;
    hNEWOBJECT newobject;
    tCHAR szName[MAX_FILENAME_LEN];

    PR_TRACE_A0( _this, "Enter MiniArchiver::RebuildArchive method" );

    error = CALL_Heap_Alloc(data->hLocalHeap, (tPTR*)&Buffer, cSizeOfBuffer+16);

    error = CALL_SYS_ObjectCreate(_this,&hMainDirIO, IID_IO, PID_TMPFILE,0);
    if(PR_SUCC(error))
    {
        CALL_SYS_PropertySetDWord( (hOBJECT)hMainDirIO, pgOBJECT_OPEN_MODE,
            fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
        CALL_SYS_PropertySetDWord( (hOBJECT)hMainDirIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
        error = CALL_SYS_ObjectCreateDone( hMainDirIO);
    }

    if(error != errOK)
        return error;

    hMainDirSeqIO = CreateSeqIO(hMainDirIO);

    if(!hMainDirSeqIO)
        return errOBJECT_NOT_CREATED;

    hNewZipSeqIO = CreateSeqIO(hOutputIo);

    if(!hNewZipSeqIO)
        return errOBJECT_NOT_CREATED;

    for (dwPos = 0; dwPos < data->dwFiles ; dwPos++)
    {
        entry = &data->lpDir[dwPos];

        bChanged = cFALSE;
        io = NULL;

        LocalHeader = entry->Header;

//Check if present in deleted list
        if(errOK == CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&io, cDELETED_OBJECT, dwPos+1))
        {
            continue;
        }
//Check in list of opened and modified (deleted) objects
        if(errOK == CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&io, cCHANGED_OBJECT, dwPos+1))
        {
//Pack it according to previously used pack algo (or just stored)
            bChanged = cTRUE;
        }
        if(!io)
        {
            tDWORD dwAreaSize;
            tDWORD dwAreaPos;

            dwAreaPos = (tDWORD)entry->qwHeaderPos;
            dwAreaSize = entry->dwSizeOfRawDataWithHeader;

            error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, &io,  IID_IO, DMAP_ID,0);
            if(PR_SUCC(error))
            {
                CALL_SYS_PropertySet( io, NULL, ppMAP_AREA_ORIGIN, &data->hArcIO, sizeof(hIO) );
                CALL_SYS_PropertySetDWord( io, pgOBJECT_ACCESS_MODE, fACCESS_READ );
                CALL_SYS_PropertySetDWord( io, pgOBJECT_OPEN_MODE, fOMODE_SHARE_DENY_READ );
                qwTmp = dwAreaSize;
                CALL_SYS_PropertySet( io, NULL, ppMAP_AREA_SIZE, &qwTmp , sizeof(qwTmp ) );
                qwTmp = dwAreaPos;
                CALL_SYS_PropertySet( io, NULL, ppMAP_AREA_START, &qwTmp, sizeof(qwTmp));
                error = CALL_SYS_ObjectCreateDone( io );
            }
        }
        if(io)
        {
//Write Got IO to tmp file
            if(bChanged)
            {
                if(LocalHeader.sig == 0)
                    CreateLocalHeader(io, &LocalHeader);
                else
                    UpdateLocalHeader(io, &LocalHeader);
                error = PackFileToZIP(io, hNewZipSeqIO, &LocalHeader, Buffer, cSizeOfBuffer, entry->szName, _toui32(strlen(entry->szName)), &qwDataPosition);
            }
            else
            {
                hTempSeqIo = CreateSeqIO(io);
                if(!hTempSeqIo)
                {
                    CALL_SYS_ObjectClose( io );
                    io = NULL;
                }

                CALL_SeqIO_SeekEnd(hNewZipSeqIO, &qwDataPosition, 0);
                error = CopyFileWithCRC(hTempSeqIo,hNewZipSeqIO, NULL, Buffer, cSizeOfBuffer);
                CALL_SYS_ObjectClose(hTempSeqIo);
            }
            if(PR_SUCC(error))
            {
                WriteIntoMainDir(hMainDirSeqIO, &LocalHeader, entry->szName, &DirHeader, qwDataPosition);
                dwTotalRecords++;
            }
            else
            {
                if(io)
                {
                    CALL_SYS_ObjectClose( io );
                    io = NULL;
                }
            }
        }
        if(io)
        {
            CALL_SYS_ObjectClose( io );
            io = NULL;
        }
    }

    if (PR_SUCC(error))
    {
        error = CALL_UniArchiverCallback_GetFirstNewObject(hCallBack, &newobject);
    }

    while (PR_SUCC(error))
    {
        error = CALL_UniArchiverCallback_GetNewObjectIo(hCallBack, (hOBJECT*)&io, newobject);

        if (PR_FAIL(error))
        {
            break;
        }

        CALL_SYS_PropertyGetStr(io, NULL, pgOBJECT_NAME, szName, sizeof(szName), cCP_OEM);

        CALL_SeqIO_SeekEnd(hNewZipSeqIO, &qwDataPosition, 0);
        CreateLocalHeader(io, &LocalHeader);
        error = PackFileToZIP(io, hNewZipSeqIO, &LocalHeader, Buffer, cSizeOfBuffer, szName, _toui32(strlen(szName)), &qwDataPosition);
        if(PR_SUCC(error))
        {
            WriteIntoMainDir(hMainDirSeqIO, &LocalHeader, szName, &DirHeader,qwDataPosition);
            dwTotalRecords++;
        }

        error = CALL_UniArchiverCallback_GetNextNewObject(hCallBack, &newobject);
    }

    if (error == errEND_OF_THE_LIST || error == errOBJECT_NOT_FOUND)
    {
        error = errOK;
    }

    if (PR_SUCC(error))
    {
        WriteMainDir( hNewZipSeqIO, hMainDirSeqIO, Buffer, cSizeOfBuffer,dwTotalRecords);
    }

    CALL_SYS_ObjectClose(hNewZipSeqIO);
    CALL_SYS_ObjectClose(hMainDirSeqIO);
    CALL_SYS_ObjectClose(hMainDirIO);

    PR_TRACE_A1( _this, "Leave MiniArchiver::RebuildArchive method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



