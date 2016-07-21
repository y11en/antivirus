#ifndef _SECURITY_H_
#define _SECURITY_H_

#include "pch.h"

void
Security_ReleaseContext(
						SECURITY_CLIENT_CONTEXT *pClientContext
						);

NTSTATUS
Security_CaptureContext(
						PETHREAD OrigTh,
						SECURITY_CLIENT_CONTEXT *pClientContext
						);

NTSTATUS
Security_ImpersonateClient(
					IN PSECURITY_CLIENT_CONTEXT ClientContext,
					IN PETHREAD ServerThread OPTIONAL
					);

//+ ------------------------------------------------------------------------------------------

#endif // _SECURITY_H_