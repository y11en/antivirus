#ifndef __userban
#define __userban

#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>

#define AVPG_ID  ((tPID)(61003))

#define pmc_USERNOTIFY          0x671568fd

#define pmc_USERBAN             0x59acd7f0

#define pm_USERBAN_GETLIST		1
#define pm_USERBAN_ADD			2
#define pm_USERBAN_DELETE		3
#define pm_USERNOTIFY_INCOME	4
#define pm_USERNOTIFY_ENUM		5


#define pmc_USERBAN_SYNC        0xf37fbb1
#define pm_BANLIST_NEWRECEIVER  0x8cdea98d
#define pm_BANLIST_SYNC         0x855013f8
#define pm_BANLIST_ITEM_ADD     0x556238c
#define pm_BANLIST_ITEM_DEL     0x145d5814
#define pm_BANLIST_OBJCRINFO	0xd7637111


struct cUserInfo : public cSerializable
{
	cUserInfo() :
		m_LocalUser(cTRUE)
	{
	};
	
	cUserInfo& operator = (const cUserInfo &that)
	{
		m_LocalUser = that.m_LocalUser;

		m_sUserName = that.m_sUserName;
		m_sMachineName = that.m_sMachineName;

		m_SysDepInfo = that.m_SysDepInfo;

		return *this;
	}
	
	DECLARE_IID( cUserInfo, cSerializable, AVPG_ID, 10 );

	tBOOL		m_LocalUser;
	
	cStringObj	m_sUserName;   // Полное имя юзера
	cStringObj	m_sMachineName;   // имя машины

	cVector<tBYTE> m_SysDepInfo;

    bool IsEqual(const cUserInfo& that) { return m_SysDepInfo == that.m_SysDepInfo; }
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUserInfo, 0 )
	SER_VALUE_M( LocalUser,    tid_DWORD )
	SER_VALUE_M( sUserName,    tid_STRING_OBJ )
	SER_VALUE_M( sMachineName, tid_STRING_OBJ )
	SER_VECTOR_M ( SysDepInfo, tid_BYTE )
IMPLEMENT_SERIALIZABLE_END();


struct cUserBanItem : public cUserInfo
{
	enum _eBan
	{
		_eBan_NotDefined = 0,
		_eBan_User = 1,
		_eBan_Machine = 2
	};

	enum _eBanAction
	{
		_eBan_None = 1,
		_eBan_Added = 1,
		_eBan_Deleted = 2,
	};

	cUserBanItem() :
		m_BanType(_eBan_Machine),
		m_Action(_eBan_None),
		m_LocalFlags(0)
	{
	}
	
	DECLARE_IID( cUserBanItem, cUserInfo, AVPG_ID, 11 );

	_eBan		m_BanType;
	_eBanAction m_Action;
	time_t		m_StartTime;
	time_t		m_StopTime;
	time_t		m_RemainTime;
	
	tDWORD		m_LocalFlags;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUserBanItem, 0 )
	SER_BASE( cUserInfo,  0 )
	SER_VALUE_M( Action,		   tid_DWORD )
	SER_VALUE_M( BanType,		   tid_DWORD )
	SER_VALUE_M( StartTime,        tid_DWORD )
	SER_VALUE_M( StopTime,         tid_DWORD )
	SER_VALUE_M( RemainTime,       tid_DWORD )
IMPLEMENT_SERIALIZABLE_END();

struct cUserBanList : public cSerializable
{
	cUserBanList()
	{
	}

	DECLARE_IID( cUserBanList, cSerializable, AVPG_ID, 12 );

	cVector<cUserBanItem> m_Users;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUserBanList, 0 )
	SER_VECTOR_M ( Users,         cUserBanItem::eIID )
IMPLEMENT_SERIALIZABLE_END();

//////////////////////////////////////////////////////////////////////////
struct cAvpgsDelayedNotify : public cSerializable
{
	cAvpgsDelayedNotify() :
		m_Luid(0),
		m_LuidHP(0)
	{
	}
	
	DECLARE_IID( cAvpgsDelayedNotify, cSerializable, AVPG_ID, 21 );

	
	tDWORD		m_Luid;
	tDWORD		m_LuidHP;
	
	cVector<tBYTE> m_FileName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAvpgsDelayedNotify, 0 )
	SER_VALUE_M( Luid,		       tid_DWORD )
	SER_VALUE_M( LuidHP,           tid_DWORD )
	SER_VECTOR_M (FileName,        tid_BYTE )
IMPLEMENT_SERIALIZABLE_END();


#endif // __userban
