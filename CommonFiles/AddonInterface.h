////////////////////////////////////////////////////////////////////
//
//  AddonInterface.h
//  AVP addon COM interface
//  AVP addition scanning components stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2001
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __ADDONINTERFACE_H__
#define __ADDONINTERFACE_H__

#include <windows.h>
#include <unknwn.h>

#define ADDON_INTERFACE_ID    _T("{741807CA-DFC2-416A-8F97-8192B4F02A22}")
#define STARTUP_SCANNER_ADDON _T("{2F85CB83-768A-489C-A28A-762A9674237A}")

typedef enum __MIDL___MIDL_itf_AvpAddon_0000_0001	{	
	opLOCKED					= 0,
	opIOERROR					= opLOCKED + 1,
	opNO_ACCESS				= opIOERROR + 1,
	opDISKOUTOFSPACE	= opNO_ACCESS + 1,
	opCORRUPTED				= opDISKOUTOFSPACE + 1,
	opPROTECTED				= opCORRUPTED + 1,
	opOK							= opPROTECTED + 1,
	opDISINF_FAILED		= opOK + 1,
	opDISINFECTED			= opDISINF_FAILED + 1,
	opRENAMED					= opDISINFECTED + 1,
	opDELETED					= opRENAMED + 1,
	opINFECTED				= opDELETED + 1,
	opENCRYPTED_BY		= opINFECTED + 1,
	opPACKED					= opENCRYPTED_BY + 1,
	opARCHIVE					= opPACKED + 1,
	opWARNING					= opARCHIVE + 1,
	opSUSPICION				= opWARNING + 1,
	opUNKNOWN					= opSUSPICION + 1,
	opVIRUSNAME				= opUNKNOWN + 1,
	opRENAMED_REBOOT	= opVIRUSNAME + 1,
	opDELETED_REBOOT	= opRENAMED_REBOOT + 1,
	opQUARANTINED			= opDELETED_REBOOT + 1,
	opSECONDPASS			= opQUARANTINED + 1
}	OpType;


#define FACILITY_AVP_ADDON 0xc00

#define ERR_ALREADY_PROCESSED(opType)    MAKE_HRESULT(SEVERITY_ERROR,FACILITY_AVP_ADDON,opType)

interface IAvpAddonObject : public IUnknown	{
  public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFirstObject( 
			/* [string][out] */ BYTE __RPC_FAR *__RPC_FAR *pObjectName) = 0;
			
			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextObject( 
			/* [string][out] */ BYTE __RPC_FAR *__RPC_FAR *pObjectName) = 0;
			
			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoOperationBegin( 
			/* [string][in] */ BYTE __RPC_FAR *pObjectName,
			/* [out][in] */ OpType __RPC_FAR *pnOperation,
			/* [out] */ DWORD __RPC_FAR *pdwResultCount) = 0;
			
			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoOperationEnd( 
			/* [string][in] */ BYTE __RPC_FAR *pObjectName) = 0;
			
			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetOpResultString( 
			/* [in] */ DWORD dwResIndex,
			/* [in] */ DWORD dwStrIndex,
			/* [string][out] */ BYTE __RPC_FAR *__RPC_FAR *pResultString) = 0;
			
			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetOpResult( 
			/* [in] */ DWORD dwResIndex,
			/* [out][in] */ OpType __RPC_FAR *pnOperation) = 0;
};

#endif // __ADDONINTERFACE_H__