// SA.H //////////////////////////////////////////

// Security Attributes (SA)
// [Version 1.07.XXX]
// Copyright (C) 1999-2000 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#ifndef __SA_H
#define __SA_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////

#define SA_AUTHORITY_EVERYONE	SECURITY_WORLD_RID

#define SA_ACCESS_MASK_ALL		GENERIC_ALL

//////////////////////////////////////////////////

typedef void * HSA;

//////////////////////////////////////////////////

	HSA						// NULL, ...
SA_Create(
	DWORD	dwAuthority,	// SA_AUTHORITY_...
	DWORD	dwAccessMask	// SA_ACCESS_...
);

	BOOL
SA_Destroy(
	HSA		hSA
);

	PSECURITY_ATTRIBUTES	// NULL, ...
SA_Get(
	HSA		hSA
);

//////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // __SA_H

// EOF ///////////////////////////////////////////