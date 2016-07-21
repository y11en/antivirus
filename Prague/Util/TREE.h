// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  07 March 2002,  12:03 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andrew V. Krukov
// File Name   -- tree.h
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __tree_h__a87ef19a_d3df_434d_8413_fbf3e2c7a41e )
#define __tree_h__a87ef19a_d3df_434d_8413_fbf3e2c7a41e
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_compar.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_tree.h>

#include "ltree.h"
#include "lru.h"
#include "list.h"
#include "stack.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_UTIL  ((tPID)(0x0000fa07))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDREW  ((tVID)(64))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Tree interface implementation
// Short comments -- tree sorting and searching interface
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
#if defined( __cplusplus )
extern "C" 
{
#endif

	tERROR pr_call Tree_Register( hROOT root );

#if defined( __cplusplus )
}
#endif


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Tree. Inner data structure

#if defined( __cplusplus )
extern "C" 
{
#endif

typedef struct tag_TreeData 
{
	tUINT          granularity;         // --
	tBOOL          dup_allowed;         // --
	tBOOL          remove_allowed;      // --
	hCOMPARE       compare_object;      // --
	tUINT          node_size;           // --
	struct sNODE * root;                // --
	hHEAP          heap;                // --
	hCOMPARE       compare_object_real; // --
} TreeData;

#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

#if defined( __cplusplus )
extern "C" 
{
#endif

typedef struct tag_hi_Tree 
{
	const iTreeVtbl*   vtbl; // pointer to the "Tree" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	TreeData*          data; // pointer to the "Tree" data structure
} *hi_Tree;

#if defined( __cplusplus )
}
#endif

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // tree_h
// AVP Prague stamp end
