/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	avt/accesscheck.h
 * \author	Andrey Lashenkov
 * \date	17.12.2003 19:55:00
 * \brief	Прототипы функций проверки прав доступа.
 * 
 */

#ifndef __KLAVT_ACCESSCHECK_H__
#define __KLAVT_ACCESSCHECK_H__

#include "accesscontrol.h"
//#include "transport/avtl/acllib.h"
#include "kca/prci/componentid.h"

KLCSTR_DECL bool KLAVT_AccessCheckForContext(
                KLWAT::ClientContext*           pContext,
	            const KLAVT::ACE_DECLARATION*   pAcl,
	            long                            nAcl,
                AVP_dword                       dwAccessMask,
                bool                            bThrowExceptions,
				const long						&lGroupId,
				const std::wstring				&hostId,
                const std::wstring*             pwstrBasicAuthLogin);


//!\brief Функция авторизации некоторого вызова, использующая security context из thtread storage.
KLCSTR_DECL bool KLAVT_AccessCheck_InCall(
	        const KLAVT::ACE_DECLARATION*   pAcl,
	        long                            nAcl,
	        AVP_dword dwAccessMask, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	        bool bGenerateException = true, //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false
			const long &lGroupId = (-1),	//!< Идентификатор группы администрирования
			const std::wstring& hostId = L"",
            const std::wstring* pwstrBasicAuthLogin = NULL); //!< Логин basic-аутентификации (должен соответствовать сохраненному в контексте соединения)

//!\brief Функция авторизации некоторого вызова, использующая security context из thtread storage.
KLCSTR_DECL bool KLAVT_AccessCheckForAction_InCall(
	        AVP_dword dwActionGroupID, //!< Идентифиатор логической группы вызовов (см. "ACLDecl.h")
	        AVP_dword dwAccessMasks, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	        bool bGenerateException = true, //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false
            const std::wstring* pwstrBasicAuthLogin = NULL); //!< Логин basic-аутентификации (должен соответствовать сохраненному в контексте соединения)

//!\brief Функция авторизации некоторого вызова, использующая security context из thtread storage.
KLCSTR_DECL bool KLAVT_AccessCheckForActionInGroup_InCall(
	        AVP_dword dwActionGroupID, //!< Идентифиатор логической группы вызовов (см. "ACLDecl.h")
	        AVP_dword dwAccessMasks, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
			const long	&lGroupId,	//!< Идентификатор группы администрирования
	        bool bGenerateException = true, //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false
            const std::wstring* pwstrBasicAuthLogin = NULL); //!< Логин basic-аутентификации (должен соответствовать сохраненному в контексте соединения)

//!\brief Функция авторизации некоторого вызова, использующая security context из thtread storage.
KLCSTR_DECL bool KLAVT_AccessCheckForActionForHost_InCall(
	        AVP_dword dwActionGroupID, //!< Идентифиатор логической группы вызовов (см. "ACLDecl.h")
	        AVP_dword dwAccessMasks, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
			const std::wstring &wstrHostId,	//!< Идентификатор хоста
	        bool bGenerateException = true, //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false
	        const std::wstring* pwstrBasicAuthLogin = NULL); //!< Логин basic-аутентификации (должен соответствовать сохраненному в контексте соединения)

KLCSTR_DECL bool KLAVT_AccessCheckForComponent_InCall(
            AVP_dword                       dwAccessMask,
            const KLPRCI::ComponentId&      id,
            bool                            bGenerateException = true);

/*
bool KLAVT_AccessCheckForComponent_AccessToken(
            KLWAT::AccessTokenHolder*       pToken,
            AVP_dword                       dwAccessMask,
            const KLPRCI::ComponentId&      id,
            bool                            bGenerateException = true);
*/

/*
//!\brief Базовая функция авторизации по массиву ACE_DECLARATION.
bool KLAVT_AccessCheck(
	AVP_dword dwDisiredAccess, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	const KLAVT::ACE_DECLARATION* pAcl, //!< Указатель на массив ACE_DECLARATION (access control entries - см. "ACLDecl.h")
	long nAcl, //!< Число элементов в массиве pAcl
	bool bGenerateException = true); //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false

//!\brief Базовая функция авторизации через интерфейс AccessControlList.
bool KLAVT_AccessCheck(
	AVP_dword dwDisiredAccess, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	KLSTD::CAutoPtr<KLAVT::AccessControlList> pAclObj, //!< Интерфейс AccessControlList
	bool bGenerateException = true); //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false


//!\brief Функция авторизации некоторого вызова, использующая security context из thtread storage.
bool KLAVT_AccessCheckForAction(
	AVP_dword dwActionGroupID, //!< Идентифиатор логической группы вызовов (см. "ACLDecl.h")
	AVP_dword dwDisiredAccess, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	KLSTD::CAutoPtr<AVTL::AclLibrary> pSecLib = NULL, //!< Библиотека-контейнер ACL (по умолчанию для default product)
	KLSTD::CAutoPtr<KLPRCI::SecContext> pSecContext = NULL, //!< Security-context
	const std::wstring* pwstrBasicAuthLogin = NULL, //!< Логин basic-аутентификации (должен соответствовать сохраненному в контексте соединения)
	bool bGenerateException = true); //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false

//!\brief Функция авторизации некоторого вызова, использующая права current thread.
bool KLAVT_AccessCheckForActionDirect(
	AVP_dword dwActionGroupID, //!< Идентифиатор логической группы вызовов (см. "ACLDecl.h")
	AVP_dword dwDisiredAccess, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	KLSTD::CAutoPtr<AVTL::AclLibrary> pSecLib = NULL, //!< Библиотека-контейнер ACL (по умолчанию для default product)
	bool bGenerateException = true); //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false

//!\brief Функция авторизации для компонента, использующая security context из thtread storage.
bool KLAVT_AccessCheckForComponent(
	AVP_dword dwDisiredAccess, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	const KLPRCI::ComponentId& compId, //!< Идентификатор компонента
	KLSTD::CAutoPtr<AVTL::AclLibrary> pSecLib = NULL, //!< Библиотека-контейнер ACL (по умолчанию для default product)
	KLSTD::CAutoPtr<KLPRCI::SecContext> pSecContext = NULL, //!< Security-context
	bool bGenerateException = true); //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false

//!\brief Функция авторизации для компонента, использующая права current thread.
bool KLAVT_AccessCheckForComponentDirect(
	AVP_dword dwDisiredAccess, //!< Битовая маска требуемых прав доступа (см. "ACLDecl.h")
	const KLPRCI::ComponentId& compId, //!< Идентификатор компонента
	KLSTD::CAutoPtr<AVTL::AclLibrary> pSecLib = NULL, //!< Библиотека-контейнер ACL (по умолчанию для default product)
	bool bGenerateException = true); //!< Генерировать ли исключение KLSTD::STDE_NOACCESS, или возвращать false
*/
#endif //__KLAVT_ACCESSCHECK_H__

// Local Variables:
// mode: C++
// End:
