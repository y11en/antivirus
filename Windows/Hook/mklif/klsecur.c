#include "klsecur.h"
#include "debug.h"

//+ ------------------------------------------------------------------------------------------

void
Security_ReleaseContext (
	SECURITY_CLIENT_CONTEXT *pClientContext
	)
{
	if(TokenPrimary == SeTokenType( pClientContext->ClientToken ))
		ObDereferenceObject(pClientContext->ClientToken);
	else if(pClientContext->ClientToken)
		ObDereferenceObject( pClientContext->ClientToken );
}

NTSTATUS
Security_CaptureContext(
	PETHREAD OrigTh,
	SECURITY_CLIENT_CONTEXT *pClientContext
	)
{
	SECURITY_QUALITY_OF_SERVICE SecurityQos;

	SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
	SecurityQos.ImpersonationLevel = SecurityImpersonation;
	SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
	SecurityQos.EffectiveOnly = FALSE;

	return SeCreateClientSecurity( OrigTh, &SecurityQos, FALSE, pClientContext );
}

NTSTATUS
Security_ImpersonateClient(
					IN PSECURITY_CLIENT_CONTEXT ClientContext,
					IN PETHREAD ServerThread OPTIONAL
					)
{
	return SeImpersonateClientEx( ClientContext, ServerThread );
}
