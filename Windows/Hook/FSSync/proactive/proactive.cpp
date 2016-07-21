// proactive.cpp : SAMPLE
//

/*#include "job_thread.h"

int main(int argc, char* argv[])
{
	OutputDebugString(L"starting...\n");
	CJob* pjob = new CJob(NULL);
	
	if (pjob)
	{
		if (pjob->Start())
		{
			OutputDebugString(L"started\n");
			pjob->ChangeActiveStatus(true);

			MessageBox(NULL, L"ready to kill!", L"protest", 0);
			
			OutputDebugString(L"stopping\n");
			pjob->ChangeActiveStatus(false);
		}

		delete pjob;
	}

	OutputDebugString(L"stopped\n");
	return 0;
}
*/