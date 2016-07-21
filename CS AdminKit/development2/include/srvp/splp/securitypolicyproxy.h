/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file securitypolicyproxy.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Прокси интерфейса для хранения прав доступа
 */


#if !defined(KLSPLP_SECURITYPOLICYPROXY)
#define KLSPLP_SECURITYPOLICYPROXY

#include <std/base/klbase.h>
#include <transport/spl/securitypolicy.h>

namespace KLSPL
{
	class KLSTD_NOVTABLE SecurityPolicyProxy : public KLSTD::KLBaseQI 
	{
    public:

		/*!
			\brief Возвращает список прав доступа к серверу администрирования

			\param permsList [out] Список прав
		*/
		virtual void GetServerPermissions( KLSPL::SecurityPolicy::PermissionsList &permsList ) = 0;

		/*!
			\brief Устанавливает права доступа к серверу администрирования

			\param permsList [in] Список прав
			\param bCheckCurrentUserRights [in] Флаг проверки прав текущего пользователя после изменения прав доступа. Если после изменения 
					данный пользователь потеряет права на запись в данную групп то будет выбрашено исключение KLSPL::SPLERR_USER_WILL_LOSE_WRITE_RIGHTS
		*/
		virtual void SetServerPermissions( const KLSPL::SecurityPolicy::PermissionsList &permsList, 
			bool bCheckCurrentUserRights = false ) = 0;


		/*!
			\brief Возвращает список прав доступа к группе администрирования

			\param groupId [in] Идентификатор группы администрирования
			\param permsList [out] Список прав
		*/
		virtual void GetGroupPermissions( const long groupId, 
			KLSPL::SecurityPolicy::PermissionsList &permsList ) = 0;

		/*!
			\brief Устанавливает права доступа к группе администрирования

			\param groupId [in] Идентификатор группы администрирования
			\param permsList [in] Список прав
			\param bCheckCurrentUserRights [in] Флаг проверки прав текущего пользователя после изменения прав доступа. Если после изменения 
					данный пользователь потеряет права на запись в данную групп то будет выбрашено исключение KLSPL::SPLERR_USER_WILL_LOSE_WRITE_RIGHTS
		*/
		virtual void SetGroupPermissions( const long groupId, 
			const KLSPL::SecurityPolicy::PermissionsList &permsList, 
			bool bCheckCurrentUserRights = false ) = 0;

	};
}

#endif // !defined(KLSPLP_SECURITYPOLICYPROXY)