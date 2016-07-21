// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  08 February 2002,  15:50 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Sequential File IO
// Purpose     -- Not defined
// Author      -- Andrew
// File Name   -- seqio.c
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define SeqIO_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "seqio.h"
// AVP Prague stamp end

#include <memory.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// SeqIO interface implementation
// Short comments -- Sequential input/output interface
// Extended comment -
//  Supplies facility sequential input/output on ordinary IO object
//  Object of the interface must have current position
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // INTERNAL DATA DESCRIPTION SECTION TRANSFERRED TO CORRESPONDING HEADER FILE
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
  // OBJECT DESCRIPTION SECTION TRANSFERRED TO CORRESPONDING HEADER FILE
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call SeqIO_ObjectInit( hi_SeqIO _this );
tERROR pr_call SeqIO_ObjectClose( hi_SeqIO _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_SeqIO_vtbl =
{
    (tIntFnRecognize)        NULL,
    (tIntFnObjectNew)        NULL,
    (tIntFnObjectInit)       SeqIO_ObjectInit,
    (tIntFnObjectInitDone)   NULL,
    (tIntFnObjectCheck)      NULL,
    (tIntFnObjectPreClose)   NULL,
    (tIntFnObjectClose)      SeqIO_ObjectClose,
    (tIntFnChildNew)         NULL,
    (tIntFnChildInitDone)    NULL,
    (tIntFnChildClose)       NULL,
    (tIntFnMsgReceive)       NULL,
    (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )

// ----------- Standard IO methods ----------
typedef   tERROR (pr_call *fnpSeqIO_SeekRead)     ( hi_SeqIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ); // -- ;
typedef   tERROR (pr_call *fnpSeqIO_SeekWrite)    ( hi_SeqIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ); // -- ;
typedef   tERROR (pr_call *fnpSeqIO_GetSize)      ( hi_SeqIO _this, tQWORD* result, IO_SIZE_TYPE type );   // -- Return size of file;
typedef   tERROR (pr_call *fnpSeqIO_SetSize)      ( hi_SeqIO _this, tQWORD new_size );     // -- Operation not possible while using region or stream reversing;
typedef   tERROR (pr_call *fnpSeqIO_Flush)        ( hi_SeqIO _this );                      // -- flush internal buffers;

// ----------- Additional Sequential IO methods - read ----------
typedef   tERROR (pr_call *fnpSeqIO_Read)         ( hi_SeqIO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- ;
typedef   tERROR (pr_call *fnpSeqIO_ReadByte)     ( hi_SeqIO _this, tBYTE* result );                      // -- ;
typedef   tERROR (pr_call *fnpSeqIO_ReadWord)     ( hi_SeqIO _this, tWORD* result );                      // -- ;
typedef   tERROR (pr_call *fnpSeqIO_ReadDword)    ( hi_SeqIO _this, tDWORD* result );                      // -- ;
typedef   tERROR (pr_call *fnpSeqIO_ReadQword)    ( hi_SeqIO _this, tQWORD* result );                      // -- ;

// ----------- Additional Sequential IO methods - write ----------
typedef   tERROR (pr_call *fnpSeqIO_Write)        ( hi_SeqIO _this, tDWORD* result, tPTR buffer, tDWORD count ); // -- ;
typedef   tERROR (pr_call *fnpSeqIO_WriteByte)    ( hi_SeqIO _this, tBYTE value );         // -- Writes byte to stream;
typedef   tERROR (pr_call *fnpSeqIO_WriteWord)    ( hi_SeqIO _this, tWORD value );         // -- ;
typedef   tERROR (pr_call *fnpSeqIO_WriteDword)   ( hi_SeqIO _this, tDWORD value );        // -- ;
typedef   tERROR (pr_call *fnpSeqIO_WriteQword)   ( hi_SeqIO _this, tQWORD value );        // -- ;

// ----------- Additional Sequential IO methods - set file position ----------
typedef   tERROR (pr_call *fnpSeqIO_Seek)         ( hi_SeqIO _this, tQWORD* result, tQWORD offset, tDWORD origin ); // -- ;
typedef   tERROR (pr_call *fnpSeqIO_SeekSet)      ( hi_SeqIO _this, tQWORD* result, tQWORD offset );       // -- ;
typedef   tERROR (pr_call *fnpSeqIO_SeekForward)  ( hi_SeqIO _this, tQWORD* result, tQWORD offset );       // -- ;
typedef   tERROR (pr_call *fnpSeqIO_SeekBack)     ( hi_SeqIO _this, tQWORD* result, tQWORD offset );       // -- ;
typedef   tERROR (pr_call *fnpSeqIO_SeekEnd)      ( hi_SeqIO _this, tQWORD* result, tQWORD offset );       // -- ;
typedef   tERROR (pr_call *fnpSeqIO_SetSizeByPos) ( hi_SeqIO _this, tQWORD* result );                      // -- Set file size according current position;
typedef   tERROR (pr_call *fnpSeqIO_IsNotEOF)     ( hi_SeqIO _this );                      // -- ;

// ----------- Additional Sequential IO methods - set file region ----------
typedef   tERROR (pr_call *fnpSeqIO_RegionSet)    ( hi_SeqIO _this, tQWORD start, tQWORD size ); // -- ;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iSeqIOVtbl
{

// ----------- Standard IO methods ----------
    fnpSeqIO_SeekRead      SeekRead;
    fnpSeqIO_SeekWrite     SeekWrite;
    fnpSeqIO_GetSize       GetSize;
    fnpSeqIO_SetSize       SetSize;
    fnpSeqIO_Flush         Flush;

// ----------- Additional Sequential IO methods - read ----------
    fnpSeqIO_Read          Read;
    fnpSeqIO_ReadByte      ReadByte;
    fnpSeqIO_ReadWord      ReadWord;
    fnpSeqIO_ReadDword     ReadDword;
    fnpSeqIO_ReadQword     ReadQword;

// ----------- Additional Sequential IO methods - write ----------
    fnpSeqIO_Write         Write;
    fnpSeqIO_WriteByte     WriteByte;
    fnpSeqIO_WriteWord     WriteWord;
    fnpSeqIO_WriteDword    WriteDword;
    fnpSeqIO_WriteQword    WriteQword;

// ----------- Additional Sequential IO methods - set file position ----------
    fnpSeqIO_Seek          Seek;
    fnpSeqIO_SeekSet       SeekSet;
    fnpSeqIO_SeekForward   SeekForward;
    fnpSeqIO_SeekBack      SeekBack;
    fnpSeqIO_SeekEnd       SeekEnd;
    fnpSeqIO_SetSizeByPos  SetSizeByPos;
    fnpSeqIO_IsNotEOF      IsNotEOF;

// ----------- Additional Sequential IO methods - set file region ----------
    fnpSeqIO_RegionSet     RegionSet;
}; // "SeqIO" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )


// ----------  Standard IO methods  ----------
tERROR pr_call SeqIO_SeekRead( hi_SeqIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count );
tERROR pr_call SeqIO_SeekWrite( hi_SeqIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count );
tERROR pr_call SeqIO_GetSize( hi_SeqIO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call SeqIO_SetSize( hi_SeqIO _this, tQWORD new_size );
tERROR pr_call SeqIO_Flush( hi_SeqIO _this );


// ----------  Additional Sequential IO methods - read  ----------
tERROR pr_call SeqIO_Read( hi_SeqIO _this, tDWORD* result, tPTR buffer, tDWORD count );
tERROR pr_call SeqIO_ReadByte( hi_SeqIO _this, tBYTE* result );
tERROR pr_call SeqIO_ReadWord( hi_SeqIO _this, tWORD* result );
tERROR pr_call SeqIO_ReadDword( hi_SeqIO _this, tDWORD* result );
tERROR pr_call SeqIO_ReadQword( hi_SeqIO _this, tQWORD* result );


// ----------  Additional Sequential IO methods - write  ----------
tERROR pr_call SeqIO_Write( hi_SeqIO _this, tDWORD* result, tPTR buffer, tDWORD count );
tERROR pr_call SeqIO_WriteByte( hi_SeqIO _this, tBYTE value );
tERROR pr_call SeqIO_WriteWord( hi_SeqIO _this, tWORD value );
tERROR pr_call SeqIO_WriteDword( hi_SeqIO _this, tDWORD value );
tERROR pr_call SeqIO_WriteQword( hi_SeqIO _this, tQWORD value );


// ----------  Additional Sequential IO methods - set file position  ----------
tERROR pr_call SeqIO_Seek( hi_SeqIO _this, tQWORD* result, tQWORD offset, tDWORD origin );
tERROR pr_call SeqIO_SeekSet( hi_SeqIO _this, tQWORD* result, tQWORD offset );
tERROR pr_call SeqIO_SeekForward( hi_SeqIO _this, tQWORD* result, tQWORD offset );
tERROR pr_call SeqIO_SeekBack( hi_SeqIO _this, tQWORD* result, tQWORD offset );
tERROR pr_call SeqIO_SeekEnd( hi_SeqIO _this, tQWORD* result, tQWORD offset );
tERROR pr_call SeqIO_SetSizeByPos( hi_SeqIO _this, tQWORD* result );
tERROR pr_call SeqIO_IsNotEOF( hi_SeqIO _this );


// ----------  Additional Sequential IO methods - set file region  ----------
tERROR pr_call SeqIO_RegionSet( hi_SeqIO _this, tQWORD start, tQWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iSeqIOVtbl e_SeqIO_vtbl =
{
    SeqIO_SeekRead,
    SeqIO_SeekWrite,
    SeqIO_GetSize,
    SeqIO_SetSize,
    SeqIO_Flush,
    SeqIO_Read,
    SeqIO_ReadByte,
    SeqIO_ReadWord,
    SeqIO_ReadDword,
    SeqIO_ReadQword,
    SeqIO_Write,
    SeqIO_WriteByte,
    SeqIO_WriteWord,
    SeqIO_WriteDword,
    SeqIO_WriteQword,
    SeqIO_Seek,
    SeqIO_SeekSet,
    SeqIO_SeekForward,
    SeqIO_SeekBack,
    SeqIO_SeekEnd,
    SeqIO_SetSizeByPos,
    SeqIO_IsNotEOF,
    SeqIO_RegionSet
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call SeqIO_PROP_ReadUpProperty( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_WriteUpProperty( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_GetAccessMode( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_SetAccessMode( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_GetAvailability( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_SetByteOrder( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_SetStreamOrder( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_SetBufferOrder( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SeqIO_PROP_SetBufferSize( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "SeqIO". Global(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "SeqIO". Property table
mPROPERTY_TABLE(SeqIO_PropTable)
    mSHARED_PROPERTY( pgINTERFACE_VERSION, ((tVERSION)(2)) )
    mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Sequential file I/O", 20 )
    mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(IID_IO)) )
    mLOCAL_PROPERTY_FN( pgOBJECT_NAME, SeqIO_PROP_ReadUpProperty, SeqIO_PROP_WriteUpProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_PATH, SeqIO_PROP_ReadUpProperty, SeqIO_PROP_WriteUpProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, SeqIO_PROP_ReadUpProperty, SeqIO_PROP_WriteUpProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_OPEN_MODE, SeqIO_PROP_ReadUpProperty, SeqIO_PROP_WriteUpProperty )
    mLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, tSEQIO_DATA, access_mode, cPROP_BUFFER_READ_WRITE, SeqIO_PROP_GetAccessMode, SeqIO_PROP_SetAccessMode )
    mLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, SeqIO_PROP_ReadUpProperty, SeqIO_PROP_WriteUpProperty )
    mLOCAL_PROPERTY( pgOBJECT_AVAILABILITY, tSEQIO_DATA, availability, cPROP_BUFFER_READ, SeqIO_PROP_GetAvailability, NULL )
    mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, tSEQIO_DATA, io, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_FN( pgOBJECT_FILL_CHAR, SeqIO_PROP_ReadUpProperty, SeqIO_PROP_WriteUpProperty )
    mLOCAL_PROPERTY( pgBYTE_ORDER, tSEQIO_DATA, byte_order, cPROP_BUFFER_READ_WRITE, NULL, SeqIO_PROP_SetByteOrder )
    mLOCAL_PROPERTY( pgBUFFER_ORDER, tSEQIO_DATA, buffer_order, cPROP_BUFFER_READ_WRITE, NULL, SeqIO_PROP_SetBufferOrder )
    mLOCAL_PROPERTY( pgBUFFER_SIZE, tSEQIO_DATA, prop_buffer_size, cPROP_BUFFER_READ_WRITE, NULL, SeqIO_PROP_SetBufferSize )
    mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, tSEQIO_DATA, origin, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(SeqIO_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "SeqIO". Register function
tERROR pr_call SeqIO_Register( hROOT root )
{
    tERROR error;

    PR_TRACE_A0( root, "Enter \"SeqIO::Register\" method" );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_SEQIO,                              // interface identifier
        PID_SEQ_IO,                             // plugin identifier
        0x00000000,                             // subtype identifier
        0x00000001,                             // interface version
        VID_ANDREW,                         // interface developer
        &i_SeqIO_vtbl,                          // internal(kernel called) function table
        (sizeof(i_SeqIO_vtbl)/sizeof(tPTR)),    // internal function table size
        &e_SeqIO_vtbl,                          // external function table
        (sizeof(e_SeqIO_vtbl)/sizeof(tPTR)),    // external function table size
        SeqIO_PropTable,                        // property table
        mPROPERTY_TABLE_SIZE(SeqIO_PropTable),  // property table size
        sizeof(tSEQIO_DATA),                    // memory size
        IFACE_SYSTEM                            // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"SeqIO(IID_SEQIO) registered [error=0x%08x]",error) );
    #endif // _PRAGUE_TRACE_

    PR_TRACE_A1( root, "Leave \"SeqIO::Register\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end

// static local procedures
static void SwapByte(tPTR buffer, tUINT count)
{
    tBYTE *begin;
    tBYTE *end;
    tBYTE tmp;

    begin = buffer;
    end = begin + count -1;

    count /= 2;

    while ( count-- )
    {
        tmp = *begin;
        *begin++ = *end;
        *end-- = tmp;
    }
}

static tERROR SeekRead(tSEQIO_DATA *data, tDWORD *result, tQWORD offset, tPTR buffer, tDWORD count)
{
    tERROR error = errOK;
    int err = 0;
    /* usign region settings, ignore buffer */

    if ( data->region_active )
    {
        tQWORD new_offset;

        new_offset = data->region_start+offset;
        if ( new_offset < offset || new_offset > data->region_end )
        {
            error = errEOF;
            *result = 0;
        }
        else
        {
            offset = new_offset;
            new_offset = data->region_end-offset;
            if ( (tQWORD)(count) > new_offset )
            {
                count = (tDWORD)(new_offset);
                err = 1;
            }
        }
    }

    if ( error == errOK )
    {
        error = data->io->vtbl->SeekRead(data->io,result,offset,buffer,count);
        if ( err && error == errOK )
        {
            error = errEOF;
        }
    }
	return error;
}

static tERROR SeekWrite(tSEQIO_DATA *data, tDWORD *result, tQWORD offset, tPTR buffer, tDWORD count)
{
    tERROR error = errOK;
    int err = 0;
    /* usign region settings, ignore buffer */

    if ( data->region_active )
    {
        tQWORD new_offset;

        new_offset = data->region_start+offset;
        if ( new_offset < offset || new_offset > data->region_end )
        {
            error = errEOF;
            *result = 0;
        }
        else
        {
            offset = new_offset;
            new_offset = data->region_end-offset;
            if ( (tQWORD)(count) > new_offset )
            {
                count = (tDWORD)(new_offset);
                err = 1;
            }
        }
    }

    if ( error == errOK )
    {
        error = data->io->vtbl->SeekWrite(data->io,result,offset,buffer,count);
        if ( err && error == errOK )
        {
            error = errEOF;
        }
    }
	return error;
}

static tERROR InvalidateBuffer(tSEQIO_DATA *data)
{
    tERROR error = errOK;

    if ( data->buffer_dirty && data->buffer_count)
    {
        /* flush buffer */
        error = SeekWrite(data,NULL,data->buffer_offset,data->buffer,data->buffer_count);
        data->buffer_dirty = cFALSE;
    }

    data->buffer_offset += data->buffer_ptr;
    data->buffer_ptr = 0;
    data->buffer_count = 0;
    data->buffer_wsize = 0;
	return error;
}

static tERROR GetSize(tSEQIO_DATA *data, IO_SIZE_TYPE type)
{
    tERROR error = errOK;

    if ( !data->file_size_valid )
    {
        error = data->io->vtbl->GetSize(data->io,&data->file_size,type);
        if ( error == errOK )
        {
            if ( data->region_active )
            {
                if ( data->file_size <= data->region_start )
                {
                    data->file_size = 0;
                }
                else
                {
                    if ( data->file_size < data->region_end )
                    {
                        data->file_size -= data->region_start;
                    }
                    else
                    {
                        data->file_size = data->region_end - data->region_start;
                    }
                }
            }
            data->file_size_valid = cTRUE;
        }
    }
    return error;
}

tERROR SetAccessMode(tSEQIO_DATA *data)
{
    tERROR error;
    error = data->io->sys->PropertyGet((hOBJECT)data->io,NULL,pgOBJECT_ACCESS_MODE,&data->access_mode,sizeof(data->access_mode));
    if ( error == errOK )
    {
        data->read_access       = ( data->access_mode & fACCESS_READ ) ? cTRUE:cFALSE;
        data->write_access      = ( data->access_mode & fACCESS_WRITE) ? cTRUE:cFALSE;
        data->read_buffering    = ( data->access_mode & fACCESS_NO_BUFFERING_READ ) ? cFALSE:cTRUE;
        data->write_buffering   = ( data->access_mode & fACCESS_NO_BUFFERING_WRITE) ? cFALSE:cTRUE;
    }
    data->write_test = cWRITE_TEST_NONE;
    return error;
}
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, ReadUpProperty )
// Interface "SeqIO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_DELETE_ON_CLOSE
//  -- OBJECT_FILL_CHAR
tERROR pr_call SeqIO_PROP_ReadUpProperty( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error;
    PR_TRACE_A0( _this, "Enter *GET* multyproperty method \"SeqIO_PROP_ReadUpProperty\"" );

    error = _this->data->io->sys->PropertyGet((hOBJECT)(_this->data->io),out_size,prop,buffer,size);

    PR_TRACE_A2( _this, "Leave *GET* multyproperty method \"SeqIO_PROP_ReadUpProperty\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, WriteUpProperty )
// Interface "SeqIO". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_DELETE_ON_CLOSE
//  -- OBJECT_FILL_CHAR
tERROR pr_call SeqIO_PROP_WriteUpProperty( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error;
    PR_TRACE_A0( _this, "Enter *SET* multyproperty method \"SeqIO_PROP_WriteUpProperty\"" );

    error = _this->data->io->sys->PropertySet((hOBJECT)(_this->data->io),out_size,prop,buffer,size);
    if ( error == errOK && size == sizeof(tBOOL) )
    {
        memcpy(&_this->data->delete_on_close,buffer,sizeof(tBOOL));
    }

    PR_TRACE_A2( _this, "Leave *SET* multyproperty method \"SeqIO_PROP_WriteUpProperty\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetAccessMode )
// Interface "SeqIO". Method "Get" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call SeqIO_PROP_GetAccessMode( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *GET* method \"SeqIO_PROP_GetAccessMode\" for property \"pgOBJECT_ACCESS_MODE\"" );

    error = _this->data->io->sys->PropertyGet((hOBJECT)_this->data->io,NULL,pgOBJECT_ACCESS_MODE,&_this->data->access_mode,sizeof(_this->data->access_mode));
    if ( error == errOK )
    {
        _this->data->access_mode &= ~fACCESS_NO_BUFFERING;

        if ( !_this->data->read_buffering )
        {
            _this->data->access_mode |= fACCESS_NO_BUFFERING_READ;
        }

        if ( !_this->data->write_buffering )
        {
            _this->data->access_mode |= fACCESS_NO_BUFFERING_WRITE;
        }
        *out_size = sizeof(_this->data->access_mode);
        if ( buffer != NULL ) {
            if ( *out_size > size )
                error = errBUFFER_TOO_SMALL;
            else
                *(tDWORD*)buffer = _this->data->access_mode;
        }
    }
    else
    {
        *out_size = 0;
    }

    PR_TRACE_A2( _this, "Leave *GET* method \"SeqIO_PROP_GetAccessMode\" for property \"pgOBJECT_ACCESS_MODE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetAccessMode )
// Interface "SeqIO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call SeqIO_PROP_SetAccessMode( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *SET* method \"SeqIO_PROP_SetAccessMode\" for property \"pgOBJECT_ACCESS_MODE\"" );


    InvalidateBuffer(_this->data);

    error = _this->data->io->sys->PropertySet((hOBJECT)_this->data->io,out_size,prop,buffer,size);
    if ( error == errOK )
    {
        error = SetAccessMode(_this->data);
        *out_size = size;
    }


    PR_TRACE_A2( _this, "Leave *SET* method \"SeqIO_PROP_SetAccessMode\" for property \"pgOBJECT_ACCESS_MODE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetAvailability )
// Interface "SeqIO". Method "Get" for property(s):
//  -- OBJECT_AVAILABILITY
tERROR pr_call SeqIO_PROP_GetAvailability( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *GET* method \"SeqIO_PROP_GetAvailability\" for property \"pgOBJECT_AVAILABILITY\"" );


    error = _this->data->io->sys->PropertyGet((hOBJECT)_this->data->io,NULL,pgOBJECT_AVAILABILITY,&_this->data->availability,sizeof(_this->data->availability));
    if ( error == errOK )
    {
        _this->data->availability |= fAVAIL_BUFFERING;
        *out_size = sizeof(_this->data->availability);
    }
    else
    {
        *out_size = 0;
    }

    PR_TRACE_A2( _this, "Leave *GET* method \"SeqIO_PROP_GetAvailability\" for property \"pgOBJECT_AVAILABILITY\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetByteOrder )
// Interface "SeqIO". Method "Set" for property(s):
//  -- BYTE_ORDER
tERROR pr_call SeqIO_PROP_SetByteOrder( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter *SET* method \"SeqIO_PROP_SetByteOrder\" for property \"pgBYTE_ORDER\"" );
    data = _this->data;
    *out_size = sizeof(tSBYTE);

    switch ( data->byte_order )
    {
        case cORDER_DIRECT:
            data->byte_reverse = cFALSE;
            break;

        case cORDER_BACK:
            data->byte_reverse = cTRUE;
            break;

        case cORDER_REVERSE:
            if ( data->byte_reverse )
            {
                data->byte_reverse = cFALSE;
                data->byte_order = cORDER_DIRECT;
            }
            else
            {
                data->byte_reverse = cTRUE;
                data->byte_order = cORDER_BACK;
            }
            break;
        default:
            // unknown value
            error = errPARAMETER_INVALID;
            *out_size = 0;
            // restore previous values
            if ( data->byte_reverse )
            {
                data->byte_order = cORDER_BACK;
            }
            else
            {
                data->byte_order = cORDER_DIRECT;
            }

    }

    PR_TRACE_A2( _this, "Leave *SET* method \"SeqIO_PROP_SetByteOrder\" for property \"pgBYTE_ORDER\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetBufferOrder )
// Interface "SeqIO". Method "Set" for property(s):
//  -- BUFFER_ORDER
tERROR pr_call SeqIO_PROP_SetBufferOrder( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter *SET* method \"SeqIO_PROP_SetBufferOrder\" for property \"pgBUFFER_ORDER\"" );

    data = _this->data;
    *out_size = sizeof(tSBYTE);

    switch ( data->buffer_order )
    {
        case cORDER_DIRECT:
            data->buffer_reverse = cFALSE;
            break;

        case cORDER_BACK:
            data->buffer_reverse = cTRUE;
            break;

        case cORDER_REVERSE:
            if ( data->buffer_reverse )
            {
                data->buffer_reverse = cFALSE;
                data->buffer_order = cORDER_DIRECT;
            }
            else
            {
                data->buffer_reverse = cTRUE;
                data->buffer_order = cORDER_BACK;
            }
            break;
        default:
            // unknown value
            error = errPARAMETER_INVALID;
            *out_size = 0;
            // restore previous values
            if ( data->buffer_reverse )
            {
                data->buffer_order = cORDER_BACK;
            }
            else
            {
                data->buffer_order = cORDER_DIRECT;
            }

    }

    PR_TRACE_A2( _this, "Leave *SET* method \"SeqIO_PROP_SetBufferOrder\" for property \"pgBUFFER_ORDER\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetBufferSize )
// Interface "SeqIO". Method "Set" for property(s):
//  -- BUFFER_SIZE
tERROR pr_call SeqIO_PROP_SetBufferSize( hi_SeqIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter *SET* method \"SeqIO_PROP_SetBufferSize\" for property \"pgBUFFER_SIZE\"" );

    *out_size = sizeof(tDWORD);
    data = _this->data;
    data->prop_buffer_size = *(tDWORD*)buffer;

    InvalidateBuffer(data);
    if ( data->prop_buffer_size != data->buffer_rsize )
    {
//        tDWORD required;
//
//        required = (sizeof(tSEQIO_DATA)-cDEFAULT_BUFFER_SIZE)+_this->data->prop_buffer_size;
//        if ( (error = _this->sys->MemoryRealloc((hOBJECT)_this,required)) == errOK )
        if ( (error = CALL_SYS_ObjHeapRealloc((hOBJECT)_this,(void**)&data->buffer,data->buffer,data->prop_buffer_size)) == errOK )
        {
            data->buffer_rsize = data->prop_buffer_size;
        }
        else
        {
            data->prop_buffer_size = data->buffer_rsize;
            *out_size = 0;
        }
    }

    PR_TRACE_A2( _this, "Leave *SET* method \"SeqIO_PROP_SetBufferSize\" for property \"pgBUFFER_SIZE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call SeqIO_ObjectInit( hi_SeqIO _this )
{
    tERROR error;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::ObjectInit\" method" );

    // Place your code here
    data = _this->data;
    data->io = (hIO)_this->sys->ParentGet((hOBJECT)_this,IID_ANY);
    error = _this->sys->ObjectCheck((hOBJECT)_this,(hOBJECT)data->io,IID_IO,PID_ANY,SUBTYPE_ANY,cTRUE);

    // petrovitch 16.05.03 (because MemoryRealloc became obsolete)
    if ( PR_SUCC(error) )
      error = CALL_SYS_ObjHeapAlloc( _this, (void**)&data->buffer, cDEFAULT_BUFFER_SIZE );

    if ( error == errOK )
    {
        data->file_size_valid = cFALSE;

        // access modes init
        data->delete_on_close = cFALSE;

        SetAccessMode(data);
        data->read_buffering = 1;
        data->write_buffering = 1;

        data->write_test = cWRITE_TEST_NONE;

        // direction init
        data->byte_order = cORDER_DIRECT;
        data->buffer_order = cORDER_DIRECT;

        data->byte_reverse = cFALSE;
        data->buffer_reverse = cFALSE;

        // buffer size init
        data->buffer_rsize = cDEFAULT_BUFFER_SIZE;
        data->buffer_wsize = 0; // direct write not allowed
        data->prop_buffer_size = cDEFAULT_BUFFER_SIZE;

        // buffer content init
        data->buffer_dirty = cFALSE;
        data->buffer_ptr = 0;
        data->buffer_count = 0;

        // region init
        data->region_start = 0;
        data->region_end = (-1);
        data->region_active = cFALSE;

        // file position init
        data->buffer_offset = 0;

        // availability init

        // origin init
        data->origin = data->io->sys->PropertyGetDWord((hOBJECT)data->io,pgOBJECT_ORIGIN);
    }

    PR_TRACE_A1( _this, "Leave \"SeqIO::ObjectInit\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call SeqIO_ObjectClose( hi_SeqIO _this )
{

    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter \"SeqIO::ObjectClose\" method" );

    // Place your code here
    if ( !_this->data->delete_on_close )
    {
        error = InvalidateBuffer(_this->data);
        // petrovitch 16.05.03 (because MemoryRealloc became obsolete)
        if ( _this->data->buffer )
          CALL_SYS_ObjHeapFree( _this, _this->data->buffer );
    }

    PR_TRACE_A1( _this, "Leave \"SeqIO::ObjectClose\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Parameters are:
tERROR pr_call SeqIO_SeekRead( hi_SeqIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count )
{
    tERROR error = errOK;
    tDWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SeekRead\" method" );

    // Place your code here
    data = _this->data;
    if ( data->read_access )
    {
        if ( buffer != NULL )
        {
            if ( data->buffer_count == 0                   ||
                 (offset+count) <= data->buffer_offset ||
                 offset >= (data->buffer_offset+data->buffer_count) )

            {
                // out of buffer or buffer is empty, simple operation
                error = SeekRead(data,&ret_val,offset,buffer,count);
            }
            else
            {
                tBYTE *buf;
                tDWORD mayread;

                buf = buffer;

                if ( offset < data->buffer_offset )
                {
                    // first part - before buffer
                    mayread = (tDWORD)(data->buffer_offset - offset);
                    error = SeekRead(data,&ret_val,offset,buf,mayread);
                    offset += ret_val;
                    count -= ret_val;
                    buf += ret_val;
                }

                if ( error == errOK )
                {
                    // second part - always in buffer
                    tDWORD boff;
                    boff = (tDWORD)(offset-data->buffer_offset);
                    mayread = data->buffer_count-boff;
                    if ( mayread > count )
                    {
                        mayread = count;
                    }
                    memcpy(buf,data->buffer+boff,mayread);
                    ret_val += mayread;
                    offset += mayread;
                    count -= mayread;
                    buf += mayread;
                }

                if ( count )
                {
                    // third part (if present) always after buffer
                    error = SeekRead(data,&mayread,offset,buf,count);
                    ret_val += mayread;
                }
            }
        }
        else
        {
            error = errPARAMETER_INVALID;
        }

    }
    else
    {
        error = errACCESS_DENIED;
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SeekRead\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Parameters are:
tERROR pr_call SeqIO_SeekWrite( hi_SeqIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count )
{
    tERROR error = errOK;
    tDWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SeekWrite\" method" );

    // Place your code here
    data = _this->data;
    if ( data->write_access )
    {
        if ( buffer != NULL )
        {
            if ( data->buffer_count == 0                   ||
                 (offset+count) <= data->buffer_offset ||
                 offset >= (data->buffer_offset+data->buffer_count))

            {
                // out of buffer or buffer is empty, simple operation
                error = SeekWrite(data,&ret_val,offset,buffer,count);
            }
            else
            {
                tBYTE *buf;
                tDWORD maywrite;

                buf = buffer;

                if ( offset < data->buffer_offset )
                {
                    // first part - before buffer
                    maywrite = (tDWORD)(data->buffer_offset - offset);
                    error = SeekWrite(data,&ret_val,offset,buf,maywrite);
                    offset += ret_val;
                    count -= ret_val;
                    buf += ret_val;
                }

                if ( error == errOK )
                {
                    // second part - always in buffer
                    tDWORD boff;
                    boff = (tDWORD)(offset-data->buffer_offset);
                    maywrite = data->buffer_count-boff;
                    if ( maywrite > count )
                    {
                        maywrite = count;
                    }
                    memcpy(data->buffer+boff,buf,maywrite);
                    ret_val += maywrite;
                    offset += maywrite;
                    count -= maywrite;
                    buf += maywrite;
                    data->buffer_dirty = cTRUE;
                }

                if ( count )
                {
                    // third part (if present) always after buffer
                    error = SeekWrite(data,&maywrite,offset,buf,count);
                    ret_val += maywrite;
                }
            }
        }
        else
        {
            error = errPARAMETER_INVALID;
        }
    }
    else
    {
        error = errOBJECT_READ_ONLY;
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SeekWrite\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Parameters are:
tERROR pr_call SeqIO_GetSize( hi_SeqIO _this, tQWORD* result, IO_SIZE_TYPE type )
{
    tERROR error;
    tQWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::GetSize\" method" );

    // Place your code here
    data = _this->data;

    InvalidateBuffer(data);

    error = GetSize(data, type);

    if ( error == errOK )
    {
        ret_val = data->file_size;
    }


    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::GetSize\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Extended method comment
//  Operation not possible while using region or stream reversing
// Parameters are:
tERROR pr_call SeqIO_SetSize( hi_SeqIO _this, tQWORD new_size )
{
    tERROR error;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SetSize\" method" );

    // Place your code here
    data = _this->data;
    if ( data->region_active )
    {
        error = errACCESS_DENIED;
    }
    else
    {
        InvalidateBuffer(data);
        data->file_size_valid = cFALSE;
        error = data->io->vtbl->SetSize(data->io, new_size);
        if ( error == errOK )
        {
            data->file_size = new_size;
            data->file_size_valid = cTRUE;
        }
    }

    PR_TRACE_A1( _this, "Leave \"SeqIO::SetSize\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//  Flushes internal buffers
// Behaviour comment
//  Returns real error of sequence previous write operations since last "Flush". Must return errOK if object opened in RO mode or write operation never invoked since last "Flush".
// Parameters are:
tERROR pr_call SeqIO_Flush( hi_SeqIO _this )
{
    tERROR error;
    PR_TRACE_A0( _this, "Enter \"SeqIO::Flush\" method" );

    // Place your code here
    error = InvalidateBuffer(_this->data);

    PR_TRACE_A1( _this, "Leave \"SeqIO::Flush\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Read )
// Parameters are:
tERROR pr_call SeqIO_Read( hi_SeqIO _this, tDWORD* result, tPTR buffer, tDWORD count )
{
    tERROR error = errOK;
    tDWORD ret_val = 0;
    tDWORD cur_count;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::Read\" method" );

    // Place your code here
    data = _this->data;

    if ( data->read_access )
    {
        tBYTE *buf;

        buf = buffer;

        if ( buffer != NULL )
        {
            cur_count = count;

            while ( cur_count && error == errOK )
            {
                tDWORD mayread;

                if ( data->buffer_ptr < data->buffer_count )
                {

                    /* some data presents in buffer */

                    mayread = data->buffer_count-data->buffer_ptr;

                    if ( mayread > cur_count )
                    {
                        mayread = cur_count;
                    }

                    memcpy(buf,data->buffer+data->buffer_ptr,mayread);
                    ret_val += mayread;
                    cur_count -= mayread;
                    buf += mayread;
                    data->buffer_ptr += mayread;

                }
                else
                {

                    /* request outside of buffer */
                    InvalidateBuffer(data);

                    if ( cur_count > data->buffer_rsize || !data->read_buffering )
                    {

                        /* request larger then buffer, probably buffering not necessary */
                        error = SeekRead(data,&mayread,data->buffer_offset,buf,cur_count);
						if( error == errOK && !mayread )
							error = errEOF;
						else
						{
							data->buffer_offset += mayread;
							cur_count -= mayread;
							ret_val += mayread;
						}
                        if ( error == errEOF )
                        {
                            data->file_eof = cTRUE;
                        }

                    }
                    else
                    {
                        /* trying to read buffer and repeat buffer access */
                        if ( data->buffer_reverse )
                        {
                            tDWORD limit;

                            limit = data->buffer_rsize/2;

                            if ( data->buffer_offset < limit )
                            {
                                data->buffer_ptr = (tDWORD)(data->buffer_offset);
                                data->buffer_offset = 0;
                            }
                            else
                            {
                                data->buffer_offset -= limit;
                                data->buffer_ptr = limit;
                            }
                        }

                        error = SeekRead(data,&data->buffer_count,data->buffer_offset,data->buffer,data->buffer_rsize);
                        if ( data->buffer_count != 0 )
                        {
                            error = errOK;
                            // EOF at the end of buffer
                            data->file_eof = cTRUE;
                        }
                        else if (error == errOK)
                        {
                            // SeekRead returns errOK *and* read zero bytes
                            error = errEOF;
                            data->file_eof = cTRUE;
                        }
                    }
                }
            }
            if (error == errOK && ret_val < count)
            {
                error = errEOF;
            }
        }
        else
        {
            error = errPARAMETER_INVALID;
        }

    }
    else
    {
        error = errACCESS_DENIED;
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::Read\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReadByte )
// Parameters are:
tERROR pr_call SeqIO_ReadByte( hi_SeqIO _this, tBYTE* result )
{
    tERROR error = errOK;
    tBYTE  ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::ReadByte\" method" );

    // Place your code here
    data = _this->data;

    if ( data->read_access )
    {
        if ( data->buffer_ptr < data->buffer_count )
        {
            ret_val = data->buffer[data->buffer_ptr++];
        }
        else
        {
            error = SeqIO_Read( _this, NULL, &ret_val, sizeof(ret_val));
        }
    }
    else
    {
        error = errACCESS_DENIED;
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::ReadByte\" method, ret tBYTE = %c, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReadWord )
// Parameters are:
tERROR pr_call SeqIO_ReadWord( hi_SeqIO _this, tWORD* result )
{
    tERROR error = errOK;
    tWORD  ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::ReadWord\" method" );

    // Place your code here
    data = _this->data;
    #include "ATOMR.h"

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::ReadWord\" method, ret tWORD = %u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReadDword )
// Parameters are:
tERROR pr_call SeqIO_ReadDword( hi_SeqIO _this, tDWORD* result )
{
    tERROR error = errOK;
    tDWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::ReadDword\" method" );

    // Place your code here
    data = _this->data;
    #include "ATOMR.h"

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::ReadDword\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReadQword )
// Parameters are:
tERROR pr_call SeqIO_ReadQword( hi_SeqIO _this, tQWORD* result )
{
    tERROR error = errOK;
    tQWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::ReadQword\" method" );

    // Place your code here
    data = _this->data;
    #include "ATOMR.h"

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::ReadQword\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Write )
// Parameters are:
tERROR pr_call SeqIO_Write( hi_SeqIO _this, tDWORD* result, tPTR buffer, tDWORD count )
{
    tERROR error = errOK;
    tDWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::Write\" method" );

    // Place your code here
    data = _this->data;
    if ( data->write_access )
    {
        if ( buffer != NULL )
        {
            tBYTE *buf;

            buf = buffer;

            while ( error == errOK && count )
            {
                tDWORD maywrite;

                if ( data->buffer_ptr < data->buffer_wsize )
                {
                    maywrite = data->buffer_wsize - data->buffer_ptr;
                    if ( maywrite > count )
                    {
                        maywrite = count;
                    }
                    memcpy(data->buffer+data->buffer_ptr,buf,maywrite);
                    data->buffer_ptr += maywrite;
                    buf += maywrite;
                    ret_val += maywrite;
					count -= maywrite;

                    if ( data->buffer_ptr > data->buffer_count )
                    {
                        data->buffer_count = data->buffer_ptr;
                    }
                    data->buffer_dirty = cTRUE;
                }
                else
                {
                    // request does not fit in buffer
                    InvalidateBuffer(data);
                    if ( count > data->buffer_rsize || !data->write_buffering )
                    {

                        /* request larger then buffer, probably buffering not necessary */
                        error = SeekWrite(data,&maywrite,data->buffer_offset,buf,count);
                        data->buffer_offset += maywrite;
                        count -= maywrite;
                        ret_val += maywrite;
                    }
                    else
                    {
                        error = GetSize(data,IO_SIZE_TYPE_EXPLICIT);

                        if ( error == errOK )
                        {
                            int level;
                            if ( data->buffer_offset > data->file_size )
                            {
                                // gap writing
                                data->file_size_valid = cFALSE;
                                level = cWRITE_TEST_GAP;
                            }
                            else if ( (data->buffer_offset+count) > data->file_size )
                            {
                                // after eof writing
                                data->file_size_valid = cFALSE;
                                level = cWRITE_TEST_EOF;
                            }
                            else
                            {
                                // standard write
                                level = cWRITE_TEST_INSIDE;
                            }

                            if ( level > data->write_test )
                            {
                                // required level higher than already tested
                                error = SeekWrite(data,&maywrite,data->buffer_offset,buf,count);
                                data->buffer_offset += maywrite;
                                count -= maywrite;
                                ret_val += maywrite;

                                if ( error == errOK )
                                {
                                    data->write_test = level;
                                    if ( !data->file_size_valid )
                                    {
                                        data->file_size_valid = cTRUE;
                                        data->file_size = data->buffer_offset;
                                    }
                                }
                            }
                            else
                            {
                                // required operation was tested, so set buffer parameters
                                if ( level == cWRITE_TEST_INSIDE && (data->buffer_offset+data->buffer_rsize) > data->file_size )
                                {
                                    data->buffer_wsize = (tUINT)(data->file_size - data->buffer_offset);
                                }
                                else
                                {
                                    data->buffer_wsize = data->buffer_rsize;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            error = errPARAMETER_INVALID;
        }
    }
    else
    {
        error = errACCESS_DENIED;
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::Write\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, WriteByte )
// Extended method comment
//  Writes byte to stream, return error if stream is read-only or position out of stream
// Parameters are:
tERROR pr_call SeqIO_WriteByte( hi_SeqIO _this, tBYTE value )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::WriteByte\" method" );

    // Place your code here
    data = _this->data;
    if ( data->write_access )
    {
        if ( data->buffer_ptr < data->buffer_wsize )
        {
            data->buffer[data->buffer_ptr++] = value;
            if ( data->buffer_count < data->buffer_ptr )
            {
                data->buffer_count = data->buffer_ptr;
            }
        }
        else
        {
            error = SeqIO_Write(_this,NULL,&value,sizeof(value));
        }
    }
    else
    {
        error = errACCESS_DENIED;
    }

    PR_TRACE_A1( _this, "Leave \"SeqIO::WriteByte\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, WriteWord )
// Parameters are:
tERROR pr_call SeqIO_WriteWord( hi_SeqIO _this, tWORD value )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::WriteWord\" method" );

    // Place your code here
    data = _this->data;
    #include "ATOMW.h"

    PR_TRACE_A1( _this, "Leave \"SeqIO::WriteWord\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, WriteDword )
// Parameters are:
tERROR pr_call SeqIO_WriteDword( hi_SeqIO _this, tDWORD value )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::WriteDword\" method" );

    // Place your code here
    data = _this->data;
    #include "ATOMW.h"

    PR_TRACE_A1( _this, "Leave \"SeqIO::WriteDword\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, WriteQword )
// Parameters are:
tERROR pr_call SeqIO_WriteQword( hi_SeqIO _this, tQWORD value )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::WriteQword\" method" );

    // Place your code here
    data = _this->data;
    #include "ATOMW.h"

    PR_TRACE_A1( _this, "Leave \"SeqIO::WriteQword\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Seek )
// Parameters are:
tERROR pr_call SeqIO_Seek( hi_SeqIO _this, tQWORD* result, tQWORD offset, tDWORD origin )
{
    tERROR error = errOK;
    tQWORD ret_val = 0;
    tQWORD real_offset;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::Seek\" method" );

    // Place your code here
    data = _this->data;

    real_offset = data->buffer_offset+data->buffer_ptr;

    // seek operations reset EOF flag

    data->file_eof = cFALSE;

    switch ( origin )
    {
        case cSEEK_SET     :
            // nothing to do
            break;

        case cSEEK_END     :
            data->file_size_valid = cFALSE;
            error = GetSize(data,IO_SIZE_TYPE_EXPLICIT);
            if ( error == errOK )
            {
                real_offset = data->file_size;
            }
            // no break here!
        case cSEEK_BACK    :
            if ( error == errOK )
            {
                if ( real_offset < offset )
                {
                    error = errBOF;
                }
                else
                {
                    offset = real_offset - offset;
                }
            }
            break;

        case cSEEK_FORWARD :
            offset += real_offset;
            break;

        default:
            error = errPARAMETER_INVALID;
    }

    if ( error == errOK )
    {
        error = SeqIO_SeekSet(_this,&ret_val,offset);
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::Seek\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end







// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekSet )
// Parameters are:
tERROR pr_call SeqIO_SeekSet( hi_SeqIO _this, tQWORD* result, tQWORD offset )
{
    tERROR error = errOK;
    tQWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SeekSet\" method" );

    // Place your code here
    data = _this->data;

    if ( offset >= data->buffer_offset && offset < (data->buffer_offset+data->buffer_count))
    {
        data->buffer_ptr = (tDWORD)(offset-data->buffer_offset);
    }
    else
    {
        InvalidateBuffer(data);
        data->buffer_offset = offset;
    }
    ret_val = offset;


    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SeekSet\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekForward )
// Parameters are:
tERROR pr_call SeqIO_SeekForward( hi_SeqIO _this, tQWORD* result, tQWORD offset )
{
    tERROR error;
    tQWORD ret_val = 0;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SeekForward\" method" );

    // Place your code here
    error = SeqIO_Seek(_this,&ret_val,offset,cSEEK_FORWARD);

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SeekForward\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekBack )
// Parameters are:
tERROR pr_call SeqIO_SeekBack( hi_SeqIO _this, tQWORD* result, tQWORD offset )
{
    tERROR error;
    tQWORD ret_val = 0;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SeekBack\" method" );

    // Place your code here
    error = SeqIO_Seek(_this,&ret_val,offset,cSEEK_BACK);

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SeekBack\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekEnd )
// Parameters are:
tERROR pr_call SeqIO_SeekEnd( hi_SeqIO _this, tQWORD* result, tQWORD offset )
{
    tERROR error;
    tQWORD ret_val = 0;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SeekEnd\" method" );

    // Place your code here
    error = SeqIO_Seek(_this,&ret_val,offset,cSEEK_END);

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SeekEnd\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetSizeByPos )
// Extended method comment
//  Set file size according current position. Operation not possible if region or stream reversing active. In this case file size will be unchanged.
// Behaviour comment
//  If current position lies beyong real file size file will be expanded
// Result comment
//  new file size
// Parameters are:
tERROR pr_call SeqIO_SetSizeByPos( hi_SeqIO _this, tQWORD* result )
{
    tERROR error;
    tQWORD ret_val = 0;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::SetSizeByPos\" method" );

    // Place your code here
    data = _this->data;
    InvalidateBuffer(data);

    data->file_size_valid = cFALSE;

    error = data->io->vtbl->SetSize(data->io, data->buffer_offset);
    if ( error == errOK )
    {
        ret_val = data->buffer_offset;
        data->file_size = data->buffer_offset;
        data->file_size_valid = cTRUE;
    }

    if ( result )
        *result = ret_val;
    PR_TRACE_A2( _this, "Leave \"SeqIO::SetSizeByPos\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IsNotEOF )
// Parameters are:
tERROR pr_call SeqIO_IsNotEOF( hi_SeqIO _this )
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::IsNotEOF\" method" );

    // Place your code here
    data = _this->data;

    if ( data->buffer_ptr == data->buffer_count )
    {
        tQWORD size;

        if ( data->file_eof )
        {
            error = errEOF;
        }
        else
        {
            if ( data->file_size_valid )
            {
                if ( data->file_size <= (data->buffer_offset+data->buffer_count) )
                {
                    error = errEOF;
                }
            }
            else
            {
                error = data->io->vtbl->GetSize(data->io,&size,IO_SIZE_TYPE_EXPLICIT);
                if ( error == errOK )
                {
                    data->file_size = size;
                    data->file_size_valid = cTRUE;

                    if ( size <= (data->buffer_offset+data->buffer_count) )
                    {
                        error = errEOF;
                    }
                }
            }
        }
    }

    PR_TRACE_A1( _this, "Leave \"SeqIO::IsNotEOF\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegionSet )
// Parameters are:
tERROR pr_call SeqIO_RegionSet( hi_SeqIO _this, tQWORD start, tQWORD end)
{
    tERROR error = errOK;
    register tSEQIO_DATA *data;
    PR_TRACE_A0( _this, "Enter \"SeqIO::RegionSet\" method" );

    // Place your code here
    data = _this->data;

    if ( start <= end )
    {
        InvalidateBuffer(data);
        data->region_start = start;
        data->region_end = end;

        data->region_active =  ( start == 0 && (end+1) == 0 );
        data->region_size = end - start;

        data->file_size_valid = cFALSE;
    }
    else
    {
        error = errPARAMETER_INVALID;
    }

    PR_TRACE_A1( _this, "Leave \"SeqIO::RegionSet\" method, ret tERROR = 0x%08x", error );
    return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	switch( dwReason ) {
		
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH : 
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		g_root = (hROOT)hInstance;
		{
            tERROR error = SeqIO_Register( (hROOT)hInstance );
			if(pERROR)*pERROR=error;
			if(PR_FAIL(error)) return cFALSE;
		}
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		g_root = NULL;
		break;
	}
	return cTRUE;
}

// AVP Prague stamp end



