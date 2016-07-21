#include "report.h"

tERROR 
cReport::Init(
			  IN cReportDBHelper* pReportHelper, 
			  IN bool bWriter
			  )
{
	tERROR error;
	// init processes database
	{
		alFile* pDB;
		tERROR error = pReportHelper->OpenFile(L"process.dat", 0, bWriter, bWriter, &pDB);
		if (PR_FAIL(error))
			return error;
		if (!m_procdb.Init(pDB))
		{
			pDB->Release();
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}


	// init data database
	{
		alFile* pDBT;
		alFile* pDBI;
		alFile* pDBD;
		pReportHelper->OpenFile(L"g_objbt.dat", 0, bWriter, bWriter, &pDBT);
		pReportHelper->OpenFile(L"g_objid.dat", 0, bWriter, bWriter, &pDBI);
		pReportHelper->OpenFile(L"g_objdt.dat", 0, bWriter, bWriter, &pDBD);
		if (PR_FAIL(m_objdb.Init(0, pDBT, pDBI, pDBD)))
		{
			if (pDBT) pDBT->Release();
			if (pDBI) pDBI->Release();
			if (pDBD) pDBD->Release();
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}

	error = cReportDBSegmentMgr::Init(pReportHelper, sizeof(tEvent), bWriter, &m_objdb);
	return error;
}

tERROR
cReport::AddEvent (
		  IN const tEvent* pEvent
		  )
{
	tEvent evtmp;
	if (!pEvent)
		return errPARAMETER_INVALID;
	if (!IS_UNIQ_PID(pEvent->PID) || !pEvent->AppID)
	{
		evtmp = *pEvent;
		m_procdb.ConvertPid2Uniq(evtmp.PID, &evtmp.PID, &evtmp.AppID);
		pEvent = &evtmp;
	}
	return cReportDBSegmentMgr::AddEvent(pEvent, sizeof(tEvent));
}

/*
tERROR cReport::GetEvents(IN int64 nFirstEvent, 
						  IN void* pEvents, 
						  IN size_t nEventsCount, 
						  IN size_t nBufferSize, 
						  OUT OPTIONAL size_t* pnEventsRead)
{
	return m_smgr.GetEvents(nFirstEvent, pEvents, nEventsCount, nBufferSize, pnEventsRead);
}
*/