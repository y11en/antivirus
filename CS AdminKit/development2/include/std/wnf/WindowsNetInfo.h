/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file WNF/WindowsNetInfo.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief Интерфейс для получения информации о Windows - сети.
 *
 */

#ifndef KLWNF_WINDOWSNETINFO_H
#define KLWNF_WINDOWSNETINFO_H

#include <time.h>
#include <string>
#include <list>

#include <std/base/klbase.h>

namespace KLWNF {

	typedef std::list<std::wstring> OBJ_LIST;

    //! Перечисляет возможные типы компьютеров в Windows - сети
	// Для UNIX систем выставляется флаг CT_SERVER_UNIX и один из флагов
	// CT_SERVER_FREEBSD | CT_SERVER_LINUX
    enum ComputerType {
		CT_WORKSTATION = 0x00000001,
		CT_SERVER = 0x00000002,
		CT_SQLSERVER = 0x00000004,
		CT_DOMAIN_CTRL = 0x00000008,
		CT_DOMAIN_BAKCTRL = 0x00000010,
		CT_TIME_SOURCE = 0x00000020,
		CT_AFP = 0x00000040,
		CT_NOVELL = 0x00000080,
		CT_DOMAIN_MEMBER = 0x00000100,
		CT_PRINTQ_SERVER = 0x00000200,
		CT_DIALIN_SERVER = 0x00000400,
		CT_XENIX_SERVER = 0x00000800,
		CT_SERVER_UNIX = CT_XENIX_SERVER,
		CT_NT = 0x00001000,
		CT_WFW = 0x00002000,
		CT_SERVER_FREEBSD = CT_WFW,
		CT_SERVER_MFPN = 0x00004000,
		CT_SERVER_NT = 0x00008000,
		CT_POTENTIAL_BROWSER = 0x00010000,
		CT_BACKUP_BROWSER = 0x00020000,
		CT_MASTER_BROWSER = 0x00040000,
		CT_DOMAIN_MASTER = 0x00080000,
		CT_SERVER_OSF = 0x00100000,
		CT_SERVER_VMS = 0x00200000,
		CT_SERVER_LINUX = CT_SERVER_VMS,
		CT_WINDOWS = 0x00400000,  /* Windows95 and above */
		CT_DFS = 0x00800000,  /* Root of a DFS tree */
		CT_CLUSTER_NT = 0x01000000,  /* NT Cluster */
		CT_DCE = 0x10000000,  /* IBM DSS (Directory and Security Services) or equivalent */
		CT_ALTERNATE_XPORT = 0x20000000,  /* return list for alternate transport */
		CT_LOCAL_LIST_ONLY = 0x40000000,  /* Return local list only */
		CT_DOMAIN_ENUM = 0x80000000,
		CT_ALL = 0xFFFFFFFF  /* handy for NetServerEnum2 */
    };

    //! Перечисляет типы ОС - платформ для компьютеров
    enum PlatformType {
        P_DOS, 
        P_OS2, 
        P_NT, 
        P_OSF, 
        P_VMS,
		P_NOVELL,
		P_LINUX,
		P_FREEBSD,
		P_WIN_MOBILE,
		P_SYMBIAN_MOBILE
    };

    //! Перечисляет типы привилегий пользователей
    enum PrivilegeLevel {
        PL_GUEST,
        PL_USER,
        PL_ADMINISTRATOR
    };

    //! Информация об объекте AD
    struct ADObjectInfo {
        std::wstring name;						// Имя объекта.
        std::wstring guid;						// GUID объекта.
		std::wstring accountName;				// sAMAccountName
		std::wstring dNSHostName;				// DNS Host Name
		std::wstring domainDistName;			// ActiveDS DNS distinguished domain name ("DC=avp,DC=ru")
        std::wstring distinguishedName;         // ActiveDS distinguished name
        
		KLSTD::CAutoPtr<KLSTD::KLBase> handle;	// Handle объекта
    };

	typedef std::list<ADObjectInfo> AD_OBJ_LIST;

    //! Информация о рабочей станции в сети.

    struct ComputerInfo {

		ComputerInfo(): 
			ip(0),
			ctype(CT_WORKSTATION),
			ptype(P_NT),
			versionMajor(0),
			versionMinor(0)
		{
		}
		
        std::wstring name;			///< Имя компьютера.
        std::wstring domain;		///< Имя домена, в который входит компьютер.
        std::wstring dnsDomain;		///< DNS домен для компьютера
        std::wstring dnsName;		///< DNS имя компьютера
        ComputerType ctype;			///< Тип компьютера
        PlatformType ptype;			///< Платформа для ПО на компьютере.
        int versionMajor;			///< Главный номер версии ОС на компьютере
        int versionMinor;			///< Главный номер версии ОС на компьютере
        int ip;						///< IP адрес компьютера
    };

    //! Информация о пользователе домена
    struct UserInfo {
        std::wstring name;			///< Имя пользователя (login).
        std::wstring domain;		///< Имя домена, в который входит пользователь.
        PrivilegeLevel pLevel;		///< Уровень привилегий пользователя.
        time_t lastLogon;			///< Время последнего logon пользователя.
        time_t lastLogoff;			///< Время последнего logoff пользователя.
 
    };

	struct ComputerInfo2000 {
		ComputerInfo2000(const wchar_t* pszName = L""):
			name(pszName?pszName:L"")
		{
		}
		std::wstring name;
		std::wstring fullDnsName;		// DNS имя с доменом (comp1.avp.ru)
	};
	typedef std::list<ComputerInfo2000> OBJ_LIST2000;

    /*!
    * \brief Интерфейс для сбора информации о Windows - сети.
    *
    *  Данный модуль предоставляет текущую информацию о Windows - сети,
    *  в которой работает приложение, использующее модуль.  Принята следующая
    *  модель Windows - сети:
    *
    *  1. Windows - сеть состоит из набора доменов и рабочих групп.
    *  2. В Windows - сети присутствуют либо Windows NT (2000, XP) рабочие станции,
    *   либо Windows 95 (98,Me) компьютеры.
    *  3. Каждый домен включает в себя набор Windows NT (2000, XP) рабочих станций и
    *    набор пользовательских accounts.
    *  4. Удаленная инсталляция агентов администрирования осуществляется только для
    *    компьютеров, на которых работают пользователи, входящие в домен.
    *  5. Компьютер может не входить в домен (и пользователь, работающий на 
    *    компьютере), но при этом система администрирования будет нормально 
    *    работать для такого компьютера, если инсталляция Агента будет проведена 
    *    вручную.
    *  6. Компьютер имеет следующие атрибуты:
    *       а) NetBios Имя (уникальное в Windows - сети).
    *       b) DNS имя.
    *       с) DNS домен.
    *       d) Имя домена либо рабочей группы, в которую входит компьютер 
    *           (не обязательно).
    *       e) IP адрес.
    *
    *
    *  Для сбора информации используются следующие объекты:
    *
    *  1. Контроллеры домена (для получения информации о Windows NT компьютерах
    *     и пользователях, зарегистрированных в домене).
    *  2. Browser service (для получения списка присутствующих в сети рабочих групп
    *     и доменов, а также Windows 95 (98,Me) компьютеров и Windows NT компьютеров,
    *     не входящих в домен.
    *
    *   Вызовы данного модуля могут блокировать вызывающих поток на неопределенное
    *   время.  Это нужно учитывать при использовании данной библиотеки.
    *
    *  
    *  
    */


    class WindowsNetInfo : public KLSTD::KLBase {
    public:

    /*!
      \brief Возвращает список рабочих групп в сети

        \param workgroups [out] Список рабочих групп в сети.
      
		Генерирует исключения из WinError.h и LMErr.h (см. WNetEnumResource)
    */
        virtual void GetWorkgroups (OBJ_LIST & workgroups)  = 0;

    /*!
      \brief Возвращает список доменов в сети
       
        \param domains [out] Список доменов в сети.

		Генерирует исключения из WinError.h и LMErr.h (см. WNetEnumResource)
    */
        virtual void GetDomains (OBJ_LIST & domains)  = 0;


    /*!
      \brief Возвращает список имен рабочих станций в домене.

        Информация получается из контроллера домена. Данный вызов
        возвращает полный список рабочих станций в домене, даже если
        рабочая станция сейчас выключена.

        \param domain       [in]  Имя домена.
        \param workstations [out] Список рабочих станций, зарегистрированных в домене.
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetQueryDisplayInformation)
    */
        virtual void GetDomainWorkstations
            ( const std::wstring & domain,
              OBJ_LIST & workstations ) = 0;

    /*!
      \brief Возвращает список имен рабочих станций в домене.

        Информация получается из контроллера домена. Данный вызов
        возвращает полный список рабочих станций в домене, даже если
        рабочая станция сейчас выключена.

        \param domain       [in]  Имя домена.
        \param workstations [out] Список рабочих станций, зарегистрированных в домене.
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetQueryDisplayInformation)
    */
        virtual void GetDomainWorkstations2
            ( const std::wstring & domain,
              OBJ_LIST2000 & workstations ) = 0;

    /*!
      \brief Возвращает список имен рабочих станций в домене.

        Информация получается из Active Directory. Данный вызов
        возвращает полный список рабочих станций в домене, даже если
        рабочая станция сейчас выключена.

        \param domain       [in]  Имя домена.
        \param workstations [out] Список рабочих станций, зарегистрированных в домене.
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetQueryDisplayInformation)
    */
		virtual void GetADDomainWorkstations(const std::wstring& domain, 
											 OBJ_LIST2000& workstations) = 0;

    /*!
      \brief Возвращает список имен компьютеров в рабочей группе или домене.

        Информация получается из browser service. Данный вызов
        возвращает список компьютеров, присутствующих в
        данных момент в сети и включенных в рабочую группу с заданным
        именем.  Имя рабочей группы может совпадать с именем домена, 
        в этом случае возвращается список всех компьютеров, входящих
        в домен и присутствующих сейчас в сети.

        \param workgroup   [in]  Имя рабочей группы либо домена.  
                                 Если пустое - возвращаются все компьютеры, 
                                 не входящие ни в одну рабочую группу
                                 и ни в один домен.
        \param computers   [out] Список компьютеров, присутствующих в рабочей группе
                                 либо в домене в настоящий момент.

		Генерирует исключения из WinError.h и LMErr.h (см. WNetEnumResource)
    */
        virtual void GetWorkgroupComputers
            ( const std::wstring & workgroup,
              OBJ_LIST & computers ) = 0;


    /*!
      \brief Возвращает список имен компьютеров в рабочей группе или домене.

        Информация получается из browser service. Данный вызов
        возвращает список компьютеров, присутствующих в
        данных момент в сети и включенных в рабочую группу с заданным
        именем.  Имя рабочей группы может совпадать с именем домена, 
        в этом случае возвращается список всех компьютеров, входящих
        в домен и присутствующих сейчас в сети.

        \param workgroup   [in]  Имя рабочей группы либо домена.  
                                 Если пустое - возвращаются все компьютеры, 
                                 не входящие ни в одну рабочую группу
                                 и ни в один домен.
        \param computers   [out] Список компьютеров, присутствующих в рабочей группе
                                 либо в домене в настоящий момент.

		Генерирует исключения из WinError.h и LMErr.h (см. WNetEnumResource)
    */
        virtual void GetWorkgroupComputers2
            ( const std::wstring & workgroup,
              OBJ_LIST2000 & computers ) = 0;

    /*!
      \brief Возвращает список пользователей, зарегистрированных в домене.

        Информация получается из контроллера домена.

        \param domain       [in]  Имя домена.
        \param users        [in, out] Список пользователей, зарегистрированных в домене.
		\param bIncludeDisabledAccounts [in] Возвращать и disabled accounts
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetQueryDisplayInformation)
    */
        virtual void GetDomainUsers
            ( const std::wstring & domain,
              OBJ_LIST & users,
			  bool bIncludeDisabledAccounts ) = 0;


    /*!
      \brief Возвращает информацию о компьютере, 
            зарегистрированном в домене или рабочей группе.

        Информация получается из Browser Service (NetServerGetInfo).

        \param workgroup    [in]  Имя рабочей группы или домена.
        \param name         [in]  Имя компьютера.
        \param info         [out] Информация о компьютере.
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetServerGetInfo)
    */
        virtual void GetComputerInfo
            ( const std::wstring & workgroup,
              const std::wstring & name,
              ComputerInfo & info ) = 0;


    /*!
      \brief Возвращает информацию о пользователе, 
            зарегистрированном в домене.

        \param domain       [in]  Имя домена.
        \param name         [in]  Имя пользователя.
        \param info         [out] Информация о пользователе.
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetUserGetInfo)
    */
        virtual void GetUserInfo
            ( const std::wstring & domain,
              const std::wstring & name,
              UserInfo & info ) = 0;

    /*!
      \brief Возвращает информацию о пользователях, использующих
            сейчас рабочую станцию.

        Информация получается при помощи вызова
        непосредственно рабочей станции (NetWkstaGetInfo, NetWkstaUserEnum).

        \param name             [in]  Имя компьютера.
        \param loggedInUsers    [out] Имена пользователей, использующих сейчас компьютер.
        \param domains          [out] Имена доменов пользователей, использующих сейчас компьютер. 
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetWkstaUserEnum)
    */
        virtual void GetLoggedInUsers
            ( const std::wstring & ComputerName,
              OBJ_LIST & loggedInUsers,
              OBJ_LIST & domains ) = 0;

    /*!
      \brief Возвращает информацию о группах прав пользователя, 
            зарегистрированного в домене.

        \param domain       [in]  Имя домена.
        \param name         [in]  Имя пользователя.
        \param groups       [out] Список глобальных групп, в которые входит пользователь.
       
		 Генерирует исключения из WinError.h и LMErr.h (см. NetUserGetGroups)
    */
        virtual void GetUserGroups
            ( const std::wstring & domain,
              const std::wstring & name,
              OBJ_LIST & groups ) = 0;

    /*!
      \brief Возвращает список organizationalUnits из ActiveDirectory

        \param OUs [out] Список organizationalUnits из ActiveDirectory.
      
		Генерирует исключения из WinError.h
    */
        virtual void GetADsOrganizationalUnits (const ADObjectInfo& parentAdObject, 
												AD_OBJ_LIST & OUs,
												bool bIncludeContainers = false)  = 0;

    /*!
      \brief Возвращает список компьютеров в указанном organizationalUnits из ActiveDirectory

        \param OUs [out] Список компьютеров.
      
		Генерирует исключения из WinError.h
    */
        virtual void GetADsOrganizationalUnitComputers (const ADObjectInfo& parentAdObject, AD_OBJ_LIST & OUComps)  = 0;

	/*!
	  \brief Конвертирует имя домена в формат NT4

		\param name			[in] distinguished domain name ("DC=avp,DC=ru")
		\return				[out] NT4 domain name
	*/
		virtual std::wstring DistDomainNameToNT4Name(const std::wstring& domain) = 0;
    };
}

/*!
  \brief Создает объект класса WindowsNetInfo

    \param wnetinfo      [out] Указатель на объект класса WindowsNetInfo
*/
void KLWNF_CreateWindowsNetInfo(KLWNF::WindowsNetInfo **ppWindowsNetInfo, bool volatile* pbCancelEnumeration = NULL);

/*!
  \brief Создает объект класса WindowsNetInfo

	\param name			 [in] имя компьютера (игнорируется для Win9x)
    \param info			 [out] Информация о компьютере
*/
void KLWNF_GetComputerInfo(const std::wstring& name, KLWNF::ComputerInfo& info);

#endif // KLWNF_WINDOWSNETINFO_H
