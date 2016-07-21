#if !defined (__AVPKEYIDS_H__)
#define __AVPKEYIDS_H__

#include "Property/Property.h"
#include "AVPComID.h"

// AVPKey IDs and PIDs
#define AVP_PID_KEYROOT_ID								AVP_DC_KEY
#define AVP_PID_KEYROOT									MAKE_AVP_PID(AVP_PID_KEYROOT_ID, AVP_APID_GLOBAL, avpt_nothing, 0)

#define AVP_PID_KEYINFO_ID								(1)
#define AVP_PID_KEYINFO									MAKE_AVP_PID(AVP_PID_KEYINFO_ID, AVP_APID_GLOBAL, avpt_nothing, 0)	

	//Описание ключа
	#define AVP_PID_KEYDESCRIPTION_ID					(1)
	#define AVP_PID_KEYDESCRIPTION						MAKE_AVP_PID(AVP_PID_KEYDESCRIPTION_ID, AVP_DC_KEY, avpt_str, 0)	

	//Версия ключа
	#define AVP_PID_KEYVERSION_ID						(2)
	#define AVP_PID_KEYVERSION							MAKE_AVP_PID(AVP_PID_KEYVERSION_ID, AVP_DC_KEY, avpt_dword, 0)	

	//продолжительность подписки в днях
	#define AVP_PID_KEYLIFESPAN_ID						(3)
	#define AVP_PID_KEYLIFESPAN							MAKE_AVP_PID(AVP_PID_KEYLIFESPAN_ID, AVP_DC_KEY, avpt_dword, 0)	

	//предельная дата жизни ключа
	#define AVP_PID_KEYEXPDATE_ID						(4)
	#define AVP_PID_KEYEXPDATE							MAKE_AVP_PID(AVP_PID_KEYEXPDATE_ID, AVP_DC_KEY, avpt_date, 0)	

	// признак триального ключа ( с 4 версии дублируется в типе лицензии)
	#define AVP_PID_KEYISTRIAL_ID						(5)
	#define AVP_PID_KEYISTRIAL							MAKE_AVP_PID(AVP_PID_KEYISTRIAL_ID, AVP_DC_KEY, avpt_nothing, 0)	

	//Признак ключ имеет тех. поддержку
	#define AVP_PID_KEYHASSUPPORT_ID					(6)
	#define AVP_PID_KEYHASSUPPORT						MAKE_AVP_PID(AVP_PID_KEYHASSUPPORT_ID, AVP_DC_KEY, avpt_nothing, 0)	

	//число лицензий в ключе
	#define AVP_PID_KEYLICENCECOUNT_ID					(7)
	#define AVP_PID_KEYLICENCECOUNT						MAKE_AVP_PID(AVP_PID_KEYLICENCECOUNT_ID, AVP_DC_KEY, avpt_dword, 0)	

	//лицензионная информация в ключе
	#define AVP_PID_KEYLICENCEINFO_ID					(8)
	#define AVP_PID_KEYLICENCEINFO						MAKE_AVP_PID(AVP_PID_KEYLICENCEINFO_ID, AVP_DC_KEY, avpt_str, 0)	

	//информация о тех. поддержке
	#define AVP_PID_KEYSUPPORTINFO_ID					(9)
	#define AVP_PID_KEYSUPPORTINFO						MAKE_AVP_PID(AVP_PID_KEYSUPPORTINFO_ID, AVP_DC_KEY, avpt_str, 0)	

	//серийный номер ключа (IDЗаказчика-IDПрограммы-IDключа)
	#define AVP_PID_KEYSERIALNUMBER_ID					(10)
	#define AVP_PID_KEYSERIALNUMBER						MAKE_AVP_PID(AVP_PID_KEYSERIALNUMBER_ID, AVP_DC_KEY, avpt_dword, 1)	

	//
	#define AVP_PID_KEYVERSIONCHECK_ID					(11)
	#define AVP_PID_KEYVERSIONCHECK						MAKE_AVP_PID(AVP_PID_KEYVERSIONCHECK_ID, AVP_DC_KEY, avpt_dword, 0)	

	/*
	#define AVP_PID_KEYUPDATEWARN_ID					(12)
	#define AVP_PID_KEYUPDATEWARN						MAKE_AVP_PID(AVP_PID_KEYUPDATEWARN_ID, AVP_DC_KEY, avpt_dword, 0)	
	*/

	//название программного продукта ( <название продукта> [ <префикс> <компонента> <суффикс>] )
	#define AVP_PID_KEYPLPOSNAME_ID						(13)
	#define AVP_PID_KEYPLPOSNAME						MAKE_AVP_PID(AVP_PID_KEYPLPOSNAME_ID, AVP_DC_KEY, avpt_str, 0)	

	//Идентификатор программного продукта ( компонента )
	#define AVP_PID_KEYPLPOS_ID							(14)
	#define AVP_PID_KEYPLPOS							MAKE_AVP_PID(AVP_PID_KEYPLPOS_ID, AVP_DC_KEY, avpt_dword, 0)	

	//дата создания ключа
	#define AVP_PID_KEYPRODDATE_ID						(15)
	#define AVP_PID_KEY_PROD_DATE						MAKE_AVP_PID(AVP_PID_KEYPRODDATE_ID, AVP_DC_KEY, avpt_date, 0)	

	//признак обновления
	#define AVP_PID_KEYISUPGRADE_ID						(16)
	#define AVP_PID_KEY_IS_UPGRADE						MAKE_AVP_PID(AVP_PID_KEYISUPGRADE_ID, AVP_DC_KEY, avpt_nothing, 0)	

	// Признак ключ не для продажи (Version 2 - 4)
	#define AVP_PID_KEY_IS_NFR_ID						(17)
	#define AVP_PID_KEY_IS_NFR							MAKE_AVP_PID(AVP_PID_KEY_IS_NFR_ID, AVP_DC_KEY, avpt_nothing, 0)	

	//Признак продажи в OEM (Version 2 or later) ( с 4 версии дублируется в типе лицензии)
	#define AVP_PID_KEY_IS_OEM_ID						(18)
	#define AVP_PID_KEY_IS_OEM							MAKE_AVP_PID(AVP_PID_KEY_IS_OEM_ID, AVP_DC_KEY, avpt_nothing, 0)	

	// Признак продажи в On-line (Version 2 - 4)
	#define AVP_PID_KEY_IS_ONLINE_ID					(19)
	#define AVP_PID_KEY_IS_ONLINE						MAKE_AVP_PID(AVP_PID_KEY_IS_ONLINE_ID, AVP_DC_KEY, avpt_nothing, 0)	

	// Признак наличия скидки (для образовательных, медицинских и государственных учреждений) (Version 2 - 4)
	#define AVP_PID_KEY_IS_DISCOUNT_ID					(20)
	#define AVP_PID_KEY_IS_DISCOUNT						MAKE_AVP_PID(AVP_PID_KEY_IS_DISCOUNT_ID, AVP_DC_KEY, avpt_nothing, 0)	

	//название компании продавца (Version 3 or later)
	#define AVP_PID_KEYCOMPANYNAME_ID					(21)
	#define AVP_PID_KEYCOMPANYNAME						MAKE_AVP_PID(AVP_PID_KEYCOMPANYNAME_ID, AVP_DC_KEY, avpt_str, 0)	

	//Признак персонального ключа и наличия информация о покупателе (Version 3 or later)
	#define AVP_PID_KEYCUSTOMER_ID						(22)
	#define AVP_PID_KEYCUSTOMER							MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_ID, AVP_DC_KEY, avpt_nothing, 0)	


		//название компании (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_COMPANYNAME_ID		(1)
		#define AVP_PID_KEYCUSTOMER_COMPANYNAME			MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_COMPANYNAME_ID, AVP_DC_KEY, avpt_str, 0)	

		// Имя покупателя (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_NAME_ID				(2)
		#define AVP_PID_KEYCUSTOMER_NAME				MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_NAME_ID, AVP_DC_KEY, avpt_str, 0)	

		//страна (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_COUNTRY_ID			(3)
		#define AVP_PID_KEYCUSTOMER_COUNTRY				MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_COUNTRY_ID, AVP_DC_KEY, avpt_str, 0)	

		// город (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_CITY_ID				(4)
		#define AVP_PID_KEYCUSTOMER_CITY				MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_CITY_ID, AVP_DC_KEY, avpt_str, 0)	

		//адрес (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_ADDRESS_ID			(5)
		#define AVP_PID_KEYCUSTOMER_ADDRESS				MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_ADDRESS_ID, AVP_DC_KEY, avpt_str, 0)	

		//телефон (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_PHONE_ID			(6)
		#define AVP_PID_KEYCUSTOMER_PHONE				MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_PHONE_ID, AVP_DC_KEY, avpt_str, 0)	

		//факс (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_FAX_ID				(7)
		#define AVP_PID_KEYCUSTOMER_FAX					MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_FAX_ID, AVP_DC_KEY, avpt_str, 0)	

		//электронная почта (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_EMAIL_ID			(8)
		#define AVP_PID_KEYCUSTOMER_EMAIL				MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_EMAIL_ID, AVP_DC_KEY, avpt_str, 0)	

		//сайт  (Version 3 or later)
		#define AVP_PID_KEYCUSTOMER_WWW_ID				(9)
		#define AVP_PID_KEYCUSTOMER_WWW					MAKE_AVP_PID(AVP_PID_KEYCUSTOMER_WWW_ID, AVP_DC_KEY, avpt_str, 0)	


	//информация о заказе (глобальный уникальный идентификатор заказа) (Version 3 or later)
	#define AVP_PID_KEYREQUESTGIUD_ID					(23)
	#define AVP_PID_KEYREQUESTGIUD						MAKE_AVP_PID(AVP_PID_KEYREQUESTGIUD_ID, AVP_DC_KEY, avpt_str, 0)	

	//информация о обновнении заказа (Version 3 or later)
	#define AVP_PID_KEYPARENTGIUD_ID					(24)
	#define AVP_PID_KEYPARENTGIUD						MAKE_AVP_PID(AVP_PID_KEYPARENTGIUD_ID, AVP_DC_KEY, avpt_str, 0)	

	//Тип лицензии (Version 4 or later)
	//1-Commercial, 2-Beta, 3-Trial, 4-NFR, 5-OEM
	#define AVP_PID_KEYLICENCETYPE_ID					(25)
	#define AVP_PID_KEYLICENCETYPE						MAKE_AVP_PID(AVP_PID_KEYLICENCETYPE_ID, AVP_DC_KEY, avpt_dword, 0)	

	// Список лицензий
	#define AVP_PID_KEYLICENCE_ID						(26)
	#define AVP_PID_KEYLICENCE							MAKE_AVP_PID(AVP_PID_KEYLICENCE_ID, AVP_DC_KEY, avpt_dword, 0)	

	// Идентификатор продукта
	#define AVP_PID_KEY_PRODUCTID_ID					(27)
	#define AVP_PID_KEY_PRODUCTID						MAKE_AVP_PID(AVP_PID_KEY_PRODUCTID_ID, AVP_DC_KEY, avpt_dword, 0)	

	// версия продукта
	#define AVP_PID_KEY_PRODUCTVERSION_ID				(28)
	#define AVP_PID_KEY_PRODUCTVERSION					MAKE_AVP_PID(AVP_PID_KEY_PRODUCTVERSION_ID, AVP_DC_KEY, avpt_str, 0)	

	// Сектор рынка
	#define AVP_PID_KEY_MARCETSECTOR_ID					(29)
	#define AVP_PID_KEY_MARCETSECTOR					MAKE_AVP_PID(AVP_PID_KEY_MARCETSECTOR_ID, AVP_DC_KEY, avpt_dword, 0)	

	// Канал продажи
	#define AVP_PID_KEY_SALESCHANNEL_ID					(30)
	#define AVP_PID_KEY_SALESCHANNEL					MAKE_AVP_PID(AVP_PID_KEY_SALESCHANNEL_ID, AVP_DC_KEY, avpt_dword, 0)	

	// Идентификатор компании дистрибутора
	#define AVP_PID_KEY_PARTNERID_ID					(31)
	#define AVP_PID_KEY_PARTNERID						MAKE_AVP_PID(AVP_PID_KEY_PARTNERID_ID, AVP_DC_KEY, avpt_dword, 0)	

	// название компании дистрибутора
	#define AVP_PID_PARTNERNAME_ID						(32)
	#define AVP_PID_KEY_PARTNERNAME						MAKE_AVP_PID(AVP_PID_PARTNERNAME_ID, AVP_DC_KEY, avpt_str, 0)	

	// номер родительского ключа
	#define AVP_PID_KEY_PARENT_ID						(33)
	#define AVP_PID_KEY_PARENT							MAKE_AVP_PID(AVP_PID_KEY_PARENT_ID, AVP_DC_KEY, avpt_dword, 0)	

	// категория ключа: 1-новый, 2-продление, 3-Дозакупка, 4-Замена
	#define AVP_PID_KEY_KEYCATEGORY_ID					(34)
	#define AVP_PID_KEY_KEYCATEGORY						MAKE_AVP_PID(AVP_PID_KEY_KEYCATEGORY_ID, AVP_DC_KEY, avpt_dword, 0)	

//Список модулей <AppID> с уровнями функциональности <avpt_dword>
//KeyRecords <<AppID>, AVP_APID_GLOBAL, avpt_dword, 0>

#endif
