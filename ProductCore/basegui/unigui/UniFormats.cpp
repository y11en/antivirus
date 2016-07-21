// Formats.cpp
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UniRoot.h"
#include "../Formats.h"

//////////////////////////////////////////////////////////////////////
/*
enum DTT_TYPE{DTT_DT, DTT_DATE, DTT_TIME, DTT_TIMESHORT, DTT_TIMESTAMP};

//////////////////////////////////////////////////////////////////////
// CPlaceHolderTime

struct CPlaceHolderTime : public CPlaceHolder
{
	CPlaceHolderTime(DTT_TYPE nType, bool bPrague = false, bool bSeconds = true, bool bLocal = true) : m_type(nType), m_pr(bPrague), m_seconds(bSeconds), m_local(bLocal) {}
	void Update(CStructData& pData, CValData& _Data, CBindProps * pProps);

private:
	DTT_TYPE   m_type;
	unsigned   m_pr : 1;
	unsigned   m_seconds : 1;
	unsigned   m_local : 1;
};

//////////////////////////////////////////////////////////////////////

void CPlaceHolderTime::Update(CStructData& pData, CValData& _Data, CBindProps * pProps)
{
	cDateTime prDt(cDateTime::zero);
	
	if( m_pr )
		prDt = _Data.GetDateTime();
	else
	{
		time_t tTime = _Data.GetInteger();
		
		switch( m_type )
		{
		case DTT_TIME: case DTT_TIMESHORT: case DTT_TIMESTAMP:
			if( IS_TIME_VALID(tTime) )
				prDt = tTime;
			break;
		
		default:
			if( IS_DATETIME_VALID(tTime) )
				prDt = tTime;
		}
	}
	
	if( prDt == cDateTime::nil() )
	{
		m_strValue = m_pRoot->GetString(PROFILE_LOCALIZE, NULL, STRID_TIMEUNKNOWN);
		return;
	}

	m_strValue = "";

	switch( m_type )
	{
	case DTT_TIMESTAMP:
	case DTT_TIME:
	{
		time_t tTime = prDt;
		
	} break;
	
	case DTT_TIMESHORT:
	{
		time_t tTime = prDt;

	} break;

	default:
	{
		tDWORD dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond;
		if( PR_FAIL(prDt.GetUTC(&dwYear, &dwMonth, &dwDay, &dwHour, &dwMinute, &dwSecond)) )
			return;

		QDateTime qdt; qdt.setTimeSpec(Qt::UTC);
		qdt.setDate(QDate(dwYear, dwMonth, dwDay));
		qdt.setTime(QTime(dwHour, dwMinute, dwSecond));

		if( m_local )
			qdt = qdt.toLocalTime();

		cStrObj strDt;
		if( m_type == DTT_DT )
			_Q2S(qdt.toString(), strDt);
		else
			_Q2S(qdt.date().toString(), strDt);

		m_pRoot->LocalizeStr(strDt, m_strValue, pProps ? pProps->m_nCnvType : escDef);
	}
	}
}

//////////////////////////////////////////////////////////////////////
// CPlaceHolderPath

struct CPlaceHolderPath : public CPlaceHolder
{
	CPlaceHolderPath(PATHOP_TYPE eType = PATHOP_ORIGINAL, bool bCompact = false) : m_eType(eType), m_bCompact(bCompact) {}
	void Update(CStructData& pData, CValData& _Data, CBindProps * pProps);

private:
	PATHOP_TYPE m_eType;
	bool        m_bCompact;
};

//////////////////////////////////////////////////////////////////////

void CPlaceHolderPath::Update(CStructData& pData, CValData& _Data, CBindProps * pProps)
{
	cStrObj& strFilePath = _Data.GetString();
	tDWORD nSlashPos = strFilePath.find_last_of("\\/");
	
	if( m_eType != PATHOP_ORIGINAL && nSlashPos != cStrObj::npos )
	{
		cStrObj strTmp;
		if( m_eType == PATHOP_FILE )
			strTmp.assign(strFilePath, nSlashPos + 1);
		else
			strTmp.assign(strFilePath, 0, nSlashPos + 1);
		
		m_pRoot->LocalizeStr(strTmp, m_strValue, pProps ? pProps->m_nCnvType : escDef);
	}
	else
		m_pRoot->LocalizeStr(strFilePath, m_strValue, pProps ? pProps->m_nCnvType : escDef);
}

//////////////////////////////////////////////////////////////////////

CPlaceHolder * CUniRoot::CreateHolder(tDWORD nId, LPCSTR strType)
{
	CPlaceHolder * pHolder = CRootItem::CreateHolder(nId, strType);
	if( pHolder )
		return pHolder;

	if( IS_SERIALIZABLE(nId) )
		return NULL;

	switch( nId )
	{
	case tid_DWORD:
	case tid_DATETIME:
	{
		bool bPr = nId == tid_DATETIME;
		sswitch(strType)
		scase(PHOLDER_T_DATETIME)     pHolder = new CPlaceHolderTime(DTT_DT, bPr); sbreak;
		scase(PHOLDER_T_DATETIME_NTZ) pHolder = new CPlaceHolderTime(DTT_DT, bPr, true, false); sbreak;
		scase(PHOLDER_T_DATE)         pHolder = new CPlaceHolderTime(DTT_DATE, bPr); sbreak;
		scase(PHOLDER_T_TIME)         pHolder = new CPlaceHolderTime(DTT_TIME, bPr); sbreak;
		scase(PHOLDER_T_TIMESHORT)    pHolder = new CPlaceHolderTime(DTT_TIMESHORT, bPr, false); sbreak;
		scase(PHOLDER_T_TIMESTAMP)    pHolder = new CPlaceHolderTime(DTT_TIMESTAMP, bPr); sbreak;
		send;
	} break;

	case tid_STRING_OBJ:
		sswitch(strType)
		scase(PHOLDER_T_COMPACT)
			if( m_nGuiFlags & GUIFLAG_ALLOWCOMPACT )
				pHolder = new CPlaceHolderPath(PATHOP_ORIGINAL, true);
			else
				pHolder = new CPlaceHolderString();
			sbreak;
		scase(PHOLDER_T_OBJDIR)    pHolder = new CPlaceHolderPath(PATHOP_DIR); sbreak;
		scase(PHOLDER_T_OBJFILE)   pHolder = new CPlaceHolderPath(PATHOP_FILE); sbreak;
		send;
		break;
	}
	
	return pHolder;
}
*/
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
