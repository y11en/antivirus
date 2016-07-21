/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PKG/Packages.h
 * \author Михаил Кармазин
 * \date 16:22 19.11.2002
 * \brief Интерфейс для создания, модификации и удаления инсталляционных пакетов.
 *
 */

#ifndef _KLPKG_H__872AC7A0_D727_4032_946A_963AA70B1EEC__
#define _KLPKG_H__872AC7A0_D727_4032_946A_963AA70B1EEC__

#include <kca/prss/settingsstorage.h>
#include <srvp/pkg/pkgcfg.h>
#include <srvp/admsrv/admsrv.h>

// название секции настроек инсталлятора продукта в SS
// <Product name> <Ver> <KLPLG_SSSECTION_NAME_INSTALLER>
#define KLPLG_SSSECTION_NAME_INSTALLER		L"InstallerSettings"		

// Параметры инсталлятора
#define INSTALLER_PARAM_SERVER_NAME			L"ServerName"
#define INSTALLER_PARAM_SERVER_PORT			L"ServerPort"
#define INSTALLER_PARAM_SERVER_SSL_PORT		L"ServerSSLPort"
#define INSTALLER_PARAM_USE_SSL				L"UseSSLConnection"
#define INSTALLER_PARAM_NO_REBOOT			L"DelayReboot"
#define INSTALLER_PARAM_PRESUMED_GROUP_NAME	L"PresumedGroupName"
#define INSTALLER_PARAM_USE_PROXY			L"UseProxy"					// BOOL_T
#define INSTALLER_PARAM_PROXY_ADDRESS		L"ProxyServerAddress"		// STRING_T
#define INSTALLER_PARAM_PROXY_USER			L"ProxyServerUser"			// STRING_T
#define INSTALLER_PARAM_PROXY_PASSWORD		L"ProxyServerPassword"		// STRING_T
#define INSTALLER_PARAM_ALLOW_NETBIOS_NAME_SERVICE	KLNAG_ALLOW_NETBIOS_NAME_SERVICE	// STRING_T
#define INSTALLER_PARAM_OPEN_PORT_IN_WFW	KLNAG_SP_OPEN_PORT_IN_WFW	// STRING_T

#define INSTALLER_PARAM_SRV_CERTIFICATE		L"ServerCert"	// BINARY_T

#define PACKAGES_FOLDER_NAME				L"Packages"
#define PACKAGES_UNINSTALL_FOLDER_NAME		L"Uninstall"

namespace KLPKG {
	const wchar_t FAKE_PRODUCT_NAME_FOR_EXECUTABLE_PACKAGE[] = L"executable_package";
	const wchar_t FAKE_PRODUCT_VERSION_FOR_EXECUTABLE_PACKAGE[] = L"1.0.0.0";

    /*!
        \brief Событие c_RecordNewPackageDone публикуется при завершении асинхронного создания пакета, в т.ч. при ошибке
		    c_evpPkgAsyncId, STRING_T - Идентификатор запроса, глобально-уникальный ключ
			c_evpPkgAsyncError, PARAMS_T - Сериализованная ошибка, если она возникла при создании пакета (может отсутствовать).
			c_evpPkgId, INT_T - Идентификатор созданного пакета (может отсутствовать - при ошибке).
    */
	const wchar_t c_RecordNewPackageDone []=L"KLPKG_RecordNewPackageDone ";
		const wchar_t c_evpPkgAsyncId[]=L"KLPKG_evpPkgAsyncId"; // STRING_T
		const wchar_t c_evpPkgAsyncError[]=L"KLPKG_evpPkgAsyncError"; // PARAMS_T
		const wchar_t c_evpPkgId[]=L"KLPPT_evpPkgId"; // INT_T
	
	typedef struct 
	{
		long nPackageId;
		std::wstring wstrName;
		std::wstring wstrProductName;
		std::wstring wstrProductVersion;
		std::wstring wstrPackagePath;
		AVP_longlong llSize;
		time_t tCreationTime;
		time_t tModificationTime;
		std::wstring wstrSettingsStorageDescr;
		std::wstring wstrProductDisplName;
		std::wstring wstrProductDisplVersion;
	} package_info_t;

	enum IncompatibleAppInfoType{
		IAI_PLAIN_TEXT
	};

	const wchar_t c_IncompatibleAppInfoType[]=L"KLPKG_IncompatibleAppInfoType"; // INT_T
	const wchar_t c_IncompatibleAppInfoPlainText[]=L"KLPKG_IncompatibleAppInfoPlainText"; // STRING_T

    class KLSTD_NOVTABLE Packages : public KLSTD::KLBaseQI {
    public:
		/*!
		  \brief Возвращает список имеющихся пакетов для удаленной инсталляции
       
			\param packages [out] Список имеющихся пакетов 
		*/
		virtual void GetPackages( std::vector<package_info_t>& packages ) = 0;



		/*!
		  \brief получить прокси настроек пакета
       
			\param wstrSettingsStorageDescr [in] дескриптор для
						получения SettingsStorage

			\param ppSettings [out] результат - интерфейс SettingsStorage.
		*/
		virtual void GetPackageSettings( const std::wstring& wstrSettingsStorageDescr,
				KLPRSS::SettingsStorage** ppSettings ) = 0;



		/*!
		  \brief переименовывает пакет
       
			\param nPackageId [in]         id пакета 
			\param wstrNewPackageName [in] новое имя 
		*/
		virtual void RenamePackage(
			long nPackageId,
			const std::wstring& wstrNewPackageName ) = 0;
		


		/*!
		  \brief Выдает путь к сетевой папке, в которую нужно скопировать дистрибутив 
					для создания нового пакета. В общем случае путь может строится на основании
					данных о дистрибутиве (имени и версии).
       
			\param wstrProductName [in] - имя продукта в дистрибутиве

			\param wstrProductVersion [in] - версия продукта в дистрибутиве

			\param wstrFolder [out] - путь к сетевой папке
		*/
		virtual void GetIntranetFolderForNewPackage(
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			std::wstring& wstrFolder ) = 0;



		/*!
		  \brief Создает пакет с настройками по умолчанию на основании дистрибутива,
					переписанного в папку, путь к которой получен при вызове
					метода GetIntranetFolderForNewPackage(...) .
       
			\param wstrFolder [in] - путь, по которому положили дистрибутив (получен при вызове 
					GetIntranetFolderForNewPackage(...)  )

			\param wstrProductName [in] - имя продукта в дистрибутиве

			\param wstrProductVersion [in] - версия продукта в дистрибутиве

			\param packageInfoNew [out] - данные о пакете на сервере, созданном в 
					результатет вызова этой функции.

		*/
		virtual void RecordNewPackage(
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFolder,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			package_info_t& packageInfoNew ) = 0;


		/*!
		  \brief Удаляет пакет
       
			\param wstrPackageName [in] - имя пакета, подлежащего удалению
		*/
		virtual void RemovePackage( long nPackageId ) = 0;

		/*!
			\brief Возвращает путь к пакету в папке общего доступа
				
				  \param long nPackageId [in] - ид-р пакета
		*/
		virtual std::wstring GetIntranetFolderForPackage(long nPackageId) = 0;
	};	
	
    class KLSTD_NOVTABLE Packages2 : public Packages {
    public:
		typedef struct 
		{
			long lTaskId;
			std::wstring wstrTaskName;
			long lGroupId;
			std::wstring wstrGroupName;
		} task_info_t;

		/*!
		  \brief Удаляет пакет
       
			\param nPackageId [in] - идентификатор пакета, подлежащего удалению
			\param vecDependTasks [out] - вектор зависимых задач

				\return - true в случае успеха
		*/
		virtual bool RemovePackage2(
			long nPackageId, 
			std::vector<task_info_t>& vecDependTasks) = 0;

		/*!
		  \brief Возвращает текст скрипта для login-script инталляции
       
			\param nPackageId [in]         id пакета 
			\param wstrTaskId [in]         id задачи (если не задан, генерится новый GUID). 
		*/
		virtual std::wstring GetLoginScript( 
			long nPackageId,
			const std::wstring& wstrTaskId) = 0;

		/*!
		  \brief Задает текст и тип сообщения о необходимости перезагрузки по завершении удаленной инсталляции
       
			\param nPackageId [in]			id пакета (0 - дефолт для новых пакетов)
			\param bRebootImmediately[bool] Перезагружать немедленно
			\param bAskForReboot[bool]		Предлагать перезагрузку
			\param nAskForRebootPeriodInMin[int] Период показа сообщения о необходимости перезагрузки (в мин.)
			\param nForceRebootTimeInMin[int]	Время форсирования перезагрузки при установленной опции "Предлагать перезагрузку"
			\param wstrAskRebootMsgText [in]	Текст сообщения о необходимости перезагрузки. 
		*/
		virtual void SetRebootOptions( 
			long nPackageId,
			bool bRebootImmediately,
			bool bAskForReboot,
			int nAskForRebootPeriodInMin,
			int nForceRebootTimeInMin,
			const std::wstring& wstrAskRebootMsgText) = 0;

		/*!
		  \brief Вычитывает текст и тип сообщения о необходимости перезагрузки по завершении удаленной инсталляции
       
			\param nPackageId [in]			id пакета (0 - дефолт для новых пакетов)
			\param bRebootImmediately[bool] Перезагружать немедленно
			\param bAskForReboot[bool]		Предлагать перезагрузку
			\param nAskForRebootPeriodInMin[int] Период показа сообщения о необходимости перезагрузки (в мин.)
			\param nForceRebootTimeInMin[int]	Время форсирования перезагрузки при установленной опции "Предлагать перезагрузку"
			\param wstrAskRebootMsgText [in]	Текст сообщения о необходимости перезагрузки. 
		*/
		virtual void GetRebootOptions( 
			long nPackageId,
			bool& bRebootImmediately,
			bool& bAskForReboot,
			int& nAskForRebootPeriodInMin,
			int& nForceRebootTimeInMin,
			std::wstring& wstrAskRebootMsgText) = 0;
	};

    class KLSTD_NOVTABLE Packages3 : public Packages2 {
    public:
		/*!
		  \brief Создает пакет с настройками по умолчанию на основании дистрибутива, 
					переданного через FT.
       
			\param wstrFileId [in] - идентификатор файла в FT

			\param wstrProductName [in] - имя продукта в дистрибутиве

			\param wstrProductVersion [in] - версия продукта в дистрибутиве

			\param packageInfoNew [out] - данные о пакете на сервере, созданном в 
					результатет вызова этой функции.

		*/
		virtual void RecordNewPackage2(
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			package_info_t& packageInfoNew ) = 0;

			/*!
		  \brief Получить бинарный образ спец. файла настроек пакета.
       
			\param nPackageId [in]			-	id пакета 
			\param wszFileRelativePath [in]	-	относительный путь к файлу; NULL для файла, указанного в .kpd
			\param ppData [out] результат	-	бинарный образ файла настроек.
		*/
		virtual void ReadPkgCfgFile(
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk** ppChunk) = 0;

		/*!
		  \brief Заменить бинарный образ спец. файла настроек пакета.
       
			\param nPackageId [in]			-	id пакета 
			\param wszFileRelativePath [in]	-	относительный путь к файлу; NULL для файла, указанного в .kpd
			\param pData [in] результат		-	бинарный образ файла настроек.
		*/
		virtual void WritePkgCfgFile(
			long nPackageId,
			wchar_t* wszFileRelativePath,
			KLSTD::MemoryChunk* pChunk) = 0;

		/*!
		  \brief получить прокси бинарных настроек пакета 
       
			\param nPackageId [in]			-	id пакета 
			\param ppPackageConfigurator [out] результат - интерфейс PackageConfigurator.
		*/
		virtual void GetPackageConfigurator(
			long nPackageId,
			KLPKG::PackageConfigurator** ppPackageConfigurator) = 0;

		/*!
		  \brief получить информацию о лицензионном ключе в пакете 
       
			\param nPackageId [in]			-	id пакета 
			\param wstrKeyFileName [out] - имя файла ключа
			\param ppMemoryChunk [out] - тело ключа
		*/
		virtual void GetLicenseKey(
			long nPackageId,
			std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk** ppMemoryChunk) = 0;

		/*!
		  \brief добавить лицензионный ключ в пакет
       
			\param nPackageId [in]			-	id пакета 
			\param wstrKeyFileName [in] - имя файла ключа
			\param ppMemoryChunk [in] - тело ключа
		*/
		virtual void SetLicenseKey(
			long nPackageId,
			const std::wstring& wstrKeyFileName,
			KLSTD::MemoryChunk* pMemoryChunk,
			bool bRemoveExisting) = 0;

		/*!
		  \brief вычитать KPD-файл
       
			\param nPackageId [in]	  -	id пакета 
			\param ppMemoryChunk [in] - тело файла
		*/
		virtual void ReadKpdFile(
			long nPackageId,
			KLSTD::MemoryChunk** ppChunk) = 0;

		/*!
		  \brief прочитать строку в KPD-файле
       
			\param nPackageId [in]	  -	id пакета 
			\param wstrSection [in] - имя секции
			\param wstrKey [in] - имя параметра
			\param wstrDefault [in] - значение параметра по умолчанию
			\param wstrValue [out] - значение параметра
		*/
		virtual void GetKpdProfileString(
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrDefault,
			std::wstring& wstrValue) = 0;

		/*!
		  \brief записать строку в KPD-файл
       
			\param nPackageId [in]	  -	id пакета 
			\param wstrSection [in] - имя секции
			\param wstrKey [in] - имя параметра
			\param wstrValue [in] - значение параметра
		*/
		virtual void WriteKpdProfileString(
			long nPackageId,
			const std::wstring& wstrSection,
			const std::wstring& wstrKey,
			const std::wstring& wstrValue) = 0;
	};
	
    class KLSTD_NOVTABLE Packages4 : public Packages3 {
    public:
		/*!
		  \brief Задает текст и тип сообщения о необходимости перезагрузки по завершении удаленной инсталляции
       
			\param nPackageId [in]			id пакета (0 - дефолт для новых пакетов)
			\param bRebootImmediately[in] Перезагружать немедленно
			\param bAskForReboot[in]		Предлагать перезагрузку
			\param nAskForRebootPeriodInMin[in] Период показа сообщения о необходимости перезагрузки (в мин.)
			\param nForceRebootTimeInMin[in]	Время форсирования перезагрузки при установленной опции "Предлагать перезагрузку"
			\param wstrAskRebootMsgText [in]	Текст сообщения о необходимости перезагрузки. 
			\param bForceAppsClosed [in]	Форсировать закрытие приложений (перезагружать залоченных пользователей)
			\param parExtraParams [in] Дополнительные параметры
		*/
		virtual void SetRebootOptionsEx( 
			long nPackageId,
			bool bRebootImmediately,
			bool bAskForReboot,
			int nAskForRebootPeriodInMin,
			int nForceRebootTimeInMin,
			const std::wstring& wstrAskRebootMsgText,
			bool bForceAppsClosed,
			KLPAR::ParamsPtr parExtraParams) = 0;

		/*!
		  \brief Вычитывает текст и тип сообщения о необходимости перезагрузки по завершении удаленной инсталляции
       
			\param nPackageId [in]			id пакета (0 - дефолт для новых пакетов)
			\param bRebootImmediately[out] Перезагружать немедленно
			\param bAskForReboot[out]		Предлагать перезагрузку
			\param nAskForRebootPeriodInMin[out] Период показа сообщения о необходимости перезагрузки (в мин.)
			\param nForceRebootTimeInMin[out]	Время форсирования перезагрузки при установленной опции "Предлагать перезагрузку"
			\param wstrAskRebootMsgText [out]	Текст сообщения о необходимости перезагрузки. 
			\param bForceAppsClosed [out]	Форсировать закрытие приложений (перезагружать залоченных пользователей)
			\param parExtraParams [out] Дополнительные параметры
		*/
		virtual void GetRebootOptionsEx( 
			long nPackageId,
			bool& bRebootImmediately,
			bool& bAskForReboot,
			int& nAskForRebootPeriodInMin,
			int& nForceRebootTimeInMin,
			std::wstring& wstrAskRebootMsgText,
			bool& bForceAppsClosed,
			KLPAR::ParamsPtr& parExtraParams) = 0;
	};
	
    class KLSTD_NOVTABLE Packages5 : public Packages4 {
    public:
		/*
		  \brief Создает пакет с настройками по умолчанию на основании дистрибутива, 
					переданного через FT.
       
			\param wstrFileId [in] - идентификатор файла в FT

			\param wstrProductName [in] - имя продукта в дистрибутиве

			\param wstrProductVersion [in] - версия продукта в дистрибутиве

			\param packageInfoNew [out] - данные о пакете на сервере, созданном в 
					результатет вызова этой функции.
		*/
		virtual void RecordNewPackageAsync(
            const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			const std::wstring& wstrRequestId,
			KLADMSRV::AdmServerAdviseSink*  pSink,
            KLADMSRV::HSINKID&              hSink) = 0;

		/*
		  \brief Возвращает информацию о пакете.
       
			\param nPackageId [in]			id пакета (0 - дефолт для новых пакетов)

			\param packageInfoNew [out] - данные о пакете на сервере.
		*/
		virtual void GetPackageInfo(
			long nPackageId,
			package_info_t& packageInfoNew) = 0;
	};

    class KLSTD_NOVTABLE Packages6 : public Packages5 {
    public:
		/*
		  \brief Возвращает информацию о несовместимых приложениях.
       
			\param nPackageId [in]

			\param ppData [out] - данные со списком несовместимых приложений;
									тип данных в c_IncompatibleAppInfoType (IncompatibleAppInfoType);
									данные в c_IncompatibleAppInfoPlainText

			\param bCanRemoveByInstaller [out] - поддерживается ли опция удаления несовместимых приложений

			\return true, если список несовместимых приложений поддерживатся, false - в противном случае
		*/
		virtual bool GetIncompatibleAppsInfo(
			long nPackageId,
			KLPAR::Params** ppData,
			bool& bCanRemoveByInstaller,
			bool& bRemoveByInstaller) = 0;
		
		/*
		  \brief Задает настройку удаления несовместимых приложений.
       
			\param nPackageId [in]			id пакета

			\param bRemoveIncompatibleApps [in]	удалять ли несовместимые приложения

			\return true, если настройка успешно применена, false - если настройка не поддерживается
		*/
		virtual bool SetRemoveIncompatibleApps(
			long nPackageId,
			bool bRemoveIncompatibleApps) = 0;
	};
}

KLCSSRVP_DECL void KLPKG_ParseSettingsStorageDescr(
	const std::wstring& wstrSettingsStorageDescr, 
	std::wstring& wstrSettingsStorageRelPath,
	long& nPackageId);

#endif // _KLPKG_H__872AC7A0_D727_4032_946A_963AA70B1EEC__
