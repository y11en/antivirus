#ifndef __INITTERM_H__
#define __INITTERM_H__

#include "stdafx.h"

#include <Prague/pr_remote.h>
#include <Prague/iface/i_loader.h>
#include <Prague/iface/i_root.h>

#include <exchext.h>

#undef PR_API
#ifdef __cplusplus
#define PR_API //extern "C"
#else
#define PR_API
#endif

BOOL InitPlugin (HINSTANCE hModule);
BOOL TermPlugin ();

BOOL InitEnvironment ();
BOOL TermEnvironment ();

hOBJECT GetMBL(BOOL bCheckIncoming);
hOBJECT GetBL();

extern PRRemoteAPI g_RP_API;
extern hROOT g_root;
extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propMailerPID;

#endif