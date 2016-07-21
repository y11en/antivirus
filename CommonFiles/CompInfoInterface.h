////////////////////////////////////////////////////////////////////
//
//  CompInfoInterface.h
//  AVP Component info COM interface
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __COMPINFOINTERFACE_H__
#define __COMPINFOINTERFACE_H__

#include <windows.h>
#include <tchar.h>
#include <unknwn.h>

#define COMP_INFO_CLASS_ID		 _T("{63BC05B0-0AD7-11D4-96B1-00D0B71DDDF6}")
#define COMP_INFO_INTERFACE_ID _T("{63BC05AF-0AD7-11D4-96B1-00D0B71DDDF6}")

typedef enum __MIDL___MIDL_itf_AvpCompInfoDll_0000_0001  {	
		aim_ApplicationID	= 0x1,
		aim_ApplicationName	= 0x2,
		aim_LocaleInfo	= 0x4,
		aim_ApplicationVersion	= 0x8,
		aim_BasesUpdate	= 0x10,
		aim_CopyRights	= 0x20,
		aim_Registration	= 0x40,
		aim_Keys	= 0x80,
		aim_Modules	= 0x100,
		aim_SplashBmp	= 0x200,
		aim_BasesUpdateAndViruses	= 0x400,
		aim_BasesViruses	= 0x800,
		aim_ExcludeSplashBmp	= 0x80000000,
		aim_AllInformation	= 0xffffffff
}	AboutInfoMask;
	


interface IAvpCompInfo : public IUnknown
{
public:
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CollectAboutInfoH( 
        /* [in] */ DWORD dwComponentType,
        /* [out][in] */ long __RPC_FAR *phInfoData) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CollectAboutInfoS( 
        /* [in] */ DWORD dwComponentType,
        /* [out] */ DWORD __RPC_FAR *pdwSize,
        /* [size_is][size_is][out] */ BYTE __RPC_FAR *__RPC_FAR *pvInfoBuffer) = 0;
    
    virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CollectAboutInfoHEx( 
        /* [in] */ DWORD dwComponentType,
        /* [out][in] */ long __RPC_FAR *phInfoData,
        /* [in] */ AboutInfoMask dwInfoMask) = 0;

    virtual /* [local][helpstring] */ void STDMETHODCALLTYPE Init( 
		/* [in] */ PVOID pfMalloc,
		/* [in] */ PVOID pfFree) = 0;

		virtual /* [local][helpstring] */ BOOL STDMETHODCALLTYPE CollectAboutInfoSEx( 
		/* [in] */ DWORD dwComponentType,
		/* [out] */ DWORD __RPC_FAR *pdwSize,
		/* [size_is][size_is][out] */ BYTE __RPC_FAR *__RPC_FAR *pvInfoBuffer,
		/* [in] */ AboutInfoMask dwInfoMask) = 0;
};


#endif // __COMPINFOINTERFACE_H__