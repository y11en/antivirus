#ifndef __TASK_SETTINGS_BASE_H__
#define __TASK_SETTINGS_BASE_H__

#include <server/srvinst/klserver.h>

namespace KLSRV
{
    /*
        T must have methods 
            T::tsk_settings_t TaskSettings_GetSettings();
            T::tsk_settings_t TaskSettings_LoadSettings(
                                KLSRV::ServerSettings* pServerSettings);
            void TaskSettings_UseSettings(
                                T::tsk_settings_t& settings);
            void TaskSettings_Restart();
            T::tsk_settings_t;
    */
    template<class T>
    class TaskSettingsT
        :   public SrvSettingsChangeSink
    {
    public:
        TaskSettingsT()
            :   m_hSink(NULL)
        {
            ;
        };
        virtual ~TaskSettingsT()
        {
            ;
        };

        ServerSettingsPtr GetServerSettings()
        {
            T* pThis = static_cast<T*>(this);
            KLSRV_AUTOPTR(pServerSettings, KLSRV::ServerSettings, pThis->m_pSrvData->m_pServer);
            return pServerSettings;
        };
        void Init(
                    const wchar_t* szwSection = KLSRV_COMMON_SETTINGS,
                    const wchar_t* szwProduct = KLCS_PRODUCT_ADMSERVER, 
                    const wchar_t* szwVersion = KLCS_VERSION_ADMSERVER)
        {
            T* pThis = static_cast<T*>(this);
            KLSRV_AUTOPTR(pServerSettings, KLSRV::ServerSettings, pThis->m_pSrvData->m_pServer);
            if(pServerSettings)
                m_hSink = pServerSettings->Advise(
                                    this,
                                    szwSection,
                                    szwProduct,
                                    szwVersion);
        };
        void Deinit()
        {
            if(m_hSink)
            {
                T* pThis = static_cast<T*>(this);
                KLSRV_AUTOPTR(pServerSettings, KLSRV::ServerSettings, pThis->m_pSrvData->m_pServer);                
                if(pServerSettings)
                    pServerSettings->Unadvise(m_hSink);
                m_hSink = NULL;
            };
        };
    //SrvSettingsChangeSink
        void OnNotify(
                        const wchar_t* szwProduct,
                        const wchar_t* szwVersion,
                        const wchar_t* szwSection)
        {
            T* pThis = static_cast<T*>(this);
            KLSRV_AUTOPTR(pServerSettings, KLSRV::ServerSettings, pThis->m_pSrvData->m_pServer);
            const T::tsk_settings_t& data = 
                pThis->TaskSettings_LoadSettings(pServerSettings);
            if(!(pThis->TaskSettings_GetSettings() == data) )
            {
                pThis->TaskSettings_UseSettings(data);
            };
        };
    protected:
        KLSRV::HCHANGESINK  m_hSink;
    };
};

#endif //__TASK_SETTINGS_BASE_H__
