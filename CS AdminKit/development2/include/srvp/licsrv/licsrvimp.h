#ifndef __KL_LICSRVIMP_H__
#define __KL_LICSRVIMP_H__

#include <std/base/klbaseqi_imp.h>
#include "./licsrv.h"
#include "./licsrvstore.h"
#include "../tsk/grouptaskclient.h"
#include "../csp/klcspi.h"


namespace KLLICSRV
{
    class SrvLicencesImpl
                            :   public KLLICSRV::SrvLicences
                            ,   public KLCSP::IteratorHost
		                    ,   public KLCSP::ChunkAccessorImp
                            ,   public KLLICSRV::SrvLicStore
    {
    public:
	    void Initialize(
                    KLSTD::KLBaseQI*    pOwner,
                    KLPRCP::CProxyBase* pOwnersProxy)
        {
            KLSTD_ASSERT(pOwnersProxy != NULL && pOwner != NULL);
            m_pOwnersProxy=pOwnersProxy;
            m_pOwner=pOwner;
        };
        KLSTD_INTERAFCE_MAP_REDIRECT(m_pOwner);
    protected:
        KLSTD::KLBaseQI*    m_pOwner;
        KLPRCP::CProxyBase* m_pOwnersProxy;
    public:
        void MakeKeyInstallTaskParams(
                            const std::wstring&             wstrKeyFile,
                            const std::wstring&             wstrProductName,
                            const std::wstring&             wstrVersion,
                            bool                            bIsCurrentKey,
                            KLPAR::Params**                 ppTaskParams);

        void MakeKeyInstallExtraTaskParams(
                            KLEVP::notification_descr_t*    pNotifications,
                            int                             nNotifications,
                            host_id_t*                      pHosts,
                            int                             nHosts,
                            KLPAR::Params**                 ppExtraTaskParams);


        KLSCH::Task* MakeSchTask();

        void InstallKeyForGroup(
                            const std::wstring&             wstrKeyFile,
                            const std::wstring&             wstrProductName,
                            const std::wstring&             wstrVersion,
                            bool                            bIsCurrentKey,
                            long                            idGroup,
                            KLEVP::notification_descr_t*    pNotifications,
                            int                             nNotifications);
        
        void InstallKeyForHosts(
                            const std::wstring&             wstrKeyFile,                            
                            const std::wstring&             wstrProductName,
                            const std::wstring&             wstrVersion,
                            bool                            bIsCurrentKey,
                            host_id_t*                      pHosts,
                            int                             nHosts,
                            KLEVP::notification_descr_t*    pNotifications,
                            long                            nNotifications);

        bool OnGetNextItems(
                        const wchar_t*                  szwIterator,
                        long&                           lCount,
                        KLPAR::Params**                 ppData);

        KLSTD_NOTHROW void OnReleaseIterator(
                        const wchar_t* szwIterator) throw();

        void GetKeysInfo(
                    const wchar_t**             ppFields,
                    int                         nFields,
                    const std::wstring&         wstrFilter,
                    long                        lTimeout,
                    KLCSP::ForwardIterator**    ppIterator);

        long GetKeyInfo(
						const std::vector<std::wstring>& vect_fields,
						const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
						const std::wstring& wstrFilter,
						long                lTimeout,
						KLCSP::ChunkAccessor**   ppAccessor);

		void DeleteKeyInfo(const std::wstring& wstrSerial);
		
		long GetRecordsCount(const wchar_t* szwChunkAccessor);

		long GetChunk(const wchar_t* szwChunkAccessor, long lStart, long lCount, KLPAR::Params** ppData);

		void ReleaseChunkAccessor(const wchar_t* szwChunkAccessor);


//KLLICSRV::SrvLicStore
    protected:
        virtual std::wstring InstallKey(KLPAR::Params* pData);
        
        virtual void UninstallKey(const wchar_t* szwSerial);

        virtual void ExportKey(
                    const wchar_t*    szwSerial, 
                    KLPAR::Params**   ppData);

        virtual void GetKeyData(
                            const wchar_t*  szwSerial, 
                            KLPAR::Params**   ppLimits);
        
        virtual bool TryToAdjustKey(
                            KLPAR::Params*  pLimits, 
                            long            lNewCountOfLicenses, 
                            long&           lNewKeyPeriod);

        virtual void AdjustKey(
                            const wchar_t*  szwSerial, 
                            KLPAR::Params*  pData);

        virtual long EnumKeys(
                const std::vector<std::wstring>&            vect_fields,
                const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
                KLPAR::Params*                              pOptions,
                long                                        lTimeout,
                KLCSP::ChunkAccessor**                      ppAccessor);
    };
};

#endif //__KL_LICSRVIMP_H__
