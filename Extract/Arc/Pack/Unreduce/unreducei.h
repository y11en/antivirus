// -------- Friday,  28 July 2000,  13:35 --------
// Project -- Prague
// Sub project -- Extractor mini plugin
// Purpose -- Unreduce
// ...........................................
// Author -- Andy Nikishin
// File Name -- unreduce.h
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------



#if !defined( __unreduce_h__ )
#define __unreduce_h__



#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Extractor, Data structure )
// Interface "Extractor". Inner data structure
typedef struct tag_UnreduceData 
{
	hSEQ_IO hInputIO; // -- 
	hSEQ_IO hOutputIO; // -- 
	tQWORD Zip_ucsize;
	tBYTE* slide;
	tDWORD compression_method;
	tERROR error;
	//tQWORD dwSizeOfFile;
	tDWORD dwYeldCounter;
	hOBJECT _Me;
	int bits_left;
	tDWORD zip_bitbuf;
///	tBOOL zipeof;
} UnreduceData;
// AVP Prague stamp end( Extractor, Data structure )
// --------------------------------------------------------------------------------


#define SIZEOFSLIDE 0x4000


tERROR unReduce(UnreduceData* data);



#endif // i_unreduce_h



