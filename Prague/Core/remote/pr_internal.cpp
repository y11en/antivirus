/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_client.cpp
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Имплементация интерфейса Prague-client.
 * 
 */
//************************************************************************

#include "pr_common.h"
#if defined (_WIN32)
#include <malloc.h>
#endif

#define  __DECLARE_SER_CONSTANTS_ONLY
#include <Prague/pr_serializable.h>

//************************************************************************

// copied from klavaeng.cpp
#ifdef _WIN32

#define _TRY_        __try
#define _EXCEPT_(x)  __except(x)

#else

#define _TRY_
#define _EXCEPT_(x)  if(0)

#endif


tERROR  Stub_ObjectInit( hOBJECT _this )
{
	g_mng.UpdateValidateList(_this, true);
	return errOK;
}

tERROR  Stub_ObjectClose( hOBJECT _this )
{
	PRRemoteObject *l_remote = CUST_OBJ(_this);
	if( l_remote->m_connect )
	{
		_TRY_
		{
			l_remote->m_connect->CloseProxy(l_remote->m_object);
		}
		_EXCEPT_(1)
		{
		}

		if( l_remote->m_is_pidref )
			g_mng.ReleaseProcess(l_remote->m_connect->m_local);
	}

	g_mng.UpdateValidateList(_this, false);

	g_mng.m_stub_lock.lock();
	g_mng.m_stub_lock.unlock();
	return errOK;
}

//************************************************************************

iINTERNAL internal_stub =
{
	NULL,// IntFnRecognize,
	NULL,// IntFnObjectNew,
	Stub_ObjectInit,
	NULL,// IntFnObjectInitDone,
	NULL,// IntFnObjectCheck,
	NULL,// IntFnObjectPreClose,
	Stub_ObjectClose,
	NULL,// IntFnChildNew,
	NULL,// IntFnChildInitDone,
	NULL,// IntFnChildClose,
	NULL,// IntFnMsgReceive,
	NULL// IntFnIFaceTransfer
};

//************************************************************************

tERROR  Proxy_ObjectInit( hOBJECT _this )
{
	g_mng.UpdateValidateList(_this, true);
	return errOK;
}

bool CheckClosedProxy(hOBJECT _this)
{
	hOBJECT parent = CALL_SYS_ParentGet(_this, IID_ANY);
	if( !parent || parent == _this )
		return true;

	if( !IS_OBJ_PROXY(parent) )
		return true;

	if( !CUST_PROXY(parent)->m_stub )
		return false;

	return CheckClosedProxy(parent);
}

tERROR  Proxy_ObjectClose( hOBJECT _this )
{
//	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tProxy closing: object=%08X", _this));

	PRRemoteProxy *l_remote = CUST_PROXY(_this);
	if( l_remote->m_connect )
	{
		if( IS_ROOT_PROXY(_this ) )
			l_remote->m_connect->Close();
		else
		{
			if( CheckClosedProxy(_this) )
				l_remote->m_connect->CloseStub(l_remote->m_stub);
		}
	}
	g_mng.UpdateValidateList(_this, false);

	PRAutoLocker locker(g_mng.m_proxy_lock);
	return errOK;
}

tERROR  Proxy_MsgReceive( hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen )
{
	if( !_this )
	{
		PR_TRACE((g_root, prtERROR, "Proxy_MsgReceive : NULL pointer!!!!!"));
		return errOK;
	}

	if( msg_cls == pmc_REMOTE_PROXY_DISCONECTED )
		return errOK;

	PRRemoteProxy *l_remote = CUST_PROXY(_this);

	if( !l_remote->m_connect || !l_remote->m_connect->IsConnected() )
	{
		PR_TRACE((g_root, prtERROR, "Proxy_MsgReceive (msg_cls=%x, msg_id=%x, pid=%x): no connection",
			msg_cls, msg_id, l_remote->m_connect ? l_remote->m_connect->m_local : 0xffffffff));
		return errOK;
	}

	tERROR _err = errOK;
	if( blen != SER_SENDMSG_PSIZE )
	{
		if( pbuff && blen && IsBadWritePtr(pbuff, 1) )
			pbuff = alloca(*blen);

		if( msg_cls != pmc_REMOTE_GLOBAL || (hOBJECT) g_mng.m_plugin != send_point )
		{
			_err = PR_INVOKE_BASE(ProxyMsgReceive, prf_IMPERSONATE, _this,
				PR_ARG6(tid_DWORD,tid_DWORD,tid_OBJECT,tid_OBJECT,tid_OBJECT,tid_BINARY|prf_SIZEPTR),
				PR_ARG7(msg_cls, msg_id, send_point, ctx, recieve_point, pbuff, blen));
		}
	}
	else
	{
		if( msg_cls != pmc_REMOTE_GLOBAL || (hOBJECT) g_mng.m_plugin != send_point )
		{
			_err = PR_INVOKE_BASE(ProxyMsgReceiveSer, prf_IMPERSONATE, _this,
				PR_ARG6(tid_DWORD,tid_DWORD,tid_OBJECT,tid_OBJECT,tid_OBJECT,tid_SERIALIZABLE),
				PR_ARG6(msg_cls, msg_id, send_point, ctx, recieve_point, pbuff));
		}
	}
	if( _err == errPROXY_STATE_INVALID )
		PR_TRACE((g_root, prtERROR, "Proxy_MsgReceive (msg_cls=%x, msg_id=%x, pid=%x): failed with exception !!!",
			msg_cls, msg_id, l_remote->m_connect->m_local));

	return _err;
}

//************************************************************************

iINTERNAL internal_proxy =
{
	NULL,// IntFnRecognize,
	NULL,// IntFnObjectNew,
	Proxy_ObjectInit,
	NULL,// IntFnObjectInitDone,
	NULL,// IntFnObjectCheck,
	NULL,// IntFnObjectPreClose,
	Proxy_ObjectClose,
	NULL,// IntFnChildNew,
	NULL,// IntFnChildInitDone,
	NULL,// IntFnChildClose,
	Proxy_MsgReceive,
	NULL// IntFnIFaceTransfer
};

//************************************************************************
