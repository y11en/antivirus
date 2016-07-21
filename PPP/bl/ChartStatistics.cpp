#include "bl.h"
#include <iface/i_report.h>
#include "ChartStatistics.h"

//-------------------------------------------------
//-----------------chReport class------------------
//-------------------------------------------------
chReport::chReport()
{
	ZeroAll();
}

chReport::chReport(tCHAR* szReportName, tINT nType)
{
	ZeroAll();
	m_szReportName = szReportName;
	m_nType = nType;

}

void chReport::ZeroAll()
{
	m_pReport = NULL;
	m_nType = 0;
}

bool chReport::Init(cTaskManager* tm)
{
	if (!tm)
		return false;

	tERROR error = tm->GetTaskReport(TASKID_TM_ITSELF, m_szReportName.c_str(), *tm, fACCESS_RW, &m_pReport);
	if( PR_FAIL(error) )
	{
		PR_TRACE((this,prtERROR,"stat\t!!!!!!!!Cannot get chart report %terr", error));
		return false;
	}

	return true;
}

tERROR chReport::ReadToChunkBuff( tDWORD dwIndex, tChunckBuff& buff, tDWORD& nSize)
{
	if (!m_pReport)
		return errPARAMETER_INVALID;

	tERROR error = m_pReport->GetRecord(&nSize, dwIndex, (tPTR)buff.ptr(), buff.count(), NULL);
	if( error == errBUFFER_TOO_SMALL )
		error = m_pReport->GetRecord(NULL, dwIndex, (tPTR)buff.get(nSize, false), nSize, NULL);

	return error;
}


bool chReport::IncrementRecord(tDWORD dwID)
{
	// ------------BINARY RECORD FORMAT---------------------
	//
	//            id         value      
	//      |   tDWORD   |   tQWORD    |
	//      |____________|_____________|
	//
	// -----------------------------------------------------

	tDWORD nCount;
	tERROR error = m_pReport->GetRecordCount(&nCount);

	if (PR_FAIL(error))
		return false;

	tChunckBuff buff; tDWORD nSize;
	for (tDWORD i = 0; i < nCount; ++i)
	{
		if ( PR_SUCC(ReadToChunkBuff(i, buff, nSize)) && (nSize >= sizeof(tDWORD) + sizeof(tQWORD)) && *(tDWORD*)buff.ptr() == dwID)
		{
			//increment value
			++(*(tQWORD*)((tBYTE*)buff.ptr()+sizeof(tDWORD)));
			m_pReport->UpdateRecord(i, buff.ptr(), nSize);
			return true;
		}
	}

	nSize = sizeof(tDWORD) + sizeof(tQWORD);
	*(tDWORD*)buff.get(nSize, false) = dwID;
	*(tQWORD*)((tBYTE*)buff.ptr()+sizeof(tDWORD)) = 1;

	tDWORD dwIndex;
	m_pReport->AddRecord(buff.ptr(), nSize, &dwIndex);

	return true;
}


void chReport::Deinit()
{
	if (m_pReport)
	{
		m_pReport->sysCloseObject();
		m_pReport = NULL;
	}
}



//-------------------------------------------------
//-----------------END chReport class------------------
//-------------------------------------------------


CChartStatistics::CChartStatistics(void) : cThreadPoolBase("ChartStatistics")
{
	m_Reports.push_back (chReport("detected_all.st", eRT_Detected_Threat_Bind));
}

CChartStatistics::~CChartStatistics(void)
{
	CloseReport();
}

void CChartStatistics::Init()
{
}


void CChartStatistics::Start(BlImpl* pBL)
{
	if (!pBL)
		return;

	m_pBL = pBL;

	InitReport();

//	cThreadPoolBase::init(g_root, 1, CHART_UPDATE_TIME);
}


void CChartStatistics::Stop()
{
//	cThreadPoolBase::de_init();

	CloseReport();
	m_pBL = NULL;

}

void CChartStatistics::Done()
{

}

tERROR CChartStatistics::on_idle() 
{
	return errOK; 
}

static tDWORD GetThreatID(enDetectType eDetectType)
{
	switch(eDetectType)
	{
	case DETYPE_VIRWARE: return 1;
	case DETYPE_TROJWARE: return 2;
	case DETYPE_MALWARE: return 3;
	case DETYPE_RISKWARE: return 4;
	}

	return 50; //other
}

void CChartStatistics::on_detected(cDetectObjectInfo* pData)
{
	if (!pData)
		return;

	for (tDWORD i = 0; i<m_Reports.count(); ++i)
	{
		if (m_Reports[i].m_nType & eRT_Detected_Threat_Bind)
			m_Reports[i].IncrementRecord(GetThreatID(pData->m_nDetectType));
	}
}


bool CChartStatistics::InitReport()
{
	if (!m_pBL || !m_pBL->m_tm)
		return false;

	for (tDWORD i = 0; i < m_Reports.count(); ++i)
		m_Reports[i].Init(m_pBL->m_tm);

	return true;
}

void CChartStatistics::CloseReport()
{
	for (tDWORD i = 0; i < m_Reports.count(); ++i)
		m_Reports[i].Deinit();

}

