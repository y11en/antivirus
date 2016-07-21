#ifndef __KLPAR_CONV_H__
#define __KLPAR_CONV_H__

#include <std/par/params.h>

namespace KLPAR
{
    
    inline KLSTD::CAutoPtr<KLPAR::Value> GetVal(KLPAR::Params* pData, const wchar_t* szwName)
    {
        KLSTD::CAutoPtr<KLPAR::Value> pVal;
        if(pData)
            pData->GetValueNoThrow(KLSTD::FixNullString(szwName), &pVal);
        return pVal;
    };

    inline KLSTD::CAutoPtr<KLPAR::Value> GetVal(KLPAR::ArrayValue* pData, size_t nIndex)
    {
        KLSTD::CAutoPtr<KLPAR::Value> pVal;
        if(pData)
            pData->GetAt(nIndex, &pVal);
        return pVal;
    };

    inline bool ParVal(KLPAR::Value* pX, bool def)
    {
        return (pX && pX->GetType() == KLPAR::Value::BOOL_T)
                    ?   static_cast<KLPAR::BoolValue*>(pX)->GetValue()
                    :   def;
    };

    inline long ParVal(KLPAR::Value* pX, long def)
    {
        return (pX && pX->GetType() == KLPAR::Value::INT_T)
                    ?   static_cast<KLPAR::IntValue*>(pX)->GetValue()
                    :   def;
    };

    inline AVP_longlong ParVal(KLPAR::Value* pX, AVP_longlong def)
    {
        return (pX && pX->GetType() == KLPAR::Value::LONG_T)
                    ?   static_cast<KLPAR::LongValue*>(pX)->GetValue()
                    :   def;
    };

    inline const wchar_t* ParVal(KLPAR::Value* pX, const wchar_t* def)
    {
        return (pX && pX->GetType() == KLPAR::Value::STRING_T)
                    ?   static_cast<KLPAR::StringValue*>(pX)->GetValue()
                    :   def;
    };

    inline const std::wstring ParVal(KLPAR::Value* pX, const std::wstring& def)
    {
        return (pX && pX->GetType() == KLPAR::Value::STRING_T)
                    ?   std::wstring(static_cast<KLPAR::StringValue*>(pX)->GetValue())
                    :   def;
    };

    inline KLPAR::time_wrapper_t ParVal(KLPAR::Value* pX, const KLPAR::time_wrapper_t& def)
    {
        return (pX && pX->GetType() == KLPAR::Value::DATE_TIME_T)
                    ?   KLPAR::time_wrapper_t(static_cast<KLPAR::DateTimeValue*>(pX)->GetValue())
                    :   def;
    };
    
    class CLongConvertor : public std::unary_function<KLPAR::Value*, long>
    {
    public:
        result_type operator()(argument_type p)
        {
            KLPAR_CHKTYPE(p, INT_T,p);
            return static_cast<KLPAR::IntValue*>(p)->GetValue();
        };
    };

    class CUShortConvertor : public std::unary_function<KLPAR::Value*, unsigned short>
    {
    public:
        result_type operator()(argument_type p)
        {
            KLPAR_CHKTYPE(p, INT_T, p);
            return (result_type)static_cast<KLPAR::IntValue*>(p)->GetValue();
        };
    };

    class CIntConvertor : public std::unary_function<KLPAR::Value*, int>
    {
    public:
        result_type operator()(argument_type p)
        {
            KLPAR_CHKTYPE(p, INT_T, p);
            return static_cast<KLPAR::IntValue*>(p)->GetValue();
        };
    };

    class CStringPtrConvertor : public std::unary_function<KLPAR::Value*, const wchar_t*>
    {
    public:
        result_type operator()(argument_type p)
        {
            KLPAR_CHKTYPE(p, STRING_T, p);
            return static_cast<KLPAR::StringValue*>(p)->GetValue();
        };
    };

    class CStringConvertor : public std::unary_function<KLPAR::Value*, std::wstring>
    {
    public:
        result_type operator()(argument_type p)
        {
            KLPAR_CHKTYPE(p, STRING_T, p);
            return KLSTD::FixNullString(static_cast<KLPAR::StringValue*>(p)->GetValue());
        };
    };

    template<class T, class F>
    void MakeStdVector(
                    KLPAR::ArrayValue*  pArray,
                    std::vector<T>&     vecData,
                    F                   f)
    {
        vecData.clear();
        if(!pArray)
            return;
        const int nSize = pArray->GetSize();
        vecData.reserve(nSize);
        for(int i=0; i < nSize; ++i)
        {            
            KLSTD::CAutoPtr<KLPAR::Value>   pValue;
            pArray->GetAt(i, &pValue);
            if(!pValue)
                continue;
            vecData.push_back(f(pValue));
        };
    };

    template<class T>
        std::vector<T> MakeStdVector(const T* pData, const int nData)
    {        
        std::vector<T> result;
        result.reserve(nData);
        for(int i=0; i < nData; ++i)
            result.push_back(pData[i]);
        return result;
    }

    template<class T>
        void MakeStdVector(const T* pData, const int nData, std::vector<T>& result)
    {
        result.clear();
        result.reserve(nData);
        for(int i=0; i < nData; ++i)
            result.push_back(pData[i]);
        return result;
    }

	class ParAdapt
	{
	public:
		ParAdapt()
		{
		};

		ParAdapt(const Params* rSrc)
		{
			m_T = const_cast<Params*>(rSrc);
		};

		ParAdapt(const ParAdapt& rSrCA)
		{
			m_T = const_cast<ParAdapt&>(rSrCA).m_T;
		};

		ParAdapt& operator=(const ParAdapt& rSrc)
		{
			m_T = const_cast<ParAdapt&>(rSrc).m_T;
			return *this;
		};

        int compare(const ParAdapt& rSrc) const
        {
            if(!m_T || !rSrc.m_T)
                return int(!!m_T) - int((!!rSrc.m_T));
            return m_T->Compare(rSrc.m_T);
        };
		
        bool operator<(const ParAdapt& rSrc) const
		{
            return compare(rSrc) < 0;
		};
		
        bool operator==(const ParAdapt& rSrc) const
		{
			return compare(rSrc) == 0;
		};

        bool operator != (const ParAdapt& rSrc) const
		{
			return compare(rSrc) != 0;
		};

        bool operator > (const ParAdapt& rSrc) const
		{
			return compare(rSrc) > 0;
		};
		
        operator KLPAR::ParamsPtr()
		{
			return m_T;
		}

        KLPAR::ParamsPtr m_T;
	};
    
    inline KLPAR::ParAdapt ParVal(KLPAR::Value* pX, const KLPAR::ParAdapt& def)
    {
        return (pX && pX->GetType() == KLPAR::Value::PARAMS_T)
                    ?   KLPAR::ParAdapt(static_cast<KLPAR::ParamsValue*>(pX)->GetValue())
                    :   def;
    };

	class BinaryAdapt
	{
	public:
		BinaryAdapt()
		{
		};

		BinaryAdapt(const BinaryValue* rSrc)
		{
			m_T = const_cast<BinaryValue*>(rSrc);
		};

		BinaryAdapt(const BinaryAdapt& rSrCA)
		{
			m_T = const_cast<BinaryAdapt&>(rSrCA).m_T;
		};

		BinaryAdapt& operator=(const BinaryAdapt& rSrc)
		{
			m_T = const_cast<BinaryAdapt&>(rSrc).m_T;
			return *this;
		};

        int compare(const BinaryAdapt& rSrc) const
        {
            if(!m_T || !rSrc.m_T)
                return int(!!m_T) - int((!!rSrc.m_T));
            return m_T->Compare(rSrc.m_T);
        };
		
        bool operator<(const BinaryAdapt& rSrc) const
		{
            return compare(rSrc) < 0;
		};
		
        bool operator==(const BinaryAdapt& rSrc) const
		{
			return compare(rSrc) == 0;
		};

        bool operator != (const BinaryAdapt& rSrc) const
		{
			return compare(rSrc) != 0;
		};

        bool operator > (const BinaryAdapt& rSrc) const
		{
			return compare(rSrc) > 0;
		};
		
        operator KLPAR::BinaryValuePtr()
		{
			return m_T;
		}

        KLPAR::BinaryValuePtr m_T;
	};

    inline KLPAR::BinaryAdapt ParVal(KLPAR::Value* pX, const KLPAR::BinaryAdapt& def)
    {
        return (pX && pX->GetType() == KLPAR::Value::BINARY_T)
                    ?   KLPAR::BinaryAdapt(static_cast<KLPAR::BinaryValue*>(pX))
                    :   def;
    };
    
	class ArrayAdapt
	{
	public:
		ArrayAdapt()
		{
		};

		ArrayAdapt(const ArrayValue* rSrc)
		{
			m_T = const_cast<ArrayValue*>(rSrc);
		};

		ArrayAdapt(const ArrayAdapt& rSrCA)
		{
			m_T = const_cast<ArrayAdapt&>(rSrCA).m_T;
		};

		ArrayAdapt& operator=(const ArrayAdapt& rSrc)
		{
			m_T = const_cast<ArrayAdapt&>(rSrc).m_T;
			return *this;
		};

        int compare(const ArrayAdapt& rSrc) const
        {
            if(!m_T || !rSrc.m_T)
                return int(!!m_T) - int((!!rSrc.m_T));
            return m_T->Compare(rSrc.m_T);
        };
		
        bool operator<(const ArrayAdapt& rSrc) const
		{
            return compare(rSrc) < 0;
		};
		
        bool operator==(const ArrayAdapt& rSrc) const
		{
			return compare(rSrc) == 0;
		};

        bool operator != (const ArrayAdapt& rSrc) const
		{
			return compare(rSrc) != 0;
		};

        bool operator > (const ArrayAdapt& rSrc) const
		{
			return compare(rSrc) > 0;
		};
		
        operator KLPAR::ArrayValuePtr()
		{
			return m_T;
		}

        KLPAR::ArrayValuePtr m_T;
	};

    inline KLPAR::ArrayAdapt ParVal(KLPAR::Value* pX, const KLPAR::ArrayAdapt& def)
    {
        return (pX && pX->GetType() == KLPAR::Value::ARRAY_T)
                    ?   KLPAR::ArrayAdapt(static_cast<KLPAR::ArrayValue*>(pX))
                    :   def;
    };

};   

    /*!
      \brief	Copies some values from pSrc into pDst. 

      \param	pSrc        source container
      \param	pDst        destination container
      \param	pszwNames   array of names
      \param	nNames      length of array of names
      \param	bClone      if to clone values or only copy references
    */
    KLCSC_DECL void KLPAR_CopyValues(
                        KLPAR::Params*  pSrc, 
                        KLPAR::Params*  pDst, 
                        const wchar_t** pszwNames, 
                        size_t          nNames, 
                        bool            bClone = true);

#endif //__KLPAR_CONV_H__
