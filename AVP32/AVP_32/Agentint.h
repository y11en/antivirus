/////////////////////////////////////////////////////////////////////////////////
// Agent interface header file
#ifndef _AGENTINT

#define _AGENTINT

#include <CCClient/taskstat.h>
#include <CCClient/LoadCCClientDLL.h>

extern UINT LM_UPDATE_DATA;

int LoadSettings(LPPOLICY lpPolicy);
int InitCCClientInterface(DWORD dwID);

#endif