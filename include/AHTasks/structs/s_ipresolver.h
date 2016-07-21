#ifndef __s_ipresolver
#define __s_ipresolver

#include <Prague/pr_cpp.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>

#include <AHTasks/plugin/p_resip.h>

#include <ProductCore/structs/s_ip.h>

enum _eResIP_IID
{
    _eiid_resolve_result = 3,
};

struct cIPResolveResult : public cSerializable
{
	cIPResolveResult() : m_IPs(),
			m_HostName(),
			m_RequestID(0)
	{
		m_HostName.setCP(cCP_UNICODE);
	}

	DECLARE_IID(cIPResolveResult, cSerializable, PID_RESIP, _eiid_resolve_result);

	tDWORD				m_RequestID;
	cStringObj			m_HostName;
	cVector<cIP>		m_IPs;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cIPResolveResult, "IPResolveResult")
	SER_VALUE_M(RequestID,		tid_DWORD)
	SER_VALUE_M(HostName,		tid_STRING_OBJ)
    SER_VECTOR(m_IPs,			cIP::eIID,		"IPs")
IMPLEMENT_SERIALIZABLE_END();

#endif // __s_ipresolver
