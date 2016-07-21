// RebarPos.h: interface for the CRebarPos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REBARPOS_H__7325840E_76C8_4C45_8116_F67A3A2BDC46__INCLUDED_)
#define AFX_REBARPOS_H__7325840E_76C8_4C45_8116_F67A3A2BDC46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRebarPos  
{
public:
	struct TBand
	{
		TBand() : m_dwID(0), m_dwCX(0), m_dwIndex(0), m_bUseFullLine(true)
		{
		}

		DWORD m_dwID;
		DWORD m_dwCX;
		DWORD m_dwIndex;
		bool  m_bUseFullLine;
	};

	bool SaveToRegistry(CRegKey& key) const;
	bool LoadFromRegistry(CRegKey& key);

	bool FillFromRebar(HWND hRebar);
	bool ApplyToRebar(HWND hRebar) const;

	void GetBand(DWORD dwID, OUT TBand& band) const;

private:
	typedef std::vector<TBand> BANDS_ARRAY;
	BANDS_ARRAY m_arrBands;
};

#endif // !defined(AFX_REBARPOS_H__7325840E_76C8_4C45_8116_F67A3A2BDC46__INCLUDED_)
