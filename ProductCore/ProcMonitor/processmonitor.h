// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  19 December 2007,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- processmonitor.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __processmonitor_cpp__a9e28325_dad2_4773_85d5_3a4314aecb68 )
#define __processmonitor_cpp__a9e28325_dad2_4773_85d5_3a4314aecb68
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <ProductCore/iface/i_task.h>
#include <ProductCore/plugin/p_processmonitor.h>
// AVP Prague stamp end


#include <kl_types.h>
#include <Prague/iface/i_csect.h>
#include <Prague/plugin/p_hash_md5.h>
#include <ProductCore/structs/s_procmon.h>
#include <ProductCore/plugin/p_reportdb.h>
#include <ProductCore/structs/ProcMonM.h>
#include <ProductCore/report.h>
#include <AV/iface/i_avs.h>

class cProcess;
class cEventHandler;
class cProcmonEventProvider;
class cMSCatCache;
struct CProcMon;

struct sProcMonInternalContext {
	sProcMonInternalContext(CProcMon* pProcMon) :
	m_nCurrentFileRevision(cUNKNOWN_FILE_REVISION),
	m_nFileId(0),
	m_nPid(0),
	m_pProcMon(pProcMon),
	m_bStop(false)
	{}

	cStringObj m_sFileName;
	cIOObj    m_hIo;
	tDWORD    m_nCurrentFileRevision;
	tObjectId m_nFileId;
	tQWORD    m_nPid;
	CProcMon* m_pProcMon;
	bool      m_bStop;
};

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CProcMon : public cProcessMonitor {
private:
// Internal function declarations
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
	tERROR pr_call CalcObjectHash( hOBJECT p_object, tQWORD* p_hash );
	tERROR pr_call GetProcessInfo( cSerializable* p_info );
	tERROR pr_call IsProcessTrusted( cSerializable* p_data, tBOOL* p_result );
	tERROR pr_call IsImageActive( const tWCHAR* p_pFileName, tDWORD* p_pdwFlags );
	tERROR pr_call RegisterEventHandler( tPTR p_pEventHandler, tDWORD p_dwFlags );
	tERROR pr_call UnregisterEventHandler( tPTR p_pEventHandler );
	tERROR pr_call GetProcessInfoEx( tQWORD p_pid, cSerializable* p_info );
	tERROR pr_call GetFileInfoEx( const tWCHAR* p_filename, cSerializable* p_info );

// Data declaration
	cTaskManager* m_tm; // --
// AVP Prague stamp end



	void FillTriggers2(cProcess& pProcess);
	void UpdateTriggers2();
	void AssignToNotify(cProcMonNotify* pNotify, cProcess& pProcess);

	tERROR AddActiveImage(const tWCHAR* wcsFileName);

	hREPORTDB m_ReportDB;

public:
	tERROR GetKLSRLInfo(cModuleInfoKLSRL* pInfo, sProcMonInternalContext& pContext);
	tERROR GetFileSignature(cModuleInfoSignature* pInfo, sProcMonInternalContext& pContext);
	tERROR GetObjectHashMD5( cFileHashMD5* pInfo, sProcMonInternalContext& pContext);
	tERROR GetObjectHashSHA1( cFileHashSHA1* pInfo, sProcMonInternalContext& pContext);
	tERROR GetObjectHashSHA1Norm( cFileHashSHA1Norm* pInfo, sProcMonInternalContext& pContext);
	tERROR GetObjectSecurityRating( cSecurityRating* pInfo, sProcMonInternalContext& pContext);
	tERROR GetAvsSession(hAVSSESSION* phAvsSession);
	tERROR CloseAvsSession();
	tERROR GetModuleInfo( cModuleInfo* pInfo, hIO hIo );

	tERROR GetCachedFileInfo( cSerializable* p_info, sProcMonInternalContext& ctx );
	tERROR SetCachedFileInfo( cSerializable* p_info, sProcMonInternalContext& ctx );

	cMSCatCache* m_pMSCatCache;

protected:
	tERROR iGetFileInfoEx( const tWCHAR* p_filename, tQWORD p_pid, cSerializable* p_info );
	tERROR iGetProcessInfoEx( tQWORD nPid, cProcessInfo* pInfo );
	tERROR GetImageNameHash(const tWCHAR* wcsFileName, tQWORD* pqwHash);
	tERROR GetPrefetch();
	tERROR OnFileDownload(hIO io, cSerializable* pParams);

	tERROR GetIOHashMD5( hIO io, tBYTE md5[16] );

	cCriticalSection* m_lock;
	cHash*            m_hash;
	cBLTrustedApps    m_settings;
	
	tINT              m_nPrefetchReqCount;

	cVector<tQWORD>   m_vActiveImages;

	cProcmonEventProvider* m_pEventProvider;
	cEventHandler* m_pEventHandlerProcMon;
	size_t m_nTriggersSlot;
	unsigned long  m_CurrentKlsrlAddress;
	unsigned short m_CurrentKlsrlPort;
	uint32 m_hid;
	uint32 m_seqn;
	time_t m_LastTimeoutTime;

	tERROR IsInLocalActiveImageList(const tWCHAR* wcsFileName);
	tERROR GetKLSRLConnectParams(unsigned long* paddress, unsigned short* pport, uint32* phid, uint32* pseqn);
	hAVS        m_hAvs;
	hAVSSESSION m_hAvsSession;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CProcMon)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // processmonitor_cpp
// AVP Prague stamp end



