// MBRLog.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "jobs.h"

int main(int argc, char* argv[])
{
	Job_Process	m_JobProcesses;

	m_JobProcesses.Start();
	m_JobProcesses.ChangeActiveStatus(true);

	SuspendThread(GetCurrentThread());

	m_JobProcesses.ChangeActiveStatus(false);
	return 0;
}
