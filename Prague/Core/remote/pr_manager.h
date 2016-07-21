/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_client.h
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Спецификация интерфейса для Prague-proxies.
 * 
 */
//************************************************************************

#ifndef __pr_manager_h
#define __pr_manager_h

#define PR_REMOTE_MANAGER_PROP	"PR_REMOTE_MANAGER_PROP"

struct	cSerializable;

class PRRemoteManager
{
public:
	virtual void		RegisterIFace(tIID iid, tPTR vtbl, tDWORD vtbl_size, iINTERNAL *internal_table, tDWORD data_size, tIID iid_compat)=0;

	virtual tERROR      Invoke(hOBJECT proxy, tDWORD flags, tDWORD func, tDWORD* types, va_list args)=0;
	virtual tERROR      InvokeEx(hOBJECT proxy, tDWORD flags, tDWORD func, ...)=0;
};

extern PRRemoteManager	* g_manager;

//************************************************************************

#define prf_IMPERSONATE		 0x00000001

#define prf_REF              0x01000000
#define prf_DEREF            0x02000000
#define prf_IN               0x04000000
#define prf_OUT              0x08000000
#define prf_OBJECT           tid_QWORD
#define prf_INBUFF           0x00100000
#define prf_SIZEPTR          0x00200000

#define PR_ARG0()                  0
#define PR_ARG1(a1)                a1,0
#define PR_ARG2(a1,a2)             a1,a2,0
#define PR_ARG3(a1,a2,a3)          a1,a2,a3,0
#define PR_ARG4(a1,a2,a3,a4)       a1,a2,a3,a4,0
#define PR_ARG5(a1,a2,a3,a4,a5)    a1,a2,a3,a4,a5,0
#define PR_ARG6(a1,a2,a3,a4,a5,a6) a1,a2,a3,a4,a5,a6,0
#define PR_ARG7(a1,a2,a3,a4,a5,a6,a7) a1,a2,a3,a4,a5,a6,a7,0

#define PR_PROXY_BEGIN(iface) static i##iface##Vtbl g_##iface;

#define PR_INVOKE(iface, func, _types_, _args_) \
	g_manager->InvokeEx((hOBJECT)_this, 0, offsetof(i##iface##Vtbl,func)/sizeof(tPTR), _types_, _args_)

#define PR_PROXY_EX(iface, func, _flags, _types_) \
		static struct iface##func { iface##func(){ g_##iface.func=(fnp##iface##_##func)proxy; } \
			static tERROR proxy(hOBJECT obj, ...) { static tDWORD types[] = {_types_}; va_list argptr; va_start(argptr, obj); \
				tERROR err = g_manager->Invoke(obj, _flags, offsetof(i##iface##Vtbl,func)/sizeof(tPTR), types, argptr); \
				va_end(argptr); return err; } } g_##iface##func;
#define PR_DUMMY(...) __VA_ARGS__
#define PR_PROXY(iface, func, _types_) PR_PROXY_EX(iface, func, 0, PR_DUMMY(_types_))
#define PR_SCPRX(iface, func, _types_) PR_PROXY_EX(iface, func, prf_IMPERSONATE, PR_DUMMY(_types_))

#define PR_PROXY_IMPLEMENT(iface, iid, internal, useddata, cm) \
	void PRRegisterProxy##iid (){ g_manager->RegisterIFace(iid, &g_##iface, sizeof(g_##iface), internal, useddata, cm); }

#define PR_PROXY_END(iface, iid) \
	void PRRegisterProxy##iid (){ g_manager->RegisterIFace(iid, &g_##iface, sizeof(g_##iface), 0, 0, 0); }

#define PR_PROXY_END_CM(iface, iid, cm) \
	void PRRegisterProxy##iid (){ g_manager->RegisterIFace(iid, &g_##iface, sizeof(g_##iface), 0, 0, cm); }

#define PR_REGISTER_PROXY(iid) PRRegisterProxy##iid();

#define PR_SYS_SIZE				11
#define PR_USE_DATA(type)		type *data = ((type*)(((tDWORD**)_this)[2]+PR_SYS_SIZE));

//************************************************************************

#endif // __pr_manager_h
