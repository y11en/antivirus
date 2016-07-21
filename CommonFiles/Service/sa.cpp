// SA.CPP ////////////////////////////////////////

// Security Attributes (SA)
// [Version 1.07.000]
// Copyright (C) 1999-2000 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////
#if defined (_WIN32)

#include <windows.h>
#include "sa.h"

//////////////////////////////////////////////////

// Handle Of Security Attributes
typedef struct _HSEC_ATTR {

	PSID				pSID;
	PSID				pMandSID;
	PACL				pACL;
	PACL				pSACL;
	SECURITY_ATTRIBUTES	SA;
	SECURITY_DESCRIPTOR	SD;

} HSEC_ATTR, * PHSEC_ATTR;

//////////////////////////////////////////////////
PSID SA_CreateAnySID(SID_IDENTIFIER_AUTHORITY* pSid, DWORD dwRid)
{
	PSID pSID;

	if(!AllocateAndInitializeSid(pSid, 1, dwRid, 0, 0, 0, 0, 0, 0, 0, &pSID))
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


PSID SA_CreateSID(DWORD	dwAuthority)
{
	SID_IDENTIFIER_AUTHORITY authWorld = SECURITY_WORLD_SID_AUTHORITY;
	return SA_CreateAnySID(&authWorld, dwAuthority);
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

#ifndef SECURITY_MANDATORY_LABEL_AUTHORITY
typedef struct _SYSTEM_MANDATORY_LABEL_ACE
{
	ACE_HEADER Header;
	ACCESS_MASK Mask;
	DWORD SidStart;
} SYSTEM_MANDATORY_LABEL_ACE,  *PSYSTEM_MANDATORY_LABEL_ACE;

#define SECURITY_MANDATORY_LABEL_AUTHORITY          {0,0,0,0,0,16}
#define SECURITY_MANDATORY_LOW_RID                  (0x00001000L)
#endif

typedef BOOL (WINAPI *pfnAddMandatoryAce)(PACL pAcl, DWORD dwAceRevision, DWORD AceFlags, DWORD MandatoryPolicy, PSID pLabelSid);

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

	pHSA->pMandSID = NULL;
	pHSA->pSACL = NULL;

	pfnAddMandatoryAce _AddMandatoryAce = NULL;
	HMODULE hAdvapi32 = GetModuleHandle("advapi32");
	if (hAdvapi32)
		_AddMandatoryAce = (pfnAddMandatoryAce) GetProcAddress(hAdvapi32, "AddMandatoryAce");
	if (_AddMandatoryAce)
	{
		bool bHasSacl = false;
		SID_IDENTIFIER_AUTHORITY authMand = SECURITY_MANDATORY_LABEL_AUTHORITY;
		pHSA->pMandSID = SA_CreateAnySID(&authMand, SECURITY_MANDATORY_LOW_RID);
		if (pHSA->pMandSID != NULL)
		{
			DWORD dwLengthSACL = GetLengthSid(pHSA->pMandSID) + sizeof(ACL)
				+ sizeof(SYSTEM_MANDATORY_LABEL_ACE) - sizeof(DWORD);

			pHSA->pSACL = (PACL)new BYTE[dwLengthSACL];
			if (pHSA->pSACL
				&& InitializeAcl(pHSA->pSACL, dwLengthSACL, ACL_REVISION)
				&& _AddMandatoryAce(pHSA->pSACL, ACL_REVISION, 0, 0, pHSA->pMandSID)
				&& SetSecurityDescriptorSacl(&(pHSA->SD), TRUE, pHSA->pSACL, FALSE))
			{
				bHasSacl = true;
			}
		}
		if (!bHasSacl)
		{
			if (pHSA->pMandSID)
			{
				SA_DestroySID(pHSA->pMandSID);
				pHSA->pMandSID = NULL;
			}
			if (pHSA->pSACL)
			{
				delete[] (BYTE *)pHSA->pSACL;
				pHSA->pSACL = NULL;
			}
		}
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

    if (pHSA == NULL)
        return FALSE;
	SA_DestroySID(pHSA->pMandSID);

	if (pHSA->pSACL)
		delete[] (BYTE *)pHSA->pSACL;

	if(	!SA_DestroySID(pHSA->pSID))
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

#endif //_WIN32
// EOF ///////////////////////////////////////////
