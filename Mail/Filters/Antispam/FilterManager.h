#ifndef _FILTER_MANAGER_H_
#define _FILTER_MANAGER_H_

#include <Prague/prague.h> 
#include <Prague/iface/i_csect.h> 
#include <Prague/plugin/p_win32loader.h>

#include <Mail/iface/i_antispamfilter.h>
#include <Mail/plugin/p_antispamtask.h>
#include <Mail/structs/s_antispam.h>

#include <ProductCore/iface/i_taskmanager.h> 

#include "../../ThreadImp.h"

class CFilterManager : public CThreadImp
{
public:
	CFilterManager(hOBJECT hParent);
	virtual ~CFilterManager();

	virtual tERROR _Run();
	
	hANTISPAMFILTER GetFilter();
	void ReleaseFilter();


protected:
	hANTISPAMFILTER CreateFilter();
	void DestroyFilter();
	void StartTimeout();
	
private:
	hOBJECT m_hParent;
	hANTISPAMFILTER m_hAntispamFilter;
	tDWORD m_dwCounter;
	HANDLE m_hStartTimeout;
	HANDLE m_hStopTimeout;
	hCRITICAL_SECTION m_csSettings, m_csFilter;
	tBOOL m_bValid;
	tBOOL m_bStarted;
	tBOOL m_bCanDestroy;

public:
	tERROR GetStatistics ( cSerializable* p_statistics );
	tERROR SetSettings ( const cSerializable* p_pSettings );
	tERROR GetSettings ( cSerializable* p_pSettings );
	tERROR SetState ( tTaskRequestState p_state );
	tERROR Process ( tDWORD* result, cSerializable* p_pProcessParams, hOBJECT p_hObjectToCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics );
	tERROR Train ( cSerializable* p_pTrainParams, hOBJECT p_hObjectForCheck, tDWORD p_dwTimeOut, cSerializable* p_pStatistics );
private:
	tTaskState m_state;
	cAntispamStatistics m_statistics;
	cAntispamStatistics m_statistics_old;
	cAntispamSettings m_settings;
};

class cAutoFilter
{
public:
	cAutoFilter(CFilterManager& f): m_f(f) {m_F = m_f.GetFilter();}
	virtual ~cAutoFilter() {m_f.ReleaseFilter();}
	hANTISPAMFILTER GetFilter() {return m_F;}
private:
	CFilterManager& m_f;
	hANTISPAMFILTER m_F;
};

#endif//_FILTER_MANAGER_H_