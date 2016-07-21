// SA.CPP ////////////////////////////////////////

// Security Attributes (SA)
// [Version 1.07.000]
// Copyright (C) 1999-2000 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#include <windows.h>
#include "sa.h"

//////////////////////////////////////////////////

// Handle Of Security Attributes
typedef struct _HSEC_ATTR {

	PSID				pSID;
	PACL				pACL;
	SECURITY_ATTRIBUTES	SA;
	SECURITY_DESCRIPTOR	SD;

} HSEC_ATTR, * PHSEC_ATTR;

//////////////////////////////////////////////////

	PSID
SA_CreateSID(
	DWORD	dwAuthority
) {

	SID_IDENTIFIER_AUTHORITY authWorld = SECURITY_WORLD_SID_AUTHORITY;
	PSID pSID;

	if(!AllocateAndInitializeSid(&authWorld, 1, dwAuthority, 0, 0, 0, 0, 0, 0, 0, &pSID))
		return NULL;

	DWORD dwLengthSID = GetLengthSid(pSID);

	PSID pCopiedSID = (PSID)new BYTE[dwLengthSID];
	if(pCopiedSID == NULL) {

		FreeSid(pSID);
		return NULL;
	}

	if(!CopySid(dwLengthSID, pCopiedSID, pSID)) {	

		delete[] (BYTE *)pCopiedSID;
		FreeSid(pSID);
		return NULL;
	}

	FreeSid(pSID);

	return pCopiedSID;
}

//////////////////////////////////////////////////

	BOOL
SA_DestroySID(
	PSID	pSID
) {

	if(pSID == NULL)
		return FALSE;

	delete[] (BYTE *)pSID;

	return TRUE;
}

//////////////////////////////////////////////////

	HSA
SA_Create(
	DWORD	dwAuthority,
	DWORD	dwAccessMask
) {

	PHSEC_ATTR pHSA = new HSEC_ATTR;
	if(pHSA == NULL)
		return NULL;

	pHSA->pSID = SA_CreateSID(dwAuthority);
	if(pHSA->pSID == NULL) {

		delete pHSA;
		return NULL;
	}

	DWORD dwLengthACL =	GetLengthSid(pHSA->pSID) + sizeof(ACL)
						+ sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD);

	pHSA->pACL = (PACL)new BYTE[dwLengthACL];
	if(pHSA->pACL == NULL) {

		SA_DestroySID(pHSA->pSID);
		delete pHSA;
		return NULL;
	}

	if(	!InitializeAcl(pHSA->pACL, dwLengthACL, ACL_REVISION) ||
		!AddAccessAllowedAce(pHSA->pACL, ACL_REVISION, dwAccessMask, pHSA->pSID) ||
		!InitializeSecurityDescriptor(&(pHSA->SD), SECURITY_DESCRIPTOR_REVISION) ||
		!SetSecurityDescriptorDacl(&(pHSA->SD), TRUE, pHSA->pACL, FALSE)) {

		delete[] (BYTE *)pHSA->pACL;
		SA_DestroySID(pHSA->pSID);
		delete pHSA;
		return NULL;
	}

	pHSA->SA.nLength			  = sizeof(SECURITY_ATTRIBUTES);
	pHSA->SA.lpSecurityDescriptor = &(pHSA->SD);
	pHSA->SA.bInheritHandle		  = FALSE;

	return (HSA)pHSA;
}

//////////////////////////////////////////////////

	BOOL
SA_Destroy(
	HSA		hSA
) {

	PHSEC_ATTR pHSA = (PHSEC_ATTR)hSA;

	if(	pHSA == NULL ||
		!SA_DestroySID(pHSA->pSID))
		return FALSE;

	delete[] (BYTE *)pHSA->pACL;
	delete pHSA;

	return TRUE;
}

//////////////////////////////////////////////////

	PSECURITY_ATTRIBUTES
SA_Get(
	HSA		hSA
) {

	PHSEC_ATTR pHSA = (PHSEC_ATTR)hSA;

	if(pHSA == NULL)
		return NULL;

	return &(pHSA->SA);
}

// EOF ///////////////////////////////////////////