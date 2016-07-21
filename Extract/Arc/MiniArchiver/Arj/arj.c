// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  18 December 2002,  14:00 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- arj.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define MiniArchiver_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Extract/iface/i_transformer.h>

#include <hashutil/hashutil.h>

#include "../../../Wrappers/Dmap/plugin_dmap.h"
#include "../../../Coders/xorio/plugin_xorio.h"
#include "../../Pack/UnArj/unarj.h"
#include "../../Pack/UnStored/plugin_unstore.h"
#include "../../Pack/stored/plugin_stored.h"
#include "../../Pack/ArjPack/plugin_arjpacker.h"
#include "arj.h"
#include "arj_i.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// MiniArchiver interface implementation
// Short comments -- Mini Plugin Arj
// AVP Prague stamp end


typedef struct _tag_VolumeData
{
    hIO     hIo;
    hSEQ_IO hSeqIo;
    tDWORD dwPosOfFirstFile;
    tDWORD dwNumberOfFiles;
}VolumeData;

#pragma pack(1)
typedef struct _tag_JustFileInfo
{
    tQWORD qwPosOfCompressedData;
    tQWORD qwPosOfHeader;
    tCHAR  szFileName[MAX_FILENAME_LEN];
    local_header lh;
}JustFileInfo;
#pragma pack()

typedef struct _tag_FileData
{
    JustFileInfo fi;
//  local_header lh;
//  tCHAR  szFileName[MAX_FILENAME_LEN];
//  tQWORD qwPosOfCompressedData;
//  tQWORD qwPosOfHeader;
    VolumeData sVolume;
    tDWORD dwNumberOfVolume;
}FileData;


// AVP Prague stamp begin( Data structure,  )
// Interface MiniArchiver. Inner data structure

#define cSizeOfBuffer 0x1000

typedef struct tag_ArjData 
{
    tCHAR  szArchiveName[MAX_FILENAME_LEN]; // --
    tBOOL  bIsReadOnly;                  // --
    tDWORD dwOrigin;                     // --
    tDWORD dwAvail;                      // --
    tBYTE  byFillChar;                   // --
    tWORD  wDelimeter;                   // --
    tDWORD dwOpenMode;                   // --
    tDWORD dwAccessMode;                 // --
    tBOOL  bMultiVolAsSingleSet;         // --
    tQWORD qwCurPosInPropertyGet;
    tBOOL  bMultiVolume;
    tQWORD qwCurPosInEnum;
    tBYTE* pArjHeader;
    tDWORD dwHeaderSize;
    tDWORD dwNumberOfVolumes;
    tDWORD dwNumberOfFiles;
    VolumeData* pVolume;
    FileData* pFiles;
    hHASH hMD5;
    tQWORD qwHash;
    hSEQ_IO hArcSeqIo;
    hIO hArcIoBackUp;
    hObjPtr hPtr;
    tBOOL  bSetIncomplete;
    tBYTE * Buffer;
    tCHAR  szPassword[MAX_PASSWORD_LEN]; // --
    tCHAR  szSeededPassword[MAX_PASSWORD_LEN]; // --
    tBOOL  bWarnCorrupted;
} ArjData;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_MiniArchiver 
{
    const iMiniArchiverVtbl* vtbl; // pointer to the "MiniArchiver" virtual table
    const iSYSTEMVTBL*       sys;  // pointer to the "SYSTEM" virtual table
    ArjData*                 data; // pointer to the "MiniArchiver" data structure
} *hi_MiniArchiver;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call MiniArchiver_Recognize( hOBJECT _that );
tERROR pr_call MiniArchiver_ObjectInitDone( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_ObjectInit( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_ObjectPreClose( hi_MiniArchiver _this );
// AVP Prague stamp end

tQWORD CalcHash(FileData* pFiles, hHASH hMD5, tDWORD dwStartupVolume, tDWORD dwNumberOfFiles);
tERROR OpenAllArchives(hi_MiniArchiver _this, hIO hArcIO);
tERROR SetCurPos(hi_MiniArchiver _this, tQWORD qwLastObjectId, tQWORD* result);
tERROR InitFileData(hi_MiniArchiver _this);
tERROR GetFileData(hSEQ_IO hArcIo, tDWORD* pdwLastObjectId, local_header* plh, tCHAR* szFileName);

tERROR SkipNameIo(hIO hArcIO, tDWORD* pLen, tCHAR* Buffer, tDWORD dwPos);
tERROR SkipName(hSEQ_IO hIo, tDWORD* pLen, tCHAR* szBuffer);
tINT SetArchiveName(tCHAR* szArcName, tUINT iSize, tINT iVolumeNumber);
tINT GetVolumeNumber(tCHAR* szArcName, tUINT iSize);
tVOID CreateDateTime(tWORD* time, tWORD*date);
tERROR FindStartPos(hSEQ_IO hSeqIo, tDWORD* pdwPos);
tERROR OpenNextVolume(hi_MiniArchiver _this, hIO hArcIO);
tERROR CreatePacker(hi_MiniArchiver _this, tDWORD dwMethod, hTRANSFORMER* phPacker, hSEQ_IO hInputIo, hSEQ_IO hOutputIo);
tERROR Rebuild(hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo);
tDWORD CountCRC32SeqIo(hSEQ_IO hFileSeqIo,tBYTE* Buffer, tDWORD dwBufferSize);
tDWORD CountCRC32Io(hIO hIo,tQWORD qwStartOffset, tQWORD qwSize, tBYTE* Buffer, tDWORD dwBufferSize);
tERROR SeqIoCopyFile(tBYTE* Buffer, hIO Src, tQWORD qwSrcStartPos, tDWORD dwSize, hSEQ_IO Dest);

char * __cdecl itoa (int val, char *buf, int radix);
int __cdecl atoi(const char *nptr);

#if defined(_WIN64)
	#include <string.h>
#elif defined (_WIN32)
	#if !defined(_INC_STRING)
		size_t __cdecl strlen(const tCHAR *);
		#if !defined(_INC_MEMORY)
			void* __cdecl memcpy(void*, const void*, size_t);
		#endif
	#endif
#endif

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_MiniArchiver_vtbl = 
{
    (tIntFnRecognize)        MiniArchiver_Recognize,
    (tIntFnObjectNew)        NULL,
    (tIntFnObjectInit)       MiniArchiver_ObjectInit,
    (tIntFnObjectInitDone)   MiniArchiver_ObjectInitDone,
    (tIntFnObjectCheck)      NULL,
    (tIntFnObjectPreClose)   MiniArchiver_ObjectPreClose,
    (tIntFnObjectClose)      NULL,
    (tIntFnChildNew)         NULL,
    (tIntFnChildInitDone)    NULL,
    (tIntFnChildClose)       NULL,
    (tIntFnMsgReceive)       NULL,
    (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpMiniArchiver_Reset)          ( hi_MiniArchiver _this );             // -- First call of Next method after Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_Next)           ( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId ); // -- First call of Next method after creating or Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_ObjCreate)      ( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO ); // -- Create IO from current enumerated object;
typedef   tERROR (pr_call *fnpMiniArchiver_SetAsCurrent)   ( hi_MiniArchiver _this, tQWORD qwObjectId ); // -- Set Object (qwObjectId) as currently enumerated object;
typedef   tERROR (pr_call *fnpMiniArchiver_RebuildArchive) ( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo ); // -- ;
// AVP Prague stamp end



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



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call MiniArchiver_Reset( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId );
tERROR pr_call MiniArchiver_ObjCreate( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO );
tERROR pr_call MiniArchiver_SetAsCurrent( hi_MiniArchiver _this, tQWORD qwObjectId );
tERROR pr_call MiniArchiver_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo );
// AVP Prague stamp end



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



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Arj_Prop_Get( hi_MiniArchiver _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MiniArchiver". Static(shared) property table variables
const tVERSION Version = 1; // must be READ_ONLY at runtime
const tSTRING szComments = "ARJ"; // must be READ_ONLY at runtime
const tBOOL IsSansative = cFALSE; // must be READ_ONLY at runtime
const tDWORD dwMaxLen = MAX_FILENAME_LEN; // must be READ_ONLY at runtime
const tBOOL IsSolid = cFALSE; // must be READ_ONLY at runtime
// AVP Prague stamp end

tERROR InitArj(hi_MiniArchiver _this, hIO hArcIO, tBOOL bInitOnlyOneVolume);

// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(MiniArchiver_PropTable)
    mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, Version, sizeof(Version) )
    mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComments, 4 )
    mSHARED_PROPERTY_PTR( pgNAME_CASE_SENSITIVE, IsSansative, sizeof(IsSansative) )
    mLOCAL_PROPERTY_FN( pgOBJECT_NAME, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_PATH, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, Arj_Prop_Get, NULL )
// IsSolid is cFALSE too
    mSHARED_PROPERTY_PTR( pgIS_FOLDER, IsSolid, sizeof(IsSolid) )

    mSHARED_PROPERTY_PTR( pgNAME_MAX_LEN, dwMaxLen, sizeof(dwMaxLen) )
    mLOCAL_PROPERTY_BUF( pgIS_READONLY, ArjData, bIsReadOnly, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, ArjData, dwOrigin, cPROP_BUFFER_READ )
    mSHARED_PROPERTY_PTR( pgIS_SOLID, IsSolid, sizeof(IsSolid) )
    mLOCAL_PROPERTY_BUF( pgOBJECT_AVAILABILITY, ArjData, dwAvail, cPROP_BUFFER_READ )
    mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
    mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_BUF( pgOBJECT_FILL_CHAR, ArjData, byFillChar, cPROP_BUFFER_READ_WRITE )
    mLOCAL_PROPERTY_BUF( pgNAME_DELIMITER, ArjData, wDelimeter, cPROP_BUFFER_READ_WRITE )
    mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, ArjData, dwOpenMode, cPROP_BUFFER_READ_WRITE )
    mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, ArjData, dwAccessMode, cPROP_BUFFER_READ_WRITE )
    mLOCAL_PROPERTY_FN( pgOBJECT_HASH, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_BUF ( pgMINIARC_PROP_OBJECT_ID, ArjData, qwCurPosInPropertyGet, cPROP_BUFFER_WRITE)
    mLOCAL_PROPERTY_BUF( pgMULTIVOL_AS_SINGLE_SET, ArjData, bMultiVolAsSingleSet, cPROP_BUFFER_READ_WRITE )

    mLOCAL_PROPERTY_FN( pgOBJECT_VOLUME_NAME, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_COMPRESSED_SIZE, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_SIGNATURE, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_COMPRESSION_METHOD, Arj_Prop_Get, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_PASSWORD_PROTECTED, Arj_Prop_Get, NULL )

    mSHARED_PROPERTY( pgOBJECT_NAME_CP, ((tCODEPAGE)(cCP_OEM)) )
    mSHARED_PROPERTY( pgOBJECT_PATH_CP, ((tCODEPAGE)(cCP_OEM)) )
    mSHARED_PROPERTY( pgOBJECT_FULL_NAME_CP, ((tCODEPAGE)(cCP_OEM)) )
mPROPERTY_TABLE_END(MiniArchiver_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "MiniArchiver". Register function
tERROR pr_call MiniArchiver_Register( hROOT root ) 
{
    tERROR error;

    PR_TRACE_A0( root, "Enter MiniArchiver::Register method" );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_MINIARCHIVER,                       // interface identifier
        PID_ARJ,                                // plugin identifier
        0x00000000,                             // subtype identifier
        Version,                                // interface version
        VID_ANDY,                               // interface developer
        &i_MiniArchiver_vtbl,                   // internal(kernel called) function table
        (sizeof(i_MiniArchiver_vtbl)/sizeof(tPTR)),// internal function table size
        &e_MiniArchiver_vtbl,                   // external function table
        (sizeof(e_MiniArchiver_vtbl)/sizeof(tPTR)),// external function table size
        MiniArchiver_PropTable,                 // property table
        mPROPERTY_TABLE_SIZE(MiniArchiver_PropTable),// property table size
        sizeof(ArjData),                        // memory size
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



// AVP Prague stamp begin( Property method implementation, Arj_Prop_Get )
// Interface "MiniArchiver". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- IS_FOLDER
//  -- OBJECT_SIZE_Q
//  -- OBJECT_HASH
//  -- MINIARC_PROP_OBJECT_ID
tERROR pr_call Arj_Prop_Get( hi_MiniArchiver _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
tERROR error;
tUINT ns;
ArjData* data;

    PR_TRACE_A0( _this, "Enter *GET* multyproperty method Arj_Prop_Get" );

    error = errOK;
    data = _this->data;
    if(data->pFiles == NULL || data->qwCurPosInPropertyGet == 0)
    {
        PR_TRACE_A2( _this, "Leave *GET* multyproperty method Arj_Prop_Get, ret tDWORD = %u(size), error = 0x%08x", *out_size, errOBJECT_NOT_INITIALIZED );
        return errOBJECT_NOT_INITIALIZED;
    }
    if(data->qwCurPosInPropertyGet == 0xffffffffffff)
    {
        data->qwCurPosInPropertyGet = data->qwCurPosInEnum;
    }
    else
    {
        data->qwCurPosInPropertyGet--;
    }
    if ( buffer && size )
    {
        switch ( prop ) 
        {

            case pgOBJECT_NAME:
                ns = _toui32(strlen( data->pFiles[data->qwCurPosInPropertyGet].fi.szFileName ));
                
                if ( ns+1 > size )
                {
                    error = errBUFFER_TOO_SMALL;
                    *out_size = ns+1;
                }
                else
                {
                    memcpy( buffer, data->pFiles[data->qwCurPosInPropertyGet].fi.szFileName, ns+1 );
                    *out_size = ns + 1;
                }
                break;

            case pgOBJECT_PATH:
                *out_size = 0;
                break;

            case pgOBJECT_FULL_NAME:
                ns = _toui32(strlen( data->pFiles[data->qwCurPosInPropertyGet].fi.szFileName ));
                
                if ( ns+1 > size )
                {
                    error = errBUFFER_TOO_SMALL;
                    *out_size = ns+1;
                }
                else
                {
                    memcpy( buffer, data->pFiles[data->qwCurPosInPropertyGet].fi.szFileName, ns+1 );
                    *out_size = ns + 1;
                }
                break;
            case pgOBJECT_COMPRESSED_SIZE:
                *out_size = sizeof(tDWORD);
                if(size < sizeof(tDWORD))
                {
                    error = errBUFFER_TOO_SMALL;
                }
                else
                {
                    *((tDWORD*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].fi.lh.compressed_size;
                    if(data->pFiles[data->qwCurPosInPropertyGet].fi.lh.arj_flags & VOLUME_FLAG)
                    {
                        do
                        {
                            data->qwCurPosInPropertyGet++;
                            *((tDWORD*)buffer) += data->pFiles[data->qwCurPosInPropertyGet].fi.lh.compressed_size;
                        }while(data->pFiles[data->qwCurPosInPropertyGet].fi.lh.arj_flags & VOLUME_FLAG);
                        
                    }
                        
                }
                break;
            case pgOBJECT_SIZE_Q:
                *out_size = sizeof(tQWORD);
                if(size < sizeof(tQWORD))
                {
                    error = errBUFFER_TOO_SMALL;
                }
                else
                {
                    *((tQWORD*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].fi.lh.original_size;
                    if(data->pFiles[data->qwCurPosInPropertyGet].fi.lh.arj_flags & VOLUME_FLAG)
                    {
                        do
                        {
                            data->qwCurPosInPropertyGet++;
                            *((tQWORD*)buffer) += data->pFiles[data->qwCurPosInPropertyGet].fi.lh.original_size;;
                        }while(data->pFiles[data->qwCurPosInPropertyGet].fi.lh.arj_flags & VOLUME_FLAG);
                    }
                        
                }
                break;

            case pgOBJECT_SIGNATURE:
                *out_size = sizeof(tDWORD);
                if(size < sizeof(tDWORD))
                {
                    error = errBUFFER_TOO_SMALL;
                }
                else
                {
                    *((tDWORD*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].fi.lh.original_CRC;
                }
                break;

            case pgOBJECT_COMPRESSION_METHOD:
                *out_size = sizeof(tDWORD);
                if(size < sizeof(tDWORD))
                {
                    error = errBUFFER_TOO_SMALL;
                }
                else
                {
                    *((tDWORD*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].fi.lh.method;
                }
                break;

            case pgOBJECT_PASSWORD_PROTECTED:
                *out_size = sizeof(tBOOL);
                if(size < sizeof(tBOOL))
                {
                    error = errBUFFER_TOO_SMALL;
                }
                else
                {
                    *((tBOOL*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].fi.lh.arj_flags & GARBLED_FLAG ? cTRUE : cFALSE;
                }
                break;

            case pgOBJECT_HASH:
                *out_size = sizeof(tQWORD);
                if(size < sizeof(tQWORD))
                {
                    error = errBUFFER_TOO_SMALL;
                }
                else
                {
                    *((tQWORD*)buffer) = data->qwHash;
                }
                break;

            case pgOBJECT_VOLUME_NAME:
                if (data->dwNumberOfVolumes > 1)
                {
                    error = CALL_SYS_PropertyGet(data->pFiles[data->qwCurPosInPropertyGet].sVolume.hIo, out_size, pgOBJECT_NAME, buffer, size);
                }
                else
                {
                    *out_size = 0;
                    error = errPROPERTY_NOT_FOUND;
                }
                break;

            default:
                *out_size = 0;
                break;
        }
    }
    else if ( !buffer && !size ) 
    {
        switch(prop)
        {
        case pgOBJECT_PATH:
        case pgOBJECT_NAME:
        case pgOBJECT_FULL_NAME:
            *out_size = MAX_FILENAME_LEN;
            break;
        case pgOBJECT_COMPRESSED_SIZE:
            *out_size = sizeof(tDWORD);
            break;
        case pgOBJECT_SIZE_Q:
        case pgOBJECT_HASH:
            *out_size = sizeof(tQWORD);
            break;
        }
    }
    else
    {
        error = errPARAMETER_INVALID;
        size = 0;
    }
    data->qwCurPosInPropertyGet = 0xffffffffffff;
    
    PR_TRACE_A2( _this, "Leave *GET* multyproperty method Arj_Prop_Get, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



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
tERROR error;
first_header ih;

    PR_TRACE_A2( 0, "Enter MiniArchiver::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
    error = errINTERFACE_INCOMPATIBLE;

    if(errOK == CALL_IO_SeekRead((hIO)_that, NULL, 0, &ih, sizeof(ih)))
    {
        if(ih.sig == ARJ_SIGNATURE && ih.version_needed_to_extract == 1 && ih.size < HEADERSIZE_MAX)
        {
            error = errOK;
        }
    }
    
    PR_TRACE_A2( 0, "Leave MiniArchiver::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
    return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit)
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call MiniArchiver_ObjectInit( hi_MiniArchiver _this ) 
{
ArjData* data;
tERROR error;
hIO hArcIO;
    
    PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectInitmethod" );
    
    data = _this->data;
    error = errOK;
    
    data->bIsReadOnly = cFALSE;
    data->dwAvail = fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE | fAVAIL_CHANGE_MODE | fAVAIL_DELETE_ON_CLOSE;
    
    hArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this, IID_IO);
    if(hArcIO)
    {
        data->dwOpenMode   = CALL_SYS_PropertyGetDWord( (hOBJECT)hArcIO, pgOBJECT_OPEN_MODE);
        data->dwAccessMode = CALL_SYS_PropertyGetDWord( (hOBJECT)hArcIO, pgOBJECT_ACCESS_MODE);
    }
    data->qwCurPosInPropertyGet = 0xffffffffffffffff;
    
    if(PR_SUCC(error))
    {
        error = InitArj(_this, hArcIO, cTRUE);
        if ( PR_FAIL(error) )
          return(error);

        if(data->bMultiVolume)
        {
            data->bIsReadOnly = cTRUE;
            data->dwAvail = fAVAIL_READ;
        }
    }
    
    if(data->hMD5 == NULL && PR_SUCC(error))
    {
        error = CALL_SYS_ObjectCreateQuick(_this, &data->hMD5, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY);
        if(PR_SUCC(error))
        {
            data->qwHash = CalcHash(data->pFiles, data->hMD5, 0, data->dwNumberOfFiles);
        }
        else
            data->qwHash = -1;
    }
    PR_TRACE_A1( _this, "Leave MiniArchiver::ObjectInitmethod, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call MiniArchiver_ObjectInitDone( hi_MiniArchiver _this ) 
{
ArjData* data;
tERROR error;
hIO hArcIO;
hOS hUniArcOS;
tDWORD dwLen ;

    PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectInitDone method" );

    data = _this->data;
    error = errOK;
    
    data->bIsReadOnly = cFALSE;
    data->dwAvail = fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE | fAVAIL_DELETE_ON_CLOSE;

    hArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this,IID_IO);
    if(hArcIO)
    {
        data->dwOpenMode   = CALL_SYS_PropertyGetDWord( (hOBJECT)hArcIO, pgOBJECT_OPEN_MODE);
        data->dwAccessMode = CALL_SYS_PropertyGetDWord( (hOBJECT)hArcIO, pgOBJECT_ACCESS_MODE);
    }
    data->qwCurPosInPropertyGet = 0xffffffffffffffff;
    
    if(PR_SUCC(error))
    {
        //data->bMultiVolAsSingleSet = cTRUE;
        
        if(data->bMultiVolAsSingleSet == cTRUE)
            error = InitArj(_this, hArcIO, cFALSE);
        if(data->bMultiVolume)
        {
            data->bIsReadOnly = cTRUE;
            data->dwAvail = fAVAIL_READ;
        }
    }

    hUniArcOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this,IID_OS);
    if(data->hMD5 != NULL && PR_SUCC(error))
    {
    tUINT i;
    tQWORD qwHash ;
        for(i=0; i< data->dwNumberOfVolumes; i++)
        {
            qwHash = CalcHash(data->pFiles, data->hMD5, i, data->dwNumberOfFiles);
            dwLen = sizeof(tQWORD);
            CALL_SYS_SendMsg(hUniArcOS, pmc_OS_VOLUME, pm_OS_VOLUME_HASH, data->pVolume[i].hIo , &qwHash, &dwLen);
        }
    }
    if(data->bSetIncomplete)
    {
        CALL_SYS_SendMsg(hUniArcOS, pmc_OS_VOLUME, pm_OS_VOLUME_SET_INCOMPLETE, 0, 0, 0);
    }
    if (PR_SUCC(error) && data->bWarnCorrupted)
    {
        // object is corrupted therefore becomes read only
        data->bIsReadOnly = cTRUE;
        data->dwAvail = fAVAIL_READ;
        error = warnOBJECT_DATA_CORRUPTED;
    }
    
    
    PR_TRACE_A1( _this, "Leave MiniArchiver::ObjectInitDone method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:

tERROR pr_call MiniArchiver_ObjectPreClose( hi_MiniArchiver _this )
{
ArjData* data;
    PR_TRACE_A0( _this, "Enter Transformer::ObjectPreClose method" );
    data = _this->data;

    if(data->Buffer)
        CALL_SYS_ObjHeapFree(_this, data->Buffer);
    if(data->pVolume)
        CALL_SYS_ObjHeapFree(_this, data->pVolume);
    if(data->pFiles)
        CALL_SYS_ObjHeapFree(_this, data->pFiles);
    if(data->pArjHeader)
        CALL_SYS_ObjHeapFree(_this, data->pArjHeader);
    if(data->hPtr)
        CALL_SYS_ObjectClose(data->hPtr);
    
    PR_TRACE_A1( _this, "Leave Transformer::ObjectPreClose method, ret tERROR = 0x%08x", errOK );
    return errOK;
}

// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Behaviour comment
//    Reset All internal data (variables) to default or initial values. After this call enumerator must start from the very beginning
// Parameters are:
tERROR pr_call MiniArchiver_Reset( hi_MiniArchiver _this )
{
tERROR error;

    PR_TRACE_A0( _this, "Enter MiniArchiver::Reset method" );

    error = errOK;
    _this->data->qwCurPosInEnum = 0;
    _this->data->qwCurPosInPropertyGet = 0xffffffffffff;
    
    PR_TRACE_A1( _this, "Leave MiniArchiver::Reset method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Parameters are:
tERROR pr_call MiniArchiver_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId )
{
tERROR error ;
tQWORD ret_val;

    PR_TRACE_A0( _this, "Enter MiniArchiver::Next method" );
    error = errOK;
    _this->data->qwCurPosInEnum = qwLastObjectId;
    ret_val = qwLastObjectId + 1;
    if(ret_val > _this->data->dwNumberOfFiles)
    {
        error = errEND_OF_THE_LIST;
    }
    if(_this->data->pFiles[qwLastObjectId].fi.lh.arj_flags & EXTFILE_FLAG)
    {
        return MiniArchiver_Next(_this, result, ret_val);
    }
    if(_this->data->pFiles[qwLastObjectId].fi.lh.arj_flags & VOLUME_FLAG)
    {
        if(ret_val+1 > _this->data->dwNumberOfFiles)
        {
            error = errEND_OF_THE_LIST; 
        }
    }
    
    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave MiniArchiver::Next method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ObjCreate )
// Parameters are:
tERROR pr_call MiniArchiver_ObjCreate( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO )
{
ArjData* data;
tERROR error ;
hIO hDMapIo;
hIO hXorIo;
hIO hInputIo;
hSEQ_IO hInputSeqIo;
hIO hTempOutputIo;
hSEQ_IO hOutputSeqIo;
hTRANSFORMER hCompressor;
hSTRING hPasswordString;
tQWORD qwSize, qwOffset;
tQWORD qwOriginSize;
tDWORD dwCrc;
tBOOL bPassword;
tINT i;

    PR_TRACE_A0( _this, "Enter MiniArchiver::ObjCreate method" );

    data = _this->data;
    error = errOK;
    hCompressor = NULL;
    hPasswordString = NULL;
    hDMapIo = NULL;
    hXorIo = NULL;
    hInputIo = NULL;
    hInputSeqIo = NULL;
    hTempOutputIo = NULL;
    hOutputSeqIo = NULL;

    bPassword = cFALSE;
    
    qwObjectIdToCreate --;
    if(data->pFiles[qwObjectIdToCreate].fi.lh.arj_flags & EXTFILE_FLAG)
    {
        if ( result )
            *result = NULL;
        return errUNEXPECTED;
    }
    if(data->pFiles[qwObjectIdToCreate].fi.lh.arj_flags & VOLUME_FLAG && qwObjectIdToCreate+1 >= data->dwNumberOfFiles)
    {
        if ( result )
            *result = NULL;
        return errUNEXPECTED;
    }

    error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&hDMapIo,  IID_IO, DMAP_ID,0);
    if (PR_FAIL(error)) goto loc_exit;

	error = CALL_IO_GetSize(data->pFiles[qwObjectIdToCreate].sVolume.hIo, &qwOriginSize, IO_SIZE_TYPE_EXPLICIT);
	if (PR_FAIL(error)) goto loc_exit;

	if (data->pFiles[qwObjectIdToCreate].fi.qwPosOfCompressedData > qwOriginSize ||
		data->pFiles[qwObjectIdToCreate].fi.qwPosOfCompressedData + 
		data->pFiles[qwObjectIdToCreate].fi.lh.compressed_size > qwOriginSize)
	{
		error = errOBJECT_DATA_CORRUPTED;
		goto loc_exit;
	}

    CALL_SYS_PropertySetQWord( hDMapIo, ppMAP_AREA_START, data->pFiles[qwObjectIdToCreate].fi.qwPosOfCompressedData);
    CALL_SYS_PropertySetQWord( hDMapIo, ppMAP_AREA_SIZE, (tQWORD)data->pFiles[qwObjectIdToCreate].fi.lh.compressed_size );
    CALL_SYS_PropertySetObj  ( hDMapIo, ppMAP_AREA_ORIGIN, (hOBJECT)data->pFiles[qwObjectIdToCreate].sVolume.hIo );

    error = CALL_SYS_ObjectCreateDone(hDMapIo);
    if (PR_FAIL(error)) goto loc_exit;

    if(data->pFiles[qwObjectIdToCreate].fi.lh.arj_flags & GARBLED_FLAG)
    {
        bPassword = cTRUE;

        error=(tERROR)CALL_SYS_ObjectCreate((hOBJECT)hDMapIo,(hOBJECT*)&hXorIo, IID_IO, PID_XORIO,0);
        if (PR_FAIL(error)) goto loc_exit;

        CALL_SYS_PropertySetObj( hXorIo, ppXOR_ORIGIN, (hOBJECT)hDMapIo);

        error = CALL_SYS_ObjectCreateQuick( _this, &hPasswordString, IID_STRING, PID_ANY, SUBTYPE_ANY );
        if (PR_FAIL(error)) goto loc_exit;

        if(!data->szPassword[0])
        {

loc_reaskpasswd:

            error = CALL_SYS_SendMsg (hUniArcIO, pmc_IO, pm_IO_PASSWORD_REQUEST, hPasswordString, NULL, NULL);

            if( error != errOK && error != errOK_DECIDED)
            {
                error = errOBJECT_PASSWORD_PROTECTED;
                goto loc_exit;
            }

            CALL_String_ExportToBuff( hPasswordString, NULL, cSTRING_WHOLE, data->szPassword, MAX_PASSWORD_LEN, cCP_ANSI, cSTRING_Z);
        }

        i = _toui32(strlen(data->szPassword));
        data->szSeededPassword[i] = '\0';

        for (--i; i >= 0; --i)
        {
            data->szSeededPassword[i] = data->szPassword[i] + data->pFiles[qwObjectIdToCreate].fi.lh.password_modifier;
        }

        CALL_SYS_PropertySet     ( hXorIo, NULL, ppXOR_KEY, data->szSeededPassword, _toui32(strlen(data->szSeededPassword)));
        CALL_SYS_PropertySetDWord( hXorIo, ppXOR_KEYSIZE, _toui32(strlen(data->szSeededPassword)));

        error = CALL_SYS_ObjectCreateDone(hXorIo);
        if (PR_FAIL(error)) goto loc_exit;

        if (!data->Buffer)
        {
          error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->Buffer, cSizeOfBuffer+16000);
          if ( PR_FAIL(error) )
            goto loc_exit;
        }

        hInputIo = hXorIo;
    }
    else
    {
        hInputIo = hDMapIo;
    }
    

    error = CALL_SYS_ObjectCreateQuick(hInputIo, &hInputSeqIo, IID_SEQIO, PID_ANY,0);
    if (PR_FAIL(error)) goto loc_exit;

    error = CALL_SYS_ObjectCreate((hOBJECT)_this,(hOBJECT*)&hTempOutputIo, IID_IO, PID_TMPFILE,0);
    if (PR_FAIL(error)) goto loc_exit;

    CALL_SYS_PropertySetDWord( (hOBJECT)hTempOutputIo, pgOBJECT_ACCESS_MODE, fACCESS_RW);
    CALL_SYS_PropertySetDWord( (hOBJECT)hTempOutputIo, pgOBJECT_OPEN_MODE,   fOMODE_CREATE_ALWAYS);// + fOMODE_SHARE_DENY_READ);

    error = CALL_SYS_ObjectCreateDone(hTempOutputIo);
    if (PR_FAIL(error)) goto loc_exit;

    error = CALL_SYS_ObjectCreateQuick(hTempOutputIo, &hOutputSeqIo, IID_SEQIO, PID_ANY,0);
    if (PR_FAIL(error)) goto loc_exit;

    if(data->pFiles[qwObjectIdToCreate].fi.lh.method == STORED)
    {
        error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&hCompressor, IID_TRANSFORMER, PID_UNSTORE,0);
        if (PR_FAIL(error)) goto loc_exit;
    }
    else
    {
        error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&hCompressor, IID_TRANSFORMER, PID_UNARJ,0);
        if (PR_FAIL(error)) goto loc_exit;
    }


    CALL_SYS_PropertySetDWord( (hOBJECT)hCompressor, plDECOMPRESSON_METHOD, data->pFiles[qwObjectIdToCreate].fi.lh.method);
    
    CALL_SYS_PropertySetQWord(hCompressor, pgTRANSFORM_SIZE, data->pFiles[qwObjectIdToCreate].fi.lh.original_size);
    
    CALL_SYS_PropertySetObj(hCompressor, pgOUTPUT_SEQ_IO, (hOBJECT)hOutputSeqIo);
    CALL_SYS_PropertySetObj(hCompressor, pgINPUT_SEQ_IO, (hOBJECT)hInputSeqIo);

    error = CALL_SYS_ObjectCreateDone(hCompressor);
    if (PR_FAIL(error)) goto loc_exit;

    qwOffset = 0;

    error = CALL_Transformer_Proccess(hCompressor, &qwSize);
    if (PR_FAIL(error))
    {
        if (bPassword)
        {
            goto loc_reaskpasswd;
        }
        else
        {
            goto loc_exit;
        }
    }

    if (bPassword)
    {
        dwCrc = CountCRC32Io(hTempOutputIo, qwOffset, qwSize, data->Buffer, cSizeOfBuffer);
        if (data->pFiles[qwObjectIdToCreate].fi.lh.original_CRC != dwCrc)
        {
            goto loc_reaskpasswd;
        }
    }
    qwOffset += qwSize;
    
    if(data->pFiles[qwObjectIdToCreate].fi.lh.arj_flags & VOLUME_FLAG && PR_SUCC(error))
    {
        do
        {
            qwObjectIdToCreate++;
            CALL_SYS_PropertySetQWord( hDMapIo, ppMAP_AREA_START, data->pFiles[qwObjectIdToCreate].fi.qwPosOfCompressedData);
            CALL_SYS_PropertySetQWord( hDMapIo, ppMAP_AREA_SIZE, (tQWORD)data->pFiles[qwObjectIdToCreate].fi.lh.compressed_size);
            CALL_SYS_PropertySetObj  ( hDMapIo, ppMAP_AREA_ORIGIN, (hOBJECT)data->pFiles[qwObjectIdToCreate].sVolume.hIo);

            if (bPassword)
            {
                // reseed the password with seed from next file
                i = _toui32(strlen(data->szPassword));
                data->szSeededPassword[i] = '\0';

                for (--i; i >= 0; --i)
                {
                    data->szSeededPassword[i] = data->szPassword[i] + data->pFiles[qwObjectIdToCreate].fi.lh.password_modifier;
                }

                CALL_SYS_PropertySet     ( hXorIo, NULL, ppXOR_KEY, data->szSeededPassword, _toui32(strlen(data->szSeededPassword)));
                CALL_SYS_PropertySetDWord( hXorIo, ppXOR_KEYSIZE, _toui32(strlen(data->szSeededPassword)));
            }

            CALL_SYS_ObjectClose(hInputSeqIo);

            error = CALL_SYS_ObjectCreateQuick(hInputIo, &hInputSeqIo, IID_SEQIO, PID_ANY,0);
            if (PR_FAIL(error)) goto loc_exit;
            
            CALL_SYS_ObjectClose(hCompressor);
            hCompressor = NULL;

            if(data->pFiles[qwObjectIdToCreate].fi.lh.method == STORED)
            {
                error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&hCompressor, IID_TRANSFORMER, PID_UNSTORE,0);
                if (PR_FAIL(error)) goto loc_exit;
            }
            else
            {
                error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&hCompressor, IID_TRANSFORMER, PID_UNARJ,0);
                if (PR_FAIL(error)) goto loc_exit;
            }
            
            CALL_SYS_PropertySetDWord(hCompressor, plDECOMPRESSON_METHOD, data->pFiles[qwObjectIdToCreate].fi.lh.method);
            CALL_SYS_PropertySetQWord(hCompressor, pgTRANSFORM_SIZE, data->pFiles[qwObjectIdToCreate].fi.lh.original_size);

            CALL_SYS_PropertySetObj(hCompressor, pgOUTPUT_SEQ_IO, (hOBJECT)hOutputSeqIo);
            CALL_SYS_PropertySetObj(hCompressor, pgINPUT_SEQ_IO, (hOBJECT)hInputSeqIo);

            error = CALL_SYS_ObjectCreateDone(hCompressor);
            if (PR_FAIL(error)) goto loc_exit;

            error = CALL_Transformer_Proccess(hCompressor, &qwSize);
            if (PR_FAIL(error))
            {
                if (bPassword)
                {
                    goto loc_reaskpasswd;
                }
                else
                {
                    goto loc_exit;
                }
            }

            if (bPassword)
            {
                dwCrc = CountCRC32Io(hTempOutputIo, qwOffset, qwSize, data->Buffer, cSizeOfBuffer);
                if (data->pFiles[qwObjectIdToCreate].fi.lh.original_CRC != dwCrc)
                {
                    goto loc_reaskpasswd;
                }
            }
            qwOffset += qwSize;
        }
        while(data->pFiles[qwObjectIdToCreate].fi.lh.arj_flags & VOLUME_FLAG && PR_SUCC(error));
    }

loc_exit:    
    if (PR_FAIL(error))
    {
        if(hTempOutputIo)
        {
            CALL_SYS_ObjectClose(hTempOutputIo);
        }
        hTempOutputIo = NULL;
    }
    
    if(hCompressor)
    {
        CALL_SYS_ObjectClose(hCompressor);
    }
    if(hPasswordString)
    {
        CALL_SYS_ObjectClose(hPasswordString);
    }
    if(hXorIo)
    {
        CALL_SYS_ObjectClose(hXorIo);
    }
    if(hDMapIo)
    {
        CALL_SYS_ObjectClose(hDMapIo);
    }
    if ( result )
        *result = hTempOutputIo;
    PR_TRACE_A2( _this, "Leave MiniArchiver::ObjCreate method, ret hIO = %p, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetAsCurrent )
// Parameters are:
tERROR pr_call MiniArchiver_SetAsCurrent( hi_MiniArchiver _this, tQWORD qwObjectId )
{
tERROR error;

    PR_TRACE_A0( _this, "Enter MiniArchiver::SetAsCurrent method" );

    error = errOK;

    _this->data->qwCurPosInEnum = qwObjectId;
    //error = SetCurPos(_this, qwObjectId, NULL)
    
    PR_TRACE_A1( _this, "Leave MiniArchiver::SetAsCurrent method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, RebuildArchive )
// Parameters are:
tERROR pr_call MiniArchiver_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo )
{
tERROR error;

    PR_TRACE_A0( _this, "Enter MiniArchiver::RebuildArchive method" );

    error = Rebuild(_this, hCallBack, hOutputIo);

    PR_TRACE_A1( _this, "Leave MiniArchiver::RebuildArchive method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end




tERROR InitArj(hi_MiniArchiver _this, hIO hArcIO, tBOOL bInitOnlyOneVolume)
{
ArjData* data;
tERROR error;
tDWORD dwPos;
tDWORD dwLen;
tWORD wword;
tDWORD dwPosOfFirstFile;
first_header fh;

    data = _this->data;
    if(hArcIO == NULL)
        return errOBJECT_NOT_CREATED;
    
    if(data->hArcSeqIo == NULL)
    {
        data->hArcIoBackUp = hArcIO;
        if(PR_FAIL(CALL_SYS_ObjectCreateQuick(hArcIO, &data->hArcSeqIo, IID_SEQIO, PID_ANY,0)))
        {
            return errOBJECT_NOT_CREATED;
        }
    }
    error = CALL_SeqIO_SeekRead(data->hArcSeqIo, &dwPos, 0, &fh, sizeof(first_header));
    if(PR_FAIL(error))
    {
        return error;
    }
    

    //  if(fh.size >= FIRST_HDR_SIZE_V)
//  {
//      CALL_SeqIO_Seek(data->hArcIo, NULL, sizeof(tBYTE)*2 + sizeof(tWORD), cSEEK_FORWARD);
//  }

    CALL_SeqIO_Seek(data->hArcSeqIo, NULL, sizeof(tWORD)*2 + fh.first_size , cSEEK_SET);
    
    dwPosOfFirstFile = fh.first_size + sizeof(tWORD)*2;
    if(PR_FAIL(error = SkipName(data->hArcSeqIo, &dwLen, data->szArchiveName)))
    {
        return error;
    }
    dwPosOfFirstFile += dwLen;
    if(PR_FAIL(error = SkipName(data->hArcSeqIo, &dwLen, NULL)))
    {
        return error;
    }
    dwPosOfFirstFile += dwLen;
    dwPosOfFirstFile += sizeof(tDWORD) + sizeof(tWORD);
    CALL_SeqIO_Seek(data->hArcSeqIo, NULL, sizeof(tDWORD) , cSEEK_FORWARD);
    error = CALL_SeqIO_ReadWord(data->hArcSeqIo, &wword);
    if(wword != 0)
    {
        dwPosOfFirstFile += wword + sizeof(tDWORD);
    }

// Pre Init
    if(data->pVolume)
    {
        CALL_SYS_ObjHeapFree(_this, data->pVolume);
    }
    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->pVolume, sizeof(VolumeData) * 20);
    if ( PR_FAIL(error) )
      return(error);

    data->dwNumberOfVolumes = 1;
    data->pVolume[0].hIo = hArcIO;
    data->pVolume[0].hSeqIo = data->hArcSeqIo;
    data->pVolume[0].dwPosOfFirstFile = dwPosOfFirstFile;

    data->dwHeaderSize = dwPosOfFirstFile;
    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->pArjHeader, dwPosOfFirstFile + 16000);
    if ( PR_FAIL(error) )
      return(error);

    if(PR_SUCC(error))
    {
        CALL_SeqIO_SeekRead(data->hArcSeqIo, &dwPos, 0, data->pArjHeader, dwPosOfFirstFile);
    }
    else
    {
        error = errOK;
        data->bIsReadOnly = cTRUE;
    }
    
    
    if(!bInitOnlyOneVolume)
    {
        if ( PR_FAIL(error = OpenAllArchives(_this, hArcIO)) )
          return(error);
    }

    //if(fh.arj_flags & SECURED_FLAG)
//  {
        //secured_size=origsize;
        //arjsec_offset=file_crc;
//  }

    if ( PR_FAIL(error = InitFileData(_this)) )
      return(error);

    if(data->dwNumberOfVolumes > 1 || fh.arj_flags & VOLUME_FLAG)
    {
        data->bMultiVolume = cTRUE;
	
		PR_TRACE((_this, prtIMPORTANT, "arj\tvolumes count %d", data->dwNumberOfVolumes));
    }
    if(data->bMultiVolume)
    {
        if (bInitOnlyOneVolume)
        {
            if(data->pFiles[data->dwNumberOfFiles-1].fi.lh.arj_flags & VOLUME_FLAG )
            {
                // Open NextVol
                if(PR_FAIL(OpenNextVolume(_this, hArcIO)))
                {
                    data->dwNumberOfFiles --;

                    data->bSetIncomplete = cTRUE;
                }
            }
        }
        else
        {
            if(data->pFiles[0].fi.lh.arj_flags & EXTFILE_FLAG)
            {
                data->bSetIncomplete = cTRUE;
            }
            if(data->pFiles[data->dwNumberOfFiles-1].fi.lh.arj_flags & VOLUME_FLAG)
            {
                data->bSetIncomplete = cTRUE;
            }
        }
    }

    return errOK;
}

tQWORD CalcHash(FileData* pFiles, hHASH hMD5, tDWORD dwStartupVolume, tDWORD dwNumberOfFiles)
{
tQWORD result;
tBYTE Hash[32];
tDWORD i;
    
    CALL_Hash_Reset(hMD5);
    for(i=0;i<dwNumberOfFiles;i++)
    {
        if(pFiles[i].dwNumberOfVolume == dwStartupVolume)
        {
            CALL_Hash_Update(hMD5, (tBYTE*)&pFiles[i].fi, sizeof(JustFileInfo));
        }
    }
    
    CALL_Hash_GetHash(hMD5, Hash, sizeof(Hash));
    result = *(tQWORD*)&Hash[0];
    result^= *(tQWORD*)&Hash[8];
    return result;
}

tERROR OpenAllArchives(hi_MiniArchiver _this, hIO hArcIO)
{
    tERROR error;
    ArjData* data;
    tCHAR* szFileName;
    tUINT iSize;
    tINT iArcNumber, iArc;
    hSTRING name;
    hOBJECT hNameIO;
    hOBJECT hIO;
    hOS hOs;

    error = errOK;
    data = _this->data;
    name = NULL;
    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&szFileName, MAX_FILENAME_LEN + 16000);
    if(PR_FAIL(error))
    {
        return error;
    }

    if (data->hPtr == NULL)
    {
        if( (hOs = (hOS)CALL_SYS_ParentGet(hArcIO, IID_OS)) != NULL )
        {
            hIO = (hOBJECT)(hArcIO);
            while ( PR_FAIL(error = CALL_OS_PtrCreate(hOs, &data->hPtr, hIO)) )
            {
                if ( (hIO = CALL_SYS_ParentGet(hIO, IID_IO)) == NULL )
                {
                    data->hPtr = NULL;
                    break;
                }
            }
        }
        else
        {
            data->hPtr = (hObjPtr)CALL_SYS_ParentGet(hArcIO, IID_OBJPTR);
            if(data->hPtr)
            {
                if (PR_FAIL(CALL_ObjPtr_Clone(data->hPtr, &data->hPtr)))
                {
                    data->hPtr = NULL;
                }
                else
                {
                    hIO = (hOBJECT)(hArcIO);
                    while ( PR_FAIL(error = CALL_ObjPtr_ChangeTo(data->hPtr, hIO)) )
                    {
                        if ( (hIO = CALL_SYS_ParentGet(hIO, IID_IO)) == NULL )
                        {
                            CALL_SYS_ObjectClose(data->hPtr);
                            data->hPtr = NULL;
                            break;
                        }
                    }
                }
            }
            else
            {
                if ((tPID)CALL_SYS_PropertyGetDWord((hOBJECT)hArcIO, pgPLUGIN_ID) == PID_NATIVE_FIO)
                {
                    if (PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &data->hPtr, IID_OBJPTR, PID_NATIVE_FIO, SUBTYPE_ANY)))
                    {
                        if (PR_FAIL(CALL_ObjPtr_ChangeTo(data->hPtr, (hOBJECT)hArcIO)))
                        {
                            CALL_SYS_ObjectClose(data->hPtr);
                            data->hPtr = NULL;
                        }
                    }
                }
            }
        }
    }

    error = CALL_SYS_ObjectCreateQuick( _this, &name, IID_STRING, PID_ANY, SUBTYPE_ANY );

    if(PR_FAIL(error) || data->hPtr == NULL)
    {
        goto loc_exit;
    }

    for ( hNameIO = (hOBJECT)(hArcIO), szFileName[0] = '\0'; hNameIO && szFileName[0] == '\0'; )
    {
      error = CALL_SYS_PropertyGetStr(hNameIO, &iSize, pgOBJECT_NAME, szFileName, MAX_FILENAME_LEN, cCP_ANSI);
      hNameIO = CALL_SYS_ParentGet(hNameIO, IID_IO);
    }
    if ( PR_FAIL(error) || szFileName[0] == '\0' )
    {
        goto loc_exit;
    }

    iArcNumber = GetVolumeNumber(szFileName, iSize);
    iArc = 0;
    if(iArcNumber != 0)
    {
        if( 0!= SetArchiveName(szFileName, iSize, iArc))
        {
            error = errNOT_OK;
            goto loc_exit;
        }
    }

    data->dwNumberOfVolumes = 0;
    if(data->pVolume)
    {
        CALL_SYS_ObjHeapFree(_this, data->pVolume);
    }
    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->pVolume, sizeof(VolumeData) * 30);
    if ( PR_FAIL(error) )
      return(error);
	
	PR_TRACE((_this, prtIMPORTANT, "arj\tvolumes %d to %d (isize %d - %s)", iArc, iArcNumber, iSize, szFileName));

    while(PR_SUCC(error))
    {
        if (iArc != iArcNumber)
        {
            CALL_String_ImportFromBuff( name, 0, szFileName, _toui32(strlen(szFileName)), cCP_ANSI, 0);
            error = CALL_ObjPtr_IOCreate(data->hPtr, &data->pVolume[data->dwNumberOfVolumes].hIo, name,  data->dwAccessMode, fOMODE_OPEN_IF_EXIST );

			PR_TRACE((_this, prtIMPORTANT, "arj\topen volume %d result %terr", iArc, error));
        }
        else // we're trying to open the same volume
        {
            data->pVolume[data->dwNumberOfVolumes].hIo = hArcIO;
        }

        iArc++;

        if(PR_SUCC(error))
        {
            error = CALL_SYS_ObjectCreateQuick((hOBJECT)data->pVolume[data->dwNumberOfVolumes].hIo, &data->pVolume[data->dwNumberOfVolumes].hSeqIo, IID_SEQIO, PID_ANY,0);
            if(PR_SUCC(error))
            {
                if (PR_SUCC(error = FindStartPos(data->pVolume[data->dwNumberOfVolumes].hSeqIo, &data->pVolume[data->dwNumberOfVolumes].dwPosOfFirstFile)))
                {
                    data->dwNumberOfVolumes++;
                    error = CALL_SYS_ObjHeapRealloc(_this, (tPTR*)&data->pVolume, data->pVolume, sizeof(VolumeData) * (data->dwNumberOfVolumes + 20));
                    if ( PR_FAIL(error) || !data->pVolume )
                      return(errNOT_ENOUGH_MEMORY);
                }
            }
        }
        else if ( error == errOBJECT_NOT_FOUND )
        {
          if ( data->dwNumberOfVolumes > 0 )
          {
            /* no more volumes */
            error = errOK;
            goto loc_exit;
          }
        }

        if(PR_FAIL(error) && iArc <= iArcNumber)
        {
            error = errOK;
            PR_TRACE((_this, prtIMPORTANT, "arj\tswitch to ok"));
        }
        if( 0!= SetArchiveName(szFileName, iSize, iArc))
        {
            goto loc_exit;
        }
    }
    
loc_exit:
    if ( PR_FAIL(error) && data->dwNumberOfVolumes > 0 )
    {
       /* no more volumes */
       error = errOK;
    }
    if(name)
    {
        CALL_SYS_ObjectClose(name);
    }
    CALL_SYS_ObjHeapFree(_this, szFileName);
    return error;
}



tERROR InitFileData(hi_MiniArchiver _this)
{
ArjData* data;
tERROR error;
tDWORD dwLastObjectId;
tUINT i;

    data = _this->data;
    if(data->pFiles)
    {
        CALL_SYS_ObjHeapFree(_this, data->pFiles);
    }
    
    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->pFiles, sizeof(FileData) * 20);
    if ( PR_FAIL(error) || !data->pFiles )
      return(errNOT_ENOUGH_MEMORY);

    error = errOK;
// Count Number of Files
    data->dwNumberOfFiles = 0;
    for(i=0;i<data->dwNumberOfVolumes; i++)
    {
        dwLastObjectId = (tDWORD)data->pVolume[i].dwPosOfFirstFile;
        data->pVolume[i].dwNumberOfFiles = 0;
        do
        {
            data->pFiles[data->dwNumberOfFiles].fi.qwPosOfHeader = dwLastObjectId;
            error = GetFileData(data->pVolume[i].hSeqIo, &dwLastObjectId, &data->pFiles[data->dwNumberOfFiles].fi.lh, data->pFiles[data->dwNumberOfFiles].fi.szFileName);
            if (error == errEND_OF_THE_LIST)
            {
                error = errOK;
                break;
            }
            if (PR_FAIL(error))
            {
                data->bWarnCorrupted = cTRUE;
                error = errOK;
                break;
            }

            if(data->pFiles[data->dwNumberOfFiles].fi.lh.arj_flags & EXTFILE_FLAG)
            {
                data->bMultiVolume = cTRUE;
            }
            data->pFiles[data->dwNumberOfFiles].fi.qwPosOfCompressedData = dwLastObjectId - data->pFiles[data->dwNumberOfFiles].fi.lh.compressed_size;
            memcpy(&data->pFiles[data->dwNumberOfFiles].sVolume, &data->pVolume[i], sizeof(VolumeData));
            data->pFiles[data->dwNumberOfFiles].dwNumberOfVolume = i;
            data->dwNumberOfFiles++;
            data->pVolume[i].dwNumberOfFiles++;
            error = CALL_SYS_ObjHeapRealloc(_this, (tPTR*)&data->pFiles, data->pFiles,  (data->dwNumberOfFiles + 10) * sizeof(FileData));
            
        }while(PR_SUCC(error) );
    }
    return error;
}


tERROR GetFileData(hSEQ_IO hArcIo, tDWORD *pdwLastObjectId, local_header* plh, tCHAR* szFileName)
{
tERROR error = errOK;
tDWORD ret;
tWORD wExtHeader;
tDWORD dwLastObjectId = *pdwLastObjectId;

    CALL_SeqIO_Seek(hArcIo, NULL, dwLastObjectId, cSEEK_SET);
    error = CALL_SeqIO_Read(hArcIo, &ret, plh, sizeof(local_header));
    if(ret == sizeof(tWORD)+sizeof(tWORD)) // terminal header
    {
        return errEND_OF_THE_LIST;
    }
    if(error == errEOF || error == errOUT_OF_OBJECT || ret != sizeof(local_header))
    {
        return errOBJECT_DATA_CORRUPTED;
    }
    if(plh->sig != ARJ_SIGNATURE)
    {
        return errOBJECT_DATA_CORRUPTED;
    }
    if(plh->size == 0)
    {
        return errOBJECT_DATA_CORRUPTED; // terminal header with some stuff after it
    }
    if(PR_SUCC(error))
    {
        CALL_SeqIO_Seek(hArcIo, NULL, (tQWORD)dwLastObjectId + plh->first_size + sizeof(tDWORD) , cSEEK_SET);
        // Skip Partial data
/*      if(plh->arj_flags & EXTFILE_FLAG)
        {
            //return MiniArchiver_Next(_this, result, qwLastObjectId);
            //continued_prevvolume=1;
        }
        if(plh->size < R9_HDR_SIZE)
        {
            if(plh->arj_flags & EXTFILE_FLAG)
            {
                CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tDWORD), cSEEK_FORWARD);
                //resume_position=hget_longword();
                //continued_prevvolume=1;
            }
        }
        else                                  // v 2.62+ - resume position is stored anyway, atime/ctime follows it.
        {
            CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tDWORD), cSEEK_FORWARD);
            //resume_position=hget_longword();
            CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tDWORD), cSEEK_FORWARD);
            //ts_store(&atime_stamp, host_os, hget_longword());
            CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tDWORD), cSEEK_FORWARD);
            //ts_store(&ctime_stamp, host_os, hget_longword());
            CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tDWORD), cSEEK_FORWARD);
            //hget_longword();                     // Reserved in revision 9 headers
        }*/
        if (PR_FAIL(error = SkipName(hArcIo, NULL, szFileName)))
        {
            return error;
        }
        if (PR_FAIL(error = SkipName(hArcIo, NULL, NULL)))
        {
            return error;
        }
        dwLastObjectId += plh->size + sizeof(tDWORD);// Signature
        dwLastObjectId += sizeof(tDWORD);//basic header CRC
loc_ReadExtHeaderAgain:
        CALL_SeqIO_Seek(hArcIo, NULL, (tQWORD)dwLastObjectId , cSEEK_SET);
        wExtHeader = 0;
        CALL_SeqIO_Read(hArcIo, NULL, &wExtHeader, sizeof(tWORD));  //1st extended header size
        dwLastObjectId += wExtHeader + sizeof(tWORD);//wExtHeader
        if(wExtHeader !=0)
        {
            dwLastObjectId += sizeof(tDWORD);//1st extended header's CRC
            goto loc_ReadExtHeaderAgain;
        }
        dwLastObjectId += plh->compressed_size;
        
    }

    *pdwLastObjectId = dwLastObjectId;

    return error;
}

tERROR FindStartPos(hSEQ_IO hArcIo, tDWORD *pdwPos)
{
tERROR error;
first_header fh;
tDWORD ret;
tWORD wword;
tDWORD dwLen;

    CALL_SeqIO_Seek(hArcIo, NULL, 0, cSEEK_SET);
    error = CALL_SeqIO_Read(hArcIo, NULL, &fh, sizeof(first_header));
    if (error != errOK)
    {
        return errOBJECT_DATA_CORRUPTED;
    }
    CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tWORD)*2 + fh.first_size , cSEEK_SET);
    
    ret = fh.first_size + sizeof(tWORD)*2;
    if (PR_FAIL(error = SkipName(hArcIo, &dwLen, NULL)))
    {
        return error;
    }
    ret += dwLen;
    if (PR_FAIL(error = SkipName(hArcIo, &dwLen, NULL)))
    {
        return error;
    }
    ret += dwLen;
    ret += sizeof(tDWORD) + sizeof(tWORD);
    CALL_SeqIO_Seek(hArcIo, NULL, sizeof(tDWORD) , cSEEK_FORWARD);
    CALL_SeqIO_ReadWord(hArcIo, &wword);
    if(wword != 0)
    {
        ret += wword + sizeof(tDWORD);
    }
    if (pdwPos)
    {
        *pdwPos = ret;
    }
    return errOK;
}

tERROR OpenNextVolume(hi_MiniArchiver _this, hIO hArcIO)
{
tCHAR* szFileName;
tERROR error;
ArjData* data;
tUINT iSize;
tINT iArcNumber;
hSTRING name;
tDWORD dwLastObjectId;
hOS hOs;
hOBJECT hIO;

    error = errOK;
    data = _this->data;

    name = NULL;

    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&szFileName, MAX_FILENAME_LEN + 16000);
    if(PR_FAIL(error))
    {
        return error;
    }


    if (data->hPtr == NULL)
    {
        if( (hOs = (hOS)CALL_SYS_ParentGet(hArcIO, IID_OS)) != NULL )
        {
            hIO = (hOBJECT)(hArcIO);
            while ( PR_FAIL(error = CALL_OS_PtrCreate(hOs, &data->hPtr, hIO)) )
            {
                if ( NULL == (hIO = CALL_SYS_ParentGet(hIO, IID_IO)) )
                {
                    data->hPtr = NULL;
                    break;
                }
            }
        }
        else
        {
            data->hPtr = (hObjPtr)CALL_SYS_ParentGet(hArcIO, IID_OBJPTR);
            if(data->hPtr)
            {
                if (PR_FAIL(CALL_ObjPtr_Clone(data->hPtr, &data->hPtr)))
                {
                    data->hPtr = NULL;
                }
                else
                {
                    hIO = (hOBJECT)(hArcIO);
                    while ( PR_FAIL(error = CALL_ObjPtr_ChangeTo(data->hPtr, hIO)) )
                    {
                        if ( (hIO = CALL_SYS_ParentGet(hIO, IID_IO)) == NULL )
                        {
                            CALL_SYS_ObjectClose(data->hPtr);
                            data->hPtr = NULL;
                            break;
                        }
                    }
                }
            }
        }
    }

    error = CALL_SYS_ObjectCreateQuick( _this, &name, IID_STRING, PID_ANY, SUBTYPE_ANY );

    if(PR_FAIL(error) || data->hPtr == NULL)
    {
        goto loc_exit;
    }
    
    error = CALL_SYS_PropertyGetStr(hArcIO, &iSize, pgOBJECT_NAME, szFileName, MAX_FILENAME_LEN, cCP_ANSI);
    if(PR_FAIL(error))
    {
        goto loc_exit;
    }
    iArcNumber = GetVolumeNumber(szFileName, iSize-1);
    if( 0!= SetArchiveName(szFileName, iSize-1, iArcNumber + 1))
    {
        error = errNOT_OK;
        goto loc_exit;
    }
    
    if(PR_SUCC(error))
    {
        CALL_String_ImportFromBuff( name, 0, szFileName, _toui32(strlen(szFileName)), cCP_ANSI, 0);

        error = CALL_SYS_ObjHeapRealloc(_this, (tPTR*)&data->pVolume, data->pVolume, sizeof(VolumeData) * (data->dwNumberOfVolumes + 20));

        error = CALL_ObjPtr_IOCreate(data->hPtr, &data->pVolume[data->dwNumberOfVolumes].hIo, name,  data->dwAccessMode, fOMODE_OPEN_IF_EXIST );
        if(PR_SUCC(error))
        {
            error = CALL_SYS_ObjectCreateQuick((hOBJECT)data->pVolume[data->dwNumberOfVolumes].hIo, &data->pVolume[data->dwNumberOfVolumes].hSeqIo, IID_SEQIO, PID_ANY,0);
            if(PR_SUCC(error))
            {
                error = FindStartPos(data->pVolume[data->dwNumberOfVolumes].hSeqIo, &data->pVolume[data->dwNumberOfVolumes].dwPosOfFirstFile);
                if(PR_FAIL(error))
                {
                    goto loc_exit;
                }

                error = CALL_SYS_ObjHeapRealloc(_this, (tPTR*)&data->pFiles, data->pFiles,  (data->dwNumberOfFiles + 10) * sizeof(FileData));
                if(PR_FAIL(error))
                {
                    return error;
                }

                dwLastObjectId = data->pVolume[data->dwNumberOfVolumes].dwPosOfFirstFile;

                error = GetFileData(data->pVolume[data->dwNumberOfVolumes].hSeqIo, 
                    &dwLastObjectId, 
                    &data->pFiles[data->dwNumberOfFiles].fi.lh, 
                    data->pFiles[data->dwNumberOfFiles].fi.szFileName);

                if(error == errEND_OF_THE_LIST)
                {
                    error = errOK;
                }
                else if (PR_FAIL(error))
                {
                    data->bWarnCorrupted = cTRUE;
                    error = errOK;
                }
                else
                {
                    if(data->pFiles[data->dwNumberOfFiles].fi.lh.arj_flags & EXTFILE_FLAG)
                    {
                        data->bMultiVolume = cTRUE;
                    }
                    data->pFiles[data->dwNumberOfFiles].fi.qwPosOfCompressedData = dwLastObjectId - data->pFiles[data->dwNumberOfFiles].fi.lh.compressed_size;
                    memcpy(&data->pFiles[data->dwNumberOfFiles].sVolume, &data->pVolume[data->dwNumberOfVolumes], sizeof(VolumeData));
                    data->pFiles[data->dwNumberOfFiles].dwNumberOfVolume = data->dwNumberOfVolumes;
                    data->pVolume[data->dwNumberOfVolumes].dwNumberOfFiles++;
                    data->dwNumberOfFiles++;

                    data->dwNumberOfVolumes++;

                    if(data->pFiles[data->dwNumberOfFiles-1].fi.lh.arj_flags & VOLUME_FLAG )
                    {
                        error = OpenNextVolume(_this, data->pFiles[data->dwNumberOfFiles-1].sVolume.hIo);
                        if(PR_FAIL(error))
                        {
                            data->dwNumberOfFiles--;
                            data->dwNumberOfVolumes--;

                            data->bSetIncomplete = cTRUE;
                        }
                    }
                }
            }
        }
    }
    
loc_exit:
    CALL_SYS_ObjHeapFree(_this, szFileName);
    if(name)
    {
        CALL_SYS_ObjectClose(name);
    }
    return error;
}

tERROR Rebuild(hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo)
{
tERROR error;
hIO hIo;
tCHAR* szName;
tDWORD i;
tQWORD qwSize, qwPackedSize;
hSEQ_IO hOutputSeqIo;
hSEQ_IO hInputSeqIo;
ArjData* data;
hTRANSFORMER hPacker;
tDWORD dwSizeOfLocalHeader;
tDWORD dwPart, dwCRC ;
local_header* plh;
tQWORD qwTmp;
hNEWOBJECT hNewObject;

    hOutputSeqIo = NULL;
    hInputSeqIo = NULL;
    hIo = NULL;
    hPacker = NULL;

    data = _this->data;

    if (!data->Buffer)
    {
      error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->Buffer, cSizeOfBuffer+16000);
      if ( PR_FAIL(error) )
        return(error);
    }

    error = CALL_SYS_ObjectCreateQuick(hOutputIo, &hOutputSeqIo, IID_SEQIO, PID_ANY,0);
    if(PR_FAIL(error))
    {
        goto loc_exit;
    }
    error = CALL_SYS_ObjectCreateQuick(data->hArcIoBackUp, &data->hArcSeqIo, IID_SEQIO, PID_ANY,0);
    if(PR_FAIL(error))
    {
        goto loc_exit;
    }
    
// Write Header of Archive
    CALL_SeqIO_Write(hOutputSeqIo, NULL, data->pArjHeader, data->dwHeaderSize);

// Enumerate גסו פאיכû
    for(i=0;i<data->dwNumberOfFiles;i++)
    {
        szName = data->pFiles[i].fi.szFileName;

//Check if present in deleted list
        if(errOK == CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&hIo, cDELETED_OBJECT, i+1))
        {
            continue;
        }
//Check in list of opened and modified (deleted) objects
        if(errOK == CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&hIo, cCHANGED_OBJECT, i+1))
        {
//Pack it according to previously used pack algo (or just stored)
            error = CALL_SYS_ObjectCreateQuick(hIo, &hInputSeqIo, IID_SEQIO, PID_ANY,0);
            if(PR_FAIL(error))
            {
                goto loc_exit;
            }
            error = CreatePacker(_this, data->pFiles[i].fi.lh.method , &hPacker, hInputSeqIo, hOutputSeqIo);
            CALL_IO_GetSize(hIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);
            if(PR_FAIL(error))
            {
                goto loc_exit;
            }
            CALL_SYS_PropertySetQWord(hPacker, pgTRANSFORM_SIZE, (tQWORD)qwSize);

// Write local_header
            CALL_SeqIO_Write(hOutputSeqIo, NULL, &data->pFiles[i].fi.lh, sizeof(local_header));
            dwPart = _toui32(strlen(data->pFiles[i].fi.szFileName));
            CALL_SeqIO_Write(hOutputSeqIo, NULL, &data->pFiles[i].fi.szFileName, dwPart );
            CALL_SeqIO_WriteWord(hOutputSeqIo, 0);
            CALL_SeqIO_WriteDword(hOutputSeqIo, 0);// CRC
            CALL_SeqIO_WriteWord(hOutputSeqIo, 0);// Extra Bytes
            CALL_SeqIO_GetSize(hOutputSeqIo,&qwTmp, IO_SIZE_TYPE_EXPLICIT );
            dwSizeOfLocalHeader = sizeof(local_header) + dwPart + sizeof(tWORD) + sizeof(tDWORD) + sizeof(tWORD);

            dwCRC = CountCRC32SeqIo(hInputSeqIo, data->Buffer, cSizeOfBuffer);

            error = CALL_Transformer_Proccess(hPacker, &qwPackedSize);
            if(PR_FAIL(error))
            {
                goto loc_exit;
            }
            CALL_SeqIO_Flush(hOutputSeqIo);
            CALL_SeqIO_GetSize(hOutputSeqIo,&qwTmp, IO_SIZE_TYPE_EXPLICIT );
            CALL_SeqIO_Seek(hOutputSeqIo, &qwTmp, qwPackedSize + dwSizeOfLocalHeader, cSEEK_BACK);
            plh = (local_header*)data->Buffer;
            CALL_SeqIO_Read(hOutputSeqIo, NULL, data->Buffer, dwSizeOfLocalHeader);
            CALL_SeqIO_Seek(hOutputSeqIo, NULL, dwSizeOfLocalHeader, cSEEK_BACK);
            dwSizeOfLocalHeader -= (sizeof(tDWORD) + sizeof(tWORD));
            plh->size = (tWORD)(dwSizeOfLocalHeader - sizeof(tDWORD));// Sig + size
            plh->first_size = sizeof(local_header) - sizeof(tDWORD);
            plh->compressed_size = (tDWORD)qwPackedSize;
            plh->original_size = (tDWORD)qwSize;
            plh->original_CRC = dwCRC;
            plh->method = data->pFiles[i].fi.lh.method ;
            plh->arj_flags &= ~GARBLED_FLAG; // clear password flag
            CreateDateTime(&plh->mod_file_time, &plh->mod_file_date);

            dwPart = iCountCRC32(plh->size, &plh->first_size, 0);
// Write mod local_header
            CALL_SeqIO_Write(hOutputSeqIo, NULL, data->Buffer, dwSizeOfLocalHeader);
            CALL_SeqIO_WriteDword(hOutputSeqIo, dwPart);
            CALL_SeqIO_WriteWord(hOutputSeqIo, 0);
            CALL_SeqIO_Seek(hOutputSeqIo, NULL, qwPackedSize, cSEEK_FORWARD);
        }
        else    // Not Changed
        {
// Read local header
            CALL_SeqIO_SeekRead(data->hArcSeqIo, NULL, (tDWORD)data->pFiles[i].fi.qwPosOfHeader, data->Buffer, (tDWORD)(data->pFiles[i].fi.qwPosOfCompressedData - data->pFiles[i].fi.qwPosOfHeader) );
            CALL_SeqIO_Write(hOutputSeqIo, NULL, data->Buffer, (tDWORD)(data->pFiles[i].fi.qwPosOfCompressedData - data->pFiles[i].fi.qwPosOfHeader));
// Copy Compressed data
            error = SeqIoCopyFile(data->Buffer, (hIO)data->hArcSeqIo, data->pFiles[i].fi.qwPosOfCompressedData, data->pFiles[i].fi.lh.compressed_size, hOutputSeqIo);
            
        }

        if(hInputSeqIo)
        {
            CALL_SYS_ObjectClose( hInputSeqIo );
            hInputSeqIo = NULL;
        }
        if(hIo)
        {
            CALL_SYS_ObjectClose( hIo );
            hIo = NULL;
        }
        if(hPacker)
        {
            CALL_SYS_ObjectClose(hPacker);
            hPacker = NULL;
        }
    }   //end of while
    
    CALL_SeqIO_Seek(hOutputSeqIo, &qwTmp, 0, cSEEK_BACK);
//if new files present pack it and write to tmp file
    if(errOK == CALL_UniArchiverCallback_GetFirstNewObject(hCallBack, &hNewObject))
    {
        do
        {
            CALL_UniArchiverCallback_GetNewObjectIo(hCallBack, (hOBJECT*)&hIo, hNewObject);

            error = CALL_SYS_ObjectCreateQuick(hIo, &hInputSeqIo, IID_SEQIO, PID_ANY,0);
            if(PR_FAIL(error))
            {
                goto loc_exit;
            }
//Pack it 
            error = CreatePacker(_this, data->pFiles[0].fi.lh.method , &hPacker, hInputSeqIo, hOutputSeqIo);
            CALL_IO_GetSize(hIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);
            if(PR_FAIL(error))
            {
                goto loc_exit;
            }
            CALL_SYS_PropertySetQWord(hPacker, pgTRANSFORM_SIZE, (tQWORD)qwSize);
            
// Get name
            CALL_SYS_PropertyGetStr(hIo, NULL, pgOBJECT_NAME, data->Buffer, cSizeOfBuffer, cCP_OEM );
// Write local_header
            CALL_SeqIO_Write(hOutputSeqIo, NULL, &data->pFiles[0].fi.lh, sizeof(local_header));
            dwPart = _toui32(strlen(data->Buffer));
            CALL_SeqIO_Write(hOutputSeqIo, NULL, data->Buffer, dwPart );
            CALL_SeqIO_WriteWord(hOutputSeqIo, 0);
            CALL_SeqIO_WriteDword(hOutputSeqIo, 0);// CRC
            CALL_SeqIO_WriteWord(hOutputSeqIo, 0);// Extra Bytes
            dwSizeOfLocalHeader = sizeof(local_header) + dwPart + sizeof(tWORD) + sizeof(tDWORD) + sizeof(tWORD);
            
            dwCRC = CountCRC32SeqIo(hInputSeqIo, data->Buffer, cSizeOfBuffer);
            
            error = CALL_Transformer_Proccess(hPacker, &qwPackedSize);
            if(PR_FAIL(error))
            {
                goto loc_exit;
            }
            CALL_SeqIO_Seek(hOutputSeqIo, &qwTmp, qwPackedSize + dwSizeOfLocalHeader, cSEEK_BACK);

            plh = (local_header*)data->Buffer;
            CALL_SeqIO_Read(hOutputSeqIo, NULL, data->Buffer, dwSizeOfLocalHeader);
            CALL_SeqIO_Seek(hOutputSeqIo, NULL, dwSizeOfLocalHeader, cSEEK_BACK);
            dwSizeOfLocalHeader -= (sizeof(tDWORD) + sizeof(tWORD));
            plh->size = (tWORD)(dwSizeOfLocalHeader - sizeof(tDWORD));// Sig + size
            plh->first_size = sizeof(local_header) - sizeof(tDWORD);
            plh->compressed_size = (tDWORD)qwPackedSize;
            plh->original_size = (tDWORD)qwSize;
            plh->method = data->pFiles[0].fi.lh.method;
            plh->arj_flags &= ~GARBLED_FLAG; // clear password flag
            plh->original_CRC = dwCRC;
            
            dwPart = iCountCRC32(plh->size, &plh->first_size, 0);
// Write mod local_header
            CALL_SeqIO_Write(hOutputSeqIo, NULL, data->Buffer, dwSizeOfLocalHeader);
            CALL_SeqIO_WriteDword(hOutputSeqIo, dwPart);
            CALL_SeqIO_WriteWord(hOutputSeqIo, 0);
            CALL_SeqIO_Seek(hOutputSeqIo, NULL, qwPackedSize, cSEEK_FORWARD);
            
            
            if(hInputSeqIo)
            {
                CALL_SYS_ObjectClose( hInputSeqIo );
                hInputSeqIo = NULL;
            }
        // Do not Close New IO!
            hIo = NULL;
            if(hPacker)
            {
                CALL_SYS_ObjectClose(hPacker);
                hPacker = NULL;
            }
        }while(errOK == CALL_UniArchiverCallback_GetNextNewObject(hCallBack, &hNewObject));
    }

    CALL_SeqIO_WriteWord(hOutputSeqIo, ARJ_SIGNATURE);
    CALL_SeqIO_WriteWord(hOutputSeqIo, 0);
    
loc_exit:
    if(hOutputSeqIo)
    {
        CALL_IO_Flush(hOutputSeqIo);
        CALL_SYS_ObjectClose(hOutputSeqIo);
    }
    if(hPacker)
    {
        CALL_SYS_ObjectClose(hPacker);
    }
    return error;
}

tERROR CreatePacker(hi_MiniArchiver _this, tDWORD dwMethod, hTRANSFORMER* phPacker, hSEQ_IO hInputIo, hSEQ_IO hOutputIo)
{
tERROR error;

//  error = errNOT_OK;
    if(dwMethod == STORED)
        error = CALL_SYS_ObjectCreate((hOBJECT)_this, phPacker, IID_TRANSFORMER, PID_STORED,0);
    else
        error = CALL_SYS_ObjectCreate((hOBJECT)_this, phPacker, IID_TRANSFORMER, PID_ARJPACKER,0);
    if(error == errOK)
    {
        CALL_SYS_PropertySetObj (*phPacker, pgOUTPUT_SEQ_IO, (hOBJECT)hOutputIo);
        CALL_SYS_PropertySetObj (*phPacker, pgINPUT_SEQ_IO, (hOBJECT)hInputIo);
        CALL_SYS_PropertySetDWord (*phPacker, plCOMPRESSON_METHOD, dwMethod);
        error = CALL_SYS_ObjectCreateDone(*phPacker);
    }
    return error;
}


tDWORD CountCRC32SeqIo(hSEQ_IO hFileSeqIo,tBYTE* Buffer, tDWORD dwBufferSize)
{
tDWORD CRC32 = 0;
tDWORD read;

    CALL_SeqIO_SeekSet(hFileSeqIo,NULL, 0);
    CALL_SeqIO_Read(hFileSeqIo, &read, Buffer, dwBufferSize);
    while(read != 0 )
    {
        CRC32 = iCountCRC32(read, Buffer, CRC32);
        CALL_SeqIO_Read(hFileSeqIo, &read, Buffer,dwBufferSize);
    }
    CALL_SeqIO_SeekSet(hFileSeqIo,NULL, 0);
    return CRC32;
}

tDWORD CountCRC32Io(hIO hIO, tQWORD qwStartOffset, tQWORD qwSize, tBYTE* Buffer, tDWORD dwBufferSize)
{
tDWORD CRC32 = 0;
tDWORD read;
tDWORD dwSize;

    while (qwSize)
    {
        if (qwSize < dwBufferSize)
        {
            dwSize = (tDWORD)qwSize;
        }
        else
        {
            dwSize = dwBufferSize;
        }

        CALL_IO_SeekRead(hIO, &read, qwStartOffset, Buffer, dwSize);
        CRC32 = iCountCRC32(read, Buffer, CRC32);

        if (read != dwSize) break;

        qwStartOffset += dwSize;
        qwSize -= dwSize;
    }

    return CRC32;
}


tERROR SeqIoCopyFile(tBYTE* Buffer, hIO Src, tQWORD qwSrcStartPos, tDWORD dwSize, hSEQ_IO Dest)
{
tDWORD read;
tDWORD wrote;
tERROR error;
tQWORD qwPosRead;
    
    error = errOK;
    wrote = 0;
    read = 0;
    
    if(error == errOK)
    {
        error = CALL_IO_SeekRead(Src, &read, qwSrcStartPos, Buffer, cSizeOfBuffer > dwSize ? dwSize : cSizeOfBuffer );
        qwPosRead = qwSrcStartPos;
        while(read != 0 )
        {
            error = CALL_SeqIO_Write(Dest,NULL, Buffer, read);
            if(errOK != error || wrote+read >=dwSize)
                break;
            
            qwPosRead += read;
            wrote += read;
            CALL_IO_SeekRead(Src, &read, qwPosRead, Buffer, cSizeOfBuffer );
            if(wrote+read > dwSize)
                read = dwSize - wrote;
        }
    }
    return error;
}
