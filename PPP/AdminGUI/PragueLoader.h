// PragueLoader.h: interface for the CPragueLoader class.
//

#pragma once

#include <guiloader.h>
#include <pr_remote.h>
#include <common\tracer.h>
#include <iface\i_datacodec.h>
#include <RootSink.h>

//////////////////////////////////////////////////////////////////////////
// CPragueLoader

class CPragueLoader :
	protected cCS,
	protected CPRRemotePrague,
	protected cPrTracer,
	protected cThreadPoolBase,
	protected cThreadTaskBase
{
public:
	CPragueLoader();
	virtual ~CPragueLoader();

	tERROR         LoadPrague(HINSTANCE hModule);
	tERROR         UnLoadPrague();

	cDataCodec *   GetBase64Codec();

protected:
	tERROR         LoadTM();
	tERROR         UnloadTM();
	tERROR         InitProfiles();

	void           do_work();

protected:
	volatile bool  m_bStarting;
	volatile bool  m_bExiting;
	volatile DWORD m_nComThreadId;
	cDataCodec *   m_hBase64Codec;
};

//////////////////////////////////////////////////////////////////////////
// cGuiStub

class cGuiStub : protected cCS, public CKav6RootSink
{
protected:
	struct CGuiLoaderRef : public CGuiLoader
	{
		CGuiLoaderRef() : m_nRef(1) {}
		tDWORD m_nRef;
	};
	typedef std::vector<CGuiLoaderRef *> CGuiLoaders;
	typedef CGuiLoaders::iterator IGuiLoader;

public:
	cGuiStub() : 
		m_pReceiver(NULL),
		m_pIpRes(NULL),
		CreateObject(NULL),
		CreateObjectQuick(NULL),
		GetObjectProcess(NULL)
	{}
	~cGuiStub();

	tERROR          Init(cObject *pParent);
	cIPRESOLVER *   GetIpResolver();
	bool            CheckPassword(CItemBase* pItem, ePasswordConfirmReason nCheckType) { return false; }
	cGuiVariables * GetGuiVariables() { return &m_Vars;}
	cStringObj      GetUserId()       { return cStringObj(TASKSETT_DEFAULT_USER); }
	tDWORD          GetOSVersion()    { return GUI_OSVER_FAMILY_NT; }
	cObject *       Obj()             { return (cObject *)m_pReceiver; }

public:
	CRootItem *     GetLocRoot();
	CRootItem *     GetRoot();
	void            DestroyRoot();
	void            DestroyAllRoots();
	tERROR          SendEvent2AllRoots(tDWORD nEventId, cSerializable* pEvent = NULL, tDWORD nFlags=0, CItemBase* pSendPoint = NULL, tDWORD nTimeOut = 0);

	tERROR (pr_call *CreateObject)(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype);
	tERROR (pr_call *CreateObjectQuick)(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype);
	tERROR (pr_call *GetObjectProcess)(hOBJECT object, tPROCESS *process);

	CGuiLoaders     m_aGuis;
	CGuiLoader      m_LocGui;

protected:
	tERROR          MsgReceive(tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen);
	static tERROR pr_call MsgReceiveLink(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen);

protected:
	cMsgReceiver *  m_pReceiver;
	cIPRESOLVER *   m_pIpRes;
	cGuiVariables   m_Vars;
};
