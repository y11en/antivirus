// файл содержит определения собственных статусов статусов

#ifndef _KL_STATUS_
#define _KL_STATUS_

typedef unsigned long KLSTATUS;

#define KL_ERROR(facility, code)    (KLSTATUS)( ((ULONG) (facility) << 24) | (code) )

#define KL_FACILITY(status)         (KLSTATUS)( (status) >> 24 )

#define KL_CODE(status)             (KLSTATUS)( (status) & 0x00FFFFFF )

/*
 	
  Значение статуса упаковано в 32-битное число, формат которого указан ниже

   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
  +---------------------------------------------------------------+
  |    Facility   |                      Code                     |
  +---------------------------------------------------------------+

  где
  
    Facility - указывает тип статуса 
        например, либо пропускать дальше, либо отправить обратно юзеру с некоторым статусом

    Code     - значение статуса.
 
*/


/*
 *	----  Facility  ----
 */

#define F_PASS                          (UCHAR)0x0  // Запрос отправляется дальше
#define F_BLOCK                         (UCHAR)0x1  // Запрос возвращается юзеру (необязательно с негативным статусом)
// #define F_PARSER                        (UCHAR)0x2  // генерируется парсером пакетов.

#define KL_PASS( status )   ( KL_FACILITY( status ) == F_PASS )
#define KL_BLOCK( status )  ( KL_FACILITY( status ) == F_BLOCK )

/*
 *	----  Code      ----
 */

#define REASON_NOT_OK                   (ULONG)0xFFFFFF
#define REASON_ALL_OK                   (ULONG)0x000000
#define REASON_BUFFER_TOO_SMALL         (ULONG)0x000001
#define REASON_DELAY_REQUEST            (ULONG)0x000002
#define REASON_MORE_PROCESSING          (ULONG)0x000003
#define REASON_PLUGIN_OT_FOUND          (ULONG)0x000004
#define REASON_INVALID_REQUEST          (ULONG)0x000005
#define REASON_FRAGMENTED_PACKET        (ULONG)0x000005
#define REASON_BANNED                   (ULONG)0x000006
#define REASON_REDIRECT                 (ULONG)0x000007
#define REASON_UNBANNED                 (ULONG)0x000008
#define REASON_ALREADY_OPENED           (ULONG)0x000009
#define REASON_NOT_FOUND                (ULONG)0x00000a
#define REASON_NOT_OPENED               (ULONG)0x00000b
#define REASON_NO_MORE_ENTRIES          (ULONG)0x00000c
#define REASON_UNSUPPORTED              (ULONG)0x00000d


/*
 *	----  Status    ----
 */

#define KL_SUCCESS                      (KLSTATUS)KL_ERROR(F_PASS,   REASON_ALL_OK)
#define KL_BUFFER_TOO_SMALL             (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_BUFFER_TOO_SMALL)
#define KL_PLUGIN_NOT_FOUND             (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_PLUGIN_OT_FOUND)
#define KL_UNSUPPORTED                  (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_UNSUPPORTED)

#define KL_DELAY_REQUEST                (KLSTATUS)KL_ERROR(F_PASS,   REASON_DELAY_REQUEST)
#define KL_COMPLETE_OK                  (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_ALL_OK)
#define KL_DENY_REQUEST                 (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_INVALID_REQUEST)
#define KL_MORE_PROCESSING              (KLSTATUS)KL_ERROR(F_PASS,   REASON_MORE_PROCESSING)
#define KL_DELAY_FRAGMENTED             (KLSTATUS)KL_ERROR(F_PASS,   REASON_FRAGMENTED_PACKET)

#define KL_UNSUCCESSFUL                 (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_NOT_OK)
#define KL_ALREADY_BANNED               (KLSTATUS)KL_ERROR(F_PASS,   REASON_BANNED)

#define KL_DROP_PACKET                  (KLSTATUS)KL_ERROR(F_BLOCK,  REASON_BANNED)
#define KL_ALLOW_PACKET                 (KLSTATUS)KL_ERROR(F_PASS,   REASON_UNBANNED)

#define KL_ALREADYOPENED				(KLSTATUS)KL_ERROR(F_BLOCK,  REASON_ALREADY_OPENED)
#define KL_PATHNOTFOUND					(KLSTATUS)KL_ERROR(F_BLOCK,  REASON_NOT_FOUND)
#define KL_NOTOPENED					(KLSTATUS)KL_ERROR(F_BLOCK,  REASON_NOT_OPENED)
#define KL_NOMOREENTRIES				(KLSTATUS)KL_ERROR(F_BLOCK,  REASON_NO_MORE_ENTRIES)

#endif // _KL_STATUS_