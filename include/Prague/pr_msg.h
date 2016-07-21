/*-----------------05.10.01 ------------------------
 * Project Prague                                 *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#ifndef __PR_MSG_H
#define __PR_MSG_H

// message class PROCESSING - plugins report about control flow.
#define  pmc_PROCESSING                    0xa6f839da
#define  pm_PROCESSING_YIELD               0x62c12165 /*pm_PROCESSING_YIELD*/
// Message pm_PROCESSING_YIELD - Each functional plugin should 
// send this message during time-consuming operations. 
// If this message return PR_FAIL - procesing should be correctly aborted ASAP
// If buffer exist, first tDWORD is timeout in milliseconds.

// message class pmc_DIAGNOSTIC - plugins evaluate current application state.
#define  pmc_DIAGNOSTIC                    0xd4e81a78 
#define  pm_DIAGNOSTIC_IS_SAFE_IP          0x633f8d4c 
// Message pm_DIAGNOSTIC_IS_SAFE_IP - decider returns errOK_DECIDED if code at IP passed in message is safe for rising exception
// Parameters:
// parameter "buffer"  contains pointer to IP address. Caution! Don't forget to suspend the thread.

// message class PRODUCT_ENVIRONMENT controls product and system environment variables
#define pmc_PRODUCT_ENVIRONMENT            0x7cf3d716 // crc32 for "pmc_PRODUCT_ENVIRONMENT"
#define pm_EXPAND_ENVIRONMENT_STRING       0x4eed86ca // crc32 for "pm_EXPAND_ENVIRONMENT_STRING"
#define pm_EXPAND_SYSTEM_ONLY_STRING       0x7a254e21 // crc32 for "pm_EXPAND_SYSTEM_ONLY_STRING"
// Message pm_EXPAND_ENVIRONMENT_STRING - by sending this message plugin request to expand string stored in send_point
// Parameters:
//     send_point      - hSTRING object containing string to expand
//     ctx             - optional hSTRING object receiving result of expension, if NULL value passed result will be stored in send_point

// message class SYSTEM_TIME_CHANGED notifies about system time change
#define pmc_SYSTEM_TIME_CHANGED            0x7cdad711l

// message class SYSTEM_STATE notifies about system state changes (network, etc...)
#define pmc_SYSTEM_STATE                  0x7cdad721l
#define pm_NETWORK_STATE_CHANGED            0x7cdad722l

#define pmc_REMOTE_GLOBAL                  0xffff1000l  // Класс сообщения : маршалиться во все хосты.
#define pmc_REMOTE_PROXY_DISCONECTED	     0xffff1001l  // Класс сообщения : прокси ссылается на несуществующий объект
#define pmc_REMOTE_PROXY_DETACHED		       0xffff1002l  // Класс сообщения : прокси отсоединилась от текущего процесса.
#define pmc_REMOTE_CLOSE_REQUEST           0xffff1003l  // Класс сообщения : запрос на закрытие хоста.

// ---
// for all this messages:
// parameter "msg_id"  is count of messages sent since program start
// parameter "buffer"  is pointer to tLONGLONG which is time in milliseconds elapsed since prague loaded
#define pmc_SYSTEM_IDLE_START              0xc8d4e258l  // sent on idle start
#define pmc_SYSTEM_IDLE_STOP               0x7c54e4e2l  // sent on idle stop
#define pmc_SYSTEM_IDLE_1_SECOND           0xd54c7896l  // sent each 1  second
#define pmc_SYSTEM_IDLE_10_SECOND          0x9f55e14dl  // sent each 10 second
#define pmc_SYSTEM_IDLE_60_SECOND          0x5734c5c2l  // sent each 60 second

#endif//__PR_MSG_H
