/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch.h
 * \author Andrew Lashenkov
 * \date	24.12.2004 18:54:00
 * \brief Общие декларации модуля иерархии серверов.
 *
 */

#include <std/base/klbase.h>

#ifndef KLSRVH_SRVHRCH_H
#define KLSRVH_SRVHRCH_H

namespace KLSRVH {

    //! Перечисление возможных состояний подчиненного сервера.
    enum ChildServerStatus {
        CSS_VISIBLE =				0x00000001	//! Сервер включен и присутствует в сети.
    };

    //! Описание базовых атрибутов подчиненного сервера.
	struct child_server_info_t {
		//! Идентификатор подчиненного сервера (генерируется родительским сервером).
        long nId;	

		//! Отображаемое имя сервера.
        std::wstring wstrDisplName;	

		//! Сетевой адрес сервера.
		std::wstring wstrNetAddress;	

		//! Идентификатор группы, в которую включен данный сервер.
        long nGroupId;	

		//! Номер версии подчиненного сервера 
        std::wstring wstrVersion;	

		//! InstanceID подчиненного сервера 
        std::wstring wstrInstanceId;	

		//! Комбинация флагов из ChildServerStatus.
		int nStatus;		
	};


	//! Имена возможных полей в контейнере Params с информацией о подчиненном сервере.

    //! INT_T, Идентификатор подчиненного сервера.
    const wchar_t c_srv_id[] = L"KLSRVH_SRV_ID";

    //! STRING_T, Отображаемое имя сервера
    const wchar_t c_srv_display_name[]	= L"KLSRVH_SRV_DN";

    //! INT_T, Идентификатор группы, в которую включен данный сервер.
    const wchar_t c_srv_group_id[] = L"KLSRVH_SRV_GROUPID";

    //! STRING_T, Сетевой адрес сервера (имя или IP-адрес).
    const wchar_t c_srv_address[] = L"KLSRVH_SRV_ADDR";

	//! STRING_T, Версия подчиненного сервера.
    const wchar_t c_srv_version[] = L"KLSRVH_SRV_VERSION";

	//! STRING_T, Идентификатор экземпляра подчиненного сервера.
    const wchar_t c_srv_instance_id[] = L"KLSRVH_SRV_INST_ID";

    //! BINARY_T, Сертификат сервера.
    const wchar_t c_srv_certificate[] = L"KLSRVH_SRV_CERTIFICATE";
	
    //! STRING_T, MD5-hash публичного ключа сервера.
    const wchar_t c_srv_public_key_hash[] = L"KLSRVH_SRV_PUBLIC_KEY_HASH";
	
    //! INT_T, Текущее состояние сервера из перечисления ChildServerStatus.
    const wchar_t c_srv_status[] = L"KLSRVH_SRV_STATUS";

    //! PARAMS_T, Дополнительный параметры сервера.
    const wchar_t c_srv_params[] = L"KLSRVH_SRV_PARAMS";

    //! STRING_T, Идентификатор хоста подчиненного сервера.
    const wchar_t c_srv_host_guid[] = L"KLSRVH_SRV_HOST_GUID";
	
    //! INT_T, Идентификатор хоста подчиненного сервера.
    const wchar_t c_srv_host_id[] = L"KLSRVH_SRV_HOST_ID";

    //! INT_T, Числовая версия сервера.
    const wchar_t c_srv_build_id[] = L"KLSRVH_SRV_BUILD_ID";
	
	/*
    //! TIME_T, Когда сервер последний раз был доступен в сети.
    const wchar_t c_srv_last_visible[] = L"KLSRVH_SRV_LAST_VISIBLE";

    //! TIME_T, Последнее время обновления информации о сервере.
    const wchar_t c_srv_last_info_update[] = L"KLSRVH_SRV_LAST_INFOUDATE";
	*/
} // namespace KLSRVH

#define KLSRVH_SLAVE_REC_DEPTH L"KLSRVH_SLAVE_REC_DEPTH"

#endif // KLSRVH_SRVHRCH_H
