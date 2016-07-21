// det_act.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "job_thread.h"

#define _SERVICE_IMP_
#include "stuff/servfuncs.h"


CLog m_LogBL(_T("global.bl.log"), NULL, DCB_BL, DL_MAX, DCB_BL, DL_IMPORTANT);
//CLog m_LogDriver(_T("global.driver.log"), &m_LogBL, DCB_DRIVER | DCB_SYSTEM);
CLog m_Log(_T("global.all.log"), &m_LogBL, DCB_ALL, DL_MAX);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	DbPrint((DCB_SYSTEM, DL_IMPORTANT, _T("Activity Detector: compiled at ") _T(__DATE__) _T(" ") _T(__TIME__)));

	CJob job;
	if (job.Start())
	{
		job.ChangeActiveStatus(true);

		MessageBox(NULL, _T("working..."), _T("Activity detector"), 0);
		
		job.ChangeActiveStatus(false);
	}

	return 0;
}



