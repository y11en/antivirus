// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------

#if !defined( __avs_scan_h )
#define __avs_scan_h

#include "avssession.h"

class CScanContext;
class CObjectContext;

// -------------------------------------------

class CInfectedContext
{
public:
	CInfectedContext() : m_pCtx(NULL){}
	~CInfectedContext() { clear(); }

	bool init(cObjectInfo& pObject)
	{
		if( m_pCtx ) return true;
		m_pCtx = new cInfectedObjectInfo();
		*(cObjectInfo*)m_pCtx = pObject;
		return true;
	}
	void clear()
	{
		if( m_pCtx ) delete m_pCtx;
		m_pCtx = NULL;
	}

	cInfectedObjectInfo * operator->(){ return m_pCtx; }
	operator cInfectedObjectInfo*() { return m_pCtx; }
	bool operator !() { return !m_pCtx; }

private:
	cInfectedObjectInfo * m_pCtx;
};

// -------------------------------------------

struct CInfectedHashData
{
	CInfectedHashData() :
		m_hIo(NULL), m_qwHash(0), m_nNumDetected(0),
		m_bWasReadOnly(cFALSE), m_bWasDone(cFALSE){}

	tQWORD          m_qwHash;
	tDWORD          m_nNumDetected;
	enDetectType    m_dwType;
	enDetectStatus  m_dwStatus;
	enDetectDanger  m_dwDanger;
	tDWORD          m_dwBehavior;
	cStrObj         m_sName;
	cIO *           m_hIo;
	tBOOL           m_bWasReadOnly;
	tBOOL           m_bWasDone;
	cVector<tQWORD> m_vVolumes;
};

// -------------------------------------------

class CObjectContext : public cDetectObjectInfo, public cProtectionStatistics
{
public:
	CObjectContext(hOBJECT pObject);
	~CObjectContext();

	void TraceOut(tSTRING strMessage, tTRACE_LEVEL nSeverity=prtIMPORTANT);

	tERROR OnMsgRecieve(tDWORD msg_cls, tDWORD msg_id);

	tERROR OnEngineProcessing(tDWORD msg_id = pm_PROCESSING_YIELD);
	tERROR OnEngineAction(tDWORD msg_id);
	tERROR OnEngineIO(tDWORD msg_id);
	tERROR OnProcessIO(tDWORD msg_id);
	tERROR OnStartupMessage(tDWORD msg_id);
	tERROR OnOsVolume(tDWORD msg_id);

	CAVSEngine *     GetEngine();
	cIO *            GetCached(){ return (cIO*)(m_pCached ? m_pCached : m_pObject); };
	CObjectContext * GetNextContext(hOBJECT pObject);

	void InitContext();
	void InitObjectInfo();
	bool InitInfectedInfo();
	void CopyInfectedInfo(cDetectObjectInfo* pData);
	void InitAskAction(cAskObjectAction& pInfo, int nActionMask);
	bool InitSimpleObjectId(tQWORD& qwObjectId);
	bool InitDetectedObjectId();
	bool InitCureOnReboot();
	bool InitExecutableParent(bool bCheck);
	bool CalcObjectHash(cInfectedObjectInfo* pInfo, CObjectContext* pReopenable);

	bool ChooseAction(int &nActionMask, enNotCuredReason nReason, int& nAction);

	bool CheckExcludeByMasks(bool bCheckAbsolute);
	bool CheckExcludeByType();
	bool CheckExcludeBySFDB();
	bool CheckDetectByMask();
	bool CheckMemoryModule(bool bCleanUp, cStrObj* strName = NULL);

	bool IsShouldBeScan(bool bPreProcess = false);
	tERROR IsOSShouldBeScan(bool bCheckOS);

	enNotCuredReason ConvertProcessingError(tERROR nProcessingError);
	enNotCuredReason GetNotProcessingReason();

	void IncrementStatistic(tDWORD nOffset, tINT nNum=1);
	void UpdateStatus(bool bForDelete=false);
	bool UpdateDetectedInfo(bool bForDelete=false);

	tERROR SendReport(tDWORD nMsgClass, enObjectStatus nStatus, int nDesc = 0, cStringObj *strDesc = NULL);
	tERROR SendDetected(long nActionMask);

	tERROR PlaceInBackup(bool bForQuarantine = false);
	tERROR PlaceInQuarantine();
	tERROR MarkDeleteOnClose();
	tERROR CureOnReboot();
	tERROR PostCureOnReboot();
	tERROR CureOnRebootFailed();
	tERROR DeleteOnReboot(bool bForQuarantine=false);
	tERROR DeleteTryAgain();
	tERROR RenameObject();
	tERROR CleanUp();

	tERROR ProcessScanBegin();
	tERROR ProcessScanDone();

	tERROR ProcessArchiveBegin(tQWORD qwOffset, tDWORD* pPlugins);
	tERROR ProcessArchiveCheck(tQWORD qwHash, tQWORD qwOffset);
	tERROR ProcessArchiveDone(cOS* pOS);

	tERROR ProcessOS();
	tERROR ProcessObjPtr(cObjPtr* obj);
	tERROR ProcessOSEnter(cOS* pOS);
	tERROR ProcessOSOffset(tQWORD qwOffset);
	tERROR ProcessOSPreClose(cOS* pOS);
	tERROR ProcessSubIO(hIO obj, tPTR ioCtx, bool bOs, bool bPswd = false);
	tERROR ProcessSubIOEnter();
	tERROR ProcessPasswordProtected();
	tERROR ProcessEncrypted();
	tERROR ProcessCorrupted();
	tERROR ProcessVirusDetect(bool fByHash = false);
	tERROR ProcessDisinfectImpossible(enNotCuredReason nNonCuredReason = NCREASON_NONCURABLE);
	tERROR ProcessDisinfected();
	tERROR ProcessProcessingError(tERROR* pError = NULL);
	tERROR ProcessNonCurable(enNotCuredReason nReason);

	tERROR ProcessSetAttributesFailed();
	tERROR ProcessDeleteOnCloseFailed();
	tERROR ProcessDeleteOnClose();
	tERROR ProcessCreateFailed();
	tERROR ProcessPasswordRequest();

	tERROR ProcessDisinfectRequest(enNotCuredReason nNonCuredReason = NCREASON_NONE, enActions* nRetAction = NULL);
	tERROR ProcessSectorOverwriteRequest();

public:
	CScanContext   *  m_pScanCtx;
	CAVSEngine *      m_pCurEngine;
	cDetectObjectInfo * m_DetectInfo;
	cVector<tQWORD> * m_pvOffsets;
	CObjectContext *  m_pPrev;
	CObjectContext *  m_pNext;

	CInfectedContext  m_InfCtx;
	cStrObj           m_sPackerName;

	hOBJECT		m_pContext;
	tPTR		m_pBuff;
	tDWORD *	m_pBuffLen;
	hIO         m_hCopyIO;

	hOBJECT		m_pObject;
	hOBJECT		m_pCached;
	tPTR        m_pIOCtx;
	tPTR        m_pProcessCtx;
	tDWORD		m_nStatus;
	tDWORD      m_nHashPos;
	tDWORD      m_nPswdPos;
	CInfectedHashData * m_pHash;
	tPTR        m_nICheckerCtx;
	tPTR        m_nOSICheckerCtx;

	unsigned	m_fPreProcessing : 1;
	unsigned	m_fLockedProcessing : 1;
	unsigned	m_fTreatsProcessing : 1;
	unsigned	m_fStopProcessing : 1;
	unsigned	m_fQuarantineProcessing : 1;
	unsigned	m_fStreamProcessing : 1;
	unsigned	m_fExternalDetectProcessing : 1;
	unsigned	m_fCancelRequest : 1;

	unsigned	m_fProcessing : 1;
	unsigned	m_fPreProcessed : 1;
	unsigned	m_fPostProcessing : 1;
	unsigned	m_fFixProcessed : 1;
	unsigned	m_fActionProcessed : 1;
	unsigned	m_fDisinfectProcessed : 1;
	unsigned	m_fOkProcessed : 1;
	unsigned	m_fUnchangedObject : 1;
	unsigned	m_fOSObject : 1;
	unsigned	m_fArchive : 1;
	unsigned	m_fEmbedded : 1;
	unsigned	m_fUnpacked : 1;
	unsigned	m_fMailBase : 1;
	unsigned	m_fNative : 1;
	unsigned	m_fBackuped : 1;
	unsigned	m_fDetected : 1;
	unsigned	m_fDetectedByHash : 1;
	unsigned	m_fQuarantined : 1;
	unsigned	m_fReopenanable : 1;
	unsigned	m_fObjectIdInited : 1;
	unsigned	m_fSFDBChecked : 1;
	unsigned	m_fMasksChecked : 1;
	unsigned	m_fSkippedByType : 1;
	unsigned	m_fSkippedByVerdict : 1;
	unsigned	m_fSkippedByExclude : 1;
	unsigned	m_fSkippedByUser : 1;
	unsigned	m_fSkippedBySFDB : 1;
	unsigned	m_fPasswordSkipped : 1;
	unsigned	m_fArchiveSkipped : 1;
	unsigned	m_fToPostProcessing : 1;
	unsigned	m_fFalseAlarm : 1;
	unsigned	m_fCurableArchive : 1;
	unsigned	m_fArchiveChecked : 1;
	unsigned	m_fReopened : 1;
	unsigned	m_fCtxInited : 1;
	unsigned	m_fPragueObj : 1;

	static tPROPID  m_propContext;
	static tPROPID	m_propActionClassMask;
	static tPROPID	m_propVirtualName;
	static tPROPID	m_propInfectedCounter;
	static tPROPID	m_propReadonlyError;
	static tPROPID	m_propReadonlyObject;
	static tPROPID	m_propSessionObject;
	static tPROPID	m_propSkipThisOne;
	static tPROPID	m_propExecutableParent;
	static tPROPID	m_propSetWriteAccess;
	static tPROPID	m_propTreatUserData;
	static tPROPID	m_propIntegralParent;
	static tPROPID	m_propIStreamsCtx;
	static tPROPID	m_propBackupFlag;
	static tPROPID	m_propSpecialCure;
	
	static tERROR InitCustomProps(hROOT root);
};

// -------------------------------------------

class CScanContext : public CObjectContext
{
public:
	CScanContext(hOBJECT pObject, AVSSession *pSession, cProtectionSettings *pSettings,
		cScanProcessInfo *pProcessInfo, cSerializable *pDetectInfo = NULL, bool bForStream = false);
	~CScanContext();

	void   CleanUp();
	void   SetActionClassMask(tDWORD nClassMask);
	void   GetStatistics(cSerializable* pStat);
	void   UpdateProcessInfo();

	tERROR CheckState(bool bPause = false);
	bool   CheckTimeLimit();
	bool   CheckExcludeByStreams();
	bool   CheckByIChecker(tPTR& pICheckerCtx, hOBJECT pObject, tPID nICheckerPID);
	bool   CheckActiveDetection(CObjectContext& pCtx);
	bool   CheckExcludeByVerdict(CObjectContext& pCtx);

	tERROR Process();
	void   ProcessStop(bool bStopAll=false);
	void   PerformABS();

	void   ICUpdate(tPID nICheckerPID, tBOOL bDeleteStatus, tPTR pICheckerCtx = NULL, cObj* pObject = NULL);

	enActions AskAction(tActionId nAction, cAskObjectAction &pRequest, tDWORD nAccessMode=0, bool bUseCache=true);
	enActions AskPassword(cAskObjectPassword &pRequest);

	bool   RegisterPassword(cStringObj& strPswd);
	bool   GetPassword(cStringObj& strPswd, tDWORD* pCookie);

	tDWORD GetTimeDelta();

public:
	AVSSession *          m_Session;
	CAVSEngine *          m_Engine;
	cProtectionSettings * m_Settings;
	cScanProcessInfo *    m_ProcessInfo;
	tDWORD                m_tmStart;
	CObjectContext *      m_pNative;
	CObjectContext *      m_pScanTop;
	hOBJECT               m_hNativeIO;
	enNotCuredReason      m_nReason;
	tDWORD                m_nProcessId;
	tDWORD                m_nScopePos;
	tDWORD                m_nClassMask;
	tDWORD                m_nClassMaskForUnchanged;
	tQWORD                m_nClassMaskEx;
	tDWORD                m_nUseICheckDays;
	tPTR                  m_nIStreamsCtx;
	tBOOL                 m_bUseABS;
	cVector<CInfectedHashData> m_vHashData;
	cVector<cStrObj>      m_vPasswords;
	tQWORD                m_dtTimeStamp;
};

// -------------------------------------------

class CAsyncScanContext : public CScanContext, public cThreadTaskBase
{
public:
	CAsyncScanContext(cObj* pObj, AVSSession *pSession, cProtectionSettings *pSettings, cScanProcessInfo *pProcessInfo) : 
		CScanContext(pObj, pSession, pSettings, pProcessInfo)
	{
		m_Settings = *pSettings;
		m_ProcessInfo = *pProcessInfo;
		CScanContext::m_Settings = &m_Settings;
		CScanContext::m_ProcessInfo = &m_ProcessInfo;
		m_ToLock = pObj->sysGetParent(IID_ANY);
		pObj->sysValidateLock(m_ToLock);
	}

	~CAsyncScanContext()
	{
		cDetectObjectInfo pDInfo; GetStatistics(&pDInfo);
		CleanUp();
		m_ProcessInfo.m_fnAsyncResult(m_pObject, &m_ProcessInfo, &pDInfo);
		m_ToLock->sysRelease();
	}

	virtual void do_work(){ Process(); }
	virtual void on_exit(){ delete this; }

private:
	cScanProcessInfo      m_ProcessInfo;
	cProtectionSettings   m_Settings;
	cObj *                m_ToLock;
};

// -------------------------------------------

struct CSaveContext
{
	CSaveContext(CObjectContext* pCtx) :
		m_pCtx(pCtx),
		m_pContext(pCtx->m_pContext),
		m_pBuff(pCtx->m_pBuff),
		m_pBuffLen(pCtx->m_pBuffLen){}

	~CSaveContext()
	{
		m_pCtx->m_pContext = m_pContext;
		m_pCtx->m_pBuff = m_pBuff;
		m_pCtx->m_pBuffLen = m_pBuffLen;
	}

	CObjectContext* m_pCtx;
	hOBJECT		m_pContext;
	tPTR		m_pBuff;
	tDWORD *	m_pBuffLen;
};

// -------------------------------------------

struct CMaskChecker
{
	CMaskChecker(cStringObj& strPath, bool bShort = false);

	bool MatchPath(cStringObj& strMask, bool bRecurse, bool bCheckAbsolute);

public:
#if defined (_WIN32)
	static bool IsAbsolute(cStringPtr strMask)
	{
		return strMask.size() > 2 && (strMask[1] == ':' || !strMask.compare("\\\\", 2));
	}
#elif defined (__unix__)
	static bool IsAbsolute(cStringPtr strMask)
	{
          return strMask.size() >= 1  && (strMask[0] == '/' );
	}

#endif
	static bool MatchStrings(cStringPtr s, cStringPtr p);

private:
	cStringObj& m_strPath;
	cStringPtr  m_nNamePos;
	bool        m_bShort;
};

// -------------------------------------------

#define STATISTIC_OFFSET(field)	offsetof(cProtectionStatistics, field)

inline void CObjectContext::TraceOut(tSTRING strMessage, tTRACE_LEVEL nSeverity)
{
	PR_TRACE((g_root, nSeverity, "%S\t%s\t%S",
		m_pScanCtx->m_Session->m_strTaskType.data(), strMessage, m_strObjectName.data()));
}

#endif // __avs_scan_h

// -------------------------------------------
