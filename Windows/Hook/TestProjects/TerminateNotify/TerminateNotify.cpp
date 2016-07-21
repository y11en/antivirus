// TerminateNotify.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "jobs.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	Job_Process	m_JobProcesses;

	m_JobProcesses.Start();
	m_JobProcesses.ChangeActiveStatus(true);

	SuspendThread(GetCurrentThread());

	m_JobProcesses.ChangeActiveStatus(false);

	return 0;
}



