#ifndef ___CKAHCOMM_H___
#define ___CKAHCOMM_H___

#include "defs.h"
#include "ProgramExecutionLog.h"
#include "GuardClient.h"

CKAHCOMMEXPORT void CkahComm_SetExternalLogger (CProgramExecutionLog::EXTERNALLOGGERPROC logger);

#endif // ___CKAHCOMM_H___