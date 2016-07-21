#ifdef _WIN32

#ifndef __VISTAVIRTHLP_H__
#define __VISTAVIRTHLP_H__

#include <windows.h>

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

class cVistaVirtHlp
{
	bool m_bIsVista;
	cVector<cStrObj> m_VistaVirtualFoders;
	cStrObj m_VistaVirtualStore;
public:
	cVistaVirtHlp();
	cStrObj GetVirtualObject(const cStrObj &Object) const;
};

cVistaVirtHlp *GetVistaVirtHlp();
void ReleaseVistaVirtHlp();

#endif // __VISTAVIRTHLP_H__

#endif // _WIN32