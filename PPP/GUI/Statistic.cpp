// Statistic.cpp:
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Statistic.h"

#define DATA_CHUNK_SIZE 1024

//////////////////////////////////////////////////////////////////////
// CScanStatisticSink

void CScanStatisticSink::OnInitProps(CItemProps& pProps)
{
	CListItemSink::OnInitProps(pProps);

	m_eDbType = (tDWORD)pProps.Get(STR_PID_PARAMS).GetLong();
	((CListItem*)m_pItem)->Bind(cGuiScanStatistic::eIID, true);

	m_pItem->AddDataSource(&m_pEvent);
}

void CScanStatisticSink::OnNodeExpanded(cRowNode* node, bool bExpanded)
{
	if( !bExpanded )
		return;

	CStatSink* stat = (CStatSink*)node->m_sink;
	if( !stat )
		return;

	for(CStatSink* child = stat->m_pChild; child; child = child->m_pNext)
	{
		cRowNode* added = AddListNode(node, (CStatSink*)child);
		if( added && child->m_pChild )
			AddListNode(added, (CStatSink*)child->m_pChild);
	}
}

bool CScanStatisticSink::OnAddRow(cRowNode* node, tIdx row)
{
	CNodeSink* sink = (CNodeSink*)node->m_sink;
	if( !sink )
	{
		sink = new CNodeSink();
		sink->m_owner = this;
		sink->m_node = node;
		sink->m_nFlags |= 2;
		node->m_sink = sink;
	}
										
	tDWORD i = (row-m_nCurPos);
	const tReportEvent& evt = m_events[i];

	tDWORD nStatOff = 0;
	switch(evt.m_Verdict)
	{
	case OBJSTATUS_OK:           nStatOff = offsetof(cGuiScanStatistic,m_nNumScaned); break;
	case OBJSTATUS_INFECTED:     nStatOff = offsetof(cGuiScanStatistic,m_nNumDetected); break;
	case OBJSTATUS_SUSPICION:    nStatOff = offsetof(cGuiScanStatistic,m_nNumDetected); break;
	case OBJSTATUS_ARCHIVE:      nStatOff = offsetof(cGuiScanStatistic,m_nNumArchived); break;
	case OBJSTATUS_PACKED:       nStatOff = offsetof(cGuiScanStatistic,m_nNumPacked); break;
	case OBJSTATUS_CORRUPTED:    nStatOff = offsetof(cGuiScanStatistic,m_nNumCorrupted); break;
	case OBJSTATUS_NOTPROCESSED: nStatOff = offsetof(cGuiScanStatistic,m_nNumNotProcessed); break;
	case OBJSTATUS_PASSWORD_PROTECTED: nStatOff = offsetof(cGuiScanStatistic,m_nNumPswProtected); break;
	case OBJSTATUS_PROCESSING_ERROR:   nStatOff = offsetof(cGuiScanStatistic,m_nNumScanErrors); break;
	default: return false;
	}

	tQWORD nParentId = m_ids.m_vParentIds[i];
	CStatSink* data = sink->FindNode(nParentId, evt.m_ObjectType);
	if( !data )
		return false;

	CStatSink* s = sink->m_last;
	for(;s; s = s->m_pParent)
		s->m_nFlags &= ~1;

	s = sink->m_last = data;
	for(;s; s = s->m_pParent)
	{
		if( nStatOff ) 
			(*(tDWORD*)((tBYTE*)s + nStatOff))++;
		s->m_nTimeSpend += evt.m_ExtraInfoSub2;
		if( s->m_pParent )
			s->m_nFlags |= 1;
	}

	return false;
}

bool CScanStatisticSink::GetInfo(cDataInfo& info)
{
	cReportDB* pReport = g_hGui->m_pReport;
	if( !pReport )
		return false;

	return PR_SUCC(pReport->GetDBInfo(m_eDbType,
		&info.m_nCount, NULL, &info.m_nTimeStamp));
}

void CScanStatisticSink::PrepareRows(tIdx nFrom, tDWORD nCount)
{
}

cSer * CScanStatisticSink::GetRowData(tIdx nItem)
{
	if( m_nCurPos > nItem || nItem >= m_nCurPos + m_nReaded )
	{
		if( DATA_CHUNK_SIZE > m_events.size() )
			m_events.resize(DATA_CHUNK_SIZE+1);

		m_nCurPos = (nItem / DATA_CHUNK_SIZE)*DATA_CHUNK_SIZE;
		m_nReaded = 0;
		m_ids.m_vParentIds.clear();
		g_hGui->m_pReport->GetEvents(m_eDbType, m_nCurPos, &*m_events.begin(),
			DATA_CHUNK_SIZE*sizeof(tReportEvent), DATA_CHUNK_SIZE, (tDWORD*)&m_nReaded, &m_ids);
	}

	(tReportEvent&)m_pEvent = m_events[nItem-m_nCurPos];
	return &m_pEvent;
}

cRowNode* CScanStatisticSink::AddListNode(cRowNode* node, CStatSink* stat)
{
	if( !stat->m_pNode )
	{
		stat->m_pNode = node->AddNode(0, 0);
		stat->m_pNode->m_sink = stat;
	}
	return stat->m_pNode;
}

//////////////////////////////////////////////////////////////////////

CStatSink* CScanStatisticSink::CNodeSink::AddNode(tQWORD qwId, tDWORD dwType, CStatSink* pParent)
{
	CStatSink& stat = m_data.insert(std::make_pair(qwId,
		CStatSink())).first->second;
	stat.m_pParent   = pParent;
	stat.m_nObjectId = qwId;
	stat.m_nObjectType = dwType;

	if( pParent )
	{
		if( !pParent->m_pChild )
			pParent->m_pChild = pParent->m_pLast = &stat;
		else
		{
			pParent->m_pLast->m_pNext = &stat;
			pParent->m_pLast = &stat;
		}
	}

	cRowNode* node = pParent == this ? m_node : (cRowNode*)pParent->m_pNode;

	if( node && (node->IsExpanded() || !node->Nodes().size()) )
		m_owner->AddListNode(node, &stat);

	return &stat;
}

CStatSink* CScanStatisticSink::CNodeSink::FindNode(tQWORD qwId, tDWORD dwType)
{
	if( dwType != eFile )
		qwId = ((tQWORD)dwType << 32) | 0xFFFFFFFF;

	tData::iterator data = m_data.find(qwId);
	if( data != m_data.end() )
		return &data->second;

	if( dwType != eFile )
		return AddNode(qwId, dwType, this);

	if( !qwId )
		return this;

	cReportParentIds parentIds;
	g_hGui->m_pReport->GetObjectInfo(qwId, &parentIds, 0);

	CStatSink* parent = this;

	size_t i, n = parentIds.m_vParentIds.size();
	tQWORD* parentId = parentIds.m_vParentIds.data()+n-1;

	for(i = 0; i < n; i++, parentId--)
	{
		tData::iterator pdata = m_data.find(*parentId); 
		if( pdata == m_data.end() )
			parent = AddNode(*parentId, dwType, parent);
		else
			parent = &pdata->second;
	}

	return AddNode(qwId, dwType, parent);
}

//////////////////////////////////////////////////////////////////////
