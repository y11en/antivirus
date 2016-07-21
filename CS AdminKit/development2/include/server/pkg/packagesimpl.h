/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	packagesimpl.h
 * \author	Mikhail Karmazine
 * \date	16:11 18.11.2002
 * \brief	Серверная имплементация интерфейса Packages
 * 
 */

#ifndef __PACKAGEIMPL_H__B6BC22D2_A327_4971_9309_FDA73EB48F36_
#define __PACKAGEIMPL_H__B6BC22D2_A327_4971_9309_FDA73EB48F36_

#include <server/srv_common.h>
#include <server/srvinst/srvinstdata.h>
#include <server/pkg/packagessrv.h>
#include <server/tsk/grouptaskcontrolsrv.h>

namespace KLSRV
{
	class PackagesImpl : public KLSTD::KLBaseImpl<KLPKG::PackagesSrv>{
	public:
		PackagesImpl();
			
		virtual ~PackagesImpl();

		virtual void GetPackages( 
			KLDB::DbConnectionPtr pDbConn,
			std::vector<KLPKG::package_info_t>& packages);

		virtual void RenamePackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrNewPackageName);

		virtual void GetIntranetFolderForNewPackage(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder);

		virtual void RecordNewPackage(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			KLPKG::package_info_t& packageInfoNew,
			std::wstring& wstrPackageGUID);

		virtual bool RemovePackage( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			bool bThrowIfTasksDepend,
			std::vector<KLPKG::Packages2::task_info_t>& vecDependTasks);
		
		virtual std::wstring GetIntranetFolderForPackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId);

		virtual std::wstring GetLoginScript( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrTaskId,
			const std::wstring& wstrPackageLocalPath,
			const std::wstring& wstrPackageNetworkPath);

		virtual void SetDefaultRebootOptions( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId);

		virtual void SetDefaultRebootOptions(const std::wstring& wstrIniFilePath);

		virtual void SetRebootOptions( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			bool bRebootImmediately,
			bool bAskForReboot,
			int nAskForRebootPeriodInMin,
			int nForceRebootTimeInMin,
			const std::wstring& wstrAskRebootMsgText,
			bool bForceAppsClosed,
			KLPAR::ParamsPtr parExtraParams,
			const wchar_t* wszTaskId = NULL);

		virtual void GetRebootOptions( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			bool& bRebootImmediately,
			bool& bAskForReboot,
			int& nAskForRebootPeriodInMin,
			int& nForceRebootTimeInMin,
			std::wstring& wstrAskRebootMsgText,
			bool& bForceAppsClosed,
			KLPAR::ParamsPtr& parExtraParams,
			const wchar_t* wszTaskId = NULL);

        virtual std::wstring GetLocalPathToPackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId);
		
        virtual void GetPackageFullPathData( 
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId,
            std::wstring & wstrProductName,
            std::wstring & wstrProductVersion,
            std::wstring & wstrProductDisplName,
            std::wstring & wstrProductDisplVersion,
            std::wstring & wstrPackageName,
            std::wstring & wstrPackagePath);

		virtual std::wstring FindSettingsStorage( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPath,
			const std::wstring& /*wstrProductName*/, 
			const std::wstring& /*wstrProductVersion*/);

		virtual void CheckFilePresence(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId, 
			const std::wstring& wstrFileName,
			bool bUpdateExisting,
			const std::wstring& wstrVirtualPackagePath = L"");

		virtual void RecordNewPackage2(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrPackageGUID,
			KLPKG::package_info_t& packageInfoNew);

		virtual bool RecordNewPackageWithGUID(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPackageGUID,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			bool bReplace,
			KLPKG::package_info_t& packageInfoNew);

		virtual std::wstring CreateNewPackageGUID(
			long nPackageId);

		virtual std::wstring GetPackageGUID(
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId);

		virtual void ReadPkgCfgFile(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk** ppChunk);

		virtual void WritePkgCfgFile(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk* pChunk);
		
		virtual long GetPackageIdByGUID(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPackageGUID);

		virtual void GetLocalFolderForNewPackage(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder);

		virtual void GetLicenseKey(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk** ppMemoryChunk);

		virtual void SetLicenseKey(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk* pMemoryChunk,
			bool bRemoveExisting);

		virtual void ReadKpdFile(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			KLSTD::MemoryChunk** ppChunk);

		virtual void GetKpdProfileString(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrDefault,
			std::wstring& wstrValue);

		virtual void WriteKpdProfileString(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrValue);
		
		virtual void OnPackageChange(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId);

		virtual void SetRebootOptions( 
			const std::wstring& wstrIniFilePath,
			bool bRebootImmediately,
			bool bAskForReboot,
			int nAskForRebootPeriodInMin,
			int nForceRebootTimeInMin,
			const std::wstring& wstrAskRebootMsgText,
			bool bForceAppsClosed,
			KLPAR::ParamsPtr parExtraParams);

		virtual void GetRebootOptions( 
			const std::wstring& wstrIniFilePath,
			bool& bRebootImmediately,
			bool& bAskForReboot,
			int& nAskForRebootPeriodInMin,
			int& nForceRebootTimeInMin,
			std::wstring& wstrAskRebootMsgText,
			bool& bForceAppsClosed,
			KLPAR::ParamsPtr& parExtraParams);

		virtual void RecordNewPackageAsync(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrPackageGUID,
			const std::wstring& wstrRequestId);
		
		virtual void GetPackageInfo(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			KLPKG::package_info_t& packageInfo);

		virtual bool GetIncompatibleAppsInfo(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			KLPAR::Params** ppData,
			bool& bCanRemoveByInstaller,
			bool& bRemoveByInstaller);
		
		virtual bool SetRemoveIncompatibleApps(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			bool bRemoveIncompatibleApps);
		
		virtual KLSTD::CAutoPtr<KLSTD::ReadWriteLock> GetLockForPackagePath(const std::wstring& wstrPackagePath);

		virtual void DeleteRebootOptions(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wszTaskId);
		
		/*
		virtual std::wstring GetPackageSSPath(
			const std::wstring& wstrPackageGUID);

		virtual void TransferPackageToSlaveServers(
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId);
		*/

		void Initialize(KLSRV::SrvData* pSrvData)
		{
            m_pSrvData = pSrvData;
		}

		void SetGroupTaskControlSrv(KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> pGroupTaskControlSrv)
		{
			m_pGroupTaskControlSrv = pGroupTaskControlSrv;
		}
		
	private:
        KLSRV::SrvData*		m_pSrvData;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
		KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> m_pGroupTaskControlSrv;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSLocks;
		std::map<std::wstring, KLSTD::KLAdapt<KLSTD::CAutoPtr<KLSTD::ReadWriteLock> > > m_mpLocks;

		struct RecordNewPkgData{
			std::wstring m_wstrNewPackageName;
			std::wstring m_wstrFileId;
			std::wstring m_wstrProductName;
			std::wstring m_wstrProductVersion;
			std::wstring m_wstrProductDisplName;
			std::wstring m_wstrProductDisplVersion;
			std::wstring m_wstrPackageGUID;
			std::wstring m_wstrRequestId;
			RecordNewPkgData(
				const std::wstring& wstrNewPackageName,
				const std::wstring& wstrFileId,
				const std::wstring& wstrProductName,
				const std::wstring& wstrProductVersion,
				const std::wstring& wstrProductDisplName,
				const std::wstring& wstrProductDisplVersion,
				const std::wstring& wstrPackageGUID,
				const std::wstring& wstrRequestId)
			:
				m_wstrNewPackageName(wstrNewPackageName),
				m_wstrFileId(wstrFileId),
				m_wstrProductName(wstrProductName),
				m_wstrProductVersion(wstrProductVersion),
				m_wstrProductDisplName(wstrProductDisplName),
				m_wstrProductDisplVersion(wstrProductDisplVersion),
				m_wstrPackageGUID(wstrPackageGUID),
				m_wstrRequestId(wstrRequestId)
			{
			}
		};

		std::wstring FindNewNameInDB(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion );

		std::wstring GetAddOnPathFromNetworkFolder(
			const std::wstring& wstrNetworkFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion );

        void GetPackageData( 
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId,
            std::wstring & wstrProductName,
            std::wstring & wstrProductVersion,
			std::wstring & wstrProductDisplName,
			std::wstring & wstrProductDisplVersion,
		    std::wstring & wstrPackageName,
            std::wstring & wstrPackagePath );

		std::wstring GetKlRbtAgtIniFilePath(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const wchar_t* wszTaskId = NULL);
		
		bool GetDependentTasks(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId, 
			std::vector<KLPKG::Packages2::task_info_t>& vecDependTasks);

		void RecordNewPackageInternal(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrPackageGUID,
			std::wstring& wstrAddOnPath,
			KLPKG::package_info_t& packageInfoNew);

		void GetFolderForNewPackage(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder);

        std::wstring GetLocalPathToOriginalPackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId);

		void SetDefaultBaseNetworkAgentSettings(
			const std::wstring& wstrSettingsStorageFullPath,
			const std::wstring& wstrProductDisplName, 
			const std::wstring& wstrProductDisplVersion,
			bool bForceOptionalChanges);

		void RecordNewPackageInternalAsync(
			RecordNewPkgData data);

		void GetPkgInfoFromRst(KLDB::DbRecordsetPtr pRecordset, KLPKG::package_info_t& package);
		
		void GetPkgUpdatesInfo(
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrPackageLocalPath,
			KLSRV::UpdProdVerMap& vecProdVers, 
			KLSRV::UpdCompVerMap& vecCompVers);

		void RegisterUpd( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const KLSRV::UpdProdVerMap& vecProdVers, 
			const KLSRV::UpdCompVerMap& vecCompVers);
	};
}

#endif  // __PACKAGEIMPL_H__B6BC22D2_A327_4971_9309_FDA73EB48F36_

