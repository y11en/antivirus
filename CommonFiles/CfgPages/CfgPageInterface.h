#ifndef CFG_PAGE_INTERFACE
#define CFG_PAGE_INTERFACE

//////////////////////////////////////////////////////////////////////////////

#define TLT_STANDARD_TAB		0
#define TLT_COOL_TAB			1

//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

typedef void* (*PF_MEM_ALLOC)(unsigned int nSize);
typedef void (*PF_MEM_FREE)(PVOID pBuffer);

/////////////////////////////////////////////////////////////////////////////

#define AVP_CONFIGURATOR_INTERFACE_ID _T("{3F483B32-A64C-11D3-96B0-00104B5B66AA}")

interface IAVPConfigurator : public IUnknown
{
public:
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetPageCount( 
        DWORD dwType,
        DWORD dwLevel) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE PageCreate( 
        HWND hwndParent,
        DWORD dwPageIndex,
        DWORD dwType,
        DWORD dwLevel) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE DeletePages( 
        HWND __RPC_FAR *phwndPages) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetDataFromPages( 
        PVOID __RPC_FAR *ppBuffer,
        HWND __RPC_FAR *phwndPages) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetDataToPages( 
        PVOID pBuffer,
        DWORD dwSize,
        HWND __RPC_FAR *phwndPages) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE GetHelpInfo( 
        unsigned char __RPC_FAR *pszHelpFile,
        DWORD dwSize,
        DWORD __RPC_FAR *pdwHelpID,
        DWORD dwType,
        DWORD dwLevel,
        DWORD dwPageIndex) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetDefaultPolicy( 
        DWORD dwType,
        PVOID __RPC_FAR *ppBuffer) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE SerializeData( 
        PVOID pSrcData,
        DWORD dwSrcSize,
        PVOID __RPC_FAR *ppDstBuffer) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE Init( 
        PVOID pfMalloc,
        PVOID pfFree) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE GetPageTitle( 
        unsigned char __RPC_FAR *pszTitle,
        DWORD dwSize,
        DWORD dwType,
        DWORD dwLevel,
        DWORD dwPageIndex) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE GetPageDescription( 
        unsigned char __RPC_FAR *pszDescription,
        DWORD dwSize,
        DWORD dwType,
        DWORD dwLevel,
        DWORD dwPageIndex) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetPageIcon( 
        DWORD dwType,
        DWORD dwLevel,
        DWORD dwPageIndex) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetUserInterfaceLevelsCount( 
        DWORD dwType) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE GetUserInterfaceLevelName( 
        DWORD dwLevel,
        unsigned char __RPC_FAR *pszLevelName,
        DWORD dwSize) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE GetUserInterfaceLevelDescription( 
        DWORD dwLevel,
        unsigned char __RPC_FAR *pszLevelDescription,
        DWORD dwSize) = 0;
    
    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetUserInterfaceLevelIcon( 
        DWORD dwLevel) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE ShowReport( 
        HWND hParentWnd,
        PVOID pData,
        DWORD dwSize,
        DWORD dwType,
        LPCTSTR pszTitle,
        LPCTSTR pszRegKeyName,
        PVOID pParam) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE GetPagesState( 
        PVOID __RPC_FAR *ppBuffer,
        DWORD __RPC_FAR *pdwSize,
        HWND __RPC_FAR *phwndPages) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetPagesState( 
        PVOID pBuffer,
        DWORD dwSize,
        HWND __RPC_FAR *phwndPages) = 0;

    virtual /* [local][helpstring] */ long STDMETHODCALLTYPE GetTabLookType( 
        DWORD dwType) = 0;

    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE NotifyUserInterfaceLevelChanging( 
        DWORD dwLevelFrom,
        DWORD dwLevelTo) = 0;
    
    virtual /* [local][helpstring] */ void STDMETHODCALLTYPE NotifyUserInterfaceLevelChanged( 
        DWORD dwLevelCur,
        DWORD dwLevelFrom) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE NotifyPageSelchanging( 
        HWND hwndPageFrom,
        HWND hwndPageTo) = 0;
    
    virtual /* [local][helpstring] */ void STDMETHODCALLTYPE NotifyPageSelchanged( 
        HWND hwndPageCur,
        HWND hwndPageFrom) = 0;

    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetKeyInfo( 
        HWND hParent,
        PVOID pBuffer,
        DWORD dwSize) = 0;

    virtual /* [local][helpstring] */ void STDMETHODCALLTYPE Done( void ) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE SetParentSideInterface( 
        IUnknown __RPC_FAR *pUnknown) = 0;

 };

//////////////////////////////////////////////////////////////////////////////

#define AVP_CUSTOM_COMMAND_INTERFACE_ID _T("{56B6B46F-2104-48DE-8A3A-92AFFBCF2908}")

interface ICCustomCommand : public IUnknown
{
public:
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE Init( 
        PVOID pfMalloc,
        PVOID pfFree) = 0;
    
    virtual /* [local][helpstring] */ void STDMETHODCALLTYPE Done( void) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CustomCommand( 
        HWND hwndParent,
        PVOID pInData,
        DWORD dwInSize,
        PVOID __RPC_FAR *ppOutData,
        DWORD __RPC_FAR *pdwOutSize) = 0;
    
};

#endif