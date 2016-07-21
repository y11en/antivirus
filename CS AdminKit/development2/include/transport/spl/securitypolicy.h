/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file securitypolicy.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Интерфейс класса для хранения прав доступа
 */

#if !defined(KLSPL_SECURITYPOLICY)
#define KLSPL_SECURITYPOLICY

#include <string>
#include <list>
#include <map>

#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <std/par/params.h>
#include <std/tp/threadspool.h>

#include <transport/wat/common.h>

#include <transport/spl/common.h>

#define KLSPL_SEC_PERMISSIONS L"spl-security-permissions"

namespace KLSPL {

	const int c_AllMask = 0xFFFFFFFF;

	class SecurityPolicy : public KLSTD::KLBase
	{
	public:

		/*
			\brief Элемент списка прав доступа
		*/
		class PermissionUnit
		{
		public:			

			PermissionUnit()
			{
				Allow = Deny = 0;
				UserGroupFlag = false;
				Uneraseable = false;
				Readonly = false;				
			}

			PermissionUnit( const std::wstring &userOrUsergroupId, bool userGroupFlag,
				PermissionMask allow, PermissionMask deny, bool readonly = false,
				bool uneraseable = false )
			{				
				UserOrUsergroupId = userOrUsergroupId;
				UserGroupFlag = userGroupFlag;
				Allow = allow;
				Deny = deny;				
				Uneraseable = uneraseable;
				Readonly = readonly;				
			}

			PermissionUnit( const PermissionUnit &p )
			{
				sid = p.sid;
				UserOrUsergroupId = p.UserOrUsergroupId;
				Allow = p.Allow;
				Deny = p.Deny;
				UserGroupFlag = p.UserGroupFlag;
				Uneraseable = p.Uneraseable;
				Readonly = p.Readonly;				
			}

			std::wstring	sid;
			std::wstring	UserOrUsergroupId;
			bool			UserGroupFlag;
			bool			Readonly;
			bool			Uneraseable;
			PermissionMask	Allow;
			PermissionMask	Deny;			
		};

		typedef std::vector<PermissionUnit> PermissionsListType;

		/*
			\brief Права доступа
		*/
		class PermissionsList
		{
			public:

				PermissionsList()
				{
					InheritedFlag = false;
					VisibilityMask = c_AllMask;
				}

				PermissionsList( const PermissionsList &p )
				{
					Permissions = p.Permissions;
					InheritedFlag = p.InheritedFlag;
					VisibilityMask = p.VisibilityMask;
				}

				PermissionsListType	Permissions;
				bool				InheritedFlag;
				PermissionMask		VisibilityMask;
		};

		/*
			\brief Права доступа группы
		*/
		class GroupsPermissions
		{
		public:
			GroupsPermissions() { Allow = Deny = 0; nGroupId = (-1); }

			GroupsPermissions( const GroupsPermissions &gp )
			{
				Allow = gp.Allow;
				Deny = gp.Deny;
				nGroupId = gp.nGroupId;
			}

			PermissionMask	Allow;
			PermissionMask	Deny;
			long			nGroupId;
		};

		typedef std::map<long, GroupsPermissions> GroupsPermissionsMap;
		

		/*
			\brief Иерархия групп ( список идентификаторов родительских групп от корневой группы ) 
		*/
		typedef std::vector<long>	GroupHierarchy;


		/*
			\brief Callback класс для получения информации об иерархии групп 
				и пренадлежности сетевого агента группе
		 */
		class GroupsInfoControl : public KLSTD::KLBaseQI
		{
		public:
			virtual void GetGroupHierarchy( const long &groupId, 
				GroupHierarchy &groupHierarchy ) = 0;

			virtual void GetNagentGroups( const std::wstring &nagentId,
				GroupHierarchy &groupHierarchy ) = 0;

			virtual void GetChildServersGroup( const std::wstring &serverId,
				GroupHierarchy &groupHierarchy ) = 0;

			virtual void GetHostGroups( const std::wstring& wstrHostName,
				GroupHierarchy &groupHierarchy ) = 0;
		};

		/*
			\brief Callback класс для нотификации об изменении прав доступа
		 */
		class Notifier : public KLSTD::KLBaseQI
		{
		public:
			virtual void SecuriryPolicyChangedNotify( const long &groupId ) = 0;
		};

		/*
			\brief Инициализация серверного режима работы
		*/
		virtual void InitServer( const std::wstring &storagePath ) = 0;
		
		/*
			\brief Режим работы
		*/
		virtual bool IsServerMode() = 0;

		/*
			\brief Устанавливает права доступа для группы

			\param groupId [in] Идентификатор группы администрирования ( если empty то установка прав для "корневой" гурппы )
			\param groupHierarchy [in] Иерархия групп для группы groupId
			\param permissions [in] Права доступа

			\exception SPLERR_WRONG_PERMISSIONS_FORMAT
		*/
		virtual void SetPermissions( const long &groupId,
			const PermissionsList &permissions, bool bCheckUserRights = false ) = 0;

		/*
			\brief Получение права доступа для группы

			\param groupId [in] Идентификатор группы администрирования
			\param groupHierarchy [in] Иерархия групп для группы groupId
			\param permissions [out] Права доступа
		*/
		virtual void GetPermissions( const long &groupId, 
			PermissionsList &permissions ) = 0;

		/*
			\brief Получение права доступа для хоста

			\param hostId [in] Идентификатор хоста 
			\param permissions [out] Права доступа
		*/
		virtual void GetPermissions( const std::wstring &hostId, 
			PermissionsList &permissions ) = 0;

		/*
			\brief Удаляет для указанной группу права доступа

			\param groupId [in] Идентификатор группы администрирования
		 */
		virtual void DeleteGroupPermissions( const long &groupId ) = 0;

		/*
			\brief Получение права доступа для текущего пользователя и для указанной группы

			\param authClntContext [in] Контекст пользователя
			\param hostId [in] Идентификатор хоста 
			\param groupHierarchy [in] Иерархия групп для группы groupId			
			\param Allow [out] Маска допустимых операций
			\param Deny [out] Маска запрешенных операций
		*/
		virtual void GetClientContextPermissionForGroup( 
			KLSTD::CAutoPtr<KLWAT::ClientContext> authClntContext, 
			const long &groupId,
			const GroupHierarchy *groupHierarchy, 
			Permissions **ppPerms ) = 0;

		/*
			\brief Получение права доступа для текущего пользователя и для указанного хоста

			\param authClntContext [in] Контекст пользователя
			\param groupId [in] Идентификатор группы администрирования
			\param groupHierarchy [in] Иерархия групп для группы groupId			
			\param Allow [out] Маска допустимых операций
			\param Deny [out] Маска запрешенных операций
		*/
		virtual void GetClientContextPermissionForHost( 
			KLSTD::CAutoPtr<KLWAT::ClientContext> authClntContext, 
			const std::wstring &hostId,
			const GroupHierarchy *groupHierarchy, 
			Permissions **ppPerms ) = 0;

		/*
			\brief Получение права доступа для текущего пользователя и 
			для указанного сетевого агента

			\param authClntContext [in] Контекст пользователя
			\param remoteNagentName [in] Идентификаор сетевого агента ( transport name )
			\param ppPerms [out] Права доступа
		*/
		virtual void GetClientContextPermission(			
			const std::wstring &remoteNagentName, 
			Permissions **ppPerms ) = 0;

		/*
			\brief Получение права доступа для текущего пользователя и 
			для указанного подченненого сервера

			\param authClntContext [in] Контекст пользователя
			\param remoteServerName [in] Идентификаор подченненого сервера ( transport name  )
			\param ppPerms [out] Права доступа			
		*/
		virtual void GetClientContextPermissionChildServer(			
			const std::wstring &remoteServerName, 
			Permissions **ppPerms ) = 0;
		
		/*
			\brief Устанавливает callback класс для работы с иерархией групп

			\param pGroupControl [in] Указатель на класс
		*/
		virtual void SetGroupsInfoControl( GroupsInfoControl *pGroupsInfoControl ) = 0;

		/*
			\brief Функции востановления/сохранения в/из backup'а
		 */
		virtual void GetStorageData( KLPAR::BinaryValue** ppData ) = 0;
		virtual void PutStorageData( KLPAR::BinaryValue* pData ) = 0;

		/*
			\brief Получение списка прав для групп
				Формат списка:
					nGroupId = (-1) права для сервера администрирования и для всх групп, которых нет в списке
					nGroupId = (идентификатор группы) - права для групп, в которых пользователь явным образом задавал права

			\param permsVect [out]
		*/
		virtual void GetGroupsPermissions( KLSTD::CAutoPtr<KLWAT::ClientContext> authClntContext, 
			GroupsPermissionsMap &permsMap ) = 0;


		/*
			\brief Возвращает системный идентификатор пользователся из клиентского контекста 
		*/
		virtual void GetUserSystemIdentifier( KLSTD::CAutoPtr<KLWAT::ClientContext> authClntContext,
			std::wstring &userId, std::vector<int> &customAccessVec ) = 0;


		/*
			\brief Устанавливает callback класс для нотификаций

			\param pNotifier [in] Указатель на класс
		*/
		virtual void SetNotifier( Notifier *pNotifier ) = 0;		
	};


} // namespace KLSPL


KLCSTR_DECL bool KLSPL_CreatePermissions( KLSPL::Permissions **ppPerms, 
			KLSPL::PermissionMask allow, KLSPL::PermissionMask deny,
			std::wstring &userAccountName );

KLCSTR_DECL KLSPL::SecurityPolicy *KLSPL_GetSecurityPolicy();

/*
	\brief Функции инициализаци/деинициализации библиотеки
*/
DECLARE_MODULE_INIT_DEINIT2( KLCSTR_DECL, KLSPL );

#endif // !defined(KLSPL_SECURITYPOLICY)
