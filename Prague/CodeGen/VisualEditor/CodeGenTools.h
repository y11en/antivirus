#ifndef _CodeGenTools_h_included
#define _CodeGenTools_h_included

namespace CGT
{
	// wizard states
	enum 
	{
		EAutoBackButton = 0,
		EAutoNextButton,
		EAutoFinishButton,
		EAutoSelectInterfacesDlg, 
		EAutoOptionsDlg, 
		EAutoSummaryDlg 
	};

	//---------------------------------------------------------------------------//
	struct SOptFile
	{
		SOptFile();
		~SOptFile();
		
		BOOL	IsLoaded			();
		BOOL	Load				(LPCTSTR i_strOptPathname, BOOL i_bImplementation);
		BOOL	SaveAs				(LPCTSTR i_strNewName);
		void	Close				();
		HDATA	GetCodeGenOptions	(CLSID const& i_rClsId);
		HDATA	GetPluginOptions	(CLSID const& i_rClsId, LPCTSTR i_strPlugin, BOOL i_bPluginRoot = FALSE);
		BOOL	SaveOptionsData		(BOOL i_bImplementation, HDATA i_hOptData, 
									GUID const& i_quidCodeGenClassId, LPCTSTR i_strPluginName);
			
		static	
		CString BrowseForNewOptPathname(BOOL i_bImplementation);

		operator HDATA		();
		LPCTSTR	 GetPathname();
	private:
		HDATA m_hOptRoot;
		TCHAR m_strOptPathname[1024];

		void UpdateOptionsTree(BOOL i_bImplementation, HDATA i_hOptData, 
								GUID const& i_quidCodeGenClassId, LPCTSTR i_strPluginName);
		void UpdateOutputDirs();
		BOOL	GetClassId		(HDATA i_hData, CLSID& o_oClsId);
	};

	//---------------------------------------------------------------------------//
	struct SCodeGenPlugin
	{
		CString m_strPluginName;
		CString m_strPublicDir;
		CString m_strPrivateDir;
	};

	struct SCodeGenItem
	{
		GUID m_oClsId;
		CString m_strDescription;
		CStringList m_oAttrs;
		CString m_strPublicDir;		// output directory in proto mode for code generator
		CString m_strPrivateDir;	
		CList<SCodeGenPlugin, SCodeGenPlugin&> m_arrPlugins;

		SCodeGenPlugin* AddNewPluginData();
		SCodeGenPlugin* FindPluginData(LPCTSTR i_strPluginName);
	};
	
	struct SCodeGeneratorsSet
	{
	private:
		void FillOneObjectCache(SCodeGenItem& rItem, REFCLSID rCLSID, IPragueCodeGen *pInterface );
		
	public:	
		SCodeGenItem m_CodeGenItemsCache[25];
		BYTE m_nRealCodeGenItems;

		SCodeGeneratorsSet();

		void LoadCodeGenItemsCache	(BOOL i_bForceRecache = FALSE);
		void UpdateGeneratorsOptions(SOptFile& i_oOptFile, LPCSTR i_strDefOutputDir);
		SCodeGenItem* GetGenCacheItem(GUID const& i_roClsid);
	};

	IPragueCodeGen* LoadGenerator(GUID i_quidClsId);
	CString		GetPluginName(HDATA i_hImpData);
	BOOL		GetPrivateDirProperty(CString& o_strPublicDir, HDATA i_hData);
	BOOL		GetPublicDirProperty(CString& o_strPrivateDir, HDATA i_hData);

	//---------------------------------------------------------------------------//
	// find first level down data, that has property with given id and given value
	HDATA	FirstOneLevelDownData(HDATA i_hRoot, DWORD i_dwPropId, void* i_pPropValue);
	
	//---------------------------------------------------------------------------//
	BOOL RegisterCOM( const TCHAR *pFullPath ); 
	BOOL UnregisterCOM( const TCHAR *pFullPath ); 
	
	//---------------------------------------------------------------------------//
	extern SCodeGeneratorsSet	s_CodeGens;
	extern SOptFile				s_OptFile;

} //namespace CGT


#endif