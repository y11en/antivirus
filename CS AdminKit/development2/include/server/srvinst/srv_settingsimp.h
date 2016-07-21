#ifndef __KLSRV_SETTINGSIMP_H__
#define __KLSRV_SETTINGSIMP_H__

#include <server/srvinst/klserver.h>
#include <kca/prci/productsettings.h>
#include <common/ak_cached_data.h>
#include <std/tp/tpcmdqueue.h>

#include <hash_map>
#include <list>

#include <server/srvinst/srv_settings.h>

namespace KLSRV
{
    class SectionsStoreImpl
        :   public KLSTD::KLBaseImpl<KLSRV::SectionsStore>
    {
    public:
        SectionsStoreImpl(KLSTD::CriticalSection* pCS);
        virtual ~SectionsStoreImpl();

        KLSTD_INTERAFCE_MAP_BEGIN(KLSRV::SectionsStore)
        KLSTD_INTERAFCE_MAP_END()

        void Create(KLPRCI::ComponentInstance* pInst);
        void Destroy();
        ;
        //! returns specified parameter. One shouldn't change returned data.
        KLSTD::CAutoPtr<KLPAR::Value> GetParameter(
                            const wchar_t* szwName,
                            const wchar_t* szwSection,
                            const wchar_t* szwProduct, 
                            const wchar_t* szwVersion);
        
        //! returns specified parameters. One shouldn't change returned data.
        KLPAR::ParamsPtr GetParameters(
                            const wchar_t** szwNames,
                            size_t          nNames,
                            const wchar_t*  szwSection,
                            const wchar_t*  szwProduct, 
                            const wchar_t*  szwVersion);
        
        KLSRV::HCHANGESINK Advise(
                            KLSRV::SrvSettingsChangeSink *pSink,
                            const wchar_t* szwSection,
                            const wchar_t* szwProduct, 
                            const wchar_t* szwVersion);

        void Unadvise(KLSRV::HCHANGESINK hSink);
    protected:
        void DestroyI();
        static void SettingsChangeCallback(
		                void*				context,
                        const std::wstring&	productName, 
                        const std::wstring&	version,
                        const std::wstring&	section, 
                        const std::wstring&	parameterName,
                        const KLPAR::Value*	oldValue,
                        const KLPAR::Value*	newValue );

        void OnSettingsChange(
                        const std::wstring&	productName, 
                        const std::wstring&	version,
                        const std::wstring&	section);

	    struct command_t
	    {
            std::wstring                    m_wstrProduct,
                                            m_wstrVersion,
                                            m_wstrSection;
	    };
        typedef std::list<command_t>    lst_cmt_t;
        typedef lst_cmt_t::iterator     it_lst_cmt_t;

        typedef std::list< KLSTD::KLAdapt<KLSRV::SrvSettingsChangeSinkPtr> > lst_sink_t;
        typedef lst_sink_t::iterator it_lst_sink_t;

        typedef KLCMDQUEUE::CmdQueue<   command_t, 
                                        command_t&, 
                                        SectionsStoreImpl > queue_t;        

        void OnProcessCommand(command_t& cmd);

        class CallString2Section
        {
        public:
            CallString2Section()
            {;};
            KLSTD::KLAdapt<KLPAR::ParamsPtr> operator() (
                        SectionsStoreImpl* pInst, 
                        const std::wstring& wstrName);
        };
        KLSTD::PairCache<   
            KLSTD::PairCachePropsHash<
                    KLSTD::KLAdapt<KLPAR::ParamsPtr>, 
                            SectionsStoreImpl*, 
                            std::wstring, 
                            CallString2Section> > m_oChacheString2Section;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;

        struct sink_data_t
        {
            sink_data_t()
            {;};
            sink_data_t(
                        KLSRV::SrvSettingsChangeSinkPtr pSink,
                        std::wstring                    wstrSection)
                :   m_pSink(pSink)
                ,   m_wstrSection(wstrSection)
            {;};
            KLSRV::SrvSettingsChangeSinkPtr m_pSink;
            std::wstring                    m_wstrSection;
        };
        typedef std::hash_map<
                        long, sink_data_t>
                                                            map_id2sink_t;
        typedef map_id2sink_t::iterator                     it_map_id2sink_t;
        typedef map_id2sink_t::value_type                   val_map_id2sink_t;

        typedef std::hash_multimap<
                        std::wstring, 
                        KLSRV::HCHANGESINK
                         >                                  map_str2id_t;
        typedef map_str2id_t::iterator                      it_map_str2id_t;
        typedef map_str2id_t::value_type                    val_map_str2id_t;

        map_id2sink_t                       m_oSinks;
        map_str2id_t                        m_oSections;
        volatile long                       m_lLastd;
        KLPRCI::HSUBSCRIBE                  m_idSubscription;
        KLPRCI::ComponentInstance*          m_pInst;
        KLSTD::CPointer<queue_t>            m_pQueue;
        KLSTD::CPointer<KLSTD::ObjectLock>  m_pExtLock;
    };
};

#endif //__KLSRV_SETTINGSIMP_H__
