/*-----------------02.05.99 12:57-------------------
 * Project Prague                                 *
 * Subproject Kernel                              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 * revision 2                                     *
--------------------------------------------------*/
/* generic handle definition */

#ifndef __PR_OBJ_H
#define __PR_OBJ_H


typedef struct tag_iSYSTEMVtbl iSYSTEMVtbl;


#if defined( __RPC__ )
	
	typedef tUINT hOBJECT;

#else

#if defined(__cplusplus) && !defined(_USE_VTBL)
	typedef struct cObject* hOBJECT;
#else

	typedef struct tag_hOBJECT {
		const tPTR         vtbl;   // interface defined function table - NULL for generic object
		const iSYSTEMVtbl* sys;    // system defined function table
	} *hOBJECT;
#endif //if defined(__cplusplus) && !defined(_USE_VTBL)

#endif //if defined( __RPC__ )

typedef tQWORD rpcOBJECT;

#endif // __PR_OBJ_H
