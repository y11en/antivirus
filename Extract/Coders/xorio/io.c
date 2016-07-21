// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  27 February 2004,  15:48 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Rubin
// File Name   -- io.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_xorio.h"

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <memory.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_XorIOData 
{
    tDWORD m_origin;  // --
    tBYTE  m_xorbyte; // --
    hIO    m_io;      // --
    tDWORD m_xorsize; // --
    tBYTE* m_xorkey;  // --
} XorIOData;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
    const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
    const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
    XorIOData*         data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call IO_ObjectInit( hi_IO _this ) 
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter IO::ObjectInit method" );

    _this->data->m_xorbyte = 0x55;
    _this->data->m_xorsize = 1;
    _this->data->m_xorkey  = 0;

    PR_TRACE_A1( _this, "Leave IO::ObjectInit method, ret %terr", error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetProperty )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
//  -- OBJECT_AVAILABILITY
//  -- OBJECT_FILL_CHAR
//  -- OBJECT_HASH
//  -- OBJECT_REOPEN_DATA
//  -- OBJECT_ATTRIBUTES
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR pr_call IO_PROP_GetProperty( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *GET* multyproperty method INTERFACE_COMMENT" );

    if (_this->data->m_io)
    {
        error = CALL_SYS_PropertyGet(_this->data->m_io, out_size, prop, buffer, size);
    }
    else
    {
        error = errOBJECT_NOT_INITIALIZED;
    }

    PR_TRACE_A2( _this, "Leave *GET* multyproperty method INTERFACE_COMMENT, ret tDWORD = %u(size), %terr", *out_size, error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetProperty )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
//  -- OBJECT_ATTRIBUTES
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR pr_call IO_PROP_SetProperty( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter *SET* multyproperty method INTERFACE_COMMENT" );

    if ( prop == ppXOR_KEY )
    {
        if (_this->data->m_xorkey)
        {
            CALL_SYS_ObjHeapFree((hOBJECT)_this, _this->data->m_xorkey);
            _this->data->m_xorkey = 0;
        }

        if (size == 1)
        {
            _this->data->m_xorbyte = *(tBYTE*)buffer;
            _this->data->m_xorsize = size;
        }
        else
        {
            error = CALL_SYS_ObjHeapAlloc ((hOBJECT)_this, &_this->data->m_xorkey, size);

            if (PR_SUCC(error))
            {
                memcpy (_this->data->m_xorkey, buffer, size);
            }
            _this->data->m_xorsize = size;
        }
    }
    else
    {
        if (_this->data->m_io)
        {
            error = CALL_SYS_PropertySet(_this->data->m_io, out_size, prop, buffer, size);
        }
        else
        {
            error = errOBJECT_NOT_INITIALIZED;
        }
    }

    PR_TRACE_A2( _this, "Leave *SET* multyproperty method INTERFACE_COMMENT, ret tDWORD = %u(size), %terr", *out_size, error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size ) 
{
    tERROR error = errOK;
    tBYTE xorbyte = _this->data->m_xorbyte;
    tUINT i;
    tDWORD res = 0;

    PR_TRACE_A0( _this, "Enter IO::SeekRead method" );

    if (_this->data->m_io)
    {
        error = CALL_IO_SeekRead(_this->data->m_io, &res, p_offset, p_buffer, p_size);

        if (PR_SUCC(error))
        {
            if ( _this->data->m_xorsize == 0 )
            {
            }
            else if ( _this->data->m_xorsize == 1 )
            {
                for (i = 0; i < res; ++i)
                {
                    ((tBYTE*)p_buffer)[i] ^= _this->data->m_xorbyte;
                }
            }
            else
            {
                for (i = 0; i < res; ++i)
                {
                    ((tBYTE*)p_buffer)[i] ^= _this->data->m_xorkey [ (p_offset + i) % _this->data->m_xorsize ];
                }
            }
        }
        if (result)
        {
            *result = res;
        }
    }
    else
    {
        error = errOBJECT_NOT_INITIALIZED;
    }

    PR_TRACE_A2( _this, "Leave IO::SeekRead method, ret tDWORD = %u, %terr", ret_val, error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
    tERROR error = errOK;
    tBYTE xorbyte = _this->data->m_xorbyte;
    tUINT i;

    PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );

    if (_this->data->m_io)
    {
        if (p_buffer)
        {
            if ( _this->data->m_xorsize == 0 )
            {
            }
            else if ( _this->data->m_xorsize == 1 )
            {
                for (i = 0; i < p_size; ++i)
                {
                    ((tBYTE*)p_buffer)[i] ^= xorbyte;
                }
            }
            else
            {
                for (i = 0; i < p_size; ++i)
                {
                    ((tBYTE*)p_buffer)[i] ^= _this->data->m_xorkey [ (p_offset + i) % _this->data->m_xorsize ];
                }
            }
        }

        error = CALL_IO_SeekWrite(_this->data->m_io, result, p_offset, p_buffer, p_size);

        if (p_buffer)
        {
            if ( _this->data->m_xorsize == 0 )
            {
            }
            else if ( _this->data->m_xorsize == 1 )
            {
                for (i = 0; i < p_size; ++i)
                {
                    ((tBYTE*)p_buffer)[i] ^= xorbyte;
                }
            }
            else
            {
                for (i = 0; i < p_size; ++i)
                {
                    ((tBYTE*)p_buffer)[i] ^= _this->data->m_xorkey [ (p_offset + i) % _this->data->m_xorsize ];
                }
            }
        }
    }
    else
    {
        error = errOBJECT_NOT_INITIALIZED;
    }

    PR_TRACE_A2( _this, "Leave IO::SeekWrite method, ret tDWORD = %u, %terr", ret_val, error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE p_type ) 
{
    tERROR error = errOK;
    tQWORD ret_val = 0;
    PR_TRACE_A0( _this, "Enter IO::GetSize method" );

    if (_this->data->m_io)
    {
        error = CALL_IO_GetSize(_this->data->m_io, result, p_type);
    }
    else
    {
        error = errOBJECT_NOT_INITIALIZED;
    }

    PR_TRACE_A2( _this, "Leave IO::GetSize method, ret tQWORD = %I64u, %terr", ret_val, error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD p_new_size ) 
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter IO::SetSize method" );

    if (_this->data->m_io)
    {
        error = CALL_IO_SetSize(_this->data->m_io, p_new_size );
    }
    else
    {
        error = errOBJECT_NOT_INITIALIZED;
    }

    PR_TRACE_A1( _this, "Leave IO::SetSize method, ret %terr", error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call IO_Flush( hi_IO _this ) 
{
    tERROR error = errOK;
    PR_TRACE_A0( _this, "Enter IO::Flush method" );

    if (_this->data->m_io)
    {
        error = CALL_IO_Flush(_this->data->m_io);
    }
    else
    {
        error = errOBJECT_NOT_INITIALIZED;
    }

    PR_TRACE_A1( _this, "Leave IO::Flush method, ret %terr", error );
    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
    mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
    mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Xoring IO wrapper", 18 )
    mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, XorIOData, m_origin, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_OPEN_MODE, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, XorIOData, m_io, cPROP_BUFFER_READ )
    mLOCAL_PROPERTY_FN( pgOBJECT_FILL_CHAR, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_HASH, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_REOPEN_DATA, IO_PROP_GetProperty, NULL )
    mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, IO_PROP_GetProperty, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_BUF( ppXOR_BYTE, XorIOData, m_xorbyte, cPROP_BUFFER_READ_WRITE )
    mLOCAL_PROPERTY_BUF( ppXOR_ORIGIN, XorIOData, m_io, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
    mLOCAL_PROPERTY_FN( ppXOR_KEY, NULL, IO_PROP_SetProperty )
    mLOCAL_PROPERTY_BUF( ppXOR_KEYSIZE, XorIOData, m_xorsize, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = 
{
    (tIntFnRecognize)         NULL,
    (tIntFnObjectNew)         NULL,
    (tIntFnObjectInit)        IO_ObjectInit,
    (tIntFnObjectInitDone)    NULL,
    (tIntFnObjectCheck)       NULL,
    (tIntFnObjectPreClose)    NULL,
    (tIntFnObjectClose)       NULL,
    (tIntFnChildNew)          NULL,
    (tIntFnChildInitDone)     NULL,
    (tIntFnChildClose)        NULL,
    (tIntFnMsgReceive)        NULL,
    (tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iIOVtbl e_IO_vtbl = 
{
    (fnpIO_SeekRead)          IO_SeekRead,
    (fnpIO_SeekWrite)         IO_SeekWrite,
    (fnpIO_GetSize)           IO_GetSize,
    (fnpIO_SetSize)           IO_SetSize,
    (fnpIO_Flush)             IO_Flush
};
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) 
{
    tERROR error;

    PR_TRACE_A0( root, "Enter IO::Register method" );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_IO,                                 // interface identifier
        PID_XORIO,                              // plugin identifier
        0x00000000,                             // subtype identifier
        IO_VERSION,                             // interface version
        VID_RUBIN,                              // interface developer
        &i_IO_vtbl,                             // internal(kernel called) function table
        (sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
        &e_IO_vtbl,                             // external function table
        (sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
        IO_PropTable,                           // property table
        mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
        sizeof(XorIOData),                      // memory size
        0                                       // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [%terr]",error) );
    #endif // _PRAGUE_TRACE_

    PR_TRACE_A1( root, "Leave IO::Register method, ret %terr", error );
    return error;
}
// AVP Prague stamp end



