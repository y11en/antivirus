
// -------- Wednesday,  26 July 2000,  13:47 --------
// Project -- Prague
// Sub project -- Extractor mini plugin
// Purpose -- Inflate
// ...........................................
// Author -- Andy Nikishin
// File Name -- unshrink.h
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------



#if !defined( __i_unshrink_h__ )
#define __i_unshrink_h__

#include <Prague/iface/i_io.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_heap.h>


typedef struct tag_UnshrinkData 
{
	hHEAP   hHeap;
	hSEQ_IO hInputIO; // -- 
	hSEQ_IO hOutputIO; // -- 
	tBOOL zip_corrupt;
	union work* area;
	tINT codesize;
	tINT maxcode;
	tINT maxcodemax;
	tINT free_ent;
	tQWORD qwSizeOfFile;
	tDWORD dwYeldCounter;
	hOBJECT _Me;	
	int bits_left;
	tDWORD zip_bitbuf;
	tBOOL zipeof;
	tERROR error;
} UnshrinkData;

#define INIT_BITS       9
#define FIRST_ENT       (tWORD)257
#define CLEAR           (tWORD)256
#define MAX_BITS      13                 /* used in unShrink() */
#define HSIZE         (1 << MAX_BITS)    /* size of global work area */
#define WSIZE 0x8000       /* window size--must be a power of two, and */

union work
{
	struct
	{
		tWORD Prefix_of[HSIZE];      /* (8194 * sizeof(short)) */
		tBYTE Suffix_of[HSIZE];       /* also s-f length_nodes (smaller) */
		tBYTE Stack[HSIZE];           /* also s-f distance_nodes (smaller) */
	} shrink;
	tBYTE Slide[WSIZE];
};


tQWORD unShrink(UnshrinkData* data);

#endif // unshrink_h
