// ABController.cpp : Implementation of CABController
#include "stdafx.h"
#include "ABController.h"

#define PR_IMPEX_DEF

#include <prague/pr_remote.h>
#include <prague/iface/i_root.h>
#include <plugin/p_gui.h>
#include <structs/s_gui.h>

hROOT g_root = NULL;
HINSTANCE g_hABController = NULL;

STDMETHODIMP CABController::DoCommand(BSTR pVal)
{
	CPRRemotePrague	g_remotePrague(g_hABController);
	if( PR_FAIL( g_remotePrague.GetError() ) )
		return E_FAIL;

	CPRRemoteProxy<cTaskManager*> pProxy(g_remotePrague, "TaskManager");
	tERROR err = g_remotePrague.LoadProxyStub("pxstub.ppl");
	if ( PR_FAIL(err) )
		return E_FAIL;

	//подготавливаем строку
	cGuiDoCommand request;
	//request.m_strCommand = bstrString;
	request.m_strCommand = pVal;
	
	if( PR_FAIL(err) || !pProxy.Attach())
		return E_FAIL;
	err = pProxy->sysSendMsg( pmc_BL_DO_COMMAND, msg_BL_DO_COMMAND, NULL, &request, SER_SENDMSG_PSIZE);
	if ( PR_FAIL(err) )
		return E_FAIL;

	return S_OK;
}

// CABController