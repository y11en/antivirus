// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#define  _CRTIMP 

#include "task.h"
#include <iface/i_buffer.h>

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>

// -------------------------------------------

CDelayedScan::CDelayedScan(cODSImpl *pTask) : m_pTask(pTask), m_bActivate(false)
{
	cStringObj strReport;
	tERROR error = strReport.assign(m_pTask->m_hBL, pgBL_PRODUCT_PATH);

	if ( PR_SUCC(error) )
		error = m_pTask->sysCreateObject((cObject**)&m_pReport, IID_REPORT, PID_REPORT);

	if ( PR_SUCC(error) )
	{
		strReport += REPORT_PATH "/";
		if( PrGetTempFileName )
			error = PrGetTempFileName(strReport, "TIF");
		else
			error = errUNEXPECTED;

		if( PR_SUCC(error) )
			error = strReport.copy(m_pReport, pgOBJECT_FULL_NAME);
	}

	if ( PR_SUCC(error) )
		error = m_pReport->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_RW);

	if ( PR_SUCC(error) )
		error = m_pReport->sysCreateObjectDone();

	if( PR_FAIL(error) && m_pReport )
	{
		m_pReport->sysCloseObject();
		m_pReport = NULL;
	}
	else if( m_pReport )
		m_pReport->propSetDWord(pgOBJECT_DELETE_ON_CLOSE, cTRUE);
}

tERROR CDelayedScan::Activate()
{
/*	if( !m_pReport )
		return errNOT_OK;

	tDWORD nRecordCount = 0;
	m_pReport->GetRecordCount(&nRecordCount);

	if( !nRecordCount )
		return errNOT_OK;

	CAskShowTIF askTIF;
	askTIF.m_nTaskId = m_pTask->m_nTaskId;
	askTIF.m_strReport = m_sReportName;

	m_bActivate = true;
	tERROR error = m_pTask->m_hBL->AskAction(CAskShowTIF::eIID, &askTIF);
	m_bActivate = false;

	return error;
*/
	return errOK;
}


tDWORD CDelayedScan::GetPercentCompleted()
{
	return m_nCount ? m_nPos * 100 / m_nCount : 100;
}

tERROR CDelayedScan::StopScan()
{
	m_nCount = 0;
	return errOK;
}

tERROR CDelayedScan::StoreRecord(cInfectedObjectInfo * pData)
{
/*	if( !m_pReport )
		return errUNEXPECTED;

	tChunckBuff buff;
	tDWORD nSize;

	tERROR error = g_root->StreamSerialize(pData, (tBYTE*)buff.ptr(), buff.count(), &nSize);
	if( error == errBUFFER_TOO_SMALL )
		error = g_root->StreamSerialize(pData, (tBYTE*)buff.get(nSize, false), nSize, &nSize);

	if( PR_SUCC(error) )
		error = m_pReport->AddRecord(buff.ptr(), nSize, NULL);

	return error;
*/
	return errOK;
}
