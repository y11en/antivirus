/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject FAT Wrapper interface               *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/


#include <prague.h>
#include <iface/i_io.h>
#include <iface/i_fat.h>
#include "i_iofat.h"


#define PROP_ORIGIN           0x1
#define PROP_ALLOC_OFFSET     0x2
#define PROP_ALLOC_SIZE       0x4
#define PROP_FAT              0x8
#define PROP_FIRST_CLUSTER    0x10
#define PROP_SIZE             0x20
#define PROP_INIT             (PROP_ORIGIN|PROP_ALLOC_OFFSET|PROP_ALLOC_SIZE|PROP_FAT|PROP_FIRST_CLUSTER|PROP_SIZE)

/* internal data structure */
typedef struct stFAT_DATA
{
    tDWORD      init;               /* initialization state of object */
    hIO         origin;             /* origin io object */
    tDWORD      allocoffset;        /* allocation space offset */
    tDWORD      allocsize;          /* allocation space size */
    tDWORD      clustersize;        /*  */
    tDWORD      startcluster;       /* first usable cluster */
    tDWORD      endcluster;         /* last usable cluster */
    hFAT        fat;                /* allocation table object */

    /* current object */
    tDWORD      firstobjectcluster; /* first cluster of object chain */
    tDWORD      objectsize;         /* size */
                                      
    tDWORD      chain[ 10 ];        /* current cluster chain */
    tDWORD      chainlen;           /* current chain length */
    tDWORD      currentcluster;     /* current cluster in object space */
                                    
} tFAT_DATA;                        



/* external interface functions */
static tDWORD  FATSeekRead   (hIO file, tDWORD offset, tPTR buffer, tDWORD count);
static tDWORD  FATSeekWrite  (hIO file, tDWORD offset, tPTR buffer, tDWORD count);
static tQWORD  FATSeekReadQ  (hIO file, tQWORD offset, tPTR buffer, tQWORD count);
static tQWORD  FATSeekWriteQ (hIO file, tQWORD offset, tPTR buffer, tQWORD count);
static tDWORD  FATResize     (hIO file, tDWORD newsize);
static tQWORD  FATResizeQ    (hIO file, tQWORD newsize);

static iIOFILE FAT_external =
{
    FATSeekRead      ,
    FATSeekWrite     ,
    FATSeekReadQ     ,
    FATSeekWriteQ    ,
    FATResize        ,
    FATResizeQ
};

tCOUNT _SetProp( hIO object, tPROPID propid, tFAT_DATA* localdata, tPTR buffer, tCOUNT count );

/* static property */
mPROPERTY_TABLE(PropTable)
    mSHARED_PROPERTY    (pgINTERFACE_REVISION,0x00010000)
    mSHARED_PROPERTY    (pgPLUGIN_VERSION    ,0x00010000)
    mSHARED_PROPERTY    (pgOBJECT_ID         ,OID_GENERIC_IO)
    mSHARED_PROPERTY_PTR(pgPLUGIN_NAME       ,"FAT"    )
    mSHARED_PROPERTY_PTR(pgPLUGIN_COMMENT    ,"FAT I/O Wrapper")

    mLOCAL_PROPERTY     (prIOFAT_ORIGIN               ,tFAT_DATA,origin,       cPROP_BUFFER_WRITE, NULL, _SetProp )
    mLOCAL_PROPERTY     (prIOFAT_ALLOC_OFFSET         ,tFAT_DATA,allocoffset,  cPROP_BUFFER_WRITE, NULL, _SetProp ) 
    mLOCAL_PROPERTY     (prIOFAT_ALLOC_SIZE           ,tFAT_DATA,allocsize,    cPROP_BUFFER_WRITE, NULL, _SetProp )
    mLOCAL_PROPERTY_BUF (prIOFAT_CLUSTER_SIZE         ,tFAT_DATA,clustersize,  cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF (prIOFAT_START_CLUSTER        ,tFAT_DATA,startcluster, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_BUF (prIOFAT_END_CLUSTER          ,tFAT_DATA,endcluster,   cPROP_BUFFER_READ )
    mLOCAL_PROPERTY     (prIOFAT_FAT_OBJECT           ,tFAT_DATA,fat,          cPROP_BUFFER_WRITE, NULL, _SetProp )
    mLOCAL_PROPERTY_BUF (prIOFAT_OBJECT_FIRST_CLUSTER ,tFAT_DATA,firstobjectcluster, cPROP_BUFFER_READ|cPROP_BUFFER_WRITE )
    mLOCAL_PROPERTY_BUF (prIOFAT_OBJECT_SIZE          ,tFAT_DATA,objectsize,   cPROP_BUFFER_READ|cPROP_BUFFER_WRITE )
    
    mLOCAL_PROPERTY_BUF (pgOBJECT_SIZE                ,tFAT_DATA,objectsize,   cPROP_BUFFER_READ )

mPROPERTY_TABLE_END(PropTable)

void RegisterFAT(tRegisterInterface *RegisterInterface)
{
    RegisterInterface(IID_IO,PID_FAT_WRAPPER,SUBTYPE_ANY,0x00010000,VID_KASPERSKY_LAB,
                      NULL,
                      &FAT_external, (sizeof(FAT_external)/sizeof(tPTR)),
                      PropTable,mPROPERTY_TABLE_SIZE(PropTable),
                      sizeof(tFAT_DATA));
}

// ---
tCOUNT _SetProp( hIO object, tPROPID propid, tFAT_DATA* data, tPTR buffer, tCOUNT count ) {
  
  object;

  switch( propid ) {
    case prIOFAT_ORIGIN :
      data->init |= PROP_ORIGIN;
      break;
    case prIOFAT_ALLOC_OFFSET :
      data->init |= PROP_ALLOC_OFFSET;
      break;
    case prIOFAT_ALLOC_SIZE :
      data->init |= PROP_ALLOC_SIZE;
      break;
    case prIOFAT_FAT_OBJECT :
      if ( 
        ( data->fat->system->ObjectCheck((hOBJECT)data->fat,IID_FAT,PID_ANY,SUBTYPE_ANY) ) &&
        ( sizeof(data->clustersize)  == data->fat->system->PropertyGet((hOBJECT)data->fat,prFAT_CLUSTER_SIZE,&data->clustersize, sizeof(data->clustersize)) ) &&
        ( sizeof(data->startcluster) == data->fat->system->PropertyGet((hOBJECT)data->fat,prFAT_FIRST_DATA_CLUSTER,&data->startcluster,sizeof(data->startcluster)) ) &&
        ( sizeof(data->endcluster)   == data->fat->system->PropertyGet((hOBJECT)data->fat,prFAT_LAST_DATA_CLUSTER,&data->endcluster,sizeof(data->endcluster)) )
      ) {
        data->init |= PROP_FAT;
        data->currentcluster = 0xffffffff;
      }
      else {
        object->system->ErrorCodeSet( (hOBJECT)object, errINCOMPATIBLE_INTERFACE );
        data->fat = NULL;
        return 0;
      }
      break;
    case prIOFAT_OBJECT_FIRST_CLUSTER :
      data->init |= PROP_FIRST_CLUSTER;
  }
  return count;
}



/* external interface functions */
static tDWORD  FATSeekRead   (hIO file, tDWORD offset, tPTR buffer, tDWORD count) {
  tFAT_DATA* data;
  
  data = file->system->MemoryLock( (hOBJECT)file );
  
  if ( data != NULL && ((data->init & PROP_INIT)==PROP_INIT) && offset < data->allocsize ) {
    
    tDWORD error;
    tDWORD first_cluster;
    tDWORD have_to_read;
    
    error = errOK;
    
    first_cluster = offset / data->clustersize;
    
    if ( first_cluster < data->currentcluster ) {
      data->currentcluster = 0;
      data->chainlen = 1;
      data->chain[0] = data->firstobjectcluster;
    }
    
    while( data->currentcluster + data->chainlen < first_cluster ) {
      
      tDWORD prev_len = data->chainlen;
      data->chainlen = sizeof(data->chain)/sizeof(data->chain[0]);
      error = data->fat->user->GetNextClusterChain( data->fat, data->chain[data->chainlen-1], data->chain, &data->chainlen );
      if ( error == errOK )
        data->currentcluster += prev_len;
      else {
        data->currentcluster = 0xffffffff;
        file->system->MemoryUnlock( (hOBJECT)file );
        return 0;
      }
    }
    
    if ( count > data->allocsize - offset )
      count = data->allocsize - offset;

    have_to_read = count;
    
    while( error == errOK && have_to_read ) {

      tDWORD wrk = data->currentcluster - first_cluster;
      tDWORD offs = offset - first_cluster * data->clustersize;
      tDWORD len = data->clustersize - offs;
      
      if ( len > have_to_read )
        len = have_to_read;
      
      for( ; wrk < data->chainlen; wrk++ ) {
        
        if ( len > have_to_read )
          len = have_to_read;
        
        len = data->origin->user->IOSeekRead( 
          data->origin, 
          data->allocoffset + data->chain[wrk] * data->clustersize + offs, 
          buffer, 
          len 
        );
        
        if ( len > 0 ) {
          have_to_read -= len;
          *((tCHAR**)&buffer) += len;
          offset += len;
        }
        else {
          error = file->system->ErrorCodeGet( (hOBJECT)file );
          if ( error == errOK )
            error = errEOB;
          break;
        }
        
        offs = 0;
        len = data->clustersize;
        
      }

      if ( error == errOK == have_to_read > 0 ) {
        tDWORD prev_len = data->chainlen;
        data->chainlen = sizeof(data->chain)/sizeof(data->chain[0]);
        error = data->fat->user->GetNextClusterChain( data->fat, data->chain[data->chainlen-1], data->chain, &data->chainlen );
        if ( error == errOK ) 
          first_cluster = data->currentcluster += prev_len;
        else {
          data->currentcluster = 0xffffffff;
          break;
        }
      }
    }
    
    return count - have_to_read;
  }
  else
    return 0;
}

static tDWORD  FATSeekWrite  (hIO file, tDWORD offset, tPTR buffer, tDWORD count)
{
    tDWORD write;
    tFAT_DATA* data;

    write = 0;
    data = file->system->MemoryLock( (hOBJECT)file );

    if ( data != NULL && ((data->init & PROP_INIT)==PROP_INIT) ) {
      if ( offset < data->allocsize ) {
        write = ( offset + count > data->allocsize ) ? data->allocsize - offset : count;
        write = data->origin->user->IOSeekWrite( data->origin, data->allocoffset+offset, buffer, write );
      }
      else {
        file->system->ErrorCodeSet( (hOBJECT)file, errEOB );
      }
    }
    return write;
}

static tQWORD  FATSeekReadQ  (hIO file, tQWORD offset, tPTR buffer, tQWORD count)
{
    tQWORD read;
    tFAT_DATA* data;

    read = 0;
    data = file->system->MemoryLock( (hOBJECT)file );

    if ( data != NULL && data->origin ) {
      if ( offset < data->allocsize ) {
        read = ( offset + count > data->allocsize ) ? data->allocsize - offset : count;
        read = data->origin->user->IOSeekReadQ( data->origin, data->allocoffset+offset, buffer, read );
      }
      else {
        file->system->ErrorCodeSet( (hOBJECT)file, errEOB );
      }
    }
    return read;
}

static tQWORD  FATSeekWriteQ (hIO file, tQWORD offset, tPTR buffer, tQWORD count)
{
    tQWORD read;
    tFAT_DATA* data;

    read = 0;
    data = file->system->MemoryLock( (hOBJECT)file );

    if ( data != NULL && data->origin ) {
      if ( offset < data->allocsize ) {
        read = ( offset + count > data->allocsize ) ? data->allocsize - offset : count;
        read = data->origin->user->IOSeekWriteQ( data->origin, data->allocoffset+offset, buffer, read );
      }
      else {
        file->system->ErrorCodeSet( (hOBJECT)file, errEOB );
      }
    }
    return read;
}

static tDWORD  FATResize     (hIO file, tDWORD newsize)
{
  file->system->ErrorCodeSet( (hOBJECT)file, errNOT_SUPPORTED );
  return 0;
}

static tQWORD  FATResizeQ    (hIO file, tQWORD newsize)
{
  file->system->ErrorCodeSet( (hOBJECT)file, errNOT_SUPPORTED );
  return 0;
}

