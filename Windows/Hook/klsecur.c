// Security Attributes
// must be on IRQL PASSIVE_LEVEL.
#include "klsecur.h"
#include "debug.h"

#include "InvThread.h"

#ifdef _HOOK_NT_
#include "nt/glbenvir.h"

#ifndef _TOKEN_USER
typedef struct _TOKEN_USER
 {
    SID_AND_ATTRIBUTES User;
} TOKEN_USER, *PTOKEN_USER;
#endif

NTSYSAPI NTSTATUS NTAPI ZwOpenProcessToken(IN HANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, OUT PHANDLE TokenHandle);
NTSYSAPI NTSTATUS NTAPI ZwOpenThreadToken(HANDLE ThreadHandle,DWORD DesiredAccess,BOOLEAN OpenAsSelf,PHANDLE TokenHandle);
NTSYSAPI NTSTATUS NTAPI ZwQueryInformationToken(IN HANDLE TokenHandle, 
												IN TOKEN_INFORMATION_CLASS TokenInformationClass, 
												OUT PVOID TokenInformation, IN ULONG TokenInformationLength, 
												OUT PULONG ReturnLength);

NTSTATUS (__stdcall *ImpersonateClient)(IN PSECURITY_CLIENT_CONTEXT  ClientContext,IN PETHREAD ServerThread  OPTIONAL); 

//+ ------------------------------------------------------------------------------------------
#define SE_SACL_AUTO_INHERITED           (0x0800)
#define SE_SACL_PROTECTED                (0x2000)

NTSTATUS ObGetObjectSecurity(IN PVOID Object, OUT PSECURITY_DESCRIPTOR *SecurityDescriptor, OUT PBOOLEAN MemoryAllocated);
VOID ObReleaseObjectSecurity(IN PSECURITY_DESCRIPTOR SecurityDescriptor, IN BOOLEAN MemoryAllocated);

//+ ------------------------------------------------------------------------------------------

BOOLEAN ImpersonateInited = FALSE;

#ifndef NtCurrentThread
#define NtCurrentThread() ((HANDLE) -2)
#endif

BOOLEAN SeGetLuid(PLUID pLuid)
{
	BOOLEAN bRet = FALSE;
	PACCESS_TOKEN  pToken = 0;

	SECURITY_SUBJECT_CONTEXT SubjectContext;
	
	SeCaptureSubjectContext(&SubjectContext);

	if (SubjectContext.ClientToken)
		pToken = SubjectContext.ClientToken;
	else if (SubjectContext.PrimaryToken)
		pToken = SubjectContext.PrimaryToken;

	if (pToken)
	{
		if (NT_SUCCESS(SeQueryAuthenticationIdToken(pToken, pLuid)))
		{
			bRet = TRUE;
		}
	}

	SeReleaseSubjectContext(&SubjectContext);

	if (!bRet)
		memset(pLuid, 0, sizeof(LUID));

	return bRet;
}

BOOLEAN SeGetSID(PSID pSid, ULONG* pSidLength)
{
	BOOLEAN bRet = FALSE;

	NTSTATUS ntStatus;
	HANDLE tokenHandle  = 0;
	
	ULONG requiredLength;
	PVOID tokenInfoBuffer;
	
	((BYTE*)pSid)[0] = 0;
	if(PsIsThreadTerminating(PsGetCurrentThread()))
	{
		DbPrint(DC_NOCAT, DL_INFO, ("ZwOpenThreadToken for terminating thread! Will crash?\n"));
		return FALSE;
	}

	ntStatus = ZwOpenThreadToken(NtCurrentThread(), /*TOKEN_QUERY*/TOKEN_READ, TRUE, &tokenHandle);

	if(ntStatus == STATUS_NO_TOKEN)
	{
		DbPrint(DC_NOCAT,DL_SPAM, ("ZwOpenThreadToken failed. Status = STATUS_NO_TOKEN\n"));
		// This is typically done by referencing the token associated with a thread when the thread is not impersonating a client.
		// Насколько я ничего не понял, стоит попробовать взяться за токен просесса
		ntStatus = ZwOpenProcessToken(NtCurrentProcess(), TOKEN_QUERY/*TOKEN_READ*/, &tokenHandle);
	}
	if((ntStatus==STATUS_SUCCESS) && (tokenHandle != 0))
	{
		ntStatus = ZwQueryInformationToken( tokenHandle, TokenUser, NULL, 0, &requiredLength);
		if( ntStatus == STATUS_BUFFER_TOO_SMALL)
		{
			tokenInfoBuffer = (PTOKEN_USER) ExAllocatePoolWithTag(NonPagedPool, requiredLength, 'SboS');
			if (tokenInfoBuffer != NULL)
			{
				ntStatus = ZwQueryInformationToken( tokenHandle, TokenUser, tokenInfoBuffer, 
					requiredLength, &requiredLength);
				if( !NT_SUCCESS(ntStatus))
				{
					DbPrint(DC_NOCAT,DL_ERROR, ("buffer with strange size for TOKEN_USER\n"));
				}
				else
				{
					PTOKEN_USER pTokenUser = (PTOKEN_USER) tokenInfoBuffer;
					DbPrint(DC_NOCAT,DL_SPAM, ("sidlen = %d\n", RtlLengthSid(pTokenUser->User.Sid)));
					ntStatus = RtlCopySid(*pSidLength, pSid, pTokenUser->User.Sid);
					if (ntStatus == STATUS_SUCCESS)
					{
						*pSidLength = RtlLengthSid(pTokenUser->User.Sid);
						bRet = TRUE;
					}
					else
					{
						DbPrint(DC_NOCAT,DL_ERROR, ("copy sid failed - status 0x%x\n", ntStatus));
					}

				}
				ExFreePool(tokenInfoBuffer);
			}
		}
		else
		{
			DbPrint(DC_NOCAT,DL_ERROR, ("NtQueryInformationToken failed. Status 0x%x\n", ntStatus));
		}

		ZwClose(tokenHandle);
	}
	else
	{
		DbPrint(DC_NOCAT,DL_ERROR, ("ZwOpen(Thread/Process)Token failed. Status 0x%x\n", ntStatus));
	}
	
	return bRet;
}

#ifdef _HOOK_IMPERSONATE_ 

void Security_ReleaseContext(SECURITY_CLIENT_CONTEXT *pClientContext)
{
	if(SeTokenType(pClientContext->ClientToken)==TokenPrimary)
		ObDereferenceObject(pClientContext->ClientToken);
	else if(pClientContext->ClientToken!=NULL)
		ObDereferenceObject(pClientContext->ClientToken);
}

NTSTATUS Security_CaptureContext(PETHREAD OrigTh, SECURITY_CLIENT_CONTEXT *pClientContext)
{
	SECURITY_QUALITY_OF_SERVICE SecurityQos;

	SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
	SecurityQos.ImpersonationLevel = SecurityImpersonation;
	SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
	SecurityQos.EffectiveOnly = FALSE;

	return SeCreateClientSecurity(OrigTh, &SecurityQos, FALSE, pClientContext);
}

/*NTSTATUS MyImpersonateThread(PETHREAD CheckTh,PETHREAD OrigTh)
{

	NTSTATUS NtStatus;
	//If the ContextTrackingMode member of ClientSecurityQos is set to SECURITY_DYNAMIC_TRACKING 
	//and ServerIsRemote is set to FALSE, SeCreateClientSecurity uses a reference to the client's effective token. 
	//Otherwise, SeCreateClientSecurity creates a copy of the client's token

	//SeDeleteClientSecurity deletes a client security context structure and performs any necessary cleanup, 
	//such as removing any client token references.
	// По идее нам это вызывать не не надо, потому как:
	//	1.Структура у нас на стеке
	//	2.Dereference делаем ручками
	//  3.ServerIsRemote - FALSE
	//	4.ClientSecurityQos is set to SECURITY_DYNAMIC_TRACKING 
	//	Вуаля!
		
		SeImpersonateClient(&ClientContext,CheckTh);
	}
	return NtStatus;	
}*/


NTSTATUS MyImpersonateClient(IN PETHREAD ServerThread, IN PSECURITY_CLIENT_CONTEXT  ClientContext)
{
	if(ImpersonateInited)
		return ImpersonateClient(ClientContext, ServerThread);
	else
		return STATUS_UNSUCCESSFUL;
}

BOOLEAN InitImpersonate(VOID)
{
	if(((PVOID) ImpersonateClient = GetExport(BaseOfNtOsKrnl,"SeImpersonateClientEx",NULL)))
		ImpersonateInited = TRUE;
	else
	{
		if(((PVOID)ImpersonateClient = GetExport(BaseOfNtOsKrnl,"SeImpersonateClient",NULL)))
		{
			ImpersonateInited = TRUE;
		}
	}

	if (!ImpersonateInited)
	{
		DbPrint(DC_NOCAT,DL_ERROR, ("InitImpersonate fail\n"));
	}
	
	return ImpersonateInited;
}

#endif //_HOOK_IMPERSONATE_

/*
//+ ------------------------------------------------------------------------------------------
// SECURITY_NT_NON_UNIQUE
// SECURITY_NULL_SID_AUTHORITY - 0
SID_IDENTIFIER_AUTHORITY gOurSidAuthority =  SECURITY_NULL_SID_AUTHORITY;

PSYSTEM_AUDIT_ACE SA_FoundOurAce(PACL pacl)
{
	NTSTATUS ntStatus;

	if (!pacl)
	{
		DbPrint(DC_SYS, DL_ERROR, ("FoundOurAce: invalidargument\n"));
		return NULL;
	}
	
	DbPrint(DC_SYS, DL_NOTIFY, ("FoundOurAce: Revision: %d, AclSize: %d (ACE count %d)\n", pacl->AclRevision, pacl->AclSize, pacl->AceCount));

	if (pacl->AceCount)
	{
		PISID pSid;
		ULONG cou;
		PSYSTEM_AUDIT_ACE pAce;
		for (cou = 0; cou < pacl->AceCount; cou++)
		{
			 ntStatus = _pfRtlGetAce(pacl, cou, &pAce);
			 if (NT_SUCCESS(ntStatus))
			 {
				 pSid = (PSID) &pAce->SidStart;

				 if (!memcmp(&pSid->IdentifierAuthority, &gOurSidAuthority, sizeof(gOurSidAuthority)))
				 {
					 if (pSid->SubAuthorityCount == SID_MAX_SUB_AUTHORITIES)
					 {
						 if (
							((BYTE*)pSid->SubAuthority)[0] == 'K' &&
							((BYTE*)pSid->SubAuthority)[1] == 'A' &&
							((BYTE*)pSid->SubAuthority)[2] == 'V')
						 {
							 return pAce;
						 }
					 }
				 }
			 }
		}
	}

	return NULL;
}

void
SA_PrintAclInfo(PACL pacl)
{
	NTSTATUS ntStatus;
	
	DbPrint(DC_SYS, DL_IMPORTANT, ("\tRevision: %d, AclSize: %d (ACE count %d) sizeof(SYSTEM_AUDIT_ACE)=%d \n", pacl->AclRevision, pacl->AclSize, pacl->AceCount, sizeof(SYSTEM_AUDIT_ACE)));
	if (pacl->AceCount)
	{
		PISID pSid;
		ULONG cou;
		PSYSTEM_AUDIT_ACE pAce;
		for (cou = 0; cou < pacl->AceCount; cou++)
		{
			 ntStatus = _pfRtlGetAce(pacl, cou, &pAce);
			 DbPrint(DC_SYS, DL_IMPORTANT, ("\t\tget ACE %d status %#x\n", cou, ntStatus));
			 if (NT_SUCCESS(ntStatus))
			 {
				 DbPrint(DC_SYS, DL_IMPORTANT, ("\t\t\tHeader: type %d, flags %#x size %d,Access Mask: %#x", 
					 pAce->Header.AceType, pAce->Header.AceFlags, pAce->Header.AceSize,
					 pAce->Mask));

				 pSid = (PSID) &pAce->SidStart;
				 DbPrint(DC_SYS, DL_IMPORTANT, ("\tsid lenght %d (sid %s)\n", RtlLengthSid(pSid), RtlValidSid(pSid) ? "valid" : "not valid"));
			 }
		}
	}
}

#define _sa_info_max_len (SID_MAX_SUB_AUTHORITIES * sizeof(ULONG))
#define _sa_valid_pos 3

BOOL SA_MakeOurSid(PISID pSid, DWORD sidsize, ULONG* pnew_sid_len, PVOID psa_info)
{
	RtlZeroMemory(pSid, sidsize);
        
	pSid->Revision = SID_REVISION;
	pSid->SubAuthorityCount = SID_MAX_SUB_AUTHORITIES;
	pSid->IdentifierAuthority = gOurSidAuthority;

	memcpy(pSid->SubAuthority, psa_info, _sa_info_max_len);

	((BYTE*)pSid->SubAuthority)[0] = 'K';
	((BYTE*)pSid->SubAuthority)[1] = 'A';
	((BYTE*)pSid->SubAuthority)[2] = 'V';

	*pnew_sid_len = RtlLengthSid(pSid);
	DbPrint(DC_SYS, DL_SPAM, ("MakeOurSid: new SID length %d\n", *pnew_sid_len));

	if (!RtlValidSid(pSid))
	{
		DbPrint(DC_SYS, DL_ERROR, ("MakeOurSid: created not valid SID (strange)\n"));
		return FALSE;
	}

	return TRUE;
}

void SA_FillAce(PSYSTEM_AUDIT_ACE pOurAce)
{
	pOurAce->Header.AceType = SYSTEM_AUDIT_ACE_TYPE;
	pOurAce->Header.AceFlags = 0;
	pOurAce->Mask = 0;
}

NTSTATUS SA_AddAce(PACL pacl, PISID	NewSid, PSYSTEM_AUDIT_ACE* ppOurAce)
{
	NTSTATUS				ntStatus = RtlAddAccessAllowedAce(pacl, ACL_REVISION2, 0, NewSid);
	if (!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS, DL_NOTIFY, ("RtlAddAccessAllowedAce failed %#x\n", ntStatus));
	}
	else
	{
		ntStatus = _pfRtlGetAce(pacl, pacl->AceCount - 1, ppOurAce);
		if (!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_SYS, DL_ERROR, ("fRtlGetAce after RtlAddAccessAllowedAce failed %#x\n", ntStatus));
			DbgBreakPoint();
		}
	}
	
	if (NT_SUCCESS(ntStatus))
		SA_FillAce(*ppOurAce);

	return ntStatus;
}

NTSTATUS
SA_GetData(HANDLE hFile, PVOID psa_info, ULONG* psa_info_len)
{
	NTSTATUS				ntStatusRet = STATUS_NOT_SUPPORTED;
	NTSTATUS				ntStatus;
	
	PFILE_OBJECT			fileObject;

	if (*NtBuildNumber <= 1381)
		return STATUS_NOT_SUPPORTED;

	if (!psa_info || !psa_info_len)
		return STATUS_INVALID_PARAMETER;

	if (*psa_info_len != _sa_info_max_len)
		return STATUS_INVALID_PARAMETER;

	ntStatus = ObReferenceObjectByHandle(hFile, ACCESS_SYSTEM_SECURITY, NULL, KernelMode, (PVOID*) &fileObject, NULL);
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS, DL_ERROR, ("SA_SetData: ObReferenceObjectByHandle failed %#x\n", ntStatus));
		DbgBreakPoint();
	}
	else
	{
		PISECURITY_DESCRIPTOR  pObjectSecurityDescriptor;
		BOOLEAN  MemoryAllocated;
		ntStatus = ObGetObjectSecurity(fileObject, &pObjectSecurityDescriptor, &MemoryAllocated);
		if(NT_SUCCESS(ntStatus))
		{
			ntStatusRet = STATUS_NOT_FOUND;
			if(SE_SACL_PRESENT & pObjectSecurityDescriptor->Control)
			{
				PSYSTEM_AUDIT_ACE		pOurAce = NULL;
				PACL pacl = pObjectSecurityDescriptor->Sacl;
				if (SE_SELF_RELATIVE & pObjectSecurityDescriptor->Control)
					pacl = (PACL)((__int8*)pObjectSecurityDescriptor + (ULONG) pObjectSecurityDescriptor->Sacl);

				if (pacl)
					pOurAce = SA_FoundOurAce(pacl);

				if (pOurAce)
				{
					PISID pSidTmp = (PSID) &pOurAce->SidStart;
					memcpy(psa_info, pSidTmp->SubAuthority, _sa_info_max_len);
					ntStatusRet = STATUS_SUCCESS;
				}
			}

			ObReleaseObjectSecurity(pObjectSecurityDescriptor, MemoryAllocated);
		}

		ObDereferenceObject(fileObject);
	}

	return ntStatusRet;
}

NTSTATUS
SA_SetData(HANDLE hFile, PVOID psa_info, ULONG sa_info_len)
{
	NTSTATUS				ntStatus;
	NTSTATUS				ntStatusResult = STATUS_NOT_SUPPORTED;
	PFILE_OBJECT			fileObject;
	
	PISECURITY_DESCRIPTOR	pDescrSet = NULL;
	PISECURITY_DESCRIPTOR	pDescrNew = NULL;
	PSYSTEM_AUDIT_ACE		pOurAce = NULL;


	UCHAR					sidBuffer[256];
	PISID					NewSid = (PISID) sidBuffer;
	ULONG					new_sid_len = 0;

	SECURITY_INFORMATION	sec_info = SACL_SECURITY_INFORMATION;

	PISECURITY_DESCRIPTOR	pResultingSecurity = NULL;

	if (*NtBuildNumber <= 1381)
		return STATUS_NOT_SUPPORTED;

	if (sa_info_len != _sa_info_max_len)
	{
		DbPrint(DC_SYS, DL_ERROR, ("wrong sa_info_len %d (must be %d)\n", sa_info_len, _sa_info_max_len));
		return ntStatusResult;
	}

	if (!SA_MakeOurSid(NewSid, sizeof(sidBuffer), &new_sid_len, psa_info))
		return ntStatusResult;

	ntStatus = ObReferenceObjectByHandle(hFile, ACCESS_SYSTEM_SECURITY, NULL, KernelMode, (PVOID*) &fileObject, NULL);
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS, DL_ERROR, ("SA_SetData: ObReferenceObjectByHandle failed %#x\n", ntStatus));
		DbgBreakPoint();
	}
	else
	{
		PISECURITY_DESCRIPTOR  pObjectSecurityDescriptor;
		BOOLEAN  MemoryAllocated = FALSE;
		ntStatus = ObGetObjectSecurity(fileObject, &pObjectSecurityDescriptor, &MemoryAllocated);
		if(!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_SYS, DL_ERROR, ("SA_SetData: ObGetObjectSecurity failed %#x\n", ntStatus));
			pObjectSecurityDescriptor = NULL;
			DbgBreakPoint();
		}
		else
		{
			ntStatusResult = STATUS_NOT_FOUND;
			if(SE_SACL_PRESENT & pObjectSecurityDescriptor->Control)
			{
				PACL pacl = pObjectSecurityDescriptor->Sacl;
				if (SE_SELF_RELATIVE & pObjectSecurityDescriptor->Control)
					pacl = (PACL)((__int8*)pObjectSecurityDescriptor + (ULONG) pObjectSecurityDescriptor->Sacl);

				if (pacl)
					pOurAce = SA_FoundOurAce(pacl);

				if (pOurAce)
				{
					PISID pSidTmp = (PSID) &pOurAce->SidStart;
					memcpy(pSidTmp, NewSid, new_sid_len);

					pDescrSet = pObjectSecurityDescriptor;
				}
				else
				{
					// add new ace!
					ULONG secr_rel_len = RtlLengthSecurityDescriptor(pObjectSecurityDescriptor);
					
					secr_rel_len += sizeof(SYSTEM_AUDIT_ACE) + new_sid_len;

					pDescrNew = ExAllocatePoolWithTag(PagedPool, secr_rel_len,'TBOS');
					if (pDescrNew)
					{
						ntStatus = SeQuerySecurityDescriptorInfo(&sec_info, (PSECURITY_DESCRIPTOR) pDescrNew, &secr_rel_len, &pObjectSecurityDescriptor);
					
						pacl = pDescrNew->Sacl;
						if (SE_SELF_RELATIVE & pDescrNew->Control)
							pacl = (PACL)((__int8*)pDescrNew + (ULONG) pDescrNew->Sacl);

						pacl->AclSize += sizeof(SYSTEM_AUDIT_ACE) + new_sid_len;
						if (NT_SUCCESS(SA_AddAce(pacl, NewSid, &pOurAce)))
							pDescrSet = pDescrNew;
					}
				}
			}
			else
			{
				// create new acl
				ULONG secr_rel_len = sizeof(SECURITY_DESCRIPTOR) + sizeof(ACL) + sizeof(SYSTEM_AUDIT_ACE) + new_sid_len;

				pDescrNew = ExAllocatePoolWithTag(PagedPool, secr_rel_len,'TBOS');
				if (pDescrNew)
				{
					ntStatus = RtlCreateSecurityDescriptor(pDescrNew, SECURITY_DESCRIPTOR_REVISION1);
					if (!NT_SUCCESS(ntStatus))
					{
						DbPrint(DC_SYS, DL_ERROR, ("RtlCreateSecurityDescriptor failed %#x\n", ntStatus));
					}
					else
					{
						PACL pacl = (PACL)((BYTE*)pDescrNew + sizeof(SECURITY_DESCRIPTOR));
						pDescrNew->Control = SE_SACL_PRESENT | SE_SELF_RELATIVE;
						pDescrNew->Sacl = (PACL) sizeof(SECURITY_DESCRIPTOR);

						if (SE_SACL_AUTO_INHERITED & pObjectSecurityDescriptor->Control)
							pDescrNew->Control |= SE_SACL_AUTO_INHERITED;

						ntStatus = RtlCreateAcl(pacl, sizeof(ACL) + sizeof(SYSTEM_AUDIT_ACE) + new_sid_len - sizeof(ULONG), ACL_REVISION2);
						if (!NT_SUCCESS(ntStatus))
						{
							DbPrint(DC_SYS, DL_ERROR, ("RtlCreateAcl failed %#x\n", ntStatus));
						}
						else
						{
							if (NT_SUCCESS(SA_AddAce(pacl, NewSid, &pOurAce)))
								pDescrSet = pDescrNew;
						}
					}
				}
			}
		}


		if (pDescrSet && pOurAce)
		{
			ntStatusResult = _pfZwSetSecurityObject(hFile, sec_info, pDescrSet);
			if (!NT_SUCCESS(ntStatusResult))
			{
				DbPrint(DC_SYS, DL_IMPORTANT, ("ZwSetSecurityObject failed %#x\n", ntStatusResult));
				DbgBreakPoint();
			}
		}

		if (pObjectSecurityDescriptor)
			ObReleaseObjectSecurity(pObjectSecurityDescriptor, MemoryAllocated);

		ObDereferenceObject(fileObject);
	}

	if (pDescrNew)
		ExFreePool(pDescrNew);

	return ntStatusResult;
}

NTSTATUS SA_ClearValidFlag(PWCHAR pwchFile, ULONG namelen)
{
	NTSTATUS ntStatusRet = STATUS_ACCESS_DENIED;
	PWCHAR pFullName;
	BOOLEAN bPrefixAlreadyExist;
	
	if (*NtBuildNumber <= 1381)
		return STATUS_NOT_SUPPORTED;

	if (pwchFile == NULL)
		return STATUS_UNSUCCESSFUL;

	if (namelen == 0)
		namelen = (wcslen(pwchFile) + 1) * sizeof(WCHAR);

	if (namelen <= 2)
		return STATUS_UNSUCCESSFUL;
	
	if (pwchFile[0] == L'\\' && pwchFile[1] == L'\\')
		return STATUS_NOT_SUPPORTED;

	if(namelen >= 8 && *(ULONG*)pwchFile==0x3f005c && *(ULONG*)(pwchFile+2)==0x5c003f)
		bPrefixAlreadyExist = TRUE;
	else {
		namelen += sizeof(L"\\??\\");
		bPrefixAlreadyExist=FALSE;
	}

	pFullName = ExAllocatePoolWithTag(NonPagedPool, namelen, 'SboS');
	if (pFullName == NULL)
		return STATUS_MEMORY_NOT_ALLOCATED;

	if(!bPrefixAlreadyExist) 
		wcscpy(pFullName, L"\\??\\");
	else
		pFullName[0] = 0;
	wcscat(pFullName, pwchFile);

	{
		NTSTATUS ntStatus;

		UNICODE_STRING		us;
		OBJECT_ATTRIBUTES	objectAttributes;
		IO_STATUS_BLOCK		ioStatus;

		HANDLE hFile_SA = 0;

		RtlInitUnicodeString(&us, pFullName);

		InitializeObjectAttributes(&objectAttributes, &us, OBJ_CASE_INSENSITIVE, NULL, NULL);
	
		AddInvisibleThread((ULONG) PsGetCurrentThreadId());
		
		ntStatus = ZwCreateFile(&hFile_SA, 0, &objectAttributes, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

		if (!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_SYS, DL_IMPORTANT, ("create file for sa failed %#x\n", ntStatus));
		}
		else
		{
			// modify SA
			BYTE sa_info[_sa_info_max_len];
			ULONG sa_info_len = sizeof(sa_info);
			ntStatusRet = STATUS_SUCCESS;

			if (NT_SUCCESS(SA_GetData(hFile_SA, sa_info, &sa_info_len)))
			{
				if (sa_info[_sa_valid_pos])
				{
					sa_info[_sa_valid_pos] = 0;
					if (!NT_SUCCESS(SA_SetData(hFile_SA, sa_info, sizeof(sa_info))))
					{
						DbgBreakPoint();
					}
				}
			}

			ZwClose(hFile_SA);
		}

		DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);
	
	}
	
	ExFreePool(pFullName);

	return ntStatusRet;
}

VOID
SA_PatchSecrDescr(PISECURITY_DESCRIPTOR pidescr)
{
	if (*NtBuildNumber <= 1381)
		return;

	if(SE_SACL_PRESENT & pidescr->Control)
	{
		PSYSTEM_AUDIT_ACE pOurAce = NULL;
		PACL pacl = pidescr->Sacl;
		if (SE_SELF_RELATIVE & pidescr->Control)
			pacl = (PACL)((__int8*)pidescr + (ULONG) pidescr->Sacl);
		
		if (pacl)
			pOurAce = SA_FoundOurAce(pacl);
		
		if (pOurAce)
		{
			PISID pSid = (PSID) &pOurAce->SidStart;
			DbgBreakPoint();

			((BYTE*)pSid->SubAuthority)[_sa_valid_pos] = 0;
		}
	}
}*/
#endif // _HOOK_NT_

