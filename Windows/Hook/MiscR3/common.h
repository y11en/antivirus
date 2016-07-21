#ifndef __COMMON_H
#define __COMMON_H
//-----------------------------------------------------------------

#include "..\hook\fssync.h"
#include "..\hook\avpgcom.h"

typedef
HRESULT
(FS_PROC *t_fDRV_Register) (
							__out PVOID* ppClientContext,
							__in ULONG AppId,
							__in ULONG Priority,
							__in ULONG ClientFlags,
							__in ULONG CacheSize,
							__in ULONG BlueScreenTimeout,
							__in DRV_pfMemAlloc pfAlloc,
							__in DRV_pfMemFree pfFree,
							__in_opt PVOID pOpaquePtr
							);
typedef
VOID
(FS_PROC *t_fDRV_UnRegister) (
							  __in PVOID* ppClientContext
							  );
typedef
HRESULT
(FS_PROC *t_fDRV_ChangeActiveStatus) (
									  __in PVOID pClientContext,
									  __in BOOL bActive
									  );

typedef
HRESULT
(FS_PROC *t_fDRV_FilterEvent) (
				 __in PVOID pClientContext,
				 __in PFILTER_EVENT_PARAM pEventParam,
				 __in BOOL bTry,
				 __out PULONG pVerdictFlags
				 );

typedef
VERDICT
(FS_PROC *t_fDRV_VerdictFlags2Verdict) (
						  ULONG VerdictFlags
						  );

typedef struct _FSSYNC_CONTEXT
{
	HMODULE								m_hFSSync;

	t_fDRV_Register						m_DRV_Register;
	t_fDRV_UnRegister					m_DRV_UnRegister;
	t_fDRV_ChangeActiveStatus			m_DRV_ChangeActiveStatus;
	t_fDRV_FilterEvent					m_DRV_FilterEvent;
	t_fDRV_VerdictFlags2Verdict			m_DRV_VerdictFlags2Verdict;

	PVOID								m_pClientContext;
} FSSYNC_CONTEXT, *PFSSYNC_CONTEXT;

//-----------------------------------------------------------------
#endif