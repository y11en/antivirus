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
	virtual bool read_(void* pOutBuff, const size_t OutBuffSize) = 0;
	virtual bool write_(const void* pDataBuff, const size_t DataBuffSize) = 0;
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
	bool is_ok_() {return m_Error == eOK || m_Error == eNoErrorOldDataVersion;}
	
	
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

		try
		{
			if (is_read_from_pol_ ())
			{
				size_t len = 0;
				if (!m_Storage.read_(&len, sizeof(size_t))
					|| len > 1024*1024)
					throw eReadBinaryError;
				
				if (len > 0)
				{
					_E* pBuf = new _E[len];
					if (pBuf == NULL)
						throw eReadBinaryError;
					if (!m_Storage.read_(pBuf, len * sizeof(_E)))
					{
						delete[] pBuf;
						throw eReadBinaryError;
					}
					Val.assign(pBuf, len);
					delete[] pBuf;
				}
				else
					Val.erase(Val.begin(), Val.end());
			}
			else
			{
				size_t len = Val.size();
				if (!m_Storage.write_(&len, sizeof(size_t))
					|| len && !m_Storage.write_(Val.c_str(), len*sizeof(_E)))
					throw eWriteBinaryError;
			}
		}
		catch (PoliError err)
		{
			m_Error = err;
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

		try
		{
			if (is_read_from_pol_ ())
			{
				size_t len = 0;
				if (!m_Storage.read_(&len, sizeof(size_t))
					|| len > 1024*1024)
					throw eReadBinaryError;

				Val.erase(Val.begin(), Val.end());
				
				for (size_t i = 0; i < len; ++i)
				{
					std::pair<_K, _Ty> item;
					*this << item;
					if (error_())
						throw m_Error;
					Val.insert(item);
				}
			}
			else
			{
				size_t len = Val.size ();
				if (!m_Storage.write_(&len, sizeof(size_t)))
					throw eWriteBinaryError;
				std::map<_K, _Ty, _Pr, _A>::iterator end = Val.end  ();
				for(std::map<_K, _Ty, _Pr, _A>::iterator i = Val.begin  (); i!= end;  ++i)
				{
					*this << (*i);
					if (error_())
						throw m_Error;
				}
			}
		}
		catch (PoliError err)
		{
			m_Error = err;
		}
		
		return *this;	
	}
	template <class _FI, class _Ty> PoliType &operator<<(std::pair<_FI, _Ty> &Val)
	{
		*this << Val.first;
		*this << Val.second;
		return *this;
	}
	
	PoliType& operator<<(SDataBuffer& simple_data)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
		{
			if (!m_Storage.read_(simple_data.m_pData, simple_data.m_DataSize))
				m_Error = eReadBinaryError;
		}
		else
		{
			if (!m_Storage.write_(simple_data.m_pData, simple_data.m_DataSize))
				m_Error = eWriteBinaryError;
		}
		
		return *this;		
	}
	
	
protected:

	template<typename _T> PoliType& serialize_simple_(_T& Val)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		if (is_read_from_pol_ ())
		{
			if (!m_Storage.read_(&Val, sizeof(_T)))
				m_Error = eReadBinaryError;
		}
		else
		{
			if (!m_Storage.write_(&Val, sizeof(_T)))
				m_Error = eWriteBinaryError;
		}
		
		return *this;
	}
	
	template <typename _T> PoliType& serialize_stl_(_T& Val)
	{
		if ( (this == NULL) || error_() )
			return *this;
		
		try
		{
			if (is_read_from_pol_())
			{
				size_t len = 0;
				if (!m_Storage.read_(&len, sizeof(size_t))
					|| len > 1024*1024)
					throw eReadBinaryError;

				Val.erase(Val.begin(), Val.end());
				
				for (size_t i = 0; i < len; ++i)
				{
					_T::value_type tmp;
					*this << tmp;
					if (error_())
						throw m_Error;
					Val.insert(Val.end(), tmp);
				}
			}
			else
			{
				size_t len = Val.size ();
				if (!m_Storage.write_(&len, sizeof(size_t)))
					throw eWriteBinaryError;
				_T::iterator end = Val.end  ();
				for(_T::iterator i = Val.begin  (); i!= end;  ++i)
				{
					*this << *i;
					if (error_())
						throw m_Error;
				}
			}
		}
		catch (PoliError err)
		{
			m_Error = err;
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

/* ѕроверка if (pt.error_() != PoliType::eOK) нужна, т.к. MarshalImpl может
   восстановить eOK (например в DeclareExternalName) и тем самым начнЄт читатьс€
   следующий элемент, который читатьс€ не должен был.
*/
#define IMPLEMENT_MARSHAL_NO_INLINE(class_name) \
	PoliType& operator<<(PoliType& pt, class_name& a){	\
		if (pt.error_() != PoliType::eOK) return pt;	\
		else return a.MarshalImpl  (pt);	\
	}

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
