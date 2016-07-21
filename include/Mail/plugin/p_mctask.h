// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  18 November 2004,  17:45 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_mctask.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_mctask )
#define __public_plugin_mctask
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Mail/iface/i_mailtask.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_MCTASK  ((tPID)(40004))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_DENISOV  ((tVID)(88))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, MailTask )
// MailTask interface implementation
// Short comments --

// constants
#define npMAILER_PID                   ((tSTRING)("MAILER_PID")) // PID локального процесса, инициализировавшего соединение
#define npMESSAGE_IS_INCOMING          ((tSTRING)("MESSAGE_IS_INCOMING")) // Флаг, выставляемый перехватчиком: данное письмо является входящим
#define npMESSAGE_CHECK_ONLY           ((tSTRING)("MESSAGE_CHECK_ONLY"))  // Флаг, выставляемый перехватчиком: данное письмо не надо лечить
#define npMAILMESSAGE_OS_PID           ((tSTRING)("MAILMESSAGE_OS_PID"))  // PID плагина, ответственного за парсинг процессируемого IO
#define npCONTENT_TYPE                 ((tSTRING)("CONTENT_TYPE")) // 

// message class
//! #define pmc_TASK_STATE_CHANGED 0xe135ba3e //

// message class
//! #define pmc_TASK_REPORT 0xc1c1bc1b //

// message class
//! #define pmc_MAILTASK_PROCESS 0x0fe9fb5a //

// message class
//! #define pmc_MAILCHECKER_PROCESS 0xd875646d //
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_mctask
// AVP Prague stamp end



