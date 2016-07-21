#ifndef __SRV_FIND_HELPERS_H__
#define __SRV_FIND_HELPERS_H__

    #define AKDEF_FUN(_name)    \
        (m_pSrvData->m_pLiterals->MakeIdName((_name), KLDB::kldbidt_function))

    #define AKDEF_COL(_name)    \
        (m_pSrvData->m_pLiterals->MakeIdName((_name), KLDB::kldbidt_column))

    #define AKDEF_TYPE(_type)    \
        (m_pSrvData->m_pLiterals->MakeIdName((_type), KLDB::kldbidt_type_for_temp_table))

namespace KLSRVFIND
{
    typedef std::map<int, std::wstring> map_int2strt_t;
    typedef map_int2strt_t::iterator    it_map_int2strt_t;
    typedef map_int2strt_t::value_type  val_map_int2strt_t;

    void PreExecSubgroups( KLDB::DbConnectionPtr pCon, map_int2strt_t& mapData);

    void PostExecSubgroups(
                    KLDB::DbConnectionPtr   pCon, 
                    map_int2strt_t&         mapData);

    template<class T>
    class FRCallbacksImpl
        :   public KLFLT::FRInclusionCallback
        ,   public KLFLT::FRCallback
    {
    public:
        FRCallbacksImpl(T& pc)
            :   m_pc(pc)
        {
            ;
        };
        virtual ~FRCallbacksImpl()
        {
            ;
        };

		virtual unsigned long AddRef()
        {
            return 0;
        };

		virtual unsigned long Release()
        {
            return 0;
        };

        KLSTD_INTERAFCE_MAP_BEGIN(KLFLT::FRInclusionCallback)
            KLSTD_INTERAFCE_MAP_ENTRY(KLFLT::FRCallback)
        KLSTD_INTERAFCE_MAP_END()

        virtual void OnValue(
                KLFLT::str2str_map_t::const_iterator   it)
        {
            m_pc.Callback(&m_pc, it);
        };

        virtual bool IfInclusion(
                KLDB::DbLiterals*   pLiterals,
                const wchar_t*  szwAttribute,
                const wchar_t*  szwValue,
                std::wstring&   wstrInclusion)
        {
            return m_pc.IfInclusion(
                                pLiterals, 
                                szwAttribute, 
                                szwValue, 
                                wstrInclusion);
        };
    protected:
        T& m_pc;
    };

    
};

#endif //__SRV_FIND_HELPERS_H__
