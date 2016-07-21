// Processes.cpp: implementation of the CProcesses class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KLGuard2.h"
#include "Processes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcesses::CProcesses()
{
	gpProcList->SetSandBoxPalcement(gpProcList->GetSandBoxSettings()->GetSandBoxPathUpper());
}

CProcesses::~CProcesses()
{
}

bool CProcesses::Start()
{
	bool bRet = true;
	
	bRet &= m_JobRegistry.Start();
	bRet &= m_Files.Start();
	bRet &= m_Self.Start();
	bRet &= m_JobProcesess.Start();
	if (bRet == false)
		return false;

	gpProcList->m_hDevice = m_Files.m_hDevice;
	gpProcList->m_AppID_JobFiles = m_Files.m_AppID;
	gpProcList->m_AppID_Processes = m_JobProcesess.m_AppID;
	gpProcList->GS_SettingsChanged();
	
	bRet = m_JobProcesess.ChangeActiveStatus(true);
	bRet &= m_JobRegistry.ChangeActiveStatus(true);
	bRet &= m_Files.ChangeActiveStatus(true);
	bRet &= m_Self.ChangeActiveStatus(true);

	if (bRet == false)
		Stop();

	return bRet;
}

void CProcesses::Stop()
{
	m_JobProcesess.ChangeActiveStatus(false);
	m_JobRegistry.ChangeActiveStatus(false);
	m_Files.ChangeActiveStatus(false);
	m_Self.ChangeActiveStatus(false);
}
