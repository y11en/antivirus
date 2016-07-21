/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	packagessrv.h
 * \author	Andrew Lashchenkov
 * \date	27/07/2005
 * \brief	Серверная версия интерфейса Packages
 * 
 */

#ifndef __PACKAGESRV_H__B6BC22D2_A327_4971_9309_FDA73EB48F36_
#define __PACKAGESRV_H__B6BC22D2_A327_4971_9309_FDA73EB48F36_

#include <std/base/klbase.h>
#include <server/srv_common.h>
#include <server/db/dbconnection.h>
#include <srvp/pkg/packages.h>

namespace KLPKG
{
	class PackagesSrv: public KLSTD::KLBase{
	public:
		virtual ~PackagesSrv(){}

		virtual void GetPackages( 
			KLDB::DbConnectionPtr pDbConn,
			std::vector<package_info_t>& packages) = 0;

		virtual void RenamePackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrNewPackageName) = 0;

		virtual void GetIntranetFolderForNewPackage(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder) = 0;

		virtual void RecordNewPackage(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			package_info_t& packageInfoNew,
			std::wstring& wstrPackageGUID) = 0;

		virtual bool RemovePackage( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			bool bThrowIfTasksDepend,
			std::vector<Packages2::task_info_t>& vecDependTasks) = 0;
		
		virtual std::wstring GetIntranetFolderForPackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId) = 0;

		virtual std::wstring GetLoginScript( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrTaskId,
			const std::wstring& wstrPackageLocalPath,
			const std::wstring& wstrPackageNetworkPath) = 0;

		virtual void SetDefaultRebootOptions( 
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId) = 0;
		
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
			const wchar_t* wszTaskId = NULL) = 0;

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
			const wchar_t* wszTaskId = NULL) = 0;

        virtual std::wstring GetLocalPathToPackage(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId) = 0;
		
        virtual void GetPackageFullPathData( 
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId,
            std::wstring & wstrProductName,
            std::wstring & wstrProductVersion,
            std::wstring & wstrProductDisplName,
            std::wstring & wstrProductDisplVersion,
            std::wstring & wstrPackageName,
            std::wstring & wstrPackagePath) = 0;

		virtual std::wstring FindSettingsStorage( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPath,
			const std::wstring& wstrProductName, 
			const std::wstring& wstrProductVersion) = 0;

		virtual void CheckFilePresence(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId, 
			const std::wstring& wstrFileName,
			bool bUpdateExisting,
			const std::wstring& wstrVirtualPackagePath = L"") = 0;

		virtual void RecordNewPackage2(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrPackageGUID,
			package_info_t& packageInfoNew) = 0;

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
			package_info_t& packageInfoNew) = 0;

		virtual std::wstring CreateNewPackageGUID(
			long nPackageId) = 0;

		virtual std::wstring GetPackageGUID(
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId) = 0;

		virtual void ReadPkgCfgFile(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk** ppChunk) = 0;

		virtual void WritePkgCfgFile(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk* pChunk) = 0;

		virtual long GetPackageIdByGUID(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPackageGUID) = 0;

		virtual void GetLocalFolderForNewPackage(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder) = 0;

		virtual void GetLicenseKey(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk** ppMemoryChunk) = 0;

		virtual void SetLicenseKey(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk* pMemoryChunk,
			bool bRemoveExisting) = 0;

		virtual void ReadKpdFile(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			KLSTD::MemoryChunk** ppChunk) = 0;

		virtual void GetKpdProfileString(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrDefault,
			std::wstring& wstrValue) = 0;

		virtual void WriteKpdProfileString(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrValue) = 0;

		virtual void OnPackageChange(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId) = 0;

		virtual void RecordNewPackageAsync(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrPackageGUID,
			const std::wstring& wstrRequestId) = 0;
		
		virtual void GetPackageInfo(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			package_info_t& packageInfo) = 0;
	
		virtual KLSTD::CAutoPtr<KLSTD::ReadWriteLock> 
			GetLockForPackagePath(const std::wstring& wstrPackagePath) = 0;		

		virtual bool GetIncompatibleAppsInfo(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			KLPAR::Params** ppData,
			bool& bCanRemoveByInstaller,
			bool& bRemoveByInstaller) = 0;
		
		virtual bool SetRemoveIncompatibleApps(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId,
			bool bRemoveIncompatibleApps) = 0;

		/*
		virtual std::wstring GetPackageSSPath(
			const std::wstring& wstrPackageGUID) = 0;

		virtual void TransferPackageToSlaveServers(
			KLDB::DbConnectionPtr pDbConn,
            long nPackageId) = 0;
		*/
	};
}

std::wstring KLPKG_GetLocalRootForPackagesInternal();

std::wstring KLSRV_GetLocalSharePath();

std::wstring KLSRV_GetNetworkSharePath();

#endif  // __PACKAGESRV_H__B6BC22D2_A327_4971_9309_FDA73EB48F36_

