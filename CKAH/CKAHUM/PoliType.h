#if !defined(_POLITYPE_H_)
#define _POLITYPE_H_

#pragma warning(disable:4786)

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <crtdbg.h>

#define POLI_ASSERT_(exp)	_ASSERT(exp)

struct PoliStorage
{
	virtual size_t read_(void* pOutBuff, const size_t OutBuffSize) = 0;
	virtual size_t write_(const void* pDataBuff, const size_t DataBuffSize) = 0;
};

struct SDataBuffer
{
	SDataBuffer(void* data, size_t size) : m_pData(data), m_DataSize(size){}
	void* m_pData;
	size_t m_DataSize;
};

class PoliType
{
public:
	enum PoliError
	{
		eOK = 0,
			eSerializeDirectionMismatch,
			eWriteBinaryError,
			eReadBinaryError,
			eNullPointer,
			eTypeMismatch,
			eDataMoreRecent,
			eNoErrorOldDataVersion
	};
	
	explicit PoliType(PoliStorage& storage, bool bReadFromStorage = true) : m_Storage(storage), m_bReadFromStorage(bReadFromStorage), m_Error(eOK), m_nUserValue(0){}
	PoliType& set_user_long_(long nVal){m_nUserValue = nVal; return *this;}
	long get_user_long_(){ return m_nUserValue;}
	bool is_read_from_pol_(){return m_bReadFromStorage;}
	PoliError error_(){return m_Error;}
	void set_error_(PoliError err ){m_Error = err;}
	
	
public:
	PoliType& operator<<(bool& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(char& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(unsigned char& Val)	{return serialize_simple_(Val);}
	PoliType& operator<<(signed char& Val)		{return serialize_simple_(Val);}
	PoliType& operator<<(short& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(unsigned short& Val)	{return serialize_simple_(Val);}
	PoliType& operator<<(int& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(unsigned int& Val)		{return serialize_simple_(Val);}
	PoliType& operator<<(long& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(unsigned long& Val)	{return serialize_simple_(Val);}
	PoliType& operator<<(__int64& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(unsigned __int64& Val)	{return serialize_simple_(Val);}
	PoliType& operator<<(float& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(double& Val)			{return serialize_simple_(Val);}
	PoliType& operator<<(long double& Val)		{return serialize_simple_(Val);}
	
	template <class _E,	class _Tr, class _A> PoliType& operator<<(std::basic_string<_E, _Tr, _A>& Val)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
		{
			Val.erase  (Val.begin  (), Val.end  ());
			long container_size = 0;
			m_Storage.read_  (&container_size, sizeof(long));

			if(container_size)
			{
				_E* pBuff = new _E[container_size];
				m_Storage.read_  (pBuff, container_size * sizeof(_E));
				Val.append  (pBuff, container_size);
				delete [] pBuff;
			}
		}
		else
		{
			long container_size = Val.size  ();
			m_Storage.write_  (&container_size, sizeof(long));
			if (container_size)
				m_Storage.write_  ((void*)Val.c_str  (), container_size * sizeof(_E));
		}
		return *this;
	}
	

	template <class _Ty, class _A> PoliType& operator<<(std::vector<_Ty, _A>& Val){return serialize_stl_  (Val);}
	template <class _Ty, class _A> PoliType& operator<<(std::list<_Ty,_A>& Val){return serialize_stl_  (Val);}
	template <class _K, class _Pr, class _A> PoliType& operator<<(std::set<_K, _Pr, _A>& Val){return serialize_stl_  (Val);}
	template <class _K, class _Pr, class _A> PoliType& operator<<(std::multiset<_K, _Pr, _A>& Val){return serialize_stl_  (Val);}
	template <class _Ty, class _A> PoliType& operator<<(std::deque<_Ty, _A>& Val){return serialize_stl_  (Val);}
	template <class _K, class _Ty, class _Pr, class _A> PoliType& operator<<(std::map<_K, _Ty, _Pr, _A>& Val)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
		{
			Val.erase(Val.begin(), Val.end());
			long container_size = 0;
			m_Storage.read_  (&container_size, sizeof(long));
			
			for (int i = 0; i<container_size; ++i)
			{
				_K key;
				_Ty value;
				*this<<key<<value;
				Val.insert(std::pair<_K, _Ty>(key, value));
			}
		}
		else
		{
			long container_size = Val.size ();
			m_Storage.write_  (&container_size, sizeof(long));
			std::map<_K, _Ty, _Pr, _A>::iterator end = Val.end  ();
			for(std::map<_K, _Ty, _Pr, _A>::iterator i = Val.begin  (); i!= end;  ++i)
			{
				*this<<(_K)(*i).first<<(_Ty)(*i).second;
			}
			
		}
		
		return *this;	
	}
	
	PoliType& operator<<(SDataBuffer& simple_data)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
			m_Storage.read_  (simple_data.m_pData, simple_data.m_DataSize);
		else
			m_Storage.write_ (simple_data.m_pData, simple_data.m_DataSize);
		
		return *this;		
	}
	

	
	
protected:

	template<typename _T> PoliType& serialize_simple_(_T& Val)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
			m_Storage.read_  (&Val, sizeof(_T));
		else
			m_Storage.write_ (&Val, sizeof(_T));
		
		return *this;
	}
	
	template <typename _T> PoliType& serialize_stl_(_T& Val)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
		{
			Val.erase(Val.begin(), Val.end());
			long container_size = 0;
			m_Storage.read_  (&container_size, sizeof(long));

			for (int i = 0; i<container_size; ++i)
			{
				_T::value_type tmp;
				*this<<tmp;
				Val.insert(Val.end(), tmp);
			}
		}
		else
		{
			long container_size = Val.size ();
			m_Storage.write_  (&container_size, sizeof(long));
			_T::iterator end = Val.end  ();
			for(_T::iterator i = Val.begin  (); i!= end;  ++i)
				*this<<*i;
		}
			
		return *this;
	}

	
private:
	bool			m_bReadFromStorage;
	PoliError		m_Error;
	PoliStorage&	m_Storage;

protected:
	long m_nUserValue;

};

#define IMPLEMENT_MARSHAL_NO_INLINE(class_name) \
	PoliType& operator<<(PoliType& pt, class_name& a) {	return a.MarshalImpl  (pt);}

#define IMPLEMENT_MARSHAL(class_name)	\
	inline IMPLEMENT_MARSHAL_NO_INLINE(class_name)

#define IMPLEMENT_MARSHAL_ENUM(enum_name)	\
inline PoliType& operator<<(PoliType& pt, enum_name& a) \
{ \
	long tmp = a; \
	pt<<tmp; \
	a = (enum_name)tmp; \
	return pt; \
}

#define MARSHAL_AS_LONG(politype, type, name) \
		{ \
		long tmp = (long)name; \
		politype<<tmp; \
		name = (type)tmp; \
		}

#define SIMPLE_DATA_SER(name) SDataBuffer(&name, sizeof (name))




#endif //_POLITYPE_H_
