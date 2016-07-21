#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "CKAHMain.h"
#include "PluginUtils.h"
#include "IDS.h"
#include "../../windows/Hook/Plugins/Include/attacknotify_api.h"

static CKAHUM_IDS g_IDS;

static bool StartIDSReceiver (CKAHIDS::ATTACKNOTIFYPROC AttackNotifyCallback, LPVOID lpCallbackContext)
{
	if (g_IDS.Enable (true))
	{
		g_IDS.SetAttackNotifyCallback (AttackNotifyCallback, lpCallbackContext);
		return true;
	}
	return false;
}

CKAHUM::OpResult CKAHIDS::Start (ATTACKNOTIFYPROC AttackNotifyCallback,
											 LPVOID lpCallbackContext)
{
	log.WriteFormat (_T("[CKAHIDS::Start] Starting IDS..."), PEL_INFO);

	if (!StartIDSReceiver (AttackNotifyCallback, lpCallbackContext))
	{
		log.WriteFormat (_T("[CKAHIDS::Start] Failed to start IDS receiver"), PEL_ERROR);
		g_IDS.Enable (false);
		return CKAHUM::srOpFailed;
	}

	CKAHUM::OpResult result = CKAHUM_InternalStart (CCKAHPlugin::ptIDS, g_lpStorage);

	if (result != CKAHUM::srOK && result != CKAHUM::srAlreadyStarted)
	{
		log.WriteFormat (_T("[CKAHIDS::Start] Failed to start IDS (0x%08x)"), PEL_ERROR, result);
		g_IDS.Enable (false);
		log.WriteFormat (_T("[CKAHIDS::Start] Starting IDS done"), PEL_ERROR);
	}
	else
		log.WriteFormat (_T("[CKAHIDS::Start] IDS started successfully (0x%08x)"), PEL_INFO, result);

	return result;	
}

CKAHUM::OpResult CKAHIDS::Stop ()
{
	log.WriteFormat (_T("[CKAHIDS::Stop] Stopping IDS..."), PEL_INFO);
	g_IDS.Enable (false);
	CKAHUM::OpResult result = CKAHUM_InternalStop (CCKAHPlugin::ptIDS);
	log.WriteFormat (_T("[CKAHIDS::Stop] Stopping IDS done"), PEL_INFO);
	return result;
}

CKAHUM::OpResult CKAHIDS::Pause ()
{
	log.WriteFormat (_T("[CKAHIDS::Pause] Pausing IDS..."), PEL_INFO);
	CKAHUM::OpResult result = CKAHUM_InternalPause (CCKAHPlugin::ptIDS);
	log.WriteFormat (_T("[CKAHIDS::Pause] Pausing IDS done"), PEL_INFO);
	return result;
}

CKAHUM::OpResult CKAHIDS::Resume ()
{
	log.WriteFormat (_T("[CKAHIDS::Resume] Resuming IDS..."), PEL_INFO);
	CKAHUM::OpResult result = CKAHUM_InternalResume (CCKAHPlugin::ptIDS);
	log.WriteFormat (_T("[CKAHIDS::Resume] Resuming IDS done"), PEL_INFO);
	return result;
}

CKAHUM::OpResult CKAHIDS::SetIDSParam (ULONG VerdictID, ULONG ParmNum, ULONG ParmValue)
{
	log.WriteFormat (_T("[CKAHIDS::SetIDSParam] Setting IDS parameter: ID = 0x%08x, ParmNum = %d, ParmValue = %d"), PEL_INFO,
					VerdictID, ParmNum, ParmValue);

	std::vector<BYTE> vec_buf (sizeof (IDSKERNELSETPARMS), 0);
	IDSKERNELSETPARMS *pIDSParams = (IDSKERNELSETPARMS *)&vec_buf[0];

	pIDSParams->VerdictID = VerdictID;
	pIDSParams->ParmNum = ParmNum;
	pIDSParams->ParmValue = ParmValue;

	IDSKERNELSETPARMSRET RetResult;
	ZeroMemory (&RetResult, sizeof (RetResult));

	return (SendPluginMessage (CCKAHPlugin::ptIDS, PLUG_IOCTL, IDS_KERNEL_PLUGINNAME, IDS_MSGID_SETPARMS, &vec_buf[0], vec_buf.size (),
									&RetResult, sizeof (RetResult)) == CKAHUM::srOK && 
				RetResult.Status != 0) ? CKAHUM::srOK : CKAHUM::srOpFailed;
}

CKAHUM::OpResult CKAHIDS::GetIDSParam (ULONG VerdictID, ULONG ParmNum, PULONG pParmValue)
{
	log.WriteFormat (_T("[CKAHIDS::GetIDSParam] Getting IDS parameter: ID = 0x%08x, ParmNum = %d"), PEL_INFO,
					VerdictID, ParmNum);

	if (pParmValue == NULL)
	{
		log.Write (_T("[CKAHIDS::GetIDSParam] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}

	std::vector<BYTE> vec_buf (sizeof (IDSKERNELGETPARM), 0);
	IDSKERNELGETPARM *pIDSParams = (IDSKERNELGETPARM *)&vec_buf[0];

	pIDSParams->VerdictID = VerdictID;
	pIDSParams->ParmNum = ParmNum;

	IDSKERNELGETPARMRET RetResult;
	ZeroMemory (&RetResult, sizeof (RetResult));

	CKAHUM::OpResult or = SendPluginMessage (CCKAHPlugin::ptIDS, PLUG_IOCTL, IDS_KERNEL_PLUGINNAME, IDS_MSGID_GETPARM, &vec_buf[0], vec_buf.size (),
									&RetResult, sizeof (RetResult));

	return (or != CKAHUM::srOK || RetResult.Status == 0) ? CKAHUM::srOpFailed : (*pParmValue = RetResult.ParmValue, CKAHUM::srOK);
}

CKAHUM::OpResult CKAHIDS::GetIDSParamCount (ULONG VerdictID, PULONG pParamCount)
{
	log.WriteFormat (_T("[CKAHIDS::GetIDSParamCount] Getting IDS parameter count: ID = 0x%08x"), PEL_INFO,
					VerdictID);

	if (pParamCount == NULL)
	{
		log.Write (_T("[CKAHIDS::GetIDSParamCount] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}

	std::vector<BYTE> vec_buf (sizeof (IDSKERNELGETPARMCNT), 0);
	IDSKERNELGETPARMCNT *pIDSParams = (IDSKERNELGETPARMCNT *)&vec_buf[0];

	pIDSParams->VerdictID = VerdictID;

	IDSKERNELGETPARMCNTRET RetResult;
	ZeroMemory (&RetResult, sizeof (RetResult));

	CKAHUM::OpResult or = SendPluginMessage (CCKAHPlugin::ptIDS, PLUG_IOCTL, IDS_KERNEL_PLUGINNAME, IDS_MSGID_GETPARMCNT, &vec_buf[0], vec_buf.size (),
									&RetResult, sizeof (RetResult));

	return (or != CKAHUM::srOK || RetResult.Status == 0) ? CKAHUM::srOpFailed : (*pParamCount = RetResult.ParmCount, CKAHUM::srOK);
}

CKAHUM::OpResult CKAHIDS::SetIDSAttackNotifyParam (ULONG VerdictID, ULONG ResponseFlags, ULONG BanTime)
{
	log.WriteFormat (_T("[CKAHIDS::SetIDSAttackNotifyParam] Setting IDS attack notify parameter: ID = 0x%08x, Flags = %d, BanTime = %d"), PEL_INFO,
					VerdictID, ResponseFlags, BanTime);

	std::vector<BYTE> vec_buf (sizeof (AttackRespondItem), 0);

	AttackRespondItem *pIDSParams = (AttackRespondItem *)&vec_buf[0];

	pIDSParams->m_ID = VerdictID;
	pIDSParams->m_Respond = ResponseFlags;
	pIDSParams->m_BanTime = BanTime;

	return SendPluginMessage (CCKAHPlugin::ptIDS, PLUG_IOCTL, ATTACK_NOTIFICATOR_PLUGINNAME, MSG_ATTACK_RESPOND, &vec_buf[0], vec_buf.size (),
									NULL, 0);
}

CKAHUM::OpResult CKAHIDS::SetIDSAttackNotifyPeriod (ULONG NotifyPeriod)
{
	log.WriteFormat (_T("[CKAHIDS::SetIDSAttackNotifyPeriod] Setting IDS attack notify period = %d"), PEL_INFO, NotifyPeriod);

    return SendPluginMessage (CCKAHPlugin::ptIDS, PLUG_IOCTL, ATTACK_NOTIFICATOR_PLUGINNAME, MSG_SET_NOTIFY_PERIOD, &NotifyPeriod, sizeof(NotifyPeriod), NULL, 0);
}