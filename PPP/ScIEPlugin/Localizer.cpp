// Localizer.cpp: implementation of the CLocalizer class.
//
//////////////////////////////////////////////////////////////////////

#define PR_IMPEX_DEF

#include "Localizer.h"
#include "ver_mod.h"
#include <ProductCore/GuiLoader.h>

#include <Prague/plugin/p_win32_reg.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
#include <Prague/iface/e_loader.h> 
IMPORT_TABLE_END

//////////////////////////////////////////////////////////////////////
// cLocalizedResources

#define SECTID_IEPLUGINBAR        "IePluginBar"

#define STRID_CAPTION             "Caption"
#define STRID_STATISTICS          "Statictics"
#define STRID_PRODUCTNOTLOADED    "ProductNotLoaded"
#define STRID_VIEWREPORT          "ViewReport"

cLocalizedResources::cLocalizedResources() :
	m_bInited(false), m_nLocale(1252)
{
	// Если Прага не загрузится, показать хоть что-то
	m_wsCaption = L"Web Anti-Virus";
	m_sCaption = "Web Anti-Virus";
	m_sStatistics = "Web Anti-Virus statistics:\n\nScripts scanned: %5d\nScripts blocked: %5d";
	m_sProductNotLoaded = "Web Anti-Virus is not running";
	m_sViewReport = "View report";
}

cLocalizedResources::~cLocalizedResources()
{
}

bool cLocalizedResources::Load(HINSTANCE hInstance, hROOT hRoot)
{
	if(!m_bInited && hRoot)
	{
		cAutoObj<cRegistry> hEnvironment;
		g_root->ResolveImportTable(NULL, import_table, PID_APPLICATION);
		if( PrSetEnviroment )
		{
			tERROR err = errOK;
			if( PR_SUCC(err) ) err = g_root->sysCreateObject((hOBJECT*)&hEnvironment, IID_REGISTRY, PID_WIN32_REG);
			if( PR_SUCC(err) ) err = hEnvironment->propSetBool(plWOW64_32KEY, cTRUE);
			if( PR_SUCC(err) ) err = hEnvironment->propSetStr(0, pgROOT_POINT, (void *)"HKLM\\" VER_PRODUCT_REGISTRY_PATH);
			if( PR_SUCC(err) ) err = hEnvironment->propSetBool(pgOBJECT_RO, cTRUE);
			if( PR_SUCC(err) ) err = hEnvironment->propSetStr(0, pgROOT_POINT, "environment", 12);
			if( PR_SUCC(err) ) err = hEnvironment->sysCreateObjectDone();
			if( PR_FAIL(err) ) 
				hEnvironment->sysCloseObject(), hEnvironment = NULL;
			else
				PrSetEnviroment((hOBJECT)hEnvironment, NULL);
		}

		CGuiLoader GuiLoader;
		if(CRootItem *pRoot = GuiLoader.CreateAndInitRoot(hRoot))
		{
			m_bInited = true;

			cStrObj temp;
			pRoot->GetString(m_sCaption, PROFILE_LOCALIZE, SECTID_IEPLUGINBAR, STRID_CAPTION);
			pRoot->LocalizeStr(temp, m_sCaption.c_str(), m_sCaption.size());
			
			pRoot->GetString(m_sStatistics, PROFILE_LOCALIZE, SECTID_IEPLUGINBAR, STRID_STATISTICS);
			pRoot->GetString(m_sProductNotLoaded, PROFILE_LOCALIZE, SECTID_IEPLUGINBAR, STRID_PRODUCTNOTLOADED);
			pRoot->GetString(m_sViewReport, PROFILE_LOCALIZE, SECTID_IEPLUGINBAR, STRID_VIEWREPORT);
			
			m_nLocale = pRoot->m_nLocaleId;
			
			GuiLoader.Unload();
		}
	}
	return m_bInited;
}
