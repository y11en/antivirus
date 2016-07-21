////////////////////////////////////////////////////////////////////
//
//  AGENTINIT.H
//  CCClient initialization
//  Project AVP
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef _AGENTINT
#define _AGENTINT

#include <CCClient/taskstat.h>
#include <CCClient/LoadCCClientDLL.h>

int LoadSettings(LPPOLICY lpPolicy);
int InitCCClientInterface(DWORD dwID);

#endif

