#ifndef __S_SETTINGS_H
#define __S_SETTINGS_H

#include <Prague/pr_cpp.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>

#include <stddef.h>
#include <string.h>

enum _eSettings_IID
{
	setst_cTaskBLSettings = 1,
	setst_cTaskSettings,
	setst_cEnabledStrItem,
	setst_cTaskSettingsPerUserItem,
	setst_cTaskSettingsPerUser,
	setst_cPolicySettings,
	setst_cTaskStatisticsPerUser,
};


#define TASKSETT_DEFAULT_USER  ".DEFAULT"

// ---
template <class T>
struct cSettingsCheckT {

	tBOOL   m_on;
	T       m_val;

public:
	cSettingsCheckT(T value, tBOOL on = -1) : m_on(on == -1 ? !!value : on), m_val(value) {}

	bool operator == ( const cSettingsCheckT<T>& o ) const { return val() == o.val(); }
	bool operator != ( const cSettingsCheckT<T>& o ) const { return !(operator == (o)); }
	T    val()                                       const { return m_on ? m_val : 0; }
	bool on()                                        const { return !!m_on; }
	bool off()                                       const { return !m_on; }
	void on( tBOOL par )                                   { m_on = par; }
};

typedef cSettingsCheckT<tDWORD> cSettingsCheck;

// --------------------------------------------------

class cBitMask
{
public:
	enum { eInvalidBitPosition = 0xffffffff, };

	cBitMask() { Zero(); }

	void Zero()
	{
		memset(bitsArray, 0, sizeof(bitsArray));
	}
	
	bool IsBitSet(tDWORD bitPos) const
	{
		tDWORD *bitsHolder, bitMask;
		if( !CheckBit(bitPos, bitsHolder, bitMask) )
			return false;
		return !!(*bitsHolder & bitMask);
	}
	
	void SetBit(tDWORD bitPos, bool nOn)
	{
		tDWORD *bitsHolder, bitMask;
		if( !CheckBit(bitPos, bitsHolder, bitMask) )
			return;
		if( nOn )
			*bitsHolder |= bitMask;
		else
			*bitsHolder &= ~bitMask;
	}

	void Not()
	{
		for(tDWORD i = 0; i < countof(bitsArray); i++)
			bitsArray[i] = ~bitsArray[i];
	}

	void Or(const cBitMask &bitMask)
	{
		for(tDWORD i = 0; i < countof(bitsArray); i++)
			bitsArray[i] |= bitMask.bitsArray[i];
	}

	void And(const cBitMask &bitMask)
	{
		for(tDWORD i = 0; i < countof(bitsArray); i++)
			bitsArray[i] &= bitMask.bitsArray[i];
	}

	bool IsEqual(const cBitMask &bitMask) const
	{
		for(tDWORD i = 0; i < countof(bitsArray); i++)
			if( bitsArray[i] != bitMask.bitsArray[i] )
				return false;
		return true;
	}
	
	bool IsZero() const
	{
		for(tDWORD i = 0; i < countof(bitsArray); i++)
			if( bitsArray[i] )
				return false;
		return true;
	}

public:
	cBitMask &operator |= (const cBitMask &bitMask)     { Or(bitMask); return *this; }
	cBitMask &operator &= (const cBitMask &bitMask)     { And(bitMask); return *this; }
	cBitMask operator ~ () const                        { cBitMask bm = *this; bm.Not(); return bm; }
	cBitMask operator & (const cBitMask &bitMask) const { cBitMask bm = *this; bm.And(bitMask); return bm; }
	bool operator == (const cBitMask &bitMask) const    { return IsEqual(bitMask); }
	bool operator ! () const                            { return IsZero(); }

protected:
	bool CheckBit(tDWORD bitPos, tDWORD *&bitsHolder, tDWORD &bitMask) const
	{
		if( bitPos == eInvalidBitPosition )
			return false;
		bitMask = 1 << (bitPos & 31);
		bitPos >>= 5;
		bitsHolder = const_cast<tDWORD *>(&bitsArray[ bitPos & 3 ]);
		return true;
	}

	tDWORD bitsArray[4];
};

// --------------------------------------------------

struct cPolicySettings : public cSerializable
{
	DECLARE_IID( cPolicySettings, cSerializable, PID_ANY, setst_cPolicySettings );

	cBitMask                 m_dwLockedFields;
	cBitMask                 m_dwMandatoriedFields;

#ifdef __pr_serdescriptor_h

public:
	bool IsLocked(const tCHAR* strField) const                    { return IsPolicySet(m_dwLockedFields, strField); }
	bool IsLockedByPtr(const tVOID* pAddr) const                  { return IsPolicySetByPtr(m_dwLockedFields, pAddr); }
	void SetLocked(const tCHAR* strField, bool bLocked)           { SetPolicyBit(m_dwLockedFields, strField, bLocked); }
	void SetLockedByPtr(const tVOID* pAddr, bool bLocked)         { SetPolicyBitByPtr(m_dwLockedFields, pAddr, bLocked); }

	bool IsMandatoried(const tCHAR* strField) const               { return IsPolicySet(m_dwMandatoriedFields, strField); }
	bool IsMandatoriedByPtr(const tVOID* pAddr) const             { return IsPolicySetByPtr(m_dwMandatoriedFields, pAddr); }
	void SetMandatoried(const tCHAR* strField, bool bMandatoried) { SetPolicyBit(m_dwMandatoriedFields, strField, bMandatoried); }
	void SetMandatoriedByPtr(const tVOID* pAddr, bool bMandatoried) { SetPolicyBitByPtr(m_dwMandatoriedFields, pAddr, bMandatoried); }

	tDWORD FindFieldBit(const tCHAR* strField) const              { return FindFieldBit(getIID(), strField, NULL); }

	static tDWORD FindFieldBit(tDWORD tUnique, const tCHAR* strField, const cSerDescriptorField * pField)
	{
		tDWORD pos = 0;
		if( FindFieldBit(tUnique, strField, pField, pos) )
			return pos;
		return cBitMask::eInvalidBitPosition;
	}

protected:
	static bool FindFieldBit(tDWORD tUnique, const tCHAR * strField, const cSerDescriptorField * pField, tDWORD& pos)
	{
		cSerDescriptor * pDesc = NULL;
		g_root->FindSerializableDescriptor(&pDesc, tUnique);
		if( !pDesc )
			return false;

		for(const cSerDescriptorField * field = pDesc->m_fields; field->m_flags != SDT_FLAG_ENDPOINT; ++field)
		{
			if( (field->m_id > tid_LAST_TYPE) && (field->m_flags & SDT_FLAG_BASE) )
			{
				if( FindFieldBit(field->m_id, strField, pField, pos) )
					return true;
				continue;
			}

			if( pField )
			{
				if( field == pField )
					return true;
			}
			else if( strField )
			{
				if( field->m_name && !strcmp(field->m_name, strField) )
					return true;
			}
			
			if (!(field->m_flags & SDT_FLAG_CANT_BE_LOCKED))
				pos++;
		}
		
		return false;
	}
	
	bool IsPolicySet(const cBitMask &bitFields, const tCHAR* strField) const
	{
		return bitFields.IsBitSet(FindFieldBit(strField));
	}

	bool IsPolicySetByPtr(const cBitMask &dwFields, const tVOID* pAddr) const
	{
		const cSerDescriptorField * pFld = getField(pAddr);
		if( !pFld || !pFld->m_name || !(*pFld->m_name) )
			return false;
		
		return IsPolicySet(dwFields, pFld->m_name);
	}

	void SetPolicyBit(cBitMask &dwFields, const tCHAR* strField, bool bOn)
	{
		dwFields.SetBit(FindFieldBit(strField), bOn);
	}

	void SetPolicyBitByPtr(cBitMask &dwFields, const tVOID* pAddr, bool bOn)
	{
		const cSerDescriptorField * pFld = getField(pAddr);
		if( !pFld || !pFld->m_name || !(*pFld->m_name) )
			return;

		SetPolicyBit(dwFields, pFld->m_name, bOn);
	}
#endif
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPolicySettings, 0 )
	SER_VALUE( m_dwLockedFields,      tid_BINARY, "LockedFields" )
	SER_VALUE( m_dwMandatoriedFields, tid_BINARY, "MandatoriedFields" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cTaskSettings : public cPolicySettings
{
	cTaskSettings() : m_dwVersion(1) {}
	
	DECLARE_IID( cTaskSettings, cPolicySettings, PID_ANY, setst_cTaskSettings );

	cSerObj<cSerializable>   m_pTaskBLSettings;
	tDWORD                   m_dwVersion;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskSettings, 0 )
	SER_BASE( cPolicySettings, 0 )
	SER_VALUE_PTR( m_pTaskBLSettings, cSerializable::eIID, "BLSettings" )
	SER_FIELD( m_dwVersion, SDT_FLAG_CANT_BE_LOCKED, tid_DWORD, "Version", 0 )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cTaskBLSettings : public cTaskSettings
{
	DECLARE_IID( cTaskBLSettings, cTaskSettings, PID_ANY, setst_cTaskBLSettings );
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskBLSettings, 0 )
	SER_BASE( cTaskSettings, 0 )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cEnabledStrItem : public cSerializable
{
	cEnabledStrItem(tSTRING pName = NULL) :
		m_bEnabled(cTRUE)
	{
		if( pName )
			m_sName = pName;
	}

	cEnabledStrItem(cStringObj& pName) :
		m_bEnabled(cTRUE)
	{
		m_sName = pName;
	}

	bool operator ==(cStringObj& pName)
	{
		if( !m_bEnabled )
			return false;

		return m_sName == pName;
	}
		
	DECLARE_IID( cEnabledStrItem, cSerializable, PID_ANY, setst_cEnabledStrItem );

	tBOOL       m_bEnabled;
	cStringObj  m_sName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cEnabledStrItem, 0 )
	SER_VALUE( m_bEnabled,   tid_BOOL,       "Enabled" )
	SER_VALUE( m_sName,      tid_STRING_OBJ, "Name" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cTaskSettingsPerUserItem : public cSerializable
{
	DECLARE_IID( cTaskSettingsPerUserItem, cSerializable, PID_ANY, setst_cTaskSettingsPerUserItem );

	bool isdefault()
	{
		return m_sUserId.empty() || m_sUserId == (tSTRING)TASKSETT_DEFAULT_USER;
	}
	
	bool checkid(const cStringObj& sUserId)
	{
		if( sUserId.empty() && isdefault() )
			return true;
		return m_sUserId == sUserId;
	}
	
	void setid(const cStringObj& sUserId)
	{
		if( sUserId.empty() )
			m_sUserId = (tSTRING)TASKSETT_DEFAULT_USER;
		else
			m_sUserId = sUserId;
	}
	
	cStringObj             m_sUserId;
	cSerObj<cSerializable> m_settings;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskSettingsPerUserItem, 0 )
	SER_KEY_VALUE( m_sUserId, "UserId" )
	SER_VALUE_PTR( m_settings, cSerializable::eIID, "settings" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

typedef cVector<cTaskSettingsPerUserItem, cSimpleMover<cTaskSettingsPerUserItem> > tTaskSettingsPerUserItems;

struct cTaskSettingsPerUser : public cTaskSettings
{
	DECLARE_IID( cTaskSettingsPerUser, cTaskSettings, PID_ANY, setst_cTaskSettingsPerUser );

	virtual tDWORD settings_byuser_getid() { return cSerializable::eIID; }
	
	cTaskSettingsPerUserItem * item_byuser(const cStringObj& sUserId, bool bCreateIfNotExist = false, cSerializable * pDefault = NULL, bool * pbCreated = NULL)
	{
		if( pbCreated )
			*pbCreated = false;
		
		tDWORD nSerId = settings_byuser_getid();

		for(tSIZE_T i = 0; i < m_users.size(); i++)
		{
			if( m_users[i].isdefault() && !pDefault )
				pDefault = m_users[i].m_settings;

			if( m_users[i].checkid(sUserId) )
			{
				cSerializableObj& pSett = m_users[i].m_settings;
				if( !pSett )
					pSett.init(nSerId);
				else if( pSett->getIID() != nSerId )
					return NULL;
				return &m_users[i];
			}
		}

		if( !bCreateIfNotExist )
			return NULL;

		if( pbCreated )
			*pbCreated = true;

		cTaskSettingsPerUserItem _user;
		_user.setid(sUserId);
		_user.m_settings.init(nSerId);

		if( pDefault && _user.m_settings )
			_user.m_settings.assign(pDefault);
		return &m_users.push_back(_user);
	}

	cSerializable * settings_byuser(const cStringObj& sUserId, bool bCreateIfNotExist = false, cSerializable * pDefault = NULL, bool * pbCreated = NULL)
	{
		cTaskSettingsPerUserItem * pItem = item_byuser(sUserId, bCreateIfNotExist, pDefault, pbCreated);
		if( !pItem )
			return NULL;
		return pItem->m_settings;
	}

	bool delete_user(const cStringObj& sUserId)
	{
		for(tSIZE_T i = 0; i < m_users.size(); i++)
		{
			if( m_users[i].isdefault() )
				continue;

			if( m_users[i].checkid(sUserId) )
			{
				m_users.remove(i);
				return true;
			}
		}
	
		return false;
	}

	tTaskSettingsPerUserItems m_users;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskSettingsPerUser, 0 )
	SER_BASE( cTaskSettings, 0 )
	SER_VECTOR( m_users, cTaskSettingsPerUserItem::eIID, "users" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

#endif //__S_SETTINGS_H
