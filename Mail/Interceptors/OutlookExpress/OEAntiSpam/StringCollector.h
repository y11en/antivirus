// StringCollector.h: interface for the CStringCollector class.
//
// this class purpose is to receive individual items (strings) and
// place them in one string, separating items with a delimiter (',' by default)
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGCOLLECTOR_H__CC21DFE7_F209_4708_B832_732881725AA8__INCLUDED_)
#define AFX_STRINGCOLLECTOR_H__CC21DFE7_F209_4708_B832_732881725AA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringCollector  
{
public:
	explicit CStringCollector(LPCTSTR szItemsDelimiter = _T(", "));
	~CStringCollector();

	bool ReInit();
	bool AddItem(LPCTSTR szItem);
	bool IsEmpty() const
	{
		return m_strHolder.empty();
	}

	tstring GetResult() const
	{
		return m_strHolder;
	}

private:
	CStringCollector(const CStringCollector&);
	const CStringCollector& operator=(const CStringCollector&);	// if you want to use this, call 800-NOCOPYS

	bool	m_bIsItemFirst;
	tstring	m_strHolder;
	tstring m_strItemsDelimiter;
};

#endif // !defined(AFX_STRINGCOLLECTOR_H__CC21DFE7_F209_4708_B832_732881725AA8__INCLUDED_)
