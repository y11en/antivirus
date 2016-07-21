#ifndef __ICONCACHE_H
#define __ICONCACHE_H

#include <windows.h>
#include <Stuff\SArray.h>

// ---
class CHIconCache {
	struct HIconCacheItem {
		HICON  m_hIcon;
		DWORD  m_nCRC;
		int    m_iIndexInImageList;
	};

	CSArray<HIconCacheItem> m_Array;

public :
	CHIconCache() : m_Array() {}
	~CHIconCache();

	int  Add( HICON hIconToAdd );
	void SetItemIndex( int iArrayIndex, int iSetIndex );
	int  FindIt( HICON hIconToFind );
	void Condemn( HICON hIcon );
	void Flush();
};


#endif

