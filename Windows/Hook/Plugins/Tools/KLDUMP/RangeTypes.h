// RangeTypes.h: interface for the RangeTypes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANGETYPES_H__D34851D4_3FF0_4CDF_AC6E_94A0C2083D3E__INCLUDED_)
#define AFX_RANGETYPES_H__D34851D4_3FF0_4CDF_AC6E_94A0C2083D3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct RangeType
{
	enum
	{
		eSingle,
			eRange
	}type;
	
	bool bval_any;
	bool bend_any;
};

template <typename T> struct Range : public RangeType
{
	union
	{
		T start;
		T val;
	};
	T end;
};

struct Address
{
	Range<ULONG>	ip;
	Range<USHORT>	port;
};

#endif // !defined(AFX_RANGETYPES_H__D34851D4_3FF0_4CDF_AC6E_94A0C2083D3E__INCLUDED_)
