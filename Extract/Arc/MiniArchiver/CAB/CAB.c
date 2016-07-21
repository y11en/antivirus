// -------- Friday,  27 October 2000,  15:18 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Universal Archiver
// Sub project -- Example
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- CAB.c
// -------------------------------------------


/*

Single Volume RO-RW

1. Проверяем на CAB и на то, что это один том.
2. Читаем каталог (имена ) CFFILE
3. Читаем CFFOLDERs
4. Читаем CFDATA
5. Клеим в MDMAP все данные для распаковки.
6. Next гуляем по считанному каталогу CFFILE
7. При открытии распаковываем с начала до нужного места (если уже распаковано, то не делаем это)
8. Вешаем DMAP.
9. Для RW копируем в Temp File или вешаем спец версию DMAP, которая сама при переоткрытии скопирует себя в TempFile

  MultiVolume
1. Находим первый том.
2. Считываем все CFFILEs (со всех томов). Запоминает, сплитован ли файл, том, в котором лежит файл.
3. По Next дебаем по считанному.
4. При открытии файла, считываем CFFOILDERs, CFDATA для нужного тома. Далее как для Single Volume, но с учетом
  того факта, что файл может быть в нескольких томах. Т.е. данные для MDMap готовим из текущего тома, плюс
  сплитованные данные.

Доходим до первого каба (если возможно)
перебираем все файлы в первом кабе, как только доходим по Split, открываем следующий и опять перебираем все 
файлы и так до последнего тома. 

MultiVolume RW

  1. Надо знать номер тома, который изменился

*/

#define PR_IMPEX_DEF


#define MiniArchiver_PRIVATE_DEFINITION

#define _CRTIMP
#include <Prague/prague.h>
#include "i_cab.h"

#include <Prague/iface/i_io.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_list.h>
#include <Extract/iface/i_uacall.h>
#include <Extract/iface/i_transformer.h>
#include <Prague/iface/i_hash.h>

#include <Prague/pr_pid.h>
#include "cab.h"
#include "FDI.h"
#include <Extract/plugin/p_inflate.h>

#include <Extract/plugin/p_unstore.h>
#include "../../Pack/UnLZX/plugin_lzxdecompressor.h"
#include "../../Pack/Quantum/quantum.h"
#include "../../Pack/deflate/deflate_t.h"
#include "../../../Wrappers/MultiDMAP/mdmap.h"
#include "Hash/MD5/plugin_hash_md5.h"

#include <Prague/iface/e_ktime.h> 
#include <Prague/iface/e_loader.h>

#define  IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_ktime.h> 
#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END

hROOT g_root = NULL;
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Interface comment )
// --------------------------------------------
// --- f533045c_723a_48c7_95ca_2aba12755d87 ---
// --------------------------------------------
// MiniArchiver interface implementation
// Short comments -- Mini Plugin for Universal Archiver
// Extended comment
//   
// AVP Prague stamp end( MiniArchiver, Interface comment )
// --------------------------------------------------------------------------------


#define SIZEOFNAME 260
#define CAB_CHUNK 0x8000


#pragma pack(1)


typedef struct _tag_CFDATA_Ex_
{
  tDWORD  csum;     // Checksum      (0 if split)
  tWORD cbData;     // Cooked Length
  tWORD cbUncomp;   // Raw Length    (0 if split)
} CFDataEx;


typedef struct _tag_CAB_FOLDER_DATA
{
    tDWORD coffCabStart;     // offset of folder data
    tWORD  cCFData;          // Number Of CFDATAs
    tWORD  typeCompress;     // compression type (tcomp* in FDI.h)
  CFDataEx* pCfDataEx;
}CABFolderData, *PCABFolderData;


typedef struct _tag_CAB_VOLUME_DATA
{
    tDWORD  cbCabinet;        // cabinet file size
    tDWORD  coffFiles;        // offset of first CAB_ENTRY
    tWORD   cFolders;         // number of folders
    tWORD   cFiles;           // number of files
    tWORD   flags;            // cabinet flags (CAB_FLAG_*)
    tWORD   setID;            // cabinet set id
    tWORD   iCabinet;         // zero-based cabinet number
  tDWORD  dwOffsetToFirstFolder;// offset to first folder in CAB Header
  // Must be at the end of structute
  tCHAR   szFileName[SIZEOFNAME]; // File name
  tBOOL   bWasChanged;    // Was volume changed or not
  hIO     hIo;        // IO of Volume
  hSEQ_IO hSeqIo;
  CABFolderData* pFolder;   // pointer to array of folders CFDATA
}CABVolumeData;

typedef struct _tag_FileData
{
  tDWORD  cbFile;           // uncompressed file size
  tDWORD  uoffFolderStart;  // file offset after decompression
  tWORD   iFolder;          // file control id (CAB_FILE_*)
  tWORD   date;             // file date stamp, as used by DOS
  tWORD   time;             // file time stamp, as used by DOS
  tWORD   attribs;          // file attributes (CAB_ATTRIB_*) 
  tCHAR   szFileName[SIZEOFNAME]; // File name
  tDWORD  dwNumberOfVolume; // Номер тома, где лежит файл (для второго и последующих) сплитованных файлов это последующий от dwStartNumberOfVolume номер
  tDWORD  dwStartNumberOfVolume;  // Номер тома, в котором впервые встретелся это файл (для сплитованных файлов)
  tDWORD  dwChangeStatus;   // Что случилось с файлом (см ниже)
  CABVolumeData*  pVolume;  // Pointer to Volume data
}FileData;
#pragma pack()

enum 
{
  FILE_NOT_CHANGED = 0,
  FILE_CHANGED     = 1,
  FILE_DELETED     = 2,
  FILE_NEW         = 3,
};

// TODO *********************************************************
//
//
// TODO *********************************************************

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Data structure )
// Interface MiniArchiver. Inner data structure

typedef struct tag_CAB_Data 
{
  tORIG_ID dwOrigin;
  tBOOL   IsFolder;    // --
  tBYTE   byFillChar;
  tWORD   wDelimeter;
  tDWORD  dwOpenMode;
  tDWORD  dwAccessMode;
  tBOOL   bMultiVolume;
  tDWORD  dwNumberOfFiles;  // Кол-во файлов (всего)
  tDWORD  dwRealNumberOfFiles;// Кол-во файлов для перебора (не считаю дупов из-за split)
  FileData* pFiles;     
  tQWORD  qwCurPosInEnum;    // --
  tQWORD  qwCurPosInPropertyGet;    // --
  tBYTE*  byUnpackWindow;
  hIO     hTempOutputIO;
  hSEQ_IO hTempOutputSeqIO;
  tDWORD  dwUnpackedPosition;
  hIO     hInputIo;
  hSEQ_IO hInputSeqIo;
  tDWORD  dwCurrentFolderOffset;
  tDWORD  dwCurrentCFData;
  tDWORD  dwStartupVolume;
  tBYTE*  Buffer;
  hObjPtr hPtr;
  tWORD   wCurrentStartFolder;    // Изначальный номер "текущего" фолдера
  tWORD   wCurrentWorkFolder;     // Текущий номер "текущего" фолдера
  tDWORD  dwCurrentStartVolume ;  // В каком томе начался "текущий" фолдер
    tDWORD  dwCurrentWorkVolume;    // В каком томе мы сейчас работаем

  tDWORD   dwNumberOfVolumes;// Общее кол-во томов
  CABVolumeData* Cvd;
  tBOOL    bScanAllVolumes;
  hHASH    hMD5;  
  tQWORD   qwHash;  // Хэш входного тома
  tBOOL    bIsReadOnly;
  tDWORD   dwAvail;
  hTRANSFORMER hTrans;  //Распаковщик
} CAB_Data;
// AVP Prague stamp end( MiniArchiver, Data structure )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, object declaration )
typedef struct tag_hi_MiniArchiver 
{
  const iMiniArchiverVtbl* vtbl; // pointer to the "MiniArchiver" virtual table
  const iSYSTEMVTBL*       sys;  // pointer to the "SYSTEM" virtual table
  CAB_Data*               data;   // pointer to the "MiniArchiver" data structure
} *hi_MiniArchiver;
// AVP Prague stamp end( MiniArchiver, object declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Internal method table prototypes )
// Interface "MiniArchiver". Internal method table. Forward declarations
tERROR pr_call MiniArchiver_Recognize( hOBJECT _that );
tERROR pr_call MiniArchiver_ObjectInit( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_ObjectInitDone( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_ObjectPreClose( hi_MiniArchiver _this );
// AVP Prague stamp end( MiniArchiver, Internal method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Internal method table )
// Interface "MiniArchiver". Internal method table.
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
// AVP Prague stamp end( MiniArchiver, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, interface function forward declarations )
typedef   tERROR (pr_call *fnpMiniArchiver_Reset)         ( hi_MiniArchiver _this );                         // -- First call of Next method after Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_Next)          ( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId );  // -- First call of Next method after creating or Reset must return *FIRST* object in enumeration;
typedef   tERROR (pr_call *fnpMiniArchiver_ObjCreate)     ( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO ); // -- ;
typedef   tERROR (pr_call *fnpMiniArchiver_SetAsCurrent)  ( hi_MiniArchiver _this, tQWORD qwObjectId );      // -- ;
typedef   tERROR (pr_call *fnpMiniArchiver_RebuildArchive)( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo );      // -- ;
// AVP Prague stamp end( MiniArchiver, interface function forward declarations )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, interface declaration )
struct iMiniArchiverVtbl  
{
  fnpMiniArchiver_Reset          Reset;
  fnpMiniArchiver_Next           Next;
  fnpMiniArchiver_ObjCreate      ObjCreate;
  fnpMiniArchiver_SetAsCurrent   SetAsCurrent;
  fnpMiniArchiver_RebuildArchive RebuildCAB;
}; // MiniArchiver
// AVP Prague stamp end( MiniArchiver, interface declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, External method table prototypes )
// Interface "MiniArchiver". External method table. Forward declarations
tERROR pr_call MiniArchiver_Reset( hi_MiniArchiver _this );
tERROR pr_call MiniArchiver_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId );
tERROR pr_call MiniArchiver_ObjCreate( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO );
tERROR pr_call MiniArchiver_SetAsCurrent( hi_MiniArchiver _this, tQWORD dwObjectId );
tERROR pr_call MiniArchiver_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo );      // -- ;
// AVP Prague stamp end( MiniArchiver, External method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, External method table )
// Interface "MiniArchiver". External method table.
static iMiniArchiverVtbl e_MiniArchiver_vtbl = 
{
  MiniArchiver_Reset,
  MiniArchiver_Next,
  MiniArchiver_ObjCreate,
  MiniArchiver_SetAsCurrent,
  MiniArchiver_RebuildArchive
};
// AVP Prague stamp end( MiniArchiver, External method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Forwarded property methods declarations )
// Interface "MiniArchiver". Get/Set property methods
tERROR pr_call CAB_PropGet( hi_MiniArchiver _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call CAB_PropSet( hi_MiniArchiver _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( MiniArchiver, Forwarded property methods declarations )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Global property variable declaration )
// Interface "MiniArchiver". Global(shared) property table variables
const tVERSION Version = 2; // must be READ_ONLY at runtime
//const tSTRING Comment = "CAB Archiver"; // must be READ_ONLY at runtime
const tBOOL IsSansative = cTRUE; // must be READ_ONLY at runtime
const tDWORD dwMaxLen = SIZEOFNAME; // must be READ_ONLY at runtime
const tBOOL bIsSolid = cTRUE;
// AVP Prague stamp end( MiniArchiver, Global property variable declaration )
// --------------------------------------------------------------------------------



tERROR ObjCreate( hi_MiniArchiver _this, hIO* hResultOutputIO, tDWORD dwObjectIdToCreate) ;
tERROR CreateUnpacker(hi_MiniArchiver _this, CAB_Data* data, CABVolumeData* pCvd, CABFolderData* pFolder);
tERROR CreateUnpackerMV(hi_MiniArchiver _this, CAB_Data* data, CABVolumeData* pCvd, CABFolderData* pFolder, tDWORD dwNumberOfVolume);
tERROR InitCAB(hi_MiniArchiver _this, hIO hArcIO, tCHAR* Buffer);
tERROR FindStartFolder( hi_MiniArchiver _this, CAB_HEADER* pch, tCHAR* Buffer);
tERROR OpenNextCAB(hSEQ_IO hSeqCurrent, hSEQ_IO* hSeqIo, CAB_HEADER* ch,  tCHAR* szNextVolName);
tERROR FillFileDataForMultiVol(CAB_Data* data);
tERROR FindFolder(hSEQ_IO hVolumeSeqIo, CAB_FOLDER* cFolder, tDWORD dwFolder);
tERROR OpenCAB(hi_MiniArchiver _this, hIO hIo, CABVolumeData* pCvd, hObjPtr hPtr, tDWORD dwOpenMode, tDWORD dwAccessMode, tCHAR* szVolName);
tERROR FillCFData(hi_MiniArchiver _this, CABVolumeData* pCvd);
tERROR Rebuild(hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo);
tERROR RebuildMV(hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo);
tQWORD CalcHash(hHASH hMD5, CABVolumeData* Cvd, tDWORD dwStartupVolume, tDWORD dwNumberOfFiles, FileData* pFiles);
tERROR DoneCAB(hi_MiniArchiver _this);
tERROR CreateOutputIO(hOBJECT _this, hIO* hTempOutputIO, hSEQ_IO* hTempOutputSeqIO);
//pgOBJECT_COMPRESSED_SIZE

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Property table )
// Interface "MiniArchiver". Property table
mPROPERTY_TABLE(MiniArchiver_PropTable)
  mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, Version, sizeof(Version) )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "CAB", 4 )
  mSHARED_PROPERTY_PTR( pgNAME_CASE_SENSITIVE, IsSansative, sizeof(IsSansative) )
  mSHARED_PROPERTY_PTR( pgNAME_MAX_LEN, dwMaxLen, sizeof(dwMaxLen) )
  mSHARED_PROPERTY_PTR( pgIS_SOLID, bIsSolid, sizeof(bIsSolid) )
  mSHARED_PROPERTY    ( pgOBJECT_OS_TYPE, ((tOS_ID)OS_TYPE_GENERIC) )

  mLOCAL_PROPERTY_BUF ( pgOBJECT_AVAILABILITY, CAB_Data, dwAvail, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_BUF ( pgIS_READONLY, CAB_Data, bIsReadOnly, cPROP_BUFFER_READ)
  mLOCAL_PROPERTY_FN  ( pgOBJECT_NAME, CAB_PropGet, NULL )
  mLOCAL_PROPERTY_FN  ( pgOBJECT_PATH, CAB_PropGet, NULL )
  mLOCAL_PROPERTY_FN  ( pgOBJECT_FULL_NAME, CAB_PropGet, NULL )
  mLOCAL_PROPERTY_FN  ( pgOBJECT_SIZE_Q, CAB_PropGet, NULL )

  mLOCAL_PROPERTY_BUF ( pgOBJECT_ORIGIN, CAB_Data, dwOrigin, cPROP_BUFFER_READ)
  mLOCAL_PROPERTY_BUF ( pgIS_FOLDER, CAB_Data, IsFolder, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF ( pgOBJECT_FILL_CHAR, CAB_Data, byFillChar, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_BUF ( pgNAME_DELIMITER, CAB_Data, wDelimeter, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_BUF ( pgOBJECT_OPEN_MODE, CAB_Data, dwOpenMode, cPROP_BUFFER_READ_WRITE )//!!!!!
  mLOCAL_PROPERTY_BUF ( pgOBJECT_ACCESS_MODE, CAB_Data, dwAccessMode, cPROP_BUFFER_READ_WRITE)
  mLOCAL_PROPERTY_FN  ( pgOBJECT_ATTRIBUTES, CAB_PropGet, NULL)
  mLOCAL_PROPERTY_FN  ( pgOBJECT_HASH, CAB_PropGet, NULL )
    mLOCAL_PROPERTY_FN  ( pgOBJECT_VOLUME_NAME, CAB_PropGet, NULL)

  mLOCAL_PROPERTY_BUF ( pgMINIARC_PROP_OBJECT_ID, CAB_Data, qwCurPosInPropertyGet, cPROP_BUFFER_WRITE)
  mLOCAL_PROPERTY_BUF ( pgMULTIVOL_AS_SINGLE_SET, CAB_Data, bScanAllVolumes, cPROP_BUFFER_READ_WRITE )

  mSHARED_PROPERTY( pgOBJECT_NAME_CP, ((tCODEPAGE)(cCP_ANSI)) )
  mSHARED_PROPERTY( pgOBJECT_PATH_CP, ((tCODEPAGE)(cCP_ANSI)) )
  mSHARED_PROPERTY( pgOBJECT_FULL_NAME_CP, ((tCODEPAGE)(cCP_ANSI)) )
mPROPERTY_TABLE_END(MiniArchiver_PropTable)
// AVP Prague stamp end( MiniArchiver, Property table )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Registration call )
// Interface "MiniArchiver". Register function
tERROR pr_call MiniArchiver_Register( hROOT root ) 
{
tERROR error;

  PR_TRACE_A0( root, "Enter \"MiniArchiver::Register\" method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_MINIARCHIVER,                       // interface identifier
    PID_CAB,                            // plugin identifier
    0x00000000,                             // subtype identifier
    0x00000002,                             // interface version
    VID_ANDY,                               // interface developer
    &i_MiniArchiver_vtbl,                   // internal(kernel called) function table
    (sizeof(i_MiniArchiver_vtbl)/sizeof(tPTR)),// internal function table size
    &e_MiniArchiver_vtbl,                   // external function table
    (sizeof(e_MiniArchiver_vtbl)/sizeof(tPTR)),// external function table size
    MiniArchiver_PropTable,                 // property table
    mPROPERTY_TABLE_SIZE(MiniArchiver_PropTable),// property table size
    sizeof(CAB_Data),                       // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error))
      PR_TRACE( (root,prtDANGER,"MiniArchiver(IID_MINIARCHIVER) registered [error=0x%08x]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave \"MiniArchiver::Register\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, Registration call )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Property method implementation )
// Interface "MiniArchiver". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call CAB_PropGet( hi_MiniArchiver _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error;
CAB_Data* data;
tUINT ns;


  PR_TRACE_A0( _this, "Enter *GET* method \"CAB_PropGet\" " );
  error = errOK;
  data = _this->data;
  if(data->pFiles == NULL)
  {
    return errOBJECT_NOT_CREATED;
  }
  if(data->qwCurPosInPropertyGet == 0xffffffffffff || data->qwCurPosInPropertyGet == 0)
  {
    data->qwCurPosInPropertyGet = data->qwCurPosInEnum;
  }
  else
  {
    data->qwCurPosInPropertyGet--;
  }

  if ( buffer && size )
  {
    switch (prop)
    {
      case pgOBJECT_ATTRIBUTES:
        if(size < sizeof(tDWORD))
        {
          size = sizeof(tDWORD);
          error = errBUFFER_TOO_SMALL;
          break;
        }
        size = sizeof(tDWORD);
        *((tDWORD*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].attribs;
        break;
      case pgOBJECT_NAME:
        if(data->pFiles)
        {
          ns = _toui32(strlen( data->pFiles[data->qwCurPosInPropertyGet].szFileName ));
          
          if ( ns+1 > size )
          {
            error = errBUFFER_TOO_SMALL;
            size = 0;
          }
          else
          {
            memcpy( buffer, data->pFiles[data->qwCurPosInPropertyGet].szFileName, ns+1 );
            size = ns + 1;
          }
        }
        else
        {
          size = sizeof(tDWORD);
          error = errOBJECT_NOT_FOUND;
        }
        break;
      case pgOBJECT_PATH:
        break;
      case pgOBJECT_SIZE_Q:
        size = sizeof(tQWORD);
        if(size < sizeof(tQWORD))
        {
          error = errBUFFER_TOO_SMALL;
          break;
        }
        *((tQWORD*)buffer) = (tQWORD)data->pFiles[data->qwCurPosInPropertyGet].cbFile;
        break;
      case pgOBJECT_FULL_NAME:
        if(data->pFiles)
        {
        tUINT ps = 0;//strlen( data->szPath );
          ns = _toui32(strlen( data->pFiles[data->qwCurPosInPropertyGet].szFileName ));//strlen( data->szFileName );

          if ( ps+ns+1 > size )
          {
            error = errBUFFER_TOO_SMALL;
            size = 0;
          }
          else
          {
  //          memcpy( buffer,    data->szPath, ps );
            memcpy( buffer+ps, data->pFiles[data->qwCurPosInPropertyGet].szFileName, ns+1 );
            size = ps + ns + 1;
          }
          break;
        }
        else
        {
          size = sizeof(tDWORD);
          error = errOBJECT_NOT_FOUND;
        }
        break;
      case pgOBJECT_HASH:
        if(size < sizeof(tQWORD))
        {
          size = sizeof(tQWORD);
          error = errBUFFER_TOO_SMALL;
          break;
        }
        size = sizeof(tQWORD);
        *((tQWORD*)buffer) = data->qwHash;
        break;

            case pgOBJECT_VOLUME_NAME:
                if (data->dwNumberOfVolumes > 1)
                {
                    error = CALL_SYS_PropertyGet(data->Cvd[data->pFiles[data->qwCurPosInPropertyGet].dwStartNumberOfVolume].hIo, &size, pgOBJECT_NAME, buffer, size);
                }
                else
                {
                    error = errPROPERTY_NOT_FOUND;
                    size = 0;
                }
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
        size = SIZEOFNAME;
        break;
      case pgOBJECT_SIZE_Q:
      case pgOBJECT_HASH:
        size = sizeof(tQWORD);
        break;
    }
  }
  else
  {
    error = errPARAMETER_INVALID;
    size = 0;
  }
  data->qwCurPosInPropertyGet = 0xffffffffffff;
    
  *out_size = size;

  PR_TRACE_A2( _this, "Leave *GET* method \"CAB_PropGet\" , ret tUINT = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, Property method implementation )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Property method implementation )
// Interface "MiniArchiver". Method "Set" for property(s):
/*
tERROR pr_call CAB_PropSet( hi_MiniArchiver _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error;
CAB_Data* data;
tUINT ns;
tBYTE Hash[32];


  PR_TRACE_A0( _this, "Enter *SET* method \"CAB_PropSet\"" );
  error = errOK;
  data = _this->data;
  if(data->pFiles == NULL)
  {
    return errOBJECT_NOT_CREATED;
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
    switch (prop)
    {
      case pgOBJECT_ATTRIBUTES:
        if(size < sizeof(tDWORD))
        {
          size = sizeof(tDWORD);
          error = errBUFFER_TOO_SMALL;
          break;
        }
        size = sizeof(tDWORD);
        *((tDWORD*)buffer) = data->pFiles[data->qwCurPosInPropertyGet].attribs;
        break;
    }
  }
  else if ( !buffer && !size ) 
  {
    switch(prop)
    {
      case pgOBJECT_ACCESS_MODE:
      case pgOBJECT_HASH:
        size = sizeof(tDWORD);
        break;
    }
  }
  else
  {
    error = errPARAMETER_INVALID;
    size = 0;
  }
  data->qwCurPosInPropertyGet = 0xffffffffffff;
    
  *out_size = size;

  PR_TRACE_A2( _this, "Leave *GET* method \"CAB_PropGet\" for property \"pgOBJECT_FULL_NAME\", ret tUINT = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
*/
// AVP Prague stamp end( MiniArchiver, Property method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Internal (kernel called) interface method implementation )
// --- Interface "MiniArchiver". Method "Recognize"
// Extended method comment
//   Static method. Kernel calls this method wihtout any created objects of this interface.
//   Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//   Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//   It must be quick (preliminary) analysis
// Result comment
//   
tERROR pr_call MiniArchiver_Recognize( hOBJECT _that ) 
{
tERROR error;
CAB_HEADER ch;

  error = errINTERFACE_INCOMPATIBLE;
  PR_TRACE_A2( 0, "Enter \"MiniArchiver::Recognize\" method for object \"%p (iid=%u)\" ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

  if(errOK == CALL_IO_SeekRead((hIO)_that, NULL, 0,&ch, sizeof(ch)))
  {
    if(ch.sig == CAB_SIGNATURE && ch.version == CAB_VERSION)
    {
      error = errOK;
    }
  }

  PR_TRACE_A3( 0, "Leave \"MiniArchiver::Recognize\" method for object \"%p (iid=%u)\", ret tERROR = 0x%08x", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID), error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------

// AVP Prague stamp begin( MiniArchiver, Internal (kernel called) interface method implementation )
// --- Interface "MiniArchiver". Method "ObjectInitDone"
// Extended method comment
//   Notification that all necessary properties have been set and object must go to operation state
// Behaviour comment
//   
// Result comment
//   Returns value differ from errOK if object cannot function properly
tERROR pr_call MiniArchiver_ObjectInit( hi_MiniArchiver _this ) 
{
tERROR error;
CAB_Data* data;
hIO hArcIO;
hOS hOs;
hOBJECT hIo;

  PR_TRACE_A0( _this, "Enter \"MiniArchiver::ObjectInit\" method" );

/*  error = MiniArchiver_Recognize((hOBJECT)_this);
  if(PR_FAIL(error))
  {
    return error;
  }*/
  
  data = _this->data;

data->bScanAllVolumes = cTRUE;
  
  // Create Temporary Buffer
  error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->Buffer, cCopyBufferSize + 16 );

  if(data->Buffer == NULL || PR_FAIL(error) )
  {
    PR_TRACE_A1( _this, "Leave \"MiniArchiver::ObjectInit\" method, ret tERROR = 0x%08x", errNOT_ENOUGH_MEMORY );
    return errNOT_ENOUGH_MEMORY;
  }
  
  data->bIsReadOnly = cFALSE;
  data->dwAvail = fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE | fAVAIL_CHANGE_MODE | fAVAIL_DELETE_ON_CLOSE;
  hArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this,IID_IO);
  if(hArcIO)
  {
    data->dwOpenMode   = CALL_SYS_PropertyGetDWord( (hOBJECT)hArcIO, pgOBJECT_OPEN_MODE);
    data->dwAccessMode = CALL_SYS_PropertyGetDWord( (hOBJECT)hArcIO, pgOBJECT_ACCESS_MODE);
    
    if( (hOs = (hOS)CALL_SYS_ParentGet(hArcIO, IID_OS)) != NULL )
    {
        hIo = (hOBJECT)(hArcIO);
        while ( PR_FAIL(error = CALL_OS_PtrCreate(hOs, &data->hPtr, hIo)) )
        {
            if ( NULL == (hIo = CALL_SYS_ParentGet(hIo, IID_IO)) )
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
                hIo = (hOBJECT)(hArcIO);
                while ( PR_FAIL(error = CALL_ObjPtr_ChangeTo(data->hPtr, hIo)) )
                {
                    if ( (hIo = CALL_SYS_ParentGet(hIo, IID_IO)) == NULL )
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
    
    if(data->hPtr == NULL && data->bMultiVolume == cTRUE)
    {
      data->bIsReadOnly = cTRUE;
      data->dwAvail = fAVAIL_READ;
      data->bMultiVolume = cFALSE;
    }

    
  }
  data->qwCurPosInPropertyGet = 0xffffffffffff;

  if(PR_SUCC(error))
  {
    error = InitCAB(_this, hArcIO, data->Buffer);
    if(data->bMultiVolume)
    {
      data->bIsReadOnly = cTRUE;
      data->dwAvail = fAVAIL_READ;
    }
  }
  if(PR_SUCC(error))
  {
      if(data->hMD5 == NULL)
      {
        error = CALL_SYS_ObjectCreateQuick(_this, &data->hMD5, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY);
        if(PR_SUCC(error))
        {
          data->qwHash = CalcHash(data->hMD5, &data->Cvd[data->dwStartupVolume], 0, data->dwRealNumberOfFiles, data->pFiles);
        }
        else
          data->qwHash = -1;
      }
    }
  
  PR_TRACE_A1( _this, "Leave \"MiniArchiver::ObjectInit\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, Internal (kernel called) interface method implementation )
// --- Interface "MiniArchiver". Method "ObjectInitDone"
// Extended method comment
//   Notification that all necessary properties have been set and object must go to operation state
// Behaviour comment
//   
// Result comment
//   Returns value differ from errOK if object cannot function properly
tERROR pr_call MiniArchiver_ObjectInitDone( hi_MiniArchiver _this ) 
{
tERROR error;
hIO hArcIO;
hOS hUniArcOS;
tQWORD qwHash;
CAB_Data* data;
tDWORD dwLen;

  PR_TRACE_A0( _this, "Enter \"MiniArchiver::ObjectInitDone\" method" );

  DoneCAB(_this);
  error = errOK;
  data = _this->data;
  data->bScanAllVolumes = cTRUE;
  data->dwCurrentStartVolume = data->dwCurrentWorkVolume = 0xffffffff;
  hArcIO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this,IID_IO);
  hUniArcOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this,IID_OS);

  error = InitCAB(_this, hArcIO, data->Buffer);
  if(PR_SUCC(error))
  {
    if(data->bMultiVolume)
    {
      data->bIsReadOnly = cTRUE;
      data->dwAvail = fAVAIL_READ;
    }
  

    // Send messages
    if(/*data->bScanAllVolumes == cTRUE &&*/ data->hMD5 != NULL)
    {
    tUINT i;
      for(i=0; i< data->dwNumberOfVolumes; i++)
      {
        qwHash = CalcHash(data->hMD5, &data->Cvd[i], 0, data->dwRealNumberOfFiles, data->pFiles );
        dwLen = sizeof(tQWORD);
        CALL_SYS_SendMsg(hUniArcOS, pmc_OS_VOLUME, pm_OS_VOLUME_HASH, data->Cvd[i].hIo , &qwHash, &dwLen);
      }
    }

    if((data->Cvd[0].flags & CAB_FLAG_HASPREV) || (data->Cvd[data->dwNumberOfVolumes-1].flags & CAB_FLAG_HASNEXT))
    {
      CALL_SYS_SendMsg(hUniArcOS, pmc_OS_VOLUME, pm_OS_VOLUME_SET_INCOMPLETE, 0, 0, 0);
    }
  }

  PR_TRACE_A1( _this, "Leave \"MiniArchiver::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// --- Interface "MiniArchiver". Method "ObjectPreClose"
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:

tERROR pr_call MiniArchiver_ObjectPreClose( hi_MiniArchiver _this )
{
    CAB_Data* data;

    PR_TRACE_A0( _this, "Enter MiniArchiver::ObjectPreClose method" );

    data = _this->data;

    if(data->hPtr)
        CALL_SYS_ObjectClose(data->hPtr);
    
    PR_TRACE_A1( _this, "Leave MiniArchiver::ObjectPreClose method, ret tERROR = 0x%08x", errOK );
    return errOK;
}
// AVP Prague stamp end( MiniArchiver, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, External (user called) interface method implementation )
// --- Interface "MiniArchiver". Method "Reset"
tERROR pr_call MiniArchiver_Reset( hi_MiniArchiver _this ) 
{
tERROR error;
  
  PR_TRACE_A0( _this, "Enter \"MiniArchiver::Reset\" method" );

  error = errOK;
  _this->data->qwCurPosInEnum = 0;
  _this->data->qwCurPosInPropertyGet = 0xffffffffffff;

  PR_TRACE_A1( _this, "Leave \"MiniArchiver::Reset\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, External (user called) interface method implementation )
// --- Interface "MiniArchiver". Method "Next"

tERROR pr_call MiniArchiver_Next( hi_MiniArchiver _this, tQWORD* result, tQWORD qwLastObjectId ) 
{
CAB_Data* data;
tERROR error ;

  PR_TRACE_A0( _this, "Enter \"MiniArchiver::Next\" method" );
  
  if(result == NULL)
  {
    PR_TRACE_A1( _this, "Leave \"MiniArchiver::Next\" method, error = 0x%08x", errPARAMETER_INVALID );
    return errPARAMETER_INVALID ;
  }

  data = _this->data;

  if(qwLastObjectId >= data->dwNumberOfFiles)
  {
    * result = 0;
    PR_TRACE_A1( _this, "Leave \"MiniArchiver::Next\" method, error = 0x%08x", errEND_OF_THE_LIST );
    return errEND_OF_THE_LIST;
  }

  error = errOK;
  data->qwCurPosInEnum = qwLastObjectId;
  qwLastObjectId++;

  if(PR_FAIL(error))
  {
    qwLastObjectId = 0;
  }
  *result = qwLastObjectId;
  PR_TRACE_A2( _this, "Leave \"MiniArchiver::Next\" method, ret tQWORD = %I64u, error = 0x%08x", qwLastObjectId, error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, External (user called) interface method implementation )
// --- Interface "MiniArchiver". Method "ObjCreate"
tERROR pr_call MiniArchiver_ObjCreate( hi_MiniArchiver _this, hIO* result, tQWORD qwObjectIdToCreate, hIO hUniArcIO ) 
{
tERROR  error;
CAB_Data* data;
hIO hResultOutputIO;

  PR_TRACE_A0( _this, "Enter \"MiniArchiver::ObjCreate\" method" );
  data = _this->data;
  error = errPARAMETER_INVALID;
  hResultOutputIO = NULL;
  if(qwObjectIdToCreate > data->dwNumberOfFiles)
  {
    error = errOBJECT_NOT_FOUND;
  }
  else
    error = ObjCreate( _this, &hResultOutputIO, (tDWORD)qwObjectIdToCreate) ;
  
  *result = hResultOutputIO;
  PR_TRACE_A2( _this, "Leave \"MiniArchiver::ObjCreate\" method, ret hOBJECT = %p, error = 0x%08x", hResultOutputIO, error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, External (user called) interface method implementation )
// --- Interface "MiniArchiver". Method "SetAsCurrent"
tERROR pr_call MiniArchiver_SetAsCurrent( hi_MiniArchiver _this, tQWORD dqObjectId ) 
{
tERROR error ;
//tQWORD qwRet;

  PR_TRACE_A0( _this, "Enter \"MiniArchiver::SetAsCurrent\" method" );
  error = errOK;

  _this->data->qwCurPosInEnum = 0;
  if(dqObjectId)
    _this->data->qwCurPosInEnum = dqObjectId-1;

  PR_TRACE_A1( _this, "Leave \"MiniArchiver::SetAsCurrent\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end( MiniArchiver, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



tERROR pr_call MiniArchiver_RebuildArchive( hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo )
{
  if(_this->data->bMultiVolume)
    return RebuildMV(_this, hCallBack, hOutputIo);
  else
    return Rebuild(_this, hCallBack, hOutputIo);
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( MiniArchiver, plugin definitions )
#include <stdarg.h>
PR_MAKE_TRACE_FUNC;



// AVP Prague stamp begin( Plugin definitions,  )
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* error ) 
{
  switch( dwReason ) 
  {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH : 
      break;
      
    case PRAGUE_PLUGIN_LOAD :
      g_root = (hROOT)hInstance;
      // register my interfaces
      if ( PR_FAIL(*error=MiniArchiver_Register( (hROOT)hInstance ) ))
        return cFALSE;

      // register my custom property ids
      // register my exports
      // resolve  my imports
      // use some system resources
      if ( PR_FAIL(*error=CALL_Root_ResolveImportTable((hROOT)hInstance , NULL, import_table, PID_APPLICATION)))
        return cFALSE;

      break;
      
    case PRAGUE_PLUGIN_UNLOAD :
      // free system resources
      // unregister my custom property ids -- you can drop it, kernel do it by itself
      // release    my imports             -- you can drop it, kernel do it by itself
      // unregister my exports             -- you can drop it, kernel do it by itself
      // unregister my interfaces          -- you can drop it, kernel do it by itself
      g_root = NULL;
      break;
  }
  return cTRUE;
}
// AVP Prague stamp end


// AVP Prague stamp end( MiniArchiver, plugin definitions )
// --------------------------------------------------------------------------------
tBOOL CheckVolumeName(hi_MiniArchiver _this, tCHAR *NextVolume)
{
    hSTRING hVolumeName = 0;
    tUINT size = _toui32(strlen(NextVolume));
    tBOOL bGoodName = cTRUE;
    tUINT i;
    CAB_Data* data = _this->data;
    tSTR_COMPARE cmp;

    if (PR_FAIL(CALL_SYS_ObjectCreateQuick(_this, &hVolumeName, IID_STRING, PID_ANY, SUBTYPE_ANY)))
    {
        return bGoodName;
    }

  for(i=0; i <data->dwNumberOfVolumes; i++)
  {
        if (PR_FAIL(CALL_String_ImportFromProp(hVolumeName, 0, (hOBJECT)data->Cvd[i].hIo, pgOBJECT_NAME)))
        {
            break;
        }

        //if (PR_FAIL(CALL_String_CompareBuff2(hVolumeName, &cmp, NextVolume, size, cCP_ANSI, fSTRING_COMPARE_CASE_INSENSITIVE)))
        if (PR_FAIL(CALL_String_CompareBuff2(hVolumeName, &cmp, cSTRING_WHOLE, NextVolume, size, cCP_ANSI, fSTRING_COMPARE_CASE_INSENSITIVE)))
        {
            break;
        }

        if (cmp == cSTRING_COMP_EQ)
        {
            bGoodName = cFALSE;
            break;
        }
    }

    if (hVolumeName)
        CALL_SYS_ObjectClose(hVolumeName);

    return bGoodName;
}

tERROR DoneCAB(hi_MiniArchiver _this)
{
CAB_Data* data;
tUINT i, j;
CABVolumeData* pCvd;
PCABFolderData pFolder;

  data = _this->data;
  CALL_SYS_ObjHeapFree(_this, data->pFiles);

  for(i=0; i <data->dwNumberOfVolumes; i++)
  {
    pCvd = &data->Cvd[i];
    for(j=0; j< pCvd->cFolders; j++)
    {
      pFolder = &pCvd->pFolder[j];
      CALL_SYS_ObjHeapFree(_this, pFolder->pCfDataEx);
    }
    CALL_SYS_ObjHeapFree(_this, pCvd->pFolder);
  }
  CALL_SYS_ObjHeapFree(_this, data->Cvd);
  data->pFiles = NULL;
  data->Cvd = NULL;
  
  return errOK;
}

tERROR InitCAB(hi_MiniArchiver _this, hIO hArcIO, tCHAR* Buffer)
{
tERROR error;
tDWORD i;
tDWORD j;
tDWORD iFile;
tDWORD dwTmp;
CAB_HEADER ch;
CAB_HEADER_RES chRes;
CAB_Data* data;
tWORD wSplitFolder;
tDWORD dwFinishEnum;
tBOOL bWasSplitFile;
tBOOL bIsNextVolume;

  data = _this->data;
  if(hArcIO == NULL)
    return errOBJECT_NOT_CREATED;

  error = CALL_IO_SeekRead(hArcIO, NULL, 0, &ch, sizeof(CAB_HEADER));
  if(PR_FAIL(error))
  {
    return error;
  }
  dwTmp = sizeof(CAB_HEADER);
  if((ch.flags & CAB_FLAG_RESERVE) == CAB_FLAG_RESERVE)
  {
//    CALL_SeqIO_Seek(pCvd->hSeqIo, NULL, sizeof(CAB_HEADER_RES), cSEEK_SET);
    dwTmp = sizeof(CAB_HEADER_RES);
    error = CALL_IO_SeekRead(hArcIO, NULL, 0, &chRes, sizeof(CAB_HEADER_RES));
    dwTmp += chRes.cbCFHeader;
  }
  data->wCurrentStartFolder = data->wCurrentWorkFolder = -1;
  data->dwNumberOfVolumes = 1;
  data->dwStartupVolume = 0;
  
  if(data->hPtr != NULL && ((ch.flags & CAB_FLAG_HASPREV) == CAB_FLAG_HASPREV || (ch.flags & CAB_FLAG_HASNEXT) == CAB_FLAG_HASNEXT))
  {
    data->bMultiVolume = cTRUE;
    error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->Cvd, sizeof(CABVolumeData) );

        data->Cvd[0].hIo = hArcIO;

    CALL_SYS_PropertyGetStr(hArcIO, NULL, pgOBJECT_NAME, Buffer, cCopyBufferSize, cCP_ANSI);

        strncpy_s (data->Cvd[0].szFileName, countof(data->Cvd[0].szFileName), Buffer, sizeof(data->Cvd[0].szFileName) - 1);

    if(data->bScanAllVolumes == cTRUE)// Scan all volumes
    {
      if((ch.flags & CAB_FLAG_HASPREV) == CAB_FLAG_HASPREV)
      {
        SkipCABnameIo(hArcIO, Buffer, dwTmp);
        error = FindStartFolder(_this, &ch, Buffer);
        if(data->Cvd->hIo == NULL && error == errOBJECT_NOT_FOUND) // We are on first volume (we don't have prev volume)
        {
          CALL_SYS_PropertyGetStr(hArcIO, NULL, pgOBJECT_NAME, Buffer, cCopyBufferSize, cCP_ANSI);
        }
      }
    }

    i = 0;
    do 
    {
      error = OpenCAB(_this, data->Cvd[i].hIo, &data->Cvd[i], data->hPtr, data->dwOpenMode, data->dwAccessMode, data->Buffer);
            if(error == errOBJECT_NOT_FOUND)
            {
                error = errOK;
                break;
            }
      if(PR_FAIL(error))
      {
        break;
      }
      error = FillCFData(_this, &data->Cvd[i]);
      if(PR_FAIL(error))
      {
        break;
      }

      data->dwNumberOfFiles += data->Cvd[i].cFiles;

            bIsNextVolume = cFALSE;

      i++;
            if (i >= data->dwNumberOfVolumes)
            {
          error = CALL_SYS_ObjHeapRealloc(_this, (tPTR*)&data->Cvd, data->Cvd, (i+1) * sizeof(CABVolumeData) );
          if(PR_FAIL(error))
          {
            break;
          }
                data->dwNumberOfVolumes = i;

                if (data->Buffer[0] != 0)
                {
                    if (!CheckVolumeName(_this, data->Buffer))
                    {
                        break;
                    }
                    bIsNextVolume = cTRUE;
                }
            }
            else
            {
                strncpy_s(data->Buffer, countof(data->Cvd[i].szFileName), data->Cvd[i].szFileName, sizeof(data->Cvd[i].szFileName));
                bIsNextVolume = cTRUE;
            }
    }
        while(PR_SUCC(error) && bIsNextVolume);

    if(i == 0)
    {
            if (PR_SUCC(error))
            {
                error = errOBJECT_NOT_FOUND;
            }
      // Can't open 
      return error;
    }
        /*
  // Set Current Volume (Need for Hash cacl)

    for(i=0;i<data->dwNumberOfVolumes;i++)
    {
      if(data->Cvd[i].hIo == hArcIO)
      {
        data->dwStartupVolume = i;
        break;
      }
    }
        */
  }
  else
  {
    if (PR_SUCC(error)) error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->Cvd, data->dwNumberOfVolumes * sizeof(CABVolumeData) );
    if (PR_SUCC(error)) 
        {
            data->Cvd[0].hIo = hArcIO;
            error = OpenCAB(_this, hArcIO, data->Cvd, data->hPtr, data->dwOpenMode, data->dwAccessMode, data->Buffer);
        }
    if (PR_SUCC(error)) error = FillCFData(_this, data->Cvd);
    if (PR_SUCC(error)) data->dwNumberOfFiles = data->Cvd->cFiles;
  }
  
// Alloc mem for Names
  if (PR_SUCC(error)) error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->pFiles, data->dwNumberOfFiles * sizeof(FileData));
  if(PR_FAIL(error))
  {
    return error;
  }

  
//Read CFFILEs
  iFile = 0;
  data->dwNumberOfFiles = 0;
  data->dwRealNumberOfFiles = 0;
  wSplitFolder = 0;
  dwFinishEnum = data->dwNumberOfVolumes;
  if(data->bScanAllVolumes == cFALSE)
    dwFinishEnum = 1;
  bWasSplitFile = cFALSE;
  for(dwTmp=0; dwTmp < dwFinishEnum; dwTmp++)
  {
    error = CALL_SeqIO_Seek(data->Cvd[dwTmp].hSeqIo, NULL, data->Cvd[dwTmp].coffFiles, cSEEK_SET);
    for(i=0;i<data->Cvd[dwTmp].cFiles;i++)
    {
      error = CALL_SeqIO_Read(data->Cvd[dwTmp].hSeqIo, NULL, &data->pFiles[iFile], sizeof(CAB_ENTRY));
      if(PR_FAIL(error))
        break;
      for(j=0;j<SIZEOFNAME-1;j++)
      {
        error = CALL_SeqIO_ReadByte(data->Cvd[dwTmp].hSeqIo, &data->pFiles[iFile].szFileName[j]);
        if(data->pFiles[iFile].szFileName[j] == 0)
          break;
      }
      data->pFiles[iFile].pVolume = &data->Cvd[dwTmp];
      //data->pFiles[iFile].dwNumberOfVolume = dwTmp;
      data->pFiles[iFile].dwStartNumberOfVolume = dwTmp;
      data->pFiles[iFile].dwChangeStatus = FILE_NOT_CHANGED;
      if(data->pFiles[iFile].iFolder == CAB_FILE_SPLIT /*|| data->pFiles[iFile].iFolder == CAB_FILE_PREV_NEXT*/)
      {
        // Check if next Vol Present
        if((dwTmp + 1) < data->dwNumberOfVolumes)
        {
          if(bWasSplitFile)
          {
            data->pFiles[iFile].iFolder = data->Cvd[dwTmp].cFolders - 1;//0;
            data->pFiles[iFile].pVolume = &data->Cvd[dwTmp];//+1
            //data->pFiles[iFile].dwNumberOfVolume = dwTmp ;//+1
            data->pFiles[iFile].dwStartNumberOfVolume = dwTmp;
          }
          else
            data->pFiles[iFile].iFolder = data->Cvd[dwTmp].cFolders - 1;
          bWasSplitFile = cTRUE;
        }
        else
        {
          iFile++;
          continue;
        }
      }
      if(data->pFiles[iFile].iFolder != CAB_FILE_CONTINUED && data->pFiles[iFile].iFolder != 0xffff)
      {
        iFile++;
        data->dwNumberOfFiles++;
      }
    }
  }
  data->dwRealNumberOfFiles = iFile;

  return error;
}


tERROR ObjCreate( hi_MiniArchiver _this, hIO* hResultOutputIO, tDWORD dwObjectIdToCreate) 
{
tERROR error;
CAB_Data* data;
FileData* pFile;
tDWORD dwSize, dwFileSize;
CABFolderData* pFolder;
tQWORD qwTmp;
tINT iBuilt;
//tDWORD dwCurVolNr;

  error = errOK;
  data = _this->data;

  *hResultOutputIO = NULL;

  pFile = &data->pFiles[dwObjectIdToCreate-1];
  if(pFile->iFolder > 0xfff0)
  {
    return errOBJECT_NOT_FOUND;
  }

    if(pFile->iFolder >= pFile->pVolume->cFolders)
    {
        return errOBJECT_DATA_CORRUPTED;
    }

  if(data->bMultiVolume == cFALSE)
  {
    if(data->wCurrentStartFolder != pFile->iFolder)
    {
      if(data->hTrans)
      {
        CALL_SYS_ObjectClose(data->hTrans);
        data->hTrans = NULL;
      }
      /*if(data->hTempOutputSeqIO)
      {
        CALL_SYS_ObjectClose(data->hTempOutputSeqIO);
        data->hTempOutputSeqIO = NULL;
      }
      if(data->hTempOutputIO)
      {
        CALL_SYS_ObjectClose(data->hTempOutputIO);
        data->hTempOutputIO = NULL;
      }*/
      data->dwUnpackedPosition = 0;
      data->dwCurrentCFData = 0;
      data->wCurrentStartFolder = pFile->iFolder;
            data->wCurrentWorkFolder = pFile->iFolder;
      data->dwCurrentStartVolume = pFile->dwStartNumberOfVolume;
      data->dwCurrentWorkVolume = pFile->dwStartNumberOfVolume;
      error = CreateUnpackerMV(_this, data, pFile->pVolume, &pFile->pVolume->pFolder[pFile->iFolder], pFile->dwStartNumberOfVolume);
/*      if(PR_SUCC(error))
      {
        error = CreateOutputIO((hOBJECT)data->hTrans, &data->hTempOutputIO, &data->hTempOutputSeqIO);
      }*/
    }
  }
  else
  {
    if(data->dwCurrentStartVolume != pFile->dwStartNumberOfVolume || 
           data->wCurrentStartFolder != pFile->iFolder)
    {
      if(data->hTrans)
      {
        CALL_SYS_ObjectClose(data->hTrans);
        data->hTrans = NULL;
      }
      error = CreateUnpackerMV(_this, data, pFile->pVolume, &pFile->pVolume->pFolder[pFile->iFolder], pFile->dwStartNumberOfVolume);
      data->wCurrentStartFolder = pFile->iFolder;
      data->wCurrentWorkFolder = pFile->iFolder;
      data->dwCurrentStartVolume = pFile->dwStartNumberOfVolume;
      data->dwCurrentWorkVolume = pFile->dwStartNumberOfVolume;
      
      data->dwUnpackedPosition = 0;
      data->dwCurrentCFData = 0;
    }
  }
  if(PR_FAIL(error))
  {
    if(data->hTrans)
      CALL_SYS_ObjectClose(data->hTrans );
    data->hTrans = NULL;
  }
  if(data->hTrans == NULL)
  {
    error = errOBJECT_NOT_CREATED;
  }
  else
  {
    error = CALL_SYS_PropertySetObj(data->hTrans, pgOUTPUT_SEQ_IO, (hOBJECT)_this->data->hTempOutputSeqIO);
    if(PR_SUCC(error))
      error = CALL_SYS_PropertySetObj(data->hTrans, pgINPUT_SEQ_IO, (hOBJECT)_this->data->hInputSeqIo);
  }

  pFolder = &data->Cvd[data->dwCurrentWorkVolume].pFolder[data->wCurrentWorkFolder];
  
  //dwCurVolNr = pFile->dwNumberOfVolume;
  if(PR_SUCC(error))
  {
// Надо дораспаковать -- отлично
    dwSize = data->dwUnpackedPosition;
    while(data->dwUnpackedPosition < pFile->uoffFolderStart + pFile->cbFile && 
          data->dwCurrentCFData < pFolder->cCFData)
    {
      tQWORD BlockStart;

      if ( NULL == pFolder->pCfDataEx )
      {
        /* wtf ??? */
        error = errNOT_OK; 
        break;
      }

      if(pFolder->pCfDataEx[data->dwCurrentCFData].cbUncomp == 0)
      {
        if((data->dwCurrentWorkVolume + 1) >= data->dwNumberOfVolumes)
        {
                    // no next volume, stopping file unpacking
          //error = errNOT_OK;
          break;
        }
        // есть сплит файл и последующий сплит файлы начинаются тут же, но мы уже сменили диск и надо
        // переставить всем последующим сплит файлам фолдер и том на тот, который мы уже используем

                // this is all bullshit
                /*
        if(dwObjectIdToCreate+1 < data->dwNumberOfFiles)
        {
        tDWORD i;
          for(i=dwObjectIdToCreate; i<data->dwNumberOfFiles;i++)
          {
            if(data->pFiles[i].dwNumberOfVolume == data->pFiles[dwObjectIdToCreate-1].dwNumberOfVolume)
            {
              data->pFiles[i].iFolder = 0;
              data->pFiles[i].pVolume = &data->Cvd[dwCurVolNr+1];
              data->pFiles[i].dwNumberOfVolume++;
            }
            else
              break;
          }
                    

        }
                */
                data->dwCurrentWorkVolume++;
                data->wCurrentWorkFolder = 0;
        pFolder = &data->Cvd[data->dwCurrentWorkVolume].pFolder[data->wCurrentWorkFolder];
        data->dwCurrentCFData = 0;
      }
      dwSize += pFolder->pCfDataEx[data->dwCurrentCFData].cbUncomp;

      if((pFolder->typeCompress & tcompMASK_TYPE) == tcompTYPE_LZX || (pFolder->typeCompress & tcompMASK_TYPE) == tcompTYPE_QUANTUM)
      {
        CALL_SYS_PropertySetQWord( (hOBJECT)data->hTrans, pgTRANSFORM_SIZE, (tQWORD)pFolder->pCfDataEx[data->dwCurrentCFData].cbUncomp);
        CALL_SYS_PropertySetDWord( (hOBJECT)data->hTrans, plRUN_INPUT_SIZE, pFolder->pCfDataEx[data->dwCurrentCFData].cbData);
      }
      else
      {
        tINT ResMode, CompMode;

        CALL_SYS_PropertySetQWord( (hOBJECT)data->hTrans, pgTRANSFORM_SIZE, pFolder->pCfDataEx[data->dwCurrentCFData].cbUncomp);
        CALL_SYS_PropertySetQWord( (hOBJECT)data->hTrans, pgTRANSFORM_INPUT_SIZE, pFolder->pCfDataEx[data->dwCurrentCFData].cbData);

        CompMode = (pFolder->typeCompress & tcompMASK_TYPE);
        ResMode  = (CompMode == tcompTYPE_MSZIP) ? (INFLATE_RESET_STATEIGNOREWND) : (UNSTORED_RESET_STATE);
        CALL_Transformer_Reset(data->hTrans, ResMode);
      }

      CALL_SeqIO_Seek(data->hInputSeqIo, &BlockStart, 0, cSEEK_FORWARD);
      error = CALL_Transformer_Proccess(data->hTrans, &qwTmp);

      /* seek to next block - can have trash after packed stream */
      BlockStart += pFolder->pCfDataEx[data->dwCurrentCFData].cbData;
      BlockStart -= ((pFolder->typeCompress & tcompMASK_TYPE) == tcompTYPE_MSZIP) ? (2) : (0);
      CALL_SeqIO_Seek(data->hInputSeqIo, NULL, BlockStart, cSEEK_SET);

      if(PR_FAIL(error))
      {
        break;
      }
      CALL_SeqIO_Flush(data->hTempOutputSeqIO);
// Увеличим на размер чанка
      data->dwUnpackedPosition += pFolder->pCfDataEx[data->dwCurrentCFData].cbUncomp;
      data->dwCurrentCFData++;
      if((pFolder->typeCompress & tcompMASK_TYPE) == tcompTYPE_NONE)
      {
        CALL_IO_GetSize(data->hInputSeqIo, &qwTmp, IO_SIZE_TYPE_EXPLICIT);
        data->dwUnpackedPosition = (tDWORD)qwTmp;
      }
    }
    if(PR_SUCC(error))
    {
            if (data->dwUnpackedPosition < pFile->uoffFolderStart)
            {
                dwFileSize = 0;
            }
            else if (data->dwUnpackedPosition < pFile->uoffFolderStart + pFile->cbFile)
            {
                dwFileSize = data->dwUnpackedPosition - pFile->uoffFolderStart;
            }
            else
            {
                dwFileSize = pFile->cbFile;
            }

      error = CALL_SYS_ObjectCreate(_this, hResultOutputIO, IID_IO, PID_TMPFILE,0);
      if(PR_SUCC(error))
      {
        CALL_SYS_PropertySetDWord( (hOBJECT)*hResultOutputIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
        CALL_SYS_PropertySetDWord( (hOBJECT)*hResultOutputIO, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
        error = CALL_SYS_ObjectCreateDone( *hResultOutputIO);
        CALL_IO_SetSize(*hResultOutputIO, dwFileSize);
      }
      iBuilt = 0;
      ioCopyFile(data->Buffer, (hIO)data->hTempOutputSeqIO, pFile->uoffFolderStart, dwFileSize, *hResultOutputIO, 0);
    }
  }
/*
  hH = (hOBJECT)CALL_SYS_PropertyGetObj( CALL_SYS_ParentGet(_this, IID_ROOT), pgOBJECT_HEAP );
  if(hH)
  {
    dwSize = CALL_SYS_PropertyGetDWord(hH, plWIN32_HEAP_SIZE);
    printf(" %d                     \r", dwSize);
    if(dwSize > 10000000)
    {
      dwSize = dwSize ;
    }
  }
*/
  return error;
}

tERROR CreateOutputIO(hOBJECT _this, hIO* hTempOutputIO, hSEQ_IO* hTempOutputSeqIO)
{
tERROR error;

  error = CALL_SYS_ObjectCreate(_this, hTempOutputIO, IID_IO, PID_TMPFILE,0);
  if(PR_SUCC(error))
  {
    CALL_SYS_PropertySetDWord( (hOBJECT)*hTempOutputIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
    CALL_SYS_PropertySetDWord( (hOBJECT)*hTempOutputIO, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
    
    error = CALL_SYS_ObjectCreateDone( *hTempOutputIO);
    if(PR_SUCC(error))
    {
      error = CALL_SYS_ObjectCreateQuick(*hTempOutputIO, hTempOutputSeqIO, IID_SEQIO, PID_ANY,0);
    }
  }
  return error;
}

tERROR CreateUnstoreExtractor(hi_MiniArchiver _this, hTRANSFORMER* hTrans, hSEQ_IO hTempOutputSeqIO, tPTR* bInflateWindow)
{
  return CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)hTrans,IID_TRANSFORMER, PID_UNSTORE,0);
}

tERROR CreateInflateExtractor(hi_MiniArchiver _this, hTRANSFORMER* hTrans, hSEQ_IO hTempOutputSeqIO, tPTR* bInflateWindow)
{
tERROR error;
tINT iTmp;

  if(*bInflateWindow)
  {
    CALL_SYS_ObjHeapFree(_this, *bInflateWindow);
  }
  error = CALL_SYS_ObjHeapAlloc(_this, bInflateWindow, 32*1024+64);
  if(PR_SUCC(error))
  {
    error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)hTrans,IID_TRANSFORMER, PID_INFLATE,0);
  }
  if(PR_SUCC(error))
  {
    error = CALL_SYS_PropertySetDWord(*hTrans, plINFLATE_WINDOW_SIZE,(tDWORD)32*1024);
    if(PR_SUCC(error))
      error = CALL_SYS_PropertySetPtr(*hTrans, plINFLATE_WINDOW, *bInflateWindow);
    iTmp=-15;
    if(PR_SUCC(error))
      error = CALL_SYS_PropertySet(*hTrans, NULL, plINFLATE_WINDOW_MULTIPIER, &iTmp, sizeof(iTmp));
    error = CALL_SYS_PropertySetBool(*hTrans, pgFORCE_WINDOW_CREATION, cTRUE);
  }
  return error;
}

tERROR CreateLZXExtractor(hi_MiniArchiver _this, hTRANSFORMER* hExtractor, hSEQ_IO hTempOutputSeqIO, tPTR* bInflateWindow, tDWORD dwWndMul)
{
tERROR error = errOK;
  
  if(*bInflateWindow)
  {
    CALL_SYS_ObjHeapFree(_this, *bInflateWindow);
  }
    *bInflateWindow = 0;
//  error = CALL_SYS_ObjHeapAlloc(_this, bInflateWindow, 2097152+64);
  
  if(PR_SUCC(error))
  {
    error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)hExtractor, IID_TRANSFORMER, PID_LZXDECOMPRESSOR,0);
  }
  if(PR_SUCC(error))
  {
    if(PR_SUCC(error))
      error = CALL_SYS_PropertySetDWord(*hExtractor, plWINDOW_SIZE, dwWndMul);
  }
  return error;
}

tERROR CreateQuantumExtractor(hi_MiniArchiver _this, hTRANSFORMER* hExtractor, hSEQ_IO hTempOutputSeqIO, tPTR* bInflateWindow, tDWORD dwWndMul)
{
tERROR error = errOK;
  
  if(*bInflateWindow)
  {
    CALL_SYS_ObjHeapFree(_this, *bInflateWindow);
  }
    *bInflateWindow = 0;
//  error = CALL_SYS_ObjHeapAlloc(_this, bInflateWindow, 2097152+64);
  
  if(PR_SUCC(error))
  {
    error = CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)hExtractor, IID_TRANSFORMER, PID_QUANTUMDECOMPRESSOR,0);
  }
  if(PR_SUCC(error))
  {
    if(PR_SUCC(error))
      error = CALL_SYS_PropertySetDWord(*hExtractor, plWINDOW_SIZE, dwWndMul);
  }
  return error;
}


tERROR CreateUnpackerMV(hi_MiniArchiver _this, CAB_Data* data, CABVolumeData* pCvd, CABFolderData* pFolder, tDWORD dwNumberOfVolume)
{
tERROR error;
tDWORD dwStart;
tDWORD iTmp;
tDWORD dwSizeOfSignature, dwSizeOfSignatureTmp;
tDWORD dwNumberOfCfDatas;
tDWORD dwTempDataForSize;

  error = errNOT_OK;
  dwSizeOfSignature = 0;
  switch(pFolder->typeCompress & tcompMASK_TYPE)
  {
    case tcompTYPE_NONE:
      error = CreateUnstoreExtractor(_this, &data->hTrans, data->hTempOutputSeqIO, (tPTR*)&data->byUnpackWindow);
      break;
    case tcompTYPE_MSZIP:
      dwSizeOfSignature = 2;
      error = CreateInflateExtractor(_this, &data->hTrans, data->hTempOutputSeqIO, (tPTR*)&data->byUnpackWindow);
      break;
    case tcompTYPE_QUANTUM:
      error = CreateQuantumExtractor(_this, &data->hTrans, data->hTempOutputSeqIO, (tPTR*)&data->byUnpackWindow, (pFolder->typeCompress & CAB_MASK_LZX_WINDOW) >> CAB_SHIFT_LZX_WINDOW);
      break;
    case tcompTYPE_LZX:
      error = CreateLZXExtractor(_this, &data->hTrans, data->hTempOutputSeqIO, (tPTR*)&data->byUnpackWindow, (pFolder->typeCompress & CAB_MASK_LZX_WINDOW) >> CAB_SHIFT_LZX_WINDOW);
      break;
  }
  
  if(PR_SUCC(error))
  {
    error = CreateOutputIO((hOBJECT)data->hTrans, &data->hTempOutputIO, &data->hTempOutputSeqIO);
    // Create Input IO
    if(PR_SUCC(error))
    {
      error = CALL_SYS_ObjectCreate(_this, &data->hInputIo, IID_IO, PID_MDMAP,0);
    }
  }
  if(PR_SUCC(error))
  {

    dwStart = pFolder->coffCabStart;
    dwNumberOfCfDatas = pFolder->cCFData;
    dwSizeOfSignatureTmp = 0;
//    pFolder->dwFolderSize = 0;
    iTmp = 0;

    /* fake decoder for zero-length files */
    if ( dwNumberOfCfDatas == 0 )
    {
      CALL_SYS_PropertySetDWord(data->hInputIo, plMAP_AREA_SIZE, 0);
      CALL_SYS_PropertySetQWord(data->hInputIo, plMAP_AREA_START, dwStart);
      error = CALL_SYS_PropertySetObj(data->hInputIo,   plMAP_AREA_ORIGIN, (hOBJECT)  pCvd->hIo);
    }

loc_NextVolume:
    for(; iTmp < dwNumberOfCfDatas; iTmp++)
    {
      dwStart += sizeof(CFDATA);
      if(pFolder->pCfDataEx[iTmp].cbData < dwSizeOfSignature) //CK signature
      {
        dwSizeOfSignatureTmp = 1;
      }
      else
      {
        CALL_SYS_PropertySetDWord(data->hInputIo, plMAP_AREA_SIZE, pFolder->pCfDataEx[iTmp].cbData - dwSizeOfSignature);
        CALL_SYS_PropertySetQWord(data->hInputIo, plMAP_AREA_START, dwStart+dwSizeOfSignature);
        error = CALL_SYS_PropertySetObj(data->hInputIo,   plMAP_AREA_ORIGIN, (hOBJECT)  pCvd->hIo);// pFolder->pCfDataEx[iTmp].hIo );
        if(PR_FAIL(error))
          break;
      }
      if(pFolder->pCfDataEx[iTmp].cbUncomp == 0)  // Partial
      {
      //GetNextCvd
        dwNumberOfVolume++;
        if(dwNumberOfVolume >= data->dwNumberOfVolumes )
        {
          goto loc_exit;
        }
        dwTempDataForSize = pFolder->pCfDataEx[iTmp].cbData;
        pCvd = &data->Cvd[dwNumberOfVolume];
        pFolder = &pCvd->pFolder[0];
        dwStart = pFolder->coffCabStart;
        

        CALL_SYS_PropertySetDWord(data->hInputIo, plMAP_AREA_SIZE, pFolder->pCfDataEx[0].cbData - dwSizeOfSignatureTmp);


        CALL_SYS_PropertySetQWord(data->hInputIo, plMAP_AREA_START, dwStart + dwSizeOfSignatureTmp + sizeof(CFDATA));
        error = CALL_SYS_PropertySetObj(data->hInputIo,   plMAP_AREA_ORIGIN, (hOBJECT) pCvd->hIo );//pFolder->pCfDataEx[0].hIo );
        

        dwNumberOfCfDatas = pFolder->cCFData ;
        iTmp = 1;
        dwStart = pFolder->coffCabStart + pFolder->pCfDataEx[0].cbData + sizeof(CFDATA);
        pFolder->pCfDataEx[0].cbData += dwTempDataForSize ;// Correct Packed Size
        
        //Folder 0
        goto loc_NextVolume;
      }
      dwStart += pFolder->pCfDataEx[iTmp].cbData;
    }
loc_exit:
    if(PR_SUCC(error))
    {
      error = CALL_SYS_ObjectCreateDone( data->hInputIo);
      if(PR_SUCC(error))
      {
        error = CALL_SYS_ObjectCreateQuick(data->hInputIo, &data->hInputSeqIo, IID_SEQIO, PID_ANY,0);
      }
    }
  }
  if(PR_SUCC(error))
  {
    error = CALL_SYS_PropertySetObj(data->hTrans, pgOUTPUT_SEQ_IO, (hOBJECT)_this->data->hTempOutputSeqIO);
    if(PR_SUCC(error))
      error = CALL_SYS_PropertySetObj(data->hTrans, pgINPUT_SEQ_IO, (hOBJECT)_this->data->hInputSeqIo);
    error = CALL_SYS_ObjectCreateDone(data->hTrans);
  }
  else
  {
    if(data->hTrans)
    {
      CALL_SYS_ObjectCreateDone(data->hTrans);
      CALL_SYS_ObjectClose(data->hTrans);
    }
    if(data->hInputIo)
      CALL_SYS_ObjectClose(data->hInputIo);
    data->hTrans = NULL;
    data->hInputIo = NULL;
  }
  
  return error;
}



tERROR FindStartFolder( hi_MiniArchiver _this, CAB_HEADER* pch, tCHAR* Buffer)
{
    CAB_Data* data = _this->data;
    tERROR error = errOK;
    hSTRING name = NULL;
    CAB_HEADER ch;
    CAB_HEADER_RES chRes;
    hIO hIoTemp = NULL;
    hSEQ_IO hArcSeqIO = NULL;

  error = CALL_SYS_ObjectCreateQuick( _this, &name, IID_STRING, PID_ANY, SUBTYPE_ANY );

  while(PR_SUCC(error) && CheckVolumeName(_this, Buffer))
  {
    CALL_String_ImportFromBuff( name, 0, Buffer, _toui32(strlen(Buffer)), cCP_ANSI, 0);
    error = CALL_ObjPtr_IOCreate(data->hPtr, &hIoTemp, name,  data->dwAccessMode, fOMODE_OPEN_IF_EXIST );
    if(PR_SUCC(error))
    {
            data->dwNumberOfVolumes ++;
            data->dwStartupVolume ++; // we've found one previous volume, so we're moving our starting volume up

      error = CALL_SYS_ObjHeapRealloc(_this, (tPTR*)&data->Cvd, data->Cvd, data->dwNumberOfVolumes * sizeof(CABVolumeData) );

            memmove (&data->Cvd[1], &data->Cvd[0], (data->dwNumberOfVolumes - 1)*sizeof(CABVolumeData) );
            memset (&data->Cvd[0], 0, sizeof(CABVolumeData) );

            strncpy_s (data->Cvd[0].szFileName, countof(data->Cvd[0].szFileName), Buffer, sizeof(data->Cvd[0].szFileName) - 1);

      /*if(*hIo)
      {
        CALL_SYS_ObjectClose((hOBJECT)*hIo);
        *hIo = NULL;
      } */
      
      data->Cvd[0].hIo = hIoTemp;
      error = CALL_SYS_ObjectCreateQuick((hOBJECT)data->Cvd[0].hIo, &hArcSeqIO, IID_SEQIO, PID_ANY,0);
      if(PR_SUCC(error))
      {
        CALL_SeqIO_Read(hArcSeqIO, NULL, &ch, sizeof(CAB_HEADER));
        if((ch.flags & CAB_FLAG_HASPREV) != CAB_FLAG_HASPREV)
        {
          break;
        }
        if((ch.flags & CAB_FLAG_RESERVE) == CAB_FLAG_RESERVE)
        {
          CALL_SeqIO_SeekRead(hArcSeqIO, NULL, 0, &chRes, sizeof(CAB_HEADER_RES));
          error = CALL_SeqIO_Seek(hArcSeqIO, NULL, sizeof(CAB_HEADER_RES) + chRes.cbCFHeader , cSEEK_SET);
        }
        SkipCABname(hArcSeqIO, Buffer);
      }
    }
  }
  if(hArcSeqIO)
  {
    CALL_SYS_ObjectClose((hOBJECT)hArcSeqIO);
  }
  
  if(name)
    {
    CALL_SYS_ObjectClose((hOBJECT)name);
    }

  return error;
}

tVOID FillCABHeaderData(PCAB_HEADER pCh, CABVolumeData* pChd)
{
  pChd->cbCabinet = pCh->cbCabinet;
  pChd->cFiles = pCh->cFiles;
  pChd->cFolders = pCh->cFolders;
  pChd->coffFiles = pCh->coffFiles;
  pChd->flags = pCh->flags;
  pChd->iCabinet = pCh->iCabinet;
  pChd->setID = pCh->setID;
}


tERROR OpenCAB(hi_MiniArchiver _this, hIO hIo, CABVolumeData* pCvd, hObjPtr hPtr, tDWORD dwOpenMode, tDWORD dwAccessMode, tCHAR* szVolName)
{
hSTRING name;
tERROR error;
CAB_HEADER ch;
CAB_HEADER_RES chRes;
tDWORD dwTmp; 

  error = errOK;
  name = NULL;

    if (!pCvd->szFileName[0]) strncpy_s(pCvd->szFileName, countof(pCvd->szFileName), szVolName, sizeof(pCvd->szFileName)-1);

  if(hIo == NULL)
  {
    error = CALL_SYS_ObjectCreateQuick( hPtr, &name, IID_STRING, PID_ANY, SUBTYPE_ANY );
    if(PR_FAIL(error))
    {
      return error;
    }
    CALL_String_ImportFromBuff( name, 0, szVolName, _toui32(strlen(szVolName)), cCP_ANSI, 0);
    szVolName[0] = 0;
    error = CALL_ObjPtr_IOCreate(hPtr, &hIo, name,  dwAccessMode, fOMODE_OPEN_IF_EXIST );
  }


  if(PR_SUCC(error))
  {
    pCvd->hIo = hIo;
    error = CALL_SYS_ObjectCreateQuick((hOBJECT)hIo, &pCvd->hSeqIo, IID_SEQIO, PID_ANY,0);
    if(PR_SUCC(error))
    {
      error = CALL_SeqIO_Read(pCvd->hSeqIo, NULL, &ch, sizeof(CAB_HEADER));

      if(PR_SUCC(error))
      {
          FillCABHeaderData(&ch, pCvd);

        dwTmp = sizeof(CAB_HEADER);
        
        if((pCvd->flags & CAB_FLAG_RESERVE) == CAB_FLAG_RESERVE)
        {
          error = CALL_SeqIO_SeekRead(pCvd->hSeqIo, NULL, 0, &chRes, sizeof(CAB_HEADER_RES));
          dwTmp = sizeof(CAB_HEADER_RES) + chRes.cbCFHeader;
          CALL_SeqIO_Seek(pCvd->hSeqIo, NULL, dwTmp  , cSEEK_SET);
        }
        if((pCvd->flags & CAB_FLAG_HASPREV) == CAB_FLAG_HASPREV)
        {
          dwTmp += SkipCABname(pCvd->hSeqIo, szVolName);
        }
        szVolName[0] = 0;
        if((pCvd->flags & CAB_FLAG_HASNEXT) == CAB_FLAG_HASNEXT)
        {
          dwTmp += SkipCABname(pCvd->hSeqIo, szVolName);
        }
        
        pCvd->dwOffsetToFirstFolder = dwTmp;
        error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&pCvd->pFolder, sizeof(CABFolderData) * pCvd->cFolders);
      }
    }
  }
  
  if(name)
    CALL_SYS_ObjectClose((hOBJECT)name);
  return error;
}

tERROR FillCFData(hi_MiniArchiver _this, CABVolumeData* pCvd)
{
tERROR error;
tINT iFolder;
tDWORD iTmp;
tDWORD dwStart ;
tDWORD dwNumberOfCFDATA;

  error = errOK;
  if(pCvd->pFolder == NULL)
  {
    error = errNOT_ENOUGH_MEMORY;
    pCvd->dwOffsetToFirstFolder = 0;
  }
  if(pCvd->dwOffsetToFirstFolder != 0)
  {
    for(iFolder = 0; iFolder < pCvd->cFolders; iFolder++)
    {
      error = CALL_SeqIO_SeekRead(pCvd->hSeqIo, &iTmp, pCvd->dwOffsetToFirstFolder, &pCvd->pFolder[iFolder], CAB_FOLDER_);

            if (PR_FAIL(error))
            {
                error = errOBJECT_DATA_CORRUPTED;
                break;
            }

      pCvd->dwOffsetToFirstFolder += CAB_FOLDER_;

      // Read CFFolders
// Alloc mem for CF data
      error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&pCvd->pFolder[iFolder].pCfDataEx, pCvd->pFolder[iFolder].cCFData * sizeof(CFDataEx));
  
      if(PR_SUCC(error))
      {
// Read CFDATA
        dwStart = pCvd->pFolder[iFolder].coffCabStart ;
        dwNumberOfCFDATA = pCvd->pFolder[iFolder].cCFData;
        for(iTmp = 0; iTmp < dwNumberOfCFDATA; iTmp++)
        {
          error = CALL_SeqIO_SeekRead(pCvd->hSeqIo, NULL, dwStart, &pCvd->pFolder[iFolder].pCfDataEx[iTmp], sizeof(CFDATA));

                    if (PR_FAIL(error))
                    {
                        error = errOBJECT_DATA_CORRUPTED;
                        break;
                    }

//            pCvd->pFolder[iFolder].pCfDataEx[iTmp].hIo = pCvd->hIo;
          dwStart += (sizeof(CFDATA) + pCvd->pFolder[iFolder].pCfDataEx[iTmp].cbData);
//            pCvd->pFolder[iFolder].dwFolderSize += pCvd->pFolder[iFolder].pCfDataEx[iTmp].cbUncomp;
        }
                if (PR_FAIL(error))
                {
                    break;
                }
      }
    }
  }
  pCvd->dwOffsetToFirstFolder = 0;
  return error;
}


tERROR CreateTempSeqIo(hi_MiniArchiver _this, hIO* hTempOutputIO, hSEQ_IO* hTempOutputSeqIO)
{
tERROR error;
  
  error = CALL_SYS_ObjectCreate(_this, hTempOutputIO, IID_IO, PID_TMPFILE,0);
  if(PR_SUCC(error))
  {
    CALL_SYS_PropertySetDWord( (hOBJECT)*hTempOutputIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
    CALL_SYS_PropertySetDWord( (hOBJECT)*hTempOutputIO, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
    
    error = CALL_SYS_ObjectCreateDone( *hTempOutputIO);
    if(PR_SUCC(error))
    {
      error = CALL_SYS_ObjectCreateQuick(*hTempOutputIO,hTempOutputSeqIO, IID_SEQIO, PID_ANY,0);
    }
  }
  return error;
}

tERROR CreatePacker(hi_MiniArchiver _this, hTRANSFORMER* phPacker)
{
tERROR error;
tINT iTmp;

  error = CALL_SYS_ObjectCreate((hOBJECT)_this, phPacker, IID_TRANSFORMER, PID_DEFLATE,0);
  if(PR_SUCC(error))
  {
    CALL_SYS_PropertySetQWord(*phPacker, pgTRANSFORM_SIZE, (tQWORD)CAB_CHUNK);
    error = CALL_SYS_PropertySetDWord(*phPacker, plDEFLATE_WINDOW_SIZE,32*1024);
    iTmp= -15;
    CALL_SYS_PropertySet(*phPacker, NULL, plDEFLATE_WINDOW_MULTIPIER, &iTmp, sizeof(iTmp));
  }
  return error;
}


tVOID CreateDateTime(tWORD* time, tWORD*date)
{
tDWORD year  ;
tDWORD month ;
tDWORD day   ;
tDWORD hour  ;
tDWORD minute;
tDWORD second;
tDT dt  ;
  
  year = 1990;
  month = 1;
  day = 1;
  hour = 0;
  minute = 0;
  second = 0;
  
  Now(&dt);
  DTGet((const tDT*)&dt, &year, &month, &day, &hour, &minute, &second, NULL);
  *time = (((tWORD)hour)<<11) + (((tWORD)minute)<<5) + ((tWORD)(second/2));
  *date = (((tWORD)year-1980)<<9) + (((tWORD)month)<<5) + (tWORD)(day);
}


tERROR Rebuild(hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo)
{
tERROR error;
hIO hIo;
tCHAR* szName;
hNEWOBJECT hNewObject;
tDWORD i;
tQWORD qwSize, qwSizeEnd;
hIO hTempSolidIO;
hSEQ_IO hTempSolidSeqIO;
hSEQ_IO hOutputSeqIo;
CAB_Data* data;
tDWORD dwPart;
CAB_HEADER ch;
CAB_FOLDER cf;
CAB_ENTRY ce;
CFDATA cd;
tWORD ZIP_SIG=0x4b43 /* 'KC' */;
tWORD wSizeOfSignature;
tBOOL bWasChanged;
hTRANSFORMER hPacker; // Упаковщик

  FillCABHeader(&ch);

  hTempSolidSeqIO = NULL;
  hTempSolidIO = NULL;
  hOutputSeqIo = NULL;

  data = _this->data;
  hPacker = NULL;
  error = CALL_SYS_ObjectCreateQuick(hOutputIo, &hOutputSeqIo, IID_SEQIO, PID_ANY,0);
  error = CreateTempSeqIo(_this, &hTempSolidIO, &hTempSolidSeqIO);
  if(PR_SUCC(error))
  {
    error = CreatePacker(_this, &hPacker);
    wSizeOfSignature = 2;
  }
  if(PR_FAIL(error))
  {
    goto loc_exit;
  }
  
  
  ce.uoffFolderStart = 0;
  hIo = NULL;
// Write Header of Archive
  CALL_SeqIO_Write(hOutputSeqIo, NULL, &ch, sizeof(ch) );

  cf.typeCompress = tcompTYPE_MSZIP;
  cf.coffCabStart = sizeof(CAB_HEADER) + sizeof(CAB_FOLDER);
  cf.cCFData = 0;
// Write folder
  CALL_SeqIO_Write(hOutputSeqIo, NULL, &cf, sizeof(cf) );

  
// Enumerate все файлы
  for(i=0;i<data->dwNumberOfFiles;i++)
  {
      bWasChanged = cFALSE;
    szName = data->pFiles[i].szFileName;

//Check if present in deleted list
    if(PR_SUCC(CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&hIo, cDELETED_OBJECT, i+1)))
    {
      //bWasChanged = cTRUE;
      continue;
    }
//Check in list of opened and modified (deleted) objects
    if(PR_FAIL(CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&hIo, cCHANGED_OBJECT, i+1)))
    {
      // Extract this file to Temp IO
      error = ObjCreate( _this, &hIo, i+1);
      if(PR_FAIL(error))
        continue;
    }
        else
        {
        bWasChanged = cTRUE;
        }
//Pack it according to previously used pack algo (or just stored)
    CALL_IO_GetSize(hIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);

    error = SeqIoCopyFile(data->Buffer, hIo, 0, (tDWORD)qwSize, hTempSolidSeqIO);
    if(PR_FAIL(error))
    {
      goto loc_exit;
    }
    ch.cFiles++;

    ce.cbFile = (tDWORD)qwSize;
    ce.iFolder = CAB_FILE_FIRST;
        if (bWasChanged)
        {
            CreateDateTime(&ce.time, &ce.date);
        }
        else
        {
        ce.date = data->pFiles[i].date;
        ce.time = data->pFiles[i].time;
        }
    ce.attribs = CAB_ATTRIB_ARCHIVE;
    CALL_SeqIO_Write(hOutputSeqIo, NULL, &ce, sizeof(ce) );
    dwPart = _toui32(strlen(szName)+1);
    error = CALL_SeqIO_Write(hOutputSeqIo, NULL, szName,  dwPart );

    ce.uoffFolderStart += ce.cbFile;
    cf.coffCabStart += sizeof(CAB_ENTRY) + dwPart ;

    if(PR_FAIL(error))
    {
      goto loc_exit;
    }
    if(hIo)
    {
      CALL_SYS_ObjectClose( hIo );
      hIo = NULL;
    }
  } //end of while
  
  
//if new files present pack it and write to tmp file
  if(PR_SUCC(CALL_UniArchiverCallback_GetFirstNewObject(hCallBack, &hNewObject)))
  {
    do
    {
      CALL_UniArchiverCallback_GetNewObjectIo(hCallBack, (hOBJECT*)&hIo, hNewObject);

      CALL_IO_GetSize(hIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);
      error = SeqIoCopyFile(data->Buffer, hIo, 0, (tDWORD)qwSize, hTempSolidSeqIO);
      if(PR_FAIL(error))
      {
        return error;
      }
      ch.cFiles++;

      ce.cbFile = (tDWORD)qwSize;
      ce.iFolder = CAB_FILE_FIRST;
      CreateDateTime(&ce.time, &ce.date);
      ce.attribs = CAB_ATTRIB_ARCHIVE;
      CALL_SeqIO_Write(hOutputSeqIo, NULL, &ce, sizeof(ce) );

// Get name
            CALL_SYS_PropertyGetStr(hIo, NULL, pgOBJECT_NAME, data->Buffer, cCopyBufferSize, cCP_ANSI );
      dwPart = _toui32(strlen(data->Buffer)+1);
      error = CALL_SeqIO_Write(hOutputSeqIo, NULL, data->Buffer,  dwPart );
      
      ce.uoffFolderStart += ce.cbFile;
      cf.coffCabStart += sizeof(CAB_ENTRY) + dwPart ;
      
      if(PR_FAIL(error))
      {
        goto loc_exit;
      }
      if(hIo)
      {
        CALL_SYS_ObjectClose( hIo );
        hIo = NULL;
      }
    }while(PR_SUCC(CALL_UniArchiverCallback_GetNextNewObject(hCallBack, &hNewObject)));
  }

  CALL_IO_Flush(hTempSolidSeqIO);
  CALL_IO_GetSize(hTempSolidSeqIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);

  CALL_SYS_PropertySetObj (hPacker, pgOUTPUT_SEQ_IO, (hOBJECT)hOutputSeqIo);//, sizeof(hOutputSeqIo));

  CALL_SYS_PropertySetObj (hPacker, pgINPUT_SEQ_IO, (hOBJECT)hTempSolidSeqIO);//, sizeof(hTempSolidSeqIO));
  error = CALL_SYS_ObjectCreateDone( hPacker);
  
  i = (tDWORD)qwSize;

  dwPart = CAB_CHUNK;
  CALL_SeqIO_Seek(hTempSolidSeqIO, NULL, 0, cSEEK_SET);
  
  do
  {
    if(i < dwPart)
    {
      dwPart = i;
    }
    CALL_SYS_PropertySetQWord(hPacker, pgTRANSFORM_SIZE, (tQWORD)dwPart);
  
    cd.cbUncomp = (tWORD)dwPart;
    CALL_SeqIO_Write(hOutputSeqIo, NULL, &cd, sizeof(cd));
    error = CALL_SeqIO_Write(hOutputSeqIo, NULL, &ZIP_SIG, wSizeOfSignature);

    if(PR_FAIL(error))
    {
      goto loc_exit;
    }
    
    CALL_IO_GetSize(hOutputSeqIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);

    error = CALL_Transformer_Proccess(hPacker, &qwSizeEnd);
    if(PR_FAIL(error))
    {
      return errNOT_OK;
    }

    //CALL_IO_GetSize(hOutputSeqIo, &qwSizeEnd, IO_SIZE_TYPE_EXPLICIT);

    cd.cbData = (tWORD)(qwSizeEnd /*- qwSize*/) + wSizeOfSignature;

    CALL_SeqIO_Seek(hOutputSeqIo, NULL, qwSize - wSizeOfSignature, cSEEK_SET);

    cd.csum = checksumIo(hOutputSeqIo, data->Buffer, cCopyBufferSize, cd.cbData, 0l);
    cd.csum = checksum((tBYTE *)&cd.cbData, 4l, cd.csum);

    CALL_SeqIO_Seek(hOutputSeqIo, NULL, qwSize - sizeof(CFDATA) - wSizeOfSignature, cSEEK_SET);
    
    error = CALL_SeqIO_Write(hOutputSeqIo, NULL, &cd, sizeof(cd));
    CALL_IO_GetSize(hOutputSeqIo, &qwSizeEnd, IO_SIZE_TYPE_EXPLICIT);
    CALL_SeqIO_Seek(hOutputSeqIo, NULL, qwSizeEnd, cSEEK_SET);
    if(PR_FAIL(error))
    {
      goto loc_exit;
    }

    cf.cCFData ++;
    i -= dwPart;
  }while(i != 0);

  if(PR_SUCC(error))
  {
    CALL_IO_GetSize(hOutputSeqIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);
    ch.cbCabinet = (tDWORD)qwSize;
    
    CALL_SeqIO_Seek(hOutputSeqIo, NULL, 0, cSEEK_SET);
// Write Header of Archive
    CALL_SeqIO_Write(hOutputSeqIo, NULL, &ch, sizeof(ch) );
// Write folder
    error = CALL_SeqIO_Write(hOutputSeqIo, NULL, &cf, sizeof(cf) );
  }
  
loc_exit:
  if(hOutputSeqIo)
  {
    CALL_IO_Flush(hOutputSeqIo);
    CALL_SYS_ObjectClose(hOutputSeqIo);
  }
  if(hTempSolidIO)
    CALL_SYS_ObjectClose(hTempSolidIO);
  if(hPacker)
    CALL_SYS_ObjectClose(hPacker);
  return error;
}



tERROR RebuildMV(hi_MiniArchiver _this, hUNIARCCALLBACK hCallBack, hIO hOutputIo)
{
  return errNOT_IMPLEMENTED;
/*
tERROR error;
hIO hIo;
tCHAR* szName;
hNEWOBJECT hNewObject;
tDWORD i;
CAB_Data* data;
tBOOL bWasChanged;
FileData* pFile;
tQWORD qwSize;


  data = _this->data; 
// Enumerate all files to find changes
  for(i=0;i<data->dwNumberOfFiles;i++)
  {
    szName = data->pFiles[i].szFileName;

//Check if present in deleted list
    if(errOK == CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&hIo, cDELETED_OBJECT, szName))
    {
      data->pFiles[i].dwChangeStatus = FILE_DELETED;
      data->pFiles[i].pVolume->bWasChanged = cTRUE;
      bWasChanged = cTRUE;
    }
//Check in list of opened and modified (deleted) objects
    else if(errOK == CALL_UniArchiverCallback_FindObjectInList(hCallBack, (hOBJECT*)&hIo, cCHANGED_OBJECT, szName))
    {
      data->pFiles[i].dwChangeStatus = FILE_CHANGED;
      data->pFiles[i].pVolume->bWasChanged = cTRUE;
      bWasChanged = cTRUE;
    }
  } //end of while
  
  
//if new files present pack it and write to tmp file
  if(errOK == CALL_UniArchiverCallback_GetFirtsNewObject(hCallBack, &hNewObject))
  {
    bWasChanged = cTRUE;
    do
    {
      // Alloc mem for Names
      data->dwNumberOfFiles++;
      error = CALL_SYS_ObjHeapRealloc(_this, &data->pFiles, data->pFiles, data->dwNumberOfFiles * sizeof(FileData));
      pFile = &data->pFiles[data->dwNumberOfFiles - 1];
      if(PR_FAIL(error))
      {
        goto loc_exit;
      }
      CALL_UniArchiverCallback_GetNewObjectIo(hCallBack, (hOBJECT*)&hIo, hNewObject, pFile->szFileName, SIZEOFNAME);
      CALL_IO_GetSize(hIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);
      pFile->attribs = CAB_ATTRIB_ARCHIVE;
      pFile->cbFile = (tDWORD)qwSize;
      CreateDateTime(&pFile->time, &pFile->date);
      pFile->dwChangeStatus = FILE_NEW;
      pFile->dwNumberOfVolume = 0xffffffff;
      pFile->iFolder = CAB_FILE_FIRST;
      pFile->pVolume = NULL;
      pFile->uoffFolderStart = 0;
      
    }while(errOK == CALL_UniArchiverCallback_GetNextNewObject(hCallBack, &hNewObject));
  }
loc_exit:
  return error;
  */
}



/*
1. проенумерим все файлы на предмет поиска изменений.
2. найдем тома, которые надо поменять.
*/


tQWORD CalcHash(hHASH hMD5, CABVolumeData* Cvd, tDWORD dwStartupVolume, tDWORD dwNumberOfFiles, FileData* pFiles)
{
tUINT ns;
tQWORD result;
tBYTE Hash[32];
  
  CALL_Hash_Reset(hMD5);
  // Calculate hash
  // Calc VolData
  CALL_Hash_Update(hMD5, (tBYTE*)&Cvd[dwStartupVolume], sizeof(CABVolumeData) - sizeof(tPTR) - sizeof(hOBJECT) - sizeof(hOBJECT) - sizeof(tBOOL) - sizeof(Cvd->szFileName));
  for(ns=0; ns < Cvd[dwStartupVolume].cFolders; ns++ )
  {
    CALL_Hash_Update(hMD5, (tBYTE*)&Cvd[dwStartupVolume].pFolder[ns], sizeof(CABFolderData) - sizeof(tPTR));
  }
  // calc file names
  for(ns=0; ns < dwNumberOfFiles ;ns++ )
  {
    if(pFiles[ns].pVolume == &Cvd[dwStartupVolume])
    {
      CALL_Hash_Update(hMD5, (tBYTE*)&pFiles[ns], sizeof(tDWORD) + sizeof(tDWORD)); //cbFile + uoffFolderStart
      CALL_Hash_Update(hMD5, (tBYTE*)&pFiles[ns].date, 3 * sizeof(tWORD) + SIZEOFNAME); //date + time + attrib + name
    }
  }
  
  CALL_Hash_GetHash(hMD5, Hash, sizeof(Hash));
  result = *(tQWORD*)&Hash[0];
  result^= *(tQWORD*)&Hash[8];
  return result;
}
