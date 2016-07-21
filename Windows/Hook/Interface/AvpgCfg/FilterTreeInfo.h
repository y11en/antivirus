// FilterTreeInfo.h: interface for the CFilterTreeInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERTREEINFO_H__4F65D0D3_D48C_11D3_9DA7_00D0B71C715A__INCLUDED_)
#define AFX_FILTERTREEINFO_H__4F65D0D3_D48C_11D3_9DA7_00D0B71C715A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _FILTER_DESCRIPTION		1
#define _FILTER_FILENAME		2
#define _FILTER_PROCESSNAME		3
#define _FILTER_EXPIRATION		4
#define _FILTER_DENY			5
#define _FILTER_PROMPT			6
#define _FILTER_TIMEOUT			7
#define _FILTER_LOG				8
#define _FILTER_FUNCTION		9
#define _FILTER_MAJ				10
#define _FILTER_MIN				11
#define _FILTER_CHECKPARAM		12
#define _FILTER_PARAM			13
#define _FILTER_PERMIT			14
#define _FILTER_PROMPT_DEFDENY	15
#define _FILTER_PROMPT_DEFALLOW	16
#define _FILTER_FLAGS			17
#define _FILTER_IPSRC			18
#define _FILTER_IPDST			19
#define _FILTER_IPPORTSRC		20
#define _FILTER_IPPORTDST		21

class CFilterTreeInfo
{
public:
	CFilterTreeInfo();
	CFilterTreeInfo(PFILTER_TRANSMIT pFilter, int nProperty);
	virtual ~CFilterTreeInfo();
	
	PFILTER_TRANSMIT m_pFilter;
	int m_nProperty;
};

#endif // !defined(AFX_FILTERTREEINFO_H__4F65D0D3_D48C_11D3_9DA7_00D0B71C715A__INCLUDED_)
