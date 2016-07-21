#ifndef RV_CUSTOM_COMMAND_INTERFACE_H
#define RV_CUSTOM_COMMAND_INTERFACE_H

//////////////////////////////////////////////////////////////////////////////

#define RV_CUSTOM_COMMAND_INTERFACE_ID "{EC4D00AA-909F-4106-A2AB-5C3CAF0789A2}"

interface IRVCustomCommand : public IUnknown
{
public:
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE Init( 
        PVOID pfMalloc,
        PVOID pfFree) = 0;
    
    virtual /* [local][helpstring] */ void STDMETHODCALLTYPE Done( void) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CustomCommand( 
        HWND hwndParent,
        LPCTSTR pszData,
        DWORD dwResultCode,
		DWORD dwCommandIndex) = 0;

    virtual /* [local][helpstring] */ PVOID STDMETHODCALLTYPE GetCommandBitmap( 
		DWORD dwCommandIndex) = 0;
    
};

#endif