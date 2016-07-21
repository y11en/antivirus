#ifndef __ahfw_h
#define __ahfw_h

#include <Windows.h>

//+ ------------------------------------------------------------------------------------------


#include <Prague/prague.h>
#include <AHTasks/plugin/p_ahfw.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_csect.h>
#include <AHTasks/structs/s_ahfw.h>

#include <AHTasks/iface/i_ipresolver.h>
#include <AHTasks/plugin/p_resip.h>

#include <AHTasks/structs/s_ipresolver.h>

#include <version/ver_product.h>        //product version definitions

#include "tchar.h"

//+ ------------------------------------------------------------------------------------------
typedef BOOL (WINAPI *tProcessIdToSessionId)(DWORD dwProcessId,DWORD* pSessionId);
extern tProcessIdToSessionId gfnProcessToSession;
//+ ------------------------------------------------------------------------------------------

#include <CKAH/CKAHUM.h>
#include <CKAH/CKAHSTAT.h>
using namespace CKAHFW;
#include <CKAH/CKAHPorts.h>
#include <CKAH/ckahrules.h>
#include <CKAH/ckahiptoa.h>

//void CALLBACK cbPacketRuleNotify(LPVOID lpContext, const PacketRuleNotify *pNotify);
//RuleCallbackResult CALLBACK cbApplicationRuleNotify(LPVOID lpContext, const ApplicationRuleNotify *pNotify);
//RuleCallbackResult CALLBACK cbCheckSumNotify(LPVOID lpContext, const ChecksumChangedNotify *pNotify);
//VOID CALLBACK cbNetworkChanges(LPVOID lpContext);


//+ ------------------------------------------------------------------------------------------

typedef struct _t_ahfwtask
{
	cTask*					m_hThis;
	cAHFWStatistics			m_Statistics;
//	cAHFWSettings			m_Settings;
	cAHFWCommonStatistics	m_CommonStat;
	
	tQWORD					m_Unique;
	cCriticalSection*		m_hSync;
	
	tDWORD					m_CallbackCounter;
	tTaskState*				m_pState;

	cIPRESOLVER*			m_IPResolver;
	tDWORD					*m_pTaskID;
}_ahfwtask;

//+ ------------------------------------------------------------------------------------------

void CALLBACK ahfwTrace(CKAHUM::LogLevel Level, LPCSTR szString);

CKAHUM::OpResult GenerateTimeRange(cFwBaseRule * pRule, HRULETIMES* phRuleTimes);
CKAHUM::OpResult GenerateAddrList(cVector<cFWAddress>* pList, HRULEADDRESSES* phRuleAddresses, cIPRESOLVER* pIPResolver);
CKAHUM::OpResult GeneratePortList(cVector<cFWPort>* pList, HRULEPORTS* phRulePorts);
CKAHUM::OpResult GenerateElement(cFwBaseRule * pSingleEl, HAPPRULEELEMENT hRuleElement, cIPRESOLVER* pIPResolver);

tERROR GetNetCfg(cVector<cIP> *pDhcp, cVector<cIP> *pDns, cVector<cIP> *pWins);

void   AHFWNetworks_CopyNetworkSettings(cAHFWNetwork &, cAHFWNetwork &);

tUINT  AHFWNetworks_FindNetwork(cAHFWNetworks &aNetworks, cAHFWNetwork &aNet);

tBOOL  AHFWNetworks_GetEffectiveNetwork(cAHFWNetworks &aNetworks, tUINT aIndex, cAHFWNetwork& aEffectiveNet);

tBOOL  AHFWNetworks_AddNetwork(cAHFWNetworks &aNetworks, cAHFWNetwork &aNewNet, tUINT aInsertIndex = (tUINT)(-1));
void   AHFWNetworks_DeleteNetwork(cAHFWNetworks &aNetworks, tUINT aIndex, tBOOL dependent, tBOOL hidden);

void   AHFWNetworks_HideNetworks(cAHFWNetworks& aNetworks, cAHFWNetwork &aNewNet, tUINT aSkipIndex = (tUINT)(-1));
void   AHFWNetworks_UnhideNetworks(cAHFWNetworks &aNetworks, cAHFWNetwork& aDeletedNet, tUINT aSkipIndex = (tUINT)(-1));

tUINT  AHFWNetworks_FindDependsOn(cAHFWNetworks &aNetworks, cAHFWNetwork &aNet);
void   AHFWNetworks_ChangeDependentNetworks(cAHFWNetworks &aNetworks, cAHFWNetwork &aNet);
tBOOL  AHFWNetworks_RedependNetworks(cAHFWNetworks& aNetworks, cAHFWNetwork& aNet);
void   AHFWNetworks_DeleteDependentNetworks(cAHFWNetworks& aNetworks, cAHFWNetwork& aDeletedNet);

void   AHFWNetworks_ChangeNetwork(cAHFWNetworks &aNetworks, tUINT aIndex, cAHFWNetwork& aNewNet);

tERROR AHFWNetworks_GetNetworks(cAHFWNetworks &aNetworks);

tBOOL  AHFWNetworks_UpdateNetworks(cAHFWNetworks &aNetworks, cAHFWNetworks& aNewNetworks);
void   AHFWNetworks_RefreshNetworks(cAHFWNetworks &aNetworks);


#define _task_name L"ahfw"

void   FWGetUnique(cCriticalSection* pSync, tQWORD* pCurrentVal, tQWORD* pUnique);
tDWORD GetSessionIDForProcess(ULONG ProcessId);
//void   CallbackEnter(_ahfwtask* pTask);
//void   CallbackLeave(_ahfwtask* pTask);

#endif // __ahfw_h