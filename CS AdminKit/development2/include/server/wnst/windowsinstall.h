/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file WNST/WindowsInstall.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief Интерфейс для удаленной инсталляции Агента на компьютеры в Windows - сети.
 *
 */

#ifndef KLWNST_WINDOWSINSTALL_H
#define KLWNST_WINDOWSINSTALL_H

#include <time.h>
#include <string>
#include <vector>
#include <map>

#include <std/base/klbase.h>

namespace KLWNST {

    //! Структура имя/пароль
    struct SecurityCtx {
        std::wstring login;
        std::wstring password;
		SecurityCtx(){};
		SecurityCtx(const std::wstring& lgn, const std::wstring& pwd): login(lgn), password(pwd){};
		bool operator == (const SecurityCtx& ctx) const {return ctx.login == login && ctx.password == password;}
    };

    //! Возможные типы доступа к разделяемой папке
    enum SharePermissions {
        SP_READ    =   0x00000001, ///< Клиент может прочитать содержимое папки.
        SP_EXECUTE =   0x00000002, ///< Клиент может запустить выполнение программы из разделяемой папки
        SP_WRITE    = 0x00000004 ///< Клиент может менять содержимое папки.
    };

	typedef std::map<std::wstring, long> NetRes2ConnMap;

    /*!
    * \brief Интерфейс для создания, подержания и автоатического удаления соединеня к
            сетевому ресурсу.
    */    
    class NetResourceConnection : public KLSTD::KLBase
    {
    public:
    /*!
      \brief Устанавливает соединенения с сетвым ресурсом. Для одного
        объекта NetResourceConnection может быть вызван только один раз.
        Последующие вызовы игнорируются. При освобождении объекта соединенение
        разрывается.
        \param wstrResource       [in]  путь к ресурсу. Например, "\\machine\" или "\\machine\с$"
        \param ctx                [in]  аккаунт, возможно локальный для этой машины.
    */
        virtual void AddConnection(
            const std::wstring & wstrResource,
            const SecurityCtx & ctx,
            const std::wstring & wstrResourceDomain = L"",
			AVP_dword* pdwSystemErrorCode = NULL) = 0;

        virtual void AddConnectionEx(
			KLWNST::NetRes2ConnMap& mpNetRes2ConnMap,
			KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS,
            const std::wstring & wstrResource,
            const SecurityCtx & ctx,
            const std::wstring & wstrResourceDomain = L"",
			AVP_dword* pdwSystemErrorCode = NULL) = 0;

        virtual void AddConnection2(
            const std::wstring & wstrResource,
            const std::vector<KLWNST::SecurityCtx>& vecSecCtxs,
            const std::wstring & wstrResourceDomain = L"",
			AVP_dword* pdwSystemErrorCode = NULL) = 0;

        virtual void AddConnectionEx2(
			KLWNST::NetRes2ConnMap& mpNetRes2ConnMap,
			KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS,
            const std::wstring & wstrResource,
            const std::vector<KLWNST::SecurityCtx>& vecSecCtxs,
            const std::wstring & wstrResourceDomain = L"",
			AVP_dword* pdwSystemErrorCode = NULL) = 0;
    };
	

    struct security_descriptor;
	//typedef SECURITY_DESCRIPTOR security_descriptor;

    /*!
    * \brief Интерфейс для удаленной инсталляции Агента на компьютеры в Windows - сети.
    *
    *   Модуль поддерживает два способа удаленной инсталляции.  Для компьютеров,
    *   работающих под управлением Windows NT (2000, XP), основным способом
    *   является инсталляция и запуск сервиса на удаленной машине.
    *
    *   Для компьютеров, работающих под управлением Windows 95, основным способом
    *   является инсталляция при помощи login script для пользователя домена.
    *
    *   Удаленная инсталляция возможна только для компьютеров и пользователей,
    *   входящих в домен.
    *   
    *   Для того чтобы вызовы данного модуля нормально работали, приложение должно
    *   обладать соответствующими правами.  По умолчанию используются права текущего
    *   потока, эти прова можно переопределить при создании объекта класса WindowsInstall.
    *
    *   Вызовы данного модуля могут блокировать вызывающих поток на неопределенное
    *   время.  Это нужно учитывать при использовании данной библиотеки.
    * 
    *   !!! Класс не является ThreadSafe !!! 
	*	!!! Для каждого нового объекта требуется заводить новый экзмпляр объекта !!!
    */


    class WindowsInstall : public KLSTD::KLBase {
    public:


    /*!
      \brief Устанавливает для пользователя домена приложение, которое будет
            запущено в момент следующего login пользователя.

        Приложение идентифицируется по имени в разделяемой директории Сервера.

        Если для пользователя домена уже определен login script , то первоначальный
        login script будет запущен после запуска определяемого параметром
        applPath приложения.


        \param domain       [in]  Имя домена.

       \param user         [in]  Имя пользователя домена.
        \param applPath     [in]  Имя приложения в разделяемой директории.
        \param identity     [in]  Идентификатор записи. Если запись с таким
                                    идентификаторм уже существует, она будет заменена.
        \param ctx          [in] аккаунт, используемый для доступа к домену
    */
        virtual void SetLoginScript (
            const std::wstring & domain,
            const std::wstring & user,
            const std::wstring & applPath,
            const std::wstring & identity,
            const SecurityCtx & ctx = SecurityCtx() ) = 0;


    /*!
      \brief Проверяет, есть ли в login script'е пользователя запись с определенным identity

        \param domain       [in]  Имя домена.
        \param user         [in]  Имя пользователя домена.
        \param identity     [in]  Идентификатор записи. 
        \param ctx          [in] аккаунт, используемый для доступа к домену
        \return       true если запись существует
    */
        virtual bool UserHasLoginScript(
            const std::wstring& sDomainName,
            const std::wstring& sUser,
            const std::wstring & identity,
            const SecurityCtx & ctx = SecurityCtx() ) = 0;

    /*!
      \brief Удаляет для пользователя домена запуск приложения, установленного
            ранее при помощи вызова SetLoginScript.

        \param domain       [in]  Имя домена.
        \param user         [in]  Имя пользователя домена.
        \param identity     [in]  Идентификатор записи. 
        \param ctx          [in] аккаунт, используемый для доступа к домену
    */
        virtual void RemoveLoginScript (
            const std::wstring & domain,
            const std::wstring & user,
            const std::wstring & identity,
            const SecurityCtx & ctx = SecurityCtx() ) = 0;


    /*!
      \brief Удаляет для всех пользователей домена запуск приложения, установленного
            ранее при помощи вызовов SetLoginScript.

        \param domain       [in]  Имя домена.
        \param ctx          [in] аккаунт, используемый для доступа к домену
   */
        virtual void RemoveAllLoginScripts (
            const std::wstring & domain,
            const SecurityCtx & ctx = SecurityCtx() ) = 0;


   /*!
      \brief Инсталлирует на удаленную рабочую станцию сервис,
             который идентифицируется по имени в локальной директории
             Сервера Администрирования.

        \param host         [in]  Имя рабочей станции в домене.
        \param name         [in]  Имя сервиса на удаленной машине.
        \param srvPath      [in]  Путь к исполняемому файлу сервиса на рабочей станции в домене.
        \param args         [in]  Аргументы командной строки на удаленной машине.
        \param comments     [in]  Описание сервиса на удаленной машине.
        \param ctx          [in]  Account, под которым будет запускаться сервис.
       
    */
        virtual void InstallService(
            const std::wstring & host,
            const std::wstring & name,
            const std::wstring & srvPath,
			const std::wstring & args,
            const std::wstring & comments,
            const SecurityCtx  & ctx ) = 0;

   /*!
      \brief Удаляет на удаленной рабочей станции сервис,
             который идентифицируется по имени в локальной директории
             Сервера Администрирования.

        \param host         [in]  Имя рабочей станции в домене.
        \param name         [in]  Имя сервиса на удаленной машине.
       
    */
        virtual void RemoveService(
            const std::wstring & host,

           const std::wstring & name ) = 0;

   /*!
      \brief Запускает на удаленной рабочей станции сервис,
             который идентифицируется по имени в локальной директории
             Сервера Администрирования.

        \param host         [in]  Имя рабочей станции в домене.
        \param name         [in]  Имя сервиса на удаленной машине.
       
    */
        virtual void StartService(
            const std::wstring & host,
            const std::wstring & name ) = 0;

   /*!
      \brief Создает на сервере администрирования разделяемую папку.

		\param host         [in]  Имя компьютера
        \param name         [in]  Имя разделяемой папки.
        \param path         [in]  Локальный путь к разделяемой папке.
        \param permissions  [in]  Битовые флаги для определения прав доступа 
                                    к разделяемой папке.
        \param username		[in]  Имя пользователя или группы которым разрешается доступ
    */
        virtual void CreateShare(const wchar_t* host,
								 const std::wstring & name,
								 const std::wstring & path,
								 SharePermissions permissions,
								 const std::wstring& username = L"everyone") = 0;

   /*!
      \brief Создает на сервере администрирования разделяемую папку.

		\param host         [in]  Имя компьютера
        \param name         [in]  Имя разделяемой папки.
        \param psd          [in]  SECURITY_DESCRIPTOR c правами доступа к разделяемой папке
        \param username		[in]  Имя пользователя или группы которым разрешается доступ
    */
		virtual void CreateShare(const wchar_t* host, 
								 const std::wstring& name, 
								 const std::wstring& path, 
								 security_descriptor* psd) = 0;

   /*!
      \brief Удаляет на сервере администрирования разделяемую папку. Содержимое
             локальной директории не меняется.

		\param host         [in]  Имя компьютера
        \param name         [in]  Имя разделяемой папки.
       
    */
        virtual void DeleteShare(const wchar_t* host, 
								 const std::wstring & name ) = 0;

   /*!
      \brief Возвращает локальный путь для указанной разделяемой папки

		\param host         [in]  Имя компьютера
        \param name         [in]  Имя разделяемой папки.
        \return				локальный путь для указанной разделяемой папки
    */
		virtual std::wstring GetShareLocalPath(const wchar_t* host, 
											   const std::wstring& name) = 0;
	
   /*!
      \brief Инсталлирует на удаленную рабочую станцию сервис,
             который идентифицируется по имени в локальной директории
             Сервера Администрирования.

        \param host         [in]  Имя рабочей станции в домене.
        \param name         [in]  Имя сервиса на удаленной машине.
        \param srvPath      [in]  Путь к исполняемому файлу сервиса на рабочей станции в домене.
        \param args         [in]  Аргументы командной строки на удаленной машине.
        \param comments     [in]  Описание сервиса на удаленной машине.
        \param ctx          [in]  Account, под которым будет запускаться сервис.
		\param ulServiceType[in]  Specifies the type of service. This parameter must be one of the following service types. 
				SERVICE_FILE_SYSTEM_DRIVER		File system driver service. 
				SERVICE_KERNEL_DRIVER			Driver service. 
				SERVICE_WIN32_OWN_PROCESS		Service that runs in its own process. 
				SERVICE_WIN32_SHARE_PROCESS		Service that shares a process with other services. 

			If you specify either SERVICE_WIN32_OWN_PROCESS or SERVICE_WIN32_SHARE_PROCESS, 
			and the service is running in the context of the LocalSystem account, 
			you can also specify the following type. 
				SERVICE_INTERACTIVE_PROCESS		The service can interact with the desktop. 

		\param ulStartType	[in]  Specifies when to start the service. 
			This parameter must be one of the following start types.
				SERVICE_AUTO_START				A service started automatically by the service control manager 
												during system startup. 
				SERVICE_BOOT_START				A device driver started by the system loader. 
												This value is valid only for driver services. 
				SERVICE_DEMAND_START			A service started by the service control manager 
												when a process calls the StartService function. 
				SERVICE_DISABLED				A service that cannot be started. 
												Attempts to start the service result in the error code ERROR_SERVICE_DISABLED. 
				SERVICE_SYSTEM_START			A device driver started by the IoInitSystem function. 
												This value is valid only for driver services. 

		\param dependencies	[in]    !!! CURRENTLY UNSUPPORTED !!! 
			Pointer to a double null-terminated array of null-separated names of services or load ordering groups 
			that the system must start before this service. Specify NULL or an empty string if the service has 
			no dependencies. Dependency on a group means that this service can run if at least one member of the 
			group is running after an attempt to start all members of the group. 
			You must prefix group names with SC_GROUP_IDENTIFIER so that they can be distinguished from a service 
			name, because services and service groups share the same name space.

		\param ulErrorControl[in]   !!! CURRENTLY UNSUPPORTED !!! 
			Specifies the severity of the error if this service fails to start during startup, 
			and determines the action taken by the startup program if failure occurs. 
			This parameter must be one of the following values.
				SERVICE_ERROR_IGNORE			The startup program logs the error but continues the startup operation. 
				SERVICE_ERROR_NORMAL			The startup program logs the error and puts up a message box 
												pop-up but continues the startup operation. 
				SERVICE_ERROR_SEVERE			The startup program logs the error. 
												If the last-known-good configuration is being started, 
												the startup operation continues. 
												Otherwise, the system is restarted with the last-known-good configuration. 
				SERVICE_ERROR_CRITICAL			The startup program logs the error, if possible. 
												If the last-known-good configuration is being started, 
												the startup operation fails. Otherwise, the system is restarted 
												with the last-known good configuration. 
		\param sLoadOrderGroup[in]	!!! CURRENTLY UNSUPPORTED !!! 
		\param ulTagId		[out]	!!! CURRENTLY UNSUPPORTED !!! 
    */
        virtual void InstallServiceEx(
            const std::wstring & host,
            const std::wstring & name,
            const std::wstring & srvPath,
			const std::wstring & args,
            const std::wstring & comments,
            const SecurityCtx  & ctx,
			const unsigned long ulServiceType, 
			const unsigned long ulStartType,
			std::vector<std::wstring>& dependencies, 
			const unsigned long ulErrorControl, 
			const std::wstring& sLoadOrderGroup, 
			unsigned long* ulTagId = NULL) = 0;

		virtual std::wstring GetLogonScriptFolderPath(const std::wstring& sDomainName) = 0;
   };
}

/*!
  \brief Создает объект класса WindowsInstall

    \param ppWindowsInstall      [out] Указатель на объект класса WindowsInstall
    \param ctx                   [in] Login, под которым будут работать вызовы модуля.

*/
KLCSSRV_DECL void KLWNST_CreateWindowsInstall
    (   KLWNST::WindowsInstall **ppWindowsInstall,
        const KLWNST::SecurityCtx & ctx = KLWNST::SecurityCtx() );

KLCSSRV_DECL void KLWNST_CreateNetResourceConnection( KLWNST::NetResourceConnection **ppNetResourceConnection );

KLCSSRV_DECL bool KLWNST_GetLocalPathForNetworkSharePath(
    const std::wstring& wstrNetworkPath,
    std::wstring& wstrLocalPath);

#endif // KLWNST_WINDOWSINSTALL_H

