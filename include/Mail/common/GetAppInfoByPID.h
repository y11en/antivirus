#ifndef __GETAPPINFOBYPID_H__
#define __GETAPPINFOBYPID_H__

#include <Prague/prague.h>
#include <Prague/iface/i_os.h>

#include <Extract/plugin/p_memmodscan.h>

inline tBOOL GetAppInfoByPid(tDWORD nPID, cStringObj &strImagePath)
{
	if(!strImagePath.empty())
		return cFALSE;

	tBOOL bIs9x = cFALSE;
#ifdef _WIN32
	bIs9x = (GetVersion() & 0x80000000) != 0;
#endif

	hOS hOs = NULL;
	cERROR err = g_root->sysCreateObject((hOBJECT *)&hOs, IID_OS, PID_MEMMODSCAN, 0);
	if(PR_SUCC(err))
	{
		err = hOs->propSetDWord(plPID, nPID);
		if(PR_SUCC(err))
			err = hOs->sysCreateObjectDone();
		if(PR_SUCC(err))
		{
			// creating enumerator
			hObjPtr hPtr = 0;
			err = hOs->PtrCreate(&hPtr, 0);
			if (PR_SUCC(err))
			{
				cStringObj strPath;
				while(PR_SUCC(hPtr->Next()))
				{
					if(PR_FAIL(err = strPath.assign(hPtr, pgOBJECT_PATH)))
						break;
					if(!bIs9x)
						break;
					// search for .exe under 9x
					tDWORD len = strPath.length();
					if(len > 4 && strPath.compare(len - 4, 4, ".exe", fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
						break;
					err = errNOT_FOUND;
				}
				if(PR_SUCC(err))
					strImagePath = strPath;
				hPtr->sysCloseObject();
			}
		}
		hOs->sysCloseObject();
	}
	return PR_SUCC(err);
}

#endif // __GETAPPINFOBYPID_H__