#ifndef __KLAGENT_H__
#define __KLAGENT_H__

#include <kca/prts/tasksstorage.h>
#include <kca/pres/eventsstorage.h>

DECLARE_MODULE_INIT_DEINIT2(KLCSA_DECL, KLAGINST);

#ifdef _WIN32
        #define MAKE_AGENT_MUTEX_NAME(_prod, _ver) \
            ((KLWTS_CheckTerminalServicesSupported() \
                ?   std::wstring(KLSTD_T2CW(KLWTS_GLOBAL))\
                :   std::wstring(L"")) +\
            std::wstring(L"KLAGINST-") +\
            std::wstring(_prod) + L"-" +\
            std::wstring(_ver) + KLSTD_StGetPrefixW())
#endif

namespace KLAGINST
{

    inline std::wstring GetLocalTsName(
                        const wchar_t* szwProduct,
                        const wchar_t* szwVersion)
    {
        return  std::wstring(szwProduct) +
                L"/" + 
                szwVersion + 
                L"/" + 
                KLPRTS::TASK_STORAGE_NAME_LOCAL;
    };

    inline std::wstring GetLocalTsName(
                        const std::wstring& wstrProduct,
                        const std::wstring& wstrVersion)
    {
        return GetLocalTsName(
                    (const wchar_t*)wstrProduct.c_str(), 
                    (const wchar_t*)wstrVersion.c_str());
    };

    inline std::wstring GetLocalEsName(
                        const wchar_t* szwProduct,
                        const wchar_t* szwVersion)
    {
        return  std::wstring(szwProduct) +
                L"/" + 
                szwVersion + 
                L"/" + 
                KLPRES::EVENT_STORAGE_NAME_LOCAL;
    };

    inline std::wstring GetLocalEsName(
                        const std::wstring& wstrProduct,
                        const std::wstring& wstrVersion)
    {
        return GetLocalEsName(
                    (const wchar_t*)wstrProduct.c_str(), 
                    (const wchar_t*)wstrVersion.c_str());
    };

    enum
    {
        /*! Ts worker will start after c_szwEv_OnAppStart if 
            KLAG_WAIT_SCHED_FOR_START_EVENT is set, otherwise it will 
            start immediately*/
        AGTLD_LOAD_AUTO = 0,
        
        //! Ts worker will start immediately regardless of KLAG_WAIT_SCHED_FOR_START_EVENT
        AGTLD_LOAD_WITH_TS = 1,
        
        //! Ts worker will start after c_szwEv_OnAppStart regardless of KLAG_WAIT_SCHED_FOR_START_EVENT
        AGTLD_LOAD_NO_TS = 2,
        
        //! Tasks will be without impersonation
        AGTLD_LOAD_NO_IMPERSONATION = 4,

        //! Scheduler won't start
        AGTLD_LOAD_NO_SCHED = 8
    };
};


/*!
  \brief	Функция инстанциации агента.

  \param	id
  \param	void*&      OUT Agent handle
  \param	dwOptions   IN AGTLD_LOAD_*
*/
KLCSA_DECL void KLAGINST_LoadAgent(
                    KLPRCI::ComponentId&    id,
                    void*&                  handle,
                    AVP_dword               dwOptions = KLAGINST::AGTLD_LOAD_NO_TS);

/*!
  \brief	Функция деинстанциации агента.
*/
KLCSA_DECL KLSTD_NOTHROW void KLAGINST_UnloadAgent(void* handle) throw();

KLCSA_DECL bool KLAGINST_ResumeTsWorker(void* handle, bool bEnabledScheduler = true);

KLCSA_DECL bool KLAGINST_SuspendTsWorker(void* handle);

/*!
  \brief	Возвращает количество посланных агенту команд INSTANCE_STOP
*/
KLCSA_DECL KLSTD_NOTHROW long KLAGINST_StopRequested(void* handle) throw();


#if defined(_WIN32) && defined(_WINBASE_)
    /*!
      \brief	Создаёт процесс для старта в нём компонента. В случае
                ошибки выбрасывается исключение.

      \param    szwCommandLine [in] - командная строка
      \param    szwDirectory [in] - директория, которая станет текущей для
                                    созданного процесса. М.б. NULL
      \param    idComponent [in] - идентификатор компонента (могут быть
                                    заполнены не все поля)
      \param    szwAsyncId [in] - AsyncId компонента. М.б. NULL
      \param    pi [out] - см. CreateProcess
    */
    KLCSA_DECL void KLAGINST_CreateProcessForComponent(
					    wchar_t*				    szwCommandLine,
					    const wchar_t*			    szwDirectory,
                        const KLPRCI::ComponentId&  idComponent,
                        const wchar_t*			    szwAsyncId,
					    PROCESS_INFORMATION&	    pi);
#endif

#endif //__KLAGENT_H__

// Local Variables:
// mode: C++
// End:
