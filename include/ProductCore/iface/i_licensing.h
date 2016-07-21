// AVP Prague stamp begin( Interface header,  )
// -------- Sunday,  13 May 2007,  17:29 --------
// File Name   -- (null)i_licensing.c
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_licensing__cedce569_253f_4d1f_8597_b71a97dd5fdb )
#define __i_licensing__cedce569_253f_4d1f_8597_b71a97dd5fdb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



#if defined ( __cplusplus )
	struct cSerializable;
#else
	typedef struct tag_cSerializable cSerializable;
#endif


//-------------- Key Add Mode --------------
#define KAM_UNKNOWN     0x0
#define KAM_ADD         0x1
#define KAM_REPLACE     0x2


#define BLACK_LIST_FILE_NAME    "black.lst"


// AVP Prague stamp begin( Interface comment,  )
// Licensing interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_LICENSING  ((tIID)(38009))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_ONLINE_LICENSING 0x1e028874 //

	#define pm_INITING 0x00001000 // (4096) -- инициализация соединения

	#define pm_POSTING_DATA 0x00001001 // (4097) -- отсыл данных на сервер

	#define pm_SAVING_TMP 0x00001002 // (4098) -- сохранение лицензии во временном каталоге

	#define pm_VERIFYING 0x00001003 // (4099) -- проверка лицензии

	#define pm_RETRYING 0x00001004 // (4100) -- повторная попытка получения лицензии

	#define pm_SAVING_DST 0x00001005 // (4101) -- сохранение лицензии в целевом каталоге

	#define pm_POSTING_STAT 0x00001006 // (4102) -- отсылка статуса завершения операции на сервер

	#define pm_DOWNLOAD_COMPLETED 0x00001007 // (4103) -- задача закачки завершена

	#define pm_CONNECTING 0x00001008 // (4104) -- соединение с сервером

	#define pm_RESOLVING 0x00001009 // (4105) -- разрешение IP адреса

	#define pm_SENDING 0x0000100a // (4106) -- отсыл сообщения

	#define pm_SENT 0x0000100b // (4107) -- сообщение послано

	#define pm_RECEIVING 0x0000100c // (4108) -- получение ответа сервера

	#define pm_RECEIVED 0x0000100d // (4109) -- ответа сервера получен

	#define pm_REDIRECTING 0x0000100e // (4110) -- перенаправление

	#define pm_COMPLETED 0x0000100f // (4111) -- задача завершена

	#define pm_GET_PASS_FOR_PROXY 0x00001010 // (4112) --

// message class
#define pmc_LICENSING 0x6124e161 //

	#define pm_DATA_CHANGED 0x00001000 // (4096) --

	#define pm_LICENSE_REPLACE 0x6124e181 // (1629806977) -- request to replace license storage, par_buff=new storage data

	#define pm_CUSTOMER_ID_OBTAINED 0x00001001 // (4097) -- получен CustomerID
	// data content comment
	//    pointer to tUINT customer id
    #define pm_GET_PRODUCT_IDENTIFIER 0x6124e182 // (1629806978) --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
#define warnKEY_ALREADY_IN_USE                   PR_MAKE_DECL_WARN(IID_LICENSING, 0x001) // 0x09479001,155684865 (1) -- Key is already in use
#define errKEY_FILE_NOT_FOUND                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x002) // 0x89479002,-1991798782 (2) -- Key is not found
#define errNOT_SIGNED                            PR_MAKE_DECL_ERR(IID_LICENSING, 0x003) // 0x89479003,-1991798781 (3) -- Digital sign is not found or corrupted
#define errSECURE_DATA_CORRUPT                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x004) // 0x89479004,-1991798780 (4) -- Licensing secure data is corrupted
#define errKEY_CORRUPT                           PR_MAKE_DECL_ERR(IID_LICENSING, 0x005) // 0x89479005,-1991798779 (5) -- Key is corrupted
#define errKEY_INST_EXP                          PR_MAKE_DECL_ERR(IID_LICENSING, 0x006) // 0x89479006,-1991798778 (6) -- Key installation period is expired
#define errNO_ACTIVE_KEY                         PR_MAKE_DECL_ERR(IID_LICENSING, 0x007) // 0x89479007,-1991798777 (7) -- No active key
#define errKEY_IS_INVALID                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x008) // 0x89479008,-1991798776 (8) -- Key is invalid
#define errWRITE_SECURE_DATA                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x009) // 0x89479009,-1991798775 (9) -- Licensing secure data saving failed
#define errREAD_SECURE_DATA                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x00a) // 0x8947900a,-1991798774 (10) -- Licensing secure data reading failed
#define errIO_ERROR                              PR_MAKE_DECL_ERR(IID_LICENSING, 0x00b) // 0x8947900b,-1991798773 (11) -- I/O error
#define errCANNOT_FIND_BASES                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x00c) // 0x8947900c,-1991798772 (12) -- Cannot find antivirus bases
#define errCANT_REG_MORE_KEYS                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x00d) // 0x8947900d,-1991798771 (13) -- Both active and reserve key already registered
#define errNOT_INITIALIZED                       PR_MAKE_DECL_ERR(IID_LICENSING, 0x00e) // 0x8947900e,-1991798770 (14) -- Licensing library is not initialized
#define errINVALID_BASES                         PR_MAKE_DECL_ERR(IID_LICENSING, 0x00f) // 0x8947900f,-1991798769 (15) -- Antivirus bases corrupted or manually updated
#define errDATA_CORRUPTED                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x010) // 0x89479010,-1991798768 (16) -- Data corrupted
#define errCANT_REG_EXPIRED_RESERVE_KEY          PR_MAKE_DECL_ERR(IID_LICENSING, 0x011) // 0x89479011,-1991798767 (17) -- Expired license key cannot be registered as reserve key
#define errSYS_ERROR                             PR_MAKE_DECL_ERR(IID_LICENSING, 0x012) // 0x89479012,-1991798766 (18) -- System error
#define errBLACKLIST_CORRUPTED                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x013) // 0x89479013,-1991798765 (19) -- Black list corrupted
#define errSIGN_DOESNT_CONFIRM_SIGNATURE         PR_MAKE_DECL_ERR(IID_LICENSING, 0x014) // 0x89479014,-1991798764 (20) -- File sign does not confirm given signature
#define errCANT_REG_BETA_KEY_FOR_RELEASE         PR_MAKE_DECL_ERR(IID_LICENSING, 0x015) // 0x89479015,-1991798763 (21) -- Cannot register beta-key for commercial product release
#define errCANT_REG_NON_BETA_KEY_FOR_BETA        PR_MAKE_DECL_ERR(IID_LICENSING, 0x016) // 0x89479016,-1991798762 (22) -- Only beta-key can be registered for beta product release
#define errKEY_WRONG_FOR_PRODUCT                 PR_MAKE_DECL_ERR(IID_LICENSING, 0x017) // 0x89479017,-1991798761 (23) -- This license key is not for this product
#define errKEY_IS_BLACKLISTED                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x018) // 0x89479018,-1991798760 (24) -- License key is blocked by Kaspersky Lab
#define errTRIAL_ALREADY_USED                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x019) // 0x89479019,-1991798759 (25) -- Trial key has been already used on this computer
#define errKEY_IS_CORRUPTED                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x01a) // 0x8947901a,-1991798758 (26) -- License key is corrupted
#define errDIGITAL_SIGN_CORRUPTED                PR_MAKE_DECL_ERR(IID_LICENSING, 0x01b) // 0x8947901b,-1991798757 (27) -- Digital signature not found, corrupted, or does not match the Kaspersky Lab digital signature
#define errCANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY PR_MAKE_DECL_ERR(IID_LICENSING, 0x01c) // 0x8947901c,-1991798756 (28) -- Cannot register expired non-commercial key
#define warnKEY_DOESNT_PROVIDE_SUPPORT           PR_MAKE_DECL_WARN(IID_LICENSING, 0x01d) // 0x0947901d,155684893 (29) -- License key has expired. It provides you full featured product functionality
#define errKEY_CREATION_DATE_INVALID             PR_MAKE_DECL_ERR(IID_LICENSING, 0x01e) // 0x8947901e,-1991798754 (30) -- License key creattion date is invalid. Check system date settings.
#define errCANNOT_INSTALL_SECOND_TRIAL           PR_MAKE_DECL_ERR(IID_LICENSING, 0x01f) // 0x8947901f,-1991798753 (31) -- Cannot register trial key while another trial key is in use
#define errBLACKLIST_CORRUPTED_OR_NOT_FOUND      PR_MAKE_DECL_ERR(IID_LICENSING, 0x020) // 0x89479020,-1991798752 (32) -- Blacklist file is corrupted or not found
#define errUPDATE_DESC_CORRUPTED_OR_NOT_FOUND    PR_MAKE_DECL_ERR(IID_LICENSING, 0x021) // 0x89479021,-1991798751 (33) -- Update description is corrupted or not found
#define errLICENSE_INFO_WRONG_FOR_PRODUCT        PR_MAKE_DECL_ERR(IID_LICENSING, 0x022) // 0x89479022,-1991798750 (34) -- Internal license info wrong for this product
#define errCANNOT_REG_NONVALID_RESERVE_KEY       PR_MAKE_DECL_ERR(IID_LICENSING, 0x023) // 0x89479023,-1991798749 (35) -- Cannot register non-valid reserve key
#define errONLINE_FILLDATA                       PR_MAKE_DECL_ERR(IID_LICENSING, 0x024) // 0x89479024,-1991798748 (36) -- ошибка заполнения формы данными
#define errONLINE_SENDING                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x025) // 0x89479025,-1991798747 (37) -- ошибка посыла http-запроса
#define errONLINE_BADCONTENT                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x026) // 0x89479026,-1991798746 (38) -- в запросе неверные ключевые данные
#define errONLINE_GETSTATUS                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x027) // 0x89479027,-1991798745 (39) -- невозможно получить статус ответа
#define errONLINE_SAVETEMP                       PR_MAKE_DECL_ERR(IID_LICENSING, 0x028) // 0x89479028,-1991798744 (40) -- ошибка при сохранении временного файла
#define errONLINE_SAVEDST                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x029) // 0x89479029,-1991798743 (41) -- ошибка при сохранении целевого файла
#define errONLINE_BADLICENSE                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x02a) // 0x8947902a,-1991798742 (42) -- лицензия не действительна
#define errONLINE_CORRUPTLIC                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x02b) // 0x8947902b,-1991798741 (43) -- получить не испорченную лицензию не удалось
#define errONLINE_CLNT_SYNTHAX                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x02c) // 0x8947902c,-1991798740 (44) -- ошибка синтаксиса в запросе
#define errONLINE_CLNT_UNAUTH                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x02d) // 0x8947902d,-1991798739 (45) -- необходима авторизация
#define errONLINE_CLNT_FORBID                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x02e) // 0x8947902e,-1991798738 (46) -- доступ к ресурсу запрещен
#define errONLINE_CLNT_NOTFND                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x02f) // 0x8947902f,-1991798737 (47) -- ресурс не найден
#define errONLINE_CLNT_BADMETH                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x030) // 0x89479030,-1991798736 (48) -- метод запрещен
#define errONLINE_CLNT_NONACPT                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x031) // 0x89479031,-1991798735 (49) -- не найдено информации требуемого формата
#define errONLINE_CLNT_PRXAUTHRQ                 PR_MAKE_DECL_ERR(IID_LICENSING, 0x032) // 0x89479032,-1991798734 (50) -- требуется прокси-авторизация
#define errONLINE_CLNT_TIMOUT                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x033) // 0x89479033,-1991798733 (51) -- таймаут ожидания запроса
#define errONLINE_CLNT_STCONFL                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x034) // 0x89479034,-1991798732 (52) -- конфликт - необходима доп. информация
#define errONLINE_CLNT_GONE                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x035) // 0x89479035,-1991798731 (53) -- ресурс удален с сервера насовсем
#define errONLINE_CLNT_LENREQ                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x036) // 0x89479036,-1991798730 (54) -- требуется длина содержимого
#define errONLINE_CLNT_PREFAIL                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x037) // 0x89479037,-1991798729 (55) -- вычисление предусловия дало ложь
#define errONLINE_CLNT_TOOLARGE                  PR_MAKE_DECL_ERR(IID_LICENSING, 0x038) // 0x89479038,-1991798728 (56) -- запрос слишком велик
#define errONLINE_CLNT_URILONG                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x039) // 0x89479039,-1991798727 (57) -- URI слишком длинный
#define errONLINE_CLNT_UNSMEDIA                  PR_MAKE_DECL_ERR(IID_LICENSING, 0x03a) // 0x8947903a,-1991798726 (58) -- не поддерживаемый формат данных
#define errONLINE_CLNT_OTHER                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x03b) // 0x8947903b,-1991798725 (59) -- неспециф. ошибка на стороне клиента
#define errONLINE_SERV_INT                       PR_MAKE_DECL_ERR(IID_LICENSING, 0x03c) // 0x8947903c,-1991798724 (60) -- внутренняя ошибка сервера
#define errONLINE_SERV_NSUP                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x03d) // 0x8947903d,-1991798723 (61) -- функциональность не поддерживается
#define errONLINE_SERV_BADGATE                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x03e) // 0x8947903e,-1991798722 (62) -- некорректный ответ от шлюза
#define errONLINE_SERV_UNVL                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x03f) // 0x8947903f,-1991798721 (63) -- ресурс временно недоступен
#define errONLINE_SERV_GATIMO                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x040) // 0x89479040,-1991798720 (64) -- таймаут ожидания ответа от шлюза
#define errONLINE_SERV_PROT                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x041) // 0x89479041,-1991798719 (65) -- протокол не поддерживается сервером
#define errONLINE_SERV_OTHER                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x042) // 0x89479042,-1991798718 (66) -- неспециф. ошибка на стороне сервера
#define errONLINE_HTTP_OTHER                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x043) // 0x89479043,-1991798717 (67) -- неспециф. ошибка http
#define errONLINE_BAD_RID                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x044) // 0x89479044,-1991798716 (68) -- некорректный идентификатор ресурса
#define errONLINE_BAD_KEY                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x045) // 0x89479045,-1991798715 (69) -- неверные данные в поле "ключ"
#define errONLINE_BAD_URL                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x046) // 0x89479046,-1991798714 (70) -- некорректный URL
#define errONLINE_BAD_FOLDER                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x047) // 0x89479047,-1991798713 (71) -- некорректный целевой каталог
#define errONLINE_MEM_ALLOC                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x048) // 0x89479048,-1991798712 (72) -- ошибка выделения памяти
#define errONLINE_CONV_ANSI                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x049) // 0x89479049,-1991798711 (73) -- ошибка конвертации параметров в ansi строку (url, folder, agent)
#define errONLINE_THR_CREATE                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x04a) // 0x8947904a,-1991798710 (74) -- ошибка создания рабочего потока
#define errONLINE_THR_STARTED                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x04b) // 0x8947904b,-1991798709 (75) -- рабочий поток уже запущен
#define errONLINE_THR_STOPPED                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x04c) // 0x8947904c,-1991798708 (76) -- рабочий поток не запущен
#define errONLINE_KEYNOTFOUND                    PR_MAKE_DECL_ERR(IID_LICENSING, 0x04d) // 0x8947904d,-1991798707 (77) -- лицензионный ключ не найден на сервере активации
#define errONLINE_BLOCKED                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x04e) // 0x8947904e,-1991798706 (78) -- лицензионный ключи или код активации заблокирован
#define errONLINE_SRVERROR                       PR_MAKE_DECL_ERR(IID_LICENSING, 0x04f) // 0x8947904f,-1991798705 (79) -- внутренняя ошибка сервера активации
#define errONLINE_MALFORMEDREQ                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x050) // 0x89479050,-1991798704 (80) -- недостаточно данных в запросе на акитвацию
#define errONLINE_WRONGCUSTORPWD                 PR_MAKE_DECL_ERR(IID_LICENSING, 0x051) // 0x89479051,-1991798703 (81) -- неверный ID пользователя или пароль
#define errONLINE_WRONACTGCODE                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x052) // 0x89479052,-1991798702 (82) -- неверный код активации
#define errKEY_INST_FUTURE_EXP                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x053) // 0x89479053,-1991798701 (83) --
#define errONLINE_BADLOCALDATE                   PR_MAKE_DECL_ERR(IID_LICENSING, 0x054) // 0x89479054,-1991798700 (84) --
#define errTRIAL_PERIOD_OVER                     PR_MAKE_DECL_ERR(IID_LICENSING, 0x055) // 0x89479055,-1991798699 (85) -- триальный период истек
#define errPERIOD_EXPIRED                        PR_MAKE_DECL_ERR(IID_LICENSING, 0x056) // 0x89479056,-1991798698 (86) -- период активации истек
#define errNO_MORE_ATTEMPTS                      PR_MAKE_DECL_ERR(IID_LICENSING, 0x057) // 0x89479057,-1991798697 (87) -- количество попыток активации исчерпано
#define errUNKNOWN_OLA_SERVER_ERROR              PR_MAKE_DECL_ERR(IID_LICENSING, 0x058) // 0x89479058,-1991798696 (88) -- Cервер активации вернул неизвестную ошибку
#define errKEY_CONFLICTS_WITH_COMMERCIAL         PR_MAKE_DECL_ERR(IID_LICENSING, 0x059) // 0x89479059,-1991798695 (89) --
#define errOLA_INTERNAL_SERVER_ERROR             PR_MAKE_DECL_ERR(IID_LICENSING, 0x05a) // 0x8947905a,-1991798694 (90) -- Internal server error
#define errOLA_BLOCKED                           PR_MAKE_DECL_ERR(IID_LICENSING, 0x05b) // 0x8947905b,-1991798693 (91) -- ключ или код активации заблокирован
#define errOLA_ACT_CODE_OR_EDITION_ID_REQUIRED   PR_MAKE_DECL_ERR(IID_LICENSING, 0x05c) // 0x8947905c,-1991798692 (92) -- в запросе вообще нет кода активации или тиража
#define errOLA_WRONG_CUSTOMER_ID_OR_PASSWORD     PR_MAKE_DECL_ERR(IID_LICENSING, 0x05d) // 0x8947905d,-1991798691 (93) -- Wrong CustomerID or password (ошибка авторизации)
#define errOLA_WRONG_ACTIVATION_CODE             PR_MAKE_DECL_ERR(IID_LICENSING, 0x05e) // 0x8947905e,-1991798690 (94) -- Wrong Activation Code (неправильный код - нет такого)
#define errOLA_ACTIVATION_CODE_REQUIRED          PR_MAKE_DECL_ERR(IID_LICENSING, 0x05f) // 0x8947905f,-1991798689 (95) -- Activation Code required (прислали тираж, к которому обязательно требуется код активации)
#define errOLA_ACTIVATION_PERIOD_EXPIRED         PR_MAKE_DECL_ERR(IID_LICENSING, 0x060) // 0x89479060,-1991798688 (96) -- Activation period expired (установлено ограничение по времени на повторные активации)
#define errOLA_QUANTITY_OF_ATTEMPTS_EXHAUSTED    PR_MAKE_DECL_ERR(IID_LICENSING, 0x061) // 0x89479061,-1991798687 (97) -- установлено ограничение на кол-во повторных активаций одного кода
#define errOLA_NO_CONNECTION_WITH_SERVER         PR_MAKE_DECL_ERR(IID_LICENSING, 0x062) // 0x89479062,-1991798686 (98) --
#define errOLA_WRONG_APPLICATION_ID              PR_MAKE_DECL_ERR(IID_LICENSING, 0x063) // 0x89479063,-1991798685 (99) -- для такого APP_ID нет ключей, включая совместимые
#define errOLA_SERVER_NOT_AVAILABLE              PR_MAKE_DECL_ERR(IID_LICENSING, 0x064) // 0x89479064,-1991798684 (100) -- Сервер не доступен (возможно нет доступа в Интренет)
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hLICENSING;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iLicensingVtbl;
typedef struct iLicensingVtbl iLicensingVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cLicensing;
	typedef cLicensing* hLICENSING;
#else
	typedef struct tag_hLICENSING 
	{
		const iLicensingVtbl* vtbl; // pointer to the "Licensing" virtual table
		const iSYSTEMVtbl*    sys;  // pointer to the "SYSTEM" virtual table
	} *hLICENSING;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Licensing_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpLicensing_GetActiveKeyInfo)       ( hLICENSING _this, cSerializable* pResult ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_GetInstalledKeysInfo)   ( hLICENSING _this, cSerializable* pResult ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_RevokeActiveKey)        ( hLICENSING _this );        // -- ;
	typedef   tERROR (pr_call *fnpLicensing_RevokeKey)              ( hLICENSING _this, cSerializable* pKeySerialNumber ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_CheckKeyFile)           ( hLICENSING _this, tWSTRING szKeyFileName, tDWORD dwKeyAddMode, cSerializable* pResult ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_CheckActiveKey)         ( hLICENSING _this, cSerializable* pResult ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_AddKey)                 ( hLICENSING _this, tWSTRING szFileName, tDWORD dwKeyAddMode ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_AddOnlineKey)           ( hLICENSING _this, cSerializable* pUserData, tDWORD dwKeyAddMode, cSerializable* pResult, tDWORD* pdwOLASrvErr ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_StopOnlineLoadKey)      ( hLICENSING _this );        // -- ;
	typedef   tERROR (pr_call *fnpLicensing_ClearTrialLimitDate)    ( hLICENSING _this );        // -- ;
	typedef   tERROR (pr_call *fnpLicensing_GetInfo)                ( hLICENSING _this, cSerializable* pInfo ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_AddOLAKey)              ( hLICENSING _this, cSerializable* pUserData, tDWORD dwKeyAddMode, hSTRING pResForm, cSerializable* pResult, tDWORD* pdwOLAErrCode, tDWORD* pdwOLAErrCodeSpecific ); // -- Initiate activation of a key using OLA library;
	typedef   tERROR (pr_call *fnpLicensing_SubmitOLAForm)          ( hLICENSING _this, cSerializable* pForm, tDWORD dwKeyAddMode, hSTRING pResForm, cSerializable* pResult, tDWORD* pdwOLAErrCode, tDWORD* pdwOLAErrCodeSpecific ); // -- ;
	typedef   tERROR (pr_call *fnpLicensing_StopOLA)                ( hLICENSING _this );        // -- ;
	typedef   tERROR (pr_call *fnpLicensing_GetCustomerCredentials) ( hLICENSING _this, hSTRING strCustomerId, hSTRING strCustomerPassword ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iLicensingVtbl 
{
	fnpLicensing_GetActiveKeyInfo        GetActiveKeyInfo;
	fnpLicensing_GetInstalledKeysInfo    GetInstalledKeysInfo;
	fnpLicensing_RevokeActiveKey         RevokeActiveKey;
	fnpLicensing_RevokeKey               RevokeKey;
	fnpLicensing_CheckKeyFile            CheckKeyFile;
	fnpLicensing_CheckActiveKey          CheckActiveKey;
	fnpLicensing_AddKey                  AddKey;
	fnpLicensing_AddOnlineKey            AddOnlineKey;
	fnpLicensing_StopOnlineLoadKey       StopOnlineLoadKey;
	fnpLicensing_ClearTrialLimitDate     ClearTrialLimitDate;
	fnpLicensing_GetInfo                 GetInfo;
	fnpLicensing_AddOLAKey               AddOLAKey;
	fnpLicensing_SubmitOLAForm           SubmitOLAForm;
	fnpLicensing_StopOLA                 StopOLA;
	fnpLicensing_GetCustomerCredentials  GetCustomerCredentials;
}; // "Licensing" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Licensing_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgSTORAGE           mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001000 )
#define pgBASES_PATH        mPROPERTY_MAKE_GLOBAL( pTYPE_WSTRING , 0x00001001 )
#define pgTM                mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00001002 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Licensing_GetActiveKeyInfo( _this, pResult )                                                                                 ((_this)->vtbl->GetActiveKeyInfo( (_this), pResult ))
	#define CALL_Licensing_GetInstalledKeysInfo( _this, pResult )                                                                             ((_this)->vtbl->GetInstalledKeysInfo( (_this), pResult ))
	#define CALL_Licensing_RevokeActiveKey( _this )                                                                                           ((_this)->vtbl->RevokeActiveKey( (_this) ))
	#define CALL_Licensing_RevokeKey( _this, pKeySerialNumber )                                                                               ((_this)->vtbl->RevokeKey( (_this), pKeySerialNumber ))
	#define CALL_Licensing_CheckKeyFile( _this, szKeyFileName, dwKeyAddMode, pResult )                                                        ((_this)->vtbl->CheckKeyFile( (_this), szKeyFileName, dwKeyAddMode, pResult ))
	#define CALL_Licensing_CheckActiveKey( _this, pResult )                                                                                   ((_this)->vtbl->CheckActiveKey( (_this), pResult ))
	#define CALL_Licensing_AddKey( _this, szFileName, dwKeyAddMode )                                                                          ((_this)->vtbl->AddKey( (_this), szFileName, dwKeyAddMode ))
	#define CALL_Licensing_AddOnlineKey( _this, pUserData, dwKeyAddMode, pResult, pdwOLASrvErr )                                              ((_this)->vtbl->AddOnlineKey( (_this), pUserData, dwKeyAddMode, pResult, pdwOLASrvErr ))
	#define CALL_Licensing_StopOnlineLoadKey( _this )                                                                                         ((_this)->vtbl->StopOnlineLoadKey( (_this) ))
	#define CALL_Licensing_ClearTrialLimitDate( _this )                                                                                       ((_this)->vtbl->ClearTrialLimitDate( (_this) ))
	#define CALL_Licensing_GetInfo( _this, pInfo )                                                                                            ((_this)->vtbl->GetInfo( (_this), pInfo ))
	#define CALL_Licensing_AddOLAKey( _this, pUserData, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific )               ((_this)->vtbl->AddOLAKey( (_this), pUserData, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific ))
	#define CALL_Licensing_SubmitOLAForm( _this, pForm, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific )               ((_this)->vtbl->SubmitOLAForm( (_this), pForm, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific ))
	#define CALL_Licensing_StopOLA( _this )                                                                                                   ((_this)->vtbl->StopOLA( (_this) ))
	#define CALL_Licensing_GetCustomerCredentials( _this, strCustomerId, strCustomerPassword )                                                ((_this)->vtbl->GetCustomerCredentials( (_this), strCustomerId, strCustomerPassword ))
#else // if !defined( __cplusplus )
	#define CALL_Licensing_GetActiveKeyInfo( _this, pResult )                                                                                 ((_this)->GetActiveKeyInfo( pResult ))
	#define CALL_Licensing_GetInstalledKeysInfo( _this, pResult )                                                                             ((_this)->GetInstalledKeysInfo( pResult ))
	#define CALL_Licensing_RevokeActiveKey( _this )                                                                                           ((_this)->RevokeActiveKey( ))
	#define CALL_Licensing_RevokeKey( _this, pKeySerialNumber )                                                                               ((_this)->RevokeKey( pKeySerialNumber ))
	#define CALL_Licensing_CheckKeyFile( _this, szKeyFileName, dwKeyAddMode, pResult )                                                        ((_this)->CheckKeyFile( szKeyFileName, dwKeyAddMode, pResult ))
	#define CALL_Licensing_CheckActiveKey( _this, pResult )                                                                                   ((_this)->CheckActiveKey( pResult ))
	#define CALL_Licensing_AddKey( _this, szFileName, dwKeyAddMode )                                                                          ((_this)->AddKey( szFileName, dwKeyAddMode ))
	#define CALL_Licensing_AddOnlineKey( _this, pUserData, dwKeyAddMode, pResult, pdwOLASrvErr )                                              ((_this)->AddOnlineKey( pUserData, dwKeyAddMode, pResult, pdwOLASrvErr ))
	#define CALL_Licensing_StopOnlineLoadKey( _this )                                                                                         ((_this)->StopOnlineLoadKey( ))
	#define CALL_Licensing_ClearTrialLimitDate( _this )                                                                                       ((_this)->ClearTrialLimitDate( ))
	#define CALL_Licensing_GetInfo( _this, pInfo )                                                                                            ((_this)->GetInfo( pInfo ))
	#define CALL_Licensing_AddOLAKey( _this, pUserData, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific )               ((_this)->AddOLAKey( pUserData, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific ))
	#define CALL_Licensing_SubmitOLAForm( _this, pForm, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific )               ((_this)->SubmitOLAForm( pForm, dwKeyAddMode, pResForm, pResult, pdwOLAErrCode, pdwOLAErrCodeSpecific ))
	#define CALL_Licensing_StopOLA( _this )                                                                                                   ((_this)->StopOLA( ))
	#define CALL_Licensing_GetCustomerCredentials( _this, strCustomerId, strCustomerPassword )                                                ((_this)->GetCustomerCredentials( strCustomerId, strCustomerPassword ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iLicensing 
	{
		virtual tERROR pr_call GetActiveKeyInfo( cSerializable* pResult ) = 0;
		virtual tERROR pr_call GetInstalledKeysInfo( cSerializable* pResult ) = 0;
		virtual tERROR pr_call RevokeActiveKey() = 0;
		virtual tERROR pr_call RevokeKey( cSerializable* pKeySerialNumber ) = 0;
		virtual tERROR pr_call CheckKeyFile( tWSTRING szKeyFileName, tDWORD dwKeyAddMode, cSerializable* pResult ) = 0;
		virtual tERROR pr_call CheckActiveKey( cSerializable* pResult ) = 0;
		virtual tERROR pr_call AddKey( tWSTRING szFileName, tDWORD dwKeyAddMode ) = 0;
		virtual tERROR pr_call AddOnlineKey( cSerializable* pUserData, tDWORD dwKeyAddMode, cSerializable* pResult, tDWORD* pdwOLASrvErr ) = 0;
		virtual tERROR pr_call StopOnlineLoadKey() = 0;
		virtual tERROR pr_call ClearTrialLimitDate() = 0;
		virtual tERROR pr_call GetInfo( cSerializable* pInfo ) = 0;
		virtual tERROR pr_call AddOLAKey( cSerializable* pUserData, tDWORD dwKeyAddMode, hSTRING pResForm, cSerializable* pResult, tDWORD* pdwOLAErrCode, tDWORD* pdwOLAErrCodeSpecific ) = 0; // -- Initiate activation of a key using OLA library
		virtual tERROR pr_call SubmitOLAForm( cSerializable* pForm, tDWORD dwKeyAddMode, hSTRING pResForm, cSerializable* pResult, tDWORD* pdwOLAErrCode, tDWORD* pdwOLAErrCodeSpecific ) = 0;
		virtual tERROR pr_call StopOLA() = 0;
		virtual tERROR pr_call GetCustomerCredentials( hSTRING strCustomerId, hSTRING strCustomerPassword ) = 0;
	};

	struct pr_abstract cLicensing : public iLicensing, public iObj 
	{

		OBJ_IMPL( cLicensing );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hLICENSING()   { return (hLICENSING)this; }

		hOBJECT pr_call get_pgSTORAGE() { return (hOBJECT)getObj(pgSTORAGE); }
		tERROR pr_call set_pgSTORAGE( hOBJECT value ) { return setObj(pgSTORAGE,(hOBJECT)value); }

		tERROR pr_call get_pgBASES_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgBASES_PATH,buff,size,cp); }
		tERROR pr_call set_pgBASES_PATH( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgBASES_PATH,buff,size,cp); }

		hOBJECT pr_call get_pgTM() { return (hOBJECT)getObj(pgTM); }
		tERROR pr_call set_pgTM( hOBJECT value ) { return setObj(pgTM,(hOBJECT)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_licensing__cedce569_253f_4d1f_8597_b71a97dd5fdb
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(Licensing_PROXY_DEFINITION)
#define Licensing_PROXY_DEFINITION

PR_PROXY_BEGIN(Licensing)
	PR_PROXY(Licensing, GetActiveKeyInfo,     PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(Licensing, GetInstalledKeysInfo, PR_ARG1(tid_SERIALIZABLE))
	PR_PROXY(Licensing, RevokeActiveKey,      PR_ARG0())
	PR_PROXY(Licensing, RevokeKey,            PR_ARG1(tid_SERIALIZABLE))
	PR_SCPRX(Licensing, CheckKeyFile,         PR_ARG3(tid_WSTRING,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(Licensing, CheckActiveKey,       PR_ARG1(tid_SERIALIZABLE))
	PR_SCPRX(Licensing, AddKey,               PR_ARG2(tid_WSTRING,tid_DWORD))
	PR_SCPRX(Licensing, AddOnlineKey,         PR_ARG3(tid_SERIALIZABLE,tid_DWORD,tid_SERIALIZABLE))
	PR_PROXY(Licensing, StopOnlineLoadKey,    PR_ARG0())
    PR_PROXY(Licensing, GetInfo,              PR_ARG1(tid_SERIALIZABLE))
	PR_SCPRX(Licensing, AddOLAKey,            PR_ARG6(tid_SERIALIZABLE, tid_DWORD, tid_OBJECT, tid_SERIALIZABLE, tid_DWORD|tid_POINTER, tid_DWORD|tid_POINTER))
	PR_SCPRX(Licensing, SubmitOLAForm,        PR_ARG6(tid_SERIALIZABLE, tid_DWORD, tid_OBJECT, tid_SERIALIZABLE, tid_DWORD|tid_POINTER, tid_DWORD|tid_POINTER))
	PR_PROXY(Licensing, StopOLA,              PR_ARG0())
	PR_PROXY(Licensing, GetCustomerCredentials,PR_ARG2(tid_OBJECT, tid_OBJECT))
PR_PROXY_END(Licensing, IID_LICENSING)

#endif // __PROXY_DECLARATION
