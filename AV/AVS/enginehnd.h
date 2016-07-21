// -------------------------------------------

struct AVSImpl;
class CScanContext;
class CObjectContext;
class CAVSEngine;
class CAVSEngineHnd;

// -------------------------------------------

extern CAVSEngineHnd* CreateEngineHnd_KLAV(AVSImpl* pAvs);
extern CAVSEngineHnd* CreateEngineHnd_AVP3(AVSImpl* pAvs);

// -------------------------------------------

class CAVSEngine
{
public:
	virtual ~CAVSEngine(){}

	virtual tERROR Load(cAVSUpdateInfo* pInfo){return errNOT_IMPLEMENTED; };
	virtual tERROR Init(){return errNOT_IMPLEMENTED; };
	virtual tERROR Unload(){return errNOT_IMPLEMENTED; };

	virtual tERROR GetBasesInfo(cAVP3BasesInfo& pInfo){ return errNOT_IMPLEMENTED; };

	virtual tERROR ICGetData(cObj* pObj, tPID pPid, tPTR& pICdata){return errNOT_IMPLEMENTED; };
	virtual tERROR ICCheckData(tPTR pICdata, tQWORD pBitData, tDWORD pMandatoryScanDays){return errNOT_IMPLEMENTED; };
	virtual tERROR ICUpdate(cObj* pObj, tPID pPid, tPTR pICdata, tQWORD pBitData, tBOOL pDeleteStatus){return errNOT_IMPLEMENTED; };
	virtual tERROR ICFree(tPTR& pICdata){return errNOT_IMPLEMENTED; };

	virtual tDWORD InitClassMask(cScanSettings& pSettings){return 0; };

	virtual tERROR InitContext(CObjectContext& pCtx){return errNOT_IMPLEMENTED; };
	virtual tERROR FreeContext(CObjectContext& pCtx){return errNOT_IMPLEMENTED; };

	virtual tERROR ProcessObject(CObjectContext& pCtx){return errNOT_IMPLEMENTED; };
	virtual tERROR ProcessStream(CObjectContext& pCtx, enChunkType eType, tPTR pChunk, tDWORD nSize){return errNOT_IMPLEMENTED; };
	virtual tERROR PostProcessObject(CObjectContext& pCtx){return errNOT_IMPLEMENTED; };

	virtual tERROR SetWriteAccess(CObjectContext& pCtx, cObj*& pDeletable){return errNOT_IMPLEMENTED; };
	virtual tERROR AddReopenData(cIO* pIO, tPTR pData, tDWORD nPos, tDWORD pUserData){return errNOT_IMPLEMENTED; };

	virtual tERROR CheckEIP(tPTR eip){ return errNOT_IMPLEMENTED; };

	virtual enDetectStatus GetDetectStatus(CObjectContext& pCtx){return DSTATUS_UNKNOWN; };
	virtual enDetectType   GetDetectType(CObjectContext& pCtx){return DETYPE_UNKNOWN; };
	virtual enDetectDanger GetDetectDanger(CObjectContext& pCtx){return DETDANGER_UNKNOWN; };
	virtual tERROR GetDetectName(CObjectContext& pCtx, cStringObj& strName){return errNOT_IMPLEMENTED; };
	virtual tERROR GetPakerName(CObjectContext& pCtx, cStringObj& strName){return errNOT_IMPLEMENTED; };
	virtual tERROR GetReopenData(CObjectContext& pCtx, cCharVector& pData){return errNOT_IMPLEMENTED; };
	virtual bool   GetDisinfectability(CObjectContext& pCtx){ return false; };
	virtual tDWORD GetDetectBehaviour(CObjectContext& pCtx){ return 0; };
	virtual bool   IsShouldBeScan(CObjectContext& pCtx){ return false; };

public:
	AVSImpl*        m_avs;
	CAVSEngineHnd*  m_enghnd;
	enEngineType    m_engtype;
	tLONG           m_nLockRef;

	cObj*           m_hCtxObj;
	cIChecker*      m_iChecker;
	cIChecker*      m_iSwift;

	cEvent*         m_hWaiter;
};

class CAVSEngineMng
{
public:
	CAVSEngineMng() : m_dwState(errNOT_READY), m_nTaskRef(0), m_pEngine(NULL){}

	tERROR          m_dwState;
	tLONG           m_nTaskRef;
	CAVSEngine *    m_pEngine;
};

// -------------------------------------------

class CAVSEngineHnd
{
public:
	virtual ~CAVSEngineHnd(){}
	virtual CAVSEngine* CreateEngine(enEngineType eType){ return errOK; };
	virtual tERROR OnMsgReceive(tDWORD dwMsg, tDWORD dwMsgId, cObj* pObj, cObj* pCtx, tPTR pBuff, tDWORD* pSize){ return errOK; };
};

// -------------------------------------------
