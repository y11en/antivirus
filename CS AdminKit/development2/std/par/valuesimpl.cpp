/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	valuesimpl.cpp
 * \author	Andrew Kazachkov
 * \date	13.09.2004 10:02:25
 * \brief	
 * 
 */

#include <build/general.h>
#include <std/base/klstd.h>
#include "./paramsimpl.h"
#include "./valuesimpl.h"
#include "common/pooled_vals.h"

#define KLCS_MODULENAME L"KLPAR"

using namespace KLSTD;
using namespace std;
using namespace KLERR;
using namespace KLPAR;
using namespace KLSTDEX;

#ifdef _WIN32
#pragma inline_recursion( on )
#pragma inline_depth( 8 )
#pragma intrinsic( memcmp, memcpy, strlen)
#endif


namespace KLPAR
{
    wchar_t* my_wcsdup(const wchar_t* src)
    {
	    size_t nBytes=src ? ((my_wcslen(src)+1)* sizeof(wchar_t)) : sizeof(wchar_t);
	    wchar_t* pResult=(wchar_t*)malloc( nBytes );
	    KLSTD_CHKMEM(pResult);
	    if(src)
		    memcpy(pResult, src, nBytes);
	    else
		    pResult[0]=0;
	    return pResult;
    };

    char* my_strdup(const char* src)
    {
	    size_t nBytes=src ? ((strlen(src)+1)* sizeof(char)) : sizeof(char);
	    char* pResult=(char*)malloc( nBytes );
	    KLSTD_CHKMEM(pResult);
	    if(src)
		    memcpy(pResult, src, nBytes);
	    else
		    pResult[0]=0;
	    return pResult;
    };

    void* my_memdup(const void* pSrc, size_t nSrc)
    {
	    if(!pSrc)
		    return NULL;
	    void* pDst=malloc(nSrc);
	    KLSTD_CHKMEM(pDst);
	    memcpy(pDst, pSrc, nSrc);
	    return pDst;
    };

    class CStringValue;
    class CDateValue;
    class CBinaryValue;
    class CParamsValue;
    class CArrayValue;
    class CParamsImpl;

	template <class T, class D, Value::Types C>
		class CPrimitiveValue;

    template<class T, class D, Value::Types C>
        void DoClone(const CPrimitiveValue<T, D, C>& , KLPAR::Value** ppRes);

    void DoClone(const CStringValue& , KLPAR::Value** ppRes);
    void DoClone(const CDateValue& , KLPAR::Value** ppRes);
    void DoClone(const CBinaryValue& , KLPAR::Value** ppRes);
    void DoClone(const CParamsValue& , KLPAR::Value** ppRes);
    void DoClone(const CArrayValue& , KLPAR::Value** ppRes);
    void DoClone(const CParamsImpl& , KLPAR::Params** ppRes);

    class CStringValue
        :   public KLSTD::KLBaseImpl<StringValue>
	{
    private:
        CStringValue(const CStringValue&);
	public:
		CStringValue()
            //:   m_data( L"" )
            //,   m_bAllocated(false)
		{;};

		void OnClone(const CStringValue& src)
		{
            //clear();
            m_data = src.m_data;
            /*
			if(src.m_bAllocated)
				m_data = my_wcsdup(src.m_data);
			else
				m_data = src.m_data;
            m_bAllocated = src.m_bAllocated;
            */
		};

		virtual ~CStringValue()
		{
			//clear();
		};

		Value::Types GetType() const
        {
			return STRING_T;
		};

		void SetValue( const wchar_t * value )
		{
			//clear();
            m_data = value;
            /*
			if(value)
            {
				m_data = my_wcsdup(value);
				m_bAllocated=true;
			};
            */
		};

        void SetValue( const KLPAR::string_t& x)
        {
            m_data = x;
        };

		void SetValueRef( const wchar_t* value )
		{
			//clear();
			//m_data=(wchar_t*)value;
            SetValue(value);
		};

		const wchar_t * GetValue() const
		{
			return m_data.get();
		};

		void Clone(Value** ppValue) const
		{
			KLSTD_CHKOUTPTR(ppValue);
            DoClone(*this, ppValue);
		};
		
		int	Compare(const Value* pValue) const
		{
			KLSTD_CHKINPTR(pValue);
			int diff=GetType()-pValue->GetType();
			if(diff)return diff;
			const wchar_t* pStr1=GetValue();
			const wchar_t* pStr2=((StringValue*)pValue)->GetValue();
			return klstd_wcscmp(pStr1?pStr1:L"", pStr2?pStr2:L"");
		};

        void Destroy()
        {
            clear();
        };
    protected:
		void clear()
		{
            m_data.clear();
            /*
			if(m_data && m_bAllocated)
            {
				free(m_data);
			};
			m_bAllocated = false;
			m_data = L"";
            */
		};		
	protected:
		//wchar_t* m_data;
		//bool m_bAllocated;
        KLPAR::string_t m_data;
	};

	template <class T, class D, Value::Types C>
		class CPrimitiveValue
            :   public KLSTD::KLBaseImpl<D>
	{
    private:
        CPrimitiveValue(const CPrimitiveValue&);
	public:
		CPrimitiveValue()
            :   m_data( 0 )
		{
			;
		};
        void OnClone(const CPrimitiveValue& src)
		{
            m_data = src.m_data;
        };

        virtual ~CPrimitiveValue()
        {
            ;
        };

		Value::Types GetType() const
        {
			return C;
		};

        void SetValue(T value)
		{
			m_data = value;
		};

        T GetValue() const
		{
			return m_data;
		};

		void Clone(Value** ppValue) const
		{
			KLSTD_CHKOUTPTR(ppValue);
            DoClone(*this, ppValue);
		};
		int	Compare(const Value* pValue) const
		{
			KLSTD_CHKINPTR(pValue);
			int diff=GetType()-pValue->GetType();
			if(diff)return diff;			
			T val1=GetValue();			
			T val2=((D*)pValue)->GetValue();
			return (val1 == val2) ? 0 :( (val1 > val2)? 1: (-1) );
		};

        void Destroy()
        {
            m_data = 0;
        };
	protected:
		T	m_data;
	};


    template <> 
        void CPrimitiveValue<time_t, DateTimeValue, Value::DATE_TIME_T>::
            SetValue(time_t value)
    {
        if(int(value) < 0)
            value = KLSTD::c_tmInvalidTime;
		m_data = value;
    }
    
    template <> 
        void CPrimitiveValue<time_t, DateTimeValue, Value::DATE_TIME_T>::
            Destroy()
    {
        m_data = KLSTD::c_tmInvalidTime;
    };

	// BoolValue
	typedef CPrimitiveValue<bool, BoolValue, Value::BOOL_T> CBoolValue;

	//IntValue
	typedef CPrimitiveValue<long, IntValue, Value::INT_T> CIntValue;

	//LongValue
	typedef CPrimitiveValue<AVP_longlong, LongValue, Value::LONG_T> CLongValue;

	//DateTimeValue
	typedef CPrimitiveValue<time_t, DateTimeValue, Value::DATE_TIME_T> CDateTimeValue;

	//FloatValue
	typedef CPrimitiveValue<float, FloatValue, Value::FLOAT_T> CFloatValue;

	//DoubleValue
	typedef CPrimitiveValue<double, DoubleValue, Value::DOUBLE_T> CDoubleValue;


    class CDateValue : public KLSTD::KLBaseImpl<DateValue>
	{
    private:
        CDateValue(const CDateValue&);
    public:
		CDateValue()
            :   m_data("")
            ,   m_bAllocated(false)
		{;};

		void OnClone(const CDateValue& src)
		{
            clear();
			if(src.m_bAllocated)
				m_data=my_strdup(src.m_data);
			else
				m_data=src.m_data;
            m_bAllocated = src.m_bAllocated;
		};

		virtual ~CDateValue()
		{
			clear();
		};

		Value::Types GetType() const
        {
			return Value::DATE_T;
		};

		void SetValue( const char * value )
		{
			clear();
			if(value)
            {
				m_data=my_strdup(value);
				m_bAllocated=true;
			};
		};

        const char * GetValue() const
		{
			return m_data;
		};

		void Clone(Value** ppValue) const
		{
			KLSTD_CHKOUTPTR(ppValue);
            DoClone(*this, ppValue);
		};

		int	Compare(const Value* pValue) const
		{
			KLSTD_CHKINPTR(pValue);
			int diff=GetType()-pValue->GetType();
			if(diff)return diff;
			const char* pStr1=GetValue();
			const char* pStr2=((DateValue*)pValue)->GetValue();
			return strcmp(pStr1?pStr1:"", pStr2?pStr2:"");
		};

        void Destroy()
        {
            clear();
        };
	protected:
		char*   m_data;
		bool    m_bAllocated;
		void clear()
		{
			if(m_data && m_bAllocated)
            {
				free(m_data);
			};
			m_bAllocated=false;
			m_data="";			
		};		
    };

	class CBinaryValue: public KLSTD::KLBaseImpl<BinaryValue>
	{
    private:
        CBinaryValue(const CBinaryValue&);
	public:
		CBinaryValue()
		{
			clear(true);
		};

		void OnClone(const CBinaryValue& src)
		{
            clear();
			if(src.m_pCounter)
				::KLSTD_InterlockedIncrement(src.m_pCounter);
			m_pCounter=src.m_pCounter;
			m_pCallback=src.m_pCallback;
			m_pContext=src.m_pContext;
            //KLSTD_ASSERT_THROW((src.m_pData && src.m_nData > 0) || (!src.m_pData && src.m_nData == 0));
			if(m_pCounter)
            {
                m_pData = src.m_pData;
                m_nData = src.m_nData;
            }
			else if(src.m_pChunk && src.m_pChunk->GetDataSize())
            {
                KLSTD_AllocMemoryChunk(src.m_pChunk->GetDataSize(), &m_pChunk);
                memcpy(m_pChunk->GetDataPtr(), src.m_pChunk->GetDataPtr(), m_pChunk->GetDataSize());
            };			
		};

		virtual ~CBinaryValue()
		{
			clear();
		};
		//Implement Value
		Value::Types GetType() const
		{
			return Value::BINARY_T;
		};
        void SetValue(KLSTD::MemoryChunkPtr pChunk)
        {
            clear();
            m_pChunk = pChunk;
        };
		//Implement BinaryValue
		void SetValue( void * value, size_t bytes)
		{
			clear();
            KLSTD_ASSERT_THROW((value && bytes > 0) || (!value && bytes == 0)) ;
			if(value)
            {                
                KLSTD_AllocMemoryChunk(bytes, &m_pChunk);
				memcpy(m_pChunk->GetDataPtr(), value, bytes);
			};
		};
		void SetValue( void * value, size_t bytes, FreeBufferCallback callback, void * context)
		{
			clear();
            KLSTD_CHKINPTR(callback);
            KLSTD_ASSERT_THROW((value && bytes > 0) || (!value && bytes == 0)) ;
            ;			
			m_pData = (unsigned char*)value;			
			m_nData = bytes;
			m_pCallback=callback;
			m_pContext=context;
			if(callback)
            {
                m_pCounter=new long(1);
                KLSTD_CHKMEM(m_pCounter);
            };
        };

		void* GetValue() const
		{
            return m_pChunk?m_pChunk->GetDataPtr():m_pData;
		};

		size_t GetSize() const
		{
            return m_pChunk?m_pChunk->GetDataSize():m_nData;
		};

		void Clone(Value** ppValue) const
		{
			KLSTD_CHKOUTPTR(ppValue);
            DoClone(*this, ppValue);
		};

		int	Compare(const Value* pValue) const
		{
			KLSTD_CHKINPTR(pValue);
			int diff=GetType()-pValue->GetType();
			if(diff)return diff;
			int nSize1=GetSize();
			int nSize2=((BinaryValue*)pValue)->GetSize();
			int diff_sizes=nSize1 - nSize2;
			if(diff_sizes)
                return diff_sizes;
			if(!nSize1)
                return 0;
			const void* p1=GetValue();
			const void* p2=((BinaryValue*)pValue)->GetValue();
			return memcmp(p1, p2, nSize1);
		};

        void Destroy()
        {
            clear();
        };

	protected:
        KLSTD::MemoryChunkPtr m_pChunk;
        void*               m_pData;
        size_t              m_nData;
		FreeBufferCallback	m_pCallback;
		void*				m_pContext;
		long*				m_pCounter;

		void clear(bool bInConstructor = false)
		{
			if(!bInConstructor)
            {
				if(m_pCounter)
                {
					KLSTD_ASSERT(m_pCallback && m_pData);
					long lCounter=::KLSTD_InterlockedDecrement(m_pCounter);
					KLSTD_ASSERT(lCounter >=0);
					if(!lCounter)
                    {
						if(m_pCallback)
							m_pCallback(m_pData, m_pContext);
						delete m_pCounter;
					};
				};
            };
			m_pData = NULL;
			m_nData = 0;
			m_pCallback=NULL;
			m_pContext=NULL;
			m_pCounter=NULL;
            m_pChunk = NULL;
		};
	};

    class CArrayValue: public KLSTD::KLBaseImpl<ArrayValue>
    {
    private:
        CArrayValue(const CArrayValue&);
	public:
        CArrayValue()
	    {;};

	    ~CArrayValue()
	    {
		    ;
	    };

        void OnClone(const CArrayValue& src)
        {
            m_contents.clear();
		    m_contents.resize(src.m_contents.size());
		    for(size_t i=0; i < src.m_contents.size(); ++i)
		    {
			    CAutoPtr<Value> pNewVal;
			    if(!src.m_contents[i].m_T)
				    continue;
			    src.m_contents[i].m_T->Clone(&pNewVal);
			    m_contents[i] = pNewVal;
		    };
        };

	    //Implement Value
	    Value::Types GetType() const
	    {
		    return Value::ARRAY_T;
	    };
	    //Implement ArrayValue
	    void SetSize(size_t size)
	    {
		    if(size < 0)
			    KLSTD_THROW_BADPARAM(size);
		    m_contents.resize(size);
	    };
	    size_t GetSize() const
	    {
		    return m_contents.size();
	    };
	    void SetAt(size_t nIndex, Value* value)
	    {
		    if(nIndex < 0 || nIndex >= (int)m_contents.size())
                KLSTD_THROW(KLSTD::STDE_BOUND);
		    m_contents.at(nIndex) = value;
	    };
        void GetAt(size_t nIndex, Value** value)
	    {
		    if(nIndex < 0 || nIndex >= (int)m_contents.size())
			    KLSTD_THROW(STDE_BOUND);
		    *value=m_contents.at(nIndex).m_T;
		    if(*value)
			    (*value)->AddRef();
	    };
	    const Value* GetAt(size_t nIndex) const
	    {
		    if(nIndex < 0 || nIndex >= (int)m_contents.size())
			    KLSTD_THROW(STDE_BOUND);
		    return m_contents.at(nIndex).m_T;
	    };

	    void Clone(Value** ppValue) const
	    {
			KLSTD_CHKOUTPTR(ppValue);
            DoClone(*this, ppValue);
	    };
	    int	Compare(const Value* pValue) const
	    {
		    KLSTD_CHKINPTR(pValue);
		    int diff=GetType()-pValue->GetType();
		    if(diff)return diff;
		    int nSize1=GetSize();
		    int nSize2=((ArrayValue*)pValue)->GetSize();
		    int diff_sizes=nSize1-nSize2;
		    if(diff_sizes)return diff_sizes;
		    if(!nSize1)return 0;
		    for(int i=0; i < nSize1; ++i){				
			    const Value *pVal1=GetAt(i);
			    const Value *pVal2=((ArrayValue*)pValue)->GetAt(i);
			    if(!pVal1 && pVal2)
				    return -1;
			    if(pVal1 && !pVal2)
				    return 1;
			    if(pVal1 && pVal2){
				    int result=pVal1->Compare(pVal2);
				    if(result)
					    return result;
			    };
		    };
		    return 0;
	    };
    
        void Destroy()
        {
            m_contents.clear();
        };
    protected:
        typedef std::vector<KLSTD::KLAdapt<KLSTD::CAutoPtr<Value> > > values_t;
		values_t	m_contents;
    };

	class CParamsValue  : public KLSTD::KLBaseImpl<ParamsValue>
	{
    private:
        CParamsValue(const CParamsValue&);
	public:
        CParamsValue()
	    {
            ;
	    };

        void OnAfterAlloc()
        {
            m_pParams = NULL;
            KLPAR_CreateParams(&m_pParams);
        };

        void OnClone(const CParamsValue& src)
	    {
            m_pParams = NULL;
		    Params* pSrcParams = src.m_pParams;
		    if(pSrcParams)
                pSrcParams->Clone(&m_pParams);
            else
                m_pParams=NULL;
	    };

	    ~CParamsValue()
	    {
		    ;
	    };
	    //Implement Value			
	    Value::Types GetType() const
        {
		    return PARAMS_T;
	    };
	    //ParamsValue
	    void SetValue( Params * value )
	    {
		    m_pParams=value;
	    };
	    Params * GetValue() const
	    {
		    return m_pParams;
	    };
	    void Clone(Value** ppValue) const
	    {
			KLSTD_CHKOUTPTR(ppValue);
            DoClone(*this, ppValue);
	    };
	    int	Compare(const Value* pValue) const
	    {
		    KLSTD_CHKINPTR(pValue);
		    int diff=GetType()-pValue->GetType();
		    if(diff)return diff;
		    const Params* pVal1=GetValue();
		    const Params* pVal2=((ParamsValue*)pValue)->GetValue();
		    if(!pVal1 && pVal2)
			    return -1;
		    if(pVal1 && !pVal2)
			    return 1;
		    if(!pVal1 && !pVal2)
			    return 0;
		    return pVal1->Compare(pVal2);
	    };

        void Destroy()
        {
            m_pParams = NULL;
        };
	protected:
		KLSTD::CAutoPtr<Params>	m_pParams;
	};

    class CValuesFactory
        :   public KLSTD::KLBaseImpl<ValuesFactory>
        ,   public KLSTDEX::ValuesPool<CStringValue>
        ,   public KLSTDEX::ValuesPool<CBoolValue>
        ,   public KLSTDEX::ValuesPool<CIntValue>
        ,   public KLSTDEX::ValuesPool<CLongValue>
        ,   public KLSTDEX::ValuesPool<CDateTimeValue>
        ,   public KLSTDEX::ValuesPool<CDateValue>
        ,   public KLSTDEX::ValuesPool<CBinaryValue>
        ,   public KLSTDEX::ValuesPool<CFloatValue>
        ,   public KLSTDEX::ValuesPool<CDoubleValue>
        ,   public KLSTDEX::ValuesPool<CParamsValue>
        ,   public KLSTDEX::ValuesPool<CArrayValue>
        ,   public KLSTDEX::ValuesPool<CParamsImpl>
    {
    public:
        CValuesFactory();
        virtual ~CValuesFactory();    
	public:
		void CreateStringValue(StringValue** ppVal);
		void CreateBoolValue(BoolValue** ppVal);
		void CreateIntValue(IntValue** ppVal);
		void CreateLongValue(LongValue** ppVal);
		void CreateDateTimeValue(DateTimeValue** ppVal);
		void CreateDateValue(DateValue** ppVal);
		void CreateBinaryValue(BinaryValue** ppVal);
		void CreateFloatValue(FloatValue** ppVal);
		void CreateDoubleValue(DoubleValue** ppVal);
		void CreateParamsValue(ParamsValue** ppVal);
		void CreateArrayValue(ArrayValue** ppVal);
        void CreateParams(Params** ppVal);
    public:
        KLSTD_INLINEONLY KLSTD::CAutoPtr<CStringValue>   CreateStringValue()
        {
            return ValuesPool<CStringValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CBoolValue>     CreateBoolValue()
        {
            return ValuesPool<CBoolValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CIntValue>      CreateIntValue()
        {
            return ValuesPool<CIntValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CLongValue>     CreateLongValue()
        {
            return ValuesPool<CLongValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CDateTimeValue> CreateDateTimeValue()
        {
            return ValuesPool<CDateTimeValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CDateValue>     CreateDateValue()
        {
            return ValuesPool<CDateValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CBinaryValue>   CreateBinaryValue()
        {
            return ValuesPool<CBinaryValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CFloatValue>    CreateFloatValue()
        {
            return ValuesPool<CFloatValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CDoubleValue>   CreateDoubleValue()
        {
            return ValuesPool<CDoubleValue>::AllocInstance2();
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CParamsValue>   CreateParamsValue()
        {
            KLSTD::CAutoPtr<CParamsValue> pRes = ValuesPool<CParamsValue>::AllocInstance2();
            pRes->OnAfterAlloc();
            return pRes;
        };
		KLSTD_INLINEONLY KLSTD::CAutoPtr<CArrayValue>    CreateArrayValue()
        {
            return ValuesPool<CArrayValue>::AllocInstance2();
        };
        KLSTD_INLINEONLY KLSTD::CAutoPtr<CParamsImpl>    CreateParams()
        {
            return ValuesPool<CParamsImpl>::AllocInstance2();
        };
    protected:    
        typedef KLSTDEX::ValuesPool<CStringValue>::pooled_val_ptr_t    StringValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CBoolValue>::pooled_val_ptr_t      BoolValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CIntValue>::pooled_val_ptr_t       IntValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CLongValue>::pooled_val_ptr_t      LongValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CDateTimeValue>::pooled_val_ptr_t  DateTimeValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CDateValue>::pooled_val_ptr_t      DateValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CBinaryValue>::pooled_val_ptr_t    BinaryValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CFloatValue>::pooled_val_ptr_t     FloatValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CDoubleValue>::pooled_val_ptr_t    DoubleValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CParamsValue>::pooled_val_ptr_t    ParamsValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CArrayValue>::pooled_val_ptr_t     ArrayValueImplPooledPtr;
        typedef KLSTDEX::ValuesPool<CParamsImpl>::pooled_val_ptr_t     CParamsImplPooledPtr;
    protected:
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
    };

    /************************************************************************/
    /*   CValuesFactory                                                     */
    /************************************************************************/

    void CValuesFactory::CreateStringValue(StringValue** ppVal)
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<StringValue>)
                (StringValue*)
                    (CStringValue*)
                        ValuesPool<CStringValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateBoolValue(BoolValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<BoolValue>)
                (BoolValue*)
                    (CBoolValue*)
                        ValuesPool<CBoolValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateIntValue(IntValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<IntValue>)
                (IntValue*)
                    (CIntValue*)
                        ValuesPool<CIntValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateLongValue(LongValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<LongValue>)
                (LongValue*)
                    (LongValue*)
                        ValuesPool<CLongValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateDateTimeValue(DateTimeValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<DateTimeValue>)
                (DateTimeValue*)
                    (CDateTimeValue*)
                        ValuesPool<CDateTimeValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateDateValue(DateValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<DateValue>)
                (DateValue*)
                    (CDateValue*)
                        ValuesPool<CDateValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateBinaryValue(BinaryValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<BinaryValue>)
                (BinaryValue*)
                    (CBinaryValue*)
                        ValuesPool<CBinaryValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateFloatValue(FloatValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<FloatValue>)
                (FloatValue*)
                    (CFloatValue*)
                        ValuesPool<CFloatValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateDoubleValue(DoubleValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<DoubleValue>)
                (DoubleValue*)
                    (CDoubleValue*)
                        ValuesPool<CDoubleValue>::AllocInstance2()).Detach();
	};

	void CValuesFactory::CreateParamsValue(ParamsValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        KLSTD::CAutoPtr<CParamsValue> pRes;
        pRes.Attach( ValuesPool<CParamsValue>::AllocInstance2().Detach());
        pRes->OnAfterAlloc();
        *ppVal = ((KLSTD::CAutoPtr<ParamsValue>)
                (ParamsValue*)
                    (CParamsValue*)pRes).Detach();
	};

	void CValuesFactory::CreateArrayValue(ArrayValue** ppVal) 
	{
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<ArrayValue>)
                (ArrayValue*)
                    (CArrayValue*)
                        ValuesPool<CArrayValue>::AllocInstance2()).Detach();
	};

    void CValuesFactory::CreateParams(Params** ppVal)
    {
        KLSTD_CHKOUTPTR(ppVal);
        *ppVal = ((KLSTD::CAutoPtr<Params>)
                (Params*)
                    (CParamsImpl*)
                        ValuesPool<CParamsImpl>::AllocInstance2()).Detach();
    };

    const size_t g_nDefPoolSizeBase = 10;

    size_t g_nPoolSizeBase = g_nDefPoolSizeBase;

    size_t GetPoolSize(size_t nMult)
    {
        size_t nRes = g_nPoolSizeBase * nMult;
        //if(!nRes)
            //nRes = 1;
        return nRes;
    };

    CValuesFactory::CValuesFactory()
    {
        KLSTD_CreateCriticalSection(&m_pCS);
	    ValuesPool<CStringValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(10));
        ValuesPool<CBoolValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(10));
        ValuesPool<CIntValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(20));
        ValuesPool<CLongValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(5));
        ValuesPool<CDateTimeValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(5));
        ValuesPool<CDateValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(1));
        ValuesPool<CBinaryValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(3));
        ValuesPool<CFloatValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(1));
        ValuesPool<CDoubleValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(1));
        ValuesPool<CParamsValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(60));
        ValuesPool<CArrayValue>::ValuesPoolInitialize(m_pCS, GetPoolSize(5));
        ValuesPool<CParamsImpl>::ValuesPoolInitialize(m_pCS, GetPoolSize(60));
    };

    CValuesFactory::~CValuesFactory()
    {
        ;
    };

    KLCSC_DECL void SetPoolSizeBase(size_t nBase)
    {
        g_nPoolSizeBase = nBase;
    };

    KLCSC_DECL size_t GetPoolSizeBase()
    {
        return g_nPoolSizeBase;
    };

    CAutoPtr<ValuesFactory> g_pFactory;
    CAutoPtr<CValuesFactory> g_pFullFactory;

    void SerCreateStringValue(const KLPAR::string_t& x, StringValue** pValue)
    {
        KLSTD::CAutoPtr<CStringValue>   pRes;
        pRes.Attach(g_pFullFactory->CreateStringValue().Detach());
        pRes->SetValue(x);
        *pValue = pRes.Detach();
    };
    
    KLCSC_DECL void SerCreateBinaryValue(KLSTD::MemoryChunkPtr pChunk, BinaryValue** pValue)
    {
        KLSTD::CAutoPtr<CBinaryValue>   pRes;
        pRes.Attach(g_pFullFactory->CreateBinaryValue().Detach());
        pRes->SetValue(pChunk);
        *pValue = pRes.Detach();
    };


    template<>
        void DoClone<bool, BoolValue, Value::BOOL_T>(
            const CBoolValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CBoolValue> pRes = g_pFullFactory->CreateBoolValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(BoolValue*)(CBoolValue*)pRes;
        pResVal.CopyTo(ppRes);
    };

    template<>
        void DoClone<long, IntValue, Value::INT_T>(
            const CIntValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CIntValue> pRes = g_pFullFactory->CreateIntValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(IntValue*)(CIntValue*)pRes;
        pResVal.CopyTo(ppRes);
    };

    template<>
        void DoClone<AVP_longlong, LongValue, Value::LONG_T>(
            const CLongValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CLongValue> pRes = g_pFullFactory->CreateLongValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(LongValue*)(CLongValue*)pRes;
        pResVal.CopyTo(ppRes);
    };

    template<>
        void DoClone<time_t, DateTimeValue, Value::DATE_TIME_T>(
            const CDateTimeValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CDateTimeValue> pRes = g_pFullFactory->CreateDateTimeValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(DateTimeValue*)(CDateTimeValue*)pRes;
        pResVal.CopyTo(ppRes);
    };

    template<>
        void DoClone<float, FloatValue, Value::FLOAT_T>(
            const CFloatValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CFloatValue> pRes = g_pFullFactory->CreateFloatValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(FloatValue*)(CFloatValue*)pRes;
        pResVal.CopyTo(ppRes);
    };

    template<>
        void DoClone<double, DoubleValue, Value::DOUBLE_T>(
            const CDoubleValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CDoubleValue> pRes = g_pFullFactory->CreateDoubleValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(DoubleValue*)(CDoubleValue*)pRes;
        pResVal.CopyTo(ppRes);
    };

    void DoClone(const CStringValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CStringValue> pRes = g_pFullFactory->CreateStringValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(StringValue*)(CStringValue*)pRes;
        pResVal.CopyTo(ppRes);
    };
    
    void DoClone(const CDateValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CDateValue> pRes = g_pFullFactory->CreateDateValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(DateValue*)(CDateValue*)pRes;
        pResVal.CopyTo(ppRes);
    };
    
    void DoClone(const CBinaryValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CBinaryValue> pRes = g_pFullFactory->CreateBinaryValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(BinaryValue*)(CBinaryValue*)pRes;
        pResVal.CopyTo(ppRes);
    };
    
    void DoClone(const CParamsValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CParamsValue> pRes = g_pFullFactory->CreateParamsValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(ParamsValue*)(CParamsValue*)pRes;
        pResVal.CopyTo(ppRes);
    };
    
    void DoClone(const CArrayValue& src, KLPAR::Value** ppRes)
    {
        KLSTD::CAutoPtr<CArrayValue> pRes = g_pFullFactory->CreateArrayValue();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Value> pResVal;
        pResVal = (Value*)(ArrayValue*)(CArrayValue*)pRes;
        pResVal.CopyTo(ppRes);
    };
    
    /*
    void DoClone(const CParamsImpl& src, KLPAR::Params** ppRes)
    {
        KLSTD::CAutoPtr<CParamsImpl> pRes = g_pFullFactory->CreateParams();
        pRes->OnClone(src);
        KLSTD::CAutoPtr<Params> pResVal;
        pResVal = (Params*)(CParamsImpl*)pRes;
        pResVal.CopyTo(ppRes);
    };
    */

    void InitValues()
    {
        g_pFactory = NULL;
        g_pFullFactory = NULL;

        g_pFullFactory.Attach( new CValuesFactory);
        g_pFactory = (ValuesFactory*)(CValuesFactory*)g_pFullFactory;
    };

    void DeinitValues()
    {
        g_pFactory = NULL;
        g_pFullFactory = NULL;
    };
};

KLCSC_DECL void KLPAR_CreateParams(KLPAR::Params** ppParams) 
{
    KLSTD_CHKOUTPTR(ppParams);
    KLSTD::CAutoPtr<KLPAR::Params> pResult = (KLPAR::Params*)(CParamsImpl*)
                        g_pFullFactory->CreateParams();
    //pResult.CopyTo(ppParams);
    *ppParams = pResult.Detach();
};

namespace KLPAR
{
    void DoCreateParams(KLPAR::CParamsImpl** ppX) 
    {
        KLSTD_CHKOUTPTR(ppX);
        KLSTD::CAutoPtr<KLPAR::CParamsImpl> pResult = (CParamsImpl*)
                            g_pFullFactory->CreateParams();
        *ppX = pResult.Detach();
    };
};