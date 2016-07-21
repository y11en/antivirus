
// -------- Wednesday,  26 July 2000,  13:47 --------
// Project -- Prague
// Sub project -- Extractor mini plugin
// Purpose -- Inflate
// ...........................................
// Author -- Andy Nikishin
// File Name -- explode.h
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------



#if !defined( __i_explode_h__ )
#define __i_explode_h__

#include <Prague/iface/i_io.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_heap.h>


// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure
typedef struct tag_ExplodeData 
{
	hSEQ_IO hInputIO; // -- 
	hSEQ_IO hOutputIO; // -- 
	tBOOL zip_corrupt;
	tBYTE* slide;
	tQWORD Zip_ucsize;
	tDWORD Zip_csize;
	tDWORD general_purpose_bit_flag;// Need to set by caller
	tERROR error;
	hOBJECT me;
	hHEAP hHeap;
} ExplodeData;
// AVP Prague stamp end

#define INIT_BITS       9
#define FIRST_ENT       (tWORD)257
#define CLEAR           (tWORD)256
#define MAX_BITS      13                 /* used in unShrink() */
#define HSIZE         (1 << MAX_BITS)    /* size of global work area */
#define WSIZE 0x8000       /* window size--must be a power of two, and */


tDWORD  explode(ExplodeData* data);

#endif // explode_h
