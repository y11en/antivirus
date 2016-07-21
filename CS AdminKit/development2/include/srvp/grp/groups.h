/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file GRP/Groups.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Интерфейс для получения и редактирования информации о логических группах 
 *        в Сервере Администрирования.
 *
 */

#ifndef KLGRP_GROUPS_H
#define KLGRP_GROUPS_H

#include <time.h>
#include <string>
#include <vector>

#include <std/base/klbase.h>
#include <std/par/params.h>
#include <srvp/admsrv/admsrv.h>
#include <server/srvinst/server_constants.h>

namespace KLGRP {
	
    const wchar_t c_grp_array_groups[] = L"groups";  // Имя параметра 'массив групп'
	const wchar_t c_grp_array_hosts[] = L"hosts";  // Имя параметра 'массив хостов'

//! Имена полей в контейнере Params с информация о группе.
    //! Идентификатор записи в базе данных
	const wchar_t c_grp_id[] = L"id";  //INT_T
	//! Имя группы.
    const wchar_t c_grp_name[] = L"name"; // STRING_T, 
	//! Идентификатор родительской группы.
    const wchar_t c_grp_parentId[] = L"parentId"; // INT_T
	//! Когда была создана группа
    const wchar_t c_grp_creationDate[] = L"creationDate"; // TIME_T
	//! Последнее время обновления информации о группе
    const wchar_t c_grp_lastUpdate[] = L"lastUpdate"; // TIME_T
	//! Полное число компьютеров в группе (во всем поддереве)
    const wchar_t c_grp_hostsNum[] = L"hostsNum"; // INT_T
	//! Полное число дочерних групп в группе (во всем поддереве)
    const wchar_t c_grp_childGroupsNum[] = L"childGroupsNum"; // INT_T   
	/*!
		Идентификатор группы, в которую нужно автоматически добавлять
		компьютеры, попадающие в данную группу - подгруппу группы UNASSIGNED.
		(Только для групп - подгрупп группы UNASSIGNED).
	*/
	const wchar_t c_grp_autoAddHost[] = L"autoAddHost"; //INT_T

    //! auto add hosts with active nagent only
    const wchar_t c_grp_autoAddNagentOnly[] = L"autoAddHostOnlyNagents";

	//! Число дней, через которое надо удалять неактивный компьютер из группы.
    const wchar_t c_grp_autoRemovePeriod[] = L"autoRemovePeriod"; // INT_T
	//! Число дней, через которое администратору посылается сообщение о неактивности компьютера.
    const wchar_t c_grp_notifyPeriod[] = L"notifyPeriod"; // INT_T
	//! Уровень вложенности группы в дереве групп.
    const wchar_t c_grp_level[] = L"level"; // INT_T
	//! Надо ли автоматически инсталлировать антивирус.
    const wchar_t c_grp_autoInstallPackageId[] = L"c_grp_autoInstallPackageId"; // ARRAY_T|INT_T

	//! host moving rule: adsunit
    const wchar_t c_grp_move_rule_adsunit[] = L"grp_mr_adsunit"; // INT_T    

	//! host moving rule: diapason
    const wchar_t c_grp_move_rule_diapason[] = L"grp_mr_diapason"; // INT_T

	//! host moving rule: unassigned group
    const wchar_t c_grp_move_rule_unassigned[] = L"grp_mr_unassigned"; // INT_T
	
    //! group full name
    const wchar_t c_grp_full_name[] = L"grp_full_name"; // STRING_T    
    //! enable full scan
    const wchar_t c_grp_enable_fscan[] = L"grp_enable_fscan"; // BOOL_T
    //!Hosts status "Critical" settings for group.
    const wchar_t c_grp_hoststat_critical[] = KLSRV_HSTSTAT_CRITICAL; //PARAMS_T
    //!Hosts status "Warning" settings for group.
    const wchar_t c_grp_hoststat_warning[] = KLSRV_HSTSTAT_WARNING; //PARAMS_T
    /*! 
        Hosts statuses "Critical" and "Warning" settings for group. 
        These Params has following  values:
            KLSRV_HSTSTAT_INHERITED (BOOL_T) Status settings are inherited from parent
            KLSRV_HSTSTAT_INHERITABLE (BOOL_T) Status settings may be inherited from parent
            KLSRV_HSTSTAT_LOCKED (BOOL_T) Status settings are locked (by policy). 
            KLSRV_HSTSTAT_MASK (INT_T) status mask
            KLSRV_HSTSTAT_SP_VIRUSES (INT_T)
            KLSRV_HSTSTAT_SP_RPT_DIFFERS (INT_T)
            KLSRV_HSTSTAT_OLD_FSCAN (INT_T)
            KLSRV_HSTSTAT_OLD_AV_BASES (INT_T)
            KLSRV_HSTSTAT_OLD_LAST_CONNECT (INT_T)
    */   


	//! Предопределенные идентификаторы специальных групп
	typedef enum{
		GROUPID_ROOT = 0,		//< Группа 'Groups'
		GROUPID_UNASSIGNED = 1,	//< Группа 'Unassigned'
		GROUPID_INVALID_ID = -1	//< Неверный идентификатор группы (например, если отсутствует родительская группа)
	}KLGRP_GROUPID;

	/*!
	* \brief Интерфейс для получения и редактирования информации о логических
	*        группах в Сервере Администрирования.
	*
	*   Группа - это набор компьютеров, объединенных администратором для
	*   удобства управления. 
	*
	*   Группы могут быть вложены в другие группы.
	*/

    class Groups : public KLSTD::KLBaseQI
	{
    public:
    /*!
      \brief Возвращает список логических групп в сети.

        Группы возвращаются в виде контейнера Params, внутренняя структура которого
        повторяет структуру дерева групп. Контейнер содержит внутри одно поле с именем
        groups(определено в c_grp_array_groups), 
		которое представляет собой массив (ARRAY_T) контейнеров Params, внутри 
        каждого из которых содержатся поля 'id', 'name' и 'groups'. Поле 'id' содержит
        идентификатор группы в базе данных, поле 'name' - имя группы, а поле 'groups'
        содержит рекурсивных массив (ARRAY_T) со списком вложенных групп.  Поле 'groups'
        может отсутствовать, если у текущей группы отсутствуют вложенные группы.

        \param groups [out] Список групп в сети.
        \param parent [in]  Идентификатор родительской группы. По умолчанию возвращаются
                            группы, начиная с верхнего уровня.
        \param depth  [in]  Глубина вложенности для получения списка групп.  По умолчанию
                            возвращаются все группы.
       
    */
		virtual void GetGroups ( 
                        KLPAR::Params**	groups,
                        long			parent = GROUPID_ROOT,
                        long			depth = 0 )  = 0;


    /*!
      \brief Возвращает информацию о группе.


        \param id   [in]    Идентификатор группы для получения информации.
        \param info [out]   Контейнер с атрибутами группы. 
		
		\except STDE_NOTFOUND указанная группа не найдена
       
    */
        virtual void GetGroupInfo ( long id, KLPAR::Params ** groups )  = 0;


    /*!
      \brief Добавляет новую группу.
       
        \param info [in] Атрибуты добавляемой группы.
				Могут быть заданы следующие атрибуты:
						Обязательные атрибуты: c_grp_name, c_grp_parentId
						Необязательные атрибуты:
												c_grp_autoAddHost,
												c_grp_autoInstall,
												c_grp_autoRemovePeriod,
												c_grp_notifyPeriod
				Прочие атрибуты игнорируются.
		\return Идентификатор добавленной группы

		\except STDE_NOTFOUND родительская группа не найдена
		\except STDE_EXIST группа с указанным именем уже существует.
    */
        virtual long AddGroup (KLPAR::Params * info)  = 0;


    /*!
      \brief Удаляет группу.
       
        \param id [in] Идентификатор удаляемой группы. 
        
		\except STDE_NOTEMPTY group contains objects that cannot be deleted
    */
        virtual void DelGroup ( long id )  = 0;
		


    /*!
      \brief Модифицирует атрибуты группы.
       
        \param id   [in] Идентификатор изменяемой группы.
        \param info [in] Изменяемые атрибуты группы.
				Изменены могут быть следующие атрибуты:
												c_grp_name,
												c_grp_autoAddHost,
												c_grp_autoInstall,
												c_grp_autoRemovePeriod,
												c_grp_notifyPeriod,
												c_grp_parentId
						Прочие атрибуты игнорируются.

		\except STDE_EXIST группа с указанным именем уже существует.
    */
        virtual void UpdateGroup ( long id, KLPAR::Params * info )  = 0;

    /*
        \brief Устанавливает на начало итератор получения списка компьютеров в группе.

        \param id       [in] Идентификатор группы для получения списка компьютеров.
        \param fields   [in] Список атрибутов компьютеров для получения.
		\param size		[in] Количество атрибутов компьютеров для получения.
        \param lifetime [in] Время жизни выборки компьютеров на сервере (сек) после
                            последнего запроса клиента.
    */

        virtual void ResetGroupHostsIterator ( 
            long				id, 
			const wchar_t**		fields,
			int					size,
            long				lifetime = 20)  = 0;


    /*
        \brief Получает информацию о следующем компьютере в группе.

        \param nCount	[in, out] Количество записей, которое необходимо считать.
									Сюда же запишется количество реально считанных записей.
        \param info     [in] содержит переменную c именем c_grp_array_hosts типа ARRAY_T.
							Элементы этого массива имеют тип PARAMS_T и содержат атрибуты компьютера в группе.
		\return	false, если достигнут конец списка компьютеров.

    */

        virtual bool GetNextGroupHost( long& nCount, KLPAR::Params ** info )  = 0;


    /*!
      \brief Перемещает компьютеры в группу.
       
        \param id       [in] Идентификатор группы для добавления компьютеров.
        \param hosts    [in] Список имен компьютеров в домене.
    */
        virtual void AddHosts ( 
                        long								id, 
                        const std::vector<std::wstring>&	hosts )  = 0;

    /*!
      \brief Удаляет компьютеры из группы. Компьютеры переходят в
            специальную группу Unassigned.
       
        \param id       [in] Идентификатор группы для удаления компьютеров.
        \param hosts    [in] Список имен компьютеров.
    */
        virtual void RemoveHosts ( 
                        const std::vector<std::wstring>&	hosts )  = 0;



    /*!
      \brief Устанавливает список компьютеров для группы.
        Контейнер содержит переменную KLHST::c_hst_array_hosts, содержащую
        массив хостов

       
        \param id       [in] Идентификатор группы для определения списка компьютеров.
        \param hosts    [in] Список имен компьютеров для добавления в группу.
    */
        virtual void SetGroupHosts ( 
                        long						id, 
                        KLPAR::Params*				hosts)  = 0;



        /*!
          \brief	Перемещает хосты в группу по одному из имен или ip-адресу.
            Если введенное имя соответствует формату ip-адреса, то сервер
            пытается найти в базе хост с означенным ip-адресом. В противном
            случае сервер пытается интерпретировать хост следующим образом:
                1. Имя              (c_wks_name)
                2. Отображаемое имя (c_wks_display_name)
                3. Имя NetBIOS      (c_wks_win_name)
                4. Имя DNS          (c_wks_dnsName)

          \param	pInfo - входной контейнер Params - должен содержать переменные:
                        KLHST::c_wks_anyname_array - массив строк - "имен" хостов
                        KLHST::c_wks_groupId - идентификатор группы, в которую
                            означенныехосты должны быть помещены
          \param	pResults - выходной контейнер Params - содержит переменную
                        c_hst_array_results - массив булевых величин, если i-й элемент
                        данного массива равен false, то i-й хост входного массива
                        c_wks_anyname_array не удалось поместить в группу
                        (не удалось разрешить имя).
        */
        virtual void ResolveAndMoveToGroup(
                        KLPAR::Params*  pInfo,
                        KLPAR::Params** pResults) = 0;
        

        /*!
          \brief	Перемещает хосты, находящиеся в корне группы lSrcGroupId
                    в корень группы lDstGroupId. Выполняется асинхронно.

          \param	lSrcGroupId [in] группа-источник
          \param	lDstGroupId [in] группа-назначение
          \param	pSink [in] указатель на KLADMSRV::AdmServerAdviseSink
          \param	hSink [out] идентификатор подписки
        */
        virtual void MoveGroupHosts(
                                long                            lSrcGroupId,
                                long                            lDstGroupId,
                                KLADMSRV::AdmServerAdviseSink*  pSink,
                                KLADMSRV::HSINKID&              hSink) = 0;
        

        /*!
          \brief	Обнуляет счетчик количества найденных вирусов для всех
                    хостов группы и подгрупп. Выполняется асинхронно.

          \param	lGroupId [in] идентификатор группы
          \param	pSink [in] указатель на KLADMSRV::AdmServerAdviseSink
          \param	hSink [out] идентификатор подписки
        */
        virtual void ZeroVirusCount(
                                long                            lGroupId,
                                KLADMSRV::AdmServerAdviseSink*  pSink,
                                KLADMSRV::HSINKID&              hSink) = 0;

        /*!
          \brief	Обнуляет счетчик количества найденных вирусов для
                    указанных хостов. Выполняется асинхронно.

          \param	vecHosts [in] массив хостов
          \param	pSink [in] указатель на KLADMSRV::AdmServerAdviseSink
          \param	hSink [out] идентификатор подписки
        */
        virtual void ZeroVirusCount(
                            const std::vector<std::wstring>&    vecHosts,
                            KLADMSRV::AdmServerAdviseSink*      pSink,
                            KLADMSRV::HSINKID&                  hSink) = 0;

    //OBSOLETE

        inline void AddHosts( 
                            long								id, 
                            const std::wstring&                 domain,
                            const std::vector<std::wstring>&	hosts )
        {
            AddHosts(id, hosts);
        };

        inline void RemoveHosts ( 
                        const std::wstring&                 domain,
                        const std::vector<std::wstring>&	hosts )
        {
            RemoveHosts(hosts);
        }
    };

    //OBSOLETE
	//! Надо ли автоматически инсталлировать антивирус.
    const wchar_t c_grp_autoInstall[] = L"autoInstall"; // BOOL_T

}

/*!
  \brief Создает объект класса KLGRP::Groups

    \param groups      [out] Указатель на объект класса KLGRP::Groups
*/
KLCSSRVP_DECL void KLGRP_CreateGroups(
								   const wchar_t*   szwAddress,
								   KLGRP::Groups ** ppGroups,
								   const unsigned short*  pPorts = NULL,
                                   bool             bUseSsl = true);


#endif // KLGRP_GROUPS_H
