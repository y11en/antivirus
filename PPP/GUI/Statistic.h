// Statistic.h:
//
//////////////////////////////////////////////////////////////////////

#ifndef __GUI_STATISTIC_H__
#define __GUI_STATISTIC_H__

//////////////////////////////////////////////////////////////////////

struct CStatSink : public cGuiScanStatistic, public cRowNode::cSink
{
	CStatSink() : m_pParent(0), m_pChild(0), m_pLast(0), m_pNext(0), m_pNode(0){}

	cSer* GetData(){ return this; }

	CStatSink* m_pParent;
	CStatSink* m_pChild;
	CStatSink* m_pLast;
	CStatSink* m_pNext;
	cRowNode*  m_pNode;
};

class CScanStatisticSink : public CListSinkMaster
{
public:
	CScanStatisticSink() : m_nCurPos(-1), m_nReaded(0){}

	tTYPE_ID OnGetType(){ return (tTYPE_ID)cGuiScanStatistic::eIID; }

	void OnInitProps(CItemProps& pProps);
	void OnNodeExpanded(cRowNode* node, bool bExpanded);

	bool GetInfo(cDataInfo& info);
	cSer * GetRowData(tIdx nItem);
	void PrepareRows(tIdx nFrom, tDWORD nCount);
	bool OnAddRow(cRowNode* node, tIdx row);

	struct CNodeSink : public CStatSink
	{
		CNodeSink() : m_owner(0), m_node(0), m_last(0){}

		virtual void Destroy(){ delete this; }

		CStatSink* AddNode(tQWORD qwId, tDWORD dwType, CStatSink* pParent);
		CStatSink* FindNode(tQWORD qwId, tDWORD dwType);

		typedef std::map<tQWORD, CStatSink> tData;
		tData      m_data;
		cRowNode*  m_node;
		CStatSink* m_last;
		CScanStatisticSink* m_owner;
	};

private:
	cRowNode* AddListNode(cRowNode* node, CStatSink* stat);

	std::vector<tReportEvent> m_events;
	cReportParentIds  m_ids;
	cReportEvent      m_pEvent;
	tDWORD            m_eDbType;
	tLONG             m_nCurPos;
	tLONG             m_nReaded;
};


//////////////////////////////////////////////////////////////////////

#endif // __GUI_STATISTIC_H__