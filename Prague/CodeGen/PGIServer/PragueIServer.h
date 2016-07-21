// PragueIServer.h : Declaration of the CPragueIServer

#ifndef __PRAGUEISERVER_H_
#define __PRAGUEISERVER_H_

#include "resource.h"       // main symbols
#include <property/property.h>
#include <Stuff\SArray.h>
#include <Stuff\PArray.h>
#include <Stuff\PathStr.h>

// ---
struct CNodeUID {
	TCHAR m_UID[41];
	CNodeUID()	{ memset(m_UID, 0, sizeof(m_UID)); }
	CNodeUID( TCHAR *pUID, DWORD dwSize)	{ 
		memset(m_UID, 0, sizeof(m_UID)); 
		memcpy(m_UID, pUID, min(dwSize, sizeof(m_UID) - 1));
		if ( lstrlen(m_UID) < sizeof(m_UID) - 1 )
			memset(m_UID + lstrlen(m_UID), 0, sizeof(m_UID) - lstrlen(m_UID));
	}
	operator LPSTR () { return m_UID; }
	operator LPCSTR () { return m_UID; }
};

// ---
class CAliasContainer : public CSArray<CNodeUID> {
};


#define FOLDER_INDEX(iT) ((iT==itAny || iT==itByContainer) ? 0 : int(iT))

class cMyPathStr : public CPathStr {
public :
	void operator delete(void* p) { CPathStr::operator delete (p); }
	TCHAR* operator =( const TCHAR *other ) { return CPathStr::operator = (other); }
};

/////////////////////////////////////////////////////////////////////////////
// CPragueIServer
class ATL_NO_VTABLE CPragueIServer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPragueIServer, &CLSID_PragueIServer>,
	public IPragueIServer
{

	HWND							m_hWndParent;
	CPArray<cMyPathStr> m_StartingFolders;
	CPArray<TCHAR>    m_Extentions;
	CPathStr					m_rcLastErrorStr;
	CPathStr          m_PrtFolder;
	CPathStr          m_ImpFolder;
	CPathStr          m_DscFolder;
	CSArray<HDATA>    m_iface_cache;

public:
	CPragueIServer();
	~CPragueIServer();

DECLARE_REGISTRY_RESOURCEID(IDR_PRAGUEISERVER)
DECLARE_NOT_AGGREGATABLE(CPragueIServer)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPragueIServer)
	COM_INTERFACE_ENTRY(IPragueIServer)
END_COM_MAP()

	HRESULT EnumFoldersForContainers( LPCTSTR pFolderName, ItType eIType, HDATA hRootData );
	HRESULT LoadAllContainersTree( ItType eIType, HDATA &hRootData );
	HRESULT LoadAllContainersTree(ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces);

	HRESULT LoadAllInterfacesTree(ItType eIType, HDATA &hRootData );
	HRESULT LoadAllInterfacesTree(ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces);

	HRESULT LoadContainerInterfacesTree(LPCTSTR pContainerName, BOOL bLockOnLoad, ItType eIType, HDATA *hRootData, BOOL bFullContainer = FALSE );
	HRESULT LoadContainerInterfacesTree(LPCTSTR pContainerName, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces);
	HRESULT LoadInterfaceTree( CNodeUID &rcUID, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces);
	HRESULT OpenContainerInterfacesTree(LPTSTR pContainerName, BOOL bLockOnLoad, ItType eIType, DWORD *pdwSize, BYTE **ppInterfaces);
	HRESULT SaveContainerInterfacesTree(BOOL bSaveAs, LPTSTR pContainerName, BOOL bLockOnSave, ItType eIType, DWORD dwSize, BYTE *pInterfaces);
	
	HRESULT PerformFileSaveAs( LPTSTR pContainerName, ItType eIType, HDATA hRootData );
	HRESULT SaveInterfaces( LPCTSTR pContainerName, HDATA hRootData );
	HRESULT SaveBakFile( LPCTSTR pContainerName );

	HRESULT SaveInterfacesDB(LPCTSTR pContainerName, DWORD dwSize, BYTE *pInterfaces);

	BOOL		PerformFileOpenDlg( ItType eIType, LPTSTR pContainerName );
	
	HRESULT AttachContainerInterfaces( LPCTSTR pContainerName, HDATA *phRootData, BOOL bSkipLinked, ItType eIType, CAliasContainer &rcAliases, BOOL bFullContainer = FALSE );
	HRESULT AttachContainerInterfacesByAlias( LPCTSTR pContainerName, HDATA *phRootData, ItType eIType, CAliasContainer &rcAliases );
	HRESULT EnumFoldersForInterfacesAliases( LPCTSTR pFolderName, ItType eIType, HDATA *phRootData, CAliasContainer &rcAliases );
	HRESULT EnumFoldersForInterfaces( LPCTSTR pFolderName, ItType eIType, HDATA hRootData, CAliasContainer &rcAliases );

	HRESULT AttachContainerIntegrity( LPCTSTR pContainerName, HDATA hRootData );
	HRESULT EnumFoldersForCheckIntegrity( LPCTSTR pFolderName, HDATA hRootData );

	HRESULT CheckIntegrity();

// IPragueIServer
public:
	STDMETHOD(EnumContainers)(/*[in]*/ ItType eIType, /*[out]*/ DWORD *pdwSize, /*[out,size_is(,*pdwSize)]*/ BYTE **ppContainers);
	STDMETHOD(SetStartingFolder)(/*[in,string]*/ BYTE *pStartingFolder, /*[in]*/ ItType eIType);
	STDMETHOD(LockContainer)(/*[in,string]*/ BYTE *pContainerName, /*[in]*/ BOOL bLock);
	STDMETHOD(SaveDescriptionDB)(/*[in,string]*/ BYTE *pContainerName, /*[in]*/ DWORD dwSize, /*[in,size_is(dwSize)]*/ BYTE *pInterfaces);
	STDMETHOD(LoadInterface)(/*[in]*/ DWORD dwUIDSize, /*[in,size_is(dwUIDSize)]*/ BYTE *pUID, /*[in]*/ BOOL bLockOnLoad, /*[in]*/ ItType eIType, /*[out]*/ DWORD *pdwSize, /*[out,size_is(,*pdwSize)]*/ BYTE **ppInterface);
	STDMETHOD(EnumInterfaces)(/*[in]*/ ItType eIType, /*[out]*/ DWORD *pdwSize, /*[out,size_is(,*pdwSize)]*/ BYTE **ppInterfaces);
	STDMETHOD(SaveContainer)(/*[in,out,string]*/ BYTE **ppContainerName, /*[in]*/ BOOL bSaveAs, /*[in]*/ BOOL bLockOnSave, /*[in]*/ DWORD dwSize, /*[in,size_is(dwSize)]*/ BYTE *pInterfaces);
	STDMETHOD(OpenContainer)(/*[in,out,string]*/ BYTE **ppContainerName, /*[in]*/ BOOL bLockOnLoad, /*[in]*/ ItType eIType, /*[out]*/ DWORD *pdwSize, /*[out,size_is(,*pdwSize)]*/ BYTE **ppInterfaces);
	STDMETHOD(LoadContainer)(/*[in,string]*/ BYTE *pContainerName, /*[in]*/ BOOL bLockOnLoad, /*[out]*/ DWORD *pdwSize, /*[out,size_is(,*pdwSize)]*/ BYTE **ppInterfaces);
	STDMETHOD(GetErrorString)( /*[in]*/ HRESULT hErr, /*[out,string]*/BYTE **pErrString );
	STDMETHOD(Done)();
	STDMETHOD(ReInit)();
	STDMETHOD(Init)();
	STDMETHOD(SetUIParent)(/*[in]*/ HWND hWndParent);
};

void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt );

BOOL			IsLinkedToFile( HDATA hData );
BOOL			IsLinkedByInterface( HDATA hData );
BOOL			IsImported( HDATA hData );
BOOL      IsAlias( HDATA hData );

BOOL			GetNodeUID( HDATA hData, CNodeUID &pvUID );
BOOL			CompareNodesUID( CNodeUID &pvFUID, CNodeUID &pvSUID );

ItType		GetTreeSType( HDATA hData );
AVP_dword IType2WType( int iwType );

AVP_dword CreateDataId( HDATA hParentData );

void *SerializeDTree( HDATA hData, DWORD &nSSize );
HDATA DeserializeDTree( void *pBuffer, DWORD nSize );

void			SetDataPropWithCheck( HDATA hData, AVP_dword dwPID, LPCTSTR pStartingFolder, LPCTSTR pValue );

TCHAR		 *LoadAndFormatString( UINT uiStrID, ... );

BOOL			PerformFileOpenDlg( HWND hWndOwner, LPCTSTR pszInitialDir, LPCTSTR pszFilter, LPCTSTR pszDefExt, TCHAR *pszFileName, int nFileNameSize );
BOOL			PerformFileSaveDlg( HWND hWndOwner, LPCTSTR pszInitialDir, LPCTSTR pszFilter, LPCTSTR pszDefExt, TCHAR *pszFileName, int nFileNameSize );
BOOL            CheckOutFile(LPCTSTR pContainerName);

#endif //__PRAGUEISERVER_H_
