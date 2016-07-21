/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Object IDentifications              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#ifndef __PR_OID_H
#define __PR_OID_H

// Object origin type constants 
// pgOBJECT_ORIGIN property

#define OID_ANY                        ((tORIG_ID)(-1))
#define OID_GENERIC_IO                 ((tORIG_ID)(0))
#define OID_PASSWORDED_EXE             ((tORIG_ID)(0x1000))         // fake тело, обнаруживаемое как Susupicion
#define OID_COMMAND_RELOAD_BASE        ((tORIG_ID)(0x2000))
#define OID_LOGICAL_DRIVE              ((tORIG_ID)(0x2001))			// логический диск (removable, fixed...). создаются в WindDiskIO.ppl. Интерфейсы IO и ObjPtr. объекты могут быть заражены, вылечены
#define OID_PHYSICAL_DISK              ((tORIG_ID)(0x2002))			// физический диск (removable, fixed...). создаются в WindDiskIO.ppl. Интерфейсы IO и ObjPtr. объекты могут быть заражены, вылечены
#define OID_AVP3_DOS_MEMORY            ((tORIG_ID)(0x3001))
#define OID_AVP3_BOOT                  ((tORIG_ID)(0x3002))
#define OID_AVP3_MBR                   ((tORIG_ID)(0x3003))
#define OID_MEMORY_PROCESS             ((tORIG_ID)(0x5001))			// память процессов. создаются в MemScan.ppl. Интерфейсы OS, ObjPtr, IO. могут быть заражены, вылечены, удалены
#define OID_MEMORY_PROCESS_MODULE      ((tORIG_ID)(0x5002))			// модули процессов в памяти. создаются в MemModSc.ppl. Интерфейсы OS, ObjPtr, IO. могут быть заражены, вылечены, удалены
#define OID_SCRIPT_TEXT                ((tORIG_ID)(0x5004))			// текст скрипта (например, вынутый из HTML)
#define OID_SCRIPT_VBS                 ((tORIG_ID)(0x5005))			// бинарное представление скрипта после обработчика языка VBScript
#define OID_SCRIPT_VBSE                ((tORIG_ID)(0x5006))			// расширенное бинарное представление скрипта после обработчика языка VBScript 
#define OID_SCRIPT_VBSH                ((tORIG_ID)(0x5007))			// расширенное представление скрипта после обработчика языка VBScript + эмуляция
#define OID_SCRIPT_JS                  ((tORIG_ID)(0x5008))			// бинарное представление скрипта после обработчика языка JavaScript
#define OID_SCRIPT_PACKED_TEXT         ((tORIG_ID)(0x5009))			// подготовленный для сканирования текст (lowercase, crlf -> lf, и т.п.)
#define OID_SCRIPT_BAT                 ((tORIG_ID)(0x500A))			// подготовленный для сканирования текст BAT-файла (убраны имена переменных, а также некоторые редиректоры)
#define OID_SCRIPT_MIRC                ((tORIG_ID)(0x500B))			// подготовленный для сканирования тест MIRC/PIRCH скрипта (убрано все относящееся к INI-файлу, вложенные команды собраны в длинные строки)
#define OID_MAIL_MSG_REF               ((tORIG_ID)(0x6000))			// почтовое сообщение, содержащееся в базе данных писем Outlook(pst-файл). Необходим для создания на его основе объектной системы.
#define OID_MAIL_MSG_MIME              ((tORIG_ID)(0x6001))			// почтовое сообщение в MIME формате.
#define OID_MAIL_MSG_BODY              ((tORIG_ID)(0x6002))			// любое тело(plain-body, rich-body, html-body) почтового сообщения (письма).
#define OID_MAIL_MSG_ATTACH            ((tORIG_ID)(0x6003))			// любое вложение(attachment) почтового сообщения (письма).
#define OID_MIRA_GENERIC               ((tORIG_ID)(0x7000))			// OID_MIRA_EXE_xxx набор - ориджинов для реализации распознавания образов в бинарных потоках (a-la MIRA) 
#define OID_MIRA_EXE                   ((tORIG_ID)(0x7001))			// неопознаный бинарный поток
#define OID_MIRA_EXE_VB_NATIVE         ((tORIG_ID)(0x7002))			// подготовленный данные из программы скомпилированной Visual Basic 5,6 Native Code
#define OID_MIRA_EXE_VB_PCODE          ((tORIG_ID)(0x7003))			// подготовленный данные из программы скомпилированной Visual Basic 5,6 PCode
#define OID_MIRA_EXE_DELPHI            ((tORIG_ID)(0x7004))			// подготовленные данные из программы скомпилированной Borland Delphi
#define OID_MIRA_EXE_BORLANDC_1994     ((tORIG_ID)(0x7005))			// подготовленные данные из программы скомпилированной Borland C++
#define OID_MIRA_EXE_MSVC              ((tORIG_ID)(0x7006))			// подготовленные данные из программы скомпилированной Microsoft Visual C
#define OID_REOPEN_DATA                ((tORIG_ID)(0x8000))         // данные для прямого открытия объекта
#define OID_QUARANTINED_OBJECT         ((tORIG_ID)(0x9000))         // объект в карантине

#endif
