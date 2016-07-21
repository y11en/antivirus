// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  15 February 2005,  16:34 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- base64p.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define DataCodec_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <Extract/plugin/p_base64p.h>
// AVP Prague stamp end



#include <memory.h>

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable Base64P : public cDataCodec /*cObjImpl*/ {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call Alphabet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call Encode( tDWORD* p_result, const tBYTE* p_pDecodedData, tDWORD p_nDataSize, tBYTE* p_pEncodedBuffer, tDWORD p_nBufferSize );
	tERROR pr_call Decode( tDWORD* p_result, const tBYTE* p_pEncodedData, tDWORD p_nDataSize, tBYTE* p_pDecodedBuffer, tDWORD p_nBufferSize );

// Data declaration
	tBYTE m_aDecodeArray[256]; // --
	tBYTE m_aEncodeArray[64]; // --
// AVP Prague stamp end



private:
	void ReinitDecodeArray();


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(Base64P)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "DataCodec". Static(shared) property table variables
// AVP Prague stamp end



void Base64P::ReinitDecodeArray()
{
	size_t nIndex;
	memset(m_aDecodeArray, 0xFF, sizeof(m_aDecodeArray)) ;
    for (nIndex = 0; nIndex < 64; ++nIndex)
        m_aDecodeArray[m_aEncodeArray[nIndex]] = (tBYTE)nIndex ;

//  aDecodeArray['='] = 64 ;
//  aDecodeArray[255] = '\xFF' ;
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR Base64P::ObjectInit() {
	tERROR error = errOK;
	size_t nIndex;
	PR_TRACE_FUNC_FRAME( "DataCodec::ObjectInit method" );

	// fill default alphabet
    for (nIndex = 0; nIndex < 64; ++nIndex)
    {
        int val ;
        if (nIndex < 26)
            val = (int)(nIndex + 'A') ;
        else if (nIndex < 52)
            val = (int)((nIndex - 26) + 'a') ;
        else if (nIndex < 62)
            val = (int)((nIndex - 52) + '0') ;
        else if (nIndex == 62)
            val = '+' ;
        else
            val = '/' ;

        m_aEncodeArray[nIndex] = (tBYTE)val ;
    }
	ReinitDecodeArray();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR Base64P::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "DataCodec::ObjectInitDone method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR Base64P::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "DataCodec::ObjectPreClose method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Alphabet )
// Interface "DataCodec". Method "Set" for property(s):
//  -- ALPHABET
tERROR Base64P::Alphabet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method Alphabet for property plALPHABET" );

	*out_size = 0;
	if (!buffer || size != sizeof(m_aEncodeArray))
		return errPARAMETER_INVALID;
	memcpy(m_aEncodeArray, buffer, size);
	*out_size = size;
	ReinitDecodeArray();

	PR_TRACE_A2( this, "Leave *SET* method Alphabet for property plALPHABET, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Encode )
// Parameters are:
tERROR Base64P::Encode( tDWORD* p_result, const tBYTE* p_pDecodedData, tDWORD p_nDataSize, tBYTE* p_pEncodedBuffer, tDWORD p_nBufferSize ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "DataCodec::Encode method" );
	
	tDWORD written = 0;
    tDWORD val = 0;
    tDWORD b;

	if (!p_pEncodedBuffer && !p_nBufferSize)
	{
		// this is buffer size request
		if (!p_result)
			return errPARAMETER_INVALID;
		*p_result = ((p_nDataSize + 2) / 3) * 4;
		return errOK;
	}

    if (p_result)
        *p_result = 0;

	if (p_nDataSize < sizeof(tDWORD))
		return errOBJECT_DATA_SIZE_UNDERSTATED;

	val = *(tDWORD*)p_pDecodedData;

    do
    {
        b = (unsigned char)(val & 0x1F);
        val >>= 5;
        if (val)
            b |= 0x20;
		if (written < p_nBufferSize)
			p_pEncodedBuffer[written] = m_aEncodeArray[b];
		else
			error = errBUFFER_TOO_SMALL;
        written++;
    } while (val);

    if (p_result)
        *p_result = written;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Decode )
// Parameters are:
tERROR Base64P::Decode( tDWORD* p_result, const tBYTE* p_pEncodedData, tDWORD p_nDataSize, tBYTE* p_pDecodedBuffer, tDWORD p_nBufferSize )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "DataCodec::Decode method" );
	
	tDWORD read = 0;
    tDWORD val = 0;
    tDWORD shift = 0;

	if (!p_pDecodedBuffer && !p_nBufferSize)
	{
		// this is buffer size request
		if (!p_result)
			return errPARAMETER_INVALID;
		*p_result = ((p_nDataSize + 3) / 4) * 3;
		return errOK;
	}

    if (!p_pEncodedData || !p_pDecodedBuffer)
        return errPARAMETER_INVALID;

    if (p_result)
        *p_result = sizeof(tDWORD);

	if (p_nBufferSize != sizeof(tDWORD))
		return errBUFFER_TOO_SMALL;

    *(tDWORD*)p_pDecodedBuffer = 0;

    while (read < p_nDataSize)
    {
        unsigned char b = m_aDecodeArray[p_pEncodedData[read++]];
        if (0xFF == b)
		{
            error = errOBJECT_DATA_CORRUPTED; // invalid base64 sequence
			break;
		}
		if (shift > 32)
		{
			error = errOUT_OF_SPACE;
			break;
		}
        val |= (b & 0x1F) << shift;
        shift += 5;
        if (0 == (b & 0x20))
        {
            // decoded ok
            if (p_result)
                *p_result = read;
			*(tDWORD*)p_pDecodedBuffer = val;
            break;
        }
    }

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "DataCodec". Register function
tERROR Base64P::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(DataCodec_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, DataCodec_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "BASE64P", 8 )
	mpLOCAL_PROPERTY( plALPHABET, Base64P, m_aEncodeArray, cPROP_BUFFER_READ_WRITE, NULL, FN_CUST(Alphabet) )
mpPROPERTY_TABLE_END(DataCodec_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(DataCodec)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(DataCodec)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(DataCodec)
	mEXTERNAL_METHOD(DataCodec, Encode)
	mEXTERNAL_METHOD(DataCodec, Decode)
mEXTERNAL_TABLE_END(DataCodec)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "DataCodec::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_DATACODEC,                          // interface identifier
		PID_BASE64P,                            // plugin identifier
		0x00000000,                             // subtype identifier
		DataCodec_VERSION,                      // interface version
		VID_MIKE,                               // interface developer
		&i_DataCodec_vtbl,                      // internal(kernel called) function table
		(sizeof(i_DataCodec_vtbl)/sizeof(tPTR)),// internal function table size
		&e_DataCodec_vtbl,                      // external function table
		(sizeof(e_DataCodec_vtbl)/sizeof(tPTR)),// external function table size
		DataCodec_PropTable,                    // property table
		mPROPERTY_TABLE_SIZE(DataCodec_PropTable),// property table size
		sizeof(Base64P)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"DataCodec(IID_DATACODEC) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call DataCodec_Register( hROOT root ) { return Base64P::Register(root); }
// AVP Prague stamp end



