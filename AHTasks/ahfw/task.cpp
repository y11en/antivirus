// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  02 May 2007,  18:53 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- AHFW
// Sub project -- Not defined
// Purpose     -- задача firewall
// Author      -- Sobko
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "ahfw_imp.h"
// AVP Prague stamp end

#include <CKAH/ipconv.h>
#include <ProductCore/iptoa.h>

#include <Prague/iface/i_threadpool.h>

#include <Updater/updater.h>

#define IMPEX_EXPORT_LIB
	#include <AHTasks/plugin/p_ahfw.h>

#include <iphlpapi.h>
#include <iptypes.h>
#include <stdio.h>

#define PROTO_ICMPV6 0x3A

class cCS : protected CRITICAL_SECTION
{
public:
	cCS()        { InitializeCriticalSection(this); }
	~cCS()       { DeleteCriticalSection(this); }
	void Enter() { EnterCriticalSection(this); }
	void Leave() { LeaveCriticalSection(this); }
};

class cAutoLockerCS
{
public:
	cAutoLockerCS(cCS &cs) : m_cs(cs) { m_cs.Enter(); }
	~cAutoLockerCS()                  { m_cs.Leave(); }

protected:
	cCS & m_cs;
};

class cNetworksFinder : public cThreadPoolBase, public cThreadTaskBase
{
public:
	cNetworksFinder() : cThreadPoolBase("NetworksFinder") {}
	tERROR run(void *pContext);

protected:
	void   do_work();

	void * m_pContext;
};

class cHash
{
public:
	cHash()                                   { Clear(); }
	bool    IsEmpty() const                   { for(tDWORD i = 0; i < sizeof(m_nHash); i++) if( m_nHash[i] ) return false; return true; }
	bool    IsEqual(const tBYTE *pHash) const { return !memcmp(m_nHash, pHash, sizeof(m_nHash)); }
	void    Copy(const tBYTE *pHash)          { memcpy(m_nHash, pHash, sizeof(m_nHash)); }
	void    Clear()                           { memset(m_nHash, 0, sizeof(m_nHash)); }

protected:
	tBYTE m_nHash[FwHashSize];
};

typedef cVector<cHash> cHashes;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAHFW : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	cCS             m_SettingsCS;
	hOBJECT         m_hBusinessLogic;
	tTaskState      m_TaskState;
	tBOOL           m_StealthEnabled;
	tDWORD          m_TaskID;
	_t_ahfwtask     m_Data;
	hOBJECT         m_hReg;
	cNetworksFinder m_NetFinder;
	cAHFWNetworks   m_AutoFoundedNetworks;
	bool            m_bAppLocked;
	bool            m_bZoneLocked;
	cHashes         m_aModifiedApp;
	cFwPacketRules  m_aRulesForNagent;
// AVP Prague stamp end



protected:
	tERROR AhErr2PrErr(CKAHUM::OpResult err);

	tERROR TaskStart();
	tERROR TaskStop();
	tERROR TaskPause();
	tERROR TaskResume();

private:
	tERROR UpdateSettingsInTask(cAHFWSettings* pSettings, tBOOL* pbSettingsChanged);
	tERROR UpdateStmSettings(cAHFWSettings* pSettings);
	tERROR NotifyAppRuleAdded(cFwAppRule &Rule);
	tERROR AddSpecialPacketRules(cFwPacketRules &aRules);
	tERROR Networks2Rules(cFwPacketRules &Rules, cAHFWNetworks &KnownNetworks);
	tERROR AddPortRules(HPACKETRULES hPorts, cFwPacketRules &Rules);
	tERROR AddAppRules(HAPPRULES hPorts, cFwAppRules &Rules, cAHFWSettings* pSettings, tBOOL* pbSettingsChanged);
	tERROR PrependAppRule(cFwAppRule &Rule);
	tERROR AppendAppRule(cFwAppRule &Rule);
	tERROR UpdatePacketRules(cAHFWSettings *pSettings = NULL);
	void   GetNewNetworks(cAHFWNetworks &aAllNetworks);
	tERROR GetUnstealthedHosts(cVector<cIP> &aUnstealthedHosts);
	void   PrependAppRule(cAHFWSettings *pSettings, cFwAppRule &Rule, tBOOL &bSettingsChanged);
	void   AppendAppRule(cAHFWSettings *pSettings, cFwAppRule &Rule, tBOOL &bSettingsChanged);

	
	cAHFWSettings* LockSettingsInTM(tBOOL bReadOnly);
	void           UnlockSettingsInTM(cAHFWSettings* pSettings, tBOOL bChanged);


	
public:
	static void               CALLBACK cbNetworkChangesLink(LPVOID lpContext);
	static void               CALLBACK cbPacketRuleNotifyLink(LPVOID lpContext, const PacketRuleNotify *pNotify);
	static RuleCallbackResult CALLBACK cbAppRuleNotifyLink(LPVOID lpContext, const ApplicationRuleNotify *pNotify);
	static RuleCallbackResult CALLBACK cbCheckSumNotifyLink(LPVOID lpContext, const ChecksumChangedNotify *pNotify);

protected:
	void               OnNetworkChanges();
	void               OnPacketRuleNotify(const PacketRuleNotify *pNotify);
	RuleCallbackResult OnAppRuleNotify(const ApplicationRuleNotify *pNotify);
	RuleCallbackResult OnCheckSumNotify(const ChecksumChangedNotify *pNotify);

	void   CallbackEnter();
	void   CallbackLeave();
	bool   CallbackIsAnyActive();
	
	void   ChangeSecurityState(bool bFWOn);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CAHFW)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR CAHFW::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here
	m_StealthEnabled = FALSE;
	m_Data.m_hThis = this;
	m_TaskID = 0;
	m_TaskState = TASK_STATE_UNKNOWN;
	m_hBusinessLogic = 0;
	
	m_Data.m_Unique = 0;
	m_Data.m_pTaskID = &m_TaskID;

	m_Data.m_pState = &m_TaskState;
	
	ChangeSecurityState(false);

	error = sysCreateObjectQuick((cObject**) &m_Data.m_hSync, IID_CRITICAL_SECTION);

	if (PR_SUCC(error))
	{
		CKAHUM::OpResult opres = CKAHUM::Initialize(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_W L"\\CKAHUM", ahfwTrace);
		if (CKAHUM::srOK != opres)
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		else
			error = sysCreateObjectQuick((cObject**) &m_Data.m_IPResolver, IID_IPRESOLVER);
	}
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CAHFW::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here

	error = errOK;
	sysRegisterMsgHandler(pmc_TASKAHFW_BREAK_CONNECTION,        rmhDECIDER,  m_hBusinessLogic, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmcTM_EVENTS,                         rmhLISTENER, m_hBusinessLogic, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_TASKAHFW_NAGENT_SETTINGS_CHANGED, rmhLISTENER, m_hBusinessLogic, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	
	SetNetworkChangeCallback(cbNetworkChangesLink, this);

	m_NetFinder.init(this);

	m_bAppLocked = m_bZoneLocked = false;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CAHFW::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	error = errOK;

	PR_TRACE((this, prtIMPORTANT, "fw\tclosing..."));

	SetNetworkChangeCallback(NULL, NULL);

	m_NetFinder.de_init();

	ChangeSecurityState(false);

	CKAHUM::Deinitialize(ahfwTrace);

	PR_TRACE((this, prtIMPORTANT, "fw\tclosed"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR CAHFW::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectClose method" );

	// Place your code here
	PR_TRACE((this, prtIMPORTANT, "fw\tobject close"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR CAHFW::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	error = errOK;

	if (pmc_TASKAHFW_BREAK_CONNECTION == p_msg_cls_id)
	{
		PR_TRACE((this, prtIMPORTANT, "fw\tBreak connection request"));
		if ((!m_bAppLocked) && p_par_buf_len && (p_par_buf_len == SER_SENDMSG_PSIZE) && p_par_buf)
		{
			cAHFWConnection* pConnection = (cAHFWConnection*) p_par_buf;
			CKAHUM::OpResult opres = BreakConnection(
                                            pConnection->m_Proto,
				                            &cIP_CKAHUMIP(pConnection->m_LocalIP),
                                            pConnection->m_LocalPort,
                                            &cIP_CKAHUMIP(pConnection->m_RemoteIP),
                                            pConnection->m_RemotePort);

			switch (opres)
			{
			case CKAHUM::srOK:
				error = errOK_DECIDED;
				break;
			default:
				error = errUNEXPECTED;
				PR_TRACE((this, prtIMPORTANT, "fw\tBreak connection failed (err %x)", opres));
				break;
			}
		}
	}

	if( p_msg_cls_id == pmcTM_EVENTS && p_msg_id == pmTM_EVENT_GUI_CONNECTED )
	{
		PR_TRACE((this, prtIMPORTANT, "fw\tGUI connected"));

		cAutoLockerCS Locker(m_SettingsCS);
		if( m_AutoFoundedNetworks.size() && !m_bZoneLocked )
			m_NetFinder.run(this);

		error = errOK;
	}

	if( p_msg_cls_id == pmc_TASKAHFW_NAGENT_SETTINGS_CHANGED )
	{
		cAHFWSettings *pSett = (cAHFWSettings *)p_par_buf;
		if( p_par_buf_len == SER_SENDMSG_PSIZE && pSett && pSett->isBasedOn(cAHFWSettings::eIID) )
		{
			{
				cAutoLockerCS Locker(m_SettingsCS);
				m_aRulesForNagent = pSett->m_PortRules;
			}
			if( m_TaskState == TASK_STATE_RUNNING || m_TaskState == TASK_STATE_PAUSED )
				UpdatePacketRules();
			
			error = errOK_DECIDED;
		}
		else
			error = errPARAMETER_INVALID;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CAHFW::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	error = errPARAMETER_INVALID;

	PR_TRACE((this, prtIMPORTANT, "fw\tSetSettings requset"));

	cAHFWSettings* pSettings = LockSettingsInTM(cFALSE);
	if(pSettings)
	{
		tBOOL bSettingsChanged = cFALSE;

		m_bAppLocked = pSettings->IsMandatoriedByPtr(&pSettings->m_AppRules);
		m_bZoneLocked = pSettings->IsMandatoriedByPtr(&pSettings->m_KnownNetworks);

		error = errUNEXPECTED;

		if( m_bZoneLocked )
		{
			cAutoLockerCS Locker(m_SettingsCS);
			m_AutoFoundedNetworks.clear();

			PR_TRACE((this, prtIMPORTANT, "fw\tCAHFW::SetSettings. zones are mandatoried by policy, m_AutoFoundedNetworks cleared"));
		}

        // clear hash for no hash check rules
	    cFwAppRules &AppRules = pSettings->m_AppRules;
	    for(tUINT i = 0; i < AppRules.size(); i++)
	    {
		    cFwAppRule &Rule = AppRules[i];
		    if( !Rule.m_bCheckHash )
            {
                memset(Rule.m_nHash, 0, sizeof(Rule.m_nHash));
                bSettingsChanged = cTRUE;
            }
	    }

		if ((pSettings->m_ProtectionLevel >= _fwwmBlockAll) && (pSettings->m_ProtectionLevel <= _fwwmAllowAll))
		{
			if ((TASK_STATE_RUNNING == m_TaskState) || (TASK_STATE_PAUSED == m_TaskState))
			{
				error = UpdateSettingsInTask(pSettings, &bSettingsChanged);
				
				if( TASK_STATE_RUNNING == m_TaskState && !m_bZoneLocked )
					m_NetFinder.run(this);
			}
			else
			{
				error = errOK;
			}
		}

		UnlockSettingsInTM(pSettings, bSettingsChanged);
	}
	
	PR_TRACE((this, prtIMPORTANT, "fw\tSetSettings result %terr", error));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CAHFW::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );
	
	// Place your code here
	// errNOT_IMPLEMENTED -> tm will be get settings from internal storage!

	PR_TRACE((this, prtIMPORTANT, "fw\tGetSettings(): err = 0x%08x", error));


	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CAHFW::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CAHFW::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "fw\tSetState. method" );

	PR_TRACE((this, prtIMPORTANT, "fw\tSetState. request to change state to %d", p_state));

	tTaskState NewState = TASK_STATE_UNKNOWN;
	switch(p_state)
	{
	case TASK_REQUEST_RUN:   error = TaskStart(); NewState = TASK_STATE_RUNNING; break;
	case TASK_REQUEST_PAUSE: error = TaskPause(); NewState = TASK_STATE_PAUSED;  break;
	case TASK_REQUEST_STOP:  error = TaskStop();  NewState = TASK_STATE_STOPPED; break;
	default:
		error = errPARAMETER_INVALID;
		PR_TRACE((this, prtERROR, "fw\tSetState. invalid state %d", p_state));
		break;
	}

	bool bSecurityStateOn = false;
	if( PR_SUCC(error) )
	{
		m_TaskState = NewState;
		bSecurityStateOn = m_TaskState == TASK_STATE_RUNNING;

		if( p_state == TASK_REQUEST_RUN && !m_bZoneLocked )
			m_NetFinder.run(this);
	
		PR_TRACE((this, prtIMPORTANT, "fw\tSetState. success. new state is %#x", m_TaskState));

		error = warnTASK_STATE_CHANGED;
	}

	ChangeSecurityState(bSecurityStateOn);

	return error;
}
// AVP Prague stamp end



#define CopyTransmitSizeData(Ts, Hs) { Ts.m_nBytesSent = Hs.outBytes; Ts.m_nBytesRecieved = Hs.inBytes; }

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CAHFW::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	error = errPARAMETER_INVALID;

	PR_TRACE((this, prtSPAM, "fw\tget statistics request"));

	if( p_statistics )
	{
		if (p_statistics->isBasedOn(cAHFWCommonStatistics::eIID))
		{
			error = p_statistics->assign(m_Data.m_CommonStat, true);
			PR_TRACE((this, prtNOT_IMPORTANT, "fw\tget common statistics result %terr", error));
			return error;
		}

		cAHFWStatisticsEx* pTreeStat = 0;
		if (p_statistics->isBasedOn(cAHFWStatisticsEx::eIID))
		{
			pTreeStat = (cAHFWStatisticsEx*)p_statistics;
			pTreeStat->assign(cAHFWStatisticsEx(), cTRUE);
			
			PR_TRACE((this, prtNOTIFY, "fw\trequest full stat"));
		}
		else
		{
			if (!p_statistics->isBasedOn(cAHFWStatistics::eIID))
			{
				PR_TRACE((this, prtERROR, "fw\tError: not based on cAHFWStatistics::eIID"));
				return error;
			}
		}

		cVector<ULONG> pids;
		int ports = 0;
		
		// port list
		HOPENPORTLIST hports;
		if (CKAHUM::srOK == GetOpenPortList(&hports))
		{
			if (CKAHUM::srOK == OpenPortList_GetSize(hports, &ports))
			{
				if (ports)
				{
					OpenPort port;
					for(int cou = 0; cou < ports; cou++)
					{
						if (CKAHUM::srOK == OpenPortList_GetItem(hports, cou, &port))
						{
							if (cVector<ULONG>::npos == pids.find(port.PID))
							{
								pids.push_back(port.PID);

								if (pTreeStat)
								{
									cAHFWAppConnection newapp;
									newapp.m_AppName.assign(port.AppName, cCP_UNICODE, 0);
									newapp.m_CmdLine.assign(port.CmdLine, cCP_UNICODE, 0);
									newapp.m_PID = port.PID;
								
									pTreeStat->m_Applications.push_back(newapp);
								}
							}
							
							// find and assign port
							if (pTreeStat)
							{
								int cou_all = pTreeStat->m_Applications.count();
								//cAHFWAppConnection* papptmp;
								for (int cou2 = 0; cou2 < cou_all; cou2++)
								{
									cAHFWAppConnection& apptmp = pTreeStat->m_Applications.at(cou2);
									if (apptmp.m_PID == port.PID)
									{
										cAHFWConnection connectiontmp;

										connectiontmp.m_Proto = port.Proto;	
										connectiontmp.m_LocalIP = CKAHUMIP_cIP(port.LocalIP);
										connectiontmp.m_LocalPort = port.LocalPort;
										connectiontmp.m_ActiveTime = port.ActiveTime;
										connectiontmp.m_InBytes = port.InBytes;
										connectiontmp.m_OutBytes = port.OutBytes;

										apptmp.m_Connections.push_back(connectiontmp);

										break;
									}
								}
							}
						}
					}
				}
			}

			OpenPortList_Delete(hports);
		}

		// connection list
		int connections = 0;

        //if ( CKAHUM::srOK != CKAHSTAT::GetConnectionCount((ULONG*)&connections) )
        {
            HCONNLIST hconnections;
            if (CKAHUM::srOK == GetConnectionList(&hconnections))
            {
                if (CKAHUM::srOK == ConnectionList_GetSize(hconnections, &connections))
                {
                    if (connections)
                    {
                        Connection connection;
                        for(int cou = 0; cou < connections; cou++)
                        {
                            if (CKAHUM::srOK == ConnectionList_GetItem(hconnections, cou, &connection))
                            {
                                if (pTreeStat)
                                {
                                    cAHFWConnection connectiontmp;

                                    connectiontmp.m_Proto = connection.Proto;	
                                    connectiontmp.m_LocalIP = CKAHUMIP_cIP(connection.LocalIP);
                                    connectiontmp.m_LocalPort = connection.LocalPort;
                                    connectiontmp.m_RemoteIP = CKAHUMIP_cIP(connection.RemoteIP);
                                    connectiontmp.m_RemotePort = connection.RemotePort;
                                    connectiontmp.m_Direction = connection.IsIncoming ? _fwInboundStream : _fwOutboundStream;
                                    connectiontmp.m_ActiveTime = connection.ActiveTime;
                                    connectiontmp.m_InBytes = connection.InBytes;
                                    connectiontmp.m_OutBytes = connection.OutBytes;

                                    cAHFWAppConnection appconnectiontmp;
                                    appconnectiontmp.m_AppName.assign(connection.AppName, cCP_UNICODE, 0);
                                    appconnectiontmp.m_CmdLine.assign(connection.CmdLine, cCP_UNICODE, 0);
                                    appconnectiontmp.m_PID = connection.PID;
                                    appconnectiontmp.m_Connections.push_back(connectiontmp);

                                    pTreeStat->m_Connections.push_back(appconnectiontmp);

                                }
                                if (cVector<ULONG>::npos == pids.find(connection.PID))
                                    pids.push_back(connection.PID);
                            }
                        }
                    }
                }

                ConnectionList_Delete(hconnections);
            }
        }

		if(pTreeStat)
		{
			HHOSTSTATLIST hHostStat;
			if(GetHostStatistics(&hHostStat) == CKAHUM::srOK)
			{
				int nSize;
				if(HostStatistics_GetSize(hHostStat, &nSize) == CKAHUM::srOK)
				{
					cFwHostStatistics &Hs = pTreeStat->m_HostStat;
					
					Hs.resize(nSize);
					for(int i = 0, j = 0; i < nSize; i++, j++)
					{
						cFwHostStat &Hsi = Hs[j];
						
						CKAHFW::HostStatItem Item;
						HostStatistics_GetItem(hHostStat, i, &Item);

                        if ( Item.HostIp.Version == 4 && Item.HostIp.v4 == 0)
                        {
                            j--;
                            Hs.resize( Hs.size() - 1 );
                            continue;
                        }

                        if ( Item.HostIp.Version == 6 && Item.HostIp.v6.O.Hi == 0 && Item.HostIp.v6.O.Lo == 0)
                        {
                            j--;
                            Hs.resize( Hs.size() - 1 );
                            continue;
                        }

						Hsi.m_Host = CKAHUMIP_cIP(Item.HostIp);
						CopyTransmitSizeData(Hsi.m_Total,     Item.TotalStat);
						CopyTransmitSizeData(Hsi.m_Tcp,       Item.TcpStat);
						CopyTransmitSizeData(Hsi.m_Udp,       Item.UdpStat);
						CopyTransmitSizeData(Hsi.m_Broadcast, Item.BroadcastStat);
					}
				}
				HostStatistics_Delete(hHostStat);
			}
		}

        __int64 nInBytes = 0, nOutBytes = 0;

        CKAHSTAT::GetTrafficInfoLastDay(&nInBytes, &nOutBytes );

        unsigned long nDroppedConnections = 0;

        CKAHSTAT::GetDroppedConnectionCount( &nDroppedConnections );

		//+ ------------------------------------------------------------------------------------------

		m_Data.m_Statistics.m_nActiveNetworkApplications = pids.count();
		m_Data.m_Statistics.m_nOpenPorts = ports;
		m_Data.m_Statistics.m_nActiveNetworkConnections = connections;
        m_Data.m_Statistics.m_nInBytes = nInBytes;
        m_Data.m_Statistics.m_nOutBytes = nOutBytes;

        m_Data.m_Statistics.m_nDroppedConnections = nDroppedConnections;

		if (p_statistics->isBasedOn(cAHFWStatistics::eIID))
			error = p_statistics->assign(m_Data.m_Statistics, true);
		else
			error = errOK;
	}

	if (PR_SUCC(error))
		PR_TRACE((this, prtSPAM, "fw\tget statistics result %terr", error));
	else
		PR_TRACE((this, prtIMPORTANT, "fw\tget statistics result %terr", error));
	
	return error;
}
// AVP Prague stamp end

tERROR CAHFW::UpdateStmSettings(cAHFWSettings* pSettings)
{
	cAHFWNetworks &Nets = pSettings->m_KnownNetworks;

	// find global net
	tBOOL bGlobalNetStealthed = pSettings->m_InetStealthed;
	tBOOL bIsAnyNetStealthed = cFALSE;
	for( tUINT i = 0; i < Nets.size(); i++ )
		bIsAnyNetStealthed |= Nets[i].m_bStealthed;

	tERROR error = errOK;
	if( m_StealthEnabled = (bIsAnyNetStealthed || bGlobalNetStealthed) )
	{
		error = AhErr2PrErr(CKAHSTM::Start());
		if( PR_SUCC(error) )
		{
			// delete all nets
			CKAHSTM::HSTMNETLIST hNetLst = NULL;
			CKAHUM::OpResult op = CKAHSTM::GetNetList(&hNetLst);
			if( op == CKAHUM::srOK )
			{
				int nSize;
				CKAHSTM::NetList_GetSize(hNetLst, &nSize);
				for(int i = 0; i < nSize; i++)
				{
					CKAHSTM::StmNet stmnet;
					CKAHSTM::NetList_GetItem(hNetLst, i, &stmnet);
					CKAHSTM::RemoveNet(&stmnet);
				}
				CKAHSTM::NetList_Delete(hNetLst);
			}

			// apply net to component
			CKAHSTM::SetNetDefaultReaction(!bGlobalNetStealthed);
			for( tUINT i = 0; i < Nets.size(); i++ )
			{
				if(Nets[i].m_bStealthed != bGlobalNetStealthed && 
                   Nets[i].m_IP.IsValid() && Nets[i].m_Mask.IsValid())
                {
                    CKAHSTM::StmNet stmnet;
                    stmnet.Net = cIP_CKAHUMIP(Nets[i].m_IP);
                    stmnet.Mask = cIP_CKAHUMIP(Nets[i].m_Mask);
                    CKAHSTM::AddNet(&stmnet);

                    PR_TRACE((this, prtIMPORTANT, "fw\tStealthNetwork. add net: %s/%s", cIPToA(Nets[i].m_IP), cIPMaskToA(Nets[i].m_Mask)));
                }
			}
			if(bGlobalNetStealthed)
			{
				cVector<cIP> aUnstealthedHosts;
				GetUnstealthedHosts(aUnstealthedHosts);
				for(tDWORD i = 0; i < aUnstealthedHosts.size(); i++)
				{
					CKAHSTM::StmNet stmnet;
					stmnet.Net = cIP_CKAHUMIP(aUnstealthedHosts[i]);
					if      (stmnet.Net.Isv4()) stmnet.Mask.Setv4Mask(32);
					else if (stmnet.Net.Isv6()) stmnet.Mask.Setv6Mask(128, 0);

					CKAHSTM::AddNet(&stmnet);

                    PR_TRACE((this, prtIMPORTANT, "fw\tUpdateStmSettings. add unstealthed ip = %s", CKAHUM::IPToA(stmnet.Net)));
				}
			}
			if( m_TaskState == TASK_STATE_RUNNING )
				error = AhErr2PrErr(CKAHSTM::Resume());
		}
	}
	else
		error = AhErr2PrErr(CKAHSTM::Stop());

	return error;
}

tERROR CAHFW::UpdateSettingsInTask(cAHFWSettings* pSettings, tBOOL* pbSettingsChanged)
{
    CKAHUM::OpResult opres = CKAHUM::srOK;
	PR_TRACE((this, prtIMPORTANT, "fw\tUpdateSettingsInTask request(protection level %d)", pSettings->m_ProtectionLevel));

	tERROR error = UpdateStmSettings(pSettings);

    opres = SetFilteringMode( pSettings->m_NdisCheckTdi ? fmMaxCompatibility : fmMaxSpeed );

    if ( opres == CKAHUM::srOK )
        PR_TRACE((this, prtNOTIFY, "fw\tSetFilteringMode  %d", pSettings->m_NdisCheckTdi));    
    else
        PR_TRACE((this, prtERROR, "fw\tunable to SetFilteringMode. opres = %d", opres));

	opres = SetWorkingMode((WorkingMode) pSettings->m_ProtectionLevel);

	if ( CKAHUM::srOK != opres )
	{
		error = errNOT_OK;
		PR_TRACE((this, prtIMPORTANT, "fw\tset settings: answer IDS  %d", opres));
	}
	else
	{
		error = errOBJECT_CANNOT_BE_INITIALIZED;

		HPACKETRULES rulports = PacketRules_Create();
		HAPPRULES apprules = ApplicationRules_Create();
		if (rulports && apprules)
		{
			tERROR ruleadd;
			//+ ------------------------------------------------------------------------------------------
			//+ fill rules

			cFwPacketRules NetRules;
			AddSpecialPacketRules(NetRules);
			Networks2Rules(NetRules, pSettings->m_KnownNetworks);
			{
				cAutoLockerCS Locker(m_SettingsCS);
				Networks2Rules(NetRules, m_AutoFoundedNetworks);
			}
			ruleadd = AddPortRules(rulports, NetRules);
			if (PR_SUCC(ruleadd))
				ruleadd = AddPortRules(rulports, pSettings->m_PortRules);
			
			if (PR_SUCC(ruleadd))
				ruleadd = AddAppRules(apprules, pSettings->m_AppRules, pSettings, pbSettingsChanged);

			//- end fill
			//- ------------------------------------------------------------------------------------------
			// if opres != CKAHUM::srOK -> create rules failed
			if (PR_SUCC(ruleadd))
			{
				opres = SetPacketRules(rulports);
				if (CKAHUM::srOK == opres)
					opres = SetApplicationRules(apprules);

				if (CKAHUM::srOK == opres)
					error = errOK;
			}
		}
		else
		{
			PR_TRACE((this, prtIMPORTANT, "fw\tcan't create Rules"));
		}

		if (rulports)
			PacketRules_Delete(rulports);
		if (apprules)
			ApplicationRules_Delete(apprules);
	}

	return error;
}


//////////////////////////////////////////////////////////////////////////
// AddPortRules

tERROR CAHFW::AddPortRules(HPACKETRULES hRules, cFwPacketRules &Rules)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;

	PR_TRACE((this, prtIMPORTANT, "fw\tport rules count %d", Rules.size()));
	
	for(tUINT i = 0; i < Rules.size() && opres == CKAHUM::srOK; i++)
	{
		cFwPacketRule &Rule = Rules[i];
		
		HPACKETRULE hRule = PacketRule_Create();
		if(hRule)
		{
			if(opres == CKAHUM::srOK)
				opres = PacketRule_SetName(hRule, Rule.m_sName.data());
			
			if(opres == CKAHUM::srOK)
				opres = PacketRule_SetIsEnabled(hRule,  !!Rule.m_bEnabled);
			
			if(opres == CKAHUM::srOK)
			   opres = PacketRule_SetIsBlocking(hRule, !!Rule.m_bBlocking);
			
			if(opres == CKAHUM::srOK)
				opres = PacketRule_SetIsNotify(hRule, !!Rule.m_bWarning);
			
			if(opres == CKAHUM::srOK)
				opres = PacketRule_SetIsLog(hRule, !!Rule.m_bLog);
			
			if(opres == CKAHUM::srOK)
			{
				PacketDirection pd;
				StreamDirection sd;
				switch(Rule.m_eDirection)
				{
				case _fwInbound:         pd = pdIncoming; sd = sdBoth;     break;
				case _fwOutbound:        pd = pdOutgoing; sd = sdBoth;     break;
				case _fwInboundOutbound: pd = pdBoth;     sd = sdBoth;     break;
				case _fwInboundStream:   pd = pdBoth;     sd = sdIncoming; break;
				case _fwOutboundStream:  pd = pdBoth;     sd = sdOutgoing; break;
				default:
					opres = CKAHUM::srOpFailed;
				}
				if(opres == CKAHUM::srOK)
					opres = PacketRule_SetPacketDirection(hRule, pd);
				
				if(opres == CKAHUM::srOK)
					opres = PacketRule_SetStreamDirection(hRule, sd);
			}
			
			if(opres == CKAHUM::srOK)
				opres = PacketRule_SetProtocol(hRule, Rule.m_nProto);
			
			if(opres == CKAHUM::srOK && (Rule.m_nTimeFrom || Rule.m_nTimeTill))
			{
				HRULETIMES hRuleTimes;
				opres = GenerateTimeRange(&Rule, &hRuleTimes);
				if(CKAHUM::srOK == opres)
				{
					opres = PacketRule_SetTimes(hRule, hRuleTimes);
					RuleTimes_Delete(hRuleTimes);
				}
			}
			
			if(opres == CKAHUM::srOK)
			{
				HRULEADDRESSES hRuleAddresses;
				opres = GenerateAddrList(&Rule.m_aRemoteAddresses, &hRuleAddresses, m_Data.m_IPResolver);
				if(opres == CKAHUM::srOK && hRuleAddresses)
				{
					opres = PacketRule_SetRemoteAddresses(hRule, hRuleAddresses);
					RuleAddresses_Delete(hRuleAddresses);
				}
			}

			if(opres == CKAHUM::srOK)
			{
				HRULEADDRESSES hRuleAddresses;
				opres = GenerateAddrList(&Rule.m_aLocalAddresses, &hRuleAddresses, m_Data.m_IPResolver);
				if(opres == CKAHUM::srOK && hRuleAddresses)
				{
					opres = PacketRule_SetLocalAddresses(hRule, hRuleAddresses);
					RuleAddresses_Delete(hRuleAddresses);
				}
			}
			
			if((_fwPROTO_TCP == Rule.m_nProto) || (_fwPROTO_UDP == Rule.m_nProto))
			{
				if(opres == CKAHUM::srOK)
				{
					HRULEPORTS hRulePorts;
					opres = GeneratePortList(&Rule.m_aRemotePorts, &hRulePorts);
					if(opres == CKAHUM::srOK && hRulePorts)
					{
						opres = PacketRule_SetTCPUDPRemotePorts(hRule, hRulePorts);
						RulePorts_Delete(hRulePorts);
					}
				}

				if(opres == CKAHUM::srOK)
				{
					HRULEPORTS hRulePorts;
					opres = GeneratePortList(&Rule.m_aLocalPorts, &hRulePorts);
					if(opres == CKAHUM::srOK && hRulePorts)
					{
						opres = PacketRule_SetTCPUDPLocalPorts(hRule, hRulePorts);
						RulePorts_Delete(hRulePorts);
					}
				}
			}
			else
			if((PROTO_ICMP == Rule.m_nProto) || (PROTO_ICMPV6 == Rule.m_nProto))
			{
				if(opres == CKAHUM::srOK)
					opres = PacketRule_SetICMPType(hRule, Rule.m_nIcmpCode);

                /*
				if(opres == CKAHUM::srOK)
					opres = PacketRule_SetICMPCode(hRule, 0);
                */
			}
			
			if (CKAHUM::srOK == opres)
				opres = PacketRules_AddItemToBack(hRules, hRule);

			PacketRule_Delete(hRule);

			PR_TRACE((this, prtIMPORTANT, "fw\tpacket rule name %S, opres = %d", Rule.m_sName.data(), opres));

			opres = CKAHUM::srOK;
		}
		else
			opres = CKAHUM::srOpFailed;
	}

	return opres == CKAHUM::srOK ? errOK : errUNEXPECTED;
}

//////////////////////////////////////////////////////////////////////////
// AddAppRules

tERROR CAHFW::AddAppRules(HAPPRULES hRules, cFwAppRules &Rules, cAHFWSettings* pSettings, tBOOL* pbSettingsChanged)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;

	PR_TRACE((this, prtIMPORTANT, "fw\tapp rules count %d", Rules.size()));
	
	ULONG DrvHashFuncVersion = 0;
	GetApplicationChecksumVersion(&DrvHashFuncVersion);

	for(tUINT i = 0; i < Rules.size() && opres == CKAHUM::srOK; i++)
	{
		cFwAppRule &AppRule = Rules[i];
		
		for(tUINT j = 0; j < AppRule.m_aRules.size() && opres == CKAHUM::srOK; j++)
		{
			cFwBaseRule &Rule = AppRule.m_aRules[j];

			HAPPRULE hRule = ApplicationRule_Create();
			if(hRule)
			{
				cStrObj AppNameTmp(AppRule.m_sAppName);
				sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (AppNameTmp), 0, 0);
				
				if(opres == CKAHUM::srOK)
					opres = ApplicationRule_SetName(hRule, Rule.m_sName.data());
				
				if(opres == CKAHUM::srOK)
					opres = ApplicationRule_SetApplicationName(hRule, AppNameTmp.data());

				PR_TRACE((this, prtIMPORTANT, "fw\tapp rule name %S, app %S (was %S)", Rule.m_sName.data(), 
					AppNameTmp.data(), AppRule.m_sAppName.data()));

				if(opres == CKAHUM::srOK)
				{
					opres = ApplicationRule_SetIsCommandLine(hRule, !!AppRule.m_bUseCmdLine);
					if(opres == CKAHUM::srOK && AppRule.m_bUseCmdLine)
						opres = ApplicationRule_SetCommandLine(hRule, AppRule.m_sCmdLine.data());
				}
				
				if(opres == CKAHUM::srOK)
				{
					tBYTE zerohash[FW_HASH_SIZE];
					memset(zerohash, 0, sizeof(zerohash));
					if (!memcmp(AppRule.m_nHash, zerohash, FW_HASH_SIZE))
					{
						PR_TRACE((this, prtIMPORTANT, "fw\tno hash. skipping"));
					}
					else
					{
						if (pSettings->m_HashFuncVersion != DrvHashFuncVersion)
						{
							PR_TRACE((this, prtIMPORTANT, "fw\thash func different: old 0x%x, new 0x%x", 
								pSettings->m_HashFuncVersion, DrvHashFuncVersion));

							tDWORD nHashSize = sizeof(AppRule.m_nHash);
							if (!PR_SUCC(FillApplicationChecksum(&AppRule.m_sAppName, AppRule.m_nHash, &nHashSize)))
							{
								memset(AppRule.m_nHash, 0, sizeof(AppRule.m_nHash));
								PR_TRACE((this, prtIMPORTANT, "fw\trecalc hash failed - set zero"));
							}
						}

						PR_TRACE((this, prtNOTIFY, "fw\tapp checksum 0x%x...0x%x", AppRule.m_nHash[0], AppRule.m_nHash[FW_HASH_SIZE - 1]));
						opres = ApplicationRule_SetApplicationChecksum(hRule, AppRule.m_nHash);	// check that size greater than FW_HASH_SIZE
					}
				}

                if(opres == CKAHUM::srOK)
                    opres = ApplicationRule_SetCheckHash(hRule, !!AppRule.m_bCheckHash);

				if(opres == CKAHUM::srOK)
					opres = ApplicationRule_SetIsEnabled(hRule, !!Rule.m_bEnabled);
				
				if(opres == CKAHUM::srOK)
					opres = ApplicationRule_SetIsBlocking(hRule, !!Rule.m_bBlocking);
				
				if(opres == CKAHUM::srOK)
					opres = ApplicationRule_SetIsNotify(hRule, !!Rule.m_bWarning);
				
				if(opres == CKAHUM::srOK)
				   opres = ApplicationRule_SetIsLog(hRule, !!Rule.m_bLog);
				
				if(opres == CKAHUM::srOK && (Rule.m_nTimeFrom || Rule.m_nTimeTill))
				{
					HRULETIMES hRuleTimes;
					opres = GenerateTimeRange(&Rule, &hRuleTimes);
					if(CKAHUM::srOK == opres)
					{
						opres = ApplicationRule_SetTimes(hRule, hRuleTimes);
						RuleTimes_Delete(hRuleTimes);
					}
				}
				
				if(opres == CKAHUM::srOK)
				{
					HAPPRULEELEMENT hRuleElement = ApplicationRuleElement_Create();
					if (hRuleElement)
					{
						opres = GenerateElement(&Rule, hRuleElement, m_Data.m_IPResolver);
						if (CKAHUM::srOK == opres)
						{
							opres = ApplicationRule_AddElementToBack(hRule, hRuleElement);
							ApplicationRuleElement_Delete(hRuleElement);
						}
					}
				}

				if(opres == CKAHUM::srOK)
					opres = ApplicationRules_AddItemToBack(hRules, hRule);
				
				ApplicationRule_Delete(hRule);
			}
			else
				opres = CKAHUM::srOpFailed;
		}
	}

	opres == CKAHUM::srOK ? errOK : errUNEXPECTED;
	
	if (CKAHUM::srOK == opres)
	{
		if (pSettings->m_HashFuncVersion != DrvHashFuncVersion)
		{
			pSettings->m_HashFuncVersion = DrvHashFuncVersion;
			if (pbSettingsChanged)
				*pbSettingsChanged = TRUE;
		}
	}

	return opres;
}

//////////////////////////////////////////////////////////////////////////
// PrependAppRule

tERROR CAHFW::PrependAppRule(cFwAppRule &Rule)
{
	if( !Rule.m_aRules.size() )
		return errOK;

	cAHFWSettings* pSettings = LockSettingsInTM(cFALSE);
	if(pSettings)
	{
		tBOOL bSettingsChanged = cFALSE;
		PrependAppRule(pSettings, Rule, bSettingsChanged);
		UnlockSettingsInTM(pSettings, bSettingsChanged);
	}

	return errOK;
}

void CAHFW::PrependAppRule(cAHFWSettings *pSettings, cFwAppRule &Rule, tBOOL &bSettingsChanged)
{
	if( !Rule.m_aRules.size() )
		return;

	tBOOL bChanged = cFALSE;

	if( Rule.IsHashEmpty() )
	{
		tDWORD nHashSize = sizeof(Rule.m_nHash);
		FillApplicationChecksum(&Rule.m_sAppName, Rule.m_nHash, &nHashSize);
	}

	cFwAppRule *pExistRule = pSettings->FindAppRule(Rule.m_sAppName, Rule.m_nHash, Rule.m_sCmdLine, Rule.m_bUseCmdLine);
	if( pExistRule )
	{
		if( !(pExistRule->m_aRules == Rule.m_aRules) )
		{
			pExistRule->InsertRules(Rule.m_aRules, 0);
			bChanged = cTRUE;
		}
	}
	else
	{
		pSettings->m_AppRules.push_back(Rule);
		bChanged = cTRUE;
	}

	if( bChanged )
	{
		HAPPRULES hRules = ApplicationRules_Create();
		if(hRules)
		{
			AddAppRules(hRules, pSettings->m_AppRules, pSettings, &bSettingsChanged);

			SetApplicationRules(hRules);

			ApplicationRules_Delete(hRules);
		}
	}

	if( bChanged )
		bSettingsChanged = bChanged;
}

//////////////////////////////////////////////////////////////////////////
// AppendAppRule

tERROR CAHFW::AppendAppRule(cFwAppRule &Rule)
{
	if( !Rule.m_aRules.size() )
		return errOK;

	cAHFWSettings* pSettings = LockSettingsInTM(cFALSE);
	if(pSettings)
	{
		tBOOL bSettingsChanged = cFALSE;
		AppendAppRule(pSettings, Rule, bSettingsChanged);
		UnlockSettingsInTM(pSettings, bSettingsChanged);
	}

	return errOK;
}

void CAHFW::AppendAppRule(cAHFWSettings *pSettings, cFwAppRule &Rule, tBOOL &bSettingsChanged)
{
	if( !Rule.m_aRules.size() )
		return;

	tBOOL bChanged = cFALSE;

	if( Rule.IsHashEmpty() )
	{
		tDWORD nHashSize = sizeof(Rule.m_nHash);
		FillApplicationChecksum(&Rule.m_sAppName, Rule.m_nHash, &nHashSize);
	}

	cFwAppRule *pExistRule = pSettings->FindAppRule(Rule.m_sAppName, Rule.m_nHash, Rule.m_sCmdLine, Rule.m_bUseCmdLine);
	if( pExistRule )
	{
		if( !(pExistRule->m_aRules == Rule.m_aRules) )
		{
			pExistRule->AppendRules(Rule.m_aRules);
			bChanged = cTRUE;
		}
	}
	else
	{
		pSettings->m_AppRules.push_back(Rule);
		bChanged = cTRUE;
	}

	if( bChanged )
	{
		HAPPRULES hRules = ApplicationRules_Create();
		if(hRules)
		{
			AddAppRules(hRules, pSettings->m_AppRules, pSettings, &bSettingsChanged);

			SetApplicationRules(hRules);

			ApplicationRules_Delete(hRules);
		}
	}

	if( bChanged )
		bSettingsChanged = bChanged;
}

//////////////////////////////////////////////////////////////////////////
// AppendNetwork

tERROR CAHFW::UpdatePacketRules(cAHFWSettings *pSettings)
{
	bool bNeedUnlock = !pSettings;
	if( !pSettings )
		pSettings = LockSettingsInTM(cFALSE);

	if( pSettings )
	{
		HPACKETRULES hRules = PacketRules_Create();
		if(hRules)
		{
			UpdateStmSettings(pSettings);
			
			cFwPacketRules aNetRules;
			AddSpecialPacketRules(aNetRules);
			Networks2Rules(aNetRules, pSettings->m_KnownNetworks);
			{
				cAutoLockerCS Locker(m_SettingsCS);
				Networks2Rules(aNetRules, m_AutoFoundedNetworks);
			}
			AddPortRules(hRules, aNetRules);
			AddPortRules(hRules, pSettings->m_PortRules);
			
			SetPacketRules(hRules);
			PacketRules_Delete(hRules);
		}
		
		if( bNeedUnlock )
			UnlockSettingsInTM(pSettings, cFALSE);
	}
	
	return errOK;
}

tERROR CAHFW::AddSpecialPacketRules(cFwPacketRules &aRules)
{
	cAutoLockerCS Locker(m_SettingsCS);
	
	if( m_aRulesForNagent.empty() )
	{
		cAHFWSettings Sett;
		if( sysSendMsg(pmc_TASKAHFW_GET_NAGENT_SETTINGS, 0, NULL, &Sett, SER_SENDMSG_PSIZE) == errOK_DECIDED )
			m_aRulesForNagent = Sett.m_PortRules;
	}
	
	PR_TRACE((this, prtIMPORTANT, "fw\tCAHFW::AddSpecialPacketRules. add rules for nagent. count=%d", m_aRulesForNagent.size()));
	
	for(tUINT i = 0; i < m_aRulesForNagent.size(); i++)
		aRules.push_back(m_aRulesForNagent[i]);

	return errOK;
}

tERROR CAHFW::Networks2Rules(cFwPacketRules &Rules, cAHFWNetworks &KnownNetworks)
{
	if( KnownNetworks.empty() )
		return errOK;
	
	for(tUINT i = 0; i < KnownNetworks.size(); i++)
	{
        if (!KnownNetworks[i].m_bHidden)
        {
            cVector<cFWAddress> aAddrs;
            cFWAddress &Addr = aAddrs.push_back();
            Addr.m_IPLo = Addr.m_IPHi = KnownNetworks[i].m_IP;
            Addr.m_IPMask = KnownNetworks[i].m_Mask;
        	
            switch(KnownNetworks[i].m_eZone)
            {
            case _fwnzTrusted:
	            {
		            cFwPacketRule &Rule = Rules.push_back();
		            Rule.m_sName = "Trusted";
		            Rule.m_nProto = _fwPROTO_ALL;
		            Rule.m_bBlocking = cFALSE;
		            Rule.m_eDirection = _fwInboundOutbound;
		            Rule.m_aRemoteAddresses = aAddrs;
	            }
	            break;

            case _fwnzNetBIOS:
	            {
		            cFWPort *pPort;
		            cFwPacketRule *pRule;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow DCOM (local port)";
		            pRule->m_nProto = _fwPROTO_TCP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundStream;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aLocalPorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 135;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow Windows NetBIOS name service (local port)";
		            pRule->m_nProto = _fwPROTO_UDP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundOutbound;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aLocalPorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 137;

		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow Windows NetBIOS datagram service (local port)";
		            pRule->m_nProto = _fwPROTO_UDP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundOutbound;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aLocalPorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 138;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow Windows NetBIOS session service (local port)";
		            pRule->m_nProto = _fwPROTO_TCP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundStream;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aLocalPorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 139;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow Windows NetBIOS session service (remote port)";
		            pRule->m_nProto = _fwPROTO_TCP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwOutboundStream;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aRemotePorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 139;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow Windows NetBIOS session service (local port)";
		            pRule->m_nProto = _fwPROTO_TCP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundStream;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aLocalPorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 445;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow Windows NetBIOS session service (remote port)";
		            pRule->m_nProto = _fwPROTO_TCP;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwOutboundStream;
		            pRule->m_aRemoteAddresses = aAddrs;
		            pPort = &pRule->m_aRemotePorts.push_back();
		            pPort->m_PortLo = pPort->m_PortHi = 445;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow any ICMP activity";
		            pRule->m_nProto = _fwPROTO_ICMP;
		            pRule->m_nIcmpCode = 0xff;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundOutbound;
		            pRule->m_aRemoteAddresses = aAddrs;
        			
		            pRule = &Rules.push_back();
		            pRule->m_sName = "Allow any ICMPv6 activity";
		            pRule->m_nProto = _fwPROTO_ICMPV6;
		            pRule->m_nIcmpCode = 0xff;
		            pRule->m_bBlocking = cFALSE;
		            pRule->m_eDirection = _fwInboundOutbound;
		            pRule->m_aRemoteAddresses = aAddrs;
	            }
	            break;
            }
            // return errOK;
        }
	}
	return errOK;
}

tERROR CAHFW::NotifyAppRuleAdded(cFwAppRule &Rule)
{
	cAHFWAppEventReport Report;
//	Report.m_New = cTRUE;
//	Report.m_Time = time(NULL);
//	Report.m_AppName = Rule.m_sAppName;
//	Report.m_CmdLine = Rule.m_sCmdLine;
//	Report.m_PID = 0;	
//	Report.m_nSessionId = 0;
//	Report.m_strTaskType.assign(_task_name, cCP_UNICODE);
//	Report.m_RuleName = Rule.m_sName;
//	Report.m_IsBlocking = Rule.m_bBlocking;
//	Report.m_Direction = Rule.m_eDirection;
//	Report.m_Proto = Rule.m_nProto;
//	Report.m_Log = Rule.m_bLog;
//	Report.m_Warning = Rule.m_bWarning;

//	return sysSendMsg(pmc_TASKAHFW_REPORT_EVENT, 0, NULL, &Report, SER_SENDMSG_PSIZE);
	return errOK;
}

//////////////////////////////////////////////////////////////////////////
// CallbackEnter & CallbackLeave & CallbackIsAnyActive

bool CAHFW::CallbackIsAnyActive()
{
	m_Data.m_hSync->Enter(SHARE_LEVEL_WRITE);
	bool bActive = !!m_Data.m_CallbackCounter;
	m_Data.m_hSync->Leave(NULL);
	
	return bActive;
}

void CAHFW::CallbackEnter()
{
	m_Data.m_hSync->Enter(SHARE_LEVEL_WRITE);
	m_Data.m_CallbackCounter++;
	m_Data.m_hSync->Leave(NULL);
}

void CAHFW::CallbackLeave()
{
	m_Data.m_hSync->Enter(SHARE_LEVEL_WRITE);
	if(m_Data.m_CallbackCounter)
		m_Data.m_CallbackCounter--;
	m_Data.m_hSync->Leave(NULL);
}

void CAHFW::ChangeSecurityState(bool bFWOn)
{
	cStrObj str("%ProductName%");
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (str), 0, 0);

	PR_TRACE((g_root, prtIMPORTANT, "AH\tchange state %s- %s-%s-%s", bFWOn ? "on" : "off",
		VER_COMPANYNAME_STR, VER_PRODUCTVERSION_STR, (tCHAR*) str.c_str(cCP_ANSI)));
}


//////////////////////////////////////////////////////////////////////////
// cbNetworkChangesLink

void CALLBACK CAHFW::cbNetworkChangesLink(LPVOID lpContext)
{
	((CAHFW *)lpContext)->OnNetworkChanges();
}

void CAHFW::OnNetworkChanges()
{
	if( m_bZoneLocked )
		return;

	//if( m_TaskState != TASK_STATE_RUNNING )
	//	return;

	CallbackEnter();

	{
		cAutoLockerCS Locker(m_SettingsCS);
		m_AutoFoundedNetworks.clear();
	}

	cAHFWNetworks NewNetworks; GetNewNetworks(NewNetworks);
	for(tUINT i = 0; i < NewNetworks.size(); i++)
	{
		cAskActionFwNetChanged AskAction;
		AskAction.m_strTaskType.assign(_task_name, cCP_UNICODE);
		AskAction.m_nObjectType = OBJTYPE_NETWORK;
		AskAction.m_nDefaultAction = ACTION_OK;
		AskAction.m_nResultAction = ACTION_OK;
		AskAction.m_nActionsAll  = ACTION_OK;
		AskAction.m_nActionsMask = ACTION_OK;
		AskAction.m_nDetectDanger = DETDANGER_INFORMATIONAL;
		AskAction.m_nDetectStatus = DSTATUS_SURE;
		AskAction.m_nDetectType = DETYPE_UNKNOWN;
		AskAction.m_NewNetwork = NewNetworks[i];
		AskAction.m_bSystemCritical = cTRUE;


		cAHFWNetwork &net = AskAction.m_NewNetwork;
        PR_TRACE((this, prtIMPORTANT, "fw\tOnNetworkChanges. New network found: description: %S, net: %s/%s, mac: %I64x",
            net.m_sAdapterDesc.data(), cIPToA(net.m_IP), cIPMaskToA(net.m_Mask),
            net.m_nMac
			));
		
		tERROR err = sysSendMsg(pmcASK_ACTION, cAskActionFwNetChanged::NETWORK_CHANGED, NULL, &AskAction, SER_SENDMSG_PSIZE);
		
		PR_TRACE((this, prtIMPORTANT, "fw\tOnNetworkChanges. GUI return %terr (result action %d)", err, AskAction.m_nResultAction));

		if( m_bZoneLocked )
			break;
	
		if( err == errOK_NO_DECIDERS )
		{
			PR_TRACE((this, prtIMPORTANT, "fw\tOnNetworkChanges. no GUI connected, postponding question"));
			{
				cAutoLockerCS Locker(m_SettingsCS);
				m_AutoFoundedNetworks.push_back(AskAction.m_NewNetwork);
			}
			
			UpdatePacketRules();
		}
		else
		if( AskAction.m_nResultAction == ACTION_OK )
		{
            bool bUpdated = false;

			cAHFWSettings* pSettings = LockSettingsInTM(cFALSE);
			if( pSettings )
			{
				cAHFWNetworks &Networks = pSettings->m_KnownNetworks;
                tUINT j = AHFWNetworks_FindNetwork(Networks, AskAction.m_NewNetwork);
                if (j != Networks.size())
                {
                    bUpdated = true;
                    AHFWNetworks_ChangeNetwork(Networks, j, AskAction.m_NewNetwork);
                    PR_TRACE((this, prtIMPORTANT, "fw\tOnNetworkChanges. changing settings for mac:%I64x, subnet:%s/%s, %s", AskAction.m_NewNetwork.m_nMac, cIPToA(Networks[j].m_IP), cIPMaskToA(Networks[j].m_Mask), Networks[j].m_sAdapterDesc.data()));
                }
                else
                {
                    PR_TRACE((this, prtIMPORTANT, "fw\tOnNetworkChanges. network not found for mac:%I64x", AskAction.m_NewNetwork.m_nMac));
                }

				UnlockSettingsInTM(pSettings, bUpdated);
			}

			if( bUpdated )
				UpdatePacketRules();
		}
	}

	CallbackLeave();
}

tERROR CAHFW::GetUnstealthedHosts(cVector<cIP> &aUnstealthedHosts)
{
	return GetNetCfg(&aUnstealthedHosts, &aUnstealthedHosts, &aUnstealthedHosts);
}

void CAHFW::GetNewNetworks(cAHFWNetworks &aNewNetworks)
{
	cAHFWSettings* pSettings = LockSettingsInTM(cFALSE);
	if( !pSettings )
		return;

    tBOOL bNetUpdated = AHFWNetworks_UpdateNetworks(pSettings->m_KnownNetworks, aNewNetworks);

	if( bNetUpdated )
		UpdatePacketRules();

	UnlockSettingsInTM(pSettings, bNetUpdated);
}

//////////////////////////////////////////////////////////////////////////
// OnPacketRuleNotify

void CALLBACK CAHFW::cbPacketRuleNotifyLink(LPVOID lpContext, const PacketRuleNotify *pNotify)
{
	((CAHFW *)lpContext)->OnPacketRuleNotify(pNotify);
}

void CAHFW::OnPacketRuleNotify(const PacketRuleNotify *pNotify)
{
	if(m_TaskState != TASK_STATE_RUNNING)
	{
		PR_TRACE((0, prtERROR, "fw\tOnPacketRuleNotify. task isn't running"));
		return;
	}
	if(!pNotify)
	{
		PR_TRACE((0, prtERROR, "fw\tOnPacketRuleNotify. bad data pNotify is null"));
		return;
	}

	// trace notification event
	{
		PR_TRACE((this, prtIMPORTANT, "fw\tOnPacketRuleNotify. rule %S, block: %d, protocol: %d, direction: %s (%s), local addr: %s:%d remote addr: %s:%d",
			pNotify->RuleName,
			pNotify->IsBlocking,
			pNotify->Proto,
			pNotify->IsIncoming ? "incoming" : "outgoing",
			pNotify->IsStream ? (pNotify->IsStreamIncoming ? "incoming stream" : "outgoing stream" ) : "no streams",
			CKAHUM_IPTOA(pNotify->LocalAddress), pNotify->TCPUDPLocalPort,
			CKAHUM_IPTOA(pNotify->LocalAddress), pNotify->TCPUDPRemotePort
			));
	}

	CallbackEnter();
	
	m_Data.m_CommonStat.m_PacketsProceed++;

	cAHFWPacketEventReport Report;
	Report.m_Time = time(NULL);
	Report.m_strTaskType.assign(_task_name, cCP_UNICODE);
	Report.m_RuleName.assign(pNotify->RuleName, cCP_UNICODE);
	Report.m_IsBlocking = pNotify->IsBlocking;
	Report.m_Direction = pNotify->IsIncoming ? _fwInbound : _fwOutbound;
	Report.m_Proto = pNotify->Proto;
	Report.m_RemoteAddress = CKAHUMIP_cIP(pNotify->RemoteAddress);
	Report.m_LocalAddress = CKAHUMIP_cIP(pNotify->LocalAddress);
	Report.m_RemotePort = pNotify->TCPUDPRemotePort;
	Report.m_LocalPort = pNotify->TCPUDPLocalPort;
	Report.m_IcmpCode = pNotify->ICMPType;
	Report.m_Log = pNotify->Log;
	Report.m_Warning = pNotify->Notify;

	if(Report.m_Log)
		sysSendMsg(pmc_TASKAHFW_REPORT_EVENT, 0, NULL, &Report, SER_SENDMSG_PSIZE);
	
	if(Report.m_Warning)
		sysSendMsg(pmc_TASKAHFW_WARNING_EVENT, 0, NULL, &Report, SER_SENDMSG_PSIZE);

	CallbackLeave();
}


//////////////////////////////////////////////////////////////////////////
// OnAppRuleNotify

static int GetLongPathNameEx(char *From, char *To)
{
	memcpy(To, From, 3);
	To[3] = 0;
	
	char *pFromLo = &From[3];
	char *pTo = &To[3];
	char *pFromHi;
	
	for(; pFromHi = strchr(pFromLo, '\\'); pFromLo = pFromHi)
	{
		pFromHi++;
		
		int nLen = pFromHi - pFromLo - 1;
		memcpy(pTo, pFromLo, nLen);
		*(pTo + nLen) = 0;
		
		WIN32_FIND_DATA wfd = {0, };
		HANDLE hFind = FindFirstFile(To, &wfd);
		if(hFind == INVALID_HANDLE_VALUE)
			return 0;
		FindClose(hFind);
		
		strcpy(pTo, wfd.cFileName);
		
		pTo += strlen(pTo);
		*pTo++ = '\\';
		*pTo = 0;
	}
	strcpy(pTo, pFromLo);
	
	return 1;
}

RuleCallbackResult CALLBACK CAHFW::cbAppRuleNotifyLink(LPVOID lpContext, const ApplicationRuleNotify *pNotify)
{
	return ((CAHFW *)lpContext)->OnAppRuleNotify(pNotify);
}

RuleCallbackResult CAHFW::OnAppRuleNotify(const ApplicationRuleNotify *pNotify)
{
	RuleCallbackResult ruleres = crPass;
	
	if(m_TaskState != TASK_STATE_RUNNING)
	{
		PR_TRACE((0, prtERROR, "fw\tOnAppRuleNotify. task isn't running"));
		return ruleres;
	}
	if(!pNotify)
	{
		PR_TRACE((0, prtERROR, "fw\tOnAppRuleNotify. bad data pNotify is null"));
		return ruleres;
	}

	// trace notification event
	{
		BYTE* pra = (BYTE*) &pNotify->RemoteAddress;
		BYTE* pla = (BYTE*) &pNotify->LocalAddress;
		PR_TRACE((this, prtIMPORTANT, "fw\tOnAppRuleNotify. %s, app: %S (pid %d), block: %d, protocol: %d, direction: %s (%s), local addr: %d.%d.%d.%d:%d remote addr: %d.%d.%d.%d:%d",
			pNotify->Popup ? "learning" : "notify",
			pNotify->ApplicationName,
			pNotify->PID,
			pNotify->IsBlocking,
			pNotify->Proto,
			pNotify->IsIncoming ? "incoming" : "outgoing",
			pNotify->IsStream ? (pNotify->IsStreamIncoming ? "incoming stream" : "outgoing stream" ) : "no streams",
			pla[3], pla[2], pla[1], pla[0], pNotify->LocalPort,
			pra[3], pra[2], pra[1], pra[0], pNotify->RemotePort
			));
	}

	if( m_bAppLocked && pNotify->Popup )
		return ruleres;

	CallbackEnter();

	m_Data.m_CommonStat.m_AppProceed++;

	cAskActionFwLearning AskAction;
	AskAction.m_nSessionId = GetSessionIDForProcess(pNotify->PID);
	AskAction.m_strTaskType.assign(_task_name, cCP_UNICODE);
	AskAction.m_nObjectType = OBJTYPE_NETWORK;
	AskAction.m_nDefaultAction = ACTION_ALLOW;
	AskAction.m_nResultAction = ACTION_ALLOW;
	AskAction.m_nActionsAll  = ACTION_ALLOW|ACTION_DENY;
	AskAction.m_nActionsMask = ACTION_ALLOW|ACTION_DENY;
	AskAction.m_nDetectDanger = DETDANGER_INFORMATIONAL;
	AskAction.m_nDetectStatus = DSTATUS_SURE;
	AskAction.m_nDetectType = DETYPE_UNKNOWN;
	AskAction.m_bSystemCritical = cTRUE;
	
	cAHFWAppEventReport& Report = AskAction.m_AppEvent;
	Report.m_Time = time(NULL);
//	Report.m_AppName.assign(pNotify->ApplicationName, cCP_UNICODE);
	Report.m_CmdLine.assign(pNotify->CommandLine, cCP_UNICODE);
	Report.m_PID = pNotify->PID;	
	Report.m_nSessionId = GetSessionIDForProcess(Report.m_PID);
	Report.m_strTaskType.assign(_task_name, cCP_UNICODE);
	Report.m_RuleName.assign(pNotify->RuleName, cCP_UNICODE);
	Report.m_IsBlocking = pNotify->IsBlocking;
	Report.m_Direction = pNotify->IsIncoming ? _fwInboundStream : _fwOutboundStream;
	Report.m_Proto = (pNotify->Proto == _fwPROTO_TCP) ? _fwPROTO_TCP : _fwPROTO_UDP;
	Report.m_RemoteAddress = CKAHUMIP_cIP(pNotify->RemoteAddress);
	Report.m_LocalAddress = CKAHUMIP_cIP(pNotify->LocalAddress);
	Report.m_RemotePort = pNotify->RemotePort;
	Report.m_LocalPort = pNotify->LocalPort;
	Report.m_Log = pNotify->Log;
	Report.m_Warning = pNotify->Notify;
	Report.m_App = cTRUE;


//	convert 2 long names

	typedef DWORD (__stdcall *tpGetLongPathNameW)(LPCWSTR lpszShortPath, LPWSTR lpszLongPath, DWORD cchBuffer);
	tpGetLongPathNameW pGetLongPathNameW = (tpGetLongPathNameW)::GetProcAddress(::GetModuleHandleA("kernel32.dll"), "GetLongPathNameW");
	if( pGetLongPathNameW )
	{
		wchar_t strLongPathBuff[5 * MAX_PATH];
		const wchar_t * strLongPath = pNotify->ApplicationName;

		if( pGetLongPathNameW(pNotify->ApplicationName, strLongPathBuff, countof(strLongPathBuff)) )
			strLongPath = strLongPathBuff;

		Report.m_AppName.assign(strLongPath, cCP_UNICODE);
	}
	else
	{
		Report.m_AppName.assign(pNotify->ApplicationName, cCP_UNICODE);

		char strLongPathBuff[5 * MAX_PATH];
		if( GetLongPathNameEx(Report.m_AppName.c_str(cCP_ANSI), strLongPathBuff) )
			Report.m_AppName = strLongPathBuff;
	}


	if(pNotify->Popup)
	{
		cAHFWSettings* pSettings = LockSettingsInTM(cTRUE);
		if(pSettings)
		{
			cFwAppRule *pExistRule = pSettings->FindAppRule(Report.m_AppName, NULL, cStringObj(), cFALSE);
			if( pExistRule )
			{
				Report.m_AppRule = *pExistRule;
			}
			else
			{
				Report.m_AppRule.m_sAppName = Report.m_AppName;
//				Report.m_AppRule.m_sCmdLine = Report.m_CmdLine;
//				Report.m_AppRule.m_bUseCmdLine = !Report.m_CmdLine.empty();
			}
			UnlockSettingsInTM(pSettings, cFALSE);
		}
	
		tERROR error = sysSendMsg(pmcASK_ACTION, cAskActionFwLearning::LEARNING, NULL, &AskAction, SER_SENDMSG_PSIZE);
		PR_TRACE((this, prtIMPORTANT, "fw\tOnAppRuleNotify. GUI answer: 0x%x for app %S (error : %terr)", AskAction.m_nResultAction, pNotify->ApplicationName, error));
		
		switch(AskAction.m_nResultAction)
		{
		case ACTION_ALLOW: ruleres = crPass;  break;
		default:           ruleres = crBlock; break;
		}

		if( error == errOK_DECIDED && !Report.m_AppRule.m_aRules.empty() )
		{
			AppendAppRule(Report.m_AppRule);
			NotifyAppRuleAdded(Report.m_AppRule);
		}
	}
	else
	{
		if(pNotify->IsBlocking)
		{
			ruleres = crBlock;
			PR_TRACE((this, prtIMPORTANT, "fw\tOnAppRuleNotify. Not popup (blocking) rule"));
		}
	}

	if(Report.m_Log)
		sysSendMsg(pmc_TASKAHFW_REPORT_EVENT, 0, NULL, &Report, SER_SENDMSG_PSIZE);

	if(Report.m_Warning)
		sysSendMsg(pmc_TASKAHFW_WARNING_EVENT, 0, NULL, &Report, SER_SENDMSG_PSIZE);
	
	CallbackLeave();
	
	return ruleres;
}

//////////////////////////////////////////////////////////////////////////
// OnCheckSumNotify

RuleCallbackResult CALLBACK CAHFW::cbCheckSumNotifyLink(LPVOID lpContext, const ChecksumChangedNotify *pNotify)
{
	return ((CAHFW *)lpContext)->OnCheckSumNotify(pNotify);
}


RuleCallbackResult CAHFW::OnCheckSumNotify(const ChecksumChangedNotify *pNotify)
{
	if( m_bAppLocked )
	{
		for(tUINT i = 0; i < m_aModifiedApp.size(); i++)
			if( m_aModifiedApp[i].IsEqual(pNotify->ApplicationNewChecksum) )
				return crBlock;
		cHash &pHash = m_aModifiedApp.push_back();
		pHash.Copy(pNotify->ApplicationNewChecksum);
	}
	
	static ChecksumChangedNotify prev_Notify;
	if(prev_Notify.PID==pNotify->PID)
		return (RuleCallbackResult)(int)prev_Notify.ApplicationName;

	RuleCallbackResult ruleres = crBlock;
	if(m_TaskState != TASK_STATE_RUNNING)
	{
		PR_TRACE((0, prtERROR, "fw\tOnCheckSumNotify. task isn't running"));
		return ruleres;
	}
	if(!pNotify)
	{
		PR_TRACE((0, prtERROR, "fw\tOnCheckSumNotify. bad data pNotify is null"));
		return ruleres;
	}

	_ahfwtask* pTask = (_ahfwtask*)&m_Data;

	if (TASK_STATE_RUNNING != *pTask->m_pState)
		return ruleres;

	CallbackEnter();

	PR_TRACE((this, prtIMPORTANT, "fw\tOnCheckSumNotify app changed %S", pNotify->ApplicationName));
	PR_TRACE((this, prtIMPORTANT, "fw\tOnCheckSumNotify old: 0x%x...0x%x, new: 0x%x...0x%x ", 
		pNotify->ApplicationOldChecksum[0], pNotify->ApplicationOldChecksum[FW_HASH_SIZE - 1],
		pNotify->ApplicationNewChecksum[0], pNotify->ApplicationNewChecksum[FW_HASH_SIZE - 1]));

    bool bZeroHash = false;
    tBYTE zerohash[FW_HASH_SIZE];
    memset(zerohash, 0, sizeof(zerohash));
    if (!memcmp(pNotify->ApplicationOldChecksum, zerohash, FW_HASH_SIZE))
    {
        bZeroHash = true;
        PR_TRACE((this, prtIMPORTANT, "fw\tZero hash found")); 
    }

	pTask->m_CommonStat.m_AppChanges++;
	
	cAskActionFwAppChanged AskAction;
	cAHFWAppChanged& Report = AskAction.m_AppChanged;
	Report.m_Time = time(NULL);

	Report.m_ApplicationName.assign(pNotify->ApplicationName, cCP_UNICODE);
	Report.m_PID = pNotify->PID;	
	Report.m_nSessionId = GetSessionIDForProcess(Report.m_PID);
	Report.m_strTaskType.assign(_task_name, cCP_UNICODE);
	Report.m_IsBLocked = cTRUE;

	AskAction.m_nSessionId = Report.m_nSessionId;

	AskAction.m_strTaskType.assign(_task_name, cCP_UNICODE);
	AskAction.m_nObjectType = OBJTYPE_NETWORK;
	AskAction.m_strObjectName.assign(Report.m_ApplicationName);
	
	AskAction.m_nDefaultAction = ACTION_ALLOW;
	AskAction.m_nResultAction = ACTION_ALLOW;
	AskAction.m_nActionsAll  = ACTION_ALLOW | ACTION_DENY;
	AskAction.m_nActionsMask = ACTION_ALLOW | ACTION_DENY;
	
	AskAction.m_nDetectDanger = DETDANGER_INFORMATIONAL;
	AskAction.m_nDetectStatus = DSTATUS_SURE;
	AskAction.m_nDetectType = DETYPE_UNKNOWN;

	FWGetUnique(pTask->m_hSync, &pTask->m_Unique, &AskAction.m_qwUniqueId);

    if (bZeroHash)
        AskAction.m_nResultAction = ACTION_ALLOW;
    else if( m_bAppLocked )
	    AskAction.m_nResultAction = ACTION_DENY;
    else 
	    sysSendMsg(pmcASK_ACTION, cAskActionFwAppChanged::APP_CHANGED, 0, &AskAction, SER_SENDMSG_PSIZE);

	switch(AskAction.m_nResultAction)
	{
	case ACTION_ALLOW:
		ruleres = crPassModifyChecksum;
		Report.m_IsBLocked = cFALSE;
		break;
	case ACTION_DENY:
		Report.m_IsBLocked = cTRUE;
		break;
	default:
		ruleres = crPassModifyChecksum;
		Report.m_IsBLocked = cFALSE;
		PR_TRACE((this, prtERROR, "fw\tOnCheckSumNotify. Wrong answer in ahfw (checksum): 0x%x. Will use allow instead!", AskAction.m_nResultAction));
		break;
	}

	PR_TRACE((this, prtIMPORTANT, "fw\tOnCheckSumNotify result 0x%x", ruleres));

	sysSendMsg(pmc_TASKAHFWAPPCHANGED_REPORT_EVENT, 0, NULL, &Report, SER_SENDMSG_PSIZE);

	if( !m_bAppLocked || bZeroHash )
	{
		cAHFWSettings *pSettings = LockSettingsInTM(cFALSE);
		if( pSettings )
		{
			tBOOL bChanged = cFALSE;
			
			for(tUINT i = 0; i < pSettings->m_AppRules.size(); i++)
			{
				cFwAppRule &Rule = pSettings->m_AppRules[i];
				if (cSTRING_COMP_EQ == Rule.m_sAppName.compare(pNotify->ApplicationName, fSTRING_COMPARE_CASE_INSENSITIVE ))
				//if( Rule.IsHashEqual((tBYTE *)pNotify->ApplicationOldChecksum) )
				{
					PR_TRACE((this, prtIMPORTANT, "fw\tOnCheckSumNotify. app %S found. check sum changed - ruleres 0x%x", 
						Rule.m_sAppName.data(), ruleres));

					if( ruleres == crPass || ruleres == crPassModifyChecksum )
					{
						PR_TRACE((this, prtIMPORTANT, "fw\tOnCheckSumNotify. app %S. check sum changed", Rule.m_sAppName.data()));
						Rule.CopyHash((tBYTE *)pNotify->ApplicationNewChecksum);
						bChanged = cTRUE;
					}
					
					if( ruleres == crBlock )
					{
						PR_TRACE((this, prtIMPORTANT, "fw\tOnCheckSumNotify. app %S. new app blocked", Rule.m_sAppName.data()));

						//Rule.SetBlocked(cTRUE);
						
						/*cFwAppRule RuleCopy = Rule;
						RuleCopy.CopyHash((tBYTE *)pNotify->ApplicationNewChecksum);
						RuleCopy.SetBlocked(cTRUE);
						AppendAppRule(pSettings, RuleCopy, bChanged);*/
					}
				}
			}
			
			UnlockSettingsInTM(pSettings, bChanged);
		}
	}

	CallbackLeave();

	memcpy(&prev_Notify,pNotify,sizeof(prev_Notify));
	prev_Notify.ApplicationName=(LPCWSTR)ruleres;

	return ruleres;
}

cAHFWSettings* CAHFW::LockSettingsInTM(tBOOL bReadOnly)
{
	PR_TRACE((this, prtIMPORTANT, "fw\tLockSettingsInTM. readonly=%d", (tDWORD)bReadOnly));

	cAHFWSettings* pSettings = NULL;
	if( PR_SUCC(((cTaskManager *)m_hBusinessLogic)->LockTaskSettings(*this, (cSerializable**) &pSettings, bReadOnly)) )
		return pSettings;
	return NULL;
}

void CAHFW::UnlockSettingsInTM(cAHFWSettings *pSettings, tBOOL bChanged)
{
	PR_TRACE((this, prtIMPORTANT, "fw\tUnlockSettingsInTM. changed=%d", (tDWORD)bChanged));

	((cTaskManager *)m_hBusinessLogic)->UnlockTaskSettings(*this, pSettings, bChanged);
}

//////////////////////////////////////////////////////////////////////////
// cNetworksFinder

tERROR cNetworksFinder::run(void *pContext)
{
	m_pContext = pContext;
	return cThreadTaskBase::start(*this);
}

void cNetworksFinder::do_work()
{
	PR_TRACE((g_root, prtIMPORTANT, "fw\tcNetworksFinder. do_work..."));
	CAHFW::cbNetworkChangesLink(m_pContext);
	PR_TRACE((g_root, prtIMPORTANT, "fw\tcNetworksFinder. do_work complete"));
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CAHFW::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "ah-firewall task", 17 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CAHFW, m_hBusinessLogic, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CAHFW, m_TaskState, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgTASK_SESSION_ID, ((tDWORD)(0xffffffff)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, CAHFW, m_TaskID, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, CAHFW, m_hReg, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_AHFW,                               // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_SOBKO,                              // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(CAHFW)-sizeof(cObjImpl),         // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CAHFW::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



#define CKAH_SUCC(op) ( op == CKAHUM::srOK )

tERROR CAHFW::AhErr2PrErr(CKAHUM::OpResult err)
{
	switch( err )
	{
	case CKAHUM::srOK:                                  return errOK;
	case CKAHUM::srAlreadyStarted:                      return errOK;
	case CKAHUM::srInsufficientBuffer:                  return errNOT_ENOUGH_MEMORY;
	case CKAHUM::srInvalidArg:                          return errPARAMETER_INVALID;
	case CKAHUM::srNeedReboot:	sysSendMsg(pmc_TASK_OS_REBOOT_REQUEST, 0, this, 0, 0);
														return errOK;
	case CKAHUM::srNeedReload:
	case CKAHUM::srManifestNotFound:
	case CKAHUM::srWrongManifestFormat:
	case CKAHUM::srErrorReadingPersistentManifest:
	case CKAHUM::srNotStarted:
	case CKAHUM::srNoPersistentManifest:
	case CKAHUM::srOpFailed:                            return errNOT_OK;
	}
	return errUNEXPECTED;
}

tERROR CAHFW::TaskStart()
{
	tERROR err = errOK;

	if( m_TaskState == TASK_STATE_PAUSED )
	{
		err = TaskResume();
		return err;
	}
	
	cStringObj strManifest("%Bases%\\CKAH.set");
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strManifest), 0, 0);

	PR_TRACE((this, prtIMPORTANT, "fw\tTaskStart. manifest file %S", strManifest.data()));

	{
        cBasesLoadLocker lck1((cObject *)this, cUPDATE_CATEGORY_ANTIHACKER_I386, cBasesLoadLocker::withLock);
		cBasesLoadLocker lck2((cObject *)this, cUPDATE_CATEGORY_ANTIHACKER_X64, cBasesLoadLocker::withLock);
		if( PR_SUCC(err) ) err = AhErr2PrErr(CKAHUM::Reload(strManifest.data()));
	}

	if( PR_SUCC(err) )
	{
		cAHFWSettings *pSettings = LockSettingsInTM(cTRUE);
		if( pSettings )
		{
			tBOOL bSettingsChanged = cFALSE;
			if( PR_SUCC(err) ) err = AhErr2PrErr(CKAHFW::Start(cbPacketRuleNotifyLink, this, cbAppRuleNotifyLink, this, cbCheckSumNotifyLink, this));
			if( PR_SUCC(err) ) err = UpdateSettingsInTask(pSettings, &bSettingsChanged);

			UnlockSettingsInTM(pSettings, bSettingsChanged);
		}
		else
			err = errPARAMETER_INVALID;
	}
	if( PR_SUCC(err) ) err = TaskResume();
	if( PR_FAIL(err) )
		TaskStop();
	
	return err;
}

tERROR CAHFW::TaskStop()
{
	CKAHFW::Stop();
	CKAHSTM::Stop();
	return errOK;
}

tERROR CAHFW::TaskPause()
{
	tERROR err = errOK;
	if( PR_SUCC(err) ) err = AhErr2PrErr(CKAHFW::Pause());
	if( m_StealthEnabled )
		if( PR_SUCC(err) ) err = AhErr2PrErr(CKAHSTM::Pause());
	return err;
}

tERROR CAHFW::TaskResume()
{
	tERROR err = errOK;
	if( m_StealthEnabled )
		if( PR_SUCC(err) ) err = AhErr2PrErr(CKAHSTM::Resume());
	if( PR_SUCC(err) ) err = AhErr2PrErr(CKAHFW::Resume());
	return err;
}
