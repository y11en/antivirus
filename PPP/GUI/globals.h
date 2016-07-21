// global.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(__GLOBALS_H__)
#define __GLOBALS_H__

//////////////////////////////////////////////////////////////////////

#define STRID_PROFILEDESCRIPTION  "ProfileDescription"
#define STRID_TASKTYPES           "TaskTypes"
#define STRID_WARNING             "Warning"

#define STRID_BROWSEOBJRESTORE    "BrowseObjectRestore"

#define EVENTID_ATTACH_FILES      0x46f09c86 //is a crc32
#define EVENTID_PASSWORD_ENTERED  0x2d8b186f //is a crc32

#define OWNEROFMEMBER_PTR( _owner_class, _member )	\
	((_owner_class *)((unsigned char *)this - (unsigned char *)&((reinterpret_cast<_owner_class *>(0))->_member)))

template<typename T>
inline int GetOneBitsCount(T num)
{
	// этот способ не самый оптимальный по быстродействию, но зато понятный при прочтении
	int count=0;
	for( ; num != 0; count += (num & 1), num>>=1 );
	return count;
}

//////////////////////////////////////////////////////////////////////

inline int MsgBox(CItemBase * pItem, LPCTSTR strTextId, LPCTSTR strCaptId = NULL, UINT uType = MB_OK|MB_ICONINFORMATION, cSerializable * pBind = NULL)
{
	return g_pRoot->ShowMsgBox(pItem, strTextId, strCaptId, uType, pBind);
}

inline void MsgBoxErr(CItemBase * pItem, cGuiErrorInfo * pBind, LPCTSTR strTextId = NULL, LPCTSTR strCaptId = NULL)
{
	g_pRoot->ShowMsgBox(pItem, strTextId, strCaptId,
		MB_OK|(PR_FAIL(pBind->m_nErr) ? MB_ICONSTOP : (pBind->m_nErr != errOK ? MB_ICONEXCLAMATION : MB_ICONINFORMATION)), pBind);
}

//////////////////////////////////////////////////////////////////////

#endif //__GLOBALS_H__
