#include <std/base/klstd.h>
#include "componentinstancebaseimpl.h"

#define KLCS_MODULENAME L"KLPRCI"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPRCI;
using namespace KLPAR;

namespace KLPRCI
{
	CInstances::CInstances()
	{
		;
	};

	CInstances::~CInstances()
	{
		;
	};

	void CInstances::Clear()
	{
		for(;;)
		{
            ComponentInstancePtr pInst;
            {
                KLCS_GLOBAL_MODULE_LOCK(acs);
                if(m_InstancesSoap.empty())
                    break;
			    instances_t::iterator it=m_InstancesSoap.begin();
                pInst = it->second;
			    m_InstancesSoap.erase(it);
            };
            pInst = NULL;
		};
		for(;;)
		{
            ComponentInstancePtr pInst;
            {
                KLCS_GLOBAL_MODULE_LOCK(acs);
                if(m_InstancesTr.empty())
                    break;
			    instances_t::iterator it=m_InstancesTr.begin();
                pInst = it->second;
			    m_InstancesTr.erase(it);
            };
            pInst = NULL;
		};
	};

	bool CInstances::addComponentForSoap(const std::wstring& ID, ComponentInstanceBase* pInstance)
	{
		KLSTD_CHKINPTR(pInstance);
		KLSTD_CHK(ID, !ID.empty());
        
		KLCS_GLOBAL_MODULE_LOCK(acs);
		instances_t::iterator it=m_InstancesSoap.find(ID);
		if( it != m_InstancesSoap.end())
			return false;		
        m_InstancesSoap.insert(instances_t::value_type(
                                        ID,
                                        ComponentInstancePtr(pInstance)));
		return true;
	};
	
	void CInstances::getComponentForSoap(const std::wstring& ID, ComponentInstanceBase** ppInstance)
	{
		KLSTD_CHKOUTPTR(ppInstance);
		KLSTD_CHK(ID, !ID.empty());

		KLCS_GLOBAL_MODULE_LOCK(acs);
		instances_t::iterator it=m_InstancesSoap.find(ID);
		if(it == m_InstancesSoap.end())
            KLERR_MYTHROW1(ERR_INSTANCE_UNAVAIL, ID.c_str());
		it->second.m_T.CopyTo(ppInstance);
	};

	bool CInstances::removeComponentForSoap(const std::wstring& ID)
	{
		KLSTD_CHK(ID, !ID.empty());
        ComponentInstancePtr pInst;
        {
            KLCS_GLOBAL_MODULE_LOCK(acs);
		    instances_t::iterator it=m_InstancesSoap.find(ID);
		    if(it == m_InstancesSoap.end())
			    return false;
		    pInst = it->second;
		    m_InstancesSoap.erase(it);
        };
        pInst = NULL;
		return true;
	};

	void CInstances::addComponentForTransport(const std::wstring& ID, ComponentInstanceBase* pInstance)
	{
		KLSTD_CHKINPTR(pInstance);
		KLSTD_CHK(ID, !ID.empty());

		KLCS_GLOBAL_MODULE_LOCK(acs);
		instances_t::iterator it=m_InstancesTr.find(ID);
		if( it != m_InstancesTr.end())
			KLSTD_THROW(STDE_EXIST);
		m_InstancesTr.insert(instances_t::value_type(
                                ID,
                                ComponentInstancePtr(pInstance)));
	};

	CInstances::ComponentInstancePtr CInstances::getComponentForTransport(const std::wstring& ID)
	{
		KLSTD_CHK(ID, !ID.empty());
        ComponentInstancePtr pInst;
        {
            KLCS_GLOBAL_MODULE_LOCK(acs);
		    instances_t::iterator it=m_InstancesTr.find(ID);
		    if( it == m_InstancesTr.end())
			    KLSTD_THROW(STDE_NOTFOUND);
		    pInst = it->second;
        };
        return pInst;
	};

	bool CInstances::removeComponentForTransport(const std::wstring& ID)
	{
		KLSTD_CHK(ID, !ID.empty());
        ComponentInstancePtr pInst;
        {
            KLCS_GLOBAL_MODULE_LOCK(acs);
		    instances_t::iterator it=m_InstancesTr.find(ID);
		    if(it == m_InstancesTr.end())
			    return false;
            pInst = it->second;
		    m_InstancesTr.erase(it);
        };
        pInst = NULL;
		return true;
	};

	int CInstances::getComponentsNumber()
	{
		KLCS_GLOBAL_MODULE_LOCK(acs);
		return m_InstancesTr.size();
	};

	CInstances g_Instances;
};
