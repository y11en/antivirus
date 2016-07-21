// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  10 August 2005,  09:55 --------
// File Name   -- (null)i_engine.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_engine__3586a210_b2c2_4130_b9f4_59d900730341 )
#define __i_engine__3586a210_b2c2_4130_b9f4_59d900730341
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// Engine interface implementation
// Short comments -- Базовый интерфейс обработки объектов.//eng:Generic interface for object processing.
//    Интерфейс определяет основные правила и методы обработки сложных объектов.
//    Служит для реализации как абстрактного антивирусного ядра, так и подключаемых сканеров.
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_ENGINE  ((tIID)(62005))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )

// npENGINE
// Расширение именованных свойств обрабатываемого объекта.//eng:Named property extension for object under processing.
// Определение текстовых имен идентификаторов свойств дла регистрации
#define npENGINE_VIRTUAL_OBJECT_NAME   ((tSTRING)("npENGINE_VIRTUAL_OBJECT_NAME")) // Имя строкового свойства "виртуальное имя объекта".//eng:Name of string property "Virtual object name".
#define npENGINE_INTEGRAL_PARENT_IO_hOBJECT ((tSTRING)("npENGINE_INTEGRAL_PARENT_IO")) // Имя свойства "неотьемлемый родитель"
#define npENGINE_OBJECT_READONLY_tBOOL ((tSTRING)("npENGINE_OBJECT_READONLY")) // Имя BOOL свойства "запись в объект невозможна".//eng:Name of BOOL property "Object readonly".
#define npENGINE_OBJECT_DETECT_STATE_tDWORD ((tSTRING)("npENGINE_OBJECT_DETECT_STATE")) // Имя DWORD счетчика инфицированности (с учетом лечения и удаления)
#define npENGINE_OBJECT_DETECT_COUNTER_tDWORD ((tSTRING)("npENGINE_OBJECT_DETECT_COUNTER_tDWORD")) // Имя DWORD счетчика обнаружений
#define npENGINE_OBJECT_SESSION_hOBJECT ((tSTRING)("npENGINE_OBJECT_SESSION_hOBJECT")) // Имя свойства "сессионный объект"
#define npENGINE_OBJECT_READONLY_tERROR ((tSTRING)("npENGINE_OBJECT_READONLY_tERROR")) // Имя tQWORD свойства "запись в объект невозможна по причине...".
#define npENGINE_OBJECT_READONLY_hOBJECT ((tSTRING)("npENGINE_OBJECT_READONLY_hOBJECT")) // Имя tQWORD свойства "запись в объект невозможна по причине...".
#define npENGINE_OBJECT_PARAM_ACTION_CLASS_MASK_tDWORD ((tSTRING)("npENGINE_OBJECT_PARAM_ACTION_CLASS_MASK_tDWORD")) // Имя tDWORD параметра  "маска классов действий".
#define npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME ((tSTRING)("npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME")) // Имя параметра  "имя обнаруженного преобразователя".
#define npENGINE_OBJECT_TRANSFORMER_NAME ((tSTRING)("npENGINE_OBJECT_TRANSFORMER_NAME")) // Имя параметра  "имя преобразователя".
#define npENGINE_OBJECT_EXECUTABLE_PARENT_IO_hOBJECT ((tSTRING)("npENGINE_OBJECT_EXECUTABLE_PARENT_IO_hOBJECT")) // Имя свойства "исполняемый родитель".
#define npENGINE_OBJECT_SKIP_THIS_ONE_tBOOL ((tSTRING)("npENGINE_OBJECT_SKIP_THIS_ONE_tBOOL")) // Имя свойства "пропустить обработку этого объекта".
#define npENGINE_OBJECT_SET_WRITE_ACCESS_tERROR ((tSTRING)("npENGINE_OBJECT_SET_WRITE_ACCESS_tERROR")) // Имя свойства "ошибка переоткрытия на запись".

// ENGINE_DETECT
// Идентификаторы рабочих данных, ассоциированых с объектом.//eng:Associated with object info IDs.
// Ассоциированные данные доступны через метод GetAssociatedInfo во время обработки сообщений.
// //eng:Associated data are available through GetAssociatedInfo method during message processing.
#define ENGINE_DETECT_NAME             ((tDWORD)(2)) // Имя детектирования.//eng:Detection name.
#define ENGINE_DETECT_DANGER           ((tDWORD)(3)) // Флаги опасности.//eng:Danger flags.
#define ENGINE_DETECT_TYPE             ((tDWORD)(4)) // Тип детектирования.//eng:Detection type.
#define ENGINE_DETECT_BEHAVIOUR        ((tDWORD)(5)) // Тип детектирования.//eng:Detection type.
#define ENGINE_DETECT_DISINFECTABILITY ((tDWORD)(6)) // Возможность удаления.//eng:Disinfectability.
#define ENGINE_DETECT_CERTANITY        ((tDWORD)(7)) // Уровень уверенности детектирования.//eng:Detection certanity.
#define ENGINE_OBTAINED_OS_NAME        ((tDWORD)(100)) // Имя обнаруженной объектной системы.//eng:Obtained object system name.
#define ENGINE_REOPEN_DATA             ((tDWORD)(101)) // Данные для прямого повторного открытия объекта и проверки.

// ENGINE_EXT
// Идентификаторы запроса расширенных свойств.//eng:Extended info IDs.
// Расширеные свойства hENGINE доступны через метод GetExtendedInfo.
// //eng:Extended properties of hENGINE are available through GetExtendedInfo method.
#define ENGINE_EXT_DETECT_TYPE_BIT_NAME_ANSI ((tDWORD)(1)) // Имя типа детектирования.//eng:Detection type name.

// ENGINE_DETECT_TYPE
// Битовые маски типов для ассоциированной переменной ENGINE_DETECT_TYPE.//eng: Type bit masks for ENGINE_DETECT_TYPE.
// Тип детектирования. Возможно наличие сразу нескольких типов.
// Если тип детектирования не соответствует документированным, имя типа детектирования можно запросить через метод GetExtendedInfo.
// //eng:Detection type.
//
#define ENGINE_DETECT_TYPE_UNDEFINED   ((tDWORD)(0)) //
#define ENGINE_DETECT_TYPE_VIRWARE     ((tDWORD)(1)) //
#define ENGINE_DETECT_TYPE_TROJWARE    ((tDWORD)(2)) //
#define ENGINE_DETECT_TYPE_MALWARE     ((tDWORD)(3)) //
#define ENGINE_DETECT_TYPE_ADWARE      ((tDWORD)(4)) //
#define ENGINE_DETECT_TYPE_PORNWARE    ((tDWORD)(5)) //
#define ENGINE_DETECT_TYPE_RISKWARE    ((tDWORD)(6)) //
#define ENGINE_DETECT_TYPE_XFILES      ((tDWORD)(20)) //
#define ENGINE_DETECT_TYPE_SOFTWARE    ((tDWORD)(21)) //
#define ENGINE_DETECT_TYPE_UNDETECT    ((tDWORD)(30)) //

// ENGINE_DETECT_DISINFECTABILITY
// Константы для ENGINE_DETECT_DISINFECTABILITY.//eng:Enum constants for ENGINE_DETECT_DISINFECTABILITY.
// Возможность удаления.//eng:Disinfectability.
#define ENGINE_DETECT_DISINFECTABILITY_UNKNOWN ((tDWORD)(0)) // Возможность удаления кода не определена.//eng:Disinfectability unknown.
#define ENGINE_DETECT_DISINFECTABILITY_NO ((tDWORD)(1)) // Удаление кода невозможно.//eng:Non disinfectable.
#define ENGINE_DETECT_DISINFECTABILITY_YES ((tDWORD)(2)) // Удаление кода возможно.//eng:Disinfectable.
#define ENGINE_DETECT_DISINFECTABILITY_YES_BY_DELETE ((tDWORD)(3)) // Удаление кода возможно вместе с объектом//eng:Disinfect by delete.
#define ENGINE_DETECT_DISINFECTABILITY_CLEAN ((tDWORD)(4)) // Не вирус. Удаление кода не имеет смысла.

// ENGINE_DETECT_CERTANITY
// Константы для ENGINE_DETECT_CERTANITY.//eng:Enum constants for ENGINE_DETECT_CERTANITY.
// Уровень уверенности детектирования.//eng:Detection certanity.
#define ENGINE_DETECT_CERTANITY_SURE   ((tDWORD)(1)) // Точное совпадение кода.
#define ENGINE_DETECT_CERTANITY_PARTIAL ((tDWORD)(2)) // Частичное совпадение кода.
#define ENGINE_DETECT_CERTANITY_HEURISTIC ((tDWORD)(3)) // Обнаружение эвристическим методом.
#define ENGINE_DETECT_CERTANITY_PROBABLY ((tDWORD)(4)) // Возможное совпадение кода.

// ENGINE_DETECT_DANGER
// Константы для ENGINE_DETECT_DANGER.//eng:Enum constants for ENGINE_DETECT_DANGER.
// Флаги опасности.//eng:Danger flags.
#define ENGINE_DETECT_DANGER_HIGH      ((tDWORD)(0)) // Обнаружен вредоносный код.
#define ENGINE_DETECT_DANGER_MEDIUM    ((tDWORD)(1)) // Обнаружен код, открывающий несанкционированный доступ.
#define ENGINE_DETECT_DANGER_LOW       ((tDWORD)(2)) // Обнаружен код легальной программы, могущей анноить пользователя
#define ENGINE_DETECT_DANGER_INFORMATIONAL ((tDWORD)(3)) //

//  Bitfield constants for ENGINE_DB_STATE
// Константы для свойства DB_STATE.
// -
#define cENGINE_DB_STATE_FUNCTIONAL    ((tDWORD)(0x1)) // Движек готов к работе.

// Types of ENGINE Actions
// Константы типов действий
// Типы действий для сообщений Action-процессора. pmc_ENGINE_ACTION
#define ENGINE_ACTION_TYPE_NO_ACTION   ((tDWORD)(0)) // Вспомогательный тип
#define ENGINE_ACTION_TYPE_SET_OF_ACTIONS ((tDWORD)(1)) // Выполнить набор действий
#define ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO ((tDWORD)(2)) // На текущем объекте пытаться найти OS
#define ENGINE_ACTION_TYPE_CONVERT_IO  ((tDWORD)(3)) // На текущем объекте попытаться создать преобразованный IO
#define ENGINE_ACTION_TYPE_SWITCH_BY_PROPERTY ((tDWORD)(4)) // Перейти к действию соответствующему значению указанного свойства.
#define ENGINE_ACTION_TYPE_SUBENGINE_PROCESS ((tDWORD)(5)) // Отдать объект на обработку сканирующему поддвижку
#define ENGINE_ACTION_TYPE_DISINFECT   ((tDWORD)(6)) // Попытаться вылечить вирус
#define ENGINE_ACTION_TYPE_SECTOR_OVERWRITE ((tDWORD)(7)) // Перезаписать сектор образом из бэкапа или стандарным.

// Predefined Classes of ENGINE Actions
// Константы классов действий
// Класс используется для группировки разных действий и управления исполнением.
#define ENGINE_ACTION_CLASS_VIRUS_SCAN ((tDWORD)(0x00010000)) // Поиск вирусов
#define ENGINE_ACTION_CLASS_VIRUS_HEURISTIC ((tDWORD)(0x00020000)) // Поиск вирусов эвристическим методом
#define ENGINE_ACTION_CLASS_ARCHIVE    ((tDWORD)(0x00000001)) // Обработка архива
#define ENGINE_ACTION_CLASS_MAIL_DATABASE ((tDWORD)(0x00000002)) // Обработка почтовой базы.
#define ENGINE_ACTION_CLASS_MAIL_PLAIN ((tDWORD)(0x00000004)) // Обработка почтового формата
#define ENGINE_ACTION_CLASS_ARCHIVE_SFX ((tDWORD)(0x00000008)) // Обработка самораспаковывающегося архива
#define ENGINE_ACTION_CLASS_OLE_STREAM ((tDWORD)(0x00000010)) // Обработка OLE потоков
#define ENGINE_ACTION_CLASS_OLE_EMBEDDED ((tDWORD)(0x00000020)) // обработка OLE вложений
#define ENGINE_ACTION_CLASS_MODULES_IN_PROCESS ((tDWORD)(0x00000200)) // обработка образов модулей в памяти процессов
#define ENGINE_ACTION_CLASS_PACKED     ((tDWORD)(0x00001000)) // обработка упакованных объектов
#define ENGINE_ACTION_CLASS_OBJECT_MODIFY ((tDWORD)(0x10000000)) // обработка, подразумевающая модификацию объекта
#define ENGINE_ACTION_CLASS_ALL_FILES  ((tDWORD)(0x08000000)) // Виртуальный класс - для использования маски в IChecker. Должен быть сброшен если используется IsProgram() при енумерации.
#define ENGINE_ACTION_CLASS_SMART_MASK  ((tDWORD)(0x04000000)) // Виртуальный класс - для использования маски в IChecker. Должен быть сброшен если используется IsProgram() при енумерации.

// Flags of ENGINE Actions
// Flags of ENGINE Actions
// Определяющие специфику обработки действий
#define ENGINE_ACTION_FLAG_NOT_REQUIRED ((tDWORD)(0x00000001)) // Действие не обязательно должно быть исполнено
#define ENGINE_ACTION_FLAG_ERROR_NOT_CRITICAL ((tDWORD)(0x00000002)) // Ошибка выполнения действия не является критичной
#define ENGINE_CONFIGURATION_REQUIRED_VERSION ((tDWORD)(3)) //  --
// AVP Prague stamp end



#define CHECK_ACTION_CLASS_EXCLUSIVE(cls,mask)  ( (cls & (mask)) && !(cls & ~(mask)) )


// AVP Prague stamp begin( Interface defined messages,  )

// message class
//    msg_id == тип Действия
//    send-point == обрабатываемый объект. (зависит от типа)
//    ctx == hENGINE
//    В буффере - tDWORD Класс действия.
//
//    Посылается только в том случае, если подходит по маске класса. См. pm_ENGINE_IO_OPTIONS_ACTION_CLASS_MASK.
//
//    Если возаратить errENGINE_ACTION_SKIP - действие будет пропущено.
#define pmc_ENGINE_ACTION 0xffe6a144 // Сообщения Action- процессора.

	#define pm_ENGINE_ACTION_NO_ACTION 0x00000000 // (0) -- Пустое действие - для фильтрации по классу.
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class
	//

	#define pm_ENGINE_ACTION_SET_OF_ACTIONS 0x00000001 // (1) -- Выполнить набор действий
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class

	#define pm_ENGINE_ACTION_PROCESS_OS_ON_IO 0x00000002 // (2) -- На текущем объекте пытаться найти OS
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class

	#define pm_ENGINE_ACTION_CONVERT_IO 0x00000003 // (3) -- На текущем объекте попытаться создать преобразованный IO
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class

	#define pm_ENGINE_ACTION_SWITCH_BY_PROPERTY 0x00000004 // (4) -- Перейти к действию соответствующему значению указанного свойства.
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class

	#define pm_ENGINE_ACTION_SUBENGINE_PROCESS 0x00000005 // (5) -- Отдать объект на обработку сканирующему поддвижку
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class

	#define pm_ENGINE_ACTION_DISINFECT 0x00000006 // (6) -- Попытаться вылечить вирус
	// context comment
	//    hENGINE
	//
	// data content comment
	//    Action class

// message class
//    Испрользуется во время инициализации.
//    В параметре ctx - hENGINE.
#define pmc_ENGINE_OPTIONS 0x84538783 // Запрос основных настроек.//eng:General engine options request.

	#define pm_ENGINE_OPTIONS_USE_MULTITHREAD_DRIVER 0x00001000 // (4096) -- Запрос режима использования многопоточного драйвера
	// context comment
	//    hENGINE
	// data content comment
	//    tBOOL

	#define pm_ENGINE_OPTIONS_SEND_LOAD_RECORD_MESSAGE 0x00001001 // (4097) -- Запрос режима посылки соощений о каждой загружаемой рекорде
	// context comment
	//    hENGINE
	// data content comment
	//    tBOOL

	#define pm_ENGINE_OPTIONS_SEND_MT_DEBUG_MESSAGE 0x00001002 // (4098) -- Запрос на посылку сообщений pm_ENGINE_IO_OUTPUT_MT_DEBUG
	// context comment
	//    hENGINE
	// data content comment
	//    tBOOL

	#define pm_ENGINE_OPTIONS_USE_SIGN_CHECK 0x00001003 // (4099) -- Запрос режима проверки подписи
	// context comment
	//    hENGINE
	// data content comment
	//    tBOOL
	//

// message class
//    Usually hIO in obj parameter.
#define pmc_ENGINE_IO 0x688c733f // Main notification class for processing.

	#define pm_ENGINE_IO_BEGIN 0x00001001 // (4097) -- Начало фрейма обработки io.
	// context comment
	//    engine
	// data content comment
	//    -
	//

	#define pm_ENGINE_IO_DONE 0x00001002 // (4098) -- Конец фрейма обработки io.
	// context comment
	//    engine
	// data content comment
	//    В буффере - tERROR после обработки.
	//

	#define pm_ENGINE_IO_OS_ENTER 0x00001003 // (4099) -- Enter OS for object enumeration and processing (archives)
	// context comment
	//    engine
	// data content comment
	//    -
	//

	#define pm_ENGINE_IO_OS_LEAVE 0x00001004 // (4100) -- Конец фрейма обработки os.
	// context comment
	//    engine
	//
	// data content comment
	//    В буффере - tERROR после обработки.
	//

	#define pm_ENGINE_IO_SUB_IO_ENTER 0x00001005 // (4101) -- Enter Sub IO processing (packers)
	// context comment
	//    engine
	// data content comment
	//    -

	#define pm_ENGINE_IO_SUB_IO_LEAVE 0x00001006 // (4102) -- Exit Sub IO processing (packers)
	// context comment
	//    engine
	// data content comment
	//    Конец фрейма обработки производного io.

	#define pm_ENGINE_IO_DETECT 0x00001007 // (4103) -- Произошло обнаружение.
	// context comment
	//    hENGINE- движек, который произвел обнаружение
	// data content comment
	//    not used

	#define pm_ENGINE_IO_DISINFECTED 0x00001008 // (4104) -- Disinfection. Details in npENGINE_... properties.
	// context comment
	//    hENGINE- движек, который произвел обнаружение
	// data content comment
	//    not used

	#define pm_ENGINE_IO_DISINFECTION_FAILED 0x00001009 // (4105) -- Disinfection. Details in npENGINE_... properties.
	// context comment
	//    hENGINE- движек, который произвел обнаружение
	// data content comment
	//    not used

	#define pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE 0x0000100a // (4106) -- Disinfection impossibility. Details in npENGINE_... properties.
	// context comment
	//    hENGINE- движек, который произвел обнаружение
	// data content comment
	//    not used

	#define pm_ENGINE_IO_CORRUPTED 0x0000100b // (4107) -- //rus:Нарушена внутренняя структура обрабатываемого объекта.//eng:Inernal structure inconsistance
	// context comment
	//    hENGINE
	// data content comment
	//    not used
	//

	#define pm_ENGINE_IO_ENCRYPTED 0x00001012 // (4114) -- объект зашифрован
	// context comment
	//    hENGINE
	// data content comment
	//    not used

	#define pm_ENGINE_IO_PASSWORD_PROTECTED 0x00001015 // (4117) -- доступ к содержимому объекта защищен паролем
	// context comment
	//    hENGINE
	// data content comment
	//    not used

	#define pm_ENGINE_IO_PROCESSING_ERROR 0x0000100c // (4108) -- критическая ошибка при обработке объекта
	// context comment
	//    hENGINE
	// data content comment
	//    tERROR - ошибка обработки

	#define pm_ENGINE_IO_WAS_MODIFIED 0x0000100d // (4109) -- Объект IO изменен//eng:IO was mofified.
	// context comment
	//    hENGINE
	// data content comment
	//    not used

	#define pm_ENGINE_IO_MAKE_VIRTUAL_NAME 0x0000100e // (4110) -- Запрос на создание виртуального имени.//eng:Request for object virtual name creation
	// context comment
	//    hENGINE
	//
	// data content comment
	//    ANSI string - forced name

	#define pm_ENGINE_IO_NAME_CHECK 0x0000100f // (4111) -- Check name for processing - should it be extracted
	// context comment
	//    hENGINE
	// data content comment
	//    not used

	#define pm_ENGINE_IO_GET_ORIGINAL_BOOT 0x00001010 // (4112) -- Fill buffer with boot sector image
	// context comment
	//    hENGINE
	// data content comment
	//    указатель на переменную, которая принимает указатель на буфффер с данными.

	#define pm_ENGINE_IO_GET_ORIGINAL_MBR 0x00001011 // (4113) -- Fill buffer with MBR sector image
	// context comment
	//    hENGINE
	// data content comment
	//    указатель на переменную, которая принимает указатель на буфффер с данными.

	#define pm_ENGINE_IO_OUTPUT_STRING 0x00001013 // (4115) -- Output undocumented messages from engine
	// context comment
	//    hENGINE
	// data content comment
	//    char* string

	#define pm_ENGINE_IO_OUTPUT_MT_DEBUG 0x00001014 // (4116) -- Debug message for multithread mode monitoring
	// context comment
	//    hENGINE
	// data content comment
	//    4 DWORDS.

	#define pm_ENGINE_IO_HASH_FRAME_BEGIN 0x00001016 // (4118) -- Начало фрейма обработки, попадающего под запоминание в HASH контейнере.
	// context comment
	//    hENGINE
	// data content comment
	//    not used

	#define pm_ENGINE_IO_HASH_FRAME_LEAVE 0x00001017 // (4119) -- Конец фрейма обработки, попадающего под запоминание в HASH контейнере.
	// context comment
	//    hENGINE
	// data content comment
	//    указатель на переменную, принимающую указатель на пользовательские данные. Указатель будет сохранен в Hash контейнере.

	#define pm_ENGINE_IO_HASH_FRAME_FOUND 0x00001018 // (4120) -- Объект найден в HASH контейнере. Следующий Фрейм обработки будет пропущен.
	// context comment
	//    hENGINE
	// data content comment
	//    указатель на переменную, указывающую на пользовательские данные. берется из Hash контейнера.

	#define pm_ENGINE_IO_CHANGE_ACCESS_MODE 0x00001019 // (4121) -- запрос на изменение режима открытия объекта
	// context comment
	//    hOBJECT
	// data content comment
	//    Dword access mode - желаемый режим
	//

	#define pm_ENGINE_IO_UNSUPPORTED_COMPRESSION_METHOD 0x0000101a // (4122) -- неизвестный/неподдерживаемый метод сжатия
	// context comment
	//    hENGINE
	// data content comment
	//    not used

	#define pm_ENGINE_IO_OS_CHECK 0x0000101b // (4123) -- Запрос на подтверждение обработки OS.
	// context comment
	//    hOS
	// data content comment
	//    not used

	#define pm_ENGINE_IO_ICHECKER_OK 0x0000101c // (4124) --

	#define pm_ENGINE_IO_OS_PRECLOSE 0x0000101d // (4125) -- Завершение обработки OS.
	// context comment
	//    hOS
	// data content comment
	//    not used

	#define pm_ENGINE_IO_HC_CLEAN 0x0000101e // (4126) --

	#define pm_ENGINE_IO_HC_DETECT 0x0000101f // (4127) --

	#define pm_ENGINE_IO_HC_OVERWRITED 0x00001020 // (4128) --

	#define pm_ENGINE_IO_HC_OVERWRITE_FAILED 0x00001021 // (4129) --

	#define pm_ENGINE_IO_HC_MARKED_FOR_DELETE 0x00001022 // (4130) --

	#define pm_ENGINE_IO_OS_CLOSE_ERROR 0x00001023 // (4131) --
	// data content comment
	//    error code

	#define pm_ENGINE_IO_OS_RECOGNIZE 0x00001024 // (4132) --
	// data content comment
	//
	//

	#define pm_ENGINE_IO_GET_EMUL_FLAGS 0x00001025

	#define pm_ENGINE_IO_FALSE_ALARM 0x00001026



// message class
//    Сообщения этого класса посылаются во время инициализации ENGINE и вызова метода LoadConfiguration.
#define pmc_ENGINE_LOAD 0x7f067b36 // Класс сообщений о ходе загрузки ENGINE и конфигурации

	#define pm_ENGINE_LOAD_BASE 0x00001000 // (4096) -- Нотификация о загрузке очередного файла базы.
	// context comment
	//    hSTRING имени базы
	// data content comment
	//    DATETIME текущей базы

	#define pm_ENGINE_LOAD_RECORD 0x00001001 // (4097) -- Нотификация о загрузке очередной записи.
	// context comment
	//    hSTRING имени рекорды
	// data content comment
	//    not used

	#define pm_ENGINE_LOAD_BASE_INTEGRITY_FAILED 0x00001002 // (4098) -- Верификация подписи модуля/базы не прошла
	// context comment
	//    hSTRING - имя базы/модуля
	// data content comment
	//    not used

	#define pm_ENGINE_LOAD_BASE_ERROR_OPEN 0x00001003 // (4099) -- Невозможно открыть базу/модуль
	// context comment
	//    hSTRING - имя базы/модуля
	// data content comment
	//    not used

	#define pm_ENGINE_LOAD_OUTPUT_STRING 0x00001004 // (4100) -- Выводит строку во время загрузки баз
	// context comment
	//    hSTRING
	// data content comment
	//    not used

	#define pm_ENGINE_LOAD_CONFIGURATION_OUT_OF_DATE 0x00001005 // (4101) --

	#define pm_ENGINE_LOAD_CONFIGURATION_LOADED 0x00001006 // (4102) --

	#define pm_ENGINE_LOAD_CONFIGURATION_UNLOADED 0x00001007 // (4103) --

	#define pm_ENGINE_LOAD_CONFIGURATION_ERROR 0x00001008 // (4104) --

// message class
#define pmc_ENGINE_PROCESS 0xd27b6592 // Сообщения о прохождении функций Process() и ICheck()

	#define pm_ENGINE_PROCESS_BEGIN 0x00001000 // (4096) -- Начало обработки объекта
	// context comment
	//    hEngine

	#define pm_ENGINE_PROCESS_DONE 0x00001001 // (4097) -- Конец обработки объекта
	// context comment
	//    hEngine
	// data content comment
	//    tERROR
// AVP Prague stamp end

#define pmc_ENGINE_DETECT 0xd27b6593 // Сообщения о прохождении процедуры обнаружения
	
	#define pm_ENGINE_DETECT_WA 0x00001000 // (4096) -- Сообщение в момент срабатывания рекорды (cWorkArea в параметре)


#define pm_ENGINE_LOAD_BASE_UNRESOLVED_SYMBOL 0x00001100 
#define pm_ENGINE_LOAD_BASE_DUPLICATE_SYMBOL  0x00001101 

// AVP Prague stamp begin( Interface defined errors,  )
#define errENGINE_BAD_CONFIGURATION              PR_MAKE_DECL_ERR(IID_ENGINE, 0x001) // 0x8f235001,-1893511167 (1) -- Неправильная конфигурация.
#define errENGINE_OS_STOPPED                     PR_MAKE_DECL_ERR(IID_ENGINE, 0x002) // 0x8f235002,-1893511166 (2) -- Обработка OS остановлена.
#define errENGINE_IO_MODIFIED                    PR_MAKE_DECL_ERR(IID_ENGINE, 0x003) // 0x8f235003,-1893511165 (3) -- Обрабатываемый объект был изменен.
#define errENGINE_ACTION_SKIP                    PR_MAKE_DECL_ERR(IID_ENGINE, 0x004) // 0x8f235004,-1893511164 (4) -- Управляющий код.
#define errENGINE_CONFIGURATION_NOT_LOADED       PR_MAKE_DECL_ERR(IID_ENGINE, 0x005) // 0x8f235005,-1893511163 (5) --
#define errENGINE_BAD_ACTION_CONFIGURATION       PR_MAKE_DECL_ERR(IID_ENGINE, 0x006) // 0x8f235006,-1893511162 (6) --
#define errENGINE_BAD_REGISTRY_CONFIGURATION     PR_MAKE_DECL_ERR(IID_ENGINE, 0x007) // 0x8f235007,-1893511161 (7) --
#define errENGINE_BAD_AVP_SET                    PR_MAKE_DECL_ERR(IID_ENGINE, 0x008) // 0x8f235008,-1893511160 (8) --
#define errENGINE_MT_DRIVER_NOT_LOADED           PR_MAKE_DECL_ERR(IID_ENGINE, 0x009) // 0x8f235009,-1893511159 (9) --
#define errENGINE_BAD_AVP1_CONFIGURATION         PR_MAKE_DECL_ERR(IID_ENGINE, 0x00a) // 0x8f23500a,-1893511158 (10) --
#define errENGINE_OS_VOLUME_SET_INCOMPLETE       PR_MAKE_DECL_ERR(IID_ENGINE, 0x00b) // 0x8f23500b,-1893511157 (11) --
#define errENGINE_HC_NO_OVERWRITE                PR_MAKE_DECL_ERR(IID_ENGINE, 0x00d) // 0x8f23500d,-1893511155 (13) --
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hENGINE;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iEngineVtbl;
typedef struct iEngineVtbl iEngineVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cEngine;
	typedef cEngine* hENGINE;
#else
	typedef struct tag_hENGINE {
		const iEngineVtbl* vtbl; // pointer to the "Engine" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hENGINE;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Engine_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpEngine_Process)           ( hENGINE _this, hOBJECT obj );   // -- Обработать объект.//eng:Process object.;
	typedef   tERROR (pr_call *fnpEngine_GetAssociatedInfo) ( hENGINE _this, hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ); // -- Получить ассоциированные данные.//eng:Get associated data.;
	typedef   tERROR (pr_call *fnpEngine_GetExtendedInfo)   ( hENGINE _this, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ); // -- Получить дополнительные свойства.//eng:Get extended information.;
	typedef   tERROR (pr_call *fnpEngine_LoadConfiguration) ( hENGINE _this, hREGISTRY reg, tRegKey key ); // -- Загрузить конфигурацию.;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iEngineVtbl {
	fnpEngine_Process            Process;
	fnpEngine_GetAssociatedInfo  GetAssociatedInfo;
	fnpEngine_GetExtendedInfo    GetExtendedInfo;
	fnpEngine_LoadConfiguration  LoadConfiguration;
}; // "Engine" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Engine_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION      mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgENGINE_VERSION         mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00001001 )
#define pgENGINE_DB_NAME         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001002 )
#define pgENGINE_DB_COUNT        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001003 )
#define pgENGINE_DB_TIME         mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001004 )
#define pgENGINE_DB_STATE        mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001005 )
#define pgENGINE_DB_LOAD_NAME    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001007 )
#define pgENGINE_DB_LOAD_TIME    mPROPERTY_MAKE_GLOBAL( pTYPE_DATETIME, 0x00001008 )
#define pgENGINE_SUBENGINE_COUNT mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001009 )
#define pgENGINE_SUBENGINE_TABLE mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x0000100a )
#define pgENGINE_DB_VERDICTS     mPROPERTY_MAKE_GLOBAL( pTYPE_BYTE    , 0x0000100b )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Engine_Process( _this, obj )                                                              ((_this)->vtbl->Process( (_this), obj ))
	#define CALL_Engine_GetAssociatedInfo( _this, obj, info_id, param, buffer, size, out_size )            ((_this)->vtbl->GetAssociatedInfo( (_this), obj, info_id, param, buffer, size, out_size ))
	#define CALL_Engine_GetExtendedInfo( _this, info_id, param, buffer, size, out_size )                   ((_this)->vtbl->GetExtendedInfo( (_this), info_id, param, buffer, size, out_size ))
	#define CALL_Engine_LoadConfiguration( _this, reg, key )                                               ((_this)->vtbl->LoadConfiguration( (_this), reg, key ))
#else // if !defined( __cplusplus )
	#define CALL_Engine_Process( _this, obj )                                                              ((_this)->Process( obj ))
	#define CALL_Engine_GetAssociatedInfo( _this, obj, info_id, param, buffer, size, out_size )            ((_this)->GetAssociatedInfo( obj, info_id, param, buffer, size, out_size ))
	#define CALL_Engine_GetExtendedInfo( _this, info_id, param, buffer, size, out_size )                   ((_this)->GetExtendedInfo( info_id, param, buffer, size, out_size ))
	#define CALL_Engine_LoadConfiguration( _this, reg, key )                                               ((_this)->LoadConfiguration( reg, key ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iEngine {
		virtual tERROR pr_call Process( hOBJECT obj ) = 0; // -- Обработать объект.//eng:Process object.
		virtual tERROR pr_call GetAssociatedInfo( hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ) = 0; // -- Получить ассоциированные данные.//eng:Get associated data.
		virtual tERROR pr_call GetExtendedInfo( tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ) = 0; // -- Получить дополнительные свойства.//eng:Get extended information.
		virtual tERROR pr_call LoadConfiguration( hREGISTRY reg, tRegKey key ) = 0; // -- Загрузить конфигурацию.
	};

	struct pr_abstract cEngine : public iEngine, public iObj {

		OBJ_IMPL( cEngine );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hENGINE()   { return (hENGINE)this; }

		tVERSION pr_call get_pgENGINE_VERSION() { return (tVERSION)getDWord(pgENGINE_VERSION); }
		tERROR pr_call set_pgENGINE_VERSION( tVERSION value ) { return setDWord(pgENGINE_VERSION,(tDWORD)value); }

		tERROR pr_call get_pgENGINE_DB_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgENGINE_DB_NAME,buff,size,cp); }
		tERROR pr_call set_pgENGINE_DB_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgENGINE_DB_NAME,buff,size,cp); }

		tDWORD pr_call get_pgENGINE_DB_COUNT() { return (tDWORD)getDWord(pgENGINE_DB_COUNT); }
		tERROR pr_call set_pgENGINE_DB_COUNT( tDWORD value ) { return setDWord(pgENGINE_DB_COUNT,(tDWORD)value); }

		tERROR pr_call get_pgENGINE_DB_TIME( tDATETIME* value ) { return get(pgENGINE_DB_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgENGINE_DB_TIME( tDATETIME* value ) { return set(pgENGINE_DB_TIME,value,sizeof(tDATETIME)); }

		tDWORD pr_call get_pgENGINE_DB_STATE() { return (tDWORD)getDWord(pgENGINE_DB_STATE); }
		tERROR pr_call set_pgENGINE_DB_STATE( tDWORD value ) { return setDWord(pgENGINE_DB_STATE,(tDWORD)value); }

		tERROR pr_call get_pgENGINE_DB_LOAD_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgENGINE_DB_LOAD_NAME,buff,size,cp); }
		tERROR pr_call set_pgENGINE_DB_LOAD_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgENGINE_DB_LOAD_NAME,buff,size,cp); }

		tERROR pr_call get_pgENGINE_DB_LOAD_TIME( tDATETIME* value ) { return get(pgENGINE_DB_LOAD_TIME,value,sizeof(tDATETIME)); }
		tERROR pr_call set_pgENGINE_DB_LOAD_TIME( tDATETIME* value ) { return set(pgENGINE_DB_LOAD_TIME,value,sizeof(tDATETIME)); }

		tDWORD pr_call get_pgENGINE_SUBENGINE_COUNT() { return (tDWORD)getDWord(pgENGINE_SUBENGINE_COUNT); }
		tERROR pr_call set_pgENGINE_SUBENGINE_COUNT( tDWORD value ) { return setDWord(pgENGINE_SUBENGINE_COUNT,(tDWORD)value); }

		tPTR pr_call get_pgENGINE_SUBENGINE_TABLE() { return (tPTR)getPtr(pgENGINE_SUBENGINE_TABLE); }
		tERROR pr_call set_pgENGINE_SUBENGINE_TABLE( tPTR value ) { return setPtr(pgENGINE_SUBENGINE_TABLE,(tPTR)value); }

		tBYTE pr_call get_pgENGINE_DB_VERDICTS() { return (tBYTE)getByte(pgENGINE_DB_VERDICTS); }
		tERROR pr_call set_pgENGINE_DB_VERDICTS( tBYTE value ) { return setByte(pgENGINE_DB_VERDICTS,(tBYTE)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_engine__3586a210_b2c2_4130_b9f4_59d900730341
// AVP Prague stamp end



