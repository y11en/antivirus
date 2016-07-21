// Stat.h : Declaration of the CStat

#ifndef __STAT_H_
#define __STAT_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CStat
class ATL_NO_VTABLE CStat : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CStat, &CLSID_Stat>,
	public IDispatchImpl<IStat, &IID_IStat, &LIBID_INVENTORYSCUMLib>
{
public:
	CStat()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_STAT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CStat)
	COM_INTERFACE_ENTRY(IStat)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IStat
public:
	STDMETHOD(get_dropped_conn_count)(/*[out, retval]*/ long* nConn);
	STDMETHOD(get_current_conn_count)(/*[out, retval]*/ long* nConn);
	STDMETHOD(get_drop_count)(/*[out, retval]*/ __int64* nVal);
	STDMETHOD(get_pass_count)(__int64* nVal);
};

#endif //__STAT_H_
