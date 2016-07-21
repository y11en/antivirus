#if !defined(_BASE_MARSHAL_H_)
#define _BASE_MARSHAL_H_

#pragma once

#include "BaseTypes.h"
#include "../cc/Common/Pid.h"
#include "../../../CommonFiles/KLUtil/inl/PoliStorage.h"
#include "../../../CommonFiles/KLUtil/SysTime.h"

// User Settings Property IDs when embedded into CUserSettings
enum UserDataPropID
{
    PROPID_BASE = 1001,
	PROPID_POLITYPE_DATA = 1002
};

#define PROPID_USERSETTINGS					    2001
#define PROPID_PRODUCT_STATUS				    2001
#define PROPID_USER_SETTINGS_STATUS			    2001
#define PROPID_SOURCE_PARAM					    2002

#define PROPID_REPORT_CHANGE_NOTIFICATION	    2001
#define PROPID_QUARANTINE_NOTIFICATION			2001
#define PROPID_USER_TASK_CHANGED_NOTIFICATION	2001

struct CCIFACEEXPORT CCBaseMarshalCommon : public CBase
{
	virtual int Store (LPPOLICY *ppPolicy) const;
	virtual int Load (LPPOLICY pPolicy);

	virtual PoliType& MarshalImpl(PoliType& pt) = 0;
};

IMPLEMENT_MARSHAL(CCBaseMarshalCommon);

template <int _TYPE_ID> struct __declspec(novtable) CCBaseMarshal : public CCBaseMarshalCommon
{
	typedef CCBaseMarshal<_TYPE_ID>	BaseMarshal;
	
	enum {CurrentVersion = 1};
	enum{TYPE_ID = _TYPE_ID};
	
	CCBaseMarshal() : m_DataVersion(0){ m_dwID = TYPE_ID; }
	
	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		m_DataVersion = GetTypeVersion();
		UINT uID = TYPE_ID;
		pt<<uID<<m_DataVersion;
		
		if (!pt.is_ok_())
			return pt;
		
		if (uID != TYPE_ID)
			pt.set_error_  (PoliType::eTypeMismatch);
		
		if (m_DataVersion > GetTypeVersion  ())
			pt.set_error_  (PoliType::eDataMoreRecent);
		
		return pt;
	}
	virtual UINT GetTypeVersion(){return CurrentVersion;}
	
	UINT GetDataVersion(){ return m_DataVersion;}
	
	
	UINT m_DataVersion;
};

template <class T> struct __declspec(novtable) DeclareNextVersion : public T
{
	typedef DeclareNextVersion<T>	BaseMarshal;
	
	enum {CurrentVersion = T::CurrentVersion + 1};
	
	virtual UINT GetTypeVersion(){return CurrentVersion;}
	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		T::MarshalImpl(pt);
		if ( pt.error_  () == PoliType::eOK && T::GetDataVersion  () < CurrentVersion )
			pt.set_error_  (PoliType::eNoErrorOldDataVersion);
		
		return pt;
	}
	
};

template <class T> struct DeclareExternalName : public T
{
public:
	virtual PoliType& MarshalImpl(PoliType& pt)
	{
		// Если мы находимся внутри какой-то новой версии
		// (pt.error_() == PoliType::eNoErrorOldDataVersion), то читать эту структуру
		// не надо, её здесь нет!
		if (pt.error_() != PoliType::eOK)
			return pt;

		T::MarshalImpl(pt);

		// Если данная структура была проапгрейдена, то дальше можно снова читать.
		if ( pt.error_  () == PoliType::eNoErrorOldDataVersion )
			pt.set_error_  (PoliType::eOK);
		
		return pt;
	}
};

#define DECLARE_EXTERNAL_NAME(version_class_name, external_class_name)	\
	typedef DeclareExternalName<version_class_name>	external_class_name;

class CCIFACEEXPORT CSysTimeSer: public KLUTIL::CSysTime
{
public:
	PoliType& MarshalImpl(PoliType& pt)
	{
		const time_t magic = 0x1312DEAD; // 02/21/80 17:55:57 GMT
		time_t prevValue = magic;
		pt << prevValue;
		if (prevValue != magic)
		{
			SetTime(prevValue);
			return pt;
		}
		return pt << SIMPLE_DATA_SER(m_st);
	}
	CSysTimeSer& operator=(const KLUTIL::CSysTime& rhs)
	{
		this->CSysTime::operator=(rhs);
		return *this;
	}
};

IMPLEMENT_MARSHAL(CSysTimeSer);

#endif //_BASE_MARSHAL_H_
