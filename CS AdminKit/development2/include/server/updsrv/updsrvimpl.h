/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	updsrvimpl.h
 * \author	Mikhail Karmazine
 * \date	05.12.2003 15:05:50
 * \brief	Класс для манипулирования автоматическим обновлениями в системе
 * 
 */

#ifndef __KLUPD_AUTOUPDATE_H__
#define __KLUPD_AUTOUPDATE_H__

#include <srvp/updsrv/updsrv.h>
#include <server/tsk/grouptaskcontrolimpl.h>
#include <kca/prss/settingsstorage.h>
#include <transport/tr/errors.h>
#include <set>
#include <server/srvinst/srvinstdata.h>

#define KLUPD_KASBASES_COMP L"KASBASES"

#define KLSRV_TRY_NO_ERROR_IF_NOT_EXISTS                    \
    KLERR_TRY

#define KLSRV_CATCH_NO_ERROR_IF_NOT_EXISTS                  \
    KLERR_CATCH(pError)                                     \
        if( ( pError->GetId() != KLPRSS::NOT_EXIST ) &&     \
            ( pError->GetId() != KLPAR::NOT_EXIST ) )       \
        {                                                   \
            KLERR_RETHROW();                                \
        }

#define KLSRV_ENDTRY_NO_ERROR_IF_NOT_EXISTS                 \
    KLERR_ENDTRY;

#define KLSRV_END_NO_ERROR_IF_NOT_EXISTS                    \
    KLSRV_CATCH_NO_ERROR_IF_NOT_EXISTS                      \
    KLSRV_ENDTRY_NO_ERROR_IF_NOT_EXISTS

namespace KLUPDSRV
{
	#define KLUPD_AUTO_DOWNLOAD_UPDATES						L"KLUPD_AUTO_DOWNLOAD_UPDATES" // BOOL
	#define KLUPD_RETRANSLATE_UPDATES						L"KLUPD_RETRANSLATE_UPDATES" // BOOL

    enum BundleBaseType { btPatch, btBase, btBaseItem, btBlst, btUnknown };
	class BundleType {
	public:
		BundleBaseType m_Type;
		bool m_bUpdateObligation;
		std::vector<std::wstring> m_vecComponentIds;
		std::vector<std::wstring> m_vecAppIds;
		BundleType() : m_Type(btUnknown), m_bUpdateObligation(false) {}
		operator BundleBaseType() const {return m_Type;}
		bool IsComponent(const std::wstring& wstrComponent) const;
		bool IsApp(const std::wstring& wstrApp) const;
		bool IsAVDB() const;
		bool IsAnitspamDB() const;
	};
    
    typedef std::set<std::wstring> BundleIdSet;

    typedef std::map<std::wstring, std::wstring> ProductToBundleIdMap;

    typedef std::vector<KLSTD::KLAdapt< KLSTD::CAutoPtr< KLPAR::Params > > > ParamsBundleVector;

	class BundleInfo: public KLSTD::KLBaseImpl<KLSTD::KLBase>{
	public:
		BundleInfo(): m_llSize(0), m_tDownloadDate(-1), m_tDate(-1){}
		virtual ~BundleInfo(){}

		BundleType m_Type;

        AVP_longlong	m_llSize;
        time_t			m_tDownloadDate;
        time_t			m_tDate;
		
		KLSTD::CAutoPtr<KLPAR::Params>	m_pBundle;
	};

	typedef std::map< std::wstring, KLSTD::KLAdapt< KLSTD::CAutoPtr< BundleInfo > > > CompIdAppId2BundleInfoMap;

    class UpdateSrvImpl
    {
    public:
        void Initialize(
            const KLPRCI::ComponentId & cidServer,
			KLSRV::SrvData* pSrvData,
            KLSTD::CAutoPtr<KLSRV::GroupTaskControlImpl> pGroupTaskControlImpl );

        void Deinitialize();

        void SetBundleOptions(KLDB::DbConnectionPtr pConnection, KLPAR::Params* pOptions );

        void SetGeneralOptions(KLDB::DbConnectionPtr pConnection, KLPAR::Params* pOptions );

        void GetGeneralOptions(KLDB::DbConnectionPtr pConnection, KLPAR::Params** ppGeneralOptions );

		void GetAvailableUpdatesInfo(
			KLDB::DbConnectionPtr pConnection,
			const wchar_t*      szwLocalization,
			KLPAR::Params** ppAvailableUpdateComps,
			KLPAR::Params** ppAvailableUpdateApps);

		void GetRequiedUpdatesInfo(
			KLDB::DbConnectionPtr pConnection,
			KLPAR::Params** ppInstalledComps,
			KLPAR::Params** ppInstalledApps);
		
        void GetAllRetranslatedBundles(
			KLDB::DbConnectionPtr pConnection,
            ParamsBundleVector & vectParamsBundles,
            std::wstring & wstrRetranslationDir );

		void AddBundleDescr(KLPAR::ParamsPtr parBundle);

        static bool MatchParamsAgainstFilter(
            KLPAR::Params* pParams,
            KLPAR::Params* pParFilter );

        static void GetBundleIdsFromParams(
            KLPAR::Params * pParams,
            BundleIdSet & bis,
            KLPAR::Params* pFilter );

        static KLSTD::CAutoPtr<KLPAR::Params> GetUpdaterSection(
            KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSettingsStorage );

        static std::wstring FormBundleIdFromRawUpdaterFileItem(
            KLSTD::CAutoPtr<KLPAR::Params> pItem );

        BundleType GetBundleType( KLPAR::Params * pBundleSource );

		bool UpdateRetranslationList(
			const KLSRV::UpdProdVerMap& vecProdVersInSS,
			const KLSRV::UpdCompVerMap& vecCompVersInSS,
			KLSTD::CAutoPtr<KLPAR::Params> parExistingTaskParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parTaskParams);
		
		bool GetProdVersInRegistry(
			KLSRV::UpdProdVerMap& vecProdVersToRetr, 
			KLSRV::UpdProdVerMap& vecProdVersToSkip, 
			KLSRV::UpdProdVerMap& vecCompVersToRetr,
			KLSRV::UpdProdVerMap& vecCompVersToSkip);

		bool GetProdVersInRegistry(
			KLSRV::UpdProdVerMap& vecProdVersToRetr, 
			KLSRV::UpdProdVerMap& vecProdVersToSkip, 
			const wchar_t* szSection);

		bool AddProdVerList(
			const KLSRV::UpdProdVerMap& vecProdVers,
			KLSTD::CAutoPtr<KLPAR::Params>& parRetranslationAppsSection,
			bool bCheckProducts);

		bool RemoveProdVerList(
			const KLSRV::UpdProdVerMap& vecProdVers,
			KLSTD::CAutoPtr<KLPAR::Params>& parRetranslationAppsSection);

		bool NeedRetranslateByDefault(const std::wstring& wstrProduct);

		bool UpdateRetranslationApps(
			const KLSRV::UpdProdVerMap& vecProdVersInSS,
			const KLSRV::UpdProdVerMap& vecProdVersToRetrInRegistry, 
			const KLSRV::UpdProdVerMap& vecProdVersToSkipInRegistry,
			KLSTD::CAutoPtr<KLPAR::Params>& parRetranslationAppsSection);

		bool UpdateRetranslationComps(
			const KLSRV::UpdCompVerMap& vecCompVersInSS,
			const KLSRV::UpdProdVerMap& vecProdVersToRetrInRegistry, 
			const KLSRV::UpdProdVerMap& vecProdVersToSkipInRegistry,
			KLSTD::CAutoPtr<KLPAR::Params>& parRetranslationAppsSection);

		bool UpdateRetranslationPath(
			KLSTD :: CAutoPtr<KLSTD::CriticalSection> pCSTaskParams,
			KLPAR::ParamsPtr parTaskParams,
			const std::wstring& wstrLocalPathToRetrDir,
			const std::wstring& wstrNetworkPathToRetrDir);
		
		bool UpgradeRetranslationTask(
			KLDB::DbConnectionPtr pConnection,
			KLSTD :: CAutoPtr<KLSTD::CriticalSection> pCSTaskParams,
			KLPAR::ParamsPtr parTaskParams,
			const std::wstring& wstrReserveDir,
			const std::wstring& wstrLocalPathToRetrDir,
			const std::wstring& wstrNetworkPathToRetrDir);
		
		void OnRetranslationSucceeded();

    private:
        KLPRCI::ComponentId m_cidServer;
        KLSRV::SrvData* m_pSrvData;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSMain;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSSetGeneralOptions;

        bool m_bCheckThreadMustStop;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSCheckThread;
        KLSTD::CPointer<KLSTD::Thread> m_pCheckThread;

        KLSTD::CAutoPtr<KLSRV::GroupTaskControlImpl> m_pGroupTaskControlImpl;

        bool m_bAllRetranslatedBundlesRead;
        std::vector< KLSTD::KLAdapt< KLSTD::CAutoPtr< KLPAR::Params > > > m_vectAllRetranslatedBundles;

        bool m_bGeneralOptionsRead;
        bool m_bAutoInstallAVDB;
        bool m_bAutoInstallPatches;
        bool m_bAutoInstallUU;

        typedef std::set< KLPRSS::product_version_t > ProductVersionSet;
        bool bKnownProductsRead;
        ProductVersionSet m_knownProducts;

		KLPAR::ParamsPtr m_pAvailableUpdateComps;
		KLPAR::ParamsPtr m_pAvailableUpdateApps;

		std::vector<std::wstring> m_vecHiddenApps;

        void EnsureKnownProductList(KLDB::DbConnectionPtr pConnection);

        void ClearCacheFlags();

        void FormRetrItem(
            ParamsBundleVector & vectParamsBundles,
            const std::wstring & wstrRetranslationDir,
            KLSTD::CAutoPtr< KLPAR::Params > pRetrItem,
            CompIdAppId2BundleInfoMap& mpCompIdAppId2BundleInfoMap);

		void GetAvailableUpdatesInfo(
			const std::wstring & wstrLocDataFile, 
			const std::wstring & wstrLocDataDiffFile,
			KLPAR::Params** ppAvailableUpdateComps,
			KLPAR::Params** ppAvailableUpdateApps);

		bool GetBundleDisplayName(KLSTD::CAutoPtr<BundleInfo> pBundleInfo, std::wstring& wstrDN, std::wstring& wstrDV);

		bool IsHiddenBundle(KLSTD::CAutoPtr<BundleInfo> pBundleInfo);

		bool IsHiddenBundle(const BundleType& type);
    };
}

bool KLUPDSRV_GetResultSettingsStorageLocation(std::wstring& wstrLocation);
std::wstring KLUPDSRV_GetRetranslationDir();

#endif // __KLUPD_AUTOUPDATE_H__

