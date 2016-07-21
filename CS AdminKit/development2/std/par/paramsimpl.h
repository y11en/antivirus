/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	paramsimpl.h
 * \author	Andrew Kazachkov
 * \date	08.09.2004 10:01:18
 * \brief	
 * 
 */

#ifndef __PARAMSIMPL_H__
#define __PARAMSIMPL_H__

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <std/par/valenum.h>
#include <std/thr/locks.h>
#include <std/par/parstrdata.h>

#include <map>
#include <hash_map>
#include <memory>

//#define KLPAR_USE_HASHMAP

namespace KLPAR
{
    struct HashFun_string_t
    {
        size_t operator()(const KLPAR::string_t& __s) const
        {
            unsigned long __h = 0;
            const size_t __len = __s.getLength();
            const wchar_t* __data = __s.get();
            for ( size_t __i = 0; __i < __len; ++__i)
                __h = 5*__h + __data[__i];
            return size_t(__h);
        }
    };

    typedef std::pair<KLPAR::string_t, 
                        KLSTD::KLAdapt< ValuePtr > > params_pair_t;

    class CParamsNodeAllocator
    {
    public:
        CParamsNodeAllocator();
        ~CParamsNodeAllocator();
        void* allocate(size_t n, const void *hint);
        void deallocate(void* p, size_t n);
    protected:
#if defined(_WIN32) && !defined(_DEBUG)
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
        std::vector<void*>  m_vecAllocated_Size1;
#ifdef KLPAR_USE_HASHMAP
        std::vector<void*>  m_vecAllocated_Size2;
#endif
        const size_t        m_nMaxAllocated;
#endif
    };

    extern KLSTD::CPointer<CParamsNodeAllocator> g_pParamsNodeAllocator;


    template<class T>
    class ParamsAlloc
        :   public std::allocator<T>
    {
    public:
        pointer allocate(size_type n, const void *hint)
        {
            return (pointer)g_pParamsNodeAllocator->allocate(sizeof(value_type)*n, hint);
        };
        void deallocate(pointer p, size_type n)
        {
            g_pParamsNodeAllocator->deallocate(p, sizeof(value_type)*n);
        };
    };

    typedef KLPAR::ParamsAlloc< params_pair_t > ParamsAllocPair;
};


#ifdef _WIN32
    namespace _STL
    {
    template <class _Tp1, class _Tp2>
        inline KLPAR::ParamsAlloc<_Tp2>& __cdecl
        __stl_alloc_rebind(KLPAR::ParamsAlloc<_Tp1>& __a, const _Tp2*) { return (KLPAR::ParamsAlloc<_Tp2>&)(__a); }
        template <class _Tp1, class _Tp2>
        inline KLPAR::ParamsAlloc<_Tp2> __cdecl
        __stl_alloc_create(const KLPAR::ParamsAlloc<_Tp1>&, const _Tp2*) { return KLPAR::ParamsAlloc<_Tp2>(); }
    }
#endif


namespace KLPAR
{
    class KLSTD_NOVTABLE ParamsImplBase
        :   public KLPAR::Params
        ,   public KLPAR::EnumValues
    {
    };

    class CParamsImpl
        :   public KLSTD::KLBaseImpl<ParamsImplBase>
	{
		//friend class CParamsValue;
		//friend 	void ParamsForSoapI(struct soap*	soap, const Params*	pParams, param__params&	params);
		//friend 	void ParamsFromSoapI(param__params&	params, Params** ppParams);
	public:
		CParamsImpl();
		virtual ~CParamsImpl();

        //void OnClone(const CParamsImpl& src);

        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::Params)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPAR::EnumValues)
        KLSTD_INTERAFCE_MAP_END()

    public:
    //EnumValues
        void EnumerateContents(EnumValuesCallback* pCallback);
    public:
    //Params
		void Clone(Params** ppParams) const;
		void Duplicate(Params** ppParams);
	    int Compare(const Params* pParams) const;
        void AddValue(const wchar_t* name, Value * value);
        //void AddValue(const std::wstring& name, Value * value);
        void ReplaceValue(const wchar_t* name, Value * value);
        //void ReplaceValue(const std::wstring& name, Value * value);
        bool DeleteValue(const wchar_t* name, int bThrow);
        //bool DeleteValue(const std::wstring& name, int bThrow);
        void SetValue(const wchar_t* name, Value * value);
        //void SetValue(const std::wstring& name, Value * value);
		void GetValue(const wchar_t* name, Value** ppValue);
        //void GetValue(const std::wstring& name, Value** ppValue);
		bool GetValueNoThrow(const wchar_t* name, Value** ppValue);
        //bool GetValueNoThrow(const std::wstring& name, Value** ppValue);
        void GetNames(names_t &names) const;
        //void GetNames(ParamsNames &names) const;
        void Clear();
        bool DoesExist(const wchar_t* name) const;
        //bool DoesExist(const std::wstring& name) const;
		void CopyFrom(const Params* pSrc);
		void MoveFrom(Params* pSrc);
        size_t GetSize() const;
        void MakeReadOnly();
        void Destroy()
        {
            entry_clear();
        }
	protected:        
#ifdef KLPAR_USE_HASHMAP
        typedef std::hash_map< 
                    string_t, 
                    KLSTD::KLAdapt< ValuePtr > , 
                    HashFun_string_t, 
                    std::equal_to<KLPAR::string_t>, 
                    KLPAR::ParamsAllocPair > contents_t;
#else
		typedef std::map< 
                    string_t, 
                    KLSTD::KLAdapt< ValuePtr >, 
                    std::less<KLPAR::string_t>,
                    KLPAR::ParamsAllocPair> contents_t;
#endif
        mutable KLSTD::CReentProtectRW m_rp;
		contents_t          m_data;
        bool                m_bIsReadOnly;
    public:
        void MoveOrCopy(Params* psrc, bool bMove);
		void entry_add		(const string_t& name, Value * value, bool bReplace);
		bool entry_delete	(const string_t& name, bool bThrow);
		void entry_setval	(contents_t::iterator it, const string_t& name, Value * value);
		void entry_setval	(const string_t& name, Value * value);
		bool entry_getval	(const string_t& name, Value** ppValue, bool bThrow);
		const Value* entry_getval(const string_t& name, bool bThrow) const;
		void entry_getnames	(names_t &names) const;
		void entry_clear();
	};
};

#endif //__PARAMSIMPL_H__
