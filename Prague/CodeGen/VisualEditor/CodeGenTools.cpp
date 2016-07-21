#include "stdafx.h"
#include <comcat.h>

#include <stuffio/ioutil.h>
#include <serialize/kldtser.h>
#include <stuff/comutils.h>

#include "../pcginterface.h"
#include "avpcontrols/checklist.h"

#include "visualeditor.h"
#include "visualeditordlg.h"
#include "codegentools.h"

#pragma warning ( disable : 4273 )
#include "property/prop_in.h"
#pragma warning ( default : 4273 )
				    
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

const IID IID_IPragueCodeGen = {0xA11E88F1,0xD95A,0x11D3,{0xBF,0x0D,0x00,0xD0,0xB7,0x16,0x1F,0xB8}};
const IID CID_PragueCodeGens = {0x3621F930,0xDA1A,0x11d3,{0xBF,0x0D,0x00,0xD0,0xB7,0x16,0x1F,0xB8}};

CGT::SOptFile			CGT::s_OptFile;
CGT::SCodeGeneratorsSet CGT::s_CodeGens;

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CGT::SCodeGenPlugin* CGT::SCodeGenItem::FindPluginData(LPCTSTR i_strPluginName)
{
	POSITION pos = m_arrPlugins.GetHeadPosition();
	while(pos)
	{
		SCodeGenPlugin& rPluginData = m_arrPlugins.GetNext(pos);
		if(rPluginData.m_strPluginName == i_strPluginName)
			return &rPluginData;
	}
	return NULL;
}

CGT::SCodeGenPlugin* CGT::SCodeGenItem::AddNewPluginData()
{
	SCodeGenPlugin rPluginData;
	POSITION pos = m_arrPlugins.AddTail(rPluginData);
	return &m_arrPlugins.GetAt(pos);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// 	CGT::SCodeGeneratorsSet

// ---
CGT::SCodeGeneratorsSet::SCodeGeneratorsSet()
{
	m_nRealCodeGenItems = 0;
}

// ---
void CGT::SCodeGeneratorsSet::FillOneObjectCache(SCodeGenItem& rItem, REFCLSID rCLSID, IPragueCodeGen *pInterface )
{
	UCHAR *pBuffer;
	
	rItem.m_oClsId = rCLSID;
	
	if ( SUCCEEDED(pInterface->GetDescription(&pBuffer)) ){
		rItem.m_strDescription = pBuffer;
		::CoTaskMemFree( pBuffer );
	}
	
	if ( SUCCEEDED(pInterface->GetVendor(&pBuffer)) ) {
		rItem.m_oAttrs.AddTail( pBuffer );
		::CoTaskMemFree( pBuffer );
	}
	
	if ( SUCCEEDED(pInterface->GetVersion(&pBuffer)) ) {
		rItem.m_oAttrs.AddTail( pBuffer );
		::CoTaskMemFree( pBuffer );
	}
}

// ---
void CGT::SCodeGeneratorsSet::LoadCodeGenItemsCache(BOOL i_bForceRecache)
{
	if(i_bForceRecache)
		m_nRealCodeGenItems = 0;

	if(m_nRealCodeGenItems)
		return;
		
	ICatInformation *pCatInfo = (ICatInformation *)
		::COMLoadCOMObjectID( CLSID_StdComponentCategoriesMgr, IID_ICatInformation );

	if(NULL == pCatInfo)
		return;

	CATID rgcatidImpl[1];
	rgcatidImpl[0] = CID_PragueCodeGens;
	
	IEnumCLSID *pObjEnum;
	if ( SUCCEEDED(pCatInfo->EnumClassesOfCategories(1, rgcatidImpl, 0, 0, &pObjEnum)) ) 
	{
		GUID rObjCLSID;
		while ( pObjEnum->Next(1, &rObjCLSID, NULL) == S_OK ) 
		{
			IPragueCodeGen *pCGInfo = LoadGenerator(rObjCLSID); 

			if ( pCGInfo )
			{
				FillOneObjectCache(m_CodeGenItemsCache[m_nRealCodeGenItems++], rObjCLSID, pCGInfo );
				pCGInfo->Release();
			}
			else 
			{
				TCHAR pBuffer[40];
				::StringFromGUID( rObjCLSID, pBuffer, sizeof(pBuffer) );

				CString rcError;
				rcError.Format( IDS_ERR_LOADINTERFACEFAILED, pBuffer );

				CAPointer<TCHAR> pErrText = GetErrorString( E_FAIL );
				rcError += "\n\r";
				rcError += pErrText;
				
				::DlgMessageBox( AfxGetMainWnd(), rcError );
			}
		}

		pObjEnum->Release();
	}
	
	pCatInfo->Release();
	::CoFreeUnusedLibraries();
}

// ---
void CGT::SCodeGeneratorsSet::UpdateGeneratorsOptions( SOptFile& i_oOptFile, LPCSTR i_strDefOutputDir)
{
	for(BYTE i=0; i<m_nRealCodeGenItems; i++)
	{
		SCodeGenItem& rCodeGen = m_CodeGenItemsCache[i];

		// fill saved generator options
		HDATA hCodeGenOpts = i_oOptFile.GetCodeGenOptions(rCodeGen.m_oClsId);
		HPROP hOldOutPutDir = NULL;

		// get proto output dir
		if(!GetPublicDirProperty(rCodeGen.m_strPublicDir, hCodeGenOpts))
			rCodeGen.m_strPublicDir = i_strDefOutputDir;
		if(!GetPrivateDirProperty(rCodeGen.m_strPublicDir, hCodeGenOpts))
			rCodeGen.m_strPrivateDir = "";
		
		rCodeGen.m_arrPlugins.RemoveAll();

		if(NULL == hCodeGenOpts)
			continue;

		// get plugins data
		HDATA hPluginData = ::DATA_Get_First( hCodeGenOpts, NULL );
		
		while ( hPluginData ) 
		{
      CString name = GetPluginName(hPluginData);
      // petrovitch ??? a lot of empty plugins - why ? [8/30/2002]
      if ( name.GetLength() ) {
        SCodeGenPlugin* pPluginData = rCodeGen.AddNewPluginData();
        pPluginData->m_strPluginName = name;
        GetPublicDirProperty(pPluginData->m_strPublicDir, hPluginData);
        GetPrivateDirProperty(pPluginData->m_strPrivateDir, hPluginData);
      }

			hPluginData = ::DATA_Get_Next( hPluginData, NULL );
		}
	}
}

// ---
CGT::SCodeGenItem* CGT::SCodeGeneratorsSet::GetGenCacheItem(GUID const& i_roClsid)
{
	for(BYTE i=0; i<m_nRealCodeGenItems; i++){
		if(i_roClsid == m_CodeGenItemsCache[i].m_oClsId)
			return &m_CodeGenItemsCache[i];
	}
	return NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// 	CGT::SOptFile
CGT::SOptFile::SOptFile()
{
	m_hOptRoot = NULL;
	m_strOptPathname[0] = 0;
}

CGT::SOptFile::~SOptFile()
{
	Close();
}

BOOL	CGT::SOptFile::IsLoaded()
{
	return lstrlen(GetPathname()) && m_hOptRoot;
}

LPCTSTR CGT::SOptFile::GetPathname()
{
	return m_strOptPathname;
}

BOOL CGT::SOptFile::Load(LPCTSTR i_strOptPathname, BOOL i_bImplementation)
{
	Close();

	if ( !::KLDT_DeserializeUsingSWM(i_strOptPathname, &m_hOptRoot) || !m_hOptRoot) 
		return FALSE;

	// check right header
	AVP_dword nFileType;
	::DATA_Get_Val( m_hOptRoot, NULL, 0, &nFileType, sizeof(nFileType) );
	
	if ( nFileType != wmt_CGOptions ) 
	{
		Close();
		return FALSE;
	}

	lstrcpy(m_strOptPathname, i_strOptPathname);
	return TRUE;
}

BOOL CGT::SOptFile::SaveAs(LPCTSTR i_strNewName)
{
	lstrcpy(m_strOptPathname, i_strNewName);
	return ::KLDT_SerializeUsingSWM(GetPathname(), m_hOptRoot, "");
}

void CGT::SOptFile::Close()
{
	if(m_hOptRoot)
	{
		::DATA_Remove(m_hOptRoot, NULL);
		m_hOptRoot = NULL;
	}

	m_strOptPathname[0] = 0;
}

// ---
// find data sub tree in options tree corresponding to the given clsid
HDATA CGT::SOptFile::GetCodeGenOptions(CLSID const& i_rClsId)
{
	if ( NULL == m_hOptRoot ) 
		return NULL;
	
	HDATA hCodeGenOpts = ::DATA_Get_First( m_hOptRoot, NULL );
	
	while ( hCodeGenOpts ) {
		
		CLSID rCLSID;
		if(GetClassId(hCodeGenOpts, rCLSID) && rCLSID == i_rClsId)
			return hCodeGenOpts;
		
		hCodeGenOpts = ::DATA_Get_Next( hCodeGenOpts, NULL );
	}
	
	return NULL;
}

HDATA	CGT::SOptFile::GetPluginOptions(CLSID const& i_rClsId, LPCTSTR i_strPlugin, BOOL i_bPluginRoot)
{
	if(0 == lstrlen(i_strPlugin))
		return NULL;

	HDATA hCodeGenData = GetCodeGenOptions(i_rClsId);

	if(!hCodeGenData)
		return NULL;

	HDATA hPluginRoot = FirstOneLevelDownData(hCodeGenData, VE_PID_PL_PLUGINNAME, (void*) i_strPlugin);
	if(!hPluginRoot)
		return NULL;

	return i_bPluginRoot ? hPluginRoot : DATA_Get_First(hPluginRoot, NULL);
}

CGT::SOptFile::operator HDATA()
{
	return m_hOptRoot;
}

void CGT::SOptFile::UpdateOptionsTree(BOOL i_bImplementation, HDATA i_hOptData, 
					   GUID const& i_quidCodeGenClassId, LPCTSTR i_strPluginName)
{
	if(NULL == m_hOptRoot)
		m_hOptRoot = ::DATA_Add( NULL, NULL, VE_PID_CGIOPTIONSROOT, AVP_dword(wmt_CGOptions), 0 );
	
	// find and remove old data
	HDATA hOldOpts;

	if(i_bImplementation)
		hOldOpts = GetPluginOptions(i_quidCodeGenClassId, i_strPluginName, TRUE);
	else
		hOldOpts  = GetCodeGenOptions(i_quidCodeGenClassId);

	if(hOldOpts)
	{
		::DATA_Detach(hOldOpts, NULL);
		::DATA_Remove(hOldOpts, 0);
	}

	UpdateOutputDirs();
	HDATA hNewOpts = ::DATA_Copy(0, 0, i_hOptData, DATA_IF_ROOT_INCLUDE);
	
	if(i_bImplementation)
		hOldOpts = GetPluginOptions(i_quidCodeGenClassId, i_strPluginName, TRUE);
	else
		hOldOpts  = GetCodeGenOptions(i_quidCodeGenClassId);

  MAKE_ADDR1( addr, DATA_Get_Id(hNewOpts,0) );
  if ( !::DATA_Find(hOldOpts,addr) ) // если не проверять - валится [8/30/2002]
	  ::DATA_Attach( hOldOpts, NULL, hNewOpts, DATA_IF_ROOT_INCLUDE );
}

void CGT::SOptFile::UpdateOutputDirs()
{
	for(BYTE i=0; i<s_CodeGens.m_nRealCodeGenItems; i++)
	{
		SCodeGenItem& rItem = s_CodeGens.m_CodeGenItemsCache[i];

		HDATA hGenOpts = GetCodeGenOptions(rItem.m_oClsId);
		
		if(NULL == hGenOpts)
		{
			hGenOpts = ::DATA_Add( NULL, NULL, VE_PID_IF_PROPERTY(::CreateDataId(m_hOptRoot)), 
				AVP_dword(wmt_CGOptions), 0 );
			
			::DATA_Add_Prop( hGenOpts, NULL, VE_PID_CGIOGENERATORCLSID, AVP_dword(&rItem.m_oClsId), sizeof(GUID) );
			::DATA_Attach( m_hOptRoot, NULL, hGenOpts, DATA_IF_ROOT_INCLUDE );
		}
		
		HPROP hOutDir = ::DATA_Find_Prop(hGenOpts, NULL, VE_PID_CGPI_PUBLICDIR);
		if(hOutDir)
			::DATA_Remove_Prop_H(hGenOpts, NULL, hOutDir);
		
		::DATA_Add_Prop( hGenOpts, NULL, VE_PID_CGPI_PUBLICDIR, AVP_dword(LPCTSTR(rItem.m_strPublicDir)), 0 );
		::DATA_Add_Prop( hGenOpts, NULL, VE_PID_CGPI_PRIVATEDIR, AVP_dword(LPCTSTR(rItem.m_strPrivateDir)), 0 );

		// insert plugin options for codegen
		POSITION pos = rItem.m_arrPlugins.GetHeadPosition();
		while(pos)
		{
			SCodeGenPlugin* pPluginData = &rItem.m_arrPlugins.GetNext(pos);

			HDATA hPluginRoot = GetPluginOptions(rItem.m_oClsId, pPluginData->m_strPluginName, TRUE ); 
			// TRUE -- ...мать, ...мать, ...мать!!!

			if(NULL == hPluginRoot)
			{
				hPluginRoot = ::DATA_Add( NULL, NULL, VE_PID_IF_PROPERTY(::CreateDataId(hGenOpts)), 
					AVP_dword(wmt_CGOptions), 0 );

				::DATA_Add_Prop( hPluginRoot, NULL, VE_PID_PL_PLUGINNAME, 
					AVP_dword((LPCTSTR)pPluginData->m_strPluginName), 0 );

				// attach plugin subroot to the generator root
				::DATA_Attach( hGenOpts, NULL, hPluginRoot, DATA_IF_ROOT_INCLUDE );
			}
			
			// refresh plugin public dir
			HPROP hOutDir = ::DATA_Find_Prop(hPluginRoot, NULL, VE_PID_CGPI_PUBLICDIR);
			if(hOutDir)
				::DATA_Remove_Prop_H(hPluginRoot, NULL, hOutDir);
			
			::DATA_Add_Prop( hPluginRoot, NULL, VE_PID_CGPI_PUBLICDIR, AVP_dword((LPCTSTR)pPluginData->m_strPublicDir), 0 );

			// refresh plugin private dir
			hOutDir = ::DATA_Find_Prop(hPluginRoot, NULL, VE_PID_CGPI_PRIVATEDIR);
			if(hOutDir)
				::DATA_Remove_Prop_H(hPluginRoot, NULL, hOutDir);
			
			::DATA_Add_Prop( hPluginRoot, NULL, VE_PID_CGPI_PRIVATEDIR, AVP_dword((LPCTSTR)pPluginData->m_strPrivateDir), 0 );
		}
	}
}

CString CGT::SOptFile::BrowseForNewOptPathname(BOOL i_bImplementation)
{
	CString rcFileName;
	
	// ---
	static UINT g_StrIDTable[][2] = 
	{
		{ IDS_CGO_DEFEXTENTION, IDS_CGO_FILTER },
		{ IDS_HGO_DEFEXTENTION, IDS_HGO_FILTER },
	};
	
	int nRegim = i_bImplementation ? 0 : 1;

	CString rcExtString;
	rcExtString.LoadString( g_StrIDTable[nRegim][0] );
	
	CString rcFltrString;
	rcFltrString.LoadString( g_StrIDTable[nRegim][1] );
	
	CFileDialog rcSaveDlg( FALSE, rcExtString, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, rcFltrString, NULL );
	
	if ( rcFileName.IsEmpty() ) 
	{
		char pCurrDir[_MAX_PATH];
		::IOSGetCurrentDirectory( pCurrDir, sizeof(pCurrDir) );
		rcSaveDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else 
	{
		char pCurrDir[_MAX_PATH];
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcSaveDlg.m_ofn.lpstrInitialDir = pCurrDir;
		
		char pFileName[_MAX_PATH];
		::IOSFileExtract( rcFileName, pFileName, _MAX_PATH );
		rcSaveDlg.m_ofn.lpstrFile = pFileName;
		rcSaveDlg.m_ofn.nMaxFile = sizeof(pFileName);
	}
	
	void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt );
	PrepareFilterIndex( rcSaveDlg.m_ofn, rcExtString );
	
	if ( rcSaveDlg.DoModal() == IDOK ) 
		rcFileName = rcSaveDlg.GetPathName();

	return rcFileName;
}

BOOL CGT::SOptFile::SaveOptionsData(BOOL i_bImplementation, HDATA i_hOptData, 
									GUID const& i_quidCodeGenClassId, LPCTSTR i_strPluginName)
{
	CString rcFileName = GetPathname();

	// check if there is no opened opts file
	if( rcFileName.IsEmpty() ) 
	{
		rcFileName = BrowseForNewOptPathname(i_bImplementation);

		// user clicked cancel
		if(rcFileName.IsEmpty())
			return FALSE;

		if(::IOSFileExists(rcFileName) == IOS_ERR_OK)
		{
			// if file can not be loaded as dtree and user does not want to override it
			if(!Load(rcFileName, i_bImplementation) &&
				AfxMessageBox(IDS_ERR_DIFFERENTTYPES, MB_YESNO) != IDYES)
				return FALSE;
		}
	}

	UpdateOptionsTree(i_bImplementation, i_hOptData, i_quidCodeGenClassId, i_strPluginName);

	if(!SaveAs(rcFileName))
	{
		CString rcString;
		rcString.Format( IDS_ERR_WRITEFILE, GetPathname() );
		::AfxMessageBox( rcString );
		return FALSE;
	}

	return TRUE;
}

// getting classid property from hdata
BOOL CGT::SOptFile::GetClassId(HDATA i_hData, CLSID& o_oClsId)
{
	if(!i_hData)
		return FALSE;
	
	HPROP hCGClsIDProp = 
		::DATA_Find_Prop( i_hData, NULL, VE_PID_CGIOGENERATORCLSID );
	
	if ( !hCGClsIDProp ) 
		return FALSE;
	
	::PROP_Get_Val( hCGClsIDProp, &o_oClsId, sizeof(o_oClsId) );
	
	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//---
BOOL CGT::RegisterCOM( const TCHAR *pFullPath ) 
{
	BOOL bResult = TRUE;
	HINSTANCE hCOMLib = ::LoadLibraryEx( pFullPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	
	if ( hCOMLib ) {
		HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
		(FARPROC&)lpDllEntryPoint = ::GetProcAddress( hCOMLib, _T("DllRegisterServer") );
		
		if ( lpDllEntryPoint )
			bResult = SUCCEEDED(lpDllEntryPoint());
		
		::FreeLibrary( hCOMLib );
	}
	
	return bResult;
}

// ---
BOOL CGT::UnregisterCOM( const TCHAR *pFullPath ) 
{
	BOOL bResult = TRUE;
	HINSTANCE hCOMLib = ::LoadLibraryEx( pFullPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	
	if ( hCOMLib ) {
		HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
		(FARPROC&)lpDllEntryPoint = ::GetProcAddress( hCOMLib, _T("DllUnregisterServer") );
		
		if ( lpDllEntryPoint )
			bResult = SUCCEEDED(lpDllEntryPoint());
		
		::FreeLibrary( hCOMLib );
	}
	
	return bResult;
}

// ---
IPragueCodeGen* CGT::LoadGenerator(GUID i_quidClsId)
{
	HRESULT hRes;
	return (IPragueCodeGen*) ::COMLoadCOMObjectID( i_quidClsId, IID_IPragueCodeGen, &hRes );
}

// ---
CString CGT::GetPluginName(HDATA i_hImpData)
{
	CString strPluginName;
	if(i_hImpData)
	{
		HPROP hPluginName = ::DATA_Find_Prop( i_hImpData, NULL , VE_PID_PL_PLUGINNAME );
		if ( hPluginName ) 
		{
			char* szPluginName = ::GetPropValueAsString( hPluginName, NULL );
			strPluginName = szPluginName;
			if(szPluginName)
				delete szPluginName;
		}
	}
	
	return strPluginName;
}

BOOL CGT::GetPublicDirProperty(CString& o_strOutputDir, HDATA i_hData)
{
	o_strOutputDir.Empty();

	if(NULL == i_hData)
		return FALSE;

	HPROP hOutputDir = DATA_Find_Prop(i_hData, NULL, VE_PID_CGPI_PUBLICDIR);
	
	if(NULL == hOutputDir)
		return FALSE;

	PROP_Get_Val(hOutputDir, o_strOutputDir.GetBufferSetLength(1024), 1024);			
	o_strOutputDir.ReleaseBuffer();

	return TRUE;
}

BOOL CGT::GetPrivateDirProperty(CString& o_strOutputDir, HDATA i_hData)
{
	o_strOutputDir.Empty();

	if(NULL == i_hData)
		return FALSE;

	HPROP hOutputDir = DATA_Find_Prop(i_hData, NULL, VE_PID_CGPI_PRIVATEDIR);
	
	if(NULL == hOutputDir)
		return FALSE;

	PROP_Get_Val(hOutputDir, o_strOutputDir.GetBufferSetLength(1024), 1024);			
	o_strOutputDir.ReleaseBuffer();

	return TRUE;
}

//=====================================================================================//
// general hdata & hprop methods

BOOL PROP_CompareWithValue(HPROP i_hProp, void* i_pVal) 
{
	if ( !i_hProp || !i_pVal )
		return FALSE;	
		
	switch(  ((AVP_Property*)i_hProp)->type  ) 
	{			  
	case avpt_nothing : return TRUE;
	case avpt_char    : return ((AVP_CHAR_Property*)i_hProp)->val     == *((AVP_char*)i_pVal);
	case avpt_wchar   : return ((AVP_WCHAR_Property*)i_hProp)->val    == *((AVP_wchar*)i_pVal);
	case avpt_short   : return ((AVP_SHORT_Property*)i_hProp)->val    == *((AVP_short*)i_pVal);
	case avpt_long    : return ((AVP_LONG_Property*)i_hProp)->val     == *((AVP_long*)i_pVal);
	case avpt_byte    : return ((AVP_BYTE_Property*)i_hProp)->val     == *((AVP_byte*)i_pVal);
	case avpt_group   : return ((AVP_GROUP_Property*)i_hProp)->val    == *((AVP_group*)i_pVal);
	case avpt_word    : return ((AVP_WORD_Property*)i_hProp)->val     == *((AVP_word*)i_pVal);
	case avpt_dword   : return ((AVP_DWORD_Property*)i_hProp)->val    == *((AVP_dword*)i_pVal);
	case avpt_qword   : return ((AVP_QWORD_Property*)i_hProp)->val    == *((AVP_qword*)i_pVal);
	case avpt_longlong: return ((AVP_LONGLONG_Property*)i_hProp)->val == *((AVP_longlong*)i_pVal);
	case avpt_bool    : return ((AVP_BOOL_Property*)i_hProp)->val     == *((AVP_bool*)i_pVal);
	case avpt_int     : return ((AVP_INT_Property*)i_hProp)->val      == *((AVP_int*)i_pVal);
	case avpt_uint    : return ((AVP_UINT_Property*)i_hProp)->val     == *((AVP_uint*)i_pVal);

	case avpt_date    : return !memcmp(&((AVP_DATE_Property*)i_hProp)->val, i_pVal, sizeof(AVP_date));
	case avpt_time    : return !memcmp(&((AVP_TIME_Property*)i_hProp)->val, i_pVal, sizeof(AVP_time));
	case avpt_datetime: return !memcmp(&((AVP_DATETIME_Property*)i_hProp)->val, i_pVal, sizeof(AVP_datetime));
	case avpt_str     : return !strcmp(((AVP_STR_Property*)i_hProp)->val,     (AVP_str) i_pVal);
	case avpt_wstr    : return !wcscmp((wchar_t*)((AVP_WSTR_Property*)i_hProp)->val,    (wchar_t*) i_pVal);

	case avpt_bin     : 
		return FALSE;
	}

	return FALSE;
}
	
//---
HDATA	CGT::FirstOneLevelDownData(HDATA i_hRoot, DWORD i_dwPropId, void* i_pPropValue)
{
	if(!i_hRoot || !i_pPropValue)
		return NULL;
	
	HDATA hCurrData = ::DATA_Get_First( i_hRoot, NULL );
	
	while ( hCurrData ) 
	{
		HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL , i_dwPropId );
		if ( hProp && PROP_CompareWithValue(hProp, i_pPropValue)) 
			return hCurrData;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	
	return NULL;
}
