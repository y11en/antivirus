//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "ResourceHelper.h"

static class CCIconCache
{
public:
	virtual ~CCIconCache()
	{
		for (IconMapType::iterator i = m_IconMap.begin  (); i!=m_IconMap.end  (); ++i)
		{
			for (IconMapType::referent_type::iterator j = i->second.begin (); j != i->second.end (); ++j)
				::DestroyIcon(j->second);
		}
	}

	HICON Find (DWORD dwIconID, DWORD dwWidth)
	{
		IconMapType::iterator i = m_IconMap.find  (dwIconID);

		if (i == m_IconMap.end  ())
			return NULL;

		IconMapType::referent_type::iterator j = i->second.find (dwWidth);

		return (j == i->second.end ())?NULL:j->second;
	}
	
	void Put(HICON hIcon, DWORD dwIconID, DWORD dwWidth)
	{
		ASSERT (hIcon!=NULL);
		m_IconMap[dwIconID][dwWidth] = hIcon;
	}

protected:
	typedef std::map<DWORD, std::map<DWORD, HICON> > IconMapType;
	IconMapType m_IconMap;
}g_CommonIconsCache;

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
HICON LoadIconLR(DWORD dwIconID, DWORD dwWidth)
{
	HICON hIcon = g_CommonIconsCache.Find(dwIconID, dwWidth);
	
	if (!hIcon)
	{
		HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(dwIconID), RT_GROUP_ICON);

		if (hInst)
		{
			hIcon = (HICON)::LoadImage ( hInst, MAKEINTRESOURCE(dwIconID), IMAGE_ICON, dwWidth ,dwWidth, LR_DEFAULTCOLOR);

			if (hIcon)
				g_CommonIconsCache.Put(hIcon, dwIconID, dwWidth);
		}
	}
	
	return hIcon;
	
}

//==============================================================================
