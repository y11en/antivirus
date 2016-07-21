#ifndef __avpgimpl
#define __avpgimpl

#include <windows.h>
#include "../../windows/hook/Hook/avpgcom.h"

#include <ProductCore/common/userban.h>


#include "job_thread.h"


#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/plugin/p_mkavio.h>
#include <Prague/plugin/p_win32_nfio.h>

#include <AV/iface/i_avpg.h>

#include <Extract/plugin/p_windiskio.h>

#include <ProductCore/plugin/p_processmonitor.h>
#include <ProductCore/structs/procmonm.h>
#include <ProductCore/structs/s_trmod.h>

//+ ------------------------------------------------------------------------------------------



extern "C" hROOT g_root;

extern tPROPID propid_EventDefinition;
extern tPROPID propid_UserInfo;
extern tPROPID propid_CrUserInfo;
extern tPROPID propid_ProcessDefinition;
extern tPROPID propid_ThreadDefinition;
extern tPROPID propid_ObjectModified;
extern tPROPID propid_EventProcessID;

extern tPROPID propid_EventStartProcessing;
extern tPROPID propid_BeforeCreateIO;
extern tPROPID propid_BeforeCheck1;
extern tPROPID propid_BeforeCheck2;
extern tPROPID propid_BeforeCheck3;
extern tPROPID propid_ICheckerReadTime;
extern tPROPID propid_EventVerdict;
extern tPROPID propid_EventEndProcessing;

extern tPROPID propid_ResultCreateIO;
extern tPROPID propid_ResultProcessing1;
extern tPROPID propid_ResultProcessingVerdict;

extern tPROPID propid_CustomOrigin;
extern tPROPID propid_DosDeviceName;

extern tPROPID propid_Streamed;

extern tPROPID propid_Delayed;
//+ ------------------------------------------------------------------------------------------

void* _MM_Alloc(unsigned int nSize);
void _MM_Free(void* p);
int _MM_Compare(void* p1, void* p2, int len);
//+ ------------------------------------------------------------------------------------------


tBOOL CheckNetworkName(hSTRING hObjectName);

_eEventOperation GetFileOperation(DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD PlatformID);

DWORD GetListNonOpFiles(OSVERSIONINFO* pOsVer);
void NonOpFilesDone();
char* GetNonOpFile(DWORD idx);
BOOL __GetDosDeviceName(char* pRing0Name, char *pDosName);

DWORD GetSubstringPosBM(BYTE* Str, DWORD sourcse_strlen, BYTE* SubStr, DWORD SubStrLen);


#endif __avpgimpl