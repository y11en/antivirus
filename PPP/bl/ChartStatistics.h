#if !defined(AFX_CHARTTASKSTATISTICS_H__5188517C_F30E_4C3A_935D_4ABF94768856__INCLUDED_)
#define AFX_CHARTTASKSTATISTICS_H__5188517C_F30E_4C3A_935D_4ABF94768856__INCLUDED_

#pragma once
#include <iface/i_threadpool.h>


struct chReport
{
	chReport();
	chReport(tCHAR* szReportName, tINT nType);
	virtual ~chReport(){}
	
	bool Init(cTaskManager* tm);
	void Deinit();

	tERROR ReadToChunkBuff( tDWORD dwIndex, tChunckBuff& buff, tDWORD& nSize);
	bool IncrementRecord(tDWORD dwID);

	tINT m_nType;

protected:
	void ZeroAll();
	
	cReport* m_pReport;
	tString m_szReportName;
};

#define CHART_MAX_FULLDUMP_DISTANCE	50
#define CHART_MIN_REPORT_SIZELIMIT	(CHART_MAX_FULLDUMP_DISTANCE + 1)
#define CHART_UPDATE_TIME	1000
#define CHART_REPORT_VERSION	1
#define CHART_REPORT_DUMP_TIME	1


class CChartStatistics :
	public cThreadPoolBase
{
public:
	enum eReportType
	{
		eRT_Detected_Threat_Bind = 1
	};
protected:
	friend struct BlImpl; 
	CChartStatistics(void);
	~CChartStatistics(void);

	bool InitReport();
	void CloseReport();

	void   Init();
	void   Start(BlImpl* pBL);
	void   Stop();
	void   Done();


protected:
	BlImpl* m_pBL;
	cVector<chReport> m_Reports;
public:
	
	tERROR on_idle();
	void CChartStatistics::on_detected(cDetectObjectInfo* pData);

};

#endif //AFX_CHARTTASKSTATISTICS_H__5188517C_F30E_4C3A_935D_4ABF94768856__INCLUDED_
