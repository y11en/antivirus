// GuiLoader.h: interface for the GuiLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILOADER_H__4D061143_0752_4D1B_B4A4_B3A104D07410__INCLUDED_)
#define AFX_GUILOADER_H__4D061143_0752_4D1B_B4A4_B3A104D07410__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ProductCore/basegui.h>
#include <ProductCore/binding.h>

class CGuiLoader
{
public:
	CGuiLoader() :  m_hGuiModule(NULL), m_pRoot(NULL) {}
	~CGuiLoader() { Unload(); }
	
	bool Load(hROOT hRoot)
	{
		if( m_hGuiModule )
			return true;
		
		hRoot->LoadModule(&m_hGuiModule, "basegui.ppl", (tDWORD)strlen("basegui.ppl"), cCP_ANSI);
		return !!m_hGuiModule;
	}
	
	CRootItem * Create(hROOT hRoot, tDWORD nFlags = 0, CRootSink * pRootSink = NULL)
	{
		if( m_pRoot )
			return m_pRoot;

		tPROPID prop;
		if( PR_FAIL(hRoot->RegisterCustomPropId(&prop, PR_BASE_GUI_PROP, pTYPE_PTR)) )
			return NULL;

		if( !Load(hRoot) )
			return NULL;

		tGUICreateRoot pGUICreateRoot = (tGUICreateRoot)hRoot->propGetPtr(prop);
		if( !pGUICreateRoot )
			return NULL;
		
		pGUICreateRoot(hRoot, nFlags, pRootSink, &m_pRoot);
		return m_pRoot;
	}
	
	bool Init(const tWCHAR* strSkinPath = NULL, bool bActivate = true)
	{
		if( !m_pRoot->InitRoot(strSkinPath) )
			return false;
		
		if( bActivate && !m_pRoot->Activate(true) )
			return false;
		
		return true;
	}
	
	CRootItem * CreateAndInitRoot(hROOT hRoot, tDWORD nFlags = 0, CRootSink * pRootSink = NULL, const tWCHAR * strSkinPath = NULL, bool bActivate = true)
	{
		if( m_pRoot )
			return m_pRoot;

		if( !Create(hRoot, nFlags, pRootSink) )
			return NULL;

		if( !Init(strSkinPath, bActivate) )
			Unload();

		return m_pRoot;
	}
	
	void Unload()
	{
		if( m_pRoot )
			m_pRoot->Destroy(), m_pRoot = NULL;
		
		if( m_hGuiModule )
		{
			//m_hGuiModule->sysCloseObject();
			m_hGuiModule = NULL;
		}
	}
	
	CRootItem * GetRoot() { return m_pRoot; }

	static CGuiLoader &Instance() {static CGuiLoader ldr; return ldr;}

protected:
	hPLUGIN      m_hGuiModule;
	CRootItem *  m_pRoot;
};

#endif // !defined(AFX_GUILOADER_H__4D061143_0752_4D1B_B4A4_B3A104D07410__INCLUDED_)
